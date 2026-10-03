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
static unsigned tri_guard, tri_horn_visuals, tri_battle_visuals;
static void *tri_guard_data[DUEL_CARD_RECORD_COUNT];
static int tri_resolution_active, tri_resolution_id, tri_field_pending;
static int ritual_field_id=UMI_ID;
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
    int i,start; unsigned count=0;
    if(!monster_slot(target)||!ritual_card_effect_enabled(TRI_HORNED_ID))return 0;
    start=side_for_record(target)?20:5;
    for(i=start;i<start+5;++i)if(i!=target && tri_live(i))++count;
    return count;
}
static void tri_begin_resolution(void)
{
    int i,id=gDuel_wEffectCardID;
    if(!(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE))return;
    if(tri_resolution_active && tri_resolution_id==id)return;
    tri_resolution_active=1;tri_resolution_id=id;tri_guard=0;
    memset(tri_guard_data,0,sizeof(tri_guard_data));
    if(!ritual_card_effect_enabled(TRI_HORNED_ID) || gDuel_bTerrain!=WASTELAND_TERRAIN ||
       id<1 || (((unsigned)gDuel_adwCardStats[id-1]>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK)!=CARD_TYPE_MAGIC)return;
    /* Snapshot BEFORE the dispatcher can remove any source. Retain until the
     * entire multi-frame effect ends; never rescan after a source dies. */
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i)
        if(monster_slot(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           card_type_id(&D_801A7AD8[i])==CARD_TYPE_DINOSAUR && tri_sources(i)) {
            tri_guard|=1u<<i;tri_guard_data[i]=D_801A7AD8[i].data;
        }
}
static int tri_guarded(int i)
{
    return monster_slot(i) && ritual_card_effect_enabled(TRI_HORNED_ID) &&
        tri_resolution_active && (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) &&
        gDuel_wEffectCardID==tri_resolution_id &&
        (tri_guard&(1u<<i)) && D_801A7AD8[i].data==tri_guard_data[i] &&
        (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED);
}
static int tri_spend_horn(int i)
{
    if(!ritual_card_effect_enabled(TRI_HORNED_ID)||!tri_identity(i)||!tri_marks[i].horns)return 0;
    --tri_marks[i].horns;tri_horn_visuals|=1u<<i;return 1;
}
static unsigned tri_battle_survivors(void)
{
    unsigned restore=0;int n,i;
    if((D_8009B174&0x8f)!=11)return 0;
    for(n=0;n<2;++n) {
        DisplayObject *o=D_800E9EF0[n];i=o?o->field_6A:-1;
        if(o && !D_800E9EF0[n+2] && tri_spend_horn(i))restore|=1u<<i;
    }
    return restore;
}
static void tri_capture(DisplayObject *o,int x,int y)
{
    int i=o?o->field_6A:-1;
    if(!tri_live(i)||D_801A7AD8[i].object!=o)return;
    tri_screen[i].data=D_801A7AD8[i].data;tri_screen[i].object=o;
    tri_screen[i].x=x+26;tri_screen[i].y=y+30;tri_screen[i].valid=1;
}
static void tri_icons(void)
{
    int i=-1,w,h,sc,vw,vh,l,t,x,y,n,row,col,size,x0,x1,y0,y1;
    DuelSelectionRecord *cursor;
    if(!host||!host->fill||!host->overlay_size||!ritual_card_effect_enabled(TRI_HORNED_ID))return;
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5 || D_8009B229)return;
    /* The normal cursor remains selected while the native details viewer is
     * pending/open/closing. Its authoritative modal-state byte becomes
     * nonzero on the same frame the viewer is requested, before it draws. */
    if(gDuel_bEffectState || (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) ||
       crab_umi_visual_state)return;
    /* Exactly the approved v0.62 Garma native-active-cursor gate. */
    cursor=&((DuelSelectionSide *)D_800E9F10)[D_8009B1D5].records[2];
    if(D_8009B1B4!=(DuelCardPickCursor *)cursor)return;
    if(cursor->row>=0&&cursor->row<4&&cursor->col>=0&&cursor->col<5)
        i=D_800907D8[D_8009B1D5*20+cursor->row*5+cursor->col];
    if(!tri_live(i)||!tri_marks[i].horns||!tri_screen[i].valid||
       tri_screen[i].data!=tri_marks[i].data||tri_screen[i].object!=D_801A7AD8[i].object)return;
    host->overlay_size(host,&w,&h,&sc);vw=w;vh=w*3/4;if(vh>h){vh=h;vw=h*4/3;}l=(w-vw)/2;t=(h-vh)/2;
    size=16*vw/320;if(size<1)size=1;n=tri_marks[i].horns;
    x=l+tri_screen[i].x*vw/320-(n*(size+2)-2)/2;
    y=t+(tri_screen[i].y-48)*vh/240;if(y<t)y=t;
    for(sc=0;sc<n;++sc)for(row=0;row<32;++row)for(col=0;col<32;++col) {
        unsigned rgba=tri_horn_rgba[row*32+col];if(!(rgba&255))continue;
        x0=col*size/32;x1=(col+1)*size/32;y0=row*size/32;y1=(row+1)*size/32;
        if(x1>x0&&y1>y0)host->fill(host,x+sc*(size+2)+x0,y+y0,x1-x0,y1-y0,rgba>>8,rgba&255);
    }
}
