#ifndef HAL26_OWNER_PREFLIGHT
#define HAL26_OWNER_PREFLIGHT
/* Experimental coordinator, no engine loader registration. Views refer to the
 * immutable incoming image, never to the still-live current duel. No pointers
 * are serialized and this helper performs no writes. Only Queen+lease coverage. */
#include "../../experiments/hal20-field-state/queen_state_schema_v1.h"
#include "fusion_lease_schema_v1.h"
typedef struct {uint16_t id,deck,block,occupied,data_id;} H26View;
enum{H26_READY=0,H26_BAD_ABI=1,H26_BUSY=2,H26_BAD_IDENTITY=3,H26_INCOMPLETE=4};
static int H26_Preflight(const QsPayload*q,const FlPayload*f,const H26View*v,unsigned n,int whole_collection){
 if(!q||!f||!v||n!=30||q->magic!=0x51535031u||q->version!=1||f->magic!=0x464c5031u||f->version!=1)return H26_BAD_ABI;
 if(q->eligible!=1||q->owner!=1||f->eligible!=1||f->owner!=1||f->serial_floor==UINT32_MAX)return H26_BUSY;
 if(q->mask&0xc0000000u||q->fulfilled[0]>1||q->fulfilled[1]>1||q->pad)return H26_BAD_IDENTITY;
 for(unsigned i=0;i<n;i++){
  if(q->mask&(1u<<i)){QsRow r=q->row[i];if(!((i>=5&&i<10)||(i>=20&&i<25))||!v[i].occupied||v[i].id!=708||v[i].data_id!=v[i].id||r.deck>=80||!r.female||r.female>10||v[i].deck!=r.deck||v[i].block!=r.block)return H26_BAD_IDENTITY;for(unsigned j=0;j<i;j++)if((q->mask&(1u<<j))&&q->row[j].deck==r.deck)return H26_BAD_IDENTITY;}
  else if(q->row[i].deck||q->row[i].block||q->row[i].female)return H26_BAD_IDENTITY;
  FlRow r=f->row[i];if(r.serial){int matches=0;if(r.deck>=80||!r.card||r.card>32767||r.serial>f->serial_floor)return H26_BAD_IDENTITY;
   for(unsigned j=0;j<n;j++)if(((j>=5&&j<10)||(j>=20&&j<25))&&v[j].occupied&&v[j].data_id==v[j].id&&v[j].deck==r.deck&&v[j].block==r.block&&v[j].id==r.card)++matches;
   if(matches!=1)return H26_BAD_IDENTITY;
   for(unsigned j=0;j<i;j++)if(f->row[j].serial&&(f->row[j].deck==r.deck||f->row[j].serial==r.serial))return H26_BAD_IDENTITY;
  }else if(r.deck||r.block||r.card)return H26_BAD_IDENTITY;
 }
 /* Full R/F caches, generated backing arrays, RD transactions and remaining
    owners have no complete audited schemas yet. Never authorize full load. */
 return whole_collection?H26_INCOMPLETE:H26_READY;
}
#endif
