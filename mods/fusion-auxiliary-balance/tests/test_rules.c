#include <assert.h>
#include <stdio.h>
#include "game/card_constants.h"
#include "../logic.h"
#define F(a,b,t,atk) aux_new_fusion(a,b,t,atk)
int main(void)
{
    assert(aux_dark_elf_heal_damage(7900,8000)==50);
    assert(aux_dark_elf_heal_damage(8000,9999)==999);
    assert(aux_dark_elf_heal_damage(6000,8000)==1000);
    assert(aux_dark_elf_heal_damage(8000,8000)==0);
    assert(aux_dark_elf_heal_damage(8000,7500)==0);
    /* New effect ownership / whitelist regression tests. */
    assert(aux_kamion_refuses_equip(0,1));
    assert(!aux_kamion_refuses_equip(0,0));
    assert(!aux_kamion_refuses_equip(2,1));
    assert(aux_warrior_coin_eligible(1,4,0));
    assert(aux_warrior_coin_eligible(1,10,0));
    assert(!aux_warrior_coin_eligible(1,3,0));
    assert(!aux_warrior_coin_eligible(1,4,1));
    assert(!aux_warrior_coin_eligible(0,4,0));
    assert(aux_elf_capped_heal(7000,8000)==1000);
    assert(aux_elf_capped_heal(7950,8000)==50);
    assert(aux_elf_capped_heal(8000,8000)==0);
    assert(aux_elf_capped_heal(8000,9999)==1000);
    assert(aux_elf_capped_heal(1000,1500)==500);
    assert(aux_fusion_whitelisted(115));
    assert(aux_fusion_whitelisted(495));
    assert(aux_fusion_whitelisted(713));
    assert(aux_fusion_whitelisted(613));
    assert(!aux_fusion_whitelisted(380));
    assert(!aux_fusion_whitelisted(100));
    assert(!aux_fusion_whitelisted(642));
    assert(aux_is_fusion_family(115));
    assert(!aux_is_fusion_family(100));
    assert(!aux_is_fusion_family(642));
    assert(!aux_is_fusion_family(380));
    assert(aux_encore_should_retry(1,0,1));
    assert(!aux_encore_should_retry(1,1,1));
    assert(!aux_encore_should_retry(0,0,1));
    assert(!aux_encore_should_retry(1,0,0));
    assert(!aux_direct_lp_registered(AUX_DIRECT_SOURCE_DARK_ELF,100));
    assert(!aux_direct_lp_registered(AUX_DIRECT_SOURCE_DARK_ELF,495));
    int a,b;
    /* Darkfire's only two inputs, in either order. */
    assert(F(122,343,0,0)==168);
    assert(F(343,122,0,0)==168);
    assert(F(122,344,0,0)==168);
    assert(F(344,122,0,0)==168);
    assert(F(122,345,0,0)==0);
    assert(F(343,344,0,0)==0);
    /* Two Moth evolution paths and Wall Shadow, both directions. */
    assert(F(16,72,0,0)==67 && F(72,16,0,0)==67);
    assert(F(56,72,0,0)==57 && F(72,56,0,0)==57);
    assert(F(57,339,0,0)==67 && F(339,57,0,0)==67);
    assert(F(368,366,0,0)==369 && F(366,368,0,0)==369);
    assert(aux_added_exact_fusion(56,72)==57);
    assert(aux_added_exact_fusion(72,56)==57);
    assert(aux_added_exact_fusion(56,57)==0);
    assert(F(56,339,0,0)==0);
    assert(F(16,56,0,0)==0);
    assert(F(368,72,0,0)==0);
    /* Elf family includes Celtic Guardian; threshold is strict. */
    assert(aux_ancient_elf_fusion(16,2,800)==433);
    assert(aux_ancient_elf_fusion(2,16,800)==433);
    assert(aux_ancient_elf_fusion(16,41,1400)==433);
    assert(aux_ancient_elf_fusion(264,16,500)==433);
    assert(aux_ancient_elf_fusion(395,16,300)==433);
    assert(!aux_ancient_elf_fusion(16,433,1450));
    assert(!aux_ancient_elf_fusion(16,2,1450));
    assert(!aux_ancient_elf_fusion(16,532,1900));
    assert(!aux_ancient_elf_fusion(16,551,2000));
    assert(!aux_ancient_elf_fusion(16,428,300)); /* Magician of Faith is not an Elf */
    assert(!aux_ancient_elf_fusion(428,658,300)); /* old recipe removed */
    assert(aux_metal_fusion_result(391,658,392)==392);
    assert(aux_metal_fusion_result(658,367,390)==390);
    assert(aux_metal_fusion_result(4,658,409)==409);
    assert(aux_metal_fusion_result(658,543,438)==438);
    /* New metal transformations, two materials per outcome, both orders. */
    assert(aux_metal_fusion_result(61,658,412)==412);
    assert(aux_metal_fusion_result(658,61,412)==412);
    assert(aux_metal_fusion_result(65,658,412)==412);
    assert(aux_metal_fusion_result(658,65,412)==412);
    assert(aux_metal_fusion_result(542,658,441)==441);
    assert(aux_metal_fusion_result(658,542,441)==441);
    assert(aux_metal_fusion_result(436,658,441)==441);
    assert(aux_metal_fusion_result(658,436,441)==441);
    assert(aux_metal_fusion_result(568,658,508)==508);
    assert(aux_metal_fusion_result(658,568,508)==508);
    assert(aux_metal_fusion_result(573,658,508)==508);
    assert(aux_metal_fusion_result(658,573,508)==508);
    assert(!aux_metal_fusion_result(61,658,441));
    assert(!aux_metal_fusion_result(65,658,508));
    assert(!aux_metal_fusion_result(436,658,412));
    assert(!aux_metal_fusion_result(568,658,441));
    assert(!aux_metal_fusion_result(573,339,508));
    assert(!aux_metal_fusion_result(658,542,508));
    assert(!aux_metal_fusion_result(428,658,433));
    assert(!aux_metal_fusion_result(391,658,390));
    assert(!aux_metal_fusion_result(392,0,392));

    assert(aux_wall_should_defend(1,1,0,1,369));
    assert(!aux_wall_should_defend(0,1,0,1,369));
    assert(!aux_wall_should_defend(1,0,0,1,369));
    assert(!aux_wall_should_defend(1,1,1,1,369));
    assert(!aux_wall_should_defend(1,1,0,0,369));
    assert(!aux_wall_should_defend(1,1,0,1,368));
    /* Thunder Dragon + itself needs no type/ATK test. */
    assert(F(425,425,0,0)==613);
    /* Two-mouth Darkruler + current-type Thunder/Dragon, 1600..2799. */
    for(a=0;a<=3200;a+=100) {
        int valid=(a>=1600&&a<2800);
        assert((F(218,123,CARD_TYPE_DRAGON,a)==613)==valid);
        assert((F(123,218,CARD_TYPE_THUNDER,a)==613)==valid);
        assert(F(218,123,CARD_TYPE_WARRIOR,a)==0);
    }
    assert(F(218,218,CARD_TYPE_DRAGON,2000)==0);
    assert(F(218,425,CARD_TYPE_THUNDER,1000)==0);
    assert(F(218,123,CARD_TYPE_DRAGON,1599)==0);
    assert(F(218,123,CARD_TYPE_DRAGON,1600)==613);
    assert(F(218,123,CARD_TYPE_DRAGON,2799)==613);
    assert(F(218,123,CARD_TYPE_DRAGON,2800)==0);
    /* No other pair can use a discarded Vanilla recipe to produce either. */
    for(a=1;a<=722;a++)for(b=1;b<=722;b++) {
        int new_result=F(a,b,CARD_TYPE_WARRIOR,0);
        if(new_result==168) assert((a==122&&(b==343||b==344)) ||
                                    (b==122&&(a==343||a==344)));
        if(new_result==613) assert(a==425 && b==425);
        assert(aux_fusion_result_allowed(168,new_result)==(new_result==168));
        assert(aux_fusion_result_allowed(613,new_result)==(new_result==613));
        assert(aux_fusion_result_allowed(36,new_result));
    }
    assert(aux_seiyaryu_eligible(39,0,0,6,1));
    assert(aux_seiyaryu_eligible(1,0,0,1,1));
    assert(!aux_seiyaryu_eligible(358,0,0,7,1));
    assert(!aux_seiyaryu_eligible(39,0,0,7,1));
    assert(!aux_seiyaryu_eligible(39,1,0,6,1));
    assert(!aux_seiyaryu_eligible(39,0,0,6,0));
    /* The attacked Yamatano / Yamadron can be lifted from the field
       (OCCUPIED cleared) and must STILL be guarded if Gaia survives. */
    assert(aux_lifted_battle_protected(5,122,CARD_TYPE_DRAGON,CARD_TYPE_DRAGON,2,1));
    assert(aux_lifted_battle_protected(20,122,CARD_TYPE_DRAGON,CARD_TYPE_DRAGON,2,1));
    assert(!aux_lifted_battle_protected(20,122,CARD_TYPE_DRAGON,CARD_TYPE_DRAGON,2,0));
    assert(!aux_lifted_battle_protected(20,122,CARD_TYPE_THUNDER,CARD_TYPE_DRAGON,2,1));
    assert(!aux_lifted_battle_protected(20,358,CARD_TYPE_DRAGON,CARD_TYPE_DRAGON,7,1));
    assert(!aux_lifted_battle_protected(20,122,CARD_TYPE_DRAGON,CARD_TYPE_DRAGON,7,1));
    assert(!aux_lifted_battle_protected(19,122,CARD_TYPE_DRAGON,CARD_TYPE_DRAGON,2,1));
    /* Javelin's proven battle sequencing: jump over state 10's 2D burn
       BEFORE presentation, for either protected battle participant. */
    assert(aux_gaia_skip_burn(9,10,0,0,1,1));
    assert(aux_gaia_skip_burn(9,10,0,1,1,1));
    assert(!aux_gaia_skip_burn(9,10,0,1,1,0));
    assert(!aux_gaia_skip_burn(9,10,0,0,0,1));
    assert(!aux_gaia_skip_burn(9,10,659,1,1,1));
    assert(!aux_gaia_skip_burn(5,10,0,1,1,1));
    assert(!aux_gaia_skip_burn(9,11,0,1,1,1));
    assert(!aux_gaia_skip_burn(9,10,0,2,1,1));
    /* Protected monster visuals must be repaired only for the exact
       surviving field instance. */
    {
        int old_card=1,replacement_card=2;
        assert(aux_visual_instance_still_present(1,122,&old_card,122,&old_card));
        assert(!aux_visual_instance_still_present(0,122,&old_card,122,&old_card));
        assert(!aux_visual_instance_still_present(1,358,&old_card,122,&old_card));
        assert(!aux_visual_instance_still_present(1,122,&replacement_card,122,&old_card));
    }
    /* The Direct LP Damage category keeps Magic, Meteor and Gaia
       distinct from ordinary piercing/battle damage. */
    for(a=343;a<=347;a++) {
        assert(aux_direct_lp_registered(AUX_DIRECT_SOURCE_MAGIC,a));
        assert(aux_direct_lp_extra_hits(AUX_DIRECT_SOURCE_MAGIC,a,2)==2);
    }
    assert(!aux_direct_lp_registered(AUX_DIRECT_SOURCE_MAGIC,342));
    assert(!aux_direct_lp_registered(AUX_DIRECT_SOURCE_MAGIC,348));
    assert(aux_direct_lp_registered(AUX_DIRECT_SOURCE_METEOR,713));
    assert(aux_direct_lp_registered(AUX_DIRECT_SOURCE_GAIA,37));
    assert(!aux_direct_lp_registered(AUX_DIRECT_SOURCE_GAIA,713));
    assert(!aux_direct_lp_registered(AUX_DIRECT_SOURCE_METEOR,37));
    assert(!aux_direct_lp_registered(0,92));
    assert(!aux_direct_lp_registered(0,713));
    for(a=0;a<=5;a++) {
        assert(aux_direct_lp_extra_hits(AUX_DIRECT_SOURCE_METEOR,713,a)==a);
        assert(aux_direct_lp_extra_hits(AUX_DIRECT_SOURCE_GAIA,37,a)==a);
    }
    assert(aux_direct_lp_extra_hits(AUX_DIRECT_SOURCE_METEOR,37,5)==0);
    assert(aux_direct_lp_extra_hits(0,92,5)==0);
    assert(aux_direct_lp_extra_hits(AUX_DIRECT_SOURCE_MAGIC,343,0)==0);
    /* 1, 2 and 3 Darkfires add 1, 2 and 3 ORIGINAL hits, not powers of two. */
    for(a=343;a<=347;a++) {
        assert(aux_darkfire_bonus_hits(a,1,1,0,1,100)==1);
        assert(aux_darkfire_bonus_hits(a,2,1,0,1,100)==2);
        assert(aux_darkfire_bonus_hits(a,3,1,0,1,100)==3);
        assert(aux_darkfire_bonus_hits(a,1,0,1,1,100)==1);
    }
    assert(aux_darkfire_bonus_hits(343,0,1,0,1,100)==0);
    assert(aux_darkfire_bonus_hits(343,1,1,1,1,100)==0);
    assert(aux_darkfire_bonus_hits(343,1,1,0,0,100)==0);
    assert(aux_darkfire_bonus_hits(348,1,1,0,1,100)==0);
    assert(aux_darkfire_bonus_hits(343,1,1,0,1,0)==0);
    assert(aux_damage_one_hit(aux_damage_one_hit(aux_damage_one_hit(8000,100),100),100)==7700);
    assert(aux_damage_one_hit(80,100)==0);
    assert(aux_damage_one_hit(0,100)==0);
    assert(aux_damage_one_hit(999,0)==999);
    assert(aux_field_target_spell(343));
    assert(aux_field_target_spell(344));
    assert(aux_field_target_spell(339));
    assert(!aux_field_target_spell(345));
    assert(!aux_field_target_spell(340));
    assert(aux_ai_extra_value(358,0,0)==450);
    assert(aux_ai_extra_value(358,5,0)==1410);
    assert(aux_ai_extra_value(168,0,2)>aux_ai_extra_value(168,0,0));
    assert(aux_ai_extra_value(38,3,5)==0);
    assert(aux_is_metal_family(390) && aux_is_metal_family(412));
    assert(aux_is_metal_family(441) && aux_is_metal_family(508));
    assert(!aux_is_metal_family(433));
    assert(aux_is_fusion_family(713) && aux_is_fusion_family(217));
    assert(aux_is_fusion_family(92) && aux_is_fusion_family(37));
    assert(!aux_is_fusion_family(358));
    assert(aux_is_fusion_family(168));
    /* Debug knob: OFF means strict provenance, ON ignores ONLY provenance. */
    assert(!aux_condition_passes(aux_is_metal_family(390),0,0));
    assert(aux_condition_passes(aux_is_metal_family(390),1,0));
    assert(aux_condition_passes(aux_is_metal_family(390),0,1));
    assert(!aux_condition_passes(aux_is_metal_family(433),0,1));
    assert(!aux_condition_passes(aux_is_fusion_family(713),0,0));
    assert(aux_condition_passes(aux_is_fusion_family(713),1,0));
    assert(aux_condition_passes(aux_is_fusion_family(713),0,1));
    assert(!aux_condition_passes(aux_is_fusion_family(358),0,1));
    /* Spellbinding Circle knockdown hits ATK and DEF together. */
    assert(aux_minus_500(0)==-500);
    assert(aux_minus_500(-500)==-1000);
    assert(aux_minus_500(-32600)==-32768);
    /* Pierce only with a defense-position monster and successful attack. */
    assert(aux_piercing(2000,1200,1,1,1)==800);
    assert(aux_piercing(2000,2200,1,1,1)==0);
    assert(aux_piercing(2000,1200,0,1,1)==0);
    assert(aux_piercing(2000,1200,1,1,0)==0);
    assert(aux_piercing(2000,1200,1,0,1)==0);
    puts("Beta13: Direct LP Damage registration, fusion & metal effects, default strict conditions: PASS");
    return 0;
}
