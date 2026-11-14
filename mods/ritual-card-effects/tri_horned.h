/* Tri-Horned: instance state, source-counted beacon and resolution snapshots.
 * Reuses the existing effect-survival/resync and activation queues in mod.c. */
#include "tri_horn_sprite.h"
/* Public native modal-state byte declared by game/duel_effect.h. */
extern u8 gDuel_bEffectState;
typedef struct {
    void *data;
    unsigned ritual, horns, battle_flags;
} TriMark;
static TriMark tri_marks[DUEL_CARD_RECORD_COUNT];
static GarmaScreen tri_screen[DUEL_CARD_RECORD_COUNT];
static unsigned tri_guard, tri_horn_visuals, tri_battle_visuals, tri_seen;
static void *tri_guard_data[DUEL_CARD_RECORD_COUNT];
static int tri_resolution_active, tri_resolution_id, tri_field_pending;
static int ritual_field_id=UMI_ID;
/* tri_horn_rgba is immutable. Convert its opaque horizontal spans to a compact
 * run table once at startup so selecting Tri-Horned does not rescan all 1024
 * source pixels on every overlay frame. */
typedef struct { u8 row,start,length; unsigned rgba; } TriIconRun;
static TriIconRun tri_icon_runs[32*32];
static unsigned tri_icon_run_count;
static void tri_icon_prepare(void)
{
    int row,col;
    if(tri_icon_run_count)return;
    for(row=0;row<32;++row)for(col=0;col<32;){
        int begin; unsigned rgba=tri_horn_rgba[row*32+col]; TriIconRun *run;
        if(!(rgba&255)){++col;continue;}
        begin=col++;while(col<32&&tri_horn_rgba[row*32+col]==rgba)++col;
        run=&tri_icon_runs[tri_icon_run_count++];
        run->row=(u8)row;run->start=(u8)begin;run->length=(u8)(col-begin);run->rgba=rgba;
    }
}
static int card_type_id(const DuelCardRecord *card);
static int visual_queue_activation(int record_index);

static int tri_identity(int i)
{
    return monster_slot(i) && tri_marks[i].ritual &&
        D_801A7AD8[i].card_id==TRI_HORNED_ID &&
        D_801A7AD8[i].data==tri_marks[i].data;
}
static int tri_live(int i)
{
    return tri_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED);
}
static unsigned tri_sources(int target)
{
    unsigned pending,count=0;
    if(!monster_slot(target)||!ritual_card_effect_enabled(TRI_HORNED_ID))return 0;
    /* "Other allied Dinosaurs" excludes the source INSTANCE, not all
     * cards of the same ID. A second Tri-Horned is a valid allied Dinosaur,
     * whether it was normal-played or ritual-summoned. Excluding the exact
     * target record in the mask below prevents any self-aura. */
    pending=tri_live_mask & side_monster_mask(side_for_record(target)) & ~(1u<<target);
    while(pending) { int i=take_mask_bit(&pending); if(tri_live(i))++count; }
    return count;
}
static void tri_begin_resolution(void)
{
    int i,id=gDuel_wEffectCardID;
    if(!tri_live_hint || !(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE))return;
    if(tri_resolution_active && tri_resolution_id==id)return;
    tri_resolution_active=1;tri_resolution_id=id;tri_guard=0;
    memset(tri_guard_data,0,sizeof(tri_guard_data));
    if(!ritual_card_effect_enabled(TRI_HORNED_ID) || gDuel_bTerrain!=WASTELAND_TERRAIN ||
       id<1 || (((unsigned)gDuel_adwCardStats[id-1]>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK)!=CARD_TYPE_MAGIC)return;
    /* Snapshot BEFORE the dispatcher can remove any source. Retain until the
     * entire multi-frame effect ends; never rescan after a source dies. */
    for(i=5;i<10;++i)
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           card_type_id(&D_801A7AD8[i])==CARD_TYPE_DINOSAUR && tri_sources(i)) {
            tri_guard|=1u<<i;tri_guard_data[i]=D_801A7AD8[i].data;
        }
    for(i=20;i<25;++i)
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           card_type_id(&D_801A7AD8[i])==CARD_TYPE_DINOSAUR && tri_sources(i)) {
            tri_guard|=1u<<i;tri_guard_data[i]=D_801A7AD8[i].data;
        }
}
static int tri_guarded(int i)
{
    /* A different allied ritual Tri-Horned may guard this one; the same
     * physical source is never counted by tri_sources(i). */
    if(i<0 || i>=DUEL_CARD_RECORD_COUNT)return 0;
    return tri_resolution_active && (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) &&
        monster_slot(i) && ritual_card_effect_enabled(TRI_HORNED_ID) &&
        gDuel_wEffectCardID==tri_resolution_id &&
        (tri_guard&(1u<<i)) && D_801A7AD8[i].data==tri_guard_data[i] &&
        (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED);
}
static int tri_spend_horn(int i)
{
    if(!ritual_card_effect_enabled(TRI_HORNED_ID)||!tri_identity(i)||!tri_marks[i].horns)return 0;
    --tri_marks[i].horns;tri_horn_visuals|=1u<<i;
    AudioReplace_Sfx(TRI_HORN_BREAK_SFX_ID,255,0);
    return 1;
}
static unsigned tri_battle_survivors(void)
{
    unsigned restore=0;int n,i;
    if((D_8009B174&0x8f)!=11 || D_8009B22A!=0)return 0;
    for(n=0;n<2;++n) {
        DisplayObject *o=D_800E9EF0[n];i=o?o->field_6A:-1;
        unsigned part=1u<<n;
        int needs_rebuild=D_800E9EF0[n+2]==0;
        if(!o || (tri_battle_charged_parts&part) ||
           (!needs_rebuild && !(tri_2d_survivor_parts&part)))continue;
        if(tri_spend_horn(i)) {
            tri_battle_charged_parts|=part;
            if(needs_rebuild)restore|=1u<<i;
            else tri_battle_visuals|=1u<<i; /* Retail rebuilds it in state 11. */
        }
    }
    return restore;
}
static void tri_capture(DisplayObject *o,int x,int y)
{
    int i=o?o->field_6A:-1,phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    if(!tri_live(i) || (phase!=4 && phase!=5))return;
    tri_screen[i].data=D_801A7AD8[i].data;tri_screen[i].object=o;
    tri_screen[i].x=x+26;tri_screen[i].y=y+30;tri_screen[i].valid=1;
}
static void tri_icons(void)
{
    int i=-1,w,h,sc,vw,vh,l,t,x,y,n,size,x0,x1,y0,y1;
    unsigned run_index;
    if(!tri_live_hint||!host||!host->fill||!host->overlay_size||!ritual_card_effect_enabled(TRI_HORNED_ID))return;
    { int counter_phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    if((counter_phase!=4 && counter_phase!=5) || D_8009B229)return; }
    /* The normal cursor remains selected while the native details viewer is
     * pending/open/closing. Its authoritative modal-state byte becomes
     * nonzero on the same frame the viewer is requested, before it draws. */
    if(gDuel_bEffectState || (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) ||
       crab_umi_visual_state)return;
    i=ritual_counter_selected_record();
    if(!tri_live(i)||!tri_marks[i].horns||!tri_screen[i].valid||
       tri_screen[i].data!=tri_marks[i].data)return;
    host->overlay_size(host,&w,&h,&sc);vw=w;vh=w*3/4;if(vh>h){vh=h;vw=h*4/3;}l=(w-vw)/2;t=(h-vh)/2;
    size=16*vw/320;if(size<1)size=1;n=tri_marks[i].horns;
    x=l+tri_screen[i].x*vw/320-(n*(size+2)-2)/2;
    y=t+(tri_screen[i].y-48)*vh/240;if(y<t)y=t;
    for(sc=0;sc<n;++sc)for(run_index=0;run_index<tri_icon_run_count;++run_index) {
        const TriIconRun *run=&tri_icon_runs[run_index];unsigned rgba=run->rgba;
        x0=run->start*size/32;x1=(run->start+run->length)*size/32;
        y0=run->row*size/32;y1=(run->row+1)*size/32;
        if(x1>x0&&y1>y0)host->fill(host,x+sc*(size+2)+x0,y+y0,x1-x0,y1-y0,rgba>>8,rgba&255);
    }
}
