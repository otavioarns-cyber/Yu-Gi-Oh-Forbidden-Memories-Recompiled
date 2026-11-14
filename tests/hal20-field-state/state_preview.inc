#include "../../experiments/hal20-field-state/queen_state_schema_v1.h"
static int state_seam_seen;
static QsPayload*state_payload(void){CHECK(modules[RE].state_size==sizeof(QsPayload)&&modules[RE].state_version==1);return modules[RE].state;}
static const QsPreview*state_api(void){const QsPreview*p=Mods_Find("ritual-card-effects-test:queen-state-preview-v1");CHECK(p&&p->abi==1&&p->size==sizeof*p&&p->scope==1);return p;}
static void state_capture_event(void){MemoriesModEvent e={MEMORIES_EVENT_SAVE,MEMORIES_BEFORE,0,0,0,0,0};Mods_Dispatch(&e);Mods_VisitState(save_chunk,NULL);}
static void state_seam(void){
 if(!Settings_GetNamed("test.state-preview",0)||Settings_GetNamed("test.state-mode",0)!=4||state_seam_seen)return;
 state_seam_seen=1;state_capture_event();RhsRecovery r=recovery();printf("STATE_PREVIEW planner pending=%d state=%u eligible=%u\n",pull()->active,r.state,state_payload()->eligible);
 result("A-planner-transaction-not-eligible",!state_payload()->eligible,"Real planner boundary; do not serialize/continue a validated selection or pending.");
}
static int queen_query(RhsResponse*r){
 ((void(*)(void))engine_function("func_80028220"))();uint8_t*sel=datum("D_800EAE88");memset(sel,0,12);sel[0]=11;sel[6]=1;RhsRequest q;request(&q);
 const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);CHECK(v&&v->rules());return v->rules()->evaluate(&q,r);
}
static void state_preview_cases(int mode){
 setup(5);field_seed();queen_setup();hand_action();setup(5);field_seed();queen_setup();begin("typed-Queen-state-preview");RhsResponse initial;CHECK(queen_query(&initial)&&initial.mandatory);
 unsigned initial_stats=((unsigned(*)(DuelCardRecord*))engine_function("Duel_CalcCardStats"))((DuelCardRecord*)datum("D_801A7AD8")+5);
 const QsPreview*api=state_api();QsPayload*p=state_payload();
 if(mode==4){Settings_SetNamed("test.state-preview",1);Settings_SetNamed("test.state-mode",4);hand_action();result("A-real-planner-seam-reached",state_seam_seen,"Only observe native planner return; no pending insertion.");return;}
 if(mode==5){hand_action();state_capture_event();result("A-post-consumption-not-eligible",!p->eligible&&hand_count()==4,"No rollback or restoring consumed cards.");return;}
 if(mode==6){Settings_SetNamed("test.fault",3);hand_action();state_capture_event();RhsRecovery r=recovery();result("A-quarantine-not-eligible",!p->eligible&&r.state==RHS_STATE_QUARANTINE&&hand_count()==5,"Corrupted physical identity created only at the labeled seam; cannot persist/revive that action.");return;}
 if(mode==7){Hal25_ForceApplied(mi(RE),0);state_capture_event();result("A-effective-OFF-not-eligible",!p->eligible&&!p->owner,"Actual callback state, not saved preference.");Hal25_ForceApplied(mi(RE),1);return;}
 if(mode==8){Mods_SetEnabled(mi(RE),0);state_capture_event();result("A-preference-OFF-effective-ON",p->eligible&&p->owner&&!Mods_Enabled(mi(RE))&&Mods_Active(mi(RE)),"Restart pending does not erase the real owner state.");Mods_SetEnabled(mi(RE),1);}
 typedef struct{unsigned abi;unsigned(*lease)(int);void(*revoke)(int);} LeaseApi;
 typedef struct{int id;void*data;unsigned serial;} SavedLease;
 LeaseApi*lease=Mods_Find("fusion-auxiliary-balance:origin-unlock-v1");CHECK(lease);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");
 put_field(6,495,1);SavedLease*l=local(F,"aux_unlocks");CHECK(l);l[6]=(SavedLease){495,&d[1],77};CHECK(lease->lease(6)==77);
 state_capture_event();CHECK(p->eligible&&p->mask==(1u<<5));
 unsigned char*ram=malloc(0x200000),*host_data=malloc(sizeof data);CHECK(ram&&host_data);memcpy(ram,(void*)0x80000000,0x200000);memcpy(host_data,data,sizeof data);
 memcpy((void*)0x80000000,ram,0x200000);memcpy(data,host_data,sizeof data);Mods_VisitState(load_chunk,NULL);Mods_Reset();
 if(mode==2){d[2]=d[0];r[5].data=&d[2];puts("INSTRUMENTED same-ID physical reference changed AFTER modeled restore; no native rollback claim");}
 if(mode==3)p->version=2;
 unsigned before=setup_count;int hand=hand_count();MemoriesModEvent e={MEMORIES_EVENT_LOAD,MEMORIES_AFTER,0,0,0,0,0};Mods_Dispatch(&e);RhsResponse restored;CHECK(queen_query(&restored));
 if(mode==2||mode==3){result("A-invalid-identity-or-version-refused",api->status()==-1&&!restored.mandatory,"Post-restore owner refuses its capsule; this is not a pre-write safe load for the whole game.");puts("STATE_PREVIEW REFUSED limited-owner capsule");}
 else {result("A-Queen-versioned-roundtrip",api->status()==1&&restored.mandatory&&restored.count==1&&restored.selected[0].id==395,"Actual registered capsule, reset and LOAD subscriber; typed deck slot/block reference, no serialized pointer or BSS.");result("A-Queen-stat-bonus-roundtrip",initial_stats==((unsigned(*)(DuelCardRecord*))engine_function("Duel_CalcCardStats"))((DuelCardRecord*)datum("D_801A7AD8")+5),"Real R/F stat wrappers preserve this Queen bonus; no full effect coverage claim.");result("A-fresh-producer-generation",initial.revision[0]!=restored.revision[0]&&!pull()->active,"No action/generation/callback resurrection.");}
 result("A-no-card-consumption-or-repair",setup_count==before&&hand_count()==hand,"Owner restores logical effect values only, never materializes/restores cards.");
 result("A-Fusion-state-coverage-gap-reproduced",lease->lease(6)==0,"The Queen-only capsule does NOT cover Fusion leases; do not admit full collection load.");puts("STATE_PREVIEW PARTIAL_COVERAGE full-collection policy A remains NOT_REACHED");
 setup(5);field_seed();RhsResponse fresh;CHECK(queen_query(&fresh));result("A-new-duel-no-capsule-replay",!fresh.mandatory,"Without LOAD_AFTER, stale capsule is never replayed at reset/new duel.");free(ram);free(host_data);
}
