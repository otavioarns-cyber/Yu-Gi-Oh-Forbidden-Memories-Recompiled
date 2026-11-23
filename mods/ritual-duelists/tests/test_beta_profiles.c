#include <assert.h>
#include <stdio.h>
#include "ritual_duelists.h"
static int weight(const RdOpponentProfile *p,int id){int i;for(i=0;i<p->pool_count;i++)if(p->pool[i].id==id)return p->pool[i].weight;return 0;}
int main(void){
 const int options[25]={101,23,51,73,118,63,74,26,121,57,129,101,151,69,78,90,83,139,60,148,70,165,97,76,87};int i,j,sum;
 assert(Rd_ProfileCount()==29);
 for(i=0;i<25;i++){
  const RdOpponentProfile *p=Rd_GetProfile(i);assert(p&&p->pool_count==options[i]);
  sum=0;
  for(j=0;j<p->pool_count;j++)sum+=p->pool[j].weight;
  assert(sum==(i==0?2048:2018) && sum+(i==0?0:30)==2048);
 }
 assert(weight(Rd_GetProfile(0),704)==0 && Rd_GetProfile(0)->recipe.ritual_magic==0);
 assert(weight(Rd_GetProfile(1),702)==20&&weight(Rd_GetProfile(1),167)==98&&weight(Rd_GetProfile(1),469)==98);
 assert(weight(Rd_GetProfile(2),702)==70&&weight(Rd_GetProfile(2),334)==0);
 assert(weight(Rd_GetProfile(3),717)==50&&weight(Rd_GetProfile(3),57)==10&&weight(Rd_GetProfile(3),67)==5&&weight(Rd_GetProfile(3),367)==48&&weight(Rd_GetProfile(3),52)==31&&weight(Rd_GetProfile(3),533)==21);
 assert(weight(Rd_GetProfile(4),720)==20 && weight(Rd_GetProfile(4),50)==0);
 assert(weight(Rd_GetProfile(5),714)==0&&weight(Rd_GetProfile(5),711)==0&&weight(Rd_GetProfile(5),722)==30&&weight(Rd_GetProfile(5),35)==36);
 assert(weight(Rd_GetProfile(6),698)==0&&weight(Rd_GetProfile(6),722)==30&&weight(Rd_GetProfile(6),35)==20&&weight(Rd_GetProfile(6),635)==64&&weight(Rd_GetProfile(6),638)==64);
 assert(Rd_GetProfile(5)->recipe.ritual_magic==721&&Rd_GetProfile(6)->recipe.result_card==722);
 /* Approved v0.3.2 exact weights and protected features. */
 assert(weight(Rd_GetProfile(3),16)==25&&weight(Rd_GetProfile(3),50)==57&&weight(Rd_GetProfile(3),54)==49&&weight(Rd_GetProfile(3),397)==49&&weight(Rd_GetProfile(3),476)==39&&weight(Rd_GetProfile(3),477)==39&&weight(Rd_GetProfile(3),478)==40&&weight(Rd_GetProfile(3),479)==40&&weight(Rd_GetProfile(3),485)==40);
 assert(weight(Rd_GetProfile(7),705)==7&&weight(Rd_GetProfile(7),82)==20&&weight(Rd_GetProfile(7),509)==20&&weight(Rd_GetProfile(7),326)==249&&weight(Rd_GetProfile(7),331)==163&&weight(Rd_GetProfile(7),680)==0);
 assert(weight(Rd_GetProfile(8),705)==30&&weight(Rd_GetProfile(8),719)==50&&weight(Rd_GetProfile(8),680)==0);
 assert(weight(Rd_GetProfile(9),719)==30&&weight(Rd_GetProfile(9),98)==35&&weight(Rd_GetProfile(9),699)==0&&weight(Rd_GetProfile(9),479)==0&&weight(Rd_GetProfile(9),485)==0);
 assert(Rd_GetProfile(7)->recipe.ritual_magic==680 && Rd_GetProfile(8)->recipe.ritual_magic==680 && Rd_GetProfile(9)->recipe.ritual_magic==699);
 assert(Rd_GetProfile(29)==0);
 puts("PASS: sixteen approved real pools (2018+30=2048), exact Heishin cuts, protected ritual monsters and Forest identity");return 0;
}
