/* Garma-only presentation. Screen positions come from the native card renderer,
 * not the world-space field object. No Equip mechanics are invoked. */
typedef struct { void *data; int x,y,valid; DisplayObject *object; } GarmaScreen;
static GarmaScreen garma_screen[DUEL_CARD_RECORD_COUNT];
static void *garma_pending[DUEL_CARD_RECORD_COUNT];
static unsigned garma_pending_mask;
static int garma_relevant=-1, garma_relevant_frames;
/* Render the sword with the mod's native-texture sprite only.  The old
 * request id 4 also draws an inseparable bright flash; never enqueue it. */
#define GARMA_SWORD_ANIMATION_FRAMES 36
static void *garma_animation_data;
/* The blade icon is immutable. Build its horizontal color runs once at mod
 * initialization instead of rescanning the 17x17 ASCII sprite every overlay
 * frame. Drawing order and rectangles remain byte-for-byte equivalent in
 * geometry to the previous row/run renderer. */
typedef struct { u8 row,start,length,tone; } GarmaIconRun;
static GarmaIconRun garma_icon_runs[17*17];
static unsigned garma_icon_run_count;
static void garma_icon_prepare(void)
{
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
    int row,col;
    if(garma_icon_run_count)return;
    for(row=0;row<17;++row)for(col=0;col<17;){
        int begin; char c=sword[row][col]; GarmaIconRun *run;
        if(c=='.'){++col;continue;}
        begin=col++;while(col<17&&sword[row][col]==c)++col;
        run=&garma_icon_runs[garma_icon_run_count++];
        run->row=(u8)row;run->start=(u8)begin;run->length=(u8)(col-begin);run->tone=(u8)c;
    }
}
static void garma_capture(DisplayObject *o,int x,int y)
{
    int i=o ? o->field_6A : -1;
    int phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    if(i<0||i>=DUEL_CARD_RECORD_COUNT || D_801A7AD8[i].card_id!=GARMA_SWORD_ID) return;
    /* Phase 4's pre-play field viewer draws temporary card clones. Capture
     * their real screen position immediately for the blade counter, but keep
     * phase 5's three-frame persistent-object stability rule for effect VFX. */
    x+=26;y+=30;
    if(phase==4) {
        garma_screen[i].data=D_801A7AD8[i].data;garma_screen[i].object=o;
        garma_screen[i].x=x;garma_screen[i].y=y;garma_screen[i].valid=1;return;
    }
    if(phase!=5 || D_801A7AD8[i].object!=o) { garma_screen[i].valid=0; return; }
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
    AudioReplace_Sfx(GARMA_SWORD_SPEND_SFX_ID,255,0);
    return 1;
}
static void garma_visual_tick(void)
{
    int i,j;
    if(garma_relevant_frames>0 && --garma_relevant_frames==0) {
        garma_relevant=-1;
        garma_animation_data=0;
    }
    if(!ritual_card_effect_enabled(GARMA_SWORD_ID)) {
        if(garma_pending_mask)memset(garma_pending,0,sizeof(garma_pending));
        garma_pending_mask=0;
        garma_relevant_frames=0;
        garma_relevant=-1;
        garma_animation_data=0;
        return;
    }
    if(!garma_pending_mask || garma_relevant_frames>0)return;
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5 || crab_umi_visual_state ||
       (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE))return;
    /* Preserve the old visual sequencing: wait for native requests to finish,
     * but don't create request 4, whose sprite contains the unwanted flash. */
    for(j=0;j<DUEL_EFFECT_REQUEST_COUNT;++j)
        if(D_800EAD88[j].flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE)return;
    { unsigned pending=garma_pending_mask; while(pending) {
        i=take_mask_bit(&pending);
        if(!garma_pending[i]||!garma_live(i)||D_801A7AD8[i].data!=garma_pending[i]){
            garma_pending[i]=0;garma_pending_mask&=~(1u<<i);continue;
        }
        if(garma_screen[i].valid<3 || garma_screen[i].object!=D_801A7AD8[i].object ||
           garma_screen[i].data!=garma_pending[i] || garma_screen[i].x<0 ||
           garma_screen[i].x>=320 || garma_screen[i].y<0 || garma_screen[i].y>=240)continue;
        SD_SEPlayFull(0x16);
        garma_relevant=i;
        garma_animation_data=garma_pending[i];
        garma_relevant_frames=GARMA_SWORD_ANIMATION_FRAMES;
        garma_pending[i]=0;
        garma_pending_mask&=~(1u<<i);
        return;
    }}
}
