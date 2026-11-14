#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int weight(const RdOpponentProfile *p,int id) {int i; for(i=0;i<p->pool_count;i++)if(p->pool[i].id==id)return p->pool[i].weight;return 0;}
static void check(int idx,const char *name,int magic,int result,int count,int ready_weight){
 const RdOpponentProfile *p=Rd_GetProfile(idx);int i,j,total=0;
 assert(p && strcmp(p->duelist_name,name)==0 && p->pool_count==count);
 assert(p->recipe.ritual_magic==magic && p->recipe.result_card==result);
 assert(weight(p,magic)==0 && weight(p,result)==ready_weight);
 for(i=0;i<p->pool_count;i++){ assert(p->pool[i].weight>0);total+=p->pool[i].weight;
   for(j=0;j<i;j++) assert(p->pool[i].id!=p->pool[j].id); }
 assert(total==2018 && total+RD_PACKAGE_WEIGHT==2048);
}
int main(void) {
 static const int no_tea[]={62,63,330,331,332,333,334,335,348,698,24,58,126,393,399};
 static const int no_ocean[]={400,51,77,630,566,76,569,381,590,700};
 static const int no_sec[]={14,32,81,35,44,106,115,363,372,433,493,495,525,532,551,578,619,622,631,
 78,93,127,352,621,641,83,86,87,204,377,379,498,523,90,111,126,170,276,396,582,584,601,608,616,633,
 124,441,508,648,351,368,596,487,614,632};
 const RdOpponentProfile *a,*b,*c;int i;
 assert(Rd_ProfileCount()==29&&Rd_GetProfile(29)==0);
 check(13,"Teana 2nd",698,708,69,30);
 check(14,"Ocean Mage",700,718,78,30);
 check(15,"High Mage Secmeton",692,710,90,48);
 a=Rd_GetProfile(13);b=Rd_GetProfile(14);c=Rd_GetProfile(15);
 for(i=0;i<(int)(sizeof(no_tea)/sizeof(no_tea[0]));i++)assert(weight(a,no_tea[i])==0);
 for(i=0;i<(int)(sizeof(no_ocean)/sizeof(no_ocean[0]));i++)assert(weight(b,no_ocean[i])==0);
 for(i=0;i<(int)(sizeof(no_sec)/sizeof(no_sec[0]));i++)assert(weight(c,no_sec[i])==0);
 assert(weight(a,531)==69 && weight(a,708)==30 && weight(a,690)==40 && weight(a,659)==60);
 assert(weight(b,565)==73 && weight(b,437)==73 && weight(b,610)==36 && weight(b,334)==160 && weight(b,348)==48 && weight(b,664)==120);
 assert(weight(c,373)==90 && weight(c,223)==82 && weight(c,449)==64 && weight(c,451)==0 && weight(c,710)==48);
 assert(weight(c,334)==72 && weight(c,337)==40 && weight(c,672)==40 && weight(c,35)==0 && weight(c,372)==0);
 puts("PASS: three approved profiles: real totals, 69/78/90, explicit bans, weighted ready cards, package-only magic");
 return 0;
}
