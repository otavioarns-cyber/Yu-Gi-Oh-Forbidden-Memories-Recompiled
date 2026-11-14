#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int weight(const RdOpponentProfile *p,int cid){int i;for(i=0;i<p->pool_count;i++)if(p->pool[i].id==cid)return p->pool[i].weight;return 0;}
static void check(int ix,const char *name,int magic,int result,int count,int ready){
 const RdOpponentProfile *p=Rd_GetProfile(ix);int i,j,sum=0;
 assert(p&&strcmp(p->duelist_name,name)==0);assert(p->pool_count==count);
 assert(p->recipe.ritual_magic==magic&&p->recipe.result_card==result);
 assert(weight(p,magic)==0&&weight(p,result)==ready);
 for(i=0;i<count;i++){
  assert(p->pool[i].weight>0);sum+=p->pool[i].weight;
  for(j=i+1;j<count;j++)assert(p->pool[i].id!=p->pool[j].id);
 }
 assert(sum==2018 && sum+RD_PACKAGE_WEIGHT==2048);
}
int main(void){
 const RdOpponentProfile *a,*b,*c;
 int i;
 const int forest_ban[]={40,76,133,291,473,529};
 const int mountain_ban[]={294,604,329,691};
 assert(Rd_ProfileCount()==29&&Rd_GetProfile(29)==0&&RD_PROFILE_CAPACITY>=29);
 check(16,"Forest Mage",673,356,83,30);
 check(17,"High Mage Anubisius",678,703,139,0);
 check(18,"Mountain Mage",666,357,60,16);
 a=Rd_GetProfile(16);b=Rd_GetProfile(17);c=Rd_GetProfile(18);
 for(i=0;i<6;i++)assert(weight(a,forest_ban[i])==0);
 assert(weight(a,403)==40&&weight(a,483)==40&&weight(a,714)==48&&weight(a,330)==160);
 assert(a->baseline_material_count==2&&a->baseline_materials[0]==403&&a->baseline_materials[1]==483);
 assert(weight(b,678)==0&&weight(b,16)==48&&weight(b,57)==48&&weight(b,72)==48&&weight(b,717)==48);
 assert(weight(b,56)==48&&weight(b,339)==48&&weight(b,67)==48&&weight(b,453)==48);
 assert(weight(c,37)==0&&weight(c,358)==144&&weight(c,38)==88&&weight(c,39)==96&&weight(c,168)==128);
 assert(weight(Rd_GetProfile(19),37)==32); /* Gaia stays in High Mage Atenza unchanged. */
 assert(weight(c,713)==15&&weight(c,712)==32&&weight(c,82)==20&&weight(c,332)==160);
 for(i=0;i<4;i++)assert(weight(c,mountain_ban[i])==0);
 assert(weight(c,343)==24&&weight(c,344)==32&&weight(c,345)==40&&weight(c,346)==86&&weight(c,347)==64);
 for(i=0;i<16;i++)assert(Rd_GetProfile(i)!=a&&Rd_GetProfile(i)!=b&&Rd_GetProfile(i)!=c);
 puts("PASS V3: 19 profiles, 83/162/60 cards, exact identity weights, rituals package-only");
 return 0;
}
