#ifndef RD_DECK_ACCESS_H
#define RD_DECK_ACCESS_H
#include "planner.h"
#include "game/duel_deck_card.h"
/* The vanilla CPU snapshots its physical hand followed by the as-yet-undrawn
   deck. The 16..55 selection range is materialized by func_8001BAF0; this
   helper NEVER invents a card and never uses a consumed/drawn deck record. */
typedef struct {
    int source_slot;   /* AI active-card slot 16..55, or -1 */
    int replace_slot;  /* real hand slot 0..4 (unprotected), or -1 */
    int target_slot;   /* free back row 6..10, or -1 */
} RdDeckPick;
RdDeckPick Rd_ChooseDeckRitual(const RdBoard *board, const RdPlan *plan,
                               int magic, const int deck_ids[40],
                               int deck_remaining);
/* Perform the vanilla paired record/index exchange without creating a card. */
/* The legacy AI can examine at most 20 entries (hand + upcoming Deck). */
int Rd_DefaultAiWindow(int physical, int remaining);
void Rd_ExchangeDeckRecords(DuelDeckCardRecord *held, DuelDeckCardRecord *upcoming);
#endif
