#include "hand_play.h"
#include <string.h>

void Rd_SelectFaceDownHandPlay(unsigned char selection[12],
                               int hand_slot, int target_slot)
{
    memset(selection, 0, 12);
    selection[0] = (unsigned char)(hand_slot + 11);
    selection[6] = (unsigned char)target_slot;
    selection[8] = 1; /* Always SET, not face-up (engine may override for card effects). */
}
