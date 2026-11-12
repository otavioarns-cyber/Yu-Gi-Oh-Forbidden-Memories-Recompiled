/* Test-owned scenarios for harness.c. Private data seeds are explicit below;
 * only hooks/providers/global exported ABI functions are called as functions.
 * Static local helpers are never invoked through guessed calling conventions. */
static void graph(const char*phase){
 const char*names[]={"AiScript_Run","Ai_GetHandSize","func_8001BAF0","Duel_CheckRitual","Duel_CheckFusion","DuelEffect_ApplyRitual","Duel_CalcCardStats"};
 for(unsigned k=0;k<sizeof names/sizeof*names;k++){
  printf("CHAIN phase=%s target=%s ",phase,names[k]);
  void *next=(unsigned char*)engine_function(names[k])+2;
  for(int i=0;i<nr;i++)if(!registrations[i].kind&&!strcmp(names[k],registrations[i].name)){
   CHECK(*registrations[i].previous==(Mods_Active(registrations[i].owner)?next:(unsigned char*)engine_function(names[k])+2));
   if(Mods_Active(registrations[i].owner))next=registrations[i].wrapper;
  }
  for(int i=nr-1;i>=0;i--)if(!registrations[i].kind&&!strcmp(names[k],registrations[i].name)&&Mods_Active(registrations[i].owner))printf("%s -> ",modules[registrations[i].owner].id);
  puts("engine");
 }
}
static void relay(void){
 begin("relay-OFF-ON");clear_engine();graph("initial");
 int(*window)(void)=engine_function("Ai_GetHandSize");CHECK(window()==5);
 int was[NMOD];for(int i=0;i<NMOD;i++){was[i]=modules[i].enabled;toggle(i,0);}
 for(int i=0;i<nr;i++)registrations[i].calls=0;CHECK(window()==5);
 MemoriesModEvent event={0};event.type=MEMORIES_EVENT_SAVE;event.phase=MEMORIES_BEFORE;Mods_Dispatch(&event);
 for(int i=0;i<nr;i++)CHECK(!registrations[i].calls);graph("all-OFF");
 for(int i=0;i<NMOD;i++)toggle(i,was[i]);CHECK(window()==5);graph("restored");
 CHECK(Mods_DamageLife(0,8000,500,0)==7500);
 result("relay-OFF-ON",1,"official Hooks_Relink excludes OFF callbacks and restores active chain");
}
static void contracts(void){
 begin("optional-contracts");clear_engine();
 RitualRulesProviderV1*rules=Mods_Find("melhoria-dos-rituais:rules_v1");RitualRulesSnapshotV1 snapshot;
 CHECK(rules&&rules->read(&snapshot,sizeof snapshot));toggle(M,0);CHECK(!rules->read(&snapshot,sizeof snapshot));CHECK(Mods_Find("melhoria-dos-rituais:rules_v1")==rules);toggle(M,1);
 RitualRecipeProviderV1*recipe=Mods_Find("ritual-card-effects-test:recipe_v1");RitualEffectIntelligenceV1*intel=Mods_Find("ritual-card-effects-test:effect_intelligence_v1");
 if(modules[R].loaded){
  RitualRecipeRequirementV1 req[3];int card=0;CHECK(recipe&&intel&&recipe->get(666,req,3,&card)==3&&card==357);CHECK(intel->enabled(357));
  toggle(R,0);CHECK(recipe->get(666,req,3,&card)==-1&&!intel->enabled(357));CHECK(Mods_Find("ritual-card-effects-test:recipe_v1")==recipe);toggle(R,1);
 }else CHECK(!recipe&&!intel);
 if(modules[RD].loaded){
  void(*read)(const MemoriesModHost*,RdAdaptiveRules*)=ObjectLoader_Symbol(&modules[RD].object,"Rd_ReadAdaptiveRules");
  const RdOpponentProfile*(*profile)(int)=ObjectLoader_Symbol(&modules[RD].object,"Rd_GetProfile");
  int(*resolve_recipe)(const MemoriesModHost*,const RdOpponentProfile*,int,RdRecipeCache*)=ObjectLoader_Symbol(&modules[RD].object,"Rd_ResolveRecipe");
  CHECK(read&&profile&&resolve_recipe);RdAdaptiveRules a;read(&modules[RD].host,&a);CHECK(a.available);
  toggle(M,0);read(&modules[RD].host,&a);CHECK(!a.available);toggle(M,1);
  RdRecipeCache cache;CHECK(resolve_recipe(&modules[RD].host,profile(23),0,&cache)&&cache.source==1);
  toggle(R,0);CHECK(resolve_recipe(&modules[RD].host,profile(23),0,&cache)&&cache.source==4);toggle(R,1);
 }
 if(modules[F].loaded&&modules[R].loaded){
  /* Real consumer forecast attachment through the shared PlayField hook. */
  void(*play)(void)=engine_function("AiScript_PlayFieldCard");play();
  void**rapi=local(F,"ea_rapi",NULL);void**fapi=local(R,"ea_fapi",NULL);CHECK(rapi&&fapi&&*rapi&&*fapi);
  toggle(R,0);play();CHECK(!*rapi);toggle(R,1);toggle(F,0);play();CHECK(!*fapi);toggle(F,1);
 }
 result("optional-contracts",1,"actual providers/consumers, absent stages, OFF lookup retained with guarded response");
}
typedef struct {int id;void*data;unsigned serial;} FixtureLease;
typedef struct {unsigned api;int(*eligible)(int);} FixtureEncore;
typedef struct {unsigned api;unsigned(*lease)(int);void(*revoke)(int);} FixtureUnlock;
static void fusion_off(void){
 if(!modules[F].loaded)return;begin("F1-fusion-provider-OFF");clear_engine();
 FixtureEncore*encore=Mods_Find("fusion-auxiliary-balance:encore-v1");FixtureUnlock*unlock=Mods_Find("fusion-auxiliary-balance:origin-unlock-v1");
 OriginEffectsV1*origin=Mods_Find("fusion-auxiliary-balance:origin-effects-v1");EaProvider*forecast=Mods_Find("fusion-auxiliary-balance:effect_forecast_v2");
 FixtureLease*leases=local(F,"aux_unlocks",NULL);DuelCardRecord*r=datum("D_801A7AD8");char identity;
 CHECK(encore&&unlock&&origin&&forecast&&leases);r[5].card_id=495;r[5].data=(void*)&identity;r[5].flags=DUEL_CARD_FLAG_OCCUPIED;leases[5]=(FixtureLease){495,&identity,77};
 CHECK(encore->eligible(0)&&unlock->lease(5)==77);toggle(F,0);
 int eligible=encore->eligible(0);unsigned lease=unlock->lease(5),extended=origin->lease(5);int enabled=forecast->on();
 printf("OBSERVED F1 encore=%d lease=%u extended=%u forecast=%d expected_all_zero\n",eligible,lease,extended,enabled);
 result("F1-fusion-provider-OFF",!eligible&&!lease&&!extended&&!enabled,"HAL-21 preexisting; no production fix");
 toggle(F,1);CHECK(unlock->lease(5)==77);modules[F].mod.reset();CHECK(!unlock->lease(5));
}
static void materialization_setup(int card){
 clear_engine();DuelSideState*s=datum("D_800E9FF0");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");AiActiveCard*a=datum("gDuel_aActiveCards");DuelHandSlot*h=datum("D_800EA030");
 *(uint8_t*)datum("D_8009B1D5")=0;*(int8_t*)datum("gDuel_bOpponentID")=9;*(int32_t*)datum("D_8009B360")=0;
 *(AiScriptState*)datum("gAiScript_State")=(AiScriptState){0};((AiScriptState*)datum("gAiScript_State"))->script_base=datum("D_801A8000");
 s[0].deck_draw_cursor=5;
 for(int i=0;i<5;i++){s[0].hand[i]=i;d[i].id=i?99:535;h[i].object=(void*)&displays[i];a[11+i].card_id=d[i].id;a[11+i].card_type=card_type(d[i].id);a[11+i].deck_index=i;}
 d[10].id=card;a[16].card_id=card;a[16].card_type=card_type(card);a[16].deck_index=10;display_count=5;
 uint8_t*sel=datum("D_800EAE88");sel[0]=16;sel[6]=6;sel[8]=1;
 CHECK(modules[D].state_size==50);memset(modules[D].state,0,50);
}
/* Offsets describe the previously audited registered 50-byte ABI v10.
 * No third-party source/struct/binary is embedded in this test. */
static uint8_t*dual_byte(int offset){CHECK(modules[D].state&&offset>=0&&offset<50);return (uint8_t*)modules[D].state+offset;}
static void quota_gate(void){
 const int cards[]={101,337,679},used[]={1,2,0};const char*names[]={"quota-Monster","quota-Support","quota-Ritual-in-bonus"};
 for(int k=0;k<3;k++){
  begin(names[k]);materialization_setup(cards[k]);*dual_byte(0)=used[k];*dual_byte(4)=1;*dual_byte(6)=5;*dual_byte(12)=1;
  void(*swap)(void)=engine_function("func_8001BAF0");swap();unsigned executed=calls(D,"func_8001BAF0");int rejected=*dual_byte(20);
  printf("OBSERVED quota card=%d used=%d dual_callbacks=%u rejected=%d selection=%u\n",cards[k],used[k],executed,rejected,*(uint8_t*)datum("D_800EAE88"));
  int pass=profile_dual?(executed>0&&rejected):!executed;
  result(names[k],pass,"actual Dual gate; synthetic bonus state; no full action/turn award");
 }
}
static void rd_materialization(void){
 if(!modules[RD].loaded)return;
 begin("RD-own-materialization-Dual-gate");materialization_setup(680);
 struct Pull{int active,side,source,replace,card,mode,target;uint8_t fallback[12];};size_t n;
 struct Pull*p=local(RD,"deck_pull",&n);CHECK(p&&n==sizeof(*p));
 *p=(struct Pull){1,0,16,0,680,1,6,{12,0}};*dual_byte(4)=1;*dual_byte(6)=5;*dual_byte(12)=1;
 void(*swap)(void)=engine_function("func_8001BAF0");swap();
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");int destination=-1;for(int i=0;i<5;i++)if(d[i].id==680)destination=i;
 unsigned dual_calls=calls(D,"func_8001BAF0"),hm_calls=calls(H,"func_8001BAF0");
 printf("OBSERVED rd_materialization dest=%d rd_pending=%d dual_callbacks=%u hm_callbacks=%u rejected=%u quota=%u selection=%u\n",destination,p->active,dual_calls,hm_calls,*dual_byte(20),*dual_byte(0),*(uint8_t*)datum("D_800EAE88"));
 result("RD-own-materialization-Dual-gate",profile_dual?(dual_calls&&*dual_byte(20)&&destination<0):destination>=0,"pending RD action explicitly seeded; actual hook chain called at engine entry");
 if(destination>=0)result("RD-pending-after-other-materializer",!p->active,"detect stale pending when another materializer never delegates");
 modules[RD].mod.reset();CHECK(!p->active);
}
static void ai_decision(void){
 begin("AI-decision-and-physical-selection");materialization_setup(101);
 uint8_t*sel=datum("D_800EAE88");memset(sel,0,12);sel[0]=12;sel[6]=1;
 void(*setup)(void)=engine_function("func_80028220");int(*run)(void)=engine_function("AiScript_Run");setup();int chosen=run();
 AiActiveCard*a=datum("gDuel_aActiveCards");int good=1,count=0;
 for(int i=0;i<5&&sel[i];i++){int slot=sel[i];if(slot<11||slot>55||a[slot].card_id<=0)good=0;count++;for(int j=0;j<i;j++)if(sel[j]==slot)good=0;}
 printf("OBSERVED AI result=%d selected_count=%d first=%u physical_or_existing_logical=%d\n",chosen,count,sel[0],good);
 result("AI-decision-and-physical-selection",good&&(!chosen||count>0),"synthetic small board only; strategic optimality/gameplay not certified");
}
static void effects_instances(void){
 if(!modules[R].loaded)return;begin("effects-per-instance-and-equipment");clear_engine();
 DuelCardRecord*r=datum("D_801A7AD8");DuelSideState*s=datum("D_800E9FF0");
 typedef struct{void*data;unsigned ritual,battle_flags;} SeaMark;SeaMark*whale=local(R,"whale_marks",NULL);unsigned*hint=local(R,"stat_effect_hints",NULL);CHECK(whale&&hint);
 int(*calc)(DuelCardRecord*)=engine_function("Duel_CalcCardStats");char identity,copy;
 r[5].card_id=r[6].card_id=718;r[5].flags=r[6].flags=DUEL_CARD_FLAG_OCCUPIED;r[5].data=(void*)&identity;r[6].data=(void*)&copy;
 whale[5]=(SeaMark){&identity,1,0};*hint=1u<<4;r[7].card_id=400;r[7].flags=DUEL_CARD_FLAG_OCCUPIED;
 int marked=(unsigned)calc(&r[5])>>16,ordinary=(unsigned)calc(&r[6])>>16;
 printf("OBSERVED whale marked=%d ordinary_copy=%d\n",marked,ordinary);
 result("effects-per-instance",marked==3000&&ordinary==1000,"actual Ritual/Fusion stats hook chain; other Whale and one synthetic Fish ally");
 if(modules[F].loaded){
  int(*equip)(int,int)=engine_function("Duel_CheckEquip");int on=equip(318,704);toggle(R,0);int off=equip(318,704);toggle(R,1);
  printf("OBSERVED Egotist ritual_provider_ON=%d OFF=%d\n",on,off);result("equipment-real-provider-OFF",on==704&&off==0,"Fusion queries actual Ritual intelligence provider");
  EaProvider*forecast=Mods_Find("fusion-auxiliary-balance:effect_forecast_v2");int out=0,origin=0;
  int pair=forecast->pair(61,658,0,&out,&origin);printf("OBSERVED Metalmorph pair=%d result=%d origin=%d\n",pair,out,origin);
  result("Metalmorph-forecast",pair&&out==412&&origin==EA_METAL,"real Fusion contract; executor/animation not run here");
  clear_engine();FixtureLease*leases=local(F,"aux_unlocks",NULL);FixtureUnlock*u=Mods_Find("fusion-auxiliary-balance:origin-unlock-v1");CHECK(leases&&u);
  r[5].card_id=r[6].card_id=495;r[5].data=(void*)&identity;r[6].data=(void*)&copy;r[5].flags=r[6].flags=DUEL_CARD_FLAG_OCCUPIED;
  leases[5]=(FixtureLease){495,&identity,200};leases[6]=(FixtureLease){495,&copy,201};
  void(*remove)(DuelCardRecord*)=engine_function("DuelCard_RemoveFromField");remove(&r[5]);
  result("remove-one-instance-retain-other",!u->lease(5)&&u->lease(6)==201&&!r[5].flags,"real Ritual/Fusion removal chain, distinct leases");
  clear_engine();void**generated=local(F,"r2_zombie_lease",NULL);CHECK(generated);DuelDeckCardRecord created={30,255,0,0,0};
  void(*restore)(int,int)=engine_function("func_80024D34");int restored=1;
  for(int side=0;side<2;side++){
   int slot=side?20:5;r[slot].card_id=30;r[slot].data=&created;r[slot].flags=DUEL_CARD_FLAG_OCCUPIED|DUEL_CARD_FLAG_DEFENSE_POSITION;generated[slot]=&created;
   restore(side?0x85:5,255);restored&=r[slot].card_id==30&&r[slot].data==&created&&r[slot].object!=NULL&&(r[slot].flags&DUEL_CARD_FLAG_OCCUPIED);
  }
  result("generated-card-reconstruction-both-sides",restored,"real restore hook chain; synthetic engine/object spawn; no texture proof");
 }
 (void)s;
}
static void ritual_field(int slot,int card,int uid){
 DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");int index=5+slot;
 d[uid].id=card;r[index].card_id=card;r[index].flags=DUEL_CARD_FLAG_OCCUPIED;r[index].data=&d[uid];r[index].object=spawn(card,0,0);
 ((uint8_t*)datum("D_800907D8"))[5+slot]=index;
}
static void ritual_hand(int slot,int card,int uid){
 DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelSideState*s=datum("D_800E9FF0");DuelHandSlot*h=datum("D_800EA030");
 d[uid].id=card;s[0].hand[slot]=uid;r[slot].card_id=card;r[slot].flags=DUEL_CARD_FLAG_OCCUPIED;r[slot].data=&d[uid];h[slot].object=spawn(card,0,0);
}
static void ritual_consumption(void){
 int r_on=modules[R].enabled;toggle(R,0);begin("Vanilla-three-field-ritual");clear_engine();
 int(*check)(DuelRitualResult*,int)=engine_function("Duel_CheckRitual");void(*consume)(DuelCardRecord*)=engine_function("DuelCard_DeactivateRecord");
 ritual_field(0,29,0);ritual_field(1,31,1);ritual_field(2,101,2);ritual_field(3,77,3);
 DuelCardRecord*r=datum("D_801A7AD8");DuelRitualResult out={0};int found=check(&out,666);
 if(found==357)for(int i=0;i<3;i++)consume(&r[5+i]);
 result("Vanilla-three-field-ritual",found==357&&!r[5].flags&&!r[6].flags&&!r[7].flags&&r[8].flags,"actual Melhoria hook/consumer; bounded synthetic native oracle");
 for(int mode=0;mode<2;mode++){
  if(mode&&!modules[R].loaded)continue;toggle(R,mode);begin(mode?"modified-mixed-ritual-consumption":"Vanilla-mixed-ritual-consumption");clear_engine();
  overrides[M][0]=1;overrides[M][1]=0;overrides[M][2]=0;overrides[M][3]=0;
  ritual_field(0,101,0);ritual_hand(0,29,1);ritual_hand(1,31,2);ritual_hand(2,31,3);ritual_field(1,1,4);
  /* A strong Blue-Eyes is a genuine nonmatching bystander under both
     the fixed Vanilla and modified <=1599-ATK Dragon requirements. */
  ((uint32_t*)datum("gDuel_adwCardStats"))[0]=300|(250u<<9);
  DuelSideState*s=datum("D_800E9FF0");s[0].deck_draw_cursor=5;int before[5];for(int i=0;i<5;i++)before[i]=s[0].hand[i];
  int probe=check(NULL,666);int probe_safe=1;for(int i=0;i<5;i++)if(before[i]!=s[0].hand[i])probe_safe=0;
  memset(&out,0,sizeof out);found=check(&out,666);printf("OBSERVED ritual mode=%d probe=%d out=%d probe_preserves_hand=%d\n",mode,probe,found,probe_safe);
  if(found==357){
   void*objects[3]={0};int(*visuals)(void**)=engine_function("Duel_RitualFieldObjects");CHECK(visuals(objects)==3);
   for(int i=0;i<3;i++)CHECK(objects[i]==out.tribute_objects[i]);
   for(int i=0;i<3;i++)consume(&r[5]);
  }
  int good=probe==357&&found==357&&probe_safe&&!r[5].flags&&s[0].hand[0]==-1&&s[0].hand[1]==-1&&s[0].hand[2]==3&&r[2].card_id==31&&r[2].flags&&r[6].flags;
  printf("OBSERVED consumed hands=%d,%d unselected_same_ID=%d bystander_flags=%x\n",s[0].hand[0],s[0].hand[1],s[0].hand[2],r[6].flags);
  result(mode?"modified-mixed-ritual-consumption":"Vanilla-mixed-ritual-consumption",good,"actual public hooks, exact selected instances, spare identical copy retained");
 }
 toggle(R,0);begin("repeated-material-physical-copies");clear_engine();
 uint16_t row[]={675,1,1,1,380,0};memcpy(datum("gDuel_awRitualData"),row,sizeof row);ritual_field(0,1,0);ritual_hand(0,1,1);
 int two=check(NULL,675);ritual_hand(1,1,2);int three=check(NULL,675);
 printf("OBSERVED Ultimate Dragon ritual two_copies=%d three_copies=%d\n",two,three);
 result("repeated-material-physical-copies",two==0&&three==380,"three actual Blue-Eyes copies required, no alias reuse");
 toggle(R,r_on);for(int k=0;k<4;k++)overrides[M][k]=-999;
}
static void planner_pipeline(void){
 if(!modules[RD].loaded)return;
 begin("RD-planner-through-Dual-setup-and-swap");materialization_setup(680);
 *dual_byte(0)=1;*dual_byte(4)=1;*dual_byte(6)=5;*dual_byte(12)=1;
 uint8_t*selection=datum("D_800EAE88");memset(selection,0,12);selection[0]=12;selection[6]=1;
 void(*setup)(void)=engine_function("func_80028220");int(*run)(void)=engine_function("AiScript_Run");void(*swap)(void)=engine_function("func_8001BAF0");
 setup();int visible=((AiActiveCard*)datum("gDuel_aActiveCards"))[16].card_id;int decision=run();
 int*pull=local(RD,"deck_pull",NULL);CHECK(pull);int pending=pull[0],selected=selection[0];swap();
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");int obtained=0;for(int i=0;i<5;i++)obtained|=d[i].id==680;
 printf("OBSERVED RD_planner setup_card=%d decision=%d selection_before=%d pending_before=%d obtained_Ritual=%d dual_swap_calls=%u rejected=%u\n",visible,decision,selected,pending,obtained,calls(D,"func_8001BAF0"),*dual_byte(20));
 result("RD-planner-through-Dual-setup-and-swap",!profile_dual||!obtained,"real AI/setup/materializer sequence; no private pending action seed");
 if(!profile_dual&&!profile_hm)result("RD-planner-positive-control",pending&&obtained,"same real planner fetches approved Rex ritual 680 without Dual; nonvacuous negative control");
}
static void pending_transition(void){
 if(!modules[RD].loaded||!profile_hm||profile_dual)return;
 begin("RD-pending-after-HM-ON-transition");materialization_setup(680);toggle(H,0);
 uint8_t*sel=datum("D_800EAE88");memset(sel,0,12);sel[0]=12;sel[6]=1;
 void(*setup)(void)=engine_function("func_80028220");int(*run)(void)=engine_function("AiScript_Run");void(*swap)(void)=engine_function("func_8001BAF0");
 setup();run();int*pending=local(RD,"deck_pull",NULL);CHECK(pending&&pending[0]);
 /* Actual planner creates the action; only the official hook activation
    changes between planning and materialization. No private action seed. */
 toggle(H,1);swap();DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");int moved=0;for(int i=0;i<5;i++)moved|=d[i].id==680;
 printf("OBSERVED HM_OFF_plan_ON_swap card_moved=%d pending_after=%d rd_swap_calls=%u hm_swap_calls=%u\n",moved,pending[0],calls(RD,"func_8001BAF0"),calls(H,"func_8001BAF0"));
 result("RD-pending-after-HM-ON-transition",moved&&!pending[0],"real RD planner -> HM OFF/ON -> actual materializer; no private pending action injection");
 modules[RD].mod.reset();CHECK(!pending[0]);
}
static void ritual_commit_identity(void){
 if(!modules[RD].loaded)return;begin("ritual-commit-shared-identity");clear_engine();
 DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");
 d[0].id=704;d[1].id=704;r[5].card_id=704;r[5].data=&d[0];r[5].flags=DUEL_CARD_FLAG_OCCUPIED;
 *(uint8_t*)datum("D_8009B19C")=5;*(int16_t*)datum("D_8009B1A0")=704;*(uint16_t*)datum("D_8009B210")=0x83;
 *(int8_t*)datum("gDuel_bOpponentID")=9;*(int32_t*)datum("D_8009B360")=0;((uint8_t*)datum("D_800907D8"))[5]=5;
 void(*commit)(void)=engine_function("DuelEffect_ApplyRitual");commit();
 typedef struct{void*data;unsigned ritual;int last_bonus;} FixtureSkull;
 FixtureSkull*skull=local(R,"skull_marks",NULL);RdSaveState*s=modules[RD].state;CHECK(skull&&s);
 int marked=s->ritual_marks[5].active&&s->ritual_marks[5].card_id==704&&skull[5].data==&d[0]&&skull[5].ritual;
 int(*fusion)(int,int)=engine_function("Duel_CheckFusion");int protected_result=fusion(704,2);r[5].data=&d[1];int ordinary=fusion(704,2);
 printf("OBSERVED shared_commit RD_mark=%u Ritual_mark=%u protected_fusion=%d changed_origin_fusion=%d\n",s->ritual_marks[5].active,skull[5].ritual,protected_result,ordinary);
 result("ritual-commit-shared-identity",marked&&protected_result==0&&ordinary!=0&&calls(R,"DuelEffect_ApplyRitual")==1&&calls(RD,"DuelEffect_ApplyRitual")==1,"actual Ritual/RD commit and three-mod fusion chain; only controller advance is native stub");
}
static void state_visit(int owner,void*p,size_t size,unsigned version,void*context){
 unsigned char(*saved)[512]=context;CHECK(size<=512);memcpy(saved[owner],p,size);printf("SAVE owner=%s size=%zu version=%u\n",modules[owner].id,size,version);
}
static void state_restore(int owner,void*p,size_t size,unsigned version,void*context){(void)version;unsigned char(*saved)[512]=context;memcpy(p,saved[owner],size);}
static void state_events(void){
 begin("reset-new-duel-save-load");clear_engine();unsigned char saved[NMOD][512]={{0}};
 *dual_byte(0)=3;if(modules[RD].loaded){RdSaveState*s=modules[RD].state;CHECK(modules[RD].state_size==288&&modules[RD].state_version==4);s->generated=7;s->starter_ready=1;s->simon_theme=2;s->simon_deck[0]=693;s->ritual_marks[20]=(RdAiRitualMark){704,59,1};}
 MemoriesModEvent e={0};e.type=MEMORIES_EVENT_SAVE;e.phase=MEMORIES_BEFORE;Mods_Dispatch(&e);Mods_VisitState(state_visit,saved);
 for(int m=0;m<NMOD;m++)if(Mods_Active(m)&&modules[m].state)memset(modules[m].state,0,modules[m].state_size);
 Mods_VisitState(state_restore,saved);for(int m=0;m<NMOD;m++)if(Mods_Active(m)&&modules[m].mod.reset)modules[m].mod.reset();
 e.type=MEMORIES_EVENT_LOAD;e.phase=MEMORIES_AFTER;Mods_Dispatch(&e);
 if(profile_dual)CHECK(*dual_byte(0)==3);if(modules[RD].loaded)CHECK(((RdSaveState*)modules[RD].state)->generated==7);
 void(*new_duel)(void)=engine_function("Duel_InitScene");new_duel();CHECK(!*dual_byte(0)||!profile_dual);
 if(modules[RD].loaded){
  RdSaveState*s=modules[RD].state;CHECK(s->ritual_marks[20].active);*(uint8_t*)datum("D_8009B368")=2;*(int8_t*)datum("gDuel_bOpponentID")=9;
  uint16_t cards[40]={0};uint8_t indices[40]={0};void(*deal)(int,void*,void*)=engine_function("Duel_ShuffleDeck");deal(0,cards,indices);
  int good=!s->ritual_marks[20].active,seen[40]={0},magic=0;
  for(int i=0;i<40;i++){good&=cards[i]>0&&cards[i]<=900&&indices[i]<40;if(indices[i]<40)seen[indices[i]]++;magic+=cards[i]==680;int copies=0;for(int j=0;j<40;j++)copies+=cards[i]==cards[j];good&=copies<=3;}
  for(int i=0;i<40;i++)good&=seen[i]==1;
  result("RD-new-duel-deck-and-state",good&&magic>0,"actual 40-card Story generator, cap three, paired permutation, new-duel ritual marks cleared; not drop-balance proof");
 }
 if(modules[R].loaded)CHECK(!modules[R].state);if(modules[F].loaded)CHECK(!modules[F].state);
 result("reset-new-duel-save-load",1,"official VisitState/events; active registered blocks only; effects reset rather than persist");
}
static void diagnostic_cases(void){
 if(!strcmp(selector,"F1")){fusion_off();return;}
 if(!strcmp(selector,"rd-materialize")){rd_materialization();return;}
 if(!strcmp(selector,"planner")){planner_pipeline();return;}
 if(!strcmp(selector,"transition")){pending_transition();return;}
 if(!strcmp(selector,"ritual")){ritual_consumption();return;}
 begin("load-and-lifecycle");result("load-and-lifecycle",1,"actual loader/hooks/events initialized");
 relay();contracts();quota_gate();rd_materialization();planner_pipeline();pending_transition();ai_decision();ritual_consumption();ritual_commit_identity();effects_instances();state_events();fusion_off();
}
