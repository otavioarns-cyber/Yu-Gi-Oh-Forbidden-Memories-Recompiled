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
#define CARD_TYPE_ZOMBIE 2
#define CARD_TYPE_WARRIOR 3
#define CARD_TYPE_BEAST 5
#define CARD_TYPE_BEAST_WARRIOR 4
#define CARD_TYPE_DINOSAUR 10
#define CARD_TYPE_PLANT 19
#define CARD_TYPE_INSECT 9
#define CARD_FUSION_GROUP_FEMALE 8
#define DUEL_CARD_FLAG_OCCUPIED 0x8000
#define DUEL_CARD_FLAG_FACE_DOWN 0x1000
#define DUEL_CARD_FLAG_DEFENSE_POSITION 0x800
#define DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2 0x200
#define DUEL_CARD_FLAG_USED_THIS_TURN 0x4000
#define DUEL_SCENE_PHASE_MASK 15
#define AUX_MUSICIAN_KING 495
#include "../fusion_round2_rules.h"
#include "../fusion_round3_rules.h"
#include "../skull_monster.h"
typedef struct {int card_id,attack,defense,stat_modifier,defense_modifier,terrain_modifier,flags,table_index;void *data,*object;} DuelCardRecord;
typedef struct {int field_6A,card_index;} DisplayObject;
typedef DisplayObject DuelCardDisplayObject;
typedef struct {int id,deck_index,data_block_index;} DuelDeckCardRecord;
static DuelCardRecord D_801A7AD8[30];
static u8 D_800907D8[40];
#define DUEL_FIELD_SIDE_GRID_SLOT_COUNT 20
static unsigned gDuel_adwCardStats[722];
static int types[723],attrs[723],female[723],enabled[723],lease[30];
static int fusion_origin[30],metal_immune[30],fusion_bound_id[30],metal_bound_id[30];
static void *fusion_bound_data[30],*metal_bound_data[30];
static int aux_elf_bound_id[30],aux_elf_healed[30];static void *aux_elf_bound_data[30];
static int battle_record[2],battle_card[2],battle_saved_flags[2],battle_guarded[2];static void *battle_saved_data[2];
static int D_8009B178[2],D_8009B22A,D_8009B174,D_8009B1B0[2],D_8009B1A4[2];
static int gDuel_wSceneStateFlags=5,gDuel_bEffectState,gDuel_wCardEffectFlags;
static int battle_lp_before[2];
static unsigned char gDuel_bTerrain[1];
static struct {struct{unsigned short unsigned_value;}life_points;int max_life_points;}D_800E9FF0[2];
static struct {int x,y;}D_800908A0[30];
static struct Host {void *(*find)(const struct Host*,const char*);} *aux_host;
static int aux_monster_slot(int i){return i>=0&&i<30&&i%15>=5&&i%15<10;}
static int Cards_Valid(int id){return id>0&&id<=722;}
static int Cards_Type(int id){return types[id];}
static int Cards_Attribute(int id){return attrs[id];}
static int Cards_AttributeNamed(const char *s){return !strcmp(s,"Earth")?2:!strcmp(s,"Fire")?3:!strcmp(s,"Dark")?4:-1;}
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
static int visual_recreated_id;
static DuelCardDisplayObject *func_80024C1C(int id,int x,int y){(void)x;(void)y;visual_recreated_id=id;return &objects[0];}
static void Duel_ApplyCardObjectFlags(DuelCardDisplayObject*p){(void)p;}
static void DisplayObject_ReleaseIfPresent(void *object){(void)object;}
static void aux_queue_stat_effect(int record,int amount){(void)record;(void)amount;}
static int aux_darkfire_card_thumbnail(int i,int id){(void)i;(void)id;return 1;}
static void aux_dark_elf_react_to_healing(int side,int amount){(void)side;(void)amount;}
static int rebirth_fire_calls,rebirth_fire_slot;
static void r3_queue_rebirth_fire(int slot,int id){assert(id==272);rebirth_fire_calls++;rebirth_fire_slot=slot;}
#include "../fusion_round2_runtime.h"
static int r3_lp_damage,r3_lp_side;
static int AuxBattleCoinFlip_Request(int a,int b,void (*callback)(int,void*),void *opaque){(void)a;(void)b;(void)callback;(void)opaque;return 0;}
static void aux_queue_lp(int side,int damage,int spell,int source){(void)spell;(void)source;r3_lp_damage+=damage;r3_lp_side=side;}
#include "../secondary_dragon_types.h"
#include "../fusion_round3_runtime.h"
/* Simulate the retail/3D battle-return path, including its overwrite from
   an unrelated combined-deck entry, then execute the REAL hooked wrapper. */
static int native_restore_calls,native_restore_deck_index;
static DuelDeckCardRecord irrelevant_native_deck={471,8,8};
static void fake_native_recreate(s32 tagged_slot,s32 deck_index){
 int slot=(tagged_slot&0x80)?(tagged_slot&0x7f)+15:tagged_slot;
 DuelCardRecord *c=&D_801A7AD8[slot];
 native_restore_calls++;native_restore_deck_index=deck_index;
 assert(deck_index>=0&&deck_index<80); /* never dereference 0xFF */
 memset(c,0,sizeof(*c));
 c->data=&irrelevant_native_deck;c->card_id=471;
 c->attack=2200;c->defense=1800;c->flags=DUEL_CARD_FLAG_OCCUPIED;
 c->object=&objects[29];
}
static void (*original_restore_generated_card)(s32,s32)=fake_native_recreate;
#include "../generated_card_restore.h"
static s32 raw_stats(DuelCardRecord*c){int at=r2_limit(c->attack+c->stat_modifier+c->terrain_modifier,9999),df=r2_limit(c->defense+c->stat_modifier+c->defense_modifier+c->terrain_modifier,9999);return (s32)(((unsigned)df<<16)|at);}
static s32 Duel_CalcCardStats(DuelCardRecord*c){return r3_stats(c,r2_stats(c,raw_stats(c)));}
static int at(int s){return (u16)Duel_CalcCardStats(D_801A7AD8+s);}
static int df(int s){return (unsigned)Duel_CalcCardStats(D_801A7AD8+s)>>16;}
static void put(int slot,int id,int attack,int defense,int origin){DuelCardRecord*c=&D_801A7AD8[slot];memset(c,0,sizeof(*c));c->card_id=id;c->attack=attack;c->defense=defense;c->data=&objects[slot];c->flags=0x8000;fusion_origin[slot]=origin;fusion_bound_id[slot]=id;fusion_bound_data[slot]=c->data;}
static void clear(void){rebirth_fire_calls=0;rebirth_fire_slot=-1;memset(D_801A7AD8,0,sizeof(D_801A7AD8));memset(fusion_origin,0,sizeof(fusion_origin));memset(lease,0,sizeof(lease));memset(battle_guarded,0,sizeof(battle_guarded));memset(types,0xff,sizeof(types));memset(attrs,0xff,sizeof(attrs));memset(female,0,sizeof(female));for(int i=0;i<723;i++)enabled[i]=1;gDuel_bTerrain[0]=0;star_bonus=0;D_8009B22A=0;D_8009B174=0;r2_reset();r3_full_reset();r3_lp_damage=0;r3_lp_side=0;D_800E9FF0[0].life_points.unsigned_value=D_800E9FF0[1].life_points.unsigned_value=4000;D_800E9FF0[0].max_life_points=D_800E9FF0[1].max_life_points=8000;}
static void begin(int a,int d){int slots[2]={a,d};for(int p=0;p<2;p++){int s=slots[p];battle_record[p]=s;battle_card[p]=D_801A7AD8[s].card_id;battle_saved_data[p]=D_801A7AD8[s].data;battle_saved_flags[p]=D_8009B178[p]=D_801A7AD8[s].flags;D_801A7AD8[s].flags&=~0x8000;}r2_begin();r3_begin();r2_prepare(2);if(D_801A7AD8[a].card_id!=43)r3_prepare(2);}
static void post(int dead0,int dead1){r2_battle.resolved=1;r2_battle.death[0]=dead0;r2_battle.death[1]=dead1;if(!dead0)D_801A7AD8[r2_battle.slot[0]].flags|=0x8000;if(!dead1)D_801A7AD8[r2_battle.slot[1]].flags|=0x8000;r2_end();r3_battle.resolved=1;r3_battle.death[0]=dead0;r3_battle.death[1]=dead1;r3_end();r2_post_battle();r3_post_battle();}
static int compare_native(DisplayObject*a,DisplayObject*b){return at(a->field_6A)-df(b->field_6A);}
int main(void){
 /* Permanent adjacency, empty neighbours and stacking; source departure. */
 clear();put(5,613,2800,2100,1);put(22,100,100,100,0);put(21,101,500,500,0);put(23,102,500,500,0);put(24,103,500,500,0);begin(5,22);post(0,1);assert(at(21)==300&&df(23)==300&&at(24)==500);D_801A7AD8[5].flags=0;assert(at(21)==300);
 put(5,613,2800,2100,1);put(22,100,100,100,0);begin(5,22);post(0,1);assert(at(21)==100);
 /* Skelgon: target can have Dragon as PRIMARY or as a SECONDARY
    type. The subtype applies no bonus to non-Dragon-like enemies. */
 clear();put(5,545,1700,1900,1);put(20,97,1000,1000,0);
 types[545]=CARD_TYPE_ZOMBIE;types[97]=CARD_TYPE_WARRIOR;
 assert(aux_dragon_like(97));assert(!aux_dragon_like(100));
 begin(5,20);assert(r3_battle.attack_bonus==800&&at(5)==2500);post(0,0);
 clear();put(5,545,1700,1900,1);put(20,100,1000,1000,0);
 types[100]=CARD_TYPE_DRAGON;begin(5,20);
 assert(aux_dragon_like(100)&&r3_battle.attack_bonus==800);post(0,0);
 clear();put(5,545,1700,1900,1);put(20,100,1000,1000,0);
 types[100]=CARD_TYPE_WARRIOR;begin(5,20);
 assert(!aux_dragon_like(100)&&r3_battle.attack_bonus==0);post(0,0);
 /* Skull Knight: one effect summon, no fusion origin, no spell-zone overwrite. */
 clear();put(5,707,2650,2250,1);put(20,100,100,100,0);begin(5,20);post(0,1);assert(D_801A7AD8[6].card_id==30&&!fusion_origin[6]&&!lease[6]);
 { /* Zombie Warrior survives a battle: no Soul Hunter/Crass Clown overwrite. */
   DuelCardRecord *z=&D_801A7AD8[6];void *instance=z->data;
   z->stat_modifier=350;z->defense_modifier=100;
   z->flags|=DUEL_CARD_FLAG_USED_THIS_TURN|DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2;
   aux_restore_generated_card(6,255);
   assert(native_restore_deck_index==0&&native_restore_calls==1);
   assert(z->card_id==30&&z->data==instance&&((DuelDeckCardRecord *)z->data)->id==30);
   assert(z->attack==r2_original_atk(30)&&z->defense==r2_original_def(30));
   assert(z->stat_modifier==350&&z->defense_modifier==100);
   assert((z->flags&(DUEL_CARD_FLAG_USED_THIS_TURN|DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2))==
                  (DUEL_CARD_FLAG_USED_THIS_TURN|DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2));
   assert(visual_recreated_id==30&&z->object==&objects[0]);
   /* Ordinary deck cards pass through to vanilla, index intact. */
   put(7,100,1000,1000,0);aux_restore_generated_card(7,17);
   assert(native_restore_deck_index==17&&D_801A7AD8[7].card_id==471);
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

 /* Skullbird bilateral temporary ATK shift. */
 clear();put(5,521,1900,1700,1);put(20,100,2000,1000,0);begin(5,20);
 assert(at(5)==2300&&at(20)==1600);post(0,0);assert(at(5)==1900&&at(20)==2000);
 /* Deepsea target face-down at selection, bonus persists after revealing. */
 clear();put(5,437,1900,1600,1);put(20,100,2000,1000,0);D_801A7AD8[20].flags|=DUEL_CARD_FLAG_FACE_DOWN;
 begin(5,20);assert(at(5)==2600);D_801A7AD8[20].flags&=~DUEL_CARD_FLAG_FACE_DOWN;assert(at(5)==2600);post(0,0);
 /* Fire, dinosaurs, LIGHT vs DARK, Plant/Zombie, Dragon and Beasts. */
 clear();put(5,473,1900,1500,1);put(6,101,1200,1200,0);put(7,102,1200,1200,0);
 attrs[473]=attrs[101]=attrs[102]=3;assert(at(5)==2300&&df(5)==1900);
 D_801A7AD8[7].flags=0;assert(at(5)==2100);
 clear();put(5,482,1900,1500,1);put(6,101,1200,1200,0);types[101]=CARD_TYPE_DINOSAUR;
 put(20,100,1500,1000,0);begin(5,20);assert(at(5)==2200);post(0,0);assert(at(5)==1900);
 clear();put(5,389,1850,1500,1);put(20,100,1800,1000,0);attrs[100]=4;
 begin(5,20);assert(at(5)==2450);post(0,0);
 clear();put(5,15,1800,1600,1);put(20,100,1800,1000,0);types[100]=CARD_TYPE_PLANT;
 begin(5,20);assert(at(5)==2400);post(0,0);
 clear();put(5,545,1700,1900,1);put(20,100,1800,1000,0);types[100]=CARD_TYPE_DRAGON;
 begin(5,20);assert(at(5)==2500);post(0,0);
 clear();put(5,230,1500,1200,1);put(20,100,1800,1000,0);types[100]=CARD_TYPE_BEAST_WARRIOR;
 begin(5,20);assert(at(5)==2200);post(0,0);
 /* Umi/Wasteland bonus is independent of native field bonus. */
 clear();put(5,639,1850,1300,1);gDuel_bTerrain[0]=5;assert(at(5)==2650&&df(5)==1300);
 gDuel_bTerrain[0]=2;assert(at(5)==1850&&df(5)==2100);
 /* Dragoness checks other Dragon and Warrior independently, never herself. */
 clear();put(5,294,1200,900,1);types[294]=CARD_TYPE_WARRIOR;assert(at(5)==1200);
 put(6,101,1300,1200,0);types[101]=CARD_TYPE_DRAGON;assert(at(5)==1700&&df(5)==1400);
 put(7,102,1300,1200,0);types[102]=CARD_TYPE_WARRIOR;assert(at(5)==2200&&df(5)==1900);
 /* Marine Beast: attack comparison uses pre-effect ATK; either side can activate. */
 clear();put(5,617,1700,1600,1);put(20,100,2000,1000,0);begin(5,20);
 assert(at(20)==1400&&at(5)==1700);post(0,0);
 clear();put(5,100,2000,1000,0);put(20,617,1700,1600,1);begin(5,20);
 assert(at(5)==1400&&at(20)==1700);post(0,0);
 /* Barox swaps its own CURRENT stats, no permanent modification. */
 clear();put(5,86,1380,1530,1);put(20,100,1000,1000,0);begin(5,20);
 assert(at(5)==1530&&df(5)==1380);post(0,0);assert(at(5)==1380&&df(5)==1530);
 /* Female destroyed in battle grants permanent +400/+400, excluding self. */
 clear();put(5,618,1900,1700,1);put(6,101,500,500,0);put(20,100,2000,1000,0);
 female[101]=1;begin(20,6);post(0,1);assert(at(5)==2300&&df(5)==2100);
 /* Destroyed Charubin summons a fresh, non-fusion Hinotama Soul. */
 clear();put(5,133,1100,800,1);put(20,100,2000,1000,0);begin(20,5);post(0,1);
 assert(D_801A7AD8[5].card_id==210&&!fusion_origin[5]&&!lease[5]);
 { /* Same exact battle-return issue affects Round03 generated monsters. */
   DuelCardRecord *h=&D_801A7AD8[5];void *instance=h->data;
   h->stat_modifier=175;
   aux_restore_generated_card(5,255);
   assert(native_restore_deck_index==0&&h->data==instance&&h->card_id==210);
   assert(h->attack==r2_original_atk(210)&&h->defense==r2_original_def(210));
   assert(h->stat_modifier==175&&visual_recreated_id==210);
 }

 /* Flame Ghost adds LP damage only on genuine battle destruction. */
 clear();put(5,215,1000,800,1);put(20,100,2000,1000,0);begin(20,5);post(0,1);
 assert(r3_lp_damage==500&&r3_lp_side==1);
 /* Direct copy never gains an effect by matching card ID. */
 clear();put(5,521,1900,1700,0);put(20,100,2000,1000,0);begin(5,20);
 assert(at(5)==1900&&at(20)==2000);post(0,0);
 assert(r3_exact_recipe(36,10)==545&&r3_exact_recipe(10,36)==545);
 assert(r3_exact_recipe(61,251)==230&&r3_exact_recipe(251,61)==230);
 assert(r3_exact_recipe(2,104)==115&&r3_exact_recipe(574,612)==495);

 /* Rose Spectre: same-controller Plant battle LP loss only. */
 clear();put(5,594,2000,1800,1);types[594]=CARD_TYPE_PLANT;
 put(20,100,2500,1000,0);begin(20,5);
 assert(r3_battle.rose_sources[0]==1&&r3_battle.plant_fighting[0]);
 battle_lp_before[0]=4000;D_800E9FF0[0].life_points.unsigned_value=3000;
 r3_battle.battle_damage[0]=1000;post(0,1);assert(r3_lp_damage==500&&r3_lp_side==1);
 /* Kwagar: guarded only once per DUEL for that exact card data. */
 clear();put(5,533,1900,1700,1);put(20,100,3000,1000,0);begin(20,5);
 assert(r3_guard(5));D_8009B174=4;D_8009B1B0[1]=-1;r3_after_step(3);
 assert(r3_kwagar_count==1);r3_end();r3_begin();assert(!r3_guard(5));
 /* Karbonala coin-success swaps current ATK only, and resets after battle. */
 clear();put(5,43,1500,1200,1);put(20,100,2500,1000,0);begin(5,20);
 assert(r3_coin_pause()==0);r3_battle.coin_swap=1;r3_prepare(2);
 assert(at(5)==2500&&at(20)==1500&&df(5)==1200);post(0,0);assert(at(5)==1500);
 /* Zombie Warrior buffs only other Skull-category allies after its death. */
 clear();put(5,30,1200,900,1);put(6,9,1000,1000,0);put(7,100,1000,1000,0);
 put(20,100,2500,1000,0);begin(20,5);post(0,1);
 assert(at(6)==1300&&df(6)==1300&&at(7)==1000);
 /* Charubin: synthetic Hinotama Soul must survive the same battle-return
    path as Skull Knight's Zombie, including identity, stats and visual art. */
 clear();put(5,133,1100,800,1);put(20,100,3000,1000,0);
 begin(20,5);post(0,1);
 assert(D_801A7AD8[5].card_id==210&&!fusion_origin[5]);
 { DuelCardRecord *h=&D_801A7AD8[5];void *original=h->data;
   h->stat_modifier=250;h->defense_modifier=-50;
   aux_restore_generated_card(5,255);
   assert(native_restore_deck_index==0);
   assert(h->data==original&&h->card_id==210);
   assert(((DuelDeckCardRecord *)h->data)->id==210);
   assert(h->attack==r2_original_atk(210)&&h->defense==r2_original_def(210));
   assert(h->stat_modifier==250&&h->defense_modifier==-50);
   assert(visual_recreated_id==210);
   /* Next turn: preserve original stats modifiers, sprite and 3D identity. */
   r3_reconcile_summons(1);
   assert(h->card_id==210&&visual_recreated_id==210);
   /* One damaged field/view ID is repairable without restarting its effect. */
   h->card_id=471;r3_reconcile_summons(0);
   assert(h->card_id==210&&h->stat_modifier==250);
   /* Real fusion changes both IDs: cannot turn the new monster into Soul. */
   h->card_id=300;((DuelDeckCardRecord *)h->data)->id=300;
   r3_reconcile_summons(1);
   assert(h->card_id==300&&!r3_summon_lease[5]);
 }
 /* Charubin as attacker also summons; the free space must be real. */
 clear();put(5,133,1100,800,1);put(20,100,3000,1000,0);
 begin(5,20);post(1,0);assert(D_801A7AD8[5].card_id==210);
 clear();put(5,133,1100,800,1);for(int i=6;i<10;i++)put(i,100,1000,1000,0);
 put(20,100,3000,1000,0);begin(5,20);post(1,0);
 assert(D_801A7AD8[5].card_id==210);
 for(int i=6;i<10;i++)assert(D_801A7AD8[i].card_id==100);
 /* Phoenix: turn switch OPENS the window only; draw may not yet have
    finished and no player/CPU can have committed a card action. */
 clear();put(5,272,1300,900,1);put(20,100,2500,1000,0);begin(20,5);post(0,1);
 assert(r3_mavelus_count==1&&r3_mavelus_pending[0].active);
 gDuel_wSceneStateFlags=4;
 r3_turn_begin(1);r3_turn_entry_before_actions(1);
 assert(r3_mavelus_pending[0].active);
 r3_turn_begin(0);assert(r3_mavelus_pending[0].active);
 /* No summon DURING the draw phase; other occupied monsters stay put. */
 gDuel_wSceneStateFlags=2;r3_turn_entry_before_actions(0);
 assert(r3_mavelus_pending[0].active);
 put(6,100,1000,1000,0);void *other=D_801A7AD8[6].data;
 gDuel_wSceneStateFlags=4;r3_turn_entry_before_actions(0);
 assert(!r3_mavelus_pending[0].active&&r3_mavelus_due_side==-1);
 assert(D_801A7AD8[5].card_id==272&&!fusion_origin[5]);assert(rebirth_fire_calls==1 && rebirth_fire_slot==5);
 assert(D_801A7AD8[6].card_id==100&&D_801A7AD8[6].data==other);
 { DuelCardRecord *ph=&D_801A7AD8[5];void *original=ph->data;
   ph->stat_modifier=120;aux_restore_generated_card(5,255);
   assert(native_restore_deck_index==0);
   assert(ph->data==original&&ph->card_id==272);
   assert(ph->attack==r2_original_atk(272)&&ph->defense==r2_original_def(272));
   assert(ph->stat_modifier==120&&visual_recreated_id==272);
   r3_reconcile_summons(1);
   assert(ph->card_id==272&&visual_recreated_id==272);
 }
 /* Idempotent during the SAME turn, never creates a second Mavelus. */
 D_801A7AD8[5].flags=0;
 r3_turn_entry_before_actions(0);assert(!D_801A7AD8[5].flags);
 r3_turn_begin(0);r3_turn_entry_before_actions(0);
 assert(r3_mavelus_count==1&&!D_801A7AD8[5].flags);
 /* The return's one opportunity expires when the monster row is full. */
 clear();put(5,272,1300,900,1);put(20,100,2500,1000,0);
 begin(20,5);post(0,1);
 for(int i=5;i<10;i++)put(i,100+i,1000,1000,0);
 r3_turn_begin(0);gDuel_wSceneStateFlags=4;r3_turn_entry_before_actions(0);
 for(int i=5;i<10;i++)assert(D_801A7AD8[i].card_id==100+i);
 assert(!r3_mavelus_pending[0].active);
 D_801A7AD8[5].flags=0;r3_turn_begin(0);
 r3_turn_entry_before_actions(0);assert(!D_801A7AD8[5].flags);
 /* AI controller: the same entry boundary runs for side 1, in its own
    monster row, before any AI play can choose an occupied zone. */
 clear();put(20,272,1300,900,1);put(5,100,2500,1000,0);
 begin(5,20);post(0,1);
 assert(r3_mavelus_pending[0].active&&r3_mavelus_pending[0].side==1);
 r3_turn_begin(1);gDuel_wSceneStateFlags=4;r3_turn_entry_before_actions(1);
 assert(!r3_mavelus_pending[0].active);
 assert(D_801A7AD8[20].card_id==272&&!fusion_origin[20]);
 /* Flower Wolf requires its own fusion provenance and another visible Plant.
    A second copy, direct from the deck, must not share this immunity. */
 clear();put(20,487,1800,1400,1);
 types[487]=CARD_TYPE_BEAST;types[101]=CARD_TYPE_PLANT;
 assert(!r3_flower_untargetable(20));
 D_800907D8[5]=20;D_800907D8[6]=21;
 assert(r3_find_attackable_enemy(0,0)==0);
 assert(!r3_flower_untargetable(20));
 put(22,101,1000,1000,0);
 assert(r3_flower_untargetable(20));
 assert(r3_find_attackable_enemy(0,0)==-1);
 put(21,101,1000,1000,0);assert(r3_find_attackable_enemy(0,0)==1);
 D_801A7AD8[21].flags=0;D_801A7AD8[22].flags=0;
 assert(!r3_flower_untargetable(20));
 put(21,101,1000,1000,0);
 D_801A7AD8[21].flags|=DUEL_CARD_FLAG_FACE_DOWN;
 assert(!r3_flower_untargetable(20));
 D_801A7AD8[21].flags&=~DUEL_CARD_FLAG_FACE_DOWN;
 assert(r3_flower_untargetable(20));
 D_801A7AD8[20].flags|=DUEL_CARD_FLAG_FACE_DOWN;
 assert(!r3_flower_untargetable(20));
 D_801A7AD8[20].flags&=~DUEL_CARD_FLAG_FACE_DOWN;
 enabled[487]=0;assert(!r3_flower_untargetable(20));enabled[487]=1;
 fusion_origin[20]=0;assert(!r3_flower_untargetable(20));
 lease[20]=1;assert(r3_flower_untargetable(20));lease[20]=0;
 fusion_origin[20]=1;fusion_bound_data[20]=D_801A7AD8[20].data;
 assert(r3_flower_untargetable(20));
 /* A duplicate Flower Wolf without valid origin is attackable. */
 put(21,487,1800,1400,0);put(22,101,1000,1000,0);D_800907D8[6]=21;
 assert(r3_find_attackable_enemy(0,0)==1);
 /* Kaminari: true entry (once), pending enemy side, next enemy turn only.
    Its presence never refreshes the prohibition, new entry does. */
 clear();put(5,458,1900,1400,1);
 r3_kaminari_scan();r3_kaminari_scan();
 assert(r3_kaminari_pending[1]==1&&r3_kaminari_used(5));
 assert(!r3_kaminari_attacks_blocked(0));
 r3_turn_begin(0);
 assert(r3_kaminari_pending[1]==1&&!r3_kaminari_attacks_blocked(0));
 r3_turn_begin(1);
 assert(r3_kaminari_attacks_blocked(1));
 assert(!r3_kaminari_attacks_blocked(0));
 r3_kaminari_scan();r3_kaminari_scan();
 assert(r3_kaminari_pending[1]==0);
 r3_turn_begin(0);
 assert(!r3_kaminari_attacks_blocked(1));
 r3_turn_begin(1);assert(!r3_kaminari_attacks_blocked(1));
 /* Kaminari leaves the field; restriction survives, new entry schedules anew. */
 r3_kaminari_forget(5);D_801A7AD8[5].flags=0;
 put(5,458,1900,1400,1);r3_kaminari_scan();
 assert(r3_kaminari_pending[1]==1);
 r3_turn_begin(1);assert(r3_kaminari_attacks_blocked(1));
 enabled[458]=0;assert(!r3_kaminari_attacks_blocked(1));
 enabled[458]=1;assert(r3_kaminari_attacks_blocked(1));
 /* A second direct-drawn Kaminari must not inherit its fused copy's trigger. */
 clear();put(5,458,1900,1400,1);put(6,458,1900,1400,0);
 r3_kaminari_scan();assert(r3_kaminari_used(5)&&!r3_kaminari_used(6));
 assert(r3_kaminari_pending[1]==1);
 clear();put(5,458,1900,1400,0);r3_kaminari_scan();
 assert(!r3_kaminari_used(5)&&!r3_kaminari_pending[1]);
 /* Two summons in the same turn still prohibit the next enemy turn once. */
 clear();put(5,458,1900,1400,1);put(6,458,1900,1400,1);
 r3_kaminari_scan();assert(r3_kaminari_pending[1]);r3_turn_begin(1);
 assert(r3_kaminari_attacks_blocked(1));r3_turn_begin(0);
 assert(!r3_kaminari_attacks_blocked(1));
 puts("PASS: 22 Round03 Fusion Effects incl Flower Wolf and Kaminari entry/turn isolation");
}
