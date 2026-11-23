#ifndef RD_STARTER_RULES_H
#define RD_STARTER_RULES_H
#include "ritual_duelists.h"
#include "recipe_adapter.h"
#include "pc/mods/modapi.h"
#define RD_STARTER_THEME_COUNT 3
const RdOpponentProfile *Rd_StarterThemeProfile(int theme);
int Rd_StarterSimonTheme(int player,int coin);
int Rd_BuildThemedStarter(const MemoriesModHost *host,int theme,int selection,
 const u16 vanilla[40],u16 out[40],RdRandom rng,void *ctx);
#endif
