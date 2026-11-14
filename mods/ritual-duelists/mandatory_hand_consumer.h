#include "hand_selection_snapshot.h"
static const RhsRules*rd_selection_rules;
static unsigned rd_selection_serial,rd_selection_generation=1;
static int rd_selection_effective,rd_selection_reentrant,rd_selection_exhausted;
static void(*rd_selection_old_applied)(int);
static void*rd_selection_old_hand;
static int rd_selection_quarantine,rd_selection_outcome;
static struct {int state;const RhsRules*producer;RhsRequest request;
 RhsResponse answer;} rd_selection_tx;
static int rd_selection_connect(const RhsRules*p)
{
 if(!p||p->abi!=RHS_ABI||p->size!=sizeof*p||!p->evaluate)return 0;
 if(rd_selection_rules&&rd_selection_rules!=p)return 0;
 rd_selection_rules=p;return 1;
}
static const RhsRules*rd_selection_get(void){return rd_selection_rules;}
static void rd_selection_reset(void)
{
 rd_selection_quarantine=0;rd_selection_outcome=0;
 if(rd_selection_tx.state)rd_selection_tx.state=-1;
 if(++rd_selection_generation==0)rd_selection_exhausted=1;
}
static int rd_selection_cancel(const char*reason)
{
 rd_selection_tx.state=-1;deck_pull.active=0;
 rd_selection_outcome=-1;
 memset((void*)D_800EAE88,0,12);
 if(host&&host->log)host->log(host,"HAL25: hand action withheld (%s); no fallback",reason);
 return -1;
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
 int ok;memset(r,0,sizeof*r);
 if(rd_selection_reentrant)return 0;rd_selection_reentrant=1;
 ok=p->evaluate(q,r);rd_selection_reentrant=0;
 return ok&&rhs_live(&q->state)&&rd_selection_response(q,r);
}
static int rd_selection_precommit(void)
{
 RhsResponse now;
 if(!rd_selection_effective||!rd_selection_tx.state)return 0;
 if(rd_selection_tx.state<0)return rd_selection_cancel("cancelled transaction");
 if(rd_selection_tx.request.state.side!=(int)D_8009B1D5||
 rd_selection_tx.producer!=rd_selection_rules||
 !rhs_live(&rd_selection_tx.request.state))return rd_selection_cancel("selection or physical snapshot changed");
 if(rd_selection_rules){
  if(!rd_selection_evaluate(rd_selection_rules,&rd_selection_tx.request,&now)||
  now.status==RHS_CANCELLED||now.status==RHS_REPLACED||
  memcmp(now.selection,rd_selection_tx.request.state.selection,12)||
  memcmp(now.revision,rd_selection_tx.answer.revision,sizeof now.revision)||
  now.mandatory!=rd_selection_tx.answer.mandatory)
   return rd_selection_cancel("owner veto, revision or response changed");
 }
 return 1;
}
static int rd_selection_last_outcome(void){return rd_selection_outcome;}
static const RhsRendezvous rd_selection_rendezvous={RHS_ABI,sizeof(RhsRendezvous),
 rd_selection_connect,rd_selection_get,rd_selection_precommit,rd_selection_last_outcome};
/* Native hand state 0 advances to 1 after a void swap even on a veto. An
 * empty byte tuple is NOT native cancellation: state 2 consumes the cursor.
 * Own only the aborted 0->1 transition; no normal controller path changes.
 * Rebuild selection next tick only if the physical hand/deck stayed exact.
 * A corrupted physical hand is quarantined until reset, never guessed at. */
static void rd_selection_hand_tick(void)
{
 RhsSnapshot before,after;int side=D_8009B1D5;
 int watching=rd_selection_effective&&side<2&&D_8009B360[side]>=0&&
 (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==4;
 if(watching&&rd_selection_quarantine)return;
 if(watching)rhs_capture(&before,side,1,1);
 ((void(*)(void))rd_selection_old_hand)();
 if(watching&&rd_selection_tx.state<0&&
 (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==4&&
 *(u16*)D_8009B20C==1){
  rhs_capture(&after,side,1,1);
  if(side!=(int)D_8009B1D5||memcmp(before.hand,after.hand,sizeof before.hand)||
     memcmp(before.deck,after.deck,sizeof before.deck)){
   rd_selection_quarantine=1;rd_selection_outcome=-2;
   if(host&&host->log)host->log(host,"HAL25: invalid physical hand; reset/new duel required");
  }else{
   /* State0's native snapshot/AiScript_Init path prepares a NEW action.
    * No cursor/hand objects have advanced in the rejected action. */
   *(u16*)D_8009B20C=0;rd_selection_outcome=2;
  }
 }
}
static int rd_selection_hand(void)
{return D_8009B1D5<2&&D_8009B360[D_8009B1D5]>=0&&
 gAiScript_State.script_base==D_801A8000;}
static s32 rd_selection_prepare(s32 result)
{
 RhsRequest q;RhsResponse r;
 if(!result||!rd_selection_effective||!rd_selection_hand())return result;
 if(rd_selection_exhausted||rd_selection_serial==UINT32_MAX)
  return rd_selection_cancel("action serial exhausted"),0;
 ++rd_selection_serial;
 memset(&q,0,sizeof q);q.abi=RHS_ABI;q.size=sizeof q;
 rhs_capture(&q.state,D_8009B1D5,rd_selection_serial,rd_selection_generation);
 memset(&r,0,sizeof r);
 if(rd_selection_rules){
  if(!rd_selection_evaluate(rd_selection_rules,&q,&r)||r.status==RHS_CANCELLED)
   return rd_selection_cancel("invalid response or explicit veto"),0;
  if(r.status==RHS_REPLACED){
   if(memcmp(q.state.selection,r.selection,12))deck_pull.active=0;
   memcpy((void*)D_800EAE88,r.selection,12);
   rhs_capture(&q.state,D_8009B1D5,q.state.action,q.state.generation);
  }
 }
 if(!q.state.selection[0])return rd_selection_cancel("no validated hand proposal"),0;
 rd_selection_tx.state=1;rd_selection_tx.producer=rd_selection_rules;
 rd_selection_tx.request=q;rd_selection_tx.answer=r;return result;
}
static void rd_selection_applied(int on)
{
 if(rd_selection_old_applied)rd_selection_old_applied(on);
 rd_selection_effective=on!=0;rd_selection_reset();
}
