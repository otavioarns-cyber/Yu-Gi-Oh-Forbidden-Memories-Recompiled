/* Ritual Duelists — Pack 03: unique Blue-Eyes sources and Story prizes.
 * Port ABI v0.3.0 (API 12). Does NOT modify the underlying random drop tables:
 * normal Free Duel uses the declarative mod.json values. SPOILS is always
 * the final roll in CardDrops_Roll, after any optional extra drops.
 *
 * Blue-Eyes sources are exhausted only when Duel_AwardCard actually adds a
 * chest copy; a roll, displayed SPOILS, or a cancelled result is not enough.
 * Slot file is keyed by the port's renewable save token, not an account-wide
 * flag. State is also present in the already-registered savestate buffer.
 */
#ifndef RD_DROP_REWARDS_TEST
#define D_8009B360_AS_SIDE_ARRAY
#endif
#include "rd_drop_rewards.h"
#ifndef RD_DROP_REWARDS_TEST
#include "game/duel_rewards.h"
#include "game/duel_side_state.h"
#include "game/duel_result_display.h"
#define DUEL_RITUAL_DATA_RESULT_VIEW
#include "game/duel_check_ritual.h"
#include "game/duel_effect.h"
#include "game/text_staging.h"
#include "game/duel_scene_state.h"
#include "game/rand_get_interval.h"
#include "pc/cards/drops.h"
#include "pc/cards/cards.h"
#include "pc/cards/tables.h"
#include "game/save_data.h"
#include "pc/mods/mod_types.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern s8 gDuel_bOpponentID;
#define RD_BEWD 1
#define RD_STORY 2
#define RD_FREE 6
#define RD_TOKEN_MAGIC 0x33444252u /* RBD3 */

typedef struct {uint32_t magic, bits, checksum;} RdSourceFile;
static const MemoriesModHost *rh;
static unsigned *rd_bits, *rd_token, *rd_pending;
static void *old_drop;
static int drop_hook;
static unsigned session_token, session_bits;
static int before_qty, before_valid, pending_source;
static unsigned reserved_bits;

static unsigned source_bit(int id){
    switch(id){case 7:return 1u; case 17:return 2u; case 32:return 4u;default:return 0;}
}
static int live_duel(void){
    return (D_8009B368==RD_FREE || D_8009B368==RD_STORY) &&
           gDuel_bWinnerSide==0 && gDuel_bOpponentID>0 &&
           D_8009B360[0]<0;
}
static int main_roll(void){
    int want=CardDrops_Count();
    if(want<1)want=1;
    if(want>CARD_DROPS_MAX)want=CARD_DROPS_MAX;
    return gCardDrops.count==want-1;
}
static int story_special(int opponent,int pool){
    /* Return 0 for a normal roll. A code-only story rule never changes the
     * weighted probabilities used in Free Duel or the Drop List. */
    switch(opponent){
    case 31: /* Labyrinth Mage */
        if(pool==0)return 374;
        if(pool==1){static const int guard[3]={371,372,373};return guard[Rand_GetInterval(3)];}
        if(pool==2)return 667;
        break;
    case 33: /* Guardian Sebek */
        return pool==0?217:pool==1?704:pool==2?679:0;
    case 34: /* Guardian Neku */
        return pool==0?360:pool==1?85:pool==2?671:0;
    case 35: /* Heishin 2nd */
        return pool==0?713:pool==1?35:pool==2?721:0;
    case 36: /* Seto 3rd, no unique-source restriction */
        return pool==0?380:pool==1?1:pool==2?675:0;
    case 37: /* DarkNite: only TEC, and Chakra is the MONSTER #709 */
        return pool==2?709:0;
    default:break; /* Nitemare and all others keep their ordinary roll. */
    }
    return 0;
}
static int reroll_forbidden(int pool){
    int id=0,tries;
    /* The game already performed this roll. Rejection sampling happens ONLY
     * when a permanently exhausted source tries to pay Blue-Eyes again.
     * Every target POW pool still contains many other valid cards. */
    for(tries=0;tries<128;tries++){
        id=((int (*)(int))old_drop)(pool);
        if(id!=RD_BEWD && id>0)return id;
    }
    /* Defensive unlikely failure: avoid an infinite loop or empty reward.
     * All three POW tables have valid non-Blue-Eyes entries. */
    {
        const u16 *current=Tables_Pool(TABLES_POOL_POW+pool,
                                        gDuel_awSaPowCardDrops[pool].weights);
        int i;
        for(i=2;i<=722;i++){
            if(current?current[i]:gDuel_awSaPowCardDrops[pool].weights[i-1])return i;
        }
    }
    return 0; /* No valid pool: engine already regards 0 as no card. */
}
static int rd_select_drop(int pool){
    unsigned source;
    int id=((int (*)(int))old_drop)(pool);
    if(!rh || !rd_bits || !live_duel() || pool<0 || pool>2)return id;
    source=source_bit(gDuel_bOpponentID);
    if(source && pool==0 && id==RD_BEWD){
        if((*rd_bits & source) || (reserved_bits & source))id=reroll_forbidden(pool);
        else {reserved_bits|=source; *rd_pending=1;} /* result not awarded yet */
    }
    if(D_8009B368==RD_STORY && main_roll()){
        int guaranteed=story_special(gDuel_bOpponentID,pool);
        if(guaranteed>0)id=guaranteed;
    }
    return id;
}
static int qty(void){
    unsigned char *p=Cards_ChestSlot((void*)gDuel_awPlayerDeck,RD_BEWD);
    return p?*p:0;
}
static void path(char out[48],unsigned token){
    snprintf(out,48,"blue_eyes_%08x.dat",token);
}
static void store(unsigned token){
    FILE *f;char name[48];RdSourceFile data;
    if(!rh||!token||!rd_bits)return;
    path(name,token);
    f=rh->open_data(rh,name,"wb");
    if(!f){rh->log(rh,"RD: cannot store Blue-Eyes unique-source state for token %08x",token);return;}
    data.magic=RD_TOKEN_MAGIC;data.bits=*rd_bits & 7u;
    data.checksum=data.magic ^ data.bits ^ token ^ 0x5D82F39Bu;
    if(fwrite(&data,sizeof(data),1,f)!=1)
        rh->log(rh,"RD: write failed for Blue-Eyes save token %08x",token);
    fclose(f);
}
static unsigned load(unsigned token){
    FILE *f;char name[48];RdSourceFile data;
    if(!rh||!token)return 0;
    path(name,token);
    f=rh->open_data(rh,name,"rb");
    if(!f)return 0;
    if(fread(&data,sizeof(data),1,f)!=1){fclose(f);return 0;}
    fclose(f);
    if(data.magic!=RD_TOKEN_MAGIC || data.checksum!=(data.magic^data.bits^token^0x5D82F39Bu))
        return 0;
    return data.bits&7u;
}
/* A savestate can restore a SPOILS page that predates a redeemed Blue-Eyes.
 * Correct its award AND its staged card before it can be displayed/credited.
 * This is needed only on a rollback that restored an older consumed bit.
 * The result pointer is checked against the game's result-only scratch. */
static void correct_restored_results(unsigned fresh_bits){
    unsigned source=source_bit(gDuel_bOpponentID);
    DuelResultDisplayState *screen=D_8009B1E8;
    int pool,i,chosen;
    if(!(fresh_bits&source) || !source || !rd_pending || !*rd_pending ||
       !live_duel() || !screen ||
       screen!=&gDuel_awRitualData || !(gDuel_wSceneStateFlags&0x8000))return;
    pool=screen->rank_tier<3?1:screen->is_tec_rank?2:0;
    if(pool!=0)return; /* unique-source Blue-Eyes came only from POW */
    chosen=0;
    for(i=2;i<=722;i++){
        const u16 *active=Tables_Pool(TABLES_POOL_POW, gDuel_awSaPowCardDrops[0].weights);
        if(active?active[i]:gDuel_awSaPowCardDrops[0].weights[i-1]){chosen=i;break;}
    }
    if(!chosen)return;
    if(screen->dropped_card_id==RD_BEWD){
        screen->dropped_card_id=(s16)chosen;
        gDuel_wSelectedCardID=(s16)chosen;
        D_801D56A8[0]=chosen;
    }
    for(i=0;i<gCardDrops.count;i++){
        if(gCardDrops.cards[i]==RD_BEWD){
            gCardDrops.cards[i]=(u16)chosen;
            gCardDrops.fresh[i]=(u8)(!Cards_ChestSlot((void*)gDuel_awPlayerDeck,chosen) ||
                !*Cards_ChestSlot((void*)gDuel_awPlayerDeck,chosen));
        }
    }
    if(gCardDrops.page>=CARD_DROPS_FIRST_PAGE)CardDrops_ComposePage(gCardDrops.page);
}
static void slot_callback(MemoriesModEvent *e){
    if(!e||e->phase!=MEMORIES_AFTER||!rd_bits||!rd_token)return;
    if(e->type==MEMORIES_EVENT_SLOT_SAVE){
        if(e->b!=0){*rd_token=(unsigned)e->b;session_token=*rd_token;
            session_bits|=*rd_bits&7u;*rd_bits|=session_bits;store(*rd_token);}
    }else if(e->type==MEMORIES_EVENT_SLOT_LOAD){
        *rd_token=(unsigned)e->b;
        session_token=*rd_token;
        session_bits=load(*rd_token);
        *rd_bits=session_bits;
        reserved_bits=0;before_valid=0;
    }
}
static void reward_callback(MemoriesModEvent *e){
    unsigned source;
    if(!e||!rd_bits||!live_duel())return;
    source=source_bit(gDuel_bOpponentID);
    if(!source)return;
    if(e->phase==MEMORIES_BEFORE){
        before_valid=e->a==RD_BEWD && !(*rd_bits&source);
        pending_source=before_valid?(int)source:0;
        if(before_valid)before_qty=qty();
    }else if(e->phase==MEMORIES_AFTER && before_valid && pending_source==(int)source){
        if(e->a==RD_BEWD && qty()>before_qty){
            *rd_bits|=source;
            session_bits|=source;
            if(rd_pending)*rd_pending=0;
            /* Immediate token-side persistence also prevents replaying an
             * earlier SPOILS savestate after a successful award. */
            if(*rd_token)store(*rd_token);
        }
        before_valid=0;pending_source=0;
    }
}
int Rd_DropRewardsInit(const MemoriesModHost *host,unsigned *consumed,unsigned *token, unsigned *pending){
    if(!host||host->api<4||!consumed||!token||!pending)return 0;
    rh=host;rd_bits=consumed;rd_token=token;rd_pending=pending;
    drop_hook=host->hook(host,(void*)Duel_SelectCardDrop,(void*)rd_select_drop,&old_drop);
    if(!drop_hook)return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_REWARD,800,reward_callback))return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_SLOT_SAVE,800,slot_callback))return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_SLOT_LOAD,800,slot_callback))return 0;
    session_token=*rd_token;session_bits=*rd_bits&7u;
    return 1;
}
void Rd_DropRewardsNewCampaign(void){
    if(rd_bits)*rd_bits=0;
    if(rd_token)*rd_token=0;
    if(rd_pending)*rd_pending=0;
    session_bits=session_token=reserved_bits=0;
    before_valid=pending_source=0;
}
void Rd_DropRewardsNewDuel(void){
    if(rd_pending)*rd_pending=0;
    reserved_bits=0;before_valid=pending_source=0;
}
void Rd_DropRewardsReset(void){
    /* The host has restored the mod's state alongside the game state. Do not
     * let a reload of a pre-award savestate resurrect an already redeemed
     * source. Never merge unrelated save-token state. */
    if(!rd_bits||!rd_token)return;
    if(session_token!=*rd_token){
        session_token=*rd_token;
        session_bits=load(*rd_token);
    }
    {unsigned old=*rd_bits;
     *rd_bits=(*rd_bits|session_bits|load(*rd_token))&7u;
     correct_restored_results(*rd_bits & ~old);
    }
    session_bits=*rd_bits;
    reserved_bits=0;before_valid=pending_source=0;
}
int Rd_BlueEyesSpent(int duelist){
    unsigned bit=source_bit(duelist);
    return bit && rd_bits && (*rd_bits&bit)!=0;
}
