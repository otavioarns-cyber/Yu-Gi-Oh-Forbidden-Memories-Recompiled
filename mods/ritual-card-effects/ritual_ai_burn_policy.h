#ifndef RITUAL_AI_BURN_POLICY_H
#define RITUAL_AI_BURN_POLICY_H
/* Ritual Effects owns Yamadron AI; independent of Ritual Duelists and its profiles.
   These are raw card flags: OCCUPIED=0x8000, FACE_DOWN=0x1000,
   USED_THIS_TURN=0x4000. The ritual's created spells have origin=-1.
   The AI must use the generated spells even after Yamadron leaves the field. */
#define RITUAL_AI_BURN_OCCUPIED 0x8000u
#define RITUAL_AI_BURN_FACE_DOWN 0x1000u
#define RITUAL_AI_BURN_USED 0x4000u
typedef struct {
    int id, origin;
    unsigned flags;
} RitualAiBurnSlot;
/* In both field views, grid positions 15..19 are the controlling side's
 * five Magic/Trap cells (positions 10..14 are MONSTERS). */
static int ritual_ai_burn_grid_index(int side,int col)
{ return side*20+15+col; }
/* Only a fully initialized CPU field decision may be preempted; never
 * displace ongoing cursor movement, resolved effects or the player's turn. */
static int ritual_ai_burn_decision_window(int phase,int initialized,
                                          int field_state,int ai_state,int cpu)
{
    return phase==5 && initialized && field_state==0x82 && ai_state==0 && cpu;
}
/* The retail PlayFieldCard(category=0,second=1) action encoding: selecting
 * one existing back-row card as both source and destination activates it. */
static void ritual_ai_burn_set_action(unsigned char selected[12],int col)
{
    int i;
    for(i=0;i<12;++i)selected[i]=0;
    selected[9]=(unsigned char)(6+col);
    selected[10]=(unsigned char)(6+col);
}
static int ritual_ai_burn_choose(const RitualAiBurnSlot slots[5])
{
    int i;
    if(!slots)return -1;
    for(i=0;i<5;i++) {
        const RitualAiBurnSlot *s=&slots[i];
        if((s->id==345 || s->id==346 || s->id==347) && s->origin==-1 &&
           (s->flags & (RITUAL_AI_BURN_OCCUPIED|RITUAL_AI_BURN_USED|
                        RITUAL_AI_BURN_FACE_DOWN))==
                        (RITUAL_AI_BURN_OCCUPIED|RITUAL_AI_BURN_FACE_DOWN) &&
           !(s->flags & 0x0800u))return i;
    }
    return -1;
}
#endif
