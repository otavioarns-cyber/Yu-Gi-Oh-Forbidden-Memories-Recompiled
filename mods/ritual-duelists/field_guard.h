#ifndef RD_FIELD_GUARD_H
#define RD_FIELD_GUARD_H
#include "planner.h"
/* Only redirects a confirmed monster-hand play which would land on a
   strategically protected field tribute. Never intercepts Equip targeting,
   Ritual Magic, field actions, or nonphysical (logical) hand cards.
   Returns 1 when a safe destination is found, 0 when the original selection
   needs no correction, -1 when no unreserved monster zone exists. */
int Rd_FieldGuardDestination(const RdBoard *board,const RdPlan *plan,
                             const unsigned char selection[12]);
/* Protect existing Ritual results independently of Beacon callback state. */
int Rd_ProtectRitualResult(const RdBoard *board,RdPlan *guard,int result_id);
#endif
