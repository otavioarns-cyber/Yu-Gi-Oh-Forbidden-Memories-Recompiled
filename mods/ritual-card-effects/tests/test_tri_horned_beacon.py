#!/usr/bin/env python3
"""Compile actual Tri-Horned state/identity helpers against a compact duel fixture."""
from pathlib import Path
import subprocess, tempfile
root=Path(__file__).resolve().parents[1]
s=(root/'tri_horned.h').read_text()
mod=(root/'mod.c').read_text()
def function(name):
    start=s.index(f'static int {name}(') if name != 'tri_sources' else s.index(f'static unsigned {name}(')
    ob=s.index('{',start); depth=0
    for pos in range(ob,len(s)):
        if s[pos]=='{':depth+=1
        elif s[pos]=='}':
            depth-=1
            if depth==0:return s[start:pos+1]
    raise AssertionError(name)
assert 'card->card_id==TRI_HORNED_ID ? 0u' not in mod
harness='''
#include <assert.h>
#include <stdio.h>
#define TRI_HORNED_ID 705
#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_CARD_FLAG_OCCUPIED 1
#define DUEL_CARD_EFFECT_FLAG_ACTIVE 2
#define CARD_TYPE_DINOSAUR 1
#define WASTELAND_TERRAIN 4
struct Card {int card_id, flags;void *data;};
static struct Card D_801A7AD8[30];
struct TriMark {void *data;unsigned ritual,horns,battle_flags;};
static struct TriMark tri_marks[30];
static unsigned tri_live_mask,tri_guard;
static void *tri_guard_data[30];
static int tri_resolution_active,tri_resolution_id=333;
static int gDuel_wEffectCardID=333,gDuel_wCardEffectFlags=2;
static int enabled=1;
static int monster_slot(int i){return (i>=5&&i<10)||(i>=20&&i<25);}
static int side_for_record(int i){return i>=15?1:0;}
static unsigned side_monster_mask(int side){return side?0x1f00000u:0x3e0u;}
static int take_mask_bit(unsigned *p){int i;for(i=0;i<30;i++)if((*p)&(1u<<i)){(*p)&=~(1u<<i);return i;}return -1;}
static int ritual_card_effect_enabled(int id){return id==705&&enabled;}
''' + '\n'.join(function(x) for x in ('tri_identity','tri_live','tri_sources','tri_guarded')) + '''
static void reset(void){
    for(int i=0;i<30;i++){
        D_801A7AD8[i]=(struct Card){0};
        tri_marks[i]=(struct TriMark){0};
        tri_guard_data[i]=0;
    }
    tri_live_mask=tri_guard=0;enabled=1;
    tri_resolution_active=1;tri_resolution_id=gDuel_wEffectCardID=333;
    gDuel_wCardEffectFlags=2;
}
static void card(int i,int id,int ritual){
    D_801A7AD8[i]=(struct Card){id,1,(void *)(long)(i+1)};
    tri_marks[i]=(struct TriMark){D_801A7AD8[i].data,(unsigned)ritual,3,0};
    if(ritual)tri_live_mask|=1u<<i;
}
int main(void){
    reset();card(5,705,1);card(6,705,0);
    assert(tri_sources(5)==0); /* NEVER self-buff */
    assert(tri_sources(6)==1); /* distinct ready-made Tri-Horned receives +1000 */
    card(7,705,1);assert(tri_sources(5)==1);assert(tri_sources(7)==1);
    assert(tri_sources(6)==2); /* both ritual copies buff ready-made ally */
    card(20,705,0);assert(tri_sources(20)==0); /* no cross-side aura */
    tri_guard|=1u<<6;tri_guard_data[6]=D_801A7AD8[6].data;
    assert(tri_guarded(6)); /* ally with the same ID gets magic resistance */
    D_801A7AD8[6].data=(void *)100;assert(!tri_guarded(6));
    reset();card(5,705,1);assert(tri_sources(5)==0);
    enabled=0;card(6,705,0);assert(tri_sources(6)==0);
    puts("PASS: Tri-Horned same-ID allied Dinosaur aura, source self-exclusion, opponent exclusion and magic protection");
}
'''
with tempfile.TemporaryDirectory() as tmp:
    src=Path(tmp)/'t.c';out=Path(tmp)/'t';src.write_text(harness)
    subprocess.run(['gcc','-std=c11','-O2',str(src),'-o',str(out)],check=True)
    subprocess.run([str(out)],check=True)
