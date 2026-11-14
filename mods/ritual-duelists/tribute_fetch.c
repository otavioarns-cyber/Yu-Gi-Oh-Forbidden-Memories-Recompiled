#include "tribute_fetch.h"
#include "game/card_constants.h"
#include <string.h>

/* This is a preference score, not a Ritual legality test. The engine remains
 * authoritative. Distinct-role matching prevents using a single monster for
 * the Skull, Warrior and specific-card requirements simultaneously. */
static int role_match(const RdOpponentProfile *p,const RdRitualRecipe *r,
                      const RdCard *card,int role,RdMaterialMatch fn,void *ctx)
{
    const RdRitualRequirement *req=&r->materials[role];
    if(card->id<=0||card->type>=CARD_TYPE_MAGIC)return 0;
    if(fn)return fn(p,card->id,req,(unsigned)role,ctx);
    if(req->exact_card>0 && card->id!=req->exact_card)return 0;
    if(req->type>=0 && card->type!=req->type)return 0;
    if(req->custom_group && (!p->in_group || !p->in_group(p,card->id,req->custom_group)))return 0;
    if(req->min_attack>=0 && card->atk<req->min_attack)return 0;
    if(req->max_attack>=0 && card->atk>req->max_attack)return 0;
    if(req->min_defense>=0 && card->def<req->min_defense)return 0;
    if(req->max_defense>=0 && card->def>req->max_defense)return 0;
    if(req->min_level>=0 && card->level<req->min_level)return 0;
    if(req->max_level>=0 && card->level>req->max_level)return 0;
    if(req->defense_gt_attack && card->def<=card->atk)return 0;
    return 1;
}

typedef struct {
    const RdOpponentProfile *p;
    const RdRitualRecipe *recipe;
    const RdAdaptiveRules *rules;
    const RdBoard *board;
    RdMaterialMatch matcher;
    void *context;
    int best;
} RdFit;

static void fit_walk(RdFit *f,int role,unsigned used,int count,int field,int hand)
{
    int i;
    if(role==3){
        int score=count*200;
        int fcount=field<f->rules->min_field?field:f->rules->min_field;
        int hcount=hand<f->rules->min_hand?hand:f->rules->min_hand;
        score+=1000*fcount+1000*hcount;
        if(score>f->best)f->best=score;
        return;
    }
    fit_walk(f,role+1,used,count,field,hand); /* allow incomplete roles */
    for(i=0;i<5+f->board->actual_hand;i++){
        const RdCard *c=i<5?&f->board->own[i]:&f->board->hand[i-5];
        if((used & (1u<<i)) || !role_match(f->p,f->recipe,c,role,f->matcher,f->context))continue;
        /* A result monster explicitly required in hand cannot be a tribute. */
        if(f->rules->require_result && c->id==f->recipe->result_card)continue;
        fit_walk(f,role+1,used|(1u<<i),count+1,field+(i<5),hand+(i>=5));
    }
}
static int fit_score(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
                     const RdAdaptiveRules *rules,const RdBoard *board,
                     RdMaterialMatch matcher,void *ctx)
{
    RdFit state={p,recipe,rules,board,matcher,ctx,0};
    fit_walk(&state,0,0,0,0,0);
    return state.best;
}
static int is_selected_hand(const unsigned char selection[12],int slot)
{
    int i;
    for(i=0;i<5 && selection[i];i++)if(selection[i]==11+slot)return 1;
    return 0;
}
static int native_choice_safe(const unsigned char sel[12],int physical,const RdPlan *plan)
{
    int i;
    /* A previous mod can legitimately choose a deck-sourced fusion.
       Never combine that unknown materialization with our own swap. */
    for(i=0;i<5 && sel[i];i++){
        if(sel[i]<11 || sel[i]>=11+physical)return 0;
        if(plan->protect_hand[sel[i]-11])return 0;
    }
    return sel[0]>=11 && sel[0]<11+physical;
}
static int replacer(const RdBoard *b,const RdPlan *plan,
                    const unsigned char selection[12],int hold)
{
    int i,best=0x7fffffff,slot=-1;
    for(i=0;i<b->actual_hand;i++){
        const RdCard *c=&b->hand[i];
        int power=c->atk>c->def?c->atk:c->def;
        int cost=c->type>=CARD_TYPE_MAGIC?1000:power;
        if(!c->id||plan->protect_hand[i]||
           (hold && is_selected_hand(selection,i)))continue;
        if(cost<best){best=cost;slot=i;}
    }
    return slot;
}
static int secure_field_play(const RdBoard *b,const RdCard *card)
{
    int i,strongest=0;
    for(i=0;i<5;i++)if(b->enemy[i].id && !b->enemy[i].face_down &&
                       b->enemy[i].atk>strongest)strongest=b->enemy[i].atk;
    return strongest==0 || card->atk>=strongest || card->def>=strongest;
}
static int field_assignment(const RdOpponentProfile *p,const RdRitualRecipe *r,
                            const RdBoard *b,RdMaterialMatch matcher,void *ctx)
{
    int a,j,k,best=0;
    for(a=0;a<5;a++)for(j=0;j<3;j++)
        if(role_match(p,r,&b->own[a],j,matcher,ctx)){
            if(best<1)best=1;
            for(k=0;k<5;k++)if(k!=a){
                int l;
                for(l=0;l<3;l++)if(l!=j &&
                    role_match(p,r,&b->own[k],l,matcher,ctx)){
                    int m,n;
                    if(best<2)best=2;
                    for(m=0;m<5;m++)if(m!=a && m!=k)
                        for(n=0;n<3;n++)if(n!=j && n!=l &&
                            role_match(p,r,&b->own[m],n,matcher,ctx))return 3;
                }
            }
        }
    return best;
}
static int hand_has_result(const RdBoard *b,int id)
{
    int i;
    for(i=0;i<b->actual_hand;i++)if(b->hand[i].id==id)return 1;
    return 0;
}

RdTributeFetch Rd_ChooseDeckTribute(const RdOpponentProfile *p,
    const RdRitualRecipe *recipe,const RdAdaptiveRules *rules,
    const RdBoard *b,const RdPlan *plan,const RdCard deck[40],int count,
    const unsigned char selection[12],int legal,
    RdMaterialMatch matcher,void *ctx)
{
    RdTributeFetch out={RD_FETCH_NONE,-1,-1,-1,0,-1};
    int base,goal,needed_field,needed_hand,free_field=-1,replace_field=-1;
    int replace_hold=-1,i,j,choice=-0x7fffffff;
    if(!p||!recipe||!rules||!b||!plan||!deck||!selection ||
       !rules->available || rules->material_count!=3 ||
       recipe->material_count!=3 || !recipe->materials ||
       rules->min_field<0 || rules->min_field>3 ||
       rules->min_hand<0 || rules->min_hand>3 ||
       rules->min_field+rules->min_hand>3 ||
       b->actual_hand<=0 || b->actual_hand>5 ||
       count<0 || count>40 || legal || plan->urgent || !plan->prep ||
       (plan->ritual_back_slot<0 && plan->ritual_hand_slot<0))return out;
    /* This component never tries to replace field tributes already prepared. */
    for(i=0;i<5;i++)if(!b->own[i].id){free_field=i+1;break;}
    replace_field=replacer(b,plan,selection,0);
    replace_hold=native_choice_safe(selection,b->actual_hand,plan)?replacer(b,plan,selection,1):-1;
    base=fit_score(p,recipe,rules,b,matcher,ctx);
    needed_field=rules->min_field;
    needed_hand=(rules->sources & RITUAL_RULES_SOURCES_DECK)?rules->min_hand:0;
    /* With tributes forbidden from the Deck, all three must be physical. */
    goal=600+1000*needed_field+1000*needed_hand;
    /* Result-in-hand is a distinct requirement from tributes. Search it
       before other materials when enabled, without treating it as tribute. */
    if(rules->require_result==1 && !hand_has_result(b,recipe->result_card)){
        if(replace_hold<0)return out;
        for(j=0;j<count;j++){
            int source=11+b->actual_hand+j;
            if(source>=16 && source<=55 && deck[j].id==recipe->result_card){
                out.mode=RD_FETCH_HOLD;out.source_slot=source;
                out.replace_slot=replace_hold;out.card_id=deck[j].id;
                return out;
            }
        }
        return out;
    }
    if((rules->sources & RITUAL_RULES_SOURCES_DECK) &&
       base>=1000*needed_field+1000*needed_hand+200*(needed_field+needed_hand)){
        /* All mandatory physical positions already have distinct roles.
           Let the Ritual engine consume the optional third from the Deck. */
        return out;
    }
    if(!(rules->sources & RITUAL_RULES_SOURCES_DECK) && base>=goal)return out;
    for(j=0;j<count;j++){
        const RdCard *c=&deck[j];
        int source=11+b->actual_hand+j;
        int role=-1,k;
        if(source<16 || source>55 || c->id<=0 || c->type>=CARD_TYPE_MAGIC ||
           (rules->require_result && c->id==recipe->result_card))continue;
        for(k=0;k<3;k++)if(role_match(p,recipe,c,k,matcher,ctx)){role=k;break;}
        if(role<0)continue;
        if(free_field>0 && replace_field>=0 &&
           field_assignment(p,recipe,b,matcher,ctx)<needed_field &&
           plan->setup_hand_slot<0 && secure_field_play(b,c)){
            RdBoard after=*b;
            int gain,score;
            /* Vanilla deck swap trades the expendable hand card back into
               the Deck; the selected monster is then consumed onto Field. */
            after.hand[replace_field].id=0;
            after.own[free_field-1]=*c;
            gain=fit_score(p,recipe,rules,&after,matcher,ctx)-base;
            score=gain*100- (c->atk>c->def?c->atk:c->def)/100 - j;
            if(gain>0 && score>choice){
                choice=score;out.mode=RD_FETCH_FIELD;
                out.source_slot=source;out.replace_slot=replace_field;
                out.destination=free_field;out.card_id=c->id;out.role=role;
            }
        }
        if(replace_hold>=0){
            RdBoard after=*b;
            int gain,score;
            after.hand[replace_hold]=*c;
            gain=fit_score(p,recipe,rules,&after,matcher,ctx)-base;
            score=gain*100+100 - (c->atk>c->def?c->atk:c->def)/100 - j;
            if(gain>0 && score>choice){
                choice=score;out.mode=RD_FETCH_HOLD;
                out.source_slot=source;out.replace_slot=replace_hold;
                out.destination=-1;out.card_id=c->id;out.role=role;
            }
        }
    }
    return out;
}
