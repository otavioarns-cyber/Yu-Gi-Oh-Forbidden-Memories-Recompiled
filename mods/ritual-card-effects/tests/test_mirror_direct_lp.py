"""C simulation of the exact damage settlement taken from production battle()."""
from pathlib import Path
import subprocess,tempfile
s=(Path(__file__).resolve().parents[1]/'mod.c').read_text()
a=s.index('    if(phase==9 && (state&0x0f)==3 && mirror_defense_direct_pending')
b=s.index('    if(phase==9 && (state&0x0f)==3 && !ultimate_trap_cinematic',a)
block=s[a:b]
assert 'mirror_queue_prepaid(target^1' in block
assert 'mirror_last_lp[target]' in block
assert 'if(!prepaid)' in s
code=r'''
#include <assert.h>
#include <stdio.h>
#define u16 unsigned short
#define DUEL_SIDE_COUNT 2
typedef struct{u16 unsigned_value;} LP;
typedef struct{LP life_points;} Side;
typedef struct{void (*log)(void*,char const*,...);} Host;
static Host *host=0;
static Side D_800E9FF0[2];
static u16 mirror_last_lp[2];
static unsigned mirror_defense_direct_pending;
static int mirror_defense_direct_side=-1;
static int D_8009B22A=0;
static int visuals_count;
static int visual_find_live_mirror(int i){return i;}
static void mirror_queue_prepaid(int i,unsigned n,int src){(void)src;assert(i>=0 && i<2);assert(n);++visuals_count;}
static void mirror_watch_lp(void){}
static void tick(int phase,int state){
''' + block + r'''
}
int main(void){
 D_800E9FF0[0].life_points.unsigned_value=8000;D_800E9FF0[1].life_points.unsigned_value=8000;
 mirror_defense_direct_side=1;mirror_defense_direct_pending=500;tick(9,3);
 assert(D_800E9FF0[0].life_points.unsigned_value==7500&&D_800E9FF0[1].life_points.unsigned_value==7500);
 assert(visuals_count==2 && mirror_last_lp[0]==7500&&mirror_last_lp[1]==7500);
 tick(9,3);assert(D_800E9FF0[0].life_points.unsigned_value==7500); /* no repeat */
 D_800E9FF0[0].life_points.unsigned_value=8000;D_800E9FF0[1].life_points.unsigned_value=8000;
 mirror_defense_direct_side=1;mirror_defense_direct_pending=4000;tick(9,3);
 assert(D_800E9FF0[0].life_points.unsigned_value==4000&&D_800E9FF0[1].life_points.unsigned_value==4000);
 D_800E9FF0[0].life_points.unsigned_value=1000;D_800E9FF0[1].life_points.unsigned_value=2000;
 mirror_defense_direct_side=1;mirror_defense_direct_pending=4000;tick(9,3);
 assert(D_800E9FF0[0].life_points.unsigned_value==0&&D_800E9FF0[1].life_points.unsigned_value==0);
 D_800E9FF0[0].life_points.unsigned_value=8000;D_800E9FF0[1].life_points.unsigned_value=8000;
 mirror_defense_direct_side=1;mirror_defense_direct_pending=700;
 D_8009B22A=0x296; /* attack aborted by Trap */
 int count_before=visuals_count;
 tick(9,3);
 assert(D_800E9FF0[0].life_points.unsigned_value==8000 && D_800E9FF0[1].life_points.unsigned_value==8000);
 assert(visuals_count==count_before);
 puts("PASS: Mirror defensive damage, lethal reflection, no repeat or trap-aborted damage");
}
'''
with tempfile.TemporaryDirectory() as t:
 p=Path(t)/'h.c';p.write_text(code)
 subprocess.run(['gcc','-std=c99','-Wall','-Werror',str(p),'-o',t+'/run'],check=True)
 subprocess.run([t+'/run'],check=True)
