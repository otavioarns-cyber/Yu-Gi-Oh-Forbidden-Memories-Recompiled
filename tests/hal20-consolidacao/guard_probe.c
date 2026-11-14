/* Reference-only probe linked to the unmodified PR20 manager object.
 * Only the version gate is exercised. Hook-chain stubs must not be called. */
#include "pc/mods/mods.h"
#include "pc/mods/hooks.h"
#include "pc/platform/settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned rd_hash;static int ai_on,dual_on,chain_queries;
int Mods_Count(void){return 6;}
const char*Mods_Id(int m){static const char*ids[]={"ai-hard-mode","ritual-duelists","dual-play","melhoria-dos-rituais","ritual-card-effects-test","fusion-auxiliary-balance"};return ids[m];}
int Mods_Active(int m){return m==0?ai_on:m==2?dual_on:1;}
unsigned Mods_CodeHash(int m){return m==0?0x3e0648cb:m==1?rd_hash:m==2?0x3028c68a:0;}
int Settings_GetNamed(const char*key,int fallback){return !strcmp(key,"experimental.hal2224_order_guard")?1:fallback;}
void*Mods_Lookup(const char*name){(void)name;++chain_queries;return NULL;}
int Hooks_At(int index,int*owner,const void**fn){(void)index;(void)owner;(void)fn;++chain_queries;return 0;}
int main(int argc,char**argv){if(argc!=4)return 2;rd_hash=strtoul(argv[1],NULL,16);ai_on=atoi(argv[2]);dual_on=atoi(argv[3]);char error[512]={0};int ready=Mods_Hal2224GuardReady(error,sizeof error);printf("GUARD_REFERENCE ready=%d chain_queries=%d reason=%s\n",ready,chain_queries,error);return dual_on?((!ready&&!chain_queries&&strstr(error,"not qualified"))?0:1):((ready&&!chain_queries)?0:1);}
