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
    int i;
    /* Called on the stable field, never during temporary battle staging.
     * Face-down is deliberately irrelevant to Ritual provenance. */
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i)
        if(serpent_marks[i].ritual && (!serpent_identity(i) ||
           !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)))
            memset(&serpent_marks[i],0,sizeof(serpent_marks[i]));
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

static u8 *serpent_stage_card(DuelCardRecord *card, s32 x, s32 y)
{
    int side=D_8009B1D5,index=record_index(card);
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
            unsigned choices=serpent_redirect_candidates(side,target->col);
            int new_col=serpent_pick_target(choices);
            if(new_col>=0) {
                /* Substate 5 is the CPU's committed field attack; 6 is manual
                 * confirmation. Discovery comes from an actual redirection,
                 * never a planner query, cursor visit or another side's hit. */
                if((D_8009B174&0x0f)==5) {
                    int source=D_800907D8[side*20+5+target->col];
                    serpent_marks[source].learned_sides|=1u<<side;
                }
                target->col=(s8)new_col;
                serpent_redirect_state=1;
                serpent_redirect_hold=SERPENT_REDIRECT_FRAMES;
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
    return (serpent_marks[source].learned_sides&(1u<<side))!=0;
}

static s32 serpent_ai_in_sets(s32 mode,s32 ai_slot)
{
    s32 result=serpent_original_ai_in_sets(mode,ai_slot);
    /* Only the opponent-target ranking below uses this extra exclusion.
     * Fusion materials, Equips, spell searches and field counts stay native. */
    if(serpent_ai_target_scan && serpent_ai_known_target(ai_slot))return 1;
    return result;
}

static void serpent_ai_find_weakest(void)
{
    int scope=0;
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
           serpent_ai_known_target(slot) || !d->card_id)continue;
        if(D_801A7AD8[i].flags&DUEL_CARD_FLAG_FACE_DOWN) {
            /* Do not give the fallback access to an unrevealed card's stats. */
            hidden|=1u<<col;continue;
        }
        margin=a->attack-((d->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?d->defense:d->attack);
        margin+=Duel_CalcGuardianStarMatchup(a->guardian_star,d->guardian_star);
        if(margin>best_margin) {best_margin=margin;best=col;}
    }
    if(best>=0)return best;
    return serpent_pick_target(hidden);
}

static s32 serpent_ai_run(void)
{
    int field=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5 &&
        (D_8009B174&0x0f)==2 && D_8009B1D5<DUEL_SIDE_COUNT;
    s32 result;
    if(field)++serpent_inside_ai_field;
    result=serpent_original_ai_run();
    if(field)--serpent_inside_ai_field;
    /* Some scripts choose a target without the weakest/best-attack handlers.
     * Check that completed ATTACK only. Never reroute a spell or a hand play,
     * and never invent an empty field/direct attack to bypass the Serpent. */
    if(field && result==2 && D_800EAE91>=1 && D_800EAE91<=5 &&
       serpent_ai_known_target(D_800EAE92)) {
        int col=serpent_ai_alternative(D_800EAE91);
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
