"""Run the real Zera aura and Ritual Ultimate identity predicates from mod.c."""
from pathlib import Path
import subprocess
import tempfile

source = Path(__file__).resolve().parents[1].joinpath('mod.c').read_text()


def extract(first, last):
    a = source.index(first)
    b = source.index(last, a)
    return source[a:b]


identity = extract('static int ultimate_identity(int i)\n{', 'static int cosmo_identity(int i)')
active = extract('static int ritual_effectively_present(int i)\n{', '/* One central truth for every opposing card-effect interaction.')
aura = extract('    /* A normal or fusion-summoned Ultimate has no Ritual immunity.',
               '    if (serpent_seen && ritual_card_effect_enabled(SERPENT_NIGHT_ID)',)
assert 'card->card_id!=BLUE_EYES_ULTIMATE_ID' not in aura
assert '!ultimate_effect_active(i)' in aura

fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_CARD_FLAG_OCCUPIED 0x8000
#define DUEL_SCENE_PHASE_MASK 0xF
#define BLUE_EYES_ULTIMATE_ID 380
#define ZERA_ID 360
#define SERPENT_NIGHT_ID 706
#define STAT_HINT_ZERA (1u<<11)
#define ZERA_AURA_PENALTY 1500
#define CARD_TYPE_FIEND 6
#define CARD_TYPE_MAGIC 20
typedef unsigned short u16;
typedef struct { int card_id; void *data; unsigned flags; } DuelCardRecord;
typedef struct { void *data; unsigned ritual; } RitualMark;
static DuelCardRecord D_801A7AD8[DUEL_CARD_RECORD_COUNT];
static RitualMark zera_marks[DUEL_CARD_RECORD_COUNT], ultimate_marks[DUEL_CARD_RECORD_COUNT];
static unsigned zera_live_mask, stat_effect_hints;
static int gDuel_wSceneStateFlags, ultimate_setting, zera_setting, battle_a=-1, battle_b=-1;
static int monster_slot(int i){return i>=5 && i<30 && (i%15)<10;}
static int battle_participant(int i){return i==battle_a || i==battle_b;}
static int ritual_card_effect_enabled(int id){
    return id==ZERA_ID ? zera_setting : (id==BLUE_EYES_ULTIMATE_ID ? ultimate_setting : 0);
}
static int card_type_id(const DuelCardRecord *c){return c->card_id==ZERA_ID ? CARD_TYPE_FIEND : 1;}
static int take_mask_bit(unsigned *mask){int i=__builtin_ctz(*mask);*mask&=(*mask-1);return i;}
REPLACE_IDENTITY
REPLACE_ACTIVE
static uint32_t eval(int i){
    DuelCardRecord *card=&D_801A7AD8[i];
    uint32_t packed=(4300u<<16)|5000u;
    int aura_monster=!!(card->flags & DUEL_CARD_FLAG_OCCUPIED) ||
        ((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 && battle_participant(i));
    int type=-1;
REPLACE_AURA
    return packed;
}
static void reset(void){
    memset(D_801A7AD8,0,sizeof(D_801A7AD8));
    memset(zera_marks,0,sizeof(zera_marks));
    memset(ultimate_marks,0,sizeof(ultimate_marks));
    zera_setting=ultimate_setting=1;
    stat_effect_hints=STAT_HINT_ZERA;
    static int ultimate_token,zera_token;
    D_801A7AD8[5]=(DuelCardRecord){BLUE_EYES_ULTIMATE_ID,&ultimate_token,DUEL_CARD_FLAG_OCCUPIED};
    D_801A7AD8[20]=(DuelCardRecord){ZERA_ID,&zera_token,DUEL_CARD_FLAG_OCCUPIED};
    zera_marks[20].ritual=1;zera_marks[20].data=&zera_token;
    zera_live_mask=1u<<20;
    battle_a=battle_b=-1;gDuel_wSceneStateFlags=5;
}
int main(void){
    uint32_t v;
    reset(); v=eval(5);assert((v&0xffffu)==3500 && (v>>16)==2800);
    reset();ultimate_marks[5].ritual=1;ultimate_marks[5].data=D_801A7AD8[5].data;
    v=eval(5);assert((v&0xffffu)==5000 && (v>>16)==4300);
    reset();ultimate_marks[5].ritual=1;ultimate_marks[5].data=D_801A7AD8[5].data;
    ultimate_setting=0;v=eval(5);assert((v&0xffffu)==3500 && (v>>16)==2800);
    reset();ultimate_marks[5].ritual=1;ultimate_marks[5].data=(void *)0x1234;
    v=eval(5);assert((v&0xffffu)==3500 && (v>>16)==2800);
    reset();zera_live_mask=0;v=eval(5);assert((v&0xffffu)==5000 && (v>>16)==4300);
    reset();zera_setting=0;v=eval(5);assert((v&0xffffu)==5000 && (v>>16)==4300);
    reset();gDuel_wSceneStateFlags=9;battle_a=5;battle_b=20;
    D_801A7AD8[5].flags=0;D_801A7AD8[20].flags=0;
    v=eval(5);assert((v&0xffffu)==3500 && (v>>16)==2800);
    reset();gDuel_wSceneStateFlags=9;battle_a=5;battle_b=20;
    D_801A7AD8[5].flags=0;D_801A7AD8[20].flags=0;
    ultimate_marks[5].ritual=1;ultimate_marks[5].data=D_801A7AD8[5].data;
    v=eval(5);assert((v&0xffffu)==5000 && (v>>16)==4300);
    puts("PASS: Zera affects ordinary Ultimate, excludes only live Ritual Ultimate, respects settings/identity/staging");
}
'''
fixture=fixture.replace('REPLACE_IDENTITY',identity).replace('REPLACE_ACTIVE',active).replace('REPLACE_AURA',aura)
with tempfile.TemporaryDirectory(prefix='zera-aura-') as t:
    path=Path(t,'test.c')
    path.write_text(fixture)
    exe=Path(t,'test')
    subprocess.run(['gcc','-std=c11','-O2','-Wall','-Wextra','-Werror','-Wno-unused-function',str(path),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
