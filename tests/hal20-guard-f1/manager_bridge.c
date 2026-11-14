/* Test-only access, the exact PR20 core itself is not edited. */
#include "../../src/pc/mods/mods.c"
void Hal20_ForceApplied(int index,int on){activate(index,on);}
const MemoriesModHost*Hal20_Host(int index){return &mods[index].host;}
