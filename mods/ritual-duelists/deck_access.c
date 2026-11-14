#include "deck_access.h"
#include "game/card_constants.h"

RdDeckPick Rd_ChooseDeckRitual(const RdBoard *board, const RdPlan *plan,
                               int magic, const int deck_ids[40],
                               int deck_remaining)
{
    RdDeckPick pick={-1,-1,-1};
    int i, candidate=-1, best=0x7fffffff;
    if(!board || !plan || !deck_ids || magic<=0 || plan->urgent ||
       board->actual_hand<=0 || board->actual_hand>5 ||
       deck_remaining<0 || deck_remaining>40)return pick;
    /* A SET Ritual Magic or physical Ritual Magic already exists. */
    for(i=0;i<5;i++)if(board->back[i].id==magic)return pick;
    for(i=0;i<board->actual_hand;i++)if(board->hand[i].id==magic)return pick;
    for(i=0;i<5;i++)if(!board->back[i].id){pick.target_slot=6+i;break;}
    if(pick.target_slot<0)return (RdDeckPick){-1,-1,-1};
    /* Preserve actual tributes, Summoned Skull support and other protected
       cards. Native materialization would blindly discard hand[0]. */
    for(i=0;i<board->actual_hand;i++){
        const RdCard *c=&board->hand[i];
        int power=c->atk>c->def?c->atk:c->def;
        int keep=c->type>=CARD_TYPE_MAGIC?1000:power;
        if(!c->id || plan->protect_hand[i])continue;
        if(keep<best){best=keep;pick.replace_slot=i;}
    }
    if(pick.replace_slot<0)return (RdDeckPick){-1,-1,-1};
    for(i=0;i<deck_remaining;i++){
        int source=11+board->actual_hand+i;
        /* The legacy materializer only recognizes deck-sourced slots >=16.
           Do not pretend indices 11..15 refer to upcoming deck cards. */
        if(source<16 || source>55)continue;
        if(deck_ids[i]==magic){candidate=source;break;}
    }
    if(candidate<0)return (RdDeckPick){-1,-1,-1};
    pick.source_slot=candidate;
    return pick;
}

void Rd_ExchangeDeckRecords(DuelDeckCardRecord *held, DuelDeckCardRecord *upcoming)
{
    DuelDeckCardRecord old;
    s8 index;
    if(!held || !upcoming || held==upcoming)return;
    /* Native func_8001BAF0 swaps deck_index FIRST, then the records. */
    index=held->deck_index;
    held->deck_index=upcoming->deck_index;
    upcoming->deck_index=index;
    old=*held;*held=*upcoming;*upcoming=old;
}

int Rd_DefaultAiWindow(int physical, int remaining)
{
    int available;
    if(physical<0)physical=0;
    if(physical>5)physical=5;
    if(remaining<0)remaining=0;
    if(remaining>40)remaining=40;
    available=physical+remaining;
    return available<20?available:20;
}
