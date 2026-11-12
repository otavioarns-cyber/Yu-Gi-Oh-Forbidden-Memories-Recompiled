/* Targeted host mock. Exercises the ACTUAL Patch 03 logic without a Windows
 * executable; no claim of actual on-device savefile/UI validation. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
static char test_data[]="/tmp/rd-drop-test-XXXXXX";
#include "../rd_drop_rewards.h"
typedef uint16_t u16;typedef uint8_t u8;typedef int8_t s8;typedef int16_t s16;
#define CARD_DROPS_MAX 99
#define CARD_DROPS_FIRST_PAGE 3
#define TABLES_POOL_POW 1
#define MEMORIES_BEFORE 0
#define MEMORIES_AFTER 1
#define RD_DROP_REWARDS_TEST 1
typedef struct {int16_t count,page;uint16_t cards[98];uint8_t fresh[98];uint8_t text[1024];} MockDrops;
typedef struct {uint16_t weights[722];} MockTable;
typedef struct {int dropped_card_id,rank_tier,is_tec_rank;} MockResult;
typedef MockResult DuelResultDisplayState;
static MockTable gDuel_awSaPowCardDrops[3];
static MockDrops gCardDrops;
static MockResult gDuel_awRitualData;
static MockResult *D_8009B1E8=&gDuel_awRitualData;
static int D_8009B360[2]={-1,0};
static unsigned char D_8009B368=6;
static uint8_t gDuel_bWinnerSide=0;
static int8_t gDuel_bOpponentID=7;
static int gDuel_wSceneStateFlags=0x8000;
static int16_t gDuel_wSelectedCardID;
static int D_801D56A8[2];
static int gDuel_awPlayerDeck[2];
static unsigned char chest[723];
static int roll_card, roll_fallback=4, card_count=1, drawn;
static int guard_counter;
static MemoriesModCallback cb_reward,cb_load,cb_save;
static int mock_roll(int pool){(void)pool;drawn++;return drawn==1?roll_card:roll_fallback;}
static int Duel_SelectCardDrop(int pool){return mock_roll(pool);}
static int CardDrops_Count(void){return card_count;}
static int Rand_GetInterval(int max){return guard_counter++%max;}
static unsigned char *Cards_ChestSlot(void *ignored,int id){(void)ignored;return &chest[id];}
static const unsigned short *Tables_Pool(int pool,const unsigned short *retail){(void)pool;(void)retail;return NULL;}
static void CardDrops_ComposePage(int page){(void)page;}
static int hook(const MemoriesModHost *h,void *orig,void *repl,void **store){
 (void)h;(void)orig;(void)repl;*store=(void*)mock_roll;return 1;
}
static int subscribe(const MemoriesModHost *h,unsigned kind,int priority,MemoriesModCallback cb){
 (void)h;(void)priority;
 switch(kind){case MEMORIES_EVENT_REWARD:cb_reward=cb;break;
 case MEMORIES_EVENT_SLOT_LOAD:cb_load=cb;break;
 case MEMORIES_EVENT_SLOT_SAVE:cb_save=cb;break;default:return 0;}return 1;
}
static FILE *open_data(const MemoriesModHost *h,const char *name,const char *mode){
 char path[180];(void)h;snprintf(path,sizeof(path),"%s/%s",test_data,name);return fopen(path,mode);
}
static void logcb(const MemoriesModHost *h,const char *s,...){(void)h;(void)s;}
#include "../rd_drop_rewards.c"
static int start_roll(int id,int duelist,int mode,int pool){
 roll_card=id;drawn=0;gDuel_bOpponentID=duelist;D_8009B368=mode;
 gDuel_bWinnerSide=0;D_8009B360[0]=-1;gCardDrops.count=0;card_count=1;
 return rd_select_drop(pool);
}
static void deliver(int card){
 MemoriesModEvent e={0};e.type=MEMORIES_EVENT_REWARD;e.phase=MEMORIES_BEFORE;e.a=card;cb_reward(&e);
 chest[card]++;e.phase=MEMORIES_AFTER;cb_reward(&e);
}
int main(void){
 MemoriesModHost h={0};unsigned bits=0,token=0,pending=0;
 assert(mkdtemp(test_data));
 h.api=12;h.hook=hook;h.subscribe=subscribe;h.open_data=open_data;h.log=logcb;
 assert(Rd_DropRewardsInit(&h,&bits,&token,&pending));
 gDuel_awSaPowCardDrops[0].weights[3]=10;
 assert(start_roll(1,7,6,0)==1);assert(pending==1);
 assert(rd_select_drop(0)==4); /* same-results duplicate rejected */
 deliver(1);assert(bits==1);assert(pending==0);
 Rd_DropRewardsNewDuel();assert(start_roll(1,7,6,0)==4); /* spent Seto */
 assert(start_roll(1,17,6,0)==1); /* Kaiba remains independent */
 assert(start_roll(1,32,6,0)==1); /* Seto 2nd remains independent */
 assert(start_roll(1,36,6,0)==1); /* Seto 3rd unlimited */
 for(int i=0;i<3;i++){assert(start_roll(42,31,2,1)==371+i);}
 assert(start_roll(42,31,2,0)==374);assert(start_roll(42,31,2,2)==667);
 assert(start_roll(42,33,2,0)==217);assert(start_roll(42,33,2,1)==704);
 assert(start_roll(42,34,2,0)==360);assert(start_roll(42,34,2,1)==85);
 assert(start_roll(42,35,2,0)==713);assert(start_roll(42,35,2,1)==35);
 assert(start_roll(42,36,2,0)==380);assert(start_roll(42,36,2,1)==1);
 assert(start_roll(42,37,2,2)==709);assert(start_roll(42,37,2,0)==42);
 assert(start_roll(42,38,2,2)==42); /* no forced Nitemare prize */
 assert(start_roll(42,31,2,0)==374); /* single card */
 card_count=3;gCardDrops.count=0;drawn=0;assert(rd_select_drop(0)==42); /* extras normal */
 gCardDrops.count=2;assert(rd_select_drop(0)==374); /* main SPOILS special */
 assert(start_roll(42,31,6,0)==42); /* Free Duel never gets Story guarantee */
 /* Full uint32_t tokens with high-bit set must be saved, not skipped. */
 gDuel_bOpponentID=7;
 MemoriesModEvent e={0};e.phase=MEMORIES_AFTER;e.type=MEMORIES_EVENT_SLOT_SAVE;e.b=(int)0xDEADBEEF;
 cb_save(&e);assert(token==0xDEADBEEF && bits==1);
 bits=0;e.type=MEMORIES_EVENT_SLOT_LOAD;cb_load(&e);assert(bits==1);
 /* Stale saved result restored before this source was exhausted. */
 gDuel_bOpponentID=7;D_8009B368=6;gDuel_wSceneStateFlags=0x8000;
 gDuel_awRitualData.rank_tier=3;gDuel_awRitualData.is_tec_rank=0;
 gDuel_awRitualData.dropped_card_id=1;gCardDrops.count=1;
 gCardDrops.cards[0]=1;bits=0;pending=1;
 Rd_DropRewardsReset();assert(bits==1);
 assert(gDuel_awRitualData.dropped_card_id==4 && gCardDrops.cards[0]==4);
 /* Isolate another save token; no cross-save contamination. */
 e.b=0x11442233;cb_load(&e);assert(bits==0);
 Rd_DropRewardsNewCampaign();assert(token==0&&bits==0);
 {char file[180];snprintf(file,sizeof file,"%s/blue_eyes_deadbeef.dat",test_data);assert(!unlink(file));assert(!rmdir(test_data));}
 puts("PASS: Story SPOILS, extras, independent Blue-Eyes sources, award-only consumption, slot tokens, savestate rollback, Free Duel");
 return 0;
}
