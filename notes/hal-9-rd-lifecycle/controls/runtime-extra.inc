/* Owner tests: real planners/providers and Main_RunDuel. IO/frontend/renderer
 * remain modeled. No injection of pending, plans or quota values. */
static int od_case,od_second,od_pause,od_fault_done,od_reentrant_done;
static const HfFinalization*od_base;static HfFinalization od_proxy;
static HfTicket od_old_ticket;static HfProposal od_old_proposal;static HfClient od_old_client,od_saved_client;
static unsigned od_u(const char*n){unsigned*p=local(RD,n);return p?*p:0;}
static int od_tw(void){return mi(T)>=0&&Mods_Active(mi(T));}
static int od_rd(void){return mi(RD)>=0&&Mods_Active(mi(RD));}
static int rd_owner_pause_controller(void){return od_case==304&&!od_second&&od_pause;}
static void rd_owner_native_init(void){
 if(od_case==313&&od_second){puts("INSTRUMENTED attempted planning inside owner InitScene");((int(*)(void))engine_function("AiScript_Run"))();}
}
static void rd_owner_after_planner(void){
 if(od_case<300||od_second)return;
 if(od_case==300||od_case==310){
  if(od_case==310)((uint8_t*)datum("D_800EAE88"))[0]=11;
 }
 if(od_case==301&&!od_fault_done){DuelSelectionRecord*c=(DuelSelectionRecord*)((char*)datum("D_800E9F10")+DUEL_SELECTION_SIDE_SIZE);c->field_0E=(c->field_0E+1)%5;od_fault_done=1;puts("INSTRUMENTED post-ACK cursor before physical collection");}
 if(od_case==304){od_pause=1;puts("INSTRUMENTED observer pauses real prepared planner result before native swap; pending not written");}
}
static void rd_owner_controller_fault(void){
 if(od_case<300||od_second||od_fault_done||!od_rd())return;
 if((od_case==302||od_case==303)&&recovery().state==RHS_STATE_HANDOFF&&(*(uint16_t*)datum("D_8009B20C")&15)>=2){
  od_fault_done=1;
  if(od_case==302){DuelSelectionRecord*c=(DuelSelectionRecord*)((char*)datum("D_800E9F10")+DUEL_SELECTION_SIDE_SIZE);c->field_0E=(c->field_0E+1)%5;}
  else{DuelCardRecord*r=datum("D_801A7AD8");r[15].data=&((DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords"))[41];}
  puts("INSTRUMENTED late cursor/physical reference fault; no repair");
 }
}
static int rd_owner_break_hand(void){
 if(od_case<300)return 0;
 if(od_pause&&!od_second)return 1;
 if(od_rd()){
  RhsRecovery s=recovery();if(s.state==RHS_STATE_BLOCKED||s.state==RHS_STATE_QUARANTINE||s.state==RHS_STATE_EXHAUSTED)return 1;
 }
 return !od_second&&od_tw()&&*(unsigned*)tw_symbol("tw_final_block");
}
static void od_board(int second){
 od_second=second;od_pause=0;recovery_new_duel_only=second;setup(5);recovery_new_duel_only=0;
 if(second){puts("MODELED phase-entry dispatch: actual owner Populate callbacks; native deck IO remains fixture");((void(*)(void))engine_function("Duel_PopulateCombinedDeckData"))();}
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");
 for(int i=0;i<2;i++){d[40+i].id=16;r[15+i].card_id=16;r[15+i].attack=500;r[15+i].defense=400;((DuelCardDisplayObject*)r[15+i].object)->field_68=CARD_TYPE_SPELLCASTER;}
 set_stat(16,500,400,CARD_TYPE_SPELLCASTER);set_stat(22,2500,1200,CARD_TYPE_FIEND);for(int i=0;i<3;i++)put_field(5+i,22,i);
 if(od_case==309)queen_setup();setup_count=spawn_count=0;
}
static int od_missing_close(const HfClient*c){(void)c;puts("INSTRUMENTED disconnect ACK absent; callbacks remain resident");return HF_BUSY;}
static int od_reentrant_authorize(const HfTicket*t,const HfProposal*p){
 int ok=od_saved_client.authorize(t,p);
 if(ok&&!od_reentrant_done){od_reentrant_done=1;*(uint8_t*)rec_main_symbol("D_8009B26C")=0x40;*(uint8_t*)rec_main_symbol("D_8009B26E")=1;puts("INSTRUMENTED Main_RunDuel requested inside real HF authorize callback");((void(*)(void))engine_function("Main_RunDuel"))();}
 return ok;
}
static int od_connect(const HfClient*c){
 od_saved_client=*c;int rc=od_base->connect(c);if(rc==HF_OK&&od_case==312){HfClient*copy=local(RD,"hf_client_copy");copy->authorize=od_reentrant_authorize;}return rc;
}
static int od_replay_authorize(const HfTicket*t){
 unsigned before=*(unsigned*)tw_symbol("tw_hf_authorizations");
 int a=od_old_client.authorize?od_old_client.authorize(&od_old_ticket,&od_old_proposal):0;
 int b=od_old_client.confirm?od_old_client.confirm(&od_old_ticket,&od_old_proposal):0;
 result("old-client-callbacks-during-new-owner-transaction",!a&&!b&&*(unsigned*)tw_symbol("tw_hf_authorizations")==before,"Fresh busy context still rejects old tokens/generation/connection; status is not authority.");
 return od_base->authorize(t);
}
static void od_same_duel_controls(void){
 RhsRecovery before=recovery();int pending=pull()->active,hand=hand_count();DuelDeckCardRecord deck[80];memcpy(deck,datum("gDuel_aDeckCardRecords"),sizeof deck);
 *(uint8_t*)rec_main_symbol("D_8009B26C")=0x40;*(uint8_t*)rec_main_symbol("D_8009B26E")=1;
 ((void(*)(void))engine_function("Duel_InitScene"))();
 RhsRecovery a=recovery();
 result("unscoped-InitScene-does-not-invalidate-owner",a.generation==before.generation&&a.connection_epoch==before.connection_epoch&&a.action==before.action&&a.state==before.state&&pull()->active==pending&&hand_count()==hand&&!memcmp(deck,datum("gDuel_aDeckCardRecords"),sizeof deck),"Even init-like globals do not replace the delegated Main_RunDuel call scope. No pending clear or physical write.");
 uint16_t flags=*(uint16_t*)datum("gDuel_wSceneStateFlags");*(uint16_t*)datum("gDuel_wSceneStateFlags")=5;Settings_SetNamed("test.owner-setting-probe",1);*(uint16_t*)datum("gDuel_wSceneStateFlags")=flags;
 uint16_t cards[40];uint8_t order[40];((void(*)(int,uint8_t*,uint8_t*))engine_function("Duel_ShuffleDeck"))(1,(uint8_t*)cards,order);
 a=recovery();result("scene-settings-shuffle-are-not-new-duel-proof",a.generation==before.generation&&a.state==before.state&&a.connection_epoch==before.connection_epoch&&pull()->active==pending,"Existing player-deck shuffle delegates; scene/settings alone do not authorize recovery.");
}
static void od_end_current(void){
 RhsRecovery before={0};if(od_rd())before=recovery();int n=hand_count(),pending=od_rd()?pull()->active:0;
 unsigned block=od_tw()?*(unsigned*)tw_symbol("tw_final_block"):0;
 *(uint8_t*)rec_main_symbol("D_8009B26C")=0x40;*(uint8_t*)rec_main_symbol("D_8009B26E")=0x81;
 *(uint16_t*)datum("D_8009B16C")|=0x2000;
 ((void(*)(void))engine_function("Main_RunDuel"))();unsigned stage=*(uint8_t*)rec_main_symbol("D_8009B26E");
 ((void(*)(void))engine_function("Main_RunDuel"))();RhsRecovery after={0};if(od_rd())after=recovery();
 result("end-controller-does-not-recover-owner",stage==2&&*(uint8_t*)datum("D_8009B0A3")==6&&n==hand_count()&&(!od_rd()||(before.generation==after.generation&&before.state==after.state&&pull()->active==pending))&&(!od_tw()||*(unsigned*)tw_symbol("tw_final_block")==block),"Actual Main cases1/2 over explicit fixture end flag, IO/frontend services modeled. End alone never revives an action.");
 *(uint16_t*)datum("D_8009B16C")&=~0x2000;
}
static void rd_owner_cases(int mode){
 od_case=mode;od_second=od_pause=od_fault_done=od_reentrant_done=0;rec_mode=rec_second=rec_projection_done=0;
 od_board(0);begin("RD-owned-duel-boundary");
 if(mode==307||mode==308||mode==314)client_fault_setup(109);
 if(mode==312){od_base=Mods_Find(HF_KEY);CHECK(od_base);od_proxy=*od_base;od_proxy.connect=od_connect;CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",&od_proxy));}
 int p=hand_action(),old_hand=hand_count();RhsRecovery old={0};if(od_rd())old=recovery();
 const HfFinalization*api=Mods_Find(HF_KEY);HfStatus hf={0};if(api)CHECK(api->read(&hf,sizeof hf)==HF_OK);
 memset(&od_old_ticket,0,sizeof od_old_ticket);memset(&od_old_proposal,0,sizeof od_old_proposal);memset(&od_old_client,0,sizeof od_old_client);
 if(od_tw()){od_old_ticket=*(HfTicket*)tw_symbol("tw_hf_ticket");od_old_proposal=*(HfProposal*)tw_symbol("tw_hf_proposal");od_old_client=*(HfClient*)tw_symbol("tw_hf_client");}
 printf("OWNER_FIRST mode=%d phase=%d hand=%d RD=%u generation=%u connection=%u serial=%u pending=%d hf=%u consumed=%u\n",mode,p,old_hand,old.state,old.generation,old.connection_epoch,old.action,od_rd()?pull()->active:0,hf.state,hf.consumed);
 int consumed=mode==305||mode==306||mode==309||mode==311||mode==313||mode==315||mode==316||mode==317||mode==318||mode==319||mode==320;
 result("first-owner-action-boundary",consumed?((p==6||p==7)&&old_hand==4):(p==4&&old_hand==5),"Real planner, producer, materializer and available native controller; adversarial faults are explicit.");
 if(mode==300||mode==310)result("real-owner-BLOCKED-reproduced",old.state==RHS_STATE_BLOCKED,"Bounded real snapshot divergences, no pending injection.");
 if(mode==301)result("real-owner-HANDOFF-reproduced",old.state==RHS_STATE_HANDOFF&&hf.state==HF_CONFIRMED&&!hf.consumed,"Confirmed handoff, no physical collection.");
 if(mode==302||mode==303)result("real-owner-QUARANTINE-reproduced",old.state==RHS_STATE_QUARANTINE&&old_hand==5,"Real hand monitor detects cursor/identity fault before physical collection.");
 if(mode==304)result("real-pending-preserved-before-consumption",pull()->active&&setup_count==0&&old.state==RHS_STATE_VALIDATED,"Observer paused real prepared result; no fabricated pending or materials.");
 if(mode==300||mode==304||mode==310)od_same_duel_controls();
 if(mode==314){int n=hand_count();CHECK(api->disconnect((HfClient*)tw_symbol("tw_hf_client"))==HF_OK);result("disconnect-alone-does-not-authorize",hand_count()==n&&*(unsigned*)tw_symbol("tw_final_block"),"Real ACK closes the exact client, never clears RD pending or resumes its action.");client_mode=0;puts("INSTRUMENTED no-ACK fault removed only for a newly connected client after real disconnect ACK");}
 if(mode==307){od_proxy=*client_real;CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",&od_proxy));puts("INSTRUMENTED provider table replaced before closing ACK");}
 if(mode==308){client_proxy.disconnect=od_missing_close;}
 if(mode==315){*(unsigned*)local(RD,"rd_selection_generation")=UINT32_MAX;*(unsigned*)local(RD,"rd_selection_serial")=UINT32_MAX;*(unsigned*)local(RD,"hf_nonce")=UINT32_MAX;puts("INSTRUMENTED allocator exhaustion metadata only, not pending/plan/quota");}
 if(mode==316){*(unsigned*)local(RD,"rd_selection_connection")=UINT32_MAX;*(unsigned*)local(RD,"rd_selection_interrupt")=UINT32_MAX;*(unsigned*)local(RD,"rd_selection_sequence")=UINT32_MAX;puts("INSTRUMENTED owner connection/interrupt/sequence exhaustion only");}
 if(mode==317){*(unsigned*)local(RD,"hf_nonce")=UINT32_MAX;*(unsigned*)local(RD,"hf_client_epoch")=UINT32_MAX;puts("INSTRUMENTED HF allocator exhaustion only");}
 if(mode==318){*(unsigned*)local(RD,"rd_duel_epoch")=UINT32_MAX;puts("INSTRUMENTED private duel-epoch exhaustion only");}
 od_end_current();unsigned resets=Hal9_ResetCount();od_board(1);RhsRecovery next={0};if(od_rd())next=recovery();
 printf("OWNER_NEW mode=%d reset_delta=%u RD=%u generation=%u connection=%u serial=%u pending=%d monitor=%u safe=%u attempts=%u owner_epoch=%u TW_block=%u\n",mode,Hal9_ResetCount()-resets,next.state,next.generation,next.connection_epoch,next.action,od_rd()?pull()->active:0,od_rd()?od_u("rd_selection_monitor"):0,od_rd()?od_u("rd_selection_safe_valid"):0,next.attempts,od_rd()?od_u("rd_duel_epoch"):0,od_tw()?*(unsigned*)tw_symbol("tw_final_block"):0);
 result("actual-Main-entry-no-ModulesReset",Hal9_ResetCount()==resets&&rec_main_init_delta==1,"Actual Main_RunDuel case1 and owner callbacks; InitScene IO/frontend/renderer remain modeled.");
 if(mode==315){result("exhaustion-not-reset-at-new-duel",next.state==RHS_STATE_EXHAUSTED&&next.serial_exhausted&&od_u("rd_selection_generation")==UINT32_MAX&&od_u("rd_selection_serial")==UINT32_MAX&&od_u("hf_nonce")==UINT32_MAX,"No allocator wraps or regains permission. New duel does not reset exhaustion.");puts("SAFE_REFUSAL exhausted owner requires process lifecycle");return;}
 if(mode==316||mode==318){result("all-owner-allocator-exhaustion-refused",next.state==RHS_STATE_EXHAUSTED&&next.serial_exhausted&&(mode==318?od_u("rd_duel_epoch")==UINT32_MAX:(od_u("rd_selection_connection")==UINT32_MAX&&od_u("rd_selection_interrupt")==UINT32_MAX&&od_u("rd_selection_sequence")==UINT32_MAX)),"All owner counters saturate; no new-duel reset or recycled permission.");puts("SAFE_REFUSAL owner allocator exhausted");return;}
 if(mode==317){p=hand_action();result("HF-allocator-exhaustion-no-new-consumption",p==4&&hand_count()==5&&od_u("hf_nonce")==UINT32_MAX&&od_u("hf_client_epoch")==UINT32_MAX,"Owner-local cleanup is not full composition recovery. Exhausted companion cannot reconnect or authorize a fresh ticket.");puts("SAFE_REFUSAL companion allocator exhausted");return;}
 if(mode==313){result("planning-during-init-refused",next.state==RHS_STATE_QUARANTINE&&hand_count()==5,"Initialization cannot run a planner or materializer through the owner; reentrancy is not a normal UI flow.");puts("SAFE_REFUSAL initialization interrupted by callback");return;}
 if(od_rd())result("new-duel-invalidates-RD-context",next.state==RHS_STATE_IDLE&&next.generation>old.generation&&next.connection_epoch>old.connection_epoch&&next.action==old.action&&!pull()->active&&!od_u("rd_selection_monitor")&&!od_u("rd_selection_safe_valid")&&!next.attempts,"Owner invalidates its own references before planning; serial never reset; no recover or physical rollback.");
 int hand=hand_count();unsigned swaps=setup_count;
 if(api){HfStatus z;const HfFinalization*live=Mods_Find(HF_KEY);int a=live->authorize(&od_old_ticket),b=live->confirm(&od_old_ticket,&z,sizeof z);result("old-companion-ticket-refused",a!=HF_OK&&b!=HF_OK&&hand_count()==hand&&setup_count==swaps,"Old generation/connection/client/action cannot cross the proven boundary.");}
 if(od_tw()){int a=od_old_client.authorize?od_old_client.authorize(&od_old_ticket,&od_old_proposal):0,b=od_old_client.confirm?od_old_client.confirm(&od_old_ticket,&od_old_proposal):0;result("old-TW-callbacks-refused",!a&&!b&&hand_count()==hand,"TW owns its own closure/context; RD never clears its internals.");}
 if(mode==307||mode==308){result("composition-refused-without-TW-closure-ACK",od_tw()&&*(unsigned*)tw_symbol("tw_final_block")&&hand_count()==5,"RD context can be clean while TW closure remains refused. No fake ACK or dropped callback table.");puts("SAFE_REFUSAL each owner must complete its own lifecycle");return;}
 if(client_real&&(mode==307||mode==308))CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",(void*)client_real));
 if(od_tw()&&od_rd()){
  const HfFinalization*live=Mods_Find(HF_KEY);if(live!=&od_proxy)od_base=live;od_proxy=*od_base;od_proxy.authorize=od_replay_authorize;
  CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",&od_proxy));
 }
 p=hand_action();RhsRecovery final={0};if(od_rd())final=recovery();
 if(mode==300){result("OBSERVE-new-refusal-is-fresh-action",p==4&&hand_count()==5&&final.state==RHS_STATE_BLOCKED&&final.generation>old.generation&&final.action>old.action,"Owner was reset at the boundary, then the unchanged OBSERVE workload creates new bounded conflicts. No priority decision is promoted.");puts("SAFE_REFUSAL fresh OBSERVE conflict remains a functional policy question");return;}
 result("new-duel-real-fresh-physical-action",(p==6||p==7)&&hand_count()==4&&conserved()&&(!od_rd()||final.action>old.action),"Fresh owner planning and one collection in a NEW board, not revival or restoration of old cards.");
 if(od_tw()&&od_rd()){HfTicket fresh=*(HfTicket*)tw_symbol("tw_hf_ticket");result("new-handoff-full-context",fresh.token.generation>od_old_ticket.token.generation&&fresh.token.connection>od_old_ticket.token.connection&&fresh.token.action>od_old_ticket.token.action&&fresh.token.nonce>od_old_proposal.token.nonce,"Actual companion tokens change, in addition to local TW cleanup.");}
 if(Mods_Active(mi(D)))result("new-owner-Dual-quota",ds()[1]==(mode==309?1:2),"Actual unchanged private Dual gate and final category ledger; no quota injection.");
 if(mode==309)result("Queen-still-mandatory",action_card()==395&&((uint8_t*)datum("D_800EAE88"))[8]==1,"Real producer keeps physical Female and position; lifecycle never chooses another priority.");
 if(mode==319){resolve_model(p);support_resolution();support_bonus();}
 printf("OWNER_FINAL mode=%d phase=%d hand=%d RD=%u generation=%u connection=%u serial=%u\n",mode,p,hand_count(),final.state,final.generation,final.connection_epoch,final.action);
}
