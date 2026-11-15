#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "deck_access.h"
#include "hand_play.h"
static RdCard c(int id,int atk,int type){RdCard x={0};x.id=id;x.atk=atk;x.def=atk;x.type=type;return x;}
int main(void){
    RdBoard b={0};RdPlan p={0};int deck[40]={0};RdDeckPick d;unsigned char sel[12];
    b.actual_hand=b.hand_count=5;
    b.hand[0]=c(535,500,9);p.protect_hand[0]=1;
    b.hand[1]=c(440,1900,3);p.protect_hand[1]=1;
    b.hand[2]=c(545,1200,1);p.protect_hand[2]=1;
    b.hand[3]=c(11,800,3);b.hand[4]=c(12,200,3);
    deck[25]=679;d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);
    assert(d.source_slot==41&&d.replace_slot==4&&d.target_slot==6);
    Rd_SelectFaceDownHandPlay(sel,d.source_slot-11,d.target_slot);
    assert(sel[0]==41&&sel[6]==6&&sel[8]==1);
    b.back[0]=c(679,0,22);d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==-1);
    b.back[0].id=0;b.hand[1]=c(679,0,22);d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==-1);
    b.hand[1]=c(440,1900,3);
    p.protect_hand[3]=p.protect_hand[4]=1;
    d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==-1);
    p.protect_hand[4]=0;
    for(int i=0;i<5;i++)b.back[i]=c(100+i,0,22);
    d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==-1);
    memset(b.back,0,sizeof b.back);
    deck[25]=0;deck[39]=679;d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==-1);
    deck[39]=0;deck[0]=679;d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==16);
    b.actual_hand=b.hand_count=3;p.protect_hand[2]=0;deck[0]=679;
    d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==-1);
    deck[2]=679;d=Rd_ChooseDeckRitual(&b,&p,679,deck,35);assert(d.source_slot==16);
    assert(Rd_DefaultAiWindow(5,35)==20);
    assert(Rd_DefaultAiWindow(5,4)==9);
    assert(Rd_DefaultAiWindow(3,0)==3);
    assert(Rd_DefaultAiWindow(0,40)==20);
    assert(Rd_DefaultAiWindow(9,42)==20);
    /* Native record exchange must preserve physical slot identity and move
       the displaced card back into the real upcoming Deck. */
    {
        DuelDeckCardRecord held={535, 2, 11, 12, 13};
        DuelDeckCardRecord upcoming={679, 23, 21, 22, 23};
        Rd_ExchangeDeckRecords(&held,&upcoming);
        assert(held.id==679 && held.deck_index==2 && held.data_block_index==21);
        assert(upcoming.id==535 && upcoming.deck_index==23 && upcoming.flags_04==12);
        Rd_ExchangeDeckRecords(&held,&upcoming);
        assert(held.id==535 && held.deck_index==2 && upcoming.id==679 && upcoming.deck_index==23);
    }
    /* Only the opponent's upcoming actual cards, not phantom cards. */
    d=Rd_ChooseDeckRitual(&b,&p,679,deck,-1);assert(d.source_slot==-1);
    d=Rd_ChooseDeckRitual(&b,&p,679,deck,41);assert(d.source_slot==-1);
    puts("PASS deck peek: real upcoming slots, safe displaced hand, set facing, bounds and already-held guards");
    return 0;
}
