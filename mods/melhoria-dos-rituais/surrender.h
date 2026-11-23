#ifndef MELHORIA_DOS_RITUAIS_SURRENDER_H
#define MELHORIA_DOS_RITUAIS_SURRENDER_H

#include "pc/mods/modapi.h"

/* Reuse the retail two-player Select prompt for single-player surrender. */
int Surrender_Init(const MemoriesModHost *host);
void Surrender_Reset(void);

#endif
