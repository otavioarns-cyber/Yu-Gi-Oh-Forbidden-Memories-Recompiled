"""Compile the actual v0.78.49 Fiend Mirror comparator branch with a fake duel."""
from pathlib import Path
import subprocess, tempfile
src=(Path(__file__).resolve().parents[1]/'mod.c').read_text()
a=src.index('    /* A Ritual Fiend\'s Mirror in DEF behaves')
b=src.index('    if(!sengenjin_live_hint',a)
branch=src[a:b]
assert 'mirror_defense_direct_pending' in branch
assert '(D_8009B174&0x0f)==4' in branch, 'Native comparator executes after phase 3 advances to phase 4'
assert '(D_8009B174&0x0f)==3' not in branch
code=r'''
#include <assert.h>
#include <stdio.h>
#define FIENDS_MIRROR_ID 365
#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_SCENE_PHASE_MASK 15
#define DUEL_CARD_FLAG_DEFENSE_POSITION 0x800
#define s32 int
#define DUEL_CARD_FLAG_OCCUPIED 0x8000
typedef struct {int field_6A;} DisplayObject;
typedef struct {int ritual; void *data;} MirrorMark;
typedef struct {int card_id;int attack;int defense;void *data;} DuelCardRecord;
static DuelCardRecord D_801A7AD8[30];
static MirrorMark fiend_mirror_marks[30];
static int fiend_mirror_live_hint=1,fiends_mirror_effect_on=1;
static int D_8009B17A=0;
static int D_8009B174=3;
static int gDuel_wSceneStateFlags=9;
static int D_8009B1D5=0;
static unsigned mirror_defense_direct_pending;
static int mirror_defense_direct_side=-1;
static int side_for_record(int i){return i>=15;}
static int Duel_CalcCardStats(DuelCardRecord *a){return a->attack;}
static int original_battle_compare(DisplayObject *a,DisplayObject *b){(void)a;(void)b;return 300;}
static int tested(DisplayObject *left_object,DisplayObject *right_object){
''' + branch + r'''
 return original_battle_compare(left_object,right_object);
}
static int run_native_battle_compare(DisplayObject *a,DisplayObject *b) {
 /* From DuelScene_UpdateBattle case 3: advance to 4 before the call. */
 assert(D_8009B174==3);
 D_8009B174=4;
 return tested(a,b);
}
int main(void){
 DisplayObject a={5},d={20};
 fiend_mirror_marks[20].ritual=1;
 D_801A7AD8[20].card_id=FIENDS_MIRROR_ID;
 D_801A7AD8[20].data=(void*)0x1234;fiend_mirror_marks[20].data=(void*)0x1234;
 D_801A7AD8[5].attack=500;
 D_8009B17A=DUEL_CARD_FLAG_DEFENSE_POSITION;
 assert(run_native_battle_compare(&a,&d)==0);
 assert(mirror_defense_direct_pending==500 && mirror_defense_direct_side==1);
 D_801A7AD8[5].attack=4000;
 D_8009B174=3;
 assert(run_native_battle_compare(&a,&d)==0 && mirror_defense_direct_pending==4000);
 D_8009B17A=0;
 assert(tested(&a,&d)==300); /* no protection in ATK */
 D_8009B17A=DUEL_CARD_FLAG_DEFENSE_POSITION;
 fiend_mirror_marks[20].ritual=0;
 assert(tested(&a,&d)==300); /* normal copy unaffected */
 fiend_mirror_marks[20].ritual=1;
 D_801A7AD8[20].card_id=FIENDS_MIRROR_ID;
 D_801A7AD8[20].data=(void*)0x1234;fiend_mirror_marks[20].data=(void*)0x1234;
 D_8009B174=3;
 assert(tested(&a,&d)==300); /* native battle() entry precedes compare */
 D_8009B174=5;
 assert(tested(&a,&d)==300); /* already past comparison */
 D_8009B174=4;
 assert(tested(&a,&d)==0 && mirror_defense_direct_pending==4000);
 /* The native controller changes to 4 before invoking the comparator.
  * Previous v0.78.49 harness incorrectly seeded stage 3. */
 /* Different card occupying a recycled slot MUST NOT inherit Fiend's Mirror. */
 D_801A7AD8[20].card_id=500;
 mirror_defense_direct_pending=0;
 assert(tested(&a,&d)==300 && mirror_defense_direct_pending==0);
 D_801A7AD8[20].card_id=FIENDS_MIRROR_ID;
 D_801A7AD8[20].data=(void*)0x9999;
 assert(tested(&a,&d)==300 && mirror_defense_direct_pending==0);
 puts("PASS: ritual Mirror defensive direct damage ignores DEF; stale marks rejected");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'h.c';p.write_text(code)
 subprocess.run(['gcc','-std=c99','-Wall','-Werror',str(p),'-o',d+'/test'],check=True)
 subprocess.run([d+'/test'],check=True)
