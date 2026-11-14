#ifndef RITUAL_VIEWER_DESCRIPTION_FIT_H
#define RITUAL_VIEWER_DESCRIPTION_FIT_H
/* Scoped viewer adjustment for long Crab Turtle descriptions.  This borrows
 * only the layout approach from Vanilla Field Effects: capture the first
 * description entry after the F8 stream switch, suppress only those entries
 * in the stock renderer, and draw the SAME live glyphs at a smaller pitch.
 * No text edits in the display hook and no other card is affected.
 *
 * Use original hook chaining: another MOD may also hook the stock viewer.
 */
static void (*ritual_viewer_original_draw)(DisplayObject *, GsOT *);
static void (*ritual_viewer_original_command)(DuelEffectChannel *);
static DuelEffectEntry *ritual_viewer_desc_start[DUEL_EFFECT_CHANNEL_COUNT];
#define RITUAL_DESC_BOTTOM_Y 171
#define RITUAL_DESC_NATIVE_PITCH 12
#define RITUAL_DESC_MIN_PITCH 5
#define RITUAL_DESC_CHAR_WIDTH 8

static void ritual_viewer_fit_reset(void)
{
    memset(ritual_viewer_desc_start,0,sizeof(ritual_viewer_desc_start));
}
static void ritual_viewer_fit_command(DuelEffectChannel *box)
{
    unsigned char command=0;
    int idx=box?box->index_57:-1;
    if(box && box==D_8009B250 && gDuel_wViewerCardID==CRAB_TURTLE_ID &&
       box->stream_58>=0 && box->stream_58<TEXT_STREAM_SLOT_COUNT) {
        TextStreamOwner *stream=(TextStreamOwner *)box;
        unsigned char *p=stream->streams[box->stream_58];
        if(p)command=*p;
        if(idx>=0 && idx<DUEL_EFFECT_CHANNEL_COUNT && (command&0x20))
            ritual_viewer_desc_start[idx]=0;
    }
    ritual_viewer_original_command(box);
    if(box && box==D_8009B250 && gDuel_wViewerCardID==CRAB_TURTLE_ID &&
       idx>=0 && idx<DUEL_EFFECT_CHANNEL_COUNT && (command&0x40))
        ritual_viewer_desc_start[idx]=box->entry_end_20;
}
static DuelEffectEntry *ritual_viewer_live_desc(DuelEffectChannel *box)
{
    DuelEffectEntry *e;
    int idx;
    if(!box || (idx=box->index_57)<0 || idx>=DUEL_EFFECT_CHANNEL_COUNT)return 0;
    e=ritual_viewer_desc_start[idx];
    if(!e || !box->entry_head_24 || !box->entry_end_20 ||
       e<box->entry_head_24 || e>=box->entry_end_20)return 0;
    return e;
}
static int ritual_viewer_desc_bounds(DuelEffectChannel *box,DuelEffectEntry *first,
                                     int *top,int *last,int *bottom)
{
    DuelEffectEntry *p;int found=0,lo=0,hi=0,end=0;
    for(p=first;p<box->entry_end_20 && (p->flags_11&DUEL_EFFECT_ENTRY_FLAG_ACTIVE);++p){
        int y=p->y_0E,h=(p->flags_11&0x20)?16:(p->field_18==1?16:12);
        if(!found){lo=hi=y;found=1;}else{if(y<lo)lo=y;if(y>hi)hi=y;}
        if(y+h>end)end=y+h;
    }
    if(!found)return 0;
    *top=lo;*last=hi;*bottom=end;return 1;
}
static int ritual_viewer_cell(u16 c,int *u,int *v)
{
    static const u16 table[]={0x8149,0x8168,0x8194,0x8190,0x8193,0x8195,
      0x8166,0x8169,0x816A,0x8196,0x817B,0x8143,0x817C,0x8144,0x815E,
      0x8146,0x8147,0x8183,0x8181,0x8184,0x8148,0x8140,0x83BF,0x83C0,
      0x81C1,0x81A9,0x81A8,0x81BC,0x81BD,0};
    int i;
    if((u32)(c-0x824F)<0x4C){
        if((u32)(c-0x8259)<7||(u32)(c-0x827A)<7){*u=0;*v=0x30;return 1;}
        *u=(c&15)*8;*v=((s32)(c-0x8240)>>4)*12;return 1;
    }
    for(i=0;table[i];++i)if(c==table[i]){
        if(i<15){*u=i*8;*v=0;return 1;}
        if(i<22){*u=i*8-0x30;*v=12;return 1;}
        switch(c){
        case 0x83BF:*u=0x68;*v=0x3C;return 1;
        case 0x83C0:*u=0x70;*v=0x3C;return 1;
        case 0x81C1:*u=0x78;*v=0x3C;return 1;
        case 0x81BD:case 0x81BC:case 0x81A9:case 0x81A8:*u=0;*v=0x30;return 1;
        }
    }
    return 0;
}
static void ritual_viewer_quad(DisplayObject *obj,GsOT *ot,int x,int y,int w,int h,
                               int u,int v,int sw,int sh,int page,int clut)
{
    POLY_FT4 p;u32 rgb=obj->field_0C,attr=obj->attribute;
    SetPolyFT4(&p);
    setRGB0(&p,(u8)rgb,(u8)(rgb>>8),(u8)(rgb>>16));
    setSemiTrans(&p,(attr&0x40000000u)!=0);
    setShadeTex(&p,(attr&0x40u)!=0);
    p.x0=p.x2=(s16)x;p.x1=p.x3=(s16)(x+w);
    p.y0=p.y1=(s16)y;p.y2=p.y3=(s16)(y+h);
    p.u0=p.u2=(u8)u;p.u1=p.u3=(u8)(u+sw);
    p.v0=p.v1=(u8)v;p.v2=p.v3=(u8)(v+sh);
    p.tpage=(u16)(page|((attr>>17)&0x180)|((attr>>23)&0x60));
    p.clut=(u16)clut;
    GsSortPoly(&p,ot,(u16)(s16)obj->field_14);
}
static void ritual_viewer_fit_glyph(DisplayObject *obj,GsOT *ot,DuelEffectEntry *e,
                                    int x,int y,int width,int height)
{
    int u=0,v=0,page=obj->field_66;
    if(!Glyphs_Cell((u32)e->code_00,0,obj->field_66,&page,&u,&v) &&
       !ritual_viewer_cell(e->code_00,&u,&v))return;
    if(HdText_Enabled())page|=HD_TEXT_MARK;
    ritual_viewer_quad(obj,ot,x,y,width,height,u,v,8,12,page,
                      ((e->field_16+0xE8)<<6)|0x28);
}
static void ritual_viewer_fit_icon(DisplayObject *obj,GsOT *ot,DuelEffectEntry *e,
                                   int x,int y,int size)
{
    int type=e->field_10,u,v,cx,cy;
    if(type<0||type>=0x22)return;
    u=((type&7)*16)-128;v=(type&0x38)*2;
    cx=((type&15)*16)+0x200;cy=(type>>4)+0xF9;
    ritual_viewer_quad(obj,ot,x,y,size,size,u,v,15,15,0xB,
                      (cy<<6)|((cx>>4)&0x3F));
}
static void ritual_viewer_fit_draw(DisplayObject *obj,GsOT *ot)
{
    DuelEffectChannel *box=D_8009B250;
    DuelEffectEntry *first,*e;
    int top,last,bottom,lines,available,pitch,width,advance,ox,oy;
    if(!obj||!ot||gDuel_wViewerCardID!=CRAB_TURTLE_ID||!box||
       !D_8009B240||!D_8009B24C||box->field_28!=obj||
       !(box->flags_34&DUEL_EFFECT_CHANNEL_FLAG_ACTIVE)||
       (obj->attribute&0x80000000u)||
       !(first=ritual_viewer_live_desc(box))||
       !ritual_viewer_desc_bounds(box,first,&top,&last,&bottom)||
       bottom<=RITUAL_DESC_BOTTOM_Y){
        ritual_viewer_original_draw(obj,ot);return;
    }
    lines=(last-top)/RITUAL_DESC_NATIVE_PITCH+1;
    if(lines<=0){ritual_viewer_original_draw(obj,ot);return;}
    available=RITUAL_DESC_BOTTOM_Y-top;
    pitch=available/lines;
    if(pitch>RITUAL_DESC_NATIVE_PITCH)pitch=RITUAL_DESC_NATIVE_PITCH;
    if(pitch<RITUAL_DESC_MIN_PITCH)pitch=RITUAL_DESC_MIN_PITCH;
    width=(RITUAL_DESC_CHAR_WIDTH*pitch+6)/12;
    if(width<5)width=5;if(width>RITUAL_DESC_CHAR_WIDTH)width=RITUAL_DESC_CHAR_WIDTH;
    advance=(RITUAL_DESC_CHAR_WIDTH+width+1)/2;
    ox=(s16)obj->field_30.h.field_30;
    oy=(s16)obj->field_30.h.field_32;
    /* Preserve the original card header: only the description entries are
     * skipped, and drawn again with their own CLUT (including green +N). */
    {
        u8 saved=first->flags_11;
        first->flags_11=0;
        ritual_viewer_original_draw(obj,ot);
        first->flags_11=saved;
    }
    for(e=first;e<box->entry_end_20&&(e->flags_11&DUEL_EFFECT_ENTRY_FLAG_ACTIVE);++e){
        int ny=top+((e->y_0E-top)*pitch+6)/12;
        int nx=(e->x_0C*advance+4)/8;
        int x=ox+nx+(advance-width)/2,y=oy+ny;
        if(ny+pitch>RITUAL_DESC_BOTTOM_Y)continue;
        if(e->flags_11&0x20)
            ritual_viewer_fit_icon(obj,ot,e,ox+nx,y,pitch);
        else if(e->flags_11&0x40){
            int u=0,v=0,page=0xB,idx=e->field_10;
            if(!Glyphs_TinyCell((u32)e->code_00,&page,&u,&v)){
                u=((idx&15)*8)-0x80;v=(idx&0xF0)>>1;
            }
            ritual_viewer_quad(obj,ot,x,y,width,pitch>8?8:pitch,
                              u,v,8,8,page,0xFA00|((e->field_16+0x29)&0x3F));
        }else ritual_viewer_fit_glyph(obj,ot,e,x,y,width,pitch);
    }
}
#endif
