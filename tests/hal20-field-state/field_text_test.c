/* Compile actual owned Field source; discard unused renderer/engine sections. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../mods/field-effects-secondary-types/field_effects.c"
static int types[723];
s32 Cards_Type(s32 id){return id>0&&id<=722?types[id]:20;}
int Tables_TerrainBonus(int t,int type,int*out){(void)t;(void)type;(void)out;return 0;}
u8 gDuel_bTerrain;
s8 gDuel_aTerrainBoost[20][6];
DisplayObject*D_8009B240,*D_8009B24C;
DuelEffectChannel*D_8009B250;
static unsigned checks,delegations;
#define C(x) do{assert(x);++checks;}while(0)
static char ascii(u16 c){if(c>=0x824F&&c<=0x8258)return '0'+c-0x824F;if(c>=0x8260&&c<=0x8279)return 'A'+c-0x8260;if(c>=0x8281&&c<=0x829A)return 'a'+c-0x8281;switch(c){case 0x8140:return ' ';case 0x815E:return '/';case 0x817B:return '+';case 0x817C:return '-';case 0x8143:return ',';case 0x8144:return '.';default:return '?';}}
static void command_previous(DuelEffectChannel*c){++delegations;c->entry_end_20++;}
int main(void){
 types[22]=CARD_TYPE_FIEND;types[438]=CARD_TYPE_MACHINE;types[30]=CARD_TYPE_ZOMBIE;types[272]=CARD_TYPE_PYRO;
 C(secondary_bonus(CARD_TYPE_FIEND,secondary_mask(22),1)==0);
 for(int f=2;f<=6;f++)C(secondary_bonus(CARD_TYPE_FIEND,secondary_mask(22),f)==(f==4?0:200));
 C(secondary_bonus(CARD_TYPE_BEAST_WARRIOR,ST_BEAST|ST_WARRIOR,1)==0);
 C(secondary_bonus(CARD_TYPE_FISH,ST_FISH|ST_AQUA|ST_SEA_SERPENT,5)==0);
 C(secondary_bonus(CARD_TYPE_DRAGON,ST_PYRO|ST_THUNDER,5)==0);
 C(secondary_bonus(CARD_TYPE_ZOMBIE,secondary_mask(30),4)==200);
 C(secondary_bonus(CARD_TYPE_PYRO,secondary_mask(272),5)==0);
 gDuel_aTerrainBoost[CARD_TYPE_MACHINE][4]=-50;
 C(terrain_card_delta(438,CARD_TYPE_MACHINE,5)==700);
 build_footer(22,secondary_mask(22));char text[256];int n=0,icons=0,green=0;
 for(int i=0;i<footer_n;i++){DuelEffectEntry*e=&footer_entries[i];if(e->flags_11&0x20)icons++;else text[n++]=ascii(e->code_00);green+=e->field_16==3;}
 text[n]=0;C(icons==2);C(green>=4);C(strstr(text,"Umi/Mountain/Yami/Wasteland")!=0);C(!strstr(text,"PS:"));
 build_footer(30,secondary_mask(30));C(footer_n>0);
 types[30]=CARD_TYPE_WARRIOR;build_footer(30,secondary_mask(30));C(footer_n==0);types[30]=CARD_TYPE_ZOMBIE;
 build_footer(22,secondary_mask(22));DuelEffectEntry entries[32]={0};DuelEffectChannel box={0};box.entry_head_24=entries;box.entry_end_20=entries+16;
 for(int i=0;i<16;i++){entries[i].flags_11=DUEL_EFFECT_ENTRY_FLAG_ACTIVE;entries[i].y_0E=60+12*i;entries[i].code_00=0x8260;}
 DuelEffectEntry saved[32];memcpy(saved,entries,sizeof saved);int pitch=description_pitch_for_footer(&box,entries,footer_top_y());C(pitch>=DESC_MIN_PITCH&&pitch<DESC_NATIVE_H);C(!memcmp(saved,entries,sizeof saved));
 box.entry_end_20=entries+2;C(description_pitch_for_footer(&box,entries,footer_top_y())==DESC_NATIVE_H);
 DisplayObject textobj={0},bg={0},card={0};D_8009B250=&box;D_8009B240=&bg;D_8009B24C=&card;box.field_28=&textobj;box.flags_34=DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
 C(footer_viewer_is_live(&textobj,&box));bg.attribute=0x80000000u;C(!footer_viewer_is_live(&textobj,&box));bg.attribute=0;bg.field_30.h.field_32=1;C(!footer_viewer_is_live(&textobj,&box));bg.field_30.h.field_32=0;
 unsigned char storage[4096]={0},command=0x40;DuelEffectChannel*c=(void*)storage;TextStreamOwner*owner=(void*)storage;c->index_57=0;c->stream_58=0;c->entry_end_20=entries+2;owner->streams[0]=&command;original_text_command=command_previous;
 text_command_hook(c);C(delegations==1&&description_start[0]==entries+3);command=0x20;text_command_hook(c);C(delegations==2&&!description_start[0]);
 mod_reset();C(!description_start[0]);printf("PASS Field actual-source rules/footer/cursor/pitch/visibility/delegation: %u checks\n",checks);return 0;
}
