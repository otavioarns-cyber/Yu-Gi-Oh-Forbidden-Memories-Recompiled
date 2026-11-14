#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "game/card_constants.h"
#include "pc/cards/cards.h"
#include "pc/cards/tables.h"
#include "ritual_recipe_provider_v1.h"

static int test_types[900], test_base[900], test_levels[900];
static int test_fusion[900];
int gCard_nCount=800;
int gDuel_adwCardStats[900];
int Cards_Valid(int id){return id>=1&&id<=gCard_nCount;}
int Cards_Type(int id){return test_types[id];}
int Cards_BaseId(int id){return test_base[id]?test_base[id]:id;}
int Cards_Level(int id){return test_levels[id];}
int Cards_InFusionGroup(int id,int group){return group==CARD_FUSION_GROUP_FEMALE && test_fusion[id];}
#define MASK_ID 720
#define MASK_RITUAL_ID 693
#define SHIELD_ID 362
#define BONUS 500
#define DARK_HOLE_ID 336
#define YAMADRON_ID 357
#define YAMADRON_RITUAL_ID 666
#define WINGS_OF_WICKED_FLAME_ID 101
#define YAMADRON_DRAGON_MAX_ATK 1599
#define YAMADRON_DRAGON_MAX_DEF 1799
#define FINAL_FLAME_ID 345
#define OOKAZI_ID 346
#define TREMENDOUS_FIRE_ID 347
#define PSYCHO_PUPPET_ID 715
#define PUPPET_RITUAL_ID 695
#define MYSTERIOUS_PUPPETEER_ID 166
#define HUNGRY_BURGER_ID 702
#define HAMBURGER_RECIPE_ID 677
#define BATTLE_STEER_ID 14
#define HUNGRY_BONUS 500
#define FIENDS_MIRROR_ID 365
#define BEASTLY_MIRROR_RITUAL_ID 674
#define JAVELIN_BEETLE_ID 717
#define JAVELIN_PACT_ID 696
#define GATE_GUARDIAN_ID 374
#define GATE_GUARDIAN_RITUAL_ID 667
#define JAVELIN_AURA_BONUS 400
#define CRAB_TURTLE_ID 710
#define TURTLE_OATH_ID 692
#define FORTRESS_WHALE_ID 718
#define FORTRESS_WHALE_OATH_ID 700
#define UMI_ID 334
#define UMI_TERRAIN 5
#define CRAB_AURA_ATK 200
#define CRAB_AURA_DEF 800
#define FORTRESS_FISH_DEF 500
#define TRI_HORNED_ID 705
#define PERFORMANCE_ID 701
#define PERFORMANCE_RITUAL_ID 676
#define DANCING_ELF_ID 395
#define CHAOS_ID 722
#define SERPENT_NIGHT_ID 706
#define SERPENT_RITUAL_ID 691
#define SERPENT_FEAR_BONUS 1000
#define WASTELAND_ID 331
#define WASTELAND_TERRAIN 2
#define SUPER_WAR_LION_ID 356
#define WAR_LION_RITUAL_ID 673
#define LEOGUN_ID 403
#define GARVAS_ID 483
#define WAR_LION_BEAST_BONUS 500
#define SENGENJIN_ID 703
#define SENGENJIN_RITUAL_ID 678
#define MILLENNIUM_GOLEM_ID 453
#define SENGENJIN_STRONG_ATK 1500
#define BLACK_LUSTER_SOLDIER_ID 364
#define SOGEN_ID 333
#define SOGEN_TERRAIN 4
#define BLS_WARRIOR_AURA_BONUS 1000
#define BLUE_EYES_ULTIMATE_ID 380
#define ULTIMATE_DRAGON_RITUAL_ID 675
#define COSMO_QUEEN_ID 708
#define COSMO_QUEEN_RITUAL_ID 698
#define DOKURORIDER_ID 719
#define DOKURORIDER_RITUAL_ID 699
#define DOKURORIDER_ESCAPE_SFX_ID 0xFFFE
#define TRI_HORN_BREAK_SFX_ID 0xFFFD
#define GARMA_SWORD_SPEND_SFX_ID 0xFFFC
#define JAVELIN_SHELL_SFX_ID 0xFFFB
#define HUNGRY_BITE_SFX_ID 0xFFFA
#define HUNGRY_BITE_DELAY_FRAMES 74
#define TEMPLE_OF_SKULLS_ID 146
#define COSMO_FEMALE_BONUS 500
#define ICE_WATER_ID 431
#define ENCHANTING_MERMAID_ID 251
#define DARK_WITCH_ID 582
#define AMAZON_OF_THE_SEAS_ID 626
#define NEKOGAL_2_ID 627
#define QUEEN_OF_AUTUMN_LEAVES_ID 638
#define MYSTICAL_SAND_ID 531
#define DOKURORIDER_AURA_ATK 1200
#define CHAKRA_ID 709
#define CHAKRA_RITUAL_ID 694
#define WALL_SHADOW_ID 369
#define ZERA_ID 360
#define ZERA_RITUAL_ID 671
#define YAMI_ID 335
#define YAMI_TERRAIN 6
#define ZERA_AURA_PENALTY 1500
#define SKULL_GUARDIAN_ID 704
#define NOVOX_PRAYER_ID 679
#define KAMAKIRIMAN_ID 535
#define SKULL_FIELD_BONUS 1000
#define SKULL_HAND_BONUS 500
#define GARMA_SWORD_ID 716
#define GARMA_SWORD_OATH_ID 697
#define VISHWAR_RANDI_ID 239
#define SUCCUBUS_KNIGHT_ID 621
#define SWORD_DARK_DESTRUCTION_ID 302
typedef struct { int ritual_id; int monster_id; const char *recipe_key; const char *effect_key; } RitualOptionDefinition;
static const RitualOptionDefinition ritual_options[] = {
    { 665, SHIELD_ID, "millennium_shield_recipe", "millennium_shield" },
    { YAMADRON_RITUAL_ID, YAMADRON_ID, "yamadron_recipe", "yamadron" },
    { PUPPET_RITUAL_ID, PSYCHO_PUPPET_ID, "psycho_puppet_recipe", "psycho_puppet" },
    { HAMBURGER_RECIPE_ID, HUNGRY_BURGER_ID, "hungry_burger_recipe", "hungry_burger" },
    { BEASTLY_MIRROR_RITUAL_ID, FIENDS_MIRROR_ID, "fiends_mirror_recipe", "fiends_mirror" },
    { MASK_RITUAL_ID, MASK_ID, "mask_shine_dark_recipe", "mask_shine_dark" },
    { JAVELIN_PACT_ID, JAVELIN_BEETLE_ID, "javelin_beetle_recipe", "javelin_beetle" },
    { GATE_GUARDIAN_RITUAL_ID, GATE_GUARDIAN_ID, "gate_guardian_recipe", "gate_guardian" },
    { TURTLE_OATH_ID, CRAB_TURTLE_ID, "crab_turtle_recipe", "crab_turtle" },
    { FORTRESS_WHALE_OATH_ID, FORTRESS_WHALE_ID, "fortress_whale_recipe", "fortress_whale" },
    { GARMA_SWORD_OATH_ID, GARMA_SWORD_ID, "garma_sword_recipe", "garma_sword" },
    { SERPENT_RITUAL_ID, SERPENT_NIGHT_ID, "serpent_night_recipe", "serpent_night" },
    { PERFORMANCE_RITUAL_ID, PERFORMANCE_ID, "performance_sword_recipe", "performance_sword" },
    { WAR_LION_RITUAL_ID, SUPER_WAR_LION_ID, "super_war_lion_recipe", "super_war_lion" },
    { SENGENJIN_RITUAL_ID, SENGENJIN_ID, "sengenjin_recipe", "sengenjin" },
    { DOKURORIDER_RITUAL_ID, DOKURORIDER_ID, "dokurorider_recipe", "dokurorider" },
    { NOVOX_PRAYER_ID, SKULL_GUARDIAN_ID, "skull_guardian_recipe", "skull_guardian" },
    { CHAKRA_RITUAL_ID, CHAKRA_ID, "chakra_recipe", "chakra" }
};

static unsigned char recipe_settings[sizeof(ritual_options)/sizeof(ritual_options[0])];

static inline const RitualOptionDefinition *option_by_ritual(int id)
{
    switch(id) {
    case 665: return &ritual_options[0];
    case YAMADRON_RITUAL_ID: return &ritual_options[1];
    case PUPPET_RITUAL_ID: return &ritual_options[2];
    case HAMBURGER_RECIPE_ID: return &ritual_options[3];
    case BEASTLY_MIRROR_RITUAL_ID: return &ritual_options[4];
    case MASK_RITUAL_ID: return &ritual_options[5];
    case JAVELIN_PACT_ID: return &ritual_options[6];
    case GATE_GUARDIAN_RITUAL_ID: return &ritual_options[7];
    case TURTLE_OATH_ID: return &ritual_options[8];
    case FORTRESS_WHALE_OATH_ID: return &ritual_options[9];
    case GARMA_SWORD_OATH_ID: return &ritual_options[10];
    case SERPENT_RITUAL_ID: return &ritual_options[11];
    case PERFORMANCE_RITUAL_ID: return &ritual_options[12];
    case WAR_LION_RITUAL_ID: return &ritual_options[13];
    case SENGENJIN_RITUAL_ID: return &ritual_options[14];
    case DOKURORIDER_RITUAL_ID: return &ritual_options[15];
    case NOVOX_PRAYER_ID: return &ritual_options[16];
    case CHAKRA_RITUAL_ID: return &ritual_options[17];
    default: return 0;
    }
}

#include "skull_monster.h"
static int mask_is_named(int id) { return id==102 || id==182 || id==220; }
static int mask_generic(int id, int type)
{
    unsigned stats;
    if (id<=0) return 0;
    stats=(unsigned)gDuel_adwCardStats[id-1];
    return ((stats>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK)==(unsigned)type &&
        (stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1999 &&
        ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1799;
}
static int garma_is_named(int id) { return id==VISHWAR_RANDI_ID || id==SUCCUBUS_KNIGHT_ID; }
static int garma_generic(int id, int type)
{
    unsigned stats; if(id<=0) return 0; stats=(unsigned)gDuel_adwCardStats[id-1];
    return ((stats>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK)==(unsigned)type &&
        (stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=2549 &&
        ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=2149;
}
static int performance_generic(int id,int female)
{
    unsigned stats;if(id<=0)return 0;stats=(unsigned)gDuel_adwCardStats[id-1];
    return (stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1949 &&
        ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1849 &&
        (female?Cards_InFusionGroup(id,CARD_FUSION_GROUP_FEMALE):Cards_Type(id)==CARD_TYPE_WARRIOR);
}

#include "ritual_recipe_data.h"
static int compat_requirement_matches(int id,const TablesRitualRequirement *q)
{
    unsigned stats;int atk,def;
    if(id<1||id>CARD_TABLE_COUNT||Cards_Type(id)>=CARD_TYPE_MAGIC)return 0;
    stats=(unsigned)gDuel_adwCardStats[id-1];
    atk=(stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    def=((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    return (!q->card||id==q->card||Cards_BaseId(id)==q->card) &&
        (q->type<0||Cards_Type(id)==q->type) &&
        (!q->fusion_group ||
         (q->fusion_group==255 ? RitualCategory_IsSkullMonster(id) :
          q->fusion_group==254 ? (id!=CHAKRA_ID && Cards_BaseId(id)!=CHAKRA_ID) :
          Cards_InFusionGroup(id,q->fusion_group))) &&
        atk>=q->min_attack&&def>=q->min_defense &&
        (q->max_attack<0||atk<=q->max_attack) &&
        (q->max_defense<0||def<=q->max_defense) &&
        (q->min_level<0||Cards_Level(id)>=q->min_level) &&
        (q->max_level<0||Cards_Level(id)<=q->max_level) &&
        (!q->defense_gt_attack||def>atk);
}


#include "ritual_recipe_export.h"

static void stats(int id,int atk,int def,int type,int level){
    /* gDuel_adwCardStats uses ATK/DEF scaled in 10-point units. */
    gDuel_adwCardStats[id-1]=(atk/CARD_STAT_SCALE)|((def/CARD_STAT_SCALE)<<CARD_STAT_DEFENSE_SHIFT)|((unsigned)type<<CARD_STAT_TYPE_SHIFT);
    test_types[id]=type;test_levels[id]=level;
}
int main(void){
    RitualRecipeRequirementV1 roles[3];int result=0,i,n,active=0;
    int ids[]={665,666,695,677,674,696,667,692,700,697,691,676,673,678,699,694,679,696};
    /* Fix the duplicate in this sanity-list with the independently sourced manifest below. */
    int all[]={665,666,695,677,674,693,696,667,692,700,697,691,676,673,678,699,679,694};
    (void)ids;
    for(i=1;i<800;i++)stats(i,500,600,CARD_TYPE_WARRIOR,4);
    stats(704,2050,2500,CARD_TYPE_WARRIOR,8);
    stats(709,2800,2900,CARD_TYPE_FIEND,7);
    stats(545,1700,1900,CARD_TYPE_ZOMBIE,5);
    stats(535,800,600,CARD_TYPE_INSECT,3);
    stats(378,2000,1600,CARD_TYPE_WARRIOR,6);
    stats(369,1600,1800,CARD_TYPE_WARRIOR,7);
    stats(20,2000,1900,CARD_TYPE_FIEND,7);
    stats(22,2500,1200,CARD_TYPE_FIEND,6);
    stats(395,900,900,CARD_TYPE_SPELLCASTER,4);
    stats(239,700,500,CARD_TYPE_WARRIOR,3);
    stats(621,750,550,CARD_TYPE_WARRIOR,3);
    stats(102,600,500,CARD_TYPE_FIEND,4);
    stats(182,600,500,CARD_TYPE_FIEND,4);
    stats(220,600,500,CARD_TYPE_FIEND,4);
    stats(30,1500,1100,CARD_TYPE_FIEND,6);
    stats(31,1500,1100,CARD_TYPE_SPELLCASTER,6);
    stats(33,1500,1100,CARD_TYPE_WARRIOR,6);
    stats(34,1500,1100,CARD_TYPE_FIEND,6);
    stats(35,1500,1100,CARD_TYPE_SPELLCASTER,6);test_fusion[35]=1;
    stats(719,1200,1400,CARD_TYPE_ZOMBIE,8);
    for(i=0;i<18;i++)recipe_settings[i]=1;
    assert(recipe_export_v1.abi==1 && recipe_export_v1.get && recipe_export_v1.matches);
    assert(recipe_export_get(679,roles,3,&result)==-1);
    recipe_export_active=1;
    for(i=0;i<18;i++){
        n=recipe_export_get(all[i],roles,3,&result);
        if(n!=3){fprintf(stderr,"missing ritual #%d index %d\n",all[i],i);return 2;}
        assert(result>0);active++;
    }
    assert(active==18);
    assert(recipe_export_get(679,roles,2,&result)==-1);
    assert(recipe_export_get(679,roles,3,&result)==3);
    assert(result==704 && roles[0].exact_card==535);
    assert(roles[1].custom_group==255 && roles[2].type==CARD_TYPE_WARRIOR);
    assert(roles[1].max_attack==2049 && roles[1].max_defense==2499);
    assert(recipe_export_matches(679,0,535));
    assert(recipe_export_matches(679,1,545));
    assert(!recipe_export_matches(679,1,535));
    assert(recipe_export_matches(679,2,378));
    assert(!recipe_export_matches(679,2,704));
    assert(recipe_export_get(694,roles,3,&result)==3);
    assert(roles[1].type==CARD_TYPE_FIEND && roles[1].min_level==7);
    assert(roles[1].max_attack==-1 && roles[1].custom_group==254);
    assert(recipe_export_matches(694,1,20));
    assert(!recipe_export_matches(694,1,709));
    assert(!recipe_export_matches(694,1,22));
    assert(recipe_export_get(693,roles,3,&result)==3);
    assert(recipe_export_matches(693,0,102));
    assert(recipe_export_matches(693,1,30));
    assert(recipe_export_matches(693,2,31));
    assert(!recipe_export_matches(693,2,30));
    assert(recipe_export_get(697,roles,3,&result)==3);
    assert(recipe_export_matches(697,0,239));
    assert(recipe_export_matches(697,1,33));
    assert(recipe_export_matches(697,2,34));
    assert(recipe_export_get(676,roles,3,&result)==3);
    assert(recipe_export_matches(676,0,395));
    assert(recipe_export_matches(676,1,33));
    assert(recipe_export_matches(676,2,35));
    assert(!recipe_export_matches(676,1,395));
    recipe_settings[16]=0;
    assert(recipe_export_get(679,roles,3,&result)==-1);
    assert(!recipe_export_matches(679,1,545));
    recipe_export_active=0;
    assert(recipe_export_get(693,roles,3,&result)==-1);
    assert(!recipe_export_matches(693,0,102));
    printf("PASS producer: 18 recipes, Skull Monster, Chakra exclusion, 3 specials, toggles, disabled mod\n");
    return 0;
}
