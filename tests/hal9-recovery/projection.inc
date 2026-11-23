/* Callback interposition ONLY in the own adapter's predecessor slots. Not
 * an admitted hook wrapper. Each test stops before native physical collection. */
static void *rec_previous_swap,*rec_previous_snapshot;static unsigned rec_probe_calls;
static int rec_selected;static AiActiveCard rec_before_ai[90];static uint32_t rec_before_stats[722];
static void rec_swap_probe(void){
 ++rec_probe_calls;AiActiveCard*a=datum("gDuel_aActiveCards");rec_selected=((uint8_t*)datum("D_800EAE88"))[0];
 if(rec_mode!=220)((void(*)(void))rec_previous_swap)();
 if(rec_mode==221)a[rec_selected].card_type=CARD_TYPE_EQUIP;
 if(rec_mode==226){a[rec_selected].deck_index^=1;a[rec_selected].card_type=CARD_TYPE_EQUIP;}
 if(rec_mode==231)Hal25_ForceApplied(mi(T),0);
 if(rec_mode==230)((uint8_t*)datum("D_800EAE88"))[6]=1;
 if(rec_mode==227){a[12].attack=1234;((uint32_t*)datum("gDuel_adwCardStats"))[17]^=1;}
 rec_projection_done=1;printf("PROJECTION_CALLBACK swap mode=%d source=%d type=%d delegated=%d\n",rec_mode,rec_selected,a[rec_selected].card_type,rec_mode!=220);
}
static void rec_snapshot_probe(void){
 ++rec_probe_calls;AiActiveCard*a=datum("gDuel_aActiveCards");uint32_t*s=datum("gDuel_adwCardStats");
 if(rec_mode==224)return;
 if(rec_mode==225&&rec_probe_calls==1)((void(*)(void))tw_symbol("tw_client_snapshot"))();
 ((void(*)(void))rec_previous_snapshot)();
 if(rec_mode==222){s[15]^=1;s[17]^=1;}
 if(rec_mode==223)for(int i=11;i<56;i++)if(a[i].card_id==16)a[i].card_type=CARD_TYPE_EQUIP;
 printf("PROJECTION_CALLBACK snapshot mode=%d calls=%u stat=%u\n",rec_mode,rec_probe_calls,s[15]);
}
static void projection_cases(int mode){
 rec_mode=mode;rec_projection_done=rec_probe_calls=0;recovery_board(1);begin("own-projection-adversarial-callback");
 AiActiveCard*a=datum("gDuel_aActiveCards");uint32_t*s=datum("gDuel_adwCardStats");
 if(mode>=222&&mode<=225){
  if(!Mods_Active(mi(D))){puts("HAL9_NOT_REACHED snapshot bonus projection requires active Dual");return;}
  int p=hand_action();if(p!=6&&p!=7){puts("HAL9_NOT_REACHED first natural Monster unavailable");return;}
  result("projection-natural-first-Monster",hand_count()==4&&ds()[1]==1,"Real plan/controller and real quota ledger, no quota injection.");resolve_model(p);CHECK(award_bonus());
  if(mode==224){a[11].card_id=16;a[11].card_type=CARD_TYPE_MAGIC;puts("INSTRUMENTED pre-existing logical Magic type, no physical card write");}
  memcpy(rec_before_ai,a,sizeof rec_before_ai);memcpy(rec_before_stats,s,sizeof rec_before_stats);
  rec_previous_snapshot=*(void**)tw_symbol("tw_client_original_snapshot");*(void**)tw_symbol("tw_client_original_snapshot")=rec_snapshot_probe;
  int hand=hand_count();((void(*)(void))tw_symbol("tw_client_snapshot"))();
  *(void**)tw_symbol("tw_client_original_snapshot")=rec_previous_snapshot;
  uint32_t expected=rec_before_stats[15]^(mode==222?1:0);
  result("projection-global-owned-type-only",s[15]==expected,"Type projection must not overwrite another callback's attack/defense/stat bits.");
  result("projection-other-card-stat-intact",s[17]==(rec_before_stats[17]^(mode==222?1:0)),"No permanent changes to another card except the explicit callback writer.");
  if(mode==223){int n=0;for(int i=11;i<56;i++)if(a[i].card_id==16){++n;result("projection-preserves-logical-callback-type",a[i].card_type==CARD_TYPE_EQUIP,"Adapter cannot replace another callback's type with its own saved default.");}CHECK(n>0);}
  if(mode==224)result("early-return-does-not-retag-preexisting-record",!memcmp(rec_before_ai,a,sizeof rec_before_ai),"No snapshot was produced. Pre-existing logical records are not owned by the projection.");
  if(mode==225)result("projection-reentrancy-refused",rec_probe_calls==1,"Nested projection cannot borrow the outer temporary type as its original state.");
  result("projection-no-physical-consumption",hand_count()==hand&&conserved()&&ds()[1]==1,"Probe stops at the snapshot boundary; no controller collection or rollback.");
 }else{
  rec_previous_swap=*(void**)tw_symbol("compat_original_ai_swap");*(void**)tw_symbol("compat_original_ai_swap")=rec_swap_probe;
  int p=hand_action();*(void**)tw_symbol("compat_original_ai_swap")=rec_previous_swap;
  result("projection-swap-returned-before-collection",p==4&&rec_probe_calls==1&&hand_count()==5&&conserved(),"Predecessor is instrumented. No unqualified wrapper is admitted or treated as a completed action.");
  if(mode==231)result("projection-restores-across-effective-OFF",!Mods_Active(mi(T))&&a[rec_selected].card_type==CARD_TYPE_SPELLCASTER,"Code stays resident while the adapter closes cooperatively; borrowed logical type is restored even after unhooking. Continuation after this forced OFF is outside this probe.");
  if(mode==230)result("projection-selection-change-still-restores-owned-type",a[rec_selected].card_type==CARD_TYPE_SPELLCASTER,"Selection changed, but physical owner did not: release borrowed type then refuse the altered action.");
  if(mode==220)result("projection-early-return-restores-type",a[rec_selected].card_type==CARD_TYPE_SPELLCASTER,"Normal C return must release the borrowed logical type even when delegation is absent.");
  if(mode==221||mode==226)result("projection-does-not-clobber-new-logical-type",a[rec_selected].card_type==CARD_TYPE_EQUIP,"A callback-owned type, including same-ID logical replacement, must survive; no restore by ID.");
  if(mode==227)result("projection-unselected-card-intact",a[12].attack==1234,"Projection restores only its own selected type, not other logical records.");
 }
 printf("PROJECTION_END mode=%d calls=%u hand=%d used=%u block=%u\n",mode,rec_probe_calls,hand_count(),Mods_Active(mi(D))?ds()[1]:0,rec_block());
 if(rec_block()&&Mods_Active(mi(T)))puts("SAFE_REFUSAL adapter latched a real projection conflict before collection");
 else puts("HAL9_NOT_REACHED physical collection deliberately outside isolated projection probe; restoration PASS is not a gameplay result");
}
