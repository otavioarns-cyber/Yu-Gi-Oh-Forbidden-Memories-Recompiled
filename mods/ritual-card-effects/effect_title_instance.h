#ifndef FMR_EFFECT_TITLE_INSTANCE_H
#define FMR_EFFECT_TITLE_INSTANCE_H
/* Source-of-truth for title eligibility. Never identify a duel card by
 * searching for the first matching card ID: the same monster can exist
 * simultaneously in the hand AND on the field, with different origins.
 * Hand card viewer opens directly from the ID (no field cursor). The
 * field viewer uses the concrete selection grid record. */
/* Main_RunDuel reuses MAIN_MODE_DUEL (3) for TWO different screens:
 * D_8009B26E == 0x80 : Build Deck / pre-duel chest, game NOT started.
 * D_8009B26E == 0x81 : actual DuelScene, with per-card origin ledgers.
 * Checking only MAIN_MODE_DUEL made pre-duel previews appear disabled.
 * Match the runner's initialized scene-step instead of stale scene flags. */
static int fmr_title_is_live_duel(void)
{
    return (D_8009B26C[0]&31)==MAIN_MODE_DUEL &&
           (D_8009B26E&0x8f)==0x81;
}
static int fmr_title_is_field_view(int mode,int scene_flags)
{
    int phase=scene_flags&DUEL_SCENE_PHASE_MASK;
    return mode==MAIN_MODE_DUEL && fmr_title_is_live_duel() &&
        (phase==5 || (phase==4 && (scene_flags&0x4000)));
}
static int fmr_title_selected_field_record(int id)
{
    int side,phase,index;
    DuelCardPickCursor *cursor=D_8009B1B4;
    DuelSelectionSide *scene_sides=(DuelSelectionSide *)D_800E9F10;
    int flags=(int)gDuel_wSceneStateFlags;
    if(!fmr_title_is_field_view(D_8009B26C[0]&31,flags) || !cursor)return -1;
    side=(int)D_8009B1D5;
    if(side<0 || side>1)return -1;
    phase=flags&DUEL_SCENE_PHASE_MASK;
    if(phase==4){
        /* Hand phase can inspect the field via L2/R2, but only with the
         * explicit field cursor in record 1. Normal hand inspection is
         * NEVER allowed to borrow that field card's activation. */
        if(cursor!=(DuelCardPickCursor *)&scene_sides[side].records[1])return -1;
    }else if(cursor!=(DuelCardPickCursor *)&scene_sides[side].records[2] &&
             cursor!=(DuelCardPickCursor *)&scene_sides[side].records[3])return -1;
    if(cursor->row<0 || cursor->row>=4 || cursor->col<0 || cursor->col>=5)return -1;
    index=(int)D_800907D8[side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+
                           cursor->row*DUEL_FIELD_ROW_SIZE+cursor->col];
    if(index<0 || index>=DUEL_CARD_RECORD_COUNT ||
       !(D_801A7AD8[index].flags&DUEL_CARD_FLAG_OCCUPIED) ||
       D_801A7AD8[index].card_id!=id)return -1;
    return index;
}
#endif
