#include "starter_rules.h"
#include "pc/cards/cards.h"
#include "pc/cards/tables.h"
#include <string.h>

/* Order follows artwork: Mask (blue), Performance (green), Yamadron (red).
   The exact fallback was verified against the USA SLUS_014.11 retail disc.
   DO NOT call gDuel_awRitualData at name-entry; it is a duel-scene scratch. */
typedef struct {u16 magic,result,field,equip;u16 vanilla[3];} RdTheme;
static const RdTheme themes[RD_STARTER_THEME_COUNT]={
  {693,720,335,323,{102,182,220}},
  {676,701,333,301,{249,395,511}},
  {666,357,332,315,{29,31,101}}
};
static const int theme_profiles[RD_STARTER_THEME_COUNT]={4,28,18};
static const u16 preferences[3][12]={
    {102,182,220,25,103,164,171,233,34,115,259,574},
    {395,249,511,110,120,231,262,293,299,352,38,403},
    {101,29,31,10,296,561,603,37,38,39,82,168}
};
int Rd_StarterSimonTheme(int player,int coin){
 return (player>=0&&player<3&&coin>=0&&coin<2)?(player+1+coin)%3:-1;
}
const RdOpponentProfile *Rd_StarterThemeProfile(int theme){
 return theme>=0&&theme<RD_STARTER_THEME_COUNT?Rd_GetProfile(theme_profiles[theme]):0;
}
static int valid(const RdOpponentProfile *p,int id,int role,const RdRecipeCache *cache,
                 const u8 copies[],int cap){
 if(id<=0||id>cap||copies[id]>=3||id==cache->recipe.result_card)return 0;
 return Rd_MatchRecipeCard(p,id,&cache->recipe.materials[role],(unsigned)role,(void*)cache);
}
static int assign(const RdOpponentProfile *p,const RdRecipeCache *cache,int theme,
 int role,u8 copies[],u16 mat[3],int cap,int *budget,RdRandom rng,void*ctx){
 int k,id,base,offset;
 if(role==3)return 1;
 if(--*budget<=0)return 0;
 /* Prefer the card on the box art when it fits; randomized themed tie breaks
    are used only when no required exact card is specified. */
 if(cache->recipe.materials[role].exact_card>0){
     id=cache->recipe.materials[role].exact_card;
     if(!valid(p,id,role,cache,copies,cap))return 0;
     copies[id]++;mat[role]=(u16)id;
     if(assign(p,cache,theme,role+1,copies,mat,cap,budget,rng,ctx))return 1;
     copies[id]--;return 0;
 }
 offset=role?rng(12,ctx):0;
 for(k=0;k<12;k++){
     id=preferences[theme][(k+offset)%12];
     if(!valid(p,id,role,cache,copies,cap))continue;
     copies[id]++;mat[role]=(u16)id;
     if(assign(p,cache,theme,role+1,copies,mat,cap,budget,rng,ctx))return 1;
     copies[id]--;
 }
 base=rng(cap,ctx);
 for(k=0;k<cap;k++){
     id=(base+k)%cap+1;
     if(!valid(p,id,role,cache,copies,cap))continue;
     copies[id]++;mat[role]=(u16)id;
     if(assign(p,cache,theme,role+1,copies,mat,cap,budget,rng,ctx))return 1;
     copies[id]--;
 }
 return 0;
}
static void exact(RdRitualRequirement *r,int id){
 memset(r,0,sizeof(*r));r->exact_card=id;
 r->type=-1;
 r->min_attack=r->max_attack=r->min_defense=r->max_defense=-1;
 r->min_level=r->max_level=-1;
}
static void vanilla_recipe(int theme,RdRecipeCache *cache){
 int i;
 memset(cache,0,sizeof(*cache));
 cache->recipe.ritual_magic=themes[theme].magic;
 cache->recipe.result_card=themes[theme].result;
 cache->recipe.material_count=3;cache->recipe.materials=cache->storage;
 for(i=0;i<3;i++)exact(&cache->storage[i],themes[theme].vanilla[i]);
 cache->source=4;cache->status=1;
}
int Rd_BuildThemedStarter(const MemoriesModHost *host,int theme,int selection,
 const u16 vanilla[40],u16 out[40],RdRandom rng,void *ctx){
 const RdOpponentProfile *p=Rd_StarterThemeProfile(theme);
 RdRecipeCache cache;
 static u8 held[RD_CARD_ID_MAX+1]; /* no large Windows stack probe */
 u16 fixed[7],trio[3];
 int role,i,n=0,budget=15000,cap=gCard_nCount,successful=0;
 if(!host||!p||!vanilla||!out||!rng||theme<0||theme>=3||cap<722||cap>RD_CARD_ID_MAX)return 0;
 vanilla_recipe(theme,&cache);
 /* Auto: pick provider/tables when available; absent means verified Vanilla.
    Force Vanilla: bypass effect MOD and all condition tables. */
 if(selection!=1){
     RdRecipeCache active;
     if(Rd_ResolveStarterRecipe(host,p,selection,&active)){
         if(active.recipe.material_count!=3){
             host->log(host,"RD Starter: theme=%d has %d active tributes; cannot fit 7-card contract",
                       theme,(int)active.recipe.material_count);
             return 0;
         }
         cache=active;
         cache.recipe.materials=cache.storage; /* repair copied internal pointer */
     }else{
         unsigned short unused[6];
         /* A table explicitly disabling the Ritual must not silently produce
            a misleading 'Vanilla-legal' thematic starter deck. */
         if(Tables_Ritual(themes[theme].magic,unused)==0){
             host->log(host,"RD Starter: active table disables Ritual #%d",themes[theme].magic);
             return 0;
         }
     }
 }
 if(cache.recipe.material_count!=3||cache.recipe.ritual_magic!=themes[theme].magic)return 0;
 memset(held,0,(size_t)cap+1);
 /* The other four guaranteed cards use the approved recipe-theme identity. */
 fixed[3]=themes[theme].magic;fixed[4]=337;
 fixed[5]=themes[theme].field;fixed[6]=themes[theme].equip;
 for(i=3;i<7;i++){
     int c=fixed[i];if(c<1||c>cap||!Cards_Valid(c)||held[c]>=3)return 0;
     held[c]++;
 }
 successful=assign(p,&cache,theme,0,held,trio,cap,&budget,rng,ctx);
 if(!successful){host->log(host,"RD Starter: no complete legal material trio for theme %d",theme);return 0;}
 for(role=0;role<3;role++)fixed[role]=trio[role];
 /* Choose 33 of Vanilla's original 40 without exceeding three copies once
    the seven fixed cards are added. No Thunder-type exclusion. */
 for(i=0;i<40&&n<33;i++){
     int id=vanilla[i];
     if(id<=0||id>cap||!Cards_Valid(id)||held[id]>=3)continue;
     out[n++]=(u16)id;held[id]++;
 }
 if(n!=33){host->log(host,"RD Starter: Vanilla sample could not supply 33 legal cards (%d)",n);return 0;}
 for(i=0;i<7;i++)out[33+i]=fixed[i];
 host->log(host,"RD Starter: theme=%d source=%d: magic=%d materials=%d/%d/%d + 33 Vanilla",
   theme,cache.source,fixed[3],fixed[0],fixed[1],fixed[2]);
 return 1;
}
