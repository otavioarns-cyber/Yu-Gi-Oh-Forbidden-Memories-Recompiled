/* HAL25 PROPOSED optional contract; no official Ritual provider implements it.
 * Producer owns effect rules. Never materialize/consume cards in this callback.
 * Only call synchronously after planning, before the swap/category gate. */
#ifndef HAL25_SELECTION_RULES_V1_H
#define HAL25_SELECTION_RULES_V1_H
#define HAL25_SELECTION_KEY "ritual-card-effects-test:selection-rules-v1"
enum { HAL25_RULE_UNAVAILABLE=0, HAL25_RULE_UNCHANGED=1,
       HAL25_RULE_REPLACED=2, HAL25_RULE_CANCELLED=3 };
typedef struct {unsigned abi; int (*constrain_hand)(int side);} Hal25SelectionRulesV1;
#endif
