"""Test the exact production rules_v1 callback, not a reimplemented copy."""
from pathlib import Path
import os
import subprocess
import tempfile

base=Path(__file__).resolve().parent.parent
content=(base/'rules.c').read_text()
func=content[content.index('static int read_rules_snapshot('):content.index('static const RitualRulesProviderV1 published_rules')]
with tempfile.TemporaryDirectory() as temp:
    p=Path(temp)
    src=p/'rules_test.c'
    src.write_text(r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "pc/mods/modapi.h"
#include "ritual_rules_provider_v1.h"
static const MemoriesModHost *mod_host;
static int is_applied, reads,field=1,deck=0,hand=0,required=0,player=0;
static int get_setting(const MemoriesModHost *h,const char *key,int fallback){
    (void)h;(void)fallback;reads++;
    if(!strcmp(key,"min_field"))return field;
    if(!strcmp(key,"use_deck"))return deck;
    if(!strcmp(key,"min_hand"))return hand;
    if(!strcmp(key,"require_result"))return required;
    if(!strcmp(key,"ritual_only_player"))return player;
    assert(0);return -1;
}
static int applied(const MemoriesModHost *h){(void)h;return is_applied;}
static int clamp_setting(int n,int min,int max){return n<min?min:n>max?max:n;}
''' + func + r'''
int main(void){
    MemoriesModHost h={0};RitualRulesSnapshotV1 s={0};
    h.applied=applied;h.setting=get_setting;mod_host=&h;
    assert(!read_rules_snapshot(&s,sizeof(s)));
    assert(reads==0);
    is_applied=1;
    assert(!read_rules_snapshot(&s,sizeof(s)-1));
    assert(read_rules_snapshot(&s,sizeof(s)));
    assert(s.abi_version==1&&s.material_count==3&&s.sources==3);
    assert(s.min_field==1 && s.min_hand==0 && s.require_result==0);
    field=0;hand=3;deck=1;required=2;player=1;
    assert(read_rules_snapshot(&s,sizeof(s)));
    assert(s.sources==7 && s.min_field==0 && s.min_hand==3);
    assert(s.require_result==2 && s.ritual_only_player==1);
    field=5;hand=-2;required=-3;
    assert(read_rules_snapshot(&s,sizeof(s)));
    assert(s.min_field==3 && s.min_hand==0 && s.require_result==0);
    deck=0;hand=3;
    assert(read_rules_snapshot(&s,sizeof(s)));
    assert(s.min_hand==0 && s.sources==3);
    puts("PASS Melhoria read-only rules_v1 production callback: defaults, live changes, 0-3, result, disabled and ABI size");
    return 0;
}
''')
    sdk=Path(os.environ['MEMORIES_SDK'])/'include'
    cmd=['gcc','-std=gnu11','-O2','-fsanitize=undefined','-fno-omit-frame-pointer','-I'+str(sdk),'-I'+str(base),str(src),'-o',str(p/'rules_test')]
    subprocess.run(cmd,check=True)
    subprocess.run([str(p/'rules_test')],check=True)
