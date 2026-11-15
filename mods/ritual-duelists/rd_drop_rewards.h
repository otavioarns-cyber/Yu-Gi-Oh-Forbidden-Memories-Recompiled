#ifndef RD_DROP_REWARDS_H
#define RD_DROP_REWARDS_H
#include "pc/mods/modapi.h"
/* Each bit is Seto / Kaiba / Seto 2nd. Carried by the existing state
 * buffer, and stored next to each normal save under the slot's token. */
int Rd_DropRewardsInit(const MemoriesModHost *host, unsigned *consumed,
                       unsigned *token,unsigned *pending);
void Rd_DropRewardsNewCampaign(void);
void Rd_DropRewardsNewDuel(void);
void Rd_DropRewardsReset(void);
int Rd_BlueEyesSpent(int duelist);
#endif
