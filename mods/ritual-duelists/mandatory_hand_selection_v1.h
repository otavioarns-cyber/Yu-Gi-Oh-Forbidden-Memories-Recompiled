/* HAL25 EXPERIMENTAL ABI1. Process-local synchronous hand-selection contract.
 * No serialization, materialization, card consumption, or strategic estimates.
 * Copies in both MODs must be byte-identical. See notes/hal-25-contract/CONTRATO.md.
 */
#ifndef MANDATORY_HAND_SELECTION_V1_H
#define MANDATORY_HAND_SELECTION_V1_H
#include <stdint.h>
#define RHS_ABI 1u
#define RHS_RENDEZVOUS "ritual-duelists:mandatory-hand-selection-v1"
enum {RHS_INVALID=-1,RHS_UNAVAILABLE=0,RHS_UNCHANGED=1,RHS_REPLACED=2,RHS_CANCELLED=3};
enum {RHS_PROPOSE=0,RHS_CANCEL_INTENT=1};
typedef struct {
 int32_t slot,record,index,id;
 uint32_t data,object,flags;
} RhsPhysical;
typedef struct {
 uint32_t size,action,generation;
 int32_t side,phase,turn[2],draw[2];
 uint8_t selection[12],deck[80*6];
 uint32_t stats[722];
 RhsPhysical hand[5],field[10];
 int32_t logical_id[45],logical_index[45];
} RhsSnapshot;
typedef struct {uint32_t abi,size,intent;RhsSnapshot state;} RhsRequest;
typedef struct {
 uint32_t abi,size,action,generation,revision[3];
 int32_t status,mandatory,side,destination,count;
 uint8_t selection[12];RhsPhysical selected[5];
} RhsResponse;
typedef struct {
 uint32_t abi,size;
 int (*evaluate)(const RhsRequest*,RhsResponse*);
} RhsRules;
typedef struct {
 uint32_t abi,size;
 int (*connect)(const RhsRules*);
 const RhsRules*(*rules)(void);
 /* 0: no consumer hand transaction; 1: still valid; -1: cancelled/stale.
  * A negative response forbids applying an override or delegating a swap. */
 int (*precommit)(void);
 /* Read-only transaction outcome: 2 retry scheduled; -2 quarantined;
  * -1 vetoed before native advance; 0 idle; 1 approved. */
 int (*outcome)(void);
} RhsRendezvous;
#endif
