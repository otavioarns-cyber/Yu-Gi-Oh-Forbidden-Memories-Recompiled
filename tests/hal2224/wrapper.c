/* Own diagnostic wrapper. No Dual code/provider or materialization logic. */
#include "pc/mods/modapi.h"
static void*previous;static const MemoriesModHost*host;
static void wrapper(void){if(!host->setting(host,"swallow",0))((void(*)(void))previous)();}
int MemoriesModInit(const MemoriesModHost*h,MemoriesMod*m){host=h;m->api=24;return host->hook(host,host->symbol(host,host->setting(host,"unrelated",0)?"Duel_DrawFieldCards":host->setting(host,"controller",0)?"DuelScene_UpdateHandActions":"func_8001BAF0"),wrapper,&previous);}
