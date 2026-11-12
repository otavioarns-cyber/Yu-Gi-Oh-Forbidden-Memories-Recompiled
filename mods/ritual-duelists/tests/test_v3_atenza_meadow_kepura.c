/* V3 acceptance: 3 new Ritual Packages, independent real pools and recipes.
 * This models active recipe-provider requirements; it does not claim to
 * execute the external mod's actual ritual summoning code. */
#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned seed=0x6174656eu;
static int rng(int n,void *arg) { (void)arg;seed=seed*1664525u+1013904223u;return (int)((seed>>1)%(unsigned)n); }
static int match(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned role,void *ctx){
 (void)p;(void)role;(void)ctx;
 if(id<1||id>722)return 0;
 if(r->exact_card>0)return id==r->exact_card;
 /* Synthetic provider categories and base-ATK validity: test the adapter
  * dispatch instead of hardcoding a monster role in Ritual Duelists. */
 if(r->type==101)return id==49||id==50;   /* Fiend */
 if(r->type==102)return id==39||id==425;  /* Dragon */
 if(r->type==103)return id==403||id==483; /* Strong Beast */
 if(r->type==104)return id==38||id==7;   /* Strong Warrior */
 return 0;
}
static RdRitualRequirement exact(int id){
 RdRitualRequirement q={0};q.exact_card=id;q.type=RD_ANY;
 q.min_attack=q.max_attack=q.min_defense=q.max_defense=q.min_level=q.max_level=RD_ANY;return q;
}
static RdRitualRequirement typed(int type){RdRitualRequirement q=exact(0);q.type=type;return q;}
static int weight(const RdOpponentProfile *p,int card){
 int i;for(i=0;i<p->pool_count;i++)if(p->pool[i].id==card)return p->pool[i].weight;return 0;
}
static int count(const u16 *cards,int target){int i,n=0;for(i=0;i<40;i++)n+=cards[i]==target;return n;}
static void check(int ix,const char *name,int magic,int ready,int size,int ready_weight){
 int i,j,sum=0;const RdOpponentProfile *p=Rd_GetProfile(ix);
 assert(p&&strcmp(p->duelist_name,name)==0&&p->pool_count==size);
 assert(p->recipe.ritual_magic==magic&&p->recipe.result_card==ready);
 assert(weight(p,ready)==ready_weight&&weight(p,magic)==0);
 assert(p->recipe.material_count==0); /* Never replace active 3-slot recipe. */
 for(i=0;i<p->pool_count;i++){
   sum+=p->pool[i].weight;
   for(j=i+1;j<p->pool_count;j++)assert(p->pool[i].id!=p->pool[j].id);
 }
 assert(sum==2018 && sum+RD_PACKAGE_WEIGHT==2048);
}
static void run(int ix,int variant){
 const RdOpponentProfile *p=Rd_GetProfile(ix);RdRitualRecipe r=p->recipe;
 RdRitualRequirement requirements[3];RdPackageStats stats;u16 deck[40];
 int mode,n,unused=0;
 if(ix==19){requirements[0]=exact(170);requirements[1]=exact(49);requirements[2]=exact(39);
     if(variant){requirements[1]=typed(101);requirements[2]=typed(102);}}
 else if(ix==20){requirements[0]=exact(453);requirements[1]=exact(403);requirements[2]=exact(38);
     if(variant){requirements[1]=typed(103);requirements[2]=typed(104);}}
 else {requirements[0]=exact(38);requirements[1]=exact(39);requirements[2]=exact(417);
     if(variant){requirements[1]=typed(102);requirements[2]=exact(425);}}
 r.materials=requirements;r.material_count=3;
 for(mode=0;mode<3;mode++)for(n=0;n<500;n++){
  int i,j,used[723]={0},has[4]={0};
  int free_mode=(mode!=0),guarantee=(mode==2);
  assert(Rd_GeneratePackageDeck(p,&r,match,0,722,free_mode?6:2,guarantee,
                                deck,rng,0,&stats)==40);
  assert(stats.packages>=0&&stats.packages<=3);
  if(!free_mode||guarantee)assert(stats.packages>=1);
  if(free_mode&&!guarantee&&stats.packages==0)unused++;
  for(i=0;i<40;i++)assert(deck[i]>0&&deck[i]<=722&&++used[deck[i]]<=3);
  assert(count(deck,r.ritual_magic)==stats.packages);
  for(i=0;i<stats.packages;i++){
   assert(stats.included[i][0]==r.ritual_magic);
   for(j=0;j<3;j++){
     int id=stats.included[i][j+1];
     assert(match(p,id,&requirements[j],j,0));
     assert(id!=r.result_card);
     has[j]++;
   }
  }
  for(j=0;j<3;j++)assert(has[j]==stats.packages);
  if(ix==20 && stats.packages>0)assert(used[453]>=stats.packages); /* off-pool tribute */
 }
 assert(unused>0); /* Free Duel OFF must not guarantee a package. */
 printf("PASS %s: 3000 40-card decks; active %s recipes; Story / Free OFF / Free ON\n",
        p->duelist_name,variant?"typed":"exact");
}
int main(void){
 assert(Rd_ProfileCount()==29&&Rd_GetProfile(29)==0&&RD_PROFILE_CAPACITY>=29);
 check(19,"High Mage Atenza",691,706,148,32);
 check(20,"Meadow Mage",697,716,70,0);
 check(21,"High Mage Kepura",670,364,165,30);
 assert(weight(Rd_GetProfile(19),680)==0&&weight(Rd_GetProfile(19),327)==50);
 assert(weight(Rd_GetProfile(19),712)==1&&weight(Rd_GetProfile(19),217)==32);
 assert(weight(Rd_GetProfile(20),653)==98&&weight(Rd_GetProfile(20),677)==40);
 assert(weight(Rd_GetProfile(20),453)==0&&weight(Rd_GetProfile(20),703)==0);
 assert(weight(Rd_GetProfile(21),697)==0&&weight(Rd_GetProfile(21),302)==92);
 assert(weight(Rd_GetProfile(21),364)==30&&weight(Rd_GetProfile(21),37)==64);
 for(int i=19;i<22;i++)for(int variant=0;variant<2;variant++)run(i,variant);
 puts("PASS V3 Atenza/Meadow/Kepura: no nonapproved changes, 3 virtual packages");
 return 0;
}
