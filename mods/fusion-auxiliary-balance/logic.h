#include "fusion_round2_rules.h"
#include "fusion_round3_rules.h"
#ifndef AUX_FUSION_LOGIC_H
#define AUX_FUSION_LOGIC_H

#include "fusionist_rules.h"
#define AUX_GAIA_DRAGON_CHAMPION 37
#define AUX_SEIYARYU 358
#define AUX_METEOR_B_DRAGON 713
#define AUX_B_SKULL_DRAGON 217
#define AUX_RABID_HORSEMAN 92
#define AUX_BATTLE_WARRIOR 100
#define AUX_MUSICIAN_KING 495
#define AUX_MYSTICAL_SHEEP_1 642
#define AUX_KAMION_WIZARD 115
#define AUX_DARK_ELF 551
#define AUX_MYSTICAL_ELF 2
#define AUX_TRAP_MASTER 224
#define AUX_DARKFIRE_DRAGON 168
#define AUX_YAMATANO_SCROLL 122
#define AUX_SPARKS 343
#define AUX_HINOTAMA 344
#define AUX_THUNDER_DRAGON 425
#define AUX_TWO_MOUTH_DARKRULER 218
#define AUX_TWIN_HEADED_THUNDER_DRAGON 613
#define AUX_LAST_DIRECT_MAGIC 347
#define AUX_TIME_WIZARD 16
#define AUX_LARVAE_MOTH 56
#define AUX_GREAT_MOTH 57
#define AUX_PERFECTLY_ULTIMATE_GREAT_MOTH 67
#define AUX_COCOON_OF_EVOLUTION 72
#define AUX_RED_MEDICINE 339
#define AUX_LABYRINTH_WALL 366
#define AUX_SHADOW_GHOUL 368
#define AUX_WALL_SHADOW 369
#define AUX_ANCIENT_ELF 433
#define AUX_METALMORPH 658
#define AUX_METALZOA 392
#define AUX_LAUNCHER_SPIDER 390
#define AUX_METAL_DRAGON 409
#define AUX_METAL_FISH 438
#define AUX_GIGATECH_WOLF 412
#define AUX_MECH_BASS 441
#define AUX_CYBER_SAURUS 508

/* Static added pairs have priority over equivalent recipes from other
   table mods. In More Fusions v1.4, Larvae Moth+Cocoon => #480 (Kuwagata
   alpha); the requested evolution must explicitly win that one overlap. */
static int aux_added_exact_fusion(int a,int b)
{
    int round2=r2_exact_recipe(a,b);if(round2)return round2;
    {int round3=r3_exact_recipe(a,b);if(round3)return round3;}
    if ((a==208 && b==282) || (a==282 && b==208)) return AUX_FUSIONIST;
    if ((a==AUX_TIME_WIZARD && b==AUX_COCOON_OF_EVOLUTION) ||
        (b==AUX_TIME_WIZARD && a==AUX_COCOON_OF_EVOLUTION) ||
        (a==AUX_GREAT_MOTH && b==AUX_RED_MEDICINE) ||
        (b==AUX_GREAT_MOTH && a==AUX_RED_MEDICINE))
        return AUX_PERFECTLY_ULTIMATE_GREAT_MOTH;
    if ((a==AUX_COCOON_OF_EVOLUTION && b==AUX_LARVAE_MOTH) ||
        (b==AUX_COCOON_OF_EVOLUTION && a==AUX_LARVAE_MOTH))
        return AUX_GREAT_MOTH;
    if ((a==AUX_SHADOW_GHOUL && b==AUX_LABYRINTH_WALL) ||
        (b==AUX_SHADOW_GHOUL && a==AUX_LABYRINTH_WALL))
        return AUX_WALL_SHADOW;
    return 0;
}

/* FM's Elf fusion family, restricted to base ATK strictly below 1450.
   Celtic Guardian is an Elf in the game's fusion vocabulary even though
   the English name does not contain the word Elf. Ancient Elf (1450),
   Gemini Elf (1900) and Dark Elf (2000) are intentionally excluded. */
static int aux_elf_under_1450(int id,int base_attack)
{
    return base_attack>=0 && base_attack<1450 &&
           (id==2 || id==41 || id==264 || id==395);
}
static int aux_ancient_elf_fusion(int a,int b,int other_attack)
{
    if(a==AUX_TIME_WIZARD) return aux_elf_under_1450(b,other_attack) ? AUX_ANCIENT_ELF : 0;
    if(b==AUX_TIME_WIZARD) return aux_elf_under_1450(a,other_attack) ? AUX_ANCIENT_ELF : 0;
    return 0;
}

/* Metal immunity applies only to the metal output of the corresponding
   real Metalmorph fusion, not to a copy drawn or summoned from the deck. */
static int aux_metal_fusion_result(int a,int b,int result)
{
    int other;
    if(a==AUX_METALMORPH) other=b;
    else if(b==AUX_METALMORPH) other=a;
    else return 0;
    if((other==391 && result==AUX_METALZOA) ||
       (other==367 && result==AUX_LAUNCHER_SPIDER) ||
       (other==4 && result==AUX_METAL_DRAGON) ||
       (other==543 && result==AUX_METAL_FISH) ||
       ((other==61 || other==65) && result==AUX_GIGATECH_WOLF) ||
       ((other==542 || other==436) && result==AUX_MECH_BASS) ||
       ((other==568 || other==573) && result==AUX_CYBER_SAURUS)) return result;
    return 0;
}

static int aux_is_metal_family(int id);
/* Physical Equip materials that the retail fusion animation consumes.
 * They are Equip transformations for game RULES and origin provenance,
 * even though DuelScene_UpdateCardPlacement keeps the fusion animation.
 * Never classify a virtual Fusionist substitute as real equipment. */
static int aux_metal_equip_recipe(int a,int b)
{
    int other=a==AUX_METALMORPH?b:(b==AUX_METALMORPH?a:0);
    if(!other)return 0;
    switch(other) {
    case 391:return AUX_METALZOA;
    case 367:return AUX_LAUNCHER_SPIDER;
    case 4:return AUX_METAL_DRAGON;
    case 543:return AUX_METAL_FISH;
    case 61:case 65:return AUX_GIGATECH_WOLF;
    case 542:case 436:return AUX_MECH_BASS;
    case 568:case 573:return AUX_CYBER_SAURUS;
    default:return 0;
    }
}
static int aux_real_equip_transformation(int a,int b,int result)
{
    /* Classification depends on the REAL physical card, not the result's
     * category or Fusionist's virtual material. Future custom recipes with
     * either Equip card keep this same rule. */
    return result>0 && (a==AUX_EGOTIST || b==AUX_EGOTIST ||
                        a==AUX_METALMORPH || b==AUX_METALMORPH);
}

/* Exclusive recipes for both fusion outputs. Returned result zero means that
   this pair does not have an approved recipe for either restricted monster.
   The thunder partner's attack must be its base, never a Field/equip bonus. */
static int aux_new_fusion(int a,int b,int partner_type,int partner_attack)
{
    if ((a==AUX_YAMATANO_SCROLL && (b==AUX_SPARKS || b==AUX_HINOTAMA)) ||
        (b==AUX_YAMATANO_SCROLL && (a==AUX_SPARKS || a==AUX_HINOTAMA)))
        return AUX_DARKFIRE_DRAGON;
    /* Exact added pairs always have priority over legacy generic rules. */
    {
        int exact=aux_added_exact_fusion(a,b);
        if(exact)return exact;
    }
    if (a==AUX_THUNDER_DRAGON && b==AUX_THUNDER_DRAGON)
        return AUX_TWIN_HEADED_THUNDER_DRAGON;
    if (a==AUX_TWO_MOUTH_DARKRULER || b==AUX_TWO_MOUTH_DARKRULER) {
        int other=a==AUX_TWO_MOUTH_DARKRULER ? b:a;
        if(other!=AUX_TWO_MOUTH_DARKRULER &&
           (partner_type==CARD_TYPE_THUNDER || partner_type==CARD_TYPE_DRAGON) &&
           partner_attack>=1600 && partner_attack<2800)
            return AUX_TWIN_HEADED_THUNDER_DRAGON;
    }
    return 0;
}

/* Even a result supplied by the retail table or a later code mod must not
   reintroduce an excluded recipe for these two specific fusion products. */
static int aux_fusion_result_allowed(int target,int approved)
{
    return (target!=AUX_DARKFIRE_DRAGON &&
            target!=AUX_TWIN_HEADED_THUNDER_DRAGON &&
            target!=AUX_ANCIENT_ELF) || target==approved;
}

/* After a resolved attack, a surviving Wall Shadow changes stance.
   Keep this check pure for host-independent unit tests. */
static int aux_wall_should_defend(int is_attacker,int attack_resolved,
                                  int attack_negated,int monster_survived,
                                  int card_id)
{
    return is_attacker && attack_resolved && !attack_negated &&
           monster_survived && card_id==AUX_WALL_SHADOW;
}

/* Seiyaryu protects OTHER friendly Dragons of printed level 1 through 6. */
static int aux_seiyaryu_eligible(int card_id,int card_type,int dragon_type,
                             int level,int champion_present)
{
    return champion_present && card_id != AUX_SEIYARYU &&
           card_type == dragon_type && level>=1 && level<=6;
}

/* A lifted battle participant is still the same monster: its occupied bit
   is intentionally NOT an argument here. The battle controller temporarily
   clears that bit before the fight, then restores it afterwards. */
static int aux_lifted_battle_protected(int record,int card_id,int card_type,
                                      int dragon_type,int level,int gaia_present)
{
    int zone=record%15;
    return record>=0 && record<30 && zone>=5 && zone<10 &&
           aux_seiyaryu_eligible(card_id,card_type,dragon_type,level,gaia_present);
}

/* Battle state 10 is the retail 2D card-burning request. Redirect only an
   actual protected loser with its result panel still present, not attack
   traps or unrelated combat. The numeric state and phase are from v0.3.0. */
static int aux_gaia_skip_burn(int phase,int state,int trap,int part,
                              int has_result_panel,int protected_dragon)
{
    return phase==9 && state==10 && trap==0 && part>=0 && part<2 &&
           has_result_panel && protected_dragon;
}

/* Central Direct LP Damage registry. Each entry is a distinct origin of
   direct (non-battle) LP damage. Future effects can join here, and changes
   to Darkfire's eligible categories won't need to touch every producer. */
#define AUX_DIRECT_SOURCE_MAGIC 1
#define AUX_DIRECT_SOURCE_METEOR 2
#define AUX_DIRECT_SOURCE_GAIA 3
#define AUX_DIRECT_SOURCE_DARK_ELF 4
static int aux_direct_lp_registered(int source,int source_id)
{
    if(source==AUX_DIRECT_SOURCE_MAGIC)
        return source_id>=AUX_SPARKS && source_id<=AUX_LAST_DIRECT_MAGIC;
    if(source==AUX_DIRECT_SOURCE_METEOR)
        return source_id==AUX_METEOR_B_DRAGON;
    if(source==AUX_DIRECT_SOURCE_GAIA)
        return source_id==AUX_GAIA_DRAGON_CHAMPION;
    return 0;
}
static int aux_direct_lp_extra_hits(int source,int source_id,int darkfires)
{
    return darkfires>0 && aux_direct_lp_registered(source,source_id)
           ? darkfires : 0;
}

/* Each face-up copy contributes ONE ADDITIONAL base hit, not a progressive
   2^n multiplier. The retail spell always deals its original hit first. */
static int aux_darkfire_bonus_hits(int spell_id,int darkfire_count,int caster,
                                   int damaged_side,int kind,int amount)
{
    return kind==1 && damaged_side==(caster^1) && amount>0
           ? aux_direct_lp_extra_hits(AUX_DIRECT_SOURCE_MAGIC,spell_id,darkfire_count)
           : 0;
}

/* Battle Warrior's normal coin checks printed opponent Level >=4, not
   relative to Battle Warrior's printed Level or any fusion provenance. */
static int aux_warrior_coin_eligible(int is_battle_warrior,int foe_level,int foe_immune)
{
    return is_battle_warrior && foe_level>=4 && !foe_immune;
}
static int aux_elf_capped_heal(int current,int cap)
{
    int difference=cap-current;
    if(difference<=0)return 0;
    return difference<1000?difference:1000;
}
/* Actual LP recovered is capped; each face-up Dark Elf deals half, truncating odd LP. */
static int aux_dark_elf_heal_damage(int before,int after)
{
    return after>before ? (after-before)/2 : 0;
}
/* One Encore per failed chance effect, regardless of Musician copies. */
static int aux_encore_should_retry(int is_failure,int already_retried,int musician_present)
{
    return is_failure && !already_retried && musician_present;
}
/* Kamion Equip blocking is prospective: old equips are untouched. */
static int aux_kamion_refuses_equip(int source_side,int sheep_opponent)
{
    return source_side>=0 && source_side<2 && sheep_opponent;
}
static int aux_damage_one_hit(int life,int amount)
{
    if(life<=0 || amount>=life) return 0;
    if(amount<=0) return life;
    return life-amount;
}

static int aux_field_target_spell(int card_id)
{
    return card_id==AUX_SPARKS || card_id==AUX_HINOTAMA ||
           card_id==AUX_RED_MEDICINE;
}


/* Only these cards receive metal/fusion special rules. The debug switch
   bypasses provenance (HOW the card was summoned), not the card family. */
static int aux_is_metal_family(int id)
{
    return id==AUX_LAUNCHER_SPIDER || id==AUX_METALZOA ||
           id==AUX_METAL_DRAGON || id==AUX_METAL_FISH ||
           id==AUX_GIGATECH_WOLF || id==AUX_MECH_BASS ||
           id==AUX_CYBER_SAURUS;
}
/* Closed user-approved eligibility list. This does NOT give 48 cards new
   powers: aux_is_fusion_family below selects the cards already scripted.
   Blue-Eyes Ultimate Dragon #380 is intentionally absent. */
static int aux_fusion_whitelisted(int id)
{
    static const short ids[48]={15,30,37,43,69,86,92,115,133,168,189,215,217,223,230,272,294,370,385,389,409,427,437,442,458,465,467,471,473,482,487,495,508,509,521,531,533,545,564,565,572,594,613,617,618,639,707,713};
    int i;
    for(i=0;i<48;i++)if(id==ids[i])return 1;
    return 0;
}
static int aux_is_fusion_family(int id)
{
    return r2_member(id) || r3_member(id) || (aux_fusion_whitelisted(id) &&
           (id==AUX_METEOR_B_DRAGON || id==AUX_B_SKULL_DRAGON ||
            id==AUX_RABID_HORSEMAN || id==AUX_GAIA_DRAGON_CHAMPION ||
            id==AUX_MUSICIAN_KING || id==AUX_KAMION_WIZARD ||
            id==AUX_DARKFIRE_DRAGON || id==AUX_FUSIONIST));
}
static int aux_condition_passes(int family_member,int used_correct_fusion,int ignore_conditions)
{
    return family_member && (used_correct_fusion || ignore_conditions);
}
/* Default retail ceiling 9999. The mod's debuff must affect DEF too:
   FM represents DEF as attack_modifier + defense_modifier, so adjusting
   attack_modifier alone reduces ATK and DEF equally. */
static int aux_minus_500(int modifier)
{
    return modifier<=-32268 ? -32768 : modifier-500;
}
static int aux_piercing(int attacker_atk,int defender_def,int is_defense,
                        int attacking_horseman,int fusion_effect_active)
{
    return is_defense && attacking_horseman && fusion_effect_active &&
           attacker_atk>defender_def ? attacker_atk-defender_def : 0;
}

/* Sprite-only post-destruction repair must never reanimate a destroyed
   field record or overwrite a different summon in the same monster zone. */
static int aux_visual_instance_still_present(int occupied,int actual_id,
                                            const void *actual_data,
                                            int expected_id,const void *expected_data)
{
    return occupied && actual_id==expected_id && actual_data==expected_data;
}

/* CPU-only, planning-time virtual values. Never change real card stats. */
static int aux_ai_extra_value(int card,int allied_dragons,int damage_spells)
{
    if(card==AUX_SEIYARYU) {
        if(allied_dragons>3) allied_dragons=3;
        return 450+allied_dragons*320;
    }
    if(card==AUX_DARKFIRE_DRAGON)
        return damage_spells>0 ? 1150 : 380;
    return 0;
}

#endif
