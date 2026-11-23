/* Local read-only adapters for ABI1; no effect IDs or private producer data. */
#include "mandatory_hand_selection_v1.h"
#include "game/duel_scene_state.h"
static RhsPhysical rhs_physical(int slot,int record,int index,void*object)
{
 RhsPhysical p;memset(&p,0,sizeof p);p.slot=slot;p.record=record;p.index=index;
 if(record>=0&&record<30){DuelCardRecord*c=&D_801A7AD8[record];p.id=c->card_id;
 p.flags=(unsigned)c->flags;p.data=(uint32_t)(uintptr_t)c->data;
 p.object=(uint32_t)(uintptr_t)object;}return p;
}
static int rhs_capture(RhsSnapshot*s,int side,unsigned action,unsigned generation)
{
 int i;memset(s,0,sizeof*s);if(side<0||side>1)return 0;
 s->size=sizeof*s;s->side=side;s->action=action;s->generation=generation;
 s->phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
 memcpy(s->selection,(const void*)D_800EAE88,12);
 memcpy(s->deck,gDuel_aDeckCardRecords,sizeof s->deck);
 memcpy(s->stats,gDuel_adwCardStats,sizeof s->stats);
 for(i=0;i<2;i++){s->turn[i]=D_800E9FF0[i].rank.turns_taken;
 s->draw[i]=D_800E9FF0[i].deck_draw_cursor;}
 for(i=0;i<5;i++)s->hand[i]=rhs_physical(11+i,D_800907CC[side*5+i],
 D_800E9FF0[side].hand[i],D_800EA030[i].object);
 for(i=0;i<10;i++){int r=(i<5?5:20)+i%5;DuelCardRecord*c=&D_801A7AD8[r];
 int n=-1;uintptr_t b=(uintptr_t)c->data,a=(uintptr_t)gDuel_aDeckCardRecords;
 if(b>=a&&b<a+sizeof s->deck&&(b-a)%6==0)n=(int)((b-a)/6);
 s->field[i]=rhs_physical(r,r,n,c->object);}
 for(i=0;i<45;i++){s->logical_id[i]=gDuel_aActiveCards[11+i].card_id;
 s->logical_index[i]=gDuel_aActiveCards[11+i].deck_index;}return 1;
}
static int rhs_live(const RhsSnapshot*s)
{
 RhsSnapshot live;return rhs_capture(&live,s->side,s->action,s->generation)&&
 !memcmp(&live,s,sizeof live);
}
static int rhs_selected(const RhsSnapshot*s,const unsigned char*sel,RhsPhysical*out)
{
 int n=0,j;unsigned used=0;
 while(n<5&&sel[n]){int h=(int)sel[n]-11,index,record;const RhsPhysical*p;
 if(h<0||h>=5||(used&(1u<<h)))return -1;used|=1u<<h;p=&s->hand[h];
 index=p->index;record=p->record;
 if(index<s->side*40||index>=s->side*40+s->draw[s->side]||
 index>=(s->side+1)*40||record<s->side*15||record>=s->side*15+5||
 !(p->flags&DUEL_CARD_FLAG_OCCUPIED)||!p->object||
 p->data!=(uint32_t)(uintptr_t)&gDuel_aDeckCardRecords[index]||
 p->id!=gDuel_aDeckCardRecords[index].id||p->id!=s->logical_id[h])return -1;
 if(out)out[n]=*p;++n;}
 if(!n)return -1;
 /* Selection sources are five bytes; byte5 and byte7 are non-hand domains. */
 if(sel[5]||sel[7])return -1;
 j=Cards_Type(s->hand[(int)sel[0]-11].id);
 if(j<CARD_TYPE_MAGIC){if(sel[6]<1||sel[6]>5)return -1;}
 else if(sel[6]<6||sel[6]>10)return -1;
 return n;
}
