/* Test-owned adversarial wrappers. No private MOD implementation copied. */
#include "pc/mods/modapi.h"
static const MemoriesModHost*h;static void*old_swap,*old_hand;static unsigned times;
static void(*edit)(int);
static int mode(void){return h->setting(h,"mode",0);}
static int allowed(void){int limit=h->setting(h,"limit",1);return !limit||times<(unsigned)limit;}
static void swap(void){int m=mode();if(m==1&&allowed()){++times;edit(m);}((void(*)(void))old_swap)();if(m==2&&allowed()){++times;edit(m);}}
static void hand(void){int m=mode();if(m==4)return;if((m==6||m==7)&&allowed()&&(*(unsigned short*)h->symbol(h,"D_8009B20C")&15)==3){++times;edit(m);}((void(*)(void))old_hand)();if((m==3||m==5)&&allowed()&&*(unsigned short*)h->symbol(h,"D_8009B20C")==1){++times;edit(m);}}
int MemoriesModInit(const MemoriesModHost*host,MemoriesMod*m){h=host;m->api=24;edit=h->symbol(h,"HAL_HARDENING_INTERPOSE");return edit&&h->hook(h,h->symbol(h,"func_8001BAF0"),swap,&old_swap)&&h->hook(h,h->symbol(h,"DuelScene_UpdateHandActions"),hand,&old_hand);}
