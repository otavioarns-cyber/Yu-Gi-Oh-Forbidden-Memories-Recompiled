"""Exercise source-extracted temporary-battle lifecycle for four Rituals."""
from pathlib import Path
import subprocess
import tempfile

src = Path(__file__).resolve().parents[1].joinpath('mod.c').read_text()
start = src.index('    if (inside_field_actions && i >= 0 && marks[i].ritual)')
stop = src.index('    if (i >= 0 && gate_marks[i].ritual && !inside_field_actions', start)
block = src[start:stop]
assert 'battle_participant(i)' in block
assert 'deferred_hungry' in block and 'deferred_javelin' in block
fixture = r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define DUEL_SCENE_PHASE_MASK 15
#define DUEL_CARD_RECORD_COUNT 30
#define HUNGRY_BURGER_ID 702
#define JAVELIN_BEETLE_ID 717
#define SHIELD_ID 362
#define YAMADRON_ID 357
struct Mark { int ritual; void *data; unsigned battle_kills; };
struct ShieldMark { int ritual; void *data; };
struct Card { void *data; int card_id; };
static struct Mark hungry_marks[30], javelin_marks[30], yamadron_marks[30];
static struct ShieldMark marks[30];
static struct Card D_801A7AD8[30];
static unsigned deferred_deactivation, deferred_yamadron, deferred_hungry, deferred_javelin;
static unsigned hungry_live_mask, javelin_live_mask;
static int inside_field_actions, gDuel_wSceneStateFlags, battle_slot;
static int forget_calls, yamadron_forget_calls;
static int battle_participant(int i){ return i==battle_slot; }
static void forget(struct Card *c){(void)c; ++forget_calls;marks[20].ritual=0;}
static void forget_yamadron(struct Card *c){(void)c;++yamadron_forget_calls;yamadron_marks[20].ritual=0;}
static void mask_forget(void *p){(void)p;}
static void run(int i, struct Card *card){
REPLACE
}
static void reset(int phase, int inside, int participant){
  static int identity;
  memset(hungry_marks,0,sizeof(hungry_marks));memset(javelin_marks,0,sizeof(javelin_marks));
  memset(yamadron_marks,0,sizeof(yamadron_marks));memset(marks,0,sizeof(marks));
  memset(D_801A7AD8,0,sizeof(D_801A7AD8));
  hungry_marks[20].ritual=javelin_marks[20].ritual=1;
  marks[20].ritual=yamadron_marks[20].ritual=1;
  hungry_marks[20].data=javelin_marks[20].data=&identity;
  D_801A7AD8[20].data=&identity;
  hungry_live_mask=javelin_live_mask=(1u<<20);
  deferred_hungry=deferred_javelin=deferred_yamadron=deferred_deactivation=0;
  forget_calls=yamadron_forget_calls=0;
  inside_field_actions=inside;
  gDuel_wSceneStateFlags=phase;
  battle_slot=participant ? 20 : -1;
}
int main(void){
  reset(9,0,1);run(20,&D_801A7AD8[20]);
  assert(marks[20].ritual && yamadron_marks[20].ritual && hungry_marks[20].ritual && javelin_marks[20].ritual);
  assert(hungry_live_mask && javelin_live_mask && !forget_calls && !yamadron_forget_calls);
  reset(9,1,1);run(20,&D_801A7AD8[20]);
  assert(deferred_deactivation==(1u<<20) && deferred_yamadron==(1u<<20) &&
         deferred_hungry==(1u<<20) && deferred_javelin==(1u<<20));
  reset(9,0,0);run(20,&D_801A7AD8[20]);
  assert(!marks[20].ritual && !yamadron_marks[20].ritual && !hungry_marks[20].ritual && !javelin_marks[20].ritual);
  reset(5,0,1);run(20,&D_801A7AD8[20]);
  assert(!marks[20].ritual && !yamadron_marks[20].ritual && !hungry_marks[20].ritual && !javelin_marks[20].ritual);
  puts("Shield/Yamadron/Hungry/Javelin native battle staging: OK");return 0;
}
'''.replace('REPLACE',block)
with tempfile.TemporaryDirectory(prefix='ritual-battle-markers-') as tmp:
    source=Path(tmp,'test.c');binary=Path(tmp,'test');source.write_text(fixture)
    subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Wno-unused-function','-o',str(binary),str(source)],check=True)
    subprocess.run([str(binary)],check=True)
