#include "hand_selection_snapshot.h"
#include "mandatory_hand_lifecycle_v1.h"
/* Real owner implementation. Evaluation calls only original Queen predicates
 * and writes caller-owned response bytes, never live selection/deck/records. */
static void selection_revision(unsigned*r)
{
 unsigned live=0,pending=cosmo_live_mask;
 while(pending){int i=take_mask_bit(&pending);if(cosmo_live(i))live|=1u<<i;}
 r[0]=selection_epoch;r[1]=live;
 r[2]=(recipe_export_active?1u:0u)|(selection_ready?2u:0u)|
 (ritual_card_effect_enabled(COSMO_QUEEN_ID)?4u:0u)|
 (cosmo_turn_fulfilled[0]?8u:0u)|(cosmo_turn_fulfilled[1]?16u:0u);
}
static int selection_evaluate(const RhsRequest*q,RhsResponse*r)
{
 int mandatory,n;RhsResponse out;
 if(selection_epoch==UINT32_MAX&&!selection_ready)return 0;
 if(!q||!r||q->abi!=RHS_ABI||q->size!=sizeof*q||
 q->state.size!=sizeof q->state||!q->state.action||!q->state.generation||
 q->state.side!=(int)D_8009B1D5||!rhs_live(&q->state)||
 q->state.phase!=4||D_8009B360[q->state.side]<0||q->intent>RHS_CANCEL_INTENT)return 0;
 memset(&out,0,sizeof out);out.abi=RHS_ABI;out.size=sizeof out;
 out.action=q->state.action;out.generation=q->state.generation;
 out.side=q->state.side;memcpy(out.selection,q->state.selection,12);
 selection_revision(out.revision);out.status=RHS_UNAVAILABLE;
 if(selection_ready&&recipe_export_active){
  if(q->intent==RHS_CANCEL_INTENT){out.status=RHS_CANCELLED;out.mandatory=1;}
  else{
   mandatory=cosmo_constrain_selection(q->state.side,q->state.selection,out.selection);
   out.mandatory=mandatory;
   out.status=memcmp(out.selection,q->state.selection,12)?RHS_REPLACED:RHS_UNCHANGED;
   if(mandatory){n=rhs_selected(&q->state,out.selection,out.selected);
    if(n<0)out.status=RHS_CANCELLED;else out.count=n;}
  }
 }
 out.destination=out.selection[6];*r=out;return 1;
}
static const RhsRules selection_rules={RHS_ABI,sizeof(RhsRules),selection_evaluate};
static const RhsRendezvous*selection_rendezvous(void)
{
 const RhsRendezvous*p=host&&host->find?host->find(host,RHS_RENDEZVOUS):0;
 return p&&p->abi==RHS_ABI&&p->size==sizeof*p&&p->connect&&p->rules&&p->precommit?p:0;
}
static void selection_connect(void)
{
 const RhsRendezvous*p=selection_rendezvous();
 if((selection_ready||selection_epoch==UINT32_MAX)&&p&&p->connect(&selection_rules)&&!selection_ready)selection_notify();
}
static int selection_precommit(void)
{
 const RhsRendezvous*p=selection_rendezvous();
 if(!p||p->rules()!=&selection_rules)return 0;
 return p->precommit();
}

/* Optional companion. The frozen ABI1 table/packet sizes do not change. */
static const RhsLifecycle*selection_lifecycle(void)
{
 const RhsLifecycle*p=host&&host->find?host->find(host,RHS_LIFECYCLE_KEY):0;
 return p&&p->abi==1&&p->size==sizeof*p&&p->magic==RHS_LIFECYCLE_MAGIC&&p->read&&p->disconnect&&p->invalidate&&p->recover?p:0;
}
static void selection_notify(void)
{const RhsLifecycle*p=selection_lifecycle();if(p)p->invalidate(&selection_rules,selection_epoch==UINT32_MAX&&!selection_ready?RHS_CAUSE_COUNTER:RHS_CAUSE_OWNER);}
static void selection_detach(void)
{const RhsLifecycle*p=selection_lifecycle();if(p)p->disconnect(&selection_rules);}
