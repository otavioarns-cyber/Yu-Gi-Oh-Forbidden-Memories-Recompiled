/* Included ONLY in a build-directory copy of RD. All live writes occur at
 * explicit test-policy confirmation, before exchange/consumption. No recover. */
#include "selection_finalization_v1.h"
static const HfClient*hf_client;
static HfClient hf_client_copy;
static uint32_t hf_client_epoch=1,hf_nonce,hf_state,hf_consumed;
static int hf_busy,hf_reason;
static int hf_reentered;
static HfOffer hf_offer_value;
static HfProposal hf_proposal;
static HfTicket hf_ticket;
static __typeof__(deck_pull) hf_pending_value;
static int hf_fail(int reason){hf_reason=reason;return reason;}
static void hf_cursor(HfCursor*c){
 const DuelSelectionRecord*r=(const DuelSelectionRecord*)(D_800E9F10+D_8009B1D5*DUEL_SELECTION_SIDE_SIZE);
 memset(c,0,sizeof*c);c->mode=D_8009B174;c->substate=*(u16*)D_8009B20C;
 c->slot=(int)(int8_t)r->field_0E;c->count=r->field_15;
 c->row=D_8009B1B4?D_8009B1B4->row:-1;c->col=D_8009B1B4?D_8009B1B4->col:-1;
 for(int i=0;i<5;i++)c->marks[i]=D_800EA030[i].active_09;
}
static int hf_stamp_live(const HfToken*t){
 return rd_selection_effective&&!rd_selection_blocked()&&!rd_selection_exhausted&&
 t->action==rd_selection_serial&&t->generation==rd_selection_generation&&
 t->connection==rd_selection_connection&&t->nonce==hf_nonce&&
 t->client==hf_client_epoch&&
 !memcmp(t->revision,rd_selection_tx.answer.revision,sizeof t->revision)&&
 D_8009B1D5==hf_offer_value.observed.side&&
 (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==4;
}
static int hf_same_world(const RhsSnapshot*a,const RhsSnapshot*b){
 RhsSnapshot copy=*a;memcpy(copy.selection,b->selection,12);return !memcmp(&copy,b,sizeof copy);
}
static int hf_same_ticket(const HfTicket*t){return t&&t->abi==HF_ABI&&t->size==sizeof*t&&!memcmp(t,&hf_ticket,sizeof*t);}
static int hf_read(HfStatus*out,uint32_t size){
 if(!out||size!=sizeof*out)return HF_ABI_ERROR;
 HfStatus z={0};z.abi=HF_ABI;z.size=sizeof z;z.state=hf_state;z.reason=hf_reason;
 z.token=hf_ticket.token;z.busy=hf_busy;z.connected=hf_client!=0;z.consumed=hf_consumed;*out=z;return HF_OK;
}
static int hf_connect(const HfClient*p){
 if(hf_busy){hf_reentered=1;return hf_fail(HF_BUSY);}
 if(!p||p->abi!=HF_ABI||p->size!=sizeof*p||!p->epoch||!p->authorize||!p->confirm)return hf_fail(HF_ABI_ERROR);
 if(hf_client)return p==hf_client&&!memcmp(p,&hf_client_copy,sizeof*p)?HF_OK:hf_fail(HF_CHANGED);
 if(hf_client_epoch==UINT32_MAX)return hf_fail(HF_EXHAUSTED);
 hf_client=p;hf_client_copy=*p;++hf_client_epoch;return HF_OK;
}
static int hf_disconnect(const HfClient*p){
 if(hf_busy){hf_reentered=1;return hf_fail(HF_BUSY);}
 if(!p||p!=hf_client)return hf_fail(HF_STALE);
 /* ACK of disconnection: never read the client's table again afterwards. */
 hf_client=0;memset(&hf_client_copy,0,sizeof hf_client_copy);
 if(hf_client_epoch!=UINT32_MAX)++hf_client_epoch;
 if(hf_state>=HF_PREPARED&&hf_state<HF_CONFIRMED)hf_state=HF_ABORTED;
 return HF_OK;
}
static int hf_offer(HfOffer*out,uint32_t size){
 if(hf_busy){hf_reentered=1;return hf_fail(HF_BUSY);}
 if(!out||size!=sizeof*out)return hf_fail(HF_ABI_ERROR);
 if(!hf_client)return hf_fail(HF_ACK_MISSING);
 if(!rd_selection_effective||rd_selection_tx.state!=1||rd_selection_blocked()||rd_selection_monitor||rd_selection_exhausted)return hf_fail(HF_STALE);
 if(hf_nonce==UINT32_MAX)return hf_fail(HF_EXHAUSTED);
 if(hf_state>=HF_CONFIRMED&&hf_state<=HF_CONSUMED&&
 hf_ticket.token.action==rd_selection_serial&&hf_ticket.token.generation==rd_selection_generation)return hf_fail(HF_ALREADY_SPENT);
 HfOffer z={0};z.abi=HF_ABI;z.size=sizeof z;z.token.action=rd_selection_serial;
 z.token.generation=rd_selection_generation;z.token.connection=rd_selection_connection;
 memcpy(z.token.revision,rd_selection_tx.answer.revision,sizeof z.token.revision);
 z.token.nonce=++hf_nonce;z.token.client=hf_client_epoch;z.mandatory=rd_selection_tx.answer.mandatory;
 memcpy(z.previous,rd_selection_tx.request.state.selection,12);
 if(!rhs_capture(&z.observed,D_8009B1D5,z.token.action,z.token.generation)||
 !hf_same_world(&z.observed,&rd_selection_tx.request.state))return hf_fail(HF_CHANGED);
 hf_cursor(&z.cursor);hf_pending_value=deck_pull;
 if(deck_pull.active){
 int s=deck_pull.source,index=s>=16&&s<=55?gDuel_aActiveCards[s].deck_index:-1;
 if(deck_pull.side!=(int)D_8009B1D5||s<16||s>55||s!=z.previous[0]||z.previous[1]||
 deck_pull.replace<0||deck_pull.replace>=5||D_800E9FF0[D_8009B1D5].hand[deck_pull.replace]<0||
 (deck_pull.mode!=RD_PULL_MAGIC&&deck_pull.mode!=RD_PULL_FIELD)||
 (deck_pull.mode==RD_PULL_MAGIC?(z.previous[8]!=1||deck_pull.target<6||deck_pull.target>10):(deck_pull.target<1||deck_pull.target>5))||
 deck_pull.card!=gDuel_aActiveCards[s].card_id||deck_pull.target!=z.previous[6]||
 index<(int)D_8009B1D5*40+(int)D_800E9FF0[D_8009B1D5].deck_draw_cursor||
 index>=((int)D_8009B1D5+1)*40||gDuel_aDeckCardRecords[index].id!=deck_pull.card)return hf_fail(HF_CHANGED);
 z.pending=(HfPending){1,s,deck_pull.replace,deck_pull.card,deck_pull.mode,deck_pull.target,index,gDuel_aDeckCardRecords[index].data_block_index};
 }
 hf_offer_value=z;hf_state=HF_OFFERED;hf_consumed=0;hf_reason=0;*out=z;return HF_OK;
}
static int hf_unchanged(void){
 HfCursor c;hf_cursor(&c);RhsSnapshot original=hf_offer_value.observed;
 memcpy(original.selection,hf_offer_value.previous,12);
 return rd_selection_tx.state==1&&rd_selection_tx.producer==rd_selection_rules&&
 rd_selection_tx.answer.mandatory==(int)hf_offer_value.mandatory&&
 !memcmp(&original,&rd_selection_tx.request.state,sizeof original)&&hf_stamp_live(&hf_ticket.token)&&rhs_live(&hf_offer_value.observed)&&
 !memcmp(&c,&hf_offer_value.cursor,sizeof c)&&!memcmp(&deck_pull,&hf_pending_value,sizeof deck_pull);
}
static int hf_owner(RhsResponse*out){
 if(!rd_selection_rules)return 0; /* unqualified absence cannot authorize transfer */
 RhsRequest q={0};q.abi=RHS_ABI;q.size=sizeof q;q.state=hf_offer_value.observed;
 hf_reentered=0;hf_busy=1;
 int ok=rd_selection_evaluate(rd_selection_rules,&q,out);hf_busy=0;
 return ok&&!hf_reentered&&out->status!=RHS_CANCELLED&&
 !memcmp(out->revision,hf_offer_value.token.revision,sizeof out->revision);
}
static int hf_prepare(const HfProposal*p,HfTicket*out,uint32_t size){
 if(hf_busy){hf_reentered=1;return hf_fail(HF_BUSY);}
 if(!p||!out||size!=sizeof*out||p->abi!=HF_ABI||p->size!=sizeof*p)return hf_fail(HF_ABI_ERROR);
 if(hf_state!=HF_OFFERED||memcmp(&p->token,&hf_offer_value.token,sizeof p->token))return hf_fail(HF_STALE);
 if(p->policy!=HF_KEEP_RD&&p->policy!=HF_ALLOW_TW)return hf_fail(HF_NO);
 if(!hf_offer_value.previous[0]||hf_offer_value.previous[1])return hf_fail(HF_CHANGED); /* draft single-card boundary only */
 hf_ticket=(HfTicket){.abi=HF_ABI,.size=sizeof(HfTicket),.token=p->token,.policy=p->policy};
 if(!hf_unchanged()||memcmp(&p->snapshot,&hf_offer_value.observed,sizeof p->snapshot)||memcmp(&p->cursor,&hf_offer_value.cursor,sizeof p->cursor))return hf_fail(HF_CHANGED);
 int h=(int)p->snapshot.selection[0]-11,side=p->snapshot.side;
 if(h<0||h>=5||p->snapshot.selection[1]||p->snapshot.selection[5]||p->snapshot.selection[7]||
 p->destination!=p->snapshot.selection[6]||memcmp(&p->physical,&p->snapshot.hand[h],sizeof p->physical)||
 p->physical.id<=0||p->physical.id>722||p->physical.id!=p->snapshot.logical_id[h]||
 /* Native hand snapshots do NOT initialize deck_index; only the deck
  * tail does. The physical hand mapping, record and data pointer own it. */
 p->physical.slot!=11+h||
 p->physical.record<side*15||p->physical.record>=side*15+5||!p->physical.object||
 !(p->physical.flags&DUEL_CARD_FLAG_OCCUPIED)||p->physical.index<side*40||
 p->physical.index>=side*40+p->snapshot.draw[side]||p->physical.data!=(uint32_t)(uintptr_t)&gDuel_aDeckCardRecords[p->physical.index]||
 p->physical.id!=gDuel_aDeckCardRecords[p->physical.index].id||
 gDuel_aDeckCardRecords[p->physical.index].deck_index!=p->physical.index)return hf_fail(HF_IDENTITY);
 int category=Cards_Type(p->physical.id)<CARD_TYPE_MAGIC?HF_MONSTER:HF_SUPPORT;
 if(p->kind==HF_TIME_MAGIC){if(p->physical.id!=16||p->category!=HF_SUPPORT||p->destination<6||p->destination>10||p->snapshot.selection[8]!=0)return hf_fail(HF_IDENTITY);}
 else if(p->kind!=HF_NATIVE||p->category!=(unsigned)category||
 (category==HF_MONSTER?(p->destination<1||p->destination>5):(p->destination<6||p->destination>10)))return hf_fail(HF_IDENTITY);
 RhsResponse r;if(!hf_owner(&r))return hf_fail(HF_CHANGED);
 if(p->policy==HF_ALLOW_TW&&(hf_offer_value.mandatory||r.mandatory))return hf_fail(HF_MANDATORY);
 hf_proposal=*p;
 int id=hf_offer_value.previous[0]>=11&&hf_offer_value.previous[0]<=55?hf_offer_value.observed.logical_id[hf_offer_value.previous[0]-11]:0;
 if(id<=0||id>722)return hf_fail(HF_IDENTITY);
 hf_ticket.category=p->policy==HF_ALLOW_TW?p->category:Cards_Type(id)<CARD_TYPE_MAGIC?HF_MONSTER:HF_SUPPORT;
 hf_ticket.destination=p->policy==HF_ALLOW_TW?p->destination:hf_offer_value.previous[6];
 hf_ticket.count=1;hf_state=HF_PREPARED;hf_reason=0;*out=hf_ticket;return HF_OK;
}
static int hf_client_call(int commit){
 if(!hf_client)return hf_fail(HF_ACK_MISSING);
 HfTicket ticket=hf_ticket;HfProposal proposal=hf_proposal;
 hf_reentered=0;
 hf_busy=1;int ok=(commit?hf_client_copy.confirm:hf_client_copy.authorize)(&ticket,&proposal);hf_busy=0;
 /* Never lend mutable aliases to the internal transaction, even through
  * const callback arguments. Reject mutation of the borrowed copies too. */
 if(!ok||memcmp(&ticket,&hf_ticket,sizeof ticket)||memcmp(&proposal,&hf_proposal,sizeof proposal)||
 hf_reentered||!hf_unchanged())return hf_fail(HF_ACK_MISSING);
 return HF_OK;
}
static int hf_authorize(const HfTicket*t){
 if(hf_busy){hf_reentered=1;return hf_fail(HF_BUSY);}
 if(hf_state!=HF_PREPARED||!hf_same_ticket(t)||!hf_unchanged())return hf_fail(HF_STALE);
 if(hf_client_call(0)!=HF_OK){hf_state=HF_ABORTED;return hf_reason;}
 hf_state=HF_AUTHORIZED;return HF_OK;
}
static int hf_confirm(const HfTicket*t,HfStatus*out,uint32_t size){
 if(hf_busy){hf_reentered=1;return hf_fail(HF_BUSY);}
 if(!out||size!=sizeof*out)return hf_fail(HF_ABI_ERROR);
 if(hf_state!=HF_AUTHORIZED||!hf_same_ticket(t)||!hf_unchanged())return hf_fail(HF_STALE);
 RhsResponse response;if(!hf_owner(&response))return hf_fail(HF_CHANGED);
 if(hf_ticket.policy==HF_ALLOW_TW&&(hf_offer_value.mandatory||response.mandatory))return hf_fail(HF_MANDATORY);
 if(hf_client_call(1)!=HF_OK){hf_state=HF_ABORTED;return hf_reason;}
 /* No callbacks between final physical check and selection write. All card
  * records/deck/hand mappings stay untouched. Failure after a callback is a
  * veto, not restoration or physical rollback. */
 const uint8_t*selection=hf_ticket.policy==HF_KEEP_RD?hf_offer_value.previous:hf_proposal.snapshot.selection;
 memcpy((void*)D_800EAE88,selection,12);
 RhsRequest q={0};q.abi=RHS_ABI;q.size=sizeof q;
 hf_busy=1;hf_reentered=0;
 int valid=rhs_capture(&q.state,D_8009B1D5,t->token.action,t->token.generation)&&
 hf_same_world(&q.state,&hf_offer_value.observed)&&
 rd_selection_evaluate(rd_selection_rules,&q,&response)&&response.status!=RHS_CANCELLED&&response.status!=RHS_REPLACED&&
 !memcmp(response.revision,t->token.revision,sizeof response.revision)&&
 !(hf_ticket.policy==HF_ALLOW_TW&&response.mandatory)&&
 !memcmp(&deck_pull,&hf_pending_value,sizeof deck_pull)&&!hf_reentered;
 HfCursor cursor;hf_cursor(&cursor);valid=valid&&!memcmp(&cursor,&hf_offer_value.cursor,sizeof cursor);
 hf_busy=0;
 if(!valid){
 hf_state=HF_ABORTED;rd_selection_cancel_cause(RHS_CAUSE_PROTOCOL,"experimental finalization revalidation failed");return hf_fail(HF_CHANGED);}
 if(hf_ticket.policy==HF_ALLOW_TW){
 /* Revoke only the exact pre-exchange owned transaction after consent, not
  * arbitrary pending. The tuple was checked in all stages. No recover(). */
 deck_pull.active=0;
 }
 rd_selection_tx.request=q;rd_selection_tx.answer=response;
 hf_state=HF_CONFIRMED;hf_reason=0;hf_read(out,size);return HF_OK;
}
static int hf_cancel(const HfTicket*t){
 if(hf_busy){hf_reentered=1;return hf_fail(HF_BUSY);}
 if(!hf_same_ticket(t))return hf_fail(HF_STALE);
 if(hf_state>=HF_CONFIRMED&&hf_state<=HF_CONSUMED)return hf_fail(HF_ALREADY_SPENT);
 if(!hf_stamp_live(&t->token))return hf_fail(HF_STALE);
 hf_state=HF_ABORTED;return hf_fail(HF_CANCELLED);
}
static void hf_materialize(void){
 if(hf_state>=HF_PREPARED&&hf_ticket.token.action==rd_selection_serial&&hf_ticket.token.generation==rd_selection_generation){
  if(hf_state!=HF_CONFIRMED||!hf_stamp_live(&hf_ticket.token)||!rhs_live(&rd_selection_tx.request.state)){
   rd_selection_cancel_cause(RHS_CAUSE_PROTOCOL,"experimental handoff lacks final ACK or already spent");return;}
  hf_state=HF_MATERIALIZED; /* reserve once, before any materializer */
 }
 rd_materialize();
}
static void hf_hand_tick(void){
 int watching=hf_state==HF_MATERIALIZED&&hf_ticket.token.action==rd_selection_serial&&hf_ticket.token.generation==rd_selection_generation;
 int before=rd_selection_hand_count(D_8009B1D5);
 /* The existing RD monitor guards the hand cursor/stack and physical refs.
  * It is not bypassed by this companion. */
 rd_selection_hand_tick();
 int after=rd_selection_hand_count(D_8009B1D5);
 if(watching&&after<before){hf_consumed+=(unsigned)(before-after);hf_state=HF_CONSUMED;}
}
static const HfFinalization hf_api={HF_ABI,sizeof(HfFinalization),HF_MAGIC,
 hf_connect,hf_disconnect,hf_offer,hf_prepare,hf_authorize,hf_confirm,hf_cancel,hf_read};
