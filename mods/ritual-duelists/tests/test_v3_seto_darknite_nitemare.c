/* V3 Beta.7: structural and simulated deck checks, no Windows runtime claim. */
#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned rng_state=0x7611BEEF;
static int rng(int bound,void *unused)
{
    (void)unused;
    rng_state=rng_state*1664525u+1013904223u;
    return (int)((rng_state>>1)%(unsigned)bound);
}
static int weight(const RdOpponentProfile *p,int id)
{
    int i; for(i=0;i<p->pool_count;i++)if(p->pool[i].id==id)return p->pool[i].weight;
    return 0;
}
static RdRitualRequirement exact(int id)
{
    RdRitualRequirement r={0};r.exact_card=id;r.type=RD_ANY;
    r.min_attack=r.max_attack=r.min_defense=r.max_defense=RD_ANY;
    r.min_level=r.max_level=RD_ANY;return r;
}
static RdRitualRequirement mock_chakra_fiend(void)
{
    RdRitualRequirement r=exact(0);
    r.type=7; /* Fixture-only encoded Fiend predicate */
    r.min_level=7;
    r.custom_group=254; /* Fixture-only not-Chakra predicate */
    return r;
}
static int matcher(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned role,void *unused)
{
    (void)p;(void)role;(void)unused;
    if(id<1||id>722)return 0;
    if(r->exact_card)return r->exact_card==id;
    if(r->custom_group==254)return id==360||id==385||id==391||id==715;
    return 1;
}
static int count(const u16 deck[RD_DECK_SIZE],int id)
{
    int i,n=0;for(i=0;i<RD_DECK_SIZE;i++)n+=deck[i]==id;return n;
}
static void simulate(const RdOpponentProfile *p,int mode,int free_guarantee,int recipe_mode)
{
    RdRitualRecipe recipe=p->recipe;
    RdRitualRequirement roles[3];
    RdPackageStats stats;
    u16 deck[RD_DECK_SIZE];
    int run,zero=0;
    assert(recipe.ritual_magic && recipe.result_card && p->pool_count>0);
    if(p->recipe.ritual_magic==675){
        roles[0]=roles[1]=roles[2]=exact(1);
    }else if(recipe_mode==0){
        roles[0]=exact(178);roles[1]=exact(269);roles[2]=exact(288);
    }else{
        roles[0]=exact(369);
        roles[1]=roles[2]=mock_chakra_fiend();
    }
    recipe.materials=roles;recipe.material_count=3;
    for(run=0;run<420;run++){
        int copies[723]={0},i,j,k;
        assert(Rd_GeneratePackageDeck(p,&recipe,matcher,0,722,mode,free_guarantee,deck,rng,0,&stats)==40);
        assert(stats.packages<=RD_MAX_PACKAGES);
        if(mode==2 || free_guarantee)assert(stats.packages>=1);
        else if(stats.packages==0)zero++;
        for(i=0;i<40;i++){
            assert(deck[i]>=1&&deck[i]<=722);
            assert(++copies[deck[i]]<=3);
        }
        assert(count(deck,recipe.ritual_magic)==stats.packages);
        for(i=0;i<stats.packages;i++){
            assert(stats.included[i][0]==recipe.ritual_magic);
            for(j=0;j<3;j++){
                int material=stats.included[i][j+1];
                assert(matcher(p,material,&roles[j],j,0));
                assert(count(deck,material)>=1);
            }
            if(recipe.ritual_magic==675){
                assert(stats.packages==1);
                assert(stats.included[i][1]==1&&stats.included[i][2]==1&&stats.included[i][3]==1);
                assert(count(deck,1)==3);
            }else if(recipe_mode==1){
                assert(stats.included[i][1]==369);
                for(k=2;k<=3;k++)assert(stats.included[i][k]!=709);
            }
        }
        if(recipe.ritual_magic==675 && stats.packages==0)assert(count(deck,1)<=3);
    }
    if(mode==6 && !free_guarantee)assert(zero>0);
}
int main(void)
{
    const RdOpponentProfile *seto=Rd_GetProfile(25),*dark=Rd_GetProfile(26),*night=Rd_GetProfile(27);
    const RdOpponentProfile *old_is=Rd_GetProfile(0);
    int i,m,sum;
    assert(Rd_ProfileCount()==29&&Rd_GetProfile(29)==0&&RD_PROFILE_CAPACITY>=29);
    assert(old_is && !old_is->recipe.ritual_magic && old_is->pool_count==101);
    assert(seto && strcmp(seto->duelist_name,"Seto 3rd")==0 && seto->pool_count==63);
    assert(dark && strcmp(dark->duelist_name,"DarkNite")==0 && dark->pool_count==91);
    assert(night && strcmp(night->duelist_name,"Nitemare")==0 && night->pool_count==70);
    assert(seto->recipe.ritual_magic==675 && seto->recipe.result_card==380);
    assert(dark->recipe.ritual_magic==694 && dark->recipe.result_card==709);
    assert(night->recipe.ritual_magic==694 && night->recipe.result_card==709);
    for(i=25;i<28;i++){
        const RdOpponentProfile *p=Rd_GetProfile(i);
        sum=0;
        for(int j=0;j<p->pool_count;j++)sum+=p->pool[j].weight;
        assert(sum==2018 && sum+RD_PACKAGE_WEIGHT==2048);
    }
    assert(weight(seto,672)==130&&weight(seto,1)==120&&weight(seto,380)==80&&!weight(seto,675));
    for(i=330;i<=334;i++)assert(weight(dark,i)==74);
    assert(weight(dark,335)==80&&!weight(dark,709)&&!weight(dark,694));
    for(i=0;i<6;i++)assert(weight(night,(int[]){79,90,99,424,518,555}[i])==3);
    assert(weight(night,709)==16&&weight(night,380)==32&&weight(night,613)==148&&!weight(night,694));
    /* Story, Free OFF/ON, and both Chakra active/Vanilla recipe conditions. */
    for(i=25;i<28;i++)for(m=0;m<3;m++){
        int mode=m==0?2:6;
        int free_guarantee=m==2;
        int recipe_mode;
        for(recipe_mode=0;recipe_mode<(i==25?1:2);recipe_mode++)
            simulate(Rd_GetProfile(i),mode,free_guarantee,recipe_mode);
    }
    /* Impossible provider recipe must not produce an incomplete guaranteed bundle. */
    {
        RdRitualRecipe r=dark->recipe;RdRitualRequirement bad[3]={exact(369),exact(723),exact(391)};
        u16 deck[40];RdPackageStats stats;
        r.materials=bad;r.material_count=3;
        assert(!Rd_GeneratePackageDeck(dark,&r,matcher,0,722,2,0,deck,rng,0,&stats));
        assert(stats.error==1 && stats.packages==0);
    }
    puts("PASS: V3 Seto 3rd, DarkNite, Nitemare — Story/Free and Chakra dynamic/Vanilla, copy cap, 40 cards, package safety");
    return 0;
}
