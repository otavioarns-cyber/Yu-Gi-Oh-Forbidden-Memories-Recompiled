/* Operate on the CPU VM's proposed bytes, before existing final command
 * policies (mandatory burn, Cosmo lock and learned redirections) run. */
extern unsigned char D_800EAE88[];
static void (*ea_old_play)(void),(*ea_old_star)(void);
static void ea_play(void){
 ea_old_play();if(!ea_native())return;int a=D_800EAE88[9]-1,t=D_800EAE88[10]-56;
 if(a<0||a>=5)return;EaBoard b=ea_capture(0);EaCard actor=b.row[0][a];if(actor.id<=0)return;
 if(t<0||t>=5){if(D_800EAE88[10]==0){if(ea_on(actor,362)||ea_on(actor,365))D_800EAE88[11]=1;
 if(ea_f(actor,495)||ea_on(actor,722))D_800EAE88[11]=0;}return;}
 if(b.blocked[0]||b.row[1][t].id<=0)return;
 int best=0,att=-1,dst=-1,affected=0;for(int i=0;i<5;i++)if(b.row[0][i].id>0&&!(b.row[0][i].flags&16384))
 for(int j=0;j<5;j++)if(b.row[1][j].id>0){EaResult f=ea_fight(&b,0,i,j,Duel_CalcGuardianStarMatchup(b.row[0][i].star,b.row[1][j].star));
 affected|=f.relevant;if(f.legal&&f.score>best){best=f.score;att=i;dst=j;}}
 if(!affected)return;if(att>=0){D_800EAE88[9]=1+att;D_800EAE88[10]=56+dst;}else{
 D_800EAE88[10]=0;D_800EAE88[11]=!(ea_f(actor,495)||ea_on(actor,722));}
 if(EA_HOST->setting(EA_HOST,"effect_ai_trace",0)&&EA_HOST->log)EA_HOST->log(EA_HOST,"[Effect AI] previous attack=%d:%d selected=%d:%d utility=%d",a+1,t+56,D_800EAE88[9],D_800EAE88[10],best);}
static void ea_star(void){
 ea_old_star();if(!ea_native())return;
 int pos=D_800EAE88[6]-1;if(pos<0||pos>=5||!D_800EAE88[0])return;EaBoard b=ea_capture(0);
 EaCard c={0};if(!D_800EAE88[8])c=b.row[0][pos];
 for(int k=0;k<5&&D_800EAE88[k];k++){
 int h=D_800EAE88[k]-11;if(h<0||h>=5||!b.hand[h].id)return;
 if(!c.id)c=b.hand[h];else{int r=0,o=0;if(!ea_fapi||!ea_fapi->pair(c.id,b.hand[h].id,c.active,&r,&o)||!r)return;
 if(r!=c.id)c=ea_card(r,-1,D_8009B1D5,0);c.active|=c.possible&o;}b.hand[h]=(EaCard){0};}
 if(c.id<=0||c.t>=20)return;unsigned st=gDuel_adwCardStats[c.id-1];
 int stars[2]={(st>>CARD_STAT_GUARDIAN_STAR_1_SHIFT)&15,(st>>CARD_STAT_GUARDIAN_STAR_2_SHIFT)&15};
 if(!stars[0]||!stars[1]||stars[0]==stars[1])return;b.row[0][pos]=c;int v[2]={0,0},seen=0;
 for(int k=0;k<2;k++){int gain=0,risk=0;for(int j=0;j<5;j++)if(b.row[1][j].id>0){
 EaResult f=ea_fight(&b,0,pos,j,Duel_CalcGuardianStarMatchup(stars[k],b.row[1][j].star));
 EaResult g=ea_fight(&b,1,j,pos,Duel_CalcGuardianStarMatchup(b.row[1][j].star,stars[k]));
 seen|=f.relevant;if(f.legal&&!b.blocked[0])gain=ea_max(gain,f.score);if(g.legal)risk=ea_max(risk,g.score);}
 v[k]=gain-risk/3;}if(seen&&v[0]!=v[1])D_800EAE88[7]=v[1]>v[0];}
