#ifndef RD_STARTER_BRIDGE_H
#define RD_STARTER_BRIDGE_H
#include "types.h"
#include "pc/mods/modapi.h"
/* This modal lives in the same pointer-free state as Ritual Duelists. */
typedef struct {
    int active,selected,confirmed,armed,release_frames,unlocked,code_index;
    int prompt_open,prompt_yes;
    unsigned short previous;
} RdStarterModal;
RdStarterModal *Rd_StarterModal(void);
void Rd_StarterBegin(void);
int Rd_StarterAccept(int theme,const u16 vanilla[40],u16 result[40]);
int Rd_StarterUIInit(const MemoriesModHost*,MemoriesMod*);
void Rd_StarterUIReset(void);
#endif
