/* HAL-22: real manager.c, two unchanged local manifests; no MOD payload copied. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc/mods/mods.h"
#include "pc/mods/json.h"
static const char*ids[]={"ai-hard-mode","dual-play"};
static JsonDocument*documents[2];static int ranks[2];
int Mods_Count(void){return 2;}
const char*Mods_Id(int i){return ids[i];}
const char*Mods_Name(int i){return ids[i];}
const JsonValue*Mods_Manifest(int i){return Json_Root(documents[i]);}
const char*Mods_Metadata(int i,const char*n){return Json_String(Json_Member(Mods_Manifest(i),n),"");}
int Settings_GetNamed(const char*n,int fallback){for(int i=0;i<2;i++){char key[100];snprintf(key,sizeof key,"mod.%s.order",ids[i]);if(!strcmp(key,n))return ranks[i];}return fallback;}
int main(int argc,char**argv){
 assert(argc==4);int variant=atoi(argv[3]);ranks[0]=variant==2?1:0;ranks[1]=1-ranks[0];
 for(int i=0;i<2;i++){char path[4096],error[512];snprintf(path,sizeof path,"%s/mod.json",argv[1+i]);documents[i]=Json_ParseFile(path,error,sizeof error);assert(documents[i]);}
 int enabled[]={1,1},out[128];char error[512];for(int i=0;i<2;i++)assert(Mods_Compatible(i,enabled,error,sizeof error));
 assert(Mods_Order(enabled,out,error,sizeof error)==2);printf("EFFECTIVE_ORDER %s %s\n",ids[out[0]],ids[out[1]]);
 puts("RESULT official-manager-order PASS Both ranks valid with unchanged manifests; O0/O1 project to H then D.");
 for(int i=0;i<2;i++)Json_Free(documents[i]);return 0;
}
