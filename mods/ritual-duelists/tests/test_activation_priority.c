#include "ritual_duelists.h"
#include "planner.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static RdCard card(int id,int atk,int def,int type)
{
    RdCard c={0};c.id=id;c.atk=atk;c.def=def;c.type=type;return c;
}
static RdRitualRecipe vanilla(const RdOpponentProfile *p,RdRitualRequirement req[3])
{
    RdRitualRecipe r=p->recipe;
    int i;
    memset(req,0,3*sizeof(*req));
    for(i=0;i<3;i++){
        req[i].exact_card=p->baseline_materials[i];
        req[i].type=-1;
        req[i].min_attack=req[i].max_attack=-1;
        req[i].min_defense=req[i].max_defense=-1;
        req[i].min_level=req[i].max_level=-1;
    }
    r.materials=req;r.material_count=3;r.material_sources=3;
    return r;
}
static void verify(int profile,int ritual,int expected_result)
{
    const RdOpponentProfile *p=Rd_GetProfile(profile);
    RdRitualRequirement req[3];
    RdRitualRecipe recipe;
    RdBoard b={0};
    RdPlan plan;
    assert(p&&p->recipe.ritual_magic==ritual&&p->recipe.result_card==expected_result);
    recipe=vanilla(p,req);
    b.actual_hand=b.hand_count=5;
    b.own_lp=b.enemy_lp=8000;
    /* Strong non-tribute monster masks the Ritual's *printed* value; the
       active engine checker alone has seen Deck/Hand materials. */
    b.own[0]=card(300,3500,3200,1);
    b.back[0]=card(ritual,0,0,22);
    plan=Rd_PlanRitual(p,&recipe,&b,1,1);
    assert(!plan.urgent&&plan.score<0);
    assert(plan.ritual_back_slot==0&&plan.trigger==2);
    assert(Rd_ShouldActivateRitual(&plan,1));
    /* Neither AI enthusiasm nor a set magic may override game legality. */
    plan=Rd_PlanRitual(p,&recipe,&b,1,0);
    assert(plan.trigger!=2 && !Rd_ShouldActivateRitual(&plan,0));
    /* If only in physical Hand, set the Magic rather than 'activate'. */
    b.back[0].id=0;b.hand[0]=card(ritual,0,0,22);
    plan=Rd_PlanRitual(p,&recipe,&b,1,1);
    assert(plan.trigger==1 && !Rd_ShouldActivateRitual(&plan,1));
    /* Don't force a strategic disadvantage when defending against lethal. */
    b.back[0]=b.hand[0];b.hand[0].id=0;
    b.own_lp=100;
    b.enemy[0]=card(1,2000,2000,1);
    plan=Rd_PlanRitual(p,&recipe,&b,1,1);
    assert(plan.urgent);
    if(plan.score<0)assert(!Rd_ShouldActivateRitual(&plan,1));
}
int main(void)
{
    verify(10,674,365); /* Bakura Fiend's Mirror, ritual-only effects */
    verify(11,695,715); /* Pegasus Psycho-Puppet steals a monster */
    verify(12,665,362); /* Mage Soldier's defensive Shield also works */
    verify(8,680,705); /* Martis Tri-Horned regression */
    puts("PASS: legal negative-value Rituals are activated across profiles; illegal, unset and lethal emergencies remain protected");
    return 0;
}
