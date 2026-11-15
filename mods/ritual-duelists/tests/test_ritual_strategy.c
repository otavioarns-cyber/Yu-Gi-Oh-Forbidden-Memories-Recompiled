#include "historical_planner_fixture.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ritual_duelists.h"
#include "planner.h"

static RdBoard board(void)
{
    RdBoard b;
    memset(&b,0,sizeof b);
    b.actual_hand=b.hand_count=5;
    b.own_lp=b.enemy_lp=8000;
    return b;
}
static RdCard c(int id,int atk,int def,int type){
    RdCard v={0};v.id=id;v.atk=atk;v.def=def;v.type=type;return v;
}
int main(void)
{
    const RdOpponentProfile *p=historical_planner_fixture();
    RdBoard b;RdPlan plan;
    assert(p && Rd_IsSkull(p,22));
    /* Set early even when not a single sacrifice is present. */
    b=board(); b.hand[0]=c(679,0,0,22);
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.ritual_goal&&plan.prep&&!plan.near);
    assert(plan.trigger==1&&plan.ritual_back_destination==0);
    assert(plan.matched_materials==0);
    /* No magic yet: the material goal is still present. */
    b.hand[0]=c(535,400,300,9);
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.ritual_goal&&plan.prep&&plan.matched_materials==1);
    assert(plan.setup_hand_slot==0&&plan.setup_destination==0);
    assert(plan.trigger==0);
    /* Prepare an exact field tribute, not a strong non-tribute Skull. */
    b.hand[0]=c(679,0,0,22);
    b.hand[1]=c(22,2500,1200,1);
    b.hand[2]=c(535,400,300,9);
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.setup_hand_slot==2);
    assert(plan.protect_hand[1]); /* buff supporter, NOT a tribute */
    assert(plan.trigger==1);
    b.back[0]=b.hand[0];b.hand[0]=c(0,0,0,0);
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.setup_hand_slot==2);
    assert(!plan.trigger);
    /* After 20 turns we still actively prepare; no artificial timeout. */
    b.turns_waited=20;
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.prep&&plan.setup_hand_slot==2);
    /* If all field zones are full, never request an impossible play. */
    for(int i=0;i<5;i++)b.own[i]=c(9+i,1000,1000,1);
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.setup_hand_slot==-1);
    /* Legal by Deck-sourced tributes: two ineligible Summoned Skulls on
       field remain buffs, and must not be treated as consumable tributes. */
    b=board();b.hand[0]=c(679,0,0,22);
    b.own[0]=c(22,2500,1200,1);
    b.own[1]=c(22,2500,1200,1);
    b.enemy[0]=c(239,4500,3800,1);
    plan=Rd_PlanRitual(p,0,&b,0,1);
    assert(!plan.legal_set);
    assert(plan.predicted_attack==4050 && plan.predicted_defense==4500);
    assert(plan.live_skulls==2);
    assert(plan.trigger==1); /* place the Magic first */
    b.back[0]=b.hand[0];b.hand[0]=c(0,0,0,0);
    plan=Rd_PlanRitual(p,0,&b,0,1);
    assert(plan.trigger==2 && plan.score>=0);
    /* The authoritative checker is a strict gate, even with big buffs. */
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.trigger!=2);
    /* Emergency: let the existing AI defend rather than force preparation. */
    b=board();b.hand[0]=c(679,0,0,22);
    b.own_lp=100;b.enemy[0]=c(239,4500,3800,1);
    plan=Rd_PlanRitual(p,0,&b,0,0);
    assert(plan.urgent && !plan.prep && plan.trigger!=1);
    /* Martis regression: Vanilla exact package provides #6/#47/#571.
       Once the game checker confirms the Ritual, a powerful unrelated
       monster should not indefinitely prevent the Tri-Horned summon. */
    {const RdOpponentProfile *martis=Rd_GetProfile(8);
     RdRitualRequirement parts[3]={{0}};
     RdRitualRecipe recipe;
     assert(martis&&martis->recipe.result_card==705);
     recipe=martis->recipe;
     parts[0].exact_card=6;parts[1].exact_card=47;parts[2].exact_card=571;
     for(int i=0;i<3;i++){
         parts[i].type=-1;parts[i].min_attack=parts[i].max_attack=-1;
         parts[i].min_defense=parts[i].max_defense=-1;
         parts[i].min_level=parts[i].max_level=-1;
     }
     recipe.materials=parts;recipe.material_count=3;
     b=board();b.back[0].id=680;
     b.own[0]=c(6,1300,1400,7);
     b.own[1]=c(300,3500,3000,1); /* not a tribute */
     plan=Rd_PlanRitual(martis,&recipe,&b,1,1);
     assert(!plan.urgent && plan.score<0 && plan.trigger==2);
     plan=Rd_PlanRitual(martis,&recipe,&b,1,0);
     assert(plan.trigger!=2); /* must remain engine legal */
    }
    puts("PASS: ritual planner including Martis legal/high-power board regression");
    return 0;
}
