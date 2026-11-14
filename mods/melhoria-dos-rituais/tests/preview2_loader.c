/* Real rebuilt ELF and official loader; simulated host registration only.
 * Does NOT execute duel hooks, render, or emulate the complete mod manager. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc/mods/object_loader.h"
#include "pc/mods/exports.h"
#include "pc/mods/modapi.h"
static int hooks,active;
static unsigned char fake_engine_data[65536];
static void unused_engine_function(void){assert(!"duel engine must not run in this fixture");}
static void *resolve(const char *name,void *context){
 (void)context;void (*p)(void)=Mods_LibcLookup(name);if(p)return (void*)p;
 if(name[0]=='D'||name[0]=='g')return fake_engine_data;
 return (void*)unused_engine_function;
}
static void *symbol(const MemoriesModHost*h,const char*n){(void)h;(void)n;return (void*)unused_engine_function;}
static int hook(const MemoriesModHost*h,void*f,void*r,void**o){(void)h;assert(f&&r&&o);*o=(void*)unused_engine_function;return ++hooks;}
static int provide(const MemoriesModHost*h,const char*n,void*p){(void)h;assert(!strcmp(n,"rules_v1")&&p);return 1;}
static int applied(const MemoriesModHost*h){(void)h;return active;}
int main(int argc,char**argv){
 assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f);fseek(f,0,SEEK_END);long size=ftell(f);rewind(f);
 void*data=malloc(size);assert(data&&fread(data,1,size,f)==(size_t)size);fclose(f);
 for(int i=0;i<20;i++){
  LoadedObject object={0};char error[512];hooks=0;
  int loaded=ObjectLoader_Load(data,size,resolve,0,&object,error,sizeof(error));if(loaded)fprintf(stderr,"%s\n",error);assert(!loaded);
  int(*entry)(const MemoriesModHost*,MemoriesMod*)=ObjectLoader_Symbol(&object,"MemoriesModInit");assert(entry);
  MemoriesModHost host={0};MemoriesMod mod={0};host.api=24;host.hook=hook;host.provide=provide;host.symbol=symbol;host.applied=applied;
  assert(entry(&host,&mod)&&hooks==7&&mod.api==4&&mod.applied&&mod.reset);
  active=1;mod.applied(1);mod.reset();active=0;mod.applied(0);active=1;mod.applied(1);mod.reset();
  ObjectLoader_Free(&object);
 }
 free(data);puts("PASS rebuilt ELF: 20 load/init/reset/OFF/ON cycles, seven hook registrations; simulated host, no duel hooks executed");
}
