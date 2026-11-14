#ifndef RD_ADAPTIVE_RULES_H
#define RD_ADAPTIVE_RULES_H
#include "ritual_duelists.h"
#include "ritual_rules_provider_v1.h"
#include "planner.h"
/* Configuration belongs to the rule mod, recipe to the current opponent.
   Consumers require neither the rule mod nor any specific Ritual. */
typedef struct {
    int available; /* 0 = unknown: leave the existing planner unchanged */
    unsigned material_count;
    unsigned sources;
    int min_field, min_hand, require_result;
} RdAdaptiveRules;
struct MemoriesModHost;
void Rd_ReadAdaptiveRules(const struct MemoriesModHost *host, RdAdaptiveRules *out);
unsigned Rd_AdaptiveRuleKey(const RdAdaptiveRules *rules);
void Rd_RefineRitualPlan(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
                         const RdBoard *board,int engine_legal,
                         RdMaterialMatch match,void *context,
                         const RdAdaptiveRules *rules,RdPlan *plan);
#endif
