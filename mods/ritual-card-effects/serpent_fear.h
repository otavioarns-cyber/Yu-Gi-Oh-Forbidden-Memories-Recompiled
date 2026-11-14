/* Serpent presentation and confirmed-attack redirection. The validated
 * pre-battle snapshot and +1000/source calculation remain in mod.c. */
static u8 *(*serpent_original_stage)(DuelCardRecord *, s32, s32);
static s32 (*serpent_original_ai_run)(void);
static void (*serpent_original_ai_weakest)(void);
static s32 (*serpent_original_ai_in_sets)(s32,s32);
static void (*serpent_original_ai_best_attack)(void);
static int serpent_inside_ai_field,serpent_ai_target_scan;
static unsigned char serpent_art_record[CARD_ART_RECORD];
/* Adjacent pixels with identical color/alpha are submitted as one rectangle.
 * This preserves the exact v0.69 artwork, resolution and edge transparency. */
typedef struct { u8 x,y,width,height; unsigned rgba; } SerpentArtRun;
static SerpentArtRun serpent_art_runs[CARD_ART_WIDTH*CARD_ART_HEIGHT];
static unsigned serpent_art_run_count;
static unsigned serpent_rng;
static int serpent_redirect_state, serpent_redirect_hold;
#define SERPENT_REDIRECT_FRAMES 24

static int serpent_identity(int i)
{
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && monster_slot(i) &&
        serpent_marks[i].ritual && D_801A7AD8[i].card_id==SERPENT_NIGHT_ID &&
        D_801A7AD8[i].data==serpent_marks[i].data;
}

static void serpent_reconcile(void)
{
    unsigned pending=serpent_live_mask;
    if(!serpent_seen)return;
    /* Called on the stable field, never during temporary battle staging.
     * The mask names only Ritual Serpent instances, so normal field ticks no
     * longer walk all ten Monster Zones. Face-down remains irrelevant. */
    while(pending) {
        int i=take_mask_bit(&pending);
        if(!serpent_marks[i].ritual || !serpent_identity(i) ||
           !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)) {
            memset(&serpent_marks[i],0,sizeof(serpent_marks[i]));
            serpent_live_mask&=~(1u<<i);
        }
    }
    /* A future Ritual Summon re-arms both the mask and this gate. */
    serpent_seen=serpent_live_mask!=0;
}

/* Find ordinary, occupied enemy Monster Zones in the attacker's view.
 * Ritual Serpents are excluded as substitute targets so they cannot
 * redirect to each other. With only Ritual Serpents, attacks resolve normally. */
static unsigned serpent_redirect_candidates(int attacker_side, int chosen_col)
{
    unsigned ordinary=0,adjacent=0;
    int col,i,source;
    if(attacker_side<0 || attacker_side>=DUEL_SIDE_COUNT ||
       chosen_col<0 || chosen_col>=DUEL_FIELD_ROW_SIZE ||
       !ritual_card_effect_enabled(SERPENT_NIGHT_ID))return 0;
    source=D_800907D8[attacker_side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+5+chosen_col];
    if(!serpent_identity(source) ||
       !(D_801A7AD8[source].flags&DUEL_CARD_FLAG_OCCUPIED) ||
       side_for_record(source)==attacker_side)return 0;
    for(col=0;col<DUEL_FIELD_ROW_SIZE;++col) {
        i=D_800907D8[attacker_side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+5+col];
        if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !monster_slot(i) ||
           side_for_record(i)==attacker_side ||
           !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) ||
           serpent_identity(i))continue;
        ordinary|=1u<<col;
    }
    if(chosen_col>0)adjacent|=1u<<(chosen_col-1);
    if(chosen_col+1<DUEL_FIELD_ROW_SIZE)adjacent|=1u<<(chosen_col+1);
    adjacent&=ordinary;
    return adjacent?adjacent:ordinary;
}

static int serpent_pick_target(unsigned candidates)
{
    unsigned count=0,pick,r;
    int col;
    for(col=0;col<DUEL_FIELD_ROW_SIZE;++col)if(candidates&(1u<<col))++count;
    if(!count)return -1;
    if(count==1)pick=0;
    else {
        /* Private RNG: neither the game's random stream nor AI planning is
         * consumed by the visual/effect. Left/right receive the same weight. */
        if(!serpent_rng) {
            uint64_t clock=host&&host->now_us?host->now_us(host):0;
            serpent_rng=(unsigned)clock^(unsigned)(clock>>32)^0x706A53D9u;
            if(!serpent_rng)serpent_rng=1;
        }
        r=serpent_rng;r^=r<<13;r^=r>>17;r^=r<<5;serpent_rng=r;
        pick=r%count;
    }
    for(col=0;col<DUEL_FIELD_ROW_SIZE;++col)if(candidates&(1u<<col)) {
        if(!pick--)return col;
    }
    return -1;
}

/* Super War-Lion is the inverse of Serpent Fear: when an enemy has committed
 * to attack another allied Beast, the target is replaced by a live Ritual
 * War-Lion before retail stages either participant.  War-Lion itself remains
 * a legal target, which prevents redirect loops with multiple copies. */
static unsigned war_lion_redirect_candidates(int attacker_side, int chosen_col)
{
    unsigned all=0;
    int source,col,i;
    if(attacker_side<0 || attacker_side>=DUEL_SIDE_COUNT ||
       chosen_col<0 || chosen_col>=DUEL_FIELD_ROW_SIZE ||
       !ritual_card_effect_enabled(SUPER_WAR_LION_ID))return 0;
    source=D_800907D8[attacker_side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+5+chosen_col];
    if(source<0 || source>=DUEL_CARD_RECORD_COUNT || !monster_slot(source) ||
       !(D_801A7AD8[source].flags&DUEL_CARD_FLAG_OCCUPIED) ||
       side_for_record(source)==attacker_side ||
       D_801A7AD8[source].card_id==SUPER_WAR_LION_ID ||
       card_type_id(&D_801A7AD8[source])!=CARD_TYPE_BEAST)return 0;
    /* Every live War-Lion may intercept. There is deliberately no adjacency
     * priority: with several copies the strongest one wins the right to
     * intercept; equal strongest copies share that right at random. */
    for(col=0;col<DUEL_FIELD_ROW_SIZE;++col) {
        i=D_800907D8[attacker_side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+5+col];
        if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !monster_slot(i) ||
           side_for_record(i)==attacker_side || !war_lion_live(i))continue;
        all|=1u<<col;
    }
    return all;
}

static unsigned war_lion_strongest_candidates(int attacker_side,unsigned candidates)
{
    unsigned strongest=0;
    int col,best=-1;
    for(col=0;col<DUEL_FIELD_ROW_SIZE;++col)if(candidates&(1u<<col)) {
        int i=D_800907D8[attacker_side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+5+col];
        int atk;
        if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !war_lion_live(i))continue;
        atk=(int)((unsigned)Duel_CalcCardStats(&D_801A7AD8[i])&0xffffu);
        if(atk>best){best=atk;strongest=1u<<col;}
        else if(atk==best)strongest|=1u<<col;
    }
    return strongest;
}

static int ritual_ai_actual_margin(int attacker_slot,int defender_index);

static u8 *serpent_stage_card(DuelCardRecord *card, s32 x, s32 y)
{
    int side,index;
    if(!serpent_seen && !war_lion_live_mask)return serpent_original_stage(card,x,y);
    side=D_8009B1D5;index=record_index(card);
    /* FieldActions calls this only AFTER accepting the attack, first for the
     * attacker and then for the defender. Change its selected target before
     * either record is staged/deactivated: retail therefore snapshots,
     * reveals and battles the actual replacement, for manual play and CPU.
     * No cursor filtering, input fabrication, battle recreation or stat change. */
    if(inside_field_actions && side>=0 && side<DUEL_SIDE_COUNT &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5 &&
       ((D_8009B174&0x0f)==5 || (D_8009B174&0x0f)==6)) {
        DuelSelectionSide *sides=(DuelSelectionSide*)D_800E9F10;
        DuelSelectionRecord *attacker=&sides[side].records[2];
        DuelSelectionRecord *target=&sides[side].records[3];
        if(attacker->row==2 && attacker->col>=0 && attacker->col<5 &&
           target->row==1 && card && monster_slot(index) &&
           side_for_record(index)==side &&
           (card->flags&DUEL_CARD_FLAG_OCCUPIED) && card_type_id(card)<CARD_TYPE_MAGIC &&
           index==D_800907D8[side*20+10+attacker->col]) {
            /* A hostile monster effect may not alter a Ritual Ultimate's
             * chosen target. This bypasses both War-Lion interception and
             * Serpent Fear redirection while keeping retail staging intact. */
            if(ultimate_effect_active(index))return serpent_original_stage(card,x,y);
            unsigned lion_choices=war_lion_redirect_candidates(side,target->col);
            lion_choices=war_lion_strongest_candidates(side,lion_choices);
            int lion_col=serpent_pick_target(lion_choices);
            if(lion_col>=0) {
                int source=D_800907D8[side*20+5+lion_col];
                if((D_8009B174&0x0f)==5 && source>=0 && source<DUEL_CARD_RECORD_COUNT) {
                    int attacker_slot=AI_SLOT_OWN_MONSTER_FIRST+attacker->col;
                    /* Learn only from the first real setback: redirecting an
                     * attack that still beats War-Lion is not evidence that
                     * the CPU should avoid this line in future turns. */
                    if(ritual_ai_actual_margin(attacker_slot,source)<=0) {
                        war_lion_marks[source].learned_sides|=1u<<side;
                        ritual_ai_learn_full_effect_for_side(side,SUPER_WAR_LION_ID);
                    }
                }
                target->col=(s8)lion_col;
                /* Do not inject an extra presentation state into the battle
                 * handoff. Retail will reveal/battle the replacement target. */
                if(host&&host->log)
                    host->log(host,"[Ritual Card Effects] Super War-Lion intercepta ataque: slot %d",source);
            } else {
                unsigned choices=serpent_redirect_candidates(side,target->col);
                int new_col=serpent_pick_target(choices);
                if(new_col>=0) {
                    /* Substate 5 is the CPU's committed field attack; 6 is manual
                     * confirmation. Discovery comes from an actual redirection,
                     * never a planner query, cursor visit or another side's hit. */
                    if((D_8009B174&0x0f)==5) {
                        int source=D_800907D8[side*20+5+target->col];
                        serpent_marks[source].learned_sides|=1u<<side;
                        /* This is the first real setback, not planner
                         * foreknowledge: promote it to the same duel-wide
                         * complete-effect knowledge used by the other Rituals. */
                        ritual_ai_learn_full_effect_for_side(side,SERPENT_NIGHT_ID);
                    }
                    target->col=(s8)new_col;
                    serpent_redirect_state=1;
                    serpent_redirect_hold=SERPENT_REDIRECT_FRAMES;
                }
            }
        }
    }
    return serpent_original_stage(card,x,y);
}

/* Knowledge follows the existing Ritual instance mark. Stable-field
 * reconciliation forgets removed/replaced instances, and a new Ritual
 * explicitly clears learned_sides, even when its deck pointer is reused. */
static int serpent_ai_known_target(int ai_slot)
{
    int side=D_8009B1D5,col,source;
    if(side<0 || side>=DUEL_SIDE_COUNT ||
       ai_slot<AI_SLOT_OPPONENT_MONSTER_FIRST ||
       ai_slot>AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST))return 0;
    col=AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST)-ai_slot;
    if(!serpent_redirect_candidates(side,col))return 0;
    source=D_800907D8[side*20+5+col];
    return serpent_identity(source) && ritual_ai_knows_effect(side,SERPENT_NIGHT_ID);
}

static s32 serpent_ai_in_sets(s32 mode,s32 ai_slot)
{
    s32 result;
    if(!serpent_seen)return serpent_original_ai_in_sets(mode,ai_slot);
    result=serpent_original_ai_in_sets(mode,ai_slot);
    /* Only the opponent-target ranking below uses this extra exclusion.
     * Fusion materials, Equips, spell searches and field counts stay native. */
    if(serpent_ai_target_scan && serpent_ai_known_target(ai_slot))return 1;
    return result;
}

static void serpent_ai_find_weakest(void)
{
    int scope=0;
    if(!serpent_seen){serpent_original_ai_weakest();return;}
    if(serpent_inside_ai_field && gAiScript_State.script_cursor) {
        unsigned reg=gAiScript_State.script_cursor[1];
        if(reg<AI_SCRIPT_MEMORY_COUNT) {
            int zone=gAiScript_aMemory[reg];
            scope=zone>=3 && zone<=5;
        }
    }
    if(scope)++serpent_ai_target_scan;
    serpent_original_ai_weakest();
    if(scope)--serpent_ai_target_scan;
}

static void serpent_ai_best_attack(void)
{
    unsigned skipped=0;
    if(!serpent_seen){serpent_original_ai_best_attack();return;}
    u16 saved[DUEL_FIELD_ROW_SIZE];
    int i;
    if(serpent_inside_ai_field)for(i=0;i<DUEL_FIELD_ROW_SIZE;++i) {
        int slot=AI_SLOT_OPPONENT_MONSTER_FIRST+i;
        if(serpent_ai_known_target(slot)) {
            saved[i]=gDuel_aActiveCards[slot].flags;
            /* This native attack-only handler skips targets with this bit.
             * No live card, card id, count or other planner sees the change. */
            gDuel_aActiveCards[slot].flags|=DUEL_CARD_FLAG_USED_THIS_TURN;
            skipped|=1u<<i;
        }
    }
    serpent_original_ai_best_attack();
    for(i=0;i<DUEL_FIELD_ROW_SIZE;++i)if(skipped&(1u<<i))
        gDuel_aActiveCards[AI_SLOT_OPPONENT_MONSTER_FIRST+i].flags=saved[i];
}

static int war_lion_ai_known_source(int ai_slot)
{
    int side=D_8009B1D5,col,source;
    unsigned choices;
    if(side<0 || side>=DUEL_SIDE_COUNT || ai_slot<AI_SLOT_OPPONENT_MONSTER_FIRST ||
       ai_slot>AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST))return -1;
    col=AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST)-ai_slot;
    choices=war_lion_redirect_candidates(side,col);
    choices=war_lion_strongest_candidates(side,choices);
    while(choices) {
        int lion_col=take_mask_bit(&choices);
        source=D_800907D8[side*20+5+lion_col];
        if(source>=0 && source<DUEL_CARD_RECORD_COUNT && war_lion_live(source) &&
           ritual_ai_knows_effect(side,SUPER_WAR_LION_ID))return source;
    }
    return -1;
}

static int sengenjin_ai_known_target(int ai_slot)
{
    int side=D_8009B1D5,col,source;
    if(side<0 || side>=DUEL_SIDE_COUNT || ai_slot<AI_SLOT_OPPONENT_MONSTER_FIRST ||
       ai_slot>AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST))return 0;
    col=AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST)-ai_slot;
    source=D_800907D8[side*20+5+col];
    return source>=0 && source<DUEL_CARD_RECORD_COUNT && sengenjin_identity(source) &&
        ritual_ai_knows_effect(side,SENGENJIN_ID);
}

static int dokurorider_ai_known_target(int ai_slot)
{
    int side=D_8009B1D5,col,source;
    if(side<0 || side>=DUEL_SIDE_COUNT || ai_slot<AI_SLOT_OPPONENT_MONSTER_FIRST ||
       ai_slot>AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST))return 0;
    col=AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST)-ai_slot;
    source=D_800907D8[side*20+5+col];
    return source>=0 && source<DUEL_CARD_RECORD_COUNT && dokurorider_identity(source) &&
        ritual_ai_knows_effect(side,DOKURORIDER_ID);
}

static int ritual_ai_attacker_record(int attacker_slot)
{
    int side=D_8009B1D5,col,index;
    if(side<0 || side>=DUEL_SIDE_COUNT || attacker_slot<AI_SLOT_OWN_MONSTER_FIRST ||
       attacker_slot>AI_SLOT_ROW_LAST(AI_SLOT_OWN_MONSTER_FIRST))return -1;
    col=attacker_slot-AI_SLOT_OWN_MONSTER_FIRST;
    index=D_800907D8[side*20+10+col];
    if(index<0 || index>=DUEL_CARD_RECORD_COUNT || !monster_slot(index) ||
       !(D_801A7AD8[index].flags&DUEL_CARD_FLAG_OCCUPIED))return -1;
    return index;
}

static int ritual_ai_actual_margin(int attacker_slot,int defender_index)
{
    int attacker_index=ritual_ai_attacker_record(attacker_slot);
    DuelCardRecord *a,*d;
    int attack,defense;
    if(attacker_index<0 || defender_index<0 || defender_index>=DUEL_CARD_RECORD_COUNT)return -32768;
    a=&D_801A7AD8[attacker_index];d=&D_801A7AD8[defender_index];
    attack=Duel_CalcBattleAttack(a,d);
    if(sengenjin_identity(defender_index) && ritual_card_effect_enabled(SENGENJIN_ID)) {
        /* Retail encodes the right-side Guardian-Star relation in the left
         * attack value; it does not independently add/subtract a star bonus
         * to the defender. After the CPU has learned Sengenjin, evaluate the
         * target with that same real battle rule. This is especially important
         * when the failed first attack was chosen because +500 Guardian Star
         * made it look safe. */
        unsigned packed=(unsigned)Duel_CalcCardStats(d);
        defense=(d->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?
            (int)(packed>>16):(int)(packed&0xffffu);
    } else {
        defense=(d->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?
            Duel_CalcBattleDefense(d,a):Duel_CalcBattleAttack(d,a);
    }
    return attack-defense;
}

static int ritual_ai_known_dokur_evades(int attacker_slot,int defender_index)
{
    int attacker_index=ritual_ai_attacker_record(attacker_slot);
    if(attacker_index<0 || defender_index<0 || defender_index>=DUEL_CARD_RECORD_COUNT)return 0;
    if(Cards_Level(D_801A7AD8[attacker_index].card_id)<6)return 0;
    return ritual_ai_actual_margin(attacker_slot,defender_index)>=0;
}

static int ritual_ai_target_is_bad(int attacker,int ai_slot)
{
    int side=D_8009B1D5,col,target,lion;
    if(serpent_ai_known_target(ai_slot))return 1;
    lion=war_lion_ai_known_source(ai_slot);
    if(lion>=0)return ritual_ai_actual_margin(attacker,lion)<=0;
    if(sengenjin_ai_known_target(ai_slot)) {
        col=AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST)-ai_slot;
        target=D_800907D8[side*20+5+col];
        return ritual_ai_actual_margin(attacker,target)<=0;
    }
    if(dokurorider_ai_known_target(ai_slot)) {
        col=AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST)-ai_slot;
        target=D_800907D8[side*20+5+col];
        return ritual_ai_known_dokur_evades(attacker,target);
    }
    return 0;
}

static int dokurorider_ai_pick_lower_level_attacker(int target_slot,int allow_equal)
{
    int side=D_8009B1D5,col,target,best=-1,best_margin=-32768,attacker;
    if(side<0 || side>=DUEL_SIDE_COUNT || !dokurorider_ai_known_target(target_slot))return 0;
    col=AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST)-target_slot;
    if(col<0 || col>=DUEL_FIELD_ROW_SIZE)return 0;
    target=D_800907D8[side*20+5+col];
    if(target<0 || target>=DUEL_CARD_RECORD_COUNT || !dokurorider_identity(target))return 0;
    /* Knowledge is the rule, not a blanket fear flag: after the CPU either
     * owns an active Dokurorider or suffers its first real dodge setback, it
     * may deliberately switch to any ready Level 5-or-lower
     * monster whose CURRENT effective stats can beat Dokurorider. Equip
     * bonuses already applied to the field therefore count naturally. */
    for(attacker=AI_SLOT_OWN_MONSTER_FIRST;
        attacker<=AI_SLOT_ROW_LAST(AI_SLOT_OWN_MONSTER_FIRST);++attacker) {
        int record,margin;
        if(!gDuel_aActiveCards[attacker].card_id ||
           (gDuel_aActiveCards[attacker].flags&DUEL_CARD_FLAG_USED_THIS_TURN))continue;
        record=ritual_ai_attacker_record(attacker);
        if(record<0 || Cards_Level(D_801A7AD8[record].card_id)>5)continue;
        margin=ritual_ai_actual_margin(attacker,target);
        if(margin<0 || (!allow_equal && margin==0))continue;
        if(best<0 || margin>best_margin) {best=attacker;best_margin=margin;}
    }
    if(best<0)return 0;
    D_800EAE91=(u8)best;
    D_800EAE92=(u8)target_slot;
    return 1;
}

static int serpent_ai_alternative(int attacker)
{
    int best=-1,best_margin=0,col,side=D_8009B1D5;
    unsigned hidden=0;
    const AiActiveCard *a=&gDuel_aActiveCards[attacker];
    for(col=0;col<DUEL_FIELD_ROW_SIZE;++col) {
        int i=D_800907D8[side*20+5+col],slot=60-col,margin;
        const AiActiveCard *d=&gDuel_aActiveCards[slot];
        if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !monster_slot(i) ||
           side_for_record(i)==side || !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) ||
           !d->card_id)continue;
        if(D_801A7AD8[i].flags&DUEL_CARD_FLAG_FACE_DOWN) {
            /* Do not give the fallback access to an unrevealed card's stats. */
            hidden|=1u<<col;continue;
        }
        if(serpent_ai_known_target(slot))continue;
        {
            int lion=war_lion_ai_known_source(slot);
            if(lion>=0)margin=ritual_ai_actual_margin(attacker,lion);
            else if(sengenjin_ai_known_target(slot))margin=ritual_ai_actual_margin(attacker,i);
            else if(dokurorider_ai_known_target(slot) &&
                    ritual_ai_known_dokur_evades(attacker,i))continue;
            else {
                margin=a->attack-((d->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?d->defense:d->attack);
                margin+=Duel_CalcGuardianStarMatchup(a->guardian_star,d->guardian_star);
            }
        }
        if(margin>best_margin) {best_margin=margin;best=col;}
    }
    if(best>=0)return best;
    return serpent_pick_target(hidden);
}

/* Yamadron's spells are an obligation, not a discretionary AI hint.
 * Only spells with the exact private generated-data record are eligible.
 * Do not depend on Yamadron surviving after generation: the spell remains
 * the same legally playable spell when its summoner leaves the field. */
static int yamadron_ai_generated_burn(int side)
{
    RitualAiBurnSlot slots[5];
    int col;
    if(side<0 || side>=DUEL_SIDE_COUNT)return -1;
    for(col=0;col<DUEL_FIELD_ROW_SIZE;++col) {
        int index=D_800907D8[ritual_ai_burn_grid_index(side,col)];
        const DuelCardRecord *c;
        slots[col]=(RitualAiBurnSlot){0,0,0};
        if(index<0 || index>=DUEL_CARD_RECORD_COUNT || !magic_slot(index))continue;
        c=&D_801A7AD8[index];
        if(c->data!=&generated_data[index] ||
           !(c->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
        slots[col].id=c->card_id;
        slots[col].origin=((const DuelDeckCardRecord *)c->data)->deck_index;
        slots[col].flags=c->flags;
    }
    return ritual_ai_burn_choose(slots);
}

/* Use the *native* field-action selection contract (the same back-row
 * activation as AiScript_PlayFieldCard with category 0 and second 1).
 * Inject at the field controller before AiScript_Run is called. Other AI
 * mods can then neither override the card with a ritual nor end the turn
 * before the generated spells are spent. The game still animates, resolves,
 * and applies the damage; this code never writes the opponent's LP. */
static int yamadron_ai_force_next_burn(void)
{
    int side=(int)D_8009B1D5, burn;
    if(side<0 || side>=DUEL_SIDE_COUNT ||
       !ritual_ai_burn_decision_window(
           gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK,
           (gDuel_wSceneStateFlags&0x8000)!=0,
           D_8009B174&0x8f,
           (unsigned)D_8009B20C[0]&0x0fu,
           ritual_ai_side_is_cpu(side)))return 0;
    burn=yamadron_ai_generated_burn(side);
    if(burn<0)return 0;
    ritual_ai_burn_set_action(D_800EAE88,burn);
    D_8009B20C[0]=1; /* Native controller's AI decision -> cursor move. */
    if(host && host->log)
        host->log(host,"[Ritual Card Effects] Yamadron CPU LAW: force native Magic activation, side=%d, M/T zone=%d, card=%d, opposing LP=%d",
                  side,burn+1,(int)D_801A7AD8[D_800907D8[ritual_ai_burn_grid_index(side,burn)]].card_id,
                  (int)D_800E9FF0[side^1].life_points.signed_value);
    return 1;
}

static s32 serpent_ai_run(void)
{
    int field;
    s32 result;
    if(!serpent_seen && !war_lion_live_mask && !sengenjin_live_hint &&
       !dokurorider_live_hint && !ultimate_live_hint && !cosmo_live_hint &&
       !yamadron_live_mask && !((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5 &&
       D_8009B1D5<DUEL_SIDE_COUNT && ritual_ai_side_is_cpu(D_8009B1D5)))
        return serpent_original_ai_run();
    field=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5 &&
        (D_8009B174&0x0f)==2 && D_8009B1D5<DUEL_SIDE_COUNT;
    if(field)++serpent_inside_ai_field;
    result=serpent_original_ai_run();
    if(field)--serpent_inside_ai_field;

    /* Prioritize generated burn even when the retail AI was about to end
       its turn (result 3) or selected an attack. Also handles lethal burn:
       no hand play, no special-case of low enemy LP. The game still owns
       the card activation and LP calculation. */
    if(field && ritual_ai_side_is_cpu(D_8009B1D5)) {
        int burn=yamadron_ai_generated_burn((int)D_8009B1D5);
        if(burn>=0) {
            ritual_ai_burn_set_action(D_800EAE88,burn);
            if(host && host->log)
                host->log(host,"[Ritual Card Effects] Yamadron CPU: generated burn card in M/T zone %d; previous AI result %d; opposing LP %d",
                          burn+1,(int)result,(int)D_800E9FF0[D_8009B1D5^1].life_points.signed_value);
            return 2;
        }
    }

    if(field && D_8009B1D5<DUEL_SIDE_COUNT && ritual_ai_side_is_cpu(D_8009B1D5) &&
       D_800E9FF0[D_8009B1D5].swords_turns_remaining && ultimate_live_hint) {
        ++ultimate_swords_diag_scans;
        ultimate_swords_diag_last_result=result;
    }

    /* Swords exception is attacker-local.  Under Swords the retail CPU can
     * repeatedly choose a DEF/idle command (result 2), never reaching the
     * EndField opcode (result 3) intercepted by the previous implementation.
     * Schedule its one legal Ritual Ultimate attack at the FIRST field
     * decision, regardless of which of those two outcomes the script chose.
     * Once that Ultimate attacks the normal USED_THIS_TURN bit stops further
     * injection; all other attackers remain subject to the native lock.
     * Use the live grid records, not the potentially pinned AI snapshot. */
    if(field && (result==2 || result==3) && D_8009B1D5<DUEL_SIDE_COUNT &&
       ritual_ai_side_is_cpu(D_8009B1D5) &&
       D_800E9FF0[D_8009B1D5].swords_turns_remaining && ultimate_live_hint) {
        int script_result=result;
        int col,best_col=-1,best_atk=-1,target_col=-1,best_margin=-32768;
        for(col=0;col<DUEL_FIELD_ROW_SIZE;++col) {
            int rec=D_800907D8[D_8009B1D5*20+10+col];
            if(rec<0 || rec>=DUEL_CARD_RECORD_COUNT || !ultimate_effect_active(rec) ||
               (D_801A7AD8[rec].flags&DUEL_CARD_FLAG_USED_THIS_TURN))continue;
            {
                unsigned packed=(unsigned)Duel_CalcCardStats(&D_801A7AD8[rec]);
                int atk=(int)(packed&0xffffu);
                if(atk>best_atk){best_atk=atk;best_col=col;}
            }
        }
        if(best_col>=0) {
            int attacker_rec=D_800907D8[D_8009B1D5*20+10+best_col];
            int enemy_count=0;
            /* Pick the most favourable visible target using CURRENT duel
             * stats.  Hidden cards remain legal targets but do not leak their
             * stats; remember the first one only as a fallback. */
            for(col=0;col<DUEL_FIELD_ROW_SIZE;++col) {
                int rec=D_800907D8[D_8009B1D5*20+5+col];
                if(rec<0 || rec>=DUEL_CARD_RECORD_COUNT || !monster_slot(rec) ||
                   !(D_801A7AD8[rec].flags&DUEL_CARD_FLAG_OCCUPIED) ||
                   side_for_record(rec)==(int)D_8009B1D5)continue;
                ++enemy_count;
                if(D_801A7AD8[rec].flags&DUEL_CARD_FLAG_FACE_DOWN) {
                    if(target_col<0)target_col=col;
                } else {
                    DuelCardRecord *a=&D_801A7AD8[attacker_rec],*d=&D_801A7AD8[rec];
                    int attack=Duel_CalcBattleAttack(a,d);
                    int defense=(d->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?
                        Duel_CalcBattleDefense(d,a):Duel_CalcBattleAttack(d,a);
                    int margin=attack-defense;
                    if(target_col<0 || margin>best_margin){best_margin=margin;target_col=col;}
                }
            }
            D_800EAE91=(u8)(AI_SLOT_OWN_MONSTER_FIRST+best_col);
            D_800EAE92=(u8)(enemy_count?60-target_col:0x3A);
            D_800EAE88[11]=0;
            result=2;
            ++ultimate_swords_diag_issued;
            if(host&&host->log)
                host->log(host,"[Ritual Card Effects] CPU Ultimate/Swords: script=%d, atacante=%d, alvo=%d",
                          script_result,best_col,(int)D_800EAE92);
        }
    }

    /* Cosmo Queen must constrain the FINAL hand choice, after AI Hard or any
     * other planner in the hook chain has selected/materialized its candidate.
     * This prevents a deck-window/non-Female choice from bypassing Queen Lock. */
    if(result==1 && !field && D_8009B1D5<DUEL_SIDE_COUNT)
        cosmo_force_ai_selection();

    /* Ultimate's immunity is learned from failure, then generalized by
     * category.  Post-filter the common AI result so this works for retail AI
     * and AI Hard Mode without teaching either planner the secret in advance. */
    if(result==1 && !field && D_8009B1D5<DUEL_SIDE_COUNT &&
       D_800EAE88[0]>=AI_SLOT_OWN_HAND_FIRST && D_800EAE88[0]<AI_SLOT_OPPONENT_MONSTER_FIRST) {
        int held=D_800EAE88[0],card=gDuel_aActiveCards[held].card_id;
        if(D_800EAE88[8]==0 && ultimate_ai_magic_would_only_fail(card,D_8009B1D5)) {
            int col;
            for(col=0;col<DUEL_FIELD_ROW_SIZE;++col)
                if(!gDuel_aActiveCards[AI_SLOT_OWN_SPELL_FIRST+col].card_id)break;
            if(col<DUEL_FIELD_ROW_SIZE) {
                D_800EAE88[6]=(u8)(6+col);
                D_800EAE88[8]=1; /* set it instead of knowingly wasting it */
            }
        }
    }
    if(field && result==2 && D_8009B1D5<DUEL_SIDE_COUNT &&
       D_800EAE91>=6 && D_800EAE91<=10) {
        int spell_slot=D_800EAE91;
        int card=gDuel_aActiveCards[spell_slot].card_id;
        if(ultimate_ai_magic_would_only_fail(card,D_8009B1D5)) {
            int attacker,replaced=0;
            /* Prefer continuing the turn with a legal attack rather than
             * activating a Magic now known to have no effective victim. */
            for(attacker=AI_SLOT_OWN_MONSTER_FIRST;
                attacker<=AI_SLOT_ROW_LAST(AI_SLOT_OWN_MONSTER_FIRST);++attacker) {
                int col;
                if(!gDuel_aActiveCards[attacker].card_id ||
                   (gDuel_aActiveCards[attacker].flags&DUEL_CARD_FLAG_USED_THIS_TURN))continue;
                col=serpent_ai_alternative(attacker);
                if(col>=0) {
                    D_800EAE88[9]=(u8)attacker;
                    D_800EAE88[10]=(u8)(60-col);
                    D_800EAE88[11]=0;
                    replaced=1;
                    break;
                }
            }
            if(!replaced)return 3;
        }
    }
    /* Some scripts choose a target without the weakest/best-attack handlers.
     * Check that completed ATTACK only. Never reroute a spell or a hand play,
     * and never invent an empty field/direct attack to bypass the Serpent. */
    if(field && result==2 && D_800EAE91>=1 && D_800EAE91<=5 &&
       ritual_ai_target_is_bad(D_800EAE91,D_800EAE92)) {
        int col;
        /* AI Hard is another AiScript_Run replacement in the hook chain. Per
         * API 4, `original` points at that replacement when it precedes this
         * zzz mod, so preserve its equal-trade policy. Retail keeps strict
         * positive-margin behavior. */
        int external_planner=(void *)serpent_original_ai_run!=(void *)AiScript_Run;
        if(dokurorider_ai_known_target(D_800EAE92) &&
           dokurorider_ai_pick_lower_level_attacker(D_800EAE92,external_planner))
            return result;
        col=serpent_ai_alternative(D_800EAE91);
        if(col>=0)D_800EAE92=(u8)(60-col);
        else return 3; /* Native end-of-field action when no viable target exists. */
    }
    return result;
}

static unsigned serpent_art_color(int pos)
{
    unsigned index=serpent_art_record[pos];
    unsigned word=serpent_art_record[CARD_ART_CLUT+2*index] |
        ((unsigned)serpent_art_record[CARD_ART_CLUT+2*index+1]<<8);
    unsigned r=word&31,g=(word>>5)&31,b=(word>>10)&31;
    return ((r<<3|r>>2)<<16)|((g<<3|g>>2)<<8)|(b<<3|b>>2);
}

static unsigned serpent_art_opacity(int x,int y)
{
    unsigned index=serpent_art_record[y*CARD_ART_WIDTH+x];
    int edge=x;
    if(!(serpent_art_record[CARD_ART_CLUT+2*index] |
       serpent_art_record[CARD_ART_CLUT+2*index+1]))return 0;
    if(CARD_ART_WIDTH-1-x<edge)edge=CARD_ART_WIDTH-1-x;
    if(y<edge)edge=y;
    if(CARD_ART_HEIGHT-1-y<edge)edge=CARD_ART_HEIGHT-1-y;
    if(edge>8)edge=8;
    return (unsigned)(edge*160/8);
}

static void serpent_prepare_art_runs(void)
{
    int x,y;
    int previous[CARD_ART_WIDTH],current[CARD_ART_WIDTH];
    for(x=0;x<CARD_ART_WIDTH;++x)previous[x]=-1;
    serpent_art_run_count=0;
    for(y=0;y<CARD_ART_HEIGHT;++y) {
      for(x=0;x<CARD_ART_WIDTH;++x)current[x]=-1;
      for(x=0;x<CARD_ART_WIDTH;) {
        unsigned alpha=serpent_art_opacity(x,y),rgb=serpent_art_color(y*CARD_ART_WIDTH+x);
        int start=x++;
        if(!alpha)continue;
        while(x<CARD_ART_WIDTH && serpent_art_opacity(x,y)==alpha &&
              serpent_art_color(y*CARD_ART_WIDTH+x)==rgb)++x;
        {
            unsigned rgba=(alpha<<24)|rgb;
            int prior=previous[start];
            SerpentArtRun *run=prior>=0?&serpent_art_runs[prior]:0;
            /* Reuse identical spans directly above as one rectangle too. */
            if(run && run->width==x-start && run->rgba==rgba &&
               run->y+run->height==y) {
                ++run->height;current[start]=prior;
            } else {
                current[start]=(int)serpent_art_run_count;
                run=&serpent_art_runs[serpent_art_run_count++];
                run->x=(u8)start;run->y=(u8)y;run->width=(u8)(x-start);
                run->height=1;run->rgba=rgba;
            }
        }
      }
      memcpy(previous,current,sizeof(previous));
    }
}

static int serpent_load_art(void)
{
    if(serpent_art_loaded)return serpent_art_loaded>0;
    /* Same artwork and patches as v0.69, prepared once per reset; no VRAM
     * allocation, bundled game art or dependency on another mod. */
    if(!DiscArt_Read((Cards_BaseId(SERPENT_NIGHT_ID)-1)*7+722,
                     serpent_art_record,CARD_ART_RECORD)) {
        serpent_art_loaded=-1;
        if(host&&host->log)host->log(host,"Serpent Fear: could not read artwork; visual skipped.");
        return 0;
    }
    Cards_PatchArtRecord(SERPENT_NIGHT_ID,serpent_art_record);
    serpent_prepare_art_runs();
    serpent_art_loaded=1;
    return 1;
}

static void serpent_ghost_close(void)
{
    serpent_ghost_state=serpent_ghost_hold=0;
    if(serpent_music_volume>=0) {
        Spu_SetBusVolume(SPU_BUS_MUSIC,serpent_music_volume);
        serpent_music_volume=-1;
    }
}

static void serpent_visual_reset(void)
{
    serpent_redirect_state=serpent_redirect_hold=0;
    serpent_art_run_count=0;
    serpent_inside_ai_field=serpent_ai_target_scan=0;
}

static void serpent_ghost_start(int slot)
{
    ++serpent_dbg_ghost_starts; serpent_dbg_last_slot=slot;
    if(!serpent_ghost_state)serpent_ghost_state=1;
}

static void serpent_ghost_begin(void)
{
    DisplayObject *window=D_800E9EF0[3];
    if(!window || !serpent_load_art()) {serpent_ghost_close();return;}
    serpent_ghost_x=window->field_30.h.field_30+0x13;
    serpent_ghost_y=window->field_30.h.field_32+0x32;
    serpent_music_volume=Spu_GetBusVolume(SPU_BUS_MUSIC);
    Spu_SetBusVolume(SPU_BUS_MUSIC,0);
    serpent_ghost_hold=32;
    serpent_ghost_state=2;
}

static int serpent_ghost_fade(void)
{
    int fade=32-serpent_ghost_hold;
    if(fade>8)fade=8;
    if(serpent_ghost_hold<fade)fade=serpent_ghost_hold;
    return fade;
}

static void serpent_redirect_begin(void)
{
    if(serpent_redirect_state==1 && ritual_card_effect_enabled(SERPENT_NIGHT_ID) &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
       (gDuel_wSceneStateFlags&0x8000) && (D_8009B174&0x8f)==3 &&
       D_800E9EF0[3]) {
        serpent_redirect_state=2;
        SD_SEPlayFull(0x21);
    }
}

static void serpent_ghost_update(void)
{
    if(!ritual_card_effect_enabled(SERPENT_NIGHT_ID) ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9) {
        serpent_redirect_state=serpent_redirect_hold=0;
        if(serpent_ghost_state)serpent_ghost_close();
        return;
    }
    /* Explain only an actual redirection, when the replacement's panel has
     * finished its native reveal. The small cue never holds the battle. */
    if(serpent_redirect_state==1)serpent_redirect_begin();
    else if(serpent_redirect_state==2 && --serpent_redirect_hold<=0)
        serpent_redirect_state=0;
    if(serpent_ghost_state==2 && --serpent_ghost_hold<=0)serpent_ghost_close();
}

static void serpent_ghost_draw(void)
{
    int width,height,scale,vw,vh,left,top,fade;
    unsigned i;
    if(serpent_ghost_state!=2 || serpent_art_loaded!=1 || !host ||
       !host->fill || !host->overlay_size)return;
    host->overlay_size(host,&width,&height,&scale);
    vw=width;vh=width*3/4;
    if(vh>height){vh=height;vw=height*4/3;}
    left=(width-vw)/2;top=(height-vh)/2;
    fade=serpent_ghost_fade();
    if(fade<=0)return;
    for(i=0;i<serpent_art_run_count;++i) {
        const SerpentArtRun *r=&serpent_art_runs[i];
        unsigned alpha=(r->rgba>>24)*fade/8;
        int x0=left+(serpent_ghost_x+r->x)*vw/320;
        int y0=top+(serpent_ghost_y+r->y)*vh/240;
        int x1=left+(serpent_ghost_x+r->x+r->width)*vw/320;
        int y1=top+(serpent_ghost_y+r->y+r->height)*vh/240;
        if(alpha && x1>x0 && y1>y0)
            host->fill(host,x0,y0,x1-x0,y1-y0,r->rgba&0xffffffu,alpha);
    }
}

static void serpent_redirect_draw(void)
{
    int width,height,scale,vw,vh,left,top,x,y,tw,alpha;
    const char *label="Serpent redirects!";
    if(serpent_redirect_state!=2 || !host || !host->overlay_size ||
       !host->draw_text || !host->fill || !host->text_width)return;
    host->overlay_size(host,&width,&height,&scale);
    if(scale<1)scale=1;
    vw=width;vh=width*3/4;if(vh>height){vh=height;vw=height*4/3;}
    left=(width-vw)/2;top=(height-vh)/2;
    tw=host->text_width(host,label,scale);
    x=left+(vw-tw)/2;y=top+26*vh/240;
    alpha=serpent_redirect_hold<6?serpent_redirect_hold*36:216;
    host->fill(host,x-6*scale,y-8*scale,tw+12*scale,16*scale,0x160c25,(unsigned)alpha);
    host->fill(host,x-6*scale,y-8*scale,2*scale,16*scale,0xb688df,(unsigned)alpha);
    host->fill(host,x+tw+4*scale,y-8*scale,2*scale,16*scale,0xb688df,(unsigned)alpha);
    host->draw_text(host,x+scale,y+scale,label,0x080408,scale);
    host->draw_text(host,x,y,label,0xdec4f5,scale);
}
