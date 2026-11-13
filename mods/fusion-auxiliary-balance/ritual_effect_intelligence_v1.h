/* Independent optional contract: Ritual Effects OWNS the meaning of its
 * effects. Ritual Duelists uses this data for planning, never casts an
 * effect itself. Copy this header unchanged into both projects. */
#ifndef RITUAL_EFFECT_INTELLIGENCE_V1_H
#define RITUAL_EFFECT_INTELLIGENCE_V1_H
#define RITUAL_EFFECT_INTELLIGENCE_ABI 1u
#define RITUAL_EFFECT_FIELD_SLOTS 5u
typedef struct RitualEffectAiBoardV1 {
    int own[5], enemy[5], hand[5];
    unsigned char own_hidden[5], enemy_hidden[5];
    unsigned char selected[10]; /* physical tributes: field 0..4, hand 5..9 */
    int hand_count;
    int own_lp, enemy_lp;
} RitualEffectAiBoardV1;
typedef struct RitualEffectAiEstimateV1 {
    int attack_bonus, defense_bonus; /* predicted ATK/DEF, not utility */
    int field_synergy, hand_synergy; /* diagnostic counts */
    int strategic_value;            /* utility for planning, not real ATK */
} RitualEffectAiEstimateV1;
typedef struct RitualEffectIntelligenceV1 {
    unsigned abi;
    /* 1 only if PROVIDER MOD is currently active and effect setting is ON.
     * 0 if disabled, unavailable, or a non-effect result. */
    int (*enabled)(int ritual_monster);
    /* Pure read-only forecast for this exact hypothetical selection.
     * Does not run a ritual, update AI memory or change card states. */
    int (*estimate)(int ritual_monster, const RitualEffectAiBoardV1 *board,
                    RitualEffectAiEstimateV1 *out);
} RitualEffectIntelligenceV1;
#endif
