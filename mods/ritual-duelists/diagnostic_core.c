#include "diagnostic_core.h"
void Rd_FindRitualMagic(const RdBoard *board,int magic,RdMagicAccess *out)
{
    int i,physical,logical;
    if(!out)return;
    out->hand_slot=out->back_slot=out->window_slot=-1;
    if(!board||magic<=0)return;
    physical=board->actual_hand;
    logical=board->hand_count;
    if(physical<0)physical=0;
    if(physical>5)physical=5;
    if(logical<physical)logical=physical;
    if(logical>RD_HAND)logical=RD_HAND;
    for(i=0;i<physical;i++)
        if(board->hand[i].id==magic){out->hand_slot=i;break;}
    for(i=0;i<5;i++)
        if(board->back[i].id==magic&&!board->back[i].used){out->back_slot=i;break;}
    for(i=physical;i<logical;i++)
        if(board->hand[i].id==magic){out->window_slot=i;break;}
}
const char *Rd_WhyText(RdDecisionWhy why)
{
    switch(why){
        case RD_WHY_NO_RECIPE:return "Recipe unavailable";
        case RD_WHY_MAGIC_UNSEEN:return "Ritual Magic not accessible";
        case RD_WHY_LOGICAL_ONLY:return "Magic only in logical window";
        case RD_WHY_ENGINE_REJECTED:return "Engine did not authorize Ritual";
        case RD_WHY_SCORE_LOW:return "Ritual score below threshold";
        case RD_WHY_NO_TRIGGER:return "No playable Ritual action chosen";
        case RD_WHY_PREVIOUS_RITUAL:return "Previous AI already chose Ritual";
        case RD_WHY_PREVIOUS_LETHAL:return "Preserved prior lethal attack";
        case RD_WHY_SET_REQUEST:return "Our AI requested Ritual set";
        case RD_WHY_ACTIVATE_REQUEST:return "Our AI requested activation";
        case RD_WHY_PRESERVE_MATERIAL:return "Our AI protected material";
        case RD_WHY_PREVIOUS_KEPT:return "Kept previous AI selection";
        case RD_WHY_EXTERNAL_MATERIALS:return "Legal via external materials";
        case RD_WHY_TRIBUTE_SETUP:return "Setting up missing Ritual tribute";
        case RD_WHY_FIELD_GUARD:return "Preserved field tribute from replacement";
        case RD_WHY_DECK_TRIBUTE_FIELD:return "Fetching missing Field tribute from shuffled Deck";
        case RD_WHY_DECK_TRIBUTE_HAND:return "Keeping missing tribute in physical Hand";
        case RD_WHY_DECK_MAGIC_PULL:return "Retrieving real Ritual Magic from shuffled Deck";
        case RD_WHY_FIELD_NO_SAFE:return "All field zones reserved; old AI kept";
        case RD_WHY_POST_RITUAL_SUPPORT:return "Active Ritual gains Beacon from played support";
        case RD_WHY_DECK_POST_RITUAL_SUPPORT:return "Fetching Beacon support from real Deck";
        default:return "No new decision yet";
    }
}
const char *Rd_OriginText(RdDecisionOrigin origin)
{
    return origin==RD_ORIGIN_DUELISTS?"Ritual Duelists":origin==RD_ORIGIN_CHAIN?"Previous AI chain":"Observation only";
}
