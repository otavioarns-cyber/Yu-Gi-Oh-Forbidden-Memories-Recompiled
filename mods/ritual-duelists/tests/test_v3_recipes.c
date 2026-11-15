/* Rodada V3: active recipe selection, no recipe code in the deck mod. */
#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
static unsigned rng_state=0x2a3be7d1u;
static int rng(int n,void *ctx){(void)ctx;rng_state=rng_state*1664525u+1013904223u;return (int)((rng_state>>1)%(unsigned)n);}
static RdRitualRequirement exact(int id){
 RdRitualRequirement r={0};r.exact_card=id;r.type=-1;
 r.min_attack=r.max_attack=r.min_defense=r.max_defense=r.min_level=r.max_level=-1;
 return r;
}
static int allowed(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned pos,void *ctx){
 (void)p;(void)pos;(void)ctx;
 if(id<1||id>722)return 0;
 if(r->exact_card>0)return id==r->exact_card;
 if(r->type==100)return id==487||id==403||id==483||id==14; /* Beast */
 if(r->type==101)return id==52||id==533||id==535||id==49; /* Insect */
 if(r->type==102)return id==39||id==7||id==31; /* Dragon */
 return 0;
}
static int count(const u16 cards[40],int id){int c=0,i;for(i=0;i<40;i++)c+=cards[i]==id;return c;}
static void run(int index,int variant){
 const RdOpponentProfile *p=Rd_GetProfile(index);
 RdRitualRecipe recipe=p->recipe;
 RdRitualRequirement req[3];
 RdPackageStats st;
 u16 deck[40];
 int t,mode,zero=0;
 assert(p);
 if(index==16){req[0]=exact(403);req[1]=exact(483);req[2]=exact(487);
  if(variant){req[2]=exact(0);req[2].type=100;}}
 else if(index==17){req[0]=exact(52);req[1]=exact(533);req[2]=exact(49);
  if(variant){req[2]=exact(0);req[2].type=101;}}
 else {req[0]=exact(7);req[1]=exact(39);req[2]=exact(31);
  if(variant){req[2]=exact(0);req[2].type=102;}}
 recipe.materials=req;recipe.material_count=3;
 for(mode=0;mode<3;mode++)for(t=0;t<1000;t++){
  int copies[723]={0},i,j;
  assert(Rd_GeneratePackageDeck(p,&recipe,allowed,0,722,mode==0?2:6,mode==2,
                                deck,rng,0,&st)==40);
  if(mode!=1)assert(st.packages>=1);
  if(mode==1&&st.packages==0)zero++;
  assert(st.packages>=0&&st.packages<=3);
  assert(count(deck,recipe.ritual_magic)==st.packages);
  for(i=0;i<40;i++)assert(deck[i]>=1&&deck[i]<=722&&++copies[deck[i]]<=3);
  for(i=0;i<st.packages;i++){
   assert(st.included[i][0]==recipe.ritual_magic);
   for(j=0;j<3;j++)assert(allowed(p,st.included[i][j+1],&req[j],j,0));
   for(j=0;j<4;j++)assert(st.included[i][j]!=recipe.result_card);
   if(index==17)for(j=1;j<4;j++)assert(st.included[i][j]!=16);
  }
 }
 assert(zero>0);
 printf("PASS V3 %s (%s): 3000 decks, modes Story/Free OFF/Free ON, correct materials\n",
        p->duelist_name,variant?"dynamic":"exact");
}
int main(void){
 int i,j;
 for(i=16;i<19;i++)for(j=0;j<2;j++)run(i,j);
 {
 const RdOpponentProfile *p=Rd_GetProfile(16);
 RdRitualRecipe r=p->recipe;RdRitualRequirement req[3];RdPackageStats st;u16 cards[40];
 req[0]=exact(403);req[1]=exact(483);req[2]=exact(356);
 r.materials=req;r.material_count=3;
 assert(!Rd_GeneratePackageDeck(p,&r,allowed,0,722,2,0,cards,rng,0,&st));
 assert(st.error==1); /* The ready result cannot be its own package tribute. */
 }
 puts("PASS V3 dynamic: no Time Wizard tribute, invalid recipe atomic refusal");return 0;
}
