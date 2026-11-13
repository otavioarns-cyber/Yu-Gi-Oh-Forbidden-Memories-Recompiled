#include "pc/mods/modapi.h"
static const MemoriesModHost*host;
static void*previous_swap,*previous_draw;
static void swap(void){((void(*)(void))previous_swap)();}
static void draw(void){((void(*)(void))previous_draw)();}
int MemoriesModInit(const MemoriesModHost*h,MemoriesMod*m){
 host=h;m->api=24;
 return host->hook(host,host->symbol(host,"func_8001BAF0"),swap,&previous_swap)&&
        host->hook(host,host->symbol(host,"Duel_DrawFieldCards"),draw,&previous_draw);
}
