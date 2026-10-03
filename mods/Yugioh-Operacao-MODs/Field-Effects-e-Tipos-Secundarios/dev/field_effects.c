#include "types.h"
#include "pc/mods/modapi.h"
#include "game/card_constants.h"
#include "game/duel_card.h"
#include "game/duel_deck_card.h"
#include "game/duel_terrain_boost.h"
#include "game/func_80024E58.h"
#include "game/card_list_text_boxes.h"
#include "game/duel_effect.h"
#include "game/text_box_lifecycle.h"
#include "game/text_box_runtime.h"
#include "game/display_object.h"

enum {
 ST_DRAGON=1u<<0,ST_WINGED=1u<<1,ST_THUNDER=1u<<2,ST_SPELLCASTER=1u<<3,ST_FIEND=1u<<4,ST_ZOMBIE=1u<<5,ST_WARRIOR=1u<<6,ST_BEAST=1u<<7,ST_PLANT=1u<<8,ST_INSECT=1u<<9,ST_ROCK=1u<<10,ST_DINOSAUR=1u<<11,ST_FISH=1u<<12,ST_AQUA=1u<<13,ST_MACHINE=1u<<14,ST_PYRO=1u<<15
};
typedef struct{u16 id;u16 mask;}SecondaryCard;
static const SecondaryCard secondary_cards[]={
{3,ST_FIEND},{5,ST_BEAST},{6,ST_BEAST},{9,ST_BEAST},{11,ST_DRAGON},{15,ST_PYRO},{22,ST_THUNDER|ST_ZOMBIE},{23,ST_FIEND},{28,ST_WARRIOR},{30,ST_WARRIOR},{31,ST_FIEND},{33,ST_FIEND},{39,ST_ZOMBIE},{40,ST_ROCK},{48,ST_BEAST},{58,ST_BEAST},{59,ST_BEAST},{68,ST_DRAGON},{70,ST_FIEND},{73,ST_DRAGON},{76,ST_AQUA},{81,ST_DRAGON},{83,ST_ROCK},{87,ST_BEAST},{91,ST_WARRIOR},{96,ST_WARRIOR},{97,ST_DRAGON},{98,ST_FIEND},{99,ST_PLANT},{108,ST_FIEND},{117,ST_SPELLCASTER},{120,ST_FIEND},{133,ST_PYRO},{138,ST_DRAGON},{141,ST_FIEND|ST_AQUA},{142,ST_PYRO},{146,ST_ROCK},{153,ST_FIEND},{154,ST_PYRO},{156,ST_ZOMBIE},{157,ST_PYRO},{160,ST_MACHINE},{161,ST_MACHINE},{165,ST_FIEND},{168,ST_PYRO},{172,ST_PYRO},{175,ST_SPELLCASTER},{176,ST_SPELLCASTER},{186,ST_SPELLCASTER},{187,ST_FIEND},{188,ST_WARRIOR},{193,ST_BEAST},{202,ST_WINGED},{206,ST_FIEND},{214,ST_PYRO},{215,ST_PYRO},{218,ST_THUNDER},{224,ST_MACHINE},{225,ST_FIEND},{228,ST_PLANT},{231,ST_FIEND},{232,ST_WINGED},{239,ST_FIEND},{242,ST_PYRO},{244,ST_PYRO},{245,ST_WINGED},{249,ST_SPELLCASTER},{254,ST_BEAST},{257,ST_BEAST},{267,ST_FIEND},{272,ST_PYRO},{277,ST_BEAST},{282,ST_SPELLCASTER},{294,ST_DRAGON},{295,ST_PLANT},{296,ST_DRAGON},{298,ST_FIEND},{353,ST_DRAGON},{357,ST_PYRO},{381,ST_AQUA},{384,ST_FIEND},{386,ST_WINGED},{390,ST_MACHINE},{404,ST_FISH},{407,ST_MACHINE},{409,ST_DRAGON|ST_PYRO},{412,ST_BEAST},{413,ST_WARRIOR},{417,ST_MACHINE},{421,ST_WARRIOR},{423,ST_BEAST},{425,ST_DRAGON},{426,ST_DRAGON},{438,ST_FISH},{441,ST_FISH},{442,ST_DRAGON},{443,ST_DRAGON},{448,ST_DRAGON},{450,ST_FIEND},{454,ST_WARRIOR},{455,ST_ROCK},{456,ST_WARRIOR},{457,ST_ZOMBIE},{458,ST_SPELLCASTER},{459,ST_BEAST},{460,ST_AQUA},{462,ST_SPELLCASTER},{467,ST_PYRO},{470,ST_SPELLCASTER},{473,ST_PYRO},{479,ST_WARRIOR},{483,ST_WINGED},{487,ST_PLANT},{490,ST_INSECT},{502,ST_DRAGON},{504,ST_PLANT|ST_PYRO},{505,ST_SPELLCASTER},{508,ST_DINOSAUR},{511,ST_WARRIOR},{518,ST_ROCK},{520,ST_WINGED},{529,ST_BEAST},{531,ST_SPELLCASTER},{539,ST_FISH},{545,ST_DRAGON},{546,ST_WARRIOR},{548,ST_BEAST},{555,ST_DRAGON|ST_PYRO},{560,ST_INSECT},{564,ST_BEAST},{566,ST_AQUA},{567,ST_FIEND},{571,ST_PLANT},{575,ST_SPELLCASTER},{582,ST_SPELLCASTER},{587,ST_INSECT},{594,ST_FIEND},{595,ST_SPELLCASTER},{596,ST_FIEND},{599,ST_THUNDER},{604,ST_FIEND},{613,ST_DRAGON},{625,ST_SPELLCASTER},{632,ST_WARRIOR},{641,ST_FIEND},{642,ST_SPELLCASTER},{643,ST_SPELLCASTER},{650,ST_WINGED},{705,ST_DRAGON}};
static u8*(*original_setup)(s32,s32);static void(*original_apply_terrain)(void);static void(*original_card_list_slot)(CardList*,s32);extern u8 gDuel_bTerrain;
static u16 secondary_mask(s32 id){s32 lo=0,hi=(s32)(sizeof(secondary_cards)/sizeof(secondary_cards[0]))-1;while(lo<=hi){s32 mid=(lo+hi)>>1,found=secondary_cards[mid].id;if(id==found)return secondary_cards[mid].mask;if(id<found)hi=mid-1;else lo=mid+1;}return 0;}
static s32 card_type(s32 id){return id>0?((gDuel_adwCardStats[id-1]>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK):CARD_TYPE_MAGIC;}
static int aquatic_type(s32 t){return t==CARD_TYPE_FISH||t==CARD_TYPE_AQUA||t==CARD_TYPE_SEA_SERPENT;}
static s32 secondary_bonus(s32 id,s32 type,u16 mask,s32 terrain){s32 bonus=0;const u16 aquatic=ST_FISH|ST_AQUA;if(!terrain||!mask)return 0;switch(terrain){case 1:if(mask&(ST_BEAST|ST_PLANT|ST_INSECT))bonus+=200;if(type==CARD_TYPE_BEAST_WARRIOR&&(mask&ST_BEAST))bonus-=200;break;case 2:if(mask&(ST_ZOMBIE|ST_ROCK|ST_DINOSAUR))bonus+=200;if((mask&aquatic)&&!aquatic_type(type))bonus-=200;break;case 3:if(mask&(ST_DRAGON|ST_WINGED|ST_THUNDER))bonus+=200;break;case 4:if((mask&ST_WARRIOR)&&type!=CARD_TYPE_BEAST_WARRIOR)bonus+=200;break;case 5:if(mask&(ST_THUNDER|ST_FISH|ST_AQUA))bonus+=200;if(mask&(ST_MACHINE|ST_PYRO))bonus-=200;break;case 6:if(mask&(ST_SPELLCASTER|ST_FIEND|ST_ZOMBIE))bonus+=200;break;}if(terrain==5&&(id==438||id==441)&&type==CARD_TYPE_MACHINE)bonus+=500;return bonus;}
static s32 total_terrain_modifier(s32 id){s32 type=card_type(id);return Duel_GetTerrainBoost(type)+secondary_bonus(id,type,secondary_mask(id),gDuel_bTerrain);}
static void adjust_record(DuelCardRecord*c){if(c&&(c->flags&DUEL_CARD_FLAG_OCCUPIED)&&c->card_id>0)c->terrain_modifier=(s16)total_terrain_modifier(c->card_id);}
static u8*setup_hook(s32 a,s32 b){u8*r=original_setup(a,b);adjust_record((DuelCardRecord*)r);return r;}
static void apply_terrain_hook(void){s32 i;original_apply_terrain();for(i=0;i<DUEL_CARD_RECORD_COUNT;i++)adjust_record(&D_801A7AD8[i]);}
/* TEST ONLY: mark approved cards in Build Deck. The text script writes the
 * channel colour while building, so v0.2's pre-build field_54 assignment was
 * overwritten. v0.3 recolours the finished glyph entries instead. */
static void card_list_slot_hook(CardList*list,s32 slot){DuelEffectChannel*box;DuelEffectEntry*entry;s32 style; s32 id=list->entries[list->first+slot].id;gDuel_wSelectedCardID=id;style=list->entries[list->first+slot].flags!=0?6:0;box=TextBox_Create(list->kind+1,style,0x22,0x2B,0x120,0xB0);box->field_3A=slot*22;box->field_28->flags&=~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;if((list->entries[list->first+slot].flags&0x80)!=0)box->field_54=4;if(list->kind!=0)*(u16*)&box->field_3C+=0x160;if(slot!=0)box->flags_34|=0x40;func_80039A14(box);if(secondary_mask(id)!=0){for(entry=box->entry_head_24;entry<box->entry_end_20;entry++){if(entry->flags_11&DUEL_EFFECT_ENTRY_FLAG_ACTIVE)entry->field_16=7;}}}
int MemoriesModInit(const MemoriesModHost*host,MemoriesMod*mod){if(host->api<4)return 0;mod->api=4;mod->name="Field Effects + Secondary Types";if(!host->hook(host,(void*)Duel_SetupCardRecord,(void*)setup_hook,(void**)&original_setup))return 0;if(!host->hook(host,(void*)DuelEffect_ApplyTerrain,(void*)apply_terrain_hook,(void**)&original_apply_terrain))return 0;if(!host->hook(host,(void*)CardList_CreateSlotTextBox,(void*)card_list_slot_hook,(void**)&original_card_list_slot))return 0;return 1;}
