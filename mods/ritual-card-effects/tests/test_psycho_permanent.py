"""Compile the actual permanent-control and delayed return functions with duel stubs."""
from pathlib import Path
import subprocess,tempfile
s=(Path(__file__).resolve().parents[1]/'mod.c').read_text()
start=s.index('static void psycho_return_control(PsychoMark *m)\n{')
end=s.index('static void psycho_trigger_for_side',start)
segment=s[start:end]
assert 'psycho_return_after_master_death' in segment
assert 'psycho_source_slot(m)<0' in segment
assert 'psycho_return_for_side' not in s
code=r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_CARD_FLAG_OCCUPIED 0x8000
#define DUEL_SCENE_PHASE_MASK 15
#define take_mask_bit fake_take_bit
static int fake_take_bit(unsigned *m){int i=0;while(!(*m&(1u<<i)))++i;*m &= ~(1u<<i);return i;}
typedef struct {void *data;int flags;int card_id;} DuelCardRecord;
typedef struct {int ritual, controlling, psycho_slot, current_slot,original_slot,controller_side;void* psycho_data;void* stolen_data;int stolen_card_id;} PsychoMark;
typedef struct {void(*log)(void*,char const*,...);} Host;
static Host *host=0;
static DuelCardRecord D_801A7AD8[30];
static PsychoMark psycho_marks[30];
static unsigned psycho_controlling_mask,psycho_controlling_count;
static int gDuel_wSceneStateFlags=5;
static int monster_slot(int r){return (r>=5&&r<10)||(r>=20&&r<25);}
static int psycho_source_slot(PsychoMark*m){int i=m->psycho_slot;return monster_slot(i)&& (D_801A7AD8[i].flags&0x8000) && D_801A7AD8[i].data==m->psycho_data ?i:-1;}
static int psycho_stolen_slot(PsychoMark*m){int i=m->current_slot;return monster_slot(i)&&(D_801A7AD8[i].flags&0x8000)&&D_801A7AD8[i].data==m->stolen_data ?i:-1;}
static void visual_queue_activation(int a){(void)a;}
static int first_free_monster_slot(int side){int i=side?20:5;for(;i<(side?25:10);i++)if(!(D_801A7AD8[i].flags&0x8000))return i;return -1;}
static int move_monster_record(int from,int to){if(!monster_slot(from)||!monster_slot(to)||!D_801A7AD8[from].flags||D_801A7AD8[to].flags)return 0;D_801A7AD8[to]=D_801A7AD8[from];memset(&D_801A7AD8[from],0,sizeof(DuelCardRecord));return 1;}
''' + segment + r'''
static void reset(void){memset(D_801A7AD8,0,sizeof(D_801A7AD8));memset(psycho_marks,0,sizeof(psycho_marks));psycho_controlling_mask=1;psycho_controlling_count=1;
 psycho_marks[0].ritual=psycho_marks[0].controlling=1;psycho_marks[0].psycho_slot=20;psycho_marks[0].psycho_data=(void*)0x777;
 psycho_marks[0].current_slot=21;psycho_marks[0].stolen_data=(void*)0x888;psycho_marks[0].original_slot=5;psycho_marks[0].controller_side=1;
 D_801A7AD8[20].flags=0x8000;D_801A7AD8[20].data=(void*)0x777;
 D_801A7AD8[21].flags=0x8000;D_801A7AD8[21].data=(void*)0x888;
}
int main(void){
 reset();psycho_return_after_master_death();assert(psycho_controlling_count==1 && D_801A7AD8[21].data==(void*)0x888); /* master alive stays stolen */
 D_801A7AD8[20].flags=0;psycho_return_after_master_death();assert(!psycho_controlling_count&&D_801A7AD8[5].data==(void*)0x888); /* at turn transition */
 reset();D_801A7AD8[20].flags=0;D_801A7AD8[21].flags=0;psycho_return_after_master_death();assert(!psycho_controlling_count && !D_801A7AD8[5].flags); /* Raigeki both dead */
 reset();D_801A7AD8[20].flags=0;D_801A7AD8[5].flags=0x8000;
 psycho_return_after_master_death();assert(!psycho_controlling_count && D_801A7AD8[6].data==(void*)0x888); /* alt slot if original occupied */
 reset();D_801A7AD8[20].flags=0;for(int i=5;i<10;i++)D_801A7AD8[i].flags=0x8000;
 psycho_return_after_master_death();assert(psycho_controlling_count==1 && D_801A7AD8[21].data==(void*)0x888); /* blocked: keep link */
 D_801A7AD8[5].flags=0;psycho_return_after_master_death();assert(!psycho_controlling_count && D_801A7AD8[5].data==(void*)0x888);
 puts("PASS: Puppet permanent control, delayed death return, Raigeki, occupied zone and retry");
}
'''
with tempfile.TemporaryDirectory() as t:
 p=Path(t)/'h.c';p.write_text(code)
 subprocess.run(['gcc','-std=c99','-Wall','-Wno-unused-function',str(p),'-o',t+'/run'],check=True)
 subprocess.run([t+'/run'],check=True)
