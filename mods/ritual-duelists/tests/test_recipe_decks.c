#include <assert.h>
#include <stdio.h>
#include "ritual_duelists.h"
static int rng(int n,void *ctx){(void)ctx;return n>1?1:0;}
static int match(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned j,void *ctx){(void)p;(void)j;(void)ctx;return id>0&&id<=722&&(!r->exact_card||id==r->exact_card);}
static RdRitualRequirement exact(int id){RdRitualRequirement r={0};r.exact_card=id;r.type=-1;r.min_attack=r.max_attack=r.min_defense=r.max_defense=r.min_level=r.max_level=-1;return r;}
int main(void){
 const RdOpponentProfile *p=Rd_GetProfile(23);RdRitualRecipe r=p->recipe;
 RdRitualRequirement roles[]={exact(535),exact(545),exact(378)};
 RdPackageStats stats;u16 deck[40];int n,i,counts[723]={0};
 assert(p->pool_count==76);r.materials=roles;r.material_count=3;
 n=Rd_GeneratePackageDeck(p,&r,match,0,722,2,0,deck,rng,0,&stats);
 assert(n==40&&stats.packages>=1);
 for(i=0;i<40;i++){assert(deck[i]>0&&deck[i]<=722);assert(++counts[deck[i]]<=3);}
 assert(counts[679]>=stats.packages&&counts[535]>=stats.packages&&counts[545]>=stats.packages&&counts[378]>=stats.packages);
 /* Explicit Vanilla recipe may use materials outside any normal pool. */
 roles[1]=exact(555);roles[2]=exact(556);
 assert(Rd_GeneratePackageDeck(p,&r,match,0,722,2,0,deck,rng,0,&stats)==40);
 assert(stats.included[0][2]==555&&stats.included[0][3]==556);
 r.material_count=2;
 assert(!Rd_GeneratePackageDeck(p,&r,match,0,722,2,0,deck,rng,0,&stats));
 r.material_count=12;
 assert(!Rd_GeneratePackageDeck(p,&r,match,0,722,2,0,deck,rng,0,&stats));
 puts("PASS: 3-role atomic package, Vanilla/out-of-pool materials, caps, incompatible recipe fallback");return 0;
}
