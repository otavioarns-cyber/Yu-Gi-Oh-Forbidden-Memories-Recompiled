#ifndef FMR_AI_MODEL_V2
#define FMR_AI_MODEL_V2
/* No engine globals, allocator, callbacks, RNG or game objects. All board
 * indices are physical five-zone rows in the viewer's ordering. id<0 is unknown. */
enum {EA_BASE=1,EA_FUSION=2,EA_RITUAL=4,EA_METAL=8};
enum {EA_DRAGON=1,EA_DARK=2,EA_FIRE=4,EA_EARTH=8,EA_FEMALE=16,EA_SKULL=32};
typedef struct {int id,t,at,df,printed,pdef,level,traits,active,possible,charge,flags,star,redirect,borrowed;} EaCard;
typedef struct {EaCard row[2][5],hand[5];int lp[2],maxlp[2],terrain,backfree[2],blocked[2],equiplock;} EaBoard;
typedef struct {int relevant,legal,target,delta,kill,loss,hit,hurt,after,score,uncertain;} EaResult;
typedef struct {unsigned abi;int(*on)(void);void(*read)(EaCard*,int,int);int(*policy)(int,int);int(*pair)(int,int,int,int*,int*);} EaProvider;
static int ea_min(int a,int b){return a<b?a:b;}static int ea_max(int a,int b){return a>b?a:b;}
static int ea_cap(int x){return ea_min(9999,ea_max(0,x));}
static int ea_on(EaCard c,int id){return c.id==id&&c.active;}
static int ea_f(EaCard c,int id){return c.id==id&&(c.active&EA_FUSION);}
static int ea_n(const EaCard*r,int except,int type,int trait,int id,int up){
 int n=0;for(int k=0;k<5;k++)if(k!=except&&r[k].id>0&&(!up||!(r[k].flags&4096))&&(type<0||r[k].t==type)&&(!trait||(r[k].traits&trait))&&(!id||ea_on(r[k],id)))n++;return n;}
static int ea_free(const EaCard*r){int n=0;for(int i=0;i<5;i++)n+=!r[i].id;return n;}
static int ea_sources(const EaCard*r,int pos,int id){return ea_n(r,pos,-1,0,id,1);}
static int ea_sea(int t){return t==12||t==13||t==16;}
static int ea_aura(const EaBoard*b,int s,int p,int def){
 const EaCard*r=b->row[s],*e=b->row[s^1];EaCard c=r[p];if(c.id<=0)return 0;int v=0;
 if(ea_f(c,473))v+=200*ea_n(r,p,-1,EA_FIRE,0,1);
 if(ea_f(c,294))v+=500*((ea_n(r,p,0,0,0,1)>0)+(ea_n(r,p,3,0,0,1)>0));
 if(ea_f(c,639)&&b->terrain==(def?2:5))v+=800;
 if(ea_f(c,509)&&b->terrain==1)v+=800;
 if(c.traits&EA_EARTH)v+=200*ea_sources(r,p,531);
 if(c.t==3)v+=1000*ea_sources(r,p,364);
 if(c.t==10)v+=1000*ea_sources(r,p,705);
 if(c.t==2&&!def)v+=1200*ea_sources(r,p,719);
 if(c.t==9)v+=400*ea_n(r,-1,9,0,0,0)*ea_sources(r,p,717);
 if(ea_sea(c.t)&&ea_sources(r,p,710))v+=def?800:200;
 if(ea_on(c,718)&&def)v+=1000*ea_n(r,p,12,0,0,0)+500*(!s?ea_n(b->hand,-1,12,0,0,0):0);
 if(ea_on(c,704))v+=1000*(ea_n(r,p,-1,EA_SKULL,0,0)+ea_n(e,-1,-1,EA_SKULL,0,0))+500*(!s?ea_n(b->hand,-1,-1,EA_SKULL,0,0):0);
 if(ea_on(c,708))v+=500*(ea_n(r,p,-1,EA_FEMALE,0,0)+ea_n(e,-1,-1,EA_FEMALE,0,0)+1);
 if(ea_on(c,356))v+=500*(ea_n(r,p,5,0,0,0)+(!s?ea_n(b->hand,-1,5,0,0,0):0));
 if(c.t!=7&&c.id!=360&&!ea_on(c,380))v-=1500*(ea_sources(r,-1,360)+ea_sources(e,-1,360));
 return v;}
static int ea_guard(const EaBoard*b,int s,int i,EaCard foe){
 const EaCard*r=b->row[s];EaCard c=r[i];
 if(ea_f(c,495)||ea_on(c,722)||(c.charge&&(ea_f(c,533)||ea_on(c,717)||ea_on(c,705))))return 1;
 if(ea_f(c,509)&&b->terrain==1&&foe.t==9)return 1;
 if(ea_on(c,365)&&(c.flags&2048))return 1;
 if(ea_on(c,710)&&b->terrain==5&&foe.t!=15&&!ea_sea(foe.t))return 1;
 if(c.t==0&&c.id!=358&&c.level<=6&&ea_sources(r,i,358))return 1;
 if((c.traits&EA_FEMALE)&&ea_sources(r,i,572))return 1;
 if(ea_f(c,427))for(int j=0;j<5;j++)if(j!=i&&r[j].id>0&&r[j].id!=427&&r[j].t==0)return 1;
 return 0;}
static int ea_death(const EaCard*r,int i,int*hit){
 EaCard c=r[i];int val=0;if(ea_f(c,215))*hit+=500;
 if(ea_f(c,133))val+=650; /* dying card frees its zone */
 if(ea_f(c,272)&&c.charge)val+=400; /* next own turn, not an immediate body */
 if(ea_f(c,30))val+=300*ea_n(r,i,-1,EA_SKULL,0,0);
 if(c.traits&EA_FEMALE)val+=400*ea_sources(r,i,618);
 return val;}
static EaResult ea_case(const EaBoard*b,int side,int i,int j,int star,int swap){
 const EaCard*r=b->row[side],*e=b->row[side^1];EaCard a=r[i],d=e[j];EaResult f={0};f.target=j;f.legal=1;
 if(a.id<=0||d.id<=0)return f;
 for(int k=0;k<5;k++)if(r[k].active||e[k].active||e[k].redirect)f.relevant=1;
 if(!f.relevant)return f;
 if(b->blocked[side]||(ea_f(d,487)&&ea_n(e,j,19,0,0,1))||(ea_on(d,706)&&ea_free(e)<4)){f.legal=0;return f;}
 if(d.redirect>0&&d.redirect<=5){j=d.redirect-1;d=e[j];f.target=j;if(d.id<=0){f.uncertain=1;return f;}}
 int at=a.at,ad=a.df,dt=d.at,df=d.df,def=(d.flags&2048)!=0,imm=ea_on(d,380);
 if(ea_on(d,703)){at=ea_min(at,a.printed);ad=ea_min(ad,a.pdef);if(star>0)star=0;}
 if(ea_on(a,703)){dt=ea_min(dt,d.printed);df=ea_min(df,d.pdef);}
 for(int k=0;k<5;k++)if((k==j-1||k==j+1)&&ea_f(e[k],465)&&!ea_on(a,380)){at-=400;ad-=400;}
 if(ea_f(a,565)&&d.at<d.printed)at+=1000;
 if(ea_f(a,385)&&!imm){int v=dt;dt=df;df=v;}
 if(ea_f(a,69)&&def&&!imm)df-=300;
 if(ea_f(a,467)&&!imm){dt-=200;df-=200;}
 if(ea_f(a,223)&&!imm)def=0;
 if(ea_f(a,521)){at+=400;if(!imm)dt-=400;}
 if(ea_f(a,437)&&(d.flags&4096))at+=700;
 if(ea_f(a,482))at+=300*ea_n(r,i,10,0,0,1);
 if(ea_f(a,389)&&(d.traits&EA_DARK))at+=600;
 if(ea_f(a,15)&&(d.t==2||d.t==19))at+=600;
 if(ea_f(a,545)&&(d.traits&EA_DRAGON))at+=800;
 if(ea_f(a,230)&&(d.t==4||d.t==5))at+=700;
 if(ea_f(a,86)){int v=at;at=ad;ad=v;}
 if(ea_f(a,617)&&d.at>a.at&&!imm)dt-=600;
 if(ea_f(d,617)&&a.at>d.at&&!ea_on(a,380))at-=600;
 if(swap){int v=at;at=dt;dt=v;}
 if(d.flags&4096)dt+=1000*ea_sources(e,j,706);
 int diff=ea_cap(at)+star-ea_cap(def?df:dt),tank=ea_f(a,370)&&def&&diff<=0;
 f.delta=diff;if(tank){f.kill=1;f.hurt=ea_min(500,b->lp[side]);}
 else if(diff>0){f.kill=1;f.hit=(!def||ea_f(a,92))?diff:0;}
 else if(diff<0){f.loss=!def;f.hurt=-diff;}else if(!def)f.kill=f.loss=1;
 if(ea_on(d,719)&&a.level>=6&&f.kill){f.kill=f.loss=f.hit=f.hurt=0;f.uncertain=1;}
 if(f.kill&&ea_guard(b,side^1,j,a))f.kill=0;if(f.loss&&ea_guard(b,side,i,d))f.loss=0;
 int hit=ea_min(f.hit,b->lp[side^1]),hurt=ea_min(f.hurt,b->lp[side]);
 if(ea_sources(e,-1,365))f.hurt+=hit;if(ea_sources(r,-1,365))f.hit+=hurt;
 if(d.t==19&&ea_sources(e,-1,594)==1)f.hurt+=hit/2;
 if(!tank&&a.t==19&&ea_sources(r,-1,594)==1)f.hit+=hurt/2;
 if(f.kill){f.after-=ea_death(e,j,&f.hurt);
 if(ea_f(a,613))for(int k=0;k<5;k++)if((k==j-1||k==j+1)&&e[k].id)f.after+=200;
 if(ea_f(a,707)&&ea_free(r))f.after+=750;
 if(ea_f(a,471))f.after+=ea_min(b->maxlp[side]-b->lp[side],d.printed/2);
 if(ea_f(a,442)&&ea_free(e)<4)f.after+=160;
 if(ea_f(a,37))f.hit+=500;if(ea_f(d,564)&&!f.loss)f.after-=500;
 if(ea_on(a,702))f.after+=500;if(ea_on(a,716)&&a.charge&&ea_free(e)<4)f.after+=450;
 }else if(ea_f(a,467))f.after+=200;
 if(f.loss)f.after+=ea_death(r,i,&f.hit);
 f.score=3*(ea_min(f.hit,b->lp[side^1])-ea_min(f.hurt,b->lp[side]))+f.after+
 f.kill*(400+ea_max(d.at,d.df))-f.loss*(400+ea_max(a.at,a.df));
 if(f.hurt>=b->lp[side])f.score-=1000000;else if(f.hit>=b->lp[side^1])f.score+=1000000;
 if(f.kill&&ea_on(d,701)){f.score-=((ea_sources(e,-1,495)?3:2)*(400+ea_max(d.at,d.df)+3*hit+(b->backfree[side^1]?160:0)))/4;f.kill=0;f.uncertain=1;}
 if(f.loss&&ea_on(a,701)){f.score+=((ea_sources(r,-1,495)?3:2)*(400+ea_max(a.at,a.df)+3*hurt+(b->backfree[side]?160:0)))/4;f.loss=0;f.uncertain=1;}
 return f;}
static EaResult ea_fight(const EaBoard*b,int s,int i,int j,int star){
 EaResult f=ea_case(b,s,i,j,star,0);EaCard a=b->row[s][i],d=b->row[s^1][f.target];
 if(f.legal&&f.relevant&&((ea_f(a,43)&&a.at<d.at)||(ea_on(a,100)&&d.level>=4&&!ea_on(d,380))||(ea_on(d,100)&&a.level>=4&&!ea_on(a,380)))){
 EaResult g=ea_case(b,s,i,j,star,1);int n=(ea_on(a,100)&&ea_sources(b->row[s],-1,495))||(ea_on(d,100)&&ea_sources(b->row[s^1],-1,495))?3:2;
 f.score=((4-n)*f.score+n*g.score)/4;f.kill&=g.kill;f.loss&=g.loss;f.hit=ea_min(f.hit,g.hit);f.uncertain=1;}return f;}
static int ea_keep(const EaBoard*b,int s,int p){
 const EaCard*r=b->row[s],*e=b->row[s^1];EaCard c=r[p];int v=0;
 if(ea_on(c,358))for(int k=0;k<5;k++)if(k!=p&&r[k].id>0&&r[k].t==0&&r[k].level<=6&&r[k].id!=358)v+=300;
 if(ea_on(c,168)&&!s)for(int k=0;k<5;k++)if(b->hand[k].id>=343&&b->hand[k].id<=347)v+=180;
 if(ea_f(c,168)||ea_on(c,357))v+=140*b->backfree[s];if(ea_f(c,272)&&c.charge)v+=400;
 if(ea_f(c,533)&&c.charge)v+=350;if(ea_f(c,594))v+=150*ea_n(r,-1,19,0,0,0);
 if(ea_on(c,374)||ea_on(c,716))v+=220*ea_min(c.charge,5-ea_free(e));
 if(c.active&EA_METAL)v+=100*(5-b->backfree[s^1]);if(ea_on(c,115))v+=150;
 if(ea_on(c,720))v+=250;if(ea_on(c,715)&&c.redirect)v+=400;if(ea_on(c,551))v+=150*ea_sources(r,p,2);
 return c.borrowed?v/3:v;}
#endif
