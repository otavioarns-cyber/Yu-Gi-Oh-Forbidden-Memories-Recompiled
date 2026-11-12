#include "adaptive_rules.h"
#include "pc/mods/modapi.h"
#include "game/card_constants.h"
#include <string.h>

void Rd_ReadAdaptiveRules(const MemoriesModHost *host, RdAdaptiveRules *out)
{
    const RitualRulesProviderV1 *provider;
    RitualRulesSnapshotV1 snapshot;
    memset(out,0,sizeof(*out));
    if (!host || !host->find) return;
    /* Late lookup supports arbitrary order and live mod activation changes. */
    provider=(const RitualRulesProviderV1 *)host->find(host,"melhoria-dos-rituais:rules_v1");
    if (!provider || provider->abi_version != RITUAL_RULES_PROVIDER_ABI_V1 ||
        !provider->read) return;
    memset(&snapshot,0,sizeof(snapshot));
    if (!provider->read(&snapshot,sizeof(snapshot)) ||
        snapshot.abi_version != RITUAL_RULES_PROVIDER_ABI_V1 ||
        snapshot.material_count != 3 || snapshot.min_field < 0 ||
        snapshot.min_field > 3 || snapshot.min_hand < 0 ||
        snapshot.min_hand > 3 || snapshot.require_result < 0 ||
        snapshot.require_result > 2 ||
        (snapshot.sources & (RITUAL_RULES_SOURCES_FIELD|RITUAL_RULES_SOURCES_HAND)) !=
        (RITUAL_RULES_SOURCES_FIELD|RITUAL_RULES_SOURCES_HAND) ||
        (snapshot.sources & ~7u)) return;
    out->available=1;
    out->material_count=snapshot.material_count;
    out->sources=snapshot.sources;
    out->min_field=snapshot.min_field;
    out->min_hand=(snapshot.sources & RITUAL_RULES_SOURCES_DECK) ? snapshot.min_hand:0;
    out->require_result=snapshot.require_result;
}
unsigned Rd_AdaptiveRuleKey(const RdAdaptiveRules *r)
{
    unsigned h=2166136261u;
    const unsigned values[]={ (unsigned)r->available,r->material_count,
        r->sources,(unsigned)r->min_field,(unsigned)r->min_hand,
        (unsigned)r->require_result };
    unsigned i;
    for(i=0;i<sizeof(values)/sizeof(values[0]);i++)h=(h^values[i])*16777619u;
    return h;
}
static const RdCard *slot_card(const RdBoard *b,int i)
{
    return i<5?&b->own[i]:&b->hand[i-5];
}
static int matches(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
                   const RdCard *card,unsigned role,RdMaterialMatch match,void *ctx)
{
    const RdRitualRequirement *r=&recipe->materials[role];
    if(!card->id || card->type>=CARD_TYPE_MAGIC)return 0;
    if(match)return match(p,card->id,r,role,ctx);
    if(r->exact_card && card->id != r->exact_card)return 0;
    if(r->type>=0 && card->type != r->type)return 0;
    if(r->custom_group && (!p->in_group || !p->in_group(p,card->id,r->custom_group)))return 0;
    if(r->min_attack>=0 && card->atk<r->min_attack)return 0;
    if(r->max_attack>=0 && card->atk>r->max_attack)return 0;
    if(r->min_defense>=0 && card->def<r->min_defense)return 0;
    if(r->max_defense>=0 && card->def>r->max_defense)return 0;
    if(r->min_level>=0 && card->level<r->min_level)return 0;
    if(r->max_level>=0 && card->level>r->max_level)return 0;
    if(r->defense_gt_attack && card->def<=card->atk)return 0;
    return 1;
}
/* Maximum distinct role matching for three positions, with/without a
 * reserved hand candidate. No role or specific card is special-cased. */
static int best_match(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
                      const RdBoard *b,int field_only,int excluded,
                      RdMaterialMatch match,void *ctx)
{
    int mask[3]={0,0,0},a,i,j,n=0,limit=field_only?5:5+b->actual_hand;
    for(i=0;i<limit;i++){
        if(i==excluded)continue;
        for(j=0;j<3;j++)if(matches(p,recipe,slot_card(b,i),(unsigned)j,match,ctx))
            mask[j]|=1<<i;
    }
    for(a=0;a<limit;a++)if(mask[0]&(1<<a)){
        int count=1;
        for(i=0;i<limit;i++)if(i!=a&&(mask[1]&(1<<i))){
            if(count<2)count=2;
            for(j=0;j<limit;j++)if(j!=a&&j!=i&&(mask[2]&(1<<j)))return 3;
        }
        if(count>n)n=count;
    }
    /* Roles can be missing in arbitrary order. */
    if(n<2)for(a=0;a<3;a++)for(i=0;i<limit;i++)if(mask[a]&(1<<i)){
        int k;
        if(n<1)n=1;
        for(k=a+1;k<3;k++)for(j=0;j<limit;j++)
            if(i!=j && (mask[k]&(1<<j)))n=2;
    }
    return n;
}
static int has_physical_result(const RdRitualRecipe *recipe,const RdBoard *b)
{
    int i;
    for(i=0;i<b->actual_hand;i++)if(b->hand[i].id==recipe->result_card)return 1;
    return 0;
}
static int monster_exposure_safe(const RdBoard *b,const RdCard *c)
{
    int i,enemy_atk=0;
    for(i=0;i<5;i++)if(b->enemy[i].id && !b->enemy[i].face_down &&
                            b->enemy[i].atk>enemy_atk)enemy_atk=b->enemy[i].atk;
    /* A forced play currently has no independently verified DEF-placement
       contract, so judge against ATK; hidden enemy stats are NEVER read. */
    return !enemy_atk || c->atk>=enemy_atk;
}
void Rd_RefineRitualPlan(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
                         const RdBoard *b,int engine_legal,
                         RdMaterialMatch match,void *ctx,
                         const RdAdaptiveRules *rules,RdPlan *plan)
{
    int present,full,needs,has_magic,can_wait,best=-1000000,i,j,free_slot=-1;
    int excluded=-1;
    if(!rules || !rules->available || !plan || !p || !recipe || !b ||
       recipe->material_count!=3 || rules->material_count!=3)return;
    /* Preserve only field monsters whose removal would actually damage
       the active recipe matching or violate the minimum Field count. This
       prevents an over-reservation deadlock with 4-5 eligible monsters. */
    {
        int before_field=best_match(p,recipe,b,1,-1,match,ctx);
        int before_total=best_match(p,recipe,b,0,-1,match,ctx);
        for(i=0;i<5;i++)if(plan->protect_field[i]){
            RdBoard without=*b;
            int after_field,after_total;
            without.own[i].id=0;
            after_field=best_match(p,recipe,&without,1,-1,match,ctx);
            after_total=best_match(p,recipe,&without,0,-1,match,ctx);
            if(after_total>=before_total &&
               !(after_field<before_field &&
                 before_field<=rules->min_field))
                plan->protect_field[i]=0;
        }
    }
    /* Melhoria's policy takes precedence over an assumed source mode
       embedded in unrelated Vanilla/recipe tables. */
    plan->setup_hand_slot=-1;
    plan->setup_destination=-1;
    if(engine_legal || !plan->prep || b->actual_hand<=0)return;
    if(plan->urgent && recipe->result_card!=365)return;
    if(rules->min_field+rules->min_hand>3)return; /* impossible Settings */
    if(!(rules->sources & RITUAL_RULES_SOURCES_FIELD) || rules->min_field==0)return;
    if(rules->require_result==1 && !has_physical_result(recipe,b))return;
    /* Do not count a required physical result card as a tribute in broad
       custom recipes; the execution engine reserves it independently. */
    if(rules->require_result && has_physical_result(recipe,b))
        for(i=0;i<b->actual_hand;i++)if(b->hand[i].id==recipe->result_card){
            excluded=5+i;break;
        }
    /* A result monster explicitly demanded by another mod is not a spare
       hand card to exchange for Deck access. */
    if(rules->require_result)
        for(i=0;i<b->actual_hand;i++)
            if(b->hand[i].id==recipe->result_card)plan->protect_hand[i]=1;
    present=best_match(p,recipe,b,1,excluded,match,ctx);
    needs=rules->min_field-present;
    if(needs<=0)return; /* meet Field minimum; additional tributes stay in hand */
    full=best_match(p,recipe,b,0,excluded,match,ctx);
    has_magic=plan->ritual_back_slot>=0 || plan->ritual_hand_slot>=0;
    can_wait=!(has_magic&&full>=3) &&
        !(rules->min_field>=2 && has_magic && full>=2) &&
        !((rules->sources & RITUAL_RULES_SOURCES_DECK) && has_magic &&
          full>=rules->min_field+rules->min_hand) &&
        b->actual_hand<5;
    if(can_wait)return; /* no benefit from exposing weak materials yet */
    if(plan->ritual_hand_slot>=0 && plan->ritual_back_slot<0 &&
       plan->trigger==1)return; /* set the Ritual Magic before spending monsters */
    for(i=0;i<5;i++)if(!b->own[i].id){free_slot=i;break;}
    if(free_slot<0)return;
    for(i=0;i<b->actual_hand;i++){
        const RdCard *card=&b->hand[i];
        int suitable=0,score;
        if(i+5==excluded || !card->id || card->type>=CARD_TYPE_MAGIC)continue;
        for(j=0;j<3;j++)if(matches(p,recipe,card,(unsigned)j,match,ctx)){
            suitable=1;break;
        }
        if(!suitable)continue;
        /* Do not waste a field placement on a duplicate that cannot fill a
           new distinct recipe role. The Field minimum is a count of actual
           recipe tributes, not just a count of monsters on the board. */
        {
            RdBoard trial=*b;
            trial.own[free_slot]=*card;
            if(best_match(p,recipe,&trial,1,excluded,match,ctx)<=present)continue;
        }
        /* When a Hand tribute is mandatory, moving the only matching hand
           monster onto the Field would undo our progress. Pull a distinct
           Field material from the real Deck instead, if it exists. */
        if((rules->sources & RITUAL_RULES_SOURCES_DECK) &&
           rules->min_hand>0){
            RdBoard after=*b;
            int k;
            after.hand[i].id=0;
            for(k=0;k<5;k++)after.own[k].id=0;
            if(best_match(p,recipe,&after,0,excluded,match,ctx)<rules->min_hand)
                continue;
        }
        /* Only waive exposure risk when all physical materials and a SET
           Ritual are ready for a same-turn engine-authorized activation. */
        if(!(full==3&&plan->ritual_back_slot>=0) &&
           !(recipe->result_card==365 && plan->ritual_back_slot>=0 &&
             plan->matched_materials>=2) &&
           !monster_exposure_safe(b,card))continue;
        score=4000-(card->atk>card->def?card->atk:card->def)/2;
        if(monster_exposure_safe(b,card))score+=250;
        if(score>best){best=score;plan->setup_hand_slot=i;}
    }
    if(plan->setup_hand_slot>=0)plan->setup_destination=free_slot;
}
