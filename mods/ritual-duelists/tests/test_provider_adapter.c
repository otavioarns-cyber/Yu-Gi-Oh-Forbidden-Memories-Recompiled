/* Compile standalone with SDK headers, using faithful v0.3 table signatures.
   No game executable or proprietary disc content is required. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ritual_duelists.h"
#include "recipe_adapter.h"
#include "pc/cards/tables.h"
#include "pc/cards/cards.h"
#include "game/card_constants.h"
#include "game/duel_check_ritual.h"
#include "planner.h"

s32 gDuel_adwCardStats[900];
u16 gDuel_awRitualData[20];
int gCard_nCount=880;
static int extended_count,extended_from,disabled;
int Cards_Valid(int id){return id>=1 && id<=gCard_nCount;}
int Cards_Type(int id){return id==679?22:3;}
int Cards_BaseId(int id){return id;}
int Cards_Level(int id){(void)id;return 8;}
int Cards_InFusionGroup(int id,int group){return id==859 && group==2;}
int Tables_RitualRule(int magic,TablesRitualRule *out){
    int i;
    if(magic!=679 || extended_count==0)return 0;
    memset(out,0,sizeof(*out));
    out->count=extended_count;out->from=extended_from;out->result=704;
    for(i=0;i<extended_count;i++){
        TablesRitualRequirement *r=&out->requirements[i];
        r->card=(unsigned short)(850+i);
        r->type=-1;r->min_level=-1;r->max_level=-1;
        r->max_attack=-1;r->max_defense=-1;
    }
    return 1;
}
int Tables_RitualRequirements(int magic,TablesRitualRequirement out[3],unsigned short *result){
    (void)magic;(void)out;(void)result;return 0;
}
int Tables_Ritual(int magic,unsigned short recipe[6]){
    (void)magic;(void)recipe;return disabled?0:-1;
}
static int variants=0;
static int provider_enabled,provider_matches_calls;
static int test_provider_get(int magic, RitualRecipeRequirementV1 *out,unsigned capacity,int *result){
    unsigned j;
    if(!provider_enabled||magic!=679||capacity<3)return -1;
    for(j=0;j<3;j++){
        out[j]=(RitualRecipeRequirementV1){0};
        out[j].type=out[j].min_attack=out[j].max_attack=out[j].min_defense=
            out[j].max_defense=out[j].min_level=out[j].max_level=-1;
    }
    out[0].exact_card=535;out[1].custom_group=255;out[2].type=3;
    *result=704;return 3;
}
static int test_provider_matches(int magic,unsigned role,int id){
    provider_matches_calls++;
    if(magic!=679||!provider_enabled)return 0;
    return (role==0&&id==535)||(role==1&&id==545)||(role==2&&id==378);
}
static const RitualRecipeProviderV1 provider={1,test_provider_get,test_provider_matches};
static void *lookup_provider(const MemoriesModHost *host,const char *name){
    (void)host;return strcmp(name,"ritual-card-effects-test:recipe_v1")==0?(void*)&provider:0;
}
static unsigned rngstate=1847u;
static int rng(int max,void *ctx){(void)ctx;rngstate=rngstate*1664525u+1013904223u;return (int)((rngstate>>1)%(unsigned)max);}
static int count_card(const u16 cards[40],int id){int i,n=0;for(i=0;i<40;i++)n+=(cards[i]==id);return n;}
int main(void){
    MemoriesModHost host={0};
    const RdOpponentProfile *p=Rd_GetProfile(23);
    RdRecipeCache cache;RdPackageStats st;int count,id,where;
    u16 deck[40];
    host.find=lookup_provider;
    /* No table => genuine disc fallback; the 3 materials are not inferred. */
    gDuel_awRitualData[0]=679;gDuel_awRitualData[1]=535;
    gDuel_awRitualData[2]=555;gDuel_awRitualData[3]=556;
    gDuel_awRitualData[4]=704;gDuel_awRitualData[5]=0;
    assert(Rd_ResolveRecipe(&host,p,0,&cache));
    assert(cache.source==4 && cache.recipe.material_count==3);
    assert(cache.recipe.materials[1].exact_card==555);
    disabled=1;
    assert(!Rd_ResolveRecipe(&host,p,0,&cache));
    disabled=0;
    /* With Efeitos active, even unchanged official tables must NOT win. */
    disabled=0;provider_enabled=1;
    assert(Rd_ResolveRecipe(&host,p,0,&cache));
    assert(cache.source==1 && cache.recipe.material_count==3);
    assert(cache.recipe.materials[0].exact_card==535);
    assert(cache.recipe.materials[1].custom_group==255);
    assert(Rd_MatchRecipeCard(p,545,&cache.recipe.materials[1],1,&cache));
    assert(!Rd_MatchRecipeCard(p,535,&cache.recipe.materials[1],1,&cache));
    assert(provider_matches_calls==2);
    assert(Rd_GeneratePackageDeck(p,&cache.recipe,Rd_MatchRecipeCard,
               &cache,880,2,0,deck,rng,0,&st)==40);
    assert(st.packages>=1 && count_card(deck,679)>=1 && count_card(deck,535)>=1);
    /* Disabled Efeitos is still loaded; consumer MUST use official rules. */
    provider_enabled=0;
    assert(Rd_ResolveRecipe(&host,p,0,&cache));
    assert(cache.source==4 && cache.recipe.materials[1].exact_card==555);
    /* The new official variable-cardinality recipe is the source of truth.
       No opaque 3-tribute fallback, even with previously loaded disc data. */
    for(count=1;count<=5;count++)for(where=0;where<3;where++){
        extended_count=count;extended_from=where;
        assert(Rd_ResolveRecipe(&host,p,0,&cache));
        assert(cache.source==6 && cache.recipe.material_count==(unsigned)count);
        assert(cache.recipe.material_sources==(unsigned)(where==0?1:where==1?2:3));
        assert(cache.recipe.result_card==704);
        assert(cache.recipe.materials[0].exact_card==850);
        assert(Rd_MatchRecipeCard(p,850,cache.recipe.materials,0,&cache));
        if(count>1)assert(!Rd_MatchRecipeCard(p,850,cache.recipe.materials+1,1,&cache));
        if(count==3){
            assert(Rd_GeneratePackageDeck(p,&cache.recipe,Rd_MatchRecipeCard,
                      &cache,gCard_nCount,2,0,deck,rng,0,&st)==40);
            for(id=850;id<850+count;id++)assert(count_card(deck,id)>=st.packages);
            assert(count_card(deck,679)>=st.packages);
        } else {
            /* Non-three-material official rules remain valid for AI, but
               cannot be forced into a misleading 4-card Ritual Package. */
            assert(!Rd_GeneratePackageDeck(p,&cache.recipe,Rd_MatchRecipeCard,
                      &cache,gCard_nCount,2,0,deck,rng,0,&st));
        }
        variants++;
    }
    extended_count=2;extended_from=2;
    assert(Rd_ResolveRecipe(&host,p,0,&cache));
    /* Third-party cards still obey the official conditions. */
    { RdRitualRequirement role=cache.storage[0];
      role.exact_card=0;role.custom_group=-2;
      assert(Rd_MatchRecipeCard(p,859,&role,0,&cache));
      assert(!Rd_MatchRecipeCard(p,858,&role,0,&cache));
    }
    /* With native source rules, the planner must ignore a forbidden source. */
    {
      RdBoard b={0};RdPlan plan;
      b.actual_hand=1;b.hand_count=1;b.own_lp=8000;b.enemy_lp=8000;
      b.back[0]=(RdCard){679,0,0,22,0,0,0};
      b.own[0]=(RdCard){850,1000,1000,3,0,8,0};
      b.own[1]=(RdCard){851,1000,1000,3,0,8,0};
      cache.recipe.material_sources=2; /* HAND only */
      plan=Rd_PlanRitualMatched(p,&cache.recipe,&b,1,0,Rd_MatchRecipeCard,&cache);
      assert(!plan.legal_set);
      cache.recipe.material_sources=1; /* FIELD only */
      plan=Rd_PlanRitualMatched(p,&cache.recipe,&b,1,0,Rd_MatchRecipeCard,&cache);
      assert(plan.legal_set);
    }
    /* Formula never authorizes a Ritual; engine remains responsible. */
    extended_count=0;
    printf("PASS provider priority/fallback + v0.3 adapter: %d official combinations, 1-5 tributes, Field/Hand/Both, \n"
           "high card IDs #850-854, Ritual Package rejects unsupported cardinalities, Vanilla/removal and shared matching\n",variants);
    return 0;
}
