/* Test-only bridge to official base; no guard source imported. */
static unsigned hal9_test_hard_calls;
#define HAL9_TRACE_HARD() (++hal9_test_hard_calls)
#define clock_gettime HalSupportClock
#include "../../src/pc/mods/mods.c"
#undef clock_gettime
extern int clock_gettime(clockid_t,struct timespec*);
static uint64_t support_clock=1000000;
int HalSupportClock(clockid_t id,struct timespec*out){if(Settings_GetNamed("test.tw-case",0)>=25){out->tv_sec=support_clock/1000000;out->tv_nsec=(support_clock%1000000)*1000;return 0;}return clock_gettime(id,out);}
void HalSupportAdvance(void){support_clock+=16667;}
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

static unsigned hal9_recovery_reset_calls;
void __real_Mods_Reset(void);
void __wrap_Mods_Reset(void){++hal9_recovery_reset_calls;__real_Mods_Reset();}
unsigned Hal9_ResetCount(void){return hal9_recovery_reset_calls;}
