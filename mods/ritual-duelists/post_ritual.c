#include "post_ritual.h"
#include "game/card_constants.h"
#include <string.h>

static int rd_max(int a,int b){return a>b?a:b;}
static int rd_min(int a,int b){return a<b?a:b;}

/* A delta-based evaluator is reusable for any Ritual whose profile supplies
 * a Beacon callback. Opponent field and the currently summoned Ritual stay
 * in both boards: only the prospective hand-to-field action changes. */
static int rd_score_play(const RdOpponentProfile *profile,
    const RdRitualRecipe *recipe,const RdBoard *board,int effect_mode,
    int held_slot,const RdCard *support,int destination,
    int *face_up,int *attack_gain,int *defense_gain)
{
    RdBoard next;
    RdEffectEstimate before,after;
    int i,enemy=0,face=0,base,benefit,risk=0;
    if(!profile||!recipe||!board||!profile->evaluate_effect||!support||
       support->id<=0||support->type>=CARD_TYPE_MAGIC||
       held_slot<0||held_slot>=board->actual_hand||
       destination<0||destination>=5||board->own[destination].id)return 0;
    /* For a Deck candidate, the actual held card in this slot will be
       exchanged for the support: account for its loss in the simulation. */
    /* A face-down Ritual result isn't treated as an active Beacon. */
    for(i=0;i<5;i++)if(board->own[i].id==recipe->result_card &&
                           !board->own[i].face_down)break;
    if(i==5 || effect_mode!=0)return 0;
    before=profile->evaluate_effect(profile,recipe->result_card,board,0,0,effect_mode);
    next=*board;
    next.hand[held_slot].id=0;
    next.own[destination]=*support;
    /* We decide positioning after scoring but need an honest BEFORE/AFTER
       delta. This profile's callback may depend on face state. */
    next.own[destination].face_down=0;
    after=profile->evaluate_effect(profile,recipe->result_card,&next,0,0,effect_mode);
    if(attack_gain)*attack_gain=after.attack_bonus-before.attack_bonus;
    if(defense_gain)*defense_gain=after.defense_bonus-before.defense_bonus;
    benefit=(after.attack_bonus-before.attack_bonus)+
            (after.defense_bonus-before.defense_bonus)/2;
    if(benefit<=0)return 0;
    for(i=0;i<5;i++)if(board->enemy[i].id && !board->enemy[i].face_down)
        enemy=rd_max(enemy,board->enemy[i].atk);
    if(enemy>0){
        /* Only reveal a sturdy offensive monster when defense would be
           needlessly unsafe; otherwise keep the card's identity concealed. */
        if(support->atk>=enemy+200 && support->atk>support->def+300)
            face=1;
        base=face?support->atk:support->def;
        if(enemy>base){
            risk=1100+rd_min(enemy-base,2200)/2;
            if(board->own_lp<=enemy)risk+=1800;
        }
    }
    /* A good combat monster is more valuable than an equally useful weak
       support. Prevent a fragile +500 Beacon play from being forced simply
       because it technically increases the Ritual's printed stats. */
    benefit+=rd_min(rd_max(support->atk,support->def),3500)/3;
    benefit-=risk;
    if(board->own_lp>0 && enemy>=board->own_lp &&
       rd_max(support->atk,support->def)<enemy)benefit-=1500;
    if(face_up)*face_up=face;
    return benefit>0?benefit:0;
}
int Rd_PostSupportScore(const RdOpponentProfile *profile,
    const RdRitualRecipe *recipe,const RdBoard *board,int effect_mode,
    int held_slot,const RdCard *support,int destination,int *face_up)
{
    return rd_score_play(profile,recipe,board,effect_mode,held_slot,
                         support,destination,face_up,0,0);
}
RdPostPlan Rd_PlanPostRitual(const RdOpponentProfile *profile,
    const RdRitualRecipe *recipe,const RdBoard *board,int effect_mode)
{
    RdPostPlan p;int i,j,best=0;
    memset(&p,0,sizeof(p));p.hand_slot=p.destination=-1;
    if(!profile||!recipe||!board||recipe->result_card<=0)return p;
    for(i=0;i<5;i++)if(board->own[i].id==recipe->result_card &&
                           !board->own[i].face_down){
        p.active=1;p.guardian_count++;p.protect_field[i]=1;
    }
    if(!p.active||!profile->evaluate_effect||effect_mode!=0)return p;
    for(j=0;j<5;j++)if(!board->own[j].id && p.destination<0)
        p.destination=j;
    for(j=0;j<5;j++)if(!board->own[j].id)
        for(i=0;i<board->actual_hand && i<5;i++){
            int face=0,atk=0,def=0,score;
            score=rd_score_play(profile,recipe,board,effect_mode,
                                i,&board->hand[i],j,&face,&atk,&def);
            if(score>best){
                best=score;p.hand_slot=i;p.destination=j;
                p.score=score;p.face_up=face;
                p.attack_gain=atk;p.defense_gain=def;
            }
        }
    return p;
}
