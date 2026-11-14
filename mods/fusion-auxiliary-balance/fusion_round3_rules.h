#ifndef FUSION_ROUND3_RULES_H
#define FUSION_ROUND3_RULES_H
/* Round 03: implementation-safe subset. Pure testable conditions. */
static int r3_member(int id){switch(id){
 case 594:case 533:case 43:case 272:case 30:case 521:case 618:case 437:case 473:case 482:case 389:case 639:
 case 15:case 545:case 617:case 230:case 86:case 294:case 133:case 215:case 458:case 487:
 return 1;default:return 0;}}
static int r3_attack_target_bonus(int source,int attacking,int target_type,int target_attribute,
                                  int target_was_face_down,int allied_dinosaurs){
 if(!attacking)return 0;
 switch(source){
 case 437:return target_was_face_down?700:0;
 case 482:return 300*allied_dinosaurs;
 case 389:return target_attribute==1?600:0; /* DARK resolved by caller */
 case 15:return target_type==CARD_TYPE_PLANT||target_type==CARD_TYPE_ZOMBIE?600:0;
 case 545:return target_type==CARD_TYPE_DRAGON?800:0;
 case 230:return target_type==CARD_TYPE_BEAST||target_type==CARD_TYPE_BEAST_WARRIOR?700:0;
 default:return 0;
 }}
static int r3_exact_recipe(int a,int b){
 static const short recipes[][3]={
 {6,620,594},{581,146,521},{299,234,618},{439,543,437},
 {673,264,473},{570,644,482},{396,537,458},{236,280,389},
 {275,445,639},{142,293,15},{574,612,495},{36,10,545},
 {430,550,617},{61,251,230},{201,5,86},{2,104,115},{172,296,294}};
 unsigned i;for(i=0;i<sizeof(recipes)/sizeof(recipes[0]);i++)
 if((recipes[i][0]==a&&recipes[i][1]==b)||(recipes[i][0]==b&&recipes[i][1]==a))return recipes[i][2];
 return 0;
}
#endif
