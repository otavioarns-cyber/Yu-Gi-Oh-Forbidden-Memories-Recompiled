#ifndef AUX_CARD_TERRAIN
#define AUX_CARD_TERRAIN(id) Duel_GetTerrainBoost(Cards_Type(id))
#endif
#ifndef FUSION_AUX_GENERATED_CARD_RESTORE_H
#define FUSION_AUX_GENERATED_CARD_RESTORE_H

/* Retail and 3D Monsters rebuild surviving fighters with func_80024D34.
   That routine treats the second argument as a REAL combined-deck index;
   synthetic effect summons have no such index. Calling it unchanged rebuilds
   the Zombie Warrior as whichever unrelated card is in that deck position.
   Preserve the original native teardown/texture setup, then restore the
   synthetic instance and its correct WA_MRG picture BEFORE the 3D caller
   reads the reconstructed record for its next field model. */
static void aux_restore_generated_card(s32 tagged_slot,s32 deck_index)
{
    int slot=(tagged_slot&0x80)?(tagged_slot&0x7F)+15:tagged_slot;
    void *lease=0;
    int id=0;
    s16 modifier=0,defense_modifier=0;
    u16 old_flags=0;
    DuelCardRecord *c;
    DuelCardDisplayObject *obj;
    if(slot>=0&&slot<30) {
        c=&D_801A7AD8[slot];
        if(c->data&&c->data==r2_zombie_lease[slot]&&c->card_id==30) {
            lease=c->data;id=30;
        }else if(c->data&&c->data==r3_summon_lease[slot]&&
                 c->card_id==r3_summon_id[slot]) {
            lease=c->data;id=r3_summon_id[slot];
        }
        if(lease) {
            modifier=c->stat_modifier;
            defense_modifier=c->defense_modifier;
            old_flags=c->flags;
        }
    }
    /* A generated card has no real deck index. Even if another renderer
       passed 0xFF from its synthetic record, NEVER feed it to the native
       80-entry combined deck table. Index 0 is a valid temporary source. */
    original_restore_generated_card(tagged_slot,lease?0:deck_index);
    if(!lease)return;
    c=&D_801A7AD8[slot];
    c->data=lease;
    ((DuelDeckCardRecord *)lease)->id=(s16)id;
    c->card_id=(s16)id;
    c->attack=(s16)r2_original_atk(id);
    c->defense=(s16)r2_original_def(id);
    c->stat_modifier=modifier;
    c->defense_modifier=defense_modifier;
    c->terrain_modifier=(s16)AUX_CARD_TERRAIN(id);
    c->flags=(u16)(DUEL_CARD_FLAG_OCCUPIED|
                   (old_flags&(DUEL_CARD_FLAG_DEFENSE_POSITION|
                               DUEL_CARD_FLAG_FACE_DOWN|
                               DUEL_CARD_FLAG_USED_THIS_TURN|
                               DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2)));
    c->table_index=(u8)slot;
    if(c->object)DisplayObject_ReleaseIfPresent(c->object);
    c->object=0;
    obj=func_80024C1C(id,D_800908A0[slot].x,D_800908A0[slot].y);
    c->object=obj;
    if(obj) {
        obj->card_index=(u8)slot;
        Duel_ApplyCardObjectFlags(obj);
    }
    aux_darkfire_card_thumbnail(slot,id);
}

#endif /* FUSION_AUX_GENERATED_CARD_RESTORE_H */
