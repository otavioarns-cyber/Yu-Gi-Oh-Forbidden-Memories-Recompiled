/* Test-owned after-planner observer. It does not alter selection or pending. */
#include "pc/mods/modapi.h"
static const MemoriesModHost*host;static void*previous;static void(*after)(void);
static int run(void){int value=((int(*)(void))previous)();after();return value;}
int MemoriesModInit(const MemoriesModHost*h,MemoriesMod*m){host=h;m->api=24;after=host->symbol(host,"HAL24_AfterPlanner");return after&&host->hook(host,host->symbol(host,"AiScript_Run"),run,&previous);}
