#include "game/duel_calc_guardian_star_matchup.h"
#include "snapshot.h"
static void(*ea_old_best)(void),(*ea_old_killer)(void),(*ea_old_power)(void);
static int ea_native(void){if(!ea_enabled()||D_8009B1D5>1||D_8009B360[D_8009B1D5]<0)return 0;ea_connect(EA_HOST);
#ifdef EA_RITUAL_OWNER
 if(ea_fapi)return 0;
#endif
 return 1;}
static void ea_best(void){if(!ea_native()||!gAiScript_State.script_cursor){ea_old_best();return;}
 int out=gAiScript_State.script_cursor[1];ea_old_best();EaBoard b=ea_capture(0);int best=0,ai=0,t=0,m=0,affected=0;
 if(gAiScript_State.attack_best_target>=56&&gAiScript_State.attack_best_target<=60&&b.row[1][gAiScript_State.attack_best_target-56].id<0)return;
 for(int i=0;i<5;i++)if(b.row[0][i].id>0&&!(b.row[0][i].flags&16384))for(int j=0;j<5;j++)if(b.row[1][j].id>0&&!(b.row[1][j].flags&(2048|16384))){
 EaResult f=ea_fight(&b,0,i,j,Duel_CalcGuardianStarMatchup(b.row[0][i].star,b.row[1][j].star));affected|=f.relevant;if(f.legal&&f.score>best){best=f.score;ai=i+1;t=j+56;m=ea_max(1,f.delta);}}
 if(affected){gAiScript_State.attack_best_stat=m;gAiScript_State.attack_best_slot=ai;gAiScript_State.attack_best_target=t;gAiScript_aMemory[out]=ai?0:1;
 if(EA_HOST->setting(EA_HOST,"effect_ai_trace",0)&&EA_HOST->log)EA_HOST->log(EA_HOST,"[Effect AI] best attacker=%d target=%d margin=%d utility=%d",ai,t,m,best);}}
static void ea_killer(void){if(!ea_native()||!gAiScript_State.script_cursor){ea_old_killer();return;}
 const u8*p=gAiScript_State.script_cursor;int j=gAiScript_aMemory[p[0]]-56,def=gAiScript_aMemory[p[1]],out=p[2];ea_old_killer();if(j<0||j>=5)return;
 EaBoard b=ea_capture(0);if(b.row[1][j].id<=0)return;if(def)b.row[1][j].flags|=2048;else b.row[1][j].flags&=~2048;
 int pick=0,score=-1000000,affected=0;for(int i=0;i<5;i++)if(b.row[0][i].id>0&&!(b.row[0][i].flags&16384)){
 EaResult f=ea_fight(&b,0,i,j,Duel_CalcGuardianStarMatchup(b.row[0][i].star,b.row[1][j].star));affected|=f.relevant;if(f.legal&&f.kill&&!f.loss&&f.score>0&&f.score-b.row[0][i].at/4>score){score=f.score-b.row[0][i].at/4;pick=i+1;}}
 if(affected)gAiScript_aMemory[out]=pick;}
static void ea_power(void){if(!ea_native()||!gAiScript_State.script_cursor){ea_old_power();return;}const u8*p=gAiScript_State.script_cursor;int id=gAiScript_aMemory[p[0]],mode=gAiScript_aMemory[p[1]],out=p[2];ea_old_power();
 if(mode||id<330||id>347)return;EaBoard b=ea_capture(0);
 if(id>=343)gAiScript_aMemory[out]*=1+ea_sources(b.row[0],-1,168);
 else if(id<=335){EaBoard n=b;n.terrain=id-329;int x=0;for(int e=0;e<2;e++)for(int k=0;k<5;k++)for(int d=0;d<2;d++)x+=(e?-1:1)*(ea_aura(&n,e,k,d)-ea_aura(&b,e,k,d))/2;gAiScript_aMemory[out]=ea_max(0,gAiScript_aMemory[out]+x);}
 else if(id>=338&&ea_sources(b.row[0],-1,551)){const int v[]={200,500,1000,2000,5000};gAiScript_aMemory[out]+=ea_min(b.maxlp[0]-b.lp[0],v[id-338])/2;}}
