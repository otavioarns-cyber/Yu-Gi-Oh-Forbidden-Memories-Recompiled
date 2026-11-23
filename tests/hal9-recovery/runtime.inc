/* New-duel board is a NEW fixture, not rollback. Owner BSS/pending are never
 * written here. Second initialization goes through real Main_RunDuel case1,
 * real hook callbacks and a modeled asset-dependent Duel_InitScene body. */
static int rec_mode,rec_second,rec_fault_once,rec_projection_done;
static unsigned rec_main_init_delta;
static HfTicket rec_old_ticket;static HfProposal rec_old_proposal;static HfClient rec_old_client;
static HfFinalization rec_replay_proxy;static const HfFinalization *rec_replay_base;
static int rec_replay_authorize(const HfTicket*t){
 unsigned count=*(unsigned*)tw_symbol("tw_hf_authorizations");
 int a=rec_old_client.authorize?rec_old_client.authorize(&rec_old_ticket,&rec_old_proposal):0;
 int b=rec_old_client.confirm?rec_old_client.confirm(&rec_old_ticket,&rec_old_proposal):0;
 result("old-callback-during-fresh-negotiation-refused",!a&&!b&&*(unsigned*)tw_symbol("tw_hf_authorizations")==count,"Busy new negotiation does not make old packets valid; exact token/context must match.");
 return rec_replay_base->authorize(t);
}
unsigned Hal9_ResetCount(void); /* implemented below via test bridge */
static void *rec_main_symbol(const char*n){
 void*p=0;for(int i=0;i<nn;i++){void*q=ObjectLoader_Symbol(&native_objects[i],n);if(q)p=q;}return p?p:datum(n);
}
static void recovery_main_init(void){
 unsigned before=ecalls("Duel_InitScene");*(uint8_t*)rec_main_symbol("D_8009B26C")=0x40;
 *(uint8_t*)rec_main_symbol("D_8009B26E")=1;
 ((void(*)(void))engine_function("Main_RunDuel"))();
 rec_main_init_delta=ecalls("Duel_InitScene")-before;
 printf("RECOVERY_MAIN case=1 flag=%u init_calls=%u modeled_init_body=1\n",*(uint8_t*)rec_main_symbol("D_8009B26E"),rec_main_init_delta);
}
static void recovery_end_scene(void){
 unsigned block=*(unsigned*)tw_symbol("tw_final_block");int hand=hand_count();
 *(uint8_t*)rec_main_symbol("D_8009B26C")=0x40;
 *(uint8_t*)rec_main_symbol("D_8009B26E")=0x81;
 *(uint16_t*)datum("D_8009B16C")|=0x2000;
 ((void(*)(void))engine_function("Main_RunDuel"))();
 unsigned stage=*(uint8_t*)rec_main_symbol("D_8009B26E");
 ((void(*)(void))engine_function("Main_RunDuel"))();
 printf("RECOVERY_END stage=%u block=%u hand=%d scene=%u synthetic_end_flag=1 modeled_services=1\n",stage,*(unsigned*)tw_symbol("tw_final_block"),hand_count(),*(uint8_t*)datum("D_8009B0A3"));
 result("duel-end-is-not-local-recovery",stage==2&&*(uint8_t*)datum("D_8009B0A3")==6&&*(unsigned*)tw_symbol("tw_final_block")==block&&hand_count()==hand,"Official case1 end flag and case2 execute; IO/frontend services are modeled. End does not authorize an old action.");
}
static void recovery_board(int first){
 rec_second=!first;recovery_new_duel_only=!first;setup(5);recovery_new_duel_only=0;
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");
 d[40].id=16;r[15].card_id=16;set_stat(16,500,400,CARD_TYPE_SPELLCASTER);r[15].attack=500;r[15].defense=400;((DuelCardDisplayObject*)r[15].object)->field_68=CARD_TYPE_SPELLCASTER;
 if(rec_mode<222||rec_mode>225){set_stat(22,2500,1200,CARD_TYPE_FIEND);for(int i=0;i<3;i++)put_field(5+i,22,i);}
 if(rec_mode>=222&&rec_mode<=225){d[41].id=16;r[16].card_id=16;r[16].attack=500;r[16].defense=400;((DuelCardDisplayObject*)r[16].object)->field_68=CARD_TYPE_SPELLCASTER;}
 if(first&&rec_mode==209)queen_setup();setup_count=spawn_count=0;
}
static unsigned rec_block(void){return *(unsigned*)tw_symbol("tw_final_block");}
static int recovery_break_hand(void){
 if(rec_mode<200||rec_mode>=220)return rec_projection_done;
 if(!rec_second&&rec_block())return 1;
 if(!rec_second&&rec_mode==206&&recovery().state==RHS_STATE_BLOCKED)return 1;
 return 0;
}
static int rec_init_authorize(const HfTicket*t){
 int rc=client_real->authorize(t);if(rc==HF_OK){puts("INSTRUMENTED reentrant InitScene during authorize; not a normal UI path");((void(*)(void))engine_function("Duel_InitScene"))();}return rc;
}
static void recovery_after_planner(void){
 if(rec_mode==202||rec_mode==207){if(!rec_second&&!rec_fault_once){
  rec_fault_once=1;DuelSelectionRecord*c=(DuelSelectionRecord*)((char*)datum("D_800E9F10")+DUEL_SELECTION_SIDE_SIZE);c->field_0E=(c->field_0E+1)%5;puts("INSTRUMENTED real-client post-ACK cursor veto");}}
}
#include "projection.inc"
static void recovery_cases(int mode){
 if(mode>=220){projection_cases(mode);return;}
 rec_mode=mode;rec_second=rec_fault_once=rec_projection_done=0;recovery_board(1);begin("TW-recovery-real-client");
 if(mode==200||mode==212||mode==214)client_fault_setup(109);if(mode==201)client_fault_setup(114);
 if(mode==203)client_fault_setup(111);if(mode==204)client_fault_setup(125);
 if(mode==205)client_fault_setup(123);if(mode==207)Mods_SetEnabled(mi(T),0);
 if(mode==210)client_fault_setup(116);
 if(mode==211){client_real=Mods_Find(HF_KEY);CHECK(client_real);client_proxy=*client_real;client_proxy.authorize=rec_init_authorize;CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",&client_proxy));}
 int p=hand_action(),old_hand=hand_count();
 HfTicket old=*(HfTicket*)tw_symbol("tw_hf_ticket");HfProposal proposal=*(HfProposal*)tw_symbol("tw_hf_proposal");HfClient old_client=*(HfClient*)tw_symbol("tw_hf_client");
 const HfFinalization*api=Mods_Find(HF_KEY);HfStatus status={0};CHECK(api&&api->read(&status,sizeof status)==HF_OK);
 RhsRecovery before=recovery();unsigned blocked=rec_block();
 printf("RECOVERY_FIRST mode=%d phase=%d hand=%d block=%u armed=%u gate=%u hf=%u consumed=%u RD=%u action=%u generation=%u client=%u\n",mode,p,old_hand,blocked,*(unsigned*)tw_symbol("tw_final_armed"),*(unsigned*)tw_symbol("tw_final_gate"),status.state,status.consumed,before.state,old.token.action,old.token.generation,old.token.client);
 if(mode==208||mode==209)result("first-action-actually-consumed",(p==6||p==7)&&old_hand==4&&status.consumed==1,"New duel starts during presentation; spent old card never restored in the old board.");
 else result("first-invalid-transaction-refused",p==4&&old_hand==5&&!status.consumed,"Real planner/companion; fault affects metadata or callback timing, never pending or quota injection.");
 if(mode==204)Hal25_ForceApplied(mi(RD),1);if(mode==205)Hal25_ForceApplied(mi(T),1);
 if(mode==200){
  int closed=api->disconnect((HfClient*)tw_symbol("tw_hf_client"))==HF_OK;int hand=hand_count();
  for(int i=0;i<3;i++)((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();
  result("disconnect-ACK-is-not-action-recovery",closed&&rec_block()&&hand_count()==hand,"Cooperative close alone cannot revive an invalidated action or erase its local fence.");
 }
 if(mode==212){CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",(void*)client_real));puts("INSTRUMENTED provider table changed before closure ACK");}
 if(mode==214)recovery_end_scene();
 unsigned resets=Hal9_ResetCount();unsigned client_epoch=*(unsigned*)local(RD,"hf_client_epoch");
 recovery_board(0);
 if(client_real&&mode!=206)CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",(void*)client_real));
 RhsRecovery next=recovery();unsigned now_block=rec_block();
 printf("RECOVERY_NEW mode=%d reset_delta=%u block=%u armed=%u gate=%u RD=%u old_generation=%u generation=%u old_client=%u client=%u pending=%d\n",mode,Hal9_ResetCount()-resets,now_block,*(unsigned*)tw_symbol("tw_final_armed"),*(unsigned*)tw_symbol("tw_final_gate"),next.state,before.generation,next.generation,client_epoch,*(unsigned*)local(RD,"hf_client_epoch"),pull()->active);
 result("new-duel-without-ModulesReset",Hal9_ResetCount()==resets&&rec_main_init_delta==1,"Actual official Main_RunDuel case1 invokes hooked InitScene; asset IO/board creation remains modeled.");
 int n=hand_count();unsigned swaps=setup_count;
 int a=old_client.authorize?old_client.authorize(&old,&proposal):0,b=old_client.confirm?old_client.confirm(&old,&proposal):0;
 const HfFinalization*current=Mods_Find(HF_KEY);
 int stale=current->authorize(&old);int confirm=current->confirm(&old,&status,sizeof status);
 result("old-client-callbacks-never-authorize",a==0&&b==0&&hand_count()==n&&setup_count==swaps,"Callbacks from old action are not an execution capability.");
 result("old-ticket-cannot-continue",stale!=HF_OK&&confirm!=HF_OK&&hand_count()==n&&setup_count==swaps,"Same physical IDs/addresses may be reused in the NEW board; client epoch/action validity must reject old ticket.");
 if(mode==212){
  result("changed-provider-without-ACK-stays-refused",now_block&&hand_count()==5,"No status read or stale provider pointer is used as permission to release the old client.");
  puts("SAFE_REFUSAL discovery changed before cooperative closure ACK");return;
 }
 if(mode==206){
  result("RD-block-is-not-TW-owned",next.state==RHS_STATE_BLOCKED,"OBSERVE refusal belongs to RD. No recover, pending clear or producer impersonation in TW.");
  p=hand_action();result("owner-block-stays-refused",p==4&&hand_count()==5,"New InitScene alone cannot reset RD's separate quarantine/retry state.");puts("SAFE_REFUSAL RD owner lifecycle needs its own verified new-duel boundary");return;
 }
 result("new-duel-clears-only-local-fence",!now_block&&!*(unsigned*)tw_symbol("tw_final_armed")&&!*(unsigned*)tw_symbol("tw_final_gate"),"Local adapter state only, after acknowledged disconnection; no owner pending writes.");
 if(now_block){puts("SAFE_REFUSAL old local fence still requires process/adapter reset");return;}
 if(mode==202||mode==207){
  p=hand_action();RhsRecovery owner=recovery();
  printf("RECOVERY_OWNER_BLOCK mode=%d state=%u cause=%u hand=%d\n",mode,owner.state,owner.cause,hand_count());
  result("RD-post-handoff-owner-refusal-preserved",p==4&&hand_count()==5&&owner.state==RHS_STATE_QUARANTINE,"TW is locally clean, but RD's old handoff monitor detects a new world. Only RD may define its new-duel invalidation; no automatic recover or pending clear.");
  puts("SAFE_REFUSAL RD post-handoff monitor requires verified owner boundary");return;
 }
 rec_old_ticket=old;rec_old_proposal=proposal;rec_old_client=old_client;
 rec_replay_base=Mods_Find(HF_KEY);rec_replay_proxy=*rec_replay_base;rec_replay_proxy.authorize=rec_replay_authorize;
 CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",&rec_replay_proxy));
 p=hand_action();HfTicket fresh=*(HfTicket*)tw_symbol("tw_hf_ticket");CHECK(api->read(&status,sizeof status)==HF_OK);
 result("new-duel-new-explicit-transaction",(p==6||p==7)&&hand_count()==4&&status.consumed==1&&fresh.token.action>old.token.action&&fresh.token.client>old.token.client&&fresh.token.nonce>proposal.token.nonce&&conserved(),"Fresh real RD/TW planning, consent and physical collection. No old ticket reuse or card rollback.");
 printf("RECOVERY_FINAL mode=%d phase=%d hand=%d old_action=%u action=%u old_client=%u client=%u offered_nonce=%u caller_nonce=%u new_nonce=%u resets=%u\n",mode,p,hand_count(),old.token.action,fresh.token.action,old.token.client,fresh.token.client,proposal.token.nonce,old.token.nonce,fresh.token.nonce,Hal9_ResetCount()-resets);
 if(mode==207)result("pending-preference-does-not-disable-owner",Mods_Active(mi(T))&&!Mods_Enabled(mi(T)),"Preference persists across duel; effective applied callback remains the authority.");
}
