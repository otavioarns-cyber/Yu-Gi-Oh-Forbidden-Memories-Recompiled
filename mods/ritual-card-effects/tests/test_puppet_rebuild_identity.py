"""Production CPU-side duel-card rebuild preserves original monster ID and stats."""
from pathlib import Path
import subprocess,tempfile
s=(Path(__file__).resolve().parents[1]/'mod.c').read_text()
a=s.index('static void puppet_safe_rebuild(s32 slot,s32 deck_index)\n{')
b=s.index('static int move_monster_record',a)
segment=s[a:b]
code=r'''
#include <stdio.h>
#include <assert.h>
#define DUEL_CARD_RECORD_COUNT 30
#define COMBINED_DECK_SIZE 80
#define s32 int
#define s16 short
typedef struct {int id,deck_index,data_block_index;} DuelDeckCardRecord;
typedef struct {void*data;short card_id;short attack,defense;} DuelCardRecord;
typedef struct{int ritual,controlling,current_slot,stolen_card_id;void*stolen_data;DuelCardRecord stolen_snapshot;} PsychoMark;
typedef struct{void(*log)(void*,char const*,...);} Host;
static Host *host=0;
static PsychoMark psycho_marks[30];static unsigned psycho_controlling_mask;
static DuelDeckCardRecord gDuel_aDeckCardRecords[80];
static DuelCardRecord D_801A7AD8[30];
static int visual_refreshes,seen_index,seen_slot;
static int take_mask_bit(unsigned *m){int i=0;while(!(*m&(1u<<i)))++i;*m&=~(1u<<i);return i;}
static int monster_slot(int i){return (i>=5&&i<10)||(i>=20&&i<25);}
static void refresh_moved_monster_visual(int i){(void)i;++visual_refreshes;}
static void native_rebuild(s32 slot,s32 b){seen_index=b;seen_slot=slot;D_801A7AD8[slot].data=&gDuel_aDeckCardRecords[b];D_801A7AD8[slot].card_id=(short)gDuel_aDeckCardRecords[b].id;D_801A7AD8[slot].attack=100;D_801A7AD8[slot].defense=200;}
static void (*original_rebuild_field_card)(s32,s32)=native_rebuild;
''' + segment + r'''
int main(void){
 psycho_controlling_mask=1;
 psycho_marks[0].ritual=psycho_marks[0].controlling=1;
 psycho_marks[0].current_slot=21;
 gDuel_aDeckCardRecords[55].id=710;gDuel_aDeckCardRecords[55].deck_index=55;
 psycho_marks[0].stolen_card_id=710;
 psycho_marks[0].stolen_data=&gDuel_aDeckCardRecords[55];
 psycho_marks[0].stolen_snapshot.attack=2500;
 psycho_marks[0].stolen_snapshot.defense=2300;
 puppet_safe_rebuild(21,0);
 assert(seen_index==55&&seen_slot==21&&D_801A7AD8[21].card_id==710);
 /* When captured monster is a fusion, deck's base card can differ. */
 psycho_marks[0].stolen_card_id=500;
 puppet_safe_rebuild(21,0);
 assert(D_801A7AD8[21].card_id==500&&D_801A7AD8[21].attack==2500&&visual_refreshes==1);
 puppet_safe_rebuild(20,0);assert(seen_index==0); /* unrelated card uses original */
 puts("PASS: CPU borrowed-card reconstruction uses original combined Deck index and original identity");
}
'''
with tempfile.TemporaryDirectory() as t:
 p=Path(t)/'h.c';p.write_text(code)
 subprocess.run(['gcc','-std=c99','-Wall',str(p),'-o',t+'/run'],check=True)
 subprocess.run([t+'/run'],check=True)
