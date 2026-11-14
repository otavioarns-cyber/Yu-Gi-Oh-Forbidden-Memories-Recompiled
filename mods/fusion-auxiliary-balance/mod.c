#define D_8009B178_AS_SIDE_ARRAY
#define D_8009B170_AS_SIDE_ARRAY
#include "pc/mods/modapi.h"
#include "game/card_constants.h"
#include "game/duel_card.h"
#include "game/duel_hand.h"
#include "game/duel_scene_hand_actions.h"
#include "game/duel_selection_layout.h"
#include "game/input.h"
#include "game/sound.h"
#include "game/duel_effect_request.h"
#include "game/duel_grid.h"
#include "game/duel_scene_state.h"
#include "game/duel_scene_battle.h"
#include "game/duel_battle_stats.h"
#include "game/duel_scene_card_placement.h"
#include "game/duel_scene_callbacks.h"
#include "game/duel_scene_field_actions.h"
#include "game/duel_scene_turn_switch.h"
#include "game/duel_magic_effect_dispatch.h"
#define DUEL_SCREEN_TABLES_TYPED_POSITIONS
#include "game/duel_screen_tables.h"
#include "game/display_object_core.h"
#include "game/duel_scene_update.h"
#include "game/display_object_work_slots.h"
#include "game/main_modes.h"
#define D_8009B208_AS_REPLAY_BYTES
#include "game/duel_scene_resume.h"
#define D_8009B360_AS_SIDE_ARRAY
#include "game/duel_side_state.h"
#include "game/duel_card_record_lifecycle.h"
#include "game/duel_apply_card_object_flags.h"
#include "game/duel_card_checks.h"
#include "game/func_8001D240.h"
#include "game/duel_card_effects.h"
#include "game/duel_action_lock.h"
#include "game/duel_get_base_card_stat.h"
#include "game/ai.h"
#include "game/ai_script_commands.h"
#include "pc/cards/cards.h"
#include "pc/cards/tables.h"
#include "pc/cards/stars.h"
#include "game/duel_effect_resource_record.h"
#include "pc/tower/tower.h"
#include "game/duel_effect.h"
#include "pc/cards/monster_effects_duel.h"
#include "psyq/libgte.h"
#include "psyq/libgpu.h"
#define D_80177EA4_VISIBLE
#include "unmatched.h"
#include "fusion_round3_rules.h"
#include "battle_stat_tween.h"
#include "skull_monster.h"
#include "logic.h"
#define DUEL_TERRAIN_AS_ARRAY
#include "game/duel_terrain_boost.h"
#include "pc/render/texture_dump.h"
#include "game/duel_deck_card_data.h"
#include "game/duel_deck_card.h"
#include "pc/cards/disc_art.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "game/duel_card_layout.h"
#define DUEL_CARD_VIEWER_ADDRESS_ALIASES
#include "game/duel_card_viewer.h"
#include "game/duel_effect.h"
#include "game/duel_field_display_objects.h"
#include "game/duel_card_object_helpers.h"
#include "game/func_80035E20.h"
#include "game/func_80036C14.h"
#include "pc/cards/type_icons.h"
#include "effect_title_visual.h"
#include "effect_title_instance.h"

/* Independent mod. No changes to rituals, drops, opponent deck weights,
   or field types. Specific recipes are added; only Darkfire and Twin-headed
   have their old result recipes globally removed. */
#define AUX_MONSTER_START 5
#define AUX_MONSTER_END 10
#define AUX_SIDE_STRIDE 15

/* Optional cross-mod ABI. This provider has no dependency on the Ritual or
   Time Wizard controllers and is read only at their resolution points. */
typedef struct { unsigned api; int (*eligible)(int side); } AuxEncoreV1;
static int aux_encore_eligible(int side);
static const AuxEncoreV1 aux_encore_api={1,aux_encore_eligible};
static void (*original_heal)(void);
static s32 (*original_calc_stats)(DuelCardRecord *);
static void (*original_turn_switch)(void);

static void (*original_rotate)(DisplayObject *);
static void (*original_remove)(DuelCardRecord *);
static void (*original_card_placement)(void);
static void (*original_card_finalize)(void);
static s32 (*original_card_effect_update)(void);
static void (*original_field_actions)(void);
static s32 (*original_fusion)(s32,s32);
static void (*original_battle)(void);
static void (*original_scene)(void);
static void (*original_aux_viewer_draw)(DisplayObject *,GsOT *);
static void (*original_append_text_entry)(DuelEffectChannel *,s32);
static void (*original_aux_viewer_info)(DuelFieldDisplaySource *,s32);
static void (*original_hand_actions)(void);
static s32 (*original_update_requests)(void);
static void (*original_resume)(void);
static void (*original_restore_generated_card)(s32,s32);
static void (*original_board)(void);
static void (*original_raigeki)(void);
static void (*original_monster_removal)(void);
static void (*original_ai_combo)(void);
static void (*original_ai_evaluate)(void);
static s32 (*original_base_stat)(s32,s32);
static int removal_context;
static int mass_destruction_active;
static int mass_seiyaryu_present[2];
/* Retail board-destruction effects can move a card sprite even when the
   RemoveFromField hook correctly protects its field record. Snapshot the
   protected instances and rebuild ONLY their field sprites after the native
   FX requests finish, not while they are still animating. */
static u8 mass_visual_marked[30];
static s16 mass_visual_id[30];
static void *mass_visual_data[30];
static int mass_visual_repair_pending;
static void aux_repair_mass_visuals(void);
/* One-heal-per-field-presence state for Mystical Elf. Record identity is
   bound to BOTH card ID and deck data and cleared when removed. Flipping
   face-down and face-up never grants an additional heal. */
static u8 aux_elf_healed[30];
static s16 aux_elf_bound_id[30];
static void *aux_elf_bound_data[30];
static void aux_elf_check_healing(void);
static void aux_dark_elf_react_to_healing(int side,int actually_healed);
static int ai_scoring;
static int battle_protection_pending;
static int battle_lp_before[2],battle_expected_loss[2],battle_lp_reported;
/* Origin tracking: a successful real in-duel combination must contain
   Metalmorph. Checking card ID alone would grant free immunity to decks. */
static int metal_placement_check;
static int metal_pending_result;
static int metal_pending_side;
static u8 metal_immune[30];
static s16 metal_bound_id[30];
static void *metal_bound_data[30];
/* Only the current opponent's actual Magic/Equip effect can be prevented;
   trap triggers and monster effects deliberately do not enable this gate. */
static int kamion_cpu_learned[2];
static int metal_spell_context;
static int metal_spell_side;
/* Independent provenance for selected Fusion Effect monsters. A direct
   copy from the deck never qualifies unless debug bypass is enabled. */
static int fusion_pending_result, fusion_pending_side;
static int placement_was_fusion;
static u8 fusion_origin[30];
static s16 fusion_bound_id[30];
static void *fusion_bound_data[30];
/* One scripted native effect at a time; up to five targets per B. Skull. */
#define AUX_FX_CAP 16
#define AUX_FX_LP 1
#define AUX_FX_STAT 2
typedef struct {
    int kind,side,amount,spell,record,card_id;
    void *card_data;
} AuxFx;
static AuxFx aux_fx_queue[AUX_FX_CAP];
static int aux_fx_count,aux_fx_playing;
static AuxFx aux_fx_current;
static int battle_special_attacker, battle_attack_won, battle_pierce_amount;
static void aux_queue_lp(int side,int damage,int spell,int source);
static void aux_queue_skull_effect(int side);
static void aux_finish_attack_effect(void);
/* Each live Darkfire contributes one FULL BASE-damage hit. The retail
   magic resolves normally first; later requests replay its original impact
   presentation sequentially, applying precisely one extra base hit each.
   Avoid recursive DAMAGE events and never let the duel advance during replay. */
static int bonus_hits_queued;
static int bonus_hit_playing;
static int bonus_hit_side;
static int bonus_hit_amount;
static int bonus_hit_spell;
/* Per-instance observed Equip totals, captured only after native equips
   complete. Fusion Kamion adds the same amount one additional time. */
typedef struct { int card; void *data; int atk,def,boosted; } AuxEquipLedger;
static AuxEquipLedger aux_equip_ledger[30];
static int aux_equip_tracking,aux_equip_slot,aux_equip_attack_before,aux_equip_defense_before;
static int aux_equip_seen_attack,aux_equip_seen_defense;
/* The extra applied during the previous frame of the native Equip climb.
   Rewind it before the native controller computes the next frame; otherwise
   the animation could add our previous bonus repeatedly. */
static int aux_equip_extra_attack,aux_equip_extra_defense;
static void *aux_equip_data;
static int aux_equip_id;
static void aux_kamion_refresh(void);
static void aux_musician_attack_only(void);
static void aux_equip_ensure(int i);
static void aux_darkfire_generate_side(int side,const char *reason);
static void r3_queue_rebirth_fire(int slot,int id);
static int aux_generated_magic_lba=-2;
static unsigned char aux_generated_magic_sector[2048];
static unsigned char aux_generated_magic_cache[30][DUEL_CARD_DATA_BLOCK_SIZE];
static DuelDeckCardRecord aux_generated_magic_data[30];
static unsigned char aux_generated_magic_present[30];
static int aux_darkfire_generated_turn[2];
/* Battle Warrior coin and temporary battle-only ATK adjustment. */
static AuxBattleStatTween aux_stat_tween;
static int bw_coin_pending,bw_coin_ready,bw_coin_used,bw_coin_side,bw_record,bw_target;
static int bw_success,bw_swapped,bw_coin_started;
static int bw_coin_result;
static int bw_original_mod[2],bw_original_defmod[2];
static int bw_original_replay_mod[2],bw_original_replay_defmod[2],bw_swapped_id[2];
static void *bw_swapped_data[2];
static void aux_bw_restore(void);
static void aux_bw_prepare(void);

/* Battle entrants have their OCCUPIED bit cleared by DuelScene_UpdateFieldActions.
   Capture the card identity before battle, then guard the 3D death flag and
   the later replay independently of the temporarily lifted field record. */
static int battle_record[2]={-1,-1};
static int battle_card[2];
static int battle_guarded[2];
static u16 battle_saved_flags[2];
static void *battle_saved_data[2];
/* A saved native 2D death request must be redirected BEFORE state 10 makes
   the fire effect, while the 3D death reaction is disabled after state 5.
   At state 11 the native path reconstructs surviving cards; we only need a
   manual fallback for losers whose panel has already disappeared. */
static unsigned battle_2d_skipped;
static unsigned battle_restore;
static unsigned battle_3d_suppressed;
/* The battle controller temporarily lifts monster records, then reconstructs
   them on returning to the field. Defer Wall Shadow's stance change until
   AFTER the native restore (including its saved flags) has finished. */
static int wall_attacker_record=-1;
static void *wall_attacker_data;
static int wall_attack_resolved;
static int wall_attack_negated;
static void aux_wall_return_to_defense(void);
static const MemoriesModHost *aux_host;
/* Card identity is required for the optional secondary-type delta. Native
 * generation bypasses Duel_SetupCardRecord, so ask the same public provider. */
#define AUX_CARD_TERRAIN(id) aux_terrain_bonus(id)
static int aux_terrain_bonus(int id)
{
    typedef int(*Delta)(int,int,int);
    int type=Cards_Type(id);
    Delta delta=aux_host&&aux_host->find?(Delta)aux_host->find(aux_host,
        "field-effects-secondary-types:terrain-card-delta-v1"):0;
    return Duel_GetTerrainBoost(type)+(delta?delta(id,type,gDuel_bTerrain[0]):0);
}

static int aux_owner_applied(void);
/* Self-contained normal Battle Warrior coin, including the original artwork. */
#include "battle_coin.h"
static void aux_coin_frame(void) { if(bwcoin.active) bwcoin_frame(); }
static void aux_coin_overlay(void) { if(bwcoin.active) bwcoin_overlay(); }
static unsigned aux_coin_signature(void) { return bwcoin.active ? (unsigned)(1+bwcoin.shown*251+bwcoin.lift*1777+bwcoin.result_up*7799) : 0; }
static void aux_coin_shutdown(void) { bwcoin_free(); }
static int aux_battle_guarded_record(int record);
static void aux_normalize_musician_battle_damage(void)
{
    int side;
    if(!battle_lp_reported)return;
    for(side=0;side<2;side++)if(battle_expected_loss[side]>0) {
        int before=battle_lp_before[side],after=D_800E9FF0[side].life_points.unsigned_value;
        int expected=before-battle_expected_loss[side];
        if(expected<0)expected=0;
        if(after<expected) {
            D_800E9FF0[side].life_points.unsigned_value=(u16)expected;
            if(aux_host&&aux_host->log)
                aux_host->log(aux_host,"[Fusion Auxiliary Balance] Musician battle LP normalized %d to %d",after,expected);
        }
    }
    battle_lp_reported=0;
}
static void aux_battle_finish(void);

static int r2_setting_enabled(int id)
{
    /* Shared toggle lookup for all Fusion Effects (rounds 02 and 03). */
    static const struct {int id;const char *key;} effects[]={
        {613,"thtd_discharge"},{707,"skull_knight_army"},{370,"tank_siege"},{69,"thousand_flames"},
        {385,"bickuribox_trick"},{427,"kaiser_protection"},{467,"sunbird_feathers"},{442,"aqua_whirlpool"},
        {509,"bracchio_forest"},{471,"soul_hunter_feast"},{564,"mammoth_curse"},{223,"ocean_snake_tide"},
        {465,"eagle_watch"},{531,"sand_blessing"},{572,"empress_protection"},{565,"shark_blood"},
        {521,"skullbird_breath"},{618,"warrior_memories"},{437,"deepsea_ambush"},
        {473,"sparrow_beacon"},{482,"pragtical_hunt"},{389,"giltia_lance"},
        {639,"bugroth_adaptation"},{15,"flame_sword"},{545,"skelgon_fury"},
        {617,"marine_instinct"},{230,"rare_fish_hunt"},{86,"barox_twist"},
        {294,"dragoness_armor"},{133,"charubin_spark"},{215,"flame_ghost_burst"},
        {594,"rose_thorns"},{533,"kwagar_shell"},{43,"karbonala_inversion"},
        {272,"mavelus_rebirth"},{30,"zombie_sacrifice"},
        {487,"flower_wolf_camouflage"},{458,"kaminari_paralysis"}};
    unsigned i;for(i=0;i<sizeof(effects)/sizeof(effects[0]);i++)if(id==effects[i].id)
        return !aux_host||!aux_host->setting||aux_host->setting(aux_host,effects[i].key,1)!=0;
    return 1;
}
static int aux_monster_slot(int index)
{
    int zone;
    if(index<0 || index>=30) return 0;
    zone=index%AUX_SIDE_STRIDE;
    return zone>=AUX_MONSTER_START && zone<AUX_MONSTER_END;
}

static int aux_live(int index,int id)
{
    const DuelCardRecord *r;
    if(!aux_monster_slot(index)) return 0;
    r=&D_801A7AD8[index];
    return (r->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           !(r->flags&DUEL_CARD_FLAG_FACE_DOWN) && r->card_id==id;
}

/* Settings > Mods: unchecked by default. Switching it ON skips summon
   origin requirements for BOTH categories, but never grants the effects to
   arbitrary cards. Read dynamically so the Settings checkbox is effective. */
static int aux_ignore_conditions(void)
{
    return aux_host && aux_host->setting &&
           aux_host->setting(aux_host,"ignore_effect_conditions",0)!=0;
}

static unsigned aux_unlock_lease(int slot);

static int aux_fusion_special(int record,int lifted)
{
    const DuelCardRecord *r;
    int origin;
    if(!aux_monster_slot(record)) return 0;
    r=&D_801A7AD8[record];
    if(!lifted && !(r->flags & DUEL_CARD_FLAG_OCCUPIED)) return 0;
    if(r->flags & DUEL_CARD_FLAG_FACE_DOWN) return 0;
    origin=fusion_origin[record] && fusion_bound_id[record]==r->card_id &&
           fusion_bound_data[record]==r->data;
    return r2_setting_enabled(r->card_id) && aux_condition_passes(aux_is_fusion_family(r->card_id),
                                origin || aux_unlock_lease(record),aux_ignore_conditions());
}

static int aux_metal_protected(int record)
{
    const DuelCardRecord *r;
    if(!aux_monster_slot(record)) return 0;
    r=&D_801A7AD8[record];
    return (r->flags & DUEL_CARD_FLAG_OCCUPIED) &&
           !(r->flags & DUEL_CARD_FLAG_FACE_DOWN) &&
           aux_condition_passes(aux_is_metal_family(r->card_id),
               (metal_immune[record] && r->card_id==metal_bound_id[record] &&
               r->data==metal_bound_data[record]) || aux_unlock_lease(record),aux_ignore_conditions());
}

static int aux_opponent_magic_immune(int record)
{
    return metal_spell_context && metal_spell_side>=0 &&
           metal_spell_side<2 && record/AUX_SIDE_STRIDE!=(metal_spell_side) &&
           aux_metal_protected(record);
}

static int aux_has_seiyaryu(int side)
{
    int i;
    for(i=side*AUX_SIDE_STRIDE+AUX_MONSTER_START;
        i<side*AUX_SIDE_STRIDE+AUX_MONSTER_END;i++)
        if(aux_live(i,AUX_SEIYARYU)) return 1;
    return 0;
}

static int aux_count_darkfire(int side)
{
    int i, count=0;
    for(i=side*AUX_SIDE_STRIDE+AUX_MONSTER_START;
        i<side*AUX_SIDE_STRIDE+AUX_MONSTER_END;i++)
        if(aux_live(i,AUX_DARKFIRE_DRAGON)) count++;
    return count;
}

static int aux_encore_eligible(int side)
{
    int i;
    if(!aux_owner_applied() || side<0||side>1)return 0;
    for(i=side*AUX_SIDE_STRIDE+AUX_MONSTER_START;
        i<side*AUX_SIDE_STRIDE+AUX_MONSTER_END;i++)
        if(aux_live(i,AUX_MUSICIAN_KING))return 1;
    return 0;
}
static int aux_fused_kamion(int side)
{
    int i;
    for(i=side*AUX_SIDE_STRIDE+AUX_MONSTER_START;
        i<side*AUX_SIDE_STRIDE+AUX_MONSTER_END;i++)
        if(D_801A7AD8[i].card_id==AUX_KAMION_WIZARD &&
           aux_fusion_special(i,0))return 1;
    return 0;
}
/* During an Equip animation the actual target record is lifted: its
   OCCUPIED bit can temporarily vanish, including when the target is the
   Mystical Kamion itself. Provenance (card + data) remains available. */
static int aux_fused_kamion_during_equip(int side,int target)
{
    if(aux_fused_kamion(side))return 1;
    if(aux_monster_slot(target) && target/AUX_SIDE_STRIDE==side &&
       D_801A7AD8[target].card_id==AUX_KAMION_WIZARD &&
       aux_fusion_special(target,1))return 1;
    return 0;
}
static int aux_has_kamion(int side)
{
    int i;
    for(i=side*AUX_SIDE_STRIDE+AUX_MONSTER_START;
        i<side*AUX_SIDE_STRIDE+AUX_MONSTER_END;i++)
        if(aux_live(i,AUX_KAMION_WIZARD))return 1;
    return 0;
}
static int aux_has_darkfire(int side)
{
    return aux_count_darkfire(side)>0;
}

static int aux_dragon_protected(int index)
{
    DuelCardRecord *r;
    int id,side;
    if(!aux_monster_slot(index)) return 0;
    r=&D_801A7AD8[index];
    if(!(r->flags&DUEL_CARD_FLAG_OCCUPIED)) return 0;
    id=r->card_id;
    if(!Cards_Valid(id)) return 0;
    side=index/AUX_SIDE_STRIDE;
    return aux_seiyaryu_eligible(id,Cards_Type(id),CARD_TYPE_DRAGON,Cards_Level(id),
           aux_has_seiyaryu(side) ||
           (removal_context && mass_destruction_active && mass_seiyaryu_present[side]));
}

static int aux_cpu_planning(void)
{
    return D_8009B1D5<2 && ai_scoring &&
           ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK)==4 ||
            (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK)==5);
}

static int aux_cpu_hand_damage_spells(void)
{
    int i,n=0;
    for(i=AI_SLOT_OWN_HAND_FIRST;i<AI_SLOT_OWN_HAND_FIRST+5;i++) {
        int id=gDuel_aActiveCards[i].card_id;
        if(id>=AUX_SPARKS && id<=AUX_LAST_DIRECT_MAGIC) n++;
    }
    return n;
}

/* Protect Darkfire's field instance when the CPU can profit from its burn
   synergy. A pair checker receives IDs, not individual instances: while a
   valuable face-up Darkfire exists, the CPU conservatively avoids fusions
   naming that same card. Never block player-chosen fusions. */
static int aux_cpu_keep_darkfire(int a,int b)
{
    int i,phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
    if(D_8009B1D5!=1 || D_8009B360[1]<0 ||
       (phase!=4 && phase!=5) || !aux_has_darkfire(1) ||
       (Cards_BaseId(a)!=AUX_DARKFIRE_DRAGON &&
        Cards_BaseId(b)!=AUX_DARKFIRE_DRAGON))return 0;
    if(aux_live(20,357)||aux_live(21,357)||aux_live(22,357)||
       aux_live(23,357)||aux_live(24,357)||aux_cpu_hand_damage_spells()>0)return 1;
    for(i=25;i<30;i++) {
        const DuelCardRecord *r=&D_801A7AD8[i];
        if((r->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           r->card_id>=AUX_SPARKS && r->card_id<=AUX_LAST_DIRECT_MAGIC)
            return 1;
    }
    return 0;
}

static int aux_cpu_covered_dragons(void)
{
    int i,n=0;
    for(i=20;i<25;i++) { /* CPU side monster zone: 15+5..15+9 */
        DuelCardRecord *r=&D_801A7AD8[i];
        if((r->flags&DUEL_CARD_FLAG_OCCUPIED) && Cards_Valid(r->card_id) &&
           r->card_id!=AUX_SEIYARYU &&
           Cards_Type(r->card_id)==CARD_TYPE_DRAGON && Cards_Level(r->card_id)<=6)
            n++;
    }
    return n;
}

/* AI planner uses this stat only to RANK possible fusion products. Boost
   strategic value within its recursive search, never on the game board. */
static int ea_rank_combo(int);static void ea_start_combo(void);
static s32 aux_base_stat(s32 card_id,s32 defense)
{
    s32 base=original_base_stat(card_id,defense);
    int id=Cards_BaseId(card_id);
    if(aux_cpu_planning())
        base+=aux_host->setting(aux_host,"effect_ai",1)?ea_rank_combo(id):aux_ai_extra_value(id,aux_cpu_covered_dragons(),aux_cpu_hand_damage_spells());
    return base;
}

static void aux_find_combo(void)
{
    int old=ai_scoring;
    ai_scoring=D_8009B1D5<2&&D_8009B360[D_8009B1D5]>=0;if(ai_scoring)ea_start_combo();
    original_ai_combo();
    ai_scoring=old;
}
static void aux_evaluate_combo(void)
{
    int old=ai_scoring;
    ai_scoring=D_8009B1D5<2&&D_8009B360[D_8009B1D5]>=0;if(ai_scoring)ea_start_combo();
    original_ai_evaluate();
    ai_scoring=old;
}

/* Keep the original field info hook for compatibility with other UI
 * systems. It is NOT used to decide effect-title activation: that old
 * last-slot cache leaked an equipped monster's state into hand copies. */
static void aux_viewer_info(DuelFieldDisplaySource *source,s32 index)
{
    original_aux_viewer_info(source,index);
}
static int r3_flower_untargetable(int slot);
static void r3_kaminari_scan(void);
static int r3_kaminari_used(int slot);
static int r3_kaminari_attacks_blocked(int side);
static int r3_find_attackable_enemy(int side,int preferred);
static int r3_grid_opponent_target(int side,int col);
static void r3_kaminari_forget(int slot);
static int aux_viewer_title_active(int id,int slot,int title)
{
    const DuelCardRecord *r;
    if(slot<0 || !aux_monster_slot(slot))return title==5; /* Generic Effect stays available; conditional origins NEVER borrow another copy. */
    r=&D_801A7AD8[slot];
    if(!(r->flags&DUEL_CARD_FLAG_OCCUPIED) ||
       (r->flags&DUEL_CARD_FLAG_FACE_DOWN))return 0;
    if(title==7) {
        if(!aux_fusion_special(slot,0))return 0;
        /* Immediate-on-summon effects have already fired on commitment. */
        if(id==AUX_METEOR_B_DRAGON || id==AUX_B_SKULL_DRAGON)return 0;
        if(id==458)return !r3_kaminari_used(slot);
        if(id==487)return r3_flower_untargetable(slot);
        return 1;
    }
    if(title==4)return aux_metal_protected(slot);
    if(title==5) {
        if(id==AUX_MYSTICAL_ELF && aux_elf_bound_id[slot]==id &&
           aux_elf_bound_data[slot]==r->data && aux_elf_healed[slot])return 0;
        if(id==AUX_BATTLE_WARRIOR && bw_coin_used && bw_record==slot)return 0;
        return 1;
    }
    return 1;
}
static void aux_viewer_draw(DisplayObject *obj,GsOT *ot)
{
    FmrTitleTint tint={0};
    DuelEffectChannel *box=fmr_title_draw_channel(obj,D_8009B250);
    int id=gDuel_wViewerCardID,slot=-1,duel=fmr_title_is_live_duel();
    if(obj && ot && box &&
       (id==AUX_DARKFIRE_DRAGON || id==AUX_WALL_SHADOW ||
        id==AUX_MUSICIAN_KING || id==AUX_KAMION_WIZARD ||
        id==AUX_MYSTICAL_ELF || id==AUX_BATTLE_WARRIOR ||
        id==AUX_SEIYARYU || id==AUX_DARK_ELF ||
        id==AUX_FUSIONIST || id==AUX_METEOR_B_DRAGON ||
        id==AUX_B_SKULL_DRAGON || id==AUX_GAIA_DRAGON_CHAMPION ||
        id==AUX_RABID_HORSEMAN || r2_member(id) || r3_member(id) || aux_is_metal_family(id))) {
        if(duel)slot=fmr_title_selected_field_record(id);
        if(id==AUX_DARKFIRE_DRAGON || id==AUX_WALL_SHADOW ||
           id==AUX_MUSICIAN_KING || id==AUX_KAMION_WIZARD ||
           id==AUX_MYSTICAL_ELF || id==AUX_BATTLE_WARRIOR ||
           id==AUX_SEIYARYU || id==AUX_DARK_ELF || id==AUX_FUSIONIST)
            fmr_title_tint_effect(&tint,box,"<Effect>",5,!duel||aux_viewer_title_active(id,slot,5));
        if(id==AUX_DARKFIRE_DRAGON || id==AUX_MUSICIAN_KING ||
           id==AUX_KAMION_WIZARD || id==AUX_FUSIONIST ||
           id==AUX_METEOR_B_DRAGON || id==AUX_B_SKULL_DRAGON ||
           id==AUX_GAIA_DRAGON_CHAMPION || id==AUX_RABID_HORSEMAN || r2_member(id) || r3_member(id))
            fmr_title_tint_effect(&tint,box,"<Fusion Effect>",7,!duel||aux_viewer_title_active(id,slot,7));
        if(aux_is_metal_family(id))
            fmr_title_tint_effect(&tint,box,"<Metalmorph Effect>",4,!duel||aux_viewer_title_active(id,slot,4));
    }
    original_aux_viewer_draw(obj,ot);
    fmr_title_restore(&tint);
}

/* The FM Editor's fusions: [{"remove":168},{"remove":613}] stop the retail
   recipes for both targets, before player/AI fusion evaluation. Explicit
   pairs live in mod.json. This callback adds only the stat/type conditional
   Two-mouth Darkruler recipe, evaluated with the current card properties.
   MEMORIES_AFTER is OBSERVATION ONLY and CANNOT change the fusion result. */
static int aux_authorized_fusion(int card_a,int card_b)
{
    int a=Cards_BaseId(card_a),b=Cards_BaseId(card_b),partner=0;
    int type=-1,atk=0;
    if(!Cards_Valid(card_a) || !Cards_Valid(card_b)) return 0;
    if(a==AUX_TWO_MOUTH_DARKRULER) partner=card_b;
    else if(b==AUX_TWO_MOUTH_DARKRULER) partner=card_a;
    if(!partner && (a==AUX_TIME_WIZARD || b==AUX_TIME_WIZARD))
        partner=a==AUX_TIME_WIZARD ? card_b : card_a;
    if(partner) {
        type=Cards_Type(partner);
        /* Printed/base ATK, not Field, equips, or AI virtual values. */
        atk=original_base_stat ? original_base_stat(partner,0) :
                              Duel_GetBaseCardStat(partner,0);
    }
    if(a==AUX_TIME_WIZARD || b==AUX_TIME_WIZARD)
    {
        int elf=aux_ancient_elf_fusion(a,b,atk);
        if(elf) return elf;
    }
    return aux_new_fusion(a,b,type,atk);
}
static void aux_fusion(MemoriesModEvent *event)
{
    int a,b,result;
    if(event->phase!=MEMORIES_BEFORE || event->handled) return;
    a=Cards_BaseId(event->a);
    b=Cards_BaseId(event->b);
    if(!Cards_Valid(event->a) || !Cards_Valid(event->b)) return;
    if(aux_cpu_keep_darkfire(event->a,event->b)) {
        event->result=0;
        event->handled=1;
        return;
    }
    if(aux_cpu_planning() && (a==AUX_SEIYARYU ||
       b==AUX_SEIYARYU) && aux_has_seiyaryu(1) &&
       aux_cpu_covered_dragons()>0) {
        /* Do not sacrifice a Gaia shielding friendly Dragons in AI search. */
        event->result=0;
        event->handled=1;
        return;
    }
    result=aux_authorized_fusion(event->a,event->b);
    if(result) {
        /* Only the Darkfire recipe consumes Sparks/Hinotama. Retain these
           direct-damage spells if the CPU already owns a Darkfire. */
        if(result==AUX_DARKFIRE_DRAGON && aux_cpu_planning() &&
           aux_has_darkfire(1)) {
            event->result=0;
            event->handled=1;
            return;
        }
        event->result=result;
        event->handled=1;
    }
}

/* The manifest handles retail (FM Editor) removals. This final guard catches
   a custom FUSION event supplied by another enabled code mod; AFTER events
   are read-only, so a guest-function wrapper is the correct final check. */
#include "fusionist_runtime.h"
#include "fusion_round2_runtime.h"
#include "secondary_dragon_types.h"
#include "fusion_round3_runtime.h"
#include "generated_card_restore.h"

/* DuelScene_UpdateCardPlacement is the ONLY live card combination
   animation, as distinct from speculative card-pair checks by the AI.
   The hooked fusion resolver captures whether Metalmorph was really used.
   Record that outcome only when the subsequent summon commits. */
/* Native Equip animation calls this GUEST function after updating the
   monster record but before publishing the new ATK/DEF on the screen.
   That is the safe point to double the actual animated stat climb, on both
   in-hand combinations and already-existing field monsters. Host port
   functions such as Tables_EquipBonuses are NOT patchable with ModHook. */
static s32 aux_calc_stats(DuelCardRecord *r)
{
    /* Native placement can recalculate its equip climb on successive frames.
       Never modify the card *inside* this calculator: doing so there lets
       the same frame call it again and may incorrectly stack the bonus. */
    s32 result=r3_stats(r,r2_stats(r,original_calc_stats(r)));
    if(aux_stat_tween.show_initial) {
        int p;
        for(p=0;p<2;p++)if(aux_monster_slot(battle_record[p]) &&
            r==&D_801A7AD8[battle_record[p]] &&
            r->card_id==battle_card[p] && r->data==battle_saved_data[p]) {
            return (s32)(((unsigned)aux_stat_tween.initial[p][1]<<16)|
                         (unsigned)aux_stat_tween.initial[p][0]);
        }
    }
    return result;
}

static void aux_card_placement(void)
{
    int old=metal_placement_check;
    /* The fresh placement begins with the scene initialization bit unset. */
    if(!(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED)) {
        int slot=D_800E9EF0[0]?(int)D_800E9EF0[0]->field_6A:-1;
        metal_pending_result=fusion_pending_result=placement_was_fusion=0;
        aux_chain_active=slot>=0 && slot<30;
        aux_chain=aux_chain_from_record(slot,aux_chain_active?D_801A7AD8[slot].card_id:0);
    }
    metal_placement_check=1;
    {
        int phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
        int sub=D_8009B174&15;
        /* D_8009B206 (the visual effect id) is not reliable until the
           Equip presentation reaches its later stage. The placement result
           is authoritative: nonzero WITHOUT fusion bit means Equip. */
        int equip_phase=phase==7 && sub==4 && D_8009B150!=0 &&
            !(D_8009B150&0x8000);
        int before_atk[5]={0},before_def[5]={0},before_id[5]={0};
        void *before_data[5]={0};
        int j,side=D_8009B1D5;
        if(equip_phase && side>=0 && side<2 && !aux_equip_tracking)
            for(j=0;j<5;j++) {
                DuelCardRecord *r=&D_801A7AD8[side*15+5+j];
                before_atk[j]=r->stat_modifier;
                before_def[j]=r->stat_modifier+r->defense_modifier;
                before_id[j]=r->card_id;
                before_data[j]=r->data;
            }
        /* This hook is scene phase SEVEN (the CardPlacement callback),
           not scene phase eight (the separate finalize callback). That
           phase mismatch kept all earlier Kamion equip trackers inactive.
           Begin before native Equip's result-climb in substate 4;
           substate 5 is only the card's Equip presentation and is too late.
           Tracking the total change per scene frame works for a card
           fused with Equip in-hand as well as on an existing monster. */
        if(equip_phase && !aux_equip_tracking && D_800E9EF0[0]) {
            int i=(int)D_800E9EF0[0]->field_6A;
            if(aux_monster_slot(i)) {
                DuelCardRecord *r=&D_801A7AD8[i];
                aux_equip_ensure(i);
                aux_equip_tracking=1;aux_equip_slot=i;
                aux_equip_attack_before=r->stat_modifier;
                aux_equip_defense_before=r->stat_modifier+r->defense_modifier;
                aux_equip_seen_attack=aux_equip_seen_defense=0;
                aux_equip_extra_attack=aux_equip_extra_defense=0;
                aux_equip_id=r->card_id;aux_equip_data=r->data;
            }
        }
        if(aux_equip_tracking && (aux_equip_extra_attack || aux_equip_extra_defense)) {
            int i=aux_equip_slot;
            DuelCardRecord *r=&D_801A7AD8[i];
            if(r->card_id==aux_equip_id && r->data==aux_equip_data) {
                r->stat_modifier=(s16)((int)r->stat_modifier-aux_equip_extra_attack);
                r->defense_modifier=(s16)((int)r->defense_modifier-
                    (aux_equip_extra_defense-aux_equip_extra_attack));
            }
            aux_equip_extra_attack=aux_equip_extra_defense=0;
        }
        original_card_placement();
        /* Hand + Equip combinations sometimes stage a hand object in slot
           zero instead of the target monster object. Identify the actual
           record by the first native stat climb on this same tick. */
        if(equip_phase && !aux_equip_tracking && side>=0 && side<2) {
            for(j=0;j<5;j++) {
                int i=side*15+5+j;
                DuelCardRecord *r=&D_801A7AD8[i];
                if(r->card_id==before_id[j] && r->data==before_data[j] &&
                   ((int)r->stat_modifier>before_atk[j] ||
                    (int)r->stat_modifier+r->defense_modifier>before_def[j])) {
                    aux_equip_ensure(i);
                    aux_equip_tracking=1;aux_equip_slot=i;
                    aux_equip_attack_before=before_atk[j];
                    aux_equip_defense_before=before_def[j];
                    aux_equip_seen_attack=aux_equip_seen_defense=0;
                aux_equip_extra_attack=aux_equip_extra_defense=0;
                    aux_equip_id=r->card_id;aux_equip_data=r->data;
                    break;
                }
            }
        }
        if(aux_equip_tracking) {
            int i=aux_equip_slot;
            DuelCardRecord *r=&D_801A7AD8[i];
            if(r->card_id==aux_equip_id && r->data==aux_equip_data) {
                int a=(int)r->stat_modifier-aux_equip_attack_before;
                int d=(int)r->stat_modifier+r->defense_modifier-aux_equip_defense_before;
                int boost=aux_fused_kamion_during_equip(i/AUX_SIDE_STRIDE,i);
                /* Work on the exact stat change the native equip just made,
                   not on a delayed visual callback. An equip climb is
                   reconstructed from its pre-equip baseline each frame.
                   Double only the newly-applied ATK/DEF and preserve any
                   unequal DEF bonus (such as unusual external Equips). */
                if(a>=0 && d>=0 && (a||d)) {
                    aux_equip_seen_attack=a;
                    aux_equip_seen_defense=d;
                    if(boost) {
                        int atk=(int)r->stat_modifier+a;
                        int def=(int)r->defense_modifier+d-a;
                        if(atk>32767)atk=32767;
                        if(atk< -32768)atk=-32768;
                        if(def>32767)def=32767;
                        if(def< -32768)def=-32768;
                        aux_equip_extra_attack=atk-(int)r->stat_modifier;
                        aux_equip_extra_defense=(atk+def)-
                            ((int)r->stat_modifier+(int)r->defense_modifier);
                        r->stat_modifier=(s16)atk;
                        r->defense_modifier=(s16)def;
                        if(aux_host && aux_host->log && aux_host->log_enabled &&
                           aux_host->log_enabled(aux_host))
                            aux_host->log(aux_host,
                              "[Fusion Auxiliary Balance] Kamion equip x2: slot=%d native ATK=%d DEF=%d",i,a,d);
                    }
                }
            }
            if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=7 ||
               (D_8009B174&15)!=4) {
                if(r->card_id==aux_equip_id && r->data==aux_equip_data &&
                   (r->flags&DUEL_CARD_FLAG_OCCUPIED)) {
                    AuxEquipLedger *e=&aux_equip_ledger[i];
                    e->atk+=aux_equip_seen_attack;
                    e->def+=aux_equip_seen_defense;
                    e->boosted=aux_fused_kamion_during_equip(i/AUX_SIDE_STRIDE,i);
                }
                aux_equip_tracking=0;
                aux_equip_extra_attack=aux_equip_extra_defense=0;
            }
        }
    }
    metal_placement_check=old;
}

/* A placement finalizes ONLY when its actual field record is committed.
   Equip animations can also visit this phase; they must not clear an
   existing monster's provenance or trigger an on-summon Fusion Effect. */
static void aux_card_finalize(void)
{
    int is_finalize=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==8 &&
                    (D_8009B174&15)==4 && !(D_8009B174&0x40);
    int is_equip=(gDuel_wSceneStateFlags & 0x4000)!=0;
    int index=(int)D_8009B19C;
    int was_committed=0;
    original_card_finalize();
    if(is_finalize && index>=0 && index<30 &&
       (D_8009B174&0x40) &&
       (D_801A7AD8[index].flags & DUEL_CARD_FLAG_OCCUPIED)) {
        DuelCardRecord *r=&D_801A7AD8[index];
        if(Cards_Valid(r->card_id) && Cards_Type(r->card_id)<CARD_TYPE_MAGIC) {
            if(aux_chain_active && aux_chain.id==r->card_id)aux_publish_chain();
            metal_immune[index]=(u8)(metal_pending_result &&
                metal_pending_side==index/AUX_SIDE_STRIDE &&
                r->card_id==metal_pending_result);
            metal_bound_id[index]=r->card_id;
            metal_bound_data[index]=r->data;
            fusion_origin[index]=(u8)(fusion_pending_result &&
                fusion_pending_side==index/AUX_SIDE_STRIDE &&
                r->card_id==fusion_pending_result);
            fusion_bound_id[index]=r->card_id;
            fusion_bound_data[index]=r->data;
            was_committed=1;
            if(aux_chain_active && aux_chain.id==r->card_id && aux_chain.egotist)
                aux_unlock_bind(index);
            if(aux_fusion_special(index,0) &&
               (aux_chain_active?(aux_chain.created || aux_chain.unlock_new):!is_equip)) {
                if(r->card_id==AUX_METEOR_B_DRAGON)
                    aux_queue_lp((index/AUX_SIDE_STRIDE)^1,1000,347,AUX_DIRECT_SOURCE_METEOR);
                else if(r->card_id==AUX_B_SKULL_DRAGON)
                    aux_queue_skull_effect(index/AUX_SIDE_STRIDE);
                else if(r->card_id==AUX_DARKFIRE_DRAGON)
                    aux_darkfire_generate_side(index/AUX_SIDE_STRIDE,"fusion summon");
            }
        }
    }
    if(was_committed) {
        aux_kamion_refresh();
        aux_musician_attack_only();
        if(aux_host && aux_host->log && (metal_pending_result || fusion_pending_result))
            aux_host->log(aux_host,
                "[Fusion Auxiliary Balance] summoned card=%d record=%d metal=%d fusion=%d",
                D_801A7AD8[index].card_id,index,metal_immune[index],fusion_origin[index]);
        metal_pending_result=fusion_pending_result=placement_was_fusion=0;
        aux_chain_active=0;
    }
}

/* All new, non-battle LP effects use the same Direct LP Damage registry
   as damage Magic Cards. Every Darkfire on the ACTING side queues exactly
   one extra original-size hit and its own native impact presentation.
   Keep the logical type as monster effect (no fake Magic DAMAGE event), so
   other mods can still distinguish the source from an actual Magic Card. */
static void aux_queue_lp(int side,int damage,int spell,int source)
{
    int extra,i,source_card;
    if(side<0 || side>1 || damage<=0)return;
    source_card=source==AUX_DIRECT_SOURCE_METEOR ? AUX_METEOR_B_DRAGON :
                source==AUX_DIRECT_SOURCE_GAIA ? AUX_GAIA_DRAGON_CHAMPION :
                source==AUX_DIRECT_SOURCE_DARK_ELF ? AUX_DARK_ELF : 0;
    /* source==0 is ordinary piercing BATTLE damage: explicitly excluded
       from Direct LP Damage and Darkfire amplification. */
    if(source!=0 && source!=AUX_DIRECT_SOURCE_DARK_ELF &&
       !aux_direct_lp_registered(source,source_card))return;
    extra=source==0 ? 0 :
          aux_direct_lp_extra_hits(source,source_card,aux_count_darkfire(side^1));
    for(i=0;i<=extra;i++) {
        AuxFx *fx;
        if(aux_fx_count>=AUX_FX_CAP) {
            /* Graceful overflow: no lost damage even if another mod floods
               the effect queue. Avoid a deadlock by resolving the tail. */
            int life=D_800E9FF0[side].life_points.unsigned_value;
            D_800E9FF0[side].life_points.unsigned_value=
                (u16)aux_damage_one_hit(life,damage*(extra-i+1));
            if(aux_host && aux_host->log)aux_host->log(aux_host,
                "[Fusion Auxiliary Balance] Direct LP overflow: %d hits applied without presentation",extra-i+1);
            break;
        }
        fx=&aux_fx_queue[aux_fx_count++];
        fx->kind=AUX_FX_LP;fx->side=side;fx->amount=damage;fx->spell=spell;
        fx->record=-1;fx->card_id=0;fx->card_data=0;
    }
}
/* The native Spellbinding Circle visual is parameterised by its displayed
   penalty. B. Skull Dragon uses 500; THTD uses 200, on each surviving
   adjacent monster. The permanent stat change is made at the effect site. */
static void aux_queue_stat_effect(int index,int amount)
{
    DuelCardRecord *r;
    AuxFx *fx;
    if(index<0||index>=30||amount<=0||amount>32767||
       aux_fx_count>=AUX_FX_CAP)return;
    r=&D_801A7AD8[index];
    if(!(r->flags&DUEL_CARD_FLAG_OCCUPIED))return;
    fx=&aux_fx_queue[aux_fx_count++];
    fx->kind=AUX_FX_STAT;fx->side=index/AUX_SIDE_STRIDE;
    fx->amount=amount;fx->spell=0;
    fx->record=index;fx->card_id=r->card_id;fx->card_data=r->data;
}
static void aux_queue_skull_effect(int caster_side)
{
    int index,opponent=caster_side^1;
    for(index=opponent*AUX_SIDE_STRIDE+AUX_MONSTER_START;
        index<opponent*AUX_SIDE_STRIDE+AUX_MONSTER_END;index++) {
        DuelCardRecord *r=&D_801A7AD8[index];
        if(!(r->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
        /* Keep B. Skull Dragon's established -500/-500 behaviour. */
        r->stat_modifier=(s16)aux_minus_500(r->stat_modifier);
        aux_queue_stat_effect(index,500);
    }
}


/* Future opponent equips fail at the genuine Duel_CheckEquip predicate.
   When CPU hasn't experienced a failed attempt, AI search sees the retail
   result. Once its actual placement fails, AI search also sees the refusal.
   No global equip table or already-applied bonuses are touched. */
static void aux_equip_event(MemoriesModEvent *event)
{
    int side=D_8009B1D5;
    if(event->phase!=MEMORIES_BEFORE||event->handled || side<0||side>1 ||
       !aux_kamion_refuses_equip(side,aux_has_kamion(side^1)))return;
    if(side==1 && !metal_placement_check && !kamion_cpu_learned[side])
        return; /* CPU's first speculative search can still pick Equip. */
    event->handled=1;
    event->result=0;
    if(metal_placement_check && side==1) {
        kamion_cpu_learned[side]=1;
        if(aux_host&&aux_host->log)
            aux_host->log(aux_host,"[Fusion Auxiliary Balance] CPU learned Kamion rejects Equip");
    }
}

/* Dark Elf receives half of the REAL LP healed, independent of Fusion origin.
   We observe the real LP change instead of the printed healing amount.
   Bad Reaction to Simochi and other reversals yield no positive delta. */
static void aux_healing(void)
{
    int before[2],after[2],side,n,i,dmg;
    before[0]=D_800E9FF0[0].life_points.unsigned_value;
    before[1]=D_800E9FF0[1].life_points.unsigned_value;
    /* Soul of the Pure at 8000+2000 must cap at 9999, not 8000.
       Retail honours max_life_points; don't alter any other heal amount. */
    if(gDuel_wEffectCardID==341 && D_8009B1C8 &&
       D_8009B1C8->max_life_points<9999)
        D_8009B1C8->max_life_points=9999;
    original_heal();
    for(side=0;side<2;side++) {
        after[side]=D_800E9FF0[side].life_points.unsigned_value;
        aux_dark_elf_react_to_healing(side,after[side]>before[side] ? after[side]-before[side] : 0);
    }
}

/* Use one common path for native healing and Mystical Elf's custom
   recovery; every face-up Dark Elf triggers independently from REAL healing.
   The damage queue is non-recursive and is not explosive Magic damage. */
static void aux_dark_elf_react_to_healing(int side,int actually_healed)
{
    int i,n=0,damage;
    if(side<0 || side>1 || actually_healed<=0)return;
    damage=actually_healed/2;
    if(damage<=0)return;
    for(i=side*AUX_SIDE_STRIDE+AUX_MONSTER_START;
        i<side*AUX_SIDE_STRIDE+AUX_MONSTER_END;i++)
        if(aux_live(i,AUX_DARK_ELF))n++;
    while(n-->0)aux_queue_lp(side^1,damage,0,AUX_DIRECT_SOURCE_DARK_ELF);
}
/* Trigger once per instance and only after face-up visibility. Native
   Goblin's Secret Remedy recovery visual = 1000 LP; the request itself
   renders only, while the capped LP addition is applied here exactly once. */
static void aux_elf_check_healing(void)
{
    int i,phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
    if(phase!=4 && phase!=5)return;
    if(gDuel_wCardEffectFlags || gDuel_bEffectState ||
       (gDuel_bEffectRequestStatus&DUEL_EFFECT_REQUEST_STATUS_BLOCKING))return;
    for(i=0;i<30;i++)if(aux_monster_slot(i)) {
        DuelCardRecord *r=&D_801A7AD8[i];
        int side=i/AUX_SIDE_STRIDE,before,limit,heal;
        DuelEffectRequest *request;
        if(!(r->flags&DUEL_CARD_FLAG_OCCUPIED)) {
            aux_elf_bound_id[i]=0;
            aux_elf_bound_data[i]=0;
            aux_elf_healed[i]=0;
            continue;
        }
        if(aux_elf_bound_id[i]!=r->card_id || aux_elf_bound_data[i]!=r->data) {
            aux_elf_bound_id[i]=r->card_id;
            aux_elf_bound_data[i]=r->data;
            aux_elf_healed[i]=0;
        }
        if(!aux_live(i,AUX_MYSTICAL_ELF) || aux_elf_healed[i])continue;
        aux_elf_healed[i]=1; /* even when LP is already at its ceiling */
        before=D_800E9FF0[side].life_points.unsigned_value;
        limit=D_800E9FF0[side].max_life_points;
        if(limit<before)limit=before;
        heal=aux_elf_capped_heal(before,limit);
        if(heal<=0)continue;
        D_800E9FF0[side].life_points.unsigned_value=(u16)(before+heal);
        /* One native healing visual; field_1A index 2 is the 1000 LP
           Goblin's Secret Remedy graphic. No second healing calculation. */
        request=DuelEffect_CreateRequest(5);
        if(request){
            request->field_00=0xA0;
            request->field_02=0x78;
            request->field_1A=2;
            SD_SEPlayFull(0x14);
        }
        aux_dark_elf_react_to_healing(side,heal);
        if(aux_host&&aux_host->log)aux_host->log(aux_host,
            "[Fusion Auxiliary Balance] Mystical Elf healed side=%d by %d (record=%d)",side,heal,i);
        /* Resolve only one newly flipped/summoned Elf per update; additional
           copies are handled after the current visual effect completes. */
        return;
    }
}

/* Reuses the validated WA_MRG thumbnail-to-field-slot pipeline used by
   Yamadron, but has its own records and cache. It needs no Ritual Effects
   MOD installed, and never edits the opponent's deck or the real hand. */
static int aux_darkfire_card_thumbnail(int index,int id)
{
    RECT *art,*name;
    if(!aux_host||!aux_host->disc_file_start||!aux_host->disc_read)return 0;
    if(aux_generated_magic_lba==-2)
        aux_generated_magic_lba=aux_host->disc_file_start(aux_host,"\\DATA\\WA_MRG.MRG;1");
    if(aux_generated_magic_lba<0)return 0;
    if(!aux_host->disc_read(aux_host,aux_generated_magic_lba+id-1,1,
                            aux_generated_magic_sector))return 0;
    memcpy(aux_generated_magic_cache[index],aux_generated_magic_sector,
           DUEL_CARD_DATA_BLOCK_SIZE);
    TextureDump_Delivered(aux_generated_magic_cache[index],DUEL_CARD_DATA_BLOCK_SIZE,
                          aux_generated_magic_lba+id-1,0);
    art=&D_80177EA4[index*2];
    art->w=20;art->h=32;
    art->x=(index%5)*20+0x380;
    art->y=(index/5)*32;
    LoadImage(art,(u32 *)aux_generated_magic_cache[index]);
    name=&D_80177EA4[index*2+1];
    name->x=0x380;name->y=index+0xE0;name->w=64;name->h=1;
    LoadImage(name,(u32 *)(aux_generated_magic_cache[index]+0x500));
    DrawSync(0);
    return 1;
}
static int aux_darkfire_create_magic(int index,int card_id)
{
    DuelCardRecord *r;
    DuelCardDisplayObject *obj;
    if(index<0||index>=30 || index%15<10 ||
       (r=&D_801A7AD8[index])->flags&DUEL_CARD_FLAG_OCCUPIED)return 0;
    aux_generated_magic_data[index].id=(s16)card_id;
    aux_generated_magic_data[index].deck_index=-1;
    aux_generated_magic_data[index].data_block_index=(u8)index;
    aux_generated_magic_data[index].flags_04=0;
    aux_generated_magic_data[index].unk_05=0;
    r->data=&aux_generated_magic_data[index];
    r->card_id=(s16)card_id;
    r->attack=r->defense=0;
    r->stat_modifier=r->terrain_modifier=0;
    r->flags=DUEL_CARD_FLAG_OCCUPIED|DUEL_CARD_FLAG_FACE_DOWN;
    r->table_index=(u8)index;
    obj=func_80024C1C(card_id,D_800908A0[index].x,D_800908A0[index].y);
    r->object=obj;
    if(obj)obj->card_index=(u8)index;
    aux_generated_magic_present[index]=1;
    if(!aux_darkfire_card_thumbnail(index,card_id) && aux_host && aux_host->log)
        aux_host->log(aux_host,"[Fusion Auxiliary Balance] Darkfire card thumbnail upload failed: slot %d id %d",index,card_id);
    return 1;
}
static void aux_darkfire_generate_side(int side,const char *reason)
{
    int i,free_slot,generated=0,eligible=0;
    if(side<0||side>1)return;
    /* Each fused and face-up Darkfire participates in the same TWO cards
       generated for the controlling side per turn; multiple copies do not
       multiply the production beyond two. */
    for(i=side*15+5;i<side*15+10;i++)
        if(aux_live(i,AUX_DARKFIRE_DRAGON)&&aux_fusion_special(i,0)) {
            eligible=1;break;
        }
    if(!eligible || aux_darkfire_generated_turn[side]>=2)return;
    for(free_slot=side*15+10;free_slot<side*15+15 &&
        aux_darkfire_generated_turn[side]<2;free_slot++) {
        int id;
        if(D_801A7AD8[free_slot].flags&DUEL_CARD_FLAG_OCCUPIED)continue;
        id=rand()%100<60?AUX_SPARKS:AUX_HINOTAMA;
        if(aux_darkfire_create_magic(free_slot,id)) {
            generated++;
            aux_darkfire_generated_turn[side]++;
        }
    }
    if(aux_host&&aux_host->log&&generated)
        aux_host->log(aux_host,"[Fusion Auxiliary Balance] Darkfire %s: created %d Explosive Magics side=%d",reason,generated,side);
}
static void aux_darkfire_turn_switch(void)
{
    int before=D_8009B1D5;
    original_turn_switch();
    if(D_8009B1D5!=before && D_8009B1D5<2 &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==2) {
        aux_darkfire_generated_turn[D_8009B1D5]=0;
        aux_darkfire_generate_side(D_8009B1D5,"turn start");
        r3_turn_begin(D_8009B1D5);
        r2_reconcile_zombies(1);
        r3_reconcile_summons(1);
    }
}

/* Track a real card's Equip totals, including those applied before Kamion
   appeared. The active bonus is reversible and never stacks across copies. */
static void aux_equip_ensure(int i)
{
    AuxEquipLedger *e;
    DuelCardRecord *r;
    if(!aux_monster_slot(i))return;
    r=&D_801A7AD8[i];e=&aux_equip_ledger[i];
    if(e->data!=r->data||e->card!=r->card_id) {
        e->data=r->data;e->card=r->card_id;e->atk=e->def=e->boosted=0;
    }
}
static void aux_kamion_refresh(void)
{
    int i,active[2]={aux_fused_kamion(0),aux_fused_kamion(1)};
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==11 ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==8)return;
    for(i=0;i<30;i++)if(aux_monster_slot(i)) {
        DuelCardRecord *r=&D_801A7AD8[i];
        AuxEquipLedger *e=&aux_equip_ledger[i];int boost;
        if(!(r->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
        aux_equip_ensure(i);
        boost=active[i/AUX_SIDE_STRIDE];
        if(boost!=e->boosted) {
            int sign=boost?1:-1;
            int atk=(int)r->stat_modifier+sign*e->atk;
            int def=(int)r->defense_modifier+sign*(e->def-e->atk);
            if(atk>32767)atk=32767;if(atk< -32768)atk=-32768;
            if(def>32767)def=32767;if(def< -32768)def=-32768;
            r->stat_modifier=(s16)atk;r->defense_modifier=(s16)def;
            e->boosted=boost;
        }
    }
}

/* An actual fusion-summoned Musician King is attack-only. Refuse defense
   without consuming the card's normal battle damage or touching LP. */
static void aux_musician_attack_only(void)
{
    int i;
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==11 ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==8)return;
    for(i=0;i<30;i++)if(aux_live(i,AUX_MUSICIAN_KING) &&
        aux_fusion_special(i,0) &&
        (D_801A7AD8[i].flags&DUEL_CARD_FLAG_DEFENSE_POSITION)) {
        DuelCardRecord *r=&D_801A7AD8[i];
        r->flags&=~DUEL_CARD_FLAG_DEFENSE_POSITION;
        if(r->object)Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)r->object);
    }
}
static void aux_rotate(DisplayObject *o)
{
    /* Match Ritual Effects / Magician of Black Chaos exactly: cancelling
       a rotation also retires the object's pending motion callback. Merely
       returning (beta16) left that callback alive and could freeze the duel. */
    if(o) {
        int i=(int)o->field_6A;
        if(aux_monster_slot(i) && aux_live(i,AUX_MUSICIAN_KING) &&
           aux_fusion_special(i,0) &&
           !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_DEFENSE_POSITION)) {
            o->field_6C=0;
            o->update=0;
            Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)o);
            return;
        }
    }
    if(original_rotate)original_rotate(o);
    aux_musician_attack_only();
}

/* Swords of Revealing Light is a side-wide Magic restriction, not a
   direct per-card callback. Let a genuine Metalmorph-fused monster ignore
   it when the player selects that monster to attack. Restore the side's
   Swords timer as soon as the native field action has run so other
   monsters remain restricted. */
static void aux_field_actions(void)
{
    int side_id=D_8009B1D5,phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
    int stage=D_8009B174&15,blocked,ai=0,changed_timer=0;
    s8 saved=0;
    DuelSideState *side=D_8009B1C8;
    DuelSelectionRecord *cursor;
    if(side_id>1||phase!=5||!side){original_field_actions();return;}
    ai=D_8009B360[side_id]>=0;
    blocked=r3_kaminari_attacks_blocked(side_id);
    cursor=(DuelSelectionRecord *)(D_800E9F48+side_id*DUEL_SELECTION_SIDE_SIZE);
    /* Native AI's row-1 target is encoded as 0x3C-column. Intercept BEFORE
       it commands the cursor to move, and validate once more at commit. */
    if(ai&&(stage==5||stage==6)){
       DuelSelectionRecord *target=(DuelSelectionRecord *)(D_800E9F64+side_id*DUEL_SELECTION_SIDE_SIZE);
       int selected=stage==5 ? (int)D_800EAE92 : -1;
       int column=stage==5 && selected>=0x38 && selected<=0x3C ? 0x3C-selected :
                  stage==6 && target->row==1 ? target->col : -1;
       int index=column>=0?r3_grid_opponent_target(side_id,column):-1;
       if(blocked || (aux_monster_slot(index)&&r3_flower_untargetable(index))){
          int next=!blocked ? r3_find_attackable_enemy(side_id,-1) : -1;
          if(next>=0 && stage==5){D_800EAE92=(u8)(0x3C-next);}
          else if(next>=0 && stage==6){target->col=(s8)next;}
          else {
             /* Native state 11->10 ends only this already chosen attack/turn.
                Never fabricate a direct attack on a protected monster. */
             D_8009B174=11;
             original_field_actions();
             return;
          }
       }
    }
    /* Player: leave target selection open when Flower Wolf is protected.
       Other targets and non-attack actions remain selectable. */
    if(!ai && stage==6 && (gInput_wPad1Pressed&0xC0)){
       DuelSelectionRecord *target=(DuelSelectionRecord *)(D_800E9F64+side_id*DUEL_SELECTION_SIDE_SIZE);
       if(target->row==1&&target->col>=0&&target->col<5){
          int index=r3_grid_opponent_target(side_id,target->col);
          if(aux_monster_slot(index)&&r3_flower_untargetable(index)){
             SD_SEPlayFull(9);return;
          }
       }
    }
    /* Existing Metalmorph/Swords exception: *not* an exception to Kaminari.
       Use the genuine Swords branch as a transient validation gate only on
       the attack-selection tick. Preserve the counter after native returns. */
    if(!ai && stage==3 && side_id==0 &&
       (gInput_wPad1Pressed&0xC0) && cursor->row==2&&
       cursor->col>=0&&cursor->col<5){
       int slot=D_800907D8[side_id*20+10+cursor->col];
       if(blocked){saved=side->swords_turns_remaining;
           side->swords_turns_remaining=1;changed_timer=1;}
       else if(side->swords_turns_remaining>0&&aux_metal_protected(slot)){
           saved=side->swords_turns_remaining;
           side->swords_turns_remaining=0;changed_timer=1;}
    }
    if(ai && blocked){
       saved=side->swords_turns_remaining;
       if(!saved){side->swords_turns_remaining=1;changed_timer=1;}
    }
    original_field_actions();
    if(changed_timer)side->swords_turns_remaining=saved;
}

/* During the opposing player's Magic/Equip handler, retain the protected
   instance's board flags and combat modifiers. The normal handler may run
   its animation, but cannot persist its negative effects on that monster.
   Destruction itself is intercepted by aux_remove() below. */
static s32 aux_card_effect_update(void)
{
    s16 attack_before[30], defense_before[30];
    u16 flags_before[30];
    u8 guarded[30]={0};
    int source=gDuel_wEffectCardID;
    int side=D_8009B1D5;
    int spell=(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE) &&
              (side>=0 && side<2 && Cards_Valid(source) &&
               (Cards_Type(source)==CARD_TYPE_MAGIC ||
                Cards_Type(source)==CARD_TYPE_EQUIP));
    int i, prev_context=metal_spell_context,prev_side=metal_spell_side;
    if(spell) {
        for(i=0;i<30;i++)if(aux_monster_slot(i) && i/AUX_SIDE_STRIDE!=side &&
           aux_metal_protected(i)) {
            DuelCardRecord *r=&D_801A7AD8[i];
            guarded[i]=1;
            attack_before[i]=r->stat_modifier;
            defense_before[i]=r->defense_modifier;
            flags_before[i]=r->flags;
        }
        metal_spell_context=1;
        metal_spell_side=side;
    }
    {
        s32 ret=original_card_effect_update();
        if(spell) {
            for(i=0;i<30;i++)if(guarded[i]) {
                DuelCardRecord *r=&D_801A7AD8[i];
                if((r->flags & DUEL_CARD_FLAG_OCCUPIED) &&
                   r->card_id==metal_bound_id[i] &&
                   r->data==metal_bound_data[i]) {
                    /* Do not undo unrelated visual/lifecycle bits. */
                    u16 stance=DUEL_CARD_FLAG_DEFENSE_POSITION |
                               DUEL_CARD_FLAG_FACE_DOWN;
                    int changed=(r->flags^flags_before[i]) & stance;
                    r->flags=(r->flags&~stance) | (flags_before[i]&stance);
                    r->stat_modifier=attack_before[i];
                    r->defense_modifier=defense_before[i];
                    if(changed && r->object)
                        Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)r->object);
                }
            }
        }
        metal_spell_context=prev_context;
        metal_spell_side=prev_side;
        return ret;
    }
}

/* Direct LP Damage includes the five damaging Magic Cards and the
   registered monster-effect hits from Meteor B. Dragon / Gaia. The former
   passes through retail's DAMAGE event here; the latter is scheduled at
   their own effect site by aux_queue_lp, without pretending to be Magic. */
/* The DAMAGE event is reached at the exact retail LP subtraction. Let
   retail subtract the original amount untouched, then block scene advance
   while one native animation (#6) and one extra base hit run per Darkfire. */
static void aux_damage(MemoriesModEvent *event)
{
    int hits;
    if(event->phase!=MEMORIES_BEFORE || event->handled || D_8009B1D5>1)
        return;
    hits=aux_darkfire_bonus_hits(gDuel_wEffectCardID,
        aux_count_darkfire(D_8009B1D5),D_8009B1D5,
        event->a,event->c,event->b);
    if(!hits) return;
    /* Scene request blocking makes a second eligible spell impossible until
       this stack completes. Refuse to overwrite it if another mod reenters. */
    if(bonus_hits_queued || bonus_hit_playing) return;
    bonus_hits_queued=hits;
    bonus_hit_playing=0;
    bonus_hit_side=event->a;
    bonus_hit_amount=event->b;
    bonus_hit_spell=gDuel_wEffectCardID;
}

static void aux_apply_bonus_hit(void)
{
    int life;
    if(bonus_hit_side<0 || bonus_hit_side>1 || bonus_hit_amount<=0) return;
    life=D_800E9FF0[bonus_hit_side].life_points.unsigned_value;
    life=aux_damage_one_hit(life,bonus_hit_amount);
    D_800E9FF0[bonus_hit_side].life_points.unsigned_value=(u16)life;
    if(aux_host && aux_host->log)
        aux_host->log(aux_host,"[Fusion Auxiliary Balance] Darkfire bonus: spell=%d side=%d amount=%d remaining=%d",
            bonus_hit_spell,bonus_hit_side,bonus_hit_amount,life);
}

/* Hook the same native request scheduler that presents the ordinary Sparks /
   Hinotama impact. Replay ID 6 with the original spell's animation index.
   Don't create all requests together: they would overlap visually. The
   scheduler's BLOCKING flag freezes battle/hand actions between replays. */
static void aux_finish_fx_lp(const AuxFx *fx)
{
    if(fx->kind==AUX_FX_LP && fx->side>=0 && fx->side<2) {
        int life=D_800E9FF0[fx->side].life_points.unsigned_value;
        int after=aux_damage_one_hit(life,fx->amount);
        D_800E9FF0[fx->side].life_points.unsigned_value=(u16)after;
        if(aux_host && aux_host->log)
            aux_host->log(aux_host,"[Fusion Auxiliary Balance] Fusion Effect %d dealt %d to side %d: LP %d",
                           fx->spell,fx->amount,fx->side,after);
    }
}

static s32 aux_update_requests(void)
{
    s32 active=original_update_requests();
    DuelEffectRequest *request;
    int i;
    if(active || (gDuel_bEffectRequestStatus & DUEL_EFFECT_REQUEST_STATUS_BLOCKING))
        return active;
    if(mass_visual_repair_pending) {
        int n,requests_busy=0;
        for(n=0;n<DUEL_EFFECT_REQUEST_COUNT;n++)
            if(D_800EAD88[n].flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE)
                requests_busy=1;
        if(!requests_busy)aux_repair_mass_visuals();
    }
    if(bonus_hit_playing) {
        aux_apply_bonus_hit();
        bonus_hit_playing=0;
        bonus_hits_queued--;
    }
    if(aux_fx_playing) {
        aux_finish_fx_lp(&aux_fx_current);
        aux_fx_playing=0;
    }
    /* Preserve Darkfire's already-tested sequential animation system. */
    if(bonus_hits_queued>0) {
        if(bonus_hit_side<0 || bonus_hit_side>1 ||
           !D_800E9FF0[bonus_hit_side].life_points.unsigned_value) {
            bonus_hits_queued=0;
        } else {
            request=DuelEffect_CreateRequest(6);
            if(!request) {
                while(bonus_hits_queued-->0 &&
                   D_800E9FF0[bonus_hit_side].life_points.unsigned_value>0)
                    aux_apply_bonus_hit();
                bonus_hits_queued=0;
            } else {
                request->field_00=0xA0;
                request->field_02=0x78;
                request->field_1A=bonus_hit_spell-AUX_SPARKS;
                SD_SEPlayFull(0x1C);
                bonus_hit_playing=1;
                gDuel_bEffectRequestStatus |= DUEL_EFFECT_REQUEST_STATUS_BLOCKING |
                                               DUEL_EFFECT_REQUEST_STATUS_ACTIVE;
                return 1;
            }
        }
    }
    while(aux_fx_count>0) {
        AuxFx fx=aux_fx_queue[0];
        DuelCardRecord *r=0;
        DisplayObject *o=0;
        if(fx.kind==AUX_FX_LP &&
           (fx.side<0 || fx.side>1 ||
            D_800E9FF0[fx.side].life_points.unsigned_value==0))
            goto dequeue;
        if(fx.kind==AUX_FX_STAT) {
            if(!aux_monster_slot(fx.record))goto dequeue;
            r=&D_801A7AD8[fx.record];
            if(!(r->flags&DUEL_CARD_FLAG_OCCUPIED) ||
               r->card_id!=fx.card_id || r->data!=fx.card_data || !r->object)
                goto dequeue;
            o=(DisplayObject *)r->object;
        }
        request=DuelEffect_CreateRequest(fx.kind==AUX_FX_STAT?0xD:(fx.spell==0?2:6));
        if(!request) {
            /* Never leave the duel stuck in a pending queue if the native
               request pool has no slot available. */
            aux_finish_fx_lp(&fx);
            goto dequeue;
        }
        if(fx.kind==AUX_FX_LP) {
            if(fx.spell==0) { /* native battle-damage number, not Magic */
                int magnitude=fx.amount/1000;
                if(magnitude>2)magnitude=2;
                request->field_00=0xF0;
                request->field_02=0x78;
                request->field_12=fx.amount;
                request->field_1A=magnitude+3;
                SD_SEPlayFull(magnitude+0x10);
            } else {
                request->field_00=0xA0;
                request->field_02=0x78;
                request->field_1A=fx.spell-AUX_SPARKS;
                SD_SEPlayFull(0x1C);
            }
        } else {
            request->field_00=o->field_30.h.field_30;
            request->field_02=o->field_30.h.field_32;
            request->field_04=o->field_34.h.field_34;
            request->field_12=(s16)-fx.amount;
            request->field_1A=2; /* native Spellbinding Circle FX */
            request->buffer=(u8 *)request->buffer+
                            ((fx.record%AUX_SIDE_STRIDE-AUX_MONSTER_START)<<14);
            SD_SEPlayFull(0x21);
        }
        aux_fx_current=fx;
        aux_fx_playing=1;
        for(i=1;i<aux_fx_count;i++)aux_fx_queue[i-1]=aux_fx_queue[i];
        aux_fx_count--;
        gDuel_bEffectRequestStatus |= DUEL_EFFECT_REQUEST_STATUS_BLOCKING |
                                       DUEL_EFFECT_REQUEST_STATUS_ACTIVE;
        return 1;
    dequeue:
        for(i=1;i<aux_fx_count;i++)aux_fx_queue[i-1]=aux_fx_queue[i];
        aux_fx_count--;
    }
    return active;
}

/* Normal face-up play remains unchanged. For three spells only, press
   Square instead of Cross to use the native field-pick and fusion flow. We temporarily present it as a set card JUST for the
   hand controller's auto-activation test; after the controller chooses
   field placement, restore the real face-up state before animation.
   This avoids patching all Magic cards' types or editing the game UI. */
static void aux_hand_actions(void)
{
    DuelSelectionRecord *selection;
    DuelHandSlot *slot;
    HandCardObject *object;
    DuelCardRecord *record;
    u8 original_face;
    int card_id;
    if((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK)!=4 ||
       !(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) ||
       (gDuel_wSceneStateFlags & 0x4000) ||
       D_8009B1D5!=0 || (D_8009B174 & 0xF)!=3 ||
       !(D_8009B174 & 0x80) || (D_8009B174 & 0x60) ||
       !(gInput_wPad1Pressed & PAD_BUTTON_SQUARE)) {
        original_hand_actions();
        return;
    }
    selection=(DuelSelectionRecord *)(D_800E9F10+
                D_8009B1D5*DUEL_SELECTION_SIDE_SIZE);
    if(selection->field_0E>=5) { original_hand_actions(); return; }
    slot=&D_800EA030[selection->field_0E];
    object=(HandCardObject *)slot->object;
    if(!object || object->face!=0 || object->card_index>=30) {
        original_hand_actions(); return;
    }
    record=&D_801A7AD8[object->card_index];
    card_id=Cards_BaseId(record->card_id);
    if(!aux_field_target_spell(card_id) || selection->field_15!=0) {
        original_hand_actions(); return;
    }
    original_face=object->face;
    object->face=0x80;
    original_hand_actions();
    object->face=original_face;
    /* Only clear the forged flip after native chose the field selection
       path. A genuine face-down play was never modified. */
    if((D_8009B174 & 0xF)==3 && (D_8009B174 & 0x10))
        record->flags &= ~DUEL_CARD_FLAG_FACE_DOWN;
    if(aux_host && aux_host->log)
        aux_host->log(aux_host,"[Fusion Auxiliary Balance] targeted magic from hand: %d",card_id);
}

static void r3_queue_rebirth_fire(int slot,int id)
{
    DuelCardRecord *c;
    DisplayObject *o;
    DuelEffectRequest *r;
    int variant;
    if(slot<0 || slot>=30 || !aux_monster_slot(slot))return;
    c=&D_801A7AD8[slot];
    if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED) || c->card_id!=id || !c->object || !c->data)return;
    o=(DisplayObject *)c->object;
    variant=Duel_GetCardEffectVariant((CardObject *)o);
    r=(DuelEffectRequest *)DuelEffect_AllocateRequest(0xB);
    if(!r)return;
    r->buffer=(u8 *)r->buffer;
    r->field_00=o->field_30.h.field_30;
    r->field_02=o->field_30.h.field_32;
    r->field_04=o->field_34.h.field_34;
    r->field_1A=(s16)variant;
    SD_SEPlayFull(0x1F);
    if(aux_host && aux_host->log)
        aux_host->log(aux_host,
            "[Fusion Auxiliary Balance] Mavelus rebirth fire: slot=%d id=%d",slot,id);
}

static void aux_remove(DuelCardRecord *r)
{
    int index,revive=0;void *revive_data=0;
    if(!r) return;
    index=(int)(r-D_801A7AD8);
    if(aux_opponent_magic_immune(index)) return;
    /* This hook runs for effect destructions and battle destruction.
       Placements, Ritual tributes and ordinary fusions are NOT protected. */
    if(((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 ||
        (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==11) &&
       aux_battle_guarded_record(index)) return;
    if(removal_context && index>=0 && aux_monster_slot(index) &&
       r->card_id==272 && r3_source(index,272)){revive=1;revive_data=r->data;}
    if(aux_dragon_protected(index) &&
       (removal_context ||
        (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 ||
        (battle_protection_pending &&
         (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==11))) return;
    original_remove(r);
    if(r->flags & DUEL_CARD_FLAG_OCCUPIED)return; /* another mod protected it */
    if(revive)r3_schedule_mavelus_data(index,revive_data);
    if(index>=0&&index<30)r3_kaminari_forget(index);
    if(index>=0 && index<30) {
        aux_unlock_revoke(index);
        metal_immune[index]=0;
        metal_bound_id[index]=0;
        metal_bound_data[index]=0;
        fusion_origin[index]=0;
        fusion_bound_id[index]=0;
        fusion_bound_data[index]=0;
        aux_elf_healed[index]=0;
        aux_elf_bound_id[index]=0;
        aux_elf_bound_data[index]=0;
    }
}

static void aux_mass_begin(void)
{
    if(!mass_destruction_active) {
        int i;
        mass_seiyaryu_present[0]=aux_has_seiyaryu(0);
        mass_seiyaryu_present[1]=aux_has_seiyaryu(1);
        mass_destruction_active=1;
        /* Capture before any native effect can start a card-destruction
           presentation. Only snapshot Dragons actually covered right now. */
        for(i=0;i<30;i++) {
            mass_visual_marked[i]=0;
            if(aux_dragon_protected(i)) {
                mass_visual_marked[i]=1;
                mass_visual_id[i]=D_801A7AD8[i].card_id;
                mass_visual_data[i]=D_801A7AD8[i].data;
            }
        }
    }
    removal_context++;
}
static void aux_mass_end(void)
{
    removal_context--;
    if(!gDuel_wCardEffectFlags) {
        /* The retail effect has finished its own state machine. Defer the
           actual sprite repair to the next idle request update, AFTER the
           game has completed all blocking/destruction animations. */
        mass_visual_repair_pending=1;
        mass_destruction_active=0;
        mass_seiyaryu_present[0]=mass_seiyaryu_present[1]=0;
    }
}
static void aux_repair_mass_visuals(void)
{
    int i;
    if(!mass_visual_repair_pending || mass_destruction_active)return;
    for(i=0;i<30;i++) {
        DuelCardRecord *r;
        DuelCardDisplayObject *obj;
        if(!mass_visual_marked[i])continue;
        mass_visual_marked[i]=0;
        r=&D_801A7AD8[i];
        /* A destroyed/replaced instance must NEVER be resurrected. */
        if(!aux_visual_instance_still_present(
               (r->flags&DUEL_CARD_FLAG_OCCUPIED)!=0,
               r->card_id,r->data,mass_visual_id[i],mass_visual_data[i]))continue;
        if(r->object)DisplayObject_ReleaseIfPresent(r->object);
        r->object=0;
        obj=func_80024C1C(r->card_id,D_800908A0[i].x,D_800908A0[i].y);
        if(obj) {
            obj->card_index=(u8)i;
            r->object=obj;
            Duel_ApplyCardObjectFlags(obj);
        }
        if(aux_host && aux_host->log)
            aux_host->log(aux_host,
                "[Fusion Auxiliary Balance] repaired protected Dragon sprite: index=%d id=%d",i,r->card_id);
    }
    mass_visual_repair_pending=0;
}
static void aux_board(void)
{
    aux_mass_begin(); original_board(); aux_mass_end();
}
static void aux_raigeki(void)
{
    aux_mass_begin(); original_raigeki(); aux_mass_end();
}
static void aux_monster_removal(void)
{
    aux_mass_begin(); original_monster_removal(); aux_mass_end();
}

/* MonsterEffects_Update is a PC-host function, not a patchable guest-game
   function. Hook DuelScene_Update (a guest function) instead: it calls the
   former at the start of stable hand/field phases, where card-defined
   MONSTER_DO_DESTROY can remove a card directly. Only give removal protection
   to the first phase tick, not subsequent hand/field actions (fusions,
   placements or rituals). Battle-triggered removals use the battle path. */
static void aux_scene(void)
{
    int phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
    if(phase==1 && !(gDuel_wSceneStateFlags&DUEL_SCENE_FLAG_INITIALIZED)) {
        aux_new_effects_reset();
        r2_reset();r2_full_reset();r3_reset();r3_full_reset();
        int i;
        metal_pending_result=metal_pending_side=metal_placement_check=0;
        fusion_pending_result=fusion_pending_side=placement_was_fusion=0;
        aux_fx_count=aux_fx_playing=0;
        mass_destruction_active=mass_visual_repair_pending=0;
        mass_seiyaryu_present[0]=mass_seiyaryu_present[1]=0;
        metal_spell_context=metal_spell_side=0;
        kamion_cpu_learned[0]=kamion_cpu_learned[1]=0;
        aux_darkfire_generated_turn[0]=aux_darkfire_generated_turn[1]=0;
        aux_equip_tracking=0;
        for(i=0;i<30;i++) {
            metal_immune[i]=0;
            metal_bound_id[i]=0;
            metal_bound_data[i]=0;
            fusion_origin[i]=0;fusion_bound_id[i]=0;fusion_bound_data[i]=0;
            aux_elf_healed[i]=0;aux_elf_bound_id[i]=0;aux_elf_bound_data[i]=0;
            mass_visual_marked[i]=0;
            aux_equip_ledger[i].data=0;aux_equip_ledger[i].card=0;
            aux_equip_ledger[i].atk=aux_equip_ledger[i].def=0;
            aux_equip_ledger[i].boosted=0;
        }
    }
    if(phase==4 || phase==5)aux_unlock_sweep();
    r2_reconcile_zombies(0);
    r3_reconcile_summons(0);
    r2_stable_tick();r3_stable_tick();
    /* Native phase 2 (draw) has completed, but the phase-4 handler has not
       run: safely return Mavelus before human or CPU can choose a field slot. */
    if(phase==4)r3_turn_entry_before_actions(D_8009B1D5);
    int effect_boundary=(phase==4 || phase==5) &&
                        !(gDuel_wSceneStateFlags&DUEL_SCENE_FLAG_INITIALIZED) &&
                        !gDuel_wCardEffectFlags && !gDuel_bEffectState;
    if(effect_boundary) removal_context++;
    original_scene();
    {int after=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
     if(after==4||after==5||after==7||after==8)r3_kaminari_scan();}
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5 ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==4)
        aux_kamion_refresh();
    aux_musician_attack_only();
    aux_elf_check_healing();
    if(effect_boundary) removal_context--;
}

/* At battle entry the two monsters have been temporarily deactivated by
   DuelScene_UpdateFieldActions, but their record card_ids are preserved and
   the lifted 2D objects identify the records. Protection must NOT check
   OCCUPIED on those two records, or it will always miss the first hit. */
static void aux_wall_return_to_defense(void)
{
    DuelCardRecord *r;
    int index=wall_attacker_record;
    if(!aux_monster_slot(index)) goto done;
    r=&D_801A7AD8[index];
    /* The instance must still exist in its original monster zone; do not
       transform a replacement card that happens to occupy the same slot. */
    if(!aux_wall_should_defend(1,wall_attack_resolved,wall_attack_negated,
             (r->flags&DUEL_CARD_FLAG_OCCUPIED) &&
             !(r->flags&DUEL_CARD_FLAG_FACE_DOWN) &&
             r->data==wall_attacker_data,r->card_id))goto done;
    r->flags|=DUEL_CARD_FLAG_DEFENSE_POSITION;
    if(r->object)
        Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)r->object);
    if(aux_host && aux_host->log)
        aux_host->log(aux_host,"[Fusion Auxiliary Balance] Wall Shadow returned to Defense: record=%d",index);
done:
    wall_attacker_record=-1;
    wall_attacker_data=0;
    wall_attack_resolved=wall_attack_negated=0;
}

static void aux_capture_battle(void)
{
    int i;
    memset(&aux_stat_tween,0,sizeof(aux_stat_tween));
    battle_protection_pending=1;
    battle_lp_before[0]=D_800E9FF0[0].life_points.unsigned_value;
    battle_lp_before[1]=D_800E9FF0[1].life_points.unsigned_value;
    battle_expected_loss[0]=battle_expected_loss[1]=0;
    battle_lp_reported=0;
    battle_special_attacker=battle_attack_won=battle_pierce_amount=0;
    wall_attacker_record=-1;
    wall_attacker_data=0;
    wall_attack_resolved=wall_attack_negated=0;
    for(i=0;i<2;i++) {
        DisplayObject *object=D_800E9EF0[i];
        int record=object ? (int)object->field_6A : -1;
        battle_record[i]=-1;
        battle_card[i]=0;
        battle_guarded[i]=0;
        battle_saved_flags[i]=0;
        battle_saved_data[i]=0;
        if(aux_monster_slot(record)) {
            const DuelCardRecord *r=&D_801A7AD8[record];
            int id=r->card_id;
            battle_record[i]=record;
            battle_card[i]=id;
            battle_saved_flags[i]=r->flags;
            battle_saved_data[i]=r->data;
            /* The current actor's side is the attacker, not necessarily
               participant 0. The normal battle participant slots are lifted. */
            if(record/AUX_SIDE_STRIDE==D_8009B1D5 && id==AUX_WALL_SHADOW) {
                wall_attacker_record=record;
                wall_attacker_data=r->data;
            }
            if(Cards_Valid(id))
                battle_guarded[i]=(id==AUX_MUSICIAN_KING && aux_fusion_special(record,1)) ||
                    aux_lifted_battle_protected(record,id,Cards_Type(id),
                       CARD_TYPE_DRAGON,Cards_Level(id),
                       aux_has_seiyaryu(record/AUX_SIDE_STRIDE));
            if(aux_host && aux_host->log && aux_has_seiyaryu(record/AUX_SIDE_STRIDE))
                aux_host->log(aux_host,"[Fusion Auxiliary Balance] Seiyaryu battle: part=%d record=%d card=%d level=%d type=%d protected=%d",
                    i,record,id,Cards_Valid(id)?Cards_Level(id):0,
                    Cards_Valid(id)?Cards_Type(id):-1,battle_guarded[i]);
        }
    }
    r2_begin();r3_begin();
    for(i=0;i<2;i++)if(r3_guard(battle_record[i]))battle_guarded[i]=1;
    battle_2d_skipped=battle_restore=battle_3d_suppressed=0;
    aux_bw_prepare();
    if(battle_record[0]>=0 &&
       battle_record[0]/AUX_SIDE_STRIDE==D_8009B1D5 &&
       aux_fusion_special(battle_record[0],1))
        battle_special_attacker=battle_card[0];
}

static int aux_battle_guarded_record(int record)
{
    int i;
    if(!battle_protection_pending || !aux_monster_slot(record)) return 0;
    for(i=0;i<2;i++)
        if(battle_guarded[i] && battle_record[i]==record &&
           D_801A7AD8[record].card_id==battle_card[i] &&
           D_801A7AD8[record].data==battle_saved_data[i] &&
           (battle_card[i]==AUX_MUSICIAN_KING || r2_guard(record) || r3_guard(record) ||
            aux_has_seiyaryu(record/AUX_SIDE_STRIDE))) return 1;
    return 0;
}

/* These two signed replay bytes name actual FIELD RECORDS, not a side. 3D
   death flags and the later 2D removal replay are independent. Protect both. */
static void aux_sanitize_battle_replay(void)
{
    s8 *entries[2]={&D_8009B208,&D_8009B209};
    int i;
    for(i=0;i<2;i++)
        if(*entries[i]>=0 && aux_battle_guarded_record(*entries[i]))
            *entries[i]=-1;
}
/* As the Javelin Beetle mod demonstrates, the small field card's fire is
   requested by battle state 10. A simple RemoveFromField hook runs too late:
   state 10 can consume the card's visual/record without calling it. Jump to
   state 6 as the retail survivor path BEFORE it requests the burn. Keep the
   original battle damage/result markers and never change either duelist's LP. */
static void aux_battle_skip_2d_fire(void)
{
    int part=(int)D_8009B1B9;
    DisplayObject *participant;
    int index;
    if((D_8009B174&15)!=10 || D_8009B22A || part<0 || part>=2)
        return;
    participant=D_800E9EF0[part];
    if(!participant) return;
    index=(int)participant->field_6A;
    if(aux_gaia_skip_burn(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK,
            D_8009B174&15,D_8009B22A,part,D_800E9EF0[part+2]!=0,
            aux_battle_guarded_record(index))) {
        battle_2d_skipped|=1u<<part;
        D_8009B174=6; /* retail's no-fire survivor path, also used by Javelin */
        if(aux_host && aux_host->log)
            aux_host->log(aux_host,"[Fusion Auxiliary Balance] Seiyaryu protected 2D card: part=%d record=%d",part,index);
    }
}

/* The battle's state 11 may have no result/panel object for a logically
   defeated card, even if we suppressed both its fire presentations. In that
   case restore the original monster instance when the native battle ends,
   just like Javelin's fallback. Do NOT recreate a second object if retail
   has already restored the record. Do not draw behind a 0-LP result screen. */
static void aux_battle_finish(void)
{
    int part;
    for(part=0;part<2;part++) {
        DuelCardRecord *r;
        DuelCardDisplayObject *obj;
        int index=battle_record[part];
        if(!(battle_restore&(1u<<part)) || !aux_battle_guarded_record(index))
            continue;
        r=&D_801A7AD8[index];
        if(r->flags&DUEL_CARD_FLAG_OCCUPIED) continue; /* native survivor */
        r->flags=battle_saved_flags[part]|DUEL_CARD_FLAG_OCCUPIED;
        if(index/AUX_SIDE_STRIDE==D_8009B1D5)
            r->flags|=DUEL_CARD_FLAG_USED_THIS_TURN;
        if(r->object) DisplayObject_ReleaseIfPresent(r->object);
        r->object=0;
        if(D_800E9FF0[0].life_points.unsigned_value &&
           D_800E9FF0[1].life_points.unsigned_value) {
            obj=func_80024C1C(r->card_id,D_800908A0[index].x,D_800908A0[index].y);
            r->object=obj;
            if(obj) obj->card_index=(u8)index;
        }
        if(aux_host && aux_host->log)
            aux_host->log(aux_host,"[Fusion Auxiliary Balance] Seiyaryu restored battle survivor: part=%d record=%d",part,index);
    }
}

static void aux_finish_attack_effect(void)
{
    int a=battle_record[0],d=battle_record[1];
    DuelCardRecord *attacker;
    if(!battle_attack_won || (battle_special_attacker!=AUX_GAIA_DRAGON_CHAMPION &&
        battle_special_attacker!=AUX_RABID_HORSEMAN) ||
       !aux_monster_slot(a) || !aux_monster_slot(d))goto done;
    attacker=&D_801A7AD8[a];
    if(!(attacker->flags&DUEL_CARD_FLAG_OCCUPIED) ||
       attacker->card_id!=battle_special_attacker ||
       attacker->data!=battle_saved_data[0] ||
       !aux_fusion_special(a,0))goto done;
    if(battle_special_attacker==AUX_GAIA_DRAGON_CHAMPION &&
       !(D_801A7AD8[d].flags&DUEL_CARD_FLAG_OCCUPIED))
        aux_queue_lp((a/AUX_SIDE_STRIDE)^1,500,346,AUX_DIRECT_SOURCE_GAIA); /* Ookazi */
    else if(battle_pierce_amount>0)
        aux_queue_lp((a/AUX_SIDE_STRIDE)^1,battle_pierce_amount,0,0);
done:
    battle_special_attacker=battle_attack_won=battle_pierce_amount=0;
}

/* The result is committed only once the coin animation has finished.
   RitualCoinFlip_Request resolves its callback AFTER the rendered coin exits. */
static void aux_bw_done(int result,void *context)
{
    (void)context;
    if(result!=0 && !bw_coin_used && aux_encore_eligible(bw_coin_side)) {
        bw_coin_used=1;
        if(AuxBattleCoinFlip_Request(0,-1,aux_bw_done,0))return;
    }
    bw_success=(result==0);
    bw_coin_ready=1;
    bw_coin_pending=0;
}
/* The i386 card fields are lifted for 3D battles, but their card identity
   remains. Save original modifiers for restoration after the duel scene. */
static void aux_bw_swap(void)
{
    DuelCardRecord *a,*b;
    int i,at[2],mod[2],def_mod[2];
    if(!bw_success || bw_swapped || !aux_monster_slot(bw_record) ||
       !aux_monster_slot(bw_target))return;
    a=&D_801A7AD8[bw_record];b=&D_801A7AD8[bw_target];
    if(a->card_id!=AUX_BATTLE_WARRIOR ||
       b->card_id!=battle_card[bw_target==battle_record[0]?0:1] ||
       a->data!=battle_saved_data[bw_record==battle_record[0]?0:1])return;
    at[0]=(s16)(Duel_CalcCardStats(a)&0xffff);
    at[1]=(s16)(Duel_CalcCardStats(b)&0xffff);
    mod[0]=a->stat_modifier;mod[1]=b->stat_modifier;
    def_mod[0]=a->defense_modifier;def_mod[1]=b->defense_modifier;
    /* On the PC port, effective DEF includes stat_modifier as well as
       defense_modifier. Change both with opposite deltas to swap ONLY ATK.
       The saved scene replay also has to use the temporary DEF correction,
       otherwise its mid-battle restore changes DEF unpredictably. */
    for(i=0;i<2;i++) {
        DuelCardRecord *r=i?b:a;
        int part=(i?bw_target:bw_record)==battle_record[0]?0:1;
        int diff=at[1-i]-at[i];
        /* Include this one-time coin exchange in the later native-style
           ATK number climb; never animate the permanent modifier itself. */
        aux_stat_tween.bw_change[part]=diff;
        int swap=mod[i]+diff;
        int def=def_mod[i]-diff;
        if(swap>32767 || swap< -32768 || def>32767 || def< -32768) {
            /* Cannot exchange ATK safely with these modifiers. */
            if(i==1) { /* first participant has already been modified */
                DuelCardRecord *r0=a;
                r0->stat_modifier=(s16)bw_original_mod[0];
                r0->defense_modifier=(s16)bw_original_defmod[0];
                D_8009B170[bw_record==battle_record[0]?0:1]=(u16)bw_original_replay_mod[0];
                gDuel_awSavedDefenseModifier[bw_record==battle_record[0]?0:1]=(s16)bw_original_replay_defmod[0];
            }
            aux_stat_tween.bw_change[0]=aux_stat_tween.bw_change[1]=0;
            bw_swapped=0;
            return;
        }
        bw_original_mod[i]=mod[i];bw_original_defmod[i]=def_mod[i];
        bw_original_replay_mod[i]=(s16)D_8009B170[part];
        bw_original_replay_defmod[i]=gDuel_awSavedDefenseModifier[part];
        r->stat_modifier=(s16)swap;
        r->defense_modifier=(s16)def;
        D_8009B170[part]=(u16)(s16)swap;
        gDuel_awSavedDefenseModifier[part]=(s16)def;
        bw_swapped_id[i]=r->card_id;
        bw_swapped_data[i]=r->data;
    }
    bw_swapped=1;
}
static void aux_bw_restore(void)
{
    int i;
    if(!bw_swapped)return;
    for(i=0;i<2;i++){
        int idx=i?bw_target:bw_record;
        int part=idx==battle_record[0]?0:1;
        D_8009B170[part]=(u16)(s16)bw_original_replay_mod[i];
        gDuel_awSavedDefenseModifier[part]=(s16)bw_original_replay_defmod[i];
        if(aux_monster_slot(idx)){
            DuelCardRecord *r=&D_801A7AD8[idx];
            if(r->card_id==bw_swapped_id[i] && r->data==bw_swapped_data[i]) {
                r->stat_modifier=(s16)bw_original_mod[i];
                r->defense_modifier=(s16)bw_original_defmod[i];
            }
        }
    }
    bw_swapped=0;
}
static int aux_bw_immune(int record)
{
    int id=D_801A7AD8[record].card_id;
    /* Universal card tags, and Ritual Effects' Ultimate Dragon protection.
       Cross-mod immunity extensions can add the same card tags. */
    return Cards_HasTag(id,"immune_effects") ||
           Cards_HasTag(id,"immune_monster_effects") ||
           Cards_HasTag(id,"effect_immune") || id==380;
}
static void aux_bw_prepare(void)
{
    int i;
    bw_coin_started=0;bw_coin_pending=0;bw_coin_ready=0;
    bw_coin_used=0;bw_success=0;bw_swapped=0;
    bw_record=bw_target=-1;
    if(battle_record[0]<0 || battle_record[1]<0)return;
    for(i=0;i<2;i++){
        int id=battle_card[i],opp=battle_card[i^1];
        if(!Cards_Valid(opp) || !Cards_Valid(id) ||
           !aux_warrior_coin_eligible(id==AUX_BATTLE_WARRIOR,
                                     Cards_Level(opp),aux_bw_immune(battle_record[i^1])))continue;
        bw_record=battle_record[i];bw_target=battle_record[i^1];
        bw_coin_side=bw_record/AUX_SIDE_STRIDE;
        break;
    }
}
/* Build the presentation delta BEFORE the native state-2 card panels are
   created.  Normal Guardian Star adjustment stays with the native state-4
   handler.  These numbers modify only the card panels, not duel stats. */
static void aux_stat_tween_prepare(void)
{
    int p,now[2][2],valid=0;
    if(D_8009B22A) {
        /* Preserve the original Round02 trap-abort state transition. */
        r2_prepare(2);r3_prepare(2);
        return;
    }
    if(aux_stat_tween.ready)return;
    for(p=0;p<2;p++) {
        int slot=battle_record[p];
        if(aux_monster_slot(slot)&&D_801A7AD8[slot].card_id==battle_card[p]&&
           D_801A7AD8[slot].data==battle_saved_data[p]) {
            s32 v=Duel_CalcCardStats(&D_801A7AD8[slot]);
            now[p][0]=(u16)v;now[p][1]=(u16)((unsigned)v>>16);
            valid|=1<<p;
        } else now[p][0]=now[p][1]=0;
    }
    r2_prepare(2);
    r3_prepare(2);
    for(p=0;p<2;p++)if(valid&(1<<p)) {
        int slot=battle_record[p],c;
        s32 v=Duel_CalcCardStats(&D_801A7AD8[slot]);
        int finish[2]={(u16)v,(u16)((unsigned)v>>16)};
        for(c=0;c<2;c++) {
            int extra=(c==0?aux_stat_tween.bw_change[p]:0);
            aux_stat_tween.initial[p][c]=now[p][c]-extra;
            aux_stat_tween.change[p][c]=finish[c]-aux_stat_tween.initial[p][c];
        }
    }
    aux_stat_tween.amount=aux_tween_amount(&aux_stat_tween);
    aux_stat_tween.ready=1;
}
/* The card panel's native ATK/DEF digits read the two modifier halfwords.
   Only those are interpolated, using Stars_DisplayStep (the exact pace of
   the Guardian Star presentation in the port).  Actual stats and native
   Guardian Star display bonuses are left intact. */
static void aux_stat_tween_draw(void)
{
    int p,c;
    for(p=0;p<2;p++)for(c=0;c<2;c++) {
        s16 *v=c?&D_800EA0E8[p].field_38:&D_800EA0E8[p].field_36;
        int result=aux_stat_tween.base_bonus[p][c]+
            aux_tween_offset(aux_stat_tween.change[p][c],
                             aux_stat_tween.progress,aux_stat_tween.amount);
        if(result>32767)result=32767;
        if(result<-32768)result=-32768;
        *v=(s16)result;
    }
}
static void aux_stat_tween_start(void)
{
    int p;
    if(!aux_stat_tween.ready || aux_stat_tween.started ||
       !aux_stat_tween.amount || D_8009B22A)return;
    aux_stat_tween.started=aux_stat_tween.active=1;
    aux_stat_tween.progress=0;
    aux_stat_tween.step=Stars_DisplayStep(aux_stat_tween.amount);
    if(aux_stat_tween.step<=0)aux_stat_tween.step=16;
    for(p=0;p<2;p++) {
        aux_stat_tween.base_bonus[p][0]=D_800EA0E8[p].field_36;
        aux_stat_tween.base_bonus[p][1]=D_800EA0E8[p].field_38;
    }
    aux_stat_tween_draw();
}
static void aux_stat_tween_tick(void)
{
    aux_tween_advance(&aux_stat_tween);
    aux_stat_tween_draw();
    if(aux_stat_tween.progress>=aux_stat_tween.amount)
        aux_stat_tween.active=0;
}
static void aux_battle(void)
{
    int was,phase;
    if(!(gDuel_wSceneStateFlags&DUEL_SCENE_FLAG_INITIALIZED) &&
       !battle_protection_pending)
        aux_capture_battle();
    was=D_8009B174&15;
    if(aux_stat_tween.active) {
        /* Pause only the battle state machine, not frames/rendering. */
        if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 && !D_8009B22A) {
            aux_stat_tween_tick();
            return;
        }
        aux_stat_tween.active=0;
    }
    if(bw_record>=0 && !bw_coin_started) {
        bw_coin_started=1;
        if(AuxBattleCoinFlip_Request(0,-1,aux_bw_done,0)) {
            bw_coin_pending=1;
            return; /* Own animation pauses this battle's entry. */
        }
        bw_success=aux_host && aux_host->now_us &&
            ((aux_host->now_us(aux_host)>>9)&1)==0;
        bw_coin_ready=1;
    }
    if(bw_coin_pending)return;
    if(bw_coin_ready) { aux_bw_swap();bw_coin_ready=0; }
    if(wall_attacker_record>=0) {
        if(D_8009B22A)wall_attack_negated=1;
        /* Native phase 3 resolves damage for both direct and defended hits.
           Do not trigger for an attack stopped earlier by a trap. */
        if(was==3 && !wall_attack_negated)wall_attack_resolved=1;
    }
    /* Redirect the 2D burning presentation while the battlefield snapshots
       still exist. This has to precede the original's state-10 code. */
    if(battle_protection_pending && was==10)
        aux_battle_skip_2d_fire();
    /* In state 11, record who lacks a native result panel before native
       releases those temporary objects. */
    if(battle_protection_pending && (D_8009B174&15)==11 && !D_8009B22A) {
        int i;
        for(i=0;i<2;i++) {
            if(!D_800E9EF0[i+2] && aux_battle_guarded_record(battle_record[i]))
                battle_restore|=1u<<i;
        }
    }
    if(r3_coin_pause())return;
    if(was==2)aux_stat_tween_prepare();
    else {r2_prepare(was);r3_prepare(was);}
    r2_battle.in_resolution=(was==3);
    /* Only panel initialization in native state 2 sees pre-effect stats.
       Every other calculator use, including combat damage, sees the true
       already-prepared result, independent of the display animation. */
    aux_stat_tween.show_initial=was==2 && aux_stat_tween.ready && aux_stat_tween.amount>0;
    original_battle();
    aux_stat_tween.show_initial=0;
    if(was==4 && (D_8009B174&15)!=4 &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9)
        aux_stat_tween_start();
    r2_after_battle_step(was);r3_after_step(was);
    if(was==3 && !battle_lp_reported &&
       (battle_card[0]==AUX_MUSICIAN_KING || battle_card[1]==AUX_MUSICIAN_KING) &&
       (battle_guarded[0] || battle_guarded[1])) {
        int side=D_8009B1D5;
        int p;
        battle_lp_reported=1;
        for(p=0;p<2;p++){
            int v=D_8009B1A4[p];
            if(v<0 && v!= -1) {
                int damaged=p==0?side:side^1;
                battle_expected_loss[damaged]=-v;
            }
        }
    }
    if(battle_special_attacker && battle_record[1]>=0 && !D_8009B22A &&
       D_800E9EF0[0] && D_800E9EF0[1] &&
       ((was==3 && D_8009B1B0[1]<0) ||
        (was==5 && D_8009B208==battle_record[1]))) {
        int delta=func_8001EFD4(D_800E9EF0[0],D_800E9EF0[1]);
        if(delta>0) {
            battle_attack_won=1;
            if(battle_special_attacker==AUX_RABID_HORSEMAN &&
               (battle_saved_flags[1]&DUEL_CARD_FLAG_DEFENSE_POSITION))
                battle_pierce_amount=aux_piercing(delta,0,1,1,1);
        }
    }
    phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
    /* 3D burn: state 5 produces the animation death flags independently of
       the 2D fire request. Suppress them immediately after native setup. */
    if(battle_protection_pending && was==5 && phase==9 && !D_8009B22A) {
        int i;
        for(i=0;i<2;i++) {
            DisplayObject *o=D_800E9EF0[i];
            if(o && aux_battle_guarded_record((int)o->field_6A) && D_800EF658[i].field_02) {
                D_800EF658[i].field_02=0;
                battle_3d_suppressed|=1u<<i;
                if(aux_host && aux_host->log)
                    aux_host->log(aux_host,"[Fusion Auxiliary Balance] Seiyaryu suppressed 3D death: part=%d",i);
            }
        }
    }
    if(battle_protection_pending)
        aux_sanitize_battle_replay();
    /* Native state 11 may end straight into phase 5; do not leave the
       logically protected card unoccupied when its result panel disappeared. */
    if(battle_protection_pending && phase!=9) {
        r2_battle.prepared=0;
        aux_bw_restore();
        aux_normalize_musician_battle_damage();
        aux_battle_finish();
        if(phase==5)aux_finish_attack_effect();
        /* Phase 5 is already back on the field. Phase 11 (resume) will
           restore native saved flags first, then apply the new defense. */
        if(phase==5)aux_wall_return_to_defense();
        else if(phase!=11) {
            wall_attacker_record=-1;
            wall_attacker_data=0;
            wall_attack_resolved=wall_attack_negated=0;
        }
        /* Leave the battle snapshot alive through resume if phase 11 is
           reached; otherwise clear it only after the final restoration. */
        if(phase!=11) {
            r2_end();r3_end();
            battle_protection_pending=0;
            battle_record[0]=battle_record[1]=-1;
            battle_2d_skipped=battle_restore=battle_3d_suppressed=0;
        }
    }
}
static void aux_resume(void)
{
    /* Restore the field first. The FIRST resume tick reconstructs its cards
       (the old code only checked AFTER the INITIALIZED flag was set), and
       can already make a replay eligible for removal on the next tick. */
    if(battle_protection_pending)
        aux_sanitize_battle_replay();
    original_resume();
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==11)
        aux_sanitize_battle_replay();
    else {
        aux_bw_restore();
        aux_normalize_musician_battle_damage();
        aux_battle_finish();
        if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5)aux_finish_attack_effect();
        if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5)
            aux_wall_return_to_defense();
        else {
            wall_attacker_record=-1;
            wall_attacker_data=0;
            wall_attack_resolved=wall_attack_negated=0;
        }
        r2_end();r3_end();
        aux_stat_tween.active=0;
        battle_protection_pending=0;
        battle_record[0]=battle_record[1]=-1;
        battle_2d_skipped=battle_restore=battle_3d_suppressed=0;
    }
}

static void aux_reset(void)
{
    r2_reset();r2_full_reset();r3_reset();r3_full_reset();
    memset(&aux_stat_tween,0,sizeof(aux_stat_tween));
    aux_new_effects_reset();
    mass_destruction_active=removal_context=ai_scoring=battle_protection_pending=0;
    kamion_cpu_learned[0]=kamion_cpu_learned[1]=0;
    aux_bw_restore();
    battle_lp_reported=0;
    bw_coin_pending=bw_coin_ready=bw_coin_used=bw_coin_started=0;
    bw_record=bw_target=-1;bw_success=0;
    bwcoin.active=0;
    aux_equip_tracking=0;
    mass_visual_repair_pending=0;
    mass_seiyaryu_present[0]=mass_seiyaryu_present[1]=0;
    battle_record[0]=battle_record[1]=-1;
    battle_card[0]=battle_card[1]=0;
    battle_guarded[0]=battle_guarded[1]=0;
    battle_saved_flags[0]=battle_saved_flags[1]=0;
    battle_saved_data[0]=battle_saved_data[1]=0;
    battle_2d_skipped=battle_restore=battle_3d_suppressed=0;
    wall_attacker_record=-1;
    wall_attacker_data=0;
    wall_attack_resolved=wall_attack_negated=0;
    bonus_hits_queued=bonus_hit_playing=0;
    bonus_hit_side=bonus_hit_amount=bonus_hit_spell=0;
    metal_pending_result=metal_pending_side=metal_placement_check=0;
    fusion_pending_result=fusion_pending_side=placement_was_fusion=0;
    metal_spell_context=metal_spell_side=0;
    aux_fx_count=aux_fx_playing=0;
    battle_special_attacker=battle_attack_won=battle_pierce_amount=0;
    memset(aux_generated_magic_present,0,sizeof(aux_generated_magic_present));
    aux_darkfire_generated_turn[0]=aux_darkfire_generated_turn[1]=0;
    {
        int i;
        for(i=0;i<30;i++){
            metal_immune[i]=0;
            metal_bound_id[i]=0;
            metal_bound_data[i]=0;
            fusion_origin[i]=0;fusion_bound_id[i]=0;fusion_bound_data[i]=0;
            aux_elf_healed[i]=0;aux_elf_bound_id[i]=0;aux_elf_bound_data[i]=0;
            aux_equip_ledger[i].card=0;aux_equip_ledger[i].data=0;
            aux_equip_ledger[i].atk=aux_equip_ledger[i].def=0;
            aux_equip_ledger[i].boosted=0;
            mass_visual_marked[i]=0;
            mass_visual_id[i]=0;
            mass_visual_data[i]=0;
        }
    }
}

/* F8 0B F0-F5 are reserved solely inside this MOD's descriptions for the
   retail LIGHT/DARK/EARTH/WATER/FIRE/WIND attribute balls. The text engine
   already lays each marker out as a 16x16/two-letter icon. The normal
   renderer can then draw the exact ball through its custom-type icon bank,
   with no shared renderer overwrite or direct dependency on other MODs.
   Six high card-type ids are icon-only: NO monster is assigned those types. */
static void aux_attribute_icon_text_entry(DuelEffectChannel *channel,s32 value)
{
    int id=gDuel_wSelectedCardID;
    if(channel && (channel->flags_34&0x80) && value>=0xF0 && value<=0xF5 &&
       (id==531 || id==473 || id==389)) {
        TypeIcons_MarkIcon(249 + (int)(value-0xF0));
    }
    original_append_text_entry(channel,value);
}

#include "owner_f.h"
#define EA_HOST aux_host
#define EA_TERRAIN gDuel_bTerrain[0]
#include "native.h"
#include "native_actions.h"
static EaBoard ea_combo;static int ea_has_combo;
static void ea_start_combo(void){ea_has_combo=ea_native();if(ea_has_combo)ea_combo=ea_capture(0);}
static int ea_rank_combo(int id){
 if(!ea_has_combo||!Cards_Valid(id))return 0;EaBoard n=ea_combo;EaCard c=ea_card(id,-1,D_8009B1D5,0);
 int depth=gAiScript_State.fusion_depth,p=-1,v=0;if(depth>4)return 0;
 for(int k=0;k<=depth;k++){int x=gAiScript_State.fusion_path[k];if(x>=1&&x<=5)p=x-1;if(x>=11&&x<=15)n.hand[x-11]=(EaCard){0};}
 if(p<0){for(int k=0;k<5;k++)if(!n.row[0][k].id){p=k;break;}}if(p<0)return 0;
 int formed=aux_ai_depth&&aux_ai_next.id==id;if(formed){
 if(aux_ai_next.fused||aux_ai_next.egotist)c.active|=c.possible&EA_FUSION;if(aux_ai_next.metal||aux_ai_next.egotist)c.active|=c.possible&EA_METAL;}
 v-=ea_keep(&ea_combo,0,p);n.row[0][p]=c;v+=ea_keep(&n,0,p);
 for(int k=0;k<5;k++)if(n.row[0][k].id>0)for(int d=0;d<2;d++)v+=(ea_aura(&n,0,k,d)-(k==p?0:ea_aura(&ea_combo,0,k,d)))/3;
 if(formed&&ea_f(c,713))v+=n.lp[1]<=1000?6500:1000;if(formed&&ea_f(c,217))v+=400*(5-ea_free(n.row[1]));if(formed&&ea_f(c,458))v+=350*(5-ea_free(n.row[1]));
 int t=0;for(int j=0;j<5;j++)if(n.row[1][j].id>0){EaResult f=ea_fight(&n,0,p,j,0);if(f.relevant&&f.legal&&f.kill&&f.score>0&&c.at<=(n.row[1][j].flags&2048?n.row[1][j].df:n.row[1][j].at))t=ea_max(t,ea_min(800,f.score/4));}return v+t;}
int MemoriesModInit(const MemoriesModHost *host,MemoriesMod *mod)
{
    if(!host || !mod || host->api<4 || !host->hook || !host->subscribe) return 0;
    aux_host=host;
    bwcoin_load(0);
    if(!bwcoin_sets[0].count) {
        if(host->log)host->log(host,"[Fusion Auxiliary Balance] Battle Warrior coin artwork missing");
        return 0;
    }
    bw_port_menu=(int (*)(void))host->symbol(host,"Menu_IsOpen");
    bw_port_notice=(int (*)(void))host->symbol(host,"Menu_NoticeShown");
    mod->frame=aux_coin_frame;
    mod->overlay=aux_coin_overlay;
    mod->overlay_signature=aux_coin_signature;
    mod->shutdown=aux_coin_shutdown;
    mod->api=MEMORIES_MOD_API;
    mod->name="Balanceamento de Fusoes e Efeitos Auxiliares";
    mod->reset=aux_reset;mod->applied=ea_apply;
    if(!host->subscribe(host,MEMORIES_EVENT_FUSION,60,aux_fusion)) {
        if(host->log) host->log(host,"fusion-auxiliary-balance: cannot subscribe FUSION");
        return 0;
    }
    if(!host->provide || !host->provide(host,"encore-v1",(void *)&aux_encore_api))
        return 0;
    if(!host->provide(host,"origin-unlock-v1",(void *)&aux_unlock_api))return 0;
    if(!host->provide(host,"origin-effects-v1",(void *)&aux_origin_effects_api))return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_EQUIP,0,aux_egotist_event))return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_EQUIP,-100,aux_equip_event))
        return 0;
    if(!host->subscribe(host,MEMORIES_EVENT_DAMAGE,50,aux_damage)) {
        if(host->log) host->log(host,"fusion-auxiliary-balance: cannot subscribe DAMAGE");
        return 0;
    }
#define AUX_HOOK(name, replacement, original) do { \
    if(!host->hook(host,(void *)(name),(void *)(replacement),(void **)&(original))) { \
        if(host->log) host->log(host,"fusion-auxiliary-balance: hook rejected: " #name); \
        return 0; \
    } \
} while(0)
    if(!host->provide(host,"effect_forecast_v2",(void*)&ea_owner))return 0;
    AUX_HOOK(AiScript_FindBestAttack,ea_best,ea_old_best);
    AUX_HOOK(AiScript_PlayFieldCard,ea_play,ea_old_play);
    AUX_HOOK(AiScript_LoadGuardianStarChoice,ea_star,ea_old_star);
    AUX_HOOK(AiScript_FindKiller,ea_killer,ea_old_killer);
    AUX_HOOK(AiScript_CalcCardPower,ea_power,ea_old_power);
    AUX_HOOK(Duel_CheckFusion,aux_check_fusion,original_fusion);
    AUX_HOOK(func_80023144,aux_viewer_info,original_aux_viewer_info);
    AUX_HOOK(func_80035E20,aux_viewer_draw,original_aux_viewer_draw);
    AUX_HOOK(DuelEffect_AppendEntry,aux_attribute_icon_text_entry,original_append_text_entry);
    AUX_HOOK(Duel_CheckEquip,aux_check_equip,original_equip);
    AUX_HOOK(Ai_CompleteFusion,aux_complete_fusion,original_complete_fusion);
    AUX_HOOK(Duel_CalcCardStats,aux_calc_stats,original_calc_stats);
    AUX_HOOK(func_8001EFD4,r2_compare,r2_original_compare);
    AUX_HOOK(DuelEffect_ApplyLifePointRecovery,aux_healing,original_heal);
    AUX_HOOK(DuelCard_RemoveFromField,aux_remove,original_remove);
    AUX_HOOK(DuelEffect_ApplyBoardDestruction,aux_board,original_board);
    AUX_HOOK(DuelEffect_ApplyRaigeki,aux_raigeki,original_raigeki);
    AUX_HOOK(DuelEffect_ApplyMonsterRemoval,aux_monster_removal,original_monster_removal);
    AUX_HOOK(DuelScene_UpdateBattle,aux_battle,original_battle);
    AUX_HOOK(DuelScene_UpdateTurnSwitch,aux_darkfire_turn_switch,original_turn_switch);
    AUX_HOOK(DuelCard_UpdateDefenseRotation,aux_rotate,original_rotate);
    AUX_HOOK(DuelScene_UpdateResume,aux_resume,original_resume);
    AUX_HOOK(func_80024D34,aux_restore_generated_card,original_restore_generated_card);
    AUX_HOOK(DuelScene_Update,aux_scene,original_scene);
    AUX_HOOK(DuelScene_UpdateHandActions,aux_hand_actions,original_hand_actions);
    AUX_HOOK(DuelScene_UpdateCardPlacement,aux_card_placement,original_card_placement);
    AUX_HOOK(func_8001B170,aux_card_finalize,original_card_finalize);
    AUX_HOOK(DuelEffect_UpdateCardEffect,aux_card_effect_update,original_card_effect_update);
    AUX_HOOK(DuelScene_UpdateFieldActions,aux_field_actions,original_field_actions);
    AUX_HOOK(DuelEffect_UpdateRequests,aux_update_requests,original_update_requests);
    AUX_HOOK(Duel_GetBaseCardStat,aux_base_stat,original_base_stat);
    AUX_HOOK(AiScript_FindBestCombo,aux_find_combo,original_ai_combo);
    AUX_HOOK(AiScript_EvaluateFusion,aux_evaluate_combo,original_ai_evaluate);
#undef AUX_HOOK
    return 1;
}
