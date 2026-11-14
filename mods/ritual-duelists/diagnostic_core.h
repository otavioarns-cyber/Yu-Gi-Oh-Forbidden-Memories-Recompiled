#ifndef RD_DIAGNOSTIC_CORE_H
#define RD_DIAGNOSTIC_CORE_H
#include "planner.h"
/* Read-only classification; logical window cards are NOT physical hand. */
typedef struct {
    int hand_slot;   /* 0..actual_hand-1, or -1 */
    int back_slot;   /* 0..4, usable magic/trap slot, or -1 */
    int window_slot; /* actual_hand..hand_count-1, or -1 */
} RdMagicAccess;

typedef enum {
    RD_WHY_NONE=0,
    RD_WHY_NO_RECIPE,
    RD_WHY_MAGIC_UNSEEN,
    RD_WHY_LOGICAL_ONLY,
    RD_WHY_ENGINE_REJECTED,
    RD_WHY_SCORE_LOW,
    RD_WHY_NO_TRIGGER,
    RD_WHY_PREVIOUS_RITUAL,
    RD_WHY_PREVIOUS_LETHAL,
    RD_WHY_SET_REQUEST,
    RD_WHY_ACTIVATE_REQUEST,
    RD_WHY_PRESERVE_MATERIAL,
    RD_WHY_PREVIOUS_KEPT,
    RD_WHY_EXTERNAL_MATERIALS,
    RD_WHY_TRIBUTE_SETUP,
    RD_WHY_FIELD_GUARD,
    RD_WHY_FIELD_NO_SAFE,
    RD_WHY_DECK_TRIBUTE_FIELD,
    RD_WHY_DECK_TRIBUTE_HAND,
    RD_WHY_DECK_MAGIC_PULL,
    RD_WHY_POST_RITUAL_SUPPORT,
    RD_WHY_DECK_POST_RITUAL_SUPPORT
} RdDecisionWhy;

typedef enum {
    RD_ORIGIN_NONE=0, RD_ORIGIN_CHAIN=1, RD_ORIGIN_DUELISTS=2
} RdDecisionOrigin;

void Rd_FindRitualMagic(const RdBoard *board,int magic,RdMagicAccess *out);
const char *Rd_WhyText(RdDecisionWhy why);
const char *Rd_OriginText(RdDecisionOrigin origin);
#endif
