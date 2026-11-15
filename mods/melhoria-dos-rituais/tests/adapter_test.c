
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ritual_recipe_provider_v1.h"
typedef struct {int id,where,slot,rec,uid;} RitualCandidate;
typedef struct {int (*find_count_dummy)(void);void *(*find)(const void *,const char *);} MemoriesModHost;
#define RITUAL_SRC_FIELD 1
#define RITUAL_SRC_HAND 2
#define RITUAL_SRC_DECK 3
#define RESULT_OFF 0
#define CARD_TABLE_COUNT 900
static RitualCandidate candidates[50];
static int candidate_count=0, after_count=0, min_field=0, min_hand=0, setting_use_deck=1;
static int result_where,setting_require_result,recipe_result;
static int where[3],slot[3],rec[3],matid[3],source_uid[3],visual_rec[3],preconsumed[3];
static void *obj[3];
static const RitualRecipeProviderV1 *recipe_provider;
static const MemoriesModHost *mod_host;
static int check_count,oracle_value, get_count,match_count,find_count;
static void *public_check=(void*)1;
static int provider_get_return=3,provider_active=1, provider_answer=500, role_ids[3]={30,10,20};
static int allow_find=1;
static RitualRecipeProviderV1 provider;
static int get(int ritual,RitualRecipeRequirementV1 *r,unsigned capacity,int *result){
    assert(ritual==698);assert(capacity==3);assert(r);
    get_count++;
    if (!provider_active) return -1;
    *result=provider_answer;
    return provider_get_return;
}
static int matches(int ritual,unsigned role,int card_id){
    assert(ritual==698);assert(role<3); match_count++;
    return card_id==role_ids[role];
}
static void *find(const void *h,const char *n){
    (void)h; assert(!strcmp(n,"ritual-card-effects-test:recipe_v1"));find_count++;
    return allow_find ? &provider : 0;
}
static MemoriesModHost host={0,find};
static int normalize_ritual_id(int id){return id;}
static void reset_result_source(void){result_where=0;}
static void collect_candidates(void){candidate_count=after_count;}
static int combo_source_counts_ok(int a,int b,int c,int *f,int *h){
    int field=(candidates[a].where==1)+(candidates[b].where==1)+(candidates[c].where==1);
    int hand=(candidates[a].where==2)+(candidates[b].where==2)+(candidates[c].where==2);
    if(f)*f=field;
    if(h)*h=hand;
    return field>=min_field &&(!setting_use_deck || hand>=min_hand);
}
static int chain_oracle_triplet(int ritual,int a,int b,int c){
    assert(ritual==698);assert(a==0&&b==1&&c==2);check_count++;
    return oracle_value;
}

static int select_unique_provider_trio(int ritual)
{
    RitualRecipeRequirementV1 requirements[3];
    const RitualRecipeProviderV1 *provider = recipe_provider;
    int result, published_ritual, a, b, c, matched = 0, i;

    if (setting_require_result != RESULT_OFF || !public_check ||
        !mod_host || !mod_host->find) return 0;
    if (!provider) {
        provider = (const RitualRecipeProviderV1 *)mod_host->find(
            mod_host, "ritual-card-effects-test:recipe_v1");
        if (provider) recipe_provider = provider;
    }
    if (!provider || provider->abi != RITUAL_RECIPE_PROVIDER_ABI ||
        !provider->get || !provider->matches) return 0;

    result = 0;
    published_ritual = normalize_ritual_id(ritual);
    if (provider->get(published_ritual, requirements, 3,
                      &result) != 3 || result < 1 || result > CARD_TABLE_COUNT)
        return 0;

    reset_result_source();
    collect_candidates();
    if (candidate_count != 3 || !combo_source_counts_ok(0, 1, 2, 0, 0))
        return 0;

    /* A private recipe's roles can be interchangeable; verify all six role
     * allocations without assuming the cards' current field/hand/deck order. */
    for (a = 0; a < 3 && !matched; a++)
        for (b = 0; b < 3 && !matched; b++) {
            if (b == a) continue;
            c = 3 - a - b; /* 0+1+2 == 3, leaving the third distinct index. */
            if (provider->matches(published_ritual, 0, candidates[a].id) &&
                provider->matches(published_ritual, 1, candidates[b].id) &&
                provider->matches(published_ritual, 2, candidates[c].id))
                matched = 1;
        }
    if (!matched) return 0;

    /* The original fallback's first legal trio is exactly (0,1,2) here.
     * It must agree with the provider; other hooks always win. */
    if (chain_oracle_triplet(ritual, 0, 1, 2) != result) return 0;
    recipe_result = result;
    /* As in the generic fallback, finish with a validation against the
     * selected physical cards to leave hook publishers coherent. */
    (void)chain_oracle_triplet(ritual, 0, 1, 2);
    for (i = 0; i < 3; i++) {
        const RitualCandidate *cand = &candidates[i];
        where[i] = cand->where;
        slot[i] = cand->slot;
        rec[i] = cand->rec;
        matid[i] = cand->id;
        source_uid[i] = cand->uid;
        visual_rec[i] = -1;
        preconsumed[i] = 0;
        obj[i] = 0;
    }
    return 1;
}



static void reset_test(void){
    int i;
    setting_require_result=0; min_field=0;min_hand=0;setting_use_deck=1;
    provider_active=1;provider_get_return=3;provider_answer=500;
    provider.abi=RITUAL_RECIPE_PROVIDER_ABI;provider.get=get;provider.matches=matches;
    recipe_provider=0;mod_host=&host; allow_find=1;
    check_count=get_count=match_count=find_count=0;oracle_value=500;
    recipe_result=-99;after_count=3;result_where=1;
    for(i=0;i<3;i++){
        candidates[i]=(RitualCandidate){(i+1)*10,i+1,i,i+5,i+50};
        where[i]=-99;slot[i]=-99;rec[i]=-99;matid[i]=-99;source_uid[i]=-99;
        visual_rec[i]=1;preconsumed[i]=1;obj[i]=(void*)1;
    }
}
int main(void){
    reset_test();
    assert(select_unique_provider_trio(698)==1);assert(recipe_result==500);
    assert(check_count==2 && get_count==1 && find_count==1 && match_count>0);
    for(int i=0;i<3;i++){
        assert(matid[i]==(i+1)*10); assert(slot[i]==i);assert(rec[i]==i+5);
        assert(where[i]==i+1);assert(source_uid[i]==i+50);
        assert(visual_rec[i]==-1&&preconsumed[i]==0&&obj[i]==0);
    }
    // Correct pointer cached, producer toggled OFF => its get returns -1.
    provider_active=0;check_count=0;
    assert(select_unique_provider_trio(698)==0 && check_count==0 && find_count==1);
    reset_test();allow_find=0;
    assert(select_unique_provider_trio(698)==0 && find_count==1 && check_count==0);
    reset_test();provider.abi=12;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test();provider.get=0;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test();provider.matches=0;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test();provider_get_return=-1;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test();provider_answer=901;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test();after_count=4;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test();after_count=2;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test(); min_field=2;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test(); min_hand=2;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    reset_test();setting_require_result=1;
    assert(select_unique_provider_trio(698)==0 && check_count==0 && find_count==0);
    reset_test();role_ids[0]=99;
    assert(select_unique_provider_trio(698)==0 && check_count==0);
    role_ids[0]=30;
    reset_test();oracle_value=333;
    assert(select_unique_provider_trio(698)==0 && check_count==1 && recipe_result==-99);
    reset_test();after_count=3;candidates[0].where=1;candidates[1].where=1;candidates[2].where=2;
    min_field=2;min_hand=1;
    assert(select_unique_provider_trio(698)==1 && where[0]==1 && where[1]==1 && where[2]==2);
    puts("PASS: 15 provider integration scenarios (compiled exact production function)");
    return 0;
}
