/* Test-only return tracing for known cdecl integer/void critical hooks.
 * Saves all GP registers/flags; does not rewrite any MOD production bytes. */
static int trace_returns;
static unsigned char*return_trampoline;
static LoadedObject loaded_history[64];static int loaded_history_count;
typedef struct{int id;uint32_t data,object,flags;} TraceIdentity;
static void trace_records(TraceIdentity*out,int16_t*terrain){DuelCardRecord*r=datum("D_801A7AD8");for(int i=0;i<30;i++){out[i]=(TraceIdentity){r[i].card_id,(uint32_t)(uintptr_t)r[i].data,(uint32_t)(uintptr_t)r[i].object,r[i].flags};terrain[i]=r[i].terrain_modifier;}}
typedef struct{TraceIdentity records[30],last_records[30];int16_t terrain[30],last_terrain[30];unsigned own_records,own_terrain;uintptr_t ret;int index,side,hand;unsigned setups;unsigned char selection[12],deck[480],last_selection[12],last_deck[480];int consumption_record;int last_hand,own_consumed,own_selection,own_deck;unsigned last_setups,own_setups;unsigned short phase,substate,last_phase,last_substate;int own_phase,own_substate;} CallFrame;
static CallFrame return_frames[256];static unsigned return_depth;
static int trace_hand_count(int side){int n=0;if(side<0||side>1)return -1;DuelSideState*s=datum("D_800E9FF0");for(int i=0;i<5;i++)n+=s[side].hand[i]>=0;return n;}
static const char*trace_target(int index){return index>=0?registrations[index].name:engine[-index-1].name;}
static int trace_critical(int index){const char*n=trace_target(index);return !strcmp(n,"AiScript_Run")||!strcmp(n,"func_8001BAF0")||!strcmp(n,"DuelScene_UpdateHandActions")||!strcmp(n,"DuelEffect_ApplyRitual")||!strcmp(n,"DuelCard_DeactivateRecord")||!strcmp(n,"Duel_SetupCardRecord")||!strcmp(n,"DuelEffect_ApplyTerrain")||!strcmp(n,"func_80037DA4")||!strcmp(n,"func_80035E20");}
static void trace_segment(CallFrame*f)
{
 int hand=trace_hand_count(f->side);unsigned short phase=*(uint16_t*)datum("gDuel_wSceneStateFlags"),sub=*(uint16_t*)datum("D_8009B20C");
 TraceIdentity records[30];int16_t terrain[30];trace_records(records,terrain);for(int i=0;i<30;i++){if(memcmp(&records[i],&f->last_records[i],sizeof records[i]))f->own_records|=1u<<i;if(terrain[i]!=f->last_terrain[i])f->own_terrain|=1u<<i;}
 f->own_selection|=memcmp(f->last_selection,datum("D_800EAE88"),12)!=0;
 f->own_deck|=memcmp(f->last_deck,datum("gDuel_aDeckCardRecords"),480)!=0;
 f->own_consumed+=f->last_hand-hand;f->own_setups+=setup_count-f->last_setups;
 f->own_phase|=f->last_phase!=phase;f->own_substate|=f->last_substate!=sub;
}
static void trace_baseline(CallFrame*f)
{
 trace_records(f->last_records,f->last_terrain);
 memcpy(f->last_selection,datum("D_800EAE88"),12);memcpy(f->last_deck,datum("gDuel_aDeckCardRecords"),480);
 f->last_hand=trace_hand_count(f->side);f->last_setups=setup_count;
 f->last_phase=*(uint16_t*)datum("gDuel_wSceneStateFlags");f->last_substate=*(uint16_t*)datum("D_8009B20C");
}
static __attribute__((force_align_arg_pointer,noinline)) void traced_begin(int index,uintptr_t*ret)
{
 traced(index);if(!trace_returns||!trace_critical(index))return;CHECK(return_depth<256);
 if(return_depth)trace_segment(&return_frames[return_depth-1]);
 if(return_depth)printf("CALLBACK_EDGE parent=%d child=%d\n",return_frames[return_depth-1].index,index);
 CallFrame*f=&return_frames[return_depth++];memset(f,0,sizeof*f);f->ret=*ret;f->index=index;
 f->consumption_record=-1;
 if(!strcmp(trace_target(index),"DuelCard_DeactivateRecord")){
  DuelCardRecord*c=(DuelCardRecord*)ret[1];DuelCardRecord*base=datum("D_801A7AD8");
  if(c>=base&&c<base+30){f->consumption_record=c-base;printf("CONSUME_ENTER node=%d record=%d id=%d occupied=%d data=%08x\n",index,f->consumption_record,c->card_id,!!(c->flags&DUEL_CARD_FLAG_OCCUPIED),(unsigned)(uintptr_t)c->data);}
 }
 f->side=*(uint8_t*)datum("D_8009B1D5");f->hand=trace_hand_count(f->side);f->setups=setup_count;
 memcpy(f->selection,datum("D_800EAE88"),12);memcpy(f->deck,datum("gDuel_aDeckCardRecords"),480);
 trace_records(f->records,f->terrain);
 f->phase=*(uint16_t*)datum("gDuel_wSceneStateFlags");f->substate=*(uint16_t*)datum("D_8009B20C");trace_baseline(f);
 *ret=(uintptr_t)return_trampoline;
}
static __attribute__((force_align_arg_pointer,noinline)) uintptr_t traced_end(void)
{
 CHECK(return_depth);CallFrame*f=&return_frames[--return_depth];trace_segment(f);
 if(f->consumption_record>=0){DuelCardRecord*c=(DuelCardRecord*)datum("D_801A7AD8")+f->consumption_record;printf("CONSUME_EXIT node=%d record=%d id=%d occupied=%d data=%08x\n",f->index,f->consumption_record,c->card_id,!!(c->flags&DUEL_CARD_FLAG_OCCUPIED),(unsigned)(uintptr_t)c->data);}

 printf("CALLBACK_EXIT node=%d target=%s selection_changed=%d deck_changed=%d consumed=%d materializations=%u\n",f->index,trace_target(f->index),memcmp(f->selection,datum("D_800EAE88"),12)!=0,memcmp(f->deck,datum("gDuel_aDeckCardRecords"),480)!=0,f->hand-trace_hand_count(f->side),setup_count-f->setups);
 printf("FIELD_WRITES node=%d identity_mask=%u terrain_mask=%u\n",f->index,f->own_records,f->own_terrain);
 printf("CALLBACK_SELF node=%d selection_changed=%d deck_changed=%d consumed=%d materializations=%u phase_changed=%d substate_changed=%d empty_exit=%d identity_mask=%u terrain_mask=%u\n",f->index,f->own_selection,f->own_deck,f->own_consumed,f->own_setups,f->own_phase,f->own_substate,((uint8_t*)datum("D_800EAE88"))[0]==0,f->own_records,f->own_terrain);
 if(Mods_Hal2224GuardEnabled()&&!strcmp(trace_target(f->index),"AiScript_Run")&&
 (!return_depth||strcmp(trace_target(return_frames[return_depth-1].index),"AiScript_Run")))HAL2224_AfterPlanner();
 if(return_depth)trace_baseline(&return_frames[return_depth-1]);
 return f->ret;
}
static void trace_node(int owner,const char*target,void*function)
{
 for(int j=loaded_history_count-1;j>=0;j--){LoadedObject*o=&loaded_history[j];
  for(size_t k=0;k<o->symbol_count;k++){struct ObjectSymbol*s=&o->symbols[k];
   if(s->function&&s->address==(uintptr_t)function){modules[which(owner)].object=*o;
    printf("CALLBACK_NODE owner=%s code=%08x target=%s function=%s bytes=%u node=%d\n",Mods_Id(owner),o->hash,target,s->name,(unsigned)s->size,nr-1);return;}
  }
 }
 printf("CALLBACK_NODE owner=%s code=00000000 target=%s function=UNRESOLVED bytes=0 node=%d\n",Mods_Id(owner),target,nr-1);
}
