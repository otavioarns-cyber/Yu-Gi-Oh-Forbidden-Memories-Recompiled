/* EXPERIMENTAL PATH A: Queen logical state only, NOT full Ritual Effects state.
 * Typed fixed-width values, no pointer/BSS snapshots. Never authorizes a load.
 * Included only by the explicitly built preview variant, not the collection. */
#include "mandatory_hand_lifecycle_v1.h"
#include "queen_state_schema_v1.h"
static QsPayload qs_payload;
static int qs_save_token,qs_load_token,qs_result;
static void(*qs_old_shutdown)(void);
static int qs_quiet(void){
 const RhsLifecycle*c=host&&host->find?host->find(host,RHS_LIFECYCLE_KEY):0;
 RhsRecovery r;
 if((gDuel_wSceneStateFlags&0x3fff)!=4)return 0;
 if(!c)return 1;
 return c->abi==1&&c->size==sizeof*c&&c->read&&c->read(&r,sizeof r)&&
        r.state==RHS_STATE_IDLE&&!r.busy;
}
static int qs_deck(void*p){uintptr_t a=(uintptr_t)p,b=(uintptr_t)gDuel_aDeckCardRecords;
 return a>=b&&a<b+80*sizeof(DuelDeckCardRecord)&&(a-b)%sizeof(DuelDeckCardRecord)==0?(int)((a-b)/sizeof(DuelDeckCardRecord)):-1;}
static void qs_save(MemoriesModEvent*e){
 if(e->phase!=MEMORIES_BEFORE)return;
 memset(&qs_payload,0,sizeof qs_payload);qs_payload.magic=0x51535031u;qs_payload.version=1;
 qs_payload.owner=recipe_export_active!=0;qs_payload.eligible=qs_quiet()&&recipe_export_active;
 qs_payload.fulfilled[0]=cosmo_turn_fulfilled[0];qs_payload.fulfilled[1]=cosmo_turn_fulfilled[1];
 for(int i=0;i<30;i++)if(cosmo_live(i)){
  int n=qs_deck(cosmo_marks[i].data);if(n<0){qs_payload.eligible=0;continue;}
  qs_payload.mask|=1u<<i;qs_payload.row[i]=(QsRow){(uint16_t)n,gDuel_aDeckCardRecords[n].data_block_index,cosmo_marks[i].female_count};
 }
}
static void qs_load(MemoriesModEvent*e){
 if(e->phase!=MEMORIES_AFTER)return;
 qs_result=-1;
 if(qs_payload.magic!=0x51535031u||qs_payload.version!=1||!qs_payload.eligible||
 !qs_payload.owner||!recipe_export_active||!qs_quiet()||qs_payload.mask&0xc0000000u||
 qs_payload.fulfilled[0]>1||qs_payload.fulfilled[1]>1)return;
 /* Validate every physical reference before modifying any owner state. */
 for(int i=0;i<30;i++)if(qs_payload.mask&(1u<<i)){
  QsRow v=qs_payload.row[i];DuelCardRecord*c=&D_801A7AD8[i];
  if(!monster_slot(i)||v.deck>=80||!v.female||v.female>10||
   !(c->flags&DUEL_CARD_FLAG_OCCUPIED)||c->card_id!=COSMO_QUEEN_ID||
   c->data!=&gDuel_aDeckCardRecords[v.deck]||gDuel_aDeckCardRecords[v.deck].id!=COSMO_QUEEN_ID||
   gDuel_aDeckCardRecords[v.deck].data_block_index!=v.block)return;
 }
 for(int i=0;i<30;i++)if(qs_payload.mask&(1u<<i)){
  cosmo_marks[i].data=&gDuel_aDeckCardRecords[qs_payload.row[i].deck];
  cosmo_marks[i].ritual=1;cosmo_marks[i].female_count=qs_payload.row[i].female;
 }
 cosmo_live_mask=qs_payload.mask;cosmo_live_hint=cosmo_live_mask!=0;
 memcpy(cosmo_turn_fulfilled,qs_payload.fulfilled,2);
 if(cosmo_live_hint)stat_effect_hints|=STAT_HINT_COSMO;
 /* clear()/reset has advanced producer epoch; do not restore callback/action tokens. */
 qs_result=1;
}
static int qs_status(void){return qs_result;}
static const QsPreview qs_preview={1,sizeof(QsPreview),1,qs_status};
static void qs_shutdown(void){
 if(qs_save_token)host->unsubscribe(host,qs_save_token);if(qs_load_token)host->unsubscribe(host,qs_load_token);
 qs_save_token=qs_load_token=0;if(qs_old_shutdown)qs_old_shutdown();
}
static int qs_init(MemoriesMod*mod){
 if(!host->register_state||!host->register_state(host,&qs_payload,sizeof qs_payload,1))return 0;
 qs_save_token=host->subscribe(host,MEMORIES_EVENT_SAVE,0,qs_save);
 qs_load_token=host->subscribe(host,MEMORIES_EVENT_LOAD,0,qs_load);
 if(!qs_save_token||!qs_load_token)return 0;
 host->provide(host,"queen-state-preview-v1",(void*)&qs_preview);
 qs_old_shutdown=mod->shutdown;mod->shutdown=qs_shutdown;return 1;
}
