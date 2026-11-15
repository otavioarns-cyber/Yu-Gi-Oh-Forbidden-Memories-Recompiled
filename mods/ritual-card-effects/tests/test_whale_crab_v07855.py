from pathlib import Path
import re,subprocess,tempfile
m=Path(__file__).resolve().parents[1]
s=(m/'mod.c').read_text()
def extract(signature):
    a=s.index(signature); p=s.index('{',a); depth=0
    for i in range(p,len(s)):
        if s[i]=='{':depth+=1
        elif s[i]=='}':
            depth-=1
            if depth==0:return s[a:i+1]
    raise AssertionError(signature)
count=extract('static void beacon_other_allied_type_counts(')
whale=extract('static unsigned whale_fish_defense_bonus(')
crab=extract('static int crab_battle_protected(')
assert 'deck_draw_cursor' not in whale
assert 'deck_draw_cursor' not in count
assert 'gDuel_aDeckCardRecords' not in count
assert '!ultimate_effect_active' not in crab
assert s.count('whale_fish_defense_bonus(side,i)')==1
assert s.count('crab_battle_protected(')==4, s.count('crab_battle_protected(') # def plus 2d, state11, 3d
assert 'D_8009B174=6; /* Native 2D survivor path, no burning card. */' in s
assert 'battle_visual_survivor_parts|=1u<<i;' in s
assert '#define CRAB_AURA_ATK 200' in s
assert '#define CRAB_AURA_DEF 800' in s
assert 'coin_battle.eligible[n]=coin_battle.fatal[n]&&ritual_card_effect_enabled(PERFORMANCE_ID)&&coin_identity' in (m/'performance_chaos.h').read_text()
text=(m/'text/crab_on.txt').read_text(); lines=text.splitlines();
assert text.count('<Ritual Effect>')==1
plain=' '.join(re.sub(r'\{[^}]+\}', '', text).split())
assert '+200 ATK and +800 DEF.' in plain
assert 'Under Umi, only' in plain
assert '{f8 0B 10}/{f8 0B 0C}/{f8 0B 0D}' in text
assert '{f8 0B 10}/{f8 0B 0C}/{f8 0B 0D}/{f8 0B 0F}' in text
assert all(x.strip() for x in lines[3:]), lines
fixture='''
#include <assert.h>
#include <stdio.h>
#define DUEL_SIDE_COUNT 2
#define DUEL_CARD_RECORD_COUNT 30
#define HAND_SIZE 5
#define DUEL_CARD_FLAG_OCCUPIED 0x8000
#define CARD_TYPE_FISH 12
#define CARD_TYPE_AQUA 1
#define CARD_TYPE_SEA_SERPENT 13
#define CARD_TYPE_THUNDER 9
#define CRAB_TURTLE_ID 710
#define FORTRESS_FISH_HAND_DEF 500u
#define FORTRESS_FISH_FIELD_DEF 1000u
#define UMI_TERRAIN 4
struct Rec {int card_id;int flags;void*data;int type;};
struct Mark {int ritual;void*data;};
struct Side {int hand[5];};
static struct Side D_800E9FF0[2];
static struct Rec D_801A7AD8[30];
static struct Mark crab_marks[30];
static unsigned char D_800907CC[10]={0,1,2,3,4,15,16,17,18,19};
static int gDuel_bTerrain=UMI_TERRAIN;
static int effect_enabled=1;
static int ritual_card_effect_enabled(int id){return effect_enabled && id==CRAB_TURTLE_ID;}
static int monster_slot(int i){return (i>=5&&i<10)||(i>=20&&i<25);}
static int side_for_record(int i){return i>=15?1:0;}
static int card_type_id(const struct Rec*r){return r->type;}
'''+count+'\n'+whale+'\n'+crab+r'''
int main(void){
    int i;
    for(i=0;i<5;++i){D_800E9FF0[0].hand[i]=-1;D_800E9FF0[1].hand[i]=-1;}
    D_801A7AD8[5]=(struct Rec){.card_id=718,.flags=0x8000,.type=CARD_TYPE_FISH,.data=(void*)1};
    D_801A7AD8[6]=(struct Rec){.card_id=400,.flags=0x8000,.type=CARD_TYPE_FISH,.data=(void*)2};
    D_801A7AD8[7]=(struct Rec){.card_id=401,.flags=0x8000,.type=CARD_TYPE_FISH,.data=(void*)3};
    D_801A7AD8[20]=(struct Rec){.card_id=402,.flags=0x8000,.type=CARD_TYPE_FISH,.data=(void*)4};
    D_800E9FF0[0].hand[0]=17;
    D_801A7AD8[0]=(struct Rec){.card_id=403,.flags=0x8000,.type=CARD_TYPE_FISH,.data=(void*)5};
    D_800E9FF0[0].hand[1]=-1;
    D_801A7AD8[1]=(struct Rec){.card_id=404,.flags=0x8000,.type=CARD_TYPE_FISH,.data=(void*)6};
    assert(whale_fish_defense_bonus(0,5)==2500); /* hand + two allied field Fish, not Whale source */
    assert(whale_fish_defense_bonus(1,5)==0); /* wrong controller */
    D_801A7AD8[7].flags=0; assert(whale_fish_defense_bonus(0,5)==1500);
    D_800E9FF0[0].hand[0]=-1;assert(whale_fish_defense_bonus(0,5)==1000);
    D_801A7AD8[5].type=0;assert(whale_fish_defense_bonus(0,5)==1000);
    /* protected Crab is on player's side, attacker's type determines outcome */
    D_801A7AD8[8]=(struct Rec){.card_id=710,.data=(void*)7,.flags=0,.type=CARD_TYPE_AQUA};
    crab_marks[8]=(struct Mark){.ritual=1,.data=(void*)7};
    D_801A7AD8[21]=(struct Rec){.card_id=380,.type=5,.data=(void*)8}; /* Ultimate Dragon: NOT exempt */
    assert(crab_battle_protected(8,21));
    D_801A7AD8[21].type=CARD_TYPE_FISH;assert(!crab_battle_protected(8,21));
    D_801A7AD8[21].type=CARD_TYPE_AQUA;assert(!crab_battle_protected(8,21));
    D_801A7AD8[21].type=CARD_TYPE_SEA_SERPENT;assert(!crab_battle_protected(8,21));
    D_801A7AD8[21].type=CARD_TYPE_THUNDER;assert(!crab_battle_protected(8,21));
    D_801A7AD8[21].type=5;gDuel_bTerrain=0;assert(!crab_battle_protected(8,21));
    gDuel_bTerrain=UMI_TERRAIN;effect_enabled=0;assert(!crab_battle_protected(8,21));
    effect_enabled=1;crab_marks[8].ritual=0;assert(!crab_battle_protected(8,21));
    crab_marks[8].ritual=1;D_801A7AD8[8].data=(void*)123;assert(!crab_battle_protected(8,21));
    puts("PASS Fortress hand +500/field +1000 only, Crab Umi/type/Ultimate/provenance, Performance preserved");
}
'''
with tempfile.TemporaryDirectory() as t:
 p=Path(t)/'test.c'; e=Path(t)/'test';p.write_text(fixture)
 subprocess.run(['gcc','-std=c11','-O2','-Wall','-Wextra','-Werror',str(p),'-o',str(e)],check=True)
 subprocess.run([str(e)],check=True)
print('PASS source wiring + compact description')
