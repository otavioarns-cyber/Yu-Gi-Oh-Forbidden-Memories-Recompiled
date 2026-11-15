#ifdef HAL26_TW_PROBE
extern const MemoriesModHost*Hal25_Host(int);
extern void Hal26_TwProbe(const MemoriesModHost*,void(*)(int),void*,void*,void*,void*,int[9]);
static void hal26_fusion_toggle(int on){Hal25_ForceApplied(mi(F),on);}
static void timewizard_contract_cases(void){
 if(mi(F)<0||!Mods_Active(mi(F)))return;
 begin("HAL26-TimeWizard-real-consumers-modeled-wheel");put_field(6,495,1);
 void*ainative=engine_function("AiScript_Run"),*ai=ainative,*drawnative=engine_function("DuelScene_UpdateDrawPhase"),*draw=drawnative;int aiowner=-1,drawowner=-1;
 for(int i=0;;i++){int owner;const void *target,*replacement,*previous,*body;if(!Hooks_Inspect(i,&owner,&target,&replacement,&previous,&body))break;if(!Mods_Active(owner))continue;
  if(target==ainative){ai=(void*)replacement;aiowner=owner;}
  if(target==drawnative){draw=(void*)replacement;drawowner=owner;}
 }
 int out[9]={0};Hal26_TwProbe(Hal25_Host(mi(F)),hal26_fusion_toggle,ainative,ai,drawnative,draw,out);
 result("TW-source-bridge-init",out[0],"Actual supplied Encore source; modeled base init and graphical wheel, real Fusion provider.");
 result("TW-real-Fusion-eligible-ON",out[1]==1,"Musician King on real provider board; optional contract resident.");
 result("TW-cached-attempt-after-Fusion-OFF-reproduced",out[2]==1,"Known consumer cache survives effective OFF for an already-started attempt. No probability/rule change implemented.");
 result("TW-fresh-attempt-after-OFF-neutral",out[3]==0,"Fresh actual-source lookup sees F1 neutral OFF provider.");
 result("TW-wrong-ID-cancel-reset-safe",out[4],"Wrong spin ID/cancelled callback does not rearm an abandoned attempt.");
 result("TW-manual-input-does-not-sample-RNG",out[5],"Original manual choice path, no roulette probability changes.");
 result("TW-scene-change-clears-attempt",out[6],"Actual source frame wrapper abandons modeled wheel transaction.");
 int h=Mods_Active(mi(H));printf("AUDIT TW_AI_DETECTOR %s actual_H=%d inferred=%d predecessor_owner=%s\n",h==out[7]?"PASS":"FAIL",h,out[7],aiowner>=0?Mods_Id(aiowner):"native");
 result("TW-AI-detection-observation",out[7]==(ai!=ainative),"Literal source predicate tests any non-native predecessor; FAIL audit when real H is OFF is retained separately, not compatibility approval.");
 printf("AUDIT TW_DUAL_SIGNATURE %s actual_D=%d recognized=%d predecessor_owner=%s\n",out[8]==Mods_Active(mi(D))?"PASS":"REFUSED",Mods_Active(mi(D)),out[8],drawowner>=0?Mods_Id(drawowner):"native");
 void*direct=mi(D)>=0&&Mods_CodeResident(mi(D))?local(D,"dual_draw_phase"):NULL;
 result("TW-current-Dual-direct-signature",out[8]==(direct&&draw==direct),"Actual source resolver on exact relocated callback body, tracing thunks removed by read-only test inspector. No whole Time Wizard admitted.");
 puts("INSTRUMENTED effective-OFF callbacks for all known owners, ignoring restart preferences solely at this test seam");
 int all_off=1,resident=0;
 for(int m=0;m<NMOD;m++)if(m!=OBS&&m!=W&&mi(m)>=0&&Mods_CodeResident(mi(m))){resident=1;Hal25_ForceApplied(mi(m),0);all_off&=!Mods_Active(mi(m));}
 char error[256]={0};result("B-all-resident-OFF-still-refused",all_off&&resident&&!Mods_Hal20StateLoadAllowed(error,sizeof error),"Actual manager and resident objects; toggling callback OFF cannot bypass incomplete-state refusal, including order guard OFF. No load performed.");
}
#endif
