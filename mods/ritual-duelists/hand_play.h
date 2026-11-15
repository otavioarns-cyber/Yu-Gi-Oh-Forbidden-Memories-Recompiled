#ifndef RD_HAND_PLAY_H
#define RD_HAND_PLAY_H

/* A single physical-hand card, explicitly set face-down.  This is the
   12-byte selection consumed by DuelScene_UpdateHandActions; byte 8 is the
   CPU face-down flag (1), not an unused/reserved byte. */
void Rd_SelectFaceDownHandPlay(unsigned char selection[12],
                               int hand_slot, int target_slot);

#endif
