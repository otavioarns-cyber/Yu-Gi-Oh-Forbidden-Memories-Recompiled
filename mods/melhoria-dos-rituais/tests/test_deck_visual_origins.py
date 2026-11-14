"""Compile the actual deck-visual origin helper and check visual separation."""
import pathlib
import subprocess
import tempfile

root = pathlib.Path(__file__).resolve().parents[1]
s = (root / 'rules.c').read_text()
a = s.index('static void deck_visual_origin(')
b = s.index('/* Build a field-style card object', a)
helper = s[a:b]
assert s.count('deck_visual_origin(source_index, &x, &z);') == 1

harness = r'''
#include <assert.h>
#include <stdio.h>
#define RITUAL_SRC_NONE 0
#define RITUAL_SRC_FIELD 1
#define RITUAL_SRC_HAND 2
#define RITUAL_SRC_DECK 3
#define DECK_VISUAL_X 210
#define DECK_VISUAL_Z 190
static int where[3], side;
''' + helper + r'''
static void check_case(int s, int a, int b, int c) {
    int x[3]={0}, z[3]={0}, i, j, count=0;
    side=s; where[0]=a; where[1]=b; where[2]=c;
    for (i=0;i<3;i++) if(where[i]==RITUAL_SRC_DECK) {
        deck_visual_origin(i,&x[i],&z[i]);
        count++;
        /* The origin stays on the acting side's Deck half of the board. */
        assert(s ? x[i]<0 : x[i]>0);
    }
    for(i=0;i<3;i++) if(where[i]==RITUAL_SRC_DECK)
        for(j=i+1;j<3;j++) if(where[j]==RITUAL_SRC_DECK)
            assert(x[i]!=x[j] && z[i]!=z[j]);
    if(count==1) for(i=0;i<3;i++) if(where[i]==RITUAL_SRC_DECK) {
        assert(x[i] == (s ? -DECK_VISUAL_X : DECK_VISUAL_X));
        assert(z[i] == (s ? DECK_VISUAL_Z : -DECK_VISUAL_Z));
    }
}
int main(void) {
    int a,b,c,s;
    for (s=0;s<2;s++) for(a=0;a<4;a++) for(b=0;b<4;b++)
        for(c=0;c<4;c++) check_case(s,a,b,c);
    puts("PASS: 128 source/side combinations; distinct Deck entrances; single-Deck preserved");
    return 0;
}
'''
with tempfile.TemporaryDirectory() as d:
    p=pathlib.Path(d);(p/'test.c').write_text(harness)
    subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror',str(p/'test.c'),'-o',str(p/'test')], check=True)
    subprocess.run([str(p/'test')],check=True)


# Regression: the port's effect_22 uses a second field-object query,
# which must return the staged visuals, not unrelated native field cards.
from pathlib import Path
import subprocess
import tempfile

base = Path(__file__).resolve().parents[1]
s = (base / 'rules.c').read_text()
a = s.index('static s32 ritual_field_objects(')
b = s.index('static void ritual_deactivate(', a)
production = s[a:b]
assert s.count('Duel_RitualFieldObjects,\n                    (void *)ritual_field_objects') == 1
harness = r'''
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
typedef int32_t s32;
#define G32
#define BRIDGE_NONE 0
#define BRIDGE_EXTENDED 1
#define BRIDGE_RETAIL_RESULT 2
static int bridge_mode, active_result, native_calls;
static void *cached_tribute_objects[3];
static int dark_magician, winged_dragon, blast_juggler, b_skull_dragon;
static s32 native_field_objects(void **objects) {
    native_calls++;
    if (objects) { objects[0] = &dark_magician; objects[1] = &b_skull_dragon; }
    return 2;
}
static s32 (*o_field_objects)(void **objects) = native_field_objects;
''' + production + r'''
int main(void) {
    void *visual[5] = { 0, 0, 0, (void *)0xA, (void *)0xB };
    /* Native ritual context has a mismatched pair: Dark Magician + innocent B. Skull.
       Actual extended recipe has Dark Magician on Field, other two in Deck. */
    bridge_mode = BRIDGE_EXTENDED; active_result = 722;
    cached_tribute_objects[0] = &winged_dragon;
    cached_tribute_objects[1] = &dark_magician;
    cached_tribute_objects[2] = &blast_juggler;
    assert(ritual_field_objects(visual) == 3);
    assert(visual[0] == &winged_dragon && visual[1] == &dark_magician && visual[2] == &blast_juggler);
    assert(visual[0] != &b_skull_dragon && visual[1] != &b_skull_dragon && visual[2] != &b_skull_dragon);
    assert(visual[3] == (void *)0xA && visual[4] == (void *)0xB);
    assert(native_calls == 0);
    /* The original three-field ritual and out-of-summon calls delegate unchanged. */
    bridge_mode = BRIDGE_NONE;
    assert(ritual_field_objects(visual) == 2 && visual[1] == &b_skull_dragon && native_calls == 1);
    bridge_mode = BRIDGE_RETAIL_RESULT;
    assert(ritual_field_objects(visual) == 2 && native_calls == 2);
    bridge_mode = BRIDGE_EXTENDED; active_result = 0;
    assert(ritual_field_objects(visual) == 2 && native_calls == 3);
    active_result = 722; cached_tribute_objects[2] = 0;
    assert(ritual_field_objects(visual) == 2 && native_calls == 4);
    assert(ritual_field_objects(0) == 2 && native_calls == 5);
    puts("PASS: extended flight uses 3 selected tributes; B. Skull excluded; native fallback preserved");
    return 0;
}
'''
with tempfile.TemporaryDirectory() as tmp:
    p = Path(tmp)
    (p / 'check.c').write_text(harness)
    subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror',str(p/'check.c'),'-o',str(p/'check')],check=True)
    subprocess.run([str(p/'check')],check=True)
