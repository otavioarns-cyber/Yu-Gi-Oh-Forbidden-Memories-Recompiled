#include <assert.h>
#include <stdio.h>
#include "../battle_stat_tween.h"

static void advance_to_end(AuxBattleStatTween *t){
    int iterations=0;
    while(t->progress<t->amount){
        aux_tween_advance(t);
        for(int p=0;p<2;p++)for(int c=0;c<2;c++) {
            int v=aux_tween_offset(t->change[p][c],t->progress,t->amount);
            if(t->change[p][c]>=0)assert(v>=0&&v<=t->change[p][c]);
            else assert(v<=0&&v>=t->change[p][c]);
        }
        assert(++iterations<=32);
    }
}
int main(void){
    AuxBattleStatTween t={0};
    t.change[0][0]=400;t.change[1][0]=-400;
    t.amount=aux_tween_amount(&t);t.step=16;
    assert(t.amount==400);
    assert(aux_tween_offset(400,200,400)==200);
    assert(aux_tween_offset(-400,200,400)==-200);
    advance_to_end(&t);
    assert(aux_tween_offset(400,t.progress,t.amount)==400);
    assert(aux_tween_offset(-400,t.progress,t.amount)==-400);
    /* ATK and DEF adjust independently, preserving guardian-star modifiers. */
    t=(AuxBattleStatTween){0};
    t.change[0][0]=-150;t.change[0][1]=600;
    t.change[1][0]=1000;t.change[1][1]=-300;
    t.base_bonus[0][0]=500;t.base_bonus[0][1]=500;
    t.amount=aux_tween_amount(&t);t.step=32;
    assert(t.amount==1000);
    assert(500+aux_tween_offset(-150,0,t.amount)==500);
    advance_to_end(&t);
    assert(500+aux_tween_offset(-150,t.progress,t.amount)==350);
    assert(500+aux_tween_offset(600,t.progress,t.amount)==1100);
    assert(aux_tween_offset(-300,t.progress,t.amount)==-300);
    /* No animation when no temporary attributes changed. */
    t=(AuxBattleStatTween){0};assert(aux_tween_amount(&t)==0);
    puts("PASS native-pace battle stat tween; increases/decreases/dual stats; no permanent changes");
}
