/* Test-only access to the official lifecycle; no core source edit.
 * Force bypasses restart as instrumentation, not a supported UI transition. */
#include "../../src/pc/mods/mods.c"
void Hal21_ForceApplied(int index,int on){activate(index,on);}
const MemoriesModHost*Hal21_Host(int index){return &mods[index].host;}
void Hal21_CallbackOnly(int index,int on){if(mods[index].hooks.applied)mods[index].hooks.applied(on);}
