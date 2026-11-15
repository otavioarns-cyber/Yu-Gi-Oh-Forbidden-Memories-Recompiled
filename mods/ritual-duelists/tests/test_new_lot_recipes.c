#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned state=0x9a1b3447u;
static int rng(int n,void *ctx){(void)ctx;state=state*1664525u+1013904223u;return (int)((state>>1)%(unsigned)n);}
static RdRitualRequirement exact(int id){RdRitualRequirement r={0};r.exact_card=id;r.type=-1;r.min_attack=r.max_attack=r.min_defense=r.max_defense=r.min_level=r.max_level=-1;return r;}
static int allowed(const RdOpponentProfile *p,int id,const RdRitualRequirement *req,unsigned role,void *ctx){
 (void)p;(void)ctx;
 if(id<=0||id>722)return 0;
 if(req->exact_card>0)return id==req->exact_card;
 if(req->type==100) return (id==436||id==542||id==565||id==440);/* Fish */
 if(req->type==101) return (id==449||id==518||id==89);/* Turtle, dynamic */
 if(req->type==102) return (id==223||id==373||id==520||id==89);/* Aqua, dynamic */
 if(req->type==103) return (id==90||id==109||id==531||id==708);/* Female mocked */
 return role<3;
}
static int occurrences(const unsigned short *deck,int id){int i,n=0;for(i=0;i<40;i++)n+=deck[i]==id;return n;}
static void check(int idx,int modified){
 const RdOpponentProfile *p=Rd_GetProfile(idx);
 RdRitualRecipe r=p->recipe;RdRitualRequirement q[3];
 int mode,t,pass=0,zero=0;
 unsigned short deck[40];RdPackageStats stats;
 assert(p && p->pool_count>0);
 if(idx==13){q[0]=exact(90);q[1]=exact(109);q[2]=exact(531);}
 else if(idx==14){q[0]=exact(436);q[1]=exact(441);q[2]=exact(542);}
 else {q[0]=exact(89);q[1]=exact(449);q[2]=exact(451);}
 if(modified){
   if(idx==13){q[0]=exact(0);q[0].type=103;q[1]=exact(90);q[2]=exact(531);} /* provider-style Female role; result cannot be a tribute */
   if(idx==14){q[0]=exact(441);q[1]=exact(0);q[1].type=100;q[2]=exact(0);q[2].type=100;}
   if(idx==15){q[0]=exact(451);q[1]=exact(0);q[1].type=101;q[2]=exact(0);q[2].type=102;}
 }
 r.material_count=3;r.materials=q;
 for(mode=0;mode<3;mode++)for(t=0;t<1300;t++){
   int copies[723]={0},k,j;int game_mode=mode==0?2:6,force=mode==2;
   assert(Rd_GeneratePackageDeck(p,&r,allowed,0,722,game_mode,force,deck,rng,0,&stats)==40);
   assert(stats.packages<=3 && (mode!=1 || stats.packages>=0));
   if(mode!=1)assert(stats.packages>=1);
   if(mode==1&&stats.packages==0)zero++;
   for(k=0;k<40;k++) {assert(deck[k]>0 && deck[k]<=722);assert(++copies[deck[k]]<=3);}
   assert(occurrences(deck,r.ritual_magic)==stats.packages);
   for(k=0;k<stats.packages;k++){
      assert(stats.included[k][0]==r.ritual_magic);
      for(j=0;j<3;j++){
         int id=stats.included[k][j+1];
         assert(allowed(p,id,&q[j],j,0));assert(id!=r.result_card);
      }
   }
   pass++;
 }
 assert(zero>0);
 printf("PASS %s %s: %d decks, cap 3, 40 cards, atomic bundles, default Free OFF\n",p->duelist_name,modified?"modified":"vanilla",pass);
}
int main(void){int i,j;for(i=13;i<16;i++)for(j=0;j<2;j++)check(i,j);
 /* Recipe invalid under active provider: no package may be silently partially emitted. */
 {const RdOpponentProfile *p=Rd_GetProfile(14);RdRitualRecipe r=p->recipe;RdRitualRequirement q[3];
 unsigned short cards[40];RdPackageStats stats;q[0]=exact(441);q[1]=exact(718);q[2]=exact(436);
 r.material_count=3;r.materials=q;
 assert(!Rd_GeneratePackageDeck(p,&r,allowed,0,722,2,0,cards,rng,0,&stats)&&stats.error==1);
 }
 puts("PASS: dynamic three profiles and invalid recipe");return 0;
}
