#include "../../experiments/hal26/owner_preflight_v1.h"
static void state_pair_cases(int mode){
 setup(5);field_seed();queen_setup();hand_action();setup(5);field_seed();queen_setup();begin("HAL26-typed-pair-not-complete");RhsResponse before;CHECK(queen_query(&before)&&before.mandatory);
 typedef struct{unsigned api;unsigned(*lease)(int);void(*revoke)(int);} Lease;
 typedef struct{int id;void*data;unsigned serial;} RawLease;
 Lease*api=Mods_Find("fusion-auxiliary-balance:origin-unlock-v1");FlPreview*preview=Mods_Find("fusion-auxiliary-balance:hal26-lease-preview-v1");CHECK(api&&preview&&preview->abi==1&&preview->size==sizeof*preview&&preview->scope==1);
 CHECK(modules[F].state_size==sizeof(FlPayload)&&modules[F].state_version==1);FlPayload*f=modules[F].state;QsPayload*q=state_payload();
 put_field(6,495,1);DuelDeckCardRecord*decks=datum("gDuel_aDeckCardRecords");DuelCardRecord*records=datum("D_801A7AD8");RawLease*raw=local(F,"aux_unlocks");unsigned*serial=local(F,"aux_unlock_serial");CHECK(raw&&serial);raw[6]=(RawLease){495,&decks[1],77};*serial=77;CHECK(api->lease(6)==77);
 puts("INSTRUMENTED seed lease77 by physical identity; not a naturally granted gameplay action");
 if(mode==11){
  void(__attribute__((fastcall))*summon)(int)=local(F,"r2_summon_zombie");void(__attribute__((fastcall))*bind)(int)=local(F,"aux_unlock_bind");CHECK(summon&&bind);summon(0);int slot=-1;
  for(int i=5;i<10;i++)if(records[i].card_id==30)slot=i;CHECK(slot>=0);bind(slot);CHECK(api->lease(slot));state_capture_event();
  result("pair-generated-backing-refused",!f->eligible,"Actual compiled Fusion generator and bind helper; backing lies outside combined-deck identities. No pointer/BSS serialization or reconstruction by card ID.");puts("SAFE_REFUSAL generated Fusion backing requires a separate owner schema");return;
 }
 if(mode==4){Settings_SetNamed("test.state-preview",1);Settings_SetNamed("test.state-mode",4);hand_action();state_capture_event();result("pair-planning-refused",!q->eligible&&!f->eligible,"Actual planner-return seam; no pending injection.");return;}
 if(mode==5){hand_action();state_capture_event();result("pair-consumption-refused",!q->eligible&&!f->eligible&&hand_count()==4,"Never reconstruct consumed cards.");return;}
 if(mode==6){Settings_SetNamed("test.fault",3);hand_action();state_capture_event();result("pair-quarantine-refused",!q->eligible&&!f->eligible&&recovery().state==RHS_STATE_QUARANTINE,"No replay of vetoed action.");return;}
 if(mode==7){Hal25_ForceApplied(mi(F),0);state_capture_event();result("pair-OFF-revokes",!api->lease(6)&&!f->eligible,"F1 effective OFF revokes before a snapshot; no old benefits revived.");Hal25_ForceApplied(mi(F),1);return;}
 if(mode==8){Mods_SetEnabled(mi(F),0);state_capture_event();result("pair-deferred-OFF-keeps-effective-state",!Mods_Enabled(mi(F))&&Mods_Active(mi(F))&&f->eligible,"Saved OFF awaiting restart differs from callback OFF.");Mods_SetEnabled(mi(F),1);}
 state_capture_event();CHECK(q->eligible&&f->eligible);H26View staged[30];memset(staged,0,sizeof staged);
 for(int i=0;i<30;i++){DuelDeckCardRecord*d=records[i].data;uintptr_t dp=(uintptr_t)d,lo=(uintptr_t)decks;int n=dp>=lo&&dp<lo+80*sizeof(*decks)&&(dp-lo)%sizeof(*decks)==0?(int)((dp-lo)/sizeof(*decks)):-1;staged[i]=(H26View){(uint16_t)records[i].card_id,(uint16_t)n,d?d->data_block_index:0,!!(records[i].flags&DUEL_CARD_FLAG_OCCUPIED),n>=0?decks[n].id:0};}
 unsigned original=setup_count;int hand=hand_count();result("pair-full-coverage-refused-prewrite",H26_Preflight(q,f,staged,30,1)==H26_INCOMPLETE,"Even valid Queen/lease capsules cannot authorize full collection restore; no game memory written.");
 if(mode==2){staged[5].deck=2;puts("INSTRUMENTED staged-image same-ID physical reference mismatch, BEFORE modeled memory overwrite");}
 if(mode==3)f->version=2;
 if(mode==10)f->serial_floor=UINT32_MAX;
 int prepared=H26_Preflight(q,f,staged,30,0);
 if(mode==2||mode==3||mode==10){result("pair-invalid-capsule-refused-prewrite",prepared!=H26_READY&&api->lease(6)==77&&hand_count()==hand&&setup_count==original,"Validate staged identities/version/exhaustion before ANY modeled guest or owner write.");puts("SAFE_REFUSAL partial state capsule invalid");return;}
 CHECK(prepared==H26_READY);unsigned stats=((unsigned(*)(DuelCardRecord*))engine_function("Duel_CalcCardStats"))(records+5);
 unsigned char*ram=malloc(0x200000),*host=malloc(sizeof data);CHECK(ram&&host);memcpy(ram,(void*)0x80000000,0x200000);memcpy(host,data,sizeof data);
 if(mode==9)*serial=88;
 memcpy((void*)0x80000000,ram,0x200000);memcpy(data,host,sizeof data);Mods_VisitState(load_chunk,NULL);Mods_Reset();MemoriesModEvent e={MEMORIES_EVENT_LOAD,MEMORIES_AFTER,0,0,0,0,0};Mods_Dispatch(&e);
 RhsResponse after;CHECK(queen_query(&after));result("pair-Queen-and-lease-restored",after.mandatory&&api->lease(6)==77&&preview->status()==1,"Both real variant callbacks restore typed scopes; whole-collection load remains refused.");
 result("pair-Queen-bonus-preserved",stats==((unsigned(*)(DuelCardRecord*))engine_function("Duel_CalcCardStats"))(records+5),"Real R/F stat callbacks; no card re-creation.");
 result("pair-serial-high-water",*serial==(mode==9?88u:77u),"Local issuer high water never decreases on rewind. Cross-process uniqueness/full owner schema not certified.");
 result("pair-no-consumption-or-resurrection",hand_count()==hand&&setup_count==original&&!pull()->active&&after.revision[0]!=before.revision[0],"Fresh producer epoch, no restored action or double materialization.");
 records[21]=records[6];memset(&records[6],0,sizeof records[6]);result("pair-control-change-follows-physical-instance",api->lease(21)==77,"Existing lease consumer follows exact data identity across side/slot.");
 records[20]=records[21];records[20].data=&decks[2];decks[2]=decks[1];result("pair-equal-ID-not-authorized",!api->lease(20),"A different physical deck entry never inherits a lease by ID.");
 Hal25_ForceApplied(mi(F),0);Hal25_ForceApplied(mi(F),1);result("pair-OFF-ON-does-not-regrant",!api->lease(21),"Original Revogar policy preserved.");
 void(__attribute__((fastcall))*bind)(int)=local(F,"aux_unlock_bind");CHECK(bind);unsigned floor=*serial;bind(21);
 result("pair-real-new-grant-advances-serial",api->lease(21)==floor+1&&*serial==floor+1,"Actual compiled grant helper, ECX ABI inspected; explicit helper call, not a full Egotist gameplay acquisition.");
 memset(&records[21],0,sizeof records[21]);result("pair-removal-neutral",!api->lease(21),"Removing the physical instance never transfers its grant to the equal-ID copy.");
 setup(5);field_seed();RhsResponse fresh;CHECK(queen_query(&fresh));result("pair-new-duel-no-replay",!fresh.mandatory&&!api->lease(21),"No LOAD event, so capsules are not automatically replayed by reset.");free(ram);free(host);
}
