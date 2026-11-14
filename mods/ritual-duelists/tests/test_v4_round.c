#include "ritual_duelists.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
static unsigned seed=0x4920A4C3u;
static int rng(int max,void *ctx){(void)ctx;seed=1664525u*seed+1013904223u;return (int)((seed>>1)%(unsigned)max);}
static int count_weight(const RdOpponentProfile *p,int id){for(int i=0;i<p->pool_count;i++)if(p->pool[i].id==id)return p->pool[i].weight;return 0;}
static RdRitualRequirement exact(int id) {RdRitualRequirement q={0};q.exact_card=id;q.type=RD_ANY;q.min_attack=q.max_attack=q.min_defense=q.max_defense=q.min_level=q.max_level=RD_ANY;return q;}
static int match(const RdOpponentProfile *p,int id,const RdRitualRequirement *q,unsigned role,void *ctx) {
 (void)p;(void)role;(void)ctx;return id==q->exact_card;
}
int main(void){
 const struct {int index,duelist,magic,result,n;int materials[3];} cases[]={
 {28,11,676,701,118,{395,38,403}},
 {4,13,693,720,118,{102,49,38}},
 {17,24,678,703,139,{453,403,38}},
 {20,29,697,716,70,{418,38,49}},
 {3,10,696,717,73,{52,533,478}}
 };
 (void)cases[0].duelist;
 assert(Rd_ProfileCount()==29 && Rd_GetProfile(29)==0);
 for(int n=0;n<5;n++){
  const RdOpponentProfile *p=Rd_GetProfile(cases[n].index);
  RdRitualRecipe r=p->recipe;RdRitualRequirement req[3];RdPackageStats stats;
  u16 deck[40];int sum=0;
  assert(p && p->pool_count==cases[n].n);
  assert(r.ritual_magic==cases[n].magic && r.result_card==cases[n].result && !r.material_count);
  assert(!count_weight(p,r.ritual_magic));
  for(int i=0;i<p->pool_count;i++)sum+=p->pool[i].weight;
  assert(sum==2018 && sum+RD_PACKAGE_WEIGHT==2048);
  for(int i=0;i<3;i++)req[i]=exact(cases[n].materials[i]);
  r.materials=req;r.material_count=3;
  for(int mode=0;mode<3;mode++)for(int run=0;run<240;run++){
   int used[723]={0};int packs=0;
   assert(Rd_GeneratePackageDeck(p,&r,match,0,722,mode?6:2,mode==2,deck,rng,0,&stats)==40);
   assert(stats.packages<=3);
   if(mode==0 || mode==2)assert(stats.packages>=1);
   for(int i=0;i<40;i++)assert(deck[i]>=1&&deck[i]<=722&&++used[deck[i]]<=3);
   for(int i=0;i<40;i++)if(deck[i]==r.ritual_magic)packs++;
   assert(packs==stats.packages);
   for(int k=0;k<stats.packages;k++){
    assert(stats.included[k][0]==r.ritual_magic);
    for(int j=0;j<3;j++)assert(stats.included[k][j+1]==cases[n].materials[j]);
   }
  }
 }
 /* Cross-mod recipe assignment must be exact; ready-made card is independent. */
 assert(count_weight(Rd_GetProfile(4),720)==20 && count_weight(Rd_GetProfile(4),716)==0);
 assert(count_weight(Rd_GetProfile(17),717)==48 && count_weight(Rd_GetProfile(17),453)==48);
 assert(count_weight(Rd_GetProfile(17),16)==48 && count_weight(Rd_GetProfile(17),72)==48);
 assert(count_weight(Rd_GetProfile(17),56)==48 && count_weight(Rd_GetProfile(17),339)==48);
 assert(count_weight(Rd_GetProfile(3),72)==30 && count_weight(Rd_GetProfile(3),339)==12);
 assert(count_weight(Rd_GetProfile(20),653)==98 && count_weight(Rd_GetProfile(28),701)==30);
 puts("PASS V4: five profiles, source weights, Story and Free Duel, 40 cards, virtual packages and active-recipe examples");
 return 0;
}
