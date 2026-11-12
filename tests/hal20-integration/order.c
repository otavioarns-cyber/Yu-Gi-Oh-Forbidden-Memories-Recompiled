/* Official manager.c ordering/compatibility, exact manifests; no full runtime. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc/mods/mods.h"
#include "pc/mods/json.h"
static const char*ids[]={"melhoria-dos-rituais","ai-hard-mode","dual-play","fusion-auxiliary-balance","ritual-card-effects-test","ritual-duelists","drop-missing-cards"};
static JsonDocument*documents[7];static int ranks[7];
int Mods_Count(void){return 7;}
const char*Mods_Id(int i){return ids[i];}
const char*Mods_Name(int i){return ids[i];}
const JsonValue*Mods_Manifest(int i){return Json_Root(documents[i]);}
const char*Mods_Metadata(int i,const char*n){return Json_String(Json_Member(Mods_Manifest(i),n),"");}
int Settings_GetNamed(const char*n,int fallback){for(int i=0;i<7;i++){char key[100];snprintf(key,sizeof key,"mod.%s.order",ids[i]);if(!strcmp(key,n))return ranks[i];}return fallback;}
int main(int argc,char**argv){
 assert(argc==7);int stage=atoi(argv[3]),hm=atoi(argv[4]),dual=atoi(argv[5]),variant=atoi(argv[6]);
 int normal[]={6,0,1,2,3,4,5},dual_outer[]={6,0,1,3,4,5,2},hm_outer[]={6,0,2,3,5,1,4};int*requested=variant==1?dual_outer:variant==2?hm_outer:normal;
 for(int i=0;i<7;i++){char path[4096],error[512];ranks[requested[i]]=i;
  if(i==2)snprintf(path,sizeof path,"%s/mod.json",argv[2]);
  else snprintf(path,sizeof path,"%s/mods/%s/mod.json",argv[1],i==4?"ritual-card-effects":ids[i]);
  documents[i]=Json_ParseFile(path,error,sizeof error);assert(documents[i]);
 }
 int enabled[]={1,hm,dual,stage>=3,stage>=2,stage>=4,1},out[128];char error[512];
 assert(!Json_Bool(Json_Member(Mods_Manifest(6),"enabled"),1)); /* base default unchanged */
 for(int i=0;i<7;i++)if(enabled[i])assert(Mods_Compatible(i,enabled,error,sizeof error));
 int count=Mods_Order(enabled,out,error,sizeof error);assert(count>=0);
 printf("EFFECTIVE_ORDER");for(int i=0;i<count;i++)printf(" %s",ids[out[i]]);puts("");
 if(stage>=4){int drop=-1,rd=-1;for(int i=0;i<count;i++){if(out[i]==6)drop=i;if(out[i]==5)rd=i;}assert(drop>=0&&rd>drop);
  enabled[6]=0;assert(Mods_Compatible(5,enabled,error,sizeof error));assert(Mods_Order(enabled,out,error,sizeof error)==count-1);
 }
 puts("RESULT official-manager-order PASS Drop Missing Cards enabled in test profile; after is ordering, not requires");
 for(int i=0;i<7;i++)Json_Free(documents[i]);return 0;
}
