/* LAB ONLY: included in the own, available TW adapter. No binary core edits.
 * Policies come only from the private test setting. Default is OBSERVE.
 * Same synchronous process/lifetime assumptions as the PR34 companion. */
#include <string.h>
#include "selection_finalization_v1.h"
static const HfFinalization *tw_hf_api;
/* Private diagnostics, never capabilities or public ABI fields. */
enum {TW_DUEL_COLD=0,TW_DUEL_READY,TW_DUEL_TRANSITION,TW_DUEL_REFUSED};
static unsigned tw_duel_epoch,tw_lifecycle_interrupt,tw_hf_tx_duel,tw_hf_tx_interrupt;
static unsigned tw_duel_transition,tw_recovery_state,tw_projection_depth,tw_projection_conflicts;
static int tw_recovery_reason;
static void *tw_client_original_init;
static int tw_client_effective;
static void (*tw_client_old_applied)(int);
static void (*tw_client_old_shutdown)(void);
static HfClient tw_hf_client;
static HfOffer tw_hf_offer;
static HfProposal tw_hf_proposal;
static HfTicket tw_hf_ticket;
static int tw_hf_busy,tw_hf_callback,tw_hf_stage,tw_hf_policy;
static volatile int tw_hf_result;
static unsigned tw_hf_authorizations,tw_hf_acks;
static unsigned tw_final_category,tw_final_armed,tw_final_gate,tw_final_block;
static uint8_t tw_final_selection[12];
static int tw_final_slot,tw_final_index,tw_final_count,tw_final_record;
static void *tw_final_data,*tw_final_object;
static int tw_final_cursor_slot,tw_final_cursor_count;
static uint8_t tw_final_marks[5];
static void tw_client_log(const char *stage,int result) {
 if(compat_host&&compat_host->log)compat_host->log(compat_host,
  "TW_CLIENT stage=%s result=%d policy=%d category=%u action=%u generation=%u connection=%u nonce=%u auth=%u ack=%u",
  stage,result,tw_hf_policy,tw_final_category,tw_hf_ticket.token.action,
  tw_hf_ticket.token.generation,tw_hf_ticket.token.connection,tw_hf_ticket.token.nonce,
  tw_hf_authorizations,tw_hf_acks);
}
static int tw_client_api(const HfFinalization*p) {
 return p&&p->abi==HF_ABI&&p->size==sizeof*p&&p->magic==HF_MAGIC&&
 p->connect&&p->disconnect&&p->offer&&p->prepare&&p->authorize&&p->confirm&&p->cancel&&p->read;
}
static int tw_client_packet(const HfTicket*t,const HfProposal*p,int stage) {
 return !tw_duel_transition&&tw_hf_tx_duel==tw_duel_epoch&&
 tw_hf_tx_interrupt==tw_lifecycle_interrupt&&tw_client_effective&&tw_hf_busy&&!tw_hf_callback&&tw_hf_stage==stage&&
 compat_host&&compat_host->find&&compat_host->find(compat_host,HF_KEY)==tw_hf_api&&
 t&&p&&!memcmp(t,&tw_hf_ticket,sizeof*t)&&!memcmp(p,&tw_hf_proposal,sizeof*p)&&
 t->abi==HF_ABI&&t->size==sizeof*t&&t->count==1&&t->policy==(unsigned)tw_hf_policy;
}
static int tw_client_authorize(const HfTicket*t,const HfProposal*p) {
 int ok=tw_client_packet(t,p,1);if(!ok)return 0;
 tw_hf_callback=1;++tw_hf_authorizations;tw_hf_callback=0;return 1;
}
static int tw_client_confirm(const HfTicket*t,const HfProposal*p) {
 int ok=tw_client_packet(t,p,2);if(!ok)return 0;
 tw_hf_callback=1;++tw_hf_acks;tw_hf_callback=0;return 1;
}
static int tw_client_disconnect(void) {
 if(!tw_hf_api)return 1;
 /* Do not invoke a cached provider after discovery changed/disappeared.
  * Code unloading without cooperative ACK is outside this native protocol. */
 if(!compat_host||!compat_host->find||compat_host->find(compat_host,HF_KEY)!=tw_hf_api)return 0;
 int rc=tw_hf_api->disconnect(&tw_hf_client);
 if(rc!=HF_OK)return 0;
 tw_hf_api=0;tw_hf_stage=0;return 1;
}
/* Close the exact resident client through protocol ACKs, not HfStatus.
 * STALE may follow an independently acknowledged disconnect. Reconnect+close
 * can prove closure only if no other client owns the provider. Busy/changed
 * tables remain refused. This is used only at a legitimate duel boundary. */
static int tw_client_close_duel(void) {
 if(!tw_hf_api)return 1;
 const HfFinalization*p=tw_hf_api;
 if(!compat_host||!compat_host->find||compat_host->find(compat_host,HF_KEY)!=p||!tw_client_api(p))return 0;
 int rc=p->disconnect(&tw_hf_client);
 if(rc==HF_STALE&&p->connect(&tw_hf_client)==HF_OK)rc=p->disconnect(&tw_hf_client);
 if(rc!=HF_OK)return 0;
 tw_hf_api=0;tw_hf_stage=0;return 1;
}
static void tw_client_init_scene(void) {
 /* Same-thread reentrancy is instrumented, not a normal UI transition.
  * Do not reinitialize the engine inside an unfinished owner callback. */
 if(tw_duel_transition||tw_hf_busy||tw_hf_callback||tw_projection_depth){
  if(tw_lifecycle_interrupt!=UINT32_MAX)++tw_lifecycle_interrupt;
  tw_final_armed=tw_final_block=1;tw_recovery_state=TW_DUEL_REFUSED;tw_recovery_reason=HF_BUSY;return;
 }
 tw_duel_transition=1;tw_recovery_state=TW_DUEL_TRANSITION;
 tw_final_armed=tw_final_block=1;tw_final_gate=0;
 int closed=tw_client_close_duel();unsigned interrupt=tw_lifecycle_interrupt;
 ((void(*)(void))tw_client_original_init)();
 if(closed&&tw_client_effective&&interrupt==tw_lifecycle_interrupt&&
    tw_duel_epoch!=UINT32_MAX&&tw_lifecycle_interrupt!=UINT32_MAX){
  ++tw_duel_epoch;
  tw_final_armed=tw_final_gate=tw_final_block=0;tw_final_category=0;
  tw_final_slot=tw_final_index=tw_final_record=tw_final_count=0;
  tw_final_data=tw_final_object=0;
  memset(tw_final_selection,0,sizeof tw_final_selection);memset(tw_final_marks,0,sizeof tw_final_marks);
  memset(&tw_hf_ticket,0,sizeof tw_hf_ticket);memset(&tw_hf_offer,0,sizeof tw_hf_offer);memset(&tw_hf_proposal,0,sizeof tw_hf_proposal);
  tw_hf_stage=0;tw_hf_result=HF_NO;tw_projection_conflicts=0;
  /* Never reset client.epoch, companion nonce, RD generation or pending. */
  tw_recovery_state=TW_DUEL_READY;tw_recovery_reason=HF_OK;
 }else{
  tw_recovery_state=TW_DUEL_REFUSED;tw_recovery_reason=closed?HF_EXHAUSTED:HF_ACK_MISSING;
 }
 tw_duel_transition=0;tw_client_log("duel-boundary",tw_recovery_reason);
}
static void tw_client_applied(int on) {
 tw_client_effective=on!=0;
 if(!on){tw_final_armed=tw_final_gate=0;tw_client_disconnect();}
 if(tw_client_old_applied)tw_client_old_applied(on);
}
static void tw_client_shutdown(void) {
 tw_client_effective=0;tw_final_armed=tw_final_gate=0;tw_client_disconnect();
 if(tw_client_old_shutdown)tw_client_old_shutdown();
}
static int tw_final_capture(void) {
 tw_final_armed=tw_final_gate=tw_final_block=0;
 if(current_side()!=1||phase_now()!=SCENE_HAND||!ai_selection_is_single_tw())return 0;
 int slot=(int)D_800EAE88[0]-11,ix=slot>=0&&slot<5?D_800E9FF0[1].hand[slot]:-1;
 unsigned dest=D_800EAE88[6];
 int record=slot>=0&&slot<5?D_800907CC[slot+5]:-1;
 if(slot<0||slot>=5||record<15||record>=20||ix<40||ix>=80||gDuel_aDeckCardRecords[ix].id!=16||
 !D_800EA030[slot].object||D_800EAE88[1]||D_800EAE88[5]||D_800EAE88[7]||
 dest<1||dest>10||(dest>=6&&D_800EAE88[8]!=0)||
 D_801A7AD8[record].card_id!=16||D_801A7AD8[record].data!=&gDuel_aDeckCardRecords[ix]||
 D_801A7AD8[record].object!=D_800EA030[slot].object) {
  tw_final_armed=tw_final_block=1;return 0;
 }
 tw_final_category=dest>=6?PLAY_SUPPORT:PLAY_MONSTER;
 tw_final_slot=slot;tw_final_index=ix;tw_final_data=D_801A7AD8[record].data;tw_final_record=record;
 tw_final_object=D_800EA030[slot].object;memcpy(tw_final_selection,D_800EAE88,12);
 tw_final_count=0;for(int i=0;i<5;i++)tw_final_count+=D_800E9FF0[1].hand[i]>=0;
 DuelSelectionRecord*c=hand_selection(1);tw_final_cursor_slot=(int)(int8_t)c->field_0E;tw_final_cursor_count=c->field_15;
 for(int i=0;i<5;i++)tw_final_marks[i]=D_800EA030[i].active_09;
 tw_final_armed=1;return 1;
}
static int tw_final_live(void) {
 return tw_final_armed&&current_side()==1&&phase_now()==SCENE_HAND&&
 !memcmp(tw_final_selection,D_800EAE88,12)&&
 D_800E9FF0[1].hand[tw_final_slot]==tw_final_index&&
 D_800907CC[tw_final_slot+5]==tw_final_record&&
 D_801A7AD8[tw_final_record].card_id==16&&
 D_801A7AD8[tw_final_record].data==tw_final_data&&
 D_801A7AD8[tw_final_record].object==tw_final_object&&
 &gDuel_aDeckCardRecords[tw_final_index]==tw_final_data&&
 gDuel_aDeckCardRecords[tw_final_index].id==16&&
 D_800EA030[tw_final_slot].object==tw_final_object;
}
static int tw_client_tick_ok(void) {
 if(tw_final_block)return 0;
 int count=0;for(int i=0;i<5;i++)count+=D_800E9FF0[1].hand[i]>=0;
 /* Boundary ends after consumption. Never block presentation or pretend to
  * roll back a card already removed. RD monitor owns multi-card paths. */
 if(count<tw_final_count){tw_final_armed=0;return 1;}
 return tw_final_live();
}
static int tw_client_offer_ok(const HfOffer*o) {
 int h=(int)o->observed.selection[0]-11;
 return o->abi==HF_ABI&&o->size==sizeof*o&&o->observed.size==sizeof o->observed&&
 o->token.action&&o->token.generation&&o->token.connection&&o->token.nonce&&o->token.client&&
 o->observed.action==o->token.action&&o->observed.generation==o->token.generation&&
 o->observed.side==1&&o->observed.phase==4&&h>=0&&h<5&&
 !o->observed.selection[1]&&!o->observed.selection[5]&&!o->observed.selection[7]&&
 o->observed.selection[6]>=1&&o->observed.selection[6]<=10&&o->mandatory<=1;
}
static int tw_client_finish(int result) {
 int negotiating=0;
 if(tw_duel_transition){if(tw_lifecycle_interrupt!=UINT32_MAX)++tw_lifecycle_interrupt;return 0;}
 if(tw_projection_conflicts){tw_final_armed=tw_final_block=1;tw_hf_result=HF_CHANGED;return 0;}
 if(!tw_client_effective)return 0;
 if(tw_hf_busy){tw_hf_result=HF_BUSY;return 0;}
 if(!tw_client_disconnect()){tw_client_log("disconnect-refused",HF_STALE);return 0;}
 tw_hf_policy=compat_host->setting?compat_host->setting(compat_host,"experimental_handoff",HF_OBSERVE):HF_OBSERVE;
 tw_hf_result=HF_NO;tw_hf_stage=0;
 if(!result||current_side()!=1||phase_now()!=SCENE_HAND)return result;
 if(tw_hf_policy==HF_KEEP_RD||tw_hf_policy==HF_ALLOW_TW) {
  const HfFinalization*p=compat_host->find?compat_host->find(compat_host,HF_KEY):0;
  if(tw_client_api(p)) {
   if(tw_hf_client.epoch==UINT32_MAX){tw_hf_result=HF_EXHAUSTED;return 0;}
   ++tw_hf_client.epoch;tw_hf_client.abi=HF_ABI;tw_hf_client.size=sizeof tw_hf_client;
   tw_hf_client.authorize=tw_client_authorize;tw_hf_client.confirm=tw_client_confirm;
   tw_hf_api=p;tw_hf_result=p->connect(&tw_hf_client);
   if(tw_hf_result==HF_OK)tw_hf_result=p->offer(&tw_hf_offer,sizeof tw_hf_offer);
   if(tw_hf_result==HF_OK)negotiating=1;
   if(tw_hf_result==HF_OK&&!tw_client_offer_ok(&tw_hf_offer))tw_hf_result=HF_ABI_ERROR;
   if(tw_hf_result==HF_OK) {
    HfProposal*q=&tw_hf_proposal;memset(q,0,sizeof*q);q->abi=HF_ABI;q->size=sizeof*q;
    q->token=tw_hf_offer.token;q->policy=tw_hf_policy;q->snapshot=tw_hf_offer.observed;q->cursor=tw_hf_offer.cursor;
    int slot=(int)q->snapshot.selection[0]-11;
    q->destination=q->snapshot.selection[6];
    if(slot>=0&&slot<5)q->physical=q->snapshot.hand[slot];
    int magic=q->physical.id==16&&q->destination>=6&&q->destination<=10;
    q->kind=magic?HF_TIME_MAGIC:HF_NATIVE;
    q->category=magic?HF_SUPPORT:((unsigned)(uint8_t)gDuel_aActiveCards[q->snapshot.selection[0]].card_type>=CARD_TYPE_MAGIC?HF_SUPPORT:HF_MONSTER);
    tw_hf_result=p->prepare(q,&tw_hf_ticket,sizeof tw_hf_ticket);
    if(tw_hf_result==HF_OK&&(tw_hf_ticket.abi!=HF_ABI||tw_hf_ticket.size!=sizeof tw_hf_ticket||
      memcmp(&tw_hf_ticket.token,&q->token,sizeof q->token)||tw_hf_ticket.policy!=q->policy||
      tw_hf_ticket.count!=1||tw_hf_ticket.category<1||tw_hf_ticket.category>2||
      tw_hf_ticket.destination<1||tw_hf_ticket.destination>10))tw_hf_result=HF_ABI_ERROR;
    if(tw_hf_result==HF_OK) {
     tw_hf_tx_duel=tw_duel_epoch;tw_hf_tx_interrupt=tw_lifecycle_interrupt;
     tw_hf_busy=1;tw_hf_stage=1;tw_hf_result=p->authorize(&tw_hf_ticket);
     if(tw_hf_result==HF_OK&&(tw_hf_tx_duel!=tw_duel_epoch||tw_hf_tx_interrupt!=tw_lifecycle_interrupt))tw_hf_result=HF_CHANGED;
     if(tw_hf_result==HF_OK){HfStatus status;tw_hf_stage=2;tw_hf_result=p->confirm(&tw_hf_ticket,&status,sizeof status);}
     tw_hf_stage=0;tw_hf_busy=0;
    }
   }
  }else tw_hf_result=p?HF_ABI_ERROR:HF_NO;
 }
 if(negotiating&&tw_hf_result!=HF_OK){
  /* A failed, begun transaction cannot fall through if its owner was turned
   * OFF during the handshake. Stop before native advances to collection.
   * Only explicit reset/new lifecycle may clear this local refusal. */
  tw_final_armed=tw_final_block=1;tw_final_gate=0;
  tw_client_log("negotiation-refused",tw_hf_result);return 0;
 }
 if(tw_hf_result==HF_OK||!compat_host->find||!compat_host->find(compat_host,HF_KEY))tw_final_capture();
 else tw_final_armed=tw_final_gate=tw_final_block=0;
 tw_client_log("planner-final",tw_hf_result);
 /* No automatic cancel/recover. The unchanged RD monitor owns refusal. */
 return result;
}
/* Final category is projected only into the selected logical AI record,
 * synchronously around the actual swap chain. No stat/physical/ledger writes.
 * Native and HARD swaps do not read this type. RD snapshots don't encode it.
 * Gate must remain in the qualified chain; guard admission is unchanged. */
static void tw_client_swap(void) {
 DualPlayStatePrefix*s=resolve_dual_state();
 /* Without a cooperative confirmation, the outer mandatory R owner may
  * replace a provisional TW proposal with a non-TW action BEFORE the gate.
  * Delegate that native action unchanged; do not invent TW ownership over it.
  * Confirmed handoffs, same-TW changes and post-gate changes remain fenced. */
 if(tw_final_armed&&!tw_final_gate&&tw_hf_result!=HF_OK&&!ai_selection_is_single_tw())tw_final_armed=0;
 if(tw_final_armed) {
  DuelSelectionRecord*c=hand_selection(1);int cursor=(int)(int8_t)c->field_0E==tw_final_cursor_slot&&c->field_15==tw_final_cursor_count;
  for(int i=0;i<5;i++)if(tw_final_marks[i]!=D_800EA030[i].active_09)cursor=0;
  if(tw_final_block||!cursor||!tw_final_live()||(s&&(s->used[1]&tw_final_category))) {
   tw_final_block=1;tw_client_log("category-or-identity-refused",HF_CHANGED);return;
  }
  int index=D_800EAE88[0];s8 old=gDuel_aActiveCards[index].card_type;
  AiActiveCard saved=gDuel_aActiveCards[index];
  int projected=tw_final_category==PLAY_SUPPORT;
  if(tw_projection_depth){++tw_projection_conflicts;tw_final_block=1;return;}
  ++tw_projection_depth;
  if(tw_final_category==PLAY_SUPPORT)gDuel_aActiveCards[index].card_type=CARD_TYPE_MAGIC;
  tw_final_gate=1;tw_client_log("pre-gate",HF_OK);
  ((void(*)(void))compat_original_ai_swap)();
  /* Restore only the same logical record, never cards/selection/ledger. */
  AiActiveCard now=gDuel_aActiveCards[index];
  if(projected){
   /* deck_index is only an opaque unchanged byte for hand entries, never
    * proof of physical identity; tw_final_live verifies the real mapping. */
   if(now.card_id==saved.card_id&&now.deck_index==saved.deck_index&&tw_final_live()&&now.card_type==CARD_TYPE_MAGIC)
    gDuel_aActiveCards[index].card_type=old;
   else {++tw_projection_conflicts;tw_final_block=1;}
  }
  --tw_projection_depth;
  return;
 }
 ((void(*)(void))compat_original_ai_swap)();
}
/* Category view only during construction of a bonus snapshot, not planning.
 * Preserve the real spent-Monster ledger. Dual may keep TW as Support, while
 * ordinary Monsters and rituals remain hidden. Restore native logical type
 * before any planner/provider runs. No hidden ID is reconstructed afterward. */
static void *tw_client_original_snapshot;
static unsigned tw_snapshot_views;
static void tw_client_snapshot(void) {
 if(tw_projection_depth){++tw_projection_conflicts;tw_final_armed=tw_final_block=1;return;}
 DualPlayStatePrefix*s=resolve_dual_state();
 int view=s&&current_side()==1&&s->bonus_hand[1]&&s->used[1]==PLAY_MONSTER;
 uint32_t mask=(uint32_t)CARD_STAT_TYPE_MASK<<CARD_STAT_TYPE_SHIFT;
 uint32_t projected=(uint32_t)CARD_TYPE_MAGIC<<CARD_STAT_TYPE_SHIFT;
 uint32_t old=gDuel_adwCardStats[15];AiActiveCard before[45];
 if(view){
  memcpy(before,&gDuel_aActiveCards[11],sizeof before);
  gDuel_adwCardStats[15]=(old&~mask)|projected;++tw_snapshot_views;++tw_projection_depth;
 }
 ((void(*)(void))tw_client_original_snapshot)();
 if(view){
  uint32_t now=gDuel_adwCardStats[15];
  /* Restore only borrowed type bits; preserve independent writers. */
  if((now&mask)==projected)gDuel_adwCardStats[15]=(now&~mask)|(old&mask);
  else ++tw_projection_conflicts;
  for(int i=11;i<56;i++)if(gDuel_aActiveCards[i].card_id==16){
   AiActiveCard*card=&gDuel_aActiveCards[i];
   /* Early return did not create a snapshot: don't retag pre-existing
    * records. Native rebuild changes the borrowed type on the normal path. */
   if(!memcmp(card,&before[i-11],sizeof*card))continue;
   if(card->card_type==CARD_TYPE_MAGIC)card->card_type=(old>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK;
   else if(card->card_type!=((old>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK))++tw_projection_conflicts;
  }
  --tw_projection_depth;
  if(tw_projection_conflicts)tw_final_armed=tw_final_block=1;
 }
}
