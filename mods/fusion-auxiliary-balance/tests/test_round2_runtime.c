#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int s32;typedef unsigned char u8;typedef unsigned short u16;typedef short s16;
#define CARD_STAT_VALUE_MASK 511
#define CARD_STAT_DEFENSE_SHIFT 9
#define CARD_STAT_SCALE 10
#define CARD_TYPE_DRAGON 0
#define CARD_TYPE_INSECT 9
#define CARD_FUSION_GROUP_FEMALE 8
#define DUEL_CARD_FLAG_OCCUPIED 0x8000
#define DUEL_CARD_FLAG_FACE_DOWN 0x1000
#define DUEL_CARD_FLAG_DEFENSE_POSITION 0x800
#define DUEL_SCENE_PHASE_MASK 15
#define AUX_MUSICIAN_KING 495
#include "../fusion_round2_rules.h"
typedef struct {int card_id,attack,defense,stat_modifier,defense_modifier,terrain_modifier,flags,table_index;void *data,*object;} DuelCardRecord;
typedef struct {int field_6A,card_index;} DisplayObject;
typedef DisplayObject DuelCardDisplayObject;
typedef struct {int id,deck_index,data_block_index;} DuelDeckCardRecord;
static DuelCardRecord D_801A7AD8[30];
static unsigned gDuel_adwCardStats[722];
static int types[723],attrs[723],female[723],enabled[723],lease[30];
static int fusion_origin[30],metal_immune[30],fusion_bound_id[30],metal_bound_id[30];
static void *fusion_bound_data[30],*metal_bound_data[30];
static int aux_elf_bound_id[30],aux_elf_healed[30];static void *aux_elf_bound_data[30];
static int battle_record[2],battle_card[2],battle_saved_flags[2],battle_guarded[2];static void *battle_saved_data[2];
static int D_8009B178[2],D_8009B22A,D_8009B174,D_8009B1B0[2],D_8009B1A4[2];
static int gDuel_wSceneStateFlags=5,gDuel_bEffectState,gDuel_wCardEffectFlags;
static unsigned char gDuel_bTerrain[1];
static struct {struct{unsigned short unsigned_value;}life_points;int max_life_points;}D_800E9FF0[2];
static struct {int x,y;}D_800908A0[30];
static struct Host {void *(*find)(const struct Host*,const char*);} *aux_host;
static int aux_monster_slot(int i){return i>=0&&i<30&&i%15>=5&&i%15<10;}
static int Cards_Valid(int id){return id>0&&id<=722;}
static int Cards_Type(int id){return types[id];}
static int Cards_Attribute(int id){return attrs[id];}
static int Cards_AttributeNamed(const char *s){assert(!strcmp(s,"Earth"));return 2;}
static int Cards_InFusionGroup(int id,int group){assert(group==8);return female[id];}
static int r2_setting_enabled(int id){return enabled[id];}
static int aux_ignore_conditions(void){return 0;}
static unsigned aux_unlock_lease(int i){return lease[i];}
static void aux_unlock_revoke(int i){lease[i]=0;}
static int aux_fusion_special(int i,int lifted){return enabled[D_801A7AD8[i].card_id]&&!(D_801A7AD8[i].flags&0x1000)&&(lifted||(D_801A7AD8[i].flags&0x8000))&&((fusion_origin[i]&&fusion_bound_id[i]==D_801A7AD8[i].card_id&&fusion_bound_data[i]==D_801A7AD8[i].data)||lease[i]);}
static int Tables_StatCap(int defense){(void)defense;return 9999;}
static s32 Duel_CalcCardStats(DuelCardRecord*);
static int star_bonus;
static int Duel_CalcBattleAttack(DuelCardRecord*a,DuelCardRecord*b){(void)b;return (u16)Duel_CalcCardStats(a)+star_bonus;}
static int Duel_GetTerrainBoost(int t){(void)t;return gDuel_bTerrain[0]?500:0;}
static DisplayObject objects[30];
static int object_cursor,thumbnail_calls,stat_fx_count,stat_fx_slot[30],stat_fx_amount[30];
static DuelCardDisplayObject *func_80024C1C(int id,int x,int y){(void)id;(void)x;(void)y;return &objects[(object_cursor++)%30];}
static void DisplayObject_ReleaseIfPresent(void *object){(void)object;}
static void aux_queue_stat_effect(int slot,int amount){assert(stat_fx_count<30);stat_fx_slot[stat_fx_count]=slot;stat_fx_amount[stat_fx_count++]=amount;}
static void Duel_ApplyCardObjectFlags(DuelCardDisplayObject*p){(void)p;}
static int aux_darkfire_card_thumbnail(int i,int id){(void)i;(void)id;thumbnail_calls++;return 1;}
static void aux_dark_elf_react_to_healing(int side,int amount){(void)side;(void)amount;}
#include "../fusion_round2_runtime.h"
static s32 raw_stats(DuelCardRecord*c){int at=r2_limit(c->attack+c->stat_modifier+c->terrain_modifier,9999),df=r2_limit(c->defense+c->stat_modifier+c->defense_modifier+c->terrain_modifier,9999);return (s32)(((unsigned)df<<16)|at);}
static s32 Duel_CalcCardStats(DuelCardRecord*c){return r2_stats(c,raw_stats(c));}
static int at(int s){return (u16)Duel_CalcCardStats(D_801A7AD8+s);}
static int df(int s){return (unsigned)Duel_CalcCardStats(D_801A7AD8+s)>>16;}
static void put(int slot,int id,int attack,int defense,int origin){DuelCardRecord*c=&D_801A7AD8[slot];memset(c,0,sizeof(*c));c->card_id=id;c->attack=attack;c->defense=defense;c->data=&objects[slot];c->flags=0x8000;fusion_origin[slot]=origin;fusion_bound_id[slot]=id;fusion_bound_data[slot]=c->data;}
static void clear(void){memset(D_801A7AD8,0,sizeof(D_801A7AD8));memset(fusion_origin,0,sizeof(fusion_origin));memset(lease,0,sizeof(lease));memset(battle_guarded,0,sizeof(battle_guarded));memset(types,0xff,sizeof(types));memset(attrs,0xff,sizeof(attrs));memset(female,0,sizeof(female));for(int i=0;i<723;i++)enabled[i]=1;gDuel_bTerrain[0]=0;star_bonus=0;D_8009B22A=0;D_8009B174=0;r2_reset();r2_full_reset();object_cursor=thumbnail_calls=stat_fx_count=0;D_800E9FF0[0].life_points.unsigned_value=D_800E9FF0[1].life_points.unsigned_value=4000;D_800E9FF0[0].max_life_points=D_800E9FF0[1].max_life_points=8000;}
static void begin(int a,int d){int slots[2]={a,d};for(int p=0;p<2;p++){int s=slots[p];battle_record[p]=s;battle_card[p]=D_801A7AD8[s].card_id;battle_saved_data[p]=D_801A7AD8[s].data;battle_saved_flags[p]=D_8009B178[p]=D_801A7AD8[s].flags;D_801A7AD8[s].flags&=~0x8000;}r2_begin();r2_prepare(2);}
static void post(int dead0,int dead1){r2_battle.resolved=1;r2_battle.death[0]=dead0;r2_battle.death[1]=dead1;if(!dead0)D_801A7AD8[r2_battle.slot[0]].flags|=0x8000;if(!dead1)D_801A7AD8[r2_battle.slot[1]].flags|=0x8000;r2_end();r2_post_battle();}
static int compare_native(DisplayObject*a,DisplayObject*b){return at(a->field_6A)-df(b->field_6A);}
int main(void){
 /* Permanent adjacency, empty neighbours and stacking; source departure. */
 clear();put(5,613,2800,2100,1);put(22,100,100,100,0);put(21,101,500,500,0);put(23,102,500,500,0);put(24,103,500,500,0);begin(5,22);post(0,1);assert(at(21)==300&&df(23)==300&&at(24)==500);
 assert(stat_fx_count==2&&stat_fx_slot[0]==21&&stat_fx_slot[1]==23&&stat_fx_amount[0]==200&&stat_fx_amount[1]==200);
 put(22,100,100,100,0);begin(5,22);post(0,1);
 assert(at(21)==100&&at(23)==100&&stat_fx_count==4);
D_801A7AD8[5].flags=0;assert(at(21)==100);
 put(5,613,2800,2100,1);put(22,100,100,100,0);begin(5,22);post(0,1);assert(at(21)==0);
 /* Skull Knight: one effect summon, no fusion origin, no spell-zone overwrite. */
 clear();put(5,707,2650,2250,1);put(20,100,100,100,0);begin(5,20);post(0,1);assert(D_801A7AD8[6].card_id==30&&!fusion_origin[6]&&!lease[6]);
 /* A later renderer/scene may corrupt a synthetic instance. Repair ONLY the
    original occupied data lease; never overwrite a replaced real card. */
 {DuelCardRecord *z=&D_801A7AD8[6];
  assert(r2_zombie_lease[6]==z->data&&thumbnail_calls==1);
  z->stat_modifier=200;r2_reconcile_zombies(0);
  assert(z->stat_modifier==200&&thumbnail_calls==1);
  z->card_id=500;z->attack=100;z->defense=200;
  r2_reconcile_zombies(0);
  assert(z->card_id==30&&z->attack==r2_original_atk(30)&&z->defense==r2_original_def(30));
  assert(z->stat_modifier==0&&thumbnail_calls==2);
  z->stat_modifier=300;r2_reconcile_zombies(1);
  assert(z->stat_modifier==300&&thumbnail_calls==3);
  z->data=&objects[6];r2_reconcile_zombies(1);
  assert(!r2_zombie_lease[6]&&z->stat_modifier==300&&thumbnail_calls==3);
 }
 clear();for(int i=5;i<10;i++)put(i,i==5?707:100,1000,1000,i==5);put(20,100,100,100,0);begin(5,20);post(0,1);for(int i=5;i<10;i++)assert(D_801A7AD8[i].card_id!=30);
 /* Tank: >= boundary, guardian/stat modifiers and lethal cost; one payment. */
 for(int defense=2399;defense<=2401;defense++){clear();put(5,370,2400,2400,1);put(20,100,1000,defense,0);D_801A7AD8[20].flags|=0x800;begin(5,20);DisplayObject a={5},d={20};r2_original_compare=compare_native;r2_battle.in_resolution=1;int value=r2_compare(&a,&d);assert((r2_battle.siege!=0)==(defense>=2400));assert(value==1);}
 for(int life=1;life<=1000;life+=(life==1?498:life==499?1:500)){clear();put(5,370,2400,2400,1);put(20,100,1000,3000,0);D_801A7AD8[20].flags|=0x800;begin(5,20);DisplayObject a={5},d={20};r2_original_compare=compare_native;r2_battle.in_resolution=1;assert(r2_compare(&a,&d)==1);D_8009B174=4;D_800E9FF0[0].life_points.unsigned_value=life;r2_after_battle_step(3);assert(D_800E9FF0[0].life_points.unsigned_value==r2_loss_cost(life));r2_after_battle_step(3);assert(D_800E9FF0[0].life_points.unsigned_value==r2_loss_cost(life));}
 /* Thousand + Bickuribox derived stats clean up without touching other modifiers. */
 clear();put(5,69,2400,2000,1);put(20,100,1300,2000,0);D_801A7AD8[20].flags|=0x800;begin(5,20);assert(df(20)==1700);post(0,0);assert(df(20)==2000);
 clear();put(5,385,2300,2000,1);put(20,100,3000,500,0);D_801A7AD8[20].stat_modifier=100;D_801A7AD8[20].defense_modifier=200;begin(5,20);assert(at(20)==800&&df(20)==3100);post(0,0);assert(at(20)==3100&&df(20)==800);
 /* Kaiser: multiple Kaisers cannot qualify as subjects. */
 clear();put(5,427,2300,2000,1);put(6,427,2300,2000,1);types[427]=CARD_TYPE_DRAGON;put(20,100,3000,1000,0);begin(5,20);assert(!r2_guard(5));put(7,200,1000,1000,0);types[200]=CARD_TYPE_DRAGON;assert(r2_guard(5));D_801A7AD8[7].flags=0;assert(!r2_guard(5));
 /* Sunbird surviving target only, even if Sunbird dies. */
 clear();put(5,467,2300,1800,1);put(20,100,3000,1000,0);begin(5,20);post(1,0);assert(D_801A7AD8[20].stat_modifier==-200);
 /* Crimson Sunbird: effect changes battle stats right now, and then
    persists once (with normal stacking across distinct later battles). */
 clear();put(5,467,2300,1800,1);put(20,100,3000,1800,0);
 begin(5,20);assert(at(20)==2800&&df(20)==1600);
 assert(D_801A7AD8[20].stat_modifier==0); /* no premature commit */
 post(0,0);assert(D_801A7AD8[20].stat_modifier==-200);
 assert(at(20)==2800&&df(20)==1600); /* no duplicate on return */
 begin(5,20);assert(at(20)==2600&&df(20)==1400);
 post(0,0);assert(D_801A7AD8[20].stat_modifier==-400);
 assert(at(20)==2600&&df(20)==1400);
 /* Negated / disabled Crimson effect must not modify this battle. */
 clear();put(5,467,2300,1800,0);put(20,100,3000,1800,0);
 begin(5,20);assert(at(20)==3000&&df(20)==1800);post(0,0);
 assert(D_801A7AD8[20].stat_modifier==0);
 /* Aqua: exactly eligible enemy ATK targets, attack-only Musician excluded. */
 clear();put(5,442,2250,1900,1);put(20,100,1000,1000,0);put(21,101,2000,1000,0);put(22,495,2000,1000,1);begin(5,20);post(0,1);assert(D_801A7AD8[21].flags&0x800);assert(!(D_801A7AD8[22].flags&0x800));
 /* Forest Bracchio: bonus and protection are conditional, no residual modifier. */
 clear();put(5,509,2200,2000,1);put(20,100,3000,1000,0);types[100]=CARD_TYPE_INSECT;gDuel_bTerrain[0]=1;assert(at(5)==3000&&df(5)==2800);begin(5,20);assert(r2_guard(5));gDuel_bTerrain[0]=2;assert(!r2_guard(5)&&at(5)==2200);post(0,0);assert(at(5)==2200);
 /* Soul Hunter: always printed ATK/2, not modified attack, obey LP cap. */
 clear();put(5,471,2200,1800,1);put(20,100,4000,1000,0);gDuel_adwCardStats[99]=200;D_800E9FF0[0].life_points.unsigned_value=7800;begin(5,20);post(0,1);assert(D_800E9FF0[0].life_points.unsigned_value==8000);
 /* Mammoth dying as attacker OR defender, actual surviving destroyer only. */
 clear();put(5,564,2200,1800,1);put(20,100,3000,1000,0);begin(5,20);post(1,0);assert(D_801A7AD8[20].stat_modifier==-500);
 clear();put(5,100,3000,1000,0);put(20,564,2200,1800,1);begin(5,20);post(0,1);assert(D_801A7AD8[5].stat_modifier==-500);
 /* Ocean Snake position persists, target only, negated attack does nothing. */
 clear();put(5,223,2100,1800,1);put(20,100,1000,3000,0);D_801A7AD8[20].flags|=0x800;begin(5,20);assert(!(D_801A7AD8[20].flags&0x800)&&!(D_8009B178[1]&0x800));post(0,0);assert(!(D_801A7AD8[20].flags&0x800));
 /* Eagle debuffs attacker only when it attacks an adjacent allied zone. */
 clear();put(5,100,2400,2200,0);put(21,101,2000,1000,0);put(20,465,2100,1800,1);begin(5,21);assert(at(5)==2000&&df(5)==1800);post(0,0);assert(at(5)==2400&&df(5)==2200);
 /* Sand true EARTH, other allies only, continuous and independently scoped. */
 clear();put(5,531,2100,1700,1);put(6,100,1000,1000,0);put(20,101,1000,1000,0);attrs[531]=attrs[100]=attrs[101]=2;assert(at(5)==2100&&at(6)==1200&&at(20)==1000);D_801A7AD8[6].stat_modifier=-1100;assert(at(6)==100);D_801A7AD8[6].stat_modifier=0;enabled[531]=0;assert(at(6)==1000);enabled[531]=1;D_801A7AD8[5].flags=0;assert(at(6)==1000);
 /* Empress: all other allied Female, self expressly excluded. */
 clear();put(5,100,3000,1000,0);put(20,101,1000,1000,0);put(24,572,2100,1700,1);female[101]=female[572]=1;begin(5,20);assert(r2_guard(20)&&!r2_guard(24));D_801A7AD8[24].flags=0;assert(!r2_guard(20));
 /* Shark uses NET prior attack below printed, ignores later battle-only reductions. */
 for(int mod=-500;mod<=500;mod+=500){clear();put(5,565,2100,1300,1);put(20,100,2000,1000,0);gDuel_adwCardStats[99]=200;D_801A7AD8[20].stat_modifier=mod;begin(5,20);assert(at(5)==(mod<0?3100:2100));post(0,0);assert(at(5)==2100);}
 /* No free effect by ID, genuine fusion enables, Egotist lease enables one copy. */
 clear();put(5,69,2400,2000,0);put(20,100,1000,2000,0);D_801A7AD8[20].flags|=0x800;begin(5,20);assert(df(20)==2000);r2_reset();lease[5]=1;begin(5,20);assert(df(20)==1700);
 /* Post-battle scheduler waits for native effects; protected survivor does not count as a kill. */
 clear();put(5,613,2800,2100,1);put(20,100,1000,1000,0);put(21,101,1000,1000,0);begin(5,20);
 r2_battle.resolved=1;r2_battle.death[1]=1;D_801A7AD8[5].flags|=0x8000;D_801A7AD8[20].flags|=0x8000;r2_end();
 gDuel_bEffectState=2;r2_stable_tick();assert(r2_battle.pending);gDuel_bEffectState=0;r2_stable_tick();assert(!r2_battle.pending&&at(21)==1000);
 /* Canceled attacks do not arm any after-battle event. */
 clear();put(5,613,2800,2100,1);put(20,100,1000,1000,0);begin(5,20);D_8009B22A=1;r2_after_battle_step(3);r2_end();assert(!r2_battle.pending);
 assert(r2_exact_recipe(36,97)==564&&r2_exact_recipe(97,36)==564&&r2_exact_recipe(646,626)==565);
 puts("PASS: all 16 runtime effects, scopes, actual origin/lease, LP boundaries, restoration and recipes");
}
