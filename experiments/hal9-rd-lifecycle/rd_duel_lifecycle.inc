/* EXPERIMENTAL owner-only boundary; included AFTER the unchanged PR34
 * companion in a private copy of RD. No public ABI or strategy changes. */
enum {RD_DUEL_COLD=0,RD_DUEL_TRANSITION,RD_DUEL_LOCAL_CLEAN,RD_DUEL_REFUSED};
static void *rd_duel_previous_main,*rd_duel_previous_init;
static const u8 *rd_duel_main_flags,*rd_duel_main_state;
static unsigned rd_duel_main_depth,rd_duel_permit,rd_duel_initializing,rd_duel_latched;
static unsigned rd_duel_epoch,rd_duel_unscoped,rd_duel_reentries;
static volatile unsigned rd_duel_state,rd_duel_last_state,rd_duel_last_hf_state,rd_duel_last_hf_consumed;
static int rd_duel_reason;
static int rd_duel_busy(void)
{return rd_duel_initializing||rd_selection_planning||rd_selection_reentrant||hf_busy;}
static void rd_duel_log(const char*event){
 if(host&&host->log)host->log(host,
  "RD_DUEL event=%s local=%u reason=%d epoch=%u generation=%u connection=%u serial=%u interrupt=%u pending=%d monitor=%d exhausted=%d",
  event,rd_duel_state,rd_duel_reason,rd_duel_epoch,rd_selection_generation,
  rd_selection_connection,rd_selection_serial,rd_selection_interrupt,deck_pull.active,
  rd_selection_monitor,rd_selection_exhausted);
}
static void rd_duel_interrupt(void){
 ++rd_duel_reentries;rd_duel_latched=1;rd_duel_state=RD_DUEL_REFUSED;rd_duel_reason=HF_BUSY;
 rd_selection_bump(&rd_selection_interrupt);
 if(hf_busy)hf_reentered=1; /* existing protocol rejects the outer callback */
 rd_duel_log("busy-entry-refused");
}
static void rd_duel_main(void){
 int entry=rd_duel_main_flags&&rd_duel_main_state&&
  (*rd_duel_main_flags&0x40)&&*rd_duel_main_state==1;
 if(rd_duel_main_depth||rd_duel_busy()){rd_duel_interrupt();return;}
 ++rd_duel_main_depth;
 /* The permit exists only during this delegated call. The core must take
  * its uninitialized case1 edge and consume it at InitScene exactly once. */
 rd_duel_permit=entry?1:0;
 ((void(*)(void))rd_duel_previous_main)();
 rd_duel_permit=0;--rd_duel_main_depth;
}
static void rd_duel_invalidate_owner(void){
 rd_duel_last_state=rd_selection_state;
 rd_duel_last_hf_state=hf_state;rd_duel_last_hf_consumed=hf_consumed;
 /* An exhausted allocator stays exhausted; no action identity is recycled. */
 if(rd_selection_serial==UINT32_MAX||rd_selection_generation==UINT32_MAX||
    rd_selection_connection==UINT32_MAX||rd_selection_interrupt==UINT32_MAX||
    rd_selection_sequence==UINT32_MAX||rd_duel_epoch==UINT32_MAX)rd_selection_exhausted=1;
 if(rd_duel_epoch!=UINT32_MAX)++rd_duel_epoch;
 rd_selection_bump(&rd_selection_generation);rd_selection_bump(&rd_selection_connection);
 rd_selection_bump(&rd_selection_interrupt);
 rd_selection_tx.state=0;rd_selection_tx.producer=0;
 memset(&rd_selection_tx.request,0,sizeof rd_selection_tx.request);
 memset(&rd_selection_tx.answer,0,sizeof rd_selection_tx.answer);
 rd_selection_monitor=rd_selection_safe_valid=rd_selection_quarantine=0;
 memset(&rd_selection_safe,0,sizeof rd_selection_safe);
 memset(rd_selection_sources,0,sizeof rd_selection_sources);
 rd_selection_attempts=0;rd_selection_outcome=0;rd_selection_last_provider_status=RHS_INVALID;
 rd_selection_last_cause=0;
 /* Only at the proven boundary: no physical card/deck/hand writes, no
  * reversal of an already executed exchange or consumption. */
 memset(&deck_pull,0,sizeof deck_pull);legal_cache.valid=plan_cache.valid=0;
 if(hf_state!=HF_CONSUMED)hf_state=HF_ABORTED;
 hf_reason=HF_STALE;
 memset(&hf_offer_value,0,sizeof hf_offer_value);
 memset(&hf_proposal,0,sizeof hf_proposal);memset(&hf_pending_value,0,sizeof hf_pending_value);
 /* Keep ticket/consumed history and every allocator. Do not disconnect a
  * client or drop its copied callback table without its protocol ACK. */
 rd_selection_mark(rd_selection_exhausted?RHS_STATE_EXHAUSTED:RHS_STATE_IDLE,
  rd_selection_exhausted?RHS_CAUSE_COUNTER:RHS_CAUSE_NONE);
}
static void rd_duel_init(void){
 if(!rd_duel_permit||rd_duel_main_depth!=1||!rd_duel_main_state||*rd_duel_main_state!=0x81){
  ++rd_duel_unscoped;rd_duel_reason=HF_STALE;rd_duel_log("unproven-init-refused");return;
 }
 if(rd_duel_busy()){rd_duel_interrupt();return;}
 rd_duel_permit=0;rd_duel_initializing=1;rd_duel_latched=0;rd_duel_state=RD_DUEL_TRANSITION;
 rd_duel_invalidate_owner();
 ((void(*)(void))rd_duel_previous_init)();
 rd_duel_initializing=0;
 if(rd_duel_latched){
  rd_selection_quarantine=1;rd_selection_mark(RHS_STATE_QUARANTINE,RHS_CAUSE_REENTRANCY);
  rd_duel_state=RD_DUEL_REFUSED;rd_duel_reason=HF_BUSY;
 }else if(rd_selection_exhausted){rd_duel_state=RD_DUEL_REFUSED;rd_duel_reason=HF_EXHAUSTED;}
 else{rd_duel_state=RD_DUEL_LOCAL_CLEAN;rd_duel_reason=HF_OK;}
 rd_duel_log("proven-init-complete");
}
static s32 rd_duel_ai(void){
 if(rd_duel_initializing){rd_duel_interrupt();return 0;}
 if(rd_duel_latched)return 0;
 return rd_ai();
}
static void rd_duel_materialize(void){
 if(rd_duel_initializing){rd_duel_interrupt();return;}
 if(rd_duel_latched)return;
 hf_materialize();
}
static void rd_duel_hand_tick(void){
 if(rd_duel_initializing){rd_duel_interrupt();return;}
 if(rd_duel_latched)return;
 hf_hand_tick();
}
static int rd_duel_register(void){
 void *main=host->symbol(host,"Main_RunDuel"),*init=host->symbol(host,"Duel_InitScene");
 rd_duel_main_flags=host->symbol(host,"D_8009B26C");
 rd_duel_main_state=host->symbol(host,"D_8009B26E");
 if(!main||!init||!rd_duel_main_flags||!rd_duel_main_state)return 0;
 int token=host->hook(host,main,rd_duel_main,&rd_duel_previous_main);
 if(!token)return 0;
 if(!host->hook(host,init,rd_duel_init,&rd_duel_previous_init)){
  host->unhook(host,token);return 0;
 }
 return 1;
}
