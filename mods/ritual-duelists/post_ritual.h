#ifndef RD_POST_RITUAL_H
#define RD_POST_RITUAL_H
#include "planner.h"

/* Profile-defined Beacon policy: no opponent/card IDs are hardcoded here.
 * The existing profile evaluator scores the same board before/after a safe
 * actual hand->field play. The planner never applies an effect itself. */
typedef struct {
    int active;          /* the recipe's result currently exists on our field */
    int guardian_count;
    int hand_slot;       /* preferred REAL held support, or -1 */
    int destination;     /* 0..4 available monster zone, or -1 */
    int score;
    int attack_gain, defense_gain; /* prediction, not runtime stat edits */
    int face_up;         /* only if ATK offers substantially better survival */
    int protect_field[5];
} RdPostPlan;
RdPostPlan Rd_PlanPostRitual(const RdOpponentProfile *profile,
    const RdRitualRecipe *recipe,const RdBoard *board,int effect_mode);
/* Score an external candidate such as an already-existing unpurchased deck
 * card against a chosen, expendable hand slot. Nonpositive -> do not play. */
int Rd_PostSupportScore(const RdOpponentProfile *profile,
    const RdRitualRecipe *recipe,const RdBoard *board,int effect_mode,
    int held_slot,const RdCard *support,int destination,int *face_up);
#endif
