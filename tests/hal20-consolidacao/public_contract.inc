/* Use the shipped snapshot adapter with host memory bindings, not private
 * producer functions. All calls below go through the public rendezvous. */
#include "game/duel_scene_state.h"
#define D_801A7AD8 ((DuelCardRecord*)datum("D_801A7AD8"))
#define D_800907CC ((uint8_t*)datum("D_800907CC"))
#define D_800E9FF0 ((DuelSideState*)datum("D_800E9FF0"))
#define D_800EA030 ((DuelHandSlot*)datum("D_800EA030"))
#define D_800EAE88 ((uint8_t*)datum("D_800EAE88"))
#define gDuel_aDeckCardRecords ((DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords"))
#define gDuel_aActiveCards ((AiActiveCard*)datum("gDuel_aActiveCards"))
#define gDuel_adwCardStats ((uint32_t*)datum("gDuel_adwCardStats"))
#define gDuel_wSceneStateFlags (*(uint16_t*)datum("gDuel_wSceneStateFlags"))
#define Cards_Type card_type
#include "../../mods/ritual-card-effects/hand_selection_snapshot.h"
#undef D_801A7AD8
#undef D_800907CC
#undef D_800E9FF0
#undef D_800EA030
#undef D_800EAE88
#undef gDuel_aDeckCardRecords
#undef gDuel_aActiveCards
#undef gDuel_adwCardStats
#undef gDuel_wSceneStateFlags
#undef Cards_Type
static void request(RhsRequest*q){memset(q,0,sizeof*q);q->abi=RHS_ABI;q->size=sizeof*q;CHECK(rhs_capture(&q->state,1,101,7));}
static int bad_response(const RhsRequest*q,RhsResponse*r){
 memset(r,0,sizeof*r);r->abi=RHS_ABI;r->size=sizeof*r;r->action=q->state.action;
 r->generation=q->state.generation;r->side=q->state.side;r->status=RHS_UNCHANGED;
 memcpy(r->selection,q->state.selection,12);r->destination=r->selection[6];
 switch(Settings_GetNamed("test.badmode",0)){
 case 1:r->abi=2;break;case 2:r->status=99;break;case 3:r->action--;break;
 case 4:r->side^=1;break;case 5:r->selection[0]=13;break;
 case 6:r->status=RHS_CANCELLED;r->mandatory=1;break;
 case 7:r->status=RHS_REPLACED;r->mandatory=1;r->count=1;break;
 case 8:return 0;
 }return 1;
}
static const RhsRules bad_rules={RHS_ABI,sizeof(RhsRules),bad_response};
static void public_contract_cases(void){
 /* First real AI call establishes the optional binding even when R loaded first. */
 setup(5);queen_setup();hand_action();
 const RhsRendezvous*v=Mods_Find(RHS_RENDEZVOUS);CHECK(v&&v->abi==1&&v->rules());
 const RhsRules*p=v->rules();CHECK(p->abi==1&&p->size==sizeof*p&&p->evaluate);
 setup(5);queen_setup();begin("public-contract-protocol");
 ((void(*)(void))engine_function("func_80028220"))();
 uint8_t*sel=datum("D_800EAE88");memset(sel,0,12);sel[0]=11;sel[6]=1;
 RhsRequest q;RhsResponse r;RhsSnapshot before,after;request(&q);before=q.state;
 CHECK(p->evaluate(&q,&r));rhs_capture(&after,1,101,7);
 result("provider-pure-replacement",r.status==RHS_REPLACED&&r.mandatory&&r.count==1&&r.selected[0].id==395&&r.selected[0].index==44&&!memcmp(&before,&after,sizeof before),"Real producer through public ABI; live selection, deck, stats and physical snapshots unchanged.");
 unsigned epoch=r.revision[0];memcpy(sel,r.selection,12);request(&q);CHECK(p->evaluate(&q,&r));
 result("mandatory-unchanged-physical-choice",r.status==RHS_UNCHANGED&&r.mandatory&&r.count==1&&r.selected[0].index==44,"Existing Female choice preserved, mandatory distinct from ordinary unchanged.");
 RhsRequest invalid=q;invalid.state.hand[4].data^=4;
 result("stale-physical-snapshot-rejected",!p->evaluate(&invalid,&r),"Equal ID does not authorize an altered identity token.");
 invalid=q;invalid.state.side=0;result("wrong-side-rejected",!p->evaluate(&invalid,&r),"No cross-side selection.");
 invalid=q;invalid.abi=2;result("request-ABI-rejected",!p->evaluate(&invalid,&r),"Versioned request required.");
 RhsRules incompatible=*p;incompatible.abi=2;
 result("provider-ABI-rejected",!v->connect(&incompatible)&&v->rules()==p,"Incompatible optional registration cannot replace qualified producer.");
 request(&q);q.intent=RHS_CANCEL_INTENT;CHECK(p->evaluate(&q,&r));
 result("explicit-cancel-intent",r.status==RHS_CANCELLED&&r.mandatory,"An explicit request veto is distinct from an empty VM proposal.");
 memset(sel,0,12);request(&q);CHECK(p->evaluate(&q,&r));
 result("empty-proposal-original-Queen-choice",r.status==RHS_REPLACED&&r.mandatory&&r.selected[0].id==395,"No veto exists: original Queen is allowed to supply its mandatory choice.");
 sel[0]=11;sel[6]=1;Hal25_ForceApplied(mi(RE),0);request(&q);CHECK(p->evaluate(&q,&r));
 result("resident-provider-effective-OFF",v->rules()==p&&r.status==RHS_UNAVAILABLE&&!r.mandatory,"Saved preference remains ON; actual applied(0) is authoritative.");
 Hal25_ForceApplied(mi(RE),1);queen_setup();request(&q);CHECK(p->evaluate(&q,&r));
 result("owner-generation-new-commit",r.revision[0]!=epoch&&r.status==RHS_REPLACED,"OFF/ON clears original effect state; genuine new ritual commit, no lease/mark invention.");
 state_cases();
}
