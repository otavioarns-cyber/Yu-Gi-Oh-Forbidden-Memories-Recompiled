#include <assert.h>
#include <stdio.h>
#define CARD_TYPE_ZOMBIE 2
#define CARD_TYPE_BEAST_WARRIOR 4
#define CARD_TYPE_BEAST 5
#define CARD_TYPE_PLANT 19
#define CARD_TYPE_DRAGON 0
#include "../fusion_round3_rules.h"
int main(void){
 const int expected[][3]={{6,620,594},{581,146,521},{299,234,618},{439,543,437},
 {673,264,473},{570,644,482},{396,537,458},{236,280,389},{275,445,639},
 {142,293,15},{574,612,495},{36,10,545},{430,550,617},{61,251,230},
 {201,5,86},{2,104,115},{172,296,294}};
 int i;for(i=0;i<17;i++){
  assert(r3_exact_recipe(expected[i][0],expected[i][1])==expected[i][2]);
  assert(r3_exact_recipe(expected[i][1],expected[i][0])==expected[i][2]);
 }
 assert(r3_exact_recipe(189,251)==0);
 assert(r3_exact_recipe(36,97)==0);
 assert(r3_exact_recipe(10,97)==0);
 assert(r3_member(487)==1); assert(r3_member(458)==1);
 printf("PASS: 17 symmetric recipes; Skelgon/Rare Fish narrow override, no Fusionist+Mermaid\n");
 return 0;
}
