/* HAL-22, derived from PR #14 own test infrastructure. Actual ELF32 AI/Dual,
 * selected native controller bodies, official loader/hooks/events. Synthetic RAM,
 * catalog/rendering/effect bridge. Never a Windows gameplay certification.
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
#include "game/duel_selection_layout.h"
#include "game/duel_card_pick_cursor.h"
#include "game/display_object.h"
enum { H,D,NMOD };
typedef struct {
 const char *id,*library;char directory[4096];int present,loaded,enabled;
 LoadedObject object;MemoriesModHost host;MemoriesMod mod;JsonDocument *manifest;
 void*state;size_t state_size;unsigned state_version;
} Module;
static Module modules[NMOD]={
 {.id="ai-hard-mode",.library="ai-hard-mode"},
 {.id="dual-play",.library="dual-play"}};
static unsigned assertions,failures;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FIXTURE_ASSERT %s:%d %s\n",__FILE__,__LINE__,#x);exit(90);}++assertions;}while(0)
static _Alignas(16) unsigned char data[160][65536];static char data_names[160][100];static int nd;
typedef struct {char name[100];void*entry;void*page;} Engine;
static Engine engine[256];static int ne;
typedef struct {int owner,event,token,kind;char name[100];void*replacement,*wrapper;void**previous;unsigned calls;} Registration;
static Registration registrations[256];static int nr;
static unsigned char*trace_memory;static size_t trace_used;
static char data_directory[]="/tmp/hal22-XXXXXX";
static int native_ai_result=0;
static int current_case,profile_order;
static const char *selector="all";
static unsigned engine_calls[256], setup_count, spawn_count;
static int snapshots, masked_ok, observed_window;
static char swap_path[256];
static const char *case_name="init";
typedef struct {char name[100];unsigned address;} Address;
static Address addresses[16000];static int na;
static void *datum(const char*n){
 for(int i=0;i<na;i++)if(!strcmp(addresses[i].name,n))return (void*)(uintptr_t)addresses[i].address;
 if(!strncmp(n,"D_",2)){unsigned a;if(sscanf(n+2,"%8x",&a)==1&&a>=0x80000000&&a<0x80200000)return(void*)(uintptr_t)a;}
 for(int i=0;i<nd;i++)if(!strcmp(data_names[i],n))return data[i];
 CHECK(nd<160&&strlen(n)<100);strcpy(data_names[nd],n);printf("MODEL_DATA %s\n",n);return data[nd++];
}
static void memory_init(const char*root){
 void*p=mmap((void*)0x80000000,0x200000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);CHECK(p==(void*)0x80000000);
 char path[4096],line[300],n[100];unsigned a;snprintf(path,sizeof path,"%s/config/pc/guest_addresses.txt",root);FILE*f=fopen(path,"r");CHECK(f);
 while(fgets(line,sizeof line,f))if(sscanf(line,"%99s %x",n,&a)==2&&a>=0x80000000&&a<0x80200000){CHECK(na<16000);strcpy(addresses[na].name,n);addresses[na++].address=a;}fclose(f);
}
static __attribute__((force_align_arg_pointer,noinline)) void traced(int index){
 if(index>=0){CHECK(index<nr);++registrations[index].calls;
  if(!strcmp(registrations[index].name,"func_8001BAF0")){strcat(swap_path,modules[registrations[index].owner].id);strcat(swap_path,">");}
  printf("TRACE case=%s owner=%s kind=%s name=%s\n",case_name,modules[registrations[index].owner].id,registrations[index].kind?"event":"hook",registrations[index].name);
 }else{index=-index-1;CHECK(index<ne);++engine_calls[index];if(!strcmp(engine[index].name,"func_8001BAF0"))strcat(swap_path,"engine>");if(!strncmp(engine[index].name,"Cards_",6)||!strncmp(engine[index].name,"Tables_",7))return;printf("TRACE case=%s owner=engine name=%s\n",case_name,engine[index].name);}
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
static int card_type(int id){return id>0?((uint32_t*)datum("gDuel_adwCardStats"))[id-1]>>CARD_STAT_TYPE_SHIFT&CARD_STAT_TYPE_MASK:0;}
static int level(int id){(void)id;return 4;}
static int native_ai(void){return native_ai_result;}
static int native_stats(DuelCardRecord*c){uint32_t n=((uint32_t*)datum("gDuel_adwCardStats"))[c->card_id-1];return (n&511)*10|((n>>9&511)*10)<<16;}
static int no_table(int n,void*p){(void)n;(void)p;return 0;}
static long table_value(int which,long fallback){(void)which;return fallback;}
static int no_legacy(int n,void*p){(void)n;(void)p;return -1;}
static _Alignas(16) unsigned char displays[512][512];static int display_count;
static void *allocate(int slot,int tag){(void)slot;(void)tag;CHECK(display_count<512);void*p=displays[display_count++];memset(p,0,512);return p;}
static void *spawn(int id,int x,int y){++spawn_count;DuelCardDisplayObject*p=allocate(0,0);p->out_x=x;p->out_y=y;p->field_68=card_type(id);return p;}
static void *spawn_hand(DuelCardRecord*r,int x,int y){DuelCardDisplayObject*p=spawn(r->card_id,x,y);p->card_index=r-(DuelCardRecord*)datum("D_801A7AD8");r->object=(void*)p;return p;}
static void *setup_record(int r,int pos){
 ++setup_count;CHECK(r>=0&&r<30&&pos>=0&&pos<80);DuelCardRecord*c=datum("D_801A7AD8");DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");
 memset(&c[r],0,sizeof c[r]);c[r].card_id=d[pos].id;c[r].data=&d[pos];c[r].flags=DUEL_CARD_FLAG_OCCUPIED;return &c[r];
}
static void deactivate(DuelCardRecord*r){if(r)r->flags=0;}
static void *ai_row(int id){return (unsigned char*)datum("gDuel_aOpponentData")+id*9;}
static int field_step(DuelCardPickCursor*c,int direction){(void)direction;c->col=((signed char*)datum("D_8009B208"))[2];return 0;}
static unsigned rng_state=42;
static int random_interval(int n){rng_state=rng_state*1664525u+1013904223u;return n>0?(rng_state>>1)%(unsigned)n:0;}
static void *body(const char*n){
 if(!strcmp(n,"SoftGpu_Bank"))return gpu_bank;
 if(!strcmp(n,"Cards_Valid"))return valid;if(!strcmp(n,"Cards_BaseId")||!strcmp(n,"Cards_EffectId")||!strcmp(n,"Cards_AiId"))return base;
 if(!strcmp(n,"Cards_Type")||!strcmp(n,"Cards_RetailType"))return card_type;if(!strcmp(n,"Cards_Level"))return level;
 if(!strcmp(n,"Duelists_AiRow"))return ai_row;
 if(!strcmp(n,"Rand_GetInterval"))return random_interval;
 if(!strcmp(n,"Tables_StatCap"))return stat_cap;if(!strcmp(n,"Tables_Value"))return table_value;
 if(!strcmp(n,"AiScript_Run"))return native_ai;
 if(!strcmp(n,"Duel_CalcCardStats"))return native_stats;
 if(!strcmp(n,"Tables_Ritual"))return no_legacy;
 if(!strcmp(n,"Tables_RitualRequirements")||!strcmp(n,"Tables_RitualRule"))return no_table;
 if(!strcmp(n,"Duel_SetupCardRecord"))return setup_record;
 if(!strcmp(n,"func_80024C1C"))return spawn;if(!strcmp(n,"func_80018004")||!strcmp(n,"func_80017F04"))return spawn_hand;
 if(!strcmp(n,"DuelCard_DeactivateRecord")||!strcmp(n,"DuelCard_RemoveFromField"))return deactivate;
 if(!strcmp(n,"DisplayObject_AcquireSlot"))return allocate;
 if(!strcmp(n,"func_80024088"))return field_step;
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
 int m=owner(h);
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

static LoadedObject native_objects[16];static int nn;
static void native_load(const char*directory,const char*unit,const char*exports){
 char path[4096],error[512],names[1024];snprintf(path,sizeof path,"%s/src_game_%s.c.o",directory,unit);FILE*f=fopen(path,"rb");CHECK(f);fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);void*bytes=malloc(n);CHECK(bytes&&fread(bytes,1,n,f)==(size_t)n);fclose(f);
 CHECK(nn<16);LoadedObject*q=&native_objects[nn++];int rc=ObjectLoader_Load(bytes,n,resolve,NULL,q,error,sizeof error);if(rc)fprintf(stderr,"NATIVE_LOAD %s %s\n",unit,error);CHECK(!rc);free(bytes);
 snprintf(names,sizeof names,"%s",exports);for(char*name=strtok(names,",");name;name=strtok(NULL,",")){
  void*p=ObjectLoader_Symbol(q,name);CHECK(p);void*entry=engine_function(name);int index=0;while(engine[index].entry!=entry)index++;
  CHECK(!mprotect(engine[index].page,4096,PROT_READ|PROT_WRITE|PROT_EXEC));thunk(p,-index-1,(unsigned char*)entry+2);printf("NATIVE_BIND %s unit=%s\n",name,unit);
 }
 for(size_t i=0;i<q->symbol_count;i++){struct ObjectSymbol*v=&q->symbols[i];if(v->global&&!v->function&&v->size&&v->name[0]!='.'){void*d=datum(v->name);memcpy(d,(void*)v->address,v->size);printf("NATIVE_DATA %s size=%u\n",v->name,(unsigned)v->size);}}
}
static unsigned char*ds(void){CHECK(modules[D].state_size==50&&modules[D].state_version==10);return modules[D].state;}
static void show_state(const char*where){unsigned char*p=ds();printf("LEDGER %s used=%u pending=%u bonus=%u stage=%u cpu=%u last_plan=%u remaining=%u rejected=%u\n",where,p[1],p[3],p[5],p[9],p[13],p[15],p[17],p[21]);}
static unsigned ecalls(const char*n){for(int i=0;i<ne;i++)if(!strcmp(n,engine[i].name))return engine_calls[i];return 0;}
static void set_stat(int id,int attack,int defense,int type){((uint32_t*)datum("gDuel_adwCardStats"))[id-1]=attack/10|((unsigned)(defense/10)<<9)|((unsigned)type<<CARD_STAT_TYPE_SHIFT);}
static void scenario_init(int variant){
 memset((void*)0x8009B000,0,0x400);memset((void*)0x800E9000,0,0x3000);memset((void*)0x801A7000,0,0x8000);memset(data,0,sizeof data);display_count=0;setup_count=spawn_count=0;
 // Do not clear initialized grid/opponent tables. Actual CPU identity aliases byte 1.
 *(int*)datum("gCard_nCount")=722;*(signed char*)datum("D_8009B360")=-1;*(signed char*)datum("gDuel_bOpponentID")=9;*(uint8_t*)datum("D_8009B1D5")=1;
 DuelSideState*s=datum("D_800E9FF0");for(int i=0;i<2;i++){memset(s[i].hand,-1,5);s[i].max_life_points=8000;s[i].life_points.unsigned_value=8000;s[i].deck_draw_cursor=10;}*(DuelSideState**)datum("D_8009B1C8")=&s[1];
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){d[i].id=100+i;d[i].deck_index=i;d[i].data_block_index=i;set_stat(100+i,500,500,0);}
 set_stat(24,300,200,3);set_stat(35,2500,2100,0);set_stat(22,2500,1200,3);set_stat(343,0,0,20);set_stat(344,0,0,20);set_stat(666,0,0,22);
 int hand[5]={24,344,343,110,666};if(variant==1){hand[0]=344;hand[1]=35;hand[2]=24;hand[3]=110;hand[4]=666;}
 if(variant==2){hand[0]=35;}if(variant==3){hand[0]=344;hand[1]=24;}
 if(variant==4){hand[0]=24;hand[1]=110;hand[2]=111;hand[3]=112;hand[4]=666;d[50].id=35;}
 for(int i=0;i<5;i++){d[40+i].id=hand[i];s[1].hand[i]=40+i;DuelCardRecord*r=setup_record(15+i,40+i);((DuelHandSlot*)datum("D_800EA030"))[i].object=spawn_hand(r,14+i*60,194);}
 // Strong occupied monster row makes a support spell preferable to replacement.
 for(int i=0;i<5;i++){d[45+i].id=(variant==1||variant==2||variant==4)?145+i:i<3?35:22;DuelCardRecord*r=setup_record(20+i,45+i);r->object=spawn_hand(r,0,0);}
 if(variant==1||variant==2||variant==4){DuelCardRecord*r=datum("D_801A7AD8");for(int i=20;i<25;i++)r[i].flags=0;}
 ((void(*)(void))engine_function("Duel_InitScene"))(); // Original body model: zero; real Dual hook resets its state.
 *(uint16_t*)datum("gDuel_wSceneStateFlags")=4;
 memset(engine_calls,0,sizeof engine_calls);for(int i=0;i<nr;i++)registrations[i].calls=0;setup_count=spawn_count=snapshots=0;masked_ok=1;swap_path[0]=0;
}
static int hand_action(int maxframes){
 for(int frame=0;frame<maxframes;frame++){
  *(uint8_t*)datum("D_8009B162")=0; // Rendering/animation completion model, no card/quota/selection edits.
  ((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();
  unsigned phase=*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff;
  if(phase==4&&(*(uint16_t*)datum("D_8009B20C"))==0x8000){
   ++snapshots;observed_window=((int(*)(void))engine_function("Ai_GetHandSize"))();
   if(ds()[5]){AiActiveCard*a=datum("gDuel_aActiveCards");printf("BONUS_SNAPSHOT window=%d used=%u ids=",observed_window,ds()[1]);
    for(int i=0;i<observed_window;i++){printf("%s%d",i?",":"",a[11+i].card_id);if(a[11+i].card_id){int t=card_type(a[11+i].card_id),c=t<20?1:2;if(t==22||(c&ds()[1]))masked_ok=0;}}puts("");
   }
  }
  if(frame<5||phase!=4)printf("STEP frame=%d phase=%u mode=%u substate=%u selection=%u,%u cursor=%d\n",frame,phase,*(uint8_t*)datum("D_8009B174"),*(uint16_t*)datum("D_8009B20C"),((uint8_t*)datum("D_800EAE88"))[0],((uint8_t*)datum("D_800EAE88"))[1],((DuelSelectionRecord*)datum("D_800E9F10"))[1].field_0E);
  if(phase!=4)return phase;
 }
 return 4;
}
static int award_bonus(void){
 // Effect/placement resolution and field presentation are outside this fixture.
 *(uint16_t*)datum("gDuel_wSceneStateFlags")=0x8005;*(uint8_t*)datum("D_8009B174")=3;
 for(int i=0;i<8;i++){*(uint8_t*)datum("D_8009B162")=0;if(modules[D].mod.frame)modules[D].mod.frame();show_state("award-frame");if((*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff)==4)return 1;}
 return 0;
}
static int hand_count(void){int n=0;DuelSideState*s=datum("D_800E9FF0");for(int i=0;i<5;i++)n+=s[1].hand[i]>=0;return n;}
static DisplayObject* hand_action_object(void){DisplayObject**o=datum("D_800E9EF0");for(int i=0;i<7;i++)if(o[i]&&o[i]->field_6A>=15&&o[i]->field_6A<20)return o[i];return NULL;}
static int action_id(void){DisplayObject*o=hand_action_object();return o?((DuelCardRecord*)datum("D_801A7AD8"))[o->field_6A].card_id:0;}
static int action_mask(void){int id=action_id();return id?card_type(id)<20?1:2:0;}
static void resolve_action_model(int phase){
 // Only bridge simple placement/direct-damage to field; no native effect/battle execution claimed.
 if(phase==6){int id=action_id();if(id==344||id==343){DuelSideState*s=datum("D_800E9FF0");s[0].life_points.unsigned_value-=id==344?100:50;}}
 else if(phase==7){int target=*(uint8_t*)datum("D_8009B19C");DisplayObject*o=hand_action_object();if(o){DuelCardRecord*r=datum("D_801A7AD8");int origin=o->field_6A;r[target]=r[origin];r[target].object=o;o->field_6A=target;}}
}
static void natural_case(int variant,const char*name){
 begin(name);scenario_init(variant);int phase=hand_action(120),first=action_mask(),id=action_id();unsigned gate=calls(D,"func_8001BAF0"),worker=calls(H,"func_8001BAF0");show_state("first-action");
 printf("OBSERVED first-id=%d category=%d hand=%d phase=%d gate=%u worker=%u stock_worker=%u setups=%u spawns=%u path=%s\n",id,first,hand_count(),phase,gate,worker,ecalls("func_8001BAF0"),setup_count,spawn_count,swap_path);
 result("controller-completes-one-action",(phase==6||phase==7)&&hand_count()==4,"Native controller consumes exactly one physical hand instance.");
 result("planner-unforced-choice",id==(variant==0?344:35),"Unmodified official Hm_PlanHand, default settings, synthetic public-ID catalog.");
 result("gate-called-on-first-plan",gate==1,"Dual authoritative CPU category snapshot must run even for physical-only plans.");
 result("one-worker-first-action",worker==1&&ecalls("func_8001BAF0")==0&&setup_count==0,"One AI materializer entry; physical selection requires no deck-record exchange.");
 result("ledger-matches-first-action",ds()[1]==first,"Quota category must match actual controller-consumed card, not initial cursor.");
 resolve_action_model(phase);int awarded=award_bonus();
 result("bonus-from-controller-state",awarded&&ds()[5]&&hand_count()==4,"Actual Dual frame rebuild; no bonus/quota state injected.");
 unsigned prev=calls(D,"func_8001BAF0"),prev_h=calls(H,"func_8001BAF0");
 int bonus_phase=hand_action(120),second=action_mask(),second_id=action_id();show_state("bonus-action");
 printf("OBSERVED second-id=%d category=%d hand=%d phase=%d window=%d mask-ok=%d path=%s\n",second_id,second,hand_count(),bonus_phase,observed_window,masked_ok,swap_path);
 result("setup-masks-recorded-used-and-ritual",masked_ok&&observed_window==4,"Actual snapshot + Dual filtering; available hand shrinks to four under default AI window.");
 result("one-worker-bonus-action",calls(H,"func_8001BAF0")==prev_h+1&&ecalls("func_8001BAF0")==0,"Exactly one materializer entry for bonus plan.");
 result("gate-called-on-bonus-plan",calls(D,"func_8001BAF0")==prev+1,"Dual gate must run before responsible materializer.");
 result("no-repeated-category",(phase==6||phase==7)&&(bonus_phase==6||bonus_phase==7)&&first&&second&&!(first&second),"Two actual controller-consumed categories must be distinct.");
 result("final-ledger-matches-actions",ds()[1]==(first|second)&&ds()[1]==3,"A complete Monster+Support pair exhausts both quotas exactly once.");
}
static int prepare_bonus(int variant){scenario_init(variant);int phase=hand_action(120);resolve_action_model(phase);return award_bonus();}
static void adversarial_gate(int variant,int ritual,int cancel,const char*name){
 begin(name);CHECK(prepare_bonus(variant));unsigned used=ds()[1];CHECK(used==1||used==2);
 ((void(*)(void))engine_function("func_80028220"))();((void(*)(void*))engine_function("AiScript_Init"))(datum("D_801A8000"));
 // Explicit fault injection AFTER filtering. Not claimed to be planner-generated.
 AiActiveCard*a=datum("gDuel_aActiveCards");int id=ritual?666:used==1?24:343;
 memset(&a[11],0,sizeof a[11]);a[11].card_id=id;a[11].card_type=card_type(id);
 memset(datum("D_800EAE88"),0,12);if(!cancel)((uint8_t*)datum("D_800EAE88"))[0]=11;
 DuelDeckCardRecord before[80];memcpy(before,datum("gDuel_aDeckCardRecords"),sizeof before);int count=hand_count();unsigned h=calls(H,"func_8001BAF0"),d=calls(D,"func_8001BAF0");
 ((void(*)(void))engine_function("func_8001BAF0"))();
 printf("OBSERVED injected=%s used=%u selection=%u rejected=%u gate=%u worker=%u\n",cancel?"empty":ritual?"ritual":"spent-category",used,((uint8_t*)datum("D_800EAE88"))[0],ds()[21],calls(D,"func_8001BAF0")-d,calls(H,"func_8001BAF0")-h);
 result("invalid-plan-rejected-before-worker",calls(D,"func_8001BAF0")==d+1&&calls(H,"func_8001BAF0")==h&&ds()[21]&&((uint8_t*)datum("D_800EAE88"))[0]==0,"Adversarial post-filter injection, not natural gameplay reachability.");
 result("rejected-plan-does-not-consume",ds()[1]==used&&hand_count()==count&&!memcmp(before,datum("gDuel_aDeckCardRecords"),sizeof before),"Validation attempt alone cannot consume hand/deck instances or quotas.");
 if(ds()[21]){
  ((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();*(uint8_t*)datum("D_8009B162")=0;if(modules[D].mod.frame)modules[D].mod.frame();
  result("rejected-bonus-cancels-cleanly",!ds()[5]&&ds()[1]==used&&hand_count()==count,"Actual rejection cleanup/frame callback, UI completion modeled.");
 }else printf("NOT_EXECUTED rejected-bonus-cancels-cleanly gate-bypassed\n");
}
static void offscreen_case(void){
 begin("controller-offscreen-first-action");scenario_init(4);DuelDeckCardRecord before[80];memcpy(before,datum("gDuel_aDeckCardRecords"),sizeof before);
 int phase=hand_action(120);int counts[80]={0},valid_instances=1;DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){if(d[i].deck_index!=i||d[i].data_block_index>=80)valid_instances=0;else counts[d[i].data_block_index]++;}for(int i=0;i<80;i++)if(counts[i]!=1)valid_instances=0;
 printf("OBSERVED offscreen id=%d phase=%d setups=%u spawns=%u path=%s\n",action_id(),phase,setup_count,spawn_count,swap_path);
 result("offscreen-exactly-one-exchange",(phase==7)&&setup_count==1&&spawn_count==1&&calls(H,"func_8001BAF0")==1&&ecalls("func_8001BAF0")==0,"Actual default native eight-card window selects upcoming Dark Magician; AI owns one exchange.");
 result("offscreen-instance-conservation",valid_instances&&memcmp(before,d,sizeof before),"All 80 unique data-block identities conserved and deck positions stable.");
 result("offscreen-gate-executed",calls(D,"func_8001BAF0")==1,"Preserve-hand worker must remain behind Dual validation/snapshot.");
}
static void toggle_case(void){
 begin("off-on-stable-chain");scenario_init(4);((void(*)(void))engine_function("func_80028220"))();((void(*)(void*))engine_function("AiScript_Init"))(datum("D_801A8000"));
 ((uint8_t*)datum("D_800EAE88"))[0]=16;((void(*)(void))engine_function("func_8001BAF0"))();char before[256];snprintf(before,sizeof before,"%s",swap_path);
 toggle(H,0);toggle(H,1);toggle(D,0);toggle(D,1);
 scenario_init(4);((void(*)(void))engine_function("func_80028220"))();((void(*)(void*))engine_function("AiScript_Init"))(datum("D_801A8000"));((uint8_t*)datum("D_800EAE88"))[0]=16;((void(*)(void))engine_function("func_8001BAF0"))();
 printf("OBSERVED before-toggle=%s after-toggle=%s\n",before,swap_path);result("loaded-off-on-preserves-chain",!strcmp(before,swap_path),"Official Hooks_Relink retains registration order; neither fixes nor reverses I2.");
 toggle(H,0);scenario_init(4);((void(*)(void))engine_function("func_80028220"))();((uint8_t*)datum("D_800EAE88"))[0]=16;((void(*)(void))engine_function("func_8001BAF0"))();
 result("ai-off-stock-worker-once",calls(D,"func_8001BAF0")==1&&calls(H,"func_8001BAF0")==0&&ecalls("func_8001BAF0")==1&&setup_count==1,"Inactive AI delegates through official chain; original materializer executes once.");toggle(H,1);
 toggle(D,0);scenario_init(4);((void(*)(void))engine_function("func_80028220"))();((void(*)(void*))engine_function("AiScript_Init"))(datum("D_801A8000"));((uint8_t*)datum("D_800EAE88"))[0]=16;((void(*)(void))engine_function("func_8001BAF0"))();
 result("dual-off-ai-worker-once",calls(D,"func_8001BAF0")==0&&calls(H,"func_8001BAF0")==1&&setup_count==1,"OFF removes gate without duplicating active AI worker.");toggle(D,1);
 scenario_init(0);result("new-duel-clears-quota-state",ds()[1]==0&&ds()[5]==0&&ds()[3]==0,"Real Dual scene-init hook clears its registered state.");
}
static void multiple_instances_case(void){
 begin("two-logical-instances");scenario_init(4);((DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords"))[51].id=35;
 ((void(*)(void))engine_function("func_80028220"))();((void(*)(void*))engine_function("AiScript_Init"))(datum("D_801A8000"));
 uint8_t*s=datum("D_800EAE88");s[0]=16;s[1]=17;((void(*)(void))engine_function("func_8001BAF0"))();
 int counts[80]={0},ok=1;DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){if(d[i].deck_index!=i||d[i].data_block_index>=80)ok=0;else counts[d[i].data_block_index]++;}for(int i=0;i<80;i++)if(counts[i]!=1)ok=0;
 result("two-instances-one-worker",calls(H,"func_8001BAF0")==1&&ecalls("func_8001BAF0")==0&&setup_count==2&&spawn_count==2&&s[0]!=s[1]&&s[0]>=11&&s[0]<16&&s[1]>=11&&s[1]<16&&ok,"Injected two-card plan: one worker, one exchange per distinct selected instance; all identities conserved.");
}
static void diagnostic_cases(void){
 natural_case(0,"controller-support-first-cursor-monster");natural_case(1,"controller-monster-first-cursor-support");offscreen_case();multiple_instances_case();
 adversarial_gate(2,0,0,"adversarial-monster-already-used");adversarial_gate(3,0,0,"adversarial-support-already-used");adversarial_gate(2,1,0,"adversarial-ritual-during-bonus");adversarial_gate(2,0,1,"adversarial-empty-plan-cancel");toggle_case();
}
int main(int argc,char**argv){
 CHECK(argc==7);setvbuf(stdout,NULL,_IOLBF,0);CHECK(mkdtemp(data_directory));memory_init(argv[1]);
 trace_memory=mmap(NULL,65536,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(trace_memory!=MAP_FAILED);
 profile_order=atoi(argv[5]);selector=argv[6];modules[H].enabled=modules[D].enabled=1;
 native_load(argv[4],"duel_field_layout","");native_load(argv[4],"ai_opponent_data","");
 native_load(argv[4],"duel_scene_hand_actions","DuelScene_UpdateHandActions");
 native_load(argv[4],"func_8001B938","func_8001B938,func_8001BAF0");
 native_load(argv[4],"duel_card_state_helpers","func_80028220");native_load(argv[4],"func_80027DF8","func_80027DF8");
 native_load(argv[4],"ai_script_vm","AiScript_Init");native_load(argv[4],"ai_get_hand_size","Ai_GetHandSize");
 native_load(argv[4],"func_8001B7AC","func_8001B780,func_8001B7AC");native_load(argv[4],"duel_state_init","Duel_ClearHandSlots");
 int sequence[2]={H,D};if(profile_order==2||!strcmp(selector,"late-ai-first-load")){sequence[0]=D;sequence[1]=H;}
 if(!strcmp(selector,"late-ai-first-load")){
  modules[H].enabled=0;init_module(D,argv[3]);printf("LIFECYCLE Dual already loaded; AI unloaded; requested rank still H<D\n");modules[H].enabled=1;init_module(H,argv[2]);
 }else for(int i=0;i<2;i++)init_module(sequence[i],argv[2+sequence[i]]);
 begin("memory-layout");result("native-data-aliases",datum("D_801AB00C")==((unsigned char*)datum("gDuel_aActiveCards")+12)&&datum("gDuel_aDeckCardRecords")==((unsigned char*)datum("gDuel_aActiveCards")-0x31e0)&&datum("gDuel_bOpponentID")==((unsigned char*)datum("D_8009B360")+1),"Synthetic RAM preserves native byte identities and staging/deck aliases.");
 diagnostic_cases();
 printf("SUMMARY assertions=%u cases=%d failures=%u (controller fixture; no Windows gameplay)\n",assertions,current_case,failures);return failures?2:0;
}
