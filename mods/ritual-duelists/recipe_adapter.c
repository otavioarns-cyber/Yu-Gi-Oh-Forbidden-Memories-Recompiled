#include "recipe_adapter.h"
#include "pc/cards/cards.h"
#include "pc/cards/tables.h"
#include "game/card_constants.h"
#include "game/duel_check_ritual.h"
#include "unmatched.h"
#include <string.h>
extern s32 gDuel_adwCardStats[];

/* Only the disc fallback has exactly three named tributes. The v0.3.0
   official table API supports one to five Field/Hand tributes, and external
   providers can describe other recipes. This MOD never executes a Ritual. */
static void rd_exact(RdRitualRequirement *role,int id)
{
    memset(role,0,sizeof(*role));
    role->exact_card=id;
    role->type=RD_ANY;
    role->min_attack=role->max_attack=RD_ANY;
    role->min_defense=role->max_defense=RD_ANY;
    role->min_level=role->max_level=RD_ANY;
}
static int rd_stock_recipe(int magic,RdRecipeCache *out)
{
    unsigned short *entry = gDuel_awRitualData;
    unsigned i;
    /* Only called from Duel_ShuffleDeck after Duel_InitScene has requested
       the opponent's card/ritual package. Check a strict bound. */
    for(i=0;i<150;i++,entry+=5){
        if(entry[0]==0)break;
        if(entry[0]==magic){
            unsigned j;
            out->recipe.material_count=3;
            out->recipe.result_card=entry[4];
            for(j=0;j<3;j++)rd_exact(&out->storage[j],entry[j+1]);
            out->source=4;return 1;
        }
    }
    return 0;
}
static void rd_from_official(RdRitualRequirement *d,const TablesRitualRequirement *t)
{
    d->exact_card=t->card;
    d->type=t->type;
    /* Negative group IDs mean official fusion groups, never profile groups. */
    d->custom_group=t->fusion_group?-(int)t->fusion_group:0;
    d->min_attack=t->min_attack;d->max_attack=t->max_attack;
    d->min_defense=t->min_defense;d->max_defense=t->max_defense;
    d->min_level=t->min_level;d->max_level=t->max_level;
    d->defense_gt_attack=t->defense_gt_attack;
}
static int rd_tables_recipe(int magic,RdRecipeCache *out,int allow_stock)
{
    TablesRitualRequirement t[3]; unsigned short result=0; unsigned j;
    TablesRitualRule extended;
    unsigned short raw[6]; int status;
    /* New in the official v0.3.0 API. Read BEFORE the legacy 3-material
       queries: those return -1/0 for extended rules and could be mistaken
       for Vanilla. Copies are stored, never pointers to volatile tables. */
    if(Tables_RitualRule(magic,&extended)){
        if(extended.count<1 || extended.count>TABLES_RITUAL_TRIBUTE_MAX ||
           extended.count>RD_MAX_RITUAL_MATERIALS ||
           !Cards_Valid(extended.result))return -1;
        out->recipe.material_count=(unsigned)extended.count;
        out->recipe.result_card=extended.result;
        out->material_sources=extended.from;
        out->recipe.material_sources=(unsigned)(extended.from==TABLES_TRIBUTES_FIELD?1:
            extended.from==TABLES_TRIBUTES_HAND?2:3);
        for(j=0;j<(unsigned)extended.count;j++)
            rd_from_official(&out->storage[j],&extended.requirements[j]);
        out->source=6;return 1;
    }
    if(Tables_RitualRequirements(magic,t,&result)){
        out->recipe.material_count=3;out->recipe.result_card=result;
        for(j=0;j<3;j++){
            RdRitualRequirement *d=&out->storage[j];
            rd_from_official(d,&t[j]);
        }
        out->source=2;return 1;
    }
    status=Tables_Ritual(magic,raw);
    if(status==0)return -1; /* Explicitly disabled: do not use Vanilla. */
    if(status>0){
        out->recipe.material_count=3;out->recipe.result_card=raw[4];
        for(j=0;j<3;j++)rd_exact(&out->storage[j],raw[j+1]);
        out->source=3;return 1;
    }
    return allow_stock?rd_stock_recipe(magic,out):0;
}
static int rd_resolve_ex(const MemoriesModHost *host,const RdOpponentProfile *p,
                     int selection,RdRecipeCache *out,int allow_stock)
{
    int n,result;const RdRecipeProviderV1 *provider;
    if(!host||!p||!out)return 0;
    memset(out,0,sizeof(*out));
    out->recipe=p->recipe;
    out->recipe.materials=out->storage;
    if(selection==2 && p->recipe.material_count==0){
        /* Unknown effect-specific conditional recipe: never invent a role.
         * The genuine disc fallback remains authoritative. */
        n=allow_stock?rd_stock_recipe(p->recipe.ritual_magic,out):0;
        if(n>0){out->status=1;return 1;}
        return 0;
    }
    if(selection==2){ /* Manual fallback for known described recipe. */
        if(p->recipe.material_count>RD_MAX_RITUAL_MATERIALS)return 0;
        if(p->recipe.material_count){
            if(!p->recipe.materials)return 0;
            memcpy(out->storage,p->recipe.materials,
                  p->recipe.material_count*sizeof(out->storage[0]));
        }
        out->source=5;out->status=1;return 1;
    }
    if(selection==0){
        /* Optional interface from Efeitos de Rituais; no hard requirement. */
        provider=(const RdRecipeProviderV1 *)host->find(
            host,"ritual-card-effects-test:recipe_v1");
        if(provider&&provider->abi==RD_RECIPE_PROVIDER_ABI&&provider->get){
            result=out->recipe.result_card;
            n=provider->get(p->recipe.ritual_magic,out->storage,
                            RD_MAX_RITUAL_MATERIALS,&result);
            if(n>=0&&n<=RD_MAX_RITUAL_MATERIALS && Cards_Valid(result)){
                out->recipe.result_card=(u16)result;
                out->recipe.material_count=(unsigned)n;
                out->provider=provider;out->source=1;out->status=1;return 1;
            }
        }
        /* External providers can be added to this registry over time. */
    }
    /* Manual Vanilla bypasses modifications to table declarations. */
    n=selection==1?(allow_stock?rd_stock_recipe(p->recipe.ritual_magic,out):0):
                   rd_tables_recipe(p->recipe.ritual_magic,out,allow_stock);
    if(n>0){out->status=1;return 1;}
    if(n<0){out->status=0;return 0;}
    /* An unavailable/opaque recipe must not be misrepresented as Vanilla. */
    return 0;
}
int Rd_ResolveRecipe(const MemoriesModHost *h,const RdOpponentProfile *p,int mode,RdRecipeCache *out){
 return rd_resolve_ex(h,p,mode,out,1);
}
int Rd_ResolveStarterRecipe(const MemoriesModHost *h,const RdOpponentProfile *p,int mode,RdRecipeCache *out){
 return rd_resolve_ex(h,p,mode,out,0);
}
int Rd_MatchRecipeCard(const RdOpponentProfile *p,int card_id,
                      const RdRitualRequirement *role,unsigned index,void *ctx)
{
    const RdRecipeCache *cache=(const RdRecipeCache *)ctx;
    int atk,def,stats;
    if(!role||!p||!Cards_Valid(card_id)||Cards_Type(card_id)>=CARD_TYPE_MAGIC)return 0;
    if(cache&&cache->provider&&cache->provider->matches)
        return cache->provider->matches(cache->recipe.ritual_magic,index,card_id)!=0;
    if(role->exact_card>0&&card_id!=role->exact_card &&
       Cards_BaseId(card_id)!=role->exact_card)return 0;
    if(role->type>=0&&Cards_Type(card_id)!=role->type)return 0;
    if(role->custom_group>0&&(!p->in_group||
       !p->in_group(p,card_id,role->custom_group)))return 0;
    if(role->custom_group<0&&
       !Cards_InFusionGroup(card_id,-role->custom_group))return 0;
    stats=gDuel_adwCardStats[card_id-1];
    atk=(stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    def=((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    if(role->min_attack>=0&&atk<role->min_attack)return 0;
    if(role->max_attack>=0&&atk>role->max_attack)return 0;
    if(role->min_defense>=0&&def<role->min_defense)return 0;
    if(role->max_defense>=0&&def>role->max_defense)return 0;
    if(role->min_level>=0&&Cards_Level(card_id)<role->min_level)return 0;
    if(role->max_level>=0&&Cards_Level(card_id)>role->max_level)return 0;
    if(role->defense_gt_attack&&def<=atk)return 0;
    return 1;
}
