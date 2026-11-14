#!/usr/bin/env python3
"""Fixture using the actual gate extracted from mod.c, independent of game runtime."""
from pathlib import Path
import subprocess, tempfile
source=(Path(__file__).resolve().parents[1]/'mod.c').read_text()
start=source.index('static int ritual_auto_field_busy(void)')
brace=source.index('{',start)
depth=0
end=None
for i in range(brace,len(source)):
 if source[i]=='{':depth+=1
 if source[i]=='}':
  depth-=1
  if depth==0:end=i+1;break
assert end
function=source[start:end]
entry=source.index('static void field_actions(void)')
body=source[entry:entry+1000]
assert 'if(ritual_auto_field_busy())return;' in body
assert body.index('if(ritual_auto_field_busy())return;')<body.index('original_field_actions()')
code=r'''
#include <assert.h>
#include <stdio.h>
#define TRI_HORNED_ID 705
#define BLACK_LUSTER_SOLDIER_ID 700
#define ZERA_ID 691
#define CRAB_TURTLE_ID 693
static int tri_field_pending,bls_field_pending,zera_field_pending;
static int crab_umi_sequence,ritual_field_source_card_id;
static int enabled[1000];
static int ritual_card_effect_enabled(int id){return id>=0&&id<1000&&enabled[id];}
'''+function+r'''
static int attacks,actions,effect_ticks,terrain;
/* Unlike the CPU field action, the asynchronous scene still advances while gated. */
static void scene_tick(void){
 if(ritual_auto_field_busy()) { effect_ticks++;
   if(tri_field_pending){tri_field_pending=0;crab_umi_sequence=1;ritual_field_source_card_id=705;}
   else if(bls_field_pending){bls_field_pending=0;crab_umi_sequence=1;ritual_field_source_card_id=700;}
   else if(zera_field_pending){zera_field_pending=0;crab_umi_sequence=1;ritual_field_source_card_id=691;}
   else if(crab_umi_sequence){crab_umi_sequence++;if(crab_umi_sequence==5){terrain=1;crab_umi_sequence=0;}}
 } else {actions++;attacks++;assert(terrain==1);}
}
static void reset(void){
 tri_field_pending=bls_field_pending=zera_field_pending=crab_umi_sequence=0;
 attacks=actions=effect_ticks=terrain=0;
 for(int i=0;i<1000;i++)enabled[i]=0;
}
static void run(int ritual){
 reset(); enabled[ritual]=1;
 if(ritual==705)tri_field_pending=1;
 else if(ritual==700)bls_field_pending=1;
 else if(ritual==691)zera_field_pending=1;
 else {ritual_field_source_card_id=ritual;crab_umi_sequence=1;}
 for(int i=0;i<(ritual==693?3:4);i++){assert(ritual_auto_field_busy());scene_tick();assert(actions==0&&attacks==0);}
 while(ritual_auto_field_busy()){scene_tick();assert(actions==0&&attacks==0);}
 assert(terrain==1&&!ritual_auto_field_busy());scene_tick();assert(actions==1&&attacks==1);
}
int main(void){
 run(705);run(700);run(691);run(693);
 reset();tri_field_pending=1;assert(!ritual_auto_field_busy());
 reset();crab_umi_sequence=3;ritual_field_source_card_id=705;assert(!ritual_auto_field_busy());
 reset();terrain=1;assert(!ritual_auto_field_busy());scene_tick();assert(attacks==1);
 puts("PASS automatic terrain action gate: Tri, BLS, Zera, Crab; complete visual before attack; disabled/normal unaffected");
 return 0;
}
'''
with tempfile.TemporaryDirectory() as tmp:
 src=Path(tmp)/'test.c';src.write_text(code)
 exe=Path(tmp)/'test'
 subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror',str(src),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
