/* Real production selector and consumption helpers, synthetic engine state.
 * Build as i386; unused rendering/engine paths are discarded by the linker. */
#include <assert.h>
#include <stdio.h>
#include "../rules.c"
DuelCardRecord D_801A7AD8[DUEL_CARD_RECORD_COUNT];
DuelSideState D_800E9FF0[2];
DuelDeckCardRecord gDuel_aDeckCardRecords[80];
s32 gDuel_adwCardStats[CARD_TABLE_COUNT];
u8 D_800907CC[10];
DuelHandSlot D_800EA030[5];
static int destroyed[30], releases;
void DisplayObject_ReleaseIfPresent(void *p) {(void)p; releases++;}
void Surrender_Reset(void) {}
static void deactivate(DuelCardRecord *p) {destroyed[p-D_801A7AD8]++;p->flags=0;}
static DisplayObject objects[5];
static void setup(void) {
 memset(D_801A7AD8,0,sizeof(D_801A7AD8));memset(D_800E9FF0,0,sizeof(D_800E9FF0));
 memset(D_800EA030,0,sizeof(D_800EA030));memset(destroyed,0,sizeof(destroyed));
 for(int i=0;i<10;i++)D_800907CC[i]=i<5?i:15+i-5;
 for(int s=0;s<2;s++)for(int i=0;i<5;i++)D_800E9FF0[s].hand[i]=-1;
 for(int i=0;i<80;i++){gDuel_aDeckCardRecords[i].id=99;gDuel_aDeckCardRecords[i].data_block_index=i;}
 side=0;setting_min_field=1;setting_use_deck=0;setting_min_hand=0;setting_require_result=0;
 reset_result_source();recipe_result=500;o_deact=deactivate;base_id=0;card_type=0;card_level=0;in_group=0;
 for(int i=0;i<3;i++)matid[i]=10+i;
 releases=0;
}
static void field(int slot,int id){int r=5+slot;D_801A7AD8[r].card_id=id;D_801A7AD8[r].flags=DUEL_CARD_FLAG_OCCUPIED;D_801A7AD8[r].object=&objects[slot];}
static void hand(int slot,int id){D_800E9FF0[0].hand[slot]=slot;gDuel_aDeckCardRecords[slot].id=id;D_801A7AD8[slot].card_id=id;D_801A7AD8[slot].flags=DUEL_CARD_FLAG_OCCUPIED;}
int main(void){
 TablesRitualRequirement q[3];memset(q,0,sizeof(q));
 /* Mixed sources and an unselected identical copy: consume physical slots. */
 setup();field(0,10);hand(0,11);hand(1,12);hand(2,12);
 assert(select_candidates(q,0));assert(where[0]==RITUAL_SRC_FIELD&&where[1]==RITUAL_SRC_HAND&&where[2]==RITUAL_SRC_HAND);
 assert(slot[2]==1);for(int i=0;i<3;i++)consume_selected_source(i);
 assert(destroyed[5]==1);assert(D_800E9FF0[0].hand[0]==-1&&D_800E9FF0[0].hand[1]==-1);
 assert(D_800E9FF0[0].hand[2]==2&&D_801A7AD8[2].card_id==12&&D_801A7AD8[2].flags);
 /* Three field tributes, Vanilla placement, preserve an unrelated monster. */
 setup();setting_min_field=3;field(0,10);field(1,11);field(2,12);field(3,77);
 assert(select_candidates(q,0));for(int i=0;i<3;i++){assert(where[i]==RITUAL_SRC_FIELD);consume_selected_source(i);}
 assert(destroyed[5]&&destroyed[6]&&destroyed[7]&&!destroyed[8]&&D_801A7AD8[8].flags);
 /* Required repeated ID needs distinct physical copies, no alias reuse. */
 setup();matid[0]=matid[1]=matid[2]=10;field(0,10);hand(0,10);assert(!select_candidates(q,0));
 hand(1,10);assert(select_candidates(q,0));assert(rec[0]!=rec[1]&&rec[0]!=rec[2]&&rec[1]!=rec[2]);
 /* Minimum field constraint and legitimate field-first selection. */
 setup();hand(0,10);hand(1,11);hand(2,12);assert(!select_candidates(q,0));
 setting_min_field=0;assert(select_candidates(q,0));field(0,10);assert(select_candidates(q,0));assert(where[0]==RITUAL_SRC_FIELD);
 /* Reset on disable and re-enable clears stale bridge references. */
 bridge_mode=BRIDGE_EXTENDED;active_result=500;cached_tribute_objects[0]=&objects[0];
 applied(0);assert(bridge_mode==BRIDGE_NONE&&!active_result&&!cached_tribute_objects[0]);
 applied(1);assert(bridge_mode==BRIDGE_NONE&&!active_result);
 puts("PASS production selector/consumption: mixed sources, Vanilla placement, duplicate identity, minima, reset OFF/ON");
}
