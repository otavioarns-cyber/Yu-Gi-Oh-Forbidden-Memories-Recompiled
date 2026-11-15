#ifndef RD_RECIPE_ADAPTER_H
#define RD_RECIPE_ADAPTER_H
#include "ritual_duelists.h"
#include "pc/mods/modapi.h"

/* Optional protocol for code-based recipe MODs; they remain authoritative.
   get() returns -1 if the ritual is not handled, or 0..capacity roles.
   matches() allows arbitrary custom conditions (if NULL, declarative rules
   will be matched). The provider never needs to reveal its ritual engine. */
#define RD_RECIPE_PROVIDER_ABI RITUAL_RECIPE_PROVIDER_ABI
typedef RitualRecipeProviderV1 RdRecipeProviderV1;

/* Single cache per duel-deck profile; rebuilt only on reset/settings restart. */
typedef struct {
    RdRitualRecipe recipe;
    RdRitualRequirement storage[RD_MAX_RITUAL_MATERIALS];
    const RdRecipeProviderV1 *provider;
    int source; /* 1 provider, 2 conditional, 3 exact, 4 disc, 5 manual, 6 v0.3 official extended */
    int material_sources; /* official: FIELD/HAND/BOTH; diagnostic only */
    int status; /* 1 known; 0 unknown */
} RdRecipeCache;
int Rd_ResolveRecipe(const MemoriesModHost *,const RdOpponentProfile *,int selection,
                    RdRecipeCache *out);
/* Name-entry-safe resolver: never inspects the duel-only retail scratch table. */
int Rd_ResolveStarterRecipe(const MemoriesModHost *,const RdOpponentProfile *,int,
                    RdRecipeCache *out);
int Rd_MatchRecipeCard(const RdOpponentProfile *,int,const RdRitualRequirement *,unsigned,void *);
#endif
