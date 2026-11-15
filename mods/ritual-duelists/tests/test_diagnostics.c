#include <stdio.h>
#include <string.h>
#include "diagnostic_core.h"
#define REQUIRE(test) do { if(!(test)) {fprintf(stderr,"FAIL: %s line %d\n",#test,__LINE__);return 1;} }while(0)
int main(void)
{
    RdBoard b;RdMagicAccess a;memset(&b,0,sizeof(b));
    b.actual_hand=5;b.hand_count=10;
    Rd_FindRitualMagic(&b,679,&a);
    REQUIRE(a.hand_slot==-1 && a.back_slot==-1 && a.window_slot==-1);
    b.hand[7].id=679;
    Rd_FindRitualMagic(&b,679,&a);
    REQUIRE(a.window_slot==7 && a.hand_slot==-1 && a.back_slot==-1);
    b.hand[3].id=679;
    Rd_FindRitualMagic(&b,679,&a);
    REQUIRE(a.window_slot==7 && a.hand_slot==3);
    b.back[2].id=679;
    Rd_FindRitualMagic(&b,679,&a);
    REQUIRE(a.back_slot==2);
    b.back[2].used=1;
    Rd_FindRitualMagic(&b,679,&a);
    REQUIRE(a.back_slot==-1);
    b.actual_hand=4;b.hand_count=4;
    Rd_FindRitualMagic(&b,679,&a);
    REQUIRE(a.hand_slot==3 && a.window_slot==-1);
    b.actual_hand=0;b.hand_count=0;
    Rd_FindRitualMagic(&b,679,&a);
    REQUIRE(a.hand_slot==-1 && a.window_slot==-1);
    Rd_FindRitualMagic(0,679,&a);
    REQUIRE(a.hand_slot==-1 && a.back_slot==-1 && a.window_slot==-1);
    REQUIRE(strstr(Rd_WhyText(RD_WHY_LOGICAL_ONLY),"logical")!=0);
    REQUIRE(strstr(Rd_WhyText(RD_WHY_ACTIVATE_REQUEST),"requested")!=0);
    REQUIRE(strstr(Rd_WhyText(RD_WHY_POST_RITUAL_SUPPORT),"Beacon")!=0);
    REQUIRE(strstr(Rd_WhyText(RD_WHY_DECK_POST_RITUAL_SUPPORT),"Deck")!=0);
    REQUIRE(strcmp(Rd_OriginText(RD_ORIGIN_CHAIN),"Previous AI chain")==0);
    puts("PASS: access window vs physical hand/back; no false playable status; decision labels");
    return 0;
}
