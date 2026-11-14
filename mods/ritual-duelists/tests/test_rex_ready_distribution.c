#include <assert.h>
#include <stdio.h>
#include "ritual_duelists.h"
static unsigned seed=82432;
static int rng(int n,void *ctx){(void)ctx;seed=seed*1664525u+1013904223u;return (int)((seed>>1)%(unsigned)n);}
static int match(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned slot,void *ctx){(void)p;(void)slot;(void)ctx;return id==r->exact_card;}
int main(void){
    const RdOpponentProfile *rex=Rd_GetProfile(7), *martis=Rd_GetProfile(8);
    RdRitualRecipe recipe;
    RdRitualRequirement roles[3]={{0}};
    u16 deck[40];RdPackageStats packs;
    int mode,t,has_ready[3]={0},trials=30000,seen=0;
    assert(rex&&martis);
    assert(rex->pool_count==26);
    for(int i=0;i<rex->pool_count;i++)if(rex->pool[i].id==705){assert(rex->pool[i].weight==7);seen++;}
    assert(seen==1);
    for(int i=0;i<martis->pool_count;i++)if(martis->pool[i].id==705){assert(martis->pool[i].weight==30);}
    roles[0].exact_card=6;roles[1].exact_card=47;roles[2].exact_card=571;
    recipe=rex->recipe;recipe.material_count=3;recipe.materials=roles;
    for(mode=0;mode<3;mode++){
        for(t=0;t<trials;t++){
            int nready=0;
            assert(Rd_GeneratePackageDeck(rex,&recipe,match,0,722,mode==0?2:6,mode==2,deck,rng,0,&packs)==40);
            for(int j=0;j<40;j++)if(deck[j]==705)nready++;
            if(nready)has_ready[mode]++;
            assert(nready<=3);
            if(mode!=1)assert(packs.packages>=1);
            for(int j=0;j<packs.packages;j++){
                assert(packs.included[j][0]==680);
                assert(packs.included[j][1]==6);
                assert(packs.included[j][2]==47);
                assert(packs.included[j][3]==571);
                for(int k=0;k<4;k++)assert(packs.included[j][k]!=705);
            }
        }
    }
    assert(has_ready[0]*100 < trials*15 && has_ready[1]*100 < trials*15 && has_ready[2]*100 < trials*15);
    printf("REX Tri-Horned ready-made presence: Story %d/%d (%.2f%%), Free OFF %d/%d (%.2f%%), Free ON %d/%d (%.2f%%). Package never contains #705.\n",has_ready[0],trials,100.*has_ready[0]/trials,has_ready[1],trials,100.*has_ready[1]/trials,has_ready[2],trials,100.*has_ready[2]/trials);
    return 0;
}
