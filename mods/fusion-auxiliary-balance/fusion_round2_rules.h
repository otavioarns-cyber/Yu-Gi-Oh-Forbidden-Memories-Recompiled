#ifndef FUSION_ROUND2_RULES_H
#define FUSION_ROUND2_RULES_H
/* Pure decisions shared by runtime and regression tests. */
static int r2_member(int id)
{
    switch(id){case 613:case 707:case 370:case 69:case 385:case 427:case 467:case 442:
    case 509:case 471:case 564:case 223:case 465:case 531:case 572:case 565:return 1;default:return 0;}
}
static int r2_limit(int v,int cap){return v<0?0:v>cap?cap:v;}
static int r2_siege(int attacking,int defense,int attack,int target_defense)
{return attacking&&defense&&target_defense>=attack;}
static int r2_shark(int attacking,int target_attack,int printed_attack)
{return attacking&&target_attack<printed_attack;}
static int r2_adjacent(int a,int b){return a/15==b/15&&(a+1==b||b+1==a);}
static int r2_loss_cost(int life){return life>500?life-500:0;}
static int r2_lower_modifier(int modifier,int amount){return modifier< -32768+amount?-32768:modifier-amount;}
static int r2_exact_recipe(int a,int b)
{
    static const short recipes[][3]={{162,175,707},{412,512,370},{95,120,385},{7,603,427},
    {125,466,467},{603,626,442},{32,81,509},{149,560,471},{36,97,564},{387,647,223},
    {636,538,465},{635,649,572},{646,626,565}};
    unsigned i;for(i=0;i<sizeof(recipes)/sizeof(recipes[0]);i++)
        if((a==recipes[i][0]&&b==recipes[i][1])||(b==recipes[i][0]&&a==recipes[i][1]))return recipes[i][2];
    return 0; /* 74+433 already exists as Mystical Sand in the disc. */
}
#endif
