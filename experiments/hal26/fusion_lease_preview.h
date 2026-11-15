/* Experimental typed lease capsule, NOT full Fusion state/persistence support. */
#include "mandatory_hand_lifecycle_v1.h"
#include "fusion_lease_schema_v1.h"
static FlPayload fl_payload;
static int fl_result,fl_save_token,fl_load_token;
static void(*fl_original_shutdown)(void);
static int fl_quiet(void){
 const RhsLifecycle*c=aux_host&&aux_host->find?aux_host->find(aux_host,RHS_LIFECYCLE_KEY):0;RhsRecovery s;
 if((gDuel_wSceneStateFlags&0x3fff)!=4)return 0;
 return !c||(c->abi==1&&c->size==sizeof*c&&c->read&&c->read(&s,sizeof s)&&s.state==RHS_STATE_IDLE&&!s.busy);
}
static int fl_index(void*p){uintptr_t a=(uintptr_t)p,b=(uintptr_t)gDuel_aDeckCardRecords;
 return a>=b&&a<b+80*sizeof(DuelDeckCardRecord)&&(a-b)%sizeof(DuelDeckCardRecord)==0?(int)((a-b)/sizeof(DuelDeckCardRecord)):-1;}
static void fl_save(MemoriesModEvent*e){
 if(e->phase!=MEMORIES_BEFORE)return;
 memset(&fl_payload,0,sizeof fl_payload);fl_payload.magic=0x464c5031u;fl_payload.version=1;
 fl_payload.owner=aux_owner_applied()!=0;fl_payload.eligible=fl_payload.owner&&fl_quiet();fl_payload.serial_floor=aux_unlock_serial;if(aux_unlock_serial==UINT32_MAX)fl_payload.eligible=0;
 for(int i=0;i<30;i++)if(aux_unlocks[i].serial){
  int k=fl_index(aux_unlocks[i].data);int slot=-1;
  for(int j=0;j<30;j++)if(aux_monster_slot(j)&&(D_801A7AD8[j].flags&DUEL_CARD_FLAG_OCCUPIED)&&D_801A7AD8[j].data==aux_unlocks[i].data&&D_801A7AD8[j].card_id==aux_unlocks[i].id){slot=j;break;}
  if(k<0||slot<0||aux_unlocks[i].serial>aux_unlock_serial){fl_payload.eligible=0;continue;}
  fl_payload.row[i]=(FlRow){(uint16_t)k,gDuel_aDeckCardRecords[k].data_block_index,(unsigned)aux_unlocks[i].id,aux_unlocks[i].serial};
 }
}
static void fl_load(MemoriesModEvent*e){
 if(e->phase!=MEMORIES_AFTER)return;fl_result=-1;
 if(fl_payload.magic!=0x464c5031u||fl_payload.version!=1||!fl_payload.eligible||!fl_payload.owner||!aux_owner_applied()||!fl_quiet()||aux_unlock_serial==UINT32_MAX)return;
 AuxUnlock ready[30];memset(ready,0,sizeof ready);
 for(int i=0;i<30;i++)if(fl_payload.row[i].serial){
  FlRow v=fl_payload.row[i];int found=-1;
  if(v.deck>=80||!v.card||v.card>32767||v.serial>fl_payload.serial_floor)return;
  if(gDuel_aDeckCardRecords[v.deck].id!=v.card||gDuel_aDeckCardRecords[v.deck].data_block_index!=v.block)return;
  for(int j=0;j<30;j++)if(aux_monster_slot(j)&&(D_801A7AD8[j].flags&DUEL_CARD_FLAG_OCCUPIED)&&D_801A7AD8[j].data==&gDuel_aDeckCardRecords[v.deck]&&D_801A7AD8[j].card_id==v.card){if(found>=0)return;found=j;}
  if(found<0)return;
  for(int j=0;j<i;j++)if(ready[j].serial&&(ready[j].data==&gDuel_aDeckCardRecords[v.deck]||ready[j].serial==v.serial))return;
  ready[i]=(AuxUnlock){(int)v.card,&gDuel_aDeckCardRecords[v.deck],v.serial};
 }
 memcpy(aux_unlocks,ready,sizeof ready);
 if(aux_unlock_serial<fl_payload.serial_floor)aux_unlock_serial=fl_payload.serial_floor;
 /* Keep live-process high water after a rewind; no new issuance reuses a past serial. */
 fl_result=1;
}
static int fl_status(void){return fl_result;}
static const FlPreview fl_api={1,sizeof(FlPreview),1,fl_status};
static void fl_shutdown(void){if(fl_save_token)aux_host->unsubscribe(aux_host,fl_save_token);if(fl_load_token)aux_host->unsubscribe(aux_host,fl_load_token);fl_save_token=fl_load_token=0;if(fl_original_shutdown)fl_original_shutdown();}
static int fl_init(MemoriesMod*m){
 if(!aux_host->register_state||!aux_host->register_state(aux_host,&fl_payload,sizeof fl_payload,1))return 0;
 fl_save_token=aux_host->subscribe(aux_host,MEMORIES_EVENT_SAVE,10,fl_save);fl_load_token=aux_host->subscribe(aux_host,MEMORIES_EVENT_LOAD,-10,fl_load);
 if(!fl_save_token||!fl_load_token)return 0;
 if(!aux_host->provide(aux_host,"hal26-lease-preview-v1",(void*)&fl_api))return 0;
 fl_original_shutdown=m->shutdown;m->shutdown=fl_shutdown;return 1;
}
