#include <assert.h>
#include <stdio.h>
#include <string.h>
#define MASK_ID 720
#define DUEL_CARD_FLAG_OCCUPIED 0x8000u
#define CARD_TYPE_MAGIC 20
#define CARD_TYPE_EQUIP 23
#define MEMORIES_BEFORE 0
#define MEMORIES_AFTER 1
typedef struct {int card_id;void *data;unsigned flags;} DuelCardRecord;
typedef struct {int phase,handled,a,b,c,result;} MemoriesModEvent;
typedef struct {void *data;unsigned learned;int slot;} MaskInstance;
static DuelCardRecord D_801A7AD8[30];
static MaskInstance mask_instances[10];
static unsigned mask_instance_count,mask_instance_mask;
static int D_8009B1D5;
static int effect_on=1;
static int monster_slot(int idx){return (idx>=5&&idx<10)||(idx>=20&&idx<25);}
static int side_for_record(int idx){return idx>=15;}
static int take_mask_bit(unsigned *mask){int i=__builtin_ctz(*mask);*mask&=*mask-1;return i;}
static int ritual_card_effect_enabled(int id){return id==MASK_ID&&effect_on;}
static int Cards_Valid(int id){return id>0&&id<723;}
static int Cards_Type(int id){if(id==700)return CARD_TYPE_EQUIP;if(id==720||id==17)return 1;if(id==340)return CARD_TYPE_MAGIC;return id==999?CARD_TYPE_MAGIC:2;}
static int mask_slot_for(MaskInstance *m)
{
    int i, slot;
    if (!m || !m->data) return -1;
    /* The Ritual result already tells us its field record.  Keep that slot as
     * a hot-path cache and only fall back to the ten-zone identity search if
     * retail has rebuilt/moved the record.  The identity checks are identical
     * to the old scan, so the cache cannot make a stale instance look live. */
    slot=m->slot;
    if (monster_slot(slot) && D_801A7AD8[slot].data == m->data &&
        D_801A7AD8[slot].card_id == MASK_ID &&
        (D_801A7AD8[slot].flags & DUEL_CARD_FLAG_OCCUPIED)) return slot;
    for (i=5; i<10; ++i)
        if (D_801A7AD8[i].data == m->data && D_801A7AD8[i].card_id == MASK_ID &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED)) { m->slot=i; return i; }
    for (i=20; i<25; ++i)
        if (D_801A7AD8[i].data == m->data && D_801A7AD8[i].card_id == MASK_ID &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED)) { m->slot=i; return i; }
    m->slot=-1;
    return -1;
}
static void mask_equip_permission(MemoriesModEvent *event)
{
    int side=D_8009B1D5, source_found=0;
    unsigned pending;
    if(!event || event->phase!=MEMORIES_BEFORE || event->handled ||
       !mask_instance_count || !ritual_card_effect_enabled(MASK_ID) ||
       side<0 || side>1 || !Cards_Valid(event->a) || !Cards_Valid(event->b) ||
       Cards_Type(event->a)!=CARD_TYPE_EQUIP || Cards_Type(event->b)>=CARD_TYPE_MAGIC ||
       event->b==MASK_ID)return;
    /* Duel_CheckEquip supplies ONLY two card IDs. During card placement,
     * DuelScene_UpdateCardPlacement reads IDs from staged replay objects,
     * and the target's DuelCardRecord is frequently DEACTIVATED. Requiring
     * an occupied field record for event->b rejected these legal targets.
     * All native Equip checks for the acting player/CPU concern their own
     * selected materials, so the active side identifies the target owner. */
    pending=mask_instance_mask;
    while(pending) {
        int i=take_mask_bit(&pending),slot=mask_slot_for(&mask_instances[i]);
        if(slot>=0 && side_for_record(slot)==side) {source_found=1;break;}
    }
    /* No target instance ID is exposed by this event: exclude Mask card ID
     * entirely, including a second Mask. Never transform two monsters (or
     * spells) into an Equip, and never override native Equip bonuses. */
    if(source_found) {event->handled=1;event->result=1;}
}
static void reset(int mask_side){
  memset(D_801A7AD8,0,sizeof D_801A7AD8);
  memset(mask_instances,0,sizeof mask_instances);
  mask_instance_count=1;mask_instance_mask=1;effect_on=1; D_8009B1D5=mask_side;
  int idx=mask_side?20:5;
  D_801A7AD8[idx].card_id=MASK_ID;
  D_801A7AD8[idx].data=(void *)0x1234;
  D_801A7AD8[idx].flags=DUEL_CARD_FLAG_OCCUPIED;
  mask_instances[0].data=(void *)0x1234;
  mask_instances[0].slot=idx;
}
static int check(int source,int target,int phase,int already){
  MemoriesModEvent event={0};event.phase=phase;event.a=source;event.b=target;event.handled=already;
  mask_equip_permission(&event);
  return event.handled && event.result==1;
}
int main(void){
  reset(0);
  /* The actual reported failure: target not occupied during staging. */
  assert(check(700,17,MEMORIES_BEFORE,0));
  D_801A7AD8[6].card_id=17;D_801A7AD8[6].flags=DUEL_CARD_FLAG_OCCUPIED;
  assert(check(700,17,MEMORIES_BEFORE,0));
  reset(1);assert(check(700,17,MEMORIES_BEFORE,0));
  reset(0);assert(!check(17,17,MEMORIES_BEFORE,0)); /* monster cannot equip */
  assert(!check(700,340,MEMORIES_BEFORE,0)); /* magic is no target */
  assert(!check(340,17,MEMORIES_BEFORE,0)); /* magic is no equip */
  assert(!check(700,720,MEMORIES_BEFORE,0)); /* Mask never self-equips */
  assert(!check(700,17,MEMORIES_AFTER,0));
  assert(!check(700,17,MEMORIES_BEFORE,1));
  assert(!check(0,17,MEMORIES_BEFORE,0));
  effect_on=0;assert(!check(700,17,MEMORIES_BEFORE,0));
  effect_on=1;mask_instance_count=0;assert(!check(700,17,MEMORIES_BEFORE,0));
  reset(0);D_8009B1D5=1;assert(!check(700,17,MEMORIES_BEFORE,0));
  reset(0);D_801A7AD8[5].flags=0;assert(!check(700,17,MEMORIES_BEFORE,0));
  printf("PASS: 14 source-backed Mask Equip cases (staging, both sides, OFF, source removal, self, types)\n");
  return 0;
}
