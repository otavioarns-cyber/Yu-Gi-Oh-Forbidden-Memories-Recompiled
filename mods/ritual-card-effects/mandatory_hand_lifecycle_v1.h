/* HAL25 EXPERIMENTAL companion; frozen selection ABI1 is not resized. */
#ifndef MANDATORY_HAND_LIFECYCLE_V1_H
#define MANDATORY_HAND_LIFECYCLE_V1_H
#include "mandatory_hand_selection_v1.h"
#define RHS_LIFECYCLE_MAGIC 0x5253484cu
#define RHS_LIFECYCLE_KEY "ritual-duelists:mandatory-hand-lifecycle-v1"
enum {RHS_STATE_IDLE=0,RHS_STATE_VALIDATED=1,RHS_STATE_RETRY=2,
 RHS_STATE_BLOCKED=3,RHS_STATE_QUARANTINE=4,RHS_STATE_EXHAUSTED=5,RHS_STATE_HANDOFF=6};
enum {RHS_CAUSE_NONE=0,RHS_CAUSE_VETO=1,RHS_CAUSE_SNAPSHOT=2,
 RHS_CAUSE_PROTOCOL=3,RHS_CAUSE_OWNER=4,RHS_CAUSE_REENTRANCY=5,
 RHS_CAUSE_COUNTER=6,RHS_CAUSE_PHYSICAL=7,RHS_CAUSE_LATE_WRITE=8,
 RHS_CAUSE_LIMIT=9,RHS_CAUSE_CONSUMER_OFF=10};
enum {RHS_RECOVER_NONE=0,RHS_RECOVER_EXPLICIT=1,RHS_RECOVER_DUEL_RESET=2,
 RHS_RECOVER_PROCESS_RESTART=3};
typedef struct {
 uint32_t abi,size,sequence,state,cause,last_cause,attempts,limit;
 uint32_t action,generation,connection_epoch,connected,busy,effective;
 uint32_t recovery,serial_exhausted;int32_t side,last_provider_status;
} RhsRecovery;
typedef struct {
 uint32_t abi,size,magic;
 int(*read)(RhsRecovery*,uint32_t);
 /* Static producer table/context/code must remain alive until success.
  * A busy disconnect returns0; it is NOT permission to free callbacks. */
 int(*disconnect)(const RhsRules*);
 int(*invalidate)(const RhsRules*,uint32_t cause);
 /* Current sequence required. No identity repair, never restores cards. */
 int(*recover)(uint32_t);
} RhsLifecycle;
#if defined(__i386__)
typedef char Rhs_frozen_ABI1_sizes[(sizeof(RhsRules)==12&&sizeof(RhsRendezvous)==24&&sizeof(RhsRequest)==4208&&sizeof(RhsResponse)==200)?1:-1];
typedef char Rhs_lifecycle_ABI1_sizes[(sizeof(RhsLifecycle)==28&&sizeof(RhsRecovery)==72)?1:-1];
#endif
#endif
