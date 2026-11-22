/* Link the actual manager.c, discarding unrelated sections. No game data. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "pc/mods/mods.h"
static unsigned live,resident;
static const char *ids[]={"ai-hard-mode","dual-play","field-effects-secondary-types","ritual-card-effects-test","ritual-duelists","fusion-auxiliary-balance","melhoria-dos-rituais","time-wizard-time-magic","drop-missing-cards","unqualified"};
int Mods_Count(void){return sizeof(ids)/sizeof(ids[0]);}
const char *Mods_Id(int n){return ids[n];}
int Mods_Active(int n){return (live>>n)&1;}
int Mods_CodeResident(int n){return (resident>>n)&1;}
int main(void){unsigned checks=0;for(live=0;live<1024;live++)for(resident=0;resident<1024;resident++){
 char error[256]={0};int allowed=Mods_Hal20StateLoadAllowed(error,sizeof error);assert(allowed==!((live|resident)&255));if(!allowed)assert(strstr(error,"current game was kept")&&strstr(error,"memory-card"));++checks;
}printf("PASS actual manager policy %u profiles including resident OFF/cold OFF/H/D-only/TW; independent of order guard\n",checks);return 0;}
