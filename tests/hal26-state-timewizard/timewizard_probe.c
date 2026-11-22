/* Supplied Time Wizard source is included only from a private, hash-recorded
 * build directory. Core/wheel transport and TW side/phase are modeled here. */
#define MemoriesModInit Hal26_TimeWizardBridgeInit
#include "tw_encore_under_test.c"
#undef MemoriesModInit
int tw_ext_phase,tw_ext_chosen,tw_ext_spin_id,tw_ext_chance_50,tw_ext_test_result;
uint32_t tw_ext_rng;
u8 D_8009B1D5;u16 gDuel_wSceneStateFlags;
static WheelRequest h26_request;static int h26_next,h26_done_count;
static int h26_spin(const void*p){h26_request=*(const WheelRequest*)p;return tw_ext_spin_id=++h26_next;}
static int h26_busy(void){return 0;}static int h26_result(int id){(void)id;return 1;}static void h26_cancel(void){}
static WheelApi h26_wheel={1,h26_spin,h26_busy,h26_result,h26_cancel};WheelApi*wheel;
int TimeWizardV0232OriginalInit(const MemoriesModHost*h,MemoriesMod*m){(void)h;(void)m;wheel=&h26_wheel;return 1;}
static void h26_done(int id,int slice,int kind,void*p){(void)id;(void)slice;(void)kind;(void)p;++h26_done_count;}
static void *ai_original_run,*h26_native_ai,*h26_native_draw,*compat_original_draw,*cached_dual_draw;
typedef struct {uint8_t used[2],pending_return[2],bonus_hand[2],bonus_hand_count[2],return_stage[2],skip_cleanup[2],is_cpu[2],last_plan[2],remaining_after_plan[2];} DualPlayStatePrefix;
static DualPlayStatePrefix*cached_dual_state;static int diag_dual_seen;
#define PLAY_BOTH 3u
#define AiScript_Run h26_native_ai
#define DuelScene_UpdateDrawPhase h26_native_draw
#include "tw_predicates_under_test.h"
#undef AiScript_Run
#undef DuelScene_UpdateDrawPhase
void Hal26_TwProbe(const MemoriesModHost*h,void(*toggle)(int),void*ai_native,void*ai_predecessor,void*draw_native,void*draw_predecessor,int out[9]){
 MemoriesMod m={0};h26_done_count=0;tw_ext_rng=123;D_8009B1D5=0;gDuel_wSceneStateFlags=6;tw_ext_phase=1;tw_ext_chance_50=0;tw_ext_test_result=0;
 out[0]=Hal26_TimeWizardBridgeInit(h,&m);WheelRequest request={6,0,0,0,"HAL26 modeled wheel",0,h26_done,0};
 toggle(1);int id=wheel->spin(&request);out[1]=eligible;toggle(0);h26_request.done(id,2,1,h26_request.user);out[2]=pending;
 m.reset();id=wheel->spin(&request);h26_request.done(id,2,1,h26_request.user);out[3]=pending;
 m.reset();toggle(1);id=wheel->spin(&request);h26_request.done(id+99,2,1,h26_request.user);out[4]=h26_done_count==1&&!pending;wheel->cancel();h26_request.done(id,2,1,h26_request.user);out[4]&=!pending;
 m.reset();id=wheel->spin(&request);first_mode=1;uint32_t rng=tw_ext_rng;out[5]=fresh_choice()==-1&&tw_ext_rng==rng;
 gDuel_wSceneStateFlags=4;m.frame();out[6]=!active_id&&!pending;
 h26_native_ai=ai_native;ai_original_run=ai_predecessor;out[7]=ai_hard_mode_active();h26_native_draw=draw_native;compat_original_draw=draw_predecessor;cached_dual_draw=0;cached_dual_state=0;out[8]=resolve_dual_state()!=0;
 m.reset();toggle(1);
}
