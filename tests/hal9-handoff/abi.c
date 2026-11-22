/* Target ABI assertion, not a model of game behavior. */
#include <stdio.h>
#include "mandatory_hand_lifecycle_v1.h"
#include "../../experiments/hal9-handoff/selection_finalization_v1.h"
int main(void){
 printf("{\"pointer\":%u,\"frozen_rules\":%u,\"frozen_rendezvous\":%u,\"frozen_request\":%u,\"frozen_response\":%u,\"frozen_lifecycle\":%u,\"token\":%u,\"cursor\":%u,\"pending\":%u,\"offer\":%u,\"proposal\":%u,\"ticket\":%u,\"client\":%u,\"status\":%u,\"table\":%u}\n",(unsigned)sizeof(void*),(unsigned)sizeof(RhsRules),(unsigned)sizeof(RhsRendezvous),(unsigned)sizeof(RhsRequest),(unsigned)sizeof(RhsResponse),(unsigned)sizeof(RhsLifecycle),(unsigned)sizeof(HfToken),(unsigned)sizeof(HfCursor),(unsigned)sizeof(HfPending),(unsigned)sizeof(HfOffer),(unsigned)sizeof(HfProposal),(unsigned)sizeof(HfTicket),(unsigned)sizeof(HfClient),(unsigned)sizeof(HfStatus),(unsigned)sizeof(HfFinalization));
 return sizeof(void*)!=4;
}
