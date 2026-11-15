/* HAL25 remaining boundaries. Failures are named instrumentation, never retail gameplay. */
static RhsRules *boundary_table;static int boundary_mode,boundary_calls,boundary_ack,boundary_once;
static int boundary_evaluate(const RhsRequest*q,RhsResponse*r){
 ++boundary_calls;memset(r,0,sizeof*r);r->abi=RHS_ABI;r->size=sizeof*r;r->action=q->state.action;r->generation=q->state.generation;r->side=q->state.side;r->status=RHS_UNCHANGED;memcpy(r->selection,q->state.selection,12);r->destination=r->selection[6];
 if(boundary_mode==2){boundary_ack+=control()->disconnect(boundary_table);puts("INSTRUMENTED disconnect while callback active; table remains alive");}
 if(boundary_mode==3&&!boundary_once++){Mods_Reset();puts("INSTRUMENTED reset while callback active");}
 if(boundary_mode==4&&!boundary_once++){Hal25_ForceApplied(mi(RD),0);puts("INSTRUMENTED effective consumer OFF while callback active");}
 if(boundary_mode==7||boundary_mode==8)r->status=RHS_CANCELLED;
 if(boundary_mode==10||boundary_mode==11||boundary_mode==12||boundary_mode==13){
  memset(r->selection,0,12);r->selection[0]=11;r->selection[1]=12;r->selection[2]=14;if(boundary_mode==12)r->selection[1]=r->selection[2]=0;r->selection[6]=1;
  r->mandatory=1;r->status=memcmp(q->state.selection,r->selection,12)?RHS_REPLACED:RHS_UNCHANGED;
  r->count=rhs_selected(&q->state,r->selection,r->selected);r->destination=r->selection[6];
 }
 return 1;
}
static void boundary_attach(int mode){
 boundary_mode=mode;boundary_calls=boundary_ack=boundary_once=0;
 boundary_table=mmap(0,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(boundary_table!=MAP_FAILED);
 *boundary_table=(RhsRules){RHS_ABI,sizeof(RhsRules),boundary_evaluate};const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);CHECK(v&&v->connect(boundary_table));
}
static void boundary_poison(void){CHECK(control()->disconnect(boundary_table));CHECK(!((const RhsRendezvous*)Mods_Find(RHS_RENDEZVOUS))->rules());CHECK(!mprotect(boundary_table,4096,PROT_NONE));puts("POISON acknowledged producer table is now PROT_NONE until process exit");}
static void boundary_life(int mode){
 setup(5);begin("boundary-lifecycle");
 if(mode==6){
  queen_setup();Settings_SetNamed("test.fault",10);int before=hand_count();hand_action();const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);
  result("real-producer-shutdown-detaches",v&&!v->rules()&&hand_count()==before&&!pull()->active&&!action_card(),"Real owner shutdown at named after-planner seam. No callback memory freed by test.");state_cases();return;
 }
 boundary_attach(mode);const RhsLifecycle*c=control();
 if(mode==5){
  RhsRecovery a=recovery();Hal25_ForceApplied(mi(RD),0);RhsRecovery b=recovery();Hal25_ForceApplied(mi(RD),1);RhsRecovery d=recovery();
  result("effective-OFF-ON-generation-distinct",!b.effective&&d.effective&&d.generation!=a.generation&&!d.busy,"Actual applied callbacks, deliberately invoked outside normal restart UI.");
 }
 hand_action();RhsRecovery s=recovery();
 if(mode==1||mode==5)result("connected-owner-one-action",hand_count()==4&&!pull()->active&&conserved(),"Real native controller and RD planner through optional test producer; no pending injection.");
 if(mode==2)result("busy-detach-no-unload-ack",boundary_ack==0&&boundary_calls==3&&hand_count()==5&&s.state==RHS_STATE_BLOCKED,"No stale callback result commits; exact retry limit observed.");
 if(mode==3||mode==4)result("interrupted-owner-result-not-committed",hand_count()==5&&!action_card()&&!pull()->active,"Reset/effective-OFF inside callback invalidates external evaluation.");
 if(mode==7||mode==8){
  int calls=boundary_calls;unsigned vm=ecalls("AiScript_Run");for(int i=0;i<100;i++)((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();RhsRecovery t=recovery();
  result("bounded-veto-100-ticks-stable",calls==3&&boundary_calls==calls&&ecalls("AiScript_Run")==vm&&s.sequence==t.sequence&&t.state==RHS_STATE_BLOCKED&&t.attempts==3&&hand_count()==5,"No infinite requests, no consumption, observable state stable.");
  if(mode==8){
   boundary_mode=1;result("cause-removal-not-implicit-recovery",recovery().state==RHS_STATE_BLOCKED&&hand_count()==5,"Removing the synthetic veto does not resume it.");
   result("stale-recovery-request-denied",!c->recover(t.sequence-1),"UI/controller must use current observable sequence.");
   result("explicit-recovery-accepted",c->recover(t.sequence),"No hand repair or ID lookup.");hand_action();result("recovery-one-fresh-action",hand_count()==4&&conserved()&&!pull()->active,"New controller/planner transaction, never resumes the old veto.");
  }
 }
 int calls=boundary_calls;boundary_poison();
 if(mode==2||mode==7){s=recovery();if(s.recovery==RHS_RECOVER_EXPLICIT)CHECK(c->recover(s.sequence));}
 if(mode==4)Hal25_ForceApplied(mi(RD),1);
 setup(5);hand_action();result("ACK-table-never-read-after-poison",boundary_calls==calls&&hand_count()==4&&conserved()&&!pull()->active,"PROT_NONE table across reset and new action; stale table access would terminate this process.");
 Hal25_ForceApplied(mi(RD),0);Hal25_ForceApplied(mi(RD),1);setup(5);hand_action();result("post-ACK-effective-cycle-no-stale-callback",boundary_calls==calls&&hand_count()==4&&!pull()->active,"New generation while producer absent; callback count unchanged.");state_cases();
}
static void boundary_combined(int mode){
 setup(5);begin("boundary-combined-controller");boundary_attach(mode);int before=hand_count();int phase=hand_action();
 printf("COMBINED mode=%d phase=%d hand_before=%d hand_after=%d calls=%d actual_card=%d\n",mode,phase,before,hand_count(),boundary_calls,action_card());
 if(mode==12||mode==13){RhsRecovery state=recovery();
 result("controller-derived-selection-cannot-change-consumption",hand_count()==before&&!action_card()&&state.state==RHS_STATE_QUARANTINE,"Test wrapper changes derived cursor/stack marks, while ABI selection/identities stay equal; reject before native collection.");
 if(Mods_Active(mi(D)))result("derived-refusal-quota-neutral",ds()[1]==0,"Real private Dual ledger after late refusal; no quota write or synthetic reset.");
 result("late-controller-quarantine-requires-new-duel",!control()->recover(state.sequence)&&hand_count()==before,"No fake rollback or automatic stack/UI repair.");boundary_poison();state_cases();return;}
 result("combined-native-three-physical-consumptions",phase==7&&hand_count()==before-3&&conserved(),"Synthetic public mandatory producer supplies 3-card proposal; native controller picks/removes all three in one confirmation; no retail-strategy claim.");
 DuelSideState*s=datum("D_800E9FF0");result("combined-bystanders-preserved",s[1].hand[0]==-1&&s[1].hand[1]==-1&&s[1].hand[3]==-1&&s[1].hand[2]>=0&&s[1].hand[4]>=0,"Physical slots, not IDs, are checked.");
 if(Mods_Active(mi(D)))result("combined-quota-one-category",ds()[1]==1,"Actual Dual ledger: three materials are one Monster action, not three quota uses.");
 if(mode==11){RhsRecovery a=recovery();int hands[5];memcpy(hands,s[1].hand,sizeof hands);DuelDeckCardRecord d[80];memcpy(d,datum("gDuel_aDeckCardRecords"),sizeof d);
 result("no-fictitious-post-consumption-rollback",!control()->recover(a.sequence)&&!memcmp(hands,s[1].hand,sizeof hands)&&!memcmp(d,datum("gDuel_aDeckCardRecords"),sizeof d),"Already committed action is not a recovery candidate; no restoration by ID or old pending.");}
 boundary_poison();state_cases();
}
static void boundary_ritual(int mode){
 setup(5);if(mode==23){Hal25_ForceApplied(mi(RE),0);Hal25_ForceApplied(mi(M),0);}
 begin("boundary-native-ritual-consumption");*(uint8_t*)datum("D_8009B1D5")=0;DuelCardRecord*r=datum("D_801A7AD8");DuelSideState*s=datum("D_800E9FF0");memset(r,0,30*sizeof*r);memset(s[0].hand,-1,5);set_stat(357,1600,1800,0);uint16_t row[]={666,29,31,101,357,0};memcpy(datum("gDuel_awRitualData"),row,sizeof row);
 int both=mode==21,hand=mode==22;
 if(both){put_field(5,101,0);put_hand(0,29,1);put_hand(1,31,2);put_hand(2,31,3);put_field(6,1,4);set_stat(1,3000,2500,0);}else if(hand){put_hand(0,29,0);put_hand(1,31,1);put_hand(2,101,2);put_hand(3,31,3);}else{put_field(5,29,0);put_field(6,31,1);put_field(7,101,2);put_field(8,31,3);}
 if(Settings_GetNamed("test.with-field",0))((void(*)(void))engine_function("DuelEffect_ApplyTerrain"))(); /* real Field wrapper; native terrain phase is modeled here */
 *(uint16_t*)datum("gDuel_wEffectCardID")=666;*(uint16_t*)datum("gDuel_wCardEffectFlags")=0;*(uint16_t*)datum("D_8009B210")=0;
 DuelDeckCardRecord deck[80];memcpy(deck,datum("gDuel_aDeckCardRecords"),sizeof deck);int hands[5];memcpy(hands,s[0].hand,sizeof hands);
 ((void(*)(void))engine_function("DuelEffect_ApplyRitual"))();
 if(hand){result("native-Hand-only-refused-default",*(int16_t*)datum("D_8009B1A0")==0&&!memcmp(hands,s[0].hand,sizeof hands)&&!memcmp(deck,datum("gDuel_aDeckCardRecords"),sizeof deck),"Default min_field=1 preserved, no fake successful ritual.");puts("REFUSED boundary Hand-only violates approved min_field default");return;}
 result("native-effect-proposes-without-spending",*(int16_t*)datum("D_8009B1A0")==357&&!memcmp(hands,s[0].hand,sizeof hands),"Real native ApplyRitual init and public selector; no manual deactivation loop.");
 for(int k=0;k<8;k++){const char*names[]={"bridge_mode","active_result","deact_step","where","rec","slot","consume_order","visual_rec"};int*q=local(M,names[k]);if(q)printf("MSTATE %s=%d,%d,%d\n",names[k],q[0],k>2?q[1]:0,k>2?q[2]:0);}
 const DuelRitualMatch*active=((const DuelRitualMatch*(*)(void))engine_function("Duel_RitualActive"))();if(active)printf("NATIVE_MATCH count=%d extended=%d records=%d,%d,%d\n",active->count,active->extended,active->record[0],active->record[1],active->record[2]);
 void*request=*(void**)datum("D_8009B17C");CHECK(request);((uint8_t*)request)[0x1d]=1;puts("MODEL_ANIMATION ritual request completion only; native effect owns all three deactivation calls");
 ((void(*)(void))engine_function("DuelEffect_ApplyRitual"))();
 int good=both?(!(r[5].flags&DUEL_CARD_FLAG_OCCUPIED)&&s[0].hand[0]==-1&&s[0].hand[1]==-1&&s[0].hand[2]==3&&(r[2].flags&DUEL_CARD_FLAG_OCCUPIED)&&(r[6].flags&DUEL_CARD_FLAG_OCCUPIED)):(!(r[5].flags&DUEL_CARD_FLAG_OCCUPIED)&&!(r[6].flags&DUEL_CARD_FLAG_OCCUPIED)&&!(r[7].flags&DUEL_CARD_FLAG_OCCUPIED)&&(r[8].flags&DUEL_CARD_FLAG_OCCUPIED));
 result("native-effect-complete-three-material-consumption",good&&(*(uint16_t*)datum("D_8009B210")==1),"Actual native effect executes all 3 public deactivations and moves to next animation stage; unselected same-ID copy remains.");
 result("ritual-deck-identities-not-replaced",!memcmp(deck,datum("gDuel_aDeckCardRecords"),sizeof deck),"Consumption never substitutes an equal-ID deck identity or invents rollback.");
 int anchor=*(uint8_t*)datum("D_8009B19C");CHECK(anchor>=0&&anchor<30);DuelDeckCardRecord*identity=r[anchor].data;CHECK(identity);DuelDeckCardRecord expected[80];memcpy(expected,deck,sizeof expected);int index=identity-(DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords");CHECK(index>=0&&index<80);expected[index].id=357;
 *(uint16_t*)datum("D_8009B210")=3;puts("MODEL_ANIMATION skip portal-only stages1/2; execute real native result publication once");
 ((void(*)(void))engine_function("DuelEffect_ApplyRitual"))();
 result("native-result-publishes-in-original-anchor",r[anchor].data==identity&&r[anchor].card_id==357&&identity->id==357&&!memcmp(expected,datum("gDuel_aDeckCardRecords"),sizeof expected),"Real native G32 write after all tributes; only anchor ID changes, all other deck identities untouched. Later rendering/activation not claimed.");state_cases();
}
static void boundary_cases(int mode){if(mode>=20)boundary_ritual(mode);else if(mode==10||mode==11||mode==12||mode==13)boundary_combined(mode);else boundary_life(mode);}
