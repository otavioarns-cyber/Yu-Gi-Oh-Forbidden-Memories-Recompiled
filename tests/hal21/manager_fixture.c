/* HAL21: real API24 loader/manager/provider registry; synthetic engine.
 * Private leases seeded only for contract tests, never claimed as gameplay. */
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
enum { F,R,NMOD };
typedef struct {const char*id;void*state;size_t state_size;unsigned state_version;LoadedObject object;} Module;
static Module modules[NMOD]={{.id="fusion-auxiliary-balance"},{.id="ritual-card-effects-test"}};
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
static char data_directory[]="/tmp/hal21-XXXXXX";
static int native_ai_result=1,stock_choice=12;

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
static void *body(const char*n){
 if(!strcmp(n,"Duel_ShuffleDeck"))return stock_deal;

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
 if(!rc){int m=-1;for(size_t i=0;i<object->symbol_count;i++){if(!strcmp(object->symbols[i].name,"aux_unlocks"))m=F;if(!strcmp(object->symbols[i].name,"borrowed_origin"))m=R;}if(m>=0){modules[m].object=*object;++mod_loads[m];printf("LIBRARY_INIT owner=%s hash=%08x\n",modules[m].id,object->hash);}}
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
#include "model.h"
#include "origin_effects_v1.h"
#include "ritual_effect_intelligence_v1.h"
typedef struct {unsigned api;int(*eligible)(int);} Encore;
typedef struct {unsigned api;unsigned(*lease)(int);void(*revoke)(int);} Unlock;
typedef struct {int id;void*data;unsigned serial;} Lease;
typedef struct {unsigned api;int(*eligible)(int);} RitualEncoreV1;
typedef struct {unsigned api;int(*eligible)(int);} EncoreProvider;
typedef struct {unsigned api;unsigned(*lease)(int);void(*revoke)(int);} OriginUnlockV1;
static const MemoriesModHost *host,*compat_host;
#include "consumer_slices.h"
extern void Hal21_ForceApplied(int,int);
extern const MemoriesModHost*Hal21_Host(int);
extern void Hal21_CallbackOnly(int,int);
extern void Hal21_TwProbe(const MemoriesModHost*,void(*)(int),int[4]);
static int policy_hide,baseline;
static char identities[8];
typedef struct {void*data;int id;unsigned serial;} Borrowed;
typedef struct {void*data;int id,ritual,in_battle;unsigned attacks,def_visual,protect_visual;} Mark;
static int foreign_enabled=1;
static int foreign_predicate(int id){return foreign_enabled&&id==850;}
static void *local(int module,const char*name){LoadedObject*q=&modules[module].object;for(size_t i=0;i<q->symbol_count;i++)if(!strcmp(q->symbols[i].name,name))return(void*)q->symbols[i].address;return NULL;}
static void live(int slot,int id,void*identity){DuelCardRecord*r=datum("D_801A7AD8");r[slot]=(DuelCardRecord){0};r[slot].card_id=id;r[slot].data=identity;r[slot].flags=DUEL_CARD_FLAG_OCCUPIED;}
static void clear_records(void){memset(datum("D_801A7AD8"),0,30*sizeof(DuelCardRecord));}
static void seed(int slot,int id,void*identity,unsigned serial){live(slot,id,identity);Lease*l=local(F,"aux_unlocks");CHECK(l);l[slot]=(Lease){id,identity,serial};unsigned*p=local(F,"aux_unlock_serial");CHECK(p);*p=serial;}
static void force(int on){Hal21_ForceApplied(mi(F),on);printf("FORCED_APPLIED on=%d actual=%d; bypass restart only for callback contract experiment\n",on,Mods_Active(mi(F)));}
static Encore*encore;static Unlock*unlock;static OriginEffectsV1*origin;static EaProvider*forecast;
static void fresh(void){if(!Mods_Active(mi(F)))force(1);if(mi(R)>=0&&!Mods_Active(mi(R)))Hal21_ForceApplied(mi(R),1);Mods_Reset();clear_records();foreign_enabled=1;}
static unsigned retained(void){return policy_hide||baseline?77:0;}
static int state_chunks;
static void count_chunk(int owner,void*p,size_t bytes,unsigned version,void*context){(void)owner;(void)p;(void)bytes;(void)version;(void)context;++state_chunks;}
static void query_log(const char*where,int slot){printf("CONTRACT %s encore0=%d encore1=%d lease=%u extended=%u forecast=%d ritualEncore=%d wizardEncore=%d borrowed=%u\n",where,encore->eligible(0),encore->eligible(1),unlock->lease(slot),origin->lease(slot),forecast->on(),ritual_encore_eligible(0),musician_present(0),borrowed_lease(slot));}

static void candidate_scope_checks(void){
 fresh();begin("candidate-callback-only-scope");seed(5,495,&identities[0],77);
 CHECK(origin->register_provider("hal21-own-probe",foreign_predicate));
 unsigned char*fo=local(F,"fusion_origin"),*metal=local(F,"metal_immune");CHECK(fo&&metal);fo[5]=metal[5]=1;
 LoadedObject*obj=&modules[F].object;unsigned char*saved=malloc(obj->size);CHECK(saved);memcpy(saved,obj->image,obj->size);
 unsigned char*leases=local(F,"aux_unlocks");unsigned char*applied=local(F,"ea_applied");CHECK(leases&&applied);
 Hal21_CallbackOnly(mi(F),0);int unchanged=1,cleared=1;
 for(size_t i=0;i<obj->size;i++) {unsigned char*at=obj->image+i;if((at>=leases&&at<leases+30*sizeof(Lease))||(at>=applied&&at<applied+sizeof(int)))continue;if(*at!=saved[i])unchanged=0;}
 for(size_t i=0;i<30*sizeof(Lease);i++)if(leases[i])cleared=0;
 result("candidate-OFF-only-owner-and-leases-mutated",unchanged&&cleared&&*(unsigned*)local(F,"aux_unlock_serial")==77,"Byte comparison of entire loaded image except ea_applied/aux_unlocks; serial, independent nonzero fusion/metal marks and provider catalogue preserved.");
 result("candidate-callback-OFF-overrides-host-preference",Mods_Active(mi(F))&&Mods_Enabled(mi(F))&&!encore->eligible(0)&&!unlock->lease(5)&&!origin->lease(5),"Callback-only instrument: host preference still ON; owner applied(0) is authoritative.");
 EaCard card={0};card.id=495;EaCard before=card;int result_id=-9,origin_kind=-9;forecast->read(&card,5,0);
 result("candidate-all-forecast-callbacks-neutral-OFF",!forecast->on()&&!memcmp(&card,&before,sizeof card)&&!forecast->policy(0,0)&&!forecast->pair(1,2,0,&result_id,&origin_kind)&&result_id==-9&&origin_kind==-9,"Public forecast callbacks are gated; no card/result mutation OFF.");
 Hal21_CallbackOnly(mi(F),0);result("candidate-repeated-OFF-idempotent",*(unsigned*)local(F,"aux_unlock_serial")==77&&!unlock->lease(5),"Repeated OFF does not reset counter or restore a lease.");
 Hal21_CallbackOnly(mi(F),1);result("candidate-ON-independent-state-and-registration-preserved",fo[5]==1&&metal[5]==1&&encore->eligible(0)&&forecast->on()&&!unlock->lease(5)&&origin->register_provider("hal21-own-probe",NULL),"Only lease grant ended. Own effects and provider management remain; ON does not recover serial 77.");
 free(saved);Mods_Reset();
}

static void tests(void){
 fresh();begin("owner-ON-ABI");seed(5,495,&identities[0],77);
 result("ABI-versions-layout",encore->api==1&&unlock->api==1&&origin->abi==1&&forecast->abi==2,"Unchanged public v1/v2 layouts; raw ELF API24 init and original provider records.");
 result("ON-public-answers",encore->eligible(0)==1&&encore->eligible(1)==0&&unlock->lease(5)==77&&origin->lease(5)==77&&forecast->on(),"Original valid owner/face-up Musician and identity lease answers.");
 result("ON-real-consumers",ritual_encore_eligible(0)&&musician_present(0)&&borrowed_lease(5)==77,"Verbatim external source consumer functions call real providers through official Mods_Find.");
 begin("original-manifest-restart");Mods_SetEnabled(mi(F),0);
 result("restart-defers-supported-OFF",Mods_RequiresRestart(mi(F))&&!Mods_Enabled(mi(F))&&Mods_Active(mi(F))&&encore->eligible(0),"Original restart:true saves OFF but retains actual ON; not a provider failure.");Mods_SetEnabled(mi(F),1);
 begin("manager-apply-and-reload-defers-OFF");int enabled[2]={0};for(int i=0;i<Mods_Count();i++)enabled[i]=Mods_Enabled(i);enabled[mi(F)]=0;char error[512]={0};CHECK(Mods_Apply(enabled,error,sizeof(error)));Mods_Load();
 result("restart-Apply-reload-keeps-actual-ON",!Mods_Enabled(mi(F))&&Mods_Active(mi(F))&&encore->eligible(0),"Actual Mods_Apply then manager reload preserve runtime state until full restart. Owner state must not use preference host->applied.");Mods_SetEnabled(mi(F),1);
 begin("OFF-cached-provider-F1");force(0);query_log("OFF",5);
 result("Mods-Find-keeps-disabled-providers",Mods_Find("fusion-auxiliary-balance:encore-v1")==encore&&Mods_Find("fusion-auxiliary-balance:origin-unlock-v1")==unlock&&Mods_Find("fusion-auxiliary-balance:origin-effects-v1")==origin,"Official registry leaves cached/public pointers callable while owner OFF.");
 result("OFF-benefit-queries-neutral",!encore->eligible(0)&&!unlock->lease(5)&&!origin->lease(5)&&!forecast->on(),"F1 invariant: preserve baseline FAIL; candidate answers all neutral.");
 result("OFF-consumers-neutral",!ritual_encore_eligible(0)&&!musician_present(0)&&!borrowed_lease(5),"Fresh Ritual/Time Wizard contract queries, not full roulette core or effects.");
 result("OFF-provider-management-still-works",origin->register_provider("hal21-own-probe",foreign_predicate)&&origin->register_provider("hal21-own-probe",NULL),"Registration/unregister are lifecycle management, not gameplay benefit; remain available OFF.");
 force(1);query_log("ON-again",5);result("policy-valid-identity-after-ON",unlock->lease(5)==retained()&&origin->lease(5)==retained()&&encore->eligible(0),"Hide retains/revalidates; revoke does not recreate old lease. Encore is current live presence, not a lease.");
 const char*names[]={"slot-move","controller-transfer","different-identity-same-ID","removed","field-to-hand","face-down","same-pointer-leave-reenter"};
 for(int k=0;k<7;k++){
  fresh();begin(names[k]);seed(5,495,&identities[0],77);force(0);DuelCardRecord*r=datum("D_801A7AD8");int dest=5;
  if(k==0||k==1){dest=k?20:6;r[dest]=r[5];r[5]=(DuelCardRecord){0};}
  if(k==2){r[5]=(DuelCardRecord){0};dest=6;live(dest,495,&identities[1]);}
  if(k==3)r[5].flags=0;
  if(k==4){dest=10;r[dest]=r[5];r[5]=(DuelCardRecord){0};}
  if(k==5)r[5].flags|=DUEL_CARD_FLAG_FACE_DOWN;
  if(k==6){r[5]=(DuelCardRecord){0};dest=6;live(dest,495,&identities[0]);}
  force(1);unsigned got=unlock->lease(dest),want=(k==0||k==1||k==5||k==6)?retained():0;
  if(baseline&&(k==3))want=77;
  printf("IDENTITY case=%s destination=%d lease=%u expected-policy=%u\n",names[k],dest,got,want);
  result("policy-identity-result",got==want,"Current identity/zone/setting revalidation; never substitute another data pointer by equal card ID.");
  if(k==1)result("control-change-follows-identity",!encore->eligible(0)&&encore->eligible(1),"Presence side changes independently of lease storage slot.");
  if(k==5)result("face-down-encore-neutral",!encore->eligible(0),"Face-down origin lease is distinct from live Encore eligibility.");
  if(k==6)result("no-old-lease-after-unobserved-zone-roundtrip",got==0,"Semantic risk control: same data pointer leaves/reenters during OFF; hide has no history/generation to distinguish it.");
 }
 fresh();begin("fresh-lease-after-ON");seed(5,495,&identities[0],77);force(0);force(1);((void(__attribute__((fastcall)) *)(int))local(F,"aux_unlock_bind"))(5);
 result("new-lease-monotonic",unlock->lease(5)==(baseline||policy_hide?77:78),"Real bind callback: revoke does not reset serial or recreate old grant; a new explicit grant obtains 78.");
 fresh();begin("external-producer-disabled-OFF");CHECK(origin->register_provider("hal21-own-probe",foreign_predicate));seed(5,850,&identities[0],77);force(0);foreign_enabled=0;force(1);
 result("external-producer-revalidation",!unlock->lease(5),"Hide rechecks external target predicate on ON; revoke already ended grant. Baseline does not revalidate.");CHECK(origin->register_provider("hal21-own-probe",NULL));
 fresh();begin("egotist-setting-OFF");seed(5,495,&identities[0],77);force(0);
 /* Invoke the owner setting through the real manager. Runtime option indexes are found from JSON. */
 int option=-1;for(int i=0;i<Mods_OptionCount(mi(F));i++){const JsonValue*j=Mods_Option(mi(F),i);const char*k=Json_String(Json_Member(j,"key"),NULL);if(k&&!strcmp(k,"elegant_egotist"))option=i;}
 CHECK(option>=0);CHECK(Mods_OptionSet(mi(F),option,0));force(1);
 result("setting-disabled-on-reactivation",!unlock->lease(5),"Egotist option OFF suppresses lease without altering recipes/settings in either candidate.");CHECK(Mods_OptionSet(mi(F),option,1));
 fresh();begin("copy-isolation-and-explicit-revoke-OFF");seed(5,495,&identities[0],77);seed(6,495,&identities[1],88);force(0);unlock->revoke(5);force(1);result("revoke-cleanup-allowed-OFF",!unlock->lease(5)&&unlock->lease(6)==(retained()?88:0),"Cleanup callback is permitted while OFF; another physical copy never inherits revoked identity.");
 fresh();begin("owner-OFF-new-duel-reset");seed(5,495,&identities[0],77);force(0);Mods_Reset();force(1);
 result("inactive-reset-does-not-revive-old-lease",!unlock->lease(5),"Official Mods_Reset skips inactive owners. Hide can preserve old memory; revoke already cleared it.");
 fresh();begin("reset-and-simulated-load");seed(5,495,&identities[0],77);MemoriesModEvent ev={MEMORIES_EVENT_SAVE,MEMORIES_BEFORE,0,0,0,0,0};Mods_Dispatch(&ev);state_chunks=0;Mods_VisitState(count_chunk,NULL);Mods_Reset();ev=(MemoriesModEvent){MEMORIES_EVENT_LOAD,MEMORIES_AFTER,0,0,0,0,0};Mods_Dispatch(&ev);
 result("load-reset-clears-leases",!unlock->lease(5)&&!origin->lease(5)&&state_chunks==0,"Actual registered-state visitation and callbacks only; no BSS save format invented and no old state compatibility claim.");
 fresh();begin("new-duel-scene-hook");seed(5,495,&identities[0],77);*(uint16_t*)datum("gDuel_wSceneStateFlags")=1;((void(*)(void))engine_function("DuelScene_Update"))();result("active-new-duel-clears-leases",!unlock->lease(5),"Real Fusion scene hook first phase tick, native scene body modeled.");
 fresh();begin("Time-Wizard-bridge-cached-vs-fresh-attempt");live(5,495,&identities[0]);int tw[4]={0};Hal21_TwProbe(host,force,tw);printf("TIME_WIZARD init=%d initially_eligible=%d retry_after_OFF=%d fresh_attempt_retry_OFF=%d\n",tw[0],tw[1],tw[2],tw[3]);
 result("TimeWizard-bridge-ON-unchanged",tw[0]&&tw[1],"Actual supplied bridge source; original compiled core and wheel are modeled boundaries.");
 result("TimeWizard-new-attempt-OFF-neutral",!tw[3],"Fresh attempt queries fixed provider; baseline F1 stays visible.");
 result("TimeWizard-cached-attempt-characterized",tw[2]==1,"Bridge caches ON eligibility. API fix alone does not cancel an in-flight attempt; no TimeWizard workaround implemented.");
 if(mi(R)>=0){
  fresh();begin("real-Ritual-reconcile-borrowed-effect");seed(5,362,&identities[0],77);
  Borrowed*b=local(R,"borrowed_origin");Mark*m=local(R,"marks");unsigned*mask=local(R,"shield_live_mask");CHECK(b&&m&&mask);b[5]=(Borrowed){&identities[0],362,77};m[5]=(Mark){.data=&identities[0],.id=362,.ritual=1};*mask=1u<<5;
  ((void(*)(void))local(R,"borrowed_reconcile"))();result("Ritual-reconcile-ON-preserved",b[5].serial==77&&m[5].ritual,"Real PR11 reconcile keeps matching ON borrow; setup of borrowed origin is explicit fixture instrumentation.");
  force(0);((void(*)(void))local(R,"borrowed_reconcile"))();result("Ritual-reconcile-OFF-retires",!b[5].serial&&!m[5].ritual&&!(*mask&(1u<<5)),"Real PR11 reconcile retires future shield eligibility on neutral lease; baseline F1 remains FAIL. Resolved actions not rolled back.");
  fresh();begin("real-Ritual-provider-disabled-during-Fusion-OFF");RitualEffectIntelligenceV1*rp=Mods_Find("ritual-card-effects-test:effect_intelligence_v1");CHECK(rp&&rp->enabled);
  result("real-Ritual-enabled-contract",rp->abi==1&&rp->enabled(704),"Unmodified PR11 object supplies effect catalogue, not fixture provider.");seed(5,704,&identities[0],77);force(0);Hal21_ForceApplied(mi(R),0);force(1);
  result("ON-revalidates-unavailable-producer",!unlock->lease(5),"Hide must retire no-longer-eligible producer on reactivation; revoke already retired it. Baseline limitation retained.");Hal21_ForceApplied(mi(R),1);
 }
}
int main(int argc,char**argv){CHECK(argc==3);setvbuf(stdout,NULL,_IOLBF,0);baseline=!strcmp(argv[2],"baseline");policy_hide=!strcmp(argv[2],"hide");CHECK(baseline||policy_hide||!strcmp(argv[2],"revoke"));CHECK(mkdtemp(data_directory));memory_init(argv[1]);trace_memory=mmap(NULL,65536,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);CHECK(trace_memory!=MAP_FAILED);prepare_exports();*(int*)datum("gCard_nCount")=900;Settings_Load();Mods_Load();for(int i=0;i<Mods_Count();i++){printf("LOADED %s active=%d status=%s\n",Mods_Id(i),Mods_Active(i),Mods_Status(i));CHECK(!Mods_Failed(i));}
 if(mi(R)>=0){host=compat_host=Hal21_Host(mi(R));}else if(mi(F)>=0){host=compat_host=Hal21_Host(mi(F));}
 encore=Mods_Find("fusion-auxiliary-balance:encore-v1");unlock=Mods_Find("fusion-auxiliary-balance:origin-unlock-v1");origin=Mods_Find("fusion-auxiliary-balance:origin-effects-v1");forecast=Mods_Find("fusion-auxiliary-balance:effect_forecast_v2");
 if(mi(F)<0 || !Mods_Active(mi(F))){result(mi(F)<0?"absent-provider-fallback":"restart-startup-OFF-no-providers",!encore&&!unlock&&!origin&&!forecast&&!ritual_encore_eligible(0)&&!musician_present(0)&&!borrowed_lease(5),"Real manager registry with Fusion absent.");}else{CHECK(encore&&unlock&&origin&&forecast);tests();if(getenv("HAL21_CANDIDATE_CHECKS")&&!strcmp(getenv("HAL21_CANDIDATE_CHECKS"),"1"))candidate_scope_checks();}
 Mods_Shutdown();printf("SUMMARY checks=%u failures=%u; real manager, partial engine\n",assertions,failures);return failures?1:0;}
