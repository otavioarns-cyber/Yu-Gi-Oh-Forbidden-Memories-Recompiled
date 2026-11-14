/* Real ELF32 and official API24 loader, synthetic host/engine. No game image.
 * Private data are seeded only for contract/identity tests, not as a save format.
 * Engine entry points and originals are stubs; this is not integrated gameplay. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "pc/mods/modapi.h"
#include "pc/mods/object_loader.h"
#include "pc/mods/exports.h"
#include "game/duel_card.h"
#include "game/duel_deck_card.h"
#include "game/duel_card_display_state.h"
#include "game/display_object.h"
#include "../model.h"
#include "../origin_effects_v1.h"
#include "../ritual_effect_intelligence_v1.h"
typedef struct{unsigned api;int(*eligible)(int);} Encore;
typedef struct{unsigned api;unsigned(*lease)(int);void(*revoke)(int);} Unlock;
typedef struct{int id;void*data;unsigned serial;} Lease;
static unsigned checks;
#define CHECK(x) do{assert(x);++checks;}while(0)
static LoadedObject object;
static const char*directory;
static _Alignas(16) unsigned char data[128][65536];static char data_names[128][100];static int datas;
static unsigned char*stubs;static char function_names[256][100];static int functions;
static int hooks,subscriptions,providers,active=1,setting_off,ritual_mode,foreign_on=1,deny_assets,no_provide,state_calls;
static void*targets[64],*replacements[64],*provided[4];static const char*provider_names[4];
static int restore_deck,restores;
static void noop(void){}
static void*datum(const char*n){
 for(int i=0;i<datas;i++)if(!strcmp(n,data_names[i]))return data[i];
 CHECK(datas<128&&strlen(n)<100);strcpy(data_names[datas],n);return data[datas++];
}
static void*stub(const char*n){
 for(int i=0;i<functions;i++)if(!strcmp(n,function_names[i]))return stubs+i*16;
 CHECK(functions<256&&strlen(n)<100);strcpy(function_names[functions],n);unsigned char*p=stubs+16*functions++;
 p[0]=0x31;p[1]=0xc0;p[2]=0xc3;return p;
}
static int valid(int id){return id>=1&&id<=900;}
static int base_id(int id){return id;}
static int card_type(int id){return id==318||id==658?23:0;}
static int level(int id){(void)id;return 4;}
static int yes(int x){(void)x;return 1;}
static int table_fusion(int a,int b){return (a==61&&b==658)||(b==61&&a==658)?412:0;}
static int original_equip(int a,int b){(void)a;return b;}
static void original_remove(DuelCardRecord*r){r->flags=0;}
static void original_restore(int tagged,int deck){
 int slot=(tagged&128)?(tagged&127)+15:tagged;restore_deck=deck;++restores;
 DuelCardRecord*r=datum("D_801A7AD8");memset(&r[slot],0,sizeof r[slot]);r[slot].card_id=1;
}
static DuelCardDisplayObject rebuilt_object;
static DuelCardDisplayObject*spawn(int id,int x,int y){(void)id;(void)x;(void)y;memset(&rebuilt_object,0,sizeof rebuilt_object);return &rebuilt_object;}
static void*resolve(const char*n,void*context){
 (void)context;void(*f)(void)=Mods_LibcLookup(n);if(f)return f;
 if(!strcmp(n,"Cards_Valid"))return valid;if(!strcmp(n,"Cards_BaseId"))return base_id;
 if(!strcmp(n,"Cards_Type"))return card_type;if(!strcmp(n,"Cards_Level"))return level;
 if(!strcmp(n,"Tables_FilterFusion"))return yes;if(!strcmp(n,"func_80024C1C"))return spawn;
 if(!strcmp(n,"CardRules_Fusion"))return table_fusion;
 if(!strncmp(n,"D_",2)||n[0]=='g')return datum(n);return stub(n);
}
static void log_message(const MemoriesModHost*h,const char*f,...){(void)h;(void)f;}
static int setting(const MemoriesModHost*h,const char*n,int fallback){(void)h;return setting_off?0:fallback;}
static int applied(const MemoriesModHost*h){(void)h;return active;}
static uint64_t now_us(const MemoriesModHost*h){(void)h;return 1000000;}
static FILE*asset(const MemoriesModHost*h,const char*n){
 (void)h;if(deny_assets)return 0;CHECK(n[0]!='/'&&!strstr(n,".."));char p[4096];CHECK(snprintf(p,sizeof p,"%s/%s",directory,n)<(int)sizeof p);return fopen(p,"rb");
}
static void*symbol(const MemoriesModHost*h,const char*n){(void)h;(void)n;return noop;}
static int hook(const MemoriesModHost*h,void*f,void*r,void**o){
 (void)h;CHECK(f&&r&&o&&hooks<64);targets[hooks]=f;replacements[hooks]=r;
 *o=f==resolve("Duel_CheckEquip",0)?(void*)original_equip:f==resolve("func_80024D34",0)?(void*)original_restore:f==resolve("DuelCard_RemoveFromField",0)?(void*)original_remove:(void*)noop;return ++hooks;
}
static int subscribe(const MemoriesModHost*h,unsigned ev,int priority,MemoriesModCallback f){(void)h;(void)priority;CHECK(f&&ev<MEMORIES_EVENT_COUNT);return ++subscriptions;}
static int provide(const MemoriesModHost*h,const char*n,void*p){(void)h;CHECK(providers<4&&p);provider_names[providers]=n;provided[providers++]=p;return 1;}
static int register_state(const MemoriesModHost*h,void*p,size_t n,unsigned v){(void)h;(void)p;(void)n;(void)v;++state_calls;return 1;}
static void*provider(const char*n){for(int i=0;i<providers;i++)if(!strcmp(n,provider_names[i]))return provided[i];return 0;}
static int ritual_enabled(int id){return ritual_mode==1&&id==704;}
static RitualEffectIntelligenceV1 ritual={1,ritual_enabled,NULL};
static int forecast_on(void){return ritual_mode==1;}
static void forecast_read(EaCard*c,int s,int v){(void)c;(void)s;(void)v;}
static int policy(int side,int kind){(void)side;(void)kind;return 0;}
static int pair(int a,int b,int active,int*r,int*o){(void)a;(void)b;(void)active;(void)r;(void)o;return 0;}
static EaProvider forecast={2,forecast_on,forecast_read,policy,pair};
static void*find(const MemoriesModHost*h,const char*n){
 (void)h;if(!strcmp(n,"ritual-card-effects-test:effect_intelligence_v1"))return ritual_mode?&ritual:0;
 if(!strcmp(n,"ritual-card-effects-test:effect_forecast_v2"))return ritual_mode?&forecast:0;
 if(!strcmp(n,"fusion-auxiliary-balance:effect_forecast_v2"))return provider("effect_forecast_v2");return 0;
}
static void*local(const char*n,size_t*size){
 for(size_t i=0;i<object.symbol_count;i++)if(!strcmp(n,object.symbols[i].name)){
  if(size)*size=object.symbols[i].size;return (void*)object.symbols[i].address;
 }return NULL;
}
static void*replacement(const char*n){void*t=resolve(n,0);for(int i=0;i<hooks;i++)if(targets[i]==t)return replacements[i];return 0;}
static int custom_effect(int id){return foreign_on&&id==850;}
static void contracts(MemoriesMod*mod){
 Encore*encore=provider("encore-v1");Unlock*u=provider("origin-unlock-v1");OriginEffectsV1*origin=provider("origin-effects-v1");EaProvider*ea=provider("effect_forecast_v2");
 CHECK(encore&&encore->api==1&&u&&u->api==1&&origin&&origin->abi==1&&ea&&ea->abi==2);
 DuelCardRecord*r=datum("D_801A7AD8");char identity,other;memset(r,0,30*sizeof(*r));
 CHECK(!encore->eligible(-1)&&!encore->eligible(2)&&!encore->eligible(0));
 r[5].card_id=495;r[5].flags=DUEL_CARD_FLAG_OCCUPIED;r[5].data=(void*)&identity;CHECK(encore->eligible(0));CHECK(!encore->eligible(1));
 r[5].flags|=DUEL_CARD_FLAG_FACE_DOWN;CHECK(!encore->eligible(0));r[5].flags=DUEL_CARD_FLAG_OCCUPIED;
 Lease*leases=local("aux_unlocks",NULL);CHECK(leases);leases[5]=(Lease){495,&identity,100};
 CHECK(u->lease(5)==100&&origin->lease(5)==100);r[6]=r[5];r[6].data=(void*)&other;CHECK(!u->lease(6));
 r[20]=r[5];r[5].data=(void*)&other;CHECK(u->lease(20)==100);u->revoke(20);CHECK(!origin->lease(20));
 memset(r,0,30*sizeof(*r));
 int(*equip)(int,int)=replacement("Duel_CheckEquip");CHECK(equip);
 ritual_mode=0;CHECK(!equip(318,704));ritual_mode=2;ritual.abi=1;CHECK(!equip(318,704));
 ritual_mode=1;ritual.abi=2;CHECK(!equip(318,704));ritual.abi=1;CHECK(equip(318,704)==704);
 ritual_mode=0;CHECK(!equip(318,850));CHECK(origin->register_provider("future-owner",custom_effect));CHECK(equip(318,850)==850);
 foreign_on=0;CHECK(!equip(318,850));foreign_on=1;CHECK(origin->register_provider("future-owner",NULL));CHECK(!equip(318,850));
 CHECK(!origin->register_provider(NULL,custom_effect));CHECK(!origin->register_provider("",custom_effect));
 CHECK(!origin->register_provider("012345678901234567890123456789012345678901234567890123456789012345",custom_effect));
 char name[32];for(int i=0;i<16;i++){snprintf(name,sizeof name,"owner-%d",i);CHECK(origin->register_provider(name,custom_effect));}
 CHECK(!origin->register_provider("overflow",custom_effect));CHECK(origin->register_provider("owner-0",custom_effect));
 for(int i=0;i<16;i++){snprintf(name,sizeof name,"owner-%d",i);CHECK(origin->register_provider(name,NULL));}
 active=1;mod->applied(1);CHECK(ea->on());int result=0,orig=0;
 CHECK(ea->pair(62,318,0,&result,&orig)&&result==63&&!orig);
 CHECK(ea->pair(61,658,0,&result,&orig)&&result==412&&orig==EA_METAL);
 EaCard c={0};c.id=189;ea->read(&c,-1,0);CHECK((c.possible&EA_FUSION)&&!(c.active&EA_FUSION));
 active=0;mod->applied(0);CHECK(!ea->on());CHECK(!ea->pair(62,318,0,&result,&orig));
 active=1;mod->applied(1);setting_off=1;CHECK(!ea->on()&&!equip(318,704)&&!u->lease(20));setting_off=0;
 void(*remove)(DuelCardRecord*)=replacement("DuelCard_RemoveFromField");CHECK(remove);
 r[5].card_id=r[6].card_id=495;r[5].data=(void*)&identity;r[6].data=(void*)&other;
 r[5].flags=r[6].flags=DUEL_CARD_FLAG_OCCUPIED;leases[5]=(Lease){495,&identity,200};leases[6]=(Lease){495,&other,201};
 remove(&r[5]);CHECK(!u->lease(5));CHECK(u->lease(6)==201);CHECK(!(r[5].flags&DUEL_CARD_FLAG_OCCUPIED));
 mod->reset();
}
static void optional_forecast(void){
 void(*play)(void)=replacement("AiScript_PlayFieldCard");void**rapi=local("ea_rapi",NULL);CHECK(play&&rapi);
 ritual_mode=0;play();CHECK(!*rapi);ritual_mode=2;forecast.abi=2;play();CHECK(!*rapi);
 ritual_mode=1;forecast.abi=1;play();CHECK(!*rapi);forecast.abi=2;play();CHECK(*rapi==&forecast);ritual_mode=0;
}
static void generated_restore(MemoriesMod*mod){
 DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord generated={0};generated.id=30;generated.deck_index=-1;
 void**leases=local("r2_zombie_lease",NULL);CHECK(leases);uint32_t*stats=datum("gDuel_adwCardStats");stats[29]=180|(120u<<9);
 void(*restore)(int,int)=replacement("func_80024D34");CHECK(restore);
 for(int side=0;side<2;side++){
  int slot=side?20:5,tagged=side?0x85:5;
  r[slot].card_id=30;r[slot].data=&generated;r[slot].flags=DUEL_CARD_FLAG_OCCUPIED|DUEL_CARD_FLAG_DEFENSE_POSITION;
  r[slot].stat_modifier=200;r[slot].defense_modifier=300;leases[slot]=&generated;
  restore(tagged,255);CHECK(restore_deck==0);CHECK(r[slot].card_id==30&&r[slot].data==&generated);
  CHECK(r[slot].attack==1800&&r[slot].defense==1200&&r[slot].stat_modifier==200&&r[slot].defense_modifier==300);
  CHECK(r[slot].object==&rebuilt_object&&r[slot].table_index==slot);
  CHECK(r[slot].flags&DUEL_CARD_FLAG_DEFENSE_POSITION);
 }
 mod->reset();memset(r,0,30*sizeof(*r));
}
static int off_contracts(MemoriesMod*mod){
 DuelCardRecord*r=datum("D_801A7AD8");Lease*leases=local("aux_unlocks",NULL);
 Encore*e=provider("encore-v1");Unlock*u=provider("origin-unlock-v1");OriginEffectsV1*o=provider("origin-effects-v1");EaProvider*f=provider("effect_forecast_v2");
 CHECK(e&&u&&o&&f&&leases);char identity;r[5].card_id=495;r[5].flags=DUEL_CARD_FLAG_OCCUPIED;r[5].data=(void*)&identity;leases[5]=(Lease){495,&identity,77};
 active=0;mod->applied(0);int encore=e->eligible(0);unsigned lease=u->lease(5),extended=o->lease(5);CHECK(!f->on());
 int failed=encore!=0||lease!=0||extended!=0;
 printf("OFF_CONTRACT: expected encore=0 lease=0 extended=0; observed encore=%d lease=%u extended=%u forecast=%d\n",encore,lease,extended,f->on());
 active=1;mod->applied(1);mod->reset();CHECK(!u->lease(5)&&!o->lease(5));CHECK(!state_calls);
 memset(r,0,30*sizeof(*r));return failed;
}
static void simulated_save_reset(MemoriesMod*mod){
 uint8_t*fused=local("fusion_origin",NULL),*metal=local("metal_immune",NULL);
 int16_t*ids=local("fusion_bound_id",NULL);void**bound=local("fusion_bound_data",NULL);
 DuelCardRecord*r=datum("D_801A7AD8");EaProvider*ea=provider("effect_forecast_v2");CHECK(fused&&metal&&ids&&bound&&ea);
 char identity;r[5].card_id=713;r[5].data=(void*)&identity;r[5].flags=DUEL_CARD_FLAG_OCCUPIED;
 fused[5]=1;metal[5]=1;ids[5]=713;bound[5]=&identity;EaCard c={0};c.id=713;ea->read(&c,5,0);CHECK(c.active&EA_FUSION);
 /* Simulated native-state round-trip is not the game's disk format. */
 uint8_t saved=fused[5];fused[5]=0;fused[5]=saved;CHECK(fused[5]==1);
 mod->reset();CHECK(!fused[5]&&!metal[5]);c=(EaCard){0};c.id=713;ea->read(&c,5,0);CHECK(!(c.active&EA_FUSION));
 CHECK(!state_calls);memset(r,0,30*sizeof(*r));
}
int main(int argc,char**argv){
 CHECK(argc==3||argc==4);int off_only=argc==4;directory=argv[2];stubs=mmap(0,4096,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(stubs!=MAP_FAILED);
 FILE*f=fopen(argv[1],"rb");CHECK(f);fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);void*bytes=malloc(n);CHECK(bytes&&fread(bytes,1,n,f)==(size_t)n);fclose(f);int known_off=0;
 for(int cycle=0;cycle<(off_only?1:12);cycle++){
  datas=hooks=subscriptions=providers=state_calls=deny_assets=no_provide=ritual_mode=setting_off=0;active=foreign_on=1;
  memset(data,0,sizeof data);memset(&object,0,sizeof object);char error[512];int loaded=ObjectLoader_Load(bytes,n,resolve,0,&object,error,sizeof error);if(loaded)fprintf(stderr,"%s\n",error);CHECK(!loaded);
  *(int*)datum("gCard_nCount")=900;
  int(*init)(const MemoriesModHost*,MemoriesMod*)=ObjectLoader_Symbol(&object,"MemoriesModInit");CHECK(init);
  MemoriesModHost host={0};MemoriesMod mod={0};host.api=24;host.symbol=symbol;host.log=log_message;host.setting=setting;
  host.applied=applied;host.now_us=now_us;host.open_asset=asset;host.hook=hook;host.subscribe=subscribe;
  host.provide=provide;host.find=find;host.register_state=register_state;
  if(cycle==10){deny_assets=1;}if(cycle==11)host.provide=NULL;
  int accepted=init(&host,&mod);
  if(cycle>=10){CHECK(!accepted);if(mod.shutdown)mod.shutdown();ObjectLoader_Free(&object);continue;}
  CHECK(accepted&&hooks==33&&subscriptions==4&&providers==4&&mod.api<=24&&mod.applied&&mod.reset&&mod.shutdown);
  if(!cycle)printf("object_hash=%08x declared_api=%u hooks=%d subscriptions=%d providers=%d\n",object.hash,mod.api,hooks,subscriptions,providers);
  if(!off_only){contracts(&mod);optional_forecast();generated_restore(&mod);simulated_save_reset(&mod);mod.frame();mod.overlay();(void)mod.overlay_signature();}
  else known_off=off_contracts(&mod);
  mod.reset();mod.applied(0);mod.applied(1);mod.shutdown();ObjectLoader_Free(&object);
 }
 free(bytes);munmap(stubs,4096);printf("PASS %u characterization assertions (synthetic host, no duel)\n",checks);
 return known_off?2:0;
}
