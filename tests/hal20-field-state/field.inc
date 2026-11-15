/* Real Field Effects callbacks; table reader is a manifest-backed host oracle.
 * Native terrain completion and stat packing bodies are loaded by run.py. */
#define gDuel_aTerrainBoost field_base_boost
#include "../../src/game/duel_terrain_boost.c"
#undef gDuel_aTerrainBoost
static const char*field_names[]={"","Forest","Wasteland","Mountain","Sogen","Umi","Yami"};
static const char*type_names[]={"Dragon","Spellcaster","Zombie","Warrior","Beast-Warrior","Beast","Winged Beast","Fiend","Fairy","Insect","Dinosaur","Reptile","Fish","Sea Serpent","Machine","Thunder","Aqua","Pyro","Plant","Rock"};
static int field_tables(int terrain,int type,int*out){
 if(terrain<1||terrain>6||type<0||type>=20||!Mods_Active(mi(E)))return 0;
 const JsonValue*v=Json_Member(Json_Member(Json_Member(Mods_Manifest(mi(E)),"terrain_bonus"),field_names[terrain]),type_names[type]);if(!v)return 0;*out=Json_Number(v,0);return 1;
}
static int field_primary(int type){int f=*(uint8_t*)datum("gDuel_bTerrain"),v;if(field_tables(f,type,&v))return v;return f>=1&&f<=6&&type>=0&&type<20?field_base_boost[type][f-1]*10:0;}
static void field_seed(void){memcpy(datum("gDuel_aTerrainBoost"),field_base_boost,sizeof field_base_boost);}
static void field_complete(int field){
 static unsigned char request[512];memset(request,0,sizeof request);*(void**)datum("D_8009B17C")=request;
 *(uint8_t*)datum("gDuel_bTerrain")=field;*(uint16_t*)datum("gDuel_wCardEffectFlags")=0x60;
 ((void(*)(void))engine_function("DuelEffect_ApplyTerrain"))();
}
static void field_cases(void){
 int f=Settings_GetNamed("test.field",0),on=Mods_Active(mi(E));setup(5);field_seed();begin("field-live-records");
 typedef int(*Delta)(int,int,int);Delta delta=Mods_Find("field-effects-secondary-types:terrain-card-delta-v1");
 result("field-optional-provider",on?delta!=0:delta==0,"Cold absent/OFF profile creates no pretend provider.");
 set_stat(22,2500,1200,CARD_TYPE_FIEND);set_stat(438,1600,1900,CARD_TYPE_MACHINE);set_stat(441,1800,1500,CARD_TYPE_MACHINE);set_stat(3,500,700,CARD_TYPE_ZOMBIE);
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");d[0].id=22;d[1].id=438;d[2].id=438;d[3].id=3;
 DuelCardRecord*(*create)(int,int)=engine_function("Duel_SetupCardRecord");DuelCardRecord*r=datum("D_801A7AD8");field_complete(f);create(5,0);create(6,1);create(7,2);create(8,3);
 int expected[4]={field_primary(CARD_TYPE_FIEND),field_primary(CARD_TYPE_MACHINE),field_primary(CARD_TYPE_MACHINE),field_primary(CARD_TYPE_ZOMBIE)};
 for(int i=0;i<4;i++)if(on)expected[i]+=delta(d[i].id,card_type(d[i].id),f);
 for(int i=0;i<4;i++){printf("FIELD card=%d type=%d terrain=%d on=%d actual=%d expected=%d\n",r[5+i].card_id,card_type(r[5+i].card_id),f,on,r[5+i].terrain_modifier,expected[i]);result("setup-real-primary-and-secondary",r[5+i].terrain_modifier==expected[i],"Real Field setup callback after native setup oracle; exact manifest terrain table.");}
 result("field-identities-and-spare-copy",r[6].data==&d[1]&&r[7].data==&d[2]&&r[6].data!=r[7].data,"Same ID is not a shared physical instance; no material consumed.");
 r[5].stat_modifier=123;r[5].defense_modifier=47;field_complete(f);unsigned stats=((unsigned(*)(DuelCardRecord*))engine_function("Duel_CalcCardStats"))(&r[5]);
 result("native-stat-pack-both-axes",(stats&65535)==2500+123+expected[0]&&(stats>>16)==1200+123+47+expected[0],"Actual native stat packer plus real R/F wrappers; permanent ATK/DEF modifiers retained.");
 field_complete(f?0:6);field_complete(f);result("field-replacement-no-stacking",r[5].terrain_modifier==expected[0]&&r[5].stat_modifier==123&&r[5].defense_modifier==47,"Replacement/removal/reapply does not accumulate terrain or erase independent effects.");
 if(on){
  int before=delta(22,CARD_TYPE_FIEND,6);Mods_SetEnabled(mi(E),0);result("Field-pending-restart-keeps-provider",Mods_RequiresRestart(mi(E))&&Mods_Active(mi(E))&&!Mods_Enabled(mi(E))&&delta(22,CARD_TYPE_FIEND,6)==before,"Real rule-table manifest requires restart: saved OFF must not hide the effective ON bonus.");Mods_SetEnabled(mi(E),1);
  Hal25_ForceApplied(mi(E),0);result("Field-effective-OFF-neutral",delta(22,CARD_TYPE_FIEND,6)==0,"Callback effective state, preference still ON; provider remains resident.");Hal25_ForceApplied(mi(E),1);
  if(Mods_Hal2224GuardEnabled()){
   char why[256]={0};Registration*binding=NULL;
   for(int i=0;i<nr;i++)if(registrations[i].owner==E&&!strcmp(registrations[i].name,"func_80035E20")){binding=&registrations[i];break;}CHECK(binding&&binding->previous);
   unsigned before_setup=setup_count;int before_hand=hand_count();void*old=*binding->previous;*binding->previous=harmless_wrapper;
   result("Field-text-predecessor-tamper-refused",!Mods_Hal2224GuardReady(why,sizeof why),"The added text target is checked by actual predecessor; no text draw or duel action executed.");*binding->previous=old;CHECK(Mods_Hal2224GuardReady(why,sizeof why));
   void*previous=NULL;int token=Hooks_Add(mi(E),engine_function("func_80035E20"),binding->replacement,&previous);CHECK(token);
   result("Field-duplicate-qualified-callback-refused",!Mods_Hal2224GuardReady(why,sizeof why),"An otherwise correct Field callback cannot be registered twice.");Hooks_Remove(mi(E),token);CHECK(Mods_Hal2224GuardReady(why,sizeof why));
   result("Field-refusals-do-not-change-cards",before_setup==setup_count&&before_hand==hand_count(),"Metadata checks do not consume, restore or materialize instances.");
  }
  if(Mods_Active(mi(H))){int(*terrain)(int,int,int)=local(H,"terrain_card");CHECK(terrain);result("AI-real-per-card-field",terrain(22,CARD_TYPE_FIEND,6)==700&&terrain(438,CARD_TYPE_MACHINE,5)==200,"Actual rebuilt adapter uses native primary and real Field provider; secondary is not double-counted.");}
 }
 if(on&&f==4){
  set_stat(30,1200,900,CARD_TYPE_ZOMBIE);
  void(__attribute__((fastcall))*summon)(int)=local(F,"r2_summon_zombie");CHECK(summon);
  summon(0);printf("FUSION_SYNTHETIC id=%d terrain=%d delta=%d\n",r[9].card_id,r[9].terrain_modifier,delta(30,CARD_TYPE_ZOMBIE,4));
  result("Fusion-real-generated-card-field",r[9].card_id==30&&r[9].data!=&d[0]&&r[9].terrain_modifier==field_primary(CARD_TYPE_ZOMBIE)+delta(30,CARD_TYPE_ZOMBIE,4),"Call the real production helper with approved generated Zombie Warrior30; private per-card backing data, not an injected card or copied algorithm.");
 }
 Mods_Reset();result("field-reset-keeps-instance-stats",r[5].terrain_modifier==expected[0]&&r[5].stat_modifier==123,"Reset clears description cache, not approved physical card stats.");
 setup(5);field_seed();*(uint8_t*)datum("gDuel_bTerrain")=f;queen_setup();begin("field-integrated-controller");int phase=hand_action();result("field-controller-one-Queen-legal-action",action_card()==395&&hand_count()==4&&setup_count<=1&&!pull()->active&&conserved(),"Real planners/materializers with Queen and Field at this terrain; no injected pending/plan/quota.");
 if(Mods_Active(mi(D)))result("field-Dual-definitive-quota",ds()[1]==action_category(),"Real Dual gate and ledger under the selected field; AI ON/OFF.");(void)phase;
}
