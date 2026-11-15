/* Test client, NOT a new official TW policy. All proposals come from the real
 * planners. Only the selected lab policy calls the experimental RD companion. */
#include "../../experiments/hal9-handoff/selection_finalization_v1.h"
static const HfFinalization*hf_lab;
static HfTicket hf_lab_ticket;
static int hf_lab_mode,hf_authorize_calls,hf_confirm_calls,hf_last_offer,hf_last_prepare,hf_last_authorize,hf_last_confirm;
static HfClient hf_lab_client;
static HfClient *hf_lab_client_ptr;
static int hf_fault_fired;
static int hf_client_authorize(const HfTicket*t,const HfProposal*p){
 ++hf_authorize_calls;
 if(hf_lab_mode==64){int r=hf_lab->authorize(t);printf("HANDOFF_REENTRANT authorize=%d\n",r);return 1;}
 return hf_lab_mode!=56&&p->physical.id>0&&p->physical.data!=0;
}
static int hf_client_confirm(const HfTicket*t,const HfProposal*p){
 ++hf_confirm_calls;
 if(hf_lab_mode==72){int r=hf_lab->disconnect(&hf_lab_client);printf("HANDOFF_REENTRANT disconnect=%d\n",r);}
 return hf_lab_mode!=57&&t->token.action==p->token.action;
}
static void handoff_after_planner(void){
 int mode=Settings_GetNamed("test.tw-case",0);if(mode<50||mode>79)return;
 hf_lab_mode=mode;hf_lab=Mods_Find(HF_KEY);CHECK(hf_lab&&hf_lab->abi==HF_ABI&&hf_lab->size==sizeof*hf_lab&&hf_lab->magic==HF_MAGIC);
 hf_lab_client=(HfClient){HF_ABI,sizeof(HfClient),1,hf_client_authorize,hf_client_confirm};hf_lab_client_ptr=&hf_lab_client;
 if(mode==63){hf_lab_client_ptr=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(hf_lab_client_ptr!=MAP_FAILED);*hf_lab_client_ptr=hf_lab_client;}
 if(mode==74||mode==75){if(mode==74)hf_lab_client_ptr->abi=0;else hf_lab_client_ptr->confirm=NULL;int rc=hf_lab->connect(hf_lab_client_ptr);result("handoff-invalid-client-ABI-refused",rc==HF_ABI_ERROR,"Unsupported ABI or missing mandatory callback is rejected before reading action state.");return;}
 CHECK(hf_lab->connect(hf_lab_client_ptr)==HF_OK);
 if(mode==73){*(unsigned*)local(RD,"hf_nonce")=UINT32_MAX;puts("INSTRUMENTED companion nonce exhaustion; no pending or generation injection");}
 HfOffer offer;hf_last_offer=hf_lab->offer(&offer,sizeof offer);
 if(hf_last_offer!=HF_OK){printf("HANDOFF offer=%d mode=%d\n",hf_last_offer,mode);return;}
 printf("HANDOFF_OFFER mode=%d action=%u generation=%u connection=%u nonce=%u previous=%u live=%u mandatory=%u pending=%d source=%d index=%d\n",mode,offer.token.action,offer.token.generation,offer.token.connection,offer.token.nonce,offer.previous[0],offer.observed.selection[0],offer.mandatory,offer.pending.active,offer.pending.source,offer.pending.index);
 if(mode==50){puts("HANDOFF_OBSERVE no policy selected, no mutation or pending cancellation");return;}
 HfProposal proposal={0};proposal.abi=HF_ABI;proposal.size=sizeof proposal;proposal.token=offer.token;
 proposal.policy=mode==51||mode==53?HF_KEEP_RD:HF_ALLOW_TW;
 proposal.category=HF_SUPPORT;proposal.kind=HF_TIME_MAGIC;proposal.destination=offer.observed.selection[6];proposal.snapshot=offer.observed;proposal.cursor=offer.cursor;
 int slot=(int)offer.observed.selection[0]-11;if(slot<0||slot>=5){puts("HAL9_NOT_REACHED final real TW proposal is not a physical hand card");return;}proposal.physical=offer.observed.hand[slot];
 if(proposal.physical.id!=16||proposal.destination<6){proposal.kind=HF_NATIVE;proposal.category=card_type(proposal.physical.id)<20?HF_MONSTER:HF_SUPPORT;}
 printf("HANDOFF_SELECTION_DIFF");for(int i=0;i<12;i++)if(offer.previous[i]!=offer.observed.selection[i])printf(" byte%d:%u->%u",i,offer.previous[i],offer.observed.selection[i]);puts("");
 if(mode==65)--proposal.size;
 if(mode==69)proposal.category=HF_MONSTER;
 if(mode==70)++proposal.token.connection;
 hf_last_prepare=hf_lab->prepare(&proposal,&hf_lab_ticket,sizeof hf_lab_ticket);
 if(hf_last_prepare!=HF_OK){printf("HANDOFF_PREPARE mode=%d result=%d\n",mode,hf_last_prepare);return;}
 if(mode==59)++hf_lab_ticket.token.generation;
 if(mode==68)--hf_lab_ticket.size;
 if(mode==78)++hf_lab_ticket.token.nonce;
 if(mode==60){DuelSelectionRecord*c=(DuelSelectionRecord*)((char*)datum("D_800E9F10")+DUEL_SELECTION_SIDE_SIZE);c->field_0E=(c->field_0E+1)%5;puts("INSTRUMENTED cursor changed between prepare and authorize");}
 if(mode==61){DuelCardRecord*r=datum("D_801A7AD8");r[15].data=&((DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords"))[41];puts("INSTRUMENTED physical data reference changed; no restore by ID");}
 if(mode==62||mode==66){const RhsRendezvous*rv=Mods_Find(RHS_RENDEZVOUS);const RhsLifecycle*life=control();if(mode==62)life->invalidate(rv->rules(),RHS_CAUSE_OWNER);else life->disconnect(rv->rules());puts("INSTRUMENTED real producer invalidation/disconnect");}
 if(mode==63){hf_lab->disconnect(hf_lab_client_ptr);CHECK(mprotect(hf_lab_client_ptr,4096,PROT_NONE)==0);puts("INSTRUMENTED client table invalidated only after disconnect ACK");}
 HfStatus status;
 if(mode==55){hf_last_confirm=hf_lab->confirm(&hf_lab_ticket,&status,sizeof status);printf("HANDOFF missing authorize confirm=%d\n",hf_last_confirm);return;}
 hf_last_authorize=hf_lab->authorize(&hf_lab_ticket);
 if(hf_last_authorize!=HF_OK){printf("HANDOFF_AUTHORIZE mode=%d result=%d\n",mode,hf_last_authorize);return;}
 if(mode==58){hf_lab->cancel(&hf_lab_ticket);printf("HANDOFF cancellation before commit\n");return;}
 hf_last_confirm=hf_lab->confirm(&hf_lab_ticket,&status,sizeof status);
 printf("HANDOFF_COMMIT mode=%d result=%d policy=%u category=%u destination=%u pending=%d selection=%u auth_calls=%d ack_calls=%d\n",mode,hf_last_confirm,hf_lab_ticket.policy,hf_lab_ticket.category,hf_lab_ticket.destination,pull()->active,((uint8_t*)datum("D_800EAE88"))[0],hf_authorize_calls,hf_confirm_calls);
 if(mode==79&&hf_last_confirm==HF_OK){hf_lab->disconnect(hf_lab_client_ptr);puts("INSTRUMENTED client disconnected after confirmation before swap");}
 if(mode==67&&hf_last_confirm==HF_OK){int again=hf_lab->confirm(&hf_lab_ticket,&status,sizeof status);result("handoff-no-ticket-reuse-before-consumption",again==HF_STALE,"One explicit confirmation only; replay is rejected without modifying approved transaction.");}
}
static void handoff_controller_fault(void){
 int mode=Settings_GetNamed("test.tw-case",0);if((mode!=76&&mode!=77)||hf_fault_fired||!hf_lab)return;
 HfStatus z;CHECK(hf_lab->read(&z,sizeof z)==HF_OK);
 if(z.state!=HF_MATERIALIZED||(*(uint16_t*)datum("D_8009B20C")&15)<2)return;
 hf_fault_fired=1;
 if(mode==76){DuelSelectionRecord*c=(DuelSelectionRecord*)((char*)datum("D_800E9F10")+DUEL_SELECTION_SIDE_SIZE);c->field_0E=(c->field_0E+1)%5;puts("INSTRUMENTED late cursor write after ACK before collection");}
 else{DuelCardRecord*r=datum("D_801A7AD8");r[15].data=&((DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords"))[41];puts("INSTRUMENTED late physical reference write after ACK before collection");}
}
static void handoff_cases(int mode){
 setup(5);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");d[40].id=16;r[15].card_id=16;set_stat(16,500,400,CARD_TYPE_SPELLCASTER);r[15].attack=500;r[15].defense=400;((DuelCardDisplayObject*)r[15].object)->field_68=CARD_TYPE_SPELLCASTER;
 set_stat(22,2500,1200,CARD_TYPE_FIEND);for(int i=0;i<3;i++)put_field(5+i,22,i);
 if(mode==53||mode==54)queen_setup();
 setup_count=spawn_count=0;begin("handoff-real-planners-explicit-lab-policy");int p=hand_action(),id=action_card();
 HfStatus status={0};if(hf_lab)CHECK(hf_lab->read(&status,sizeof status)==HF_OK);
 printf("HANDOFF_STOP mode=%d phase=%d id=%d hand=%d exchanges=%u pending=%d gate=%u RD=%u H=%u hf_state=%u consumed=%u reason=%d\n",mode,p,id,hand_count(),setup_count,pull()->active,calls(D,"func_8001BAF0"),calls(RD,"func_8001BAF0"),calls(H,"func_8001BAF0"),status.state,status.consumed,status.reason);
 int positive=mode==51||mode==52||mode==53||mode==67||mode==71;
 if(positive){
  int expected=mode==51?680:mode==53?395:16;
  result("handoff-final-real-controller",(p==6||p==7)&&id==expected,"Real RD/TW planners and native controller; policy is explicit in LAB only, never production default.");
  result("handoff-one-physical-consumption",hand_count()==4&&conserved()&&status.consumed==1&&status.state==HF_CONSUMED&&!pull()->active&&setup_count<2,"No recover, ID restoration, postconsumption rollback or repeated materializer.");
  if(Mods_Active(mi(D)))result("handoff-real-Dual-category",calls(D,"func_8001BAF0")>0&&ds()[1]==(mode==53?1:2),"Actual private Dual gate callback executes; final ledger agrees with final action, native hybrid type caveat documented.");
  if(mode==52||mode==67||mode==71)result("handoff-revokes-only-old-unconsumed-pending",setup_count==0&&d[45].id==680,"Abandoned ritual remains physically in undrawn deck; no old fetch occurred.");
  if(mode==51)result("handoff-keeps-owned-pending-until-materializer",setup_count==1,"RD's original deck-fetch materializer alone performs its existing exchange.");
  if(mode==71){int before=hand_count();unsigned setups=setup_count;int rc=hf_lab->cancel(&hf_lab_ticket);result("handoff-cannot-cancel-after-consumption",rc==HF_ALREADY_SPENT&&hand_count()==before&&setup_count==setups,"Terminal action cannot be rolled back or revived through companion cancellation.");}
 }else{
  result("handoff-invalid-flow-refused-before-consumption",p==4&&hand_count()==5&&setup_count==0&&!status.consumed,"Missing ACK, stale metadata, mandatory conflict or injected fault never receives a substitute physical action.");
  if(mode==54)result("handoff-mandatory-cannot-be-erased",hf_last_prepare==HF_MANDATORY,"Real Queen producer mandatory response, no priority override.");
  if(mode==63)result("handoff-no-callback-after-disconnect-ACK",hf_authorize_calls==0&&hf_confirm_calls==0,"Client table cleared after acknowledged disconnect; no callback dereference follows.");
  puts("SAFE_REFUSAL handoff alternative not confirmed; original validation remains effective");
 }
}
