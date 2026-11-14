/* Standalone regression for the real field-viewer routing contract.
   This mocks the RECOMP's concrete scene cursor, NOT card-id searches. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define DUEL_SCENE_PHASE_MASK 0xF
#define MAIN_MODE_DUEL 3
#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_FIELD_SIDE_GRID_SLOT_COUNT 20
#define DUEL_FIELD_ROW_SIZE 5
#define DUEL_CARD_FLAG_OCCUPIED 0x80000000u
typedef struct {int card_id;unsigned flags;} Card;
Card D_801A7AD8[30];
typedef struct {signed char col,row;} DuelCardPickCursor;
typedef struct {DuelCardPickCursor records[4];} DuelSelectionSide;
DuelSelectionSide sides[2];
#define D_800E9F10 ((unsigned char *)sides)
DuelCardPickCursor *D_8009B1B4;
unsigned char D_8009B1D5;
unsigned short gDuel_wSceneStateFlags;
unsigned char D_8009B26C[1];
unsigned char D_8009B26E;
unsigned char D_800907D8[40];
#include "../effect_title_instance.h"
int main(void)
{
    int id=380;
    memset(D_801A7AD8,0,sizeof(D_801A7AD8));
    memset(D_800907D8,0,sizeof(D_800907D8));
    /* Two identical Blue-Eyes Ultimates: hand record 0 and field record 5. */
    D_801A7AD8[0].card_id=id;
    D_801A7AD8[0].flags=DUEL_CARD_FLAG_OCCUPIED;
    D_801A7AD8[5].card_id=id;
    D_801A7AD8[5].flags=DUEL_CARD_FLAG_OCCUPIED;
    D_800907D8[5]=5;
    sides[0].records[2].row=1;
    sides[0].records[2].col=0;
    D_8009B1D5=0;
    D_8009B26C[0]=MAIN_MODE_DUEL;
    /* The duel's pre-battle deck editor runs under SAME MAIN_MODE_DUEL
       as the battle. Step 0x80 must ALWAYS use colorful previews. */
    D_8009B26E=0x80;
    assert(!fmr_title_is_live_duel());
    gDuel_wSceneStateFlags=5; /* stale battle scene from previous duel */
    D_8009B1B4=&sides[0].records[2];
    assert(!fmr_title_is_field_view(3,5));
    assert(fmr_title_selected_field_record(id)==-1);
    D_8009B26E=0; /* setup, before the editor is initialized */
    assert(!fmr_title_is_live_duel());
    D_8009B26E=0x81;
    assert(fmr_title_is_live_duel());
    gDuel_wSceneStateFlags=4;
    D_8009B1B4=&sides[0].records[0];
    assert(!fmr_title_is_field_view(3,4));
    assert(fmr_title_selected_field_record(id)==-1); /* Hand NEVER borrows field. */
    gDuel_wSceneStateFlags=5;
    D_8009B1B4=&sides[0].records[2];
    assert(fmr_title_selected_field_record(id)==5); /* Exact field copy. */
    /* Two copies on field still resolve by cursor, not the first match. */
    D_801A7AD8[6]=D_801A7AD8[5];
    D_800907D8[6]=6;
    sides[0].records[2].col=1;
    assert(fmr_title_selected_field_record(id)==6);
    sides[0].records[2].col=0;
    /* Off-duel: leftover duel scene and field cursor cannot activate title. */
    D_8009B26C[0]=7;
    assert(!fmr_title_is_live_duel());
    assert(fmr_title_selected_field_record(id)==-1);
    D_8009B26C[0]=3;
    gDuel_wSceneStateFlags=4;
    D_8009B1B4=&sides[0].records[1];
    sides[0].records[1].row=1;
    sides[0].records[1].col=0;
    assert(fmr_title_selected_field_record(id)==-1);
    gDuel_wSceneStateFlags=4|0x4000;
    assert(fmr_title_selected_field_record(id)==5); /* Explicit field inspection from hand phase. */
    D_8009B1B4=&sides[0].records[0];
    assert(fmr_title_selected_field_record(id)==-1);
    D_8009B1B4=&sides[0].records[1];
    D_801A7AD8[5].card_id=99;
    assert(fmr_title_selected_field_record(id)==-1); /* Never accept stale slot. */
    puts("PASS: per-instance hand/field selection and off-duel isolation");
    return 0;
}
