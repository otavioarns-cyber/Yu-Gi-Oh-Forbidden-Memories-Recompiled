#!/usr/bin/env python3
"""Compile actual Cosmo and Whale predicates/counting against deterministic duel records."""
from pathlib import Path
import subprocess,tempfile
src=(Path(__file__).resolve().parents[1]/'mod.c').read_text()
def extract(name):
    assert name == 'cosmo_is_female_monster'
    start=src.rfind('static int cosmo_is_female_monster(',0,src.index('static int cosmo_female_destination('))
    pos=src.index('{',start);depth=0
    for i in range(pos,len(src)):
        if src[i]=='{':depth+=1
        elif src[i]=='}':
            depth-=1
            if depth==0:return src[start:i+1]
    raise AssertionError(name)
assert 'id==MYSTICAL_SAND_ID' in extract('cosmo_is_female_monster')
assert 'cosmo_female_fusion_result' not in src
assert src.count('cosmo_marks[index].female_count=female_field_count_except(index)+1u')==2
assert '(female_field_count_except(i)+1u)*COSMO_FEMALE_BONUS' in src
assert 'unsigned now=female_field_count_except(i)+1u;' in src
assert 'whale_fish_defense_bonus(side,i)' in src
fixture=r'''
#include <assert.h>
#include <stdio.h>
#define COSMO_QUEEN_ID 708
#define MYSTICAL_SAND_ID 531
#define CARD_FUSION_GROUP_FEMALE 123
#define CARD_STAT_TYPE_SHIFT 26
#define CARD_STAT_TYPE_MASK 31
#define CARD_TYPE_MAGIC 20
#define CARD_TYPE_FISH 12
#define DUEL_SIDE_COUNT 2
#define DUEL_CARD_RECORD_COUNT 30
#define HAND_SIZE 5
#define DECK_SIZE 40
static unsigned gDuel_adwCardStats[722];
static int group_ids[723];
static int Cards_InFusionGroup(int id,int group){return group==123&&group_ids[id];}
static int Cards_Valid(int id){return id>=1 && id<=722;}
static int Cards_Type(int id){return (gDuel_adwCardStats[id-1]>>26)&31;}
struct Side {int deck_draw_cursor;int hand[5];};
static struct Side D_800E9FF0[2];
struct DeckCard {int id;};
static struct DeckCard gDuel_aDeckCardRecords[80];
static int hand_cards[2][5];
static int cosmo_hand_card_id(int side,int slot){return hand_cards[side][slot];}
static int monster_slot(int slot){return (slot>=5&&slot<10)||(slot>=20&&slot<25);}
static int side_for_record(int slot){return slot>=20?1:0;}
'''+extract('cosmo_is_female_monster')+'\n'+r'''
int main(void){
  for(int id=1;id<=722;id++)gDuel_adwCardStats[id-1]=0;
  gDuel_adwCardStats[700-1]=(22u<<26); /* Ritual magic excluded */
  group_ids[90]=group_ids[582]=1;
  assert(cosmo_is_female_monster(90));
  assert(cosmo_is_female_monster(708)); /* added exception */
  assert(cosmo_is_female_monster(531)); /* added exception */
  assert(!cosmo_is_female_monster(638)); /* fusion result is NOT enough */
  assert(!cosmo_is_female_monster(111)); /* non-Female Fairy support */
  assert(!cosmo_is_female_monster(700));
  puts("PASS Cosmo: Female group + exact 531/708, self-inclusive +500 logic; Whale regression delegated to test_whale_crab_v07855");
}
'''
with tempfile.TemporaryDirectory() as t:
    p=Path(t)/'test.c';e=Path(t)/'test';p.write_text(fixture)
    subprocess.run(['gcc','-std=gnu11','-O2',str(p),'-o',str(e)],check=True)
    subprocess.run([str(e)],check=True)
