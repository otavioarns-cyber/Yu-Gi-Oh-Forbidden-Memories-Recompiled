#include "historical_planner_fixture.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "planner.h"
#include "field_guard.h"
#include "ritual_duelists.h"
#include "game/card_constants.h"

static RdCard monster(int id,int atk){RdCard c={0};c.id=id;c.atk=atk;c.def=atk;c.type=9;return c;}
int main(void)
{
    RdBoard b;RdPlan p;unsigned char sel[12]={0};int i;
    memset(&b,0,sizeof b);memset(&p,0,sizeof p);
    b.actual_hand=b.hand_count=2;
    b.own[0]=monster(378,2000); /* Swordstalker, required Warrior */
    b.hand[0]=monster(600,900); /* ordinary non-Ritual play */
    b.hand[1]=monster(535,500); /* Kamakiriman in hand */
    p.prep=1;p.protect_field[0]=1;
    sel[0]=11;sel[6]=1;
    /* Regression: the AI replaces the lone Warrior despite a set Ritual;
       another free zone must be chosen instead of discarding him. */
    assert(Rd_FieldGuardDestination(&b,&p,sel)==2);
    sel[6]=2;assert(Rd_FieldGuardDestination(&b,&p,sel)==0);
    sel[6]=1;
    p.prep=0;assert(Rd_FieldGuardDestination(&b,&p,sel)==0);p.prep=1;
    p.urgent=1;assert(Rd_FieldGuardDestination(&b,&p,sel)==0);p.urgent=0;
    /* Equips targeting a reserved monster are beneficial and MUST remain. */
    b.hand[0].type=CARD_TYPE_EQUIP;
    assert(Rd_FieldGuardDestination(&b,&p,sel)==0);
    b.hand[0].type=9;
    /* An undrawn logical card cannot become a playable choice. */
    sel[0]=16;assert(Rd_FieldGuardDestination(&b,&p,sel)==0);
    sel[0]=11;
    /* Full field: use weakest UNPROTECTED zone rather than the Warrior. */
    for(i=1;i<5;i++)b.own[i]=monster(400+i,(i==3)?100:1200);
    assert(Rd_FieldGuardDestination(&b,&p,sel)==4);
    p.protect_field[3]=1;
    assert(Rd_FieldGuardDestination(&b,&p,sel)==2);
    for(i=0;i<5;i++)p.protect_field[i]=1;
    assert(Rd_FieldGuardDestination(&b,&p,sel)==-1);
    /* Never invent a nonphysical source, nor interpret a Ritual Magic as a monster. */
    b.hand[0].id=0;assert(Rd_FieldGuardDestination(&b,&p,sel)==0);
    /* End-to-end planner contract for the user report: reserved Warrior,
       Novox already set, Kamakiriman in hand, Skelgon still missing. */
    {
        const RdOpponentProfile *profile=historical_planner_fixture();
        RdPlan computed;
        assert(profile);
        memset(&b,0,sizeof b);
        b.own_lp=b.enemy_lp=8000;
        b.own[0]=monster(378,2000);b.own[0].type=3;
        b.back[0].id=679;
        b.hand[0]=monster(600,1000);
        b.hand[1]=monster(535,500);
        b.actual_hand=b.hand_count=2;
        computed=Rd_PlanRitual(profile,0,&b,0,0);
        assert(computed.prep && computed.protect_field[0]);
        memset(sel,0,sizeof sel);sel[0]=11;sel[6]=1;
        assert(Rd_FieldGuardDestination(&b,&computed,sel)==2);
    }
    /* Desert Mage regression: a ritualized Dokurorider must not be consumed
       by an unrelated hand-on-field fusion, even without a Beacon callback.
       The face flag can lag behind the Ritual presentation. */
    memset(&b,0,sizeof b);memset(&p,0,sizeof p);memset(sel,0,sizeof sel);
    b.actual_hand=b.hand_count=1;
    b.own[3]=monster(719,1900);b.own[3].face_down=1;
    b.hand[0]=monster(98,600);
    sel[0]=11;sel[6]=4;
    p.urgent=1; /* defensive emergency must NOT permit self-fusion */
    assert(Rd_ProtectRitualResult(&b,&p,719));
    assert(p.prep && !p.urgent && p.protect_field[3]);
    assert(Rd_FieldGuardDestination(&b,&p,sel)==1);
    b.own[0]=monster(99,900);b.own[1]=monster(545,2300);
    b.own[2]=monster(200,1100);b.own[4]=monster(77,1400);
    assert(Rd_FieldGuardDestination(&b,&p,sel)==1); /* weakest expendable */
    assert(!Rd_ProtectRitualResult(&b,&p,704));
    puts("PASS: protected Field Warrior and post-Ritual Dokurorider survive stock fusion placement; Equip/emergency/full-field checks");
    return 0;
}
