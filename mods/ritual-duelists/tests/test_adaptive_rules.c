#include "historical_planner_fixture.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "adaptive_rules.h"
#include "pc/mods/modapi.h"

static int active=1,bad_abi,mode=0,find_enabled=1;
static RitualRulesProviderV1 provider;
static int read_rules(RitualRulesSnapshotV1 *out,unsigned size){
    if(!active || size!=sizeof(*out))return 0;
    out->abi_version=bad_abi?55:RITUAL_RULES_PROVIDER_ABI_V1;
    out->material_count=3;
    out->sources=3u+(mode?4u:0u);
    out->min_field=mode==1?2:1;
    out->min_hand=mode==1?1:0;
    out->require_result=mode==2?1:0;
    out->ritual_only_player=1;
    return 1;
}
static void *lookup(const MemoriesModHost *h,const char *id){
    (void)h;
    assert(strcmp(id,"melhoria-dos-rituais:rules_v1")==0);
    return find_enabled?&provider:0;
}
static RdCard c(int id,int atk){RdCard card={0};card.id=id;card.atk=atk;card.def=atk;card.type=3;return card;}
static RdBoard board(void){RdBoard b={0};b.actual_hand=b.hand_count=5;b.own_lp=b.enemy_lp=8000;return b;}
static RdAdaptiveRules rules(int field,int deck,int hand,int result){
    RdAdaptiveRules r={0};r.available=1;r.material_count=3;
    r.sources=3u+(deck?4u:0u);r.min_field=field;r.min_hand=hand;r.require_result=result;
    return r;
}
static RdPlan plan(const RdOpponentProfile *p,const RdBoard *b,const RdAdaptiveRules *r,int legal){
    RdPlan d=Rd_PlanRitual(p,0,b,0,legal);
    Rd_RefineRitualPlan(p,&p->recipe,b,legal,0,0,r,&d);
    return d;
}
int main(void){
    MemoriesModHost host={0};RdAdaptiveRules r,out;
    RdBoard b;RdPlan d;const RdOpponentProfile *p=historical_planner_fixture();
    assert(p);
    provider.abi_version=1;provider.read=read_rules;host.find=lookup;
    Rd_ReadAdaptiveRules(&host,&out);
    assert(out.available && out.min_field==1 && out.sources==3 && out.min_hand==0);
    mode=1;Rd_ReadAdaptiveRules(&host,&out);
    assert(out.available && out.min_field==2 && out.sources==7 && out.min_hand==1);
    assert(Rd_AdaptiveRuleKey(&out)!=Rd_AdaptiveRuleKey(&(RdAdaptiveRules){0}));
    bad_abi=1;Rd_ReadAdaptiveRules(&host,&out);assert(!out.available);bad_abi=0;
    active=0;Rd_ReadAdaptiveRules(&host,&out);assert(!out.available);active=1;
    find_enabled=0;Rd_ReadAdaptiveRules(&host,&out);assert(!out.available);find_enabled=1;
    provider.abi_version=3;Rd_ReadAdaptiveRules(&host,&out);assert(!out.available);provider.abi_version=1;
    /* The Isis recipe roles are distinct: #535, a Skull 2050..2499, a
       type-3 2050..2499. This fixture only uses known role categories. */
    b=board();b.actual_hand=b.hand_count=4;
    b.hand[0]=c(535,500);b.hand[0].type=9;
    b.hand[1]=c(545,1200);b.hand[1].type=1;
    b.hand[2]=c(440,1900);b.hand[2].type=3;
    b.back[0]=c(679,0);b.back[0].type=22;
    r=rules(1,0,0,0);
    d=plan(p,&b,&r,0);
    assert(d.setup_hand_slot>=0 && d.setup_destination==0);
    /* With no Ritual Magic, keep tribute monsters in hand instead of
       manufacturing Field fodder prematurely. */
    b.back[0].id=0;
    d=plan(p,&b,&r,0);assert(d.setup_hand_slot==-1);
    b.back[0]=c(679,0);b.back[0].type=22;
    b.own[0]=b.hand[0];b.hand[0].id=0;
    d=plan(p,&b,&r,0);assert(d.setup_hand_slot==-1);
    /* The minimum is configuration, not hardcoded to all three roles. */
    b.own[0].id=0;b.hand[0]=c(535,500);b.hand[0].type=9;
    r=rules(0,0,0,0);d=plan(p,&b,&r,0);assert(d.setup_hand_slot==-1);
    r=rules(3,0,0,0);d=plan(p,&b,&r,0);assert(d.setup_hand_slot>=0);
    /* An incomplete recipe does not expose the only weak tribute while
       waiting for another draw (except when the hand is full). */
    b.hand[2].id=0;r=rules(1,0,0,0);
    d=plan(p,&b,&r,0);assert(d.setup_hand_slot==-1);
    b.hand[2]=c(440,1900);b.hand[2].type=3;
    /* Require the result in physical hand: no premature setup when absent. */
    r=rules(1,0,0,1);d=plan(p,&b,&r,0);
    assert(d.setup_hand_slot==-1);
    b.hand[3]=c(704,2500);b.hand[3].type=1;
    d=plan(p,&b,&r,0);assert(d.setup_hand_slot>=0);
    /* With Deck on, one valid Field monster + Magic can be enough to
       initiate a safe source-aware Field setup without all drawn tributes. */
    b=board();b.actual_hand=b.hand_count=2;b.hand[0]=c(535,500);b.hand[0].type=9;
    b.back[0]=c(679,0);b.back[0].type=22;
    r=rules(1,1,0,0);d=plan(p,&b,&r,0);assert(d.setup_hand_slot==0);
    /* Field>=1, Hand>=1 and only one eligible material in hand: do not
       place that one on the Field and thereby break the Hand minimum. */
    r=rules(1,1,1,0);
    d=plan(p,&b,&r,0);assert(d.setup_hand_slot==-1);
    r=rules(1,1,1,0);
    /* A visible superior enemy means a delayed Deck Ritual is safer. */
    b.enemy[0]=c(400,4000);
    d=plan(p,&b,&r,0);assert(d.setup_hand_slot==-1);
    /* When legal NOW the engine, not our guessing, authorizes the Ritual. */
    d=plan(p,&b,&r,1);assert(d.setup_hand_slot==-1);
    /* No rules provider => legacy planner remains byte-for-byte in charge. */
    { RdPlan original=Rd_PlanRitual(p,0,&b,0,0);
      RdAdaptiveRules absent={0};
      d=plan(p,&b,&absent,0);
      assert(d.setup_hand_slot==original.setup_hand_slot);
    }
    /* When Field minimum is two, do not place a duplicate of an already
       represented exact-only role. Distinct roles matter more than raw count. */
    b=board();b.actual_hand=b.hand_count=3;
    b.own[0]=c(535,500);b.own[0].type=9;
    b.hand[0]=c(535,100);b.hand[0].type=9;
    b.hand[1]=c(545,1200);b.hand[1].type=1;
    b.hand[2]=c(440,1900);b.hand[2].type=3;
    b.back[0]=c(679,0);b.back[0].type=22;
    r=rules(2,0,0,0);d=plan(p,&b,&r,0);
    assert(d.setup_hand_slot==1 || d.setup_hand_slot==2);
    /* Already having two distinct Field tributes means no extra placement. */
    b.own[1]=b.hand[1];
    d=plan(p,&b,&r,0);assert(d.setup_hand_slot==-1);
    /* Reservation control: one Warrior on field is pivotal with Field>=1;
       two interchangeable Warriors are not individually pivotal. Also
       preserve progress when the Field minimum is 2 but only 1 is ready. */
    {
      RdPlan guarded;
      RdBoard reserve=board();
      reserve.actual_hand=reserve.hand_count=2;
      reserve.back[0]=c(679,0);reserve.back[0].type=22;
      reserve.own[0]=c(440,1900);reserve.own[0].type=3;
      reserve.hand[0]=c(535,500);reserve.hand[0].type=9;
      reserve.hand[1]=c(545,1200);reserve.hand[1].type=1;
      r=rules(1,0,0,0);guarded=plan(p,&reserve,&r,0);
      assert(guarded.protect_field[0]);
      r=rules(2,0,0,0);guarded=plan(p,&reserve,&r,0);
      assert(guarded.protect_field[0]);
      reserve.own[1]=c(441,1800);reserve.own[1].type=3;
      r=rules(1,0,0,0);guarded=plan(p,&reserve,&r,0);
      assert(!guarded.protect_field[0] && !guarded.protect_field[1]);
    }
    /* Exhaust every published combination, including contradictory settings.
       This invariant is generic: never suggest an invalid slot or a forced
       setup from a logical/nonphysical hand location. */
    { int f,d,h,req,n=0;
      for(f=0;f<=3;f++)for(d=0;d<=1;d++)for(h=0;h<=3;h++)for(req=0;req<=2;req++){
          RdAdaptiveRules cfg=rules(f,d,h,req);
          RdBoard sample=board();RdPlan result;
          sample.actual_hand=sample.hand_count=3;
          sample.hand[0]=c(535,500);sample.hand[0].type=9;
          sample.hand[1]=c(545,1200);sample.hand[1].type=1;
          sample.hand[2]=c(440,1900);sample.hand[2].type=3;
          sample.back[0]=c(679,0);sample.back[0].type=22;
          result=plan(p,&sample,&cfg,0);
          assert((result.setup_hand_slot>=0 && result.setup_hand_slot<sample.actual_hand &&
                  result.setup_destination>=0 && result.setup_destination<5) ||
                 (result.setup_hand_slot==-1 && result.setup_destination==-1));
          if(!f || f+(d?h:0)>3 || req==1)
              assert(result.setup_hand_slot==-1);
          n++;
      }
      assert(n==96);
    }
    printf("PASS adaptive rules: live provider, ON/OFF, ABI, 0-3 Field, Hand/Deck, result, risk and fallback\n");
    return 0;
}
