#include "model.h"
static int ea_enabled(void){return recipe_export_active&&host&&host->setting(host,"effect_ai",1);}
static void ea_inspect(EaCard*c,int s,int viewer){
 if(!ea_enabled())return;int id=c->id,live=monster_slot(s)&&D_801A7AD8[s].card_id==id&&(D_801A7AD8[s].flags&DUEL_CARD_FLAG_OCCUPIED);
 if(RitualCategory_IsSkullMonster(id))c->traits|=EA_SKULL;
 if(live){for(int p=0;p<30;p++){const PsychoMark*m=&psycho_marks[p];if(!m->controlling)continue;
 if(D_801A7AD8[s].data==m->stolen_data)c->borrowed=1;
 if(D_801A7AD8[s].data==m->psycho_data&&s/15!=viewer)for(int j=0;j<5;j++){int t=D_800907D8[viewer*20+9-j];if((D_801A7AD8[t].flags&DUEL_CARD_FLAG_OCCUPIED)&&D_801A7AD8[t].data==m->stolen_data)c->redirect=j+1;}}
 if(s/15!=viewer&&ritual_ai_knows_effect(viewer,356))for(int j=0;j<5;j++)if(D_800907D8[viewer*20+9-j]==s){int t=war_lion_ai_known_source(j+56);for(int k=0;k<5;k++)if(D_800907D8[viewer*20+9-k]==t)c->redirect=k+1;}}
 if(!ritual_effect_ai_enabled(id))return;
 if(live&&s/15!=viewer&&(id==380||id==706||id==356||id==719||id==703||id==720)&&!(id==720?(mask_known_sides&(1<<viewer)):ritual_ai_knows_effect(viewer,id)))return;
 c->possible|=EA_RITUAL;if(live?(!(c->flags&4096)&&borrowed_effect_armed(s)):test_ignore_ritual_requirement)c->active|=EA_RITUAL;
 if(id==533)return;if(id==717)c->charge=!live||!javelin_marks[s].protection_used_turn;
 if(id==705)c->charge=live?tri_marks[s].horns:3;if(id==374)c->charge=live?3-(int)gate_marks[s].attacks_this_turn:3;
 if(id==716)c->charge=live?5-(int)garma_marks[s].blades_used:5;}
static const EaProvider ea_owner={2,ea_enabled,ea_inspect,0,0};
