/* Auto/style adaptation of coin-flip-lib 1.0. PeterIron's original frames,
 * clock mixing, 1.9 s cubic easing/lift and 2.2 s result hold are retained.
 * There is one central coin and continuation, never a per-card renderer.
 * No gameplay rand(), calls/labels or separately installed library. */
#include "png.h"
enum { RCOIN_NORMAL, RCOIN_ATEM, RCOIN_HEADS=0, RCOIN_TAILS=1 };
typedef void (*RitualCoinDone)(int,void *);
typedef struct { int count,tails; PngImage frames[64]; } RitualCoinSet;
static RitualCoinSet ritual_coin_sets[2];
static struct { int active,style,landed,shown,lift,result_up; uint64_t since,paused_at;
    RitualCoinDone done;void *context; } ritual_coin;
static u16 ritual_coin_pressed;
static int (*ritual_port_menu)(void),(*ritual_port_notice)(void);
static void (*ritual_original_pads)(void),(*ritual_original_run)(void);
static int ritual_coin_port_ui(void)
{ return (ritual_port_menu&&ritual_port_menu()) || (ritual_port_notice&&ritual_port_notice()); }
static void ritual_coin_load(int which)
{
    RitualCoinSet *s=&ritual_coin_sets[which];char path[80],text[128];FILE *f;
    const char *folder=which?"assets/coin-atem":"assets/coin-normal",*at;size_t n;int i,count,tails;
    snprintf(path,sizeof(path),"%s/coin.txt",folder);f=host->open_asset(host,path);if(!f)return;
    n=fread(text,1,sizeof(text)-1,f);fclose(f);text[n]=0;
    at=strstr(text,"frames=");count=at?atoi(at+7):0;at=strstr(text,"tails=");tails=at?atoi(at+6):0;
    if(count<2||count>64||tails<=0||tails>=count)return;
    for(i=0;i<count;++i){long bytes;uint8_t *data;
        snprintf(path,sizeof(path),"%s/%02d.png",folder,i);f=host->open_asset(host,path);if(!f)break;
        if(fseek(f,0,SEEK_END)==0&&(bytes=ftell(f))>0&&bytes<(4L<<20)&&fseek(f,0,SEEK_SET)==0&&(data=malloc((size_t)bytes))){
            if(fread(data,1,(size_t)bytes,f)!=(size_t)bytes||!Png_Decode(data,(size_t)bytes,&s->frames[i]))memset(&s->frames[i],0,sizeof(s->frames[i]));
            free(data);
        }
        fclose(f);if(!s->frames[i].pixels)break;
    }
    if(i==count){s->count=count;s->tails=tails;}
    else {for(i=0;i<64;++i){free(s->frames[i].pixels);memset(&s->frames[i],0,sizeof(s->frames[i]));}}
}
static int RitualCoinFlip_Request(int style,int result,RitualCoinDone done,void *context)
{
    uint64_t clock;
    if(ritual_coin.active||style<0||style>1)return 0;
    memset(&ritual_coin,0,sizeof(ritual_coin));
    clock=host->now_us(host);ritual_coin.since=clock;
    clock^=clock>>17;clock*=0x9E3779B97F4A7C15ull;
    ritual_coin.landed=result==0||result==1?result:(int)((clock>>32)&1);
    ritual_coin.style=style;ritual_coin.done=done;ritual_coin.context=context;
    ritual_coin.active=1;ritual_coin_pressed=0;return 1;
}
static void ritual_coin_frame(void)
{
    uint64_t now,elapsed;int confirm;RitualCoinSet *set;
    if(!ritual_coin.active)return;
    now=host->now_us(host);
    if(ritual_coin_port_ui()){if(!ritual_coin.paused_at)ritual_coin.paused_at=now;ritual_coin_pressed=0;return;}
    if(ritual_coin.paused_at){ritual_coin.since+=now-ritual_coin.paused_at;ritual_coin.paused_at=0;}
    elapsed=now-ritual_coin.since;confirm=(ritual_coin_pressed&0xC0)!=0;ritual_coin_pressed=0;
    set=&ritual_coin_sets[ritual_coin.style];
    if(elapsed<1900000u){double t=(double)elapsed/1900000u,eased=1.0-(1.0-t)*(1.0-t)*(1.0-t);
        int count=set->count?set->count:2,target=ritual_coin.landed?(set->count?set->tails:1):0;
        ritual_coin.shown=(int)(eased*(6*count+target))%count;ritual_coin.lift=(int)(400.0*t*(1.0-t));return;}
    if(!ritual_coin.result_up){ritual_coin.shown=ritual_coin.landed?set->tails:0;ritual_coin.lift=0;ritual_coin.result_up=1;SD_SEPlayFull(7);}
    if(elapsed>=4100000u||(confirm&&elapsed>=2150000u)){
        RitualCoinDone done=ritual_coin.done;void *context=ritual_coin.context;int landed=ritual_coin.landed;
        ritual_coin.active=0;if(done)done(landed,context);
    }
}
static void ritual_coin_overlay(void)
{
    int w,h,scale,size,x,y,dy,dx;RitualCoinSet *s;PngImage *p;
    if(!ritual_coin.active||ritual_coin_port_ui())return;
    host->overlay_size(host,&w,&h,&scale);if(scale<1)scale=1;
    size=80*scale+80*scale*ritual_coin.lift/100*2/5;x=(w-size)/2;y=(h-size)/2;
    s=&ritual_coin_sets[ritual_coin.style];
    host->fill(host,0,0,w,h,0,150);
    if(!s->count)return; /* Missing assets never silently substitute the other coin. */
    p=&s->frames[ritual_coin.shown%s->count];
    /* Upscaling the box filter repeats a source pixel in several output
     * rows/columns. Submit each source run once at its exact integer bounds.
     * Identical pixels, no per-output-pixel averaging or extra frame buffers. */
    if(size>=p->width && size>=p->height){
        for(dy=0;dy<p->height;++dy){
            int y0=(dy*size+p->height-1)/p->height;
            int y1=((dy+1)*size+p->height-1)/p->height;
            for(dx=0;dx<p->width;){
                int begin=dx,x0,x1;uint32_t color=p->pixels[dy*p->width+dx++];
                while(dx<p->width && p->pixels[dy*p->width+dx]==color)++dx;
                if(!(color>>24))continue;
                x0=(begin*size+p->width-1)/p->width;
                x1=(dx*size+p->width-1)/p->width;
                host->fill(host,x+x0,y+y0,x1-x0,y1-y0,color&0xFFFFFF,color>>24);
            }
        }
        return;
    }
    /* Same alpha-weighted box filter as the supplied library. Horizontal
     * identical output pixels are submitted together to reduce fill calls. */
    for(dy=0;dy<size;++dy){int y0=dy*p->height/size,y1=(dy+1)*p->height/size,run=0;uint32_t previous=0;
        if(y1<=y0)y1=y0+1;
        for(dx=0;dx<=size;++dx){uint32_t color=0;
            if(dx<size){int x0=dx*p->width/size,x1=(dx+1)*p->width/size,sx,sy;unsigned long a=0,r=0,g=0,b=0,n=0;
                if(x1<=x0)x1=x0+1;
                for(sy=y0;sy<y1;++sy)for(sx=x0;sx<x1;++sx){uint32_t c=p->pixels[sy*p->width+sx],al=c>>24;a+=al;r+=((c>>16)&255)*al;g+=((c>>8)&255)*al;b+=(c&255)*al;++n;}
                if(a)color=((uint32_t)(a/n)<<24)|((uint32_t)(r/a)<<16)|((uint32_t)(g/a)<<8)|(uint32_t)(b/a);
            }
            if(dx==size||color!=previous){if(previous>>24)host->fill(host,x+run,y+dy,dx-run,1,previous&0xFFFFFF,previous>>24);run=dx;previous=color;}
        }
    }
}
static void ritual_coin_free(void)
{ int j,i;ritual_coin.active=0;for(j=0;j<2;++j){for(i=0;i<64;++i)free(ritual_coin_sets[j].frames[i].pixels);memset(&ritual_coin_sets[j],0,sizeof(ritual_coin_sets[j]));} }
