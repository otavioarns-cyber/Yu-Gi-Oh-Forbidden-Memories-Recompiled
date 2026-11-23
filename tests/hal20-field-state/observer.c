/* Test-owned after-planner observer. It does not alter selection or pending. */
#include "pc/mods/modapi.h"
static const MemoriesModHost*host;static void*previous;static void(*after)(void);
static int (*pause_controller)(void);
static int run(void){int value=((int(*)(void))previous)();after();return pause_controller&&pause_controller()?0:value;}
int MemoriesModInit(const MemoriesModHost*h,MemoriesMod*m){host=h;m->api=24;pause_controller=host->symbol(host,"HAL_RD_DUEL_PAUSE");after=host->symbol(host,"HAL2224_AfterPlanner");return after&&host->hook(host,host->symbol(host,"AiScript_Run"),run,&previous);}
