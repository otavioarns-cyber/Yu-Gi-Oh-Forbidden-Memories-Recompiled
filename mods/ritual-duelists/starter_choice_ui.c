/* 0.4.0: new themed deck lists, highlight and explicit confirmation. */
#include "pc/mods/modapi.h"
#include "game/input.h"
#include "game/main_frame.h"
#include "game/save_data.h"
#include "game/sound.h"
#include "pc/cards/cards.h"
#include "pc/platform/title_jump.h"
#include "decks.h"
#include "starter_bridge.h"
#include "psyq/libgte.h"
#include "psyq/libgpu.h"
#include "psyq/libgs.h"
#include "game/ordering_tables.h"
#include "game/graphics_frame.h"
#include <stdint.h>
#include "native_assets.h"
#include "pc/render/soft_gpu.h"
#include "pc/render/texture_pack.h"
#include <string.h>
static void canvas_fill(const MemoriesModHost *,int,int,int,int,unsigned,unsigned);
#include "box_art.h"
#include "pc/platform/menu.h"
#include <string.h>
#include <stdio.h>

static const MemoriesModHost *host;
static int hd_ready,hd_last_scale=-1;
static POLY_FT4 hd_packets[32];
static int hd_count,hd_draw_remaining,hd_tried,hd_asset_scale;
static u8 saved_clear;
static int saved_clear_valid;
static int hd_setup(void);
static void hd_quad(int,int,int,int,int,int,int,int);
#define HD_OWNER 0x535443u
#define HD_BANK 14
static unsigned short native_pixels[320*240],native_saved[320*240];
static int native_saved_valid;
static unsigned native_last_signature=~0u;
static void native_end(void);
static POLY_FT4 native_quads[2];
static void canvas_fill(const MemoriesModHost *unused,int x,int y,int w,int ht,unsigned rgb,unsigned alpha) {
    int xx,yy,x1=(x+w)/2,y1=(y+ht)/2;unsigned sr=(rgb>>19)&31,sg=(rgb>>11)&31,sb=(rgb>>3)&31;
    (void)unused;x/=2;y/=2;if(x<0)x=0;if(y<0)y=0;if(x1>320)x1=320;if(y1>240)y1=240;
    if(!alpha)return;
    for(yy=y;yy<y1;yy++)for(xx=x;xx<x1;xx++){
        unsigned c=native_pixels[yy*320+xx];
        if(alpha==255)c=sr|(sg<<5)|(sb<<10);
        else c=(((c&31)*(255-alpha)+sr*alpha)/255)|(((((c>>5)&31)*(255-alpha)+sg*alpha)/255)<<5)|(((((c>>10)&31)*(255-alpha)+sb*alpha)/255)<<10);
        native_pixels[yy*320+xx]=(unsigned short)(c?c:0x8000);
    }
}
static int canvas_width(const MemoriesModHost *unused,const char *text,int scale){(void)unused;(void)scale;return (int)strlen(text)*16;}
static void canvas_text(const MemoriesModHost *unused,int x,int middle,const char *text,unsigned rgb,int scale) {
    int xx,yy;(void)unused;(void)rgb;(void)scale;x/=2;middle=middle/2-6;
    while(*text){unsigned char c=(unsigned char)*text++;if(c>=128)c='?';
        for(yy=0;yy<12;yy++)for(xx=0;xx<8;xx++){unsigned v=native_letters[c][yy*8+xx];if(v!=65535&&x+xx>=0&&x+xx<320&&middle+yy>=0&&middle+yy<240)native_pixels[(middle+yy)*320+x+xx]=v?v:0x8000;}
        x+=8;
    }
}
/* Nine-slice border copied from the game's own boot/menu panel. */
static void canvas_panel(int x,int y,int w,int ht) {
    int xx,yy;x/=2;y/=2;w/=2;ht/=2;
    for(yy=0;yy<ht;yy++)for(xx=0;xx<w;xx++){
        int sx=xx<3?xx:(xx>=w-3?30-(w-xx):15);
        int sy=yy<3?yy:(yy>=ht-3?21-(ht-yy):10);
        unsigned v=native_frame[sy*30+sx];
        if(v!=65535&&x+xx>=0&&x+xx<320&&y+yy>=0&&y+yy<240)native_pixels[(y+yy)*320+x+xx]=v?v:0x8000;
    }
}

static void (*original_deal)(void);
static int active, selected, confirmed, armed, release_frames;
static int unlocked,code_index;
static int prompt_open,prompt_yes;
static int opening[4],hit_x[4],hit_y[4],hit_w[4],hit_h[4];
static int hover=-1,pointer_mode,mouse_confirm;
static unsigned short previous;
/* Modal persistence is registered together with the Ritual Duelists state. */
#define stored (*Rd_StarterModal())
/* host->pad uses raw PS1 bits; game/input.h is the game's swapped-byte view. */
#define CHOICE_UP 0x0010
#define CHOICE_RIGHT 0x0020
#define CHOICE_DOWN 0x0040
#define CHOICE_LEFT 0x0080
#define CHOICE_CONFIRM (0x4000|0x8000|0x0008)
#define CHOICE_CANCEL 0x2000

static void clear(void)
{
    if(native_saved_valid)native_end();
    active=confirmed=armed=release_frames=0;
    selected=unlocked=code_index=0;previous=0;
    prompt_open=0;prompt_yes=1;
    memset(opening,0,sizeof(opening));hover=-1;pointer_mode=mouse_confirm=0;
}
static void applied(int on) { (void)on; clear(); }

static void state_event(MemoriesModEvent *event)
{
    if(event->type==MEMORIES_EVENT_SAVE && event->phase==MEMORIES_BEFORE) {
        stored.active=active;stored.selected=selected;stored.confirmed=confirmed;
        stored.armed=armed;stored.release_frames=release_frames;stored.previous=previous;
        stored.unlocked=unlocked;stored.code_index=code_index;
        stored.prompt_open=prompt_open;stored.prompt_yes=prompt_yes;
    } else if(event->type==MEMORIES_EVENT_LOAD && event->phase==MEMORIES_AFTER) {
        active=stored.active;selected=stored.selected;confirmed=stored.confirmed;
        armed=stored.armed;release_frames=stored.release_frames;previous=stored.previous;
        unlocked=stored.unlocked;code_index=stored.code_index;
        prompt_open=stored.prompt_open;prompt_yes=stored.prompt_yes;
        native_last_signature=~0u;
        if(!active){native_saved_valid=0;saved_clear_valid=0;}
        hd_draw_remaining=2; /* The loaded scene owns its restored VRAM. */
        if(selected<0 || selected>2 ||
           code_index<0 || code_index>9 || prompt_open<0 || prompt_open>1 ||
           prompt_yes<0 || prompt_yes>1)clear();
    }
}

/* Raw pad is still available through host->pad; only the underlying game is
 * blocked. No work/allocation/logging in this VBlank input callback. */
static void input(MemoriesModEvent *event)
{
    if (active && event->phase==MEMORIES_BEFORE) {
        event->handled=1;event->b=event->result=0;
    }
}

static void frame(void)
{
    static const unsigned short code[10]={0x10,0x10,0x40,0x40,0x80,0x20,0x80,0x20,0x2000,0x4000};
    unsigned short pad,pressed;
    int count;
    if (!active)return;
    for(count=0;count<4;count++) {
        int goal=(!pointer_mode?count==selected:count==hover)&& (count<3||unlocked);
        if(goal&&opening[count]<20)opening[count]+=2;
        else if(!goal&&opening[count]>0)opening[count]-=2;
    }
    pad=host->pad(host,0);
    pressed=(unsigned short)(pad & ~previous);
    previous=pad;
    if(pressed & (CHOICE_UP|CHOICE_RIGHT|CHOICE_DOWN|CHOICE_LEFT))pointer_mode=0;
    if(mouse_confirm){pressed|=0x4000;mouse_confirm=0;}
    /* The name confirmation must be released before choosing a deck. */
    if (!armed) {
        if (!pad) {if (++release_frames>=2)armed=1;}
        else release_frames=0;
        return;
    }
    if(prompt_open) {
        if(pressed & CHOICE_CANCEL) {
            prompt_open=0;
            SD_SEPlayFull(11);
            host->log(host,"starter-choice: CANCEL_PROMPT");
        } else if(pressed & (CHOICE_LEFT|CHOICE_RIGHT|CHOICE_UP|CHOICE_DOWN)) {
            prompt_yes=!prompt_yes;
            SD_SEPlayFull(11);
        } else if(pressed & CHOICE_CONFIRM) {
            if(prompt_yes) {
                confirmed=1;
                SD_SEPlayFull(45);
            } else {
                prompt_open=0;
                SD_SEPlayFull(11);
                host->log(host,"starter-choice: CANCEL_PROMPT");
            }
        }
        return;
    }
    /* Ritual Duelists offers exactly three starter themes, no hidden fourth. */
    count=3;
    if (pressed & (CHOICE_UP|CHOICE_LEFT)) {selected=(selected+count-1)%count;SD_SEPlayFull(11);}
    else if (pressed & (CHOICE_DOWN|CHOICE_RIGHT)) {selected=(selected+1)%count;SD_SEPlayFull(11);}
    else if (pressed & CHOICE_CONFIRM) {
        prompt_open=1;prompt_yes=1;
        SD_SEPlayFull(45);
        host->log(host,"starter-choice: PROMPT deck=%d",selected);
    }
}

static unsigned signature(void) {
    return active?(1u+(unsigned)selected+(armed?8u:0u)+(unlocked?16u:0u)+
                   (prompt_open?32u:0u)+(prompt_yes?64u:0u)+(unsigned)opening[0]*128u+(unsigned)opening[1]*4096u+(unsigned)opening[2]*131072u+(unsigned)opening[3]*4194304u):0u;
}

/* Soft oval glow built from translucent horizontal bands, not a backdrop box. */
static void deck_glow(int cx,int cy,int width,int height,unsigned color,int s)
{
    int layer,dy,rx,ry,step,extent,alpha;
    for(layer=0;layer<4;layer++) {
        rx=width/2+(26+layer*12)*s;
        ry=height/2+(16+layer*10)*s;
        step=3*s;
        for(dy=-ry;dy<=ry;dy+=step) {
            /* Squared falloff forms a soft, elongated oval behind the art. */
            extent=rx*(ry*ry-dy*dy)/(ry*ry);
            alpha=(4-layer)*8;
            if(extent>0)canvas_fill(host,cx-extent,cy+dy,2*extent,step,color,alpha);
        }
    }
}

static void render_canvas(void)
{
    static const unsigned colors[4]={0x67c8e0,0xe99c61,0xa2d17c,0xcbb1ff};
    int w,h,s,x,y,panel,row,top,padding,count,gap,cardw,cardh,artw,arth,allw;
    int imgx,imgy,zoomw,zoomh,cx,cy,boxw,boxh,boxx,boxy;
    if(!active)return;
    w=640;h=480;s=1;
    if(w<=0||h<=0)return;
    s=h/480;if(s<1)s=1;
    count=3;gap=12*s;padding=12*s;
    cardw=(w-32*s-gap*(count-1))/count;if(cardw>230*s)cardw=230*s;
    if(cardw<50)cardw=50;
    artw=cardw-2*padding;if(artw>160*s)artw=160*s;
    arth=artw*BOX_H/BOX_W;
    cardh=arth+32*s;
    if(cardh>h-132*s){arth=h-216*s;if(arth<48)arth=48;artw=arth*BOX_W/BOX_H;cardh=arth+84*s;}
    allw=count*cardw+(count-1)*gap;x=(w-allw)/2;top=(h-cardh-116*s)/2;
    if(top<12)top=12;
    if(hd_ready)memset(native_pixels,0,sizeof(native_pixels));else memcpy(native_pixels,native_background,sizeof(native_pixels));
    if(!hd_ready)canvas_fill(host,0,0,w,h,0x080d18,55);
    canvas_panel(x-12*s,top+4*s,allw+24*s,36*s);
    canvas_text(host,(w-canvas_width(host,"ESCOLHA SEU DECK INICIAL",s))/2,top+12*s,"ESCOLHA SEU DECK INICIAL",0xe8c986,s);
    y=top+44*s;
    for(row=0;row<count;row++) {
        if(row==selected)continue;
        panel=x+row*(cardw+gap);
        if(!hd_ready)box_draw(host,row,panel+(cardw-artw)/2,y+12*s,artw,arth,opening[row]);
        hit_x[row]=panel+(cardw-artw)/2;hit_y[row]=y+12*s;hit_w[row]=artw;hit_h[row]=arth;
    }
    panel=x+selected*(cardw+gap);
    cx=panel+cardw/2;cy=y+12*s+arth/2;
    zoomw=artw*118/100;zoomh=arth*118/100;
    imgx=cx-zoomw/2;imgy=cy-zoomh/2;
    if(!hd_ready)deck_glow(cx,cy,zoomw,zoomh,colors[selected],s);
    if(!hd_ready)box_draw(host,selected,imgx,imgy,zoomw,zoomh,opening[selected]);
    hit_x[selected]=imgx;hit_y[selected]=imgy;hit_w[selected]=zoomw;hit_h[selected]=zoomh;
    canvas_panel(x-12*s,y+cardh+12*s,allw+24*s,60*s);
    canvas_text(host,(w-canvas_width(host,"Direcionais: mover   X: escolher",s))/2,y+cardh+26*s,"Direcionais: mover   X: escolher",0xe8c986,s);
    if(!armed)canvas_text(host,x,y+cardh+48*s,"Solte o botao para continuar.",0xa8bdc8,s);
    if(prompt_open) {
        /* Small, readable dialog over the existing arena. */
        boxw=390*s;if(boxw>w-24*s)boxw=w-24*s;
        boxh=124*s;boxx=(w-boxw)/2;boxy=(h-boxh)/2;
        canvas_fill(host,0,0,w,h,0x00040a,110);
        canvas_panel(boxx-3*s,boxy-3*s,boxw+6*s,boxh+6*s);
        canvas_text(host,(w-canvas_width(host,"TEM CERTEZA?",s))/2,boxy+25*s,"TEM CERTEZA?",0xf5dfa2,s);
        canvas_fill(host,boxx+52*s,boxy+68*s,(boxw-104*s)/2,29*s,prompt_yes?0x615331:0x22283a,prompt_yes?235:180);
        canvas_fill(host,boxx+52*s+(boxw-104*s)/2,boxy+68*s,(boxw-104*s)/2,29*s,!prompt_yes?0x615331:0x22283a,!prompt_yes?235:180);
        canvas_text(host,boxx+52*s+(boxw-104*s)/4-canvas_width(host,"SIM",s)/2,boxy+76*s,"SIM",prompt_yes?0xffebae:0xbac2d0,s);
        canvas_text(host,boxx+52*s+3*(boxw-104*s)/4-canvas_width(host,"NAO",s)/2,boxy+76*s,"NAO",!prompt_yes?0xffebae:0xbac2d0,s);
        canvas_text(host,(w-canvas_width(host,"X: confirmar    O: voltar",s))/2,boxy+106*s,"X: confirmar    O: voltar",0xaab7c5,s);
    }
}

static void native_begin(void) {
    RECT rect={640,0,320,240};
    StoreImage(&rect,(u32 *)native_saved);native_saved_valid=1;native_last_signature=~0u;
    saved_clear=D_8009B0D0;saved_clear_valid=1;D_8009B0D0=0;
    hd_tried=SoftGpu_Scale()>1;hd_ready=hd_tried?hd_setup():0;hd_draw_remaining=2;
}
static void native_end(void) {
    RECT rect={640,0,320,240};
    DrawSync(0);if(native_saved_valid)LoadImage(&rect,(u32 *)native_saved);
    native_saved_valid=0;
    TexturePack_BankSpritesClear(HD_OWNER);hd_ready=0;hd_tried=0;
    if(saved_clear_valid){D_8009B0D0=saved_clear;saved_clear_valid=0;}
}
static void fallback_draw(void) {
    unsigned sig=signature();RECT rect={640,0,320,240};int i;
    if(sig!=native_last_signature){render_canvas();LoadImage(&rect,(u32 *)native_pixels);native_last_signature=sig;}
    for(i=0;i<2;i++){
        GsSPRITE sp;memset(&sp,0,sizeof(sp));sp.x=i?256:0;sp.y=0;sp.w=i?64:256;sp.h=240;
        sp.attribute=0x02000000u;sp.r=sp.g=sp.b=128;sp.tpage=getTPage(2,0,640+sp.x,0);sp.scalex=sp.scaley=4096;
        GsSortFastSprite(&sp,D_800E9D90[0],0);
    }
    /* SDK copies packets into the game's guest OT/packet buffer (G32),
       avoiding native pointer truncation on Android ARM64. */
    GsDrawOt(D_800E9D90[0]);DrawSync(0);
}

static int hd_meta(int index,int *px,int *py,int *u,int *v,int *w,int *h) {
    if(index==18){*px=768;*py=256;*u=*v=0;*w=160;*h=200;return 1;}
    if(index>=16){*px=index==16?0:256;*py=0;*u=*v=0;*w=160;*h=240;return 1;}
    *px=index/6==1?768:512;*py=index/6==2?256:0;
    *u=(index%3)*80;*v=((index%6)/3)*128;*w=80;*h=128;return 1;
}
static int hd_setup(void) {
    int i,px,py,u,v,w,h,y;u16 *bank=SoftGpu_Bank(HD_BANK);static u16 temp[160*240];char path[1024],rel[96];FILE *f;
    if(!bank)return 0;
    hd_asset_scale=SoftGpu_Scale()<=2?2:4;
    TexturePack_BankSpritesClear(HD_OWNER);TexturePack_BankSpritesUseOwner(HD_OWNER);
    for(i=0;i<19;i++){
        hd_meta(i,&px,&py,&u,&v,&w,&h);
        snprintf(rel,sizeof(rel),"assets/hd-layer-%02d.bin",i);f=host->open_asset(host,rel);
        if(!f || fread(temp,sizeof(u16),(size_t)w*h,f)!=(size_t)w*h){if(f)fclose(f);goto failed;}
        fclose(f);
        for(y=0;y<h;y++)memcpy(bank+(py+v+y)*1024+px+u,temp+y*w,(size_t)w*2);
        snprintf(path,sizeof(path),"%s/assets/hd-layer-%02d%s.png",host->directory,i,hd_asset_scale==2?"-2x":"");
        if(!TexturePack_AddBankSpriteCrop(HD_BANK,px,py,2,0,0,u,v,w,h,path,0,0,(i<16?400:(i<18?640:400))/(hd_asset_scale==2?2:1),(i<16?750:(i<18?960:500))/(hd_asset_scale==2?2:1)))goto failed;
        {const unsigned char *rgba=0;int iw=0,ih=0;int entry=TexturePack_BankEntryFor(HD_BANK,px,py,2,0,0,u,v);
         if(!entry||!TexturePack_BankEntryLevel(entry,0,&rgba,&iw,&ih)||!rgba)goto failed;
         if(i==0)host->log(host,"starter-choice: HD_SOURCE=%dx%d entry=%d",iw,ih,entry);}
    }
    TexturePack_BankSpritesUseOwner(0);hd_last_scale=-1;
    {u32 rgb=0;int ok=TexturePack_BankSample(HD_BANK,0,0,2,0,0,80,120,4,&rgb);host->log(host,"starter-choice: HD_SAMPLE=%d rgb=%x bank=%x",ok,rgb,bank[120*1024+80]);}
    host->log(host,"starter-choice: HD_READY bank14; 19 assets preloaded; native sprites and CRT");return 1;
failed:
    TexturePack_BankSpritesUseOwner(0);TexturePack_BankSpritesClear(HD_OWNER);
    host->log(host,"starter-choice: HD unavailable; native 320 fallback retained");return 0;
}
static void hd_quad(int index,int x,int y,int w,int h,int alpha,int cut,int priority) {
    POLY_FT4 q;int px,py,u,v,tw,th,drawh=h;
    if(!alpha||w<=0||h<=0)return;
    hd_meta(index,&px,&py,&u,&v,&tw,&th);
    if(cut>0&&cut<drawh)drawh=cut;
    memset(&q,0,sizeof(q));setPolyFT4(&q);setRGB0(&q,128,128,128);
    q.tpage=getTPage(2,0,px,py)|(u16)(HD_BANK<<11);
    q.x0=q.x2=(short)x;q.x1=q.x3=(short)(x+w);q.y0=q.y1=(short)y;q.y2=q.y3=(short)(y+drawh);
    q.u0=q.u2=(u8)u;q.u1=q.u3=(u8)(u+tw-1);q.v0=q.v1=(u8)v;q.v2=q.v3=(u8)(v+th*drawh/h-1);
    if(alpha<255)q.pad1=SOFT_GPU_FADE|(u16)(255-alpha);
    (void)priority;if(hd_count<32)hd_packets[hd_count++]=q;
}
static void hd_box(int i,int x,int y,int w,int h) {
    int t=opening[i],alpha=t*255/20,off=(110-t*4)*h/300,cut=132*h/300-off;
    hd_quad(i*4,x,y,w,h,255-alpha,0,4);
    if(t){hd_quad(i*4+1,x,y,w,h,alpha,0,3);if(cut>0)hd_quad(i*4+2,x,y+off,w,h,alpha,cut,2);hd_quad(i*4+3,x,y,w,h,alpha,0,1);}
}
static void native_draw(void) {
    int scale=SoftGpu_Scale(),i,count=unlocked?4:3,cardw,artw,arth,cardh,allw,x,top,y,panel;
    unsigned sig=signature();u16 *bank;POLY_FT4 ui;
    if(hd_ready && scale>1 && (scale<=2?2:4)!=hd_asset_scale){hd_ready=hd_setup();native_last_signature=~0u;hd_draw_remaining=2;}
    if(scale>1&&!hd_ready&&!hd_tried){hd_tried=1;hd_ready=hd_setup();native_last_signature=~0u;hd_draw_remaining=2;}
    if(!hd_ready){if(scale!=hd_last_scale){hd_draw_remaining=2;native_last_signature=~0u;hd_last_scale=scale;host->log(host,"starter-choice: INTERNAL_SCALE=%d; native fallback",scale);}fallback_draw();return;}
    if(scale!=hd_last_scale){hd_last_scale=scale;host->log(host,"starter-choice: INTERNAL_SCALE=%d; native HD assets selected by renderer",scale);}
    bank=SoftGpu_Bank(HD_BANK);if(!bank){hd_ready=0;fallback_draw();return;}
    if(sig!=native_last_signature){render_canvas();for(i=0;i<240;i++)memcpy(bank+(256+i)*1024,native_pixels+i*320,640);native_last_signature=sig;hd_draw_remaining=2;}
    if(hd_draw_remaining<=0)return;
    hd_draw_remaining--;
    hd_count=0;
    hd_quad(16,0,0,160,240,255,0,8);hd_quad(17,160,0,160,240,255,0,8);
    cardw=(640-32-12*(count-1))/count;if(cardw>230)cardw=230;artw=cardw-24;if(artw>160)artw=160;arth=artw*300/160;cardh=arth+32;
    allw=count*cardw+(count-1)*12;x=(640-allw)/2;top=(480-cardh-116)/2;if(top<12)top=12;y=top+44;
    for(i=0;i<count;i++)if(i!=selected){panel=x+i*(cardw+12);hd_box(i,(panel+(cardw-artw)/2)/2,(y+12)/2,artw/2,arth/2);}
    panel=x+selected*(cardw+12);{
        int zw=artw*118/100,zh=arth*118/100,cx=panel+cardw/2,cy=y+12+arth/2;
        hd_quad(18,(cx-zw/2)/2-12,(cy-zh/2)/2-18,zw/2+24,zh/2+36,180,0,6);
        hd_box(selected,(cx-zw/2)/2,(cy-zh/2)/2,zw/2,zh/2);
    }
    /* Native text/border layer has the original low-res font; all art is separate HD sprites. */
    for(i=0;i<2;i++){
        memset(&ui,0,sizeof(ui));setPolyFT4(&ui);setRGB0(&ui,128,128,128);
        ui.tpage=getTPage(2,0,i?256:0,256)|(u16)(HD_BANK<<11);
        ui.x0=ui.x2=i?256:0;ui.x1=ui.x3=i?320:256;ui.y0=ui.y1=0;ui.y2=ui.y3=240;
        ui.u1=ui.u3=i?63:255;ui.v2=ui.v3=239;
        if(hd_count<32)hd_packets[hd_count++]=ui;
    }
    for(i=hd_count-1;i>=0;i--)GsSortPoly(&hd_packets[i],D_800E9D90[0],0);
    GsDrawOt(D_800E9D90[0]);DrawSync(0);
}

static void deal(void)
{
    int i;
    unsigned char saved_name[SAVE_DATA_PLAYER_NAME_SIZE];
    clear();
    Rd_StarterBegin();
    memcpy(saved_name,gSaveData_aPlayerNameSjis,sizeof(saved_name));
    active=1;
    previous=host->pad(host,0);
    /* Name confirmation faded this loaded track. Replay the name-entry OST. */
    SD_BGMPlay(0x7010);
    host->log(host,"starter-choice: AUDIO BGM 0x7010; cursor=11 confirm=45 unlock=48");
    host->log(host,"starter-choice: OPEN after name confirmation, before campaign");
    native_begin();
    while(active && !confirmed && host->applied(host)) {
        Main_AdvanceFrame();
        if(!active)break;
        /* The name entry fades to black and stops submitting ordering tables.
         * mod->frame runs from GsDrawOt, so poll here even without game draws. */
        frame();
        if(active && !confirmed)native_draw();
        /* A restart longjmps. Leave no stale modal/input capture on title. */
        active=0;
        TitleJump_Poll();
        active=1;
    }
    native_end();
    active=0;
    if(!host->applied(host)) {if(original_deal)original_deal();return;}
    if(!confirmed)return; /* state load/reset: never overwrite a loaded deck */
    SD_BGMFadeOut();
    {
        u16 vanilla[40],themed[40];
        if(!original_deal){host->log(host,"Ritual Duelists: starter Vanilla delegate missing");return;}
        original_deal(); /* The original vanilla 40-card RNG. */
        memcpy(vanilla,gDuel_awPlayerDeck,sizeof(vanilla));
        if(Rd_StarterAccept(selected,vanilla,themed)){
            for(i=0;i<40;i++){
                gDuel_awPlayerDeck[i]=themed[i];
                Cards_MarkSeen(themed[i]);
            }
        }else host->log(host,"Ritual Duelists: invalid active starter recipe; retaining Vanilla deck");
    }
    memcpy(gSaveData_aPlayerNameSjis,saved_name,sizeof(saved_name));
    host->log(host,"starter-choice: SELECTED %d %s; 40 cards; name preserved=%d",selected,initial_names[selected],memcmp(saved_name,gSaveData_aPlayerNameSjis,sizeof(saved_name))==0);
    /* Drain the confirmation so it cannot also advance Heishin's first line. */
    active=1;
    while(active && host->applied(host) && host->pad(host,0)) {
        Main_AdvanceFrame();
        if(!active)break;
        active=0;TitleJump_Poll();active=1;
    }
    clear();
}

int Rd_StarterUIInit(const MemoriesModHost *h,MemoriesMod *mod)
{
    void *target;
    /* Target the tested preview engine and its guest-branch ABI. */
    if(h->api<10)return 0;
    host=h;
    target=host->symbol(host,"NameEntry_BuildStarterDeck");
    if(!target || !host->hook(host,target,deal,(void **)&original_deal))return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_INPUT,1000,input))return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_SAVE,0,state_event))return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_LOAD,0,state_event))return 0;
    mod->api=10;mod->overlay=0;mod->overlay_signature=0;
    mod->reset=clear;mod->applied=applied;
    clear();
    return 1;
}

void Rd_StarterUIReset(void){clear();}
