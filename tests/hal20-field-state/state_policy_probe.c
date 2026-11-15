/* Exact manager predicate, no engine/ROM. No load is executed by this probe. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "pc/mods/mods.h"
static int guard,mask;
int Mods_Count(void){return 7;}
const char*Mods_Id(int i){static const char*ids[]={"ai-hard-mode","dual-play","field-effects-secondary-types","ritual-card-effects-test","ritual-duelists","fusion-auxiliary-balance","melhoria-dos-rituais"};return ids[i];}
int Mods_Active(int i){return mask&(1<<i);}
int Settings_GetNamed(const char*key,int fallback){return !strcmp(key,"experimental.hal2224_order_guard")?guard:fallback;}
int main(void){const int masks[]={0,1,2,4,8,16,32,64,127};unsigned checks=0;for(guard=0;guard<=1;guard++)for(unsigned i=0;i<sizeof masks/sizeof*masks;i++){mask=masks[i];char error[256]={0};int allow=Mods_Hal20StateLoadAllowed(error,sizeof error);int expected=!guard||!(mask&124);assert(allow==expected);if(!allow)assert(strstr(error,"current game was kept"));++checks;}printf("PASS actual load-policy predicate: %u profiles; Field-only refusal and H/D-only legacy behavior included\n",checks);return 0;}
