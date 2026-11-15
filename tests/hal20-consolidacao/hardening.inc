/* Real controller and planner, synthetic adversarial lifecycle at named seams. */
#include "../../mods/ritual-duelists/mandatory_hand_lifecycle_v1.h"
static int hard_calls,hard_nested,hard_case,hard_disconnect_ack;
static RhsRules hard_rules;
static int hard_evaluate(const RhsRequest*q,RhsResponse*r){
 ++hard_calls;memset(r,0,sizeof*r);r->abi=RHS_ABI;r->size=sizeof*r;r->action=q->state.action;r->generation=q->state.generation;r->side=q->state.side;r->destination=q->state.selection[6];memcpy(r->selection,q->state.selection,12);r->status=RHS_UNCHANGED;
 if(hard_case==1)r->status=RHS_CANCELLED;
 if(hard_case==7&&!hard_nested++){Mods_Reset();puts("INSTRUMENTED reset inside evaluate");}
 if(hard_case==8&&!hard_nested++){((int(*)(void))engine_function("AiScript_Run"))();puts("INSTRUMENTED reentrant planner inside evaluate");}
 if(hard_case==17){const RhsLifecycle*c=Mods_Find(RHS_LIFECYCLE_KEY);CHECK(c);hard_disconnect_ack+=c->disconnect(&hard_rules);puts("INSTRUMENTED busy disconnect request, table kept alive");}
 if(hard_case==15&&hard_calls<=2)r->status=RHS_CANCELLED;
 return 1;
}
static RhsRules hard_rules={RHS_ABI,sizeof(RhsRules),hard_evaluate};
static int modified_evaluate(const RhsRequest*q,RhsResponse*r){puts("MUTATED_CALLBACK_EXECUTED");return hard_evaluate(q,r);}
static void hardening_cases(void){
 hard_case=Settings_GetNamed("test.hcase",0);setup(5);if(Mods_Active(mi(RE)))queen_setup();
 const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);CHECK(v);begin("hardening-adversarial-controller");
 if(hard_case==1||hard_case==6||hard_case==7||hard_case==8||hard_case==15||hard_case==17)CHECK(v->connect(&hard_rules));
 if(hard_case==6)hard_rules.evaluate=modified_evaluate;
 if(hard_case==9){unsigned*p=local(RD,"rd_selection_serial");CHECK(p);*p=0xffffffffu;puts("INSTRUMENTED serial exhaustion, not gameplay reachability");}
 if(hard_case==10){unsigned*p=local(RD,"rd_selection_generation");CHECK(p);*p=0xffffffffu;Mods_Reset();puts("INSTRUMENTED generation exhaustion");}
 if(hard_case==14)native_ai_result=0;
 int phase=hand_action();printf("HARDENING case=%d phase=%d hand=%d card=%d producer_calls=%d vm_calls=%u outcome=%d\n",hard_case,phase,hand_count(),action_card(),hard_calls,ecalls("AiScript_Run"),v->outcome());
 if(hard_case==1||hard_case==9||hard_case==10){result("bounded-invalid-retries",hard_calls<=8&&ecalls("AiScript_Run")<=8,"Persistent veto/exhaustion must stop issuing new requests instead of spinning indefinitely.");result("no-consumption-on-veto",hand_count()==5&&!action_card()&&!pull()->active,"No stale cursor play or pending resurrection.");}
 else if(hard_case==2||hard_case==3||hard_case==4||hard_case==22){result("post-validation-interposer-no-stale-card",hand_count()==5&&!action_card(),"Owned swap/controller interposition before physical consumption; no extra materializer called by harness.");}
 else if(hard_case==5){puts("NOT_REACHED controller wrapper deliberately did not delegate");result("nondelegating-wrapper-no-materialization",hand_count()==5&&setup_count==0,"Refusal/non-reachability is not validated gameplay.");}
 else if(hard_case==6){result("mutated-callback-not-called",hard_calls==0&&hand_count()==5,"A connected native vtable must not silently change its qualified callback.");}
 else if(hard_case==7||hard_case==8){result("interrupted-evaluation-not-committed",hand_count()==5&&!action_card(),"Reset/reentrant planning invalidates the outer request even if physical bytes happen to match.");}
 else if(hard_case==14){puts("NOT_REACHED VM reports no completed proposal; no fabricated action");result("no-legal-action-no-consumption",hand_count()==5&&!action_card(),"Normal VM wait is distinguished from a provider veto.");}
 else if(hard_case==17){result("busy-disconnect-not-an-unload-ack",hard_disconnect_ack==0&&hard_calls<=3&&hand_count()==5&&!action_card(),"Producer code/table remained alive; interrupted evaluation cannot commit.");}
 else if(hard_case==15){result("two-vetoes-then-one-action",hard_calls>=3&&hand_count()==4&&action_card()>0&&conserved()&&!pull()->active,"Consecutive invalid transactions followed by one genuine native action.");}
 state_cases();
}
