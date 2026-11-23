#include "planner.h"
#include "game/card_constants.h"
#include <string.h>

/* Materials and candidate instances are separate dimensions. Nothing in
 * this planner assumes that a Ritual always consumes exactly three cards.
 * The game checker, not this module, authorizes the actual invocation. */
static int rd_is_tribute(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
                         const RdCard *c,unsigned role,RdMaterialMatch match,void *context)
{
    if(match)return match(p,c->id,&recipe->materials[role],role,context);
    const RdRitualRequirement *r=&recipe->materials[role];
    if(c->id<=0||c->type>=CARD_TYPE_MAGIC)return 0;
    if(r->exact_card>0&&c->id!=r->exact_card)return 0;
    if(r->type>=0&&c->type!=r->type)return 0;
    if(r->custom_group&&(!p->in_group||!p->in_group(p,c->id,r->custom_group)))return 0;
    if(r->min_attack>=0&&c->atk<r->min_attack)return 0;
    if(r->max_attack>=0&&c->atk>r->max_attack)return 0;
    if(r->min_defense>=0&&c->def<r->min_defense)return 0;
    if(r->max_defense>=0&&c->def>r->max_defense)return 0;
    if(r->min_level>=0&&c->level<r->min_level)return 0;
    if(r->max_level>=0&&c->level>r->max_level)return 0;
    if(r->defense_gt_attack&&c->def<=c->atk)return 0;
    return 1;
}
static const RdCard *rd_pick(const RdBoard *b,int loc)
{
    return loc<5?&b->own[loc]:&b->hand[loc-5];
}
static int rd_power(const RdCard *c){return c->atk>c->def?c->atk:c->def;}
static int rd_cost(const RdCard *c,int field)
{
    int power=rd_power(c);
    return power/2+(field?power/2:0);
}
static int rd_emergency(const RdBoard *b)
{
    int i,enemy=0,own=0;
    for(i=0;i<5;++i){
        if(!b->enemy[i].face_down&&b->enemy[i].atk>enemy)enemy=b->enemy[i].atk;
        if(b->own[i].atk>own)own=b->own[i].atk;
    }
    return (enemy>0 && enemy>=b->own_lp) || (enemy>own+1000&&b->own_lp<3000);
}
static int rd_popcount(unsigned mask)
{
    int result=0;
    while(mask){result+=(mask&1u)!=0;mask>>=1;}
    return result;
}

typedef struct {
    const RdOpponentProfile *p;
    const RdRitualRecipe *recipe;
    const RdBoard *b;
    RdPlan *out;
    int effect_mode;
    RdMaterialMatch matcher;void *match_context;
    int slots,role_count,nodes;
    unsigned candidate_mask[RD_MAX_RITUAL_MATERIALS];
    unsigned char order[RD_MAX_RITUAL_MATERIALS];
    int selected[RD_MAX_RITUAL_MATERIALS];
    int best_score;
} RdSearch;

/* Bipartite matching: finds the maximum number of roles that can be filled
 * with distinct physical slots. Unlike checking each role independently,
 * one monster cannot count as several tributes. Complexity is bounded by
 * (number of roles) * (10 slots)^2, even with heavily overlapping rules. */
static int rd_augment(const RdSearch *s,int role,unsigned *visited,int owners[RD_PHYSICAL_CANDIDATES])
{
    int loc;
    for(loc=0;loc<s->slots;++loc){
        unsigned bit=1u<<loc;
        if(!(s->candidate_mask[role]&bit)||(*visited&bit))continue;
        *visited|=bit;
        if(owners[loc]<0||rd_augment(s,owners[loc],visited,owners)){
            owners[loc]=role;
            return 1;
        }
    }
    return 0;
}
static int rd_maximum_matching(const RdSearch *s,int result[RD_MAX_RITUAL_MATERIALS])
{
    int owners[RD_PHYSICAL_CANDIDATES],i,count=0;
    for(i=0;i<RD_PHYSICAL_CANDIDATES;++i)owners[i]=-1;
    for(i=0;i<s->role_count;++i){
        unsigned visited=0;
        if(rd_augment(s,s->order[i],&visited,owners))count++;
    }
    for(i=0;i<RD_MAX_RITUAL_MATERIALS;++i)result[i]=-1;
    for(i=0;i<s->slots;++i)if(owners[i]>=0)result[owners[i]]=i;
    return count;
}
static void rd_consider(RdSearch *s)
{
    RdEffectEstimate estimate={0};int atk,def,score,a,j;
    if(s->p->evaluate_effect)
        estimate=s->p->evaluate_effect(s->p,s->recipe->result_card,s->b,s->selected,
                                        (unsigned)s->role_count,s->effect_mode);
    atk=s->recipe->result_attack+estimate.attack_bonus;
    def=s->recipe->result_defense+estimate.defense_bonus;
    score=(atk>def?atk:def)+(estimate.attack_bonus+estimate.defense_bonus)/8+estimate.strategy_bonus;
    for(j=0;j<s->role_count;++j){
        int loc=s->selected[j];
        score-=rd_cost(rd_pick(s->b,loc),loc<5);
    }
    for(a=0;a<5;++a)if(s->b->enemy[a].id&&!s->b->enemy[a].face_down &&
        atk>=s->b->enemy[a].atk && s->b->enemy[a].atk>900)score+=350;
    if(atk>=s->b->enemy_lp)score+=2400;
    if(s->out->urgent&&def>0)score+=600;
    if(score>s->best_score){
        s->best_score=score;
        s->out->score=score;
        s->out->live_skulls=estimate.field_synergy;
        s->out->hand_skulls=estimate.hand_synergy;
        s->out->legal_set=1;
        for(j=0;j<s->role_count;++j)s->out->tribute[j]=s->selected[j];
    }
}
static void rd_search_best(RdSearch *s,int depth,unsigned used)
{
    int slot,role;
    if(++s->nodes>RD_SEARCH_BUDGET)return;
    if(depth==s->role_count){rd_consider(s);return;}
    role=s->order[depth];
    for(slot=0;slot<s->slots;++slot){
        unsigned bit=1u<<slot;
        if((used&bit)||(s->candidate_mask[role]&bit)==0)continue;
        s->selected[role]=slot;
        rd_search_best(s,depth+1,used|bit);
        if(s->nodes>=RD_SEARCH_BUDGET)break;
    }
}

RdPlan Rd_PlanRitualMatched(const RdOpponentProfile *p,const RdRitualRecipe *active_recipe,
                     const RdBoard *b,int effect_mode,int ritual_is_legal,
                     RdMaterialMatch match,void *context)
{
    RdPlan d;RdSearch search;int i,j,free_back=-1,free_monster=-1,matched;
    int initial_match[RD_MAX_RITUAL_MATERIALS];
    int field_match[RD_MAX_RITUAL_MATERIALS];
    const RdRitualRecipe *recipe;
    memset(&d,0,sizeof(d));
    for(i=0;i<RD_MAX_RITUAL_MATERIALS;++i)d.tribute[i]=-1;
    d.ritual_hand_slot=d.ritual_back_slot=d.ritual_back_destination=-1;
    d.prefer_hand_slot=d.prefer_destination=-1;
    d.setup_hand_slot=d.setup_destination=-1;
    if(!p||!b)return d;
    recipe=active_recipe?active_recipe:&p->recipe;
    if(recipe->material_count>RD_MAX_RITUAL_MATERIALS ||
       (recipe->material_count&&!recipe->materials)||
       b->actual_hand<0||b->actual_hand>5||
       b->hand_count<b->actual_hand||b->hand_count>RD_HAND)return d;
    d.material_count=recipe->material_count;
    d.urgent=rd_emergency(b);
    for(i=0;i<5;++i){
        if(b->back[i].id==recipe->ritual_magic && !b->back[i].used)d.ritual_back_slot=i;
        if(!b->back[i].id&&free_back<0)free_back=i;
        if(!b->own[i].id&&free_monster<0)free_monster=i;
    }
    for(i=0;i<b->hand_count;++i){
        if(b->hand[i].id==recipe->ritual_magic && i<b->actual_hand)
            d.ritual_hand_slot=i;
    }
    /* Never run exponential searches to decide whether a ritual is near.
       First compute a maximum distinct-material matching in a tiny graph. */
    memset(&search,0,sizeof(search));
    search.p=p;search.recipe=recipe;search.b=b;search.out=&d;
    search.effect_mode=effect_mode;
    search.matcher=match;search.match_context=context;
    search.role_count=(int)recipe->material_count;
    search.slots=5+b->actual_hand;
    search.best_score=-1000000;
    for(i=0;i<search.role_count;++i){
        search.order[i]=(unsigned char)i;
        for(j=0;j<search.slots;++j)
            if((recipe->material_sources!=1 || j<5) &&
               (recipe->material_sources!=2 || j>=5) &&
               rd_is_tribute(p,recipe,rd_pick(b,j),(unsigned)i,search.matcher,search.match_context))
                search.candidate_mask[i]|=1u<<j;
    }
    /* More restrictive requirements first; useful when a named card also
       fits another broad rule. Sorting does not change recipe semantics. */
    for(i=0;i<search.role_count;++i)for(j=i+1;j<search.role_count;++j)
        if(rd_popcount(search.candidate_mask[search.order[j]])<
           rd_popcount(search.candidate_mask[search.order[i]])){
            unsigned char t=search.order[i];search.order[i]=search.order[j];search.order[j]=t;
        }
    matched=rd_maximum_matching(&search,initial_match);
    d.near=(d.ritual_hand_slot>=0||d.ritual_back_slot>=0) &&
         (recipe->material_count==0 ||
          (matched>0&&matched>=(int)recipe->material_count-1));
    if(matched==search.role_count){
        for(i=0;i<search.role_count;++i)search.selected[i]=initial_match[i];
        rd_consider(&search); /* guaranteed complete match before budgeted search */
        if(search.role_count>0)rd_search_best(&search,0,0);
    }
    /* For an externally legal ritual, never pretend that the physical
       hand or the AI's logical deck window contains its actual tributes.
       Instead estimate a SAFE synergy floor: count only field/hand support
       cards which are ineligible for EVERY role of this active recipe.
       Such cards (e.g. Summoned Skull) cannot be consumed as tributes by
       a checker honoring that recipe. Do not award uncertain hand bonuses. */
    if(ritual_is_legal && !d.legal_set){
        RdBoard remaining=*b;
        RdEffectEstimate bonus={0};
        int own_best=0,enemy_best=0,atk,def;
        int no_selections[RD_MAX_RITUAL_MATERIALS];
        for(i=0;i<RD_MAX_RITUAL_MATERIALS;i++)no_selections[i]=-1;
        for(i=0;i<5;i++){
            if(b->own[i].id && rd_power(&b->own[i])>own_best)
                own_best=rd_power(&b->own[i]);
            if(b->enemy[i].id && !b->enemy[i].face_down && b->enemy[i].atk>enemy_best)
                enemy_best=b->enemy[i].atk;
            for(j=0;j<search.role_count;j++)
                if(rd_is_tribute(p,recipe,&b->own[i],(unsigned)j,match,context)){
                    remaining.own[i].id=0;break;
                }
        }
        for(i=0;i<b->actual_hand;i++)
            for(j=0;j<search.role_count;j++)
                if(rd_is_tribute(p,recipe,&b->hand[i],(unsigned)j,match,context)){
                    remaining.hand[i].id=0;break;
                }
        if(p->evaluate_effect)
            bonus=p->evaluate_effect(p,recipe->result_card,&remaining,no_selections,
                                      recipe->material_count,effect_mode);
        atk=recipe->result_attack+bonus.attack_bonus;
        def=recipe->result_defense+bonus.defense_bonus;
        d.predicted_attack=atk;d.predicted_defense=def;
        d.live_skulls=bonus.field_synergy;d.hand_skulls=bonus.hand_synergy;
        d.score=(atk>def?atk:def)-own_best+bonus.strategy_bonus;
        if(enemy_best>own_best)d.score+=600;
        if(atk>=b->enemy_lp&&b->enemy_lp>0)d.score+=1500;
        if(d.urgent)d.score+=800;
    }else if(d.legal_set){
        /* A full local assignment has already computed its effect bonus.
           This is the outcome used when all selected materials are known. */
        RdEffectEstimate bonus={0};
        if(p->evaluate_effect)
            bonus=p->evaluate_effect(p,recipe->result_card,b,d.tribute,recipe->material_count,effect_mode);
        d.predicted_attack=recipe->result_attack+bonus.attack_bonus;
        d.predicted_defense=recipe->result_defense+bonus.defense_bonus;
    }
    /* Unlike the old 'near=2 of 3' gate, preparation starts early. A ritual
       deck has a strategic objective even with ZERO materials collected.
       Never time out after a few turns: it may take many draws to build.
       Emergency mode still allows the original AI to defend itself. */
    d.ritual_goal=d.ritual_hand_slot>=0||d.ritual_back_slot>=0||matched>0;
    d.near=d.ritual_goal && (recipe->material_count==0 ||
                              matched>=(int)recipe->material_count-1);
    d.matched_materials=matched;
    d.prep=d.ritual_goal&&!d.urgent;
    /* Fiend's Mirror needs three very weak, named materials, at least one
     * exposed on the Field. An "enemy stronger by 1000" emergency used to
     * cancel all preparation indefinitely even when the Ritual Magic was
     * already set and two distinct materials were physically available.
     * Allow the needed FACE-DOWN setup unless the enemy can finish this
     * duelist immediately. Other profiles keep their existing policy. */
    if(p->recipe.result_card==365 && d.ritual_back_slot>=0 && matched>=2 &&
       d.urgent && b->own_lp>0){
        int enemy_max=0;
        for(i=0;i<5;i++)
            if(b->enemy[i].id && !b->enemy[i].face_down &&
               b->enemy[i].atk>enemy_max)enemy_max=b->enemy[i].atk;
        if(enemy_max<b->own_lp)d.prep=1;
    }
    if(d.prep){
        for(i=0;i<5;i++)for(j=0;j<search.role_count;j++)
            if(search.candidate_mask[j]&(1u<<i)){d.protect_field[i]=1;break;}
        for(i=0;i<b->actual_hand;i++){
            if(b->hand[i].id==recipe->ritual_magic)d.protect_hand[i]=1;
            for(j=0;j<search.role_count;j++)
                if(search.candidate_mask[j]&(1u<<(i+5))){d.protect_hand[i]=1;break;}
            /* Skull-like non-tribute support must survive to strengthen the
               result; eligibility still comes from the active recipe. */
            if(effect_mode==0 && p->skull_count>0 &&
               b->hand[i].id && Rd_IsSkull(p,b->hand[i].id))
                d.protect_hand[i]=1;
        }
    }
    /* Field-material rituals need each role represented by a DIFFERENT
       monster. Compare the five field zones alone, then choose a hand
       material for a truly missing role (not merely any high-ATK monster).
       No special move is requested with a full field or a legal ritual. */
    if(d.prep&&!ritual_is_legal&&free_monster>=0&&
       recipe->material_sources!=2&&recipe->material_count>0){
        int best=-1000000;
        int field_count,slots=search.slots;
        search.slots=5;
        field_count=rd_maximum_matching(&search,field_match);
        search.slots=slots;
        if(field_count<(int)recipe->material_count){
            for(i=0;i<b->actual_hand;i++){
                const RdCard *card=&b->hand[i];
                if(card->id<=0 || card->type>=CARD_TYPE_MAGIC)continue;
                for(j=0;j<search.role_count;j++){
                    int quality;
                    if(field_match[j]>=0 || !(search.candidate_mask[j]&(1u<<(i+5))))continue;
                    /* Exact roles are hardest to replace. Lower-value
                       tributes are preferred to irreplaceable big attackers. */
                    quality=3000-rd_power(card)/3;
                    if(recipe->materials[j].exact_card)quality+=1000;
                    quality+=500-rd_popcount(search.candidate_mask[j])*20;
                    if(quality>best){best=quality;d.setup_hand_slot=i;d.setup_destination=free_monster;}
                }
            }
        }
    }
    /* Set the Ritual Magic early, not only after an immediate legal match.
       This also frees a physical hand slot for future tribute draws. Do not
       request an activation unless the real engine oracle authorizes it. */
    if(d.ritual_hand_slot>=0 && d.ritual_back_slot<0 && free_back>=0 && !d.urgent){
        d.trigger=1;d.ritual_back_destination=free_back;
    }
    /* Once the real Ritual checker authorizes a SET Ritual, a negative
       printed-ATK comparison is NOT a valid reason to stall indefinitely.
       Fiend's Mirror and Psycho-Puppet have important ritual-only effects,
       just as Tri-Horned has field/horn effects and Millennium Shield has
       defensive value. This policy is shared by all ritualist profiles.
       In an emergency, defer a negative-value Ritual to the existing AI;
       the caller independently preserves a previously chosen lethal attack. */
    if(ritual_is_legal && d.ritual_back_slot>=0 &&
       (!d.urgent || d.score>=0 ||
        (p->recipe.result_card==365 && d.prep)))
        d.trigger=2;
    /* When the previous AI would consume a protected tribute, offer a
       nonprotected physical monster instead as a defensive fallback. */
    if(d.prep && d.setup_hand_slot<0 && free_monster>=0){
        int best=-1;
        for(i=0;i<b->actual_hand;i++){
            const RdCard *card=&b->hand[i];
            if(card->id>0 && card->type<CARD_TYPE_MAGIC && !d.protect_hand[i] &&
               rd_power(card)>best){
                best=rd_power(card);
                d.prefer_hand_slot=i;d.prefer_destination=free_monster;
            }
        }
    }
    return d;
}

RdPlan Rd_PlanRitual(const RdOpponentProfile *p,const RdRitualRecipe *active_recipe,
                     const RdBoard *b,int effect_mode,int ritual_is_legal)
{
    return Rd_PlanRitualMatched(p,active_recipe,b,effect_mode,ritual_is_legal,0,0);
}

/* The planner controls strategic preference, the engine controls legality;
   the hook must not silently reapply an ATK/DEF score threshold. */
int Rd_ShouldActivateRitual(const RdPlan *plan,int engine_legal)
{
    return plan && engine_legal==1 && plan->trigger==2 &&
           plan->ritual_back_slot>=0;
}
