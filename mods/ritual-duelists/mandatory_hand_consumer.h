#include "hand_selection_snapshot.h"
#include "mandatory_hand_lifecycle_v1.h"
#include "game/duel_selection_layout.h"
#define RHS_RETRY_LIMIT 3u
static const RhsRules*rd_selection_rules;
static RhsRules rd_selection_rules_copy;
static unsigned rd_selection_serial,rd_selection_generation=1,rd_selection_connection=1;
static unsigned rd_selection_sequence=1,rd_selection_interrupt,rd_selection_attempts;
static unsigned rd_selection_counted_action,rd_selection_counted_generation;
static int rd_selection_effective,rd_selection_reentrant,rd_selection_planning,rd_selection_exhausted;
static void(*rd_selection_old_applied)(int);
static void*rd_selection_old_hand;
static int rd_selection_quarantine,rd_selection_outcome,rd_selection_state,rd_selection_cause,rd_selection_last_cause;
static RhsSnapshot rd_selection_safe;
static int rd_selection_safe_valid,rd_selection_monitor;
static int rd_selection_last_provider_status=RHS_INVALID;
static unsigned char rd_selection_sources[5];
static struct {int state;const RhsRules*producer;RhsRequest request;
 RhsResponse answer;} rd_selection_tx;
static void rd_selection_bump(unsigned*);
static int rd_selection_blocked(void)
{return rd_selection_state==RHS_STATE_BLOCKED||rd_selection_state==RHS_STATE_QUARANTINE||rd_selection_state==RHS_STATE_EXHAUSTED;}
static void rd_selection_mark(int state,int cause)
{
 if(rd_selection_state!=state||rd_selection_cause!=cause){if(rd_selection_sequence!=UINT32_MAX)++rd_selection_sequence;else rd_selection_exhausted=1;}
 rd_selection_state=state;rd_selection_cause=cause;
 if(cause)rd_selection_last_cause=cause;
}
static int rd_selection_cancel_cause(int cause,const char*reason)
{
 int root=cause;
 if(rd_selection_counted_action==rd_selection_serial&&rd_selection_counted_generation==rd_selection_generation&&rd_selection_last_cause)root=rd_selection_last_cause;
 rd_selection_bump(&rd_selection_interrupt);
 if(rd_selection_counted_action!=rd_selection_serial||rd_selection_counted_generation!=rd_selection_generation){
  rd_selection_counted_action=rd_selection_serial;rd_selection_counted_generation=rd_selection_generation;
  if(rd_selection_attempts<RHS_RETRY_LIMIT)++rd_selection_attempts;
 }
 rd_selection_tx.state=-1;deck_pull.active=0;rd_selection_outcome=-1;
 memset((void*)D_800EAE88,0,12);
 if(rd_selection_exhausted)rd_selection_mark(RHS_STATE_EXHAUSTED,RHS_CAUSE_COUNTER);
 else if(!rd_selection_blocked())rd_selection_mark(rd_selection_attempts>=RHS_RETRY_LIMIT?RHS_STATE_BLOCKED:RHS_STATE_RETRY,
  rd_selection_attempts>=RHS_RETRY_LIMIT?RHS_CAUSE_LIMIT:cause);
 rd_selection_last_cause=root;
 if(host&&host->log)host->log(host,"HAL25: hand action withheld cause=%d (%s); attempts=%u/%u; no fallback",cause,reason,rd_selection_attempts,RHS_RETRY_LIMIT);
 return -1;
}
static int rd_selection_cancel(const char*reason){return rd_selection_cancel_cause(RHS_CAUSE_VETO,reason);}
static void rd_selection_bump(unsigned*p){if(*p==UINT32_MAX)rd_selection_exhausted=1;else ++*p;}
static int rd_selection_connect(const RhsRules*p)
{
 if(rd_selection_reentrant||(rd_selection_planning&&rd_selection_tx.state>0))return 0;
 if(!p||p->abi!=RHS_ABI||p->size!=sizeof*p||!p->evaluate)return 0;
 if(rd_selection_rules&&rd_selection_rules!=p)return 0;
 if(rd_selection_rules){return !memcmp(p,&rd_selection_rules_copy,sizeof*p);}
 rd_selection_rules_copy=*p;rd_selection_rules=p;rd_selection_bump(&rd_selection_connection);
 if(rd_selection_tx.state>0||rd_selection_monitor)rd_selection_cancel_cause(RHS_CAUSE_OWNER,"producer attached during action");
 return 1;
}
static const RhsRules*rd_selection_get(void){return rd_selection_rules;}
static void rd_selection_reset(void)
{
 rd_selection_last_provider_status=RHS_INVALID;
 rd_selection_bump(&rd_selection_interrupt);rd_selection_quarantine=0;rd_selection_outcome=0;
 rd_selection_monitor=rd_selection_safe_valid=0;rd_selection_attempts=0;
 if(rd_selection_tx.state)rd_selection_tx.state=-1;
 rd_selection_bump(&rd_selection_generation);
 rd_selection_mark(rd_selection_exhausted?RHS_STATE_EXHAUSTED:RHS_STATE_IDLE,
  rd_selection_exhausted?RHS_CAUSE_COUNTER:RHS_CAUSE_NONE);
}
static int rd_selection_response(const RhsRequest*q,const RhsResponse*r)
{
 RhsPhysical selected[5];int n;
 if(r->abi!=RHS_ABI||r->size!=sizeof*r||r->action!=q->state.action||
 r->generation!=q->state.generation||r->side!=q->state.side||
 r->mandatory<0||r->mandatory>1||r->destination!=r->selection[6])return 0;
 if(r->status==RHS_CANCELLED)return 1;
 if(r->status!=RHS_UNAVAILABLE&&r->status!=RHS_UNCHANGED&&r->status!=RHS_REPLACED)return 0;
 if(r->status!=RHS_REPLACED&&memcmp(r->selection,q->state.selection,12))return 0;
 if(r->status==RHS_REPLACED&&!r->mandatory)return 0;
 if(r->status==RHS_UNAVAILABLE&&r->mandatory)return 0;
 if(r->mandatory){memset(selected,0,sizeof selected);n=rhs_selected(&q->state,r->selection,selected);
 if(n<0||r->count!=n||memcmp(selected,r->selected,sizeof selected))return 0;}
 else if(r->count)return 0;
 return 1;
}
static int rd_selection_evaluate(const RhsRules*p,const RhsRequest*q,RhsResponse*r)
{
 int ok;unsigned generation=rd_selection_generation,connection=rd_selection_connection,interrupt=rd_selection_interrupt;
 memset(r,0,sizeof*r);
 if(rd_selection_reentrant||p!=rd_selection_rules||!p||
 memcmp(p,&rd_selection_rules_copy,sizeof*p))return 0;
 rd_selection_reentrant=1;ok=rd_selection_rules_copy.evaluate(q,r);rd_selection_reentrant=0;
 ok=ok&&p==rd_selection_rules&&connection==rd_selection_connection&&
 generation==rd_selection_generation&&interrupt==rd_selection_interrupt&&
 q->state.action==rd_selection_serial&&q->state.generation==rd_selection_generation&&
 !rd_selection_exhausted&&rhs_live(&q->state)&&rd_selection_response(q,r);
 rd_selection_last_provider_status=ok?r->status:RHS_INVALID;return ok;
}
static int rd_selection_precommit(void)
{
 RhsResponse now;
 if(!rd_selection_effective||!rd_selection_tx.state)return 0;
 if(rd_selection_blocked())return -1;
 if(rd_selection_tx.state<0)return rd_selection_cancel("cancelled transaction");
 if(rd_selection_tx.request.state.side!=(int)D_8009B1D5||
 rd_selection_tx.producer!=rd_selection_rules||
 !rhs_live(&rd_selection_tx.request.state))return rd_selection_cancel_cause(RHS_CAUSE_SNAPSHOT,"selection or physical snapshot changed");
 if(rd_selection_rules){
  if(!rd_selection_evaluate(rd_selection_rules,&rd_selection_tx.request,&now))
   return rd_selection_cancel_cause(RHS_CAUSE_PROTOCOL,"invalid owner response");
  if(now.status==RHS_CANCELLED)return rd_selection_cancel_cause(RHS_CAUSE_VETO,"owner veto");
  if(now.status==RHS_REPLACED||
     memcmp(now.revision,rd_selection_tx.answer.revision,sizeof now.revision)||
     now.mandatory!=rd_selection_tx.answer.mandatory)
   return rd_selection_cancel_cause(RHS_CAUSE_OWNER,"owner policy changed after capture");
  if(memcmp(now.selection,rd_selection_tx.request.state.selection,12))
   return rd_selection_cancel_cause(RHS_CAUSE_PROTOCOL,"owner response changed captured selection");
 }
 return 1;
}
static int rd_selection_last_outcome(void){return rd_selection_outcome;}
static const RhsRendezvous rd_selection_rendezvous={RHS_ABI,sizeof(RhsRendezvous),
 rd_selection_connect,rd_selection_get,rd_selection_precommit,rd_selection_last_outcome};
static int rd_selection_hand_count(int side)
{int n=0,i;for(i=0;i<5;i++)n+=D_800E9FF0[side].hand[i]>=0;return n;}
static int rd_selection_physical_same(const RhsSnapshot*a,const RhsSnapshot*b)
{return a->side==b->side&&!memcmp(a->hand,b->hand,sizeof a->hand)&&!memcmp(a->deck,b->deck,sizeof a->deck);}
static void rd_selection_quarantine_now(int cause)
{
 rd_selection_quarantine=1;rd_selection_outcome=-2;
 rd_selection_mark(RHS_STATE_QUARANTINE,cause);
}
static int rd_selection_disconnect(const RhsRules*p)
{
 if(p!=rd_selection_rules||!p)return 0;
 if(rd_selection_reentrant||rd_selection_planning){rd_selection_bump(&rd_selection_interrupt);return 0;}
 rd_selection_last_provider_status=RHS_INVALID;rd_selection_rules=0;memset(&rd_selection_rules_copy,0,sizeof rd_selection_rules_copy);
 rd_selection_bump(&rd_selection_connection);
 if(rd_selection_tx.state>0||rd_selection_monitor)rd_selection_cancel_cause(RHS_CAUSE_OWNER,"producer disconnected");
 return 1;
}
static int rd_selection_invalidate(const RhsRules*p,uint32_t cause)
{
 if(!p||p!=rd_selection_rules)return 0;
 if(cause==RHS_CAUSE_COUNTER)rd_selection_exhausted=1;
 rd_selection_last_provider_status=RHS_INVALID;
 rd_selection_bump(&rd_selection_interrupt);rd_selection_bump(&rd_selection_connection);
 if(rd_selection_tx.state>0||rd_selection_monitor)rd_selection_cancel_cause(cause==RHS_CAUSE_COUNTER?RHS_CAUSE_COUNTER:RHS_CAUSE_OWNER,"producer state changed");
 if(rd_selection_exhausted)rd_selection_mark(RHS_STATE_EXHAUSTED,RHS_CAUSE_COUNTER);
 return 1;
}
static int rd_selection_recovery_mode(void)
{
 if(rd_selection_exhausted)return RHS_RECOVER_PROCESS_RESTART;
 if(!rd_selection_blocked())return RHS_RECOVER_NONE;
 if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=4||!rd_selection_safe_valid||
 (*(u16*)D_8009B20C!=0&&*(u16*)D_8009B20C!=0x8000&&*(u16*)D_8009B20C!=1))return RHS_RECOVER_DUEL_RESET;
 return RHS_RECOVER_EXPLICIT;
}
static int rd_selection_read(RhsRecovery*out,uint32_t size)
{
 RhsRecovery s;if(!out||size!=sizeof s)return 0;memset(&s,0,sizeof s);
 s.abi=1;s.size=sizeof s;s.sequence=rd_selection_sequence;s.state=rd_selection_state;
 s.cause=rd_selection_cause;s.last_cause=rd_selection_last_cause;
 s.attempts=rd_selection_attempts;s.limit=RHS_RETRY_LIMIT;s.action=rd_selection_serial;
 s.generation=rd_selection_generation;s.connection_epoch=rd_selection_connection;
 s.connected=rd_selection_rules!=0;s.busy=rd_selection_reentrant||rd_selection_planning;
 s.effective=rd_selection_effective;s.recovery=rd_selection_recovery_mode();
 s.serial_exhausted=rd_selection_exhausted;s.side=rd_selection_safe_valid?rd_selection_safe.side:-1;
 s.last_provider_status=rd_selection_last_provider_status;*out=s;return 1;
}
static int rd_selection_recover(uint32_t sequence)
{
 RhsSnapshot live;
 if(sequence!=rd_selection_sequence||rd_selection_reentrant||rd_selection_planning||
 rd_selection_recovery_mode()!=RHS_RECOVER_EXPLICIT)return 0;
 if(!rhs_capture(&live,D_8009B1D5,1,1)||!rd_selection_physical_same(&rd_selection_safe,&live))return 0;
 rd_selection_bump(&rd_selection_generation);rd_selection_attempts=0;
 rd_selection_quarantine=rd_selection_monitor=0;rd_selection_tx.state=0;deck_pull.active=0;
 memset((void*)D_800EAE88,0,12);*(u16*)D_8009B20C=0;rd_selection_outcome=0;
 rd_selection_mark(RHS_STATE_IDLE,RHS_CAUSE_NONE);return !rd_selection_exhausted;
}
static const RhsLifecycle rd_selection_lifecycle={1,sizeof(RhsLifecycle),RHS_LIFECYCLE_MAGIC,
 rd_selection_read,rd_selection_disconnect,rd_selection_invalidate,rd_selection_recover};
/* After handoff, protect identities until the first native hand consumption.
 * Visual cursor/object fields may animate; physical hand mappings and selected
 * deck identities may not silently change under an already approved action. */
/* At the native collection boundary, selection bytes alone no longer own
 * the action: the controller consumes its cursor/ordered hand-stack marks.
 * Validate those derived inputs before delegation, never undo consumption. */
static int rd_selection_controller_matches(int side)
{
 int i,n=0,expected[5]={0,0,0,0,0};
 const DuelSelectionRecord*cursor=(const DuelSelectionRecord*)(D_800E9F10+side*DUEL_SELECTION_SIDE_SIZE);
 unsigned sub=*(u16*)D_8009B20C&15;
 if(sub<2)return 1; /* native cursor/stack construction is still in progress */
 while(n<5&&rd_selection_sources[n]){
  int h=(int)rd_selection_sources[n]-11;
  if(h<0||h>=5||expected[h])return 0;
  expected[h]=n+1;++n;
 }
 if(!n)return 0;
 if(n==1){
  if(cursor->field_15||
     (int)cursor->field_0E!=(int)rd_selection_sources[0]-11)return 0;
  memset(expected,0,sizeof expected);
 }else if((int)cursor->field_15!=n)return 0;
 for(i=0;i<5;i++)if((int)D_800EA030[i].active_09!=expected[i])return 0;
 return 1;
}
static int rd_selection_handoff_live(void)
{
 int i,side=(int)D_8009B1D5;
 if(!rd_selection_safe_valid||side!=rd_selection_safe.side||side<0||side>1)return 0;
 if(!rd_selection_controller_matches(side))return 0;
 for(i=0;i<5;i++)if(D_800E9FF0[side].hand[i]!=rd_selection_safe.hand[i].index)return 0;
 for(i=0;i<5&&rd_selection_sources[i];i++){
  int h=(int)rd_selection_sources[i]-11,record,index;const RhsPhysical*p;
  if(h<0||h>=5)return 0;p=&rd_selection_safe.hand[h];
  record=D_800907CC[side*5+h];index=p->index;
  if(record<side*15||record>=side*15+5||index<side*40||index>=(side+1)*40)return 0;
  if((uint32_t)(uintptr_t)D_801A7AD8[record].data!=p->data||
     D_801A7AD8[record].card_id!=p->id||
     !(D_801A7AD8[record].flags&DUEL_CARD_FLAG_OCCUPIED)||
     memcmp(&gDuel_aDeckCardRecords[index],rd_selection_safe.deck+index*6,4))return 0;
 }
 return 1;
}
static void rd_selection_hand_tick(void)
{
 RhsSnapshot before,after;int side=D_8009B1D5;
 unsigned generation=rd_selection_generation;int hand_before=0;
 int watching=rd_selection_effective&&side<2&&D_8009B360[side]>=0&&
 (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==4;
 if(watching&&rd_selection_blocked())return;
 if(watching){rhs_capture(&before,side,1,1);hand_before=rd_selection_hand_count(side);
  if(rd_selection_monitor&&(memcmp(rd_selection_sources,(const void*)D_800EAE88,5)||!rd_selection_handoff_live())){
   int cause=memcmp(rd_selection_sources,(const void*)D_800EAE88,5)?RHS_CAUSE_LATE_WRITE:RHS_CAUSE_PHYSICAL;
   rd_selection_cancel_cause(cause,"controller input/identity changed after handoff");
   rd_selection_quarantine_now(cause);return;
  }
 }
 ((void(*)(void))rd_selection_old_hand)();
 if(!watching)return;
 rhs_capture(&after,side,1,1);
 if(rd_selection_hand_count(side)<hand_before){rd_selection_monitor=0;rd_selection_attempts=0;rd_selection_safe_valid=0;
  rd_selection_tx.state=0;rd_selection_outcome=0;rd_selection_mark(RHS_STATE_IDLE,RHS_CAUSE_NONE);return;}
 if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==4&&*(u16*)D_8009B20C==1){
  if(generation!=rd_selection_generation&&rd_selection_tx.state>=0)rd_selection_cancel_cause(RHS_CAUSE_OWNER,"reset interrupted controller handoff");
  if(rd_selection_tx.state>0&&rd_selection_tx.answer.mandatory&&!rhs_live(&rd_selection_tx.request.state))rd_selection_cancel_cause(RHS_CAUSE_LATE_WRITE,"late materializer changed approved physical action");
  if(rd_selection_tx.state<0){
   rd_selection_safe=before;rd_selection_safe_valid=1;rd_selection_monitor=0;
   if(side!=(int)D_8009B1D5||!rd_selection_physical_same(&before,&after))rd_selection_quarantine_now(RHS_CAUSE_PHYSICAL);
   else{*(u16*)D_8009B20C=0;if(!rd_selection_blocked())rd_selection_mark(RHS_STATE_RETRY,rd_selection_cause);rd_selection_outcome=2;}
  }else{
   rd_selection_safe=after;rd_selection_safe_valid=1;rd_selection_monitor=1;
   memcpy(rd_selection_sources,(const void*)D_800EAE88,5);
   rd_selection_mark(RHS_STATE_HANDOFF,RHS_CAUSE_NONE);
  }
 }else if(rd_selection_tx.state<0){rd_selection_safe=before;rd_selection_safe_valid=1;}
}
static int rd_selection_hand(void)
{return D_8009B1D5<2&&D_8009B360[D_8009B1D5]>=0&&
 gAiScript_State.script_base==D_801A8000;}
static s32 rd_selection_prepare(s32 result)
{
 RhsRequest q;RhsResponse r;
 if(!result||!rd_selection_effective||!rd_selection_hand())return result;
 if(rd_selection_exhausted||rd_selection_serial==UINT32_MAX){rd_selection_exhausted=1;return rd_selection_cancel_cause(RHS_CAUSE_COUNTER,"action serial exhausted"),0;}
 ++rd_selection_serial;memset(&q,0,sizeof q);q.abi=RHS_ABI;q.size=sizeof q;
 rhs_capture(&q.state,D_8009B1D5,rd_selection_serial,rd_selection_generation);memset(&r,0,sizeof r);
 if(rd_selection_rules){
  if(!rd_selection_evaluate(rd_selection_rules,&q,&r))
   return rd_selection_cancel_cause(RHS_CAUSE_PROTOCOL,"invalid response"),0;
  if(r.status==RHS_CANCELLED)
   return rd_selection_cancel_cause(RHS_CAUSE_VETO,"explicit producer veto"),0;
  if(r.status==RHS_REPLACED){if(memcmp(q.state.selection,r.selection,12))deck_pull.active=0;
   memcpy((void*)D_800EAE88,r.selection,12);rhs_capture(&q.state,D_8009B1D5,q.state.action,q.state.generation);}
 }
 if(!q.state.selection[0])return rd_selection_cancel("no validated hand proposal"),0;
 rd_selection_tx.state=1;rd_selection_tx.producer=rd_selection_rules;
 rd_selection_tx.request=q;rd_selection_tx.answer=r;rd_selection_mark(RHS_STATE_VALIDATED,RHS_CAUSE_NONE);return result;
}
static void rd_selection_applied(int on)
{
 if(rd_selection_old_applied)rd_selection_old_applied(on);
 rd_selection_effective=on!=0;rd_selection_reset();
}
