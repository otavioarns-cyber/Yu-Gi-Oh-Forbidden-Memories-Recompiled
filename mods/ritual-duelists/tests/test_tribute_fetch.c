#include "historical_planner_fixture.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "tribute_fetch.h"
#include "game/card_constants.h"

static RdCard c(int id,int power,int type){RdCard x={0};x.id=id;x.atk=x.def=power;x.type=type;return x;}
static RdRitualRequirement need(int id){
    RdRitualRequirement r={0};r.exact_card=id;r.type=-1;
    r.min_attack=r.max_attack=r.min_defense=r.max_defense=-1;
    r.min_level=r.max_level=-1;return r;
}
static RdAdaptiveRules rules(int field,int hand,int deck,int result){
    RdAdaptiveRules r={0};r.available=1;r.material_count=3;
    r.min_field=field;r.min_hand=hand;r.sources=3|(deck?4:0);
    r.require_result=result;return r;
}
static RdBoard board(void){
    RdBoard b={0};b.actual_hand=b.hand_count=5;
    b.hand[0]=c(535,500,9);b.hand[1]=c(77,1200,1);
    b.hand[2]=c(78,1300,1);b.hand[3]=c(79,1400,1);
    b.hand[4]=c(80,100,1);
    b.back[0]=c(679,0,CARD_TYPE_MAGIC);
    return b;
}
static RdPlan plan(void){RdPlan p={0};p.prep=1;p.ritual_back_slot=0;p.ritual_hand_slot=-1;p.setup_hand_slot=-1;return p;}
static RdTributeFetch fetch(RdBoard *b,RdPlan *p,RdRitualRecipe *r,RdAdaptiveRules *cfg,RdCard d[40],
                            unsigned char sel[12],int legal){
    RdOpponentProfile prof={0};prof.recipe=*r;
    return Rd_ChooseDeckTribute(&prof,r,cfg,b,p,d,35,sel,legal,0,0);
}
int main(void){
    RdRitualRequirement req[3]={need(535),need(440),need(545)};
    RdRitualRecipe rec={0};rec.ritual_magic=679;rec.result_card=704;
    rec.materials=req;rec.material_count=3;
    RdCard deck[40]={0};RdBoard b;RdPlan p;
    RdAdaptiveRules cfg;RdTributeFetch pick;
    unsigned char sel[12]={11,0,0,0,0,0,1,0,1,0,0,0};
    /* Min F=1 H=1: Warrior already on Field; skull requirement comes
       from real undrawn Deck into the PHYSICAL hand, not onto Field. */
    b=board();p=plan();cfg=rules(1,1,1,0);
    b.own[0]=c(440,1900,3);b.hand[0]=c(20,500,9);sel[0]=12;
    deck[7]=c(535,500,9);deck[8]=c(545,1500,2);
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode==RD_FETCH_HOLD && pick.source_slot==23 && pick.card_id==535);
    assert(pick.replace_slot>=2 && pick.replace_slot<=4);
    /* Once all mandatory sources are satisfied, the remaining Skull can
       stay in Deck; do not needlessly expose more tributes. */
    b.hand[0]=c(535,500,9);p.protect_hand[0]=1;
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);assert(pick.mode==RD_FETCH_NONE);
    /* When no Field tribute exists, seek one and SET it face down. */
    b=board();p=plan();cfg=rules(1,1,1,0);p.protect_hand[0]=1;
    deck[8]=c(440,1900,3);sel[0]=12;
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode==RD_FETCH_FIELD && pick.destination==1 && pick.card_id==440);
    /* Against a strong visible attacker, do NOT expose weak materials. */
    b.enemy[0]=c(99,3000,1);
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode!=RD_FETCH_FIELD);
    /* Deck material disabled: physically collect all 3 distinct cards. */
    b=board();p=plan();cfg=rules(1,0,0,0);
    b.own[0]=c(440,1900,3);p.protect_hand[0]=1;
    deck[8]=c(545,1500,2);sel[0]=12;
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode==RD_FETCH_HOLD && pick.card_id==545);
    /* F=1 H=1 with deck source enabled should fetch only the missing
       physical quota; a legal Ritual is ALWAYS left to the real engine. */
    cfg=rules(1,1,1,0);
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,1);assert(pick.mode==RD_FETCH_NONE);
    /* Logical deck selection by a previous mod must not be interleaved. */
    sel[0]=26;
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);assert(pick.mode!=RD_FETCH_HOLD);
    sel[0]=12;
    /* A protected physical hand may not be traded for our selection. */
    for(int i=0;i<5;i++)p.protect_hand[i]=1;
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);assert(pick.mode==RD_FETCH_NONE);
    /* Notional result-in-hand requirement: this is not a tribute. */
    b=board();p=plan();cfg=rules(1,1,1,1);
    b.own[0]=c(440,1900,3);p.protect_hand[0]=1;
    memset(deck,0,sizeof deck);deck[7]=c(704,2500,1);
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode==RD_FETCH_HOLD && pick.card_id==704 && pick.role==-1);
    /* Result in hand not available in Deck: no false materialization. */
    deck[7].id=0;pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode==RD_FETCH_NONE);
    /* Live Settings change is picked up on each plan call. */
    cfg=rules(1,0,0,0);deck[8]=c(545,1500,2);
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode==RD_FETCH_HOLD && pick.card_id==545);
    /* Invalid rule counts, missing recipe, and impossible minima: no action. */
    cfg=rules(2,2,1,0);pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
    assert(pick.mode==RD_FETCH_NONE);
    cfg=rules(1,0,0,0);cfg.available=0;
    pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);assert(pick.mode==RD_FETCH_NONE);
    /* Use Isis's real recipe descriptor, including the skull-group matcher
       and the constrained Warrior type. No hardcoded material categories
       are allowed in the generic Deck search itself. */
    {
        const RdOpponentProfile *isis=historical_planner_fixture();
        assert(isis && Rd_IsSkull(isis,545));
        b=board();p=plan();cfg=rules(1,1,1,0);
        b.own[0]=c(378,2150,3);  /* synthetic qualifying Warrior */
        b.hand[0]=c(88,300,9);
        memset(deck,0,sizeof deck);
        deck[7]=c(535,500,9);
        deck[8]=c(545,1700,1);
        sel[0]=12;
        pick=Rd_ChooseDeckTribute(isis,&isis->recipe,&cfg,&b,&p,deck,35,sel,0,0,0);
        assert(pick.mode==RD_FETCH_HOLD && (pick.card_id==535 || pick.card_id==545));
    }
    /* Every combination of settings must produce either a valid, physically
       representable exchange or abstain without affecting the game state. */
    {
        int f,h,d,result,count=0;
        for(f=0;f<=3;f++)for(h=0;h<=3;h++)
            for(d=0;d<=1;d++)for(result=0;result<=2;result++){
                b=board();p=plan();cfg=rules(f,h,d,result);
                b.own[0]=c(440,1900,3);
                p.protect_hand[0]=1;
                memset(deck,0,sizeof deck);
                deck[10]=c(545,1500,2);
                deck[11]=c(704,2500,1);
                sel[0]=12;
                pick=fetch(&b,&p,&rec,&cfg,deck,sel,0);
                if(f+h>3){assert(pick.mode==RD_FETCH_NONE);continue;}
                if(pick.mode!=RD_FETCH_NONE){
                    assert(pick.source_slot>=16 && pick.source_slot<=55);
                    assert(pick.replace_slot>=0 && pick.replace_slot<5);
                    assert(!p.protect_hand[pick.replace_slot]);
                    assert(pick.card_id==545 || pick.card_id==704);
                    if(pick.mode==RD_FETCH_FIELD){
                        assert(pick.destination>=1 && pick.destination<=5);
                        assert(!b.own[pick.destination-1].id);
                    }else assert(pick.mode==RD_FETCH_HOLD && pick.destination==-1);
                }
                count++;
            }
        assert(count>0);
    }
    printf("PASS tribute search: field/hand/deck quotas, distinct roles, real deck, emergency, protected physical hand, prior AI, result requirement, settings and legal engine\n");
    return 0;
}
