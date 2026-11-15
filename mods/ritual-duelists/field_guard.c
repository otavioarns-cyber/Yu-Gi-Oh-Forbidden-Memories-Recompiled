#include "field_guard.h"
#include "game/card_constants.h"

static int rd_field_value(const RdCard *c)
{
    int power=c->atk>c->def?c->atk:c->def;
    return power;
}

int Rd_FieldGuardDestination(const RdBoard *board,const RdPlan *plan,
                             const unsigned char selection[12])
{
    int destination,source,i,best=-1,best_value=0x7fffffff;
    if(!board || !plan || !selection || !plan->prep || plan->urgent)
        return 0;
    destination=(int)selection[6]-1;
    if(destination<0 || destination>=5 ||
       !board->own[destination].id || !plan->protect_field[destination])
        return 0;
    /* A front-zone target can also be a perfectly legal Equip or Spell.
       Only monster-source hand decisions are safe to retarget. */
    source=(int)selection[0]-11;
    if(source<0 || source>=board->actual_hand || source>=5 ||
       !board->hand[source].id || board->hand[source].type>=CARD_TYPE_MAGIC)
        return 0;
    for(i=0;i<5;i++)
        if(!board->own[i].id && !plan->protect_field[i])
            return i+1;
    /* A full field may still have expendable monsters. Redirect to the
       weakest unreserved target rather than destroying a required tribute. */
    for(i=0;i<5;i++)if(!plan->protect_field[i] && board->own[i].id){
        int value=rd_field_value(&board->own[i]);
        if(value<best_value){best_value=value;best=i;}
    }
    if(best>=0)return best+1;
    return -1; /* all field zones reserved: no safe replacement */
}

/* A Ritual result is strategically valuable even if the accompanying MOD
 * exposes no Beacon callback, or its face-state flag is transiently stale.
 * This is independent of ritual setup, and protects the result from the
 * stock AI's hand-on-monster fusion/overwrite choice. */
int Rd_ProtectRitualResult(const RdBoard *board,RdPlan *guard,int result_id)
{
    int i,found=0;
    if(!board || !guard || result_id<=0)return 0;
    for(i=0;i<5;i++)if(board->own[i].id==result_id){
        guard->protect_field[i]=1;
        found=1;
    }
    if(found){guard->prep=1;guard->urgent=0;}
    return found;
}
