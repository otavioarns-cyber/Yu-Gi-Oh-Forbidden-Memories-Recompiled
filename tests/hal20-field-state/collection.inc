/* Consolidated real-provider probes; explicit seeded leases are test setup. */
#include "../../mods/fusion-auxiliary-balance/model.h"
#include "../../mods/fusion-auxiliary-balance/origin_effects_v1.h"
typedef struct {unsigned abi;int(*eligible)(int);} Encore;
typedef struct {unsigned abi;unsigned(*lease)(int);void(*revoke)(int);} Unlock;
typedef struct {int id;void*data;unsigned serial;} Lease;
static void provider_cases(void){
 setup(5);begin("providers-integrated");Encore*e=Mods_Find("fusion-auxiliary-balance:encore-v1");Unlock*u=Mods_Find("fusion-auxiliary-balance:origin-unlock-v1");OriginEffectsV1*o=Mods_Find("fusion-auxiliary-balance:origin-effects-v1");EaProvider*f=Mods_Find("fusion-auxiliary-balance:effect_forecast_v2");
 if(!Mods_Active(mi(F))){result("Fusion-provider-absent",!e&&!u&&!o&&!f,"Fusion never loaded in this profile; no substitute provider.");return;}
 CHECK(e&&u&&o&&f);DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");d[0].id=495;r[5]=(DuelCardRecord){0};r[5].card_id=495;r[5].data=&d[0];r[5].flags=DUEL_CARD_FLAG_OCCUPIED;
 Lease*l=local(F,"aux_unlocks");unsigned*serial=local(F,"aux_unlock_serial");CHECK(l&&serial);l[5]=(Lease){495,&d[0],77};*serial=77;
 result("F1-ON-contracts",Mods_CodeHash(mi(F))==0x69a6af85&&e->eligible(0)&&u->lease(5)==77&&o->lease(5)==77&&f->on(),"Exact PR22 loaded code and explicitly seeded physical lease77; four real contracts.");
 Mods_SetEnabled(mi(F),0);result("F1-restart-pending-stays-ON",!Mods_Enabled(mi(F))&&Mods_Active(mi(F))&&e->eligible(0)&&u->lease(5)==77,"Original restart:true preserved: saved preference differs from effective callback state.");Mods_SetEnabled(mi(F),1);
 Hal25_ForceApplied(mi(F),0);printf("F1 OFF encore=%d lease=%u extended=%u forecast=%d\n",e->eligible(0),u->lease(5),o->lease(5),f->on());
 result("F1-effective-OFF-neutral",!e->eligible(0)&&!u->lease(5)&&!o->lease(5)&&!f->on()&&*serial==77,"Owner OFF neutral with unchanged serial; no registry teardown.");result("F1-disabled-provider-still-findable",Mods_Find("fusion-auxiliary-balance:encore-v1")==e,"Real Mods_Find keeps owner records resident OFF.");
 Hal25_ForceApplied(mi(F),1);result("F1-reactivation-no-old-lease",e->eligible(0)&&!u->lease(5)&&!o->lease(5),"Encore current presence resumes; old grant never recreated.");
 void(__attribute__((fastcall))*bind)(int)=local(F,"aux_unlock_bind");CHECK(bind);bind(5);result("F1-new-grant-new-serial",u->lease(5)==78&&o->lease(5)==78&&*serial==78,"Real private bind callback with qualified compiler fastcall; new grant explicitly requested ON.");
 int out=0,kind=0;int pair=f->pair(61,658,0,&out,&kind);result("Metalmorph-forecast-unchanged",pair&&out==412&&kind==EA_METAL,"Existing approved 61+658 recipe in modeled native oracle; real Fusion forecast.");
 if(Mods_Active(mi(RE))){RitualRecipeProviderV1*recipe=Mods_Find("ritual-card-effects-test:recipe_v1");RitualRecipeRequirementV1 req[3];int result_card=0;result("Ritual-real-recipe-present",recipe&&recipe->get(666,req,3,&result_card)==3&&result_card==357,"Actual PR11 provider, no recipe provider stub.");
  out=kind=0;pair=f->pair(318,704,0,&out,&kind);int on=pair&&out==704;Hal25_ForceApplied(mi(RE),0);out=kind=0;pair=f->pair(318,704,0,&out,&kind);result("Egotist-real-Ritual-ON-OFF",on&&(!pair||!out),"Real effect_intelligence consumed; disabled producer grants no target.");Hal25_ForceApplied(mi(RE),1);
 }
 Mods_Reset();result("reset-clears-Fusion-lease",!u->lease(5),"Actual manager Reset; no artificial BSS save format.");
}
