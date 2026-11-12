#include "historical_planner_fixture.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "post_ritual.h"
#include "game/card_constants.h"

static RdCard card(int id,int atk,int def,int type)
{
    RdCard x={0};x.id=id;x.atk=atk;x.def=def;x.type=type;return x;
}
static RdBoard board(void)
{
    RdBoard b={0};b.actual_hand=b.hand_count=5;
    b.own[1]=card(704,4050,4500,1);
    b.hand[0]=card(22,2500,1200,1);
    b.hand[1]=card(24,300,200,1);
    b.hand[2]=card(679,0,0,CARD_TYPE_MAGIC);
    b.own_lp=b.enemy_lp=8000;
    return b;
}
/* Different Ritual, different support rule. This proves the generic planner
 * uses the profile-provided callback, not hardcoded Skull/Isis identities. */
static RdEffectEstimate custom_effect(const RdOpponentProfile *p,int result_card,const RdBoard *b,
                                     const int *slots,unsigned count,int mode)
{
    RdEffectEstimate e={0};int i;
    (void)p;(void)result_card;(void)slots;(void)count;
    if(mode)return e;
    for(i=0;i<5;i++)if(b->own[i].id==444)e.attack_bonus+=900;
    return e;
}
int main(void)
{
    const RdOpponentProfile *isis=historical_planner_fixture();
    RdRitualRecipe ritual=isis->recipe;
    RdPostPlan p; RdBoard b=board(); int face=0;
    assert(ritual.result_card==704);
    p=Rd_PlanPostRitual(isis,&ritual,&b,0);
    assert(p.active&&p.guardian_count==1&&p.protect_field[1]);
    assert(p.hand_slot==0&&p.destination==0);
    assert(p.attack_gain==500&&p.defense_gain==500);
    assert(p.score>0&&!p.face_up);
    /* The high-ATK support is preferred over a fragile small Skull. */
    b.enemy[0]=card(44,1800,1600,1);
    p=Rd_PlanPostRitual(isis,&ritual,&b,0);
    assert(p.hand_slot==0&&p.face_up); /* keeps Summoned Skull alive */
    /* A large hostile monster deters risky support deployment. */
    b.enemy[0]=card(45,4500,4000,1);
    p=Rd_PlanPostRitual(isis,&ritual,&b,0);
    assert(p.active&&p.hand_slot==-1);
    /* A hidden enemy does not grant our AI knowledge of its actual power. */
    b.enemy[0].face_down=1;
    p=Rd_PlanPostRitual(isis,&ritual,&b,0);
    assert(p.hand_slot==0);
    /* No actual Ritual in the monster row => no Post-Ritual state. */
    b=board();b.own[1].id=0;
    p=Rd_PlanPostRitual(isis,&ritual,&b,0);
    assert(!p.active&&p.hand_slot==-1);
    b=board();b.own[1].face_down=1;
    p=Rd_PlanPostRitual(isis,&ritual,&b,0);
    assert(!p.active);
    /* Vanilla/disabled Beacon mode retains the existing AI decisions. */
    b=board();p=Rd_PlanPostRitual(isis,&ritual,&b,1);
    assert(p.active&&p.hand_slot==-1);
    /* No free zone: never replace Ritual result or invent an extra play. */
    b=board();for(int i=0;i<5;i++)if(i!=1)b.own[i]=card(54,1000,1000,1);
    p=Rd_PlanPostRitual(isis,&ritual,&b,0);
    assert(p.active&&p.destination==-1&&p.hand_slot==-1);
    /* Deck-side candidate uses the real expendable hand card as the swap
       cost. It is not a new or conjured card. */
    b=board();b.hand[0]=card(111,300,300,1);
    assert(Rd_PostSupportScore(isis,&ritual,&b,0,0,&(RdCard){22,2500,1200,1,0,0,0},0,&face)>0);
    assert(Rd_PostSupportScore(isis,&ritual,&b,0,0,&(RdCard){90,1200,1200,1,0,0,0},0,&face)==0);
    /* Generic profile callback supports other Rituals without modifications
       to post_ritual.c. */
    {
        RdOpponentProfile other={0};
        RdRitualRecipe target={0};
        b=board();b.own[1]=card(800,2800,2000,1);
        b.hand[0]=card(444,1800,1500,1);
        b.hand[1]=card(22,2500,1200,1);
        target.result_card=800;other.evaluate_effect=custom_effect;
        p=Rd_PlanPostRitual(&other,&target,&b,0);
        assert(p.active&&p.hand_slot==0&&p.attack_gain==900);
    }
    puts("PASS: existing Ritual, Skull Guardian +500/+500 from Summoned Skull, risk, source safety and generic Beacon callback");
    return 0;
}
