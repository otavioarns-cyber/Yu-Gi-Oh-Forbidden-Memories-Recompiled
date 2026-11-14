/* 17/10/2026 V3 exact pools and real-package construction simulations.
 * Simulated recipe providers here, not a claim of running the Windows game. */
#include "ritual_duelists.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned state=0x71431u;
static int rng(int n,void *ignored){(void)ignored;state=state*1664525u+1013904223u;return (int)((state>>1)%(unsigned)n);}
static RdRitualRequirement exact(int id){RdRitualRequirement x={0};x.exact_card=id;x.type=RD_ANY;x.min_attack=x.max_attack=x.min_defense=x.max_defense=x.min_level=x.max_level=RD_ANY;return x;}
static RdRitualRequirement custom(int group){RdRitualRequirement x=exact(0);x.custom_group=group;return x;}
static int valid(const RdOpponentProfile *p,int id,const RdRitualRequirement *r,unsigned role,void *ctx){
 (void)role;(void)ctx;
 if(id<1||id>722)return 0;
 if(r->exact_card)return id==r->exact_card;
 if(r->custom_group==1)return Rd_IsSkull(p,id);
 if(r->custom_group==2)return id==38||id==7; /* mock Warrior eligible by current recipe */
 return 1;
}
static int w(const RdOpponentProfile *p,int card){int i;for(i=0;i<p->pool_count;i++)if(p->pool[i].id==card)return p->pool[i].weight;return 0;}
static void run(int index,int ritual,int monster,const int base[3],int dynamic){
 const RdOpponentProfile *p=Rd_GetProfile(index);RdRitualRecipe rec=p->recipe;
 RdRitualRequirement m[3];RdPackageStats st;u16 cards[40];int iter,mode;
 assert(p && rec.ritual_magic==ritual && rec.result_card==monster);
 assert(w(p,ritual)==0 && p->baseline_material_count==3);
 for(int i=0;i<3;i++)assert(p->baseline_materials[i]==base[i]);
 for(int k=0;k<3;k++)m[k]=exact(base[k]);
 if(dynamic){m[1]=custom(1);m[2]=custom(2);} /* Sebek: exact #535 + Skull + Warrior */
 rec.materials=m;rec.material_count=3;
 for(mode=0;mode<3;mode++){
  int zero_seen=0;
  for(iter=0;iter<300;iter++){
    int count[723]={0},number;
    int free=(mode!=0),guarantee=(mode==2);
    assert(Rd_GeneratePackageDeck(p,&rec,valid,0,722,free?6:2,guarantee,cards,rng,0,&st)==40);
    assert(st.packages<=3);
    if(!free||guarantee)assert(st.packages>=1);
    for(int i=0;i<40;i++)assert(cards[i]>0&&cards[i]<=722&&++count[cards[i]]<=3);
    if(!st.packages)zero_seen++;
    assert(count[ritual]==(int)st.packages);
    for(int k=0;k<(int)st.packages;k++){
     assert(st.included[k][0]==ritual);
     for(int r=0;r<3;r++)assert(valid(p,st.included[k][r+1],&m[r],r,0));
     if(dynamic)assert(st.included[k][1]==535);
    }
    number=0;for(int i=0;i<40;i++)number+=cards[i]!=0;assert(number==40);
  }
  if(mode==1)assert(zero_seen>0); /* Free OFF does not guarantee */
 }
 printf("PASS: %s complete 40-card decks, Story / Free OFF / Free ON, active mock recipe\n",p->duelist_name);
}
int main(void){
 static const int labyrinth[3]={371,372,373},sebek[3]={535,22,38},neku[3]={85,298,377};
 const RdOpponentProfile *isis=Rd_GetProfile(0),*s=Rd_GetProfile(23);
 int i,total=0;
 assert(Rd_ProfileCount()==29 && RD_PROFILE_CAPACITY>=29 && Rd_GetProfile(29)==0);
 assert(isis && !strcmp(isis->duelist_name,"Isis") && isis->pool_count==101);
 assert(isis->recipe.ritual_magic==0 && isis->recipe.result_card==0 && !isis->skull_count);
 for(i=0;i<isis->pool_count;i++)total+=isis->pool[i].weight;
 assert(total==2048 && !w(isis,704) && !w(isis,679));
 assert(s && s->pool_count==76 && s->skull_count==21 && w(s,82)==30 && w(s,22)==116 && w(s,217)==72);
 assert(w(s,335)==0 && w(s,535)==0);
 for(i=1;i<=722;i++)if(Rd_IsSkull(s,i))assert(w(s,i)>0);
 assert(w(Rd_GetProfile(21),364)==30); /* previous round unchanged */
 run(22,667,374,labyrinth,0);
 run(23,679,704,sebek,1);
 run(24,671,360,neku,0);
 return 0;
}
