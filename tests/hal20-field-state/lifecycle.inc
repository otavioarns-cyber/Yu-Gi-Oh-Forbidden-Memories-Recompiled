#include "../../mods/ritual-duelists/mandatory_hand_lifecycle_v1.h"
static const RhsLifecycle*control(void){const RhsLifecycle*c=Mods_Find(RHS_LIFECYCLE_KEY);CHECK(c&&c->abi==1&&c->size==sizeof*c&&c->magic==RHS_LIFECYCLE_MAGIC);return c;}
static RhsRecovery recovery(void){RhsRecovery s;CHECK(control()->read(&s,sizeof s));printf("RECOVERY state=%u cause=%u attempts=%u action=%u gen=%u epoch=%u sequence=%u mode=%u\n",s.state,s.cause,s.attempts,s.action,s.generation,s.connection_epoch,s.sequence,s.recovery);return s;}
static void lifecycle_cases(int hc){
 const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);const RhsLifecycle*c=control();CHECK(v);
 setup(5);if(Mods_Active(mi(RE)))queen_setup();begin("hardening-lifecycle-public-control");
 if(hc==11){
  DuelCardRecord*r=datum("D_801A7AD8");DuelSideState*side=datum("D_800E9FF0");DuelCardRecord saved=r[18];
  Settings_SetNamed("test.fault",3);hand_action();RhsRecovery a=recovery();
  result("physical-error-quarantined",a.state==RHS_STATE_QUARANTINE&&hand_count()==5&&!action_card(),"Real invalid instance after approved planner; no fabricated pending.");
  r[18]=saved;
  for(int i=0;i<3;i++)((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();
  RhsRecovery b=recovery();result("consistent-return-does-not-auto-release",b.state==RHS_STATE_QUARANTINE&&a.sequence==b.sequence&&hand_count()==5,"Recovery is explicit, not triggered by equal card ID or a transient return to valid bytes.");
  int tmp=side[1].hand[3];side[1].hand[3]=side[1].hand[4];side[1].hand[4]=tmp;
  void*data=r[18].data;r[18].data=r[19].data;r[19].data=data;
  result("same-ID-new-identity-not-a-recovery",!c->recover(b.sequence),"Both copies395 remain structurally consistent; different identity/mapping is not the original safe checkpoint.");
  tmp=side[1].hand[3];side[1].hand[3]=side[1].hand[4];side[1].hand[4]=tmp;data=r[18].data;r[18].data=r[19].data;r[19].data=data;
  result("stale-UI-recovery-token-rejected",!c->recover(b.sequence-1),"Sequence-qualified request.");
  result("explicit-recovery-acknowledged",c->recover(b.sequence),"Only controller boundary reset; production did not write a physical record.");
  Settings_SetNamed("test.fault",0);hand_action();result("recovery-one-new-real-action",hand_count()==4&&action_card()==395&&!pull()->active&&conserved(),"Fresh native snapshot/planner and exact copy identity.");
 }else if(hc==12){
  RhsRules wrong=hard_rules;wrong.size=sizeof wrong-1;
  result("provider-short-table-rejected",!v->connect(&wrong)&&!v->rules(),"Exact frozen ABI1 size before initial registration.");
  wrong.size=sizeof wrong+4;result("provider-extended-table-rejected",!v->connect(&wrong)&&!v->rules(),"Evolution requires a new key/version, not reading a larger legacy table.");
  wrong=hard_rules;wrong.evaluate=0;result("provider-null-callback-rejected",!v->connect(&wrong)&&!v->rules(),"Unavailable producer does not create a dangling registration.");
  CHECK(v->connect(&hard_rules));RhsRecovery a=recovery(),b;
  int calls_before=hard_calls;memset(&b,0xa5,sizeof b);RhsRecovery sentinel=b;
  result("read-size-rejected-without-write",!c->read(&b,sizeof b-1)&&!memcmp(&b,&sentinel,sizeof b),"Caller-size guard.");
  for(int i=0;i<5;i++){CHECK(c->read(&b,sizeof b));result("UI-read-is-pure",!memcmp(&a,&b,sizeof a)&&hard_calls==calls_before,"No callbacks, retry or epoch changes during observation.");}
  RhsRules other=hard_rules;result("disconnect-owner-must-match",!c->disconnect(&other)&&v->rules()==&hard_rules,"No detachment by similar table contents.");
  result("explicit-disconnect-ack",c->disconnect(&hard_rules)&&!v->rules(),"No active callback; caller may retire table after success.");
  hard_rules.evaluate=modified_evaluate;hand_action();result("disconnected-callback-not-used",hard_calls==0&&hand_count()==4&&conserved(),"RD operates independently; not a call through freed/mutated provider.");
  hard_rules.evaluate=hard_evaluate;setup(5);CHECK(v->connect(&hard_rules));b=recovery();result("new-connection-epoch",b.connection_epoch!=a.connection_epoch,"Fresh registration after acknowledged disconnect.");
 }else if(hc==13){
  int enabled[512];CHECK(Mods_Count()<512);for(int i=0;i<Mods_Count();i++)enabled[i]=Mods_Enabled(i);enabled[mi(RE)]=0;char error[512]={0};
  int ok=Mods_Apply(enabled,error,sizeof error);printf("MANAGER apply=%d error=%s saved=%d effective=%d restart=%d\n",ok,error,Mods_Enabled(mi(RE)),Mods_Active(mi(RE)),Mods_RequiresRestart(mi(RE)));
  result("restart-pending-does-not-disable-effective-owner",ok&&!Mods_Enabled(mi(RE))&&Mods_Active(mi(RE))&&Mods_RequiresRestart(mi(RE)),"Actual Mods_Apply, not force-applied fixture.");
  hand_action();result("pending-preference-keeps-Queen",hand_count()==4&&action_card()==395,"Own applied state overrides saved preference until restart.");
  Mods_SetEnabled(mi(RE),1);result("SetEnabled-preserves-loaded-owner",Mods_Active(mi(RE)),"Manager operation outside planner gap.");
  for(int i=0;i<Mods_Count();i++)enabled[i]=Mods_Enabled(i);enabled[mi(RD)]=0;
  ok=Mods_Apply(enabled,error,sizeof error);
  result("restart-pending-keeps-consumer-boundary",ok&&!Mods_Enabled(mi(RD))&&Mods_Active(mi(RD)),"No mid-action hook removal through the normal manager.");
 }else if(hc==16){
  CHECK(v->connect(&hard_rules));hard_case=1;hand_action();RhsRecovery a=recovery();
  result("persistent-veto-has-bounded-observable-state",a.state==RHS_STATE_BLOCKED&&a.attempts==a.limit&&hard_calls<=a.limit&&hand_count()==5,"No infinite new requests or actions.");
  for(int i=0;i<5;i++)((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();RhsRecovery b=recovery();
  result("blocked-state-stable",a.sequence==b.sequence&&hard_calls<=a.limit,"No log/state churn while awaiting explicit recovery.");
  hard_case=0;result("explicit-retry-after-cause-removed",c->recover(b.sequence),"Cause removal alone did not silently resume.");
  hand_action();result("bounded-retry-one-physical-action",hand_count()==4&&action_card()>0&&conserved(),"Fresh allowed plan after explicit acknowledgement.");
 }else if(hc==18){
  CHECK(v->connect(&hard_rules));hard_case=1;hand_action();RhsRecovery old=recovery();
  Mods_Reset();RhsRecovery fresh=recovery();
  result("reset-invalidates-recovery-request",old.sequence!=fresh.sequence&&!c->recover(old.sequence),"A UI request observed before reset cannot resume a newer generation.");
  hard_case=0;setup(5);hand_action();result("reset-after-block-one-fresh-action",hand_count()==4&&conserved()&&!pull()->active,"No old vetoed pending restored after reset.");
 }else if(hc==20){
  RhsRecovery a=recovery();result("cold-OFF-no-producer-callback",mi(RE)>=0&&!Mods_Active(mi(RE))&&!Mods_CodeHash(mi(RE))&&!a.connected&&!v->rules(),"Manifest present, disabled at real process initialization.");
  hand_action();result("cold-OFF-independent-planner",hand_count()==4&&action_card()>0&&conserved(),"No missing-provider dependency.");
 }else if(hc==21){
  hand_action();setup(5);queen_setup();unsigned*epoch=local(RE,"selection_epoch");CHECK(epoch);*epoch=UINT32_MAX;Mods_Reset();queen_setup();begin("producer-generation-exhaustion");hand_action();RhsRecovery a=recovery();
  result("producer-exhaustion-fails-closed",a.state==RHS_STATE_EXHAUSTED&&a.recovery==RHS_RECOVER_PROCESS_RESTART&&hand_count()==5&&!action_card(),"Counter seeded adversarially; no ordinary gameplay reachability claim.");
  result("exhaustion-not-UI-recoverable",!c->recover(a.sequence),"Reset/retry cannot reuse exhausted producer incarnation.");
 }
 state_cases();
}
