#include "ritual_duelists.h"
#include "planner.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int on=1,read_count;
static int enabled(int id){return on && id==704;}
static int estimate(int id,const RitualEffectAiBoardV1 *b,RitualEffectAiEstimateV1 *e){
    if(id!=704)return 0;
    read_count++;
    for(int i=0;i<5;i++) {
        if(b->own[i]==22&&!b->selected[i])e->attack_bonus+=1000;
        if(b->hand[i]==22&&!b->selected[i+5])e->attack_bonus+=500;
    }
    e->defense_bonus=e->attack_bonus;
    return 1;
}
static const RitualEffectIntelligenceV1 api={1,enabled,estimate};
static RdBoard field(void){RdBoard b={0};b.actual_hand=b.hand_count=5;b.own_lp=b.enemy_lp=8000;
    b.own[0]=(RdCard){22,2500,1200,1};return b;}
int main(void){
    const RdOpponentProfile *p=Rd_GetProfile(23);
    RdBoard b=field(); RdRitualRecipe r=p->recipe;
    assert(p->evaluate_effect!=0);
    Rd_ConnectEffectIntelligence(0);
    assert(!Rd_EffectIntelligenceEnabled(704));
    assert(p->evaluate_effect(p,704,&b,0,0,0).attack_bonus==0);
    Rd_ConnectEffectIntelligence(&api);
    assert(Rd_EffectIntelligenceEnabled(704));
    assert(p->evaluate_effect(p,704,&b,0,0,0).attack_bonus==1000);
    {int selected[]={0};assert(p->evaluate_effect(p,704,&b,selected,1,0).attack_bonus==0);}
    on=0;
    assert(!Rd_EffectIntelligenceEnabled(704));
    assert(p->evaluate_effect(p,704,&b,0,0,0).attack_bonus==0);
    on=1;
    Rd_ConnectEffectIntelligence(&(RitualEffectIntelligenceV1){999,enabled,estimate});
    assert(!Rd_EffectIntelligenceEnabled(704));
    assert(p->evaluate_effect(p,704,&b,0,0,0).attack_bonus==0);
    assert(read_count==2);
    Rd_ConnectEffectIntelligence(&api);
    assert(p->evaluate_effect(p,357,&b,0,0,0).attack_bonus==0); /* live recipe result, not Isis profile */
    printf("PASS: optional ABI, settings off, no imaginary buffs, tribute exclusion\n");
    (void)r;return 0;
}
