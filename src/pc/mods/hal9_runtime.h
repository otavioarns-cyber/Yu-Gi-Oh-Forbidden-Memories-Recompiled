/* Experimental private imports for one pinned TW object, API24/i386 only.
 * This is read-only owner/callback discovery, not hook admission or a new
 * provider owned by Dual Play. Include inside mods.c after Mod is defined. */
#include "hal9_runtime_pins.h"
#ifndef HAL9_TRACE_HARD
#define HAL9_TRACE_HARD() ((void)0)
#endif
static int hal9_enabled(void){return sizeof(void*)==4&&MEMORIES_MOD_API==24&&Settings_GetNamed("experimental.hal9_contracts",0);}
static int hal9_digest(const Mod*m,const char*sha){
 static const char hex[]="0123456789abcdef";
 if(!m||!sha||strlen(sha)!=64)return 0;
 for(unsigned i=0;i<32;i++)if(sha[i*2]!=hex[m->file_sha256[i]>>4]||sha[i*2+1]!=hex[m->file_sha256[i]&15])return 0;
 return 1;
}
static int hal9_find(const char*id){for(int i=0;i<mod_count;i++)if(!strcmp(mods[i].id,id))return i;return -1;}
static int hal9_self(void){int i=hal9_find("time-wizard-time-magic");return hal9_enabled()&&i>=0&&mods[i].object.image&&hal9_digest(&mods[i],HAL9_TW_SHA);}
static int hal9_known_hard(int i){return Mods_CodeMatches(i,0x00049e34u,"05cf957bc4575503d257432cd927748367207258189856217bc1bf2adb0f7d19");}
static int hal9_known_dual(int i){return Mods_CodeMatches(i,0x3028c68au,"77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378");}
static int hal9_hard_state(void){
 HAL9_TRACE_HARD();
 if(!hal9_self())return -1;
 int h=hal9_find("ai-hard-mode");if(h<0||!mods[h].active)return 0;
 if(!hal9_known_hard(h))return -1;
 const void*target=Mods_Lookup("AiScript_Run");
 for(int n=0;;n++){int owner;const void*t,*r,*p,*b;if(!Hooks_Inspect(n,&owner,&t,&r,&p,&b))break;
  if(owner==h&&t==target&&Mods_CallbackMatches(h,r,"run",3820))return 1;
 }
 return -1;
}
static int hal9_owners_ready(void){
 if(!hal9_self()||!(uintptr_t)Mods_Lookup("AiScript_Run")||((uintptr_t)Mods_Lookup("AiScript_Run")==1))return 0;
 int d=hal9_find("dual-play");return hal9_hard_state()>=0&&(d<0||!mods[d].active||hal9_known_dual(d));
}
static int hal9_draw_callback(int owner,const void*r){
 if(owner<0||owner>=mod_count)return 0;
 if(!strcmp(mods[owner].id,"dual-play"))return hal9_known_dual(owner)&&Mods_CallbackMatches(owner,r,"dual_draw_phase",152);
 if(!strcmp(mods[owner].id,"ritual-card-effects-test"))return Mods_CodeMatches(owner,0x1e478c68u,"78ec3ce45694b1f583213530dfc0a527c8974056f67c49e7791fb6821f55ab45")&&Mods_CallbackMatches(owner,r,"draw_phase",33);
 if(!strcmp(mods[owner].id,"time-wizard-time-magic")){
  /* Both records are compiled functions in the exact object; no arbitrary
     callback from the same named owner is accepted. */
  if(!hal9_digest(&mods[owner],HAL9_TW_SHA))return 0;
  return Mods_CallbackMatches(owner,r,"compat_draw_phase",HAL9_TW_DRAW_BYTES);
 }
 return 0;
}
static const void*hal9_dual_draw(void){
 if(!hal9_owners_ready())return NULL;
 int d=hal9_find("dual-play");if(d<0||!mods[d].active)return NULL;
 const void*target=Mods_Lookup("DuelScene_UpdateDrawPhase"),*last=NULL,*dual=NULL;
 for(int n=0;;n++){int owner;const void*t,*r,*p,*body;if(!Hooks_Inspect(n,&owner,&t,&r,&p,&body))break;
  if(owner<0||owner>=mod_count||!mods[owner].active||t!=target)continue;
  if(!hal9_draw_callback(owner,r)||p!=(last?last:body))return NULL;
  if(owner==d){if(dual)return NULL;dual=r;}last=r;
 }
 return dual;
}
static void*hal9_resolve(const char*name,Mod*caller){
 if(!hal9_enabled()||!caller||strcmp(caller->id,"time-wizard-time-magic")||!hal9_digest(caller,HAL9_TW_SHA))return NULL;
 if(!strcmp(name,"Hal9_CoreHardStateV1"))return (void*)hal9_hard_state;
 if(!strcmp(name,"Hal9_CoreDualDrawV1"))return (void*)hal9_dual_draw;
 if(!strcmp(name,"Hal9_CoreOwnersReadyV1"))return (void*)hal9_owners_ready;
 return NULL;
}
