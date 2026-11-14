#define main old_effect_suite
#include "test_round3_runtime.c"
#undef main
#include "../model.h"
int main(void){
 const int ids[]={389,545,521,467,385,69,86,617,15,230,565};
 int cases=0;
 for(int side=0;side<2;side++)for(int def=0;def<2;def++)for(int enable=0;enable<2;enable++)
 for(unsigned k=0;k<sizeof(ids)/sizeof(ids[0]);k++){
  int id=ids[k],a=side?20:5,d=side?5:20,target=id==545?613:201;clear();
  put(a,id,1800,1550,enable);put(d,target,2200,2050,0);
  attrs[target]=Cards_AttributeNamed("Dark");types[target]=id==15?CARD_TYPE_ZOMBIE:id==230?CARD_TYPE_BEAST:15;
  if(def)D_801A7AD8[d].flags|=DUEL_CARD_FLAG_DEFENSE_POSITION;
  gDuel_adwCardStats[target-1]=230|(205<<9);
  EaBoard b={0};b.lp[0]=b.lp[1]=4000;b.maxlp[0]=b.maxlp[1]=8000;
  b.row[side][0]=(EaCard){.id=id,.at=1800,.df=1550,.active=enable?EA_FUSION:0,.printed=1800,.pdef=1550};
  b.row[side^1][0]=(EaCard){.id=target,.t=id==15?2:id==230?5:15,.at=2200,.df=2050,.printed=2300,.flags=def?2048:0,.traits=EA_DARK|(target==613?EA_DRAGON:0)};
  EaResult f=ea_fight(&b,side,0,0,0);
  begin(a,d);int margin=at(a)-(def?df(d):at(d));
  assert((f.relevant?f.delta:1800-(def?2050:2200))==margin);cases++;
 }
 printf("PASS: %d forecast/executor comparisons, both sides, ATK/DEF, fusion/direct copies\n",cases);
}
