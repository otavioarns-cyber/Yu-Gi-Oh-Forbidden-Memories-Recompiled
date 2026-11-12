#include "historical_planner_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ritual_duelists.h"
#include "planner.h"
static unsigned x=0x12345678;
static int rng(int n,void *unused){(void)unused;x^=x<<13;x^=x>>17;x^=x<<5;return (int)(x%(unsigned)n);}
static int expect_trio(const RdOpponentProfile *p,int field_kamak)
{
 RdBoard b={0};RdPlan z;
 b.actual_hand=5;b.hand_count=5;b.own_lp=8000;b.enemy_lp=8000;
 b.hand[0]=(RdCard){679,0,0,22,0};
 b.hand[1]=(RdCard){field_kamak?440:535,100,100,3,0};
 b.hand[2]=(RdCard){545,1700,1900,2,0};
 b.hand[3]=(RdCard){378,2000,1600,3,0};
 b.hand[4]=(RdCard){704,2050,2500,3,0};
 if(field_kamak)b.own[0]=(RdCard){535,600,500,9,0};
 z=Rd_PlanRitual(p,0,&b,0,1);
 return z.legal_set && z.tribute[0]>=0 && z.tribute[1]!=z.tribute[2] && z.hand_skulls==1;
}
int main(void)
{
 const RdOpponentProfile *p=historical_planner_fixture();int i;
 assert(p&&Rd_ProfileCount()==29);
 assert(Rd_GetProfile(29)==0);
 assert(Rd_IsSkull(p,228)&&Rd_IsSkull(p,704)&&!Rd_IsSkull(p,535));
 assert(expect_trio(p,0)&&expect_trio(p,1));
 {
 RdBoard b={0};RdPlan z;
 b.actual_hand=5;b.hand_count=5;b.own_lp=8000;
 b.hand[0]=(RdCard){679,0,0,22,0};b.hand[1]=(RdCard){535,800,500,9,0};
 b.hand[2]=(RdCard){545,1700,1900,2,0};b.hand[3]=(RdCard){378,2000,1600,3,0};
 b.hand[4]=(RdCard){704,2050,2500,3,0};
 z=Rd_PlanRitual(p,0,&b,0,1);
 assert(z.legal_set);assert(z.tribute[0]!=z.tribute[1]&&z.tribute[1]!=z.tribute[2]);
 b.hand[3]=(RdCard){217,3200,2800,0,0};z=Rd_PlanRitual(p,0,&b,0,1);assert(!z.legal_set);
 }
 {
  RdBoard b={0};RdPlan z;
  b.actual_hand=5;b.hand_count=5;b.own_lp=8000;b.enemy_lp=8000;
  b.hand[0]=(RdCard){679,0,0,22,0};
  b.hand[1]=(RdCard){535,800,500,9,0};
  b.hand[2]=(RdCard){545,1700,1900,2,0};
  b.hand[3]=(RdCard){378,2000,1600,3,0};
  b.hand[4]=(RdCard){9,300,200,2,0};
  z=Rd_PlanRitual(p,0,&b,0,0);
  assert(z.near&&z.legal_set&&z.trigger==1&&z.ritual_back_destination==0);
  assert(z.protect_hand[0]&&z.protect_hand[1]&&z.protect_hand[2]&&z.protect_hand[3]);
  assert(z.hand_skulls>=1); /* remaining Skull contributes after consuming Skelgon */
  b.own_lp=50;b.enemy[0]=(RdCard){380,2000,2000,0,0};
  z=Rd_PlanRitual(p,0,&b,0,0);assert(z.urgent&&!z.prep);
  b.own_lp=8000;b.enemy[0]=(RdCard){0,0,0,0,0};b.turns_waited=5;
  z=Rd_PlanRitual(p,0,&b,0,0);assert(z.prep && z.ritual_goal);
  b.turns_waited=0;b.hand[0]=(RdCard){334,0,0,20,0};
  z=Rd_PlanRitual(p,0,&b,0,0);assert(z.near && z.prep);
 }
 /* Future-opponent portability: entirely new recipe using ONLY the profile
    descriptor (no changes whatsoever to planner.c or the live Isis profile). */
 {
   RdOpponentProfile q=*p;RdRitualRequirement qmaterials[3];RdBoard b={0};RdPlan z;
   q.recipe.materials=qmaterials;q.recipe.material_count=3;
   q.recipe.ritual_magic=694;q.recipe.result_card=709;q.recipe.result_attack=2450;q.recipe.result_defense=2000;
   qmaterials[0]=(RdRitualRequirement){369,RD_ANY,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY};
   qmaterials[1]=(RdRitualRequirement){0,1,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,7,RD_ANY};
   qmaterials[2]=(RdRitualRequirement){0,1,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,7,RD_ANY};
   q.evaluate_effect=0;
   b.actual_hand=4;b.hand_count=4;b.own_lp=8000;b.enemy_lp=8000;
   b.hand[0]=(RdCard){694,0,0,22,0,0};
   b.hand[1]=(RdCard){369,1600,3000,1,0,7};
   b.hand[2]=(RdCard){22,2500,1200,1,0,7};
   b.hand[3]=(RdCard){154,2200,1700,1,0,7};
   z=Rd_PlanRitual(&q,0,&b,0,1);
   assert(z.legal_set&&z.tribute[0]==6);
   assert(z.tribute[1]!=z.tribute[2]);
   b.hand[3].level=6;z=Rd_PlanRitual(&q,0,&b,0,1);
   assert(!z.legal_set);
 }
 /* Variable number of material roles is a recipe property, never a
    hardcoded property of the opponent/planner. The physical game checker
    still decides whether a summon is legal. */
 {
    RdOpponentProfile q=*p;RdBoard b={0};RdPlan z;
    RdRitualRecipe recipe=p->recipe;
    RdRitualRequirement roles[RD_MAX_RITUAL_MATERIALS];
    int count,c,i,j,seen[10];
    b.own_lp=8000;b.enemy_lp=8000;b.actual_hand=5;b.hand_count=5;
    b.hand[0]=(RdCard){679,0,0,22,0,0};
    /* Ten distinct possible material instances, five field and four hand. */
    for(i=0;i<5;i++)b.own[i]=(RdCard){100+i,100+i,100+i,3,0,4};
    for(i=1;i<5;i++)b.hand[i]=(RdCard){110+i,100+i,100+i,3,0,4};
    q.evaluate_effect=0;recipe.materials=roles;
    for(i=0;i<RD_MAX_RITUAL_MATERIALS;i++)
        roles[i]=(RdRitualRequirement){0,3,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY};
    for(count=0;count<=10;count++){
        recipe.material_count=(unsigned)count;
        z=Rd_PlanRitual(&q,&recipe,&b,0,1);
        assert(z.material_count==(unsigned)count);
        if(count<=9){
            assert(z.legal_set);
            memset(seen,0,sizeof(seen));
            for(j=0;j<count;j++){
                c=z.tribute[j];assert(c>=0&&c<10);assert(!seen[c]);seen[c]=1;
            }
        }else assert(!z.legal_set);
    }
    /* With zero materials, the engine's actual legality still controls
       activation. Without a valid checker response it must not activate. */
    recipe.material_count=0;
    z=Rd_PlanRitual(&q,&recipe,&b,0,0);assert(z.legal_set&&z.trigger!=2);
    b.back[0]=(RdCard){679,0,0,22,0,0};
    z=Rd_PlanRitual(&q,&recipe,&b,0,1);assert(z.trigger==2);
    /* One exact card and repeated broad roles: unique matching finds the
       single exact card first and cannot re-use it in another role. */
    roles[0]=(RdRitualRequirement){101,RD_ANY,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY};
    recipe.material_count=4;
    z=Rd_PlanRitual(&q,&recipe,&b,0,1);
    assert(z.legal_set);
    assert(z.tribute[0]==1);
    for(i=1;i<4;i++)assert(z.tribute[i]!=1);
    /* Impossible recipes and unsafe counts never cause a hot-loop. */
    recipe.material_count=RD_MAX_RITUAL_MATERIALS;
    z=Rd_PlanRitual(&q,&recipe,&b,0,0);
    assert(!z.legal_set&&!z.trigger); /* oracle rejects */
    z=Rd_PlanRitual(&q,&recipe,&b,0,1);
    assert(!z.legal_set&&z.trigger==2); /* external MOD oracle accepts Deck tributes */
    recipe.material_count=RD_MAX_RITUAL_MATERIALS+1u;
    z=Rd_PlanRitual(&q,&recipe,&b,0,1);
    assert(!z.legal_set);
    recipe.material_count=5;recipe.materials=0;
    z=Rd_PlanRitual(&q,&recipe,&b,0,1);
    assert(!z.legal_set);
    /* No local materials: the active game's oracle may recognize all
       tributes in the Deck. The planner must not reject that legal ritual. */
    {
        RdBoard external={0};RdPlan z2;int j;
        external.actual_hand=1;external.hand_count=20;
        external.own_lp=8000;external.enemy_lp=8000;
        external.hand[0]=(RdCard){679,0,0,22,0,0};
        for(j=1;j<20;j++)external.hand[j]=(RdCard){100+j,1600,1500,3,0,5};
        z2=Rd_PlanRitual(p,&p->recipe,&external,0,1);
        assert(!z2.legal_set && z2.trigger==1 && z2.ritual_hand_slot==0);
        z2=Rd_PlanRitual(p,&p->recipe,&external,0,0);
        assert(!z2.legal_set && z2.trigger==1); /* proactive setting is legal even before tribute match */
        external.hand[0].id=0;external.back[0]=(RdCard){679,0,0,22,0,0};
        z2=Rd_PlanRitual(p,&p->recipe,&external,0,1);
        assert(!z2.legal_set && z2.trigger==2);
        external.back[0].used=1;
        z2=Rd_PlanRitual(p,&p->recipe,&external,0,1);
        assert(z2.trigger==0); /* spent Ritual must not be activated */
        external.back[0].used=0;
        external.back[0].id=0;
        z2=Rd_PlanRitual(p,&p->recipe,&external,0,1);
        assert(z2.trigger==0);
        /* A strong non-tribute monster is NOT evidence against a legal
           Deck-sourced Ritual: special effects matter beyond printed ATK.
           The caller still preserves an already lethal battle attack. */
        external.back[0]=(RdCard){679,0,0,22,0,0};
        external.own[0]=(RdCard){22,4500,2500,1,0,7};
        z2=Rd_PlanRitual(p,&p->recipe,&external,0,1);
        assert(!z2.legal_set && z2.score<0 && z2.trigger==2);
    }
    puts("PASS variable recipe sizes 0..10; engine-authoritative Deck tribute source; zero/no-source safety");
 }
 printf("PASS legacy Isis planner regression, 21 skulls, ritual distinctness, protection, urgency and persistent ritual setup\n");
 return 0;
}
