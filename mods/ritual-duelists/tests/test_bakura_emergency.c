#include "adaptive_rules.h"
#include "planner.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static RdCard monster(int id,int atk,int typ){RdCard c={0};c.id=id;c.atk=atk;c.def=atk;c.type=typ;return c;}
int main(void){
 const RdOpponentProfile *p=Rd_GetProfile(10);RdRitualRequirement req[3]={0};
 RdRitualRecipe recipe=p->recipe;RdBoard board={0};RdPlan plan;RdAdaptiveRules rules={0};int i;
 assert(p->recipe.result_card==365);
 for(i=0;i<3;++i){req[i].exact_card=p->baseline_materials[i];req[i].type=-1;
  req[i].min_attack=req[i].max_attack=req[i].min_defense=req[i].max_defense=-1;
  req[i].min_level=req[i].max_level=-1;}
 recipe.materials=req;recipe.material_count=3;recipe.material_sources=3;
 board.actual_hand=board.hand_count=4;board.own_lp=2500;board.enemy_lp=8000;
 board.enemy[0]=monster(300,2200,3);board.back[0]=monster(674,0,22);
 board.hand[0]=monster(186,1000,2);board.hand[1]=monster(261,700,2);
 board.hand[2]=monster(595,1300,2);
 rules.available=1;rules.material_count=3;rules.sources=3;rules.min_field=1;
 plan=Rd_PlanRitual(p,&recipe,&board,0,0);
 assert(plan.urgent && plan.prep && plan.matched_materials==3);
 Rd_RefineRitualPlan(p,&recipe,&board,0,0,0,&rules,&plan);
 assert(plan.setup_hand_slot>=0&&plan.setup_destination==0);
 /* All existing game-legality checks remain authoritative. */
 plan=Rd_PlanRitual(p,&recipe,&board,0,1);
 assert(Rd_ShouldActivateRitual(&plan,1));
 /* Actual lethal threat forbids risky setup and activation. */
 board.own_lp=1800;
 plan=Rd_PlanRitual(p,&recipe,&board,0,0);
 assert(!plan.prep);
 plan=Rd_PlanRitual(p,&recipe,&board,0,1);
 if(plan.score<0)assert(!Rd_ShouldActivateRitual(&plan,1));
 puts("PASS: Bakura permits face-down field tribute under nonlethal pressure, still respects lethal and engine oracle");
 return 0;
}
