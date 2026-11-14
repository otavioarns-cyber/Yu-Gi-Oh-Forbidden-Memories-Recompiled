"""Compile and exercise the production Mirror LP watcher around SDK AFTER callbacks."""
from pathlib import Path
import subprocess, tempfile
s=Path(__file__).resolve().parents[1].joinpath('mod.c').read_text()
a=s.rindex('static void mirror_watch_lp(void)\n{')
b=s.index('\nstatic void turn_switch(void)',a)
body=s[a:b]
assert 'mirror_commit_pending[side]' in body
code=r'''
#include <assert.h>
#include <stdio.h>
#define DUEL_SIDE_COUNT 2
#define DUEL_SCENE_PHASE_MASK 15
#define MIRROR_QUEUE_CAP 8
#define u16 unsigned short
#define u8 unsigned char
typedef struct {u16 unsigned_value;} LP;
typedef struct {LP life_points;} Side;
typedef struct {int active;} Reflection;
typedef struct {int target_side;unsigned damage;int source_index;int visual_queued;int prepaid;} MirrorQueuedHit;
typedef struct {void(*log)(void *,const char*,...);} Host;
static Host *host;
static Side D_800E9FF0[2];
static u16 mirror_last_lp[2], mirror_commit_before[2], mirror_commit_after[2];
static u8 mirror_commit_pending[2], mirror_battle_grace[2];
static int mirror_lp_ready=1,fiend_mirror_live_hint=1;
static Reflection mirror_reflection;
static unsigned mirror_queue_count,mirror_queue_head;
static MirrorQueuedHit mirror_queue[MIRROR_QUEUE_CAP];
static int gDuel_wSceneStateFlags=9,queued_count;
static int side_has_ritual_fiend_mirror(int side){return side==0;}
static int visual_find_live_mirror(int side){(void)side;return -1;}
static int visual_queue_activation(int i){(void)i;return 1;}
static int visual_activation_busy(void){return 0;}
static void mirror_finish_reflection(void){}
static void mirror_queue_hit(int side,unsigned damage,int source_index){
 (void)side;(void)source_index;assert(damage>0);++queued_count;
 ++mirror_queue_count;
}
static void mirror_begin_reflection(int side,unsigned damage,int prepaid){
 (void)side;(void)damage;(void)prepaid;
}
''' + body + r'''
int main(void){
 D_800E9FF0[0].life_points.unsigned_value=8000;
 D_800E9FF0[1].life_points.unsigned_value=8000;
 mirror_last_lp[0]=8000;mirror_last_lp[1]=8000;
 /* SDK AFTER has already queued the battle hit before native writes it. */
 mirror_commit_pending[0]=1;mirror_commit_before[0]=8000;mirror_commit_after[0]=7000;
 mirror_last_lp[0]=7000;
 mirror_queue_hit(1,1000,-1);
 mirror_watch_lp();
 assert(mirror_last_lp[0]==7000 && mirror_commit_pending[0]==1 && queued_count==1);
 /* The actual LP write must NOT be reflected a second time. */
 D_800E9FF0[0].life_points.unsigned_value=7000;
 mirror_watch_lp();
 assert(mirror_last_lp[0]==7000 && mirror_commit_pending[0]==0 && queued_count==1);
 /* Spellbinding Circle is a stat debuff: no LP change, no reflection. */
 mirror_watch_lp();
 assert(queued_count==1);
 /* An independent real LP loss is still recognized by the fallback. */
 D_800E9FF0[0].life_points.unsigned_value=6800;
 mirror_watch_lp();
 assert(queued_count==2);
 puts("PASS: no duplicated battle LP event or phantom Spellbinding reflection; other real losses preserved");
}
'''
with tempfile.TemporaryDirectory() as td:
 p=Path(td)/'test.c';p.write_text(code)
 subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror',str(p),'-o',str(Path(td)/'test')],check=True)
 subprocess.run([str(Path(td)/'test')],check=True)
