/* HAL-22/24 diagnostic only: actual API 24 objects, real manager, selected native
 * controller/snapshot/materializer. The stock VM proposal, catalog, renderer
 * and phase-resolution bridge are models. Never inject or write deck_pull.
 * hal2224-observer is a test-owned wrapper for explicitly instrumented reentrancy.
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
#include "pc/platform/settings.h"
#include "pc/debug/symbols.h"
#include "export_names.h"
enum { H,D,RD,OBS,M,W,NMOD };
typedef struct {const char*id;void*state;size_t state_size;unsigned state_version;LoadedObject object;} Module;
static Module modules[NMOD]={{.id="ai-hard-mode"},{.id="dual-play"},{.id="ritual-duelists"},{.id="hal2224-observer"},{.id="melhoria-dos-rituais"},{.id="hal2224-wrapper"}};
static int mi(int m){for(int i=0;i<Mods_Count();i++)if(!strcmp(Mods_Id(i),modules[m].id))return i;return -1;}
static int which(int owner){for(int i=0;i<NMOD;i++)if(!strcmp(Mods_Id(owner),modules[i].id))return i;return -1;}
static unsigned assertions,failures;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FIXTURE_ASSERT %s:%d %s\n",__FILE__,__LINE__,#x);exit(90);}++assertions;}while(0)
static _Alignas(16) unsigned char data[160][65536];static char data_names[160][100];static int nd;
typedef struct {char name[100];void*entry;void*page;} Engine;
static Engine engine[512];static int ne;
typedef struct {int owner,event,token,kind;char name[100];void*replacement,*wrapper;void**previous;unsigned calls;} Registration;
static Registration registrations[256];static int nr;
static unsigned char*trace_memory;static size_t trace_used;
static char data_directory[]="/tmp/hal2224-XXXXXX";
static int native_ai_result=1,stock_choice=12;
void HAL2224_AfterPlanner(void);
static int current_case;
static unsigned engine_calls[512], setup_count, spawn_count;
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
static int native_ai(void){puts("MODEL_VM_COMPLETION status=1; no plan/selection/pending/quota written");return 1;}
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
static int named(const char*n){
 const char*names[]={"Simon Muran","Teana","Jono","Villager 1","Villager 2","Villager 3","Seto","Heishin","Rex Raptor","Weevil Underwood","Mai Valentine","Bandit Keith","Shadi","Yami Bakura","Pegasus","Isis","Kaiba","Mage Soldier","Jono 2nd","Teana 2nd","Ocean Mage","High Mage Secmeton","Forest Mage","High Mage Anubisius","Mountain Mage","High Mage Atenza","Desert Mage","High Mage Martis","Meadow Mage","High Mage Kepura","Labyrinth Mage","Seto 2nd","Guardian Sebek","Guardian Neku","Heishin 2nd","Seto 3rd","DarkNite","Nitemare"};for(int i=0;i<38;i++)if(!strcmp(n,names[i]))return i+1;return 0;
}
static void stock_deal(int src,uint8_t*out16,uint8_t*out8){(void)src;for(int i=0;i<40;i++){((uint16_t*)out16)[i]=99;out8[i]=i;}puts("MODEL_STOCK_DEAL fallback");}
void Platform_ShowError(const char*,const char*);
static void *body(const char*n){
 if(!strcmp(n,"Mods_Hal2224GuardReady"))return Mods_Hal2224GuardReady;
 if(!strcmp(n,"Platform_ShowError"))return Platform_ShowError;
 if(!strcmp(n,"Duel_ShuffleDeck"))return stock_deal;
 if(!strcmp(n,"HAL2224_AfterPlanner"))return HAL2224_AfterPlanner;
 if(!strcmp(n,"Duelists_Named"))return named;
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
 CHECK(ne<512&&strlen(n)<100);int i=ne++;strcpy(engine[i].name,n);
 unsigned char*p=mmap(NULL,4096,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(p!=MAP_FAILED);
 memset(p,0x90,24);engine[i].page=p;engine[i].entry=p+16;thunk(body(n),-i-1,p+18);return p+16;
}
static void *resolve(const char*n,void*ctx){
 (void)ctx;void(*lib)(void)=Mods_LibcLookup(n);if(lib)return lib;
 if(!strncmp(n,"D_",2)||n[0]=='g')return datum(n);return engine_function(n);
}
static void begin(const char*n){case_name=n;++current_case;printf("CASE %s\n",n);for(int i=0;i<nr;i++)registrations[i].calls=0;}
static unsigned calls(int m,const char*n){unsigned result=0;for(int i=0;i<nr;i++)if(registrations[i].owner==m&&!strcmp(registrations[i].name,n))result+=registrations[i].calls;return result;}
static void result(const char*n,int pass,const char*details){printf("RESULT %s %s %s\n",n,pass?"PASS":"FAIL",details);if(!pass)++failures;}
static const char*target_name(void*f){for(int i=0;i<ne;i++)if(engine[i].entry==f)return engine[i].name;return "unknown-target";}
int __real_Hooks_Add(int owner,void*f,void*r,void**previous);
int __wrap_Hooks_Add(int owner,void*f,void*r,void**previous){
 CHECK(owner>=0&&owner<Mods_Count()&&nr<256);int i=nr++;Registration*q=&registrations[i];q->owner=which(owner);CHECK(q->owner>=0);q->replacement=r;q->previous=previous;snprintf(q->name,sizeof q->name,"%s",target_name(f));q->wrapper=wrap(r,i);q->token=__real_Hooks_Add(owner,f,q->wrapper,previous);printf("REGISTER owner=%s target=%s token=%d\n",Mods_Id(owner),q->name,q->token);return q->token;
}
int __real_Mods_RegisterState(int owner,void*p,size_t size,unsigned version);
int __wrap_Mods_RegisterState(int owner,void*p,size_t size,unsigned version){
 CHECK(owner>=0&&owner<Mods_Count());int m=which(owner);CHECK(m>=0);modules[m].state=p;modules[m].state_size=size;modules[m].state_version=version;printf("STATE owner=%s size=%u version=%u\n",Mods_Id(owner),(unsigned)size,version);return __real_Mods_RegisterState(owner,p,size,version);
}
int __real_ObjectLoader_Load(const void*,size_t,ObjectResolver,void*,LoadedObject*,char*,size_t);
static unsigned mod_loads[NMOD];
int __wrap_ObjectLoader_Load(const void*data,size_t size,ObjectResolver resolver,void*ctx,LoadedObject*object,char*error,size_t cap){
 int rc=__real_ObjectLoader_Load(data,size,resolver,ctx,object,error,cap);
 if(!rc){int m=object->hash==0x3e0648cb?H:object->hash==0x3028c68a?D:-1;
  for(size_t i=0;i<object->symbol_count;i++)if(!strcmp(object->symbols[i].name,"deck_pull")){m=RD;modules[RD].object=*object;}
if(m>=0){++mod_loads[m];printf("LIBRARY_INIT owner=%s hash=%08x loads=%u\n",modules[m].id,object->hash,mod_loads[m]);}}
 return rc;
}
MemoriesModExport test_export_storage[TEST_EXPORT_COUNT];
__asm__(".globl Memories_ModExports\n.set Memories_ModExports,test_export_storage");
const unsigned Memories_ModExportCount=TEST_EXPORT_COUNT;
static void prepare_exports(void){for(int i=0;i<TEST_EXPORT_COUNT;i++){test_export_storage[i].name=test_export_names[i];test_export_storage[i].address=resolve(test_export_names[i],NULL);}printf("REAL_HOST_EXPORTS count=%u\n",Memories_ModExportCount);}
/* External platform/disc services, same boundary used by official mods_test.c. */
int Log_Enabled(int channel){(void)channel;return 0;}
int Log_Wanted(int channel){(void)channel;return 0;}
void Log_Printf(int channel,const char*format,...){(void)channel;(void)format;}
unsigned short Platform_Pad(int port){(void)port;return 0;}
int Symbols_Add(const SymbolsEntry*entries,size_t count){(void)entries;(void)count;return 0;}
int Memories_DiscReadSectors(int lba,int sectors,void*out){(void)lba;memset(out,0,sectors*2048);return sectors;}
int Memories_DiscSectorCount(void){return 10000;}
int Memories_DiscOriginalFileInfo(const char*path,int*lba,unsigned*size){(void)path;(void)lba;(void)size;return -1;}
int Memories_DiscFileInfo(const char*path,int*lba,unsigned*size){return Memories_DiscOriginalFileInfo(path,lba,size);}
int Memories_DiscFileStart(const char*path){(void)path;return -1;}
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
static void show_state(const char*where){if(!modules[D].state){printf("LEDGER %s Dual not initialized\n",where);return;}unsigned char*p=ds();printf("LEDGER %s used=%u pending=%u bonus=%u stage=%u cpu=%u last_plan=%u remaining=%u rejected=%u\n",where,p[1],p[3],p[5],p[9],p[13],p[15],p[17],p[21]);}
static unsigned ecalls(const char*n){for(int i=0;i<ne;i++)if(!strcmp(n,engine[i].name))return engine_calls[i];return 0;}
static void set_stat(int id,int attack,int defense,int type){((uint32_t*)datum("gDuel_adwCardStats"))[id-1]=attack/10|((unsigned)(defense/10)<<9)|((unsigned)type<<CARD_STAT_TYPE_SHIFT);}

typedef struct {int active,side,source,replace,card,mode,target;uint8_t fallback[12];} Pull;
static Pull*pull(void){LoadedObject*q=&modules[RD].object;for(size_t i=0;i<q->symbol_count;i++)if(!strcmp(q->symbols[i].name,"deck_pull")){CHECK(q->symbols[i].size==sizeof(Pull));return(void*)q->symbols[i].address;}CHECK(!Mods_Active(mi(RD)));static Pull absent;return &absent;}
static void pending_log(const char*where){Pull*p=pull();printf("PENDING %s active=%d side=%d source=%d replacement=%d card=%d mode=%d target=%d fallback=%u selection=%u\n",where,p->active,p->side,p->source,p->replace,p->card,p->mode,p->target,p->fallback[0],((uint8_t*)datum("D_800EAE88"))[0]);}
static int planned_pending,plan_selection,plan_id,plan_type,after_calls,mask_ok=1,logical_window;
void HAL2224_AfterPlanner(void){
 static int tried_wrapper;
 if(Settings_GetNamed("test.late_wrapper",0)&&!tried_wrapper){tried_wrapper=1;CHECK(mi(W)>=0);int old=Mods_Enabled(mi(W));Mods_SetEnabled(mi(W),1);printf("INSTRUMENTED_LATE_LIBRARY old=%d want=%d active=%d code=%08x\n",old,Mods_Enabled(mi(W)),Mods_Active(mi(W)),Mods_CodeHash(mi(W)));if(Mods_Hal2224GuardEnabled())result("unqualified-first-load-refused-before-init",Mods_Enabled(mi(W))==old&&!Mods_Active(mi(W))&&!Mods_CodeHash(mi(W)),"Own manager reentrancy control; no pending/plan/quota injection and no previous_swap call added.");}

 ++after_calls;Pull*p=pull();uint8_t*sel=datum("D_800EAE88");AiActiveCard*a=datum("gDuel_aActiveCards");planned_pending=p->active;plan_selection=sel[0];plan_id=sel[0]>=11&&sel[0]<56?a[sel[0]].card_id:0;plan_type=card_type(plan_id);
 pending_log("after-real-planners");printf("PLAN pending=%d selection=%u id=%d type=%d bonus=%u used=%u\n",planned_pending,sel[0],plan_id,plan_type,Mods_Active(mi(D))?ds()[5]:0,Mods_Active(mi(D))?ds()[1]:0);
}
static void setup(int variant){
 memset((void*)0x8009B000,0,0x400);memset((void*)0x800E9000,0,0x3000);memset((void*)0x801A7000,0,0x8000);memset(data,0,sizeof data);display_count=0;
 *(int*)datum("gCard_nCount")=722;*(signed char*)datum("D_8009B360")=-1;*(signed char*)datum("gDuel_bOpponentID")=9;*(uint8_t*)datum("D_8009B1D5")=1;
 DuelSideState*s=datum("D_800E9FF0");for(int i=0;i<2;i++){memset(s[i].hand,-1,5);s[i].max_life_points=s[i].life_points.unsigned_value=8000;s[i].deck_draw_cursor=5;}*(DuelSideState**)datum("D_8009B1C8")=&s[1];
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){d[i].id=99;d[i].deck_index=i;d[i].data_block_index=i;}
 for(int i=1;i<=722;i++)set_stat(i,1000,1000,0);set_stat(680,0,0,22);set_stat(344,0,0,20);set_stat(343,0,0,20);set_stat(535,2000,2000,0);set_stat(99,300,200,0);set_stat(6,1800,1600,0);set_stat(47,1000,1000,0);set_stat(571,1000,1000,0);set_stat(110,1000,1000,0);set_stat(111,1100,1100,0);set_stat(112,1200,1200,0);
 int hand[]={535,99,110,111,112};d[45].id=680;d[46].id=571;
 if(variant==1){int h[]={680,6,47,344,112};memcpy(hand,h,sizeof h);d[45].id=571;}
 if(variant==2){int h[]={6,6,47,344,680};memcpy(hand,h,sizeof h);d[45].id=571;}
 if(variant==3){int h[]={344,343,680,6,47};memcpy(hand,h,sizeof h);d[45].id=571;}
 if(variant==6){int h[]={680,344,99,110,112};memcpy(hand,h,sizeof h);}
 if(variant==7){int h[]={680,6,47,571,344};memcpy(hand,h,sizeof h);}
 if(variant==8){int h[]={680,47,571,344,112};memcpy(hand,h,sizeof h);}
 if(variant==5){int h[]={535,99,344,111,112};memcpy(hand,h,sizeof h);}
 if(variant==4){int h[]={6,47,571,344,680};memcpy(hand,h,sizeof h);d[45].id=535;}
 for(int i=0;i<5;i++){d[40+i].id=hand[i];s[1].hand[i]=40+i;DuelCardRecord*r=setup_record(15+i,40+i);((DuelHandSlot*)datum("D_800EA030"))[i].object=spawn_hand(r,14+i*60,194);}
 if(variant==6||variant==8){int ids[]={6,47,571},n=variant==6?3:1;for(int i=0;i<n;i++){d[45+i].id=ids[i];DuelCardRecord*r=setup_record(20+i,45+i);r->object=spawn_hand(r,0,0);}s[1].deck_draw_cursor=5+n;d[45+n].id=535;}
 *(uint8_t*)datum("D_8009B368")=2;uint16_t row[]={680,6,47,571,705,0};memcpy(datum("gDuel_awRitualData"),row,sizeof row);
 Mods_Reset();((void(*)(void))engine_function("Duel_InitScene"))();*(uint16_t*)datum("gDuel_wSceneStateFlags")=4;
 memset(engine_calls,0,sizeof engine_calls);for(int i=0;i<nr;i++)registrations[i].calls=0;setup_count=spawn_count=after_calls=planned_pending=0;swap_path[0]=0;mask_ok=1;
}
static int conserved(void){int count[80]={0};DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){if(d[i].deck_index!=i||d[i].data_block_index>=80)return 0;count[d[i].data_block_index]++;}for(int i=0;i<80;i++)if(count[i]!=1)return 0;return 1;}
static int hand_count(void){int n=0;DuelSideState*s=datum("D_800E9FF0");for(int i=0;i<5;i++)n+=s[1].hand[i]>=0;return n;}
static DisplayObject*action_object(void){DisplayObject**o=datum("D_800E9EF0");for(int i=0;i<7;i++)if(o[i]&&o[i]->field_6A>=15&&o[i]->field_6A<20)return o[i];return NULL;}
static int action_card(void){DisplayObject*o=action_object();return o?((DuelCardRecord*)datum("D_801A7AD8"))[o->field_6A].card_id:0;}
static void instance_log(const char*where){DisplayObject*o=action_object();if(!o)return;DuelCardRecord*r=(DuelCardRecord*)datum("D_801A7AD8")+o->field_6A;DuelDeckCardRecord*d=r->data;printf("ACTION_INSTANCE %s record=%u id=%d deck_index=%d data_block=%u pointer_matches=%d\n",where,o->field_6A,r->card_id,d?d->deck_index:-1,d?d->data_block_index:255,d==((DuelDeckCardRecord*)datum("gDuel_aDeckCardRecords"))+(d?d->deck_index:0));}
static int action_category(void){int id=action_card();return id?card_type(id)<20?1:2:0;}
static int hand_action(void){
 DuelDeckCardRecord checkpoint[80];memcpy(checkpoint,datum("gDuel_aDeckCardRecords"),sizeof checkpoint);int checkpoint_hand=hand_count(),checked_rejection=0;unsigned checkpoint_swaps=setup_count;
 for(int frame=0;frame<180;frame++){
  *(uint16_t*)datum("D_8009B162")=0;((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();int phase=*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff;
  if(Mods_Active(mi(D))&&ds()[21]&&!checked_rejection){checked_rejection=1;result("rejection-preserves-hand-and-deck",hand_count()==checkpoint_hand&&setup_count==checkpoint_swaps&&!memcmp(checkpoint,datum("gDuel_aDeckCardRecords"),sizeof checkpoint),"Observe actual rejected empty plan before cleanup; no substitute plan or pending supplied.");}
  if(phase==4&&*(uint16_t*)datum("D_8009B20C")==0x8000){logical_window=((int(*)(void))engine_function("Ai_GetHandSize"))();AiActiveCard*a=datum("gDuel_aActiveCards");printf("SNAPSHOT bonus=%u used=%u window=%d ids=",Mods_Active(mi(D))?ds()[5]:0,Mods_Active(mi(D))?ds()[1]:0,logical_window);for(int i=0;i<logical_window;i++){int id=a[11+i].card_id;printf("%s%d",i?",":"",id);if(Mods_Active(mi(D))&&ds()[5]&&id&&(card_type(id)==22||((card_type(id)<20?1:2)&ds()[1])))mask_ok=0;}puts("");}
  if(frame<3||phase!=4)printf("STEP frame=%d phase=%d substate=%u selection=%u\n",frame,phase,*(uint16_t*)datum("D_8009B20C"),((uint8_t*)datum("D_800EAE88"))[0]);
  if(phase!=4)return phase;
 }
 return 4;
}
static void resolve_model(int phase){
 if(phase==7){DisplayObject*o=action_object();CHECK(o);DuelCardRecord*r=datum("D_801A7AD8");int target=*(uint8_t*)datum("D_8009B19C");CHECK(target>=0&&target<30);r[target]=r[o->field_6A];r[target].object=o;o->field_6A=target;puts("MODEL_PLACEMENT complete display placement; no selected/quota/pending edits");}
 else puts("MODEL_EFFECT completion only; no effect engine claim");
}
static int award_bonus(void){
 *(uint16_t*)datum("gDuel_wSceneStateFlags")=0x8005;*(uint8_t*)datum("D_8009B174")=3;puts("MODEL_PHASE effect/placement -> field entry (no native full-turn claim)");
 for(int i=0;i<8;i++){*(uint16_t*)datum("D_8009B162")=0;Mods_DrawFrame();show_state("real-frame-award");if((*(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff)==4)return 1;}return 0;
}
typedef struct {unsigned abi,count,sources;int min_field,min_hand,require_result,only_player;} RulesSnapshot;
typedef struct {unsigned abi;int(*read)(RulesSnapshot*,unsigned);} RulesProvider;
static void inspect_rules(void){RulesProvider*p=Mods_Find("melhoria-dos-rituais:rules_v1");RulesSnapshot s={0};int ok=p&&p->abi==1&&p->read(&s,sizeof s);printf("RULES provider=%d abi=%u count=%u sources=%u min_field=%d min_hand=%d require=%d\n",ok,s.abi,s.count,s.sources,s.min_field,s.min_hand,s.require_result);if(mi(M)>=0&&Mods_Active(mi(M)))result("actual-optional-rules-provider",ok&&s.abi==1&&s.count==3,"Existing PR9 provider; no synthetic provider, rules or tribute selection injected.");}
static int deck_changes(const DuelDeckCardRecord*before){int n=0;DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++)n+=memcmp(&before[i],&d[i],sizeof(*d))!=0;return n;}
static unsigned error_notices;static char last_notice[512];
void Platform_ShowError(const char*title,const char*message){++error_notices;snprintf(last_notice,sizeof last_notice,"%s: %s",title,message);printf("VISIBLE_ERROR %s\n",last_notice);}
static int fence(const char*where){char why[512];int ready=Mods_Hal2224GuardReady(why,sizeof why);uint8_t*st=ObjectLoader_Symbol(&native_objects[nn-1],"D_8009B26C");CHECK(st);*st=0;*(uint8_t*)datum("D_8009B0A3")=0;((void(*)(void))engine_function("Main_RunDuel"))();int entered=*(uint8_t*)datum("D_8009B0A3")==10;printf("ADMISSION %s enabled=%d ready=%d entered=%d notices=%u reason=%s\n",where,Mods_Hal2224GuardEnabled(),ready,entered,error_notices,ready?"none":why);result("native-duel-entry-fence",entered==ready,"Actual experimental Main_RunDuel returns before initialization/controller for unverified composition.");if(!ready){result("explicit-profile-refusal",error_notices&&strstr(last_notice,"HAL-22/24"),"Visible platform error observed by stub, no silent disabling or action execution.");puts("NOT_RUN controller-suite profile refused before setup");}return ready;}
extern int Hal2224_WindowApply(int,int,int,int,int,int,int,int,int,int,int);
extern const char*Hal2224_WindowStatus(void);
static void profile_change(const char*method){
 int h=mi(H),d=mi(D),r=mi(RD);int old[3]={Mods_Enabled(h),Mods_Enabled(r),Mods_Enabled(d)};
 if(!strncmp(method,"arm-",4)){Settings_SetNamed("experimental.hal2224_order_guard",1);printf("ARM_GUARD legacy chain preserved\n");method+=4;}
 if(!strcmp(method,"late-ai"))Mods_SetEnabled(h,1);
 else if(!strcmp(method,"late-dual"))Mods_SetEnabled(d,1);
 else if(!strcmp(method,"cycle-ai")){Mods_SetEnabled(h,0);Mods_SetEnabled(h,1);}
 else if(!strcmp(method,"cycle-dual")){Mods_SetEnabled(d,0);Mods_SetEnabled(d,1);}
 else if(!strcmp(method,"late-rd"))Mods_SetEnabled(r,1);
 else if(!strcmp(method,"cycle-rd")){Mods_SetEnabled(r,0);Mods_SetEnabled(r,1);}
 else if(!strcmp(method,"reload"))Mods_Load();
 else if(!strncmp(method,"window-",7)){
  int invert=!strcmp(method,"window-invert"),repair=strstr(method,"repair")!=NULL;
  int rh=(repair||invert)?(invert?2:0):-999,rr=1,rd=invert?0:2;
  int oldrank[3]={Settings_GetNamed("mod.ai-hard-mode.order",0),Settings_GetNamed("mod.ritual-duelists.order",0),Settings_GetNamed("mod.dual-play.order",0)};
  int code=Hal2224_WindowApply(h,r,d,1,1,1,rh,rr,rd,strstr(method,"cancel")==NULL,strstr(method,"fail")!=NULL);
  printf("WINDOW_CODE %d\n",code);
  if(invert&&Mods_Hal2224GuardEnabled())result("window-rejection-keeps-ranks",Settings_GetNamed("mod.ai-hard-mode.order",0)==oldrank[0]&&Settings_GetNamed("mod.ritual-duelists.order",0)==oldrank[1]&&Settings_GetNamed("mod.dual-play.order",0)==oldrank[2],"Original staged-rank validation rejects before persisting/restarting.");
  if(strstr(method,"fail"))result("window-restart-failure-visible",strstr(Hal2224_WindowStatus(),"Restart failed")!=NULL,"Observed platform failure, not an actual failed Windows restart.");
 }else CHECK(!strcmp(method,"none"));
 if(Mods_Hal2224GuardEnabled()&&!strcmp(method,"late-ai")&&!Mods_RequiresRestart(h)&&!Mods_Enabled(h))result("late-ai-rejection-preserves-preference",Mods_Enabled(h)==old[0],"No silent enabled preference overwrite on rejected direct activation.");
 printf("LIFECYCLE method=%s H-want=%d H-active=%d D-want=%d D-active=%d H-restart=%d RD-want=%d RD-active=%d\n",method,Mods_Enabled(h),Mods_Active(h),Mods_Enabled(d),Mods_Active(d),Mods_RequiresRestart(h),Mods_Enabled(r),Mods_Active(r));
}
int main(int argc,char**argv){
 CHECK(argc==7);setvbuf(stdout,NULL,_IOLBF,0);CHECK(mkdtemp(data_directory));memory_init(argv[1]);trace_memory=mmap(NULL,65536,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(trace_memory!=MAP_FAILED);
 native_load(argv[2],"duel_field_layout","");native_load(argv[2],"ai_opponent_data","");native_load(argv[2],"duel_scene_hand_actions","DuelScene_UpdateHandActions");native_load(argv[2],"func_8001B938","func_8001B938,func_8001BAF0");native_load(argv[2],"duel_card_state_helpers","func_80028220");native_load(argv[2],"func_80027DF8","func_80027DF8");native_load(argv[2],"ai_script_vm","AiScript_Init");native_load(argv[2],"ai_get_hand_size","Ai_GetHandSize");native_load(argv[2],"func_8001B7AC","func_8001B780,func_8001B7AC");native_load(argv[2],"duel_state_init","Duel_ClearHandSlots");
 native_load(argv[6],"guard_main","Main_RunDuel");
 prepare_exports();*(int*)datum("gCard_nCount")=722;Settings_Load();Mods_Load();CHECK(mi(H)>=0&&mi(D)>=0&&mi(RD)>=0&&mi(OBS)>=0);for(int i=0;i<Mods_Count();i++){printf("MANAGER id=%s enabled=%d active=%d status=%s\n",Mods_Id(i),Mods_Enabled(i),Mods_Active(i),Mods_Status(i));CHECK(!Mods_Failed(i));}
 profile_change(!strcmp(argv[5],"reboot")?"none":argv[4]);inspect_rules();if(!fence("before-controller"))goto done;setup(atoi(argv[3]));DuelDeckCardRecord initial[80];memcpy(initial,datum("gDuel_aDeckCardRecords"),sizeof initial);begin("real-controller-first-action");
 int phase=hand_action(),first=action_category(),firstid=action_card(),created=planned_pending;unsigned firstswap=setup_count;show_state("after-first");instance_log("first");printf("ACTION first id=%d category=%d phase=%d hand=%d staged=%d pending_after=%d gate=%u rd=%u ai=%u exchanges=%u path=%s\n",firstid,first,phase,hand_count(),created,pull()->active,calls(D,"func_8001BAF0"),calls(RD,"func_8001BAF0"),calls(H,"func_8001BAF0"),firstswap,swap_path);
 if(phase==4){puts("NOT_REACHED first-action no selected plan supplied; retail VM unavailable");goto done;}
 if(Mods_Hal2224GuardEnabled()&&Mods_Active(mi(D)))result("first-gate-executed",calls(D,"func_8001BAF0")==1,"Actual callback trace, not just registered presence.");result("first-controller-consumes-one",(phase==6||phase==7)&&hand_count()==4,"Native hand controller consumed a single existing physical instance.");result("first-instance-conservation",conserved()&&firstswap<=1,"At most one offscreen exchange; unique deck record identities conserved.");result("unselected-deck-records-intact",deck_changes(initial)==(firstswap?2:0),"Only two actual exchanged records differ; nonselected material/deck records unchanged.");result("first-pending-consumed",!pull()->active,"No forced AI toggle between planner and swap in HAL24.");
 if(Mods_Active(mi(D))){result("first-ledger-matches-physical-category",first&&ds()[1]==first,"Dual hand callback must account actual consumption even when its swap gate is skipped.");resolve_model(phase);int awarded=award_bonus();printf("BONUS awarded=%d used=%u\n",awarded,ds()[1]);if(!awarded){puts("NOT_REACHED bonus not granted by real Dual callbacks for this state");goto done;}
 begin("real-controller-bonus-action");int count=hand_count();unsigned gate=calls(D,"func_8001BAF0"),setupbefore=setup_count;DuelDeckCardRecord beforebonus[80];memcpy(beforebonus,datum("gDuel_aDeckCardRecords"),sizeof beforebonus);int bonus=hand_action(),second=action_category(),secondid=action_card();show_state("after-bonus");instance_log("bonus");printf("ACTION bonus id=%d category=%d phase=%d hand=%d staged=%d pending_after=%d gate=%u rd=%u ai=%u exchanges=%u masks=%d path=%s\n",secondid,second,bonus,hand_count(),planned_pending,pull()->active,calls(D,"func_8001BAF0")-gate,calls(RD,"func_8001BAF0"),calls(H,"func_8001BAF0"),setup_count-setupbefore,mask_ok,swap_path);
 if(Mods_Hal2224GuardEnabled()&&Mods_Active(mi(D)))result("bonus-gate-executed",calls(D,"func_8001BAF0")==gate+1,"The real bonus action enters Dual gate before materializer.");result("bonus-setup-hides-forbidden-categories",mask_ok,"Actual Dual snapshot filtering excludes spent category and ritual magic before planners.");
 if(bonus==6||bonus==7){result("bonus-category-authorized",second&&!(first&second)&&card_type(secondid)!=22,"Compare actual consumed category to first physical action, not assumed plan/cursor.");result("bonus-one-consumption",hand_count()==count-1,"A single physical instance leaves hand in bonus action.");result("bonus-ledger-matches-actions",ds()[1]==(first|second),"Real Dual accounting matches definitive categories.");}else {puts("NOT_REACHED bonus commit; no substitute selection injected");if(ds()[21])result("rejected-plan-rollback",hand_count()==count&&!memcmp(beforebonus,datum("gDuel_aDeckCardRecords"),sizeof beforebonus),"Real Dual rejection: no physical/deck consumption at the checked boundary.");}
 result("bonus-instance-conservation",conserved()&&setup_count-setupbefore<=1,"No duplicated data block identity or second offscreen exchange.");result("bonus-pending-consumed",!pull()->active,"Observe real pending lifecycle, never clear it to pass.");
 }else puts("NOT_RUN Dual-OFF positive planner control; no quota authority active");
 done:if(mi(W)>=0&&Settings_GetNamed("mod.hal2224-wrapper.unrelated",0)&&Mods_Active(mi(W))){unsigned n=calls(W,"Duel_DrawFieldCards");((void(*)(void))engine_function("Duel_DrawFieldCards"))();result("unrelated-wrapper-kept",calls(W,"Duel_DrawFieldCards")==n+1,"Own unrelated hook still executes; no global hook order change.");}CHECK(Settings_Save());Mods_Shutdown();printf("SUMMARY assertions=%u failures=%u no pending/plan/quota injection\n",assertions,failures);return failures?1:0;
}
