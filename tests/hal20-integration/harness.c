/* HAL-20 integration diagnostics. Real ELF32 objects + official loader,
 * hooks.c and events.c; synthetic engine/catalog/renderer. Never gameplay.
 * Production files and object bytes are not patched by this fixture.
 * Test-owned ABI-preserving trace thunks wrap host hook registrations.
 */
#define D_8009B360_AS_SIDE_ARRAY
#define AI_GET_HAND_SIZE_RETURNS_S32
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <unistd.h>
#include "pc/mods/modapi.h"
#include "pc/mods/object_loader.h"
#include "pc/mods/exports.h"
#include "pc/mods/hooks.h"
#include "pc/mods/events.h"
#include "pc/mods/mods.h"
#include "pc/mods/json.h"
#include "pc/cards/tables.h"
#include "game/ai.h"
#include "game/duel_card.h"
#include "game/duel_deck_card.h"
#include "game/duel_side_state.h"
#include "game/duel_hand.h"
#include "game/duel_card_display_state.h"
#include "game/duel_check_ritual.h"
#include "../../mods/ritual-duelists/recipe_adapter.h"
#include "../../mods/ritual-duelists/adaptive_rules.h"
#include "../../mods/fusion-auxiliary-balance/model.h"
#include "../../mods/fusion-auxiliary-balance/origin_effects_v1.h"
enum { M,H,D,F,R,RD,NMOD };
typedef struct {
 const char *id,*library;char directory[4096];int present,loaded,enabled;
 LoadedObject object;MemoriesModHost host;MemoriesMod mod;JsonDocument *manifest;
 void*state;size_t state_size;unsigned state_version;
} Module;
static Module modules[NMOD]={
 {.id="melhoria-dos-rituais",.library="rituals"},
 {.id="ai-hard-mode",.library="ai-hard-mode"},
 {.id="dual-play",.library="dual-play"},
 {.id="fusion-auxiliary-balance",.library="fusion-auxiliary-balance"},
 {.id="ritual-card-effects-test",.library="ritual-card-effects"},
 {.id="ritual-duelists",.library="ritual-duelists"}};
static unsigned assertions,failures;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FIXTURE_ASSERT %s:%d %s\n",__FILE__,__LINE__,#x);exit(90);}++assertions;}while(0)
static _Alignas(16) unsigned char data[160][65536];static char data_names[160][100];static int nd;
typedef struct {char name[100];void*entry;void*page;} Engine;
static Engine engine[256];static int ne;
typedef struct {int owner,event,token,kind;char name[100];void*replacement,*wrapper;void**previous;unsigned calls;} Registration;
static Registration registrations[256];static int nr;
static unsigned char*trace_memory;static size_t trace_used;
static char data_directory[]="/tmp/hal20-integration-XXXXXX";
static int overrides[NMOD][8],native_ai_result=1,native_ritual_result;
static const char*override_keys[]={"min_field","use_deck","min_hand","require_result","deck_access","deck_window","ai_planner","recipe_source"};
static int current_case,profile_hm,profile_dual,profile_order;
static const char *selector="all";
static const char *case_name="init";
static void *datum(const char*n){
 for(int i=0;i<nd;i++)if(!strcmp(data_names[i],n))return data[i];
 CHECK(nd<160&&strlen(n)<100);strcpy(data_names[nd],n);return data[nd++];
}
static __attribute__((force_align_arg_pointer,noinline)) void traced(int index){
 if(index>=0){CHECK(index<nr);++registrations[index].calls;
  printf("TRACE case=%s owner=%s kind=%s name=%s\n",case_name,modules[registrations[index].owner].id,registrations[index].kind?"event":"hook",registrations[index].name);
 }else{index=-index-1;CHECK(index<ne);if(!strncmp(engine[index].name,"Cards_",6)||!strncmp(engine[index].name,"Tables_",7))return;printf("TRACE case=%s owner=engine name=%s\n",case_name,engine[index].name);}
}
static void emit32(unsigned char**p,uint32_t n){memcpy(*p,&n,4);*p+=4;}
static void *thunk(void*destination,int trace_index,unsigned char*memory){
 unsigned char*p=memory;
 *p++=0x9c;*p++=0x60;*p++=0x68;emit32(&p,(uint32_t)trace_index);
 *p++=0xe8;emit32(&p,(uint32_t)((uintptr_t)traced-(uintptr_t)(p+4)));
 *p++=0x83;*p++=0xc4;*p++=4;*p++=0x61;*p++=0x9d;
 *p++=0xe9;emit32(&p,(uint32_t)((uintptr_t)destination-(uintptr_t)(p+4)));
 return memory;
}
static void *wrap(void*p,int index){CHECK(trace_used+64<65536);void*r=thunk(p,index,trace_memory+trace_used);trace_used+=64;return r;}
static uint16_t *banks[16];
static uint16_t*gpu_bank(int n){CHECK(n>=0&&n<16);if(!banks[n])banks[n]=calloc(1024*512,2);return banks[n];}
static int zero(void){return 0;}
static int one(void){return 1;}
static int stat_cap(int side){(void)side;return 9999;}
static int valid(int id){return id>0&&id<=900;}
static int base(int id){return id;}
static int card_type(int id){
 if(id>=665&&id<=700||id==721)return 22;
 if(id==658)return 23;if(id==318||id==337)return 20;
 return id==400||id==401||id==718?CARD_TYPE_FISH:0;
}
static int level(int id){(void)id;return 4;}
static int yes2(int a,int b){(void)a;(void)b;return 1;}
static int native_window(void){return 5;}
static int native_ai(void){return native_ai_result;}
static int native_stats(DuelCardRecord*c){(void)c;return (1000<<16)|1000;}
static int native_fusion(int a,int b){return (a==61&&b==658)||(a==658&&b==61)?412:42;}
static int table_fusion(int a,int b){return (a==61&&b==658)||(a==658&&b==61)?412:0;}
/* Synthetic table includes the approved Fusion Egotist/Skull Guardian pair;
 * the real table parser/catalog is a separate test, not claimed here. */
static int native_equip(int a,int b){return a==318&&b==704?704:0;}
static int no_table(int n,void*p){(void)n;(void)p;return 0;}
static int no_legacy(int n,void*p){(void)n;(void)p;return -1;}
static DuelCardDisplayObject displays[80];static int display_count;
static void *spawn(int id,int x,int y){(void)id;CHECK(display_count<80);DuelCardDisplayObject*p=&displays[display_count++];memset(p,0,sizeof(*p));p->out_x=x;p->out_y=y;return p;}
static void *spawn_hand(DuelCardRecord*r,int x,int y){return spawn(r?r->card_id:0,x,y);}
static void *setup_record(int r,int pos){
 CHECK(r>=0&&r<30&&pos>=0&&pos<80);DuelCardRecord*c=datum("D_801A7AD8");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");
 c[r].card_id=d[pos].id;c[r].data=&d[pos];c[r].flags=DUEL_CARD_FLAG_OCCUPIED;return &c[r];
}
static void deactivate(DuelCardRecord*r){if(r)r->flags=0;}
static void native_restore(int tagged,int deck){
 (void)deck;int slot=tagged&128?(tagged&127)+15:tagged;CHECK(slot>=0&&slot<30);DuelCardRecord*r=datum("D_801A7AD8");memset(&r[slot],0,sizeof r[slot]);r[slot].card_id=1;
}
static void native_ritual(void){*(uint16_t*)datum("D_8009B210")=4;}
static int native_check(DuelRitualResult*out,int magic){
 DuelCardRecord*r=datum("D_801A7AD8");int result=native_ritual_result,chosen[3]={-1,-1,-1};
 if(result<0){
  result=0;uint16_t*row=datum("gDuel_awRitualData");int side=*(uint8_t*)datum("D_8009B1D5");
  for(int n=0;n<150&&row[0];n++,row+=5)if(row[0]==magic){
   for(int role=0;role<3;role++)for(int i=side*15+5;i<side*15+10;i++){
    if(r[i].card_id!=row[role+1]||!(r[i].flags&DUEL_CARD_FLAG_OCCUPIED))continue;
    int used=0;for(int j=0;j<role;j++)used|=chosen[j]==i;if(!used){chosen[role]=i;break;}
   }
   if(chosen[0]>=0&&chosen[1]>=0&&chosen[2]>=0)result=row[4];break;
  }
 }
 if(result&&out){for(int i=0;i<3;i++)out->tribute_objects[i]=r[chosen[i]>=0?chosen[i]:5+i].object;out->field_0C=0;}return result;
}
static unsigned rng_state=42;
static int random_interval(int n){rng_state=rng_state*1664525u+1013904223u;return n>0?(rng_state>>1)%(unsigned)n:0;}
static int named(const char*n){
 const char*names[]={"Simon Muran","Teana","Jono","Villager 1","Villager 2","Villager 3","Seto","Heishin","Rex Raptor","Weevil Underwood","Mai Valentine","Bandit Keith","Shadi","Yami Bakura","Pegasus","Isis","Kaiba","Mage Soldier","Jono 2nd","Teana 2nd","Ocean Mage","High Mage Secmeton","Forest Mage","High Mage Anubisius","Mountain Mage","High Mage Atenza","Desert Mage","High Mage Martis","Meadow Mage","High Mage Kepura","Labyrinth Mage","Seto 2nd","Guardian Sebek","Guardian Neku","Heishin 2nd","Seto 3rd","DarkNite","Nitemare"};
 for(int i=0;i<38;i++)if(!strcmp(n,names[i]))return i+1;return 0;
}
static const char*name_of(int id){(void)id;return "Synthetic duelist";}
static void *body(const char*n){
 if(!strcmp(n,"SoftGpu_Bank"))return gpu_bank;
 if(!strcmp(n,"Cards_Valid"))return valid;if(!strcmp(n,"Cards_BaseId")||!strcmp(n,"Cards_EffectId"))return base;
 if(!strcmp(n,"Cards_Type")||!strcmp(n,"Cards_RetailType"))return card_type;if(!strcmp(n,"Cards_Level"))return level;
 if(!strcmp(n,"Duelists_Named"))return named;if(!strcmp(n,"Duelists_Name"))return name_of;
 if(!strcmp(n,"Rand_GetInterval"))return random_interval;
 if(!strcmp(n,"Tables_StatCap"))return stat_cap;
 if(!strcmp(n,"Ai_GetHandSize"))return native_window;if(!strcmp(n,"AiScript_Run"))return native_ai;
 if(!strcmp(n,"Duel_CheckFusion"))return native_fusion;if(!strcmp(n,"CardRules_Fusion"))return table_fusion;
 if(!strcmp(n,"Duel_CheckEquip"))return native_equip;if(!strcmp(n,"Tables_FilterFusion"))return one;
 if(!strcmp(n,"Duel_CalcCardStats"))return native_stats;
 if(!strcmp(n,"Tables_Ritual"))return no_legacy;
 if(!strcmp(n,"Tables_RitualRequirements")||!strcmp(n,"Tables_RitualRule"))return no_table;
 if(!strcmp(n,"Duel_CheckRitual"))return native_check;if(!strcmp(n,"DuelEffect_ApplyRitual"))return native_ritual;
 if(!strcmp(n,"Duel_SetupCardRecord"))return setup_record;
 if(!strcmp(n,"func_80024C1C"))return spawn;if(!strcmp(n,"func_80018004"))return spawn_hand;
 if(!strcmp(n,"DuelCard_DeactivateRecord")||!strcmp(n,"DuelCard_RemoveFromField"))return deactivate;
 if(!strcmp(n,"func_80024D34"))return native_restore;
 return zero;
}
static void *engine_function(const char*n){
 for(int i=0;i<ne;i++)if(!strcmp(n,engine[i].name))return engine[i].entry;
 CHECK(ne<256&&strlen(n)<100);int i=ne++;strcpy(engine[i].name,n);
 unsigned char*p=mmap(NULL,4096,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(p!=MAP_FAILED);
 memset(p,0x90,24);engine[i].page=p;engine[i].entry=p+16;thunk(body(n),-i-1,p+18);return p+16;
}
static void *resolve(const char*n,void*ctx){
 (void)ctx;void(*lib)(void)=Mods_LibcLookup(n);if(lib)return lib;
 if(!strncmp(n,"D_",2)||n[0]=='g')return datum(n);return engine_function(n);
}
int Mods_Count(void){return NMOD;}
const char*Mods_Id(int i){return i>=0&&i<NMOD?modules[i].id:"";}
int Mods_Active(int i){return i>=0&&i<NMOD&&modules[i].loaded&&modules[i].enabled;}
int Mods_SettingKeyValid(const char*n){return n&&*n&&strlen(n)<64;}
long Mods_Limit(const char*n,long fallback){(void)n;return fallback;}
static int owner(const MemoriesModHost*h){return (int)(uintptr_t)h->reserved;}
static int active(const MemoriesModHost*h){return modules[owner(h)].enabled;}
static void host_log(const MemoriesModHost*h,const char*f,...){
 printf("MODLOG owner=%s ",h->id);va_list args;va_start(args,f);vprintf(f,args);va_end(args);putchar('\n');
}
static void *symbol(const MemoriesModHost*h,const char*n){(void)h;return resolve(n,NULL);}
static int setting(const MemoriesModHost*h,const char*n,int fallback){
 int m=owner(h);for(int k=0;k<8;k++)if(!strcmp(n,override_keys[k])&&overrides[m][k]!=-999)return overrides[m][k];
 if(!strcmp(n,"debug_overlay")||!strcmp(n,"diagnostics"))return 0;
 const JsonValue*settings=Json_Member(Json_Root(modules[m].manifest),"settings");
 for(int i=0;i<Json_Count(settings);i++){const JsonValue*s=Json_At(settings,i);if(!strcmp(Json_String(Json_Member(s,"key"),""),n)){
  const JsonValue*v=Json_Member(s,"default");return Json_TypeOf(v)==JSON_BOOL?Json_Bool(v,fallback):Json_Number(v,fallback);
 }}return fallback;
}
static void set_setting(const MemoriesModHost*h,const char*n,int v){host_log(h,"TEST ignored setting request %s=%d",n,v);}
static FILE *asset(const MemoriesModHost*h,const char*n){char p[4096];CHECK(n[0]!='/'&&!strstr(n,".."));snprintf(p,sizeof p,"%s/%s",h->directory,n);return fopen(p,"rb");}
static FILE *open_data(const MemoriesModHost*h,const char*n,const char*mode){char p[4096];CHECK(!strstr(n,"..")&&n[0]!='/');snprintf(p,sizeof p,"%s/%s-%s",data_directory,h->id,n);return fopen(p,mode);}
static unsigned short pad(const MemoriesModHost*h,int port){(void)h;(void)port;return 0;}
static uint64_t now(const MemoriesModHost*h){(void)h;return 1000000;}
static const char*target_name(void*f){for(int i=0;i<ne;i++)if(engine[i].entry==f)return engine[i].name;return "unknown-target";}
static int hook(const MemoriesModHost*h,void*f,void*r,void**previous){
 CHECK(nr<256&&f&&r&&previous);int i=nr++;Registration*q=&registrations[i];q->owner=owner(h);q->replacement=r;q->previous=previous;snprintf(q->name,sizeof q->name,"%s",target_name(f));
 q->wrapper=wrap(r,i);int token=Hooks_Add(q->owner,f,q->wrapper,previous);q->token=token;
 printf("REGISTER hook owner=%s target=%s token=%d\n",h->id,q->name,token);return token;
}
static void unhook(const MemoriesModHost*h,int token){Hooks_Remove(owner(h),token);}
static int subscribe(const MemoriesModHost*h,unsigned event,int priority,MemoriesModCallback cb){
 CHECK(nr<256);int i=nr++;Registration*q=&registrations[i];q->owner=owner(h);q->event=event;q->kind=1;snprintf(q->name,sizeof q->name,"event-%u-priority-%d",event,priority);
 return Mods_Subscribe(owner(h),event,priority,wrap(cb,i));
}
static void unsubscribe(const MemoriesModHost*h,int token){Mods_Unsubscribe(owner(h),token);}
static int reg_state(const MemoriesModHost*h,void*p,size_t n,unsigned version){
 Module*m=&modules[owner(h)];m->state=p;m->state_size=n;m->state_version=version;
 printf("STATE owner=%s bytes=%zu version=%u\n",h->id,n,version);return Mods_RegisterState(owner(h),p,n,version);
}
static int provide(const MemoriesModHost*h,const char*n,void*p){printf("PROVIDER owner=%s name=%s\n",h->id,n);return Mods_Provide(owner(h),n,p);}
static void *find(const MemoriesModHost*h,const char*n){(void)h;return Mods_Find(n);}
static void toggle(int m,int on){if(!modules[m].loaded)return;modules[m].enabled=on;Hooks_Relink();if(modules[m].mod.applied)modules[m].mod.applied(on);printf("TOGGLE owner=%s on=%d\n",modules[m].id,on);}
static void *local(int m,const char*n,size_t*size){
 for(size_t i=0;i<modules[m].object.symbol_count;i++){struct ObjectSymbol*s=&modules[m].object.symbols[i];if(!strcmp(n,s->name)){if(size)*size=s->size;return (void*)s->address;}}return NULL;
}
static void begin(const char*n){case_name=n;++current_case;printf("CASE %s\n",n);for(int i=0;i<nr;i++)registrations[i].calls=0;}
static unsigned calls(int m,const char*n){unsigned result=0;for(int i=0;i<nr;i++)if(registrations[i].owner==m&&!strcmp(registrations[i].name,n))result+=registrations[i].calls;return result;}
static void result(const char*n,int pass,const char*details){printf("RESULT %s %s %s\n",n,pass?"PASS":"FAIL",details);if(!pass)++failures;}
static void init_module(int m,const char*directory){
 Module*q=&modules[m];snprintf(q->directory,sizeof q->directory,"%s",directory);char path[4096],error[512];
 snprintf(path,sizeof path,"%s/mod.json",directory);q->manifest=Json_ParseFile(path,error,sizeof error);CHECK(q->manifest);
 CHECK(!strcmp(Json_String(Json_Member(Json_Root(q->manifest),"id"),""),q->id));
 snprintf(path,sizeof path,"%s/%s.o",directory,q->library);FILE*f=fopen(path,"rb");CHECK(f);fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);void*bytes=malloc(n);CHECK(bytes&&fread(bytes,1,n,f)==(size_t)n);fclose(f);
 int loaded=ObjectLoader_Load(bytes,n,resolve,NULL,&q->object,error,sizeof error);if(loaded)fprintf(stderr,"LOAD %s %s\n",q->id,error);CHECK(!loaded);free(bytes);
 q->host=(MemoriesModHost){0};q->host.api=24;q->host.id=q->id;q->host.directory=q->directory;q->host.reserved=(void*)(uintptr_t)m;
 q->host.applied=active;q->host.log=host_log;q->host.symbol=symbol;q->host.setting=setting;q->host.set_setting=set_setting;q->host.pad=pad;q->host.now_us=now;
 q->host.open_asset=asset;q->host.open_data=open_data;q->host.hook=hook;q->host.unhook=unhook;q->host.subscribe=subscribe;q->host.unsubscribe=unsubscribe;q->host.register_state=reg_state;q->host.provide=provide;q->host.find=find;
 int(*init)(const MemoriesModHost*,MemoriesMod*)=ObjectLoader_Symbol(&q->object,"MemoriesModInit");CHECK(init);int accepted=init(&q->host,&q->mod);if(!accepted)fprintf(stderr,"INIT FAIL %s\n",q->id);CHECK(accepted);
 q->loaded=1;Hooks_Relink();if(q->mod.applied)q->mod.applied(q->enabled);
 printf("LOADED owner=%s api=%u hash=%08x enabled=%d\n",q->id,q->mod.api,q->object.hash,q->enabled);
}
static void clear_engine(void){
 memset(data,0,sizeof data);memset(displays,0,sizeof displays);display_count=0;native_ritual_result=-1;
 *(int*)datum("gCard_nCount")=900;memset(datum("D_800907D8"),255,40);
 DuelSideState*s=datum("D_800E9FF0");for(int side=0;side<2;side++){memset(s[side].hand,-1,5);s[side].life_points.unsigned_value=8000;}
 for(int i=0;i<10;i++)((uint8_t*)datum("D_800907CC"))[i]=i<5?i:15+i-5;
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){d[i].id=99;d[i].deck_index=i;d[i].data_block_index=i;}
 uint32_t*stats=datum("gDuel_adwCardStats");for(int i=0;i<900;i++)stats[i]=100|(100u<<9)|((unsigned)card_type(i+1)<<CARD_STAT_TYPE_SHIFT);
 *(DuelSideState**)datum("D_8009B1C8")=&s[0];
 uint16_t row[]={666,29,31,101,357,679,535,555,556,704,680,6,47,571,705,0};memcpy(datum("gDuel_awRitualData"),row,sizeof row);
 stats[356]=200|(200u<<9);for(int m=0;m<NMOD;m++)if(modules[m].loaded&&modules[m].mod.reset)modules[m].mod.reset();
}
static void diagnostic_cases(void);
int main(int argc,char**argv){
 CHECK(argc==9);setvbuf(stdout,NULL,_IOLBF,0);CHECK(mkdtemp(data_directory));
 trace_memory=mmap(NULL,65536,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(trace_memory!=MAP_FAILED);
 for(int i=0;i<NMOD;i++)for(int k=0;k<8;k++)overrides[i][k]=-999;
 int stage=atoi(argv[4]),hm=atoi(argv[5]),dual=atoi(argv[6]),order=atoi(argv[7]);selector=argv[8];profile_hm=hm;profile_dual=dual;profile_order=order;
 modules[M].present=modules[H].present=modules[D].present=1;modules[R].present=stage>=2;modules[F].present=stage>=3;modules[RD].present=stage>=4;
 for(int i=0;i<NMOD;i++)modules[i].enabled=1;modules[H].enabled=hm;modules[D].enabled=dual;
 int normal[]={M,H,D,F,R,RD},reverse[]={M,H,F,R,RD,D},hm_last[]={M,D,F,RD,H,R};int*sequence=order==1?reverse:order==2?hm_last:normal;
 printf("PROFILE stage=%d hm=%d dual=%d order=%d drop-missing-cards=ON-in-profile (table execution separate)\n",stage,hm,dual,order);
 *(int*)datum("gCard_nCount")=900;
 for(int i=0;i<NMOD;i++){int m=sequence[i];if(!modules[m].present)continue;char path[4096];
  if(m==H)snprintf(path,sizeof path,"%s",argv[2]);else if(m==D)snprintf(path,sizeof path,"%s",argv[3]);
  else snprintf(path,sizeof path,"%s/mods/%s",argv[1],m==R?"ritual-card-effects":modules[m].id);
  init_module(m,path);
 }
 clear_engine();diagnostic_cases();
 for(int i=NMOD-1;i>=0;i--){int m=sequence[i];if(!modules[m].loaded)continue;if(modules[m].mod.shutdown)modules[m].mod.shutdown();Mods_ClearHooks(m);modules[m].loaded=0;ObjectLoader_Free(&modules[m].object);Json_Free(modules[m].manifest);}
 printf("SUMMARY assertions=%u cases=%d failures=%u (synthetic engine; no gameplay)\n",assertions,current_case,failures);return failures?2:0;
}
#include "cases.inc"
