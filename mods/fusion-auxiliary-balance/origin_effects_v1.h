/* Optional extension contract. Register an active-effect predicate, not a
 * list of all monsters. The owner must implement its effect and honor lease().
 * No callback may mutate gameplay state. Unregister before unloading code. */
#ifndef ORIGIN_EFFECTS_V1_H
#define ORIGIN_EFFECTS_V1_H
#define ORIGIN_EFFECTS_V1_KEY "fusion-auxiliary-balance:origin-effects-v1"
typedef struct OriginEffectsV1 {
    unsigned abi;
    /* Stable owner key <=63 bytes. Replaces same owner; NULL unregisters.
     * Returns 0 for invalid input or all 16 extension slots in use. */
    int (*register_provider)(const char *owner, int (*enabled)(int card_id));
    /* Serial for this physical field monster; 0 means no borrowed origin.
     * Use as an additional condition, never rewrite real summon provenance. */
    unsigned (*lease)(int field_slot);
} OriginEffectsV1;
#endif
