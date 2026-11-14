/* Own integration probes. All artificial state seeds are labeled; never a UI gap. */
#include "../../mods/fusion-auxiliary-balance/model.h"
#include "../../mods/fusion-auxiliary-balance/origin_effects_v1.h"
#include "../../mods/melhoria-dos-rituais/ritual_recipe_provider_v1.h"
extern void Hal20_ForceApplied(int,int);
extern const MemoriesModHost*Hal20_Host(int);
static void*local(int m,const char*n){for(size_t i=0;i<modules[m].object.symbol_count;i++)if(!strcmp(modules[m].object.symbols[i].name,n))return(void*)modules[m].object.symbols[i].address;return NULL;}
static void force(int m,int on){CHECK(mi(m)>=0&&Mods_CodeHash(mi(m)));Hal20_ForceApplied(mi(m),on);printf("FORCED_APPLIED owner=%s actual=%d callback experiment bypasses restart only after admission\n",modules[m].id,Mods_Active(mi(m)));}
static void critical_audit(const char*phase){
 const char*targets[]={"func_8001BAF0","DuelScene_UpdateHandActions","AiScript_Run","func_80028220","Duel_CheckRitual","DuelEffect_ApplyRitual","Duel_PopulateCombinedDeckData"};
 for(size_t k=0;k<sizeof targets/sizeof*targets;k++) {printf("CHAIN phase=%s target=%s ",phase,targets[k]);for(int i=nr-1;i>=0;i--)if(!strcmp(registrations[i].name,targets[k])&&Mods_Active(mi(registrations[i].owner)))printf("%s -> ",modules[registrations[i].owner].id);puts("engine");}
 int owner,first=0;const void*target;for(int i=0;Hooks_At(i,&owner,&target);i++)if(Mods_Active(owner)){
  const char*n=target_name((void*)target);if(strcmp(n,"func_8001BAF0")&&strcmp(n,"DuelScene_UpdateHandActions"))continue;
  printf("CRITICAL index=%d owner=%s target=%s code=%08x phase=%s\n",i,Mods_Id(owner),n,Mods_CodeHash(owner),phase);
  int known=owner==mi(H)||owner==mi(RD)||owner==mi(D);if(!first&&((!strcmp(n,"func_8001BAF0")&&!known)||(!strcmp(n,"DuelScene_UpdateHandActions")&&owner!=mi(D)))){printf("FIRST_UNQUALIFIED owner=%s target=%s phase=%s\n",Mods_Id(owner),n,phase);first=1;}
 }
 for(int i=0;i<nr;i++)if(registrations[i].calls)printf("CALLBACK_COUNT owner=%s target=%s calls=%u phase=%s\n",modules[registrations[i].owner].id,registrations[i].name,registrations[i].calls,phase);
}
static void natural_lifecycle(void){
 int pending=pull()->active;printf("HAL23 boundary=after-complete-controller pending=%d; no manager call from planner/observer\n",pending);
 if(Mods_CodeHash(mi(RD)))result("HAL23-normal-controller-consumes-pending",!pending,"No artificial UI window or mid-planner activation. This is a synchronous native controller call, not Windows UI proof.");
 int old=Mods_Enabled(mi(H));Mods_SetEnabled(mi(H),!old);char why[512];int ready=Mods_Hal2224GuardReady(why,sizeof why);printf("LIFECYCLE between-controller-calls oldH=%d wantH=%d actualH=%d ready=%d pending=%d reason=%s\n",old,Mods_Enabled(mi(H)),Mods_Active(mi(H)),ready,pull()->active,ready?"none":why);Mods_SetEnabled(mi(H),old);
 result("HAL23-no-pending-created-by-manager-change",pull()->active==pending,"Preference/activation request strictly after controller returns; no pending injected/cleared to pass.");
}
typedef struct {unsigned abi;int(*eligible)(int);} Encore;
typedef struct {unsigned abi;unsigned(*lease)(int);void(*revoke)(int);} Unlock;
typedef struct {int id;void*data;unsigned serial;} Lease;
static void provider_cases(void){
 setup(5);begin("providers-integrated");Encore*e=Mods_Find("fusion-auxiliary-balance:encore-v1");Unlock*u=Mods_Find("fusion-auxiliary-balance:origin-unlock-v1");OriginEffectsV1*o=Mods_Find("fusion-auxiliary-balance:origin-effects-v1");EaProvider*f=Mods_Find("fusion-auxiliary-balance:effect_forecast_v2");
 if(!Mods_Active(mi(F))){result("Fusion-provider-absent",!e&&!u&&!o&&!f,"Fusion never loaded in this profile; no substitute provider.");return;}
 CHECK(e&&u&&o&&f);DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");d[0].id=495;r[5]=(DuelCardRecord){0};r[5].card_id=495;r[5].data=&d[0];r[5].flags=DUEL_CARD_FLAG_OCCUPIED;
 Lease*l=local(F,"aux_unlocks");unsigned*serial=local(F,"aux_unlock_serial");CHECK(l&&serial);l[5]=(Lease){495,&d[0],77};*serial=77;
 result("F1-ON-contracts",Mods_CodeHash(mi(F))==0xafb75202&&e->eligible(0)&&u->lease(5)==77&&o->lease(5)==77&&f->on(),"Exact PR22 loaded code and explicitly seeded physical lease77; four real contracts.");
 Mods_SetEnabled(mi(F),0);result("F1-restart-pending-stays-ON",!Mods_Enabled(mi(F))&&Mods_Active(mi(F))&&e->eligible(0)&&u->lease(5)==77,"Original restart:true preserved: saved preference differs from effective callback state.");Mods_SetEnabled(mi(F),1);
 force(F,0);printf("F1 OFF encore=%d lease=%u extended=%u forecast=%d\n",e->eligible(0),u->lease(5),o->lease(5),f->on());
 result("F1-effective-OFF-neutral",!e->eligible(0)&&!u->lease(5)&&!o->lease(5)&&!f->on()&&*serial==77,"Owner OFF neutral with unchanged serial; no registry teardown.");result("F1-disabled-provider-still-findable",Mods_Find("fusion-auxiliary-balance:encore-v1")==e,"Real Mods_Find keeps owner records resident OFF.");
 force(F,1);result("F1-reactivation-no-old-lease",e->eligible(0)&&!u->lease(5)&&!o->lease(5),"Encore current presence resumes; old grant never recreated.");
 void(__attribute__((fastcall))*bind)(int)=local(F,"aux_unlock_bind");CHECK(bind);bind(5);result("F1-new-grant-new-serial",u->lease(5)==78&&o->lease(5)==78&&*serial==78,"Real private bind callback with qualified compiler fastcall; new grant explicitly requested ON.");
 int out=0,kind=0;int pair=f->pair(61,658,0,&out,&kind);result("Metalmorph-forecast-unchanged",pair&&out==412&&kind==EA_METAL,"Existing approved 61+658 recipe in modeled native oracle; real Fusion forecast.");
 if(Mods_Active(mi(RE))){RitualRecipeProviderV1*recipe=Mods_Find("ritual-card-effects-test:recipe_v1");RitualRecipeRequirementV1 req[3];int result_card=0;result("Ritual-real-recipe-present",recipe&&recipe->get(666,req,3,&result_card)==3&&result_card==357,"Actual PR11 provider, no recipe provider stub.");
  out=kind=0;pair=f->pair(318,704,0,&out,&kind);int on=pair&&out==704;force(RE,0);out=kind=0;pair=f->pair(318,704,0,&out,&kind);result("Egotist-real-Ritual-ON-OFF",on&&(!pair||!out),"Real effect_intelligence consumed; disabled producer grants no target.");force(RE,1);
 }
 Mods_Reset();result("reset-clears-Fusion-lease",!u->lease(5),"Actual manager Reset; no artificial BSS save format.");
}
static void put_field(int slot,int id,int uid){DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");d[uid].id=id;DuelCardRecord*r=setup_record(slot,uid);r->object=spawn_hand(r,0,0);((uint8_t*)datum("D_800907D8"))[slot]=slot;}
static void put_hand(int slot,int id,int uid){DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelSideState*s=datum("D_800E9FF0");d[uid].id=id;s[0].hand[slot]=uid;DuelCardRecord*r=setup_record(slot,uid);((DuelHandSlot*)datum("D_800EA030"))[slot].object=spawn_hand(r,0,0);}
static void ritual_cases(void){
 if(!Mods_Active(mi(M))){puts("NOT_RUN ritual materials Melhoria absent");return;}int re=Mods_Active(mi(RE));
 for(int producer=0;producer<=re;producer++){
  if(re)force(RE,producer);
  for(int layout=0;layout<3;layout++){
   setup(5);begin("ritual-materials-public-hooks");*(uint8_t*)datum("D_8009B1D5")=0;DuelCardRecord*r=datum("D_801A7AD8");memset(r,0,30*sizeof*r);DuelSideState*s=datum("D_800E9FF0");memset(s[0].hand,-1,5);set_stat(357,1600,1800,0);uint16_t row[]={666,29,31,101,357,0};memcpy(datum("gDuel_awRitualData"),row,sizeof row);
   if(layout==0){put_field(5,29,0);put_field(6,31,1);put_field(7,101,2);put_field(8,31,3);}else if(layout==1){put_hand(0,29,0);put_hand(1,31,1);put_hand(2,101,2);put_hand(3,31,3);}else{put_field(5,101,0);put_hand(0,29,1);put_hand(1,31,2);put_hand(2,31,3);put_field(6,1,4);set_stat(1,3000,2500,0);}
   int(*check)(DuelRitualResult*,int)=engine_function("Duel_CheckRitual");void(*consume)(DuelCardRecord*)=engine_function("DuelCard_DeactivateRecord");DuelRitualResult out={0};int before[5];memcpy(before,s[0].hand,sizeof s[0].hand);int probe=check(NULL,666),found=check(&out,666);
   printf("RITUAL layout=%s provider=%d probe=%d result=%d min_field_default=1\n",layout==0?"Field":layout==1?"Hand":"Both",producer,probe,found);
   result("ritual-probe-preserves-hand",!memcmp(before,s[0].hand,sizeof s[0].hand),"Public search does not consume physical cards.");
   if(layout==1){result("Hand-only-rejected-default-min-field",!found&&!probe,"Approved min_field default1 preserved; no setting relaxed for PASS.");continue;}
   if(found==357)for(int k=0;k<3;k++)consume(&r[5+k*(layout==0)]);
   int good=found==357&&probe==357&&(layout==0?(!r[5].flags&&!r[6].flags&&!r[7].flags&&r[8].flags):(!r[5].flags&&s[0].hand[0]==-1&&s[0].hand[1]==-1&&s[0].hand[2]==3&&r[2].flags&&r[6].flags));
   result(layout==0?"Field-three-tributes-physical":"Both-tributes-spare-identical-copy",good,"Public Melhoria selector/consumer through real Ritual/Fusion hook stack; bounded native oracle, no animation/gameplay proof.");
  }
 }if(re)force(RE,1);
}
static unsigned char state_copy[NMOD][512];static int chunks;
static void save_chunk(int owner,void*p,size_t size,unsigned version,void*ctx){(void)ctx;int m=which(owner);CHECK(m>=0&&size<=512);memcpy(state_copy[m],p,size);chunks++;printf("STATE_CHUNK owner=%s size=%u version=%u\n",Mods_Id(owner),(unsigned)size,version);}
static void restore_chunk(int owner,void*p,size_t size,unsigned version,void*ctx){(void)version;(void)ctx;int m=which(owner);CHECK(m>=0&&size<=512);memcpy(p,state_copy[m],size);}
static void state_cases(void){setup(5);begin("save-reset-load-new-duel");MemoriesModEvent e={MEMORIES_EVENT_SAVE,MEMORIES_BEFORE,0,0,0,0,0};Mods_Dispatch(&e);Mods_VisitState(save_chunk,NULL);Mods_Reset();Mods_VisitState(restore_chunk,NULL);e=(MemoriesModEvent){MEMORIES_EVENT_LOAD,MEMORIES_AFTER,0,0,0,0,0};Mods_Dispatch(&e);result("save-load-registered-blocks-only",1,"Actual VisitState/SAVE/LOAD/reset; registered buffers only, no serializing unregistered BSS.");((void(*)(void))engine_function("Duel_InitScene"))();result("new-duel-and-reset-no-stale-pending",!pull()->active,"Actual reset/native-init entry, not complete duel/save compatibility.");}
static void extra_cases(const char*name){if(!strcmp(name,"providers"))provider_cases();else if(!strcmp(name,"ritual"))ritual_cases();else if(!strcmp(name,"state"))state_cases();else puts("NOT_RUN unknown probe");}

static int queen_setup(void){
 CHECK(Mods_Active(mi(RE)));begin("Queen-public-ritual-commit");put_field(5,708,0);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");d[44].id=395;
 /* Female physically belongs to CPU hand, with no duplicate record on human side. */
 DuelSideState*s=datum("D_800E9FF0");s[1].hand[4]=44;DuelCardRecord*r=setup_record(19,44);((DuelHandSlot*)datum("D_800EA030"))[4].object=spawn_hand(r,0,0);
 *(uint8_t*)datum("D_8009B1D5")=0;*(uint8_t*)datum("D_8009B19C")=5;*(int16_t*)datum("D_8009B1A0")=708;*(uint16_t*)datum("D_8009B210")=0x83;
 ((void(*)(void))engine_function("DuelEffect_ApplyRitual"))();
 unsigned*hint=local(RE,"cosmo_live_hint"),*mask=local(RE,"cosmo_live_mask");CHECK(hint&&mask);printf("QUEEN real_commit hint=%u mask=%x female_hand=395; no private effect mark written\n",*hint,*mask);
 *(uint8_t*)datum("D_8009B1D5")=1;*(uint16_t*)datum("gDuel_wSceneStateFlags")=4;setup_count=0;
 for(int i=0;i<nr;i++)registrations[i].calls=0;
 if(!*hint||!(*mask&(1u<<5))){puts("NOT_REACHED Queen public commit did not activate lock");return 0;}return 1;
}
