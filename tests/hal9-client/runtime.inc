/* Instrumentation observes the real client linked in the private TW adapter.
 * No driver calls offer/prepare/authorize/confirm in these modes. */
#include "faults.inc"
static void client_cases(int mode){
 if(mode==120){
  setup(5);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");
  for(int i=0;i<2;i++){d[40+i].id=16;r[15+i].card_id=16;r[15+i].attack=500;r[15+i].defense=400;((DuelCardDisplayObject*)r[15+i].object)->field_68=CARD_TYPE_SPELLCASTER;}
  set_stat(16,500,400,CARD_TYPE_SPELLCASTER);setup_count=0;begin("natural-Monster-then-TimeMagic");
  int p=hand_action();printf("CLIENT_FIRST_MONSTER phase=%d id=%d used=%u hand=%d\n",p,action_card(),Mods_Active(mi(D))?ds()[1]:0,hand_count());
  if((p==6||p==7)&&(!Mods_Active(mi(D))||ds()[1]==1)){
   result("natural-first-Monster",hand_count()==4,"Actual planner/controller, no pending, selection or quota injection.");resolve_model(p);
   set_stat(22,2500,1200,CARD_TYPE_FIEND);for(int i=0;i<3;i++)put_field(5+i,22,i);
   if(Mods_Active(mi(D))){int awarded=award_bonus();printf("CLIENT_MONSTER_BONUS awarded=%d used=%u\n",awarded,ds()[1]);if(awarded){int n=hand_count();p=hand_action();printf("CLIENT_SECOND_SUPPORT phase=%d id=%d used=%u hand=%d views=%u\n",p,action_card(),ds()[1],hand_count(),*(unsigned*)tw_symbol("tw_snapshot_views"));
    result("natural-Monster-Support-quotas",(p==6||p==7)&&action_card()==16&&ds()[1]==3&&hand_count()==n-1&&conserved(),"Real Dual award, snapshot and final gate. Spent Monster never cleared during swap validation.");}
    else puts("HAL9_NOT_REACHED natural bonus unavailable");}
  }else puts("HAL9_NOT_REACHED real first planner did not choose Monster");return;
 }
 setup(5);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");
 d[40].id=16;r[15].card_id=16;set_stat(16,500,400,CARD_TYPE_SPELLCASTER);r[15].attack=500;r[15].defense=400;((DuelCardDisplayObject*)r[15].object)->field_68=CARD_TYPE_SPELLCASTER;
 set_stat(22,2500,1200,CARD_TYPE_FIEND);for(int i=0;i<3;i++)put_field(5+i,22,i);
 if(mode==103||mode==104)queen_setup();
 if(mode==113||mode==121){d[41].id=16;r[16].card_id=16;r[16].attack=500;r[16].defense=400;((DuelCardDisplayObject*)r[16].object)->field_68=CARD_TYPE_SPELLCASTER;}
 if((mode>=123&&mode<=125)||(mode>=105&&mode<=117&&mode!=112&&mode!=113))client_fault_setup(mode);
 setup_count=spawn_count=0;begin("TW-real-client-category");int p=hand_action(),id=action_card();
 const HfFinalization*api=Mods_Find(HF_KEY);HfStatus z={0};if(api)CHECK(api->read(&z,sizeof z)==HF_OK);
 unsigned auth=*(unsigned*)tw_symbol("tw_hf_authorizations"),acks=*(unsigned*)tw_symbol("tw_hf_acks");
 int rc=*(int*)tw_symbol("tw_hf_result");
 printf("CLIENT_STOP mode=%d phase=%d id=%d hand=%d exchanges=%u pending=%d rc=%d state=%u consumed=%u auth=%u ack=%u category=%u gate=%u block=%u\n",mode,p,id,hand_count(),setup_count,pull()->active,rc,z.state,z.consumed,auth,acks,*(unsigned*)tw_symbol("tw_final_category"),*(unsigned*)tw_symbol("tw_final_gate"),*(unsigned*)tw_symbol("tw_final_block"));
 if(mode==119){
  HfTicket*t=tw_symbol("tw_hf_ticket");
  result("post-ACK-old-action-not-resumed",auth>=2&&acks>=2&&t->token.action>client_first_token.action&&hand_count()==4&&setup_count==0,"Old selection was vetoed by real R/RD; a newly planned action received distinct tokens and two new explicit callbacks, not silent ticket reuse.");
  return;
 }
 int positive=mode==101||mode==102||mode==103||mode==113||mode==118||mode==121||mode==124;
 if(positive){
  int expected=mode==101?680:mode==103?395:16;
  result("real-client-final-controller",(p==6||p==7)&&id==expected&&auth==1&&acks==1,"Client is inside actual available TW adapter; old after-planner driver is not called for this suite.");
  result("real-client-one-consumption",hand_count()==4&&conserved()&&z.consumed==1&&z.state==HF_CONSUMED&&!pull()->active&&setup_count==(mode==101),"One physical card and exactly original RD exchange or independent materializer, no recover or restore by ID.");
  if(mode==113){resolve_model(p);support_resolution();support_bonus();}
  if(mode==121){resolve_model(p);support_resolution();if(Mods_Active(mi(D))){int awarded=award_bonus();if(awarded){int n=hand_count();unsigned setups=setup_count;int p2=hand_action();printf("CLIENT_SPENT_SUPPORT phase=%d hand=%d used=%u gate=%u\n",p2,hand_count(),ds()[1],calls(D,"func_8001BAF0"));result("real-Dual-rejects-spent-Support",p2==4&&hand_count()==n&&ds()[1]==2&&setup_count==setups,"Adverse type fault only; actual Dual gate refuses used Support, no physical consumption or fabricated rollback.");puts("SAFE_REFUSAL original Dual rejects spent category");}else puts("HAL9_NOT_REACHED bonus for adverse quota probe");}else puts("HAL9_NOT_REACHED Dual OFF has no quota gate");}
  if(mode==118){HfTicket ticket=*(HfTicket*)tw_symbol("tw_hf_ticket");HfProposal proposal=*(HfProposal*)tw_symbol("tw_hf_proposal");HfClient old=*(HfClient*)tw_symbol("tw_hf_client");Mods_Reset();int n=hand_count();int a=old.authorize(&ticket,&proposal),b=old.confirm(&ticket,&proposal);result("client-old-callback-refused",a==0&&b==0&&hand_count()==n,"Client phase closed and owner reset; old pointers do not authorize or resurrect a consumed action.");}
  if(Mods_Active(mi(D))&&mode!=113&&mode!=121)result("real-client-Dual-ledger",calls(D,"func_8001BAF0")>0&&ds()[1]==(mode==103?1:2),"Actual original private gate and final category ledger.");
 }else{
  result("real-client-refused",p==4&&hand_count()==5&&!setup_count&&!z.consumed,"OBSERVE, absent companion or mandatory refusal leaves the RD monitor effective; not a completed play.");
  puts("SAFE_REFUSAL real client did not approve conflicting selection");
 }
 if(mode==124)result("pending-preference-not-effective-OFF",Mods_Active(mi(T))&&!Mods_Enabled(mi(T)),"Actual restart manifest keeps owner applied while preference differs; callback uses real applied transition.");
 if(mode==104)result("real-client-Queen-no-override",rc==HF_MANDATORY,"Real producer, mandatory selection cannot be replaced.");
}
