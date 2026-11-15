"""Regressions for the FM Editor / v0.3.0 fusion-manifest semantics."""
import json
from pathlib import Path

manifest = json.loads((Path(__file__).resolve().parent.parent / 'mod.json').read_text())
rules = manifest['fusions']
removed = {e['remove'] for e in rules if 'remove' in e}
assert {168, 613, 433}.issubset(removed), f'missing retail fusion removals: {removed}'
assert 'all' not in removed, 'must not remove unrelated vanilla fusions'
added = {tuple(sorted(e['with'])): e['result'] for e in rules if 'with' in e}
assert added == {
    (122, 343): 168, (122, 344): 168, (425, 425): 613,
    (16, 72): 67, (56, 72): 57, (57, 339): 67, (366, 368): 369,
    (61, 658): 412, (65, 658): 412,
    (542, 658): 441, (436, 658): 441,
    (568, 658): 508, (573, 658): 508, (208,282): 189, (62,318): 63,
    (162,175):707,(412,512):370,(95,120):385,(7,603):427,(125,466):467,
    (603,626):442,(32,81):509,(149,560):471,(36,97):564,(387,647):223,
    (538,636):465,(635,649):572,(626,646):565,
}, added
assert len(added) == sum('with' in e for e in rules)

# v0.3.0: Tables_FilterFusion only blocks the *retail* result;
# explicitly added recipes take precedence over the retail lookup.
def game_fusion_result(pair, retail):
    pair = tuple(sorted(pair))
    return added.get(pair, 0 if retail in removed else retail)

assert game_fusion_result((37, 425), 613) == 0  # user's Mountain Mage regression
assert game_fusion_result((425, 37), 613) == 0
assert game_fusion_result((122, 343), 168) == 168
assert game_fusion_result((122, 344), 0) == 168
assert game_fusion_result((425, 425), 0) == 613
for material1, material2, result in ((16,72,67),(56,72,57),(57,339,67),(366,368,369)):
    assert game_fusion_result((material1, material2), 0)==result
    assert game_fusion_result((material2, material1), 0)==result
assert game_fusion_result((23, 31), 45) == 45  # unrelated retail fusion still works
assert game_fusion_result((10, 15), 168) == 0
assert game_fusion_result((218, 999), 613) == 0  # special conditional is code, not a static rule
old_texts = [x['description'] for x in manifest['cards']]
assert all('{f8 0A 05}<Effect>{f8 0A 00}' in x for x in old_texts[1:3])
assert any(x['replace']==369 and 'Defense Position' in x['description'] for x in manifest['cards'])
print('FM Editor manifest + forbidden retail fusions + old text: PASS')

# Beta.9: Quadrado selects the field target for only the three designated
# face-up spells; Cross retains its untouched Vanilla path.
src = (Path(__file__).resolve().parent.parent / 'mod.c').read_text()
assert '!(gInput_wPad1Pressed & PAD_BUTTON_SQUARE)' in src
assert 'PAD_BUTTON_L1' not in src
assert 'static void aux_hand_actions(void)' in src
assert 'aux_field_target_spell(card_id)' in src
assert 'original_hand_actions();' in src
assert manifest['version']=='0.1.0-beta.44-effect-ai'
metal={x['replace']:x for x in manifest['cards'] if x['replace'] in (390,392,409,438,412,441,508)}
assert len(metal)==7
for item in metal.values():
    assert item['description']=='{f8 0A 04}<Metalmorph Effect>{f8 0A 00}\nThis monster is unaffected by the opponent\'s Magic Cards.'
assert 'static void aux_card_finalize(void)' in src
assert 'static s32 aux_card_effect_update(void)' in src
assert 'static int aux_metal_protected(int record)' in src
assert 'static int aux_card_placement(void)' not in src # placement hook has void return
assert 'aux_ancient_elf_fusion' in (Path(__file__).resolve().parent.parent / 'logic.h').read_text()
assert not any(x.get('with')==[428,658] for x in rules)
assert not any(x.get('with')==[428,16] for x in rules)

print('Beta.9 Square routing and Vanilla fallback: PASS')

# Beta.11: each listed transformation must be a real Metalmorph pair.
for a,b,result in ((61,658,412),(65,658,412),(542,658,441),
                   (436,658,441),(568,658,508),(573,658,508)):
    assert game_fusion_result((a,b),0)==result
    assert game_fusion_result((b,a),0)==result
assert all(x['frame']=='Orange' for x in metal.values())
assert all(x['description'].startswith('{f8 0A 04}<Metalmorph Effect>') for x in metal.values())
print('Beta11: seven metal transforms, six exact recipes, title markup: PASS')

# Beta12: toggle is visible in native Settings > Mods and OFF by default.
assert len(manifest['settings'])==43
assert [x['key'] for x in manifest['settings'][-2:]]==['effect_ai','effect_ai_trace']
setting=manifest['settings'][0]
assert setting['key']=='ignore_effect_conditions'
assert setting['type']=='bool' and setting['default']==0
assert 'aux_host->setting(aux_host,"ignore_effect_conditions",0)' in src
assert 'aux_condition_passes(aux_is_metal_family(r->card_id)' in src
assert 'aux_condition_passes(aux_is_fusion_family(r->card_id)' in src
assert 'fusion_pending_result' in src and 'fusion_origin[index]' in src
assert 'aux_queue_lp((index/AUX_SIDE_STRIDE)^1,1000,347,AUX_DIRECT_SOURCE_METEOR)' in src
assert 'aux_queue_lp((a/AUX_SIDE_STRIDE)^1,500,346,AUX_DIRECT_SOURCE_GAIA)' in src
assert 'aux_queue_skull_effect(index/AUX_SIDE_STRIDE)' in src
assert 'battle_pierce_amount' in src
assert 'aux_has_seiyaryu' in src
assert 'aux_live(i,AUX_SEIYARYU)' in src
by_id={x['replace']:x for x in manifest['cards']}
assert by_id[358]['description'].startswith('{f8 0A 05}<Effect>')
for id in (37,92,217,713):
    assert by_id[id]['description'].startswith('{f8 0A 07}<Fusion Effect>{f8 0A 00}\n')
    assert 'When Fusion Summoned' not in by_id[id]['description']
assert '1000 damage' in by_id[713]['description']
assert '500 ATK and DEF' in by_id[217]['description']
assert 'piercing' in by_id[92]['description']
assert '500 damage' in by_id[37]['description']
assert 'Protects your other Dragons' not in by_id[37]['description']
print('Beta12: Fusion Effects + Seiyaryu + settings OFF: PASS')

# Beta13: short flavor text must fit the card box, leaving the full rule.
limits = {37:118, 92:118, 217:118, 713:118}
for card_id,max_len in limits.items():
    description=by_id[card_id]['description']
    body=description.split('\n',1)[1]
    assert len(body)<=max_len, (card_id,len(body),body)
    assert description.startswith('{f8 0A 07}<Fusion Effect>{f8 0A 00}\n')
assert '1000 damage to the opponent' in by_id[713]['description']
assert '500 damage to the opponent' in by_id[37]['description']
assert 'destro' in by_id[37]['description']
assert '500 ATK and DEF' in by_id[217]['description']
assert 'piercing battle damage' in by_id[92]['description']
assert 'Defense Position' in by_id[92]['description']
assert 'Doubles Explosive Damage.' in by_id[168]['description']

# Beta13: visual repairs must respect instance identity, wait for native FX
# requests to finish, and never revive a truly removed monster.
assert 'static void aux_repair_mass_visuals(void)' in src
assert 'mass_visual_repair_pending' in src
assert 'mass_visual_id[i]=D_801A7AD8[i].card_id' in src
assert 'mass_visual_data[i]=D_801A7AD8[i].data' in src
assert 'aux_visual_instance_still_present(' in src
assert 'r->card_id,r->data,mass_visual_id[i],mass_visual_data[i]' in src
assert 'Duel_ApplyCardObjectFlags(obj)' in src
assert 'if(!requests_busy)aux_repair_mass_visuals();' in src
assert 'aux_mass_begin(); original_board(); aux_mass_end();' in src
assert 'aux_mass_begin(); original_raigeki(); aux_mass_end();' in src
assert 'aux_mass_begin(); original_monster_removal(); aux_mass_end();' in src

# Separate Direct LP Damage category: original Magic events and fusion
# monster effects participate, piercing (source 0) does not.
logic=(Path(__file__).resolve().parent.parent/'logic.h').read_text()
assert 'AUX_DIRECT_SOURCE_MAGIC' in logic
assert 'AUX_DIRECT_SOURCE_METEOR' in logic
assert 'AUX_DIRECT_SOURCE_GAIA' in logic
assert 'aux_direct_lp_registered(' in logic
assert 'aux_direct_lp_extra_hits(' in logic
assert 'extra=source==0 ? 0 :' in src
assert 'aux_queue_lp((a/AUX_SIDE_STRIDE)^1,battle_pierce_amount,0,0);' in src
assert 'aux_count_darkfire(side^1)' in src
print('Beta13: concise descriptions, protected-card sprite repair, Direct LP Damage: PASS')

# Beta21: approved reorganization, no silent assignment of the 48.
by_id={x['replace']:x for x in manifest['cards']}
assert 642 not in by_id, 'Mystical Sheep #1 must have no customized card description'
assert 115 in by_id and 551 in by_id and 2 in by_id and 100 in by_id
assert 'Its mystical aura rejects enemy equipment.' in by_id[115]['description']
assert '<Fusion Effect>' in by_id[115]['description']
assert '<Fusion Effect>' not in by_id[100]['description']
assert by_id[100]['description'].startswith('{f8 0A 05}<Effect>')
assert 'Level 4+' in by_id[100]['description']
assert 'Half the LP' in by_id[551]['description']
assert '1000 LP once' in by_id[2]['description']
assert not {224,283,643}.intersection(by_id), 'No unapproved new card powers'
assert {37,92,217,713,168,495,115}.issubset(by_id)
for id,item in by_id.items():
    if '<Fusion Effect>' in item['description']:
        assert id in {37,92,217,713,168,495,115,189,613,707,370,69,385,427,467,442,509,471,564,223,465,531,572,565,521,618,437,473,482,389,639,15,545,617,230,86,294,133,215,594,533,43,272,30,487,458}, id
# Closed whitelist in source is explicit and contains 48 vanilla TCG/OCG Fusions.
logic=(Path(__file__).resolve().parent.parent/'logic.h').read_text()
import re
ids=re.search(r'static const short ids\[48\]=\{([^}]+)\}',logic)
assert ids is not None
listed=[int(v) for v in ids.group(1).split(',')]
assert len(listed)==len(set(listed))==48
assert 380 not in listed and 115 in listed and 495 in listed
assert 'id==AUX_KAMION_WIZARD' in logic
assert 'id==AUX_BATTLE_WARRIOR' not in logic.split('static int aux_is_fusion_family',1)[1].split('static int aux_condition_passes',1)[0]
assert 'id==AUX_MYSTICAL_SHEEP_1' not in logic.split('static int aux_is_fusion_family',1)[1].split('static int aux_condition_passes',1)[0]
assert 'static void aux_dark_elf_react_to_healing' in src
assert 'if(aux_live(i,AUX_DARK_ELF))n++' in src
assert 'if(aux_live(i,AUX_BATTLE_WARRIOR))n++' not in src
assert 'aux_warrior_coin_eligible(id==AUX_BATTLE_WARRIOR' in src
assert 'aux_fused_kamion_during_equip' in src
assert 'aux_kamion_refresh' in src
assert 'aux_has_kamion' in src
assert 'aux_elf_check_healing' in src and 'aux_elf_healed[i]=1' in src
assert 'aux_elf_capped_heal' in src
assert 'AUX_HOOK(DuelScene_UpdateCardPlacement' in src
assert 'AUX_HOOK(DuelEffect_ApplyLifePointRecovery' in src
assert 'DuelEffect_CreateRequest(5)' in src
assert 'encore-v1' in src and 'AuxBattleCoinFlip_Request' in src
assert 'aux_darkfire_generate_side' in src and 'rand()%100<60' in src
assert 'AUX_HOOK(Duel_CalcCardStats,aux_calc_stats,original_calc_stats)' in src
assert 'int equip_phase=phase==7 && sub==4' in src
assert 'bw_original_defmod' in src
print('Beta21: current card owners, whitelist, runtime hooks and test coverage: PASS')

assert by_id[318]["effect"] == 303
assert manifest["equips"] == [{"card":318,"bonus":0}]

# Beta25: a private fusion lilac ramp without modifying the ritual blue slot.
assert manifest['palette_ramps']=={'7':'#D78BE8'}
assert all('{f8 0A 02}<Fusion Effect>' not in c.get('description','') for c in manifest['cards'])
print('Beta25: independent lilac fusion heading and white inactive titles: PASS')
