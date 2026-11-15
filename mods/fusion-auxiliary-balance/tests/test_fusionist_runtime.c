#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CARD_STAT_DEFENSE_SHIFT 9
#define CARD_STAT_VALUE_MASK 511
#define CARD_STAT_SCALE 10
static unsigned gDuel_adwCardStats[900];
#define CARD_TYPE_DRAGON 0
#define CARD_TYPE_THUNDER 7
#define CARD_TYPE_ZOMBIE 2
#define CARD_TYPE_BEAST_WARRIOR 4
#define CARD_TYPE_BEAST 5
#define CARD_TYPE_PLANT 19
#include "../logic.h"
#include "../ritual_effect_intelligence_v1.h"
typedef int s32;
typedef unsigned char u8;
typedef struct {int card_id,flags;void *data;} DuelCardRecord;
typedef struct {int field_6A;} DisplayObject;
typedef struct Host {int (*setting)(const struct Host*,const char*,int);void (*log)(const struct Host*,const char*,...); void *(*find)(const struct Host*,const char*);} Host;
typedef struct {int phase,handled,a,b,result;} MemoriesModEvent;
#define MEMORIES_BEFORE 0
#define CARD_TYPE_MAGIC 20
#define TOWER_RULE_NO_FUSIONS 1
#define TOWER_RULE_NO_EQUIPS 2
#define DUEL_SCENE_PHASE_MASK 15
#define DUEL_CARD_FLAG_OCCUPIED 1
#define DUEL_CARD_FLAG_FACE_DOWN 2
#define AUX_SIDE_STRIDE 15
/* Standalone ON model; actual applied(0/1) is covered by the ELF lifecycle suite. */
static int aux_owner_applied(void){return 1;}
static int effect_on=1,equip_on=1,debug_on,refuse,refuse_fusion,refuse_equip;
static int kamion_cpu_learned[2],kamion_present[2];
static int aux_has_kamion(int side){return kamion_present[side];}
static int setting(const Host*h,const char*k,int d){(void)h;(void)d;return !strcmp(k,"fusionist_effects")?effect_on:!strcmp(k,"elegant_egotist")?equip_on:debug_on;}
static int ritual_on=1;
static int ritual_enabled(int id){return ritual_on && id==380;}
static RitualEffectIntelligenceV1 ritual_api={1,ritual_enabled,0};
static void *find_api(const Host*h,const char*key){(void)h;return !strcmp(key,"ritual-card-effects-test:effect_intelligence_v1")?&ritual_api:0;}
static Host host={setting,0,find_api};static const Host *aux_host=&host;
static DuelCardRecord D_801A7AD8[30];
static DisplayObject objects[2],*D_800E9EF0[2]={&objects[0],&objects[1]};
static struct {int fusion_path[6],fusion_depth;} gAiScript_State;
static int D_8009B1D5,gDuel_wSceneStateFlags,D_8009B174,D_8009B19C;
static int fusion_origin[30],metal_immune[30],fusion_bound_id[30],metal_bound_id[30];
static void *fusion_bound_data[30],*metal_bound_data[30];
static int metal_placement_check,ai_scoring,gCard_nCount=900;
static int fusion_pending_result,metal_pending_result,fusion_pending_side,metal_pending_side,placement_was_fusion;
static int recipe[901][901],stats[901][2],types[901],catalog_calls;
static int Cards_Valid(int id){return id>0 && id<=gCard_nCount;}
static int Cards_BaseId(int id){return id;}
static int Cards_Type(int id){return Cards_Valid(id)?types[id]:99;}
static int aux_monster_slot(int i){return i>=0&&i<30&&i%15>=5&&i%15<10;}
static int r2_setting_enabled(int id){(void)id;return 1;}
static int aux_ignore_conditions(void){return debug_on;}
static int aux_cpu_keep_darkfire(int a,int b){(void)a;(void)b;return 0;}
static int aux_authorized_fusion(int a,int b){return recipe[a][b];}
static int Tower_Refuses(int rule,int a,int b,int r){(void)a;(void)b;(void)r;return refuse || (rule==TOWER_RULE_NO_FUSIONS?refuse_fusion:refuse_equip);}
static int base_stat(int id,int which){return stats[id][which];}
static int resolve(int a,int b){++catalog_calls;return Cards_Valid(a)&&Cards_Valid(b)?recipe[a][b]:0;}
static s32 (*original_fusion)(s32,s32)=resolve;
static s32 (*original_base_stat)(s32,s32)=base_stat;
static int Tables_Fusion(int a,int b,int*r){(void)a;(void)b;(void)r;return 0;}
static int Tables_FilterFusion(int r){return r;}
#include "../fusionist_runtime.h"
static int equip_resolve(int a,int b){return !refuse&&equip_on&&a==318&&Cards_Type(b)<20?b:0;}
static int fired;
static void aux_queue_lp(int a,int b,int c,int d){(void)a;(void)b;(void)c;(void)d;++fired;}
static void aux_queue_skull_effect(int side){(void)side;++fired;}
static void aux_darkfire_generate_side(int side,const char*r){(void)side;(void)r;++fired;}
static void aux_kamion_refresh(void){}
static void aux_musician_attack_only(void){}
static void finalize_native(void){D_8009B174|=0x40;D_801A7AD8[D_8009B19C].flags|=1;}
static void (*original_card_finalize)(void)=finalize_native;
/* This generated include contains the actual functions extracted from mod.c,
 * not a second implementation of the finalization and eligibility rules. */
#include "finalize_under_test.h"
static void add(int a,int b,int r){recipe[a][b]=recipe[b][a]=r;}
static void start(int id){for(int i=1;i<=900;i++)gDuel_adwCardStats[i-1]=(stats[i][0]/10)|((stats[i][1]/10)<<9);aux_new_effects_reset();metal_placement_check=1;objects[0].field_6A=0;objects[1].field_6A=1;D_801A7AD8[0].card_id=id;aux_chain_active=0;}
static void finish(int id,int slot){D_8009B19C=slot;gDuel_wSceneStateFlags=8;D_8009B174=4;D_801A7AD8[slot].card_id=id;D_801A7AD8[slot].data=&stats[slot];aux_card_finalize();metal_placement_check=0;}
static void ai_inner(int id){assert(id==189);assert(aux_check_fusion(id,658)==392);}
static void ai_outer(int id){assert(aux_check_fusion(id,282)==189);gAiScript_State.fusion_depth++;original_complete_fusion=ai_inner;aux_complete_fusion(189);}
static int future_enabled=1;
static int future_effect(int id){return future_enabled && id==850;}
int main(void)
{
    original_equip=equip_resolve;types[318]=23;types[658]=23;types[800]=20;types[801]=22;
    /* Unsupported monsters remain rejected even with blanket equip grants. */
    assert(!aux_check_equip(318,1));assert(!aux_check_equip(318,208));
    assert(!aux_check_equip(318,850));assert(!aux_check_equip(318,800));
    assert(aux_register_effect_provider("future-mod",future_effect));
    assert(aux_check_equip(318,850)==850);
    start(850);assert(!aux_check_fusion(850,318));assert(aux_check_equip(318,850)==850);
    finish(850,5);assert(aux_origin_effects_api.lease(5));
    future_enabled=0;assert(!aux_check_equip(318,850));future_enabled=1;
    assert(aux_register_effect_provider("future-mod",0));assert(!aux_check_equip(318,850));
    assert(!aux_register_effect_provider("",future_effect));
    ritual_on=0;assert(!aux_check_equip(318,380));ritual_on=1;
    assert(aux_check_equip(318,380)==380);
    effect_on=0;assert(!aux_check_equip(318,189));effect_on=1;
    MemoriesModEvent event={MEMORIES_BEFORE,0,318,1,1};aux_egotist_event(&event);
    assert(event.handled && !event.result);
    event=(MemoriesModEvent){MEMORIES_BEFORE,1,318,189,0};aux_egotist_event(&event);
    assert(!event.result); /* Earlier protection is preserved. */
    start(62);assert(aux_check_fusion(62,318)==63);assert(!aux_chain.fused&&!aux_chain.egotist);
    start(318);assert(aux_check_fusion(318,62)==63);
    refuse=1;assert(!aux_check_fusion(62,318));assert(!aux_check_equip(318,189));refuse=0;
    equip_on=0;assert(aux_check_fusion(62,318)==63);equip_on=1;
    add(208,642,483);add(391,658,392);add(367,658,390);add(4,658,409);
    stats[392][0]=3000;stats[392][1]=2300;stats[390][0]=2600;stats[409][0]=1850;
    assert(aux_plain_fusion(208,282)==189 && aux_plain_fusion(282,208)==189);
    assert(aux_plain_fusion(208,642)==483 && aux_plain_fusion(642,208)==483);
    start(208);assert(aux_check_fusion(208,282)==189);assert(aux_chain.fused);
    assert(aux_check_fusion(189,658)==392);assert(aux_chain.metal&&!aux_chain.fused&&!aux_chain.egotist);
    finish(392,5);assert(!fusion_origin[5]&&metal_immune[5]);
    start(282);assert(aux_check_fusion(282,208)==189);assert(aux_check_fusion(189,658)==392);
    start(391);assert(aux_check_fusion(391,189)==392);assert(!aux_chain.metal&&aux_chain.virtual_material==658);
    finish(392,5);assert(fusion_origin[5]&&!metal_immune[5]);
    start(189);assert(aux_check_fusion(189,658)==0);
    assert(aux_check_fusion(189,189)==0);
    debug_on=1;assert(aux_check_fusion(189,658)==392);assert(aux_check_fusion(189,189)==0);debug_on=0;
    start(318);assert(!aux_check_fusion(318,189));assert(aux_check_equip(189,318)==0);
    assert(aux_check_equip(318,189)==189);assert(aux_chain.egotist&&!aux_chain.fused);
    assert(aux_check_fusion(189,658)==392);assert(!aux_chain.egotist&&aux_chain.metal);
    start(189);assert(!aux_check_fusion(189,318));assert(aux_check_equip(318,189)==189);
    assert(aux_check_fusion(189,658)==392);
    /* A second genuine fusion must not inherit the previous metal material. */
    add(392,50,409);assert(aux_check_fusion(392,50)==409);assert(!aux_chain.metal&&aux_chain.fused);
    /* Field identity, copies and replacement. */
    start(189);D_801A7AD8[5].card_id=189;D_801A7AD8[5].data=&stats[5];
    fusion_origin[5]=1;fusion_bound_id[5]=189;fusion_bound_data[5]=D_801A7AD8[5].data;
    objects[0].field_6A=5;assert(aux_check_fusion(189,658)==392);
    aux_chain_active=0;fusion_bound_data[5]=&stats[6];assert(!aux_check_fusion(189,658));
    /* Egotist summons a previously ordinary Meteor's effect ONCE. */
    start(713);assert(!aux_check_fusion(713,318));assert(aux_check_equip(318,713)==713);
    fired=0;finish(713,5);assert(fired==1&&aux_unlock_lease(5)&&!fusion_origin[5]);
    aux_card_finalize();assert(fired==1);
    D_801A7AD8[6]=D_801A7AD8[5];D_801A7AD8[6].data=&stats[6];assert(!aux_unlock_lease(6));
    assert(aux_fusion_special(5,0));aux_unlock_revoke(5);assert(!aux_fusion_special(5,0));
    /* Equipment transformations pass NO_FUSIONS and obey NO_EQUIPS. */
    refuse_fusion=1;start(62);assert(aux_check_fusion(62,318)==63);
    start(391);assert(aux_check_fusion(391,658)==392);
    start(208);assert(!aux_check_fusion(208,282));refuse_fusion=0;
    refuse_equip=1;start(62);assert(!aux_check_fusion(62,318));
    start(391);assert(!aux_check_fusion(391,658));
    start(208);assert(aux_check_fusion(208,282)==189);refuse_equip=0;
    kamion_present[1]=1;D_8009B1D5=0;start(62);
    assert(!aux_check_fusion(62,318));
    start(391);assert(!aux_check_fusion(391,658));
    kamion_present[1]=0;
    /* Ultimate receives no Fusion Effect, even when equipment is attached. */
    start(380);assert(!aux_check_fusion(380,318));assert(aux_check_equip(318,380)==380);finish(380,5);
    assert(aux_unlock_lease(5)&&!aux_fusion_special(5,0)&&!fusion_origin[5]);
    /* Unsupported results and partner categories; asymmetric mod recipe. */
    start(800);recipe[60][800]=51;stats[51][0]=10;assert(aux_check_fusion(800,189)==51);
    add(801,61,800);assert(!aux_check_fusion(801,189));add(801,62,52);assert(aux_check_fusion(801,189)==52);
    /* Printed result stats, ATK then DEF; duplicate recipe paths count once. */
    add(70,71,72);add(70,73,74);add(70,75,72);add(70,76,77);
    stats[72][0]=stats[74][0]=100;stats[72][1]=300;stats[74][1]=200;stats[77][0]=200;
    start(70);assert(aux_check_fusion(70,189)==74);
    start(189);debug_on=1;assert(aux_check_fusion(189,70)==77);debug_on=0;
    stats[72][1]=stats[74][1]=200;stats[77][0]=100;stats[77][1]=200;
    {int counts[3]={0},k;for(k=0;k<12000;k++){int r;start(70);r=aux_check_fusion(70,189);counts[r==72?0:r==74?1:2]++;}
     for(k=0;k<3;k++)assert(counts[k]>3700&&counts[k]<4300);}
    /* Settings and table changes take effect without a stale recipe cache. */
    effect_on=0;start(70);assert(aux_check_fusion(70,189)==recipe[70][189]);effect_on=1;
    equip_on=0;start(189);assert(!aux_check_fusion(189,318));assert(!aux_check_equip(318,189));equip_on=1;
    refuse=1;start(391);assert(!aux_check_fusion(391,189));refuse=0;
    /* AI search has a scoped provenance stack; previews never consume RNG. */
    metal_placement_check=0;gAiScript_State.fusion_depth=1;gAiScript_State.fusion_path[0]=11;
    original_complete_fusion=ai_outer;aux_complete_fusion(208);assert(!aux_ai_depth);
    aux_ai_depth=1;memset(&aux_ai_card,0,sizeof(aux_ai_card));aux_ai_card.id=189;
    assert(aux_check_equip(189,318)==189&&aux_ai_next.egotist);aux_ai_depth=0;
    puts("PASS: runtime chains, field identity, actual metal, finalization, equip triggers, AI scope, ranking and uniform distinct ties");
    return 0;
}
