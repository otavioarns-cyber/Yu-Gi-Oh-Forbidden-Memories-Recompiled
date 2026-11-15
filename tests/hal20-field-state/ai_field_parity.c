/* Compare the exact historical planner with the candidate, plus actual Field rules. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../mods/ai-hard-mode/planner.h"
#include "../../mods/field-effects-secondary-types/field_effects.c"
HmDecision Old_Hm_PlanHand(const HmBoard*,const HmOptions*,const HmRules*);
HmDecision Old_Hm_PlanField(const HmBoard*,const HmOptions*,const HmRules*,const HmDecision*);
static int type_table[723];static unsigned rng=412;
s32 Cards_Type(s32 id){return id>0&&id<=722?type_table[id]:20;}
int Tables_TerrainBonus(int f,int t,int*b){(void)f;(void)t;(void)b;return 0;}
u8 gDuel_bTerrain;s8 gDuel_aTerrainBoost[20][6];
static unsigned next(void){rng=rng*1664525u+1013904223u;return rng;}
static HmCard card(int id){HmCard c={0};c.id=c.effect=id;c.type=Cards_Type(id);c.attack=500+id%200*10;c.defense=400+id%170*10;c.star=id%10+1;c.star2=(id+4)%10+1;return c;}
static int fusion(int a,int b){return (a+b)%7?0:1+(a+b)%722;}
static int equip(int a,int b){(void)a;(void)b;return 0;}
static int native_terrain(int type,int field){return field>0&&field<=6&&type>=0&&type<20?gDuel_aTerrainBoost[type][field-1]*10:0;}
static int actual_terrain(int id,int type,int field){return native_terrain(type,field)+terrain_card_delta_v1(id,type,field);}
static int ritual(int id){(void)id;return 0;}
int main(void){
 for(int i=1;i<=722;i++)type_table[i]=i%20;type_table[22]=CARD_TYPE_FIEND;type_table[535]=CARD_TYPE_DRAGON;
 gDuel_aTerrainBoost[CARD_TYPE_FIEND][5]=50;gDuel_aTerrainBoost[CARD_TYPE_DRAGON][2]=50;
 MemoriesModHost h={0};h.api=24;mod_host=&h;owner_effective=0;
 HmRules rules={card,fusion,equip,native_terrain,ritual,actual_terrain};HmOptions opts={1,1,1,1,1,1,1,3,256,50,150,1};unsigned compared=0;
 for(int n=0;n<1000;n++){
  HmBoard b={0};b.lp=b.max_lp=b.enemy_lp=8000;b.terrain=next()%7;b.pinned=b.enemy_pinned=-1;b.hand_count=5;
  for(int i=0;i<5;i++){b.hand[i]=card(1+next()%722);if(next()%2)b.own[i]=card(1+next()%722);if(next()%2)b.enemy[i]=card(1+next()%722);}
  HmDecision old=Old_Hm_PlanHand(&b,&opts,&rules),now=Hm_PlanHand(&b,&opts,&rules);assert(!memcmp(&old,&now,sizeof old));++compared;
  old=Old_Hm_PlanField(&b,&opts,&rules,NULL);now=Hm_PlanField(&b,&opts,&rules,NULL);assert(!memcmp(&old,&now,sizeof old));++compared;
 }
 /* A close choice changes only when the real Field provider adds Skull's +200. */
 owner_effective=1;HmBoard b={0};b.lp=b.max_lp=b.enemy_lp=8000;b.terrain=6;b.pinned=b.enemy_pinned=-1;b.hand_count=2;b.hand[0]=card(22);b.hand[0].attack=1200;b.hand[0].defense=1000;b.hand[1]=card(535);opts.fusion=0;int found=0;
 for(int at=1400;at<=2400&&!found;at+=10){b.hand[1].attack=at;b.hand[1].defense=1500;HmRules without=rules;without.terrain_card=NULL;HmDecision old=Hm_PlanHand(&b,&opts,&without),now=Hm_PlanHand(&b,&opts,&rules);if(old.selection[0]!=now.selection[0]){assert(old.selection[0]==12&&now.selection[0]==11);printf("REAL_FIELD_DECISION Skull ATK1200 DEF1000, Dragon ATK%d DEF1500, Yami: old=%u new=%u; +500 primary +200 secondary\n",at,old.selection[0],now.selection[0]);found=1;}}
 assert(found);printf("PASS %u exact historical hand/field decision comparisons with provider OFF; real-provider ON changes approved terrain input only\n",compared);return 0;
}
