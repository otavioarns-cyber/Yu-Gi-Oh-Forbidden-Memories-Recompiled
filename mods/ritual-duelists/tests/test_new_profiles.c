#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {int id,was;} Cut;
static int weight(const RdOpponentProfile *p,int id){int i;for(i=0;i<p->pool_count;i++)if(p->pool[i].id==id)return p->pool[i].weight;return 0;}
static void check(int idx,const char *name,int ready,int magic,int count,
                  const int *roles,const Cut *cuts,int other)
{
 const RdOpponentProfile *p=Rd_GetProfile(idx);int i,j,sum=0;
 assert(p&&strcmp(p->duelist_name,name)==0&&p->pool_count==count);
 assert(p->recipe.result_card==ready&&p->recipe.ritual_magic==magic);
 assert(p->baseline_material_count==3);
 for(i=0;i<3;i++)assert(p->baseline_materials[i]==roles[i]);
 assert(weight(p,ready)==30&&weight(p,magic)==0);
 for(i=0;i<12;i++){
   assert(cuts[i].was>5&&weight(p,cuts[i].id)==cuts[i].was-5);
   for(j=i+1;j<12;j++)assert(cuts[i].id!=cuts[j].id);
 }
 for(i=0;i<p->pool_count;i++)sum+=p->pool[i].weight;
 assert(sum==2018 && sum+RD_PACKAGE_WEIGHT==2048);
 /* The same ready ritual card must not contaminate another duelist's pool. */
 assert(weight(Rd_GetProfile(other),ready)!=30);
}
int main(void){
 static const Cut bakura[12]={{493,64},{128,48},{111,72},{131,32},{362,32},{349,80},{106,48},{633,48},{170,32},{89,32},{542,32},{323,32}};
 static const Cut pegasus[12]={{577,64},{382,48},{528,48},{357,48},{712,48},{42,48},{337,160},{668,120},{672,120},{329,120},{684,120},{690,120}};
 static const Cut soldier[12]={{233,48},{25,42},{164,40},{281,40},{600,32},{171,24},{60,24},{64,24},{430,24},{363,24},{234,24},{647,24}};
 static const int br[3]={186,261,595},pr[3]={166,405,416},sr[3]={255,296,629};
 assert(Rd_ProfileCount()==29&&Rd_GetProfile(29)==0&&RD_PROFILE_CAPACITY>=29);
 check(10,"Yami Bakura",365,674,129,br,bakura,11);
 check(11,"Pegasus",715,695,101,pr,pegasus,10);
 check(12,"Mage Soldier",362,665,151,sr,soldier,11);
 assert(weight(Rd_GetProfile(10),362)==27); /* A normal Shield, not a Ritual */
 assert(weight(Rd_GetProfile(12),335)==12&&weight(Rd_GetProfile(12),341)==24&&weight(Rd_GetProfile(12),346)==12);
 assert(weight(Rd_GetProfile(10),365)==30&&weight(Rd_GetProfile(11),715)==30&&weight(Rd_GetProfile(12),362)==30);
 puts("PASS: Bakura/Pegasus/Mage Soldier exact 36 cuts; 3 ready cards and 3 virtual packages; 2018+30/40-card prerequisites");return 0;
}
