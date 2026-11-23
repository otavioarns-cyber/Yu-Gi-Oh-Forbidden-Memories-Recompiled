/* Prefix for the literal state.c:load function, extracted without edits.
 * Post-parse/context paths trap: this proves refusal, never a successful load. */
#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ucontext.h>
#include "pc/guest/state.h"
#include "pc/guest/state_io.h"
#include "pc/mods/mods.h"
#include "pc/memory_map.h"
#include "pc/tower/tower.h"
#define VERSION 3u
#define SYSTEM "Linux"
#define STACK_BASE MEMORIES_GAME_STACK_BASE
#define STACK_SIZE MEMORIES_GAME_STACK_SIZE
#define STACK_TOP (STACK_BASE+STACK_SIZE)
#define OLD_LINUX_STACK_BASE 0x70000000u
#define SCRATCHPAD_SIZE MEMORIES_GUEST_SCRATCHPAD_SIZE
static unsigned active,resident,seeks,reads,allocs,parses,contexts;
static const char*ids[]={"melhoria-dos-rituais","ritual-card-effects-test","ritual-duelists","fusion-auxiliary-balance","field-effects-secondary-types","ai-hard-mode","dual-play","time-wizard-time-magic"};
int Mods_Count(void){return 8;}const char*Mods_Id(int n){return ids[n];}
int Mods_Active(int n){return (active>>n)&1;}int Mods_CodeResident(int n){return (resident>>n)&1;}
static int tower_refused;static unsigned build_id=0x1234;
static uint8_t*pending_image;static size_t pending_size;
static ucontext_t game_context,service_context;
static char reason[512];
static void refuse(const char*fmt,...){va_list a;va_start(a,fmt);vsnprintf(reason,sizeof reason,fmt,a);va_end(a);}
int Tower_Guard(int what,int tell,char*why,size_t size){(void)what;(void)tell;(void)why;(void)size;return 0;}
static int counted_seek(FILE*f,long off,int whence){++seeks;return fseek(f,off,whence);}
static size_t counted_read(void*p,size_t size,size_t count,FILE*f){++reads;return fread(p,size,count,f);}
static void*counted_alloc(size_t n){++allocs;return malloc(n);}
const uint8_t*Memories_StateFindChunk(const MemoriesState*s,const char*name,size_t*n){(void)s;(void)name;*n=0;++parses;return NULL;}
static int relocate(uint8_t*a,size_t b,uint32_t c){(void)a;(void)b;(void)c;abort();}
int Memories_StateCompatibleMods(MemoriesState*s){(void)s;abort();}
static int compatible_language(const MemoriesState*s,const char*p){(void)s;(void)p;abort();}
static int counted_context(ucontext_t*a,ucontext_t*b){(void)a;(void)b;++contexts;abort();}
#define fseek counted_seek
#define fread counted_read
#define malloc counted_alloc
#define swapcontext counted_context
