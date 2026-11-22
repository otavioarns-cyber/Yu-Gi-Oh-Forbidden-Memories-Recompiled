/* Fixture for extracted, unmodified baseline/current rendering callbacks.
 * Native packets are compared at the shared sprite helper's argument boundary;
 * the unchanged GPU submission implementation is outside this fixture. */
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "render_constants.inc"

typedef int32_t s32;
typedef uint32_t u32;
typedef struct { int unused; } GsOT;
typedef struct { struct { int field_30,field_32; } h; } Position;
typedef struct DisplayObject { int field_6A,field_14; Position field_30; } DisplayObject;
typedef struct { unsigned flags; int card_id; void *data; DisplayObject *object; } DuelCardRecord;
typedef struct { int row,col; } DuelCardPickCursor;
typedef struct { int ritual; void *data; unsigned blades_used,horns,protection_used_turn; } Mark;
typedef struct Host Host;
struct Host {
    void (*overlay_size)(const Host *,int *,int *,int *);
    void (*draw_text)(const Host *,int,int,const char *,unsigned,int);
};

#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_FIELD_ROW_SIZE 5
#define DUEL_FIELD_SIDE_GRID_SLOT_COUNT 20
#define DUEL_SCENE_PHASE_MASK 0xF
#define DUEL_CARD_FLAG_OCCUPIED 0x8000
#define DUEL_CARD_EFFECT_FLAG_ACTIVE 0x8000

static DuelCardRecord D_801A7AD8[30];
static DisplayObject objects[32];
static Mark garma_marks[30],tri_marks[30],javelin_marks[30],hungry_marks[30],chakra_marks[30];
static int gDuel_wSceneStateFlags,D_8009B229,gDuel_bEffectState,gDuel_wCardEffectFlags;
static int crab_umi_visual_state,garma_live_hint,tri_live_hint;
static unsigned javelin_live_mask;
static int visual_popup_kind,visual_popup_frames,visual_popup_record_index,visual_popup_amount;
static int visual_popup_x,visual_popup_y,garma_relevant_frames,garma_relevant;
static void *garma_animation_data;
static int D_8009B174,D_8009B1D5,D_800907D8[40];
static DuelCardPickCursor cursor,*D_8009B1B4;
static int effect_enabled[800],skip_card_sprite,coin_port;
static struct { int active,style,shown,lift; } ritual_coin;
static struct { int dodge_ticks; DisplayObject *dodge_object; } coin_battle;
static int serpent_ghost_state,serpent_redirect_state,serpent_art_loaded;
static int serpent_ghost_x,serpent_ghost_y,ghost_fade,serpent_redirect_hold;
static int diagnostic_ai_positions,diagnostic_skull_ritual;
static struct {
    unsigned attempts,matches[3],materials[3],deck_candidates,deck_matches[3];
    unsigned oracle_checks,oracle_accepted,oracle_rejected,oracle_candidates_seen;
    unsigned oracle_signature_hits,oracle_last_trio[3],oracle_requirement_masks[3];
    int row_found,stage,retail_result,side,phase,last_oracle_result;
} skull_ritual_diag;
static unsigned char recipe_settings[24];
static struct {
    int valid,own_id,own_atk,own_def,opp_id,opp_atk,opp_def,phase,turn;
    unsigned count;
} ai_position_diag;
static unsigned ultimate_swords_diag_scans,ultimate_swords_diag_issued;
static int ultimate_swords_diag_last_result;

enum { EVENT_CARD,EVENT_CAPTURE,EVENT_SPRITE,EVENT_SHINE,EVENT_COIN,EVENT_DODGE,EVENT_GHOST,EVENT_REDIRECT,EVENT_TEXT };
typedef struct { int kind,arg[11]; char text[128]; } Event;
typedef struct { unsigned count; Event events[32]; } Trace;
static Trace trace;
#ifndef RENDER_BENCHMARK
static unsigned selected_calls,hashed_bytes,format_calls,strlen_calls;
#endif
static volatile unsigned benchmark_sink;
static unsigned cases;

static void emit(int kind,int a,int b,int c,int d,int e,int f,int g,int h,int i,int j,int k,const char *text)
{
#ifdef RENDER_BENCHMARK
    (void)kind;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i;(void)j;(void)k;(void)text;
    benchmark_sink^=(unsigned)(a+b);
#else
    Event *event;
    assert(trace.count<32);
    event=&trace.events[trace.count++];
    memset(event,0,sizeof(*event));
    event->kind=kind;
    event->arg[0]=a;event->arg[1]=b;event->arg[2]=c;event->arg[3]=d;
    event->arg[4]=e;event->arg[5]=f;event->arg[6]=g;event->arg[7]=h;
    event->arg[8]=i;event->arg[9]=j;event->arg[10]=k;
    if(text)snprintf(event->text,sizeof(event->text),"%s",text);
#endif
}
static void native_vfx_sprite(GsOT *ot,int pri,int x,int y,int w,int h,int u,int v,int tw,int th,int brightness)
{ emit(EVENT_SPRITE,(int)(uintptr_t)ot,pri,x,y,w,h,u,v,tw,th,brightness,0); }
static void native_vfx_shine(GsOT *ot,int pri,int x,int y,int brightness)
{ emit(EVENT_SHINE,(int)(uintptr_t)ot,pri,x,y,brightness,0,0,0,0,0,0,0); }
static void draw_card_with_hand_aura(DisplayObject *object,s32 ot,s32 x,s32 y)
{ emit(EVENT_CARD,object?object->field_6A:-1,ot,x,y,0,0,0,0,0,0,0,0); }
static void garma_capture(DisplayObject *object,int x,int y)
{ emit(EVENT_CAPTURE,object?object->field_6A:-1,x,y,0,0,0,0,0,0,0,0,0); }
static int chakra_skip_transitional_card_sprite(const DisplayObject *object)
{ (void)object;return skip_card_sprite; }
static int ritual_card_effect_enabled(int id) { return id>=0&&id<800&&effect_enabled[id]; }
static int ritual_coin_port_ui(void) { return coin_port; }
static int serpent_ghost_fade(void) { return ghost_fade; }
static void ritual_coin_overlay(void)
{ if(!coin_port)emit(EVENT_COIN,ritual_coin.style,ritual_coin.shown,ritual_coin.lift,0,0,0,0,0,0,0,0,0); }
static void performance_dodge_flash(void)
{ emit(EVENT_DODGE,coin_battle.dodge_ticks,coin_battle.dodge_object->field_30.h.field_30,coin_battle.dodge_object->field_30.h.field_32,0,0,0,0,0,0,0,0,0); }
static void serpent_ghost_draw(void)
{ if(serpent_art_loaded==1&&ghost_fade>0)emit(EVENT_GHOST,serpent_ghost_x,serpent_ghost_y,ghost_fade,0,0,0,0,0,0,0,0,0); }
static void serpent_redirect_draw(void)
{ emit(EVENT_REDIRECT,serpent_redirect_hold<6?serpent_redirect_hold:6,0,0,0,0,0,0,0,0,0,0,0); }
static void host_size(const Host *from,int *w,int *h,int *scale)
{ (void)from;*w=1280;*h=720;*scale=1; }
static void host_text(const Host *from,int x,int y,const char *value,unsigned rgb,int scale)
{ (void)from;emit(EVENT_TEXT,x,y,(int)rgb,scale,0,0,0,0,0,0,0,value); }
static const Host host_fixture={host_size,host_text};
static const Host *host=&host_fixture;

#include "render_helpers.inc"
static int ritual_counter_selected_record(void)
{
#ifndef RENDER_BENCHMARK
    ++selected_calls;
#endif
    return actual_selected_record();
}
static unsigned overlay_hash(unsigned hash,const void *data,unsigned bytes)
{
#ifndef RENDER_BENCHMARK
    hashed_bytes+=bytes;
#endif
    return actual_overlay_hash(hash,data,bytes);
}
#ifndef RENDER_BENCHMARK
static int tracked_snprintf(char *value,size_t size,const char *format,...)
{
    va_list args;int result;
    ++format_calls;va_start(args,format);result=vsnprintf(value,size,format,args);va_end(args);
    return result;
}
static size_t tracked_strlen(const char *value) { ++strlen_calls;return strlen(value); }
#define snprintf tracked_snprintf
#define strlen tracked_strlen
#endif
#include "render_callbacks.inc"
#ifndef RENDER_BENCHMARK
#undef snprintf
#undef strlen
#endif

static void defaults(void)
{
    int i;
    memset(D_801A7AD8,0,sizeof(D_801A7AD8));
    memset(objects,0,sizeof(objects));
    memset(garma_marks,0,sizeof(garma_marks));memset(tri_marks,0,sizeof(tri_marks));
    memset(javelin_marks,0,sizeof(javelin_marks));memset(hungry_marks,0,sizeof(hungry_marks));memset(chakra_marks,0,sizeof(chakra_marks));
    for(i=0;i<30;++i) {
        DuelCardRecord *card=&D_801A7AD8[i];
        objects[i].field_6A=i;objects[i].field_14=3;
        card->card_id=10;card->data=&objects[i];card->object=&objects[i];card->flags=DUEL_CARD_FLAG_OCCUPIED;
        garma_marks[i].ritual=tri_marks[i].ritual=javelin_marks[i].ritual=hungry_marks[i].ritual=chakra_marks[i].ritual=1;
        garma_marks[i].data=tri_marks[i].data=javelin_marks[i].data=hungry_marks[i].data=chakra_marks[i].data=card->data;
        tri_marks[i].horns=3;
    }
    for(i=0;i<800;++i)effect_enabled[i]=1;
    for(i=0;i<40;++i)D_800907D8[i]=i%30;
    cursor.row=0;cursor.col=0;D_8009B1B4=&cursor;D_8009B1D5=0;D_8009B174=4;
    gDuel_wSceneStateFlags=5;D_8009B229=gDuel_bEffectState=gDuel_wCardEffectFlags=0;
    crab_umi_visual_state=garma_live_hint=tri_live_hint=0;javelin_live_mask=0;
    visual_popup_kind=STAT_VISUAL_NORMAL;visual_popup_frames=0;visual_popup_record_index=-1;visual_popup_amount=1000;
    visual_popup_x=100;visual_popup_y=80;garma_relevant_frames=0;garma_relevant=-1;garma_animation_data=0;
    skip_card_sprite=coin_port=0;
    memset(&ritual_coin,0,sizeof(ritual_coin));memset(&coin_battle,0,sizeof(coin_battle));
    serpent_ghost_state=serpent_redirect_state=serpent_art_loaded=0;ghost_fade=0;
    diagnostic_ai_positions=diagnostic_skull_ritual=0;
    memset(&ai_position_diag,0,sizeof(ai_position_diag));memset(&skull_ritual_diag,0,sizeof(skull_ritual_diag));
}

static void equal_traces(const Trace *old,const Trace *now)
{
    assert(old->count==now->count);
    assert(!memcmp(old->events,now->events,old->count*sizeof(Event)));
    ++cases;
}
static void compare_card(DisplayObject *object,int x,int y)
{
    Trace before;
    memset(&trace,0,sizeof(trace));baseline_visual_card_frame_draw(object,3,x,y);before=trace;
    memset(&trace,0,sizeof(trace));optimized_visual_card_frame_draw(object,3,x,y);
    equal_traces(&before,&trace);
}

#ifndef RENDER_BENCHMARK
static void test_native_counters(void)
{
    static const int ids[]={10,GARMA_SWORD_ID,TRI_HORNED_ID,JAVELIN_BEETLE_ID,HUNGRY_BURGER_ID,CHAKRA_ID};
    static const int phases[]={4,5,9};
    int hints,phase,gates,i,id,selected;
    defaults();
    for(hints=0;hints<8;++hints)for(phase=0;phase<3;++phase)for(gates=0;gates<16;++gates)
        for(i=0;i<30;++i)for(id=0;id<6;++id)for(selected=0;selected<2;++selected) {
            garma_live_hint=hints&1;tri_live_hint=hints&2;javelin_live_mask=hints&4;
            gDuel_wSceneStateFlags=phases[phase];D_8009B229=gates&1;gDuel_bEffectState=gates&2;
            gDuel_wCardEffectFlags=(gates&4)?DUEL_CARD_EFFECT_FLAG_ACTIVE:0;crab_umi_visual_state=gates&8;
            D_801A7AD8[i].card_id=ids[id];D_800907D8[0]=selected?i:(i+1)%30;
            compare_card(&objects[i],17-i,45+i);
            D_801A7AD8[i].card_id=10;
        }
    /* Draw-side identity, resources, disabled settings and null/invalid objects. */
    defaults();garma_live_hint=tri_live_hint=1;javelin_live_mask=1;
    for(i=0;i<30;++i)for(id=0;id<6;++id)for(gates=0;gates<32;++gates) {
        D_801A7AD8[i].card_id=ids[id];D_800907D8[0]=i;
        effect_enabled[ids[id]]=(gates&1)==0;
        D_801A7AD8[i].flags=(gates&2)?0:DUEL_CARD_FLAG_OCCUPIED;
        D_801A7AD8[i].data=(gates&4)?&objects[31]:&objects[i];
        garma_marks[i].blades_used=(unsigned)(gates%7);tri_marks[i].horns=(unsigned)(gates%4);
        javelin_marks[i].protection_used_turn=gates&8;
        garma_marks[i].ritual=tri_marks[i].ritual=javelin_marks[i].ritual=(gates&16)==0;
        compare_card(&objects[i],-20,200);
    }
    compare_card(0,0,0);objects[30].field_6A=-1;compare_card(&objects[30],0,0);
    objects[30].field_6A=30;compare_card(&objects[30],0,0);
    skip_card_sprite=1;compare_card(&objects[0],0,0);
}

static void test_cursor_boundaries(void)
{
    int side,phase,ui,row,col,i;
    defaults();garma_live_hint=tri_live_hint=1;javelin_live_mask=1;
    for(i=0;i<30;++i)D_801A7AD8[i].card_id=(i%4==0)?GARMA_SWORD_ID:
        ((i%4==1)?TRI_HORNED_ID:((i%4==2)?JAVELIN_BEETLE_ID:10));
    for(side=0;side<2;++side)for(phase=4;phase<=5;++phase)for(ui=0;ui<4;++ui)
        for(row=-1;row<=4;++row)for(col=-1;col<=5;++col) {
            D_8009B1D5=side;gDuel_wSceneStateFlags=phase|((ui&1)?0x4000:0);
            D_8009B174=(ui&2)?4:0;cursor.row=row;cursor.col=col;
            for(i=0;i<30;++i)compare_card(&objects[i],-4,100);
        }
    D_8009B1B4=0;
    for(i=0;i<30;++i)compare_card(&objects[i],-4,100);
}

static void test_popup_timelines(void)
{
    static const unsigned amounts[]={0,1,9,10,99,100,999,1000,9999,65535,100000,UINT_MAX};
    unsigned amount;int frame,phase;
    for(amount=0;amount<sizeof(amounts)/sizeof(amounts[0]);++amount)for(phase=4;phase<=5;++phase) {
        defaults();D_801A7AD8[5].card_id=CHAKRA_ID;visual_popup_kind=STAT_VISUAL_CHAKRA;
        visual_popup_record_index=5;visual_popup_amount=(int)amounts[amount];gDuel_wSceneStateFlags=phase;
        for(frame=60;frame>=0;--frame) { visual_popup_frames=frame;compare_card(&objects[5],-7,60); }
    }
    defaults();D_801A7AD8[5].card_id=HUNGRY_BURGER_ID;visual_popup_kind=STAT_VISUAL_HUNGRY_STACK;visual_popup_record_index=5;
    for(frame=60;frame>=0;--frame) { visual_popup_frames=frame;compare_card(&objects[5],7,-20); }
    defaults();garma_live_hint=1;D_801A7AD8[5].card_id=GARMA_SWORD_ID;garma_relevant=5;garma_animation_data=D_801A7AD8[5].data;
    for(frame=36;frame>=0;--frame) { garma_relevant_frames=frame;compare_card(&objects[5],7,60); }
    /* Exhaust the game's unsigned 16-bit stat range directly, then exercise
     * large amounts and alternating cache keys in the complete timeline above. */
    for(amount=0;amount<=65535;++amount) {
        Trace before;
        memset(&trace,0,sizeof(trace));baseline_chakra_native_popup((GsOT *)3,2,120,60,amount,193);before=trace;
        memset(&trace,0,sizeof(trace));optimized_chakra_native_popup((GsOT *)3,2,120,60,amount,193);
        equal_traces(&before,&trace);
    }
}

static void test_overlay_signatures(void)
{
    int hints,state,variation;
    unsigned dormant=2166136261u;
    for(hints=0;hints<8;++hints)for(state=0;state<8;++state)for(variation=0;variation<4;++variation) {
        Trace before;unsigned old_signature,new_signature;
        defaults();garma_live_hint=hints&1;tri_live_hint=hints&2;javelin_live_mask=hints&4;
        switch(state) {
        case 1:ritual_coin.active=1;ritual_coin.style=variation%2;ritual_coin.shown=variation;ritual_coin.lift=variation*20;coin_port=variation==3;break;
        case 2:coin_battle.dodge_ticks=12+variation;coin_battle.dodge_object=&objects[5];objects[5].field_30.h.field_30=variation*7;break;
        case 3:serpent_ghost_state=2;serpent_art_loaded=1;ghost_fade=variation;serpent_ghost_x=variation*9;break;
        case 4:serpent_redirect_state=2;serpent_redirect_hold=variation+4;break;
        case 5:visual_popup_kind=STAT_VISUAL_NORMAL;visual_popup_frames=42-variation;visual_popup_amount=variation*500;break;
        case 6:diagnostic_skull_ritual=1;skull_ritual_diag.attempts=(unsigned)variation;break;
        case 7:diagnostic_ai_positions=1;ai_position_diag.valid=1;ai_position_diag.own_id=variation;break;
        }
        old_signature=baseline_overlay_signature();new_signature=optimized_overlay_signature();
        memset(&trace,0,sizeof(trace));baseline_overlay();before=trace;
        memset(&trace,0,sizeof(trace));optimized_overlay();equal_traces(&before,&trace);
        if(state)assert(old_signature==new_signature);
        else { assert(!trace.count);assert(new_signature==dormant); }
    }
    /* Pixel-changing transitions retain the old invalidation distinctions. */
    defaults();visual_popup_frames=42;visual_popup_kind=STAT_VISUAL_NORMAL;
    { unsigned old=baseline_overlay_signature(),now=optimized_overlay_signature();
      ++visual_popup_amount;assert(old!=baseline_overlay_signature());assert(now!=optimized_overlay_signature()); }
    defaults();ritual_coin.active=1;
    { unsigned old=baseline_overlay_signature(),now=optimized_overlay_signature();
      coin_port=1;assert(old!=baseline_overlay_signature());assert(now!=optimized_overlay_signature()); }
    defaults();serpent_redirect_state=2;serpent_redirect_hold=6;
    { unsigned old=baseline_overlay_signature(),now=optimized_overlay_signature();
      serpent_redirect_hold=20;assert(old==baseline_overlay_signature());assert(now==optimized_overlay_signature()); }
}

static void report_call_counts(void)
{
    unsigned old_selected,new_selected,old_hash,new_hash,old_formats,new_formats;int i;
    defaults();garma_live_hint=tri_live_hint=1;javelin_live_mask=1;
    selected_calls=hashed_bytes=0;baseline_overlay_signature();old_selected=selected_calls;old_hash=hashed_bytes;
    selected_calls=hashed_bytes=0;optimized_overlay_signature();new_selected=selected_calls;new_hash=hashed_bytes;
    assert(old_hash==32&&new_hash==0&&old_selected==1&&new_selected==0);
    printf("idle native overlay: hash bytes %u -> %u, cursor queries %u -> %u per callback\n",old_hash,new_hash,old_selected,new_selected);
    D_801A7AD8[5].card_id=GARMA_SWORD_ID;D_800907D8[0]=5;
    selected_calls=0;
    for(i=0;i<30;++i) { memset(&trace,0,sizeof(trace));baseline_visual_card_frame_draw(&objects[i],3,20,40); }
    old_selected=selected_calls;selected_calls=0;
    for(i=0;i<30;++i) { memset(&trace,0,sizeof(trace));optimized_visual_card_frame_draw(&objects[i],3,20,40); }
    new_selected=selected_calls;assert(old_selected==30&&new_selected==1);
    printf("30-card fixture (one eligible counter): cursor queries %u -> %u\n",old_selected,new_selected);
    format_calls=strlen_calls=0;
    for(i=0;i<60;++i) { memset(&trace,0,sizeof(trace));baseline_chakra_native_popup((GsOT *)3,2,120,60,123456789u,255); }
    old_formats=format_calls;assert(strlen_calls==60);format_calls=strlen_calls=0;
    for(i=0;i<60;++i) { memset(&trace,0,sizeof(trace));optimized_chakra_native_popup((GsOT *)3,2,120,60,123456789u,255); }
    new_formats=format_calls;assert(old_formats==60&&new_formats==1&&strlen_calls==1);
    printf("60-frame Chakra fixture: decimal formatting/length calls %u -> %u\n",old_formats,new_formats);
}
#endif

#ifdef RENDER_BENCHMARK
static unsigned (*volatile signature_callback)(void);
static void (*volatile card_callback)(DisplayObject *,s32,s32,s32);
static void (*volatile popup_callback)(GsOT *,int,int,int,unsigned,int);
static double bench_signature(unsigned (*callback)(void),unsigned iterations)
{
    clock_t start;unsigned i;signature_callback=callback;start=clock();
    for(i=0;i<iterations;++i)benchmark_sink^=signature_callback();
    return (double)(clock()-start)/CLOCKS_PER_SEC;
}
static double bench_card(void (*callback)(DisplayObject *,s32,s32,s32),unsigned iterations)
{
    clock_t start;unsigned i;card_callback=callback;start=clock();
    for(i=0;i<iterations;++i)card_callback(&objects[i%30],3,20,40);
    return (double)(clock()-start)/CLOCKS_PER_SEC;
}
static double bench_popup(void (*callback)(GsOT *,int,int,int,unsigned,int),unsigned iterations)
{
    clock_t start;unsigned i;popup_callback=callback;start=clock();
    for(i=0;i<iterations;++i)popup_callback((GsOT *)3,2,120,60,123456789u,255);
    return (double)(clock()-start)/CLOCKS_PER_SEC;
}
static void report_time(const char *label,double old,double now,unsigned iterations)
{ printf("local mocked CPU %s: median of 7 runs of %u calls, baseline %.6f s, optimized %.6f s (ratio %.3f)\n",label,iterations,old,now,now/old); }
static double median(double values[7])
{
    int i,j;
    for(i=1;i<7;++i)for(j=i;j>0&&values[j]<values[j-1];--j) {
        double swap=values[j];values[j]=values[j-1];values[j-1]=swap;
    }
    return values[3];
}
#endif

int main(void)
{
#ifdef RENDER_BENCHMARK
    unsigned iterations=3000000;double old[7],now[7];int run;
    defaults();garma_live_hint=tri_live_hint=1;javelin_live_mask=1;
    for(run=0;run<7;++run) {
        if(run%2) { now[run]=bench_signature(optimized_overlay_signature,iterations);old[run]=bench_signature(baseline_overlay_signature,iterations); }
        else { old[run]=bench_signature(baseline_overlay_signature,iterations);now[run]=bench_signature(optimized_overlay_signature,iterations); }
    }
    report_time("idle-native-signature",median(old),median(now),iterations);
    D_801A7AD8[5].card_id=GARMA_SWORD_ID;D_800907D8[0]=5;
    for(run=0;run<7;++run) {
        if(run%2) { now[run]=bench_card(optimized_visual_card_frame_draw,iterations);old[run]=bench_card(baseline_visual_card_frame_draw,iterations); }
        else { old[run]=bench_card(baseline_visual_card_frame_draw,iterations);now[run]=bench_card(optimized_visual_card_frame_draw,iterations); }
    }
    report_time("30-card-counter-fixture",median(old),median(now),iterations);
    for(run=0;run<7;++run) {
        if(run%2) { now[run]=bench_popup(optimized_chakra_native_popup,iterations);old[run]=bench_popup(baseline_chakra_native_popup,iterations); }
        else { old[run]=bench_popup(baseline_chakra_native_popup,iterations);now[run]=bench_popup(optimized_chakra_native_popup,iterations); }
    }
    report_time("fixed-Chakra-amount",median(old),median(now),iterations);
    puts("Timings exclude real GPU/game/Windows execution and do not measure FPS.");
#else
    test_native_counters();test_cursor_boundaries();test_popup_timelines();test_overlay_signatures();report_call_counts();
    printf("PASS: %u actual-source differential render command traces\n",cases);
#endif
    return 0;
}
