"""Execute the real existing arming switch and new bridge with engine mocks.
The helpers below stand in for presentation/game actions; their call counts
check one-shot triggering without pretending to test Windows rendering.
"""
import os,re,subprocess,tempfile
from pathlib import Path
root=Path(__file__).resolve().parent.parent
s=(root/'mod.c').read_text()
def function(name):
    at=s.index(name+'(void)\n{');start=s.rfind('\nstatic ',0,at)+1
    brace=s.index('{',at);depth=1;end=brace+1
    while depth:depth+=(s[end]=='{')-(s[end]=='}');end+=1
    return s[start:end]
arm=function('limit_test_arm_field_effects')
bridge=(root/'egotist_bridge.h').read_text()
families=re.findall(r'case (\w+)_ID:return (\w+)\[i\]\.ritual',bridge)
fields=set(re.findall(r'\w+\[index\]\.(\w+)',arm))-{'data','life_points'}
constants=set(re.findall(r'\b[A-Z][A-Z_]+\b',arm+bridge))
defines=dict(re.findall(r'^#define (\w+)\s+(\d+)\b',s,re.M))
defines.update({'DUEL_CARD_FLAG_OCCUPIED':'1','DUEL_CARD_FLAG_FACE_DOWN':'2'})
out='#include <assert.h>\n#include <string.h>\n#include <stdio.h>\n'
for n,c in enumerate(sorted(constants),1000):
    if n and c not in ['NULL','ATK','LP','DEF','FM']:
        out+=f'#define {c} {defines.get(c,str(n))}\n'
out+='''
typedef struct {int card_id,flags;void *data;} DuelCardRecord;
static DuelCardRecord D_801A7AD8[30];
typedef struct {void *data;int id;unsigned serial;} BorrowedOrigin;
static BorrowedOrigin borrowed_origin[30];
static unsigned leases[30];
static int test_ignore_ritual_requirement;
static unsigned borrowed_lease(int i){return leases[i];}
static int monster_slot(int i){return i>=0&&i<30&&i%15>=5&&i%15<10;}
typedef struct {void *data;
'''+''.join('unsigned '+f+';' for f in sorted(fields))+'} Mark;\n'
for _,m in families:out+=f'static Mark {m}[30];\n'
scalars=set(re.findall(r'\b(\w+(?:_live_mask|_live_hint|_summons|_pending|_seen))\b',arm+bridge))
scalars|={'summons','stat_effect_hints','tri_seen','serpent_seen','mirror_queue_count','mirror_lp_ready','gDuel_bTerrain','psycho_pending_mask','psycho_pending_count','chakra_pending_mask','crab_umi_sequence','ritual_field_id','ritual_field_source_card_id'}
scalars-=set(fields)
for v in sorted(scalars):out+='static unsigned '+v+';\n'
out+='''
static unsigned mirror_last_lp[2];
static struct {int active;} mirror_reflection;
static struct {struct {unsigned unsigned_value;} life_points;} D_800E9FF0[2];
typedef struct {int ritual,psycho_slot,pending_summon_trigger;void *psycho_data;} PsychoMark;
static PsychoMark psycho_marks[30];
static PsychoMark *find_psycho_mark(void *d){for(int i=0;i<30;i++)if(psycho_marks[i].ritual&&psycho_marks[i].psycho_data==d)return &psycho_marks[i];return 0;}
static PsychoMark *alloc_psycho_mark(void *d){for(int i=0;i<30;i++)if(!psycho_marks[i].ritual){psycho_marks[i].ritual=1;psycho_marks[i].psycho_data=d;return &psycho_marks[i];}return 0;}
static void *mask_data;
static int limit_test_mask_armed(void *d){return d&&mask_data==d;}
static void mask_summon(DuelCardRecord *r){mask_data=r->data;}
static void mask_forget(void *d){if(mask_data==d)mask_data=0;}
static int effect_enabled=1;
static int ritual_card_effect_enabled(int id){(void)id;return effect_enabled;}
static int side_for_record(int i){return i/15;}
static void ritual_ai_learn_full_effect_for_side(int side,int id){(void)side;(void)id;}
static int count_attack_monsters_except(int i){(void)i;return 2;}
static int female_field_count_except(int i){(void)i;return 1;}
static void visual_queue_activation(int i){(void)i;}
static void visual_queue_stat(int i,int n){(void)i;(void)n;}
static int generated,coin_calls;
static void generate_yamadron_burns(int i,const char *s){(void)i;(void)s;++generated;}
static void coin_summon(int i){++coin_calls;Mark*m=D_801A7AD8[i].card_id==PERFORMANCE_ID?&performance_marks[i]:&chaos_marks[i];m->ritual=1;m->data=D_801A7AD8[i].data;}
static int skull_identity(int i){return skull_marks[i].ritual&&skull_marks[i].data==D_801A7AD8[i].data;}
'''
out+=bridge+'\n'+arm+'\n'
out+='int main(void){int ids[]={'+','.join(c+'_ID' for c,_ in families)+',MASK_ID,PSYCHO_PUPPET_ID};\n'
out+='''
for(unsigned k=0;k<sizeof(ids)/sizeof(ids[0]);k++) {
    int id=ids[k];
    memset(D_801A7AD8,0,sizeof(D_801A7AD8));memset(leases,0,sizeof(leases));
    D_801A7AD8[5].card_id=id;D_801A7AD8[5].flags=DUEL_CARD_FLAG_OCCUPIED;
    D_801A7AD8[5].data=&ids[k];
    assert(!borrowed_effect_armed(5));
    limit_test_arm_field_effects();assert(!borrowed_effect_armed(5));
    leases[5]=k+1;limit_test_arm_field_effects();assert(borrowed_effect_armed(5));
    int before=generated+coin_calls;limit_test_arm_field_effects();assert(before==generated+coin_calls);
    assert(borrowed_origin[5].serial==k+1);
    leases[5]=0;borrowed_reconcile();assert(!borrowed_effect_armed(5));
    assert(before==generated+coin_calls); /* no rollback/replay of completed actions */
}
/* A true Ritual instance must survive revocation of an added equipment lease. */
D_801A7AD8[5].card_id=BLUE_EYES_ULTIMATE_ID;ultimate_marks[5].data=D_801A7AD8[5].data;
ultimate_marks[5].ritual=1;leases[5]=100;
limit_test_arm_field_effects();assert(!borrowed_origin[5].serial);
leases[5]=0;borrowed_reconcile();assert(ultimate_marks[5].ritual);
/* A disabled effect and a face-down ordinary monster are not armed. */
ultimate_marks[5].ritual=0;leases[5]=101;effect_enabled=0;
limit_test_arm_field_effects();assert(!ultimate_marks[5].ritual);
effect_enabled=1;D_801A7AD8[5].flags|=DUEL_CARD_FLAG_FACE_DOWN;
limit_test_arm_field_effects();assert(!ultimate_marks[5].ritual);
puts("PASS: all 24 Ritual families, one-shot arming, revocation, true Ritual preservation, settings and face-down gate");return 0;
}
'''
with tempfile.TemporaryDirectory() as tmp:
    src=Path(tmp)/'bridge.c';src.write_text(out);exe=Path(tmp)/'bridge'
    subprocess.run(['gcc','-std=c11','-O1','-g','-fsanitize=address,undefined',str(src),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'})
