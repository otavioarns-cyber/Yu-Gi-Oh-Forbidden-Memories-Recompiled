/* Executes the relocated private candidate and the real manager queries.
 * Test services below the engine boundary remain modeled, as in PR30. */
const MemoriesModHost*Hal25_Host(int);int Hal9_TestModAPI(int);unsigned Hal9_TestCalls(void);void Hal9_TestMachine(void*,void*);
int Hal9_TestHard(void);const void*Hal9_TestDual(void);int Hal9_TestReady(void);
void*Hal9_TestImport(const char*,int);
static void *tw_symbol(const char *name){void*p=NULL;for(size_t i=0;i<modules[T].object.symbol_count;i++)if(!strcmp(modules[T].object.symbols[i].name,name))p=(void*)modules[T].object.symbols[i].address;CHECK(p);return p;}
static void hal9_bank_fixture(void){
 setup(5);put_field(25,16,45);DuelCardRecord*r=datum("D_801A7AD8");r[25].flags|=DUEL_CARD_FLAG_FACE_DOWN;
 ((uint8_t*)datum("D_800907D8"))[35]=25;*(uint8_t*)datum("D_8009B1D5")=1;*(uint16_t*)datum("gDuel_wSceneStateFlags")=5;
}
static void *hal9_extra_previous;
static void hal9_extra_draw(void){((void(*)(void))hal9_extra_previous)();}
typedef struct {int slices;const void*sectors;int mode,forced;const char*title;int pause;void(*done)(int,int,int,void*);void*user;} H9WheelRequest;
typedef struct {unsigned api;int(*spin)(const void*);int(*busy)(void);int(*result)(int);void(*cancel)(void);} H9Wheel;
static H9WheelRequest h9_request;static int h9_done_count,h9_spin_id;
static int h9_spin(const void*p){h9_request=*(const H9WheelRequest*)p;return ++h9_spin_id;}
static int h9_busy(void){return 0;}static void h9_cancel(void){}
static void h9_done(int id,int slice,int kind,void*p){(void)id;(void)slice;(void)kind;(void)p;++h9_done_count;}
static void hal9_encore_real_provider(void){
 setup(5);put_field(5,495,0);*(uint8_t*)datum("D_8009B1D5")=0;*(uint16_t*)datum("gDuel_wSceneStateFlags")=6;
 Encore*provider=Mods_Find("fusion-auxiliary-balance:encore-v1");CHECK(provider&&provider->eligible(0));
 H9Wheel **real=tw_symbol("real_wheel"),*saved=*real,mock={1,h9_spin,h9_busy,NULL,h9_cancel};*real=&mock;
 H9Wheel*proxy=*(H9Wheel**)tw_symbol("wheel");CHECK(proxy&&proxy->spin);
 int*core=tw_symbol("state"),*spin=tw_symbol("spin_id");core[0]=123;core[1]=1;
 H9WheelRequest req={6,NULL,0,0,"HAL9 private wheel transport",0,h9_done,NULL};int id=proxy->spin(&req);*spin=id;
 result("actual-encore-real-Fusion-ON",id>0&&*(int*)tw_symbol("eligible"),"Actual final Encore adapter consumes actual Fusion provider; wheel transport only is modeled.");
 Hal25_ForceApplied(mi(F),0);CHECK(!provider->eligible(0));
 h9_request.done(id,2,1,NULL);
 result("started-Encore-retains-cache-after-F-OFF",*(int*)tw_symbol("pending")&&*(int*)tw_symbol("retry"),"Reproduces unchanged approved code: attempt begun ON retains cached eligibility. No new unilateral cache rule.");
 char error[512];result("Encore-pending-policy-B-refusal",!Mods_Hal20StateLoadAllowed(error,sizeof error)&&core[1]==1&&*(int*)tw_symbol("pending"),"Unchanged policy B refuses during cached retry without clearing attempt or loading partial state.");
 proxy->cancel();core[1]=1;id=proxy->spin(&req);*spin=id;
 result("fresh-Encore-F-OFF-neutral",!*(int*)tw_symbol("eligible"),"Resident Mods_Find provider remains available but returns effective OFF neutrality.");
 int before=h9_done_count;h9_request.done(id,2,1,NULL);
 result("fresh-OFF-no-retry-one-completion",!*(int*)tw_symbol("pending")&&h9_done_count==before+1,"Actual adapter forwards final completion exactly once; no extra roulette grant.");
 Hal25_ForceApplied(mi(F),1);*real=saved;
}
static void hal9_collection_action(void){
 setup(5);queen_setup();begin("HAL9-collection-Queen-physical-action");
 DuelDeckCardRecord initial[80];memcpy(initial,datum("gDuel_aDeckCardRecords"),sizeof initial);
 int phase=hand_action(),id=action_card(),cat=action_category();
 printf("HAL9_PHYSICAL phase=%d id=%d category=%d hand=%d swaps=%u D_gate=%u RD_swap=%u H_swap=%u\n",phase,id,cat,hand_count(),setup_count,calls(D,"func_8001BAF0"),calls(RD,"func_8001BAF0"),calls(H,"func_8001BAF0"));
 result("TW-resident-real-controller-Queen",(phase==6||phase==7)&&id==395,"Real R mandatory provider, RD consumer and native hand controller; TW wrapper stays active. Synthetic board/VM oracle, not Windows duel.");
 result("TW-resident-unique-consumption",hand_count()==4&&conserved()&&setup_count<=1&&!pull()->active,"One physical instance consumed; no pending injection/rollback/restoration.");
 result("TW-resident-unselected-integrity",deck_changes(initial)==(setup_count?2:0),"Only selected physical exchange permitted; data-block permutation remains unique.");
 if(Mods_Active(mi(D)))result("TW-resident-actual-Dual-quota",ds()[1]==cat,"Actual private Dual ledger equals materialized definitive category; callback trace retained.");
}
static void hal9_time_magic_action(void){
 setup(5);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");d[40].id=16;r[15].card_id=16;
 set_stat(16,500,400,CARD_TYPE_SPELLCASTER);r[15].attack=500;r[15].defense=400;
 put_field(0,6,0);put_field(1,47,1);put_field(2,571,2);
 setup_count=spawn_count=0;begin("HAL9-real-controller-TimeMagic-synthetic-board");int phase=hand_action(),id=action_card();
 printf("HAL9_TIME_MAGIC phase=%d id=%d hand=%d swaps=%u D_gate=%u RD_swap=%u H_swap=%u quota=%u\n",phase,id,hand_count(),setup_count,calls(D,"func_8001BAF0"),calls(RD,"func_8001BAF0"),calls(H,"func_8001BAF0"),Mods_Active(mi(D))?ds()[1]:0);
 if(id==16&&phase==7){result("actual-TW-Monster-physical",hand_count()==4&&conserved()&&setup_count<=1,"Real controller selected physical TW as Monster, not Time Magic; no role/plan forced.");if(Mods_Active(mi(D)))result("actual-TW-Monster-quota",ds()[1]==1,"Actual private ledger counts Monster for this physical TW play.");}
 if(id!=16||phase!=6){puts("HAL9_NOT_REACHED real planners did not commit Time Magic in the available VM/catalog model; no selection/plan supplied");return;}
 result("actual-TimeMagic-single-physical",hand_count()==4&&conserved()&&setup_count<=1,"Actual planner/controller commit of TW Support; no hand selection/pending injected. Wheel presentation tested separately.");
 if(Mods_Active(mi(D)))result("actual-TimeMagic-Dual-support-quota",ds()[1]==2,"Actual Dual ledger must count Support for TW materialized into Time Magic.");
}
#include "../hal9-support/support.inc"
static void hal9_object_cases(void){
 int t=mi(T);CHECK(t>=0);printf("HAL9_ABI host=%u mod=%d pointer_bytes=%u\n",Hal25_Host(t)->api,Hal9_TestModAPI(t),(unsigned)sizeof(void*));int mode=Settings_GetNamed("test.tw-case",0);
 if(Settings_GetNamed("test.tw-reference",0)){
  CHECK(Mods_Active(t)&&!Mods_Failed(t));hal9_bank_fixture();((void(*)(void))tw_symbol("ai_field_actions"))();
  int bank=((int*)tw_symbol("diag"))[18],hard=Mods_Active(mi(H));
  printf("HAL9_BASELINE actual-core bank_discovered=%d H_effective=%d\n",bank,hard);
  result("original-core-HARD-detector",bank==hard,"Actual received object's embedded field callback: non-HARD predecessor incorrectly discovers BANK when H is OFF. Expected historical FAIL retained.");
  void*(*old_resolve)(void)=tw_symbol("resolve_dual_state");
  int before=old_resolve()!=NULL;
  if(!Mods_Active(mi(D))){Mods_SetEnabled(mi(D),1);int after=old_resolve()!=NULL;
   result("original-late-Dual-recognition",after,"Actual received adapter after real late Dual activation; direct predecessor remains native/other wrapper. Expected historical FAIL retained.");printf("HAL9_BASELINE dual_before=%d after=%d D_effective=%d\n",before,after,Mods_Active(mi(D)));
  }
  return;
 }
 if(mode==4||mode==5||(mode==6&&Mods_Active(mi(D)))||(mode==7&&Mods_Active(mi(H)))){
  result("private-import-startup-refusal",Mods_Failed(t)&&!Mods_Active(t),"No opt-in or changed caller/active owner hash rejects candidate before controller. Preferences retained.");
  puts("SAFE_REFUSAL HAL9 unqualified caller/owner or opt-in disabled");return;
 }

 result("candidate-load-init",!Mods_Failed(t)&&Mods_Active(t),"Actual MemoriesModInit and embedded core/adapters initialized by real manager.");
 if(Mods_Failed(t)||!Mods_Active(t))return;
 result("actual-owner-readiness",Hal9_TestReady()==1,"Effective owner state and pinned libraries; not saved preference.");
 unsigned ca,cb,cc,cd;__asm__ volatile("cpuid":"=a"(ca),"=b"(cb),"=c"(cc),"=d"(cd):"a"(1));printf("HAL9_CPU fxsr=%u sse=%u\n",(cd>>24)&1,(cd>>25)&1);
 int h=mi(H),d=mi(D),on=h>=0&&Mods_Active(h),don=d>=0&&Mods_Active(d);
 result("hard-effective-state",Hal9_TestHard()==on,"Real active H hook and full digest; R/RD predecessor does not imply H.");
 int (*eax)(void)=tw_symbol("Hal9_ReadHardEAX");
 result("relocated-core-probe",eax()==on,"State-preserving EAX probe in actual final object, not unlinked AI C.");
 const void *dual=Hal9_TestDual();
 printf("HAL9_QUERY hard=%d dual=%d expected-dual=%d\n",Hal9_TestHard(),dual!=NULL,don);
 result("dual-qualified-chain",(dual!=NULL)==don,"Inspect every active draw wrapper, actual callback symbol/full file hash and predecessor links.");
 void*(*resolve_dual)(void)=tw_symbol("resolve_dual_state");
 void *state=resolve_dual();
 result("executed-dual-resolver",don?state==modules[D].state:state==NULL,"Actual linked new Dual adapter resolves original registered state through intervening R wrapper.");

 if(mode==6&&!Mods_Active(mi(D))){Mods_SetEnabled(mi(D),1);result("late-unknown-Dual-refused",!Hal9_TestReady()&&Hal9_TestDual()==NULL&&resolve_dual()==NULL,"Real manager activates unqualified digest; query fails closed and old cached state is not exposed.");puts("SAFE_REFUSAL HAL9 late unknown Dual remains outside admitted collection");return;}
 if(mode==7&&!Mods_Active(mi(H))){Hal25_ForceApplied(mi(H),1);result("late-unknown-HARD-refused",Hal9_TestHard()==-1&&!Hal9_TestReady(),"Instrumented effective activation of changed owner digest: UNKNOWN is explicit, not a qualified OFF/ON state.");puts("SAFE_REFUSAL HAL9 late unknown HARD remains outside admitted collection");return;}
 if(mode>=20){support_cases(mode);return;}
 if(mode==9){hal9_encore_real_provider();return;}
 if(mode==11){hal9_collection_action();return;}
 if(mode==12||mode==13){hal9_time_magic_action();return;}
 /* Independent machine-state check on the exact relocated probe code. */
 for(int which_probe=0;which_probe<2;which_probe++){
  unsigned out[20]={0};double one;
  Hal9_TestMachine(tw_symbol(which_probe?"Hal9_ReadHardECX":"Hal9_ReadHardEAX"),out);memcpy(&one,out+10,8);
  int ok=out[0]==0x66666666&&out[1]==0x55555555&&out[4]==0x44444444&&out[5]==0x33333333&&out[6]==(which_probe?(unsigned)on:0x22222222)&&out[7]==(which_probe?0x11111111:(unsigned)on)&&out[8]==out[9]&&out[2]==out[13]&&out[3]==out[14]&&(out[12]&65535)==out[15]&&one==1.0;
  result(which_probe?"probe-ECX-register-flags-x87":"probe-EAX-register-flags-x87",ok,"Actual linked assembly preserves other GPR, EFLAGS and x87 value; CPU path logged separately.");
 }
 setup(5);*(int8_t*)((char*)datum("D_8009B360")+1)=0;
 *(void**)((char*)datum("gAiScript_State")+4)=datum("D_801A8000");
 unsigned count=Hal9_TestCalls();int decision=hand_action();
 printf("HAL9_EXEC ai_run result=%d queries=%u\n",decision,Hal9_TestCalls()-count);
 if(decision==4)puts("HAL9_NOT_REACHED controller has not completed a physical action; detector execution alone is not gameplay qualification");
 result("embedded-ai-run-query-executed",Hal9_TestCalls()>count,"Actual embedded ai_run executed its replaced detector after real prior planner; no replacement AI source linked.");
 count=Hal9_TestCalls();((void(*)(void))tw_symbol("ai_field_actions"))();
 printf("HAL9_EXEC ai_field_actions queries=%u\n",Hal9_TestCalls()-count);
 result("embedded-field-query-executed",Hal9_TestCalls()>count,"Actual embedded field callback executes patched detector; native field services remain modeled.");
 result("private-import-caller-denied",Hal9_TestImport("Hal9_CoreHardStateV1",mi(RE))==NULL&&Hal9_TestImport("Hal9_CoreHardStateV1",-1)==NULL,"Imports require opt-in plus exact full candidate digest; no name-only approval.");
 result("private-import-real-caller",Hal9_TestImport("Hal9_CoreHardStateV1",t)!=NULL,"Experimental private extension, not official API24 export.");
 if(mode==1){
  Mods_SetEnabled(h,!on);
  result("pending-restart-effective-H-state",Mods_Enabled(h)==!on&&Mods_Active(h)==on&&Hal9_TestHard()==on&&eax()==on,"Saved preference changes without active-state change; canceling restart does not impersonate applied H.");
  Mods_SetEnabled(h,on);
  Hal25_ForceApplied(h,!on);result("effective-H-transition",Hal9_TestHard()==!on&&eax()==!on,"Explicit effective lifecycle test; first loading is distinguished from saved restart preference, not a gameplay reachability claim.");
  Hal25_ForceApplied(h,on);result("effective-H-restored",Hal9_TestHard()==on&&eax()==on,"No decision/settings change; reads actual active hook even after OFF/ON.");
 }
 if(mode==2){
  Mods_SetEnabled(d,!don);int now=Mods_Active(d);
  result("late-or-OFF-Dual-real-manager",(resolve_dual()!=NULL)==now,"Actual Mods_SetEnabled and Hooks_Relink; direct predecessor no longer determines detection.");
  Mods_SetEnabled(d,don);result("Dual-cache-revalidated",(resolve_dual()!=NULL)==don,"OFF clears old callback/state cache; ON qualifies actual callback again.");
 }
 if(mode==3){
  int token=Hooks_Add(mi(OBS),engine_function("DuelScene_UpdateDrawPhase"),hal9_extra_draw,&hal9_extra_previous);CHECK(token);Hooks_Relink();
  result("unknown-draw-wrapper-refused",Hal9_TestDual()==NULL&&resolve_dual()==NULL,"Unknown callback is never accepted by owner name; no unsafe chain executed.");
  Hooks_Remove(mi(OBS),token);Hooks_Relink();result("known-draw-chain-recovered",(Hal9_TestDual()!=NULL)==don,"Removing explicit adversarial wrapper requalifies actual chain.");
 }
 if(mode==10&&don){
  Registration*bad=NULL;for(int n=0;n<nr;n++)if(registrations[n].owner==RE&&!strcmp(registrations[n].name,"DuelScene_UpdateDrawPhase"))bad=&registrations[n];CHECK(bad&&bad->previous);
  void*saved=*bad->previous;*bad->previous=(void*)0x12345000;
  result("broken-real-predecessor-refused",!Hal9_TestDual()&&!resolve_dual(),"Adversarial pointer perturbation: inspect only, never execute broken wrapper.");
  *bad->previous=saved;Hooks_Relink();result("relinked-predecessor-recovers",Hal9_TestDual()!=NULL&&resolve_dual()!=NULL,"Original pointer restored then official relink; cached state requalified.");
 }
 hal9_bank_fixture();((void(*)(void))tw_symbol("ai_field_actions"))();
 result("embedded-field-no-false-HARD-BANK",((int*)tw_symbol("diag"))[18]==on,"Exact embedded field callback sees an existing physical face-down TW; non-HARD wrapper does not enable BANK discovery.");
 if(on){
  /* Existing bank discovered above; second initialized field frame reaches
     the third inlined HARD predicate, without injecting AI pending or plan. */
  *(uint16_t*)datum("gDuel_wSceneStateFlags")=0x8005;*(uint8_t*)datum("D_8009B174")=3;
  count=Hal9_TestCalls();((void(*)(void))tw_symbol("ai_field_actions"))();
  result("third-embedded-detector-executed",Hal9_TestCalls()-count>=2,"Both field detection and should_activate_bank inlined predicates executed in the actual embedded core.");
 }
 char load_error[512];result("policy-B-keeps-load-refused",!Mods_Hal20StateLoadAllowed(load_error,sizeof load_error),"Unchanged PR30 prewrite policy remains independent of order guard, including resident TW.");
 Mods_Reset();result("reset-no-stale-TW-transients",!((int*)tw_symbol("state"))[1]&&!*(int*)tw_symbol("spin_id"),"Actual manager Reset and chained Time Wizard reset callbacks; no invented persistence.");

}
