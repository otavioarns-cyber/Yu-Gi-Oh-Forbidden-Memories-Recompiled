/* Adversarial metadata/callback faults are test instrumentation, not gameplay.
 * Provider forwards to the real PR34 implementation. No pending/plan injection. */
static HfFinalization client_proxy;static const HfFinalization*client_real;
static HfClient client_saved;static int client_mode,client_fault_once;
static HfToken client_first_token;
static int client_noack(const HfTicket*t,const HfProposal*p){(void)t;(void)p;return 0;}
static int client_reenter(const HfTicket*t,const HfProposal*p){
 int rc=client_real->authorize(t);printf("CLIENT_FAULT reentrant=%d\n",rc);return client_saved.authorize(t,p);
}
static int client_connect_proxy(const HfClient*c){
 client_saved=*c;int rc=client_real->connect(c);
 if(rc==HF_OK&&(client_mode==109||client_mode==110)){
  HfClient*x=local(RD,"hf_client_copy");
  if(client_mode==109)x->confirm=client_noack;else x->authorize=client_reenter;
 }
 return rc;
}
static int client_offer_proxy(HfOffer*o,uint32_t size){
 int rc=client_real->offer(o,size);
 if(rc==HF_OK&&client_mode==117)o->observed.selection[0]=255;
 return rc;
}
static int client_prepare_proxy(const HfProposal*p,HfTicket*t,uint32_t size){
 HfProposal copy=*p;if(client_mode==107)++copy.token.generation;
 int rc=client_real->prepare(&copy,t,size);
 if(rc==HF_OK&&client_mode==114)++t->token.nonce;
 return rc;
}
static int client_authorize_proxy(const HfTicket*t){
 if(client_mode==111){const HfClient*c=*(const HfClient**)local(RD,"hf_client");CHECK(client_real->disconnect(c)==HF_OK);}
 int rc=client_real->authorize(t);
 if(rc==HF_OK){
  if(client_mode==123)Hal25_ForceApplied(mi(T),0);
  if(client_mode==124)Mods_SetEnabled(mi(T),0);
  if(client_mode==125)Hal25_ForceApplied(mi(RD),0);
  if(client_mode==108){const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);control()->invalidate(v->rules(),RHS_CAUSE_OWNER);}
  if(client_mode==115){DuelCardRecord*r=datum("D_801A7AD8");r[15].data=&((DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords"))[41];}
  if(client_mode==116)client_real->cancel(t);
 }
 return rc;
}
static void client_fault_setup(int mode){
 client_mode=mode;client_fault_once=0;client_real=Mods_Find(HF_KEY);CHECK(client_real);
 client_proxy=*client_real;client_proxy.connect=client_connect_proxy;client_proxy.offer=client_offer_proxy;
 client_proxy.prepare=client_prepare_proxy;client_proxy.authorize=client_authorize_proxy;
 if(mode==105)client_proxy.abi=0;if(mode==106)client_proxy.confirm=NULL;
 CHECK(Mods_Provide(mi(RD),"selection-finalization-v1",&client_proxy));
 printf("INSTRUMENTED provider metadata/callback fault mode=%d; real transaction remains owned by RD\n",mode);
}
static void client_after_planner(void){
 int mode=Settings_GetNamed("test.tw-case",0);if(mode<100)return;
 printf("CLIENT_SELECTION stage=after-planner source=%u destination=%u face=%u category=%u ticket_category=%u last_rc=%d\n",((uint8_t*)datum("D_800EAE88"))[0],((uint8_t*)datum("D_800EAE88"))[6],((uint8_t*)datum("D_800EAE88"))[8],*(unsigned*)tw_symbol("tw_final_category"),((HfTicket*)tw_symbol("tw_hf_ticket"))->category,*(int*)tw_symbol("tw_hf_result"));
 if(mode==121&&Mods_Active(mi(D))&&ds()[5]&&ds()[1]==2&&!client_fault_once){
  client_fault_once=1;uint8_t*s=datum("D_800EAE88");AiActiveCard*a=datum("gDuel_aActiveCards");
  if(s[0]>=11&&s[0]<16&&a[s[0]].card_id==16){a[s[0]].card_type=CARD_TYPE_MAGIC;puts("INSTRUMENTED adverse logical category Support after Monster handoff; original private Dual gate must reject spent Support");}
 }
 if((mode==112||mode==119)&&!client_fault_once){
  client_first_token=((HfTicket*)tw_symbol("tw_hf_ticket"))->token;
  client_fault_once=1;DuelSelectionRecord*c=(DuelSelectionRecord*)((char*)datum("D_800E9F10")+DUEL_SELECTION_SIDE_SIZE);c->field_0E=(c->field_0E+1)%5;
  if(mode==119)((uint8_t*)datum("D_800EAE88"))[6]=1;
  puts("INSTRUMENTED post-ACK cursor/category mutation before materialization");
 }
}
