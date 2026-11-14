#include "starter_rules.h"
#include "pc/cards/cards.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int gCard_nCount=722;
static int request_dynamic;
static void test_log(const MemoriesModHost *h,const char *f,...){(void)h;(void)f;}
int Tables_Ritual(int magic,unsigned short out[6]){(void)magic;(void)out;return -1;}
int Cards_Valid(int card){return card>=1&&card<=gCard_nCount;}
int Rd_ResolveStarterRecipe(const MemoriesModHost *host,const RdOpponentProfile *p,int mode,RdRecipeCache *out){
  (void)host;
  if(!request_dynamic||mode==1)return 0;
  memset(out,0,sizeof(*out));out->recipe=p->recipe;
  out->recipe.material_count=3;
  out->recipe.materials=out->storage;
  for(int i=0;i<3;i++){
    RdRitualRequirement *r=&out->storage[i];
    r->exact_card= i==0?40:(i==1?41:42);
    r->type=r->min_attack=r->max_attack=r->min_defense=r->max_defense=r->min_level=r->max_level=-1;
  }
  out->source=1;out->status=1;
  return 1;
}
int Rd_MatchRecipeCard(const RdOpponentProfile *p,int card,
 const RdRitualRequirement *r,unsigned role,void *context){
 (void)p;(void)role;(void)context;return card==r->exact_card;
}
static int rng(int m,void *ctx){unsigned *seed=(unsigned*)ctx;*seed=*seed*1664525u+1013904223u;return m?(int)((*seed>>1)%(unsigned)m):0;}
static int count(const u16 *d,int id){int r=0;for(int i=0;i<40;i++)r+=d[i]==id;return r;}
int main(void){
 const int magics[]={693,676,666},fields[]={335,333,332},equips[]={323,301,315};
 const int tributes[3][3]={{102,182,220},{249,395,511},{29,31,101}};
 u16 native[40],result[40];unsigned seed=418923u;
 MemoriesModHost h={0};h.log=test_log;
 for(int i=0;i<40;i++)native[i]=(u16)(450+i);
 for(int t=0;t<3;t++)for(int dynamic=0;dynamic<2;dynamic++){
   request_dynamic=dynamic;
   assert(Rd_BuildThemedStarter(&h,t,0,native,result,rng,&seed));
   assert(count(result,magics[t])==1&&count(result,337)==1);
   assert(count(result,fields[t])==1&&count(result,equips[t])==1);
   for(int i=0;i<3;i++)assert(count(result,dynamic?40+i:tributes[t][i])==1);
   for(int i=0;i<40;i++){assert(result[i]>=1&&result[i]<=722);assert(count(result,result[i])<=3);}
   assert(Rd_StarterSimonTheme(t,0)!=t && Rd_StarterSimonTheme(t,1)!=t);
   assert(Rd_StarterSimonTheme(t,0)!=Rd_StarterSimonTheme(t,1));
   request_dynamic=1;
   assert(Rd_BuildThemedStarter(&h,t,1,native,result,rng,&seed));
   for(int i=0;i<3;i++)assert(count(result,tributes[t][i])==1);
 }
 puts("PASS starter choice: three Vanilla disc recipes, dynamic override, force Vanilla, 40 cards, max three copies, two other Simon themes");
 return 0;
}
