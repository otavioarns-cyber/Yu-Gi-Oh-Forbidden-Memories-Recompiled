#ifndef RD_RITUAL_DUELISTS_H
#define RD_RITUAL_DUELISTS_H
#include "types.h"
#define RD_PROFILE_CAPACITY 30
#define RD_DECK_SIZE 40
#define RD_COPY_CAP 3
#define RD_PACKAGE_WEIGHT 30
#define RD_MAX_PACKAGES 3
#define RD_WINDOW 20
#define RD_ANY -1
/* Actual PC card id limit, not the 722-card retail catalog. */
#define RD_CARD_ID_MAX 0x7FFE
/* Declarative recipe limit, not a hardcoded three-tribute game rule. The
 * current field+hand planner sees at most 10 distinct physical monsters;
 * future providers may attach a larger candidate universe (e.g. deck). */
#define RD_MAX_RITUAL_MATERIALS 40

typedef struct { u16 id, weight; } RdWeightedCard;

typedef struct RdBoard RdBoard;
typedef struct RdOpponentProfile RdOpponentProfile;
/* The shared provider ABI defines the identical role layout for both MODs. */
#include "ritual_recipe_provider_v1.h"
typedef RitualRecipeRequirementV1 RdRitualRequirement;

/* A resolved recipe is independent of the duelist profile. In a later
 * integration, the recipe resolver replaces this descriptor once after
 * mods/tables load, without rebuilding the generic AI/planner. */
typedef struct {
    u16 ritual_magic, result_card;
    int result_attack, result_defense;
    const RdRitualRequirement *materials;
    unsigned material_count; /* 0..RD_MAX_RITUAL_MATERIALS, not fixed at 3 */
    /* 0 unknown/any, 1 only Field, 2 only Hand, 3 Field+Hand.
       The original recipe stays unknown here; its engine is authoritative. */
    unsigned material_sources;
} RdRitualRecipe;

typedef struct {int attack_bonus,defense_bonus,field_synergy,hand_synergy,strategy_bonus;} RdEffectEstimate;
typedef int (*RdGroupMatcher)(const RdOpponentProfile *,int card_id,int group_id);
/* `selected` is an array of `material_count` distinct physical slot indices:
 * 0..4 own field, 5..9 real hand. Not a hardcoded triple. */
typedef RdEffectEstimate (*RdEffectEvaluator)(const RdOpponentProfile *,int ritual_result,const RdBoard *,const int *selected,unsigned material_count,int rules_mode);
struct RdOpponentProfile {
    const char *duelist_name;
    const RdWeightedCard *pool;
    u16 pool_count;
    RdRitualRecipe recipe;
    const u16 *baseline_materials; /* preferred tribute IDs when active recipe permits */
    u8 baseline_material_count;
    const u16 *skull_ids;
    u8 skull_count;
    RdGroupMatcher in_group;
    RdEffectEvaluator evaluate_effect;
};
/* Version 2 appends pointer-free ritual provenance to the original prefix.
   These marks survive a save/load and are reset for each freshly dealt duel. */
#include "ai_policy.h"
#include "starter_bridge.h"
typedef struct {
    unsigned profile,mode,opp_id,seed_calls,generated,reserved;
    RdAiRitualMark ritual_marks[RD_AI_RECORDS];
    /* v3 extension: three starter themes and one permanent Simon choice. */
    int starter_theme,simon_theme,starter_ready;
    u16 simon_deck[RD_DECK_SIZE];
    RdStarterModal starter_modal;
    /* Pack 03: independent Blue-Eyes source flags, per save token. */
    unsigned blue_eye_source_bits, blue_eye_slot_token, blue_eye_result_pending;
} RdSaveState;

typedef int (*RdRandom)(int maximum, void *ctx);
typedef int (*RdMaterialMatch)(const RdOpponentProfile *,int card_id,
                                const RdRitualRequirement *,unsigned role,void *context);
/* Just adapt decks. The provider of the recipe handles actual summoning. */
typedef struct {
    int packages; /* 0..3 complete bundles (each exactly 4 real cards) */
    u16 included[RD_MAX_PACKAGES][4]; /* magic then three chosen materials */
    int error; /* 1 recipe invalid; 2 pool invalid; 3 RNG/deck failure */
} RdPackageStats;
/* mode 2=Story, 6=Free. Free guarantee only when the explicit setting is ON.
   The resolver/matcher is the same one that informs the actual Ritual AI. */
int Rd_GeneratePackageDeck(const RdOpponentProfile *profile,
    const RdRitualRecipe *active, RdMaterialMatch match,void *context,
    int catalog_max_id,int mode,int free_guarantee,
    u16 out[RD_DECK_SIZE],RdRandom rng,void *rng_context,
    RdPackageStats *stats);
int Rd_IsSkull(const RdOpponentProfile *profile, int id);
const RdOpponentProfile *Rd_GetProfile(int index);
int Rd_ProfileCount(void);
#include "ritual_effect_intelligence_v1.h"
void Rd_ConnectEffectIntelligence(const RitualEffectIntelligenceV1 *provider);
int Rd_EffectIntelligenceEnabled(int monster);
#endif
