#!/usr/bin/env python3
"""Native mock regression using the *actual* Hungry Burger source statements.

Exercises both duel sides and phase-9 battle staging. It is not a Windows
runtime test and makes no claims about actual 2D/3D visual presentation.
"""
from pathlib import Path
import subprocess
import tempfile

SRC = Path(__file__).resolve().parent.parent / 'mod.c'
src = SRC.read_text()

def between(left: str, right: str):
    return src.split(left, 1)[1].split(right, 1)[0]

# Pull the exact changed C snippets, not hand-written restatements.
deferred = '    while (deferred_hungry)' + between('    while (deferred_hungry)', '    while (deferred_javelin)')
refresh = '    /* The Ritual instance is authoritative;' + between('    /* The Ritual instance is authoritative;', '    if(old_stats&STAT_HINT_JAVELIN)')
reward = '    /* Hungry Burger gains a stack only' + between('    /* Hungry Burger gains a stack only', '    /* Only participant zero initiated this battle.')
stat_block = '    if ((stat_effect_hints & STAT_HINT_HUNGRY)' + between('    if ((stat_effect_hints & STAT_HINT_HUNGRY)', '    if ((garma_live_hint')
# CPU commits independently of rendering; the player's legacy increment
# remains at the start of its own green popup (v0.78.43 behavior).
visual = between('static void visual_update_stats(void)', 'static int visual_find_live_mirror(')
assert 'if(!ritual_ai_side_is_cpu(side_for_record(e->record_index)))\n                    ++m->battle_kills;' in visual
assert 'if(ritual_ai_side_is_cpu(side_for_record(idx)))\n                    ++hungry_marks[idx].battle_kills;' in reward
assert 'if(ritual_ai_side_is_cpu(side_for_record(i)) || visual_queue_hungry_stack(i))' in src
assert 'hungry_live_mask' in between('static int field_actions_hook_needed(void)', 'static void field_actions(void)')
assert 'hungry_live_mask' in between('static int battle_hook_needed(void)', 'static void battle(void)')

fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define HUNGRY_BURGER_ID 702
#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_SCENE_PHASE_MASK 15
#define DUEL_CARD_FLAG_OCCUPIED 0x8000u
#define STAT_HINT_HUNGRY 2u
#define HUNGRY_BONUS 500u
#define CARD_STAT_MAX 9999u

typedef struct { int card_id; unsigned flags; void *data; } DuelCardRecord;
typedef struct { int field_6A; } DisplayObject;
typedef struct { void* data; int ritual; unsigned battle_kills, pending_stacks; } HungryMark;
typedef struct { int valid; } HungryScreen;
static DuelCardRecord D_801A7AD8[DUEL_CARD_RECORD_COUNT];
static DisplayObject *D_800E9EF0[4];
static HungryMark hungry_marks[DUEL_CARD_RECORD_COUNT];
static HungryScreen hungry_screen[DUEL_CARD_RECORD_COUNT];
static void *hungry_visual_pending[DUEL_CARD_RECORD_COUNT];
static unsigned hungry_live_mask, hungry_visual_pending_mask, deferred_hungry;
static unsigned gDuel_wSceneStateFlags;
static int D_8009B22A, effect_enabled=1;
static signed char D_8009B1B0[2];
/* -1 is human; >=0 is the real CPU flag in the original game. */
static signed char D_8009B360[2] = {-1,16};
static unsigned last_hint;
struct Host { void (*log)(void *, const char *, ...); };
static struct Host *host;
static int take_mask_bit(unsigned *mask) {
    int i; for (i=0;i<30;++i) if (*mask & (1u<<i)) { *mask &= ~(1u<<i); return i; }
    assert(0 && "mask bit not found"); return -1;
}
static int monster_slot(int i) { return (i>=5 && i<10) || (i>=20 && i<25); }
static int side_for_record(int i) { return i>=15; }
static int ritual_ai_side_is_cpu(int side) { return side>=0 && side<2 && D_8009B360[side]>=0; }
static int battle_participant(int i) {
    return (D_800E9EF0[0] && D_800E9EF0[0]->field_6A==i) ||
           (D_800E9EF0[1] && D_800E9EF0[1]->field_6A==i);
}
static int coin_battle_protected(int side) { (void)side; return 0; }
static int ritual_card_effect_enabled(int id) { (void)id; return effect_enabled; }
static void check_deferred(void) {
    int i;
REPLACE_DEFERRED
}
static void check_reconcile(void) {
    unsigned pending, stats=0;
    int i;
REPLACE_REFRESH
    last_hint=stats;
}
static void check_reward(void) {
    unsigned tri_restore=0, javelin_restore=0, crab_restore=0;
    int state11=1, i;
REPLACE_REWARD
}
static unsigned check_stats(int i) {
    DuelCardRecord *card = &D_801A7AD8[i];
    unsigned packed = (1850u << 16) | 2000u;
    unsigned stat_effect_hints = STAT_HINT_HUNGRY;
REPLACE_STATS
    return packed;
}
static DisplayObject objects[2], survivors[2];
static char identities[2];
static void reset_state(int hungry_slot, int other_slot) {
    memset(D_801A7AD8,0,sizeof(D_801A7AD8));
    memset(hungry_marks,0,sizeof(hungry_marks));
    memset(hungry_visual_pending,0,sizeof(hungry_visual_pending));
    memset(hungry_screen,0,sizeof(hungry_screen));
    memset(D_800E9EF0,0,sizeof(D_800E9EF0));
    hungry_live_mask=(1u<<hungry_slot);
    hungry_visual_pending_mask=deferred_hungry=0;
    effect_enabled=1; D_8009B22A=0;
    D_8009B360[0]=-1; D_8009B360[1]=16;
    D_8009B1B0[0]=0; D_8009B1B0[1]=0;
    D_801A7AD8[hungry_slot].card_id=HUNGRY_BURGER_ID;
    D_801A7AD8[hungry_slot].data=&identities[0];
    hungry_marks[hungry_slot].data=&identities[0];
    hungry_marks[hungry_slot].ritual=1;
    D_801A7AD8[other_slot].card_id=400;
    D_801A7AD8[other_slot].data=&identities[1];
    D_801A7AD8[other_slot].flags=DUEL_CARD_FLAG_OCCUPIED;
    objects[0].field_6A=hungry_slot; objects[1].field_6A=other_slot;
    survivors[0].field_6A=hungry_slot;
    survivors[1].field_6A=other_slot;
    D_800E9EF0[0]=&objects[0]; D_800E9EF0[1]=&objects[1];
    D_800E9EF0[2]=&survivors[0]; D_800E9EF0[3]=NULL;
    gDuel_wSceneStateFlags=9;
}
int main(void) {
    /* CPU's Monster Zone 20 and player's zone 5, both reach reward code. */
    reset_state(20,5);
    /* Native field action cleared OCCUPIED before switching to battle. */
    deferred_hungry=(1u<<20); D_801A7AD8[20].flags=0;
    check_deferred();
    assert(hungry_marks[20].ritual && (hungry_live_mask&(1u<<20)));
    check_reward();
    assert(hungry_marks[20].pending_stacks==1 && (hungry_visual_pending_mask&(1u<<20)));
    assert(hungry_visual_pending[20]==&identities[0]);
    assert(hungry_marks[20].battle_kills==1);
    assert((check_stats(20)&65535u)==2500u && (check_stats(20)>>16)==2350u);
    /* Field reconciliation restores the optimization hint from identity. */
    check_reconcile(); assert(last_hint & STAT_HINT_HUNGRY);

    /* Player's Ritual Burger defending and destroying CPU attacker. */
    reset_state(5,20);
    D_800E9EF0[2]=NULL; D_800E9EF0[3]=&survivors[1];
    objects[0].field_6A=20; objects[1].field_6A=5;
    D_8009B1B0[0]=-1; D_8009B1B0[1]=0;
    check_reward(); assert(hungry_marks[5].battle_kills==0 && hungry_marks[5].pending_stacks==1);
    assert((check_stats(5)&65535u)==2000u && (check_stats(5)>>16)==1850u);
    /* The old player's visual-update stage awards when +500 animation starts. */
    ++hungry_marks[5].battle_kills;
    assert((check_stats(5)&65535u)==2500u && (check_stats(5)>>16)==2350u);
    /* A human on field side 1 is still a human: no CPU bonus. */
    reset_state(20,5); D_8009B360[1]=-1;
    check_reward(); assert(hungry_marks[20].battle_kills==0 && hungry_marks[20].pending_stacks==1);
    /* If the CPU is assigned field side 0, the new rule follows CPU control. */
    reset_state(5,20); D_8009B360[0]=8; D_8009B360[1]=-1;
    check_reward(); assert(hungry_marks[5].battle_kills==1 && hungry_marks[5].pending_stacks==1);

    /* Direct attacks, trap cancellations, losses and non-ritual do not buff. */
    reset_state(20,5); D_800E9EF0[1]=NULL; check_reward(); assert(!hungry_marks[20].pending_stacks && !hungry_marks[20].battle_kills);
    reset_state(20,5); D_8009B22A=701; check_reward(); assert(!hungry_marks[20].pending_stacks && !hungry_marks[20].battle_kills);
    reset_state(20,5); D_800E9EF0[2]=NULL; D_800E9EF0[3]=&survivors[1]; D_8009B1B0[0]=-1; D_8009B1B0[1]=0;
    check_reward(); assert(!hungry_marks[20].pending_stacks && !hungry_marks[20].battle_kills);
    /* Native battle outcome is authoritative when the presentation objects
       have not yet been released. */
    reset_state(20,5); D_800E9EF0[3]=&survivors[1]; D_8009B1B0[0]=0; D_8009B1B0[1]=-1;
    check_reward(); assert(hungry_marks[20].pending_stacks==1 && hungry_marks[20].battle_kills==1);
    reset_state(20,5); D_800E9EF0[2]=NULL; D_800E9EF0[3]=&survivors[1]; D_8009B1B0[0]=-1; D_8009B1B0[1]=0;
    check_reward(); assert(!hungry_marks[20].pending_stacks && !hungry_marks[20].battle_kills);
    reset_state(20,5); hungry_marks[20].ritual=0; check_reward(); assert(!hungry_marks[20].pending_stacks && !hungry_marks[20].battle_kills);
    reset_state(20,5); effect_enabled=0; check_reward(); assert(!hungry_marks[20].pending_stacks && !hungry_marks[20].battle_kills);
    reset_state(20,5); D_801A7AD8[20].data=NULL; check_reward(); assert(!hungry_marks[20].pending_stacks && !hungry_marks[20].battle_kills);

    /* A real nonbattle deactivation still discards the mark. */
    reset_state(20,5); gDuel_wSceneStateFlags=5; deferred_hungry=(1u<<20);
    check_deferred(); assert(!hungry_marks[20].ritual && !hungry_live_mask);
    puts("Hungry Burger player/CPU battle lifecycle: OK");
    return 0;
}
'''
fixture = fixture.replace('REPLACE_DEFERRED', deferred).replace('REPLACE_REFRESH', refresh).replace('REPLACE_REWARD', reward).replace('REPLACE_STATS', stat_block)
with tempfile.TemporaryDirectory(prefix='hungry-cpu-') as directory:
    code = Path(directory) / 'test.c'
    exe = Path(directory) / 'test'
    code.write_text(fixture)
    subprocess.run(['gcc','-std=c99','-O1','-Wall','-Wextra','-Wno-unused-function','-o',str(exe),str(code)],check=True)
    subprocess.run([str(exe)],check=True)
