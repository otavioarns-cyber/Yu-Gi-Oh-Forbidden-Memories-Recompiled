/* Test-only bridge to official base; no guard source imported. */
#include "../../src/pc/mods/mods.c"
void Hal25_ForceApplied(int i,int on){activate(i,on);}
const MemoriesModHost*Hal25_Host(int i){return &mods[i].host;}
