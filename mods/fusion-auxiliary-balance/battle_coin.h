/* Auto/style adaptation of coin-flip-lib 1.0. PeterIron's original frames,
 * clock mixing, 1.9 s cubic easing/lift and 2.2 s result hold are retained.
 * There is one central coin and continuation, never a per-card renderer.
 * No gameplay rand(), calls/labels or separately installed library. */
#include "png.h"
enum { BWCOIN_NORMAL, BWCOIN_ATEM, BWCOIN_HEADS=0, BWCOIN_TAILS=1 };
typedef void (*AuxBattleCoinDone)(int,void *);
typedef struct { int count,tails; PngImage frames[64]; } AuxBattleCoinSet;
static AuxBattleCoinSet bwcoin_sets[2];
static struct { int active,style,landed,shown,lift,result_up; uint64_t since,paused_at;
    AuxBattleCoinDone done;void *context; } bwcoin;
static u16 bwcoin_pressed;
static int (*bw_port_menu)(void),(*bw_port_notice)(void);
/* Exact integer source/destination boundaries used by the scaler. The coin
 * changes size while it lifts, but thousands of pixels/runs share the same
 * boundaries within one frame. Cache those divisions once per size. */
#define BWCOIN_EDGE_MAX 256
static int bwcoin_xedge[BWCOIN_EDGE_MAX+1],bwcoin_yedge[BWCOIN_EDGE_MAX+1];
static int bwcoin_edge_size=-1,bwcoin_edge_w=-1,bwcoin_edge_h=-1,bwcoin_edge_up=-1;
static int bwcoin_prepare_edges(int size,int w,int h,int upscale)
{
    int i,xn=upscale?w:size,yn=upscale?h:size;
    if(xn<0||yn<0||xn>BWCOIN_EDGE_MAX||yn>BWCOIN_EDGE_MAX||w<=0||h<=0||size<=0)return 0;
    if(bwcoin_edge_size==size&&bwcoin_edge_w==w&&bwcoin_edge_h==h&&bwcoin_edge_up==upscale)return 1;
    if(upscale) {
        for(i=0;i<=w;++i)bwcoin_xedge[i]=(i*size+w-1)/w;
        for(i=0;i<=h;++i)bwcoin_yedge[i]=(i*size+h-1)/h;
    } else {
        for(i=0;i<=size;++i)bwcoin_xedge[i]=i*w/size;
        for(i=0;i<=size;++i)bwcoin_yedge[i]=i*h/size;
    }
    bwcoin_edge_size=size;bwcoin_edge_w=w;bwcoin_edge_h=h;bwcoin_edge_up=upscale;
    return 1;
}
static void (*bw_original_pads)(void),(*bw_original_run)(void);
static int bwcoin_port_ui(void)
{ return (bw_port_menu&&bw_port_menu()) || (bw_port_notice&&bw_port_notice()); }
static void bwcoin_load(int which)
{
    AuxBattleCoinSet *s=&bwcoin_sets[which];char path[80],text[128];FILE *f;
    const char *folder=which?"assets/coin-atem":"assets/coin-normal",*at;size_t n;int i,count,tails;
    snprintf(path,sizeof(path),"%s/coin.txt",folder);f=aux_host->open_asset(aux_host,path);if(!f)return;
    n=fread(text,1,sizeof(text)-1,f);fclose(f);text[n]=0;
    at=strstr(text,"frames=");count=at?atoi(at+7):0;at=strstr(text,"tails=");tails=at?atoi(at+6):0;
    if(count<2||count>64||tails<=0||tails>=count)return;
    for(i=0;i<count;++i){long bytes;uint8_t *data;
        snprintf(path,sizeof(path),"%s/%02d.png",folder,i);f=aux_host->open_asset(aux_host,path);if(!f)break;
        if(fseek(f,0,SEEK_END)==0&&(bytes=ftell(f))>0&&bytes<(4L<<20)&&fseek(f,0,SEEK_SET)==0&&(data=malloc((size_t)bytes))){
            if(fread(data,1,(size_t)bytes,f)!=(size_t)bytes||!Png_Decode(data,(size_t)bytes,&s->frames[i]))memset(&s->frames[i],0,sizeof(s->frames[i]));
            free(data);
        }
        fclose(f);if(!s->frames[i].pixels)break;
    }
    if(i==count){s->count=count;s->tails=tails;}
    else {for(i=0;i<64;++i){free(s->frames[i].pixels);memset(&s->frames[i],0,sizeof(s->frames[i]));}}
}
static int AuxBattleCoinFlip_Request(int style,int result,AuxBattleCoinDone done,void *context)
{
    uint64_t clock;
    if(bwcoin.active||style<0||style>1)return 0;
    memset(&bwcoin,0,sizeof(bwcoin));
    clock=aux_host->now_us(aux_host);bwcoin.since=clock;
    clock^=clock>>17;clock*=0x9E3779B97F4A7C15ull;
    bwcoin.landed=result==0||result==1?result:(int)((clock>>32)&1);
    bwcoin.style=style;bwcoin.done=done;bwcoin.context=context;
    bwcoin.active=1;bwcoin_pressed=0;return 1;
}
static void bwcoin_frame(void)
{
    uint64_t now,elapsed;int confirm;AuxBattleCoinSet *set;
    if(!bwcoin.active)return;
    now=aux_host->now_us(aux_host);
    if(bwcoin_port_ui()){if(!bwcoin.paused_at)bwcoin.paused_at=now;bwcoin_pressed=0;return;}
    if(bwcoin.paused_at){bwcoin.since+=now-bwcoin.paused_at;bwcoin.paused_at=0;}
    elapsed=now-bwcoin.since;confirm=(bwcoin_pressed&0xC0)!=0;bwcoin_pressed=0;
    set=&bwcoin_sets[bwcoin.style];
    if(elapsed<1900000u){double t=(double)elapsed/1900000u,eased=1.0-(1.0-t)*(1.0-t)*(1.0-t);
        int count=set->count?set->count:2,target=bwcoin.landed?(set->count?set->tails:1):0;
        bwcoin.shown=(int)(eased*(6*count+target))%count;bwcoin.lift=(int)(400.0*t*(1.0-t));return;}
    if(!bwcoin.result_up){bwcoin.shown=bwcoin.landed?set->tails:0;bwcoin.lift=0;bwcoin.result_up=1;SD_SEPlayFull(7);}
    if(elapsed>=4100000u||(confirm&&elapsed>=2150000u)){
        AuxBattleCoinDone done=bwcoin.done;void *context=bwcoin.context;int landed=bwcoin.landed;
        bwcoin.active=0;if(done)done(landed,context);
    }
}
static void bwcoin_overlay(void)
{
    int w,h,scale,size,x,y,dy,dx;AuxBattleCoinSet *s;PngImage *p;
    if(!bwcoin.active||bwcoin_port_ui())return;
    aux_host->overlay_size(aux_host,&w,&h,&scale);if(scale<1)scale=1;
    size=80*scale+80*scale*bwcoin.lift/100*2/5;x=(w-size)/2;y=(h-size)/2;
    s=&bwcoin_sets[bwcoin.style];
    aux_host->fill(aux_host,0,0,w,h,0,150);
    if(!s->count)return; /* Missing assets never silently substitute the other coin. */
    p=&s->frames[bwcoin.shown%s->count];
    /* Upscaling the box filter repeats a source pixel in several output
     * rows/columns. Submit each source run once at its exact integer bounds.
     * Identical pixels, no per-output-pixel averaging or extra frame buffers. */
    if(size>=p->width && size>=p->height){
        int cached=bwcoin_prepare_edges(size,p->width,p->height,1);
        for(dy=0;dy<p->height;++dy){
            int y0=cached?bwcoin_yedge[dy]:(dy*size+p->height-1)/p->height;
            int y1=cached?bwcoin_yedge[dy+1]:((dy+1)*size+p->height-1)/p->height;
            for(dx=0;dx<p->width;){
                int begin=dx,x0,x1;uint32_t color=p->pixels[dy*p->width+dx++];
                while(dx<p->width && p->pixels[dy*p->width+dx]==color)++dx;
                if(!(color>>24))continue;
                x0=cached?bwcoin_xedge[begin]:(begin*size+p->width-1)/p->width;
                x1=cached?bwcoin_xedge[dx]:(dx*size+p->width-1)/p->width;
                aux_host->fill(aux_host,x+x0,y+y0,x1-x0,y1-y0,color&0xFFFFFF,color>>24);
            }
        }
        return;
    }
    /* Same alpha-weighted box filter as the supplied library. Horizontal
     * identical output pixels are submitted together to reduce fill calls. */
    { int cached=bwcoin_prepare_edges(size,p->width,p->height,0);
    for(dy=0;dy<size;++dy){int y0=cached?bwcoin_yedge[dy]:dy*p->height/size,y1=cached?bwcoin_yedge[dy+1]:(dy+1)*p->height/size,run=0;uint32_t previous=0;
        if(y1<=y0)y1=y0+1;
        for(dx=0;dx<=size;++dx){uint32_t color=0;
            if(dx<size){int x0=cached?bwcoin_xedge[dx]:dx*p->width/size,x1=cached?bwcoin_xedge[dx+1]:(dx+1)*p->width/size,sx,sy;unsigned long a=0,r=0,g=0,b=0,n=0;
                if(x1<=x0)x1=x0+1;
                for(sy=y0;sy<y1;++sy)for(sx=x0;sx<x1;++sx){uint32_t c=p->pixels[sy*p->width+sx],al=c>>24;a+=al;r+=((c>>16)&255)*al;g+=((c>>8)&255)*al;b+=(c&255)*al;++n;}
                if(a)color=((uint32_t)(a/n)<<24)|((uint32_t)(r/a)<<16)|((uint32_t)(g/a)<<8)|(uint32_t)(b/a);
            }
            if(dx==size||color!=previous){if(previous>>24)aux_host->fill(aux_host,x+run,y+dy,dx-run,1,previous&0xFFFFFF,previous>>24);run=dx;previous=color;}
        }
    }}
}
static void bwcoin_free(void)
{ int j,i;bwcoin.active=0;for(j=0;j<2;++j){for(i=0;i<64;++i)free(bwcoin_sets[j].frames[i].pixels);memset(&bwcoin_sets[j],0,sizeof(bwcoin_sets[j]));} }
