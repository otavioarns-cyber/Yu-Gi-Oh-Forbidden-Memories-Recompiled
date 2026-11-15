/* Own instrumentation only. Reads the pinned core's retained diagnostics;
 * never writes a selected plan, pending, quota or wheel outcome. */
static void support_trace(int index,const char*stage){
 if(Settings_GetNamed("test.tw-case",0)<20||index<0)return;
 if(registrations[index].owner==D&&!strcmp(registrations[index].name,"func_80028220")){printf("SUPPORT_QUOTA_SNAPSHOT stage=%s used=%u bonus=%u mode=%u substate=%u selection=%u destination=%u\n",stage,ds()[1],ds()[5],*(uint8_t*)datum("D_8009B174"),*(uint16_t*)datum("D_8009B20C"),((uint8_t*)datum("D_800EAE88"))[0],((uint8_t*)datum("D_800EAE88"))[6]);}
 if(registrations[index].owner!=T||strcmp(registrations[index].name,"AiScript_Run"))return;
 int*d=local(T,"diag");if(!d)return;AiActiveCard*a=datum("gDuel_aActiveCards");
 printf("SUPPORT_PLANNER stage=%s side=%u cpu=%d script=%d handTW=%d enemy=%d,%d,%d diag_enemy=%d worth=%d reason=%d evaluations=%u available=%d locked=%d EV=%d normal=%d bank=%d choice=%d selected=%u destination=%u\n",stage,*(uint8_t*)datum("D_8009B1D5"),((int8_t*)datum("D_8009B360"))[1],*(void**)((char*)datum("gAiScript_State")+4)==datum("D_801A8000"),a[11].card_id,a[56].card_id,a[57].card_id,a[58].card_id,d[4],d[9],d[10],d[11],d[41],d[42],d[45],d[36],d[20],d[49],((uint8_t*)datum("D_800EAE88"))[0],((uint8_t*)datum("D_800EAE88"))[6]);
}
void HalSupportAdvance(void);
static H9WheelRequest support_request;static int support_spin_id,support_busy,support_spins;
static int support_spin(const void*request){support_request=*(const H9WheelRequest*)request;support_busy=1;++support_spins;printf("SUPPORT_WHEEL id=%d mode=%d forced_by_core=%d\n",support_spin_id+1,support_request.mode,support_request.forced);return ++support_spin_id;}
static int support_wheel_busy(void){return support_busy;}
static int support_wheel_result(int id){(void)id;return support_request.forced;}
static void support_wheel_cancel(void){support_busy=0;}
static void support_resolution(void){
 H9Wheel**real=tw_symbol("real_wheel"),*saved=*real,mock={1,support_spin,support_wheel_busy,support_wheel_result,support_wheel_cancel};*real=&mock;
 int side=*(uint8_t*)datum("D_8009B1D5"),mode=Settings_GetNamed("test.tw-case",0),interrupted=0;int*state=tw_symbol("state"),last=-1,lastscene=-1;unsigned setup_before=setup_count;int hand_before=trace_hand_count(side);
 for(int tick=0;tick<1200;tick++){
  *(uint16_t*)datum("D_8009B162")=0;HalSupportAdvance();
  ((void(*)(void))engine_function("DuelScene_UpdateCardUse"))();Mods_DrawFrame();
  if(support_busy&&!interrupted&&(mode==33||mode==34||mode==35)){
   interrupted=1;
   if(mode==33){Hal25_ForceApplied(mi(F),0);printf("SUPPORT_LIFECYCLE Fusion effective OFF after wheel start eligible=%d\n",*(int*)tw_symbol("eligible"));}
   if(mode==34){H9WheelRequest stale=support_request;int old=support_spin_id;Mods_Reset();int phase_after_reset=state[1],chosen_after_reset=state[5];stale.done(old,stale.forced,(stale.forced==0||stale.forced==3)?0:1,stale.user);char error[512];printf("SUPPORT_RESET phase=%d spin_id=%d pending=%d hand=%d\n",state[1],*(int*)tw_symbol("spin_id"),*(int*)tw_symbol("pending"),trace_hand_count(side));result("reset-stale-completion-ignored",state[1]==phase_after_reset&&state[5]==chosen_after_reset&&!*(int*)tw_symbol("pending")&&!*(int*)tw_symbol("spin_id")&&trace_hand_count(side)==hand_before,"Actual reset preserves a legacy phase; old wheel callback is ignored and spent hand is not restored.");result("reset-rejects-stale-wheel-completion",state[1]==0&&!*(int*)tw_symbol("pending")&&trace_hand_count(side)==hand_before&&setup_count==setup_before&&!Mods_Hal20StateLoadAllowed(error,sizeof error),"Actual reset; already consumed card is not returned; stale callback ignored; B still refuses load.");break;}
   if(mode==35){Mods_SetEnabled(mi(T),0);result("TW-pending-restart-still-effective",Mods_Active(mi(T))&&!Mods_Enabled(mi(T)),"Original restart policy keeps applied TW during pending preference.");Mods_SetEnabled(mi(T),1);Hal25_ForceApplied(mi(T),0);char error[512];result("TW-resident-OFF-load-refused",!Mods_Active(mi(T))&&!Mods_Hal20StateLoadAllowed(error,sizeof error),"Explicit instrumented effective OFF; no automatic gameplay reachability claim.");Hal25_ForceApplied(mi(T),1);}
  }
  if(support_busy){int slice=support_request.forced;if(support_request.mode==1||slice<0||slice>=6){puts("HAL9_NOT_REACHED real manual input is not supplied by automatic transport");break;}support_busy=0;support_request.done(support_spin_id,slice,(slice==0||slice==3)?0:1,support_request.user);}
  int scene=*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff;
  if((mode==38||mode==39)&&state[1]==4&&last!=4){int*plan=(int*)((char*)state+0x60);for(int i=0;i<state[8]&&i<10;i++)printf("SUPPORT_PLAN zone=%d original=%d type=%d strength=%d result=%d\n",plan[i*5],plan[i*5+1],plan[i*5+2],plan[i*5+3],plan[i*5+4]);}
  if(state[1]!=last||scene!=lastscene){printf("SUPPORT_RESOLUTION tick=%d phase=%d scene=%d hand=%d won=%d plan_count=%d setup_calls=%u\n",tick,state[1],scene,trace_hand_count(side),state[6],state[8],setup_count-setup_before);last=state[1];lastscene=scene;}
  if(scene!=6){result("Support-resolution-single-consumption",trace_hand_count(side)==hand_before&&setup_count==setup_before,"Native use/core/frame chain completes with wheel transport delivering only the sector already selected by original RNG.");break;}
 }
 DuelCardRecord*cards=datum("D_801A7AD8");printf("SUPPORT_EFFECTS enemies_occupied=%u,%u,%u ids=%d,%d,%d\n",!!(cards[5].flags&DUEL_CARD_FLAG_OCCUPIED),!!(cards[6].flags&DUEL_CARD_FLAG_OCCUPIED),!!(cards[7].flags&DUEL_CARD_FLAG_OCCUPIED),cards[5].card_id,cards[6].card_id,cards[7].card_id);
 if(mode==31||mode==33)result("Encore-natural-retry-destroys-three",support_spins==2&&!((cards[5].flags|cards[6].flags|cards[7].flags)&DUEL_CARD_FLAG_OCCUPIED),"Actual RNG sector1 then sector0; real Encore provider and core destruction, native graphical services modeled.");
 if(mode==39)puts("HAL9_NOT_REACHED transform_card returned before asset read: disc_file_start=-1; no legitimate image provided");
 if(mode==38){DuelDeckCardRecord*deck=datum("gDuel_aDeckCardRecords");printf("SUPPORT_EVOLUTION elf=%d cocoon=%d uidElf=%d uidCocoon=%d\n",cards[21].card_id,cards[22].card_id,cards[21].data==&deck[49],cards[22].data==&deck[50]);result("real-elf-Cocoon-existing-evolutions",cards[21].card_id==433&&cards[22].card_id==67&&cards[21].data==&deck[49]&&cards[22].data==&deck[50],"Actual core snapshot/evolution adapter and transform callbacks, known-name catalogue subset; no new Egg rule.");}
 printf("SUPPORT_RESOLUTION_END phase=%d scene=%d spins=%d hand=%d\n",state[1],*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff,support_spins,trace_hand_count(side));
 *real=saved;
}
static void support_bonus(void){
 if(!Mods_Active(mi(D))){puts("SUPPORT_BONUS absent Dual, not exercised");return;}
 int awarded=award_bonus();printf("SUPPORT_BONUS awarded=%d used=%u hand=%d\n",awarded,ds()[1],hand_count());
 if(!awarded){puts("HAL9_NOT_REACHED real bonus award unavailable");return;}
 int before=hand_count();unsigned setups=setup_count;int p=hand_action();printf("SUPPORT_BONUS_COMMIT phase=%d id=%d used=%u hand=%d extra_exchanges=%u\n",p,action_card(),ds()[1],hand_count(),setup_count-setups);
 if(p==6||p==7)result("Support-bonus-real-category-once",ds()[1]==3&&hand_count()==before-1&&conserved()&&setup_count-setups<=1,"Real Dual award and planner/controller; field UI completion modeled, no quota or pending injected.");
 else puts("HAL9_NOT_REACHED bonus controller did not commit a legal action in available VM");
}
static void support_human(int mode){
 setup(5);*(uint8_t*)datum("D_8009B1D5")=0;DuelSideState*sides=datum("D_800E9FF0");*(DuelSideState**)datum("D_8009B1C8")=&sides[0];memset(sides[0].hand,-1,5);((int8_t*)datum("D_8009B360"))[0]=-1;
 for(int i=0;i<5;i++)put_hand(i,i==0?16:110+i,i);set_stat(16,500,400,CARD_TYPE_SPELLCASTER);((DuelCardDisplayObject*)((DuelCardRecord*)datum("D_801A7AD8"))[0].object)->field_68=CARD_TYPE_SPELLCASTER;
 setup_count=spawn_count=0;int selected=0,flip=0,cancelled=0;
 for(int tick=0;tick<200;tick++){
  unsigned handmode=*(uint8_t*)datum("D_8009B174");*(uint16_t*)datum("D_8009B162")=0;*(uint16_t*)datum("gInput_wPad1Pressed")=0;*(uint16_t*)datum("gInput_wPad1Repeat")=0;
  if((handmode&15)==1&&!selected){*(uint16_t*)datum("gInput_wPad1Pressed")=mode==28?0x40:0x80;selected=1;}
  if((handmode&15)==3&&(handmode&0x80)&&!(handmode&0x40)&&!(handmode&0x20)){
   if(mode==30&&!cancelled){*(uint16_t*)datum("gInput_wPad1Pressed")=0x20;cancelled=1;}
   else if(mode==29&&!flip){*(uint16_t*)datum("gInput_wPad1Repeat")=0x2000;flip=1;}
   else if(mode!=30)*(uint16_t*)datum("gInput_wPad1Pressed")=0x40;
  }
  if((handmode&15)==4&&!(handmode&0x40))*(uint16_t*)datum("gInput_wPad1Pressed")=0x40;
  ((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();
  int phase=*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff,count=trace_hand_count(0);
  if(tick<3||phase!=4)printf("SUPPORT_INPUT tick=%d input_mode=%d phase=%d handmode=%u hand=%d role=%u target=%u\n",tick,mode,phase,*(uint8_t*)datum("D_8009B174"),count,*(uint8_t*)tw_symbol("tw_player_role"),*(uint8_t*)datum("D_8009B19C"));
  if(mode==30&&cancelled&&(*(uint8_t*)datum("D_8009B174")&15)==1){result("Square-cancel-before-consumption",count==5&&(!Mods_Active(mi(D))||ds()[0]==0),"Actual button presses through native human controller and TW wrapper; no selection injection.");return;}
  if(phase==6||phase==7){result(mode==28?"Cross-Monster-input":"Square-Support-input",count==4&&(!Mods_Active(mi(D))||ds()[0]==(mode==28?1:2)),"Native controller category and physical hand; UI rendering not certified.");printf("SUPPORT_INPUT_COMMIT phase=%d target=%u face=%u\n",phase,*(uint8_t*)datum("D_8009B19C"),((DisplayObject**)datum("D_800E9EF0"))[0]?((DisplayObject**)datum("D_800E9EF0"))[0]->field_20.b.field_21:0);if(mode==36&&phase==6)support_resolution();if(mode==29)puts("HAL9_NOT_REACHED face-down M/T placement committed but native placement/render-to-later-activation not executed");return;}
 }
 puts("HAL9_NOT_REACHED human controller waits on unavailable display/input boundary");
}
static void support_cases(int mode){
 if((mode>=28&&mode<=30)||mode==36){support_human(mode);return;}
 setup(5);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelCardRecord*r=datum("D_801A7AD8");d[40].id=16;r[15].card_id=16;
 set_stat(16,500,400,CARD_TYPE_SPELLCASTER);r[15].attack=500;r[15].defense=400;((DuelCardDisplayObject*)r[15].object)->field_68=CARD_TYPE_SPELLCASTER;
 int first=mode==20?0:5;
 if(mode>=23){set_stat(22,2500,1200,CARD_TYPE_FIEND);for(int i=0;i<3;i++)put_field(first+i,22,i);}else{put_field(first,6,0);put_field(first+1,47,1);put_field(first+2,571,2);}
 if(mode==26)queen_setup();
 if(mode==31||mode==32||mode==33||mode==38||mode==39){put_field(20,495,48);if(mode==32)Hal25_ForceApplied(mi(F),0);}
 if(mode==38||mode==39){for(int i=3;i<5;i++)put_field(5+i,22,i);set_stat(41,1400,1200,CARD_TYPE_WARRIOR);set_stat(72,0,2000,CARD_TYPE_INSECT);put_field(21,41,49);put_field(22,72,50);}
 setup_count=spawn_count=0;begin("Support-natural-planner-controlled-board");int p=hand_action(),id=action_card();
 int*diag=tw_symbol("diag");printf("SUPPORT_STOP mode=%d phase=%d id=%d hand=%d exchanges=%u category=%d diag_reason=%d choice=%d EV=%d enemy=%d\n",mode,p,id,hand_count(),setup_count,action_category(),diag[10],diag[49],diag[45],diag[4]);
 printf("SUPPORT_TARGET record=%u cursor_row=%u face=%u source_identity=%u\n",*(uint8_t*)datum("D_8009B19C"),(*(DuelCardPickCursor**)datum("D_8009B1B4"))->row,action_object()?action_object()->field_20.b.field_21:0,action_object()?action_object()->field_6B:0);
 if(p==4&&Mods_Active(mi(RD))){RhsRecovery z=recovery();printf("SUPPORT_BLOCK state=%u cause=%u last=%u attempts=%u limit=%u recovery=%u\n",z.state,z.cause,z.last_cause,z.attempts,z.limit,z.recovery);pending_log("blocked-by-selection-boundary");result("Support-stale-selection-refused",z.state==RHS_STATE_BLOCKED&&z.last_cause==RHS_CAUSE_SNAPSHOT&&hand_count()==5&&!setup_count,"Real RD captured action overwritten by TW; fail closed before consumption, bounded three retries.");puts("SAFE_REFUSAL Support existing RD transaction disagrees with later TW action");return;}
 if(mode==26&&(p==6||p==7)&&id==395){result("real-Monster-before-Support",hand_count()==4&&(!Mods_Active(mi(D))||ds()[1]==1),"Queen producer's actual required Female action; no quota injection.");resolve_model(p);support_bonus();return;}
 if(p==7&&id==16&&(*(DuelCardPickCursor**)datum("D_8009B1B4"))->row==3&&mode>=23){
  int before=hand_count();unsigned materialized=setup_count;((void(*)(void))engine_function("DuelScene_UpdateCardPlacement"))();
  printf("SUPPORT_PLACEMENT phase=%u hand=%d source=%d identity=%p calls=%u extra_materialization=%u\n",*(uint16_t*)datum("gDuel_wSceneStateFlags"),hand_count(),((int*)tw_symbol("state"))[3],r[25].data,ecalls("Duel_SetupCardRecord"),setup_count-materialized);
  result("Support-real-invisible-activation",(*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff)==6&&hand_count()==before&&r[25].card_id==16&&!(r[25].flags&DUEL_CARD_FLAG_OCCUPIED)&&r[25].data==&d[40],"Real pinned placement callback, exact physical deck identity; native graphics transport modeled.");
  p=*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff;
 }
 if(p==6&&id==16){result("Support-physical-single",hand_count()==4&&setup_count<=1&&conserved(),"Actual final TW planner/controller in controlled board; no plan or result injected.");if(Mods_Active(mi(D)))result("Support-real-Dual-quota",ds()[1]==2,"Final actual private Dual ledger must be Support.");if(mode>=25){support_resolution();if(mode==27)support_bonus();}}
 else puts("HAL9_NOT_REACHED real planners did not commit Time Magic; exact stop/diagnostic retained");
}
