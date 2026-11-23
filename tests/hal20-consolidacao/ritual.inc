static void put_field(int slot,int id,int uid){DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");d[uid].id=id;DuelCardRecord*r=setup_record(slot,uid);r->object=spawn_hand(r,0,0);((uint8_t*)datum("D_800907D8"))[slot]=slot;}
static void put_hand(int slot,int id,int uid){DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelSideState*s=datum("D_800E9FF0");d[uid].id=id;s[0].hand[slot]=uid;DuelCardRecord*r=setup_record(slot,uid);((DuelHandSlot*)datum("D_800EA030"))[slot].object=spawn_hand(r,0,0);}
static void ritual_cases(void){
 if(!Mods_Active(mi(M))){puts("NOT_RUN ritual materials Melhoria absent");return;}int re=Mods_Active(mi(RE));
 for(int producer=0;producer<=re;producer++){
  if(re)Hal25_ForceApplied(mi(RE),producer);
  for(int layout=0;layout<3;layout++){
   setup(5);begin("ritual-materials-public-hooks");*(uint8_t*)datum("D_8009B1D5")=0;DuelCardRecord*r=datum("D_801A7AD8");memset(r,0,30*sizeof*r);DuelSideState*s=datum("D_800E9FF0");memset(s[0].hand,-1,5);set_stat(357,1600,1800,0);uint16_t row[]={666,29,31,101,357,0};memcpy(datum("gDuel_awRitualData"),row,sizeof row);
   if(layout==0){put_field(5,29,0);put_field(6,31,1);put_field(7,101,2);put_field(8,31,3);}else if(layout==1){put_hand(0,29,0);put_hand(1,31,1);put_hand(2,101,2);put_hand(3,31,3);}else{put_field(5,101,0);put_hand(0,29,1);put_hand(1,31,2);put_hand(2,31,3);put_field(6,1,4);set_stat(1,3000,2500,0);}
   int(*check)(DuelRitualResult*,int)=engine_function("Duel_CheckRitual");void(*consume)(DuelCardRecord*)=engine_function("DuelCard_DeactivateRecord");DuelRitualResult out={0};int before[5];memcpy(before,s[0].hand,sizeof s[0].hand);int probe=check(NULL,666),found=check(&out,666);
   printf("RITUAL layout=%s provider=%d probe=%d result=%d min_field_default=1\n",layout==0?"Field":layout==1?"Hand":"Both",producer,probe,found);
   result("ritual-probe-preserves-hand",!memcmp(before,s[0].hand,sizeof s[0].hand),"Public search does not consume physical cards.");
   if(layout==1){result("Hand-only-rejected-default-min-field",!found&&!probe,"Approved min_field default1 preserved; no setting relaxed for PASS.");continue;}
   if(found==357)for(int k=0;k<3;k++)consume(&r[5+k*(layout==0)]);
   int good=found==357&&probe==357&&(layout==0?(!r[5].flags&&!r[6].flags&&!r[7].flags&&r[8].flags):(!r[5].flags&&s[0].hand[0]==-1&&s[0].hand[1]==-1&&s[0].hand[2]==3&&r[2].flags&&r[6].flags));
   result(layout==0?"Field-three-tributes-physical":"Both-tributes-spare-identical-copy",good,"Public Melhoria selector/consumer through real Ritual/Fusion hook stack; bounded native oracle, no animation/gameplay proof.");
  }
 }if(re)Hal25_ForceApplied(mi(RE),1);
}
