#!/usr/bin/env python3
"""V4 source-backed contract checks; Windows gameplay is not emulated."""
from pathlib import Path
import re
P=Path(__file__).resolve().parents[1]
source=(P/'mod.c').read_text(); performance=(P/'performance_chaos.h').read_text(); garma=(P/'garma_visual.h').read_text()
assert 'if(coin_battle.eligible[n])performance_queue_reward(side);' in performance
assert 'performance_release_rewards();' in source
assert 'create_burn_card_in_slot(slot,id)' in source
assert 'card->flags = DUEL_CARD_FLAG_OCCUPIED | DUEL_CARD_FLAG_FACE_DOWN' in source
assert 'if((D_801A7AD8[slot].flags&DUEL_CARD_FLAG_OCCUPIED) || performance_reward_reserved[slot])continue;' in source
assert 'roll<50?340:roll<85?341:342' in source
# This is the exact 0..99 partition the source uses.
s={340:0,341:0,342:0}
for n in range(100):s[340 if n<50 else 341 if n<85 else 342]+=1
assert s=={340:50,341:35,342:15}
assert 'ritual_equip_token=host->subscribe(host,MEMORIES_EVENT_EQUIP' in source
assert 'if(source_found) {event->handled=1;event->result=1;}' in source
assert 'Cards_Type(event->a)!=CARD_TYPE_EQUIP' in source
assert 'Cards_Type(event->b)>=CARD_TYPE_MAGIC' in source
assert 'event->b==MASK_ID)return;' in source
assert 'target_found' not in source[source.index('static void mask_equip_permission(MemoriesModEvent *event)'):source.index('#include \"skull_monster.h\"')]
assert 'if(garma_award(a,d,' in source and 'garma_marks[a].strike_bonus+=200u;' in source
assert 'garma_marks[a].final_attack_cleanup=1;' in source
assert 'original_battle();\n    if(state11' in source
assert 'garma_marks[gi].strike_bonus=0;' in source
assert 'garma_marks[i].strike_bonus=0;' in source
assert 'garma_marks[attacker].blades_used' in garma
for name in ('performance_on','mask_on','garma_on'):
 d=(P/'text'/f'{name}.txt').read_text()
 assert d.count('<Ritual Effect>')==1 and '@bank descriptions' in d
assert '{f8 0A 03}+200' in (P/'text/garma_on.txt').read_text()
print('PASS V4 static contracts: dodge rewards and 50/35/15; face-down; Mask Equip aura; Garma sixth combat; descriptions')
