/* HAL-23 diagnostic only: actual API 24 objects, real manager, selected native
 * controller/snapshot/materializer. The stock VM proposal, catalog, renderer
 * and phase-resolution bridge are models. Never inject or write deck_pull.
 * hal23-observer is a test-owned wrapper for explicitly instrumented reentrancy.
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
enum { H,D,RD,OBS,NMOD };
typedef struct {const char*id;void*state;size_t state_size;unsigned state_version;LoadedObject object;} Module;
static Module modules[NMOD]={{.id="ai-hard-mode"},{.id="dual-play"},{.id="ritual-duelists"},{.id="hal23-observer"}};
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
static char data_directory[]="/tmp/hal23-XXXXXX";
static int native_ai_result=1,stock_choice=12;
void HAL23_AfterPlanner(void);
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
static int native_ai(void){uint8_t*s=datum("D_800EAE88");memset(s,0,12);s[0]=stock_choice;s[6]=1;s[8]=1;printf("MODEL_STOCK_AI choice=%d result=%d\n",stock_choice,native_ai_result);return native_ai_result;}
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
static void *body(const char*n){
 if(!strcmp(n,"Duel_ShuffleDeck"))return stock_deal;
 if(!strcmp(n,"HAL23_AfterPlanner"))return HAL23_AfterPlanner;
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
static void show_state(const char*where){unsigned char*p=ds();printf("LEDGER %s used=%u pending=%u bonus=%u stage=%u cpu=%u last_plan=%u remaining=%u rejected=%u\n",where,p[1],p[3],p[5],p[9],p[13],p[15],p[17],p[21]);}
static unsigned ecalls(const char*n){for(int i=0;i<ne;i++)if(!strcmp(n,engine[i].name))return engine_calls[i];return 0;}
static void set_stat(int id,int attack,int defense,int type){((uint32_t*)datum("gDuel_adwCardStats"))[id-1]=attack/10|((unsigned)(defense/10)<<9)|((unsigned)type<<CARD_STAT_TYPE_SHIFT);}

typedef struct {int active,side,source,replace,card,mode,target;uint8_t fallback[12];} Pull;
static Pull*pull(void){LoadedObject*q=&modules[RD].object;for(size_t i=0;i<q->symbol_count;i++)if(!strcmp(q->symbols[i].name,"deck_pull")){CHECK(q->symbols[i].size==sizeof(Pull));return(void*)q->symbols[i].address;}CHECK(0);return NULL;}
static void pending_log(const char*where){Pull*p=pull();printf("PENDING %s active=%d side=%d source=%d replacement=%d card=%d mode=%d target=%d fallback=%u selection=%u\n",where,p->active,p->side,p->source,p->replace,p->card,p->mode,p->target,p->fallback[0],((uint8_t*)datum("D_800EAE88"))[0]);}
static int armed,after_calls,planned_pending,plan_selection,target_ai=1;
static const char*boundary="gap";
static const char*change="none";
extern int Hal22_WindowApply(int,int,int,int,int,int,int,int);
static void manager_change(void){
 if(!strcmp(change,"apply")){int want[MODS_MAX];char error[512];for(int i=0;i<Mods_Count();i++)want[i]=Mods_Enabled(i);want[mi(H)]=target_ai;CHECK(Mods_Apply(want,error,sizeof error));}
 else if(!strcmp(change,"direct"))Mods_SetEnabled(mi(H),target_ai);
 else if(!strcmp(change,"reload")){Settings_SetNamed("mod.ai-hard-mode",target_ai);CHECK(Settings_Save());Mods_Load();}
 else if(!strcmp(change,"window")||!strcmp(change,"window-cancel")||!strcmp(change,"window-fail")){
  int code=Hal22_WindowApply(mi(H),mi(D),target_ai,Mods_Enabled(mi(D)),-999,-999,strcmp(change,"window-cancel")!=0,!strcmp(change,"window-fail"));printf("WINDOW_CODE %d\n",code);
 }
 printf("CHANGE method=%s H-want=%d H-active=%d H-restart=%d loads=%u\n",change,Mods_Enabled(mi(H)),Mods_Active(mi(H)),Mods_RequiresRestart(mi(H)),mod_loads[H]);
}
void HAL23_AfterPlanner(void){
 ++after_calls;planned_pending=pull()->active;plan_selection=((uint8_t*)datum("D_800EAE88"))[0];pending_log("after-planner");
 if(armed&&!strcmp(boundary,"gap")){armed=0;printf("INSTRUMENTED_GAP manager call inserted by test observer; not a UI event pump\n");manager_change();pending_log("after-change");}
}
static void setup(void){
 memset((void*)0x8009B000,0,0x400);memset((void*)0x800E9000,0,0x3000);memset((void*)0x801A7000,0,0x8000);memset(data,0,sizeof data);display_count=0;
 *(int*)datum("gCard_nCount")=722;*(signed char*)datum("D_8009B360")=-1;*(signed char*)datum("gDuel_bOpponentID")=9;*(uint8_t*)datum("D_8009B1D5")=1;
 DuelSideState*s=datum("D_800E9FF0");for(int i=0;i<2;i++){memset(s[i].hand,-1,5);s[i].max_life_points=s[i].life_points.unsigned_value=8000;s[i].deck_draw_cursor=5;}*(DuelSideState**)datum("D_8009B1C8")=&s[1];
 DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){d[i].id=99;d[i].deck_index=i;d[i].data_block_index=i;}
 for(int i=1;i<=722;i++)set_stat(i,1000,1000,0);set_stat(680,0,0,22);set_stat(535,2000,2000,0);set_stat(99,300,200,0);set_stat(110,1000,1000,0);set_stat(111,1100,1100,0);set_stat(112,1200,1200,0);
 int hand[]={535,99,110,111,112};for(int i=0;i<5;i++){d[40+i].id=hand[i];s[1].hand[i]=40+i;DuelCardRecord*r=setup_record(15+i,40+i);((DuelHandSlot*)datum("D_800EA030"))[i].object=spawn_hand(r,14+i*60,194);}
 *(uint8_t*)datum("D_8009B368")=2;d[45].id=680;uint16_t row[]={680,6,47,571,705,0};memcpy(datum("gDuel_awRitualData"),row,sizeof row);
 Mods_Reset();((void(*)(void))engine_function("Duel_InitScene"))();*(uint16_t*)datum("gDuel_wSceneStateFlags")=4;
 memset(engine_calls,0,sizeof engine_calls);for(int i=0;i<nr;i++)registrations[i].calls=0;setup_count=spawn_count=after_calls=planned_pending=0;swap_path[0]=0;
}
static int tick(void){*(uint16_t*)datum("D_8009B162")=0;((void(*)(void))engine_function("DuelScene_UpdateHandActions"))();return *(uint16_t*)datum("gDuel_wSceneStateFlags")&0x3fff;}
static int run_action(void){for(int i=0;i<160;i++){int phase=tick();if(i<4||phase!=4)printf("STEP frame=%d phase=%d substate=%u selection=%u\n",i,phase,*(uint16_t*)datum("D_8009B20C"),((uint8_t*)datum("D_800EAE88"))[0]);if(!strcmp(boundary,"after")&&armed&&after_calls){armed=0;printf("FRAME_BOUNDARY planner and swap already returned; substate=%u pending=%d\n",*(uint16_t*)datum("D_8009B20C"),pull()->active);manager_change();}
 if(phase!=4)return phase;}return 4;}
static int count_hand(void){int n=0;DuelSideState*s=datum("D_800E9FF0");for(int i=0;i<5;i++)n+=s[1].hand[i]>=0;return n;}
static DisplayObject*action_object(void){DisplayObject**o=datum("D_800E9EF0");for(int i=0;i<7;i++)if(o[i]&&o[i]->field_6A>=15&&o[i]->field_6A<20)return o[i];return NULL;}
static int action_card(void){DisplayObject*o=action_object();return o?((DuelCardRecord*)datum("D_801A7AD8"))[o->field_6A].card_id:0;}
static void placement_model(void){
 DisplayObject*o=action_object();CHECK(o);DuelCardRecord*r=datum("D_801A7AD8");int target=*(uint8_t*)datum("D_8009B19C"),origin=o->field_6A;CHECK(target>=0&&target<30);r[target]=r[origin];r[target].object=o;o->field_6A=target;printf("MODEL_PLACEMENT target=%d card=%d origin=%u\n",target,r[target].card_id,((DuelDeckCardRecord*)r[target].data)->data_block_index);
}
static int conserved(void){int count[80]={0};DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");for(int i=0;i<80;i++){if(d[i].deck_index!=i||d[i].data_block_index>=80)return 0;count[d[i].data_block_index]++;}for(int i=0;i<80;i++)if(count[i]!=1)return 0;return 1;}
static void next_hand_model(void){
 // Explicit next-action hand rebuild/draw model, not a complete native turn.
 DuelSideState*s=datum("D_800E9FF0");int kept[5],n=0;for(int i=0;i<5;i++)if(s[1].hand[i]>=0)kept[n++]=s[1].hand[i];
 while(n<5){CHECK(s[1].deck_draw_cursor<40);kept[n++]=40+s[1].deck_draw_cursor++;}
 ((void(*)(void))engine_function("Duel_ClearHandSlots"))();
 for(int i=0;i<5;i++){s[1].hand[i]=kept[i];DuelCardRecord*r=setup_record(15+i,kept[i]);((DuelHandSlot*)datum("D_800EA030"))[i].object=spawn_hand(r,14+i*60,194);}
 s[1].rank.turns_taken++;*(uint16_t*)datum("gDuel_wSceneStateFlags")=4;*(uint16_t*)datum("D_8009B174")=0;*(uint16_t*)datum("D_8009B20C")=0;
 printf("MODEL_NEXT_HAND cursor=%d hand=%d,%d,%d,%d,%d\n",s[1].deck_draw_cursor,s[1].hand[0],s[1].hand[1],s[1].hand[2],s[1].hand[3],s[1].hand[4]);
}
typedef struct {int owner;size_t size;unsigned version;unsigned char*bytes;} Chunk;
static Chunk chunks[8];static int nchunks,overlap;
static void capture_state(int owner,void*data,size_t size,unsigned version,void*context){
 (void)context;CHECK(nchunks<8);Chunk*c=&chunks[nchunks++];*c=(Chunk){owner,size,version,malloc(size)};CHECK(c->bytes);memcpy(c->bytes,data,size);
 uintptr_t lo=(uintptr_t)data,hi=lo+size,p=(uintptr_t)pull();if(p<hi&&p+sizeof(Pull)>lo)overlap=1;
 printf("SAVE_CHUNK owner=%s size=%u version=%u includes_pending=%d\n",Mods_Id(owner),(unsigned)size,version,p<hi&&p+sizeof(Pull)>lo);
}
static void restore_state(int owner,void*data,size_t size,unsigned version,void*context){(void)context;for(int i=0;i<nchunks;i++)if(chunks[i].owner==owner){CHECK(chunks[i].size==size&&chunks[i].version==version);memcpy(data,chunks[i].bytes,size);return;}CHECK(0);}
static void aftermath(const char*which_action){
 begin(which_action);armed=0;boundary="none";
 if(!strcmp(which_action,"next")){
  int stale=pull()->active;placement_model();target_ai=0;change="direct";manager_change();next_hand_model();stock_choice=14;setup_count=spawn_count=0;for(int i=0;i<nr;i++)registrations[i].calls=0;
  int phase=run_action(),consumed=action_card();printf("FOLLOWUP stale_before=%d planner_selection=%d final_selection=%u consumed=%d pending_after=%d swaps=%u phase=%d\n",stale,plan_selection,((uint8_t*)datum("D_800EAE88"))[0],consumed,pull()->active,setup_count,phase);
  result("next-plan-physical-selection-preserved",((uint8_t*)datum("D_800EAE88"))[0]==plan_selection,"Conditional next-turn model: completed old pending must not replace a newer selection with its old fallback.");
  result("no-second-deck-exchange",setup_count==0&&conserved(),"No duplicate offscreen swap or card identity creation observed in follow-up.");
 }else if(!strcmp(which_action,"reset")){
  Mods_Reset();pending_log("after-reset");result("real-reset-clears-pending",!pull()->active,"Actual registered RD reset callback.");
 }else if(!strcmp(which_action,"cancel")){
  ((void(*)(void*))engine_function("AiScript_Init"))(datum("D_801A8000"));pending_log("after-script-init");
  printf("CANCEL_LIMIT actual VM initializer clears selection; not a user AI-turn cancel event\n");
  result("new-script-init-clears-pending",!pull()->active,"Diagnostic: VM init is not RD reset; no arbitrary pending clear is performed.");
 }else if(!strcmp(which_action,"deal")||!strcmp(which_action,"deal-unregistered")){
  uint16_t cards[40]={0};uint8_t order[40]={0};if(!strcmp(which_action,"deal-unregistered"))*(int8_t*)datum("gDuel_bOpponentID")=16;
  ((void(*)(int,uint8_t*,uint8_t*))engine_function("Duel_ShuffleDeck"))(0,(uint8_t*)cards,order);pending_log("after-dealer");
  result("new-deal-clears-pending",!pull()->active,"Actual RD dealer wrapper; catalog/deck source remains modeled. No full new-duel claim.");
 }else if(!strcmp(which_action,"save")){
  MemoriesModEvent event={MEMORIES_EVENT_SAVE,MEMORIES_BEFORE,0,0,0,0,0};Mods_Dispatch(&event);unsigned signature=Mods_Signature();Mods_VisitState(capture_state,NULL);
  result("pending-not-in-registered-save",!overlap&&nchunks>0,"Only actual registered chunks captured; no private object/BSS persistence invented.");
  Mods_VisitState(restore_state,NULL);Mods_Reset();event=(MemoriesModEvent){MEMORIES_EVENT_LOAD,MEMORIES_AFTER,0,0,0,0,0};Mods_Dispatch(&event);
  printf("SIMULATED_LOAD signature=%08x pending_after=%d\n",signature,pull()->active);result("supported-load-order-clears-pending",!pull()->active,"Restore registered blocks, then actual Mods_Reset/LOAD as official state.c; same profile only.");
 }else CHECK(!strcmp(which_action,"none"));
}
int main(int argc,char**argv){
 CHECK(argc==9);setvbuf(stdout,NULL,_IOLBF,0);CHECK(mkdtemp(data_directory));memory_init(argv[1]);trace_memory=mmap(NULL,65536,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(trace_memory!=MAP_FAILED);
 native_load(argv[2],"duel_field_layout","");native_load(argv[2],"ai_opponent_data","");native_load(argv[2],"duel_scene_hand_actions","DuelScene_UpdateHandActions");native_load(argv[2],"func_8001B938","func_8001B938,func_8001BAF0");native_load(argv[2],"duel_card_state_helpers","func_80028220");native_load(argv[2],"func_80027DF8","func_80027DF8");native_load(argv[2],"ai_script_vm","AiScript_Init");native_load(argv[2],"ai_get_hand_size","Ai_GetHandSize");native_load(argv[2],"func_8001B7AC","func_8001B780,func_8001B7AC");native_load(argv[2],"duel_state_init","Duel_ClearHandSlots");
 prepare_exports();*(int*)datum("gCard_nCount")=722;Settings_Load();Mods_Load();CHECK(mi(H)>=0&&mi(D)>=0&&mi(RD)>=0&&mi(OBS)>=0);for(int i=0;i<Mods_Count();i++)if(Mods_Failed(i))fprintf(stderr,"MOD_FAILED %s %s\n",Mods_Id(i),Mods_Status(i));CHECK(!Mods_Failed(mi(RD)));
 printf("BOOT phase=%s pid=%ld pending=%d H-want=%d H-active=%d loads=%u RD-state=%u/v%u\n",argv[8],(long)getpid(),pull()->active,Mods_Enabled(mi(H)),Mods_Active(mi(H)),mod_loads[H],(unsigned)modules[RD].state_size,modules[RD].state_version);
 result("fresh-library-has-no-pending",!pull()->active,"Fresh process/library BSS has no prior pending; settings do not contain this action.");
 if(atoi(argv[8])!=0){CHECK(Settings_Save());printf("SUMMARY assertions=%u failures=%u reboot-only\n",assertions,failures);return failures?1:0;}
 change=argv[3];boundary=argv[4];int off_on=!strcmp(argv[5],"off-on");target_ai=off_on?1:0;
 if(off_on&&atoi(argv[6]))Mods_SetEnabled(mi(H),0);
 int active_before=Mods_Active(mi(H));setup();unsigned signature_before=Mods_Signature();
 if(!strcmp(boundary,"before")){printf("FRAME_BOUNDARY before planning\n");manager_change();armed=0;}else armed=strcmp(change,"none")!=0;
 begin("controller-pending-lifecycle");int phase=run_action(),first_pending=planned_pending,first_stale=pull()->active,card=action_card();pending_log("after-commit");
 printf("OBSERVED phase=%d pending-created=%d pending-after=%d H-before=%d H-after=%d RDswap=%u Hswap=%u swaps=%u consumed=%d hand=%d path=%s\n",phase,first_pending,first_stale,active_before,Mods_Active(mi(H)),calls(RD,"func_8001BAF0"),calls(H,"func_8001BAF0"),setup_count,card,count_hand(),swap_path);
 if(first_pending)result("pending-consumed-by-materializer",!first_stale,"Real RD planner -> actual controller/swap. Stale pending remains FAIL; no private action seed.");else puts("NOT_REACHED pending-specific-check planner did not stage a pending; retain as negative/lifecycle control");
 result("controller-action-committed",(phase==7||phase==6)&&count_hand()==4,"Actual native hand controller commits one physical selection; effect/placement completion not native gameplay.");
 result("at-most-one-offscreen-exchange",setup_count<=1&&conserved(),"Actual worker exchange count and all 80 data-block identities preserved.");
 if(Mods_RequiresRestart(mi(H)))result("restart-defers-actual-state-change",Mods_Active(mi(H))==active_before,"Candidate restart policy keeps actual AI state throughout the in-process change.");
 if(!strcmp(boundary,"before")||!strcmp(boundary,"after")||!strcmp(boundary,"none"))result("frame-boundary-has-no-abandoned-pending",!first_stale,"No manager change injected between planner return and swap in this control.");
 printf("PROFILE_SIGNATURE before=%08x after=%08x compatible=%d\n",signature_before,Mods_Signature(),signature_before==Mods_Signature());
 aftermath(argv[7]);CHECK(Settings_Save());Mods_Shutdown();printf("SUMMARY assertions=%u failures=%u\n",assertions,failures);return failures?1:0;
}
