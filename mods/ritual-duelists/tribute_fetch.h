#ifndef RD_TRIBUTE_FETCH_H
#define RD_TRIBUTE_FETCH_H
#include "adaptive_rules.h"
/* Native CPU deck access, not the Ritual engine's tribute-from-Deck source.
 * A FETCH_FIELD becomes an ordinary face-down monster play.
 * FETCH_HOLD changes an unselected expendable hand card while the AI plays
 * its original action, so the needed material remains in the physical hand.
 * Neither action creates a card, advances a draw, or changes the game rules. */
typedef enum {
    RD_FETCH_NONE=0,
    RD_FETCH_FIELD=1,
    RD_FETCH_HOLD=2
} RdFetchMode;
typedef struct {
    RdFetchMode mode;
    int source_slot;      /* 16..55, actual undrawn shuffled deck */
    int replace_slot;     /* 0..4, expendable actual physical hand */
    int destination;      /* FIELD: 1..5; HOLD: -1 */
    int card_id;          /* rechecked against deck records at materialize */
    int role;             /* 0..2, or -1 for required result monster */
} RdTributeFetch;
RdTributeFetch Rd_ChooseDeckTribute(const RdOpponentProfile *profile,
    const RdRitualRecipe *recipe,const RdAdaptiveRules *rules,
    const RdBoard *board,const RdPlan *plan,
    const RdCard remaining_deck[40],int deck_count,
    const unsigned char previous_selection[12],int ritual_legal,
    RdMaterialMatch matcher,void *context);
#endif
