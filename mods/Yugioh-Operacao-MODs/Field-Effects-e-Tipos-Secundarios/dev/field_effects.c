#include "types.h"
#include "pc/mods/modapi.h"
#include "game/card_constants.h"
#include "game/duel_card.h"
#include "game/duel_deck_card.h"
#include "game/duel_terrain_boost.h"
#include "game/func_80024E58.h"
#include "game/duel_effect.h"
#define DUEL_CARD_VIEWER_ADDRESS_ALIASES
#include "game/duel_card_viewer.h"
#include "game/display_object.h"
#include "game/display_object_core.h"
#include "game/display_object_helpers.h"
#include "game/func_80035E20.h"
#include "psyq/libgpu.h"
#include "psyq/libgs.h"
#include "pc/text/glyphs.h"
#include "pc/text/hd_text.h"
#include "pc/cards/cards.h"
#include "pc/cards/tables.h"

enum { ST_DRAGON=1u<<0,ST_WINGED=1u<<1,ST_THUNDER=1u<<2,ST_SPELLCASTER=1u<<3,ST_FIEND=1u<<4,ST_ZOMBIE=1u<<5,ST_WARRIOR=1u<<6,ST_BEAST=1u<<7,ST_PLANT=1u<<8,ST_INSECT=1u<<9,ST_ROCK=1u<<10,ST_DINOSAUR=1u<<11,ST_FISH=1u<<12,ST_AQUA=1u<<13,ST_MACHINE=1u<<14,ST_PYRO=1u<<15,ST_SEA_SERPENT=1u<<16 };
typedef struct{u16 id;u32 mask;} SecondaryCard;
static const SecondaryCard secondary_cards[]={
{3,ST_FIEND},{5,ST_BEAST},{6,ST_BEAST},{9,ST_BEAST},{11,ST_DRAGON|ST_WARRIOR},{15,ST_PYRO},{22,ST_THUNDER|ST_ZOMBIE},{23,ST_FIEND},{28,ST_WARRIOR},{30,ST_WARRIOR},{31,ST_FIEND},{33,ST_FIEND},{39,ST_ZOMBIE},{40,ST_ROCK},{48,ST_BEAST},{58,ST_BEAST},{59,ST_BEAST},{68,ST_DRAGON},{70,ST_FIEND},{73,ST_DRAGON|ST_AQUA},{76,ST_AQUA},{81,ST_DRAGON},{83,ST_ROCK},{87,ST_BEAST},{91,ST_WARRIOR},{96,ST_WARRIOR},{97,ST_DRAGON},{98,ST_FIEND},{99,ST_PLANT},{108,ST_FIEND},{117,ST_SPELLCASTER},{120,ST_FIEND},{133,ST_PYRO|ST_WARRIOR},{138,ST_DRAGON},{141,ST_FIEND|ST_AQUA},{142,ST_PYRO},{146,ST_ROCK},{149,ST_SPELLCASTER},{153,ST_FIEND},{154,ST_PYRO},{156,ST_ZOMBIE},{157,ST_PYRO},{160,ST_MACHINE},{161,ST_MACHINE},{165,ST_FIEND},{168,ST_PYRO},{172,ST_PYRO},{175,ST_SPELLCASTER},{176,ST_SPELLCASTER},{186,ST_SPELLCASTER},{187,ST_FIEND},{188,ST_WARRIOR},{193,ST_BEAST},{202,ST_WINGED},{206,ST_FIEND},{214,ST_PYRO},{215,ST_PYRO},{218,ST_THUNDER},{224,ST_MACHINE},{225,ST_FIEND},{228,ST_PLANT},{231,ST_FIEND},{232,ST_WINGED},{239,ST_FIEND},{242,ST_PYRO},{244,ST_PYRO},{245,ST_WINGED},{249,ST_SPELLCASTER},{254,ST_BEAST},{257,ST_BEAST},{267,ST_FIEND},{272,ST_PYRO},{277,ST_BEAST},{282,ST_SPELLCASTER},{294,ST_DRAGON},{295,ST_PLANT},{296,ST_DRAGON},{298,ST_FIEND},{353,ST_DRAGON},{357,ST_PYRO},{381,ST_AQUA},{384,ST_FIEND},{386,ST_WINGED},{390,ST_MACHINE},{404,ST_FISH},{407,ST_MACHINE},{409,ST_DRAGON|ST_PYRO},{412,ST_BEAST},{413,ST_WARRIOR},{417,ST_MACHINE},{421,ST_WARRIOR},{423,ST_BEAST},{425,ST_DRAGON},{426,ST_DRAGON},{431,ST_FISH},{438,ST_FISH},{441,ST_FISH},{442,ST_DRAGON},{443,ST_DRAGON},{448,ST_DRAGON|ST_AQUA},{450,ST_FIEND},{454,ST_WARRIOR},{455,ST_ROCK},{456,ST_WARRIOR},{457,ST_ZOMBIE},{458,ST_SPELLCASTER},{459,ST_BEAST},{460,ST_AQUA},{462,ST_SPELLCASTER},{467,ST_PYRO},{470,ST_SPELLCASTER},{473,ST_PYRO|ST_WARRIOR},{479,ST_WARRIOR},{483,ST_WINGED},{487,ST_PLANT},{490,ST_INSECT},{502,ST_DRAGON},{504,ST_PLANT|ST_PYRO},{505,ST_SPELLCASTER},{508,ST_DINOSAUR},{511,ST_WARRIOR},{518,ST_ROCK},{520,ST_WINGED},{529,ST_BEAST},{531,ST_SPELLCASTER},{539,ST_FISH},{545,ST_DRAGON},{546,ST_WARRIOR},{548,ST_BEAST},{555,ST_DRAGON|ST_PYRO},{560,ST_INSECT},{564,ST_BEAST},{566,ST_AQUA},{567,ST_FIEND},{571,ST_PLANT},{575,ST_SPELLCASTER},{582,ST_SPELLCASTER},{587,ST_INSECT},{594,ST_FIEND},{595,ST_SPELLCASTER},{596,ST_FIEND},{599,ST_THUNDER},{604,ST_FIEND},{613,ST_DRAGON},{625,ST_SPELLCASTER},{632,ST_WARRIOR},{641,ST_FIEND},{642,ST_SPELLCASTER},{643,ST_SPELLCASTER},{650,ST_WINGED},{705,ST_DRAGON}};
static u8*(*original_setup)(s32,s32); static void(*original_apply_terrain)(void); extern u8 gDuel_bTerrain;
static const MemoriesModHost *mod_host;

static u32 secondary_mask(s32 id){s32 lo=0,hi=(s32)(sizeof(secondary_cards)/sizeof(secondary_cards[0]))-1;while(lo<=hi){s32 mid=(lo+hi)>>1,found=secondary_cards[mid].id;if(id==found)return secondary_cards[mid].mask;if(id<found)hi=mid-1;else lo=mid+1;}return 0;}
static s32 card_type(s32 id){s32 type=Cards_Type(id);return type>=0?type:CARD_TYPE_MAGIC;}
static int aquatic_type(s32 t){return t==CARD_TYPE_FISH||t==CARD_TYPE_AQUA||t==CARD_TYPE_SEA_SERPENT;}
static u32 type_bit(s32 type){switch(type){case CARD_TYPE_DRAGON:return ST_DRAGON;case CARD_TYPE_WINGED_BEAST:return ST_WINGED;case CARD_TYPE_THUNDER:return ST_THUNDER;case CARD_TYPE_SPELLCASTER:return ST_SPELLCASTER;case CARD_TYPE_FIEND:return ST_FIEND;case CARD_TYPE_ZOMBIE:return ST_ZOMBIE;case CARD_TYPE_WARRIOR:return ST_WARRIOR;case CARD_TYPE_BEAST:return ST_BEAST;case CARD_TYPE_PLANT:return ST_PLANT;case CARD_TYPE_INSECT:return ST_INSECT;case CARD_TYPE_ROCK:return ST_ROCK;case CARD_TYPE_DINOSAUR:return ST_DINOSAUR;case CARD_TYPE_FISH:return ST_FISH;case CARD_TYPE_AQUA:return ST_AQUA;case CARD_TYPE_SEA_SERPENT:return ST_SEA_SERPENT;case CARD_TYPE_MACHINE:return ST_MACHINE;case CARD_TYPE_PYRO:return ST_PYRO;default:return 0;}}
static int subtype_blocked(s32 type,u32 bit){
 if(type<0||type>=CARD_TYPE_MAGIC)return 1;
 if(bit==type_bit(type))return 1;
 if(aquatic_type(type)&&(bit&(ST_FISH|ST_AQUA|ST_SEA_SERPENT)))return 1;
 if(type==CARD_TYPE_BEAST_WARRIOR&&(bit==ST_BEAST||bit==ST_WARRIOR))return 1;
 return 0;
}
static int subtype_relation(u32 bit,s32 terrain){
 switch(terrain){
 case 1:return (bit&(ST_BEAST|ST_PLANT|ST_INSECT))?1:0;
 case 2:if(bit&(ST_ZOMBIE|ST_ROCK|ST_DINOSAUR))return 1;if(bit&(ST_FISH|ST_AQUA|ST_SEA_SERPENT))return -1;return 0;
 case 3:return (bit&(ST_DRAGON|ST_WINGED|ST_THUNDER))?1:0;
 case 4:return bit==ST_WARRIOR?1:0;
 case 5:if(bit&(ST_THUNDER|ST_FISH|ST_AQUA|ST_SEA_SERPENT))return 1;if(bit&(ST_MACHINE|ST_PYRO))return -1;return 0;
 case 6:return (bit&(ST_SPELLCASTER|ST_FIEND|ST_ZOMBIE))?1:0;
 default:return 0;
 }
}
static s32 secondary_bonus(s32 type,u32 mask,s32 terrain){
 u32 bit;int pos=0,neg=0,rel;
 if(terrain<1||terrain>6||!mask)return 0;
 for(bit=1;bit<=ST_SEA_SERPENT;bit<<=1){
  if(!(mask&bit)||subtype_blocked(type,bit))continue;
  rel=subtype_relation(bit,terrain);if(rel>0)pos=1;else if(rel<0)neg=1;
 }
 if(pos&&neg)return 0;if(pos)return 200;if(neg)return -200;return 0;
}
static s32 primary_terrain_bonus(s32 type,s32 terrain){
 s32 bonus=0;if(terrain<1||terrain>6||type<0||type>=CARD_TYPE_MAGIC)return 0;
 if(Tables_TerrainBonus(terrain,type,&bonus))return bonus;
 return (s32)gDuel_aTerrainBoost[type][terrain-1]*CARD_STAT_SCALE;
}
static s32 terrain_card_delta(s32 id,s32 type,s32 terrain){
 s32 delta=secondary_bonus(type,secondary_mask(id),terrain);
 if(terrain==5&&(id==438||id==441)&&type==CARD_TYPE_MACHINE)delta-=primary_terrain_bonus(type,terrain);
 return delta;
}
static s32 total_terrain_modifier(s32 id){s32 type=card_type(id),terrain=(s32)gDuel_bTerrain;return primary_terrain_bonus(type,terrain)+terrain_card_delta(id,type,terrain);}
static s32 terrain_card_delta_v1(s32 id,s32 type,s32 terrain){if(!mod_host||!mod_host->applied(mod_host))return 0;return terrain_card_delta(id,type,terrain);}
static void adjust_record(DuelCardRecord*c){if(c&&(c->flags&DUEL_CARD_FLAG_OCCUPIED)&&c->card_id>0)c->terrain_modifier=(s16)total_terrain_modifier(c->card_id);}
static u8*setup_hook(s32 a,s32 b){u8*r=original_setup(a,b);adjust_record((DuelCardRecord*)r);return r;}
static void apply_terrain_hook(void){s32 i;original_apply_terrain();for(i=0;i<DUEL_CARD_RECORD_COUNT;i++)adjust_record(&D_801A7AD8[i]);}
#define FOOTER_ENTRY_CAP 128
static DuelEffectEntry footer_entries[FOOTER_ENTRY_CAP];
static void (*original_text_draw)(DisplayObject*, GsOT*);
static int footer_n;
static s16 footer_x, footer_y;
#define FOOTER_ADV 5
#define FOOTER_ICON_ADV 10
#define FOOTER_LINE_H 10
#define FOOTER_W 160

static u16 sjis_ascii(char c){
    if(c==' ')return 0x8140;
    if(c>='0'&&c<='9')return (u16)(0x824F+(c-'0'));
    if(c>='A'&&c<='Z')return (u16)(0x8260+(c-'A'));
    if(c>='a'&&c<='z')return (u16)(0x8281+(c-'a'));
    switch(c){case ',':return 0x8143;case '.':return 0x8144;case ':':return 0x8146;case ';':return 0x8147;case '/':return 0x815E;case '+':return 0x817B;case '-':return 0x817C;default:return 0;}
}
static void footer_clear_entries(void){int i;footer_n=0;footer_x=4;footer_y=0;for(i=0;i<FOOTER_ENTRY_CAP;i++)footer_entries[i].flags_11=0;}
static void footer_newline(void){footer_x=4;footer_y=(s16)(footer_y+FOOTER_LINE_H);}
static int footer_text_width(const char*s){int w=0;while(*s++)w+=FOOTER_ADV;return w;}
static void footer_ensure(int width){if(footer_x>4&&footer_x+width>FOOTER_W)footer_newline();}
static void footer_char_raw(char c,u8 color){DuelEffectEntry*e;u16 code;if(footer_n>=FOOTER_ENTRY_CAP-1)return;code=sjis_ascii(c);if(!code)return;e=&footer_entries[footer_n++];e->code_00=code;e->pad_02=0;e->field_04=0;e->field_08=0;e->x_0C=footer_x;e->y_0E=footer_y;e->field_10=0;e->flags_11=0x80;e->field_12=1;e->field_13=1;e->pad_14=0;e->field_15=0;e->field_16=color;e->field_17=0;e->field_18=0;footer_x=(s16)(footer_x+FOOTER_ADV);footer_entries[footer_n].flags_11=0;}
static void footer_word(const char*s,u8 color){int w=footer_text_width(s);footer_ensure(w+(footer_x>4?FOOTER_ADV:0));if(footer_x>4)footer_char_raw(' ',color);while(*s)footer_char_raw(*s++,color);}
static void footer_icon(u8 type){DuelEffectEntry*e;if(footer_n>=FOOTER_ENTRY_CAP-1)return;footer_ensure(FOOTER_ICON_ADV);e=&footer_entries[footer_n++];e->code_00=0;e->pad_02=0;e->field_04=0;e->field_08=0;e->x_0C=footer_x;e->y_0E=footer_y;e->field_10=type;e->flags_11=0xA0;e->field_12=1;e->field_13=1;e->pad_14=0;e->field_15=0;e->field_16=0;e->field_17=0;e->field_18=0;footer_x=(s16)(footer_x+FOOTER_ICON_ADV);footer_entries[footer_n].flags_11=0;}
static void footer_punct(const char*s,u8 color){while(*s)footer_char_raw(*s++,color);}
static void plus200(void){footer_word("+200",3);} static void minus200(void){footer_word("-200",6);} static void normal_word(const char*s){footer_word(s,0);}
static void atk_on(void){normal_word("ATK");normal_word("on");}
static int bit_icon(u32 bit){switch(bit){case ST_DRAGON:return CARD_TYPE_DRAGON;case ST_WINGED:return CARD_TYPE_WINGED_BEAST;case ST_THUNDER:return CARD_TYPE_THUNDER;case ST_SPELLCASTER:return CARD_TYPE_SPELLCASTER;case ST_FIEND:return CARD_TYPE_FIEND;case ST_ZOMBIE:return CARD_TYPE_ZOMBIE;case ST_WARRIOR:return CARD_TYPE_WARRIOR;case ST_BEAST:return CARD_TYPE_BEAST;case ST_PLANT:return CARD_TYPE_PLANT;case ST_INSECT:return CARD_TYPE_INSECT;case ST_ROCK:return CARD_TYPE_ROCK;case ST_DINOSAUR:return CARD_TYPE_DINOSAUR;case ST_FISH:return CARD_TYPE_FISH;case ST_AQUA:return CARD_TYPE_AQUA;case ST_SEA_SERPENT:return CARD_TYPE_SEA_SERPENT;case ST_MACHINE:return CARD_TYPE_MACHINE;case ST_PYRO:return CARD_TYPE_PYRO;default:return -1;}}
static const char*field_name(s32 terrain){switch(terrain){case 1:return "Forest";case 2:return "Wasteland";case 3:return "Mountain";case 4:return "Sogen";case 5:return "Umi";case 6:return "Yami";default:return "";}}
static int subtype_visible(s32 type,u32 mask,u32 bit){s32 t,rel,final;if(!(mask&bit)||subtype_blocked(type,bit))return 0;for(t=1;t<=6;t++){rel=subtype_relation(bit,t);if(!rel)continue;final=secondary_bonus(type,mask,t);if((rel>0&&final>0)||(rel<0&&final<0))return 1;}return 0;}
static void footer_icons(s32 type,u32 mask){u32 bit;int first=1,icon;normal_word("Viewed");normal_word("as");for(bit=1;bit<=ST_SEA_SERPENT;bit<<=1){if(!subtype_visible(type,mask,bit))continue;icon=bit_icon(bit);if(icon<0)continue;if(first){footer_ensure(FOOTER_ICON_ADV+(footer_x>4?FOOTER_ADV:0));if(footer_x>4)footer_char_raw(' ',0);first=0;}else footer_punct("/",0);footer_icon((u8)icon);}footer_punct(",",0);}
static void footer_fields(s32 type,u32 mask,int sign){
 static const u8 order[6]={5,3,6,2,4,1};char grouped[64];int i,n=0;const char*name;
 for(i=0;i<6;i++){s32 t=order[i],delta=secondary_bonus(type,mask,t);const char*p;if((sign>0&&delta<=0)||(sign<0&&delta>=0))continue;if(n)grouped[n++]='/';name=field_name(t);p=name;while(*p&&n<(int)sizeof(grouped)-1)grouped[n++]=*p++;}
 grouped[n]=0;if(n)normal_word(grouped);
}
static void build_footer(u16 id,u32 m){
 s32 type=card_type(id),t;int have_pos=0,have_neg=0,have_icon=0;u32 bit;
 footer_clear_entries();
 for(t=1;t<=6;t++){s32 d=secondary_bonus(type,m,t);if(d>0)have_pos=1;else if(d<0)have_neg=1;}
 if(!have_pos&&!have_neg)return;
 for(bit=1;bit<=ST_SEA_SERPENT;bit<<=1)if(subtype_visible(type,m,bit)){have_icon=1;break;}
 if(!have_icon)return;
 footer_icons(type,m);
 if(have_pos){plus200();atk_on();footer_fields(type,m,1);if(!have_neg)footer_punct(".",0);}
 if(have_neg){if(have_pos)footer_punct(",",0);minus200();atk_on();footer_fields(type,m,-1);footer_punct(".",0);}
}
static void footer_quad(GsOT*ot,s32 pri,int x,int y,int dw,int dh,int u,int v,int sw,int sh,int tpage,int clut){
 POLY_FT4 q;SetPolyFT4(&q);setRGB0(&q,0x80,0x80,0x80);setSemiTrans(&q,1);
 q.x0=q.x2=(s16)x;q.x1=q.x3=(s16)(x+dw);q.y0=q.y1=(s16)y;q.y2=q.y3=(s16)(y+dh);
 q.u0=q.u2=(u8)u;q.u1=q.u3=(u8)(u+sw);q.v0=q.v1=(u8)v;q.v2=q.v3=(u8)(v+sh);
 q.tpage=(u16)tpage;q.clut=(u16)clut;GsSortPoly(&q,ot,(u16)pri);
}
static void draw_footer_scaled(DisplayObject*obj,GsOT*ot){
 int i,lines=1,base_y,x0=(s16)obj->field_30.h.field_30,y0=(s16)obj->field_30.h.field_32,pri=(s16)obj->field_14;
 for(i=0;i<footer_n;i++){int l=footer_entries[i].y_0E/FOOTER_LINE_H+1;if(l>lines)lines=l;}
 base_y=174-lines*FOOTER_LINE_H;
 for(i=0;i<footer_n;i++){
  DuelEffectEntry*e=&footer_entries[i];int x=x0+e->x_0C,y=y0+base_y+e->y_0E;
  if(e->flags_11&0x20){
   int type=e->field_10,u=((type&7)*16)-128,v=(type&0x38)*2,cx=((type&15)*16)+0x200,cy=(type>>4)+0xF9;
   footer_quad(ot,pri,x,y,10,10,u,v,15,15,0xB,(cy<<6)|((cx>>4)&0x3F));
  }else{
   int u=0,v=0,tpage=obj->field_66;
   if(!Glyphs_RetailCell((u32)(e->code_00>=0x824F&&e->code_00<=0x8258?'0'+(e->code_00-0x824F):e->code_00>=0x8260&&e->code_00<=0x8279?'A'+(e->code_00-0x8260):e->code_00>=0x8281&&e->code_00<=0x829A?'a'+(e->code_00-0x8281):e->code_00==0x8140?' ':e->code_00==0x8143?',':e->code_00==0x8144?'.':e->code_00==0x8146?':':e->code_00==0x8147?';':e->code_00==0x815E?'/':e->code_00==0x817B?'+':e->code_00==0x817C?'-':'?'),0,&u,&v))continue;
   if(HdText_Enabled())tpage|=HD_TEXT_MARK;
   footer_quad(ot,pri,x,y+1,5,9,u,v,7,11,tpage,((e->field_16+0xE8)<<6)|0x28);
  }
 }
}
static int footer_viewer_is_live(DisplayObject*obj,DuelEffectChannel*box){
 DisplayObject*bg=D_8009B240;DisplayObject*card=D_8009B24C;
 if(!obj||!box||!bg||!card)return 0;
 if(D_8009B250!=box||box->field_28!=obj)return 0;
 if(!(box->flags_34&DUEL_EFFECT_CHANNEL_FLAG_ACTIVE))return 0;
 if((obj->attribute|bg->attribute|card->attribute)&0x80000000u)return 0;
 if(obj->field_30.h.field_30!=bg->field_30.h.field_30)return 0;
 if(obj->field_30.h.field_32!=bg->field_30.h.field_32)return 0;
 return 1;
}
static void text_draw_hook(DisplayObject*obj,GsOT*ot){
 DuelEffectChannel*box=D_8009B250;u16 id=gDuel_wViewerCardID;u32 m=secondary_mask(id);
 original_text_draw(obj,ot);
 if(!m||!mod_host||!mod_host->setting(mod_host,"description_notes",1))return;
 if(!footer_viewer_is_live(obj,box))return;
 build_footer(id,m);if(footer_n)draw_footer_scaled(obj,ot);
}
static void mod_reset(void){}
static void mod_applied(int on){(void)on;}
int MemoriesModInit(const MemoriesModHost*host,MemoriesMod*mod){if(host->api<4)return 0;mod_host=host;mod->api=4;mod->name="Field Effects + Secondary Types";mod->reset=mod_reset;mod->applied=mod_applied;if(!host->provide(host,"terrain-card-delta-v1",(void*)terrain_card_delta_v1))return 0;if(!host->hook(host,(void*)Duel_SetupCardRecord,(void*)setup_hook,(void**)&original_setup))return 0;if(!host->hook(host,(void*)DuelEffect_ApplyTerrain,(void*)apply_terrain_hook,(void**)&original_apply_terrain))return 0;if(!host->hook(host,(void*)func_80035E20,(void*)text_draw_hook,(void**)&original_text_draw))return 0;return 1;}
