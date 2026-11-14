/* Rebuilt ELF + official object loader. Host/game data and rendering are fake.
 * Does not emulate a duel, Windows, the complete manager or disk save states. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "pc/mods/object_loader.h"
#include "pc/mods/exports.h"
#include "pc/mods/modapi.h"
#include "game/duel_card.h"
#include "game/duel_side_state.h"
#include "game/card_constants.h"
#include "../ritual_recipe_provider_v1.h"
#include "../ritual_effect_intelligence_v1.h"
#include "../model.h"
typedef struct {unsigned abi;int(*can_defend)(int);} Position;
static unsigned checks;
#define CHECK(x) do {assert(x);++checks;}while(0)
static LoadedObject object;
static const char* moddir;
static unsigned char engine[128][65536];
static char engine_names[128][96];
static unsigned char*function_stubs;
static char function_names[256][96];static int functions;
static int engines,hooks,removed,subscriptions,unsubscribed,provide_count,active=1;
static int without_assets,without_provide,disabled_settings,foreign_mode;
static unsigned state_registrations;
static const char*provided_names[4];static void*provided[4];
static void*hook_target[64],*hook_replacement[64];
static uint16_t bank[1024*512];
static void noop(void){}
static int original_stats(DuelCardRecord*c){(void)c;return (1000<<16)|1000;}
static uint16_t *gpu_bank(int n){CHECK(n==13);return bank;}
static int card_valid(int n){return n>=1&&n<=722;}
static int card_base(int n){return n;}
static int card_type(int n){(void)n;return 0;}
static int card_level(int n){(void)n;return 1;}
static int in_group(int id,int group){(void)id;(void)group;return 0;}
static void *data_for(const char*n){
 for(int i=0;i<engines;i++)if(!strcmp(n,engine_names[i]))return engine[i];
 CHECK(engines<128&&strlen(n)<96);strcpy(engine_names[engines],n);return engine[engines++];
}
static void *function_for(const char*n){
 for(int i=0;i<functions;i++)if(!strcmp(n,function_names[i]))return function_stubs+i*16;
 CHECK(functions<256&&strlen(n)<96);strcpy(function_names[functions],n);
 unsigned char*p=function_stubs+functions++*16;p[0]=0x31;p[1]=0xc0;p[2]=0xc3;return p;
}
static void *resolve(const char*n,void*context){
 (void)context;void(*p)(void)=Mods_LibcLookup(n);if(p)return (void*)p;
 if(!strcmp(n,"SoftGpu_Bank"))return gpu_bank;
 if(!strcmp(n,"Cards_Valid"))return card_valid;
 if(!strcmp(n,"Cards_BaseId"))return card_base;
 if(!strcmp(n,"Cards_Type"))return card_type;
 if(!strcmp(n,"Cards_Level"))return card_level;
 if(!strcmp(n,"Cards_InFusionGroup"))return in_group;
 if(!strncmp(n,"D_",2)||n[0]=='g')return data_for(n);
 return function_for(n);
}
static void *symbol(const MemoriesModHost*h,const char*n){(void)h;(void)n;return noop;}
static void log_message(const MemoriesModHost*h,const char*f,...){(void)h;(void)f;}
static int setting(const MemoriesModHost*h,const char*n,int fallback){
 (void)h;if(disabled_settings&&strcmp(n,"effect_ai_trace"))return 0;return fallback;
}
static void set_setting(const MemoriesModHost*h,const char*k,int v){(void)h;(void)k;(void)v;CHECK(!"bulk setting was not requested");}
static int applied(const MemoriesModHost*h){(void)h;return active;}
static uint64_t now_us(const MemoriesModHost*h){(void)h;return 1000000;}
static int hook(const MemoriesModHost*h,void*f,void*r,void**o){
 (void)h;CHECK(f&&r&&o&&hooks<64);hook_target[hooks]=f;hook_replacement[hooks]=r;*o=f==resolve("Duel_CalcCardStats",NULL)?(void*)original_stats:(void*)noop;return ++hooks;
}
static void unhook(const MemoriesModHost*h,int token){(void)h;CHECK(token>=1&&token<=hooks);++removed;}
static int subscribe(const MemoriesModHost*h,unsigned event,int priority,MemoriesModCallback cb){
 (void)h;(void)priority;CHECK(cb&&(event==MEMORIES_EVENT_DAMAGE||event==MEMORIES_EVENT_EQUIP));return ++subscriptions;
}
static void unsubscribe(const MemoriesModHost*h,int token){(void)h;CHECK(token==1||token==2);++unsubscribed;}
static int register_state(const MemoriesModHost*h,void*p,size_t n,unsigned v){(void)h;(void)p;(void)n;(void)v;++state_registrations;return 1;}
static int provide(const MemoriesModHost*h,const char*n,void*p){
 (void)h;CHECK(provide_count<4&&p);provided_names[provide_count]=n;provided[provide_count++]=p;return !without_provide;
}
static void*provider(const char*n){for(int i=0;i<provide_count;i++)if(!strcmp(n,provided_names[i]))return provided[i];return 0;}
static int foreign_on(void){return foreign_mode!=2;}
static void foreign_read(EaCard*c,int slot,int viewer){(void)c;(void)slot;(void)viewer;}
static int foreign_policy(int side,int kind){(void)side;(void)kind;return 0;}
static int foreign_pair(int a,int b,int active,int*r,int*o){(void)a;(void)b;(void)active;(void)r;(void)o;return 0;}
static EaProvider foreign={2,foreign_on,foreign_read,foreign_policy,foreign_pair};
static void*find(const MemoriesModHost*h,const char*n){
 (void)h;if(strstr(n,"fusion-auxiliary-balance:effect_forecast_v2"))return foreign_mode?&foreign:0;
 if(strstr(n,"ritual-card-effects-test:effect_forecast_v2"))return provider("effect_forecast_v2");
 return 0;
}
static FILE*open_asset(const MemoriesModHost*h,const char*relative){
 (void)h;if(without_assets)return 0;CHECK(!strstr(relative,"..")&&relative[0]!='/');
 char path[4096];CHECK(snprintf(path,sizeof path,"%s/%s",moddir,relative)<(int)sizeof path);return fopen(path,"rb");
}
static void*local(const char*n,size_t*size){
 for(size_t i=0;i<object.symbol_count;i++)if(!strcmp(n,object.symbols[i].name)){
  if(size)*size=object.symbols[i].size;return (void*)object.symbols[i].address;
 }return 0;
}
static void check_providers(MemoriesMod*mod){
 RitualRecipeProviderV1*r=provider("recipe_v1");RitualEffectIntelligenceV1*ai=provider("effect_intelligence_v1");
 EaProvider*forecast=provider("effect_forecast_v2");Position*pos=provider("position-rules-v1");
 CHECK(r&&r->abi==1&&ai&&ai->abi==1&&forecast&&forecast->abi==2&&pos&&pos->abi==1);
 RitualRecipeRequirementV1 req[3];int result=0;RitualEffectAiBoardV1 board={0};RitualEffectAiEstimateV1 estimate={0};
 active=1;mod->applied(1);CHECK(r->get(666,req,3,&result)==3&&result==357);
 CHECK(r->get(666,req,2,&result)==-1);CHECK(r->get(666,NULL,3,&result)==-1);
 CHECK(r->matches(666,0,101));CHECK(!r->matches(666,3,101));CHECK(!r->matches(666,0,0));
 CHECK(ai->enabled(357));CHECK(ai->estimate(357,&board,&estimate)&&estimate.strategic_value==600);
 CHECK(!ai->estimate(357,NULL,&estimate));CHECK(!ai->estimate(357,&board,NULL));
 board.own[0]=22;board.enemy[0]=545;board.enemy_hidden[0]=1;board.hand[0]=24;board.hand_count=1;
 CHECK(ai->estimate(704,&board,&estimate)&&estimate.attack_bonus==1500);
 board.selected[0]=board.selected[5]=1;CHECK(ai->estimate(704,&board,&estimate)&&!estimate.attack_bonus);
 CHECK(forecast->on());EaCard card={0};card.id=704;forecast->read(&card,-1,0);CHECK(card.possible&EA_RITUAL);CHECK(!card.active);
 CHECK(pos->can_defend(-1)&&pos->can_defend(5));
 active=0;mod->applied(0);CHECK(r->get(666,req,3,&result)==-1);CHECK(!r->matches(666,0,101));
 CHECK(!ai->enabled(357));memset(&estimate,1,sizeof estimate);CHECK(!ai->estimate(357,&board,&estimate)&&!estimate.strategic_value);
 CHECK(!forecast->on());CHECK(pos->can_defend(5));
 active=1;mod->applied(1);
 disabled_settings=1;mod->applied(1);CHECK(!ai->enabled(357)&&!forecast->on()&&r->get(666,req,3,&result)==-1);
 disabled_settings=0;mod->applied(1);
}
static void check_optional_forecast(void){
 void(*play)(void)=NULL;for(int i=0;i<hooks;i++)if(hook_target[i]==resolve("AiScript_PlayFieldCard",NULL))play=hook_replacement[i];
 CHECK(play);void**fapi=local("ea_fapi",NULL);CHECK(fapi);
 /* Empty synthetic AI hand: exercise connection/fallback, not strategic decisions. */
 foreign_mode=0;play();CHECK(!*fapi);
 foreign_mode=2;foreign.abi=2;play();CHECK(!*fapi);
 foreign_mode=1;foreign.abi=1;play();CHECK(!*fapi);
 foreign.abi=2;play();CHECK(*fapi==&foreign);foreign_mode=0;
}
static void check_effect_instances(MemoriesMod*mod){
 DuelCardRecord*cards=data_for("D_801A7AD8");DuelSideState*sides=data_for("D_800E9FF0");
 uint8_t*hand_records=data_for("D_800907CC");uint32_t*stats=data_for("gDuel_adwCardStats");
 int(*calc)(DuelCardRecord*)=NULL;for(int i=0;i<hooks;i++)if(hook_target[i]==resolve("Duel_CalcCardStats",NULL))calc=hook_replacement[i];CHECK(calc);
 typedef struct{void*data;unsigned ritual,battle_flags;} SeaMark;
 SeaMark*whale=local("whale_marks",NULL);unsigned*hint=local("stat_effect_hints",NULL);CHECK(whale&&hint);char whale_identity;
 for(int side=0;side<2;side++){
  memset(cards,0,30*sizeof(*cards));memset(sides,0,2*sizeof(*sides));
  memset(sides[0].hand,-1,HAND_SIZE);memset(sides[1].hand,-1,HAND_SIZE);
  int field=side?20:5,hand=side?15:0;
  cards[field].card_id=718;cards[field].flags=DUEL_CARD_FLAG_OCCUPIED;cards[field].data=(void*)&whale_identity;whale[field].data=&whale_identity;whale[field].ritual=1;*hint=1u<<4;
  cards[field+1].card_id=400;cards[field+1].flags=DUEL_CARD_FLAG_OCCUPIED;
  cards[hand].card_id=401;cards[hand].flags=DUEL_CARD_FLAG_OCCUPIED;
  hand_records[side*5]=hand;sides[side].hand[0]=side*40;
  stats[717]=stats[399]=stats[400]=(uint32_t)CARD_TYPE_FISH<<CARD_STAT_TYPE_SHIFT;
  CHECK(((unsigned)calc(&cards[field])>>16)==2500);CHECK((calc(&cards[field])&65535)==1000);
  sides[side].hand[0]=-1;CHECK(((unsigned)calc(&cards[field])>>16)==2000); /* stale physical hand card ignored */
  cards[field+1].flags=0;CHECK(((unsigned)calc(&cards[field])>>16)==1000);
 }
 memset(cards,0,30*sizeof(*cards));mod->reset();
 typedef struct{void*data;unsigned ritual;} CoinMark;
 CoinMark*marks=local("chaos_marks",NULL);Position*pos=provider("position-rules-v1");CHECK(marks&&pos);
 char identity,replacement;cards[5].card_id=722;cards[5].flags=DUEL_CARD_FLAG_OCCUPIED;cards[5].data=(void*)&identity;
 marks[5].data=&identity;marks[5].ritual=1;CHECK(!pos->can_defend(5));
 cards[5].data=(void*)&replacement;CHECK(pos->can_defend(5)); /* identical ID, different instance */
 cards[5].data=(void*)&identity;CHECK(!pos->can_defend(5));mod->reset();CHECK(pos->can_defend(5));
 memset(cards,0,30*sizeof(*cards));
}
static void simulated_load_reset(MemoriesMod*mod){
 size_t n=0;void*marks=local("skull_marks",&n);CHECK(marks&&n==360);
 unsigned char saved[360];memset(marks,1,n);memcpy(saved,marks,n);
 /* A simulated guest restore does not serialize native MOD BSS. */
 memcpy(marks,saved,n);mod->reset();for(size_t i=0;i<n;i++)CHECK(!((unsigned char*)marks)[i]);
 CHECK(!state_registrations); /* Production deliberately clears effects on load. */
}
int main(int argc,char**argv){
 function_stubs=mmap(NULL,4096,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(function_stubs!=MAP_FAILED);
 CHECK(argc==3);moddir=argv[2];FILE*f=fopen(argv[1],"rb");CHECK(f);fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);
 void*bytes=malloc(n);CHECK(bytes&&fread(bytes,1,n,f)==(size_t)n);fclose(f);
 for(int cycle=0;cycle<12;cycle++){
  engines=hooks=removed=subscriptions=unsubscribed=provide_count=state_registrations=0;
  without_assets=without_provide=disabled_settings=foreign_mode=0;memset(engine,0,sizeof engine);memset(&object,0,sizeof object);
  char error[512];int status=ObjectLoader_Load(bytes,n,resolve,NULL,&object,error,sizeof error);if(status)fprintf(stderr,"%s\n",error);CHECK(!status);
  int(*init)(const MemoriesModHost*,MemoriesMod*)=ObjectLoader_Symbol(&object,"MemoriesModInit");CHECK(init);
  MemoriesModHost host={0};MemoriesMod mod={0};host.api=24;host.symbol=symbol;host.log=log_message;
  host.setting=setting;host.set_setting=set_setting;host.applied=applied;host.now_us=now_us;
  host.hook=hook;host.unhook=unhook;host.subscribe=subscribe;host.unsubscribe=unsubscribe;
  host.register_state=register_state;host.provide=provide;host.find=find;host.open_asset=open_asset;
  if(cycle==10)host.provide=NULL;if(cycle==11)without_assets=1;
  int accepted=init(&host,&mod);
  if(cycle==11){CHECK(!accepted&&hooks==0&&subscriptions==2&&unsubscribed==2);ObjectLoader_Free(&object);continue;}
  CHECK(accepted&&hooks==50&&subscriptions==2&&mod.api==4&&mod.reset&&mod.shutdown&&mod.frame);
  if(cycle<10){CHECK(provide_count==4);check_providers(&mod);check_optional_forecast();check_effect_instances(&mod);simulated_load_reset(&mod);}
  else {CHECK(!provide_count);mod.applied(1);mod.reset();}
  mod.frame();(void)mod.overlay_signature();active=0;mod.applied(0);active=1;mod.applied(1);
  mod.shutdown();CHECK(removed==50&&unsubscribed==2);
  if(!cycle)printf("object_hash=%08x hooks=%d provider_count=%d api=%u; PNG coin assets decoded by production code\n",object.hash,hooks,provide_count,mod.api);
  ObjectLoader_Free(&object);
 }
 free(bytes);munmap(function_stubs,4096);printf("PASS %u assertions; 10 full provider/lifecycle cycles, publication-absent init, assets-absent rejection; synthetic host, no duel\n",checks);
}
