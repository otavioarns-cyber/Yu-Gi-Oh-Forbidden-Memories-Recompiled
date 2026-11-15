/* Opt-in, API24+i386 experimental core imports. No fallback to heuristics. */
#include "pc/mods/modapi.h"
extern int TimeWizardContractOriginalInit(const MemoriesModHost*,MemoriesMod*);
extern int Hal9_CoreOwnersReadyV1(void);
int MemoriesModInit(const MemoriesModHost*h,MemoriesMod*m){
 if(!h||!m||h->api!=24||sizeof(void*)!=4||!Hal9_CoreOwnersReadyV1())return 0;
 return TimeWizardContractOriginalInit(h,m);
}
