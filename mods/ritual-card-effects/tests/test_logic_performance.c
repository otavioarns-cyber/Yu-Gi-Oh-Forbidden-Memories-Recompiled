/* Engine doubles surround generated, verbatim baseline/optimized functions.
 * All changed filtering and reconciliation code is taken from actual mod.c.
 * Native host timings describe these workloads only, not game FPS. */
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generated_logic_types.h"

static DuelCardRecord D_801A7AD8[DUEL_CARD_RECORD_COUNT];
static s32 gDuel_adwCardStats[800];
static SengenjinMark sengenjin_marks[DUEL_CARD_RECORD_COUNT];
static SengenjinNegativeMark sengenjin_negative[DUEL_CARD_RECORD_COUNT];
static BlsMark bls_marks[DUEL_CARD_RECORD_COUNT];
static DokuroriderMark dokurorider_marks[DUEL_CARD_RECORD_COUNT];
static UltimateMark ultimate_marks[DUEL_CARD_RECORD_COUNT];
static SeaRitualMark crab_marks[DUEL_CARD_RECORD_COUNT];
static unsigned bls_live_mask,bls_live_hint,bls_new_source_pending;
static unsigned dokurorider_live_mask,dokurorider_live_hint,dokurorider_new_source_pending;
static unsigned stat_effect_hints,sengenjin_live_hint,gDuel_wSceneStateFlags;
static u8 bls_prev_sources[DUEL_CARD_RECORD_COUNT],dokurorider_prev_sources[DUEL_CARD_RECORD_COUNT];
static void *bls_prev_data[DUEL_CARD_RECORD_COUNT],*dokurorider_prev_data[DUEL_CARD_RECORD_COUNT];
static u16 bls_prev_card_id[DUEL_CARD_RECORD_COUNT],dokurorider_prev_card_id[DUEL_CARD_RECORD_COUNT];
static RitualActivationPos visual_activation_queue[VISUAL_QUEUE_SIZE];
static int visual_activation_head,visual_activation_count;
static RitualStatVisual visual_stat_queue[VISUAL_QUEUE_SIZE];
static unsigned visual_stat_active_mask;
static int visual_stat_count;
static u8 D_800907D8[DUEL_SIDE_COUNT*DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
static struct { s16 x,y; } D_80090800[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
static struct Participant { int field_6A; } participants[2];
static struct Participant *D_800E9EF0[2];
static int sengenjin_preview_filter_guard;
static unsigned staged_mask;
static int effect_enabled=1,ultimate_enabled;
static int cross_axis,external_atk,external_def,guardian_bonus;
static uint32_t rng_state=0x71870339;
static volatile uint64_t public_calls,native_calls,checksum;
static uint64_t checks;
static s32 (*selected_preview)(DuelCardRecord *);
static int trace_enabled;
static char call_trace[64];
static unsigned call_trace_length;

static void trace_call(char kind)
{
    if(trace_enabled) {
        assert(call_trace_length<sizeof(call_trace));
        call_trace[call_trace_length++]=kind;
    }
}

static unsigned random_value(void)
{ rng_state=rng_state*1664525u+1013904223u; return rng_state; }
static int random_range(int upper) { return (int)(random_value()%(unsigned)upper); }
static int battle_participant(int i) { return i>=0 && i<30 && !!(staged_mask&(1u<<i)); }
static int javelin_live(int i) { (void)i; return 0; }
static int ritual_card_effect_enabled(int id)
{ return id==BLUE_EYES_ULTIMATE_ID ? ultimate_enabled : effect_enabled; }
static unsigned clamp_stat(int value)
{ return (unsigned)(value<0?0:value>CARD_STAT_MAX?CARD_STAT_MAX:value); }
static __attribute__((noinline)) s32 model_native_stats(DuelCardRecord *card)
{
    int atk,def;
    trace_call('N');
    ++native_calls;
    /* A cross-axis hook catches accidental simultaneous ATK/DEF clamping. */
    atk=card->attack+card->stat_modifier+card->terrain_modifier;
    def=card->defense+card->stat_modifier+card->terrain_modifier+card->defense_modifier;
    if(cross_axis) { atk-=card->defense/7; def-=card->attack/11; }
    return (s32)(clamp_stat(atk)|(clamp_stat(def)<<16));
}
static s32 (*original_stats)(DuelCardRecord *)=model_native_stats;
static uint32_t model_full_stats(DuelCardRecord *card)
{
    unsigned packed=(unsigned)original_stats(card);
    return clamp_stat((int)(packed&65535u)+external_atk)|
        (clamp_stat((int)(packed>>16)+external_def)<<16);
}
static __attribute__((noinline)) s32 Duel_CalcCardStats(DuelCardRecord *card)
{ trace_call('P');++public_calls; return selected_preview(card); }
static s32 Duel_CalcGuardianStarBonus(DuelCardRecord *card,DuelCardRecord *opponent)
{ (void)card; (void)opponent; return guardian_bonus; }
static s32 new_sengenjin_filtered_battle_stat_with_full(DuelCardRecord *,DuelCardRecord *,int,int,SengenjinStatSnapshot *);
#include "generated_logic_functions.h"

static void reset_world(void)
{
    int side,i;
    memset(D_801A7AD8,0,sizeof(D_801A7AD8));
    memset(bls_marks,0,sizeof(bls_marks));
    memset(dokurorider_marks,0,sizeof(dokurorider_marks));
    memset(sengenjin_marks,0,sizeof(sengenjin_marks));
    memset(sengenjin_negative,0,sizeof(sengenjin_negative));
    memset(ultimate_marks,0,sizeof(ultimate_marks));
    memset(visual_activation_queue,0,sizeof(visual_activation_queue));
    memset(visual_stat_queue,0,sizeof(visual_stat_queue));
    memset(bls_prev_sources,0,sizeof(bls_prev_sources));
    memset(bls_prev_data,0,sizeof(bls_prev_data));
    memset(bls_prev_card_id,0,sizeof(bls_prev_card_id));
    memset(dokurorider_prev_sources,0,sizeof(dokurorider_prev_sources));
    memset(dokurorider_prev_data,0,sizeof(dokurorider_prev_data));
    memset(dokurorider_prev_card_id,0,sizeof(dokurorider_prev_card_id));
    visual_stat_count=visual_activation_count=visual_activation_head=0;
    visual_stat_active_mask=bls_live_mask=dokurorider_live_mask=staged_mask=0;
    bls_new_source_pending=dokurorider_new_source_pending=0;
    bls_live_hint=dokurorider_live_hint=sengenjin_live_hint=0;
    stat_effect_hints=0;gDuel_wSceneStateFlags=5;
    effect_enabled=1;ultimate_enabled=0;cross_axis=external_atk=external_def=guardian_bonus=0;
    sengenjin_preview_filter_guard=0;
    D_800E9EF0[0]=&participants[0];D_800E9EF0[1]=&participants[1];
    participants[0].field_6A=5;participants[1].field_6A=20;
    for(side=0;side<2;++side)for(i=0;i<20;++i) {
        D_800907D8[side*20+i]=(u8)(side*15+i%15);
        D_80090800[side][i].x=(s16)(i*13);D_80090800[side][i].y=(s16)(side*100+i*7);
    }
}
static void occupy(int i,int id,int type)
{
    DuelCardRecord *card=&D_801A7AD8[i];
    card->card_id=(s16)id;card->attack=3000;card->defense=2600;
    card->flags=DUEL_CARD_FLAG_OCCUPIED;
    card->data=(void *)(uintptr_t)(0x10000+i*16);
    card->object=(void *)(uintptr_t)(0x20000+i*16);
    gDuel_adwCardStats[id-1]=(s32)(240u|(200u<<CARD_STAT_DEFENSE_SHIFT)|
                                     ((unsigned)type<<CARD_STAT_TYPE_SHIFT));
}
static void require(int condition,const char *label)
{ ++checks;if(!condition){fprintf(stderr,"FAILED: %s, case %llu\n",label,(unsigned long long)checks);exit(1);} }

static void test_sengenjin(void)
{
    int iteration,axis,with_guardian;
    for(iteration=0;iteration<25000;++iteration) {
        DuelCardRecord *card;
        SengenjinNegativeMark negative_before[DUEL_CARD_RECORD_COUNT];
        s32 before,after;
        uint64_t old_calls,new_calls;
        char old_trace[64];unsigned old_trace_length;
        reset_world();
        occupy(5,SENGENJIN_ID,CARD_TYPE_BEAST);occupy(20,100,CARD_TYPE_WARRIOR);
        card=&D_801A7AD8[20];
        card->attack=(s16)random_range(10000);card->defense=(s16)random_range(10000);
        card->stat_modifier=(s16)(random_range(6001)-3000);
        card->terrain_modifier=(s16)(random_range(3001)-1500);
        card->defense_modifier=(s16)(random_range(1201)-600);
        gDuel_adwCardStats[99]=(s32)(random_range(512)|(random_range(512)<<CARD_STAT_DEFENSE_SHIFT));
        sengenjin_marks[5].data=D_801A7AD8[5].data;sengenjin_marks[5].ritual=1;
        sengenjin_live_hint=1;gDuel_wSceneStateFlags=9;
        cross_axis=iteration%2;external_atk=random_range(6001)-3000;external_def=random_range(6001)-3000;
        guardian_bonus=(random_range(3)-1)*500;
        sengenjin_negative[20].data=iteration%7?card->data:(void *)(uintptr_t)77;
        sengenjin_negative[20].card_id=(u16)(iteration%11?card->card_id:111);
        sengenjin_negative[20].negative=(s16)(-random_range(2501));
        /* Each gate is independently exercised: normal field, stale source,
         * Ultimate, same side, missing participant, OFF and nested guard. */
        switch(iteration%13) {
        case 0:gDuel_wSceneStateFlags=5;break;
        case 1:sengenjin_live_hint=0;break;
        case 2:sengenjin_marks[5].data=(void *)(uintptr_t)77;break;
        case 3:card->card_id=BLUE_EYES_ULTIMATE_ID;ultimate_enabled=1;
               ultimate_marks[20].data=card->data;ultimate_marks[20].ritual=1;
               gDuel_adwCardStats[card->card_id-1]=gDuel_adwCardStats[99];break;
        case 4:participants[1].field_6A=6;break;
        case 5:D_800E9EF0[1]=0;break;
        case 6:effect_enabled=0;break;
        case 7:sengenjin_preview_filter_guard=1;break;
        case 8:card->flags=0;staged_mask=1u<<20;break;
        default:break;
        }
        memcpy(negative_before,sengenjin_negative,sizeof(negative_before));
        trace_enabled=1;call_trace_length=0;
        selected_preview=old_preview;public_calls=0;
        before=Duel_CalcCardStats(card);old_calls=public_calls;
        old_trace_length=call_trace_length;memcpy(old_trace,call_trace,old_trace_length);
        memcpy(sengenjin_negative,negative_before,sizeof(negative_before));
        call_trace_length=0;
        selected_preview=new_preview;public_calls=0;
        after=Duel_CalcCardStats(card);new_calls=public_calls;
        require(before==after,"Sengenjin packed preview and recursion guard");
        require(old_calls==3?new_calls==2:new_calls==old_calls,"Sengenjin full chain call count");
        require(call_trace_length<=old_trace_length && !memcmp(old_trace,call_trace,call_trace_length),
                "Sengenjin retained native/public callback order");
        trace_enabled=0;
        for(axis=0;axis<2;++axis)for(with_guardian=0;with_guardian<2;++with_guardian) {
            memcpy(sengenjin_negative,negative_before,sizeof(negative_before));
            selected_preview=old_preview;
            before=old_sengenjin_filtered_battle_stat(card,&D_801A7AD8[5],axis,with_guardian);
            memcpy(sengenjin_negative,negative_before,sizeof(negative_before));
            selected_preview=new_preview;
            after=new_sengenjin_filtered_battle_stat(card,&D_801A7AD8[5],axis,with_guardian);
            require(before==after,"Sengenjin battle/guardian filtering");
        }
    }
}

typedef struct {
    u8 bls_sources[30],doku_sources[30];void *bls_data[30],*doku_data[30];
    u16 bls_ids[30],doku_ids[30];
    RitualActivationPos activation[VISUAL_QUEUE_SIZE];
    RitualStatVisual stats[VISUAL_QUEUE_SIZE];
    unsigned mask,bls_pending,doku_pending;
    int stat_count,activation_count,activation_head;
} AuraState;
static void capture(AuraState *state)
{
    memset(state,0,sizeof(*state));
    memcpy(state->bls_sources,bls_prev_sources,sizeof(bls_prev_sources));
    memcpy(state->doku_sources,dokurorider_prev_sources,sizeof(dokurorider_prev_sources));
    memcpy(state->bls_data,bls_prev_data,sizeof(bls_prev_data));
    memcpy(state->doku_data,dokurorider_prev_data,sizeof(dokurorider_prev_data));
    memcpy(state->bls_ids,bls_prev_card_id,sizeof(bls_prev_card_id));
    memcpy(state->doku_ids,dokurorider_prev_card_id,sizeof(dokurorider_prev_card_id));
    memcpy(state->activation,visual_activation_queue,sizeof(visual_activation_queue));
    memcpy(state->stats,visual_stat_queue,sizeof(visual_stat_queue));
    state->mask=visual_stat_active_mask;state->stat_count=visual_stat_count;
    state->activation_count=visual_activation_count;state->activation_head=visual_activation_head;
    state->bls_pending=bls_new_source_pending;state->doku_pending=dokurorider_new_source_pending;
}
static void restore(const AuraState *state)
{
    memcpy(bls_prev_sources,state->bls_sources,sizeof(bls_prev_sources));
    memcpy(dokurorider_prev_sources,state->doku_sources,sizeof(dokurorider_prev_sources));
    memcpy(bls_prev_data,state->bls_data,sizeof(bls_prev_data));
    memcpy(dokurorider_prev_data,state->doku_data,sizeof(dokurorider_prev_data));
    memcpy(bls_prev_card_id,state->bls_ids,sizeof(bls_prev_card_id));
    memcpy(dokurorider_prev_card_id,state->doku_ids,sizeof(dokurorider_prev_card_id));
    memcpy(visual_activation_queue,state->activation,sizeof(visual_activation_queue));
    memcpy(visual_stat_queue,state->stats,sizeof(visual_stat_queue));
    visual_stat_active_mask=state->mask;visual_stat_count=state->stat_count;
    visual_activation_count=state->activation_count;visual_activation_head=state->activation_head;
    bls_new_source_pending=state->bls_pending;dokurorider_new_source_pending=state->doku_pending;
}
static void test_auras(void)
{
    int kind,iteration,i,tick;
    for(kind=0;kind<2;++kind)for(iteration=0;iteration<35000;++iteration) {
        AuraState initial,expected,result;
        DuelCardRecord before_records[DUEL_CARD_RECORD_COUNT];
        void (*old_reconcile)(void)=kind?old_dokurorider_reconcile_aura_visuals:old_bls_reconcile_aura_visuals;
        void (*new_reconcile)(void)=kind?new_dokurorider_reconcile_aura_visuals:new_bls_reconcile_aura_visuals;
        reset_world();
        bls_live_hint=dokurorider_live_hint=1;
        stat_effect_hints=STAT_HINT_BLS|STAT_HINT_DOKU;
        gDuel_wSceneStateFlags=iteration%4?5:9;
        for(i=0;i<30;++i) {
            int variant=random_range(10),is_source=variant<6&&monster_slot(i);
            int id=is_source?(kind?DOKURORIDER_ID:BLACK_LUSTER_SOLDIER_ID):100+i;
            occupy(i,id,random_range(4)?(kind?CARD_TYPE_ZOMBIE:CARD_TYPE_WARRIOR):CARD_TYPE_BEAST);
            if(variant==2 || variant==7)D_801A7AD8[i].flags=0;
            if(variant==3 || variant==8)D_801A7AD8[i].object=0;
            if(variant==9)D_801A7AD8[i].data=0;
            if(is_source) {
                bls_live_mask|=1u<<i;dokurorider_live_mask|=1u<<i;
                bls_marks[i].ritual=dokurorider_marks[i].ritual=1;
                bls_marks[i].data=dokurorider_marks[i].data=D_801A7AD8[i].data;
                if(variant==4)bls_marks[i].data=dokurorider_marks[i].data=(void *)(uintptr_t)77;
                if(variant==5)D_801A7AD8[i].card_id=444;
                if(random_range(2))bls_new_source_pending|=1u<<i;
                if(random_range(2))dokurorider_new_source_pending|=1u<<i;
            }
            if(random_range(2))staged_mask|=1u<<i;
            bls_prev_sources[i]=dokurorider_prev_sources[i]=(u8)random_range(6);
            bls_prev_data[i]=dokurorider_prev_data[i]=random_range(2)?D_801A7AD8[i].data:(void *)(uintptr_t)77;
            bls_prev_card_id[i]=dokurorider_prev_card_id[i]=(u16)(random_range(2)?id:321);
        }
        if(iteration%17==0)bls_live_mask=dokurorider_live_mask=0;
        if(iteration%19==0)bls_live_hint=dokurorider_live_hint=0;
        if(iteration%23==0)stat_effect_hints=0;
        visual_stat_count=random_range(33);
        visual_stat_active_mask=visual_stat_count==32?~0u:((1u<<visual_stat_count)-1u);
        visual_activation_count=random_range(33);visual_activation_head=random_range(32);
        memcpy(before_records,D_801A7AD8,sizeof(before_records));
        for(tick=0;tick<3;++tick) {
            capture(&initial);old_reconcile();capture(&expected);
            restore(&initial);new_reconcile();capture(&result);
            require(!memcmp(&expected,&result,sizeof(result)),"Aura queue, identity caches and pending sources");
            require(!memcmp(before_records,D_801A7AD8,sizeof(before_records)),"Aura never changes duel records");
        }
    }
}

static double cpu_seconds(void)
{
    struct timespec time;
    assert(clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time)==0);
    return time.tv_sec+time.tv_nsec/1e9;
}
static double benchmark_preview(s32 (*preview)(DuelCardRecord *),unsigned iterations)
{
    unsigned i;double begin;
    selected_preview=preview;begin=cpu_seconds();
    for(i=0;i<iterations;++i)checksum+=(unsigned)Duel_CalcCardStats(&D_801A7AD8[20]);
    return cpu_seconds()-begin;
}
static double benchmark_aura(void (*reconcile)(void),unsigned iterations)
{
    unsigned i;double begin=cpu_seconds();
    for(i=0;i<iterations;++i){reconcile();checksum+=bls_prev_sources[6]+dokurorider_prev_sources[6];}
    return cpu_seconds()-begin;
}
static double median(double values[7])
{
    int i,j;
    for(i=1;i<7;++i)for(j=i;j>0 && values[j]<values[j-1];--j) {
        double temporary=values[j];values[j]=values[j-1];values[j-1]=temporary;
    }
    return values[3];
}
static void benchmarks(void)
{
    const unsigned iterations=500000;double old_time,new_time,old_samples[7],new_samples[7];
    int i,sample;
    reset_world();occupy(5,SENGENJIN_ID,CARD_TYPE_BEAST);occupy(20,100,CARD_TYPE_WARRIOR);
    sengenjin_marks[5].ritual=1;sengenjin_marks[5].data=D_801A7AD8[5].data;
    sengenjin_live_hint=1;gDuel_wSceneStateFlags=9;external_atk=-500;external_def=-750;cross_axis=1;
    public_calls=native_calls=0;
    for(sample=0;sample<7;++sample) {
        if(sample%2) {
            new_samples[sample]=benchmark_preview(new_preview,iterations);
            old_samples[sample]=benchmark_preview(old_preview,iterations);
        } else {
            old_samples[sample]=benchmark_preview(old_preview,iterations);
            new_samples[sample]=benchmark_preview(new_preview,iterations);
        }
    }
    old_time=median(old_samples);new_time=median(new_samples);
    printf("Sengenjin median of 7: baseline %.2f ns/preview, optimized %.2f ns/preview; CPU change %.1f%%\n",old_time*1e9/iterations,new_time*1e9/iterations,(1-new_time/old_time)*100);
    printf("Sengenjin calls per matching preview: public chain 3 -> 2; native stats 5 -> 4.\n");
    for(i=0;i<2;++i) {
        int slot;
        reset_world();bls_live_hint=dokurorider_live_hint=1;stat_effect_hints=STAT_HINT_BLS|STAT_HINT_DOKU;
        for(slot=0;slot<30;++slot)if(monster_slot(slot)) {
            occupy(slot,i?DOKURORIDER_ID:BLACK_LUSTER_SOLDIER_ID,i?CARD_TYPE_ZOMBIE:CARD_TYPE_WARRIOR);
            bls_marks[slot].ritual=dokurorider_marks[slot].ritual=1;
            bls_marks[slot].data=dokurorider_marks[slot].data=D_801A7AD8[slot].data;
            bls_live_mask|=1u<<slot;dokurorider_live_mask|=1u<<slot;
        }
        if(i)old_dokurorider_reconcile_aura_visuals();else old_bls_reconcile_aura_visuals();
        for(sample=0;sample<7;++sample) {
            if(sample%2) {
                new_samples[sample]=benchmark_aura(i?new_dokurorider_reconcile_aura_visuals:new_bls_reconcile_aura_visuals,iterations);
                old_samples[sample]=benchmark_aura(i?old_dokurorider_reconcile_aura_visuals:old_bls_reconcile_aura_visuals,iterations);
            } else {
                old_samples[sample]=benchmark_aura(i?old_dokurorider_reconcile_aura_visuals:old_bls_reconcile_aura_visuals,iterations);
                new_samples[sample]=benchmark_aura(i?new_dokurorider_reconcile_aura_visuals:new_bls_reconcile_aura_visuals,iterations);
            }
        }
        old_time=median(old_samples);new_time=median(new_samples);
        printf("%s dense field median of 7: baseline %.2f ns/call, optimized %.2f ns/call; CPU change %.1f%%\n",i?"Dokurorider":"BLS",old_time*1e9/iterations,new_time*1e9/iterations,(1-new_time/old_time)*100);
    }
    printf("Microbenchmarks use a synthetic native host workload; checksum=%llu. No FPS claim.\n",(unsigned long long)checksum);
}
int main(void)
{
    test_sengenjin();test_auras();
    printf("PASS: %llu differential checks from actual baseline/optimized functions.\n",(unsigned long long)checks);
    benchmarks();return 0;
}
