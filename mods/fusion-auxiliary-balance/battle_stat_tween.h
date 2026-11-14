#ifndef AUX_BATTLE_STAT_TWEEN_H
#define AUX_BATTLE_STAT_TWEEN_H
/* Presentation-only counterpart of the native Guardian Star stat climb.
   The duel's actual stats are NEVER changed by these interpolation helpers.
   Native rendering sums field_32+field_36 (ATK) and field_34+field_38
   (DEF); those additions, not card records, are the animation target. */
typedef struct {
    int ready, active, started, show_initial;
    int initial[2][2];       /* displayed numbers before this battle's effects */
    int change[2][2];        /* final minus initial, ATK then DEF */
    int bw_change[2];        /* the existing Battle Warrior coin swap */
    int base_bonus[2][2];    /* preserved native Guardian Star adjustments */
    int progress, amount, step;
} AuxBattleStatTween;
static int aux_tween_abs(int n) { return n < 0 ? -n : n; }
static int aux_tween_amount(const AuxBattleStatTween *t)
{
    int p,q,m=0;
    for(p=0;p<2;p++)for(q=0;q<2;q++) {
        int a=aux_tween_abs(t->change[p][q]);
        if(a>m)m=a;
    }
    return m;
}
static int aux_tween_offset(int delta,int progress,int total)
{
    /* Keep rounding toward zero, never overshoot; negatives are reductions. */
    if(total<=0 || progress<=0)return 0;
    if(progress>=total)return delta;
    return (int)(((long long)delta*progress)/total);
}
static void aux_tween_advance(AuxBattleStatTween *t)
{
    if(t->progress + t->step >= t->amount) t->progress=t->amount;
    else t->progress+=t->step;
}
#endif
