/* Consolidation probes; no changes to the owned or private MOD binaries. */
static void harmless_wrapper(void){}
static int load_seams;
static void load_checkpoint(const char*where){
 if(!Settings_GetNamed("test.load-policy",0))return;
 char error[256]={0};unsigned char selection[12];memcpy(selection,datum("D_800EAE88"),12);
 int before=pull()->active,n=hand_count();unsigned swaps=setup_count;
 int allow=Mods_Hal20StateLoadAllowed(error,sizeof error);
 result("load-preflight-refused-before-write",!allow&&before==pull()->active&&n==hand_count()&&swaps==setup_count&&!memcmp(selection,datum("D_800EAE88"),12),"Real shared load preflight; no invented rollback or card restoration.");
 printf("LOAD_BOUNDARY %s pending=%d hand=%d swaps=%u allow=%d reason=%s\n",where,before,n,swaps,allow,error);++load_seams;
}
extern int Hal2224_WindowApply(int,int,int,int,int,int,int,int,int,int,int);
extern const char*Hal2224_WindowStatus(void);
static void consolidation_cases(int mode){
 setup(5);queen_setup();begin("consolidated-boundaries");
 if(mode==1){
  /* Establish the actual optional connection, then start a fresh modeled board. */
  hand_action();setup(5);queen_setup();((void(*)(void))engine_function("func_80028220"))();
  uint8_t*sel=datum("D_800EAE88");memset(sel,0,12);sel[0]=11;sel[6]=1;
  const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);const RhsRules*p=v->rules();CHECK(p);
  RhsRequest q;RhsResponse r;request(&q);CHECK(p->evaluate(&q,&r));result("quiet-Queen-mandatory-before-load",r.mandatory,"Real ritual commit and public ABI evaluator at phase4.");
  load_checkpoint("quiet-Queen");
  unsigned char*ram=malloc(0x200000),*host=malloc(sizeof data);CHECK(ram&&host);memcpy(ram,(void*)0x80000000,0x200000);memcpy(host,data,sizeof data);
  MemoriesModEvent e={MEMORIES_EVENT_SAVE,MEMORIES_BEFORE,0,0,0,0,0};Mods_Dispatch(&e);Mods_VisitState(save_chunk,NULL);
  /* Diagnostic alternative only: match native restore -> registered chunks -> reset -> event order.
     No MOD private BSS is serialized by this model. */
  memcpy((void*)0x80000000,ram,0x200000);memcpy(data,host,sizeof data);Mods_VisitState(load_chunk,NULL);Mods_Reset();e=(MemoriesModEvent){MEMORIES_EVENT_LOAD,MEMORIES_AFTER,0,0,0,0,0};Mods_Dispatch(&e);
  request(&q);CHECK(p->evaluate(&q,&r));result("legacy-load-loses-Queen-state",!r.mandatory,"Reproduced state coverage gap even at a quiet boundary; not a Windows save-file roundtrip.");free(ram);free(host);
 }else if(mode==2){
  Settings_SetNamed("test.load-policy",1);hand_action();load_checkpoint("after-consumption");result("planner-load-boundary-exercised",load_seams>=2,"Real planner return and consumed hand boundary; no pending injection.");result("load-refusal-keeps-single-consumption",hand_count()==4&&setup_count<=1&&conserved(),"Declined load never restores a consumed instance.");
 }else if(mode==3){
  Settings_SetNamed("test.load-policy",1);Settings_SetNamed("test.fault",3);hand_action();RhsRecovery s=recovery();load_checkpoint("quarantine");result("load-during-quarantine-refused",s.state==RHS_STATE_QUARANTINE&&hand_count()==5&&!action_card(),"Real invalid physical identity probe; explicit recovery remains necessary.");
 }else if(mode==4){
  char err[256]={0};CHECK(Mods_Hal2224GuardReady(err,sizeof err));
  void*previous=NULL;int token=Hooks_Add(mi(H),engine_function("AiScript_Run"),harmless_wrapper,&previous);CHECK(token);
  result("wrong-qualified-owner-callback-rejected",!Mods_Hal2224GuardReady(err,sizeof err),"Known owner is not enough: target/callback address/size and duplicates checked.");Hooks_Remove(mi(H),token);CHECK(Mods_Hal2224GuardReady(err,sizeof err));
  Registration*t=NULL;for(int i=0;i<nr;i++)if(registrations[i].previous&&Mods_Active(mi(H))&&registrations[i].owner==H&&!strcmp(registrations[i].name,"AiScript_Run")){t=&registrations[i];break;}CHECK(t);
  void*saved=*t->previous;*t->previous=harmless_wrapper;result("corrupted-delegation-rejected",!Mods_Hal2224GuardReady(err,sizeof err),"Actual previous pointer, not declared rank alone.");*t->previous=saved;CHECK(Mods_Hal2224GuardReady(err,sizeof err));
  token=Hooks_Add(mi(OBS),engine_function("AiScript_Run"),harmless_wrapper,&previous);CHECK(token);Hal25_ForceApplied(mi(OBS),1);result("unknown-critical-wrapper-rejected",!Mods_Hal2224GuardReady(err,sizeof err),"Unknown wrapper rejected before controller.");Hal25_ForceApplied(mi(OBS),0);Hooks_Remove(mi(OBS),token);CHECK(Mods_Hal2224GuardReady(err,sizeof err));
  result("guard-negative-probes-no-materialization",setup_count==0&&hand_count()==5,"No controller called while a corrupted chain was installed.");
 }else if(mode>=6){
  int h=mi(H),rd=mi(RD),d=mi(D);int oldH=Mods_Enabled(h),ranks[3]={Settings_GetNamed("mod.ai-hard-mode.order",0),Settings_GetNamed("mod.ritual-duelists.order",0),Settings_GetNamed("mod.dual-play.order",0)};
  if(mode==6||mode==7){int code=Hal2224_WindowApply(h,rd,d,1,1,1,50,10,20,1,0);(void)code;result("window-inverted-ranks-not-saved",Settings_GetNamed("mod.ai-hard-mode.order",0)==ranks[0]&&Settings_GetNamed("mod.ritual-duelists.order",0)==ranks[1]&&Settings_GetNamed("mod.dual-play.order",0)==ranks[2],"Real staged window validation, no preference overwrite/restart.");}
  else if(mode==8){int code=Hal2224_WindowApply(h,rd,d,!oldH,1,Mods_Enabled(d),-999,0,0,0,0);result("cancelled-restart-keeps-preferences",Mods_Enabled(h)==oldH&&code%10==1,"Real first Apply requests confirmation only; second Apply intentionally not performed.");}
  else if(mode==9){Hal2224_WindowApply(h,rd,d,!oldH,1,Mods_Enabled(d),-999,0,0,1,1);result("failed-restart-visible-chain-stays-safe",strstr(Hal2224_WindowStatus(),"Restart failed")!=0,"Platform restart failure injected; no successful process restart claimed.");}
  else if(mode==10){Mods_SetEnabled(h,1);result("first-late-AI-is-restart-deferred",Mods_Enabled(h)&&!Mods_Active(h)&&Mods_RequiresRestart(h),"Experimental local restart:true; no first late library initialization in the duel.");}
  else if(mode==11){Mods_Load();result("manager-reload-does-not-reorder-hooks",Mods_Active(rd),"Real manager reload, current initialized state retained.");}
  char error[256]={0};result("window-lifecycle-ready",Mods_Hal2224GuardReady(error,sizeof error),"Every reached controller remains under an actually qualified chain.");hand_action();result("window-lifecycle-single-consumption",hand_count()==4&&conserved()&&setup_count<=1,"No pending or materialization injected by the window helper.");
 }else if(mode==5){
  char err[256]={0};int enabled[512];for(int i=0;i<Mods_Count();i++)enabled[i]=Mods_Enabled(i);
  enabled[mi(RE)]=enabled[mi(F)]=enabled[mi(RD)]=enabled[mi(H)]=0;
  int ok=Mods_Apply(enabled,err,sizeof err);result("preferences-OFF-deferred-keeps-effective-state",ok&&Mods_Active(mi(RE))&&Mods_Active(mi(RD))&&Mods_Active(mi(F)),"Real restart-required manifests; saved preferences are not effective OFF.");
  result("deferred-OFF-still-refuses-load",!Mods_Hal20StateLoadAllowed(err,sizeof err),"Load preflight uses actual applied state.");
  for(int i=0;i<Mods_Count();i++)enabled[i]=Mods_Enabled(i);enabled[mi(RE)]=enabled[mi(F)]=enabled[mi(RD)]=enabled[mi(H)]=1;CHECK(Mods_Apply(enabled,err,sizeof err));
  result("apply-ON-preserves-qualified-chain",Mods_Hal2224GuardReady(err,sizeof err),"Preferences retained; hooks not rebuilt globally.");
  Settings_SetNamed("mod.ai-hard-mode.order",50);int order[512];Mods_Order(enabled,order,err,sizeof err);result("inverted-order-refused",!Mods_Hal2224GuardValidate(enabled,NULL,err,sizeof err),"No unsafe new profile accepted.");Settings_SetNamed("mod.ai-hard-mode.order",0);Mods_Order(enabled,order,err,sizeof err);CHECK(Mods_Hal2224GuardValidate(enabled,NULL,err,sizeof err));
  hand_action();result("lifecycle-one-physical-action",hand_count()==4&&conserved()&&setup_count<=1,"Real current controller/materializers after manager operations.");
 }
}
