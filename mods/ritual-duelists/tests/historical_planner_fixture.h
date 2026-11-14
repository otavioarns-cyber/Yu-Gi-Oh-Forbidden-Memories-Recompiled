/* Legacy planner regression fixture: the OLD Isis ritual mechanics are tested
 * with a synthetic profile, never as an active opponent. Isis itself is now
 * entirely unmanaged/Vanilla in the runtime. Skull group is still used by Sebek. */
#ifndef RD_HISTORICAL_PLANNER_FIXTURE_H
#define RD_HISTORICAL_PLANNER_FIXTURE_H
#include "ritual_duelists.h"
static const RdOpponentProfile *historical_planner_fixture(void)
{
    static RdOpponentProfile mock;
    static const RdRitualRequirement requirements[]={
        {535,RD_ANY,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY},
        {0,RD_ANY,1,RD_ANY,2049,RD_ANY,2499,RD_ANY,RD_ANY},
        {0,3,0,RD_ANY,2049,RD_ANY,2499,RD_ANY,RD_ANY}
    };
    static const u16 baseline[]={535,545,378};
    mock=*Rd_GetProfile(23); /* Sebek owns the valid 21-Skull ID matcher. */
    mock.duelist_name="Historical planner fixture, NOT Isis runtime";
    mock.recipe.ritual_magic=679;
    mock.recipe.result_card=704;
    mock.recipe.result_attack=2050;
    mock.recipe.result_defense=2500;
    mock.recipe.materials=requirements;
    mock.recipe.material_count=3;
    mock.baseline_materials=baseline;
    mock.baseline_material_count=3;
    return &mock;
}
#endif
