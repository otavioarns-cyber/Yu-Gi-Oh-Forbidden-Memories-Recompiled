#include "model.h"
#include "game/duel_grid.h"
static const EaProvider *ea_fapi,*ea_rapi;
static void ea_connect(const MemoriesModHost*h){ea_fapi=h->find?h->find(h,"fusion-auxiliary-balance:effect_forecast_v2"):0;ea_rapi=h->find?h->find(h,"ritual-card-effects-test:effect_forecast_v2"):0;
 if(ea_fapi&&(ea_fapi->abi!=2||!ea_fapi->on()))ea_fapi=0;if(ea_rapi&&(ea_rapi->abi!=2||!ea_rapi->on()))ea_rapi=0;}
static EaCard ea_card(int id,int s,int side,int flags){
 EaCard c={0};if(!Cards_Valid(id))return c;
 unsigned x=gDuel_adwCardStats[id-1];c.id=id;c.flags=flags;c.t=Cards_Type(id);c.level=Cards_Level(id);
 c.at=c.printed=(x&511)*10;c.df=c.pdef=((x>>9)&511)*10;
 int attr=Cards_Attribute(id);c.traits=(c.t==0?EA_DRAGON:0)|(attr==Cards_AttributeNamed("Dark")?EA_DARK:0)|(attr==Cards_AttributeNamed("Earth")?EA_EARTH:0)|(attr==Cards_AttributeNamed("Fire")?EA_FIRE:0)|(Cards_InFusionGroup(id,CARD_FUSION_GROUP_FEMALE)?EA_FEMALE:0);
 if(ea_fapi)ea_fapi->read(&c,s,side);if(ea_rapi)ea_rapi->read(&c,s,side);return c;}
static EaBoard ea_capture(int reveal){
 EaBoard b={0};int side=D_8009B1D5;b.terrain=EA_TERRAIN;
 if(side<0||side>1)return b;
 for(int e=0;e<2;e++){int who=side^e;b.lp[e]=D_800E9FF0[who].life_points.unsigned_value;b.maxlp[e]=D_800E9FF0[who].max_life_points;
 b.blocked[e]=D_800E9FF0[who].swords_turns_remaining!=0||(ea_fapi&&ea_fapi->policy(who,0));
 for(int j=0;j<5;j++){const AiActiveCard*a=&gDuel_aActiveCards[(e?56:1)+j];EaCard*c=&b.row[e][j];
 b.backfree[e]+=!(D_801A7AD8[who*15+10+j].flags&DUEL_CARD_FLAG_OCCUPIED);
 if(!a->card_id)continue;if(e&&!reveal&&(a->flags&4096)){c->id=-1;c->flags=a->flags;continue;}
 int slot=D_800907D8[side*20+(e?9-j:10+j)];*c=ea_card(a->card_id,slot,side,a->flags);c->at=a->attack;c->df=a->defense;c->star=a->guardian_star;}}
 for(int j=0;j<5;j++){int n=D_800E9FF0[side].hand[j];if(n>=side*40&&n<side*40+40)b.hand[j]=ea_card(gDuel_aDeckCardRecords[n].id,-1,side,0);}
 b.equiplock=ea_fapi&&ea_fapi->policy(side,1);return b;}
