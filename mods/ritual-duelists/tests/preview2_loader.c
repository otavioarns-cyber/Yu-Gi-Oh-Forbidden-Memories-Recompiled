/* Real rebuilt ELF32 + official API24 loader. Synthetic engine and providers;
 * no game/disc, real graphics, hook relinking or gameplay validation. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "pc/mods/object_loader.h"
#include "pc/mods/exports.h"
#include "../recipe_adapter.h"
#include "../adaptive_rules.h"
#include "game/duel_card.h"
#include "game/duel_deck_card.h"
#include "game/ai.h"
#include "game/duel_side_state.h"
#include "game/duel_hand.h"
#include "game/duel_card_display_state.h"
static unsigned checks;
#define CHECK(x) do { assert(x); ++checks; } while(0)
static LoadedObject object;
static _Alignas(16) unsigned char data[96][65536];
static char names[96][100],functions[160][100];
static unsigned char *stubs;
static int nd,nf,hooks,subscriptions,on=1,mode,original_calls,deny_state,deny_symbol;
static void *targets[32],*replacements[32];
static MemoriesModCallback callbacks[MEMORIES_EVENT_COUNT];
static RdSaveState *state;
static const char *directory;
static void *datum(const char *n);
static void noop(void) { ++original_calls; }
static int original_int(void) { ++original_calls; return 7; }
static int original_fusion(int a,int b) { (void)a;(void)b; ++original_calls; return 42; }
static void original_ritual(void) { ++original_calls; *(uint16_t*)datum("D_8009B210")=4; }
static void original_deal(int s,void *a,void *b) { (void)s;(void)a;(void)b; ++original_calls; }
static void *datum(const char *n) {
 for(int i=0;i<nd;i++)if(!strcmp(n,names[i]))return data[i];
 CHECK(nd<96&&strlen(n)<100); strcpy(names[nd],n);return data[nd++];
}
static void *stub(const char *n) {
 for(int i=0;i<nf;i++)if(!strcmp(n,functions[i]))return stubs+16*i;
 CHECK(nf<160&&strlen(n)<100);strcpy(functions[nf],n);unsigned char*p=stubs+16*nf++;
 p[0]=0x31;p[1]=0xc0;p[2]=0xc3;return p;
}
static int valid(int n){return n>0&&n<=900;}
static int base(int n){return n;}
static int no_table(int n,void *out){(void)n;(void)out;return 0;}
static int no_legacy(int n,void *out){(void)n;(void)out;return -1;}
static int named(const char*n){return !strcmp(n,"Rex Raptor")?9:!strcmp(n,"Isis")?16:0;}
static DuelCardDisplayObject display;
static void *spawn(DuelCardRecord*r,int x,int y){(void)r;(void)x;(void)y;return &display;}
static int recipe_get(int n,RitualRecipeRequirementV1 *out,unsigned capacity,int *result){
 if(mode!=1||n!=679||capacity<3)return -1;
 for(int i=0;i<3;i++){out[i]=(RitualRecipeRequirementV1){0};out[i].exact_card=535+i;
 out[i].type=out[i].min_attack=out[i].max_attack=out[i].min_defense=out[i].max_defense=out[i].min_level=out[i].max_level=-1;}
 *result=704;return 3;
}
static RitualRecipeProviderV1 recipe={1,recipe_get,0};
static int read_rules(RitualRulesSnapshotV1 *out,unsigned size){
 if(mode!=1||size<sizeof(*out))return 0;
 *out=(RitualRulesSnapshotV1){1,3,7,1,1,2,1};return 1;
}
static RitualRulesProviderV1 rules={1,read_rules};
static int effect_enabled(int n){return mode==1&&n==704;}
static int estimate(int id,const RitualEffectAiBoardV1*b,RitualEffectAiEstimateV1*e){(void)id;(void)b;(void)e;return 1;}
static RitualEffectIntelligenceV1 intelligence={1,effect_enabled,estimate};
static void *resolve(const char*n,void*context){
 (void)context;void(*lib)(void)=Mods_LibcLookup(n);if(lib)return lib;
 if(!strcmp(n,"Cards_Valid"))return valid;
 if(!strcmp(n,"Cards_BaseId"))return base;
 if(!strcmp(n,"Duelists_Named"))return named;
 if(!strcmp(n,"func_80018004"))return spawn;
 if(!strcmp(n,"Tables_Ritual"))return no_legacy;
 if(!strcmp(n,"Tables_RitualRequirements")||!strcmp(n,"Tables_RitualRule"))return no_table;
 if(!strncmp(n,"D_",2)||n[0]=='g')return datum(n);
 return stub(n);
}
static void log_message(const MemoriesModHost*h,const char*f,...){(void)h;(void)f;}
static int setting(const MemoriesModHost*h,const char*n,int fallback){(void)h;return !strcmp(n,"debug_overlay")||!strcmp(n,"diagnostics")?0:fallback;}
static int applied(const MemoriesModHost*h){(void)h;return on;}
static void *symbol(const MemoriesModHost*h,const char*n){(void)h;return deny_symbol?0:resolve(n,0);}
static void *find(const MemoriesModHost*h,const char*n){
 (void)h;if(!mode)return 0;
 if(!strcmp(n,"ritual-card-effects-test:recipe_v1"))return &recipe;
 if(!strcmp(n,"melhoria-dos-rituais:rules_v1"))return &rules;
 if(!strcmp(n,"ritual-card-effects-test:effect_intelligence_v1"))return &intelligence;
 return 0;
}
static FILE *asset(const MemoriesModHost*h,const char*n){
 (void)h;CHECK(!strstr(n,"..")&&n[0]!='/');char path[4096];snprintf(path,sizeof path,"%s/%s",directory,n);return fopen(path,"rb");
}
static int register_state(const MemoriesModHost*h,void*p,size_t n,unsigned v){
 (void)h;CHECK(n==sizeof(RdSaveState)&&v==4);state=p;return !deny_state;
}
static void unhook(const MemoriesModHost*h,int token){(void)h;(void)token;}
static int hook(const MemoriesModHost*h,void*f,void*r,void**original){
 (void)h;CHECK(f&&r&&original&&hooks<32);targets[hooks]=f;replacements[hooks]=r;
 *original=f==resolve("AiScript_Run",0)||f==resolve("Ai_GetHandSize",0)?(void*)original_int:
 f==resolve("Duel_CheckFusion",0)?(void*)original_fusion:
 f==resolve("Duel_ShuffleDeck",0)?(void*)original_deal:
 f==resolve("DuelEffect_ApplyRitual",0)?(void*)original_ritual:(void*)noop;
 return ++hooks;
}
static int subscribe(const MemoriesModHost*h,unsigned e,int priority,MemoriesModCallback cb){
 (void)h;(void)priority;CHECK(e<MEMORIES_EVENT_COUNT&&cb);callbacks[e]=cb;return ++subscriptions;
}
static void *replacement(const char*n){void*t=resolve(n,0);for(int i=0;i<hooks;i++)if(targets[i]==t)return replacements[i];return 0;}
static void *local_data(const char*n,size_t size){
 for(size_t i=0;i<object.symbol_count;i++)if(!strcmp(n,object.symbols[i].name)&&!object.symbols[i].function){
  CHECK(object.symbols[i].size==size);return (void*)object.symbols[i].address;
 }CHECK(0);return 0;
}
static void instances(void){
 void(*ritual)(void)=replacement("DuelEffect_ApplyRitual");int(*fusion)(int,int)=replacement("Duel_CheckFusion");
 DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord physical={704,59,0,0,0},copy={704,58,0,0,0};
 *(uint8_t*)datum("D_8009B19C")=20;*(int16_t*)datum("D_8009B1A0")=704;
 *(uint16_t*)datum("D_8009B210")=0x83;r[20].card_id=704;r[20].flags=DUEL_CARD_FLAG_OCCUPIED;r[20].data=&physical;
 int before=original_calls;ritual();CHECK(original_calls==before+1&&state->ritual_marks[20].active&&state->ritual_marks[20].origin==59);
 *(int8_t*)datum("gDuel_bOpponentID")=9;*(uint8_t*)datum("D_8009B1D5")=1;((int32_t*)datum("D_8009B360"))[1]=0;
 memset(datum("D_800907D8"),255,40);((uint8_t*)datum("D_800907D8"))[25]=20;
 CHECK(fusion(704,2)==0); /* Original ID-only checker conservatively protects normal copies too. */
 r[20].data=&copy;CHECK(fusion(704,2)==42);r[20].data=&physical;CHECK(fusion(704,2)==0);
 r[20].flags=0;CHECK(fusion(704,2)==42);
 *(int8_t*)datum("gDuel_bOpponentID")=16;r[20].flags=DUEL_CARD_FLAG_OCCUPIED;CHECK(fusion(704,2)==42); /* Isis stays Vanilla. */
 memset(r,0,30*sizeof(*r));memset(state->ritual_marks,0,sizeof(state->ritual_marks));
}
static void materialize(void){
 /* Seed private pending action to isolate the public materialization hook;
    not a game save format or a claim that the full AI produced this action. */
 struct Pull{int active,side,source,replace,card,mode,target;unsigned char fallback[12];};
 struct Pull*p=local_data("deck_pull",sizeof(*p));void(*swap)(void)=replacement("func_8001BAF0");
 DuelDeckCardRecord*deck=datum("gDuel_aDeckCardRecords");AiActiveCard*a=datum("gDuel_aActiveCards");
 DuelSideState*s=datum("D_800E9FF0");DuelHandSlot*hand=datum("D_800EA030");uint8_t*sel=datum("D_800EAE88");
 for(int side=0;side<2;side++){
  int base=side*40;*(uint8_t*)datum("D_8009B1D5")=side;s[side].hand[0]=base;s[side].deck_draw_cursor=5;
  deck[base]=(DuelDeckCardRecord){535,base,11,12,13};deck[base+10]=(DuelDeckCardRecord){679,base+10,21,22,23};
  a[16].card_id=679;a[16].deck_index=base+10;hand[0].object=(uint8_t*)&display;
  *p=(struct Pull){1,side,16,0,679,1,6,{12,0}};memset(sel,0,12);sel[0]=16;sel[6]=6;sel[8]=1;
  swap();CHECK(!p->active&&deck[base].id==679&&deck[base+10].id==535&&deck[base].deck_index==base&&deck[base+10].deck_index==base+10&&sel[0]==11);
  int before=original_calls;swap();CHECK(original_calls==before+1); /* consumed once */
  *p=(struct Pull){1,side,16,0,679,1,6,{12,0}};sel[0]=16;before=original_calls;
  swap();CHECK(!p->active&&original_calls==before+1&&sel[0]==12&&deck[base].id==679); /* stale source: no second exchange */
 }
}
static void contracts(MemoriesModHost*h){
 const RdOpponentProfile*(*profile)(int)=ObjectLoader_Symbol(&object,"Rd_GetProfile");
 int(*resolve_recipe)(const MemoriesModHost*,const RdOpponentProfile*,int,RdRecipeCache*)=ObjectLoader_Symbol(&object,"Rd_ResolveRecipe");
 void(*read)(const MemoriesModHost*,RdAdaptiveRules*)=ObjectLoader_Symbol(&object,"Rd_ReadAdaptiveRules");
 void(*connect)(const RitualEffectIntelligenceV1*)=ObjectLoader_Symbol(&object,"Rd_ConnectEffectIntelligence");
 int(*enabled)(int)=ObjectLoader_Symbol(&object,"Rd_EffectIntelligenceEnabled");
 CHECK(profile&&resolve_recipe&&read&&connect&&enabled);const RdOpponentProfile*p=profile(23);CHECK(p);
 uint16_t *retail=datum("gDuel_awRitualData");uint16_t entry[]={679,535,555,556,704,0};memcpy(retail,entry,sizeof entry);
 for(mode=0;mode<4;mode++){
  recipe.abi=rules.abi_version=intelligence.abi=mode==3?999:1;
  RdRecipeCache cache;RdAdaptiveRules adaptive;
  CHECK(resolve_recipe(h,p,0,&cache));CHECK(cache.source==(mode==1?1:4));
  CHECK(cache.recipe.materials[1].exact_card==(mode==1?536:555));
  read(h,&adaptive);CHECK(adaptive.available==(mode==1));
  connect(find(h,"ritual-card-effects-test:effect_intelligence_v1"));CHECK(enabled(704)==(mode==1));
  CHECK(resolve_recipe(h,p,1,&cache)&&cache.source==4); /* Force Vanilla ignores provider. */
 }
 mode=0;connect(NULL);memset(retail,0,sizeof entry);
}
static void save_reset(MemoriesMod*mod){
 state->generated=7;state->opp_id=9;state->starter_ready=1;state->starter_theme=0;state->simon_theme=2;
 state->simon_deck[0]=693;state->ritual_marks[20]=(RdAiRitualMark){704,59,1};
 RdSaveState snapshot=*state;memset(state,0,sizeof(*state));memcpy(state,&snapshot,sizeof snapshot);
 mod->reset();CHECK(!memcmp(state,&snapshot,sizeof snapshot));
 /* Exercise SAVE/LOAD callbacks of the starter modal, not the game's file format. */
 MemoriesModEvent e={0};state->starter_modal.selected=2;e.type=MEMORIES_EVENT_LOAD;e.phase=MEMORIES_AFTER;callbacks[e.type](&e);
 e.type=MEMORIES_EVENT_SAVE;e.phase=MEMORIES_BEFORE;callbacks[e.type](&e);CHECK(state->starter_modal.selected==2);
}
int main(int argc,char**argv){
 CHECK(argc==3);directory=argv[2];stubs=mmap(NULL,4096,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(stubs!=MAP_FAILED);
 FILE*f=fopen(argv[1],"rb");CHECK(f);fseek(f,0,SEEK_END);long length=ftell(f);rewind(f);void*bytes=malloc(length);CHECK(bytes&&fread(bytes,1,length,f)==(size_t)length);fclose(f);
 for(int cycle=0;cycle<12;cycle++){
  nd=nf=hooks=subscriptions=mode=original_calls=0;deny_state=cycle==10;deny_symbol=cycle==11;on=1;
  memset(data,0,sizeof data);memset(callbacks,0,sizeof callbacks);char error[512];
  int result=ObjectLoader_Load(bytes,length,resolve,NULL,&object,error,sizeof error);if(result)fprintf(stderr,"%s\n",error);CHECK(!result);
  *(int*)datum("gCard_nCount")=900;
  MemoriesModHost host={0};MemoriesMod mod={0};host.api=24;host.id="ritual-duelists";host.directory=directory;host.log=log_message;host.setting=setting;
  host.applied=applied;host.symbol=symbol;host.find=find;host.hook=hook;host.unhook=unhook;host.subscribe=subscribe;host.register_state=register_state;host.open_asset=asset;
  int(*init)(const MemoriesModHost*,MemoriesMod*)=ObjectLoader_Symbol(&object,"MemoriesModInit");CHECK(init);int accepted=init(&host,&mod);
  if(cycle>=10){CHECK(!accepted);ObjectLoader_Free(&object);continue;}
  CHECK(accepted&&hooks==8&&subscriptions==6&&mod.api==12&&mod.reset&&mod.applied&&!mod.frame);
  if(!cycle)printf("object_hash=%08x declared_api=%u state_size=%zu state_version=4 hooks=%d subscriptions=%d\n",object.hash,mod.api,sizeof(*state),hooks,subscriptions);
  contracts(&host);save_reset(&mod);instances();materialize();mod.reset();
  int(*ai)(void)=replacement("AiScript_Run"),(*window)(void)=replacement("Ai_GetHandSize");
  int(*fusion)(int,int)=replacement("Duel_CheckFusion");void(*swap)(void)=replacement("func_8001BAF0");
  void(*deal)(int,void*,void*)=replacement("Duel_ShuffleDeck");
  CHECK(ai&&window&&fusion&&swap&&deal);*(int8_t*)datum("gDuel_bOpponentID")=0;
  CHECK(ai()==7&&window()==7&&fusion(1,2)==42);int before=original_calls;swap();deal(1,NULL,NULL);CHECK(original_calls==before+2);
  for(int i=0;i<5;i++){on=0;mod.applied(0);mod.reset();on=1;mod.applied(1);mod.reset();}
  ObjectLoader_Free(&object);
 }
 free(bytes);munmap(stubs,4096);printf("PASS %u assertions; ten independent loads and two expected init refusals; synthetic host\n",checks);return 0;
}
