#ifndef RD_PLANNER_H
#define RD_PLANNER_H
#include "ritual_duelists.h"
#define RD_MONSTER 5
#define RD_HAND 20
#define RD_PHYSICAL_CANDIDATES 10 /* 5 field + 5 ACTUAL hand slots */
#define RD_SEARCH_BUDGET 4000       /* no factorial explosion per AI call */
typedef struct {int id,atk,def,type,face_down,level,used;} RdCard;
struct RdBoard {
 RdCard own[5],enemy[5],hand[20],back[5];
 int hand_count,own_lp,enemy_lp,terrain,actual_hand,turns_waited;
};
typedef struct {
 int prep,legal_set,score,live_skulls,hand_skulls,near,urgent;
 int ritual_goal,matched_materials,predicted_attack,predicted_defense;
 int setup_hand_slot,setup_destination;
 unsigned material_count;
 int tribute[RD_MAX_RITUAL_MATERIALS]; /* valid first material_count only */
 int protect_hand[5],protect_field[5];
 int ritual_hand_slot,ritual_back_slot,ritual_back_destination,prefer_hand_slot,prefer_destination;
 int trigger; /* 0 normal, 1 prefer placing magic, 2 activate from back */
} RdPlan;
/* Pure logic. The recipe can be replaced by an auto-detected one when a
 * provider is available; NULL falls back to profile's declared recipe. */
RdPlan Rd_PlanRitual(const RdOpponentProfile *p,const RdRitualRecipe *active_recipe,
                     const RdBoard *b,int effect_mode,int ritual_is_legal);
/* Production planner shares the exact same official/provider recipe matcher
   with deck generation. A NULL matcher preserves the portable test fallback. */
RdPlan Rd_PlanRitualMatched(const RdOpponentProfile *p,const RdRitualRecipe *active_recipe,
                     const RdBoard *b,int effect_mode,int ritual_is_legal,
                     RdMaterialMatch match,void *context);
/* Single activation gate shared with the final AI hook: NEVER duplicate
   a printed-stat-score restriction after the engine has confirmed legality. */
int Rd_ShouldActivateRitual(const RdPlan *plan,int engine_legal);
#endif
