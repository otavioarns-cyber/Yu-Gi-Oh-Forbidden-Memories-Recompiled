/* Test-only bridge to official base; no guard source imported. */
static unsigned hal9_test_hard_calls;
#define HAL9_TRACE_HARD() (++hal9_test_hard_calls)
#include "../../src/pc/mods/mods.c"
void Hal25_ForceApplied(int i,int on){activate(i,on);}
const MemoriesModHost*Hal25_Host(int i){return &mods[i].host;}

void Hal25_StopOne(int i){if(i>=0&&i<mod_count&&mods[i].initialized&&mods[i].hooks.shutdown)mods[i].hooks.shutdown();}

/* Actual core-query probes; test-only exports, not the public SDK. */
int Hal9_TestHard(void){return hal9_hard_state();}
const void *Hal9_TestDual(void){return hal9_dual_draw();}
int Hal9_TestReady(void){return hal9_owners_ready();}
void *Hal9_TestImport(const char*n,int i){return resolve(n,i>=0?&mods[i]:NULL);}

unsigned Hal9_TestCalls(void){return hal9_test_hard_calls;}

int Hal9_TestModAPI(int i){return mods[i].hooks.api;}
