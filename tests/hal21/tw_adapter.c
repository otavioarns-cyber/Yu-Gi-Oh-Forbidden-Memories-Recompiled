/* Local-only build of the supplied bridge, with core/wheel boundaries modeled.
 * The external source is hash-checked and copied to a temporary include dir.
 * Neither that source nor this test binary is redistributed as a Time Wizard MOD. */
#define MemoriesModInit Hal21_TimeWizardBridgeInit
#include "time_wizard_under_test.c"
#undef MemoriesModInit
int tw_ext_phase,tw_ext_chosen,tw_ext_spin_id,tw_ext_chance_50,tw_ext_test_result;
uint32_t tw_ext_rng;
u8 D_8009B1D5;u16 gDuel_wSceneStateFlags;
static WheelRequest saved_request;static int next_id,finished;
static int fake_spin(const void*p){saved_request=*(const WheelRequest*)p;tw_ext_spin_id=++next_id;return next_id;}
static int fake_busy(void){return 0;}
static int fake_result(int id){(void)id;return 0;}
static void fake_cancel(void){}
static WheelApi actual_wheel={1,fake_spin,fake_busy,fake_result,fake_cancel};
WheelApi *wheel;
int TimeWizardV0232OriginalInit(const MemoriesModHost*h,MemoriesMod*m){(void)h;(void)m;wheel=&actual_wheel;return 1;}
static void done(int id,int slice,int kind,void*user){(void)id;(void)slice;(void)kind;(void)user;++finished;}
void Hal21_TwProbe(const MemoriesModHost*h,void(*toggle)(int),int out[4]){
 MemoriesMod m={0};tw_ext_rng=123;D_8009B1D5=0;gDuel_wSceneStateFlags=6;tw_ext_phase=1;tw_ext_chance_50=0;finished=0;
 out[0]=Hal21_TimeWizardBridgeInit(h,&m);WheelRequest request={6,0,0,0,"fixture",0,done,0};
 toggle(1);int id=wheel->spin(&request);out[1]=eligible;toggle(0);saved_request.done(id,0,1,saved_request.user);out[2]=pending;
 m.reset();id=wheel->spin(&request);saved_request.done(id,0,1,saved_request.user);out[3]=pending;
 m.reset();toggle(1);
}
