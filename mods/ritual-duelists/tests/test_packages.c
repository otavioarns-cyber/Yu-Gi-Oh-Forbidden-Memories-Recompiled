#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned seed=1972021u;
static int rng(int n,void *ctx){(void)ctx;seed=seed*1664525u+1013904223u;return (int)((seed>>1)%(unsigned)n);}
static int zero_rng(int n,void *ctx){(void)n;(void)ctx;return 0;}
static int matches(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned role,void *ctx){
 (void)p;(void)role;(void)ctx;
 if(id<1||id>722)return 0;
 if(r->exact_card>0)return id==r->exact_card;
 if(r->type>=0) {
   if(r->type==2)return id==30||id==99||id==132||id==564||id==545||id==98||id==146;
   if(r->type==19)return id==123||id==637||id==547;
   if(r->type==17)return id==133||id==473;
   if(r->type==9)return id==49||id==50||id==52||id==533||id==535;
   if(r->type==14)return id==405||id==416||id==296;
   if(r->type==18)return id==74||id==366||id==531||id==632;
   if(r->type==3)return id==14||id==35||id==378;
   if(r->type==7)return id==621||id==22||id==595||id==261;
   return 0;
 }
 return 1;
}
/* Defense-recipe mock: #74 Giant Soldier of Stone and #255 Prevent Rat
 * satisfy the modified Shield requirements; weak/over-3000 candidates do
 * not. This tests dynamic stats conditions rather than a fixed card ID trio. */
static int matches_defense(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned role,void *ctx){
 (void)p;(void)role;(void)ctx;
 if(r->exact_card>0)return id==r->exact_card;
 if(r->type>=0)return 0;
 {
   int atk=0,def=0;
   if(id==74){atk=1300;def=2000;}
   else if(id==255){atk=500;def=1100;}
   else if(id==362){atk=0;def=3000;}
   else return 0;
   if(r->min_defense>=0 && def<r->min_defense)return 0;
   if(r->max_defense>=0 && def>r->max_defense)return 0;
   if(r->defense_gt_attack && def<=atk)return 0;
   return 1;
 }
}
static RdRitualRequirement exact(int id){RdRitualRequirement r={0};r.exact_card=id;r.type=-1;r.min_attack=r.max_attack=r.min_defense=r.max_defense=r.min_level=r.max_level=-1;return r;}
static RdRitualRequirement typed(int type){RdRitualRequirement r=exact(0);r.type=type;return r;}
static int count(const u16 *cards,int id){int n=0,i;for(i=0;i<40;i++)n+=cards[i]==id;return n;}
static void init_recipe(int idx,const RdOpponentProfile *p,RdRitualRecipe *r,RdRitualRequirement m[3]){
 static const int ids[19][3]={{535,545,378},{14,123,133},{14,637,473},{52,533,49},{239,378,621},{7,35,417},{7,35,417},{6,47,571},{6,47,571},{146,479,485},{186,261,595},{166,405,416},{255,296,629},{90,582,109},{436,441,542},{89,449,451},{403,483,487},{52,533,49},{7,39,31}};
 int j;*r=p->recipe;
 for(j=0;j<3;j++)m[j]=exact(ids[idx][j]);r->materials=m;r->material_count=3;
}
int main(void){
 int idx,mode,t,seen_0=0,seen_3=0,totals[3][4]={{0}},trials=2400;
 const RdOpponentProfile *p;
 assert(Rd_ProfileCount()==29);
 for(idx=1;idx<19;idx++){
   RdRitualRecipe recipe;RdRitualRequirement roles[3];int weight=0,i,j;
   p=Rd_GetProfile(idx);assert(p);init_recipe(idx,p,&recipe,roles);
   for(i=0;i<p->pool_count;i++){
     assert(p->pool[i].weight>0);
     for(j=0;j<i;j++)assert(p->pool[j].id!=p->pool[i].id);
     weight+=p->pool[i].weight;
   }
   assert(weight==2018&&weight+RD_PACKAGE_WEIGHT==2048);
   for(mode=0;mode<3;mode++){
     int counts[4]={0};
     for(t=0;t<trials;t++){
       RdPackageStats st={0};u16 deck[40];int k,by_id[723]={0};
       int game_mode=mode==0?2:6,force=mode==2;
       assert(Rd_GeneratePackageDeck(p,&recipe,matches,0,722,game_mode,force,
                                    deck,rng,0,&st)==40);
       assert(st.packages>=0&&st.packages<=3);
       if(mode!=1)assert(st.packages>=1);
       for(k=0;k<40;k++){assert(deck[k]>0&&deck[k]<=722);assert(++by_id[deck[k]]<=3);}
       for(k=0;k<st.packages;k++){
         assert(st.included[k][0]==recipe.ritual_magic);
         for(j=0;j<3;j++)assert(st.included[k][j+1]==roles[j].exact_card);
       }
       assert(count(deck,recipe.ritual_magic)==st.packages);
       counts[st.packages]++;
     }
     printf("%s %s: 0=%d 1=%d 2=%d 3=%d / %d\n",p->duelist_name,
            mode==0?"Story":mode==1?"Free OFF":"Free ON",
            counts[0],counts[1],counts[2],counts[3],trials);
     assert(counts[1]>0);
     if(mode==1){assert(counts[0]>0);seen_0+=counts[0];}
     seen_3+=counts[3];
   }
 }
 assert(seen_0>0);
 /* Forced selection reaches 3 complete bundles, without a fourth. */
 {RdRitualRecipe rec;RdRitualRequirement roles[3];RdPackageStats st;u16 cards[40];
  p=Rd_GetProfile(5);init_recipe(5,p,&rec,roles);
  assert(Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,zero_rng,0,&st)==40);
  assert(st.packages==3&&count(cards,721)==3&&count(cards,35)==3);
  assert(count(cards,722)<=3); /* Ritual result remains just normal weighted draw. */
  for(int i=0;i<3;i++)roles[i]=exact(7);
  assert(Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,zero_rng,0,&st)==40);
  assert(st.packages==1&&count(cards,721)==1&&count(cards,7)==3);
  roles[2]=exact(722); /* Result monster cannot also satisfy its own package tribute. */
  assert(!Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,rng,0,&st)&&st.error==1);
  roles[2]=exact(417);rec.material_count=2;
  assert(!Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,rng,0,&st)&&st.error==1);
 }
 /* Type-specific preferences follow active recipes even if off-pool. */
 {RdRitualRecipe rec;RdRitualRequirement roles[3];RdPackageStats st;u16 cards[40];
  p=Rd_GetProfile(2);init_recipe(2,p,&rec,roles);roles[1]=typed(19);roles[2]=typed(17);
  assert(Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,zero_rng,0,&st)==40);
  assert(st.packages>=1&&st.included[0][2]==637&&st.included[0][3]==473);
 }
 /* Time Wizard is weighted only in Weevil's real pool, not eligible in Javelin packages. */
 {RdRitualRecipe rec;RdRitualRequirement roles[3];RdPackageStats st;u16 cards[40];
  p=Rd_GetProfile(3);init_recipe(3,p,&rec,roles);roles[2]=exact(16);
  assert(!Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,rng,0,&st)&&st.error==1);
  roles[2]=typed(9);
  assert(Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,rng,0,&st)==40);
  for(int j=0;j<st.packages;j++)assert(st.included[j][3]!=16);
 }
 /* Pegasus' active recipe: #166 + Machine + Fiend, distinct materials.
    Generic materials can come from the weighted pool or catalog; the
    original Vanilla exact materials are only preferences. */
 {RdRitualRecipe rec;RdRitualRequirement roles[3];RdPackageStats st;u16 cards[40];
  p=Rd_GetProfile(11);init_recipe(11,p,&rec,roles);
  roles[1]=typed(14);roles[2]=typed(7);
  assert(Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,rng,0,&st)==40);
  assert(st.packages>=1);
  for(int j=0;j<st.packages;j++){
    assert(st.included[j][0]==695&&st.included[j][1]==166);
    assert(matches(p,st.included[j][2],&roles[1],1,0));
    assert(matches(p,st.included[j][3],&roles[2],2,0));
    assert(st.included[j][2]!=st.included[j][3]);
    for(int k=0;k<4;k++)assert(st.included[j][k]!=715);
  }
 }
 /* Mage Soldier's active defensive recipe: #296 + two DEF-dominant
    materials. Test against a mock eligibility subset, not a fake stock
    recipe hardcoded into the production mod. */
 {RdRitualRecipe rec;RdRitualRequirement roles[3];RdPackageStats st;u16 cards[40];
  p=Rd_GetProfile(12);init_recipe(12,p,&rec,roles);
  roles[0]=exact(296);
  roles[1]=typed(-1);roles[2]=typed(-1);
  for(int i=1;i<3;i++){
    roles[i].min_defense=1001;
    roles[i].max_defense=2999;
    roles[i].defense_gt_attack=1;
  }
  assert(Rd_GeneratePackageDeck(p,&rec,matches_defense,0,722,2,0,cards,rng,0,&st)==40);
  assert(st.packages>=1);
  for(int j=0;j<st.packages;j++){
    assert(st.included[j][0]==665&&st.included[j][1]==296);
    assert(matches_defense(p,st.included[j][2],&roles[1],1,0));
    assert(matches_defense(p,st.included[j][3],&roles[2],2,0));
    /* Different physical copies of the same ID may satisfy two generic roles. */
    for(int k=0;k<4;k++)assert(st.included[j][k]!=362);
  }
 }
 /* Desert's active modified recipe uses Temple of Skulls plus two Zombies
    while the Vanilla-only Insects (#479/#485) remain excluded from the pool. */
 {RdRitualRecipe rec;RdRitualRequirement roles[3];RdPackageStats st;u16 cards[40];
  p=Rd_GetProfile(9);init_recipe(9,p,&rec,roles);roles[1]=typed(2);roles[2]=typed(2);
  assert(Rd_GeneratePackageDeck(p,&rec,matches,0,722,2,0,cards,rng,0,&st)==40);
  for(int j=0;j<st.packages;j++)assert(st.included[j][1]==146&&st.included[j][2]!=479&&st.included[j][3]!=485);
 }
 puts("PASS Ritual Package distributions, legal decks, dynamic materials, cap and IDs 721/722");
 return 0;
}
