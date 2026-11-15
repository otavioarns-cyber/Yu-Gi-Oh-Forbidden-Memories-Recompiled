/* Shared, standalone, read-only ABI between ritual recipe providers and
 * consumers. Both mods compile this exact header independently.
 * A provider's pointers belong to its loaded module for the process life. */
#ifndef RITUAL_RECIPE_PROVIDER_V1_H
#define RITUAL_RECIPE_PROVIDER_V1_H
#define RITUAL_RECIPE_PROVIDER_ABI 1u
typedef struct {
    int exact_card;       /* 0: any card */
    int type;             /* -1: any monster type */
    int custom_group;     /* 0: none; producer's matches() is authoritative */
    int min_attack, max_attack, min_defense, max_defense; /* -1: no bound */
    int min_level, max_level; /* -1: no bound */
    int defense_gt_attack;
} RitualRecipeRequirementV1;
typedef struct {
    unsigned abi;
    /* -1: this provider has no active rule for magic; >=0: role count.
     * Never mutate the game or scan the duel on this call. */
    int (*get)(int ritual_magic, RitualRecipeRequirementV1 *out,
               unsigned capacity, int *result_card);
    /* Callback for special/private classes (skulls, exclusions, alternatives).
     * Must be a pure predicate on card properties and the active recipe. */
    int (*matches)(int ritual_magic, unsigned role, int card_id);
} RitualRecipeProviderV1;
#endif
