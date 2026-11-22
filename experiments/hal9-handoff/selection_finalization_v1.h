/* DRAFT / LAB ONLY. Separate companion, never a reinterpretation of ABI1.
 * Process-local synchronous pointers; no serialization or native-code sandbox. */
#ifndef HAL9_SELECTION_FINALIZATION_V1_H
#define HAL9_SELECTION_FINALIZATION_V1_H
#include "mandatory_hand_selection_v1.h"
#define HF_KEY "ritual-duelists:selection-finalization-v1"
#define HF_ABI 1u
#define HF_MAGIC 0x48465331u
/* Policies are selected ONLY by the lab driver. No production default. */
enum {HF_OBSERVE=0,HF_KEEP_RD=1,HF_ALLOW_TW=2};
enum {HF_IDLE=0,HF_OFFERED,HF_PREPARED,HF_AUTHORIZED,HF_CONFIRMED,
 HF_MATERIALIZED,HF_CONSUMED,HF_ABORTED};
enum {HF_OK=1,HF_NO=0,HF_ABI_ERROR=-1,HF_STALE=-2,HF_MANDATORY=-3,
 HF_IDENTITY=-4,HF_BUSY=-5,HF_ACK_MISSING=-6,HF_CHANGED=-7,
 HF_ALREADY_SPENT=-8,HF_EXHAUSTED=-9,HF_CANCELLED=-10};
enum {HF_MONSTER=1,HF_SUPPORT=2};
enum {HF_NATIVE=0,HF_TIME_MAGIC=1};
typedef struct {uint32_t action,generation,connection,revision[3],nonce,client;} HfToken;
typedef struct {
 uint32_t mode,substate;int32_t slot,count,row,col;uint8_t marks[5],reserved[3];
} HfCursor;
typedef struct {
 int32_t active,source,replacement,card,mode,destination,index,physical_token;
} HfPending;
typedef struct {
 uint32_t abi,size;HfToken token;uint32_t mandatory;
 uint8_t previous[12];HfPending pending;RhsSnapshot observed;HfCursor cursor;
} HfOffer;
typedef struct {
 uint32_t abi,size;HfToken token;uint32_t policy,category,kind,destination;
 RhsPhysical physical;RhsSnapshot snapshot;HfCursor cursor;
} HfProposal;
typedef struct {
 uint32_t abi,size;HfToken token;uint32_t policy,category,destination,count;
} HfTicket;
typedef struct {
 uint32_t abi,size,epoch;
 int(*authorize)(const HfTicket*,const HfProposal*);
 int(*confirm)(const HfTicket*,const HfProposal*);
} HfClient;
typedef struct {
 uint32_t abi,size,state;int32_t reason;HfToken token;
 uint32_t busy,connected,consumed;
} HfStatus;
typedef struct {
 uint32_t abi,size,magic;
 int(*connect)(const HfClient*);
 int(*disconnect)(const HfClient*);
 int(*offer)(HfOffer*,uint32_t);
 int(*prepare)(const HfProposal*,HfTicket*,uint32_t);
 int(*authorize)(const HfTicket*);
 int(*confirm)(const HfTicket*,HfStatus*,uint32_t);
 int(*cancel)(const HfTicket*);
 int(*read)(HfStatus*,uint32_t);
} HfFinalization;
#if defined(__i386__)
_Static_assert(sizeof(HfToken)==32&&sizeof(HfCursor)==32&&sizeof(HfPending)==32,"HF metadata ABI");
_Static_assert(sizeof(HfOffer)==4316&&sizeof(HfProposal)==4312&&sizeof(HfTicket)==56,"HF packet ABI");
_Static_assert(sizeof(HfClient)==20&&sizeof(HfStatus)==60&&sizeof(HfFinalization)==44,"HF callback ABI");
#endif
#endif
