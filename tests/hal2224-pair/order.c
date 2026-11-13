/* Real official/custom manifests; only metadata/order regression, no gameplay. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc/mods/mods.h"
#include "pc/mods/json.h"
static JsonDocument*docs[10];static int ranks[10],guard;
int Mods_Count(void){return 10;}
const char*Mods_Id(int i){return Json_String(Json_Member(Json_Root(docs[i]),"id"),"");}
const char*Mods_Name(int i){return Mods_Id(i);}
const JsonValue*Mods_Manifest(int i){return Json_Root(docs[i]);}
const char*Mods_Metadata(int i,const char*n){return Json_String(Json_Member(Mods_Manifest(i),n),"");}
int Mods_Active(int i){(void)i;return 0;}
unsigned Mods_CodeHash(int i){(void)i;return 0;}
int Mods_RequiresRestart(int i){return Json_Bool(Json_Member(Mods_Manifest(i),"restart"),0);}
void*Mods_Lookup(const char*n){static int swap,hand;return !strcmp(n,"func_8001BAF0")?&swap:!strcmp(n,"DuelScene_UpdateHandActions")?&hand:NULL;}
int Hooks_At(int i,int*owner,const void**target){(void)i;(void)owner;(void)target;return 0;}
int Settings_GetNamed(const char*n,int fallback){
 if(!strcmp(n,"experimental.hal2224_order_guard"))return guard;
 for(int i=0;i<10;i++){char key[128];snprintf(key,sizeof key,"mod.%s.order",Mods_Id(i));if(!strcmp(n,key))return ranks[i];}return fallback;
}
int main(int argc,char**argv){
 assert(argc==4);const char*paths[]={"ai-hard-mode","dual-play","melhoria-dos-rituais","ritual-card-effects","fusion-auxiliary-balance","ritual-duelists","drop-missing-cards","3d-monsters","hand-camera","yamyi-mods"};
 for(int i=0;i<10;i++){char path[4096],error[512];snprintf(path,sizeof path,"%s/%s/mod.json",i==1?argv[2]:i>=2&&i<=5?argv[3]:argv[1],i==1?"":paths[i]);docs[i]=Json_ParseFile(path,error,sizeof error);if(!docs[i])fprintf(stderr,"%s: %s\n",path,error);assert(docs[i]);}
 unsigned checks=0,accepted=0,rejected=0,single=0;
 for(int variant=0;variant<3;variant++)for(unsigned mask=0;mask<1024;mask++){
  int enabled[MODS_MAX]={0},before[MODS_MAX],after[MODS_MAX];char error[512];
  for(int i=0;i<10;i++){enabled[i]=(mask>>i)&1;ranks[i]=variant==0?i:variant==1?9-i:0;}
  guard=0;int a=Mods_Order(enabled,before,error,sizeof error);assert(a>=0);assert(Mods_Hal2224GuardValidate(enabled,NULL,error,sizeof error));
  guard=1;int b=Mods_Order(enabled,after,error,sizeof error);assert(a==b&&!memcmp(before,after,a*sizeof(int)));
  int hp=-1,dp=-1,rp=-1;for(int i=0;i<a;i++){if(before[i]==0)hp=i;if(before[i]==1)dp=i;if(before[i]==5)rp=i;}
  int expected=dp<0||((hp<0||hp<dp)&&(rp<0||rp<dp)&&(hp<0||rp<0||hp<rp));int actual=Mods_Hal2224GuardValidate(enabled,NULL,error,sizeof error);if(actual!=expected){fprintf(stderr,"ORDER_MISMATCH variant=%d mask=%u expected=%d actual=%d hp=%d rp=%d dp=%d why=%s\n",variant,mask,expected,actual,hp,rp,dp,error);}assert(actual==expected);if(actual)accepted++;else rejected++;
  if(dp<0||(hp<0&&rp<0)){assert(actual);single++;}
  // A staged order must also be evaluated without changing saved ranks.
  int staged[MODS_MAX];for(int i=0;i<10;i++)staged[i]=9-ranks[i];
  int saved[10];memcpy(saved,ranks,sizeof saved);for(int i=0;i<10;i++)ranks[i]=staged[i];
  int testorder[MODS_MAX];int n=Mods_Order(enabled,testorder,error,sizeof error);assert(n==a);
  int sh=-1,sr=-1,sd=-1;for(int i=0;i<n;i++){if(testorder[i]==0)sh=i;if(testorder[i]==5)sr=i;if(testorder[i]==1)sd=i;}
  int staged_expected=sd<0||((sh<0||sh<sd)&&(sr<0||sr<sd)&&(sh<0||sr<0||sh<sr));
  memcpy(ranks,saved,sizeof saved);assert(Mods_Hal2224GuardValidate(enabled,staged,error,sizeof error)==staged_expected);
  int again=Mods_Order(enabled,after,error,sizeof error);assert(again==a&&!memcmp(before,after,a*sizeof(int)));checks++;
 }
 printf("RESULT other-mod-order-regression PASS variants=3 masks=1024 comparisons=%u accepted=%u rejected=%u singleton_or_absent=%u; exact order unchanged with guard ON/OFF/staged checks\n",checks,accepted,rejected,single);
 for(int i=0;i<10;i++)Json_Free(docs[i]);return 0;
}
