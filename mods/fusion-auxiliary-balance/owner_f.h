#include "model.h"
#include "pc/cards/rules.h"
static int ea_applied=1;
static int aux_owner_applied(void)
{ return ea_applied && aux_host; }
static void ea_apply(int on)
{
    ea_applied=on;
    if(!on)memset(aux_unlocks,0,sizeof(aux_unlocks));
}
static int ea_enabled(void){return aux_owner_applied()&&aux_host&&aux_host->setting(aux_host,"effect_ai",1);}
static void ea_inspect(EaCard*c,int s,int viewer){
 if(!ea_enabled())return;int id=c->id,live=aux_monster_slot(s)&&D_801A7AD8[s].card_id==id&&(D_801A7AD8[s].flags&DUEL_CARD_FLAG_OCCUPIED);
 if(aux_dragon_like(id))c->traits|=EA_DRAGON;if(RitualCategory_IsSkullMonster(id))c->traits|=EA_SKULL;
 if(id==358||id==168||id==2||id==551||id==115||id==495||id==100||id==369||(id==189&&aux_enabled("fusionist_effects"))){
 c->possible|=EA_BASE;if(!live||!(c->flags&4096))c->active|=EA_BASE;}
 if(aux_is_fusion_family(id)&&r2_setting_enabled(id)&&(id!=189||aux_enabled("fusionist_effects"))){
 c->possible|=EA_FUSION;if(live?aux_fusion_special(s,0):aux_ignore_conditions())c->active|=EA_FUSION;}
 if(aux_is_metal_family(id)){c->possible|=EA_METAL;if(live?aux_metal_protected(s):aux_ignore_conditions())c->active|=EA_METAL;}
 if(id==533||id==272)c->charge=!live||!r3_seen_data((void*const*)(id==533?r3_kwagar_spent:r3_mavelus_spent),id==533?r3_kwagar_count:r3_mavelus_count,live?D_801A7AD8[s].data:0);
 if(id==2)c->charge=!live||aux_elf_bound_data[s]!=D_801A7AD8[s].data||!aux_elf_healed[s];(void)viewer;}
static int ea_policy(int s,int q){if(s<0||s>1||!ea_enabled())return 0;return q?kamion_cpu_learned[s]&&aux_has_kamion(s^1):r3_kaminari_attacks_blocked(s);}
static int ea_recipe(int a,int b){int x=aux_authorized_fusion(a,b),r=x?x:CardRules_Fusion(a,b);return aux_fusion_result_allowed(r,x)?r:0;}
static unsigned ea_deterministic(unsigned n){return n-1;}
static int ea_pair(int a,int b,int active,int*result,int*origin){
 if(!ea_enabled())return 0;int r=0,o=EA_FUSION,eq=a==658||b==658||a==318||b==318;
 if(a==189&&b==189)r=0;
 else if(a==318||b==318){int c=a==318?b:a;if(c==62){r=63;o=0;}else if(aux_egotist_target(c)){r=c;o=EA_FUSION|EA_METAL|EA_RITUAL;}}
 else if(aux_enabled("fusionist_effects")&&(b==189||(a==189&&(active&EA_FUSION)))){
 int n=gCard_nCount,sub=0;if(n>0&&n<=32767){unsigned char*seen=malloc(n+1);if(seen){AuxFusionCatalog cat={n,ea_recipe,aux_catalog_monster,aux_catalog_stat,ea_deterministic};r=aux_fusionist_pick(&cat,b==189?a:b,b!=189,seen,&sub);free(seen);}}if(eq)o=EA_METAL;}
 else {r=ea_recipe(a,b);if(eq)o=EA_METAL;}
 if(r&&Tower_Refuses(eq?TOWER_RULE_NO_EQUIPS:TOWER_RULE_NO_FUSIONS,a,b,r))r=0;
 *result=r;*origin=o;return 1;}
static const EaProvider ea_owner={2,ea_enabled,ea_inspect,ea_policy,ea_pair};
