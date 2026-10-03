/* Garma-only presentation. Screen positions come from the native card renderer,
 * not the world-space field object. No Equip mechanics are invoked. */
typedef struct { void *data; int x,y,valid; DisplayObject *object; } GarmaScreen;
static GarmaScreen garma_screen[DUEL_CARD_RECORD_COUNT];
static void *garma_pending[DUEL_CARD_RECORD_COUNT];
static int garma_relevant=-1, garma_relevant_frames;
static DuelEffectRequest *garma_request;
static void garma_capture(DisplayObject *o,int x,int y)
{
    int i=o ? o->field_6A : -1;
    if(i<0||i>=DUEL_CARD_RECORD_COUNT) return;
    if(D_801A7AD8[i].object!=o || D_801A7AD8[i].card_id!=GARMA_SWORD_ID) return;
    /* The renderer also visits cards during battle/target-camera transitions.
     * Never start the effect from one of those stale positions. Three matching
     * field draws of the rebuilt object establish its current screen centre. */
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5) {
        garma_screen[i].valid=0; return;
    }
    x+=26;y+=30;
    if(garma_screen[i].data==D_801A7AD8[i].data && garma_screen[i].object==o &&
       garma_screen[i].x==x && garma_screen[i].y==y) {
        if(garma_screen[i].valid<3)++garma_screen[i].valid;
    } else garma_screen[i].valid=1;
    garma_screen[i].data=D_801A7AD8[i].data;garma_screen[i].object=o;
    garma_screen[i].x=x;garma_screen[i].y=y;
}
static int garma_live(int i)
{
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && garma_marks[i].ritual &&
        D_801A7AD8[i].card_id==GARMA_SWORD_ID &&
        (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
        D_801A7AD8[i].data==garma_marks[i].data;
}
static int garma_award(int attacker,int defender,int attacker_survives,int defender_survives)
{
    if(!ritual_card_effect_enabled(GARMA_SWORD_ID) || !monster_slot(attacker) ||
       !monster_slot(defender) || side_for_record(attacker)!=D_8009B1D5 ||
       side_for_record(attacker)==side_for_record(defender) ||
       !attacker_survives || defender_survives || !garma_marks[attacker].ritual ||
       garma_marks[attacker].blades_used>=5 || garma_marks[attacker].extra_attack_pending) return 0;
    garma_marks[attacker].extra_attack_pending=1;
    return 1;
}
/* Called once at the committed battle result, BEFORE testing for a new kill.
 * A direct attack has no defender, but still uses the previously unlocked
 * extra attack. Normal attacks never consume one of the five extra blades. */
static int garma_spend_extra(int attacker)
{
    if(!ritual_card_effect_enabled(GARMA_SWORD_ID) || !monster_slot(attacker) ||
       side_for_record(attacker)!=D_8009B1D5 || !garma_marks[attacker].ritual ||
       !garma_marks[attacker].extra_attack_ready) return 0;
    garma_marks[attacker].extra_attack_ready=0;
    if(garma_marks[attacker].blades_used>=5)return 0;
    ++garma_marks[attacker].blades_used;
    return 1;
}
static void garma_visual_tick(void)
{
    int i,j;
    if(garma_relevant_frames>0)--garma_relevant_frames;
    if(!ritual_card_effect_enabled(GARMA_SWORD_ID)) {
        memset(garma_pending,0,sizeof(garma_pending));return;
    }
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5 || crab_umi_visual_state ||
       (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE))return;
    /* Native requests share scratch memory. Do not overlap another request. */
    for(j=0;j<DUEL_EFFECT_REQUEST_COUNT;++j)
        if(D_800EAD88[j].flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE)return;
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i) {
        DuelEffectRequest *r;
        if(!garma_pending[i])continue;
        if(!garma_live(i)||D_801A7AD8[i].data!=garma_pending[i]){garma_pending[i]=0;continue;}
        if(garma_screen[i].valid<3 || garma_screen[i].object!=D_801A7AD8[i].object ||
           garma_screen[i].data!=garma_pending[i] || garma_screen[i].x<0 ||
           garma_screen[i].x>=320 || garma_screen[i].y<0 || garma_screen[i].y>=240)continue;
        r=(DuelEffectRequest *)DuelEffect_AllocateRequest(4);
        if(!r)return;
        r->field_00=garma_screen[i].x;r->field_02=garma_screen[i].y;/* 0 keeps the sword pass but suppresses the companion flash. */r->field_1A=0;
        SD_SEPlayFull(0x16);garma_request=r;garma_pending[i]=0;
        garma_relevant=i;garma_relevant_frames=90;return;
    }
}
static void garma_icons(void)
{
    /* Garma blade counter: rasterized from the user-provided sword icon.
     * It intentionally uses the overlay primitive only, so the visual remains
     * ZIP-only and does not require a framework texture/resource change. */
    static const char *const sword[]={
        ".................",
        "..BBBB...........",
        ".BBWWWWB.........",
        ".BBBWWWWB........",
        ".BBBBWWWB........",
        ".BBBBBWWSB.......",
        ".BBBBBBWWBB......",
        ".BBBBBBBWBB...B..",
        "..BBBBBBBWBB.SDB.",
        "....BBBBBBWBSGDB.",
        ".....BBBBBBBSGDB.",
        ".......BBBBBSGB..",
        "........BBBSGDSB.",
        "........SGGGSDGS.",
        ".......SGDDSDGGGB",
        ".......BSSBSSDGD.",
        "............BSSS."
    };
    int i=-1,w,h,sc,vw,vh,l,t,x,y,n,row,col,px;
    DuelSelectionRecord *cursor;
    if(!host||!host->fill||!host->overlay_size||!ritual_card_effect_enabled(GARMA_SWORD_ID))return;
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5 || D_8009B229)return;
    cursor=&((DuelSelectionSide *)D_800E9F10)[D_8009B1D5].records[2];
    /* Record 2 is the normal field-navigation cursor. Retail changes
     * D_8009B1B4 away from this record as soon as the player commits the
     * monster and enters attack/target selection. The row/col in record 2
     * intentionally remains stale, which is why v0.60/v0.61 kept drawing.
     * Tie the counter to the cursor record that is ACTUALLY active instead. */
    if(D_8009B1B4!=(DuelCardPickCursor *)cursor)return;
    if(cursor->row>=0&&cursor->row<4&&cursor->col>=0&&cursor->col<5)
        i=D_800907D8[D_8009B1D5*20+cursor->row*5+cursor->col];
    if(!garma_live(i)||garma_marks[i].blades_used>=5||!garma_screen[i].valid||garma_screen[i].data!=garma_marks[i].data)return;
    host->overlay_size(host,&w,&h,&sc);vw=w;vh=w*3/4;if(vh>h){vh=h;vw=h*4/3;}l=(w-vw)/2;t=(h-vh)/2;
    px=vw/320;if(px<1)px=1;
    n=5-garma_marks[i].blades_used;
    x=l+garma_screen[i].x*vw/320-n*19*px/2;
    y=t+(garma_screen[i].y-48)*vh/240;if(y<t)y=t;
    for(sc=0;sc<n;++sc)for(row=0;row<17;++row)for(col=0;col<17;++col){
        char c=sword[row][col];u32 rgb;
        if(c=='.')continue;
        if(c=='W')rgb=0xdce7f4;      /* blade highlight */
        else if(c=='B')rgb=0x536a9a; /* blue steel/outline */
        else if(c=='G')rgb=0xd4a21f; /* gold hilt */
        else if(c=='D')rgb=0x76501b; /* dark gold */
        else rgb=0x9aa9c0;           /* steel midtone */
        host->fill(host,x+(sc*19+col)*px,y+row*px,px,px,rgb,245);
    }
}
