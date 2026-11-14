#include "pc/mods/modapi.h"
#include "pc/render/texture_dump.h"
#include "pc/render/soft_gpu.h"
#include <stdio.h>
#include "psyq/libgte.h"
#include "psyq/libgpu.h"
#define D_80177EA4_VISIBLE
#define D_8009B360_AS_SIDE_ARRAY
#include "unmatched.h"
#include "game/duel_card.h"
#include "game/duel_display.h"
#define DUEL_CARD_STAGING_DECK_VIEW
#include "game/duel_card_staging.h"
#include "game/duel_card_record_lifecycle.h"
#include "game/duel_card_layout.h"
#include "game/duel_card_display_state.h"
#include "game/duel_apply_card_object_flags.h"
#include "game/func_8001D240.h"
#include "game/duel_deck_card_data.h"
#include "game/duel_ritual_effect.h"
#include "game/duel_check_ritual.h"
#include "game/duel_ritual_controller.h"
#include "game/duel_action_lock.h"
#include "game/duel_side_state.h"
#include "game/duel_scene_field_actions.h"
#include "game/duel_scene_card_placement.h"
#include "game/duel_scene_hand_actions.h"
#include "game/func_8001B938.h"
#include "game/duel_scene_battle.h"
#include "game/main_modes.h"
#include "game/duel_battle_stats.h"
#include "game/duel_scene_callbacks.h"
#include "game/duel_scene_state.h"
#include "game/duel_selection_layout.h"
#include "game/duel_field_display_objects.h"
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "game/input.h"
#include "game/duel_scene_turn_switch.h"
#include "game/duel_phase_entry.h"
#include "game/duel_deck_card.h"
#include "game/duel_card_effects.h"
#include "game/duel_field_effect_steps.h"
#include "game/duel_effect_request.h"
#include "game/duel_effect.h"
#include "game/duel_effect_allocate_request.h"
#include "game/sound.h"
#include "game/duel_card_checks.h"
#include "game/duel_field_equip_search.h"
#include "game/duel_card_can_act_this_turn.h"
#include "game/duel_magic_effect_dispatch.h"
#include "game/duel_effect_tables.h"
#include "game/duel_trap_resolution.h"
#include "game/display_object_work_slots.h"
#include "game/display_object_core.h"
#include "game/func_80016784.h"
#include "game/display_object_packet_submit.h"
#define DUEL_SCREEN_TABLES_TYPED_POSITIONS
#include "game/duel_screen_tables.h"
#include "game/card_constants.h"
#include "game/duel_terrain_boost.h"
#include "game/duel_scene_update.h"
#include "game/func_80022D94.h"
#include "game/duel_side_view_angles.h"
#include "game/view_state.h"
#include "game/duel_effect_resource_record.h"
#include "game/duel_effect_resource_setup.h"
#include "game/func_800291E0.h"
#include "game/func_80029574.h"
#include "game/file_transfer.h"
#include "game/func_8001944C.h"
#include "game/duel_create_card_effect_overlay.h"
#include "game/display_object_helpers.h"
#include "game/duel_card_object_helpers.h"
#include "psyq/libgs.h"
extern u8 gDuel_bTerrain;
#include "pc/cards/tables.h"
#include "pc/cards/cards.h"
#include "pc/audio/spu.h"
#include "pc/audio/replace.h"
#include "pc/cards/art.h"
#include "pc/cards/disc_art.h"
#include "game/ai.h"
extern unsigned char D_800EAE88[];
#include "game/ai_script_commands.h"
#include "game/duel_calc_guardian_star_matchup.h"
#include "game/duel_cursor_status.h"
#include "game/card_preview_callbacks.h"
#define DUEL_CARD_VIEWER_ADDRESS_ALIASES
#include "game/duel_card_viewer.h"
#include "game/duel_effect_command.h"
#include "game/func_80035E20.h"
#include "pc/text/glyphs.h"
#include "pc/text/hd_text.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Exported by the v0.2.0 guest even though the SDK has no dedicated header. */
extern s32 func_80027508(void);

/* Standalone ritual-card effects mod. Ritual recipes are declared in mod.json
 * so the public Tables_RitualRequirements interface is shared with
 * MelhoriaDosRituais (field + hand). The ritual sequencer's initialized
 * state 3 builds the actual result record; mark it only AFTER that call
 * reaches state 4 (player) or 5 (CPU). */
/* Permanent diagnostic switch support.  The release rule still requires a
 * Ritual Summon by default.  When the player enables the TEST setting, every
 * implemented Ritual monster arms its effect from ordinary Monster-Zone
 * presence too.  This keeps future stress tests available without a special
 * recompile while preserving normal behaviour with the setting OFF. */
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
#define FORTRESS_FISH_HAND_DEF 500
#define FORTRESS_FISH_FIELD_DEF 1000
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
/* Approved bite clip is 1.204 s. At the NTSC duel rate, 74 frames ensures
 * the +500 popup/stat commit begins only after both bites have finished. */
#define HUNGRY_BITE_DELAY_FRAMES 74
#define TEMPLE_OF_SKULLS_ID 146
#define COSMO_FEMALE_BONUS 500
/* Results of the validated generic fusion families that explicitly require
 * [Female].  The Cosmo Queen effect treats these results as Female even when
 * the port's historical secondary-type table omitted one of them (notably
 * Mystical Sand).  This is local to Ritual Effects and does not alter fusion
 * classification globally. */
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
extern u8 D_8009B174;
/* A hostile Attack Trap gets its own cinematic before the immune Ultimate's
 * original battle.  Do not mistake the trap's temporary card preview for the
 * defender, or let its destruction request target the attacking Ultimate. */
static int ultimate_trap_cinematic;
typedef struct {
    void *data;
    int card_id;
    int ritual;
    int in_battle;
    unsigned attack_count;
    unsigned def_visual_shown;
    unsigned protect_visual_shown;
} RitualMark;
static RitualMark marks[DUEL_CARD_RECORD_COUNT];
static unsigned shield_live_mask;
static const MemoriesModHost *host;
/* A borrowed effect is not a Ritual summon. Keep its source separate from
 * the legacy .ritual fields, which are private effect-initialization state. */
typedef struct { unsigned api; unsigned (*lease)(int); void (*revoke)(int); } OriginUnlockV1;
typedef struct { void *data; int id; unsigned serial; } BorrowedOrigin;
static BorrowedOrigin borrowed_origin[30];
static void borrowed_reconcile(void);
static unsigned borrowed_lease(int index)
{
    const OriginUnlockV1 *api=host && host->find ?
        (const OriginUnlockV1 *)host->find(host,"fusion-auxiliary-balance:origin-unlock-v1"):0;
    return api && api->api==1 && api->lease ? api->lease(index):0;
}

/* Only the Crab's unusually long text needs the native viewer fit pass. */
#include "ritual_viewer_description_fit.h"
#include "effect_title_visual.h"
#include "effect_title_instance.h"
static int fiends_mirror_effect_on = 1;

/* Mini-framework de configuracao inspirado no Monster Effects: um master
 * switch + uma chave por carta. As receitas continuam independentes; estas
 * chaves controlam somente os efeitos especiais. */
typedef struct { int card_id; const char *setting_key; const char *name; } RitualEffectDefinition;
static const RitualEffectDefinition ritual_effects[] = {
    { SHIELD_ID, "millennium_shield", "Millennium Shield" },
    { YAMADRON_ID, "yamadron", "Yamadron" },
    { PSYCHO_PUPPET_ID, "psycho_puppet", "Psycho-Puppet" },
    { HUNGRY_BURGER_ID, "hungry_burger", "Hungry Burger" },
    { FIENDS_MIRROR_ID, "fiends_mirror", "Fiend's Mirror" },
    { MASK_ID, "mask_shine_dark", "Mask of Shine & Dark" },
    { JAVELIN_BEETLE_ID, "javelin_beetle", "Javelin Beetle" },
    { GATE_GUARDIAN_ID, "gate_guardian", "Gate Guardian" },
    { CRAB_TURTLE_ID, "crab_turtle", "Crab Turtle" },
    { FORTRESS_WHALE_ID, "fortress_whale", "Fortress Whale" },
    { GARMA_SWORD_ID, "garma_sword", "Garma Sword" },
    { TRI_HORNED_ID, "tri_horned", "Tri-Horned Dragon" },
    { SERPENT_NIGHT_ID, "serpent_night", "Serpent Night Dragon" },
    { PERFORMANCE_ID, "performance_sword", "Performance of Sword" },
    { CHAOS_ID, "black_chaos", "Magician of Black Chaos" },
    { SUPER_WAR_LION_ID, "super_war_lion", "Super War-Lion" },
    { SENGENJIN_ID, "sengenjin", "Sengenjin" },
    { BLACK_LUSTER_SOLDIER_ID, "black_luster_soldier", "Black Luster Soldier" },
    { BLUE_EYES_ULTIMATE_ID, "blue_eyes_ultimate", "Blue-Eyes Ultimate Dragon" },
    { COSMO_QUEEN_ID, "cosmo_queen", "Cosmo Queen" },
    { DOKURORIDER_ID, "dokurorider", "Dokurorider" },
    { CHAKRA_ID, "chakra", "Chakra" },
    { ZERA_ID, "zera", "Zera the Mant" },
    { SKULL_GUARDIAN_ID, "skull_guardian", "Skull Guardian" }
};
enum {
    EFFECT_SHIELD, EFFECT_YAMADRON, EFFECT_PSYCHO, EFFECT_HUNGRY, EFFECT_MIRROR,
    EFFECT_MASK, EFFECT_JAVELIN, EFFECT_GATE, EFFECT_CRAB, EFFECT_WHALE,
    EFFECT_GARMA, EFFECT_TRI, EFFECT_SERPENT, EFFECT_PERFORMANCE, EFFECT_CHAOS,
    EFFECT_WAR_LION, EFFECT_SENGENJIN, EFFECT_BLS, EFFECT_ULTIMATE,
    EFFECT_COSMO, EFFECT_DOKURORIDER, EFFECT_CHAKRA, EFFECT_ZERA, EFFECT_SKULL, EFFECT_COUNT
};
static int test_ignore_ritual_requirement;
static int test_cpu_prioritize_traps;
static int diagnostic_ai_positions;
/* OFF by default. This traces the Novox's Prayer path without overriding
 * any successful or failed Ritual result. Snapshot persists for screenshots. */
static int diagnostic_skull_ritual;
/* Optional runtime trace. Not consulted by effect controllers and OFF by
 * default. Captures the ACTUAL native draw callback, not synthetic test
 * entries, so the next screenshot can distinguish state/mode from glyph
 * matching without touching combat logic. */
static int diagnostic_effect_titles;
static struct {
    unsigned calls, heading_matches;
    int id, mode, slot, armed, active, channel, text_color;
} title_color_trace;

typedef struct {
    unsigned attempts;
    int stage, side, phase, candidates, row_found, recipe_on;
    unsigned matches[3];
    unsigned short materials[3];
    int retail_result;
    unsigned oracle_checks,oracle_accepted,oracle_rejected;
    int last_oracle_result;
    unsigned oracle_candidates_seen,oracle_signature_hits;
    unsigned short oracle_last_trio[3];
    unsigned char oracle_requirement_masks[3];
    unsigned deck_candidates,deck_matches[3];
} SkullRitualDiagnostic;
static SkullRitualDiagnostic skull_ritual_diag;
typedef struct {
    void *data[DUEL_CARD_RECORD_COUNT];
    s16 id[DUEL_CARD_RECORD_COUNT];
    int valid,own_id,own_atk,own_def,opp_id,opp_atk,opp_def,phase,turn;
    unsigned count;
} AiPositionDiagnostic;
static AiPositionDiagnostic ai_position_diag;
static unsigned char effect_settings[EFFECT_COUNT];
static inline int ritual_effect_index_for_card(int card_id)
{
    switch(card_id) {
    case SHIELD_ID: return EFFECT_SHIELD;
    case YAMADRON_ID: return EFFECT_YAMADRON;
    case PSYCHO_PUPPET_ID: return EFFECT_PSYCHO;
    case HUNGRY_BURGER_ID: return EFFECT_HUNGRY;
    case FIENDS_MIRROR_ID: return EFFECT_MIRROR;
    case MASK_ID: return EFFECT_MASK;
    case JAVELIN_BEETLE_ID: return EFFECT_JAVELIN;
    case GATE_GUARDIAN_ID: return EFFECT_GATE;
    case CRAB_TURTLE_ID: return EFFECT_CRAB;
    case FORTRESS_WHALE_ID: return EFFECT_WHALE;
    case GARMA_SWORD_ID: return EFFECT_GARMA;
    case TRI_HORNED_ID: return EFFECT_TRI;
    case SERPENT_NIGHT_ID: return EFFECT_SERPENT;
    case PERFORMANCE_ID: return EFFECT_PERFORMANCE;
    case CHAOS_ID: return EFFECT_CHAOS;
    case SUPER_WAR_LION_ID: return EFFECT_WAR_LION;
    case SENGENJIN_ID: return EFFECT_SENGENJIN;
    case BLACK_LUSTER_SOLDIER_ID: return EFFECT_BLS;
    case BLUE_EYES_ULTIMATE_ID: return EFFECT_ULTIMATE;
    case COSMO_QUEEN_ID: return EFFECT_COSMO;
    case DOKURORIDER_ID: return EFFECT_DOKURORIDER;
    case CHAKRA_ID: return EFFECT_CHAKRA;
    case ZERA_ID: return EFFECT_ZERA;
    case SKULL_GUARDIAN_ID: return EFFECT_SKULL;
    default: return -1;
    }
}
static inline int ritual_card_effect_enabled(int card_id)
{
    int i=ritual_effect_index_for_card(card_id);
    return i>=0 ? effect_settings[i] : 0;
}

/* Private recipes are published by ritual_recipe_compat.h. */
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
/* Host settings are committed through applied(), never polled per card. */
static void refresh_settings(void)
{
    unsigned i;
    for(i=0;i<sizeof(effect_settings);++i)
        effect_settings[i]=!host||!host->setting||host->setting(host,ritual_effects[i].setting_key,1)!=0;
    for(i=0;i<sizeof(recipe_settings);++i)
        recipe_settings[i]=!host||!host->setting||host->setting(host,ritual_options[i].recipe_key,1)!=0;
    /* Public-default gameplay: Ritual effects require a real Ritual Summon.
     * New key intentionally discards an old tester's saved ON value for the
     * deprecated TEST switch; new and upgraded installs start OFF. Players
     * can still opt in explicitly through the regular Settings switch. */
    test_ignore_ritual_requirement=host&&host->setting&&
        host->setting(host,"effects_without_ritual_summon",0)!=0;
    /* Legacy test/diagnostic switches no longer appear in Settings.  Do not
     * read their persisted values: a former tester may have left them ON,
     * and hidden debug modes must never affect public gameplay.  Keep the
     * source for possible future developer builds, but leave it dormant. */
    test_cpu_prioritize_traps=0;
    diagnostic_ai_positions=0;
    diagnostic_skull_ritual=0;
    diagnostic_effect_titles=host&&host->setting&&
        host->setting(host,"title_color_diagnostic",0)!=0;
    fiends_mirror_effect_on=ritual_card_effect_enabled(FIENDS_MIRROR_ID);
}

/* ZIP-only bulk commands. The launcher stages the command and only passes it
 * to the mod after Apply/Reset and Apply.  0 keeps individual settings;
 * 1 writes every switch ON; 2 writes every switch OFF.  The command resets
 * itself to 0 after execution so later individual edits remain independent. */
static void apply_bulk_group(const char *command_key, int recipes)
{
    int command;
    unsigned i;
    if (!host || !host->setting || !host->set_setting) return;
    command = host->setting(host, command_key, 0);
    if (command != 1 && command != 2) return;
    if (recipes) {
        for (i = 0; i < sizeof(ritual_options)/sizeof(ritual_options[0]); ++i)
            host->set_setting(host, ritual_options[i].recipe_key, command == 1 ? 1 : 0);
    } else {
        /* Effect bulk follows the complete effect table, including cards whose
         * Ritual recipe intentionally remains retail (for example BLS). */
        for (i = 0; i < sizeof(ritual_effects)/sizeof(ritual_effects[0]); ++i)
            host->set_setting(host, ritual_effects[i].setting_key, command == 1 ? 1 : 0);
    }
    host->set_setting(host, command_key, 0);
}

static void sync_global_settings(void)
{
    apply_bulk_group("recipes_bulk", 1);
    apply_bulk_group("effects_bulk", 0);
}
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
static void (*original_apply)(void);
static s32 (*original_stats)(DuelCardRecord *);
static void (*original_remove)(DuelCardRecord *);
static void (*original_deactivate)(DuelCardRecord *);
static void (*original_reset_card_records)(void);
static void (*original_populate)(void);
static void (*original_field_actions)(void);
static void (*original_defense_rotation)(DisplayObject *);
static void (*original_card_placement)(void);
static void (*original_hand_actions)(void);
static void (*original_hand_swap)(void);
static void (*original_battle)(void);
static s32 (*original_battle_attack)(DuelCardRecord *, DuelCardRecord *);
static s32 (*original_battle_defense)(DuelCardRecord *, DuelCardRecord *);
static s32 (*original_guardian_star_bonus)(DuelCardRecord *, DuelCardRecord *);
static s32 (*original_battle_compare)(DisplayObject *, DisplayObject *);
static void (*original_func_8001B170)(void);
static void (*original_stat_penalty)(void);
static void (*original_cursebreaker)(void);
static s32 (*original_ai_turn_action)(void);
static s32 (*original_update_card_effect)(void);
static s32 (*original_update_effect_requests)(void);
static void (*original_draw_phase)(void);
static void (*original_board_destruction)(void);
static void (*original_monster_removal)(void);
static void (*original_raigeki)(void);
static void (*original_turn_switch)(void);
static int tokens[56];
static int token_count;
static unsigned summons;
static int inside_field_actions;
static unsigned deferred_deactivation;
static int inside_effect_destruction;
static unsigned protected_visuals;
/* Shared mini-framework helper: cards whose logical destruction was suppressed
 * during an effect are queued here for a single post-resolution visual rebuild.
 * This is intentionally card-agnostic so future effects can reuse the same
 * Black Hole/Raigeki survival recipe. */
static unsigned effect_survivor_visuals;

typedef struct {
    void *data;
    int card_id;
    int ritual;
    int visual_shown;
} YamadronMark;
static YamadronMark yamadron_marks[DUEL_CARD_RECORD_COUNT];
static DuelDeckCardRecord generated_data[DUEL_CARD_RECORD_COUNT];
static unsigned yamadron_summons;
static unsigned yamadron_live_mask;
static unsigned yamadron_generations;
static unsigned deferred_yamadron;
static int card_mrg_lba = -2;
static unsigned char generated_thumbnail_sector[2048];
static unsigned char generated_thumbnail_cache[DUEL_CARD_RECORD_COUNT][DUEL_CARD_DATA_BLOCK_SIZE];
static unsigned generated_thumbnail_valid;
static int generated_thumbnail_touched;

typedef struct {
    void *psycho_data;
    int ritual;
    int psycho_slot;
    int controlling;
    void *stolen_data;
    int stolen_card_id;
    DuelCardRecord stolen_snapshot;
    int original_slot;
    int current_slot;
    int controller_side;
    int pending_summon_trigger;
    int visual_shown;
} PsychoMark;
static PsychoMark psycho_marks[DUEL_CARD_RECORD_COUNT];
static unsigned psycho_controlling_count, psycho_pending_count;
static unsigned psycho_mark_mask, psycho_controlling_mask, psycho_pending_mask;
static unsigned psycho_summons;
static unsigned psycho_steals;

typedef struct {
    void *data;
    int ritual;
    unsigned battle_kills;     /* CPU: battle result; player: native +500 popup start */
    unsigned pending_stacks;   /* pending bite sound and player-side green popup */
    unsigned bite_started;
    int bite_delay;
} HungryMark;
static HungryMark hungry_marks[DUEL_CARD_RECORD_COUNT];
typedef struct { void *data; int x,y,valid; DisplayObject *object; } HungryScreen;
static HungryScreen hungry_screen[DUEL_CARD_RECORD_COUNT];
static unsigned hungry_summons;
static unsigned hungry_live_mask;
static unsigned deferred_hungry;

typedef struct {
    void *data;
    int ritual;
    int protection_used_turn;
    int protection_visual_shown;
    u16 battle_flags;
} JavelinMark;
static JavelinMark javelin_marks[DUEL_CARD_RECORD_COUNT];
static unsigned deferred_javelin;
static unsigned javelin_summons;
static unsigned javelin_live_mask;
static unsigned javelin_protected_visuals;
typedef struct { void *data; int x,y,valid; DisplayObject *object; } JavelinScreen;
static JavelinScreen javelin_screen[DUEL_CARD_RECORD_COUNT];

typedef struct {
    void *data;
    int ritual;
    unsigned attacks_this_turn;
    unsigned extra_attack_pending;
} GateMark;
static GateMark gate_marks[DUEL_CARD_RECORD_COUNT];
static unsigned gate_summons;
/* State 11 lasts multiple frames. This mask makes each Gate Guardian count
 * one completed battle exactly once. */
static unsigned gate_state11_counted_mask;

typedef struct { void *data; int ritual; unsigned blades_used; unsigned extra_attack_pending;
    unsigned extra_attack_ready, strike_bonus, final_attack_cleanup; } GarmaMark;
static GarmaMark garma_marks[DUEL_CARD_RECORD_COUNT];
static unsigned garma_summons;
static unsigned garma_visual_pending;
/* Hot-path gates. Unlike the historical *_summons counters, these are allowed
 * to return to zero once no live instance can affect rendering/stats. New
 * Ritual Summons re-arm them immediately; stable field ticks retire them. */
static unsigned garma_live_hint, tri_live_hint, gate_live_hint;
static unsigned garma_live_mask, tri_live_mask, gate_live_mask;
#define STAT_HINT_SHIELD   (1u << 0)
#define STAT_HINT_HUNGRY   (1u << 1)
#define STAT_HINT_JAVELIN  (1u << 2)
#define STAT_HINT_CRAB     (1u << 3)
#define STAT_HINT_WHALE    (1u << 4)
#define STAT_HINT_TRI      (1u << 5)
static unsigned stat_effect_hints;

/* New v0.77 Ritual instances. These marks follow the same provenance rule as
 * the existing effects: card id alone is never enough; data + record identity
 * must still belong to the Ritual Summon that armed the effect. */
typedef struct {
    void *data;
    unsigned ritual;
    unsigned beast_count;
    unsigned learned_sides;
    u16 battle_flags;
} WarLionMark;
static WarLionMark war_lion_marks[DUEL_CARD_RECORD_COUNT];
static unsigned war_lion_live_mask;
static unsigned deferred_war_lion;

typedef struct { void *data; unsigned ritual; unsigned learned_sides; u16 battle_flags; } SengenjinMark;
static SengenjinMark sengenjin_marks[DUEL_CARD_RECORD_COUNT];
static unsigned sengenjin_live_mask;
static unsigned sengenjin_live_hint;
static unsigned sengenjin_battle_visual_mask;
/* Native v0.2.0 stores both Equip boosts and Spellbinding/Shadow penalties in
 * the same signed stat_modifier.  Sengenjin must ignore the positive source
 * while retaining the negative source even when the two have already netted
 * together.  Track only native negative effects, keyed by card identity; this
 * ledger never changes the card's real stats and is consulted only for a
 * battle involving Ritual Sengenjin. */
typedef struct { void *data; u16 card_id; s16 negative; } SengenjinNegativeMark;
static SengenjinNegativeMark sengenjin_negative[DUEL_CARD_RECORD_COUNT];
typedef struct {
    int active;
    int effect_id;
    unsigned mask;
    unsigned touched;
    DuelCardRecord saved[DUEL_CARD_RECORD_COUNT];
} SengenjinTrapState;
static SengenjinTrapState sengenjin_trap;

typedef struct { void *data; unsigned ritual; } BlsMark;
static BlsMark bls_marks[DUEL_CARD_RECORD_COUNT];
static unsigned bls_live_mask;
static unsigned bls_live_hint;
static unsigned bls_new_source_pending;
static u8 bls_prev_sources[DUEL_CARD_RECORD_COUNT];
static void *bls_prev_data[DUEL_CARD_RECORD_COUNT];
static u16 bls_prev_card_id[DUEL_CARD_RECORD_COUNT];
static unsigned bls_field_pending;
/* Ritual-only Zera instances own a live, global two-sided stat penalty. */
typedef struct { void *data; unsigned ritual; } ZeraMark;
static ZeraMark zera_marks[DUEL_CARD_RECORD_COUNT];
static unsigned zera_live_mask;
static unsigned zera_field_pending;
/* Ritual-only Skull Guardian bonuses are derived, not written to stat_modifier.
 * last_bonus is the most recently announced total, not a cached gameplay stat. */
typedef struct { void *data; unsigned ritual, last_bonus; } SkullGuardianMark;
static SkullGuardianMark skull_marks[DUEL_CARD_RECORD_COUNT];
static unsigned skull_live_mask;

#define STAT_HINT_WAR_LION (1u << 6)
#define STAT_HINT_BLS      (1u << 7)
#define STAT_HINT_COSMO    (1u << 8)
#define STAT_HINT_DOKU     (1u << 9)
#define STAT_HINT_CHAKRA   (1u << 10)
#define STAT_HINT_ZERA     (1u << 11)
#define STAT_HINT_SKULL    (1u << 12)
#define STAT_HINT_GARMA    (1u << 13)

typedef struct { void *data; unsigned ritual; u16 battle_flags; } UltimateMark;
static UltimateMark ultimate_marks[DUEL_CARD_RECORD_COUNT];
static unsigned ultimate_live_mask;
static unsigned ultimate_live_hint;
/* Short-lived test-build telemetry for the repeatedly reported CPU/Swords
 * case. Render only during the relevant CPU turn, never during normal play. */
static unsigned ultimate_swords_diag_scans,ultimate_swords_diag_issued;
static int ultimate_swords_diag_last_result;
typedef struct {
    int active;
    int effect_id;
    int source_side;
    unsigned mask;
    unsigned touched;
    DuelCardRecord saved[DUEL_CARD_RECORD_COUNT];
} UltimateEffectState;
static UltimateEffectState ultimate_card_effect;
/* Remember an actual CPU Magic activation against an opposing, effect-active
 * Ultimate. Some native effect handlers filter immune cards before mutating
 * the records, so the snapshot's `touched` bit alone cannot teach the CPU
 * about those failed attempts (Dark Hole also skips that snapshot entirely).
 * The observation is committed only when the real card effect finishes. */
typedef struct {
    int active,source_side,card_id;
    unsigned target_mask;
    void *target_data[DUEL_CARD_RECORD_COUNT];
} UltimateAiMagicAttempt;
static UltimateAiMagicAttempt ultimate_ai_magic_attempt;
/* Effect knowledge is card-wide, not symptom-by-symptom.
 * - When a CPU side itself puts an effect-enabled Ritual monster in play, that
 *   CPU knows the card's complete effect package immediately: it owns the card.
 * - Merely seeing the opponent put that card in play teaches nothing.
 * - The first real setback caused by that unknown effect teaches the complete
 *   package for the rest of the duel.
 * Knowledge never grants an inactive instance the effect: every tactical use
 * still checks that exact field instance's Ritual/effect-active identity. */
#define RITUAL_AI_KNOWLEDGE_CARD_LIMIT 1024
#define RITUAL_AI_KNOWLEDGE_WORDS (RITUAL_AI_KNOWLEDGE_CARD_LIMIT / 32)
static u32 ritual_ai_effect_known[DUEL_SIDE_COUNT][RITUAL_AI_KNOWLEDGE_WORDS];

typedef struct { void *data; unsigned ritual; unsigned female_count; } CosmoMark;
static CosmoMark cosmo_marks[DUEL_CARD_RECORD_COUNT];
static unsigned cosmo_live_mask;
static unsigned cosmo_live_hint;
/* Queen Lock is a once-per-turn obligation for each side.  It is armed by a
 * live opposing Ritual Cosmo Queen plus a legal Female in the five physical
 * hand slots, and is satisfied when one of those real Female cards actually
 * leaves the hand as the committed play. */
static unsigned char cosmo_turn_fulfilled[DUEL_SIDE_COUNT];

typedef struct { void *data; unsigned ritual; unsigned learned_sides; u16 battle_flags; } DokuroriderMark;
static DokuroriderMark dokurorider_marks[DUEL_CARD_RECORD_COUNT];
static unsigned dokurorider_live_mask;
static unsigned dokurorider_live_hint;
static unsigned dokurorider_new_source_pending;
/* Chakra never writes stat_modifier: its one-time Ritual bonus changes ATK
 * only and is owned by this exact card instance. Sengenjin can strip this
 * positive layer for one battle without consuming the stored bonus. */
typedef struct { void *data; unsigned ritual; unsigned bonus; } ChakraMark;
static ChakraMark chakra_marks[DUEL_CARD_RECORD_COUNT];
static unsigned chakra_live_mask, chakra_pending_mask;
typedef struct {
    int active, index, side, next, count;
    int stage, hold, timeout, popup_seen, cam_x, cam_z, cam_angle, cam_value;
    int watchdog; /* Hard-stop if a visual or camera request never completes. */
    int camera_active; /* Own a camera-only native tween, never a field-panel request. */
    DuelEffectRequest *lightning; /* The actual retail Raigeki 0x10 VFX request. */
    int lightning_started; /* Never restart a naturally ended/aborted request. */
    void *data;
    unsigned total_atk, destroyed, bonus;
    u8 targets[DUEL_FIELD_ROW_SIZE];
} ChakraSequence;
static ChakraSequence chakra_sequence;

static unsigned dokurorider_evade_pending;
static unsigned dokurorider_evade_hold;
static u8 dokurorider_prev_sources[DUEL_CARD_RECORD_COUNT];
static void *dokurorider_prev_data[DUEL_CARD_RECORD_COUNT];
static u16 dokurorider_prev_card_id[DUEL_CARD_RECORD_COUNT];

typedef struct { void *data; unsigned ritual; unsigned battle_flags; } SeaRitualMark;
static SeaRitualMark crab_marks[DUEL_CARD_RECORD_COUNT];
static SeaRitualMark whale_marks[DUEL_CARD_RECORD_COUNT];
typedef struct { void *data; unsigned ritual, learned_sides; } SerpentMark;
static SerpentMark serpent_marks[DUEL_CARD_RECORD_COUNT];
static unsigned serpent_seen;
static unsigned serpent_live_mask;
static unsigned serpent_fear_bonus[DUEL_CARD_RECORD_COUNT];
static int serpent_fear_battle_slot = -1;
static int serpent_ghost_state, serpent_ghost_hold, serpent_music_volume = -1;
static int serpent_ghost_x, serpent_ghost_y;
static int serpent_art_loaded;
static void serpent_visual_reset(void);
static unsigned serpent_dbg_ritual, serpent_dbg_battle_calls, serpent_dbg_facedown, serpent_dbg_sources, serpent_dbg_bonus_reads, serpent_dbg_ghost_starts;
static int serpent_dbg_last_slot=-1;

static unsigned crab_restore_visuals;
static unsigned crab_summons, whale_summons;
static unsigned crab_live_mask, whale_live_mask;
static unsigned crab_umi_pending;
static unsigned crab_umi_sequence; /* 0 idle, 1 wait ritual, 2 pre-effect, 3 visual, 4 post-effect */
static int crab_umi_visual_state;
static int ritual_large_visual_card_id = UMI_ID;
static int crab_umi_visual_hold;
static int crab_umi_camera_delay;
static int ritual_field_source_card_id;
static DisplayObject *crab_umi_overlay_add;
static DisplayObject *crab_umi_overlay_sub;
static void (*original_scene_update)(void);
static unsigned whale_destruction_guard;
typedef struct {
    void *data;
    int ritual;
} FiendMirrorMark;
static FiendMirrorMark fiend_mirror_marks[DUEL_CARD_RECORD_COUNT];
static u16 mirror_last_lp[2];
static int mirror_lp_ready;
/* The SDK DAMAGE AFTER event runs before retail commits the returned LP to
 * DuelSideState. Do not treat that transient pre-commit value as a fresh
 * source of damage, then reflect the same hit a second time. */
static u8 mirror_commit_pending[DUEL_SIDE_COUNT];
static u16 mirror_commit_before[DUEL_SIDE_COUNT];
static u16 mirror_commit_after[DUEL_SIDE_COUNT];
static unsigned fiend_mirror_summons;
static unsigned fiend_mirror_live_hint;
static unsigned fiend_mirror_live_mask;
/* Retail burn animation request (id 6) indexes this five-entry table.  Keep
 * one entry temporarily dynamic while the request is alive, then restore it. */
extern u8 gDuel_abDirectDamageUnits[5];
typedef struct {
    int active;
    int seen_active;
    int target_side;
    unsigned damage;
    DuelEffectRequest *request;
    u8 saved_units[5];
} MirrorReflection;

#define MIRROR_QUEUE_CAP 8
typedef struct { int target_side; unsigned damage; int source_index; int visual_queued; int prepaid; } MirrorQueuedHit;
static MirrorQueuedHit mirror_queue[MIRROR_QUEUE_CAP];
static unsigned mirror_queue_head, mirror_queue_tail, mirror_queue_count;
static MirrorReflection mirror_reflection;
static u8 mirror_battle_grace[2];
static unsigned mirror_defense_direct_pending;
static int mirror_defense_direct_side;
/* Battle participant bits (0 attacker, 1 defender) whose logical destruction
 * is prevented by a Ritual protection but whose native 3D model would still
 * choose the retail death/burn reaction.  The model hook consumes this only
 * for presentation; duel result, LP damage and the protection resource stay
 * owned by the normal battle controller. */
static unsigned battle_visual_survivor_parts;
/* Participant bits whose retail 2D burn phase (battle state 10) was skipped
 * because the Ritual's battle protection will preserve that monster. Keep
 * the native result display object so state 11 restores it normally, without
 * requesting fire or destroying/recreating its field-card object. */
static unsigned javelin_2d_survivor_parts;
static unsigned tri_2d_survivor_parts;
/* A Tri-Horned's horns are a counter (unlike Javelin's once-per-turn shell).
 * Guard the entire native state-11 presentation, which runs for many frames,
 * so a single battle cannot spend more than one horn per participant. */
static unsigned tri_battle_charged_parts;
static void (*original_rebuild_field_card)(s32, s32);
static s32 (*original_check_fusion)(s32, s32);
static s32 (*original_check_ritual)(DuelRitualResult *, s32);

static int monster_slot(int index)
{ return (index >= 5 && index < 10) || (index >= 20 && index < 25); }

static int record_index(DuelCardRecord *card);
static int battle_participant(int index);
static int effect_is_magic_or_trap(void);
static int RitualEffects_IsCardEffectImmune(int target_index,int source_side);
static int ultimate_effect_active(int i);
static int cosmo_lock_active(int side,int *hand_slot,int *dest_col);
static void cosmo_force_ai_selection(void);
static unsigned selection_epoch=1;
static int selection_ready;
static unsigned female_field_count_except(int excluded);
static unsigned female_field_count(void);
static int cosmo_live(int i);
static void mirror_watch_lp(void);
static void mirror_queue_hit(int target_side,unsigned damage,int source_index);
static void mirror_queue_prepaid(int target_side,unsigned damage,int source_index);
static int side_has_ritual_fiend_mirror(int side);
static int ritual_counter_selected_record(void);
static int ritual_ai_side_is_cpu(int side);
static int ritual_ai_knows_effect(int side,int card_id);
static int create_burn_card_in_slot(int index,int card_id);
static int ritual_equip_token;
static void ritual_ai_learn_full_effect_for_side(int side,int card_id);

static int magic_slot(int index)
{ return (index >= 10 && index < 15) || (index >= 25 && index < 30); }

static int side_for_record(int index)
{ return index >= DUEL_CARD_SIDE_RECORD_COUNT; }
static unsigned side_monster_mask(int side);
static inline int take_mask_bit(unsigned *mask);

/* Visual helper is defined below. Mask uses it only when a real Fusion is
 * blocked during placement; planner-only checks never animate. */
static int visual_queue_activation(int record_index);

#include "mask_effect.h"
/* Universal Equip compatibility is a target-check permission only.
 * Later removal of a Mask cannot undo previously applied native bonuses. */
static void mask_equip_permission(MemoriesModEvent *event)
{
    int side=D_8009B1D5, source_found=0;
    unsigned pending;
    if(!event || event->phase!=MEMORIES_BEFORE || event->handled ||
       !mask_instance_count || !ritual_card_effect_enabled(MASK_ID) ||
       side<0 || side>1 || !Cards_Valid(event->a) || !Cards_Valid(event->b) ||
       Cards_Type(event->a)!=CARD_TYPE_EQUIP || Cards_Type(event->b)>=CARD_TYPE_MAGIC ||
       event->b==MASK_ID)return;
    /* Duel_CheckEquip supplies ONLY two card IDs. During card placement,
     * DuelScene_UpdateCardPlacement reads IDs from staged replay objects,
     * and the target's DuelCardRecord is frequently DEACTIVATED. Requiring
     * an occupied field record for event->b rejected these legal targets.
     * All native Equip checks for the acting player/CPU concern their own
     * selected materials, so the active side identifies the target owner. */
    pending=mask_instance_mask;
    while(pending) {
        int i=take_mask_bit(&pending),slot=mask_slot_for(&mask_instances[i]);
        if(slot>=0 && side_for_record(slot)==side) {source_found=1;break;}
    }
    /* No target instance ID is exposed by this event: exclude Mask card ID
     * entirely, including a second Mask. Never transform two monsters (or
     * spells) into an Equip, and never override native Equip bonuses. */
    if(source_found) {event->handled=1;event->result=1;}
}
#include "skull_monster.h"
#include "ritual_recipe_data.h"
#include "ritual_recipe_compat.h"
#include "ritual_recipe_provider_v1.h"
#include "ritual_recipe_export.h"
#include "ritual_effect_intelligence_v1.h"
#include "ritual_ai_effect_source.h"
#include "garma_visual.h"
#include "tri_horned.h"
#include "javelin_shell_sprite.h"
#include "hungry_plus500_sprite.h"
#include "native_vfx_textures.h"
#include "chakra_green_atlas.h"
static void serpent_reconcile(void);
static void mirror_watch_lp(void);
#include "ritual_coin.h"
/* Reward reservations keep a successful dodge independent of the battle
 * animation/turn owner. Each reservation owns one vacant magic-zone slot. */
static int performance_reward_reserved[DUEL_CARD_RECORD_COUNT];
static void performance_queue_reward(int side)
{
    int first=side?25:10;
    int roll,card_id;
    if(side<0||side>1||!ritual_card_effect_enabled(PERFORMANCE_ID))return;
    for(int slot=first;slot<first+5;++slot) {
        if((D_801A7AD8[slot].flags&DUEL_CARD_FLAG_OCCUPIED) || performance_reward_reserved[slot])continue;
        roll=rand()%100;
        card_id=roll<50?340:roll<85?341:342;
        performance_reward_reserved[slot]=card_id;
        return;
    }
}
static void performance_release_rewards(void)
{
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5 ||
       (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) || ritual_coin.active)return;
    for(int slot=10;slot<30;++slot)if(performance_reward_reserved[slot]) {
        int id=performance_reward_reserved[slot];
        performance_reward_reserved[slot]=0;
        if(!ritual_card_effect_enabled(PERFORMANCE_ID))continue;
        if(magic_slot(slot) && !(D_801A7AD8[slot].flags&DUEL_CARD_FLAG_OCCUPIED)) {
            create_burn_card_in_slot(slot,id); /* face-down; native manual activation */
        }
    }
}
#include "performance_chaos.h"

static void forget_yamadron(DuelCardRecord *card)
{
    int i = record_index(card);
    if (i >= 0) {
        yamadron_marks[i].data = 0;
        yamadron_marks[i].card_id = 0;
        yamadron_marks[i].ritual = 0;
        yamadron_marks[i].visual_shown = 0;
        yamadron_live_mask &= ~(1u << i);
    }
}

static inline int take_mask_bit(unsigned *mask)
{
    unsigned value = *mask;
    int bit = __builtin_ctz(value);
    *mask = value & (value - 1u);
    return bit;
}

/* Shared rule for bonuses described as counting OTHER monsters.  Compare
 * record identity, not card ID: a second copy of the source must count,
 * but the source instance must never contribute to its own bonus. */
static int beacon_is_other_record(int candidate, int source)
{
    return candidate != source;
}
/* A Ritual/Hand-Deck bridge may briefly materialize the same physical card
 * in a second field record.  A bonus for OTHER cards must exclude both the
 * owner's record and aliases to its backing DuelCardRecord.data.  Distinct
 * copies of the same card ID still count normally. */
static int beacon_is_other_physical_record(int candidate, int source)
{
    void *owner;
    if(!beacon_is_other_record(candidate,source))return 0;
    if(source<0 || source>=DUEL_CARD_RECORD_COUNT ||
       candidate<0 || candidate>=DUEL_CARD_RECORD_COUNT)return 1;
    owner=D_801A7AD8[source].data;
    return !owner || D_801A7AD8[candidate].data!=owner;
}

/* All source-based auras use the same physical-instance exclusion rule.
 * The kind-specific rule decides WHICH live Ritual source is eligible; the
 * target's record (rather than its card ID) decides WHO must be excluded. */
enum { BEACON_JAVELIN, BEACON_BLS, BEACON_DOKU, BEACON_CRAB };
static unsigned beacon_source_count(unsigned mask,int side,int target,int kind);

static unsigned snapshot_attack_monsters(unsigned *mask_out)
{
    unsigned count=0, mask=0; int i;
    for(i=5;i<10;++i) {
        DuelCardRecord *card=&D_801A7AD8[i];
        if((card->flags&DUEL_CARD_FLAG_OCCUPIED) && !(card->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)) {++count;mask|=1u<<i;}
    }
    for(i=20;i<25;++i) {
        DuelCardRecord *card=&D_801A7AD8[i];
        if((card->flags&DUEL_CARD_FLAG_OCCUPIED) && !(card->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)) {++count;mask|=1u<<i;}
    }
    if(mask_out)*mask_out=mask;
    return count;
}
static unsigned count_attack_monsters_except(int excluded_index)
{
    unsigned mask=0,count=snapshot_attack_monsters(&mask);
    if(excluded_index>=0 && excluded_index<DUEL_CARD_RECORD_COUNT && (mask&(1u<<excluded_index)))--count;
    return count;
}

static int record_index(DuelCardRecord *card)
{
    uintptr_t address = (uintptr_t)card, base = (uintptr_t)D_801A7AD8;
    uintptr_t size = sizeof(DuelCardRecord);
    if (address < base || address >= base + size * DUEL_CARD_RECORD_COUNT ||
        (address - base) % size) return -1;
    return (int)((address - base) / size);
}

/* Record selected by the currently ACTIVE field cursor. In hand phase (4),
 * holding the field-view control sets scene flag 0x4000 and uses record 1,
 * while placement targeting uses another grid cursor. Never assume record 2.
 * In field phase (5), D_8009B1B4 is likewise authoritative. */
static int ritual_counter_selected_record(void)
{
    int phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    DuelCardPickCursor *cursor=D_8009B1B4;
    if(phase!=4 && phase!=5)return -1;
    if(phase==4 && !(gDuel_wSceneStateFlags&0x4000) && ((D_8009B174&0x0f)!=4))return -1;
    if(!cursor || cursor->row<0 || cursor->row>=4 || cursor->col<0 || cursor->col>=5)return -1;
    return D_800907D8[D_8009B1D5*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+
                      cursor->row*DUEL_FIELD_ROW_SIZE+cursor->col];
}

/* The field's card frame reads live Duel_CalcCardStats on every draw, but
 * func_80023144 renders the bottom ATK/DEF panel only when the cursor settles.
 * An automatic Field (e.g. Tri-Horned's Wasteland) can finish afterwards and
 * leave the panel showing pre-Field stats until the cursor moves again.
 * Track precisely the panel's last drawn card/value, without changing any
 * duel stat or Field effect. */
/* Native Fields already flow through both card-frame and card-info builders.
 * Hand auras must join BOTH rendering paths, without modifying actual hand
 * stats or any duel/AI computation.  The helper's definition is alongside the
 * card-frame hook; this shared marker is scoped to a synchronous draw only. */
#define HAND_AURA_PREVIEW_HINTS (STAT_HINT_JAVELIN|STAT_HINT_CRAB|STAT_HINT_TRI|STAT_HINT_BLS|STAT_HINT_DOKU|STAT_HINT_ZERA)
static int hand_aura_preview_record=-1;
static int logical_hand_record(int i);
static void (*original_field_card_info)(DuelFieldDisplaySource *,s32);
static struct {
    DuelFieldDisplaySource *source;
    void *data;
    int index;
    int card_id;
    uint32_t packed_stats;
} field_info_cache={0,0,-1,0,0};

static void field_card_info(DuelFieldDisplaySource *source,s32 index)
{
    DuelCardRecord *card;
    /* The native info panel calls Duel_CalcCardStats separately from the card
     * sprite.  Give it the same render-only hand-aura context, including while
     * the view is transitioning, then restore the previous context. */
    if ((stat_effect_hints & HAND_AURA_PREVIEW_HINTS) &&
        index>=0 && index<DUEL_CARD_RECORD_COUNT && logical_hand_record(index)) {
        int previous=hand_aura_preview_record;
        hand_aura_preview_record=index;
        original_field_card_info(source,index);
        hand_aura_preview_record=previous;
    } else {
        original_field_card_info(source,index);
    }
    field_info_cache.source=0;
    field_info_cache.index=-1;
    if(!source || index<0 || index>=DUEL_CARD_RECORD_COUNT)return;
    card=&D_801A7AD8[index];
    if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED) ||
       !monster_slot(index) || card->card_id<=0)return;
    field_info_cache.source=source;
    field_info_cache.index=index;
    field_info_cache.card_id=card->card_id;
    field_info_cache.data=card->data;
    field_info_cache.packed_stats=(uint32_t)Duel_CalcCardStats(card);
}

static void refresh_field_card_info(void)
{
    DuelFieldDisplaySource *source=field_info_cache.source;
    DuelCardRecord *card;
    int phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    int index=field_info_cache.index;
    uint32_t stats;
    if(!source || index<0 || (phase!=4 && phase!=5) ||
       D_8009B162 || D_8009B229 || gDuel_bEffectState ||
       (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) ||
       (DuelFieldDisplaySource *)D_8009B1B4!=source ||
       !source->field_00 || ritual_counter_selected_record()!=index)return;
    card=&D_801A7AD8[index];
    if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED) ||
       card->card_id!=field_info_cache.card_id ||
       card->data!=field_info_cache.data)return;
    stats=(uint32_t)Duel_CalcCardStats(card);
    if(stats!=field_info_cache.packed_stats)
        field_card_info(source,index); /* Same native builder used on a cursor move. */
}

static void forget(DuelCardRecord *card)
{
    int i = record_index(card);
    if (i >= 0) {
        marks[i].data = 0; marks[i].card_id = 0;
        marks[i].ritual = 0; marks[i].in_battle = 0; marks[i].attack_count = 0; marks[i].def_visual_shown = 0; marks[i].protect_visual_shown = 0;
        shield_live_mask &= ~(1u << i);
    }
}

/* Javelin's shell counter uses the user-approved beetle image. Like the
 * Garma/Tri counters, it is rasterized into compact horizontal runs once at
 * startup and follows the native field renderer's current card position. */
typedef struct { u8 row,start,length; unsigned rgba; } JavelinIconRun;
static JavelinIconRun javelin_icon_runs[32*32];
static unsigned javelin_icon_run_count;
static void javelin_icon_prepare(void)
{
    int row,col;
    if(javelin_icon_run_count)return;
    for(row=0;row<32;++row)for(col=0;col<32;){
        int begin; unsigned rgba=javelin_shell_rgba[row*32+col]; JavelinIconRun *run;
        if(!(rgba&255)){++col;continue;}
        begin=col++;while(col<32&&javelin_shell_rgba[row*32+col]==rgba)++col;
        run=&javelin_icon_runs[javelin_icon_run_count++];
        run->row=(u8)row;run->start=(u8)begin;run->length=(u8)(col-begin);run->rgba=rgba;
    }
}
static int javelin_live(int i)
{
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && javelin_marks[i].ritual &&
        (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
        D_801A7AD8[i].card_id==JAVELIN_BEETLE_ID &&
        D_801A7AD8[i].data==javelin_marks[i].data;
}
static void javelin_capture(DisplayObject *o,int x,int y)
{
    int i=o?o->field_6A:-1,phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    if(!javelin_live(i) || (phase!=4 && phase!=5))return;
    javelin_screen[i].data=D_801A7AD8[i].data;javelin_screen[i].object=o;
    javelin_screen[i].x=x+26;javelin_screen[i].y=y+30;javelin_screen[i].valid=1;
}
static void javelin_icons(void)
{
    int i=-1,w,h,sc,vw,vh,l,t,x,y,size,x0,x1,y0,y1;
    unsigned run_index;
    if(!javelin_live_mask||!host||!host->fill||!host->overlay_size||
       !ritual_card_effect_enabled(JAVELIN_BEETLE_ID))return;
    { int counter_phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    if((counter_phase!=4 && counter_phase!=5) || D_8009B229 ||
       gDuel_bEffectState || (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) ||
       crab_umi_visual_state)return;
    i=ritual_counter_selected_record();
    if(!javelin_live(i)||javelin_marks[i].protection_used_turn||!javelin_screen[i].valid||
       javelin_screen[i].data!=javelin_marks[i].data)return;
    host->overlay_size(host,&w,&h,&sc);vw=w;vh=w*3/4;if(vh>h){vh=h;vw=h*4/3;}l=(w-vw)/2;t=(h-vh)/2;
    size=16*vw/320;if(size<1)size=1;
    x=l+javelin_screen[i].x*vw/320-size/2;
    y=t+(javelin_screen[i].y-48)*vh/240;if(y<t)y=t;
    for(run_index=0;run_index<javelin_icon_run_count;++run_index){
        const JavelinIconRun *run=&javelin_icon_runs[run_index];unsigned rgba=run->rgba;
        x0=run->start*size/32;x1=(run->start+run->length)*size/32;
        y0=run->row*size/32;y1=(run->row+1)*size/32;
        if(x1>x0&&y1>y0)host->fill(host,x+x0,y+y0,x1-x0,y1-y0,rgba>>8,rgba&255);
    }
    }
}

static int hungry_live(int i)
{
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && hungry_marks[i].ritual &&
        (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
        D_801A7AD8[i].card_id==HUNGRY_BURGER_ID &&
        D_801A7AD8[i].data==hungry_marks[i].data;
}
static void hungry_capture(DisplayObject *o,int x,int y)
{
    int i=o?o->field_6A:-1;
    if(!hungry_live(i)||D_801A7AD8[i].object!=o)return;
    hungry_screen[i].data=D_801A7AD8[i].data;hungry_screen[i].object=o;
    /* func_80016784 receives the card's final 320x240 screen position. Keep
     * the popup centred on the card and anchor its bottom to the card top, so
     * the approved +500 art gives a brief plim immediately above the card. */
    hungry_screen[i].x=x+26;hungry_screen[i].y=y;hungry_screen[i].valid=1;
}

#define STAT_VISUAL_NORMAL 0
#define STAT_VISUAL_HUNGRY_STACK 1
#define STAT_VISUAL_CHAKRA 2
#define CHAKRA_POPUP_FRAMES 60
#define HUNGRY_POPUP_FRAMES 60
static int visual_popup_frames;
static int visual_popup_kind;
static int visual_popup_record_index=-1;
static int visual_popup_amount;
static void native_vfx_sprite(GsOT *ot,int pri,int x,int y,int draw_w,int draw_h,
                              int u,int v,int tex_w,int tex_h,int brightness);

/* All characters share one 256x16 PS1 atlas made from the supplied green
 * 0-9 and +/ATK sprites. Build the amount at runtime; one popup, not one
 * popup per destroyed monster. */
static void chakra_native_popup(GsOT *ot,int pri,int x,int y,unsigned amount,int brightness)
{
    /* The amount stays fixed throughout the popup. Cache only its pure text
     * layout, keyed by the actual value; no duel/instance state is cached. */
    static struct { unsigned amount; int valid,len,width; char value[12]; } layout;
    int k,cursor;
    if(!layout.valid || layout.amount!=amount) {
        snprintf(layout.value,sizeof(layout.value),"%u",amount);
        layout.len=(int)strlen(layout.value);
        layout.width=CHAKRA_PLUS_W+2+layout.len*10+3+CHAKRA_ATK_W;
        layout.amount=amount;
        layout.valid=1;
    }
    cursor=x-layout.width/2;
    native_vfx_sprite(ot,pri,cursor,y,CHAKRA_PLUS_W,CHAKRA_GLYPH_H,
        CHAKRA_PLUS_U,CHAKRA_ATLAS_V,CHAKRA_PLUS_W,CHAKRA_GLYPH_H,brightness);
    cursor+=CHAKRA_PLUS_W+2;
    for(k=0;k<layout.len;++k) {
        int digit=layout.value[k]-'0';
        if(digit>=0 && digit<=9)
            native_vfx_sprite(ot,pri,cursor,y,CHAKRA_GLYPH_W,CHAKRA_GLYPH_H,
                digit*12,CHAKRA_ATLAS_V,CHAKRA_GLYPH_W,CHAKRA_GLYPH_H,brightness);
        cursor+=10;
    }
    cursor+=3;
    native_vfx_sprite(ot,pri,cursor,y,CHAKRA_ATK_W,CHAKRA_GLYPH_H,
        CHAKRA_ATK_U,CHAKRA_ATLAS_V,CHAKRA_ATK_W,CHAKRA_GLYPH_H,brightness);
}

/* Pilot for card-attached visuals: submit the approved artwork to the
 * game's own PS1 ordering table, in the same 320x240 coordinate space as the
 * small field card.  The platform scales/crops/letterboxes the finished game
 * picture afterwards, so these visuals cannot drift when the window aspect
 * ratio changes. */
static void native_vfx_upload_texture(u16 *bank,int u,int v,int w,int h,const u16 *pixels)
{
    int row;
    if(!bank || !pixels)return;
    for(row=0;row<h;++row)
        memcpy(bank+(v+row)*SOFT_GPU_WIDTH+u,pixels+row*w,(size_t)w*sizeof(u16));
}

/* Translate Garma's existing approved 17x17 sword sprite into the same
 * PS1 texture bank as the Horn and Shell counters. The palette and shape
 * are unchanged; only the rendering pipeline and anchor become native. */
static void native_vfx_upload_garma(u16 *bank)
{
    u16 texels[RITUAL_GARMA_W*RITUAL_GARMA_H];
    unsigned r;
    memset(texels,0,sizeof(texels));
    for(r=0;r<garma_icon_run_count;++r) {
        const GarmaIconRun *run=&garma_icon_runs[r];
        unsigned rgb;
        int k;
        u16 psx;
        if(run->tone=='W')rgb=0xdce7f4;
        else if(run->tone=='B')rgb=0x536a9a;
        else if(run->tone=='G')rgb=0xd4a21f;
        else if(run->tone=='D')rgb=0x76501b;
        else rgb=0x9aa9c0;
        psx=(u16)(((rgb>>19)&31)|(((rgb>>11)&31)<<5)|(((rgb>>3)&31)<<10));
        for(k=0;k<run->length;++k)
            texels[run->row*RITUAL_GARMA_W+run->start+k]=psx;
    }
    native_vfx_upload_texture(bank,RITUAL_GARMA_U,RITUAL_GARMA_V,
        RITUAL_GARMA_W,RITUAL_GARMA_H,texels);
}

static int native_vfx_upload(void)
{
    u16 *bank=SoftGpu_Bank(RITUAL_VFX_BANK);
    if(!bank)return 0;
    native_vfx_upload_texture(bank,RITUAL_HORN_U,RITUAL_HORN_V,
        RITUAL_HORN_W,RITUAL_HORN_H,ritual_horn_texels);
    native_vfx_upload_texture(bank,RITUAL_JAVELIN_U,RITUAL_JAVELIN_V,
        RITUAL_JAVELIN_W,RITUAL_JAVELIN_H,ritual_javelin_texels);
    native_vfx_upload_texture(bank,RITUAL_HUNGRY_U,RITUAL_HUNGRY_V,
        RITUAL_HUNGRY_W,RITUAL_HUNGRY_H,ritual_hungry_texels);
    native_vfx_upload_garma(bank);
    native_vfx_upload_texture(bank,CHAKRA_ATLAS_U,CHAKRA_ATLAS_V,CHAKRA_ATLAS_W,CHAKRA_ATLAS_H,chakra_green_atlas_texels);
    return 1;
}

/* Direct-colour textured quad in the game's own 320x240 ordering table.
 * This preserves the approved artwork while keeping the aspect-ratio fix. */
static void native_vfx_sprite(GsOT *ot,int pri,int x,int y,int draw_w,int draw_h,
                              int u,int v,int tex_w,int tex_h,int brightness)
{
    POLY_FT4 p;
    int level;
    if(!ot || draw_w<=0 || draw_h<=0 || tex_w<=0 || tex_h<=0 || brightness<=0)return;
    if(pri<0)pri=0;
    if(brightness>255)brightness=255;
    level=128*brightness/255;
    if(level<1)level=1;
    memset(&p,0,sizeof(p));
    setPolyFT4(&p);
    setRGB0(&p,level,level,level);
    setXY4(&p,x,y,x+draw_w,y,x,y+draw_h,x+draw_w,y+draw_h);
    setUV4(&p,u,v,u+tex_w-1,v,u,v+tex_h-1,u+tex_w-1,v+tex_h-1);
    p.tpage=RITUAL_VFX_TPAGE;
    p.clut=0;
    GsSortPoly(&p,ot,(u16)pri);
}

static void native_vfx_shine(GsOT *ot,int pri,int x,int y,int brightness)
{
    GsBOXF box; int b=brightness;
    if(!ot || b<=0)return;if(b>255)b=255;if(pri<0)pri=0;
    memset(&box,0,sizeof(box));box.attribute=GsALON|GsAONE;
    box.r=(u8)(18*b/255);box.g=(u8)(80*b/255);box.b=(u8)(18*b/255);
    /* Thin four-sided pulse, inside the card silhouette. */
    box.x=(short)(x+2);box.y=(short)(y+2);box.w=48;box.h=2;GsSortBoxFill(&box,ot,(u16)pri);
    box.y=(short)(y+56);GsSortBoxFill(&box,ot,(u16)pri);
    box.x=(short)(x+2);box.y=(short)(y+4);box.w=2;box.h=52;GsSortBoxFill(&box,ot,(u16)pri);
    box.x=(short)(x+48);GsSortBoxFill(&box,ot,(u16)pri);
}

/* Hand aura numbers are a RENDER-ONLY prediction. Never let a hand card's
 * projected ATK/DEF escape into fusion, battle, Ritual recipes or the AI.
 * Logical hand membership is verified against the real five side-state slots;
 * physical records alone may remain OCCUPIED after a card was consumed. */
static int logical_hand_record(int i)
{
    int side,slot;
    if(!((i>=0 && i<HAND_SIZE) || (i>=DUEL_CARD_SIDE_RECORD_COUNT &&
        i<DUEL_CARD_SIDE_RECORD_COUNT+HAND_SIZE))) return 0;
    side=side_for_record(i);
    for(slot=0;slot<HAND_SIZE;++slot)
        if(D_800E9FF0[side].hand[slot]>=0 &&
           D_800907CC[side*HAND_SIZE+slot]==i)return 1;
    return 0;
}
static void (*original_card_frame_draw)(DisplayObject *, s32, s32, s32);
static void draw_card_with_hand_aura(DisplayObject *object,s32 a1,s32 a2,s32 a3)
{
    int i,previous;
    /* Field terrain uses the same native renderer in every transition.
     * A frame can still display the player's real hand even when phase != 4.
     * Restrict our projection by logical hand identity, not the scene phase. */
    if(!(stat_effect_hints & HAND_AURA_PREVIEW_HINTS) || !object) {
        original_card_frame_draw(object,a1,a2,a3);
        return;
    }
    i=(int)object->field_6A;
    if(!logical_hand_record(i)) {
        original_card_frame_draw(object,a1,a2,a3);
        return;
    }
    previous=hand_aura_preview_record;
    hand_aura_preview_record=i;
    original_card_frame_draw(object,a1,a2,a3);
    hand_aura_preview_record=previous;
}
/* Chakra camera: the native field renderer draws large CARD BASE quads,
 * while 3D Monsters draws its standing character models separately.  For the
 * approved 0.78.31 camera, suppress only the opposing side's ten base cards:
 * five Monster Zones AND five back-row Magic/Trap/Ritual Zones.  Do not hide
 * the arena or any monster models; do not touch logical occupied flags. */
static int chakra_hide_enemy_board_bases(void)
{
    return chakra_sequence.active && chakra_sequence.camera_active &&
           (chakra_sequence.stage==1 || chakra_sequence.stage==2 ||
            chakra_sequence.stage==3 || chakra_sequence.stage==6);
}
/* Card records are 15 per player: 5 hand, 5 monsters, 5 back-row cards. */
static int chakra_enemy_board_card(int index)
{
    int first=(chakra_sequence.side^1)*DUEL_CARD_SIDE_RECORD_COUNT+HAND_SIZE;
    return index>=first && index<first+DUEL_FIELD_SIDE_ZONE_COUNT;
}
/* Also handle an occasional 2D projected preview of the same base.  Only
 * the opposing board bases are suppressed; existing protection of actual
 * hand previews and deactivated monster sprites remains unchanged. */
static int chakra_skip_transitional_card_sprite(const DisplayObject *object)
{
    int index;
    DuelCardRecord *card;
    if(!object || !chakra_sequence.active || !chakra_sequence.camera_active)
        return 0;
    index=(int)object->field_6A;
    if(index<0 || index>=DUEL_CARD_RECORD_COUNT)return 0;
    if(logical_hand_record(index))return 1;
    card=&D_801A7AD8[index];
    if(monster_slot(index) && !(card->flags&DUEL_CARD_FLAG_OCCUPIED))return 1;
    if(chakra_hide_enemy_board_bases() && chakra_enemy_board_card(index) &&
       (card->flags&DUEL_CARD_FLAG_OCCUPIED) && card->object)
        return 1;
    return 0;
}
/* During a SINGLE native draw pass, temporarily remove render objects from
 * all occupied opponent board slots.  The engine's field quad renderer then
 * omits those card bases.  Restore synchronously BEFORE the independent 3D
 * Monsters render pass, which sees untouched occupied flags and objects.
 * Inactive / non-Chakra frames take a one-branch vanilla fast path. */
static void (*original_field_card_draw)(void);
static void chakra_draw_field_cards(void)
{
    typedef struct { DuelCardRecord *card; DisplayObject *object; } SavedCard;
    SavedCard saved[DUEL_FIELD_SIDE_ZONE_COUNT];
    int first,i,n=0;
    if(!original_field_card_draw)return;
    if(!chakra_hide_enemy_board_bases()) {
        original_field_card_draw();
        return;
    }
    first=(chakra_sequence.side^1)*DUEL_CARD_SIDE_RECORD_COUNT+HAND_SIZE;
    for(i=first;i<first+DUEL_FIELD_SIDE_ZONE_COUNT;++i) {
        DuelCardRecord *card=&D_801A7AD8[i];
        if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED) || !card->object)continue;
        saved[n].card=card;
        saved[n].object=card->object;
        ++n;
        card->object=0;
    }
    original_field_card_draw();
    while(n) {
        --n;
        saved[n].card->object=saved[n].object;
    }
}

static void visual_card_frame_draw(DisplayObject *object,s32 a1,s32 a2,s32 a3)
{
    int i,phase,pri;
    GsOT *ot;
    if(chakra_skip_transitional_card_sprite(object))return;
    /* This hook runs for EVERY field/hand card, not just Ritual monsters.
     * When no native Ritual sprite is visible, let the retail renderer run
     * without cursor selection, scene-state or coordinate bookkeeping.
     * No change to card drawing or to the Ritual gameplay state machine. */
    if(!garma_live_hint && !tri_live_hint && !javelin_live_mask &&
       !(visual_popup_kind==STAT_VISUAL_HUNGRY_STACK && visual_popup_frames>0) &&
       !(visual_popup_kind==STAT_VISUAL_CHAKRA && visual_popup_frames>0)) {
        draw_card_with_hand_aura(object,a1,a2,a3);
        return;
    }
    i=object?object->field_6A:-1;
    phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
    ot=(GsOT *)(uintptr_t)(u32)a1;
    pri=object?(int)object->field_14-1:0;

    /* Garma's capture is still needed to place its extra-attack animation.
     * All three resource counters now render directly with the native card. */
    if(i>=0 && i<DUEL_CARD_RECORD_COUNT && garma_live_hint &&
       D_801A7AD8[i].card_id==GARMA_SWORD_ID)garma_capture(object,a2,a3);

    draw_card_with_hand_aura(object,a1,a2,a3);

    if(i<0 || i>=DUEL_CARD_RECORD_COUNT || (phase!=4 && phase!=5) || D_8009B229 ||
       gDuel_bEffectState || (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) ||
       crab_umi_visual_state)return;

    /* Native counters are intentionally shown only for the card the field
     * cursor is inspecting, matching the old presentation while fixing its
     * coordinate system. Unrelated cards need no cursor lookup. */
    if(((garma_live_hint && D_801A7AD8[i].card_id==GARMA_SWORD_ID) ||
        (tri_live_hint && D_801A7AD8[i].card_id==TRI_HORNED_ID) ||
        (javelin_live_mask && D_801A7AD8[i].card_id==JAVELIN_BEETLE_ID)) &&
       i==ritual_counter_selected_record()) {
        /* During phase 4's pre-play field viewer, keep resource counters above
         * the inspected card (the same placement Garma already uses there).
         * Normal field navigation in phase 5 retains the approved below-card
         * placement from v0.78.12/13. */
        int counter_y=(phase==4)?a3-18:a3+62;
        if(tri_live_hint && ritual_card_effect_enabled(TRI_HORNED_ID) && tri_live(i) && tri_marks[i].horns) {
            int n=(int)tri_marks[i].horns, gap=2, size=16;
            int x=a2+26-(n*(size+gap)-gap)/2, k;
            for(k=0;k<n;++k)
                native_vfx_sprite(ot,pri,x+k*(size+gap),counter_y,size,size,
                    RITUAL_HORN_U,RITUAL_HORN_V,RITUAL_HORN_W,RITUAL_HORN_H,255);
        }
        if(garma_live_hint && ritual_card_effect_enabled(GARMA_SWORD_ID) && garma_live(i) &&
           garma_marks[i].blades_used<5) {
            int n=5-(int)garma_marks[i].blades_used, gap=2, size=16;
            int x=a2+26-(n*(size+gap)-gap)/2, k;
            for(k=0;k<n;++k)
                native_vfx_sprite(ot,pri,x+k*(size+gap),counter_y,size,size,
                    RITUAL_GARMA_U,RITUAL_GARMA_V,RITUAL_GARMA_W,RITUAL_GARMA_H,255);
        }
        if(javelin_live_mask && ritual_card_effect_enabled(JAVELIN_BEETLE_ID) && javelin_live(i) &&
           !javelin_marks[i].protection_used_turn)
            native_vfx_sprite(ot,pri,a2+18,counter_y,16,16,
                RITUAL_JAVELIN_U,RITUAL_JAVELIN_V,RITUAL_JAVELIN_W,RITUAL_JAVELIN_H,255);
    }

    /* Garma extra attack: preserve the animated sword and its approved art,
     * but not the native request-4 companion flash.  The sword drops into
     * position, holds briefly, then retreats.  No glow or screen flash. */
    if(phase==5 && garma_relevant_frames>0 && garma_relevant==i &&
       garma_live(i) && garma_animation_data==D_801A7AD8[i].data &&
       D_801A7AD8[i].object==object) {
        int elapsed=GARMA_SWORD_ANIMATION_FRAMES-garma_relevant_frames;
        int size, sword_y;
        if(elapsed<8) {
            size=20+elapsed*2;
            sword_y=a3-38+elapsed*3;
        } else if(elapsed<20) {
            size=34;
            sword_y=a3-17;
        } else {
            size=34-(elapsed-20)*2;
            sword_y=a3-17-(elapsed-20)*2;
        }
        if(size>0)
            native_vfx_sprite(ot,pri,a2+26-size/2,sword_y,size,size,
                RITUAL_GARMA_U,RITUAL_GARMA_V,RITUAL_GARMA_W,RITUAL_GARMA_H,255);
    }

    /* Hungry Burger reward pilot. The timer is gameplay-side; its position,
     * scale, pop and fade all live here in the native card renderer. */
    if(visual_popup_kind==STAT_VISUAL_HUNGRY_STACK && visual_popup_frames>0 &&
       visual_popup_record_index==i && hungry_live(i)) {
        int elapsed=HUNGRY_POPUP_FRAMES-visual_popup_frames;
        int draw_w,draw_h,brightness=255;
        if(elapsed<4)draw_w=36+elapsed*5;          /* 36,41,46,51 */
        else if(elapsed==4)draw_w=54;             /* small satisfying overshoot */
        else if(elapsed==5)draw_w=51;
        else draw_w=48;
        draw_h=draw_w*RITUAL_HUNGRY_H/RITUAL_HUNGRY_W;
        if(visual_popup_frames<13)brightness=visual_popup_frames*255/13;
        if(elapsed<10)native_vfx_shine(ot,pri,a2,a3,255-elapsed*20);
        native_vfx_sprite(ot,pri,a2+26-draw_w/2,a3-draw_h-4,draw_w,draw_h,
            RITUAL_HUNGRY_U,RITUAL_HUNGRY_V,RITUAL_HUNGRY_W,RITUAL_HUNGRY_H,brightness);
    }
    if(visual_popup_kind==STAT_VISUAL_CHAKRA && visual_popup_frames>0 &&
       visual_popup_record_index==i && chakra_marks[i].ritual &&
       chakra_marks[i].data==D_801A7AD8[i].data &&
       D_801A7AD8[i].card_id==CHAKRA_ID) {
        int brightness=visual_popup_frames<14?visual_popup_frames*255/14:255;
        int elapsed=CHAKRA_POPUP_FRAMES-visual_popup_frames;
        chakra_native_popup(ot,pri,a2+26,a3-22-(elapsed<10?elapsed/2:5),
                            (unsigned)visual_popup_amount,brightness);
    }
}

/* Shared Ritual presentation helpers, adapted from monster-effects 0.19.0. */
#define VISUAL_QUEUE_SIZE 32
#define STAT_POPUP_FRAMES 42
#define STAT_CHANGE_INTERVAL 16
#define VISUAL_ACTIVATION_DELAY 30

typedef struct { s16 x, y; s16 delay; } RitualActivationPos;
typedef struct { int active, record_index, card_id, amount, kind; void *card_data; } RitualStatVisual;
static RitualActivationPos visual_activation_queue[VISUAL_QUEUE_SIZE];
static int visual_activation_head, visual_activation_count;
static RitualStatVisual visual_stat_queue[VISUAL_QUEUE_SIZE];
static unsigned visual_stat_active_mask;
static int visual_stat_count;
static int visual_stat_cooldown, visual_popup_x, visual_popup_y;
typedef struct { u8 row,start,length; unsigned rgba; } HungryPopupRun;
static HungryPopupRun hungry_popup_runs[HUNGRY_PLUS500_W*HUNGRY_PLUS500_H];
static unsigned hungry_popup_run_count;
static void hungry_popup_prepare(void)
{
    int row,col;
    if(hungry_popup_run_count)return;
    for(row=0;row<HUNGRY_PLUS500_H;++row)for(col=0;col<HUNGRY_PLUS500_W;){
        int begin; unsigned rgba=hungry_plus500_rgba[row*HUNGRY_PLUS500_W+col]; HungryPopupRun *run;
        /* Ignore nearly transparent fringe pixels from the supplied PNG. */
        if((rgba&255)<16){++col;continue;}
        begin=col++;
        while(col<HUNGRY_PLUS500_W && hungry_plus500_rgba[row*HUNGRY_PLUS500_W+col]==rgba)++col;
        run=&hungry_popup_runs[hungry_popup_run_count++];
        run->row=(u8)row;run->start=(u8)begin;run->length=(u8)(col-begin);run->rgba=rgba;
    }
}
static void *hungry_visual_pending[DUEL_CARD_RECORD_COUNT];
static unsigned hungry_visual_pending_mask;

static int visual_queue_activation(int record_index)
{
    int side, grid;
    RitualActivationPos *pos;
    if (record_index >= 5 && record_index < 15) side = 0;
    else if (record_index >= 20 && record_index < 30) side = 1;
    else return 0;
    for (grid = 0; grid < DUEL_FIELD_SIDE_GRID_SLOT_COUNT; ++grid)
        if (D_800907D8[side * DUEL_FIELD_SIDE_GRID_SLOT_COUNT + grid] == record_index) break;
    if (grid == DUEL_FIELD_SIDE_GRID_SLOT_COUNT || visual_activation_count == VISUAL_QUEUE_SIZE) return 0;
    pos = &visual_activation_queue[(visual_activation_head + visual_activation_count) % VISUAL_QUEUE_SIZE];
    pos->x = D_80090800[side][grid].x; pos->y = D_80090800[side][grid].y;
    pos->delay = VISUAL_ACTIVATION_DELAY;
    ++visual_activation_count;
    return 1;
}

static int visual_activation_busy(void)
{
    int i;
    if (visual_activation_count) return 1;
    for (i=0;i<DUEL_EFFECT_REQUEST_COUNT;++i)
        if ((D_800EAD88[i].flags & DUEL_EFFECT_REQUEST_FLAG_ACTIVE) && D_800EAD88[i].id == 0xC) return 1;
    return 0;
}

static void visual_update_activation(void)
{
    if(garma_relevant_frames>0) return;
    DuelEffectRequest *request; RitualActivationPos *pos; int i;
    if (!visual_activation_count) return;
    for (i=0;i<DUEL_EFFECT_REQUEST_COUNT;++i)
        if ((D_800EAD88[i].flags & DUEL_EFFECT_REQUEST_FLAG_ACTIVE) && D_800EAD88[i].id == 0xC) return;
    pos=&visual_activation_queue[visual_activation_head];
    if (pos->delay > 0) { --pos->delay; return; }
    request=(DuelEffectRequest *)DuelEffect_AllocateRequest(0xC); if(!request) return;
    request->field_00=pos->x; request->field_02=0; request->field_04=pos->y;
    request->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
    visual_activation_head=(visual_activation_head+1)%VISUAL_QUEUE_SIZE; --visual_activation_count;
}

static int visual_queue_stat_kind(int record_index, int amount, int kind)
{
    DuelCardRecord *card; unsigned free_mask; int i;
    if(record_index<0 || record_index>=DUEL_CARD_RECORD_COUNT || !amount || visual_stat_count>=VISUAL_QUEUE_SIZE) return 0;
    card=&D_801A7AD8[record_index];
    if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED)||!card->data||!card->object) return 0;
    /* Preserve the old lowest-free-slot ordering without scanning all 32
     * queue entries. The bitset also lets the update path visit only live
     * entries, which matters while several stat popups are waiting. */
    free_mask=~visual_stat_active_mask;
    if(!free_mask)return 0;
    i=__builtin_ctz(free_mask);
    visual_stat_active_mask|=1u<<i;
    visual_stat_queue[i].active=1; visual_stat_queue[i].record_index=record_index;
    visual_stat_queue[i].card_id=card->card_id; visual_stat_queue[i].card_data=card->data;
    visual_stat_queue[i].amount=amount; visual_stat_queue[i].kind=kind; ++visual_stat_count; return 1;
}
static int visual_queue_stat(int record_index, int amount)
{ return visual_queue_stat_kind(record_index,amount,STAT_VISUAL_NORMAL); }
static int visual_queue_hungry_stack(int record_index)
{ return visual_queue_stat_kind(record_index,HUNGRY_BONUS,STAT_VISUAL_HUNGRY_STACK); }

static void visual_update_stats(void)
{
    unsigned pending;
    if(garma_relevant_frames>0) return;
    if(visual_popup_frames>0){ --visual_popup_frames; if(visual_popup_frames>0)return; }
    if(visual_activation_busy()) return;
    if(visual_stat_cooldown>0){--visual_stat_cooldown;return;}
    if(!visual_stat_count) return;
    pending=visual_stat_active_mask;
    while(pending){
        int i=take_mask_bit(&pending);
        RitualStatVisual *e=&visual_stat_queue[i]; DuelCardRecord *card; DisplayObject *obj;
        card=&D_801A7AD8[e->record_index];
        if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED)||card->card_id!=e->card_id||card->data!=e->card_data||!card->object){
            e->active=0; visual_stat_active_mask&=~(1u<<i); --visual_stat_count; continue;
        }
        obj=(DisplayObject *)card->object;
        if(e->amount>0){
            if(e->kind==STAT_VISUAL_HUNGRY_STACK) {
                HungryMark *m=&hungry_marks[e->record_index];
                int phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK);
                if(!m->ritual || m->data!=card->data || card->card_id!=HUNGRY_BURGER_ID) {
                    e->active=0; visual_stat_active_mask&=~(1u<<i); --visual_stat_count; continue;
                }
                /* Keep the player's established v0.78.43 timing: the stack
                 * commits when its own green +500 presentation starts.
                 * CPU stacks were already committed by the battle hook. */
                if((phase!=4 && phase!=5) || D_8009B229)return;
                if(!ritual_ai_side_is_cpu(side_for_record(e->record_index)))
                    ++m->battle_kills;
            }
            visual_popup_amount=e->amount; visual_popup_kind=e->kind;
            if(e->kind==STAT_VISUAL_HUNGRY_STACK) {
                /* For the player, the native +500 art, sound and stat gain
                 * stay synchronized exactly as before the CPU-only fix. */
                visual_popup_record_index=e->record_index;
                visual_popup_frames=HUNGRY_POPUP_FRAMES;
                SD_SEPlayFull(0x2F);
            } else if(e->kind==STAT_VISUAL_CHAKRA) {
                visual_popup_record_index=e->record_index;
                visual_popup_frames=CHAKRA_POPUP_FRAMES;
                SD_SEPlayFull(0x21);
            } else {
                visual_popup_x=obj->field_30.h.field_30;
                visual_popup_y=obj->field_30.h.field_32;
                visual_popup_frames=STAT_POPUP_FRAMES;
                visual_popup_record_index=-1;
                SD_SEPlayFull(0x21);
            }
            e->active=0; visual_stat_active_mask&=~(1u<<i); --visual_stat_count; visual_stat_cooldown=STAT_CHANGE_INTERVAL; return;
        }
        else { DuelEffectRequest *r=DuelEffect_CreateRequest(0xD); if(!r)return; r->field_00=obj->field_30.h.field_30; r->field_02=obj->field_30.h.field_32; r->field_04=obj->field_34.h.field_34; r->field_12=(s16)e->amount; SD_SEPlayFull(0x21); e->active=0; visual_stat_active_mask&=~(1u<<i); --visual_stat_count; visual_stat_cooldown=STAT_CHANGE_INTERVAL; return; }
    }
}

static int visual_find_live_mirror(int controller_side)
{
    unsigned pending=fiend_mirror_live_mask & side_monster_mask(controller_side);
    while(pending) {
        int i=take_mask_bit(&pending); DuelCardRecord *c=&D_801A7AD8[i];
        if(fiend_mirror_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           c->card_id==FIENDS_MIRROR_ID && c->data==fiend_mirror_marks[i].data && c->object) return i;
    }
    return -1;
}

static void visual_clear(void)
{
    memset(visual_activation_queue,0,sizeof(visual_activation_queue)); visual_activation_head=visual_activation_count=0;
    memset(visual_stat_queue,0,sizeof(visual_stat_queue)); visual_stat_active_mask=0; visual_stat_count=0; visual_stat_cooldown=0; visual_popup_frames=0; visual_popup_kind=STAT_VISUAL_NORMAL; visual_popup_record_index=-1;
    memset(hungry_visual_pending,0,sizeof(hungry_visual_pending)); hungry_visual_pending_mask=0;
    memset(hungry_screen,0,sizeof(hungry_screen));
    memset(javelin_screen,0,sizeof(javelin_screen));
}

static void clear(void)
{
    ++selection_epoch;if(!selection_epoch)selection_ready=0;
    memset(borrowed_origin,0,sizeof(borrowed_origin));
    ultimate_trap_cinematic=0;
    memset(&ai_position_diag,0,sizeof(ai_position_diag));
    memset(&skull_ritual_diag,0,sizeof(skull_ritual_diag));
    memset(skull_marks,0,sizeof(skull_marks)); skull_live_mask=0;
    memset(chakra_marks,0,sizeof(chakra_marks));
    chakra_live_mask=chakra_pending_mask=0; memset(&chakra_sequence,0,sizeof(chakra_sequence));
    ultimate_swords_diag_scans=ultimate_swords_diag_issued=0;
    ultimate_swords_diag_last_result=0;
    field_info_cache.source=0;
    field_info_cache.index=-1;
    compat_recipe_restore();
    ritual_viewer_fit_reset();
    coin_reset();
    memset(performance_reward_reserved,0,sizeof(performance_reward_reserved));
    visual_clear();
    memset(mask_instances, 0, sizeof(mask_instances)); mask_instance_count=mask_instance_mask=0;
    mask_inside_placement = mask_known_sides = 0;
    mask_blocked = mask_learned = mask_released = mask_summons = 0;
    memset(fiend_mirror_marks, 0, sizeof(fiend_mirror_marks));
    mirror_lp_ready = 0;
    memset(mirror_commit_pending,0,sizeof(mirror_commit_pending));
    memset(mirror_commit_before,0,sizeof(mirror_commit_before));
    memset(mirror_commit_after,0,sizeof(mirror_commit_after));
    mirror_defense_direct_pending=0; mirror_defense_direct_side=-1;
    fiend_mirror_summons = 0;
    fiend_mirror_live_hint = 0; fiend_mirror_live_mask = 0;
    memset(&mirror_reflection, 0, sizeof(mirror_reflection));
    memset(mirror_queue, 0, sizeof(mirror_queue));
    mirror_queue_head = mirror_queue_tail = mirror_queue_count = 0;
    memset(mirror_battle_grace, 0, sizeof(mirror_battle_grace));
    battle_visual_survivor_parts=0;
    javelin_2d_survivor_parts=0;
    tri_2d_survivor_parts=tri_battle_charged_parts=0;
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) { forget(&D_801A7AD8[i]); forget_yamadron(&D_801A7AD8[i]); }
    summons = 0;
    shield_live_mask = 0;
    deferred_deactivation = 0;
    inside_field_actions = 0;
    inside_effect_destruction = 0;
    protected_visuals = 0;
    effect_survivor_visuals = 0;
    deferred_yamadron = 0;
    yamadron_summons = 0;
    yamadron_live_mask = 0;
    yamadron_generations = 0;
    generated_thumbnail_valid = 0;
    generated_thumbnail_touched = 0;
    memset(psycho_marks, 0, sizeof(psycho_marks)); psycho_controlling_count=psycho_pending_count=0;
    psycho_mark_mask=psycho_controlling_mask=psycho_pending_mask=0;
    psycho_summons = 0;
    psycho_steals = 0;
    memset(hungry_marks, 0, sizeof(hungry_marks)); memset(hungry_screen,0,sizeof(hungry_screen)); hungry_live_mask = 0;
    memset(javelin_marks, 0, sizeof(javelin_marks));
    memset(gate_marks, 0, sizeof(gate_marks));
    memset(garma_marks, 0, sizeof(garma_marks));
    memset(tri_marks,0,sizeof(tri_marks));memset(tri_screen,0,sizeof(tri_screen));
    memset(tri_guard_data,0,sizeof(tri_guard_data));
    tri_guard=tri_horn_visuals=tri_battle_visuals=tri_seen=0;
    tri_resolution_active=tri_resolution_id=tri_field_pending=0;ritual_field_id=UMI_ID;
    memset(garma_screen,0,sizeof(garma_screen));
    memset(garma_pending,0,sizeof(garma_pending)); garma_pending_mask=0;
    garma_relevant=-1;garma_relevant_frames=0;garma_animation_data=0;
    memset(crab_marks, 0, sizeof(crab_marks)); memset(whale_marks, 0, sizeof(whale_marks)); memset(serpent_marks,0,sizeof(serpent_marks)); serpent_seen=0; serpent_live_mask=0; memset(serpent_fear_bonus,0,sizeof(serpent_fear_bonus)); serpent_fear_battle_slot=-1; serpent_ghost_state=serpent_ghost_hold=0; serpent_art_loaded=0; serpent_visual_reset(); if(serpent_music_volume>=0){Spu_SetBusVolume(SPU_BUS_MUSIC,serpent_music_volume);serpent_music_volume=-1;}
    deferred_javelin = 0; javelin_summons = 0; javelin_live_mask = 0; javelin_protected_visuals = 0; gate_summons = 0; gate_state11_counted_mask = 0; garma_summons = 0; garma_visual_pending = 0;
    garma_live_hint = tri_live_hint = gate_live_hint = 0; garma_live_mask = tri_live_mask = gate_live_mask = 0; stat_effect_hints = 0;
    hand_aura_preview_record=-1;
    memset(war_lion_marks,0,sizeof(war_lion_marks)); war_lion_live_mask=0; deferred_war_lion=0;
    memset(sengenjin_marks,0,sizeof(sengenjin_marks)); sengenjin_live_mask=0; sengenjin_live_hint=0; sengenjin_battle_visual_mask=0; memset(sengenjin_negative,0,sizeof(sengenjin_negative)); memset(&sengenjin_trap,0,sizeof(sengenjin_trap));
    memset(bls_marks,0,sizeof(bls_marks)); bls_live_mask=0; bls_live_hint=0; bls_new_source_pending=0; memset(bls_prev_sources,0,sizeof(bls_prev_sources)); memset(bls_prev_data,0,sizeof(bls_prev_data)); memset(bls_prev_card_id,0,sizeof(bls_prev_card_id)); bls_field_pending=0;
    memset(zera_marks,0,sizeof(zera_marks)); zera_live_mask=0; zera_field_pending=0;
    memset(ultimate_marks,0,sizeof(ultimate_marks)); ultimate_live_mask=0; ultimate_live_hint=0; memset(&ultimate_card_effect,0,sizeof(ultimate_card_effect));
    memset(&ultimate_ai_magic_attempt,0,sizeof(ultimate_ai_magic_attempt));
    memset(ritual_ai_effect_known,0,sizeof(ritual_ai_effect_known));
    memset(cosmo_marks,0,sizeof(cosmo_marks)); cosmo_live_mask=0; cosmo_live_hint=0; memset(cosmo_turn_fulfilled,0,sizeof(cosmo_turn_fulfilled));
    memset(dokurorider_marks,0,sizeof(dokurorider_marks)); dokurorider_live_mask=0; dokurorider_live_hint=0; dokurorider_new_source_pending=0; dokurorider_evade_pending=0; dokurorider_evade_hold=0;
    memset(dokurorider_prev_sources,0,sizeof(dokurorider_prev_sources)); memset(dokurorider_prev_data,0,sizeof(dokurorider_prev_data)); memset(dokurorider_prev_card_id,0,sizeof(dokurorider_prev_card_id));
    crab_restore_visuals = 0; crab_summons = whale_summons = 0; crab_live_mask = whale_live_mask = 0; crab_umi_pending = 0; crab_umi_sequence = 0; whale_destruction_guard = 0;
    crab_umi_visual_state = 0; crab_umi_visual_hold = 0; crab_umi_camera_delay = 0; ritual_field_source_card_id=0; crab_umi_overlay_add = crab_umi_overlay_sub = 0;
}


static int roll_burn_card(void)
{
    int roll = rand() % 100;
    if (roll < 50) return FINAL_FLAME_ID;
    if (roll < 85) return OOKAZI_ID;
    return TREMENDOUS_FIRE_ID;
}


static int upload_generated_card_thumbnail(int index, int card_id)
{
    RECT *art;
    RECT *name;
    if (!host || !host->disc_file_start || !host->disc_read) return 0;
    if (card_mrg_lba == -2)
        card_mrg_lba = host->disc_file_start(host, "\\DATA\\WA_MRG.MRG;1");
    if (card_mrg_lba < 0) return 0;

    /* WA_MRG.MRG starts with one 2048-byte thumbnail/data sector per retail
     * card. The enlarged/effect artwork begins after CARD_COUNT sectors.
     * A generated Yamadron Magic did not pass through the combined-deck
     * loader, so upload this retail card's field thumbnail directly into the
     * VRAM rectangles owned by its field slot. This leaves every deck block
     * untouched. */
    if (!host->disc_read(host, card_mrg_lba + card_id - 1, 1,
                         generated_thumbnail_sector))
        return 0;

    memcpy(generated_thumbnail_cache[index], generated_thumbnail_sector,
           DUEL_CARD_DATA_BLOCK_SIZE);
    /* The private copy keeps its real disc origin for HD texture lookup. */
    TextureDump_Delivered(generated_thumbnail_cache[index],DUEL_CARD_DATA_BLOCK_SIZE,
                          card_mrg_lba+card_id-1,0);
    generated_thumbnail_valid |= (1u << index);
    generated_thumbnail_touched = 1;

    /* Do NOT copy this block into D_8018C2D8. That cache belongs to the
     * combined decks; field-record indices are not private image-block
     * indices. v0.13 proved WA_MRG.MRG is the correct source, but writing
     * index * 0x580 into that cache replaced thumbnails used by cards in
     * the hand. Generated cards only need their field-slot VRAM rectangles
     * refreshed here. */

    art = &D_80177EA4[index * 2];
    art->w = 0x14;
    art->h = 0x20;
    art->x = (index % DUEL_FIELD_ROW_SIZE) * 0x14 + 0x380;
    art->y = (index / DUEL_FIELD_ROW_SIZE) * 0x20;
    LoadImage(art, (u32 *)generated_thumbnail_cache[index]);

    name = &D_80177EA4[index * 2 + 1];
    name->x = 0x380;
    name->y = index + 0xE0;
    name->w = 0x40;
    name->h = 1;
    LoadImage(name, (u32 *)(generated_thumbnail_cache[index] + 0x500));

    /* LoadImage starts a GPU transfer. Yamadron can generate the next Magic
     * immediately and reuse generated_thumbnail_sector for its disc read.
     * Wait for both uploads to finish before that buffer can be overwritten;
     * otherwise the viewer can receive partially replaced pixel/palette data. */
    DrawSync(0);
    return 1;
}

static int create_burn_card_in_slot(int index, int card_id)
{
    DuelCardRecord *card;
    DuelCardDisplayObject *obj;
    if (index < 0 || index >= DUEL_CARD_RECORD_COUNT || !magic_slot(index)) return 0;
    card = &D_801A7AD8[index];
    if (card->flags & DUEL_CARD_FLAG_OCCUPIED) return 0;

    generated_data[index].id = (s16)card_id;
    generated_data[index].deck_index = -1;
    generated_data[index].data_block_index = (u8)index;
    generated_data[index].flags_04 = 0;
    generated_data[index].unk_05 = 0;

    card->data = &generated_data[index];
    card->card_id = (s16)card_id;
    card->attack = 0;
    card->defense = 0;
    card->stat_modifier = 0;
    card->terrain_modifier = 0;
    card->flags = DUEL_CARD_FLAG_OCCUPIED | DUEL_CARD_FLAG_FACE_DOWN;
    card->table_index = (u8)index;
    obj = func_80024C1C(card_id, D_800908A0[index].x, D_800908A0[index].y);
    card->object = obj;
    if (obj) obj->card_index = (u8)index;
    if (!upload_generated_card_thumbnail(index, card_id) && host && host->log)
        host->log(host, "[Ritual Card Effects] aviso: arte da Magic gerada nao foi carregada (slot %d, carta %d)", index, card_id);
    return 1;
}

/* Read-only comparison: unlike StoreImage this does not flush the renderer.
 * Check actual VRAM, not our last upload, because other mods/native scenes
 * may have changed the atlas. Keep existing HD tags when pixels already match. */
static int thumbnail_rect_changed(const RECT *r,const unsigned char *block)
{
    const uint16_t *vram=SoftGpu_Vram();
    int row;
    if(!vram)return 1;
    for(row=0;row<r->h;++row)
        if(memcmp(vram+(r->y+row)*SOFT_GPU_WIDTH+r->x,
                  block+row*r->w*2,r->w*2))return 1;
    return 0;
}
static int upload_slot_thumbnail_from_block(int index, const unsigned char *block)
{
    int changed=0;
    RECT *art = &D_80177EA4[index * 2];
    RECT *name = &D_80177EA4[index * 2 + 1];
    art->w = 0x14; art->h = 0x20;
    art->x = (index % DUEL_FIELD_ROW_SIZE) * 0x14 + 0x380;
    art->y = (index / DUEL_FIELD_ROW_SIZE) * 0x20;
    if(thumbnail_rect_changed(art,block)){LoadImage(art,(u32 *)block);changed=1;}
    name->x = 0x380; name->y = index + 0xE0;
    name->w = 0x40; name->h = 1;
    if(thumbnail_rect_changed(name,block+0x500)){LoadImage(name,(u32 *)(block+0x500));changed=1;}
    return changed;
}

/* The retail renderer deliberately reuses parts of the card-thumbnail VRAM
 * atlas between the hand and the field.  A generated Yamadron card has no
 * combined-deck entry, so it cannot rely on Duel_SetupCardRecord to restore
 * its pixels when the scene changes.  Keep a private RAM copy and restore the
 * side of the atlas that is about to be used instead of leaving one view's
 * pixels resident while the other view is drawn. */
static void restore_generated_field_thumbnails(void)
{
    unsigned pending = generated_thumbnail_valid;
    int any = 0;
    if(!pending)return;
    /* The validity mask already names the exact generated Magic/Trap slots.
     * Iterate only those cards instead of revisiting all ten M/T zones on
     * every field refresh while generated art is live. */
    while(pending) {
        int i=take_mask_bit(&pending);
        DuelCardRecord *card = &D_801A7AD8[i];
        if (!magic_slot(i) || !(card->flags & DUEL_CARD_FLAG_OCCUPIED) || card->data != &generated_data[i]) {
            generated_thumbnail_valid &= ~(1u << i);
            continue;
        }
        any |= upload_slot_thumbnail_from_block(i, generated_thumbnail_cache[i]);
    }
    if (any) DrawSync(0);
}

static void restore_hand_thumbnails(void)
{
    int side,i;
    int any = 0;
    if(!generated_thumbnail_touched)return;
    for(side=0;side<DUEL_SIDE_COUNT;++side) {
        int start=side?15:0;
        for(i=start;i<start+5;++i) {
            DuelCardRecord *card=&D_801A7AD8[i];
            DuelDeckCardRecord *deck;
            int off;
            if (!(card->flags & DUEL_CARD_FLAG_OCCUPIED) || !card->data) continue;
            deck = (DuelDeckCardRecord *)card->data;
            if (deck < gDuel_aDeckCardRecords ||
                deck >= gDuel_aDeckCardRecords + COMBINED_DECK_SIZE) continue;
            off = deck->data_block_index * DUEL_CARD_DATA_BLOCK_SIZE;
            if(deck->data_block_index>=COMBINED_DECK_SIZE)continue;
            any |= upload_slot_thumbnail_from_block(i, D_8018C2D8 + off);
        }
    }
    if (any) DrawSync(0);
    if(!generated_thumbnail_valid)generated_thumbnail_touched=0;
}

static int find_occupied_monster_by_data(void *data)
{
    int i;
    if (!data) return -1;
    for (i = 5; i < 10; ++i)
        if ((D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].data == data) return i;
    for (i = 20; i < 25; ++i)
        if ((D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].data == data) return i;
    return -1;
}

/* Psycho-Puppet is identity-based, but its source and borrowed monster spend
 * almost all of their lifetime in known field slots.  Validate those cached
 * slots first and only pay the ten-zone identity search after retail actually
 * moved/rebuilt a record.  Falling back to data identity preserves the old
 * behaviour exactly. */
static int psycho_source_slot(PsychoMark *m)
{
    int slot; DuelCardRecord *c;
    if(!m || !m->ritual || !m->psycho_data)return -1;
    slot=m->psycho_slot;
    if(monster_slot(slot)) {
        c=&D_801A7AD8[slot];
        if((c->flags&DUEL_CARD_FLAG_OCCUPIED) && c->data==m->psycho_data && c->card_id==PSYCHO_PUPPET_ID)return slot;
    }
    slot=find_occupied_monster_by_data(m->psycho_data);
    if(slot>=0 && D_801A7AD8[slot].card_id==PSYCHO_PUPPET_ID){m->psycho_slot=slot;return slot;}
    m->psycho_slot=-1;
    return -1;
}
static int psycho_stolen_slot(PsychoMark *m)
{
    int slot; DuelCardRecord *c;
    if(!m || !m->controlling || !m->stolen_data)return -1;
    slot=m->current_slot;
    if(monster_slot(slot)) {
        c=&D_801A7AD8[slot];
        if((c->flags&DUEL_CARD_FLAG_OCCUPIED) && c->data==m->stolen_data)return slot;
    }
    slot=find_occupied_monster_by_data(m->stolen_data);
    m->current_slot=slot;
    return slot;
}

static PsychoMark *find_psycho_mark(void *data)
{
    unsigned pending=psycho_mark_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        if (psycho_marks[i].ritual && psycho_marks[i].psycho_data == data)
            return &psycho_marks[i];
    }
    return 0;
}

static PsychoMark *alloc_psycho_mark(void *data)
{
    PsychoMark *m = find_psycho_mark(data);
    int i;
    if (m) return m;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (!psycho_marks[i].ritual) {
            memset(&psycho_marks[i], 0, sizeof(psycho_marks[i]));
            psycho_marks[i].psycho_data = data;
            psycho_marks[i].ritual = 1;
            psycho_marks[i].psycho_slot = -1;
            psycho_marks[i].original_slot = -1;
            psycho_marks[i].current_slot = -1;
            psycho_mark_mask |= 1u << i;
            return &psycho_marks[i];
        }
    }
    return 0;
}

static void refresh_moved_monster_visual(int index)
{
    DuelCardRecord *card;
    DuelDeckCardRecord *deck;
    DuelCardDisplayObject *obj;
    int off;
    if (index < 0 || index >= DUEL_CARD_RECORD_COUNT || !monster_slot(index)) return;
    card = &D_801A7AD8[index];
    if (!(card->flags & DUEL_CARD_FLAG_OCCUPIED) || !card->data) return;
    deck = (DuelDeckCardRecord *)card->data;
    if (deck >= gDuel_aDeckCardRecords && deck < gDuel_aDeckCardRecords + COMBINED_DECK_SIZE) {
        off = deck->data_block_index * DUEL_CARD_DATA_BLOCK_SIZE;
        upload_slot_thumbnail_from_block(index, D_8018C2D8 + off);
        DrawSync(0);
    }
    if (card->object) DisplayObject_ReleaseIfPresent(card->object);
    obj = func_80024C1C(card->card_id, D_800908A0[index].x, D_800908A0[index].y);
    card->object = obj;
    if (obj) {
        obj->card_index = (u8)index;
        /* The retail battle controller rebuilds surviving cards using its
         * display object's field_6B as the combined-deck index. The generic
         * icon constructor does NOT initialize it. Leaving it zero made a
         * CPU-borrowed monster respawn as deck card #0 after combat. Preserve
         * its real source when ownership changes in either direction. */
        obj->field_6B = (u8)deck->deck_index;
    }
}

/* During battle retail briefly removes field records and rebuilds them from
 * the display object's combined-deck index. A borrowed card is still tied to
 * the ORIGINAL deck record, never to the controller's local deck. This hook
 * covers CPU-side reconstruction as well as the already-working human path.
 * It is inert for all non-borrowed cards. */
static void puppet_safe_rebuild(s32 slot,s32 deck_index)
{
    unsigned pending=psycho_controlling_mask;
    PsychoMark *borrowed=0;
    while(pending) {
        int i=take_mask_bit(&pending);
        PsychoMark *m=&psycho_marks[i];
        if(m->ritual && m->controlling && m->current_slot==slot && m->stolen_data) {
            borrowed=m;break;
        }
    }
    if(borrowed) {
        DuelDeckCardRecord *deck=(DuelDeckCardRecord *)borrowed->stolen_data;
        if(deck>=gDuel_aDeckCardRecords && deck<gDuel_aDeckCardRecords+COMBINED_DECK_SIZE) {
            /* Do not use an encoded side-relative slot (0x80+n) here. */
            deck_index=(s32)deck->deck_index;
        }
    }
    original_rebuild_field_card(slot,deck_index);
    if(borrowed && monster_slot(slot)) {
        DuelCardRecord *card=&D_801A7AD8[slot];
        /* The base deck card can differ from its summoned/fused identity.
         * Restore the captured monster, never a magic or another deck card. */
        if(card->card_id!=borrowed->stolen_card_id || card->data!=borrowed->stolen_data) {
            card->card_id=(s16)borrowed->stolen_card_id;
            card->data=borrowed->stolen_data;
            card->attack=borrowed->stolen_snapshot.attack;
            card->defense=borrowed->stolen_snapshot.defense;
            refresh_moved_monster_visual(slot);
            if(host&&host->log)host->log(host,
                "[Ritual Card Effects] Puppet: restaurada identidade real da marionete %d (slot %d)",
                borrowed->stolen_card_id,slot);
        }
        borrowed->current_slot=slot;
    }
}

static int move_monster_record(int from, int to)
{
    DuelCardRecord moved;
    if (!monster_slot(from) || !monster_slot(to)) return 0;
    if (!(D_801A7AD8[from].flags & DUEL_CARD_FLAG_OCCUPIED)) return 0;
    if (D_801A7AD8[to].flags & DUEL_CARD_FLAG_OCCUPIED) return 0;
    moved = D_801A7AD8[from];
    if (moved.object) DisplayObject_ReleaseIfPresent(moved.object);
    moved.object = 0;
    D_801A7AD8[to] = moved;
    D_801A7AD8[to].table_index = (u8)to;
    D_801A7AD8[from].object = 0;
    D_801A7AD8[from].data = 0;
    D_801A7AD8[from].flags = 0;
    refresh_moved_monster_visual(to);
    return 1;
}

static int first_free_monster_slot(int side)
{
    int i;
    int start = side ? 20 : 5;
    for (i = start; i < start + 5; ++i)
        if (!(D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED)) return i;
    return -1;
}

static int random_enemy_monster_slot(int side)
{
    int candidates[5];
    int count = 0;
    int i;
    int start = side ? 5 : 20;
    for (i = start; i < start + 5; ++i)
        if (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED)
            candidates[count++] = i;
    if (!count) return -1;
    return candidates[rand() % count];
}

static int psycho_attempt_control(PsychoMark *m, const char *reason)
{
    if (!ritual_card_effect_enabled(PSYCHO_PUPPET_ID)) return 0;
    int psycho_slot, target_slot, free_slot, side;
    DuelCardRecord *psycho;
    if (!m || !m->ritual || m->controlling) return 0;
    psycho_slot = psycho_source_slot(m);
    if (psycho_slot < 0) return 0;
    psycho = &D_801A7AD8[psycho_slot];
    if (psycho->card_id != PSYCHO_PUPPET_ID) return 0;
    side = side_for_record(psycho_slot);
    free_slot = first_free_monster_slot(side);
    if (free_slot < 0) return 0;
    target_slot = random_enemy_monster_slot(side);
    if (target_slot < 0) return 0;
    /* Psycho-Puppet does not reroll when its random target is immune.  The
     * chosen target consumes this activation exactly as specified. */
    if(RitualEffects_IsCardEffectImmune(target_slot,side)) {
        visual_queue_activation(target_slot);
        if(host&&host->log)host->log(host,"[Ritual Card Effects] Psycho-Puppet falhou: Blue-Eyes Ultimate imune no slot %d",target_slot);
        return 0;
    }

    m->stolen_data = D_801A7AD8[target_slot].data;
    m->stolen_card_id = D_801A7AD8[target_slot].card_id;
    m->stolen_snapshot = D_801A7AD8[target_slot];
    m->original_slot = target_slot;
    m->current_slot = free_slot;
    m->controller_side = side;
    if (!move_monster_record(target_slot, free_slot)) {
        m->stolen_data = 0;
        m->original_slot = m->current_slot = -1;
        return 0;
    }
    /* The controlled monster may attack this turn. Psycho-Puppet itself may
     * not attack while its control effect is active. */
    D_801A7AD8[free_slot].flags &= ~DUEL_CARD_FLAG_USED_THIS_TURN;
    psycho->flags |= DUEL_CARD_FLAG_USED_THIS_TURN | DUEL_CARD_FLAG_DEFENSE_POSITION;
    m->controlling = 1;
    psycho_controlling_mask |= 1u << (unsigned)(m - psycho_marks);
    ++psycho_controlling_count;
    /* Presentation only: show the source, then the chosen monster.  The
     * gameplay transfer above remains unchanged. */
    if (!m->visual_shown) {
        visual_queue_activation(psycho_slot);
        visual_queue_activation(free_slot);
        m->visual_shown = 1;
    }
    ++psycho_steals;
    if (host && host->log)
        host->log(host, "[Ritual Card Effects] Psycho-Puppet %s: carta %d roubada do slot %d para %d",
                  reason, m->stolen_card_id, target_slot, free_slot);
    return 1;
}

static void psycho_return_control(PsychoMark *m)
{
    int current, destination;
    if (!m || !m->controlling) return;
    current = psycho_stolen_slot(m);
    if (current >= 0) {
        /* Brief marker at the borrowed monster before retail ownership is
         * restored.  The queued position survives the subsequent move. */
        visual_queue_activation(current);
        destination = m->original_slot;
        if (!monster_slot(destination) ||
            (D_801A7AD8[destination].flags & DUEL_CARD_FLAG_OCCUPIED))
            destination = first_free_monster_slot(m->controller_side ^ 1);
        if (destination < 0 || !move_monster_record(current,destination)) {
            /* Both zones blocked / staged record: retry at the next safe
             * transition. Never orphan a borrowed live card by clearing its
             * ownership mark without moving it back. */
            if(host && host->log) host->log(host,
                "[Ritual Card Effects] Psycho-Puppet: devolucao pendente, slot %d",current);
            return;
        }
    } else if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK)==9) {
        return; /* Participant is temporarily deactivated in retail battle. */
    }
    /* If the marionette is gone, retire the link; never recreate a dead card. */
    m->controlling = 0;
    psycho_controlling_mask &= ~(1u << (unsigned)(m - psycho_marks));
    if(psycho_controlling_count)--psycho_controlling_count;
    m->stolen_data = 0;
    m->stolen_card_id = 0;
    m->original_slot = m->current_slot = -1;
}

static void psycho_return_after_master_death(void)
{
    unsigned pending=psycho_controlling_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        PsychoMark *m=&psycho_marks[i];
        if(!m->ritual || !m->controlling)continue;
        /* The Puppet dying does not immediately release the stolen card.
         * Raigeki and all subsequent destruction in this same turn resolve
         * first. Only a surviving marionette goes home at turn transition. */
        if(psycho_source_slot(m)<0)psycho_return_control(m);
    }
}

static void psycho_trigger_for_side(int side, const char *reason)
{
    unsigned pending=psycho_mark_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        int slot;
        if (!psycho_marks[i].ritual || psycho_marks[i].controlling) continue;
        slot = psycho_source_slot(&psycho_marks[i]);
        if (slot >= 0 && side_for_record(slot) == side &&
            D_801A7AD8[slot].card_id == PSYCHO_PUPPET_ID)
            psycho_attempt_control(&psycho_marks[i], reason);
    }
}

static int data_is_stolen(void *data, int side)
{
    unsigned pending=psycho_controlling_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        if (psycho_marks[i].ritual && psycho_marks[i].controlling &&
            psycho_marks[i].controller_side == side &&
            psycho_marks[i].stolen_data == data)
            return 1;
    }
    return 0;
}



/* Resolve the redirect by field RECORD, not by ID: another normally
 * summoned Puppet must not borrow the ritual Puppet's effect. This is called
 * only before retail commits an attack target, while both cards are live. */
static int psycho_redirect_target_slot(int attempted,int attacking_side)
{
    unsigned pending=psycho_controlling_mask;
    if (!ritual_card_effect_enabled(PSYCHO_PUPPET_ID) ||
        !monster_slot(attempted) || side_for_record(attempted)==attacking_side)
        return attempted;
    while(pending) {
        int i=take_mask_bit(&pending);
        PsychoMark *m=&psycho_marks[i];
        int borrowed;
        if(!m->ritual || !m->controlling ||
           m->controller_side==attacking_side ||
           psycho_source_slot(m)!=attempted)continue;
        borrowed=psycho_stolen_slot(m);
        if(borrowed>=0 && borrowed!=attempted &&
           side_for_record(borrowed)==side_for_record(attempted) &&
           (D_801A7AD8[borrowed].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           D_801A7AD8[borrowed].data==m->stolen_data)
            return borrowed;
    }
    return attempted;
}

/* The retail UI and CPU both funnel to phase 5 but express target columns
 * differently. The map is authoritative; never assume logical side 0 is
 * the player or that field-record indices increase left-to-right. */
static int psycho_enemy_column(int side,int record)
{
    int col;
    if(side<0 || side>=DUEL_SIDE_COUNT)return -1;
    for(col=0;col<5;++col)
        if(D_800907D8[side*20+5+col]==record)return col;
    return -1;
}
static void psycho_redirect_attack_selection(void)
{
    int side=D_8009B1D5,col,slot,replacement,new_col;
    DuelSelectionRecord *cursor;
    if(!psycho_controlling_count ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5 ||
       side<0 || side>=DUEL_SIDE_COUNT)return;
    if ((D_8009B174&0x0f)==5 && !(D_8009B174&0x80)) {
        /* CPU phase: D_800EAE92 is the enemy selection 60-col. */
        int selection=(int)(u8)D_800EAE92;
        if(selection<56 || selection>60)return;
        col=60-selection;
        slot=D_800907D8[side*20+5+col];
        replacement=psycho_redirect_target_slot(slot,side);
        if(replacement==slot)return;
        new_col=psycho_enemy_column(side,replacement);
        if(new_col>=0){
            D_800EAE92=(u8)(60-new_col);
            if(host&&host->log)host->log(host,
                "[Ritual Card Effects] Psycho-Puppet redirecionou ataque CPU: %d -> %d",slot,replacement);
        }
    } else if((D_8009B174&0x0f)==6 &&
              (gInput_wPad1Pressed&0xC0)) {
        /* Human selection: change the commit cursor before native state 6
         * snapshots/deactivates the two participants. */
        cursor=&((DuelSelectionSide *)D_800E9F10)[side].records[3];
        if(cursor->row!=1 || cursor->col<0 || cursor->col>=5)return;
        slot=D_800907D8[side*20+5+cursor->col];
        replacement=psycho_redirect_target_slot(slot,side);
        if(replacement==slot)return;
        new_col=psycho_enemy_column(side,replacement);
        if(new_col>=0)cursor->col=(s8)new_col;
    }
}

static s32 psycho_check_fusion(s32 a, s32 b)
{
    if (!ritual_card_effect_enabled(PSYCHO_PUPPET_ID) || !psycho_controlling_count) return original_check_fusion(a, b);
    unsigned pending=psycho_controlling_mask;
    /* Duel_CheckFusion only receives IDs, not record pointers. During the
     * temporary-control window reject a pair containing the stolen card's ID.
     * This prevents the borrowed monster from being consumed as Fusion
     * material; the restriction disappears as soon as it returns. */
    while(pending) {
        int i=take_mask_bit(&pending);
        if (psycho_marks[i].ritual && psycho_marks[i].controlling &&
            psycho_marks[i].controller_side == D_8009B1D5 &&
            (a == psycho_marks[i].stolen_card_id || b == psycho_marks[i].stolen_card_id))
            return 0;
    }
    return original_check_fusion(a, b);
}

static s32 mask_check_fusion(s32 a, s32 b)
{ return mask_filter_fusion(psycho_check_fusion(a, b),a,b); }

static s32 psycho_check_ritual(DuelRitualResult *out, s32 ritual_id)
{
    if (!ritual_card_effect_enabled(PSYCHO_PUPPET_ID)) return original_check_ritual(out, ritual_id);
    s32 result = original_check_ritual(out, ritual_id);
    int i;
    if (!result || !out) return result;
    for (i = 0; i < DUEL_RITUAL_TRIBUTE_COUNT; ++i) {
        int r = record_index((DuelCardRecord *)out->tribute_objects[i]);
        if (r >= 0 && data_is_stolen(D_801A7AD8[r].data, D_8009B1D5))
            return 0;
    }
    return result;
}

static int cosmo_enemy_live_for_side(int side)
{
    unsigned pending;
    if(side<0 || side>=DUEL_SIDE_COUNT || !cosmo_live_hint ||
       !ritual_card_effect_enabled(COSMO_QUEEN_ID))return 0;
    pending=cosmo_live_mask & side_monster_mask(side^1);
    while(pending) {
        int i=take_mask_bit(&pending);
        if(cosmo_live(i))return 1;
    }
    return 0;
}

static int cosmo_hand_card_id(int side,int hand_slot)
{
    int n;
    DuelCardStagingDeckView *view=(DuelCardStagingDeckView*)D_8015C424;
    if(side<0 || side>=DUEL_SIDE_COUNT || hand_slot<0 || hand_slot>=HAND_SIZE)return 0;
    n=D_800E9FF0[side].hand[hand_slot];
    if(n<0 || n>=COMBINED_DECK_SIZE)return 0;
    return view->cards[n].id;
}

/* Reuse the existing five-slot logical hand accessor; never inspect the
 * AI deck window, unused staging slots or the full combined deck. */
static unsigned skull_hand_count(int side)
{
    int slot;unsigned total=0;
    if(side<0 || side>=DUEL_SIDE_COUNT)return 0;
    for(slot=0;slot<HAND_SIZE;++slot)
        if(RitualCategory_IsSkullMonster(cosmo_hand_card_id(side,slot)))++total;
    return total;
}
static unsigned skull_field_count_except(int excluded)
{
    int side,slot;unsigned total=0;
    for(side=0;side<DUEL_SIDE_COUNT;++side)
        for(slot=side*DUEL_CARD_SIDE_RECORD_COUNT+5;
            slot<side*DUEL_CARD_SIDE_RECORD_COUNT+10;++slot) {
            DuelCardRecord *c=&D_801A7AD8[slot];
            if(beacon_is_other_physical_record(slot,excluded) && ((c->flags&DUEL_CARD_FLAG_OCCUPIED) ||
                 ((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
                  battle_participant(slot))) &&
               RitualCategory_IsSkullMonster(c->card_id))++total;
        }
    return total;
}
static unsigned skull_bonus_for(int index)
{
    return skull_field_count_except(index)*SKULL_FIELD_BONUS+
        skull_hand_count(side_for_record(index))*SKULL_HAND_BONUS;
}
static int skull_identity(int index)
{
    DuelCardRecord *c;
    if(index<0 || index>=DUEL_CARD_RECORD_COUNT || !monster_slot(index))return 0;
    c=&D_801A7AD8[index];
    return skull_marks[index].ritual && c->card_id==SKULL_GUARDIAN_ID &&
        c->data && skull_marks[index].data==c->data;
}
static int skull_live(int index)
{
    return skull_identity(index) &&
        (D_801A7AD8[index].flags&DUEL_CARD_FLAG_OCCUPIED)!=0;
}

/* Ritual-only Female definition: the game's actual [Female] fusion group,
 * plus the two approved exceptions, regardless of their principal types.
 * Do NOT infer Female from every monster producible by a Female fusion. */
static int cosmo_is_female_monster(int id)
{
    unsigned stats;
    int type;
    if(id<=0 || id>722)return 0;
    stats=(unsigned)gDuel_adwCardStats[id-1];
    type=(stats>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK;
    return type<CARD_TYPE_MAGIC && (id==COSMO_QUEEN_ID ||
        id==MYSTICAL_SAND_ID ||
        Cards_InFusionGroup(id,CARD_FUSION_GROUP_FEMALE));
}

static int cosmo_female_destination(int side,int hand_slot)
{
    int id=cosmo_hand_card_id(side,hand_slot),start,i;
    if(!cosmo_is_female_monster(id))return -1;
    start=side?20:5;
    /* Ordinary summon/placement is always legal when a Monster Zone is free. */
    for(i=0;i<DUEL_FIELD_ROW_SIZE;++i)
        if(!(D_801A7AD8[start+i].flags&DUEL_CARD_FLAG_OCCUPIED))return i;
    /* With a full row the Female can still be the only hand card used in a
     * legal Fusion with a monster already on the field. */
    for(i=0;i<DUEL_FIELD_ROW_SIZE;++i) {
        DuelCardRecord *field=&D_801A7AD8[start+i];
        if((field->flags&DUEL_CARD_FLAG_OCCUPIED) && Duel_CheckFusion(id,field->card_id))return i;
    }
    return -1;
}

static int cosmo_lock_active(int side,int *hand_slot,int *dest_col)
{
    int h,d;
    if(side<0 || side>=DUEL_SIDE_COUNT || cosmo_turn_fulfilled[side] ||
       !cosmo_enemy_live_for_side(side))return 0;
    /* The lock is derived only from the five real hand slots. Deck tails and
     * AI Hard Mode's expanded candidates are deliberately never consulted. */
    for(h=0;h<HAND_SIZE;++h) {
        d=cosmo_female_destination(side,h);
        if(d>=0) {
            if(hand_slot)*hand_slot=h;
            if(dest_col)*dest_col=d;
            return 1;
        }
    }
    return 0;
}

static int cosmo_ai_selection_complies_bytes(int side,const unsigned char *selection)
{
    int i,used=0;
    /* Preserve the AI's own choice when every hand material it selected is a
     * Female from the five physical hand slots.  This keeps free choice among
     * multiple Females (and Female+Female chains) while still rejecting deck
     * window/pool candidates and non-Female hand material. */
    for(i=0;i<HAND_SIZE && selection[i];++i) {
        int slot=(int)selection[i],hand_slot;
        if(slot<AI_SLOT_OWN_HAND_FIRST || slot>=AI_SLOT_OWN_HAND_FIRST+HAND_SIZE)return 0;
        hand_slot=slot-AI_SLOT_OWN_HAND_FIRST;
        if(!cosmo_is_female_monster(cosmo_hand_card_id(side,hand_slot)))return 0;
        used=1;
    }
    return used;
}

/* Same rule and preference as the original wrapper; writes only caller output. */
static int cosmo_constrain_selection(int side,const unsigned char *selection,
                                   unsigned char *out)
{
    int hand_slot=-1,dest=-1,face;
    memcpy(out,selection,12);
    if(!cosmo_lock_active(side,&hand_slot,&dest))return 0;
    if(cosmo_ai_selection_complies_bytes(side,selection))return 1;
    face=selection[8]&1;
    memset(out,0,12);out[0]=(u8)(AI_SLOT_OWN_HAND_FIRST+hand_slot);
    out[6]=(u8)(dest+1);out[8]=(u8)face;
    return 1;
}
static void cosmo_force_ai_selection(void)
{
    unsigned char out[12];
    cosmo_constrain_selection(D_8009B1D5,(const unsigned char*)D_800EAE88,out);
    memcpy((void*)D_800EAE88,out,12);
}
#include "mandatory_hand_provider.h"

static void cosmo_hand_swap(void)
{
    if(selection_precommit()<0)return;
    if(D_8009B1D5<DUEL_SIDE_COUNT && D_8009B360[D_8009B1D5]>=0)
        cosmo_force_ai_selection();
    original_hand_swap();
}

static void hand_actions(void)
{
    int cosmo_side=-1,cosmo_lock_before=0,cosmo_before[HAND_SIZE],h;
    if(chaos_turn.active)return;
    for(h=0;h<HAND_SIZE;++h)cosmo_before[h]=-1;
    restore_hand_thumbnails();
    if(D_8009B1D5<DUEL_SIDE_COUNT) {
        int legal_slot=-1,dest=-1;
        cosmo_side=D_8009B1D5;
        cosmo_lock_before=cosmo_lock_active(cosmo_side,&legal_slot,&dest);
        if(cosmo_lock_before) {
            /* Remember only Female cards that physically occupy the five real
             * hand slots before this hand-action tick.  A deck-window or pool
             * candidate can never satisfy the obligation. */
            for(h=0;h<HAND_SIZE;++h) {
                int id=cosmo_hand_card_id(cosmo_side,h);
                if(cosmo_is_female_monster(id))
                    cosmo_before[h]=D_800E9FF0[cosmo_side].hand[h];
            }
        }
        if(D_8009B360[cosmo_side]<0 && cosmo_lock_before) {
            DuelSelectionSide *sides=(DuelSelectionSide*)D_800E9F10;
            DuelSelectionRecord *sel=&sides[cosmo_side].records[0];
            int state=D_8009B174&0x0f;
            int current=(s8)sel->field_0E;
            int id=cosmo_hand_card_id(cosmo_side,current);
            if((state==1 || state==3) && !cosmo_is_female_monster(id) &&
               (gInput_wPad1Pressed&0xC0)) {
                gInput_wPad1Pressed&=~0xC0;
                SD_SEPlayFull(9);
            }
        }
    }
    original_hand_actions();
    if(cosmo_lock_before && cosmo_side>=0 && cosmo_side<DUEL_SIDE_COUNT &&
       !cosmo_turn_fulfilled[cosmo_side]) {
        /* Selection alone does not satisfy Queen Lock: the physical hand slot
         * must actually change after the native hand action commits the play.
         * This lets Female + field-monster Fusion satisfy the rule, while a
         * canceled selection or a merely highlighted Female does not. */
        for(h=0;h<HAND_SIZE;++h) {
            if(cosmo_before[h]>=0 &&
               D_800E9FF0[cosmo_side].hand[h]!=cosmo_before[h]) {
                cosmo_turn_fulfilled[cosmo_side]=1;
                if(host&&host->log)
                    host->log(host,"[Ritual Card Effects] Cosmo Queen: Queen Lock cumprido pelo lado %d",cosmo_side);
                break;
            }
        }
    }
}

static int generate_yamadron_burns(int yamadron_index, const char *reason)
{
    if (!ritual_card_effect_enabled(YAMADRON_ID)) return 0;
    int side, slot, made = 0;
    int made_slots[3] = {-1,-1,-1};
    DuelCardRecord *y;
    if (yamadron_index < 0 || yamadron_index >= DUEL_CARD_RECORD_COUNT) return 0;
    y = &D_801A7AD8[yamadron_index];
    if (!yamadron_marks[yamadron_index].ritual ||
        !(y->flags & DUEL_CARD_FLAG_OCCUPIED) ||
        y->data != yamadron_marks[yamadron_index].data || y->card_id != YAMADRON_ID) {
        forget_yamadron(y);
        return 0;
    }
    side = side_for_record(yamadron_index);
    for (slot = side ? 25 : 10; slot < (side ? 30 : 15) && made < 3; ++slot) {
        int id;
        if (D_801A7AD8[slot].flags & DUEL_CARD_FLAG_OCCUPIED) continue;
        id = roll_burn_card();
        if (create_burn_card_in_slot(slot, id)) {
            made_slots[made] = slot;
            ++made;
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Yamadron %s: slot %d recebeu carta %d", reason, slot, id);
        }
    }
    if (made) {
        ++yamadron_generations;
        if (!yamadron_marks[yamadron_index].visual_shown) {
            int v;
            yamadron_marks[yamadron_index].visual_shown = 1;
            for (v = 0; v < made; ++v) if (made_slots[v] >= 0) visual_queue_activation(made_slots[v]);
        }
    }
    return made;
}

static void generate_for_side(int side, const char *reason)
{
    unsigned pending=yamadron_live_mask;
    int start=side?20:5, end=start+5;
    while(pending) {
        int i=take_mask_bit(&pending);
        if(i>=start && i<end && yamadron_marks[i].ritual) generate_yamadron_burns(i, reason);
    }
}

static void apply_ritual(void)
{
    unsigned before = D_8009B210;
    int index = D_8009B19C;
    original_apply();
    if ((before & 0x8f) == 0x83 &&
        ((D_8009B210 & 15) == 4 || (D_8009B210 & 15) == 5) &&
        monster_slot(index)) {
        DuelCardRecord *card = &D_801A7AD8[index];
        /* Arm only the hot paths this Ritual can actually need. The stable
         * field reconciliation below retires these bits after the instance
         * leaves play, unlike the cumulative diagnostic counters. */
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data) {
            memset(&borrowed_origin[index],0,sizeof(borrowed_origin[index])); /* genuine Ritual */
            if(ritual_card_effect_enabled(card->card_id))
                ritual_ai_learn_full_effect_for_side(side_for_record(index),card->card_id);
            /* Renderer/field bookkeeping historically follows Ritual identity
             * even when its optional effect setting is off. Preserve that
             * behaviour; only the stat hot-path bits depend on the setting. */
            if(card->card_id==TRI_HORNED_ID)tri_live_hint=1;
            else if(card->card_id==GARMA_SWORD_ID)garma_live_hint=1;
            else if(card->card_id==GATE_GUARDIAN_ID)gate_live_hint=1;
            else if(card->card_id==SENGENJIN_ID && ritual_card_effect_enabled(SENGENJIN_ID))sengenjin_live_hint=1;
            else if(card->card_id==BLACK_LUSTER_SOLDIER_ID && ritual_card_effect_enabled(BLACK_LUSTER_SOLDIER_ID))bls_live_hint=1;
            else if(card->card_id==BLUE_EYES_ULTIMATE_ID && ritual_card_effect_enabled(BLUE_EYES_ULTIMATE_ID))ultimate_live_hint=1;
            else if(card->card_id==COSMO_QUEEN_ID && ritual_card_effect_enabled(COSMO_QUEEN_ID))cosmo_live_hint=1;
            else if(card->card_id==DOKURORIDER_ID && ritual_card_effect_enabled(DOKURORIDER_ID))dokurorider_live_hint=1;
            if (ritual_card_effect_enabled(card->card_id)) {
                switch (card->card_id) {
                case SHIELD_ID: stat_effect_hints |= STAT_HINT_SHIELD; break;
                case HUNGRY_BURGER_ID: stat_effect_hints |= STAT_HINT_HUNGRY; break;
                case JAVELIN_BEETLE_ID: stat_effect_hints |= STAT_HINT_JAVELIN; break;
                case CRAB_TURTLE_ID: stat_effect_hints |= STAT_HINT_CRAB; break;
                case FORTRESS_WHALE_ID: stat_effect_hints |= STAT_HINT_WHALE; break;
                case TRI_HORNED_ID: stat_effect_hints |= STAT_HINT_TRI; break;
                case SUPER_WAR_LION_ID: stat_effect_hints |= STAT_HINT_WAR_LION; break;
                case BLACK_LUSTER_SOLDIER_ID: stat_effect_hints |= STAT_HINT_BLS; break;
                case COSMO_QUEEN_ID: stat_effect_hints |= STAT_HINT_COSMO; break;
                case DOKURORIDER_ID: stat_effect_hints |= STAT_HINT_DOKU; break;
                case CHAKRA_ID: stat_effect_hints |= STAT_HINT_CHAKRA; break;
                case SKULL_GUARDIAN_ID: stat_effect_hints |= STAT_HINT_SKULL; break;
                case ZERA_ID: if(zera_marks[index].ritual && zera_marks[index].data==card->data) stat_effect_hints |= STAT_HINT_ZERA; break;
                default: break;
                }
            }
        }
        if ((card->flags&DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id==SKULL_GUARDIAN_ID && ritual_card_effect_enabled(SKULL_GUARDIAN_ID)) {
            skull_marks[index].data=card->data;skull_marks[index].ritual=1;
            skull_marks[index].last_bonus=0;skull_live_mask|=1u<<index;
            stat_effect_hints|=STAT_HINT_SKULL;
        }
        if(card->card_id==CHAKRA_ID && ritual_card_effect_enabled(CHAKRA_ID)) {
            chakra_marks[index].data=card->data;
            chakra_marks[index].ritual=1;
            chakra_marks[index].bonus=0;
            chakra_live_mask|=1u<<index;
            chakra_pending_mask|=1u<<index;
        }
        mask_summon(card);
        coin_summon(index);
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id==TRI_HORNED_ID) {
            tri_marks[index].data=card->data;tri_marks[index].ritual=1;tri_seen=1;tri_live_mask|=1u<<index;
            tri_marks[index].horns=3;tri_marks[index].battle_flags=card->flags;
            if(ritual_card_effect_enabled(TRI_HORNED_ID))tri_field_pending=1;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id==SERPENT_NIGHT_ID) {
            serpent_marks[index].data=card->data; serpent_marks[index].ritual=1; serpent_marks[index].learned_sides=0; serpent_seen=1; serpent_live_mask|=1u<<index; ++serpent_dbg_ritual; serpent_dbg_last_slot=index;
            if(host&&host->log)host->log(host,"[Ritual Card Effects] Serpent Night Dragon Ritual detectado: lado %d, slot %d",index>=15,index);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id==BLUE_EYES_ULTIMATE_ID) {
            ultimate_marks[index].data=card->data; ultimate_marks[index].ritual=1; ultimate_marks[index].battle_flags=card->flags;
            ultimate_live_mask|=1u<<index; ultimate_live_hint=ritual_card_effect_enabled(BLUE_EYES_ULTIMATE_ID);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id==COSMO_QUEEN_ID) {
            cosmo_marks[index].data=card->data; cosmo_marks[index].ritual=1;
            cosmo_marks[index].female_count=female_field_count_except(index)+1u; cosmo_live_mask|=1u<<index;
            cosmo_live_hint=ritual_card_effect_enabled(COSMO_QUEEN_ID);
            if(cosmo_live_hint) { stat_effect_hints|=STAT_HINT_COSMO; visual_queue_activation(index); visual_queue_stat(index,(int)(cosmo_marks[index].female_count*COSMO_FEMALE_BONUS)); }
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id==DOKURORIDER_ID) {
            dokurorider_marks[index].data=card->data; dokurorider_marks[index].ritual=1;
            dokurorider_marks[index].learned_sides=0; dokurorider_marks[index].battle_flags=card->flags;
            dokurorider_live_mask|=1u<<index; dokurorider_live_hint=ritual_card_effect_enabled(DOKURORIDER_ID);
            if(dokurorider_live_hint){stat_effect_hints|=STAT_HINT_DOKU;dokurorider_new_source_pending|=1u<<index;}
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == SHIELD_ID) {
            marks[index].data = card->data;
            marks[index].card_id = card->card_id;
            marks[index].ritual = 1;
            shield_live_mask |= 1u << index;
            marks[index].in_battle = 0;
            marks[index].attack_count = count_attack_monsters_except(index);
            ++summons;
            if (marks[index].attack_count) {
                if (!marks[index].def_visual_shown) { visual_queue_activation(index); marks[index].def_visual_shown = 1; }
                visual_queue_stat(index, (int)(marks[index].attack_count * BONUS));
            }
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Millennium Shield Ritual: lado %d, slot %d, atacantes=%u, bonus DEF=%u",
                          index >= 15, index, marks[index].attack_count,
                          marks[index].attack_count * BONUS);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == YAMADRON_ID) {
            yamadron_marks[index].data = card->data;
            yamadron_marks[index].card_id = card->card_id;
            yamadron_marks[index].ritual = 1;
            yamadron_live_mask |= 1u << index;
            ++yamadron_summons;
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Yamadron Ritual detectado: lado %d, slot %d", index >= 15, index);
            generate_yamadron_burns(index, "invocacao");
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == PSYCHO_PUPPET_ID) {
            PsychoMark *m = alloc_psycho_mark(card->data);
            if (m) {
                m->psycho_slot=index;
                ++psycho_summons;
                if (host && host->log)
                    host->log(host, "[Ritual Card Effects] Psycho-Puppet Ritual detectado: lado %d, slot %d", index >= 15, index);
                /* The Ritual result is detected before the Ritual effect has
                 * completely returned control to the field scene. At that
                 * instant the tribute records can still occupy monster slots,
                 * so attempting the steal here can incorrectly see a full
                 * field. Defer exactly one summon attempt to the first field
                 * tick after Ritual resolution. */
                if(!m->pending_summon_trigger){
                    unsigned mi=(unsigned)(m-psycho_marks);
                    m->pending_summon_trigger=1; psycho_pending_mask|=1u<<mi; ++psycho_pending_count;
                }
            }
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == HUNGRY_BURGER_ID) {
            hungry_marks[index].data = card->data;
            hungry_marks[index].ritual = 1;
            hungry_marks[index].battle_kills = 0;
            hungry_marks[index].pending_stacks = 0; hungry_marks[index].bite_started = 0; hungry_marks[index].bite_delay = 0;
            hungry_live_mask |= 1u << index;
            ++hungry_summons;
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Hungry Burger Ritual detectado: lado %d, slot %d", index >= 15, index);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == FIENDS_MIRROR_ID && fiends_mirror_effect_on) {
            fiend_mirror_marks[index].data = card->data;
            fiend_mirror_marks[index].ritual = 1;
            fiend_mirror_live_mask |= 1u << index;
            /* With the live fast-path, LP watching sleeps after the last Mirror
             * and any queued reflection are gone. Rebase exactly when a new
             * live window starts so damage taken while no Mirror existed can
             * never be reflected retroactively. */
            if(!fiend_mirror_live_hint && !mirror_queue_count && !mirror_reflection.active) {
                mirror_last_lp[0]=D_800E9FF0[0].life_points.unsigned_value;
                mirror_last_lp[1]=D_800E9FF0[1].life_points.unsigned_value;
                mirror_lp_ready=1;
            }
            ++fiend_mirror_summons;
            fiend_mirror_live_hint = 1;
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Fiend's Mirror Ritual detectado: lado %d, slot %d", index >= 15, index);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == JAVELIN_BEETLE_ID) {
            javelin_marks[index].data = card->data; javelin_marks[index].ritual = 1;
            javelin_marks[index].protection_used_turn = 0; javelin_marks[index].protection_visual_shown = 0; javelin_marks[index].battle_flags = card->flags;
            javelin_live_mask |= 1u << index;
            ++javelin_summons;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == GATE_GUARDIAN_ID) {
            gate_marks[index].data = card->data; gate_marks[index].ritual = 1; gate_marks[index].attacks_this_turn = 0; gate_marks[index].extra_attack_pending = 0; gate_live_mask |= 1u << index;
            ++gate_summons;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == GARMA_SWORD_ID) {
            garma_marks[index].data = card->data; garma_marks[index].ritual = 1; garma_marks[index].blades_used = 0; garma_marks[index].extra_attack_pending = 0; garma_marks[index].extra_attack_ready = 0; garma_marks[index].strike_bonus=0;garma_marks[index].final_attack_cleanup=0; garma_live_mask |= 1u << index;
            ++garma_summons;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == SUPER_WAR_LION_ID &&
            ritual_card_effect_enabled(SUPER_WAR_LION_ID)) {
            war_lion_marks[index].data=card->data; war_lion_marks[index].ritual=1;
            war_lion_marks[index].beast_count=0; war_lion_marks[index].learned_sides=0; war_lion_marks[index].battle_flags=card->flags;
            war_lion_live_mask|=1u<<index;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == SENGENJIN_ID &&
            ritual_card_effect_enabled(SENGENJIN_ID)) {
            sengenjin_marks[index].data=card->data; sengenjin_marks[index].ritual=1;
            sengenjin_marks[index].learned_sides=0; sengenjin_marks[index].battle_flags=card->flags;
            sengenjin_live_mask|=1u<<index; sengenjin_live_hint=1;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == BLACK_LUSTER_SOLDIER_ID &&
            ritual_card_effect_enabled(BLACK_LUSTER_SOLDIER_ID)) {
            bls_marks[index].data=card->data; bls_marks[index].ritual=1;
            bls_live_mask|=1u<<index; bls_live_hint=1; bls_new_source_pending|=1u<<index;
            if(gDuel_bTerrain!=SOGEN_TERRAIN) bls_field_pending=1;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == ZERA_ID &&
            ritual_card_effect_enabled(ZERA_ID)) {
            zera_marks[index].data=card->data; zera_marks[index].ritual=1;
            zera_live_mask|=1u<<index; stat_effect_hints|=STAT_HINT_ZERA;
            if(gDuel_bTerrain!=YAMI_TERRAIN) zera_field_pending=1;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == CRAB_TURTLE_ID) {
            crab_marks[index].data = card->data; crab_marks[index].ritual = 1; crab_marks[index].battle_flags = card->flags;
            crab_live_mask |= 1u << index;
            ++crab_summons;
            if (ritual_card_effect_enabled(CRAB_TURTLE_ID)) { crab_umi_pending = 1; crab_umi_sequence = 1; ritual_field_id=UMI_ID; ritual_field_source_card_id=CRAB_TURTLE_ID; }
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == FORTRESS_WHALE_ID) {
            whale_marks[index].data = card->data; whale_marks[index].ritual = 1; whale_marks[index].battle_flags = card->flags;
            whale_live_mask |= 1u << index;
            ++whale_summons;
        }
    }
}

static int limit_test_mask_armed(void *data)
{
    unsigned pending=mask_instance_mask;
    while(pending) {
        int n=take_mask_bit(&pending);
        if(mask_instances[n].data==data) return 1;
    }
    return 0;
}

#include "egotist_bridge.h"

/* Arm an implemented Ritual effect once when its monster is simply present in
 * a Monster Zone.  Every branch mirrors the already-approved Ritual-Summon
 * initialization above, but is guarded by instance identity so stable field
 * ticks never reset counters, charges or once-per-turn state. */
static void limit_test_arm_field_effects(void)
{
    int ranges[2][2]={{5,10},{20,25}};
    int r,index;
    for(r=0;r<2;++r) for(index=ranges[r][0];index<ranges[r][1];++index) {
        DuelCardRecord *card=&D_801A7AD8[index];
        if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED)||!card->data||
           !ritual_card_effect_enabled(card->card_id)) continue;
        if(!test_ignore_ritual_requirement) {
            unsigned serial=borrowed_lease(index);
            if(!serial || (card->flags&DUEL_CARD_FLAG_FACE_DOWN))continue;
            if(!borrowed_effect_armed(index)) {
                borrowed_origin[index].data=card->data;
                borrowed_origin[index].id=card->card_id;
                borrowed_origin[index].serial=serial;
            }
        }
        ritual_ai_learn_full_effect_for_side(side_for_record(index),card->card_id);
        switch(card->card_id) {
        case MASK_ID:
            if(!limit_test_mask_armed(card->data)) mask_summon(card);
            break;
        case PERFORMANCE_ID:
            if(!(performance_marks[index].ritual&&performance_marks[index].data==card->data))
                coin_summon(index);
            break;
        case CHAOS_ID:
            if(!(chaos_marks[index].ritual&&chaos_marks[index].data==card->data))
                coin_summon(index);
            break;
        case TRI_HORNED_ID:
            if(!(tri_marks[index].ritual&&tri_marks[index].data==card->data)) {
                tri_live_hint=1;stat_effect_hints|=STAT_HINT_TRI;
                tri_marks[index].data=card->data;tri_marks[index].ritual=1;tri_seen=1;
                tri_marks[index].horns=3;tri_marks[index].battle_flags=card->flags;
                tri_live_mask|=1u<<index;tri_field_pending=1;
            }
            break;
        case SERPENT_NIGHT_ID:
            if(!(serpent_marks[index].ritual&&serpent_marks[index].data==card->data)) {
                serpent_marks[index].data=card->data;serpent_marks[index].ritual=1;
                serpent_marks[index].learned_sides=0;serpent_seen=1;
                serpent_live_mask|=1u<<index;
            }
            break;
        case SHIELD_ID:
            if(!(marks[index].ritual&&marks[index].data==card->data&&marks[index].card_id==SHIELD_ID)) {
                marks[index].data=card->data;marks[index].card_id=SHIELD_ID;marks[index].ritual=1;
                marks[index].in_battle=0;marks[index].attack_count=count_attack_monsters_except(index);
                marks[index].def_visual_shown=0;marks[index].protect_visual_shown=0;
                shield_live_mask|=1u<<index;stat_effect_hints|=STAT_HINT_SHIELD;++summons;
                if(marks[index].attack_count) {
                    visual_queue_activation(index);marks[index].def_visual_shown=1;
                    visual_queue_stat(index,(int)(marks[index].attack_count*BONUS));
                }
            }
            break;
        case YAMADRON_ID:
            if(!(yamadron_marks[index].ritual&&yamadron_marks[index].data==card->data)) {
                yamadron_marks[index].data=card->data;yamadron_marks[index].card_id=YAMADRON_ID;
                yamadron_marks[index].ritual=1;yamadron_marks[index].visual_shown=0;
                yamadron_live_mask|=1u<<index;++yamadron_summons;
                generate_yamadron_burns(index,"limit-test");
            }
            break;
        case PSYCHO_PUPPET_ID: {
            PsychoMark *m=find_psycho_mark(card->data);
            if(!m) {
                m=alloc_psycho_mark(card->data);
                if(m) {
                    unsigned mi=(unsigned)(m-psycho_marks);
                    m->psycho_slot=index;++psycho_summons;
                    m->pending_summon_trigger=1;psycho_pending_mask|=1u<<mi;++psycho_pending_count;
                }
            } else m->psycho_slot=index;
            break;
        }
        case HUNGRY_BURGER_ID:
            if(!(hungry_marks[index].ritual&&hungry_marks[index].data==card->data)) {
                hungry_marks[index].data=card->data;hungry_marks[index].ritual=1;
                hungry_marks[index].battle_kills=0;hungry_marks[index].pending_stacks=0;
                hungry_marks[index].bite_started=0;hungry_marks[index].bite_delay=0;hungry_live_mask|=1u<<index;
                stat_effect_hints|=STAT_HINT_HUNGRY;++hungry_summons;
            }
            break;
        case FIENDS_MIRROR_ID:
            if(!(fiend_mirror_marks[index].ritual&&fiend_mirror_marks[index].data==card->data)) {
                if(!fiend_mirror_live_hint&&!mirror_queue_count&&!mirror_reflection.active) {
                    mirror_last_lp[0]=D_800E9FF0[0].life_points.unsigned_value;
                    mirror_last_lp[1]=D_800E9FF0[1].life_points.unsigned_value;mirror_lp_ready=1;
                }
                fiend_mirror_marks[index].data=card->data;fiend_mirror_marks[index].ritual=1;
                fiend_mirror_live_mask|=1u<<index;fiend_mirror_live_hint=1;++fiend_mirror_summons;
            }
            break;
        case JAVELIN_BEETLE_ID:
            if(!(javelin_marks[index].ritual&&javelin_marks[index].data==card->data)) {
                javelin_marks[index].data=card->data;javelin_marks[index].ritual=1;
                javelin_marks[index].protection_used_turn=0;javelin_marks[index].protection_visual_shown=0;
                javelin_marks[index].battle_flags=card->flags;javelin_live_mask|=1u<<index;
                stat_effect_hints|=STAT_HINT_JAVELIN;++javelin_summons;
            }
            break;
        case GATE_GUARDIAN_ID:
            if(!(gate_marks[index].ritual&&gate_marks[index].data==card->data)) {
                gate_marks[index].data=card->data;gate_marks[index].ritual=1;
                gate_marks[index].attacks_this_turn=0;gate_marks[index].extra_attack_pending=0;gate_live_mask|=1u<<index;
                gate_live_hint=1;++gate_summons;
            }
            break;
        case GARMA_SWORD_ID:
            if(!(garma_marks[index].ritual&&garma_marks[index].data==card->data)) {
                garma_marks[index].data=card->data;garma_marks[index].ritual=1;
                garma_marks[index].blades_used=0;garma_marks[index].extra_attack_pending=0;
                garma_marks[index].extra_attack_ready=0;garma_marks[index].strike_bonus=0;garma_marks[index].final_attack_cleanup=0;garma_live_mask|=1u<<index;
                garma_live_hint=1;++garma_summons;
            }
            break;
        case SUPER_WAR_LION_ID:
            if(!(war_lion_marks[index].ritual&&war_lion_marks[index].data==card->data)) {
                war_lion_marks[index].data=card->data;war_lion_marks[index].ritual=1;
                war_lion_marks[index].beast_count=0;war_lion_marks[index].learned_sides=0;war_lion_marks[index].battle_flags=card->flags;
                war_lion_live_mask|=1u<<index;stat_effect_hints|=STAT_HINT_WAR_LION;
            }
            break;
        case SENGENJIN_ID:
            if(!(sengenjin_marks[index].ritual&&sengenjin_marks[index].data==card->data)) {
                sengenjin_marks[index].data=card->data;sengenjin_marks[index].ritual=1;
                sengenjin_marks[index].learned_sides=0;sengenjin_marks[index].battle_flags=card->flags;sengenjin_live_mask|=1u<<index;
                sengenjin_live_hint=1;
            }
            break;
        case BLACK_LUSTER_SOLDIER_ID:
            if(!(bls_marks[index].ritual&&bls_marks[index].data==card->data)) {
                bls_marks[index].data=card->data;bls_marks[index].ritual=1;
                bls_live_mask|=1u<<index;bls_live_hint=1;bls_new_source_pending|=1u<<index;
                stat_effect_hints|=STAT_HINT_BLS;
                if(gDuel_bTerrain!=SOGEN_TERRAIN)bls_field_pending=1;
            }
            break;
        case ZERA_ID:
            if(!(zera_marks[index].ritual&&zera_marks[index].data==card->data)) {
                zera_marks[index].data=card->data;zera_marks[index].ritual=1;
                zera_live_mask|=1u<<index;stat_effect_hints|=STAT_HINT_ZERA;
                if(gDuel_bTerrain!=YAMI_TERRAIN)zera_field_pending=1;
            }
            break;
        case BLUE_EYES_ULTIMATE_ID:
            if(!(ultimate_marks[index].ritual&&ultimate_marks[index].data==card->data)) {
                ultimate_marks[index].data=card->data;ultimate_marks[index].ritual=1;ultimate_marks[index].battle_flags=card->flags;
                ultimate_live_mask|=1u<<index;ultimate_live_hint=1;
            }
            break;
        case COSMO_QUEEN_ID:
            if(!(cosmo_marks[index].ritual&&cosmo_marks[index].data==card->data)) {
                cosmo_marks[index].data=card->data;cosmo_marks[index].ritual=1;
                cosmo_marks[index].female_count=female_field_count_except(index)+1u;cosmo_live_mask|=1u<<index;cosmo_live_hint=1;
                stat_effect_hints|=STAT_HINT_COSMO;
                visual_queue_activation(index);visual_queue_stat(index,(int)(cosmo_marks[index].female_count*COSMO_FEMALE_BONUS));
            }
            break;
        case SKULL_GUARDIAN_ID:
            if(!skull_identity(index)) {
                skull_marks[index].data=card->data;skull_marks[index].ritual=1;
                skull_marks[index].last_bonus=0;skull_live_mask|=1u<<index;
                stat_effect_hints|=STAT_HINT_SKULL;
            }
            break;
        case CHAKRA_ID:
            if(!(chakra_marks[index].ritual&&chakra_marks[index].data==card->data)) {
                chakra_marks[index].data=card->data;chakra_marks[index].ritual=1;chakra_marks[index].bonus=0;
                chakra_live_mask|=1u<<index;chakra_pending_mask|=1u<<index;
                stat_effect_hints|=STAT_HINT_CHAKRA;
            }
            break;
        case DOKURORIDER_ID:
            if(!(dokurorider_marks[index].ritual&&dokurorider_marks[index].data==card->data)) {
                dokurorider_marks[index].data=card->data;dokurorider_marks[index].ritual=1;
                dokurorider_marks[index].learned_sides=0;dokurorider_marks[index].battle_flags=card->flags;
                dokurorider_live_mask|=1u<<index;dokurorider_live_hint=1;stat_effect_hints|=STAT_HINT_DOKU;
                dokurorider_new_source_pending|=1u<<index;
            }
            break;
        case CRAB_TURTLE_ID:
            if(!(crab_marks[index].ritual&&crab_marks[index].data==card->data)) {
                crab_marks[index].data=card->data;crab_marks[index].ritual=1;
                crab_marks[index].battle_flags=card->flags;crab_live_mask|=1u<<index;
                stat_effect_hints|=STAT_HINT_CRAB;++crab_summons;
                crab_umi_pending=1;crab_umi_sequence=1;ritual_field_id=UMI_ID;
                ritual_field_source_card_id=CRAB_TURTLE_ID;
            }
            break;
        case FORTRESS_WHALE_ID:
            if(!(whale_marks[index].ritual&&whale_marks[index].data==card->data)) {
                whale_marks[index].data=card->data;whale_marks[index].ritual=1;
                whale_marks[index].battle_flags=card->flags;whale_live_mask|=1u<<index;
                stat_effect_hints|=STAT_HINT_WHALE;++whale_summons;
            }
            break;
        default: break;
        }
    }
}

static int card_type_id(const DuelCardRecord *card)
{
    if (!card || card->card_id < 1) return -1;
    return (int)(((unsigned)gDuel_adwCardStats[card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK);
}

static unsigned allied_insect_count(int side)
{
    int i, start = side ? 20 : 5; unsigned n = 0;
    for (i = start; i < start + 5; ++i)
        if ((D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) && card_type_id(&D_801A7AD8[i]) == CARD_TYPE_INSECT) ++n;
    return n;
}

static unsigned side_monster_mask(int side)
{
    return side ? 0x01F00000u : 0x000003E0u;
}

static int sea_family_type(int t) { return t == CARD_TYPE_FISH || t == CARD_TYPE_AQUA || t == CARD_TYPE_SEA_SERPENT; }

static int war_lion_identity(int i)
{
    DuelCardRecord *c;
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && monster_slot(i) &&
        war_lion_marks[i].ritual && (c=&D_801A7AD8[i])->card_id==SUPER_WAR_LION_ID &&
        c->data==war_lion_marks[i].data;
}
static int war_lion_live(int i)
{ return war_lion_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED); }
static int sengenjin_identity(int i)
{
    DuelCardRecord *c;
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && monster_slot(i) &&
        sengenjin_marks[i].ritual && (c=&D_801A7AD8[i])->card_id==SENGENJIN_ID &&
        c->data==sengenjin_marks[i].data;
}
static int sengenjin_live(int i)
{ return sengenjin_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED); }
static int bls_identity(int i)
{
    DuelCardRecord *c;
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && monster_slot(i) &&
        bls_marks[i].ritual && (c=&D_801A7AD8[i])->card_id==BLACK_LUSTER_SOLDIER_ID &&
        c->data==bls_marks[i].data;
}
static int bls_live(int i)
{ return bls_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED); }

static int ultimate_identity(int i)
{
    DuelCardRecord *c;
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && monster_slot(i) &&
        ultimate_marks[i].ritual && (c=&D_801A7AD8[i])->card_id==BLUE_EYES_ULTIMATE_ID &&
        c->data==ultimate_marks[i].data;
}
static int ultimate_live(int i)
{ return ultimate_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED); }
static int cosmo_identity(int i)
{
    DuelCardRecord *c;
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && monster_slot(i) &&
        cosmo_marks[i].ritual && (c=&D_801A7AD8[i])->card_id==COSMO_QUEEN_ID &&
        c->data==cosmo_marks[i].data;
}
static int cosmo_live(int i)
{ return cosmo_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED); }
static int dokurorider_identity(int i)
{
    DuelCardRecord *c;
    return i>=0 && i<DUEL_CARD_RECORD_COUNT && monster_slot(i) &&
        dokurorider_marks[i].ritual && (c=&D_801A7AD8[i])->card_id==DOKURORIDER_ID &&
        c->data==dokurorider_marks[i].data;
}
static int dokurorider_live(int i)
{ return dokurorider_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED); }
static int ritual_effectively_present(int i)
{
    if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !monster_slot(i))return 0;
    if(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)return 1;
    return (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 && battle_participant(i);
}
static int ultimate_effect_active(int i)
{
    return ritual_card_effect_enabled(BLUE_EYES_ULTIMATE_ID) &&
        ultimate_identity(i) && ritual_effectively_present(i);
}
/* One central truth for every opposing card-effect interaction. Field cards
 * and Guardian Stars are duel context, not direct hostile effects, and are
 * therefore deliberately outside this predicate. */
static int RitualEffects_IsCardEffectImmune(int target_index,int source_side)
{
    if(target_index<0 || target_index>=DUEL_CARD_RECORD_COUNT ||
       source_side<0 || source_side>=DUEL_SIDE_COUNT)return 0;
    return ultimate_effect_active(target_index) && side_for_record(target_index)!=source_side;
}
static int ultimate_field_card(int id)
{
    /* Retail Field cards: Forest, Wasteland, Mountain, Sogen, Umi, Yami.
     * They are duel context by design and never count as hostile effects for
     * Ultimate's immunity. */
    return id>=330 && id<=335;
}
static int ritual_ai_side_is_cpu(int side)
{
    return side>=0 && side<DUEL_SIDE_COUNT && D_8009B360[side]>=0;
}
static int ritual_ai_knows_effect(int side,int card_id)
{
    unsigned id=(unsigned)card_id;
    if(!ritual_ai_side_is_cpu(side) || id>=RITUAL_AI_KNOWLEDGE_CARD_LIMIT)return 0;
    return (ritual_ai_effect_known[side][id>>5]&(1u<<(id&31)))!=0;
}
static void ritual_ai_learn_full_effect_for_side(int side,int card_id)
{
    unsigned id=(unsigned)card_id;
    u32 bit,*word;
    if(!ritual_ai_side_is_cpu(side) || id>=RITUAL_AI_KNOWLEDGE_CARD_LIMIT)return;
    bit=1u<<(id&31); word=&ritual_ai_effect_known[side][id>>5];
    if(!(*word&bit) && host&&host->log)
        host->log(host,"[Ritual Card Effects] CPU lado %d aprendeu o efeito completo da carta %d",side,card_id);
    *word|=bit;
}
static int ultimate_ai_side_is_cpu(int side)
{ return ritual_ai_side_is_cpu(side); }
static int ultimate_ai_knows_magic(int side)
{ return ritual_ai_knows_effect(side,BLUE_EYES_ULTIMATE_ID); }
static int ultimate_ai_knows_trap(int side)
{ return ritual_ai_knows_effect(side,BLUE_EYES_ULTIMATE_ID); }
static void ultimate_ai_learn_category(int source_side,int trap)
{
    (void)trap;
    /* A first real failed interaction teaches the COMPLETE Ultimate effect,
     * but only to the CPU side that actually suffered that setback. */
    ritual_ai_learn_full_effect_for_side(source_side,BLUE_EYES_ULTIMATE_ID);
}
static int ultimate_effect_group(int card_id)
{
    int id,index;
    if(!Cards_Valid(card_id))return -1;
    id=Cards_EffectId(card_id);
    if(id>=DUEL_EFFECT_FIRST_BLOCK_CARD_ID &&
       id<DUEL_EFFECT_FIRST_BLOCK_CARD_ID+DUEL_EFFECT_CARD_BLOCK_SIZE)
        index=id-DUEL_EFFECT_FIRST_BLOCK_CARD_ID;
    else if(id>=DUEL_EFFECT_SECOND_BLOCK_CARD_ID &&
            id<DUEL_EFFECT_SECOND_BLOCK_CARD_ID+DUEL_EFFECT_CARD_BLOCK_SIZE)
        index=id-(DUEL_EFFECT_SECOND_BLOCK_CARD_ID-DUEL_EFFECT_CARD_BLOCK_SIZE);
    else if(id==DUEL_DARK_MAGIC_RITUAL_CARD_ID)
        index=DUEL_DARK_MAGIC_RITUAL_EFFECT_INDEX;
    else return -1;
    if(index<0 || index>=DUEL_EFFECT_ID_COUNT)return -1;
    return gDuelEffect_abGroupByEffectId[index];
}
static int ultimate_nonimmune_enemy_monster(int acting_side,int wanted_type,
                                             int min_attack,int require_defense,
                                             int require_facedown)
{
    int i;
    for(i=5;i<10;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED) || side_for_record(i)==acting_side ||
           ultimate_effect_active(i))continue;
        if(require_defense && !(c->flags&DUEL_CARD_FLAG_DEFENSE_POSITION))continue;
        if(require_facedown && !(c->flags&DUEL_CARD_FLAG_FACE_DOWN))continue;
        if(wanted_type>=0 && card_type_id(c)!=wanted_type)continue;
        if(min_attack>=0 && ((int)((unsigned)Duel_CalcCardStats(c)&0xffff))<min_attack)continue;
        return 1;
    }
    for(i=20;i<25;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED) || side_for_record(i)==acting_side ||
           ultimate_effect_active(i))continue;
        if(require_defense && !(c->flags&DUEL_CARD_FLAG_DEFENSE_POSITION))continue;
        if(require_facedown && !(c->flags&DUEL_CARD_FLAG_FACE_DOWN))continue;
        if(wanted_type>=0 && card_type_id(c)!=wanted_type)continue;
        if(min_attack>=0 && ((int)((unsigned)Duel_CalcCardStats(c)&0xffff))<min_attack)continue;
        return 1;
    }
    return 0;
}
/* After Magic immunity has actually been discovered, suppress only a Magic
 * whose hostile work would have no legal non-Ultimate victim.  This lets
 * Raigeki/Crush/etc. remain useful against other monsters instead of teaching
 * the CPU the incorrect rule "never play Magic again". */
static int ultimate_ai_magic_would_only_fail(int card_id,int acting_side)
{
    int group,id,type=-1;
    if(!ultimate_ai_knows_magic(acting_side) || !Cards_Valid(card_id) ||
       Cards_Type(card_id)!=CARD_TYPE_MAGIC)return 0;
    id=Cards_EffectId(card_id);
    group=ultimate_effect_group(card_id);
    switch(group) {
    case DUEL_EFFECT_GROUP_BOARD_DESTRUCTION:
        if(id==DUEL_DRAGON_CAPTURE_JAR_CARD_ID)
            return !ultimate_nonimmune_enemy_monster(acting_side,CARD_TYPE_DRAGON,-1,0,0);
        /* Dark Hole still has value when at least one opposing non-Ultimate
         * monster would actually be removed. */
        return !ultimate_nonimmune_enemy_monster(acting_side,-1,-1,0,0);
    case DUEL_EFFECT_GROUP_MONSTER_REMOVAL:
        switch(id) {
        case DUEL_WARRIOR_ELIMINATION_CARD_ID:type=CARD_TYPE_WARRIOR;break;
        case DUEL_ETERNAL_REST_CARD_ID:type=CARD_TYPE_ZOMBIE;break;
        case DUEL_STAIN_STORM_CARD_ID:type=CARD_TYPE_MACHINE;break;
        case DUEL_ERADICATING_AEROSOL_CARD_ID:type=CARD_TYPE_INSECT;break;
        case DUEL_BREATH_OF_LIGHT_CARD_ID:type=CARD_TYPE_ROCK;break;
        case DUEL_ETERNAL_DRAUGHT_CARD_ID:type=CARD_TYPE_FISH;break;
        case DUEL_CRUSH_CARD_ID:
            return !ultimate_nonimmune_enemy_monster(
                acting_side,-1,DUEL_CRUSH_CARD_ATTACK_THRESHOLD,0,0);
        default:return 0;
        }
        return !ultimate_nonimmune_enemy_monster(acting_side,type,-1,0,0);
    case DUEL_EFFECT_GROUP_STOP_DEFENSE:
        return !ultimate_nonimmune_enemy_monster(acting_side,-1,-1,1,0);
    case DUEL_EFFECT_GROUP_RAIGEKI:
    case DUEL_EFFECT_GROUP_STAT_PENALTY:
    case DUEL_EFFECT_GROUP_SWORDS:
        return !ultimate_nonimmune_enemy_monster(acting_side,-1,-1,0,0);
    case DUEL_EFFECT_GROUP_DARK_PIERCING_LIGHT:
        return !ultimate_nonimmune_enemy_monster(acting_side,-1,-1,0,1);
    default:
        /* Terrain, LP, burn, Cursebreaker, Rituals, equips and Duster do not
         * become invalid just because Ultimate learned Magic immunity. */
        return 0;
    }
}
/* Test whether this *activated* Magic would try to affect this specific
 * opposing Ultimate at all. Do not grant knowledge for terrain, LP recovery,
 * equips, Rituals, or type/position-restricted effects that could not touch
 * the Ultimate in its actual pre-resolution state. */
static int ultimate_ai_magic_targets_ultimate(int card_id,int target)
{
    int group,id,type=-1;
    DuelCardRecord *c=&D_801A7AD8[target];
    if(!ultimate_effect_active(target) || !Cards_Valid(card_id) ||
       Cards_Type(card_id)!=CARD_TYPE_MAGIC)return 0;
    id=Cards_EffectId(card_id);
    group=ultimate_effect_group(card_id);
    switch(group) {
    case DUEL_EFFECT_GROUP_BOARD_DESTRUCTION:
        return id!=DUEL_DRAGON_CAPTURE_JAR_CARD_ID ||
            card_type_id(c)==CARD_TYPE_DRAGON;
    case DUEL_EFFECT_GROUP_MONSTER_REMOVAL:
        switch(id) {
        case DUEL_WARRIOR_ELIMINATION_CARD_ID:type=CARD_TYPE_WARRIOR;break;
        case DUEL_ETERNAL_REST_CARD_ID:type=CARD_TYPE_ZOMBIE;break;
        case DUEL_STAIN_STORM_CARD_ID:type=CARD_TYPE_MACHINE;break;
        case DUEL_ERADICATING_AEROSOL_CARD_ID:type=CARD_TYPE_INSECT;break;
        case DUEL_BREATH_OF_LIGHT_CARD_ID:type=CARD_TYPE_ROCK;break;
        case DUEL_ETERNAL_DRAUGHT_CARD_ID:type=CARD_TYPE_FISH;break;
        case DUEL_CRUSH_CARD_ID:
            return ((unsigned)Duel_CalcCardStats(c)&0xffffu)>=
                DUEL_CRUSH_CARD_ATTACK_THRESHOLD;
        default:return 0;
        }
        return card_type_id(c)==type;
    case DUEL_EFFECT_GROUP_STOP_DEFENSE:
        return (c->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)!=0;
    case DUEL_EFFECT_GROUP_DARK_PIERCING_LIGHT:
        return (c->flags&DUEL_CARD_FLAG_FACE_DOWN)!=0;
    case DUEL_EFFECT_GROUP_RAIGEKI:
    case DUEL_EFFECT_GROUP_STAT_PENALTY:
    case DUEL_EFFECT_GROUP_SWORDS:
        return 1;
    default:return 0;
    }
}
static void ultimate_ai_magic_observe_begin(void)
{
    int side=D_8009B1D5,id=gDuel_wEffectCardID;
    unsigned pending;
    if(ultimate_ai_magic_attempt.active || !ultimate_live_hint ||
       !ritual_ai_side_is_cpu(side) || ultimate_ai_knows_magic(side) ||
       !Cards_Valid(id) || Cards_Type(id)!=CARD_TYPE_MAGIC)return;
    pending=ultimate_live_mask & side_monster_mask(side^1);
    while(pending) {
        int i=take_mask_bit(&pending);
        if(ultimate_ai_magic_targets_ultimate(id,i)) {
            ultimate_ai_magic_attempt.target_mask|=1u<<i;
            ultimate_ai_magic_attempt.target_data[i]=D_801A7AD8[i].data;
        }
    }
    if(ultimate_ai_magic_attempt.target_mask) {
        ultimate_ai_magic_attempt.active=1;
        ultimate_ai_magic_attempt.source_side=side;
        ultimate_ai_magic_attempt.card_id=id;
    }
}
static void ultimate_ai_magic_observe_finish(void)
{
    unsigned pending;
    int side=ultimate_ai_magic_attempt.source_side;
    if(!ultimate_ai_magic_attempt.active)return;
    pending=ultimate_ai_magic_attempt.target_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        if(ultimate_effect_active(i) &&
           D_801A7AD8[i].data==ultimate_ai_magic_attempt.target_data[i]) {
            /* A Magic really resolved against this opponent's Ultimate, yet
             * that very instance remained immune. Learn only now, never
             * during candidate planning or merely on sight of the card. */
            ritual_ai_learn_full_effect_for_side(side,BLUE_EYES_ULTIMATE_ID);
            if(host&&host->log)
                host->log(host,"[Ritual Card Effects] CPU aprendeu imunidade Magic do Ultimate apos %d falhar no slot %d",
                          ultimate_ai_magic_attempt.card_id,i);
            break;
        }
    }
    memset(&ultimate_ai_magic_attempt,0,sizeof(ultimate_ai_magic_attempt));
}
static void ultimate_card_effect_begin(void)
{
    unsigned pending;
    int source=D_8009B1D5;
    /* Dark Hole is pure destruction and already has a dedicated lifecycle
     * guard below.  Do not route it through the generic Magic/Trap snapshot:
     * Black Chaos can launch Dark Hole while its own automatic turn queue is
     * active, and keeping an Ultimate snapshot alive across request 0x11
     * couples two independent survivor systems.  That combination is the
     * only runtime case observed to stall.  Dark Hole immunity is therefore
     * handled directly by board_visual_immune_mask() + remove/deactivate. */
    if(gDuel_wEffectCardID==DARK_HOLE_ID)return;
    if(ultimate_card_effect.active || !ultimate_live_hint ||
       !ritual_card_effect_enabled(BLUE_EYES_ULTIMATE_ID) ||
       !effect_is_magic_or_trap() || ultimate_field_card(gDuel_wEffectCardID) ||
       source<0 || source>=DUEL_SIDE_COUNT)return;
    memset(&ultimate_card_effect,0,sizeof(ultimate_card_effect));
    ultimate_card_effect.active=1;
    ultimate_card_effect.effect_id=gDuel_wEffectCardID;
    ultimate_card_effect.source_side=source;
    pending=ultimate_live_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        if(ultimate_live(i) &&
           (gDuel_wEffectCardID==DARK_HOLE_ID ||
            RitualEffects_IsCardEffectImmune(i,source))) {
            /* Black/Dark Hole is the explicit global-destruction exception:
             * Ultimate survives it even when its own side activated the card.
             * Other Magic/Trap immunity remains hostile-effect only. */
            ultimate_card_effect.mask|=1u<<i;
            ultimate_card_effect.saved[i]=D_801A7AD8[i];
        }
    }
    if(!ultimate_card_effect.mask)memset(&ultimate_card_effect,0,sizeof(ultimate_card_effect));
}
static void ultimate_restore_snapshot_record(int i,DuelCardRecord *old,int preserve_used)
{
    DuelCardRecord *c=&D_801A7AD8[i];
    int rebuild_visual=c->data!=old->data || c->card_id!=old->card_id;
    void *object=c->object;
    u16 used=preserve_used?(c->flags&DUEL_CARD_FLAG_USED_THIS_TURN):0;
    /* A transformation may already have rebuilt the field object for the new
     * card.  Restoring only card_id/data would then leave the wrong artwork on
     * screen, so identity changes rebuild the native object as Ultimate. */
    if(rebuild_visual && object) { DisplayObject_ReleaseIfPresent(object); object=0; }
    c->data=old->data;c->card_id=old->card_id;c->attack=old->attack;c->defense=old->defense;
    c->stat_modifier=old->stat_modifier;c->terrain_modifier=old->terrain_modifier;
    c->table_index=old->table_index;c->flags=(old->flags&~DUEL_CARD_FLAG_USED_THIS_TURN)|
        used|(preserve_used?0:(old->flags&DUEL_CARD_FLAG_USED_THIS_TURN))|DUEL_CARD_FLAG_OCCUPIED;
    if(!object) {
        object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
        if(object)((DuelCardDisplayObject*)object)->card_index=(u8)i;
    }
    c->object=object;
    if(object)Duel_ApplyCardObjectFlags((DuelCardDisplayObject*)object);
}
static void ultimate_rebuild_field_visual(int i)
{
    DuelCardRecord *c;
    DuelCardDisplayObject *object;
    if(i<0 || i>=DUEL_CARD_RECORD_COUNT)return;
    c=&D_801A7AD8[i];
    if(c->object)DisplayObject_ReleaseIfPresent(c->object);
    object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
    c->object=object;
    if(object) {
        object->card_index=(u8)i;
        Duel_ApplyCardObjectFlags(object);
    }
}

static void ultimate_card_effect_restore(int finished)
{
    unsigned pending;
    if(!ultimate_card_effect.active)return;
    pending=ultimate_card_effect.mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        DuelCardRecord *c=&D_801A7AD8[i],*old=&ultimate_card_effect.saved[i];
        int changed=c->data!=old->data || c->card_id!=old->card_id ||
            c->attack!=old->attack || c->defense!=old->defense ||
            c->stat_modifier!=old->stat_modifier || c->terrain_modifier!=old->terrain_modifier ||
            c->flags!=old->flags || !(c->flags&DUEL_CARD_FLAG_OCCUPIED);
        if(changed) {
            ultimate_restore_snapshot_record(i,old,0);
            ultimate_card_effect.touched|=1u<<i;
        }
    }
    if(finished) {
        /* Discovery happens only after an actual failed hostile interaction,
         * never because the planner inspected Ultimate. */
        if(ultimate_card_effect.touched &&
           ultimate_card_effect.source_side>=0 &&
           ultimate_card_effect.source_side<DUEL_SIDE_COUNT) {
            unsigned hostile=ultimate_card_effect.touched;
            int failed=0;
            while(hostile) {
                int hi=take_mask_bit(&hostile);
                if(side_for_record(hi)!=ultimate_card_effect.source_side) { failed=1; break; }
            }
            if(failed && Cards_Valid(ultimate_card_effect.effect_id)) {
                int kind=Cards_Type(ultimate_card_effect.effect_id);
                if(kind==CARD_TYPE_TRAP)ultimate_ai_learn_category(ultimate_card_effect.source_side,1);
                else if(kind==CARD_TYPE_MAGIC)ultimate_ai_learn_category(ultimate_card_effect.source_side,0);
            }
        }
        pending=ultimate_card_effect.touched;
        while(pending) {
            int i=take_mask_bit(&pending);
            if(ultimate_effect_active(i)) {
                /* Request 0x11 (Dark Hole) already hides immune records from
                 * the native destruction presentation through
                 * board_visual_immune_mask(), so its field object was never
                 * consumed. Releasing/rebuilding that still-live object again
                 * after the request can invalidate presentation state when
                 * several Ritual monsters are removed in the same sweep.
                 * Other removal presentations still need the old resync. */
                if(ultimate_card_effect.effect_id!=DARK_HOLE_ID)
                    ultimate_rebuild_field_visual(i);
                visual_queue_activation(i);
            }
        }
        memset(&ultimate_card_effect,0,sizeof(ultimate_card_effect));
    }
}
static unsigned female_field_count_except(int excluded)
{
    int i; unsigned n=0;
    for(i=5;i<10;++i) if(beacon_is_other_record(i,excluded) && ritual_effectively_present(i) &&
        cosmo_is_female_monster(D_801A7AD8[i].card_id))++n;
    for(i=20;i<25;++i) if(beacon_is_other_record(i,excluded) && ritual_effectively_present(i) &&
        cosmo_is_female_monster(D_801A7AD8[i].card_id))++n;
    return n;
}
static unsigned female_field_count(void)
{
    return female_field_count_except(-1);
}
static unsigned beacon_source_count(unsigned mask,int side,int target,int kind)
{
    unsigned pending,n=0;
    if(side<0 || side>=DUEL_SIDE_COUNT)return 0;
    pending=mask & side_monster_mask(side);
    while(pending) {
        int i=take_mask_bit(&pending),eligible=0;
        if(!beacon_is_other_record(i,target))continue;
        switch(kind) {
        case BEACON_JAVELIN: eligible=javelin_live(i); break;
        case BEACON_BLS: eligible=bls_live(i); break;
        case BEACON_DOKU: eligible=dokurorider_identity(i) && ritual_effectively_present(i); break;
        case BEACON_CRAB:
            eligible=crab_marks[i].ritual &&
                (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
                D_801A7AD8[i].card_id==CRAB_TURTLE_ID &&
                D_801A7AD8[i].data==crab_marks[i].data;
            break;
        }
        n+=(unsigned)!!eligible;
    }
    return n;
}

/* The retail duel keeps the five hand records immediately before each side's
 * five monster records. Only visit these ten slots when a live War-Lion needs
 * its dynamic Beast count. */
/* Count only cards that are logically still in the hand plus occupied allied
 * Monster Zones.  The five physical hand records can remain OCCUPIED briefly
 * after a card has already been consumed by placement/Fusion/Ritual; the
 * side-state hand[] entries are the authoritative logical membership.  Keep
 * this helper generic so every current/future hand-based aura shares the same
 * rule instead of reading stale physical records. */
/* Count allied Fish (or Beast for War-Lion) by *logical* hand and
 * occupied monster slots, separately.  The first half ignores physical hand
 * records whose deck index was already consumed by an action. */
static void beacon_other_allied_type_counts(int side, int type, int source,
                                           unsigned *hand_count, unsigned *field_count)
{
    unsigned hand=0,field=0;
    int slot,i,first;
    if(side>=0 && side<DUEL_SIDE_COUNT && monster_slot(source) &&
       side_for_record(source)==side) {
        for(slot=0;slot<HAND_SIZE;++slot) {
            if(D_800E9FF0[side].hand[slot]<0) continue;
            i=D_800907CC[side*HAND_SIZE+slot];
            if(i>=0 && i<DUEL_CARD_RECORD_COUNT && i!=source &&
               (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
               card_type_id(&D_801A7AD8[i])==type) ++hand;
        }
        first=side?20:5;
        for(i=first;i<first+5;++i)
            if(i!=source && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
               card_type_id(&D_801A7AD8[i])==type) ++field;
    }
    if(hand_count)*hand_count=hand;
    if(field_count)*field_count=field;
}
static unsigned beacon_other_allied_type_hand_field(int side,int type,int source)
{
    unsigned hand,field;
    beacon_other_allied_type_counts(side,type,source,&hand,&field);
    return hand+field;
}

static unsigned aura_sources_for_preview(unsigned live_mask,int side,int kind)
{
    unsigned pending,n=0;
    if(kind==0)return beacon_source_count(live_mask,side,-1,BEACON_BLS);
    if(kind==2)return beacon_source_count(live_mask,side,-1,BEACON_DOKU);
    pending=live_mask & side_monster_mask(side);
    while(pending) {
        int i=take_mask_bit(&pending);
        switch(kind) {
        case 1: if(tri_live(i))++n; break;
        }
    }
    return n;
}

/* War-Lion attack interception is implemented at the same committed-target
 * staging point used by Serpent Fear (serpent_fear.h).  No battle survivor or
 * destruction-substitution state is required. */
/* Crab's underwater protection is a property of Crab itself, not an
 * offensive effect applied to the attacking monster. Even an Ultimate whose
 * immunity negates hostile effects cannot bypass this defender protection. */
static int crab_battle_protected(int crab,int opponent)
{
    int type;
    if(!ritual_card_effect_enabled(CRAB_TURTLE_ID) ||
       gDuel_bTerrain!=UMI_TERRAIN || !monster_slot(crab) ||
       !monster_slot(opponent) || side_for_record(crab)==side_for_record(opponent) ||
       !crab_marks[crab].ritual ||
       D_801A7AD8[crab].card_id!=CRAB_TURTLE_ID ||
       D_801A7AD8[crab].data!=crab_marks[crab].data) return 0;
    type=card_type_id(&D_801A7AD8[opponent]);
    return type!=CARD_TYPE_AQUA && type!=CARD_TYPE_FISH &&
           type!=CARD_TYPE_SEA_SERPENT && type!=CARD_TYPE_THUNDER;
}

static int side_has_other_crab(int side,int target) {
    return beacon_source_count(crab_live_mask,side,target,BEACON_CRAB)!=0;
}
static unsigned valid_whale_sources_for_side(int side) {
    unsigned pending, valid=0;
    if (!ritual_card_effect_enabled(FORTRESS_WHALE_ID)) return 0;
    pending=whale_live_mask & side_monster_mask(side);
    while(pending) {
        int i=take_mask_bit(&pending);
        if(whale_marks[i].ritual && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           D_801A7AD8[i].card_id==FORTRESS_WHALE_ID && D_801A7AD8[i].data==whale_marks[i].data)
            valid |= 1u<<i;
    }
    return valid;
}
static int side_has_whale(int side) {
    return valid_whale_sources_for_side(side) != 0;
}
/* Fortress Whale projects its Umi protection to allied sea-family monsters,
 * including the ritualized Fortress Whale source itself. Normal ready-made
 * copies benefit only when an actual ritualized Whale is present. */
static int whale_protects_target(int target) {
    unsigned sources;
    if (target < 0 || !monster_slot(target)) return 0;
    sources=valid_whale_sources_for_side(side_for_record(target));
    return sources != 0;
}
static int effect_is_magic_or_trap(void);
/* Fortress Whale: each other Fish in the logical hand gives +500 DEF,
 * while each other Fish in an occupied allied monster slot gives +1000 DEF.
 * These are deliberately separate multipliers; never count the Deck. */
static unsigned whale_fish_defense_bonus(int side,int source)
{
    unsigned hand=0,field=0;
    beacon_other_allied_type_counts(side,CARD_TYPE_FISH,source,&hand,&field);
    return hand*FORTRESS_FISH_HAND_DEF + field*FORTRESS_FISH_FIELD_DEF;
}
static unsigned whale_snapshot_protection(void) {
    unsigned mask=0; int side,i;
    if(!(stat_effect_hints & STAT_HINT_WHALE) || gDuel_bTerrain!=UMI_TERRAIN || !effect_is_magic_or_trap()) return 0;
    for(side=0;side<DUEL_SIDE_COUNT;++side) {
        int start;
        unsigned sources=valid_whale_sources_for_side(side);
        /* Resolve the Ritual Whale sources once for this side. Every target
         * in the same snapshot sees the same stable pre-effect board. */
        if(!sources) continue;
        start=side?20:5;
        for(i=start;i<start+5;++i) {
            DuelCardRecord *c=&D_801A7AD8[i];
            if((c->flags&DUEL_CARD_FLAG_OCCUPIED) && sea_family_type(card_type_id(c)) &&
               sources)
                mask |= 1u<<i;
        }
    }
    return mask;
}
static int effect_is_magic_or_trap(void) {
    int id=gDuel_wEffectCardID, t; if(id<1) return 0; t=(int)(((unsigned)gDuel_adwCardStats[id-1]>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK); return t==CARD_TYPE_MAGIC || t==CARD_TYPE_TRAP;
}
static int effect_is_trap(void) {
    int id=gDuel_wEffectCardID, t;
    if(id<1)return 0;
    t=(int)(((unsigned)gDuel_adwCardStats[id-1]>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK);
    return t==CARD_TYPE_TRAP;
}
static int attack_trap_resolution_active(void)
{
    return (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
        D_8009B22A>=DUEL_ATTACK_TRAP_FIRST_CARD_ID && D_8009B22A<=DUEL_FAKE_TRAP_CARD_ID;
}
static int ultimate_attack_trap_immune(int i)
{
    DisplayObject *attacker;
    if(!attack_trap_resolution_active() || !ultimate_effect_active(i))return 0;
    attacker=D_800E9EF0[0];
    return attacker && (int)attacker->field_6A==i;
}
static void sengenjin_trap_begin(void)
{
    unsigned pending;
    if(sengenjin_trap.active || !sengenjin_live_hint || !effect_is_trap() ||
       !ritual_card_effect_enabled(SENGENJIN_ID))return;
    memset(&sengenjin_trap,0,sizeof(sengenjin_trap));
    sengenjin_trap.active=1;
    sengenjin_trap.effect_id=gDuel_wEffectCardID;
    pending=sengenjin_live_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        if(sengenjin_live(i)) {
            sengenjin_trap.mask|=1u<<i;
            sengenjin_trap.saved[i]=D_801A7AD8[i];
        }
    }
    if(!sengenjin_trap.mask)memset(&sengenjin_trap,0,sizeof(sengenjin_trap));
}
static void sengenjin_trap_restore(int finished)
{
    unsigned pending;
    if(!sengenjin_trap.active)return;
    pending=sengenjin_trap.mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        DuelCardRecord *c=&D_801A7AD8[i], *old=&sengenjin_trap.saved[i];
        int changed=0;
        if(c->data!=old->data || c->card_id!=old->card_id || c->attack!=old->attack ||
           c->defense!=old->defense || c->stat_modifier!=old->stat_modifier ||
           c->terrain_modifier!=old->terrain_modifier ||
           ((c->flags^old->flags)&~DUEL_CARD_FLAG_USED_THIS_TURN))
            changed=1;
        if(changed || !(c->flags&DUEL_CARD_FLAG_OCCUPIED)) {
            void *object=c->object;
            u16 used=c->flags&DUEL_CARD_FLAG_USED_THIS_TURN;
            c->data=old->data; c->card_id=old->card_id; c->attack=old->attack; c->defense=old->defense;
            c->stat_modifier=old->stat_modifier; c->terrain_modifier=old->terrain_modifier;
            c->table_index=old->table_index; c->flags=old->flags|used|DUEL_CARD_FLAG_OCCUPIED;
            if(!object) {
                object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
                if(object)((DuelCardDisplayObject*)object)->card_index=(u8)i;
            }
            c->object=object;
            /* A Trap may already have rotated/flipped the existing display
             * object before we restore the record snapshot.  Re-derive the
             * object state from the restored flags so logical immunity and
             * the visible card cannot diverge. */
            if(object)Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)object);
            sengenjin_trap.touched|=1u<<i;
        }
    }
    if(finished) {
        pending=sengenjin_trap.touched;
        while(pending) {
            int i=take_mask_bit(&pending);
            if(sengenjin_identity(i) && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED))
                visual_queue_activation(i);
        }
        memset(&sengenjin_trap,0,sizeof(sengenjin_trap));
    }
}

/* Recompute the small set of hot-path gates only on a stable field. This is
 * deliberately separate from the cumulative debug counters: once the last
 * relevant Ritual leaves play, CalcCardStats and renderer hooks return to
 * their near-vanilla fast paths for the rest of the duel. */
static void refresh_hot_hints(void)
{
    unsigned old_stats=stat_effect_hints, stats=0, pending;
    int i;
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 5) return;

    /* Shield now follows the same live-mask framework as the newer effects.
     * Stable field reconciliation touches only Ritual Shield instances instead
     * of rescanning all ten Monster Zones every FieldActions tick. */
    if(old_stats&STAT_HINT_SHIELD) {
        pending=shield_live_mask;
        while(pending) {
            i=take_mask_bit(&pending); { DuelCardRecord *c=&D_801A7AD8[i];
                if(marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
                   marks[i].data==c->data && marks[i].card_id==c->card_id) stats|=STAT_HINT_SHIELD;
                else shield_live_mask&=~(1u<<i);
            }
        }
    }

    /* The Ritual instance is authoritative; an optimization hint is not.
     * Field reconciliation can run after the native battle controller has
     * temporarily cleared OCCUPIED.  Keep a staged combatant by its original
     * record identity, for either side, until native state 11 rebuilds it. */
    pending=hungry_live_mask;
    while(pending) {
        DuelCardRecord *c;
        int staged;
        i=take_mask_bit(&pending);
        c=&D_801A7AD8[i];
        staged=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 && battle_participant(i);
        if(hungry_marks[i].ritual && hungry_marks[i].data==c->data &&
           c->card_id==HUNGRY_BURGER_ID &&
           ((c->flags&DUEL_CARD_FLAG_OCCUPIED)||staged))
            stats|=STAT_HINT_HUNGRY;
        else
            hungry_live_mask&=~(1u<<i);
    }
    if(old_stats&STAT_HINT_JAVELIN) {
        pending=javelin_live_mask;
    while(pending) { i=take_mask_bit(&pending); { DuelCardRecord *c=&D_801A7AD8[i];
        if(javelin_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           javelin_marks[i].data==c->data && c->card_id==JAVELIN_BEETLE_ID) stats|=STAT_HINT_JAVELIN;
        else javelin_live_mask&=~(1u<<i);
    }}
    } else javelin_live_mask=0;
    if(old_stats&STAT_HINT_CRAB) {
        pending=crab_live_mask;
    while(pending) { i=take_mask_bit(&pending); { DuelCardRecord *c=&D_801A7AD8[i];
        if(crab_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           crab_marks[i].data==c->data && c->card_id==CRAB_TURTLE_ID) stats|=STAT_HINT_CRAB;
        else crab_live_mask&=~(1u<<i);
    }}
    } else crab_live_mask=0;
    if(old_stats&STAT_HINT_WHALE) {
        pending=whale_live_mask;
    while(pending) { i=take_mask_bit(&pending); { DuelCardRecord *c=&D_801A7AD8[i];
        if(whale_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           whale_marks[i].data==c->data && c->card_id==FORTRESS_WHALE_ID) stats|=STAT_HINT_WHALE;
        else whale_live_mask&=~(1u<<i);
    }}
    } else whale_live_mask=0;
    pending=tri_live_mask;
    while(pending) { i=take_mask_bit(&pending);
        if(tri_live(i)) { if(old_stats&STAT_HINT_TRI)stats|=STAT_HINT_TRI; }
        else tri_live_mask&=~(1u<<i);
    }
    pending=garma_live_mask;
    while(pending) { i=take_mask_bit(&pending); { DuelCardRecord *c=&D_801A7AD8[i];
        if(!(garma_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
             garma_marks[i].data==c->data && c->card_id==GARMA_SWORD_ID)) garma_live_mask&=~(1u<<i);
    }}
    pending=gate_live_mask;
    while(pending) { i=take_mask_bit(&pending); { DuelCardRecord *c=&D_801A7AD8[i];
        if(!(gate_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
             gate_marks[i].data==c->data && c->card_id==GATE_GUARDIAN_ID)) gate_live_mask&=~(1u<<i);
    }}

    if(old_stats&STAT_HINT_WAR_LION) {
        pending=war_lion_live_mask;
        while(pending) { i=take_mask_bit(&pending);
            if(war_lion_live(i)) stats|=STAT_HINT_WAR_LION;
            else war_lion_live_mask&=~(1u<<i);
        }
    } else war_lion_live_mask=0;
    if(old_stats&STAT_HINT_BLS) {
        pending=bls_live_mask;
        while(pending) { i=take_mask_bit(&pending);
            if(bls_live(i)) stats|=STAT_HINT_BLS;
            else bls_live_mask&=~(1u<<i);
        }
    } else bls_live_mask=0;
    if(old_stats&STAT_HINT_COSMO) {
        pending=cosmo_live_mask;
        while(pending) { i=take_mask_bit(&pending);
            if(cosmo_identity(i) && ritual_effectively_present(i)) stats|=STAT_HINT_COSMO;
            else { memset(&cosmo_marks[i],0,sizeof(cosmo_marks[i])); cosmo_live_mask&=~(1u<<i); }
        }
    } else cosmo_live_mask=0;
    if(old_stats&STAT_HINT_DOKU) {
        pending=dokurorider_live_mask;
        while(pending) { i=take_mask_bit(&pending);
            if(dokurorider_identity(i) && ritual_effectively_present(i)) stats|=STAT_HINT_DOKU;
            else { memset(&dokurorider_marks[i],0,sizeof(dokurorider_marks[i])); dokurorider_live_mask&=~(1u<<i); }
        }
    } else dokurorider_live_mask=0;
    if(old_stats&STAT_HINT_CHAKRA) {
        pending=chakra_live_mask;
        while(pending) {
            i=take_mask_bit(&pending);
            if(chakra_marks[i].ritual && D_801A7AD8[i].data==chakra_marks[i].data &&
               D_801A7AD8[i].card_id==CHAKRA_ID &&
               ((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) ||
                ((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 && battle_participant(i))))
                stats|=STAT_HINT_CHAKRA;
            else { memset(&chakra_marks[i],0,sizeof(chakra_marks[i]));
                   chakra_live_mask&=~(1u<<i);chakra_pending_mask&=~(1u<<i); }
        }
    }
    pending=skull_live_mask;
    while(pending) {
        i=take_mask_bit(&pending);
        if(skull_live(i) && ritual_card_effect_enabled(SKULL_GUARDIAN_ID))stats|=STAT_HINT_SKULL;
        else {memset(&skull_marks[i],0,sizeof(skull_marks[i]));skull_live_mask&=~(1u<<i);}
    }
    /* Retire the exact Ritual source when it has left the field; during the
     * native battle scene its field record may only be temporarily deactivated. */
    pending=zera_live_mask;
    while(pending) {
        i=take_mask_bit(&pending);
        { DuelCardRecord *c=&D_801A7AD8[i];
          int staged=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 && battle_participant(i);
          if(zera_marks[i].ritual && c->card_id==ZERA_ID && c->data==zera_marks[i].data &&
             ((c->flags&DUEL_CARD_FLAG_OCCUPIED)||staged) && ritual_card_effect_enabled(ZERA_ID))
              stats|=STAT_HINT_ZERA;
          else {memset(&zera_marks[i],0,sizeof(zera_marks[i]));zera_live_mask&=~(1u<<i);}
        }
    }
    pending=ultimate_live_mask;
    while(pending) { i=take_mask_bit(&pending);
        if(!(ultimate_identity(i) && ritual_effectively_present(i))) { memset(&ultimate_marks[i],0,sizeof(ultimate_marks[i])); ultimate_live_mask&=~(1u<<i); }
    }
    pending=sengenjin_live_mask;
    while(pending) { i=take_mask_bit(&pending);
        int staged = (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
            ((D_800E9EF0[0] && D_800E9EF0[0]->field_6A==i) ||
             (D_800E9EF0[1] && D_800E9EF0[1]->field_6A==i));
        if(!sengenjin_live(i) && !(staged && sengenjin_identity(i)))
            sengenjin_live_mask&=~(1u<<i);
    }

    if(garma_live_mask && ritual_card_effect_enabled(GARMA_SWORD_ID))stats|=STAT_HINT_GARMA;
    stat_effect_hints=stats;
    garma_live_hint=garma_live_mask!=0;
    tri_live_hint=tri_live_mask!=0;
    gate_live_hint=gate_live_mask!=0;
    sengenjin_live_hint=sengenjin_live_mask!=0 && ritual_card_effect_enabled(SENGENJIN_ID);
    bls_live_hint=bls_live_mask!=0 && ritual_card_effect_enabled(BLACK_LUSTER_SOLDIER_ID);
    ultimate_live_hint=ultimate_live_mask!=0 && ritual_card_effect_enabled(BLUE_EYES_ULTIMATE_ID);
    cosmo_live_hint=cosmo_live_mask!=0 && ritual_card_effect_enabled(COSMO_QUEEN_ID);
    dokurorider_live_hint=dokurorider_live_mask!=0 && ritual_card_effect_enabled(DOKURORIDER_ID);
}

static int sengenjin_preview_filter_guard;
typedef struct { unsigned packed; int valid; } SengenjinStatSnapshot;
static s32 sengenjin_filtered_battle_stat(DuelCardRecord *card,DuelCardRecord *opponent,int defense,int with_guardian);
static s32 sengenjin_filtered_battle_stat_with_full(DuelCardRecord *card,DuelCardRecord *opponent,int defense,int with_guardian,SengenjinStatSnapshot *full_stats);

static s32 calc_stats(DuelCardRecord *card)
{
    uint32_t packed = (uint32_t)original_stats(card);
    int i, live_monster, new_effect_monster, preview_monster, aura_monster, side, type=-1;
    /* CalcCardStats is one of the hottest hooks in a duel. If no currently
     * live Ritual can alter stats (and Fear is not active in battle), avoid
     * even the record-index/slot/type bookkeeping. */
    if (!stat_effect_hints && serpent_fear_battle_slot < 0 && !sengenjin_live_hint && !garma_live_hint) return (s32)packed;
    i = record_index(card);
    live_monster = i >= 0 && monster_slot(i) && (card->flags & DUEL_CARD_FLAG_OCCUPIED);
    /* Retail deactivates attacker/defender field records before phase 9.
     * New persistent stat effects must still see that exact participant as a
     * field monster during the battle, without changing the established
     * live semantics used by older Ritual effects. */
    new_effect_monster = live_monster || (i >= 0 && monster_slot(i) &&
        (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 && battle_participant(i));
    /* Only func_80016784's synchronous draw sets this flag.  Other callers
     * continue to receive the unchanged real duel stats for hand records. */
    preview_monster=(i>=0 && i==hand_aura_preview_record);
    aura_monster=new_effect_monster || preview_monster;
    side = aura_monster ? side_for_record(i) : -1;
    if ((stat_effect_hints & STAT_HINT_SHIELD) && i >= 0 && monster_slot(i) && marks[i].ritual) {
        if (marks[i].data != card->data || marks[i].card_id != card->card_id) {
            forget(card);
        } else if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) || marks[i].in_battle) {
            unsigned defense = (packed >> 16) + marks[i].attack_count * BONUS;
            if (defense > CARD_STAT_MAX) defense = CARD_STAT_MAX;
            packed = (packed & 0xffffu) | (defense << 16);
        }
    }
    if ((stat_effect_hints & STAT_HINT_CHAKRA) && new_effect_monster &&
        i >= 0 && chakra_marks[i].ritual && card->card_id==CHAKRA_ID &&
        chakra_marks[i].data==card->data && ritual_card_effect_enabled(CHAKRA_ID)) {
        unsigned atk=(packed&0xffffu)+chakra_marks[i].bonus;
        if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;
        packed=(packed&0xffff0000u)|atk; /* Defense untouched. */
    }
    if ((stat_effect_hints & STAT_HINT_HUNGRY) && i >= 0 && monster_slot(i) && hungry_marks[i].ritual) {
        if (hungry_marks[i].data != card->data || card->card_id != HUNGRY_BURGER_ID) {
            hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0; hungry_live_mask &= ~(1u<<i);
        } else {
            unsigned atk = (packed & 0xffffu) + hungry_marks[i].battle_kills * HUNGRY_BONUS;
            unsigned def = (packed >> 16) + hungry_marks[i].battle_kills * HUNGRY_BONUS;
            if (atk > CARD_STAT_MAX) atk = CARD_STAT_MAX;
            if (def > CARD_STAT_MAX) def = CARD_STAT_MAX;
            packed = atk | (def << 16);
        }
    }
    /* V4 Garma: a temporary attack-only layer, tied to the Ritual instance.
     * Keep it across the sixth battle until the retail state-11 fade ends. */
    if ((garma_live_hint || (stat_effect_hints & STAT_HINT_GARMA)) && new_effect_monster &&
        i>=0 && garma_marks[i].ritual && card->card_id==GARMA_SWORD_ID &&
        garma_marks[i].data==card->data && ritual_card_effect_enabled(GARMA_SWORD_ID)) {
        unsigned atk=(packed&0xffffu)+garma_marks[i].strike_bonus;
        if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;
        packed=(packed&0xffff0000u)|atk;
    }
    if ((stat_effect_hints & (STAT_HINT_JAVELIN|STAT_HINT_CRAB|STAT_HINT_TRI)) && (live_monster||preview_monster))
        type = card_type_id(card);
    else if ((stat_effect_hints & (STAT_HINT_WAR_LION|STAT_HINT_BLS|STAT_HINT_COSMO|STAT_HINT_DOKU)) && aura_monster)
        type = card_type_id(card);
    if ((stat_effect_hints & STAT_HINT_JAVELIN) && (live_monster||preview_monster) && type == CARD_TYPE_INSECT) {
        unsigned sources = beacon_source_count(javelin_live_mask,side,i,BEACON_JAVELIN);
        if (sources) {
            /* On placement, this previewed Insect joins the allied count. */
            unsigned bonus = (allied_insect_count(side)+(preview_monster?1u:0u)) * JAVELIN_AURA_BONUS * sources;
            unsigned atk = (packed & 0xffffu) + bonus, def = (packed >> 16) + bonus;
            if (atk > CARD_STAT_MAX) atk = CARD_STAT_MAX; if (def > CARD_STAT_MAX) def = CARD_STAT_MAX;
            packed = atk | (def << 16);
        }
    }
    if (live_monster||preview_monster) {
        if ((stat_effect_hints & STAT_HINT_CRAB) && sea_family_type(type) &&
            side_has_other_crab(side,preview_monster?-1:i)) {
            unsigned atk=(packed&0xffffu)+CRAB_AURA_ATK, def=(packed>>16)+CRAB_AURA_DEF;
            if(atk>CARD_STAT_MAX) atk=CARD_STAT_MAX; if(def>CARD_STAT_MAX) def=CARD_STAT_MAX; packed=atk|(def<<16);
        }
        if (live_monster && (stat_effect_hints & STAT_HINT_WHALE) && card->card_id==FORTRESS_WHALE_ID && whale_marks[i].ritual && whale_marks[i].data==card->data) {
            unsigned def=(packed>>16)+whale_fish_defense_bonus(side,i); if(def>CARD_STAT_MAX) def=CARD_STAT_MAX; packed=(packed&0xffffu)|(def<<16);
        }
    }
    if ((stat_effect_hints & STAT_HINT_TRI) && (live_monster||preview_monster) && type==CARD_TYPE_DINOSAUR) {
        /* Another Tri-Horned is still an allied Dinosaur. Previewed cards
         * are not yet a live aura source and cannot buff themselves. */
        unsigned sources=preview_monster ?
            aura_sources_for_preview(tri_live_mask,side,1) : tri_sources(i);
        unsigned bonus=1000u*sources;
        if(bonus) {
            unsigned atk=(packed&0xffffu)+bonus,def=(packed>>16)+bonus;
            if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
            packed=atk|(def<<16);
        }
    }
    if ((stat_effect_hints & STAT_HINT_WAR_LION) && new_effect_monster && war_lion_identity(i)) {
        unsigned bonus=beacon_other_allied_type_hand_field(side,CARD_TYPE_BEAST,i)*WAR_LION_BEAST_BONUS;
        if(bonus) {
            unsigned atk=(packed&0xffffu)+bonus,def=(packed>>16)+bonus;
            if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
            packed=atk|(def<<16);
        }
    }
    if ((stat_effect_hints & STAT_HINT_BLS) && aura_monster && type==CARD_TYPE_WARRIOR) {
        unsigned sources=ritual_card_effect_enabled(BLACK_LUSTER_SOLDIER_ID) ?
            beacon_source_count(bls_live_mask,side,preview_monster?-1:i,BEACON_BLS) : 0;
        unsigned bonus=sources*BLS_WARRIOR_AURA_BONUS;
        if(bonus) {
            unsigned atk=(packed&0xffffu)+bonus,def=(packed>>16)+bonus;
            if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
            packed=atk|(def<<16);
        }
    }
    if ((stat_effect_hints & STAT_HINT_COSMO) && new_effect_monster && cosmo_identity(i)) {
        unsigned bonus=(female_field_count_except(i)+1u)*COSMO_FEMALE_BONUS;
        if(bonus) {
            unsigned atk=(packed&0xffffu)+bonus,def=(packed>>16)+bonus;
            if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
            packed=atk|(def<<16);
        }
    }
    if ((stat_effect_hints&STAT_HINT_SKULL) && new_effect_monster &&
        skull_identity(i) && ritual_card_effect_enabled(SKULL_GUARDIAN_ID)) {
        unsigned bonus=skull_bonus_for(i);
        unsigned atk=(packed&0xffffu)+bonus,def=(packed>>16)+bonus;
        if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;
        if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
        packed=atk|(def<<16);
    }
    if ((stat_effect_hints & STAT_HINT_DOKU) && aura_monster && type==CARD_TYPE_ZOMBIE) {
        unsigned sources=beacon_source_count(dokurorider_live_mask,side,preview_monster?-1:i,BEACON_DOKU);
        unsigned bonus=sources*DOKURORIDER_AURA_ATK;
        if(bonus) {
            unsigned atk=(packed&0xffffu)+bonus;
            if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;
            packed=(packed&0xffff0000u)|atk;
        }
    }
    /* A normal or fusion-summoned Ultimate has no Ritual immunity. Only an
     * Ultimate whose Ritual effect is actually active can ignore Zera's aura.
     * Keep the earlier immunity of a genuine Ritual Ultimate intact. */
    if ((stat_effect_hints&STAT_HINT_ZERA) && aura_monster &&
        card->card_id!=ZERA_ID && !ultimate_effect_active(i) &&
        ritual_card_effect_enabled(ZERA_ID)) {
        int target_type=type<0?card_type_id(card):type;
        if(target_type!=CARD_TYPE_FIEND && target_type>=0 && target_type<CARD_TYPE_MAGIC) {
            unsigned sources=0,pending=zera_live_mask;
            while(pending) {
                int src=take_mask_bit(&pending);
                DuelCardRecord *z=&D_801A7AD8[src];
                if(zera_marks[src].ritual && zera_marks[src].data==z->data &&
                   z->card_id==ZERA_ID && ((z->flags&DUEL_CARD_FLAG_OCCUPIED)||
                    ((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 && battle_participant(src))))++sources;
            }
            if(sources) {
                unsigned penalty=sources*ZERA_AURA_PENALTY;
                unsigned atk=packed&0xffffu,def=packed>>16;
                atk=atk>penalty?atk-penalty:0;
                def=def>penalty?def-penalty:0;
                packed=atk|(def<<16);
            }
        }
    }
    if (serpent_seen && ritual_card_effect_enabled(SERPENT_NIGHT_ID) && i>=0 && i<DUEL_CARD_RECORD_COUNT && serpent_fear_bonus[i]) {
        unsigned atk=(packed&0xffffu)+serpent_fear_bonus[i], def=(packed>>16)+serpent_fear_bonus[i];
        if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX; if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
        packed=atk|(def<<16);
    }
    /* Battle case 2 builds the visible ATK/DEF panels with plain
     * Duel_CalcCardStats().  Sengenjin's comparison hooks already used the
     * filtered value, so without this preview-only branch the screen could
     * advertise a buffed value that is not actually used by the battle.
     * Restrict the override to the two currently staged participants and use
     * a recursion guard because the filter asks the full hook chain once to
     * preserve third-party debuffs. */
    if(!sengenjin_preview_filter_guard && sengenjin_live_hint &&
       ritual_card_effect_enabled(SENGENJIN_ID) && i>=0 &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
       D_800E9EF0[0] && D_800E9EF0[1]) {
        int left=(int)D_800E9EF0[0]->field_6A;
        int right=(int)D_800E9EF0[1]->field_6A;
        int source=-1,target=-1;
        if(left>=0 && left<DUEL_CARD_RECORD_COUNT && right>=0 && right<DUEL_CARD_RECORD_COUNT) {
            if(sengenjin_identity(left) && side_for_record(left)!=side_for_record(right)) { source=left; target=right; }
            else if(sengenjin_identity(right) && side_for_record(left)!=side_for_record(right)) { source=right; target=left; }
        }
        if(i==target && source>=0 && !ultimate_effect_active(target)) {
            unsigned atk,def;
            SengenjinStatSnapshot full_stats={0,0};
            sengenjin_preview_filter_guard=1;
            /* Both axes see the same synchronous hook-chain snapshot. Keep
             * their independent native copies: a chained hook may derive
             * one attribute from the other, which must remain unclamped.
             * The first filter captures it after its native calculation,
             * preserving the existing native-then-public call order. */
            atk=(unsigned)sengenjin_filtered_battle_stat_with_full(card,&D_801A7AD8[source],0,0,&full_stats);
            def=(unsigned)sengenjin_filtered_battle_stat_with_full(card,&D_801A7AD8[source],1,0,&full_stats);
            sengenjin_preview_filter_guard=0;
            packed=(atk&0xffffu)|((def&0xffffu)<<16);
        }
    }
    return (s32)packed;
}

static int sengenjin_native_negative(DuelCardRecord *card)
{
    int i=record_index(card); SengenjinNegativeMark *m;
    if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !card || !card->data)return 0;
    m=&sengenjin_negative[i];
    if(m->data!=card->data || m->card_id!=card->card_id) {
        m->data=card->data; m->card_id=card->card_id; m->negative=0;
    }
    return m->negative;
}
static void sengenjin_add_native_negative(int i,void *data,u16 card_id,int amount)
{
    SengenjinNegativeMark *m; int next;
    if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !data || amount>=0)return;
    m=&sengenjin_negative[i];
    if(m->data!=data || m->card_id!=card_id) {
        m->data=data; m->card_id=card_id; m->negative=0;
    }
    next=(int)m->negative+amount;
    if(next<-32767)next=-32767;
    m->negative=(s16)next;
}
/* Spellbinding Circle and Shadow Spell are the native negative stat sources.
 * Snapshot the ten Monster Zones around their rare effect step so their
 * contribution stays separable from Equip bonuses already sharing the same
 * stat_modifier.  This bookkeeping is dormant when Sengenjin's effect switch
 * is OFF and never writes gameplay stats itself. */
static void sengenjin_track_stat_penalty(void)
{
    int slots[10],n=0,side,k; s16 before[10]; void *data[10]; u16 ids[10];
    if(!ritual_card_effect_enabled(SENGENJIN_ID)){original_stat_penalty();return;}
    for(side=0;side<2;++side) {
        int start=side?20:5;
        for(k=start;k<start+5;++k) {
            DuelCardRecord *c=&D_801A7AD8[k];
            slots[n]=k; before[n]=c->stat_modifier; data[n]=c->data; ids[n]=c->card_id; ++n;
        }
    }
    original_stat_penalty();
    for(k=0;k<n;++k) {
        int i=slots[k]; DuelCardRecord *c=&D_801A7AD8[i];
        if(c->data==data[k] && c->card_id==ids[k] && c->stat_modifier<before[k])
            sengenjin_add_native_negative(i,data[k],ids[k],(int)c->stat_modifier-(int)before[k]);
        else if(c->data!=data[k] || c->card_id!=ids[k])
            memset(&sengenjin_negative[i],0,sizeof(sengenjin_negative[i]));
    }
}
/* Cursebreaker clears a negative net modifier.  When that happens the native
 * penalty ledger for the same card identity is cleared as well; if the net is
 * still positive, retail did not remove the hidden penalty and neither do we. */
static void sengenjin_track_cursebreaker(void)
{
    int slots[10],n=0,side,k; s16 before[10]; void *data[10]; u16 ids[10];
    if(!ritual_card_effect_enabled(SENGENJIN_ID)){original_cursebreaker();return;}
    for(side=0;side<2;++side) {
        int start=side?20:5;
        for(k=start;k<start+5;++k) {
            DuelCardRecord *c=&D_801A7AD8[k];
            slots[n]=k; before[n]=c->stat_modifier; data[n]=c->data; ids[n]=c->card_id; ++n;
        }
    }
    original_cursebreaker();
    for(k=0;k<n;++k) {
        int i=slots[k]; DuelCardRecord *c=&D_801A7AD8[i];
        if(c->data==data[k] && c->card_id==ids[k] && before[k]<0 && c->stat_modifier>=0) {
            SengenjinNegativeMark *m=&sengenjin_negative[i];
            if(m->data==data[k] && m->card_id==ids[k])m->negative=0;
        } else if(c->data!=data[k] || c->card_id!=ids[k])
            memset(&sengenjin_negative[i],0,sizeof(sengenjin_negative[i]));
    }
}

static s32 sengenjin_filtered_battle_stat_with_full(DuelCardRecord *card,DuelCardRecord *opponent,int defense,int with_guardian,SengenjinStatSnapshot *full_stats)
{
    DuelCardRecord copy;
    unsigned stats;
    int base, current, ceiling, gs, native_negative;
    if(!card)return 0;
    copy=*card;
    stats=(unsigned)gDuel_adwCardStats[card->card_id-1];
    base=defense ? (int)(((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE)
                 : (int)((stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE);
    current=defense?copy.defense:copy.attack;
    if(current>base) {
        if(defense)copy.defense=(s16)base;
        else copy.attack=(s16)base;
    }
    native_negative=sengenjin_native_negative(card);
    if(native_negative<0) {
        /* If the live net is already more negative, preserve that too (for
         * example another mod's debuff). Otherwise expose the tracked native
         * penalty without the positive Equip amount that had masked it. */
        if(copy.stat_modifier>native_negative)copy.stat_modifier=(s16)native_negative;
    } else if(copy.stat_modifier>0)copy.stat_modifier=0;
    if(copy.terrain_modifier>0)copy.terrain_modifier=0;
    stats=(unsigned)original_stats(&copy);
    current=defense?(int)(stats>>16):(int)(stats&0xffffu);
    {
        unsigned full;
        if(full_stats && full_stats->valid)full=full_stats->packed;
        else {
            full=(unsigned)Duel_CalcCardStats(card);
            if(full_stats) {full_stats->packed=full;full_stats->valid=1;}
        }
        int full_value=defense?(int)(full>>16):(int)(full&0xffffu);
        if(full_value<current)current=full_value;
    }
    /* The sanitised record removes retail Field/Equip/direct positive layers.
     * Cap any remaining chained positive stat hook while allowing a lower
     * value (a debuff) to survive. */
    ceiling=base;
    if((defense?card->defense:card->attack)<base)
        ceiling-=base-(defense?card->defense:card->attack);
    if(card->stat_modifier<0)ceiling+=card->stat_modifier;
    if(card->terrain_modifier<0)ceiling+=card->terrain_modifier;
    if(ceiling<0)ceiling=0;
    if(current>ceiling)current=ceiling;
    if(with_guardian) {
        gs=Duel_CalcGuardianStarBonus(card,opponent);
        if(gs<0)current+=gs;
    }
    if(current<0)current=0;
    if(current>CARD_STAT_MAX)current=CARD_STAT_MAX;
    return current;
}
static s32 sengenjin_filtered_battle_stat(DuelCardRecord *card,DuelCardRecord *opponent,int defense,int with_guardian)
{
    return sengenjin_filtered_battle_stat_with_full(card,opponent,defense,with_guardian,0);
}
static s32 battle_attack(DuelCardRecord *card,DuelCardRecord *opponent)
{
    int oi;
    if(!sengenjin_live_hint || !opponent || !ritual_card_effect_enabled(SENGENJIN_ID))
        return original_battle_attack(card,opponent);
    oi=record_index(opponent);
    if(oi>=0 && sengenjin_identity(oi)) {
        int ci=record_index(card);
        if(ci>=0 && side_for_record(ci)!=side_for_record(oi) && ultimate_effect_active(ci))
            return original_battle_attack(card,opponent);
        return sengenjin_filtered_battle_stat(card,opponent,0,1);
    }
    return original_battle_attack(card,opponent);
}
static s32 battle_defense(DuelCardRecord *card,DuelCardRecord *opponent)
{
    int oi;
    if(!sengenjin_live_hint || !opponent || !ritual_card_effect_enabled(SENGENJIN_ID))
        return original_battle_defense(card,opponent);
    oi=record_index(opponent);
    if(oi>=0 && sengenjin_identity(oi)) {
        int ci=record_index(card);
        if(ci>=0 && side_for_record(ci)!=side_for_record(oi) && ultimate_effect_active(ci))
            return original_battle_defense(card,opponent);
        return sengenjin_filtered_battle_stat(card,opponent,1,1);
    }
    return original_battle_defense(card,opponent);
}

/* Guardian Star is directional: a positive value favours `left`, while a
 * negative value favours `right`.  Sengenjin ignores only the opponent's
 * favourable half of that matchup.  Hooking this primitive also keeps AI
 * comparisons and any other battle-stat caller consistent with the visible
 * duel result instead of patching one presentation path. */
static s32 guardian_star_bonus(DuelCardRecord *left,DuelCardRecord *right)
{
    s32 bonus=original_guardian_star_bonus(left,right);
    /* Keep the native Guardian-Star relation for field/UI colour.  The battle
     * sequencer, however, asks this same primitive again in phase 9/state 4
     * specifically to animate the +/-500 stat change.  If that positive layer
     * belongs to Sengenjin's foe, return neutral only for this presentation
     * call: the red/blue matchup colour remains Vanilla, but no false +500
     * animation or temporary 2800-style display is shown. */
    if(bonus && sengenjin_live_hint && left && right &&
       ritual_card_effect_enabled(SENGENJIN_ID) &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
       (D_8009B174&0x0f)==4) {
        int li=record_index(left),ri=record_index(right);
        if(li>=0 && li<DUEL_CARD_RECORD_COUNT && ri>=0 && ri<DUEL_CARD_RECORD_COUNT &&
           side_for_record(li)!=side_for_record(ri)) {
            if(sengenjin_identity(ri) && bonus>0 && !ultimate_effect_active(li))return 0;
            if(sengenjin_identity(li) && bonus<0 && !ultimate_effect_active(ri))return 0;
        }
    }
    return bonus;
}

static int dokurorider_should_evade(DuelCardRecord *attacker,DuelCardRecord *defender)
{
    int ai,di,level,attacker_value,defender_value;
    if(!attacker||!defender||!dokurorider_live_hint||
       !ritual_card_effect_enabled(DOKURORIDER_ID))return 0;
    ai=record_index(attacker);di=record_index(defender);
    if(ai<0||di<0||!dokurorider_identity(di)||side_for_record(ai)==side_for_record(di))return 0;
    /* Ultimate immunity does not stop this: Dokurorider moves itself out of
     * harm instead of applying an effect to the attacker. */
    level=Cards_Level(attacker->card_id);
    if(level<6)return 0;
    attacker_value=Duel_CalcBattleAttack(attacker,defender);
    defender_value=(defender->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?
        Duel_CalcBattleDefense(defender,attacker):Duel_CalcBattleAttack(defender,attacker);
    return attacker_value>=defender_value;
}

/* Retail compares the attacking/left monster through Duel_CalcBattle* but
 * reads the defending/right monster through plain Duel_CalcCardStats.  That
 * asymmetry matters for Sengenjin: when Sengenjin attacks, filtering only the
 * BattleAttack hook would leave a defender's positive Field/Equip layers in
 * the comparison.  Reproduce retail's comparison only for that one case and
 * feed the opponent through the same positive-layer filter. */
static s32 sengenjin_battle_compare(DisplayObject *left_object,DisplayObject *right_object)
{
    DuelCardRecord *left,*right; int li,ri,actual,wanted,gs; unsigned packed;
    if(left_object&&right_object && (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9) {
        int dli=(int)left_object->field_6A, dri=(int)right_object->field_6A;
        if(dli>=0&&dli<DUEL_CARD_RECORD_COUNT&&dri>=0&&dri<DUEL_CARD_RECORD_COUNT &&
           dokurorider_should_evade(&D_801A7AD8[dli],&D_801A7AD8[dri])) {
            int attacker_side=side_for_record(dli);
            /* The CPU learns the speed-evasion only after a real committed
             * setback.  Level 6+ is the final design threshold.  Knowledge is
             * stored on this Ritual instance and is reset with a new summon. */
            if(attacker_side>=0 && attacker_side<DUEL_SIDE_COUNT &&
               D_8009B1D5==attacker_side && D_8009B360[attacker_side]>=0) {
                dokurorider_marks[dri].learned_sides|=1u<<attacker_side;
                ritual_ai_learn_full_effect_for_side(attacker_side,DOKURORIDER_ID);
            }
            dokurorider_evade_pending|=1u<<dri;
            return 0; /* Native no-destruction/no-damage result; attack remains consumed. */
        }
    }
    /* The native battle controller enters this comparator AFTER switching
     * D_8009B174 from state 3 to state 4. Testing for state 3 here made the
     * whole protection unreachable during actual battles (v0.78.49).
     * battle() itself still enters on state 3 and applies the prepaid LP
     * loss after original_battle() calls this comparator at state 4. */
    /* A Ritual Fiend's Mirror in DEF behaves like an open path to its
     * controller's LP: ATK is applied in full, irrespective of Mirror DEF,
     * and the Mirror survives. The normal reflection watcher then makes the
     * attacker pay the same loss. A normal/summoned Mirror is untouched. */
    if(fiend_mirror_live_hint && fiends_mirror_effect_on &&
       left_object && right_object &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
       (D_8009B174&0x0f)==4) {
        int attacker=(int)left_object->field_6A;
        int defender=(int)right_object->field_6A;
        if(attacker>=0 && attacker<DUEL_CARD_RECORD_COUNT &&
           defender>=0 && defender<DUEL_CARD_RECORD_COUNT &&
           fiend_mirror_marks[defender].ritual &&
           D_801A7AD8[defender].card_id==FIENDS_MIRROR_ID &&
           D_801A7AD8[defender].data &&
           D_801A7AD8[defender].data==fiend_mirror_marks[defender].data &&
           (D_8009B17A&DUEL_CARD_FLAG_DEFENSE_POSITION) &&
           side_for_record(defender)!=D_8009B1D5) {
            mirror_defense_direct_pending=(unsigned)Duel_CalcCardStats(&D_801A7AD8[attacker]) & 0xffffu;
            mirror_defense_direct_side=side_for_record(defender);
            return 0; /* neutral combat: no damage to a monster, no kill */
        }
    }
    if(!sengenjin_live_hint || !right_object || !left_object ||
       !ritual_card_effect_enabled(SENGENJIN_ID))
        return original_battle_compare(left_object,right_object);
    li=(int)left_object->field_6A; ri=(int)right_object->field_6A;
    if(li<0||li>=DUEL_CARD_RECORD_COUNT||ri<0||ri>=DUEL_CARD_RECORD_COUNT)
        return original_battle_compare(left_object,right_object);
    left=&D_801A7AD8[li]; right=&D_801A7AD8[ri];
    /* Learn from the first committed battle in which Sengenjin actually
     * suppresses a positive layer.  The old test required a very specific
     * predicted-win -> real-loss transition and could miss cases such as a
     * Guardian-Star +500 (Crimson Sunbird-style 2300 -> fake 2800).  Once the
     * CPU has visibly experienced any reduction, later target evaluation uses
     * ritual_ai_actual_margin() and therefore does not repeat that mistake. */
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
       sengenjin_identity(ri) && side_for_record(li)!=side_for_record(ri) &&
       D_8009B1D5==side_for_record(li) && D_8009B360[D_8009B1D5]>=0 &&
       !ultimate_effect_active(li)) {
        int native_actual=(left->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?
            original_battle_defense(left,right):original_battle_attack(left,right);
        int filtered_actual=(left->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?
            sengenjin_filtered_battle_stat(left,right,1,1):
            sengenjin_filtered_battle_stat(left,right,0,1);
        if(filtered_actual<native_actual) {
            sengenjin_marks[ri].learned_sides|=1u<<D_8009B1D5;
            ritual_ai_learn_full_effect_for_side(D_8009B1D5,SENGENJIN_ID);
            if(host&&host->log)
                host->log(host,"[Ritual Card Effects] CPU aprendeu Sengenjin: slot %d native=%d real=%d",
                          ri,native_actual,filtered_actual);
        }
    }
    if(!sengenjin_identity(li) || side_for_record(li)==side_for_record(ri) ||
       ultimate_effect_active(ri))
        return original_battle_compare(left_object,right_object);
    packed=(unsigned)Duel_CalcCardStats(left);
    actual=(left->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)?(int)(packed>>16):(int)(packed&0xffffu);
    gs=original_guardian_star_bonus(left,right);
    /* A negative left->right matchup is the opponent's favourable Guardian
     * Star layer. Sengenjin ignores that gameplay bonus, but the global
     * Guardian-Star primitive remains untouched so the UI still paints the
     * matchup with Vanilla colours. */
    if(gs<0 && !ultimate_effect_active(ri))gs=0;
    actual+=gs; if(actual<0)actual=0; if(actual>CARD_STAT_MAX)actual=CARD_STAT_MAX;
    wanted=sengenjin_filtered_battle_stat(right,left,
        (right->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)!=0,0);
    if(actual!=wanted)return actual-wanted;
    if((left->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)||
       (right->flags&DUEL_CARD_FLAG_DEFENSE_POSITION))return 0;
    return -1;
}

/* Reverse Trap (689) is resolved by func_8001B170 rather than the generic
 * card-effect dispatcher.  Keep its native presentation/consumption, but
 * restore Sengenjin's modifier on the exact frame where Reverse Trap tries
 * to invert it.  Other equip/boost processing in this callback is untouched. */
static void sengenjin_reverse_trap_step(void)
{
    DisplayObject *object=D_800E9EF0[0];
    int idx=object?(int)object->field_6A:-1;
    int reverse=idx>=0 && idx<DUEL_CARD_RECORD_COUNT &&
        D_8009B22A==DUEL_REVERSE_TRAP_CARD_ID &&
        ritual_card_effect_enabled(SENGENJIN_ID);
    int immune=reverse && (sengenjin_identity(idx) || ultimate_effect_active(idx));
    s16 before=0; void *data=0; u16 card_id=0;
    if(reverse){before=D_801A7AD8[idx].stat_modifier;data=D_801A7AD8[idx].data;card_id=D_801A7AD8[idx].card_id;}
    original_func_8001B170();
    if(!reverse || D_801A7AD8[idx].data!=data || D_801A7AD8[idx].card_id!=card_id)return;
    if(immune && D_801A7AD8[idx].stat_modifier!=before) {
        D_801A7AD8[idx].stat_modifier=before;
        if(ultimate_effect_active(idx))ultimate_ai_learn_category(D_8009B1D5^1,1);
        visual_queue_activation(idx);
        if(host&&host->log)host->log(host,"[Ritual Card Effects] %s ignorou Reverse Trap: slot %d",
            ultimate_effect_active(idx)?"Blue-Eyes Ultimate":"Sengenjin",idx);
    } else if(!immune && D_801A7AD8[idx].stat_modifier!=before && D_8009B154>0) {
        /* Reverse Trap converts this placement's +bonus into -bonus.  Record
         * the negative half, not the full -2*bonus delta, so filtering away
         * the positive Equip later leaves exactly the reversed penalty. */
        sengenjin_add_native_negative(idx,data,card_id,-(int)D_8009B154);
    }
}

/* Javelin Beetle's shell is a battle-only survivor. It is intentionally
 * absent from generic card-removal/deactivation hooks; only the battle state-11
 * survivor path may spend it. */
static void javelin_feedback_tick(void)
{
    unsigned pending=javelin_protected_visuals;
    while(pending) {
        int i=take_mask_bit(&pending);DuelCardRecord *c=&D_801A7AD8[i];
        if(javelin_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           c->data==javelin_marks[i].data && c->card_id==JAVELIN_BEETLE_ID) {
            /* The shell SFX + disappearing counter are the feedback.  Do not
             * queue the generic card-activation/fire presentation here: the
             * monster survived, so that animation falsely reads as destruction. */
            AudioReplace_Sfx(JAVELIN_SHELL_SFX_ID,255,0);
            javelin_marks[i].protection_visual_shown=1;
        }
        javelin_protected_visuals&=~(1u<<i);
    }
}

static void remove_card(DuelCardRecord *card)
{
    int i;
    /* Scene phase 7 is card placement/combination. A monster borrowed by
     * Psycho-Puppet is not owned material: do not let placement consume it. */
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 7 &&
        card && data_is_stolen(card->data, D_8009B1D5))
        return;
    i = record_index(card);
    if(i>=0 && ultimate_card_effect.active && (ultimate_card_effect.mask&(1u<<i)) &&
       card->data==ultimate_card_effect.saved[i].data &&
       (ultimate_card_effect.effect_id==DARK_HOLE_ID ||
        RitualEffects_IsCardEffectImmune(i,ultimate_card_effect.source_side))) {
        ultimate_card_effect.touched|=1u<<i;
        return;
    }
    if(i>=0 && ultimate_effect_active(i) &&
       (inside_effect_destruction || gDuel_wEffectCardID==DARK_HOLE_ID) &&
       (gDuel_wEffectCardID==DARK_HOLE_ID ||
        RitualEffects_IsCardEffectImmune(i,D_8009B1D5))) {
        if(ultimate_card_effect.active && (ultimate_card_effect.mask&(1u<<i)))
            ultimate_card_effect.touched|=1u<<i;
        else if(gDuel_wEffectCardID!=DARK_HOLE_ID)
            visual_queue_activation(i);
        /* Dark Hole request 0x11 is already presenting the board wipe.  An
         * immune Ultimate must simply remain in place; queuing its separate
         * activation presentation while 0x11 is blocking can deadlock that
         * request. */
        return;
    }
    if(i>=0 && ultimate_attack_trap_immune(i)) { visual_queue_activation(i); return; }
    /* External monster effects commonly resolve inside FieldActions rather
     * than the generic Magic/Trap dispatcher.  A hostile direct removal of
     * the opponent's Ritual Ultimate is rejected at the lifecycle boundary;
     * normal battle staging uses DeactivateRecord instead and is unaffected. */
    if(i>=0 && inside_field_actions && RitualEffects_IsCardEffectImmune(i,D_8009B1D5)) {
        visual_queue_activation(i);
        return;
    }
    if(i>=0 && sengenjin_identity(i) && ritual_card_effect_enabled(SENGENJIN_ID) &&
       ((sengenjin_trap.active && (sengenjin_trap.mask&(1u<<i)) &&
         card->data==sengenjin_trap.saved[i].data) || attack_trap_resolution_active())) {
        if(sengenjin_trap.active)sengenjin_trap.touched|=1u<<i;
        else visual_queue_activation(i);
        return;
    }
    if(chaos_hole_protected(i)){
        /* Dark Hole request 0x11 already omits Chaos from its destruction
         * presentation. Keep the same live field object instead of scheduling
         * a redundant post-effect release/rebuild. */
        return;
    }
    if (tri_guarded(i)) {
        effect_survivor_visuals |= 1u << i;
        return;
    }
    if (i >= 0 && monster_slot(i) && (whale_destruction_guard & (1u << i)) &&
        (card->flags & DUEL_CARD_FLAG_OCCUPIED)) {
        effect_survivor_visuals |= 1u << i;
        if (host && host->log) host->log(host, "[Ritual Card Effects] Fortress Whale protegeu slot %d durante toda a resolucao", i);
        return;
    }
        /* Destructive magic effects call the normal field-removal routine.
     * A ritual Millennium Shield ignores that destruction, but ordinary
     * battle removal and non-destruction lifecycle paths are untouched. */
    /* Internal effect-handler calls may bypass hooks on the individual
     * handler.  Dark Hole therefore also identifies its destruction context
     * from the active effect card id at the actual removal point. */
    if ((inside_effect_destruction || gDuel_wEffectCardID == DARK_HOLE_ID) &&
        i >= 0 && marks[i].ritual &&
        marks[i].data == card->data && marks[i].card_id == card->card_id &&
        (card->flags & DUEL_CARD_FLAG_OCCUPIED)) {
        protected_visuals |= 1u << i;
        effect_survivor_visuals |= 1u << i;
        if (host && host->log)
            host->log(host, "[Ritual Card Effects] Millennium Shield resistiu a destruicao por efeito: slot %d", i);
        return;
    }
    if(psycho_controlling_mask) {
        unsigned pending=psycho_controlling_mask;
        while(pending) {
            int p=take_mask_bit(&pending);
            if (psycho_marks[p].ritual && psycho_marks[p].controlling &&
                psycho_marks[p].stolen_data == card->data) {
                /* Retail deactivates both participants during combat and
                 * restores survivors later. Do not mistake this staging for
                 * an actual removal of the controlled monster. */
                if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK)==9 &&
                    i>=0 && battle_participant(i)) continue;
                psycho_marks[p].controlling = 0;
                psycho_controlling_mask &= ~(1u<<p);
                if(psycho_controlling_count)--psycho_controlling_count;
                psycho_marks[p].stolen_data = 0;
                psycho_marks[p].stolen_card_id = 0;
                psycho_marks[p].original_slot = psycho_marks[p].current_slot = -1;
            }
        }
    }
    if (i >= 0 && hungry_marks[i].ritual && hungry_marks[i].data == card->data) {
        hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0;
        hungry_live_mask &= ~(1u<<i);
    }
    /* Garma's five blades belong to the ritualized field instance, not to a
     * turn.  Clear them only when that instance really leaves the field. */
    if (i >= 0 && garma_marks[i].ritual && garma_marks[i].data == card->data) {
        memset(&garma_marks[i], 0, sizeof(garma_marks[i])); garma_live_mask &= ~(1u<<i);
    }
    if(i>=0 && tri_identity(i)){memset(&tri_marks[i],0,sizeof(tri_marks[i]));tri_live_mask&=~(1u<<i);}
    if(i>=0 && gate_marks[i].ritual && gate_marks[i].data==card->data){memset(&gate_marks[i],0,sizeof(gate_marks[i]));gate_live_mask&=~(1u<<i);}
    if(i>=0){
        memset(&performance_marks[i],0,sizeof(performance_marks[i]));
        memset(&chaos_marks[i],0,sizeof(chaos_marks[i]));
        performance_live_mask&=~(1u<<i); chaos_live_mask&=~(1u<<i);
        coin_marks_seen=(performance_live_mask|chaos_live_mask)!=0;
    }
    if(i>=0){memset(&serpent_marks[i],0,sizeof(serpent_marks[i]));serpent_live_mask&=~(1u<<i);}
    if(i>=0 && war_lion_identity(i) && (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9) {
        memset(&war_lion_marks[i],0,sizeof(war_lion_marks[i])); war_lion_live_mask&=~(1u<<i);
    }
    if(i>=0 && sengenjin_identity(i)) {
        memset(&sengenjin_marks[i],0,sizeof(sengenjin_marks[i])); sengenjin_live_mask&=~(1u<<i);
    }
    if(i>=0 && skull_marks[i].ritual && skull_marks[i].data==card->data) {
        memset(&skull_marks[i],0,sizeof(skull_marks[i]));skull_live_mask&=~(1u<<i);
        if(!skull_live_mask)stat_effect_hints&=~STAT_HINT_SKULL;
    }
    if(i>=0 && zera_marks[i].ritual && zera_marks[i].data==card->data) {
        memset(&zera_marks[i],0,sizeof(zera_marks[i]));
        zera_live_mask&=~(1u<<i);
        if(!zera_live_mask)stat_effect_hints&=~STAT_HINT_ZERA;
    }
    if(i>=0 && bls_identity(i)) {
        memset(&bls_marks[i],0,sizeof(bls_marks[i])); bls_live_mask&=~(1u<<i);
    }
    if(i>=0 && ultimate_identity(i)) {
        memset(&ultimate_marks[i],0,sizeof(ultimate_marks[i])); ultimate_live_mask&=~(1u<<i);
    }
    if(i>=0 && cosmo_identity(i)) {
        memset(&cosmo_marks[i],0,sizeof(cosmo_marks[i])); cosmo_live_mask&=~(1u<<i);
    }
    if(i>=0 && dokurorider_identity(i)) {
        memset(&dokurorider_marks[i],0,sizeof(dokurorider_marks[i])); dokurorider_live_mask&=~(1u<<i);
    }
    if(i>=0 && chakra_marks[i].ritual && chakra_marks[i].data==card->data) {
        memset(&chakra_marks[i],0,sizeof(chakra_marks[i]));
        chakra_live_mask&=~(1u<<i);chakra_pending_mask&=~(1u<<i);
    }
    forget(card);
    forget_yamadron(card);
    mask_forget(card->data);
    original_remove(card);
}
static void deactivate_card(DuelCardRecord *card)
{
    int i;
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 7 &&
        card && data_is_stolen(card->data, D_8009B1D5))
        return;
    i = record_index(card);
    if(i>=0 && ultimate_card_effect.active && (ultimate_card_effect.mask&(1u<<i)) &&
       card->data==ultimate_card_effect.saved[i].data &&
       (ultimate_card_effect.effect_id==DARK_HOLE_ID ||
        RitualEffects_IsCardEffectImmune(i,ultimate_card_effect.source_side))) {
        ultimate_card_effect.touched|=1u<<i;
        return;
    }
    if(i>=0 && ultimate_effect_active(i) &&
       (inside_effect_destruction || gDuel_wEffectCardID==DARK_HOLE_ID) &&
       (gDuel_wEffectCardID==DARK_HOLE_ID ||
        RitualEffects_IsCardEffectImmune(i,D_8009B1D5))) {
        if(ultimate_card_effect.active && (ultimate_card_effect.mask&(1u<<i)))
            ultimate_card_effect.touched|=1u<<i;
        else if(gDuel_wEffectCardID!=DARK_HOLE_ID)
            visual_queue_activation(i);
        /* Dark Hole request 0x11 is already presenting the board wipe.  An
         * immune Ultimate must simply remain in place; queuing its separate
         * activation presentation while 0x11 is blocking can deadlock that
         * request. */
        return;
    }
    if(i>=0 && ultimate_attack_trap_immune(i)) { visual_queue_activation(i); return; }
    if(i>=0 && sengenjin_identity(i) && ritual_card_effect_enabled(SENGENJIN_ID) &&
       ((sengenjin_trap.active && (sengenjin_trap.mask&(1u<<i)) &&
         card->data==sengenjin_trap.saved[i].data) || attack_trap_resolution_active())) {
        if(sengenjin_trap.active)sengenjin_trap.touched|=1u<<i;
        else visual_queue_activation(i);
        return;
    }
    if(tri_identity(i) && (card->flags&DUEL_CARD_FLAG_OCCUPIED))tri_marks[i].battle_flags=card->flags;
    /* Field attack commit temporarily deactivates both records BEFORE
     * switching to battle phase 9. Defer classification until its return.
     * All other deactivations (including Ritual tributes) still clear it. */
    if (inside_field_actions && i >= 0 && marks[i].ritual)
        deferred_deactivation |= 1u << i;
    else if (!(i >= 0 && marks[i].ritual &&
               (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 &&
               battle_participant(i)))
        forget(card);
    if (inside_field_actions && i >= 0 && yamadron_marks[i].ritual)
        deferred_yamadron |= 1u << i;
    else if (!(i >= 0 && yamadron_marks[i].ritual &&
               (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 &&
               battle_participant(i)))
        forget_yamadron(card);
    if (i >= 0 && hungry_marks[i].ritual) {
        if (inside_field_actions) deferred_hungry |= 1u << i;
        else if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 &&
                 battle_participant(i)) {
            /* Native battle staging can deactivate a participant after the
             * field scene has relinquished control. Keep the Ritual instance
             * until state 11 reconstructs the actual survivor. */
        } else {
            hungry_marks[i].ritual = 0; hungry_marks[i].data = 0;
            hungry_marks[i].battle_kills = 0; hungry_live_mask &= ~(1u<<i);
        }
    }
    if (!inside_field_actions && (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9)
        mask_forget(card->data);
    if (i >= 0 && javelin_marks[i].ritual) {
        if (inside_field_actions) deferred_javelin |= 1u << i;
        else if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 &&
                 battle_participant(i)) {
            /* Same temporary native staging window as Hungry Burger. */
        } else {
            memset(&javelin_marks[i], 0, sizeof(javelin_marks[i]));
            javelin_live_mask &= ~(1u<<i);
        }
    }
    if (i >= 0 && gate_marks[i].ritual && !inside_field_actions && (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9) {
        memset(&gate_marks[i], 0, sizeof(gate_marks[i])); gate_live_mask &= ~(1u<<i);
    }
    if(i>=0 && war_lion_marks[i].ritual) {
        if(inside_field_actions) {
            war_lion_marks[i].battle_flags=card->flags;
            deferred_war_lion|=1u<<i;
        } else if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9) {
            memset(&war_lion_marks[i],0,sizeof(war_lion_marks[i]));
            war_lion_live_mask&=~(1u<<i);
        }
    }
    if(i>=0 && sengenjin_marks[i].ritual && (card->flags&DUEL_CARD_FLAG_OCCUPIED))
        sengenjin_marks[i].battle_flags=card->flags;
    if(i>=0 && ultimate_marks[i].ritual && (card->flags&DUEL_CARD_FLAG_OCCUPIED))
        ultimate_marks[i].battle_flags=card->flags;
    if(i>=0 && sengenjin_marks[i].ritual && !inside_field_actions &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9) {
        memset(&sengenjin_marks[i],0,sizeof(sengenjin_marks[i])); sengenjin_live_mask&=~(1u<<i);
    }
    if(i>=0 && bls_marks[i].ritual && !inside_field_actions &&
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9) {
        memset(&bls_marks[i],0,sizeof(bls_marks[i])); bls_live_mask&=~(1u<<i);
    }
    if(i>=0 && !inside_field_actions && (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9) {
        if(ultimate_marks[i].ritual) { memset(&ultimate_marks[i],0,sizeof(ultimate_marks[i])); ultimate_live_mask&=~(1u<<i); }
        if(cosmo_marks[i].ritual) { memset(&cosmo_marks[i],0,sizeof(cosmo_marks[i])); cosmo_live_mask&=~(1u<<i); }
        if(dokurorider_marks[i].ritual) { memset(&dokurorider_marks[i],0,sizeof(dokurorider_marks[i])); dokurorider_live_mask&=~(1u<<i); }
    }
    /* Do not clear Garma here. Retail temporarily deactivates/rebuilds field
     * records during normal duel transitions (including turn changes). Its
     * persistent blade pool is cleared by remove_card or identity validation. */
    original_deactivate(card);
}

static int battle_participant(int index)
{
    int n;
    for (n = 0; n < 2; ++n)
        if (D_800E9EF0[n] && D_800E9EF0[n]->field_6A == index) return 1;
    return 0;
}


/* Crab Turtle uses the same large-card burn presentation as a retail Magic.
 * The normal card-use scene calls Umi twice: handler 0 before the burn and
 * handler 1 after it. Earlier test builds only called handler 0, which is why
 * the SEA terrain never actually committed. This sequence reproduces that
 * two-stage contract without consuming an Umi card from either hand. */
static void crab_umi_close_visual(void)
{
    if (crab_umi_visual_state == 3) func_80029528(2);
    if (crab_umi_visual_state == 4) {
        DisplayObject_ReleaseIfPresent(crab_umi_overlay_add);
        DisplayObject_ReleaseIfPresent(crab_umi_overlay_sub);
    }
    crab_umi_overlay_add = crab_umi_overlay_sub = 0;
    crab_umi_visual_state = 0;
    crab_umi_visual_hold = 0;
}

static void ritual_start_large_card_visual(int card_id)
{
    crab_umi_close_visual();
    ritual_large_visual_card_id = card_id;
    crab_umi_visual_state = 1;
}

static void crab_umi_update_visual(void)
{
    if (crab_umi_visual_state == 1) {
        DuelEffectResourceRecord *card;
        if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134_abs) return;
        card = &D_800EA0E8[2];
        DuelEffect_ClearResourceObjectPointers(2);
        card->src_x = 128; card->src_y = 256; card->field_2C = 0; card->field_2E = 253;
        if (!func_80029164(2, ritual_large_visual_card_id)) return;
        crab_umi_visual_state = 2; return;
    }
    if (crab_umi_visual_state == 2) {
        DisplayObject *object;
        if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134_abs) return;
        object = (DisplayObject *)func_800291E0(2, -1, -1);
        if (!object) { crab_umi_close_visual(); return; }
        object->field_30.h.field_30 = 0x5A; object->field_30.h.field_32 = 0x16;
        object->field_20.b.field_21 = 0; crab_umi_visual_hold = 30; crab_umi_visual_state = 3; return;
    }
    if (crab_umi_visual_state == 3) {
        DisplayObject *card;
        if (--crab_umi_visual_hold > 0) return;
        card = D_800EA0E8[2].object_00;
        if (!card) { crab_umi_close_visual(); return; }
        func_8001944C(card);
        crab_umi_overlay_add = Duel_CreateCardEffectOverlay((DisplayObjectConfigView *)card);
        crab_umi_overlay_sub = Duel_CreateCardEffectOverlay((DisplayObjectConfigView *)card);
        if (!crab_umi_overlay_add || !crab_umi_overlay_sub) { crab_umi_close_visual(); return; }
        crab_umi_overlay_add->attribute = (crab_umi_overlay_add->attribute | GsALON | GsAONE) & ~GsROTOFF;
        DisplayObject_SetDepthOffset(crab_umi_overlay_sub, -1);
        crab_umi_overlay_sub->attribute = (crab_umi_overlay_sub->attribute | GsALON | GsATWO) & ~GsROTOFF;
        func_80029528(2); crab_umi_visual_state = 4; return;
    }
    if (crab_umi_visual_state == 4) {
        int value = crab_umi_overlay_add->field_44.h.field_44 + 0x80;
        int brightness = (int)(crab_umi_overlay_add->field_0C & 0xFF) - 4;
        unsigned rgb;
        if (brightness < 0) brightness = 0;
        crab_umi_overlay_add->field_44.h.field_44 = value; crab_umi_overlay_add->field_44.h.field_46 = value;
        crab_umi_overlay_sub->field_44.h.field_44 = value; crab_umi_overlay_sub->field_44.h.field_46 = value;
        rgb = (unsigned)brightness * 0x010101u;
        crab_umi_overlay_add->field_0C = rgb; crab_umi_overlay_sub->field_0C = rgb;
        if (brightness == 0) crab_umi_close_visual();
    }
}

/* Retail Field ids 330..335 map to terrain codes 1..6. Every automatic
 * Field presentation uses this gate before acquiring any visual resource. */
static int ritual_field_already_active(int card_id)
{
    return card_id>=330 && card_id<=335 && gDuel_bTerrain==card_id-329;
}

static unsigned serpent_sources_for_side_battle(int side, int defender_slot)
{
    unsigned n=0,pending;
    if(!ritual_card_effect_enabled(SERPENT_NIGHT_ID) || side<0 || side>=DUEL_SIDE_COUNT)return 0;
    pending=serpent_live_mask & side_monster_mask(side);
    while(pending) {
        int i=take_mask_bit(&pending); DuelCardRecord *c=&D_801A7AD8[i]; int live;
        if(!serpent_marks[i].ritual || c->card_id!=SERPENT_NIGHT_ID || c->data!=serpent_marks[i].data)continue;
        /* FieldActions deactivates both battle records before phase 9.
         * A defending Ritual Serpent is still a valid Fear source itself;
         * every other Serpent source must still be live on the field. */
        live=(c->flags&DUEL_CARD_FLAG_OCCUPIED)!=0;
        if(live || i==defender_slot)++n;
    }
    return n;
}

#include "ritual_ai_burn_policy.h"
#include "serpent_fear.h"

/* Optional and passive: snapshots an actual CPU monster placed face-up in
 * Attack while a stronger opposing monster is visible. The snapshot remains
 * on-screen until the next event, allowing a single screenshot afterwards.
 * It neither edits AI selection nor modifies card stats. */
static void ai_position_diagnostic_update(void)
{
    int side,k,i;
    if(!diagnostic_ai_positions)return;
    for(side=0;side<DUEL_SIDE_COUNT;++side)for(k=0;k<DUEL_FIELD_ROW_SIZE;++k) {
        int base=side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+10;
        DuelCardRecord *c;
        i=D_800907D8[base+k];
        if(i<0 || i>=DUEL_CARD_RECORD_COUNT)continue;
        c=&D_801A7AD8[i];
        if((c->flags&DUEL_CARD_FLAG_OCCUPIED) && c->data) {
            int is_new=ai_position_diag.data[i]!=c->data || ai_position_diag.id[i]!=c->card_id;
            ai_position_diag.data[i]=c->data;
            ai_position_diag.id[i]=c->card_id;
            if(is_new && D_8009B360[side]>=0 && D_8009B1D5==side &&
               !(c->flags&DUEL_CARD_FLAG_DEFENSE_POSITION) &&
               (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9) {
                int j,strongest=-1,best_id=0,def=0;
                unsigned packed=(unsigned)Duel_CalcCardStats(c);
                for(j=0;j<DUEL_FIELD_ROW_SIZE;++j) {
                    int other=D_800907D8[(side^1)*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+10+j];
                    DuelCardRecord *e;
                    int attack;
                    if(other<0 || other>=DUEL_CARD_RECORD_COUNT)continue;
                    e=&D_801A7AD8[other];
                    if(!(e->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
                    attack=(int)((unsigned)Duel_CalcCardStats(e)&0xffffu);
                    if(attack>strongest) {strongest=attack;best_id=e->card_id;def=(int)((unsigned)Duel_CalcCardStats(e)>>16);}
                }
                if(strongest>(int)(packed&0xffffu)+100) {
                    ai_position_diag.valid=1;
                    ++ai_position_diag.count;
                    ai_position_diag.own_id=c->card_id;
                    ai_position_diag.own_atk=packed&0xffffu;
                    ai_position_diag.own_def=packed>>16;
                    ai_position_diag.opp_id=best_id;
                    ai_position_diag.opp_atk=strongest;
                    ai_position_diag.opp_def=def;
                    ai_position_diag.phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
                    ai_position_diag.turn=side;
                }
            }
        } else {ai_position_diag.data[i]=0;ai_position_diag.id[i]=0;}
    }
}

static void scene_update(void)
{
    borrowed_reconcile();
    original_scene_update();
    performance_release_rewards();
    if(diagnostic_ai_positions)ai_position_diagnostic_update();
    /* A Ritual Chaos with its effect active cannot remain in Defense,
     * including positions set indirectly by CPU or other effects.  Never
     * touch non-Ritual copies, or copies whose effect setting is OFF. */
    chaos_enforce_attack_position();
    /* The bottom field-info text is cached while the card-frame stats are
     * live. Refresh it only when the selected card's effective stats differ. */
    refresh_field_card_info();
    /* DuelScene_Update runs every duel frame. In the overwhelmingly common
     * case no Ritual-owned scene sequence is active, return after the retail
     * update instead of walking the rest of this dispatcher. */
    if(!chaos_turn.active && !serpent_ghost_state && !serpent_redirect_state &&
       !crab_umi_visual_state && !crab_umi_sequence && !tri_field_pending && !bls_field_pending && !zera_field_pending)return;
    if(chaos_turn.active)chaos_sequence();
    if(serpent_ghost_state||serpent_redirect_state)serpent_ghost_update();
    if(crab_umi_visual_state)crab_umi_update_visual();
    if(!crab_umi_sequence && tri_field_pending && !(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        tri_field_pending=0;
        if(ritual_card_effect_enabled(TRI_HORNED_ID)) {ritual_field_id=WASTELAND_ID;ritual_field_source_card_id=TRI_HORNED_ID;crab_umi_sequence=1;}
    }
    if(!crab_umi_sequence && bls_field_pending && !(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        bls_field_pending=0;
        if(ritual_card_effect_enabled(BLACK_LUSTER_SOLDIER_ID)) {ritual_field_id=SOGEN_ID;ritual_field_source_card_id=BLACK_LUSTER_SOLDIER_ID;crab_umi_sequence=1;}
    }
    if(!crab_umi_sequence && zera_field_pending && !(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        zera_field_pending=0;
        if(ritual_card_effect_enabled(ZERA_ID)) {
            ritual_field_id=YAMI_ID;ritual_field_source_card_id=ZERA_ID;crab_umi_sequence=1;
        }
    }
    if (!crab_umi_sequence || !ritual_card_effect_enabled(ritual_field_source_card_id)) return;
    /* Wait until Ritual has completely released the card-effect dispatcher. */
    if (crab_umi_sequence == 1 && !(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        crab_umi_pending = 0;
        if(ritual_field_already_active(ritual_field_id)) {
            crab_umi_sequence=0;
            return;
        }
        DuelEffect_StartCardEffect(ritual_field_id, 0);
        crab_umi_sequence = 2;
        crab_umi_camera_delay = 24;
        if (host && host->log) host->log(host, "[Ritual Card Effects] Field automatico: pre-effect iniciado");
        return;
    }
    if (crab_umi_sequence == 2 && !(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        if (crab_umi_camera_delay > 0) { --crab_umi_camera_delay; return; }
        ritual_start_large_card_visual(ritual_field_id); crab_umi_sequence = 3;
        if (host && host->log) host->log(host, "[Ritual Card Effects] Field automatico: apresentacao nativa iniciada");
        return;
    }
    if (crab_umi_sequence == 3 && !crab_umi_visual_state && !(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        DuelEffect_StartCardEffect(ritual_field_id, 1);
        crab_umi_sequence = 4;
        if (host && host->log) host->log(host, "[Ritual Card Effects] Field automatico: post-effect iniciado");
        return;
    }
    if (crab_umi_sequence == 4 && !(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        crab_umi_sequence = 0;
        if (host && host->log) host->log(host, "[Ritual Card Effects] Field automatico concluido; terrain=%d", (int)gDuel_bTerrain);
    }
}

/* Spell/trap field destruction does not necessarily enter Battle(). If a
 * marionette was removed while its Puppet survived, clear the link before
 * the next turn starts so the Puppet may steal again. Never treat retail's
 * temporary battle deactivation as destruction. */
static void psycho_reconcile_controlled(void)
{
    unsigned pending=psycho_controlling_mask;
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9)return;
    while(pending) {
        int i=take_mask_bit(&pending);
        PsychoMark *m=&psycho_marks[i];
        if(!m->ritual || !m->controlling || psycho_stolen_slot(m)>=0)continue;
        m->controlling=0;
        psycho_controlling_mask&=~(1u<<i);
        if(psycho_controlling_count)--psycho_controlling_count;
        m->stolen_data=0;m->stolen_card_id=0;
        m->original_slot=m->current_slot=-1;
    }
}

static void psycho_reconcile(void)
{
    unsigned pending=psycho_mark_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        PsychoMark *m=&psycho_marks[i];
        if(!m->ritual) { psycho_mark_mask&=~(1u<<i); continue; }
        if(m->controlling || m->pending_summon_trigger) continue;
        { int slot=psycho_source_slot(m);
          if(slot>=0) continue; }
        memset(m,0,sizeof(*m));
        psycho_mark_mask &= ~(1u<<i);
    }
}

static void mirror_reconcile_live(void)
{
    unsigned pending=fiend_mirror_live_mask;
    if(!fiend_mirror_live_hint)return;
    while(pending) {
        int i=take_mask_bit(&pending); DuelCardRecord *c=&D_801A7AD8[i];
        if(fiend_mirror_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           c->card_id==FIENDS_MIRROR_ID && c->data==fiend_mirror_marks[i].data)continue;
        /* FieldActions deactivates attacker/defender records before phase 9.
         * A Ritual Mirror that entered that battle must keep its provenance
         * until the battle controller can snapshot mirror_battle_grace; if we
         * erase the mark here, a Mirror destroyed by that same battle cannot
         * reflect the resulting LP loss.  Only preserve the exact staged
         * participant, and retire it normally as soon as phase 9 is over. */
        if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
           fiend_mirror_marks[i].ritual &&
           c->card_id==FIENDS_MIRROR_ID && c->data &&
           c->data==fiend_mirror_marks[i].data &&
           battle_participant(i))continue;
        fiend_mirror_marks[i].ritual=0; fiend_mirror_marks[i].data=0;
        fiend_mirror_live_mask&=~(1u<<i);
    }
    fiend_mirror_live_hint=fiend_mirror_live_mask!=0;
}


static void war_lion_reconcile_bonus_visuals(void)
{
    unsigned pending=war_lion_live_mask;
    if(!(stat_effect_hints&STAT_HINT_WAR_LION))return;
    while(pending) {
        int i=take_mask_bit(&pending);
        if(war_lion_live(i)) {
            WarLionMark *m=&war_lion_marks[i];
            unsigned now=beacon_other_allied_type_hand_field(side_for_record(i),CARD_TYPE_BEAST,i);
            if(now>m->beast_count && ritual_card_effect_enabled(SUPER_WAR_LION_ID)) {
                visual_queue_activation(i);
                visual_queue_stat(i,(int)((now-m->beast_count)*WAR_LION_BEAST_BONUS));
            }
            m->beast_count=now;
        }
    }
}

static void bls_reconcile_aura_visuals(void)
{
    int i,side;
    unsigned new_sources=bls_new_source_pending;
    unsigned sources[DUEL_SIDE_COUNT]={0},source_counts[DUEL_SIDE_COUNT]={0};
    if(!bls_live_hint || !(stat_effect_hints&STAT_HINT_BLS)) {
        if(!bls_live_mask) {
            memset(bls_prev_sources,0,sizeof(bls_prev_sources));
            memset(bls_prev_data,0,sizeof(bls_prev_data));
            memset(bls_prev_card_id,0,sizeof(bls_prev_card_id));
        }
        bls_new_source_pending=0;
        return;
    }
    /* A just-summoned BLS owns the activation presentation. Warriors that
     * enter later receive only their stat popup, never another BLS flash. */
    while(new_sources) {
        int src=take_mask_bit(&new_sources);
        if(bls_live(src)) visual_queue_activation(src);
    }
    /* The queue only records presentation work, so the live field remains
     * stable throughout this reconciliation. Validate each source once per
     * side, then exclude the exact recipient record from its local count. */
    for(side=0;side<DUEL_SIDE_COUNT;++side) {
        unsigned pending=bls_live_mask & side_monster_mask(side);
        while(pending) {
            int src=take_mask_bit(&pending);
            if(bls_live(src)) { sources[side]|=1u<<src; ++source_counts[side]; }
        }
    }
    /* Hotpass: only the 10 Monster Zones can receive this aura.  The
     * remaining 20 cached entries never hold sources and stay zeroed at
     * initialization/reset, so do not scan/clear them each field tick. */
    for(side=0;side<DUEL_SIDE_COUNT;++side)
    for(i=side*DUEL_CARD_SIDE_RECORD_COUNT+HAND_SIZE;
        i<side*DUEL_CARD_SIDE_RECORD_COUNT+HAND_SIZE+DUEL_FIELD_ROW_SIZE;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        unsigned now, old;
        if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED) ||
           card_type_id(c)!=CARD_TYPE_WARRIOR) {
            bls_prev_sources[i]=0; bls_prev_data[i]=0; bls_prev_card_id[i]=0; continue;
        }
        now=source_counts[side]-((sources[side]&(1u<<i))?1u:0u);
        old=(bls_prev_data[i]==c->data && bls_prev_card_id[i]==c->card_id)?bls_prev_sources[i]:0;
        if(now>old)
            visual_queue_stat(i,(int)((now-old)*BLS_WARRIOR_AURA_BONUS));
        bls_prev_sources[i]=(u8)now;
        bls_prev_data[i]=c->data;
        bls_prev_card_id[i]=c->card_id;
    }
    bls_new_source_pending=0;
}

static void cosmo_reconcile_bonus_visuals(void)
{
    unsigned pending=cosmo_live_mask;
    if(!cosmo_live_hint || !(stat_effect_hints&STAT_HINT_COSMO))return;
    while(pending) {
        int i=take_mask_bit(&pending);
        if(cosmo_live(i)) {
            CosmoMark *m=&cosmo_marks[i];
            unsigned now=female_field_count_except(i)+1u;
            if(now>m->female_count) {
                visual_queue_activation(i);
                visual_queue_stat(i,(int)((now-m->female_count)*COSMO_FEMALE_BONUS));
            }
            /* Losses are intentionally silent, but the cached count follows
             * every Female entering/leaving so the next gain delta is exact. */
            m->female_count=now;
        }
    }
}

static void skull_reconcile_bonus_visuals(void)
{
    unsigned pending=skull_live_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        unsigned now,old;
        if(!skull_live(i) || !ritual_card_effect_enabled(SKULL_GUARDIAN_ID))continue;
        now=skull_bonus_for(i);old=skull_marks[i].last_bonus;
        /* Do not announce the same amount repeatedly or spam on decreases.
         * Retry a positive delta when the shared queue is momentarily full. */
        if(now>old && !visual_queue_stat(i,(int)(now-old)))continue;
        skull_marks[i].last_bonus=now;
    }
}

static void dokurorider_reconcile_aura_visuals(void)
{
    int i,side;
    unsigned new_sources=dokurorider_new_source_pending;
    unsigned sources[DUEL_SIDE_COUNT]={0},source_counts[DUEL_SIDE_COUNT]={0};
    if(!dokurorider_live_hint || !(stat_effect_hints&STAT_HINT_DOKU)) {
        if(!dokurorider_live_mask) {
            memset(dokurorider_prev_sources,0,sizeof(dokurorider_prev_sources));
            memset(dokurorider_prev_data,0,sizeof(dokurorider_prev_data));
            memset(dokurorider_prev_card_id,0,sizeof(dokurorider_prev_card_id));
        }
        dokurorider_new_source_pending=0;
        return;
    }
    while(new_sources) {
        int src=take_mask_bit(&new_sources);
        if(dokurorider_live(src))visual_queue_activation(src);
    }
    /* Match the stat hook's staged-participant rule for Dokurorider. This
     * snapshot belongs only to this synchronous call; never reuse it after
     * a duel action or across frames. */
    for(side=0;side<DUEL_SIDE_COUNT;++side) {
        unsigned pending=dokurorider_live_mask & side_monster_mask(side);
        while(pending) {
            int src=take_mask_bit(&pending);
            if(dokurorider_identity(src) && ritual_effectively_present(src)) {
                sources[side]|=1u<<src; ++source_counts[side];
            }
        }
    }
    /* Hotpass: only the 10 Monster Zones can receive this aura.  The
     * remaining 20 cached entries never hold sources and stay zeroed at
     * initialization/reset, so do not scan/clear them each field tick. */
    for(side=0;side<DUEL_SIDE_COUNT;++side)
    for(i=side*DUEL_CARD_SIDE_RECORD_COUNT+HAND_SIZE;
        i<side*DUEL_CARD_SIDE_RECORD_COUNT+HAND_SIZE+DUEL_FIELD_ROW_SIZE;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        unsigned now,old;
        if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED) ||
           card_type_id(c)!=CARD_TYPE_ZOMBIE) {
            dokurorider_prev_sources[i]=0;dokurorider_prev_data[i]=0;dokurorider_prev_card_id[i]=0;continue;
        }
        now=source_counts[side]-((sources[side]&(1u<<i))?1u:0u);
        old=(dokurorider_prev_data[i]==c->data && dokurorider_prev_card_id[i]==c->card_id)?dokurorider_prev_sources[i]:0;
        if(now>old)visual_queue_stat(i,(int)((now-old)*DOKURORIDER_AURA_ATK));
        dokurorider_prev_sources[i]=(u8)now;
        dokurorider_prev_data[i]=c->data;
        dokurorider_prev_card_id[i]=c->card_id;
    }
    dokurorider_new_source_pending=0;
}

static int ultimate_selected_attacker(void)
{
    DuelSelectionSide *sides;
    DuelSelectionRecord *sel;
    int side,col,index;
    side=D_8009B1D5;
    if(side<0 || side>=DUEL_SIDE_COUNT || !ultimate_live_hint)return -1;
    sides=(DuelSelectionSide*)D_800E9F10;
    sel=&sides[side].records[2];
    if(sel->row!=2 || sel->col<0 || sel->col>=DUEL_FIELD_ROW_SIZE)return -1;
    col=sel->col;
    index=D_800907D8[side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+10+col];
    return ultimate_effect_active(index)?index:-1;
}

static unsigned ultimate_snapshot_enemy_field(DuelCardRecord saved[DUEL_CARD_RECORD_COUNT])
{
    unsigned pending,mask=0;
    int source=D_8009B1D5;
    if(source<0 || source>=DUEL_SIDE_COUNT || !ultimate_live_hint)return 0;
    pending=ultimate_live_mask & side_monster_mask(source^1);
    while(pending) {
        int i=take_mask_bit(&pending);
        if(RitualEffects_IsCardEffectImmune(i,source) && ultimate_live(i)) {
            saved[i]=D_801A7AD8[i];mask|=1u<<i;
        }
    }
    return mask;
}

static void ultimate_restore_external_field_changes(unsigned mask,DuelCardRecord saved[DUEL_CARD_RECORD_COUNT])
{
    unsigned pending=mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        DuelCardRecord *c=&D_801A7AD8[i],*old=&saved[i];
        if(c->data!=old->data || c->card_id!=old->card_id || c->attack!=old->attack ||
           c->defense!=old->defense || c->stat_modifier!=old->stat_modifier ||
           c->terrain_modifier!=old->terrain_modifier ||
           ((c->flags^old->flags)&~DUEL_CARD_FLAG_USED_THIS_TURN) ||
           !(c->flags&DUEL_CARD_FLAG_OCCUPIED)) {
            ultimate_restore_snapshot_record(i,old,1);
            visual_queue_activation(i);
            if(host&&host->log)host->log(host,"[Ritual Card Effects] Blue-Eyes Ultimate ignorou alteracao hostil de Monster Effect: slot %d",i);
        }
    }
}

/* Chakra is a monster effect, but its presentation reuses the exact retail
 * Raigeki lightning request (0x10), and each stroke's retail destruction
 * request (0x0B).  It does NOT start the Raigeki Magic card, so no spell
 * dispatch or unrelated card-effect rule can change its target selection. */
static int chakra_any_shatter_busy(void)
{
    int r;
    for(r=0;r<DUEL_EFFECT_REQUEST_COUNT;++r)
        if((D_800EAD88[r].flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE) && D_800EAD88[r].id==0xB)
            return 1;
    return 0;
}
static int chakra_instance_live(int index)
{
    DuelCardRecord *c;
    if(index<0 || index>=DUEL_CARD_RECORD_COUNT || !monster_slot(index))return 0;
    c=&D_801A7AD8[index];
    return chakra_marks[index].ritual && chakra_marks[index].data==c->data &&
           c->card_id==CHAKRA_ID && c->object && (c->flags&DUEL_CARD_FLAG_OCCUPIED);
}
/* The regular field scene advances func_80022D94 through func_800235C0 ONLY
 * while D_8009B162 is nonzero. The earlier cinematic attempt supplied the
 * tween but not that scheduling flag; waiting for it could freeze the turn.
 * Use 0x8000 (already-initialized, no cursor/panel operations) to let the
 * native scene advance our camera-only request. Do not replace its update. */
#define CHAKRA_CAMERA_WAIT_LIMIT 180
#define CHAKRA_CAMERA_FRAMES 16

static void chakra_start_camera(int x,int z,int angle,int value)
{
    func_80022D94(CHAKRA_CAMERA_FRAMES,x,z,angle,value);
    D_8009B162=0x8000;
}

/* Only the Chakra-owned camera tween can be cancelled. A missing Chakra or
 * a stalled tween must never leave the field cursor stuck at the wrong view. */
static void chakra_abort_camera(ChakraSequence *s)
{
    if(!s->camera_active)return;
    if(D_8009B162==0x8000) {
        D_8009B162=0;
        D_8009B204=0;
    }
    D_800F2848.field_00=(s16)s->cam_x;
    D_800F2848.field_04=(s16)s->cam_z;
    D_800F2848.angle=(s16)s->cam_angle;
    D_800F2848.view.vrz=s->cam_value;
    ViewState_ApplyOrbit();
    s->camera_active=0;
}

/* Stages: 0 await an idle camera, 1 pan to the enemy field,
 * 6 play the FULL retail Raigeki lightning request and synchronized shatters,
 * 2 fail-safe remaining shatters if the VFX request ends prematurely,
 * 3 await native shatters and initiate return, 4 finish camera return,
 * 5 queue the one green popup. */
static void chakra_begin(int index)
{
    int side=side_for_record(index),enemy=side^1,k,j;
    ChakraSequence *s=&chakra_sequence;
    memset(s,0,sizeof(*s));
    s->active=1;s->index=index;s->data=chakra_marks[index].data;s->side=side;
    for(k=0;k<DUEL_FIELD_ROW_SIZE;++k)
        s->targets[k]=D_800907D8[enemy*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+10+k];
    /* Physical X, not card-record order (which is reversed for side 1). */
    for(k=0;k<DUEL_FIELD_ROW_SIZE;++k)for(j=k+1;j<DUEL_FIELD_ROW_SIZE;++j) {
        int ak=s->targets[k],aj=s->targets[j];
        int ik=-1,ij=-1,t;
        int xk,xj;
        for(t=0;t<DUEL_FIELD_ROW_SIZE;++t) {
            if(D_800907D8[enemy*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+10+t]==ak)ik=t;
            if(D_800907D8[enemy*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+10+t]==aj)ij=t;
        }
        xk=D_80090800[enemy][10+ik].x;
        xj=D_80090800[enemy][10+ij].x;
        if(xj<xk) {u8 temp=s->targets[k];s->targets[k]=s->targets[j];s->targets[j]=temp;}
    }
    s->count=DUEL_FIELD_ROW_SIZE;
    if(host&&host->log)host->log(host,"[Ritual Card Effects] Chakra: comecou efeito de monstro, lado=%d index=%d",side,index);
}
/* A single strike's *logical* removal remains Chakra-owned. Raigeki's
 * lightning drives its frame, but a protected Ultimate or other surviving
 * immune card cannot contribute ATK or receive a false shatter animation. */
static void chakra_strike_target(ChakraSequence *s,int index,int strike)
{
    int atk,x,y,z,variant;
    void *old_data;
    s16 old_id;
    DuelCardRecord *c=&D_801A7AD8[index];
    DisplayObject *o;
    DuelEffectRequest *r;
    if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED)||!c->object||!c->data)return;
    if(ultimate_effect_active(index))return;
    atk=(int)((unsigned)Duel_CalcCardStats(c)&0xffffu);
    old_data=c->data;old_id=c->card_id;
    o=(DisplayObject *)c->object;
    x=o->field_30.h.field_30;y=o->field_30.h.field_32;
    z=o->field_34.h.field_34;
    variant=Duel_GetCardEffectVariant((CardObject *)o);
    DuelCard_RemoveFromField(c);
    if((c->flags&DUEL_CARD_FLAG_OCCUPIED) && c->data==old_data && c->card_id==old_id)
        return;
    s->total_atk+=(unsigned)atk;
    ++s->destroyed;
    /* Retail Raigeki allocates the shatter directly and uses separate
     * 0x3000-byte sub-buffers by strike; recreating that contract avoids
     * corrupting overlapping Raigeki/0x0B visual sprites. */
    r=(DuelEffectRequest *)DuelEffect_AllocateRequest(0xB);
    if(r) {
        r->buffer=(u8 *)r->buffer+(unsigned)strike*0x3000u;
        r->field_00=(s16)x;r->field_02=(s16)y;r->field_04=(s16)z;
        r->field_1A=(s16)variant;
        SD_SEPlayFull(0x1F);
    }
    if(host&&host->log)
        host->log(host,"[Ritual Card Effects] Chakra: raio=%d destruiu slot=%d ATK=%d total=%u",strike,index,atk,s->total_atk);
}

/* DuelEffect_UpdateRequests is called every frame, even while native VFX is
 * blocking field actions. Observe Raigeki's real lightning-step milestones
 * HERE, rather than inventing a separate timer in Chakra's field callback.
 * This keeps each shatter and sound on retail Raigeki's original frame. */
#define CHAKRA_LIGHTNING_WAIT_LIMIT 420
static void chakra_advance_lightning(void)
{
    ChakraSequence *s=&chakra_sequence;
    DuelEffectRequest *r;
    int pulse;
    if(!s->active || s->stage!=6 || !s->lightning)return;
    r=s->lightning;
    if(!chakra_instance_live(s->index) || chakra_marks[s->index].data!=s->data) {
        if(r->id==0x10 && (r->flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE))r->flags=0;
        s->lightning=0;
        return;
    }
    /* Process milestones before checking the active flag: the final frame
     * can advance field_1D and clear the request in the SAME native update. */
    if(r->id==0x10) {
        pulse=(int)r->field_1D;
        if(pulse>s->count)pulse=s->count;
        while(s->next<pulse) {
            SD_SEPlayFull(0x15); /* The original Raigeki lightning-step sound. */
            chakra_strike_target(s,s->targets[s->next],s->next+1);
            ++s->next;
        }
    }
    if(r->id!=0x10 || !(r->flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE)) {
        s->lightning=0;
        s->watchdog=0;
        s->stage=s->next<s->count?2:3;
        return;
    }
    if(++s->watchdog>CHAKRA_LIGHTNING_WAIT_LIMIT) {
        if(host&&host->log)host->log(host,"[Ritual Card Effects] Chakra: timeout do Raigeki nativo; liberando pedido e preservando o turno");
        r->flags=0;
        s->lightning=0;
        s->watchdog=0;
        s->stage=s->next<s->count?2:3;
    }
}

static int chakra_tick(void)
{
    ChakraSequence *s=&chakra_sequence;
    if(!s->active) {
        unsigned pending=chakra_pending_mask;
        if(!pending || (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) ||
           visual_activation_busy())return 0;
        while(pending) {
            int i=take_mask_bit(&pending);
            chakra_pending_mask&=~(1u<<i);
            if(chakra_instance_live(i) && ritual_card_effect_enabled(CHAKRA_ID)){
                chakra_begin(i);break;
            }
        }
        if(!s->active)return 0;
    }
    if(!chakra_instance_live(s->index) || chakra_marks[s->index].data!=s->data) {
        if(s->lightning && s->lightning->id==0x10 &&
           (s->lightning->flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE))
            s->lightning->flags=0;
        chakra_abort_camera(s);
        memset(s,0,sizeof(*s));return 0;
    }
    if(s->stage==0) {
        /* Even with an empty or fully immune opposing board, Raigeki plays
         * its complete lightning sweep. Keep that Vanilla presentation; only
         * the destruction/ATK accounting skips invalid or immune targets. */
        if(D_8009B162 || D_8009B204) {
            if(++s->timeout>CHAKRA_CAMERA_WAIT_LIMIT) {
                if(host&&host->log)host->log(host,"[Ritual Card Effects] Chakra: camera inicial ocupada; segue destruição sem pan");
                s->stage=2;
            } else return 1;
        } else {
            s->cam_x=D_800F2848.field_00;
            s->cam_z=D_800F2848.field_04;
            s->cam_angle=D_800F2848.angle;
            s->cam_value=D_800F2848.view.vrz;
            s->camera_active=1;
            s->timeout=0;
            s->hold=8;
            /* Target the opposing board, not the neutral centre of the mat.
             * Side 0 faces the +Z rows; side 1 faces the -Z rows. Both the
             * lightning sweep and the exact saved-camera return are unchanged. */
            chakra_start_camera(0x218,0xE8,D_8009AF20[s->side],
                                s->side == 0 ? 104 : -104);
            s->stage=1;
            return 1;
        }
    }
    if(s->stage==1) {
        if(D_8009B162 || D_8009B204) {
            if(++s->timeout>CHAKRA_CAMERA_WAIT_LIMIT) {
                if(host&&host->log)host->log(host,"[Ritual Card Effects] Chakra: camera de destruição excedeu espera; recuperação segura");
                chakra_abort_camera(s);
                s->stage=2;
            } else return 1;
        } else if(s->hold>0) {--s->hold;return 1;}
        else s->stage=2;
    }
    if(s->stage==2 && !s->lightning_started && s->next==0 && !s->lightning && s->camera_active) {
        /* Match Raigeki's native start: its 0x10 request holds the lightning
         * timeline and reports field_1D=1..5 as each stroke reaches a slot.
         * No need to simulate timing, particles, flashes or sound. */
        DuelEffectRequest *r=(DuelEffectRequest *)DuelEffect_AllocateRequest(0x10);
        if(r) {
            DuelFieldPosition *pos=&D_80090800[s->side][DUEL_FIELD_ROW_SIZE];
            r->field_00=pos->x;
            r->field_02=0;
            r->field_04=pos->y;
            s->lightning=r;
            s->lightning_started=1;
            s->watchdog=0;
            SD_SEPlayFull(0x15);
            s->stage=6;
            return 1;
        }
        /* Leave a full request pool alone, then retry; eventually fall back
         * to the proven safe shatter path instead of freezing the duel. */
        if(++s->timeout<CHAKRA_CAMERA_WAIT_LIMIT)return 1;
        if(host&&host->log)host->log(host,"[Ritual Card Effects] Chakra: sem slot 0x10; retorno seguro ao caminho sem raio");
    }
    if(s->stage==6)return 1; /* Progress comes from request-update hook. */
    if(s->stage==2) {
        /* Rare fallback: native lightning request was interrupted. Never
         * lose card removals or freeze the field if another MOD consumes it. */
        if(chakra_any_shatter_busy())return 1;
        while(s->next<s->count) {
            int step=s->next++;
            chakra_strike_target(s,s->targets[step],step+1);
            return 1;
        }
        s->stage=3;
    }
    if(s->stage==3) {
        /* Native Raigeki starts several 0x0B shatters before its lightning
         * finishes; do not pan back while any one of them is still visible. */
        if(chakra_any_shatter_busy())return 1;
        /* The last shatter request has finished; restore the precise angle,
         * zoom and target that the field camera held before this effect. */
        if(s->camera_active) {
            if(D_8009B162 || D_8009B204) {
                if(++s->timeout<=CHAKRA_CAMERA_WAIT_LIMIT)return 1;
                chakra_abort_camera(s);
            } else {
                s->timeout=0;
                chakra_start_camera(s->cam_x,s->cam_z,s->cam_angle,s->cam_value);
                s->stage=4;
                return 1;
            }
        }
        s->stage=5;
    }
    if(s->stage==4) {
        if(D_8009B162 || D_8009B204) {
            if(++s->timeout<=CHAKRA_CAMERA_WAIT_LIMIT)return 1;
            if(host&&host->log)host->log(host,"[Ritual Card Effects] Chakra: retorno da camera excedeu espera; recuperação segura");
            chakra_abort_camera(s);
        }
        s->camera_active=0;
        s->stage=5;
    }
    /* Final integer's ones digit 0..5 -> down, 6..9 -> up.  This also
     * handles the .5 from odd totals without rounding the half first. */
    {
        unsigned half=s->total_atk/2u;
        unsigned bonus=(half/10u+(half%10u>=6u))*10u;
        chakra_marks[s->index].bonus=bonus;
        if(bonus>0u)visual_queue_stat_kind(s->index,(int)bonus,STAT_VISUAL_CHAKRA);
        if(host&&host->log)host->log(host,"[Ritual Card Effects] Chakra: %u destruidos, total=%u, buff ATK=+%u",s->destroyed,s->total_atk,bonus);
    }
    memset(s,0,sizeof(*s));
    return 1;
}

static int field_actions_hook_needed(void)
{
    return coin_marks_seen || coin_battle.active || ritual_coin.active || coin_battle.dodge_ticks ||
        serpent_seen || serpent_ghost_state || serpent_redirect_state || serpent_fear_battle_slot>=0 ||
        mask_instance_count || generated_thumbnail_valid || yamadron_live_mask || psycho_mark_mask ||
        stat_effect_hints || garma_live_hint || tri_live_hint || gate_live_hint ||
        fiend_mirror_live_hint || mirror_queue_count || mirror_reflection.active ||
        visual_activation_count || visual_stat_count || visual_stat_cooldown || visual_popup_frames ||
        hungry_live_mask || hungry_visual_pending_mask || garma_pending_mask || garma_visual_pending || garma_relevant_frames>0 ||
        javelin_protected_visuals || tri_battle_visuals ||
        war_lion_live_mask || deferred_war_lion ||
        sengenjin_live_hint || sengenjin_trap.active || bls_live_hint ||
        bls_new_source_pending || bls_field_pending || ultimate_live_hint ||
        cosmo_live_hint || dokurorider_live_hint || dokurorider_new_source_pending ||
        chakra_pending_mask || chakra_sequence.active;
}

/* Automatic terrain caused by Ritual monsters resolves asynchronously after
 * the Ritual's own card effect. The duel scene may return to the CPU's field
 * phase before the pre-effect/card presentation/post-effect has finished.
 * Never advance the field action state machine during this interval: the AI
 * must attack only after the terrain and cached stats have settled.
 * DuelScene_Update still runs and advances the effect dispatcher and this
 * module's scene_update; only actual field actions are held here.
 * This covers Crab Turtle/UMI, Tri-Horned/Wasteland, BLS/Sogen and Zera/Yami,
 * for both sides without changing the manual/normal Field Spell flow. */
static int ritual_auto_field_busy(void)
{
    if (tri_field_pending && ritual_card_effect_enabled(TRI_HORNED_ID)) return 1;
    if (bls_field_pending && ritual_card_effect_enabled(BLACK_LUSTER_SOLDIER_ID)) return 1;
    if (zera_field_pending && ritual_card_effect_enabled(ZERA_ID)) return 1;
    if (crab_umi_sequence && ritual_card_effect_enabled(ritual_field_source_card_id)) return 1;
    return 0;
}

static void field_actions(void)
{
    borrowed_reconcile();
    limit_test_arm_field_effects();
    /* Before ANY original field tick, including its fast path, so a legal
     * attack cannot be chosen or committed in the terrain transition. */
    if(ritual_auto_field_busy())return;
    if(chaos_turn.active)return;
    /* Let the field scene initialise its native cursor/panels before Chakra
     * takes over. The first retail field tick cannot play an attack. */
    if(chakra_pending_mask && !chakra_sequence.active &&
       !(gDuel_wSceneStateFlags&0x8000)) { original_field_actions(); return; }
    if(chakra_tick())return;
    /* Mandatory Yamadron generated-Magic law. Apply *before* any normal
     * CPU planner runs, including Ritual Duelists, AI Hard and retail. */
    yamadron_ai_force_next_burn();
    if(!field_actions_hook_needed()){original_field_actions();return;}
    if(coin_battle.active){coin_restore_dodge();memset(&coin_battle,0,sizeof(coin_battle));}
    coin_reconcile();
    chaos_enforce_attack_position();
    if(serpent_fear_battle_slot>=0){ serpent_fear_bonus[serpent_fear_battle_slot]=0; serpent_fear_battle_slot=-1; }
    serpent_reconcile();
    mask_reconcile();
    restore_generated_field_thumbnails();
    int i;
    DuelCardRecord ultimate_field_saved[DUEL_CARD_RECORD_COUNT];
    unsigned ultimate_field_guard=0;

    /* FieldActions can deactivate a Shield before our post-call bookkeeping.
     * Snapshot the ten Monster Zones once instead of writing a 30-entry count
     * table every field tick. The bit remembers whether each Shield itself was
     * one of the attackers in that exact pre-call state. */
    unsigned attack_mask=0, attack_total=(stat_effect_hints & STAT_HINT_SHIELD) ? snapshot_attack_monsters(&attack_mask) : 0;
    deferred_deactivation = 0;
    deferred_yamadron = 0;
    deferred_hungry = 0;
    deferred_javelin = 0;
    deferred_war_lion = 0;
    /* Psycho-Puppet may change position, but while it controls a borrowed
     * monster it must never become an eligible attacker again on later turns.
     * Reassert the retail used-this-turn attack lock every field tick. */
    if(psycho_controlling_mask) {
        unsigned pending=psycho_controlling_mask;
        while(pending) {
            i=take_mask_bit(&pending);
            if (psycho_marks[i].ritual && psycho_marks[i].controlling) {
                int ps = psycho_source_slot(&psycho_marks[i]);
                if (ps >= 0)
                    D_801A7AD8[ps].flags |= DUEL_CARD_FLAG_USED_THIS_TURN;
            }
        }
    }
    /* Snapshot only the opponent's live Ritual Ultimate around the chained
     * FieldActions call. This catches monster-effect adapters such as Time
     * Wizard that mutate a record in place, while never blocking its owner's
     * normal actions. Battle staging (phase 9) is intentionally not restored. */
    if(ultimate_live_hint)ultimate_field_guard=ultimate_snapshot_enemy_field(ultimate_field_saved);
    /* Player-side execution gate only: retail checks Swords directly inside
     * FieldActions before committing a manually selected attacker.  Do not
     * expose a fake unpinned state to any AI/planner.  For the one exact frame
     * in which the player presses attack with an effect-active Ritual Ultimate,
     * lend retail a zero counter, then restore it immediately after the call. */
    {
        u8 player_swords=0;
        int player_ultimate_bypass=0;
        int sub=D_8009B174&0x0f;
        if(D_8009B1D5==0 && D_8009B1C8 && D_8009B1C8->swords_turns_remaining &&
           sub==3 && (gInput_wPad1Pressed&0xC0) && ultimate_selected_attacker()>=0) {
            player_swords=D_8009B1C8->swords_turns_remaining;
            D_8009B1C8->swords_turns_remaining=0;
            player_ultimate_bypass=1;
        }
        psycho_redirect_attack_selection();
        inside_field_actions = 1;
        original_field_actions();
        inside_field_actions = 0;
        if(player_ultimate_bypass)D_8009B1C8->swords_turns_remaining=player_swords;
    }
    if(ultimate_field_guard && (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5)
        ultimate_restore_external_field_changes(ultimate_field_guard,ultimate_field_saved);
    mirror_watch_lp();
    mirror_reconcile_live();

    /* Ritual Summon is Psycho-Puppet's first once-per-turn activation.
     * Field actions only resume after the Ritual effect has finished cleaning
     * up its tributes, so this is the first reliable point at which free
     * Monster Zones can be evaluated. Consume the pending trigger regardless
     * of success: if there is no target or no free zone now, Psycho-Puppet
     * simply does not steal a monster this turn. */
    if(psycho_pending_mask) {
        unsigned pending=psycho_pending_mask;
        while(pending) {
            i=take_mask_bit(&pending);
            if (psycho_marks[i].ritual && psycho_marks[i].pending_summon_trigger) {
                psycho_marks[i].pending_summon_trigger = 0;
                psycho_pending_mask &= ~(1u<<i);
                if(psycho_pending_count)--psycho_pending_count;
                psycho_attempt_control(&psycho_marks[i], "invocacao-pos-ritual");
            } else psycho_pending_mask &= ~(1u<<i);
        }
    }
    psycho_reconcile_controlled();
    psycho_reconcile();
    while (deferred_deactivation) {
        i = take_mask_bit(&deferred_deactivation);
        if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 &&
            battle_participant(i)) {
            marks[i].in_battle = 1;
            marks[i].attack_count = attack_total - ((attack_mask&(1u<<i))?1u:0u);
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Millennium Shield entrou em combate: slot %d, atacantes=%u, bonus DEF=%u",
                          i, marks[i].attack_count, marks[i].attack_count * BONUS);
        } else {
            forget(&D_801A7AD8[i]);
        }
    }
    while (deferred_yamadron) {
        i = take_mask_bit(&deferred_yamadron);
        if (!((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 && battle_participant(i)))
            forget_yamadron(&D_801A7AD8[i]);
    }
    while (deferred_hungry) {
        i = take_mask_bit(&deferred_hungry);
        /* The native field controller temporarily deactivates combatants.
         * The phase-9 transition is authoritative; a pointer in the battle
         * presentation workspace is not a reliable lifecycle token yet.
         * Preserve the instance until post-battle reconstruction validates
         * its deck-record identity, for the player AND for the CPU. */
        if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9) {
            hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0; hungry_live_mask &= ~(1u<<i);
        }
    }
    while (deferred_javelin) {
        i = take_mask_bit(&deferred_javelin);
        if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 && battle_participant(i))
            javelin_marks[i].battle_flags = D_801A7AD8[i].flags | DUEL_CARD_FLAG_OCCUPIED;
        else {
            memset(&javelin_marks[i], 0, sizeof(javelin_marks[i]));
            javelin_live_mask &= ~(1u<<i);
        }
    }
    while (deferred_war_lion) {
        i = take_mask_bit(&deferred_war_lion);
        if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 && battle_participant(i))
            war_lion_marks[i].battle_flags = D_801A7AD8[i].flags | DUEL_CARD_FLAG_OCCUPIED;
        else {
            memset(&war_lion_marks[i], 0, sizeof(war_lion_marks[i]));
            war_lion_live_mask &= ~(1u<<i);
        }
    }
    /* A persistent Garma mark survives temporary deactivation, but never a
     * genuinely different occupant taking the same slot. The live masks let
     * these stable-field checks touch only actual Ritual instances. */
    if(garma_live_mask) { unsigned pending=garma_live_mask; while(pending) {
        i=take_mask_bit(&pending);
        if (!garma_marks[i].ritual) { garma_live_mask&=~(1u<<i); continue; }
        if ((D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            (D_801A7AD8[i].card_id != GARMA_SWORD_ID ||
             (D_801A7AD8[i].data && D_801A7AD8[i].data != garma_marks[i].data))) {
            memset(&garma_marks[i], 0, sizeof(garma_marks[i])); garma_live_mask&=~(1u<<i);
        }
    }}

    if(tri_live_mask) { unsigned pending=tri_live_mask; while(pending) {
        i=take_mask_bit(&pending);
        if(!tri_marks[i].ritual) { tri_live_mask&=~(1u<<i); continue; }
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) && !tri_identity(i)) {
            memset(&tri_marks[i],0,sizeof(tri_marks[i])); tri_live_mask&=~(1u<<i);
        }
    }}
    /* First attack uses retail availability. A new attack is unlocked only
     * after a battle really completed. Never reopen Gate Guardian every field
     * tick: under Swords that can make the CPU reconsider it forever. */
    if(gate_live_mask) { unsigned pending=gate_live_mask; while(pending) {
        i=take_mask_bit(&pending);
        if (gate_marks[i].ritual && gate_marks[i].extra_attack_pending &&
            gate_marks[i].attacks_this_turn < 3 &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            D_801A7AD8[i].card_id == GATE_GUARDIAN_ID &&
            D_801A7AD8[i].data == gate_marks[i].data) {
            D_801A7AD8[i].flags &= ~DUEL_CARD_FLAG_USED_THIS_TURN;
            gate_marks[i].extra_attack_pending=0;
        }
    }}
    /* Garma unlocks an extra attack after a battle kill; using it spends a blade.
     * The extra attack is granted immediately, but its Sword of Dark Destruction
     * presentation waits until the camera has returned to the field. */
    if (garma_live_mask && ritual_card_effect_enabled(GARMA_SWORD_ID)) { unsigned pending=garma_live_mask; while(pending) {
        i=take_mask_bit(&pending);
        if (garma_marks[i].ritual && garma_marks[i].extra_attack_pending &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].card_id == GARMA_SWORD_ID && D_801A7AD8[i].data == garma_marks[i].data) {
            D_801A7AD8[i].flags &= ~DUEL_CARD_FLAG_USED_THIS_TURN;
            garma_marks[i].extra_attack_pending = 0;
            garma_marks[i].extra_attack_ready = 1;
            garma_pending[i] = garma_marks[i].data;
            garma_pending_mask |= 1u << i;
            garma_screen[i].valid = 0; /* Require fresh field draws after the battle camera. */
        }
    }}
    garma_visual_tick();
    if(javelin_protected_visuals)javelin_feedback_tick();
    /* Player: preserve the old two-bite/audio/+500 popup/stat timeline.
     * CPU: the battle result already awarded stats, while only the bite
     * sound remains; no green popup is enqueued for the computer. */
    if(hungry_visual_pending_mask) {
        unsigned pending=hungry_visual_pending_mask;
        while(pending) {
            HungryMark *m;
            i=take_mask_bit(&pending);m=&hungry_marks[i];
            if (!hungry_visual_pending[i] || !m->ritual || !m->pending_stacks ||
                !(D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) || D_801A7AD8[i].card_id != HUNGRY_BURGER_ID ||
                D_801A7AD8[i].data != hungry_visual_pending[i] || D_801A7AD8[i].data != m->data || !D_801A7AD8[i].object) {
                hungry_visual_pending[i]=0; m->pending_stacks=0; m->bite_started=0; m->bite_delay=0;
                hungry_visual_pending_mask &= ~(1u<<i); continue;
            }
            if(!m->bite_started) {
                AudioReplace_Sfx(HUNGRY_BITE_SFX_ID,255,0);
                m->bite_started=1;m->bite_delay=HUNGRY_BITE_DELAY_FRAMES;
                continue;
            }
            if(m->bite_delay>0) { --m->bite_delay; continue; }
            /* Suppress the green sprite only for a computer-controlled
             * Burger. A human on either field side keeps the old visual. */
            if(ritual_ai_side_is_cpu(side_for_record(i)) || visual_queue_hungry_stack(i)) {
                --m->pending_stacks;m->bite_started=0;m->bite_delay=0;
                if(!m->pending_stacks){hungry_visual_pending[i]=0;hungry_visual_pending_mask&=~(1u<<i);}
            }
        }
    }
    visual_update_activation();
    visual_update_stats();
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==5 && !crab_umi_sequence &&
       !crab_umi_visual_state && !(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        war_lion_reconcile_bonus_visuals();
        cosmo_reconcile_bonus_visuals();
        skull_reconcile_bonus_visuals();
        dokurorider_reconcile_aura_visuals();
        /* A newly ritual-summoned BLS must finish its automatic Meadow path
         * before its aura presentation.  If Meadow is already active,
         * bls_field_pending was never armed and the aura may present now. */
        if(!bls_field_pending)bls_reconcile_aura_visuals();
    }
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9)sengenjin_battle_visual_mask=0;
    if(stat_effect_hints||garma_live_hint||tri_live_hint||gate_live_hint||sengenjin_live_hint||bls_live_hint||ultimate_live_hint||cosmo_live_hint||dokurorider_live_hint)refresh_hot_hints();
}


static void card_placement(void)
{
    if(!mask_instance_count && !psycho_controlling_count){original_card_placement();return;}
    /* When an invalid monster-on-monster combination reaches retail case 7,
     * retail throws one of the two placement objects away.  A monster borrowed
     * by Psycho-Puppet must be the survivor: it is not material owned by the
     * temporary controller.  Keep the borrowed object in slot 1 (the survivor
     * slot for this resolution) and make its placement-side marker local for
     * this tick so retail cannot swap it back into the discard position.
     * The marker is restored immediately afterwards; only the resolution of
     * this failed combination is changed. */
    DisplayObject *borrowed = 0;
    u8 saved_field_68 = 0;
    int n;

    if ((D_8009B174 & 0x0F) == 7) {
        for (n = 0; n < 2; ++n) {
            DisplayObject *o = D_800E9EF0[n];
            int r = o ? (int)o->field_6A : -1;
            if (r >= 0 && r < DUEL_CARD_RECORD_COUNT &&
                data_is_stolen(D_801A7AD8[r].data, D_8009B1D5)) {
                borrowed = o;
                if (n == 0) {
                    D_800E9EF0[0] = D_800E9EF0[1];
                    D_800E9EF0[1] = borrowed;
                }
                saved_field_68 = borrowed->field_68;
                borrowed->field_68 = 0;
                if (host && host->log)
                    host->log(host, "[Ritual Card Effects] Psycho-Puppet: combinacao invalida descarta a carta jogada; roubada preservada (slot %d)", r);
                break;
            }
        }
    }

    ++mask_inside_placement;
    original_card_placement();
    --mask_inside_placement;

    if (borrowed) borrowed->field_68 = saved_field_68;
}

static int battle_hook_needed(void)
{
    /* DuelScene_UpdateBattle is called many times per battle. Most duels do
     * not have a live Ritual effect that needs to touch this scene at all.
     * Keep the wrapper completely transparent in that common case. Live
     * masks are used instead of cumulative summon counters so the wrapper can
     * go dormant again after the last relevant Ritual leaves play. */
    return serpent_seen || serpent_ghost_state || serpent_redirect_state ||
        serpent_fear_battle_slot >= 0 || coin_marks_seen || coin_battle.active ||
        ritual_coin.active || coin_battle.dodge_ticks || stat_effect_hints ||
        hungry_live_mask || garma_live_hint || tri_live_hint || gate_live_hint || fiend_mirror_live_hint ||
        mirror_queue_count || mirror_reflection.active ||
        psycho_controlling_count || yamadron_live_mask || javelin_protected_visuals ||
        tri_battle_visuals || war_lion_live_mask ||
        sengenjin_live_hint || dokurorider_live_hint || dokurorider_evade_pending || dokurorider_evade_hold || ultimate_live_hint;
}


static void battle(void)
{
    int i;
    int state, phase, state11;
    int serpent_work, coin_work;
    DisplayObject *ultimate_trap_saved_defender=0;
    int ultimate_trap_preview=0;
    unsigned tri_restore = 0, javelin_restore = 0, crab_restore = 0, sengenjin_trap_restore_battle = 0, ultimate_trap_restore_battle = 0;

    if (!battle_hook_needed() && !ultimate_trap_cinematic) {
        original_battle();
        return;
    }

    state = D_8009B174 & 0x8f;
    phase = gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK;
    state11 = state == 11;
    if(!state11)gate_state11_counted_mask=0;
    serpent_work = serpent_seen || serpent_ghost_state || serpent_redirect_state ||
        serpent_fear_battle_slot >= 0;
    coin_work = coin_marks_seen || coin_battle.active || ritual_coin.active ||
        coin_battle.dodge_ticks;

    /* The first confrontation is strictly the trap, not the original
     * defender.  At native state 2 the card-art slot already contains the
     * revealed trap, but retail would put the real monster's ATK/DEF beside
     * it.  Hide ONLY the defender pointer for that first preview creation;
     * restore it immediately after calling retail.  The real defender never
     * leaves the battle and is used in the second confrontation below. */
    if(phase==9 && (state&0x0f)==2 && !(state&0x80) && ultimate_live_hint &&
       D_8009B22A>=DUEL_ATTACK_TRAP_FIRST_CARD_ID &&
       (D_8009B22A<DUEL_ATTACK_TRAP_FIRST_CARD_ID+DUEL_ATTACK_TRAP_COUNT ||
        D_8009B22A==DUEL_FAKE_TRAP_CARD_ID)) {
        DisplayObject *attacker=D_800E9EF0[0];
        int idx=attacker?(int)attacker->field_6A:-1;
        if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT && ultimate_effect_active(idx)) {
            ultimate_trap_saved_defender=D_800E9EF0[1];
            ultimate_trap_preview=1;
        }
    }

    if(phase==9 && dokurorider_evade_pending && ritual_card_effect_enabled(DOKURORIDER_ID)) {
        int escaped=0;
        unsigned pending=dokurorider_evade_pending;
        /* The evade result was already committed by the battle comparison
         * hook. Post the custom motorcycle pass-by exactly once for a real
         * Dokurorider evade; presentation/model animation is intentionally
         * left entirely to the game and any separate visual mods. */
        while(pending) {
            int idx=take_mask_bit(&pending);
            if(dokurorider_identity(idx)) {
                escaped=1;
                if(host&&host->log)
                    host->log(host,"[Ritual Card Effects] Dokurorider evadiu sem bloquear o controller: slot %d",idx);
            }
        }
        if(escaped)
            AudioReplace_Sfx(DOKURORIDER_ESCAPE_SFX_ID,255,0);
        dokurorider_evade_pending=0;
        dokurorider_evade_hold=0;
    }

    if(phase==9 && sengenjin_live_hint && ritual_card_effect_enabled(SENGENJIN_ID)) {
        int n;
        for(n=0;n<2;++n) {
            DisplayObject *participant=D_800E9EF0[n];
            int idx=participant?(int)participant->field_6A:-1;
            if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT && sengenjin_identity(idx) &&
               !(sengenjin_battle_visual_mask&(1u<<idx))) {
                /* Queue the Sengenjin flash once, but never stall the battle
                 * controller waiting for it. scene_update() already services
                 * the shared activation queue every frame.  The previous hold
                 * introduced an artificial pause before every Sengenjin battle. */
                visual_queue_activation(idx);
                sengenjin_battle_visual_mask|=1u<<idx;
            }
        }
    }

    /* Catch the ready defender panel before native state 3 advances, even
     * when a face-up replacement does not request the Fear ghost hold. */
    if (serpent_work) {
        serpent_redirect_begin();
        /* Fear ambush: before retail reveals the defending card, snapshot a face-down
         * defender attacked by the opposing side. One +1000/+1000 layer is added per
         * live ritual-summoned Serpent on the defender's side. The battle state is
         * held while the short ghost presentation runs. */
        if(serpent_ghost_state==2)return;
        /* State 3 starts after the native defender reveal has finished, before
         * the fade/result calculation. Keep the working snapshot below intact. */
        if(serpent_ghost_state==1 && (gDuel_wSceneStateFlags&0x8000) && state==3) {
            serpent_ghost_begin();
            if(serpent_ghost_state==2)return;
        }
        if(serpent_fear_battle_slot<0 && serpent_seen && ritual_card_effect_enabled(SERPENT_NIGHT_ID)){
            /* FieldActions has already deactivated attacker and defender before
             * phase 9. Retail saves the defender's pre-battle flags in
             * D_8009B17A, so that is the authoritative face-down snapshot. */
            DisplayObject *p=D_800E9EF0[1];
            int idx=p?(int)p->field_6A:-1;
            if(idx>=0&&idx<DUEL_CARD_RECORD_COUNT&&monster_slot(idx) &&
               (D_8009B17A&DUEL_CARD_FLAG_FACE_DOWN)){
                unsigned sources;
                ++serpent_dbg_facedown; serpent_dbg_last_slot=idx;
                sources=serpent_sources_for_side_battle(side_for_record(idx),idx); serpent_dbg_sources=sources;
                if(sources){
                    serpent_fear_battle_slot=idx;
                    serpent_fear_bonus[idx]=sources*SERPENT_FEAR_BONUS;
                    serpent_ghost_start(idx);
                    if(host&&host->log)host->log(host,"[Ritual Card Effects] Fear: defensor %d face-down (saved=%04X), +%u/+%u (%u Serpent)",idx,(unsigned)D_8009B17A,serpent_fear_bonus[idx],serpent_fear_bonus[idx],sources);
                }
            }
        }
    }

    if(coin_work && coin_before_battle())return;

    /* Native state 3 must see the trap token.  It deliberately skips ALL
     * monster battle calculations and enters state 10 to make the Trap card
     * disappear.  Once that transition occurs, use the RIGHT-HAND preview
     * for the burn, even for Widespread Ruin: the Ultimate on the left is
     * immune and must never receive the fire/destruction animation. */

    /* Retail state 10 is the 2D CARD-burning presentation, separate from the
     * later 3D model reaction.  Javelin's shell and Tri-Horned's horns both
     * prevent the logical destruction, so take the native survivor path
     * (state 6 -> 11) before the fire request is created. Battle math and LP
     * damage were already resolved by retail; only the presentation changes. */
    if(phase==9 && state==10 && D_8009B22A==0) {
        int part=D_8009B1B9;
        if(part>=0 && part<2 && D_800E9EF0[part] && D_800E9EF0[part+2]) {
            int idx=(int)D_800E9EF0[part]->field_6A;
            if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT &&
               ritual_card_effect_enabled(JAVELIN_BEETLE_ID) &&
               javelin_marks[idx].ritual && !javelin_marks[idx].protection_used_turn &&
               D_801A7AD8[idx].card_id==JAVELIN_BEETLE_ID &&
               D_801A7AD8[idx].data==javelin_marks[idx].data) {
                javelin_2d_survivor_parts|=1u<<part;
                D_8009B174=6;
                if(host&&host->log)
                    host->log(host,"[Ritual Card Effects] Javelin: evitou fogo da carta 2D, participante %d, registro %d",part,idx);
            } else if (crab_battle_protected(idx,
                       D_800E9EF0[1-part] ? (int)D_800E9EF0[1-part]->field_6A : -1)) {
                D_8009B174=6; /* Native 2D survivor path, no burning card. */
                if(host && host->log)
                    host->log(host,"[Ritual Card Effects] Crab/Umi: bloqueou destruicao 2D no slot %d",idx);
            } else if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT &&
                      ritual_card_effect_enabled(TRI_HORNED_ID) &&
                      tri_identity(idx) && tri_marks[idx].horns &&
                      !(tri_battle_charged_parts&(1u<<part))) {
                tri_2d_survivor_parts|=1u<<part;
                D_8009B174=6;
                if(host&&host->log)
                    host->log(host,"[Ritual Card Effects] Tri-Horned: evitou fogo da carta 2D, participante %d, registro %d, chifres %u",part,idx,tri_marks[idx].horns);
            }
        }
    }
    if(tri_live_hint)tri_restore=tri_battle_survivors();

    /* Remember that a ritual Fiend's Mirror entered this battle alive.
     * Its final battle-damage reflection is still legal even if that same
     * battle removes it from the field before LP loss is observed. */
    if (fiend_mirror_live_hint && fiends_mirror_effect_on && phase == 9) {
        int n;
        /* Snapshot the actual battle participants BEFORE asking the live-field
         * scanner.  On the opponent's turn retail can stage/deactivate the
         * defending record before this hook sees it; D_800E9EF0 still retains
         * field_6A, so the Ritual mark tells us that this battle began with a
         * valid Fiend's Mirror.  This grace is battle-local and is cleared as
         * soon as phase 9 ends, preventing the old post-death ghost source. */
        for (n = 0; n < 2; ++n) {
            DisplayObject *participant = D_800E9EF0[n];
            int idx = participant ? (int)participant->field_6A : -1;
            if (idx >= 0 && idx < DUEL_CARD_RECORD_COUNT &&
                fiend_mirror_marks[idx].ritual &&
                D_801A7AD8[idx].card_id==FIENDS_MIRROR_ID &&
                D_801A7AD8[idx].data &&
                D_801A7AD8[idx].data==fiend_mirror_marks[idx].data) {
                int side = idx >= 15;
                mirror_battle_grace[side] = 1;
            }
        }
        if (!mirror_battle_grace[0] && side_has_ritual_fiend_mirror(0)) mirror_battle_grace[0] = 1;
        if (!mirror_battle_grace[1] && side_has_ritual_fiend_mirror(1)) mirror_battle_grace[1] = 1;
    }

    /* State 11 rebuilds survivors and releases the battle display objects.
     * Inspect the survivor slots before the original call releases them;
     * this also covers records rebuilt for a 3D battle preview. */
    if (state11) {
        /* Only Shield/Yamadron keep provenance that must be explicitly retired
         * from a losing battle participant here. */
        if ((stat_effect_hints & STAT_HINT_SHIELD) || yamadron_live_mask) {
            for (i = 0; i < 2; ++i) {
                if (D_800E9EF0[i] && !D_800E9EF0[i + 2]) {
                    int index = D_800E9EF0[i]->field_6A;
                    if (monster_slot(index)) { forget(&D_801A7AD8[index]); forget_yamadron(&D_801A7AD8[index]); }
                }
            }
        }
        if (gate_live_hint) {
            for (i = 0; i < 2; ++i) {
                DisplayObject *participant = D_800E9EF0[i];
                int idx = participant ? (int)participant->field_6A : -1;
                if (idx >= 0 && idx < DUEL_CARD_RECORD_COUNT &&
                    !(gate_state11_counted_mask&(1u<<idx)) &&
                    side_for_record(idx) == D_8009B1D5 &&
                    gate_marks[idx].ritual && gate_marks[idx].data == D_801A7AD8[idx].data &&
                    D_801A7AD8[idx].card_id == GATE_GUARDIAN_ID &&
                    gate_marks[idx].attacks_this_turn < 3) {
                    ++gate_marks[idx].attacks_this_turn;
                    gate_state11_counted_mask|=1u<<idx;
                    if(gate_marks[idx].attacks_this_turn<3 && D_800E9EF0[i+2])
                        gate_marks[idx].extra_attack_pending=1;
                }
            }
        }
    }

    /* Battle state 11 knows the logical loser.  For a Javelin whose 2D fire
     * was skipped, retail still owns a live result object and will reconstruct
     * the field card normally.  Other Javelin destruction paths still use the
     * existing manual rebuild.  Spend the shell in either case, after battle
     * math has completed, without presenting a bogus destruction animation. */
    if (state11 && D_8009B22A==0 && (stat_effect_hints & STAT_HINT_JAVELIN) && ritual_card_effect_enabled(JAVELIN_BEETLE_ID)) {
        for (i = 0; i < 2; ++i) {
            DisplayObject *participant = D_800E9EF0[i];
            int idx = participant ? (int)participant->field_6A : -1;
            if (idx >= 0 && idx < DUEL_CARD_RECORD_COUNT &&
                (!D_800E9EF0[i + 2] || (javelin_2d_survivor_parts&(1u<<i))) &&
                javelin_marks[idx].ritual && !javelin_marks[idx].protection_used_turn &&
                D_801A7AD8[idx].card_id == JAVELIN_BEETLE_ID &&
                D_801A7AD8[idx].data == javelin_marks[idx].data) {
                javelin_marks[idx].protection_used_turn = 1;
                if(D_800E9EF0[i+2])
                    javelin_protected_visuals|=1u<<idx; /* Native state 11 restores it. */
                else
                    javelin_restore|=1u<<idx; /* Fallback for other destroy paths. */
                if (host && host->log) host->log(host, "[Ritual Card Effects] Javelin Beetle marcou protecao de batalha: slot %d", idx);
            }
        }
    }
    if(state11 && sengenjin_live_hint && ritual_card_effect_enabled(SENGENJIN_ID) &&
       D_8009B22A>=DUEL_ATTACK_TRAP_FIRST_CARD_ID &&
       D_8009B22A<DUEL_ATTACK_TRAP_FIRST_CARD_ID+DUEL_ATTACK_TRAP_COUNT) {
        DisplayObject *attacker=D_800E9EF0[0];
        int idx=attacker?(int)attacker->field_6A:-1;
        if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT && sengenjin_identity(idx) && !D_800E9EF0[2]) {
            sengenjin_trap_restore_battle|=1u<<idx;
            visual_queue_activation(idx);
            if(host&&host->log)host->log(host,"[Ritual Card Effects] Sengenjin ignorou Trap de ataque: slot %d trap=%d",idx,(int)D_8009B22A);
        }
    }
    if(state11 && ultimate_live_hint && ritual_card_effect_enabled(BLUE_EYES_ULTIMATE_ID) &&
       D_8009B22A>=DUEL_ATTACK_TRAP_FIRST_CARD_ID &&
       D_8009B22A<DUEL_ATTACK_TRAP_FIRST_CARD_ID+DUEL_ATTACK_TRAP_COUNT) {
        DisplayObject *attacker=D_800E9EF0[0];
        int idx=attacker?(int)attacker->field_6A:-1;
        if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT && ultimate_identity(idx) && !D_800E9EF0[2]) {
            ultimate_trap_restore_battle|=1u<<idx;
            visual_queue_activation(idx);
            if(host&&host->log)host->log(host,"[Ritual Card Effects] Blue-Eyes Ultimate ignorou Trap de ataque: slot %d trap=%d",idx,(int)D_8009B22A);
        }
    }
    if (state11 && (stat_effect_hints & STAT_HINT_CRAB) && D_8009B22A==0) {
        for (i=0;i<2;++i) {
            DisplayObject *p=D_800E9EF0[i], *enemy=D_800E9EF0[1-i];
            int idx=p?(int)p->field_6A:-1, eidx=enemy?(int)enemy->field_6A:-1;
            if(!D_800E9EF0[i+2] && crab_battle_protected(idx,eidx))
                crab_restore|=1u<<idx;
        }
    }

    /* One trace per native battle completion, rather than per-frame logging.
     * It distinguishes a missing Ritual mark from a rejected battle result. */
    if (state11 && D_8009B22A == 0) {
        for (i = 0; i < 2; ++i) {
            int idx = D_800E9EF0[i] ? (int)D_800E9EF0[i]->field_6A : -1;
            if (monster_slot(idx) && D_801A7AD8[idx].card_id == HUNGRY_BURGER_ID &&
                host && host->log)
                host->log(host,
                    "[Ritual Card Effects] Hungry battle: slot=%d side=%d marked=%d mask=%d live=%d native=%d/%d visual=%d/%d setting=%d",
                    idx, side_for_record(idx), hungry_marks[idx].ritual,
                    (hungry_live_mask & (1u << idx)) != 0,
                    hungry_marks[idx].data == D_801A7AD8[idx].data,
                    (int)D_8009B1B0[i], (int)D_8009B1B0[1-i],
                    D_800E9EF0[i+2] != 0, D_800E9EF0[(1-i)+2] != 0,
                    ritual_card_effect_enabled(HUNGRY_BURGER_ID));
        }
    }

    /* Hungry Burger gains a stack only for an actual monster destroyed by
     * battle.  A direct attack has no opposing participant and therefore can
     * never satisfy this rule.  Also wait until every battle-survival
     * substitution is known so Tri-Horned/Javelin/Crab protection cannot be
     * misread as a kill just because retail's survivor slot was temporarily
     * empty. */
    if (state11 && D_8009B22A==0 && hungry_live_mask &&
        ritual_card_effect_enabled(HUNGRY_BURGER_ID)) {
        for (i = 0; i < 2; ++i) {
            DisplayObject *participant=D_800E9EF0[i];
            DisplayObject *enemy=D_800E9EF0[1-i];
            int idx=participant?(int)participant->field_6A:-1;
            int enemy_idx=enemy?(int)enemy->field_6A:-1;
            unsigned enemy_bit=(enemy_idx>=0 && enemy_idx<DUEL_CARD_RECORD_COUNT)?(1u<<enemy_idx):0;
            if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT && enemy_bit && monster_slot(enemy_idx) &&
               (hungry_live_mask & (1u<<idx)) && hungry_marks[idx].ritual &&
               D_801A7AD8[idx].card_id==HUNGRY_BURGER_ID &&
               hungry_marks[idx].data==D_801A7AD8[idx].data &&
               ((D_800E9EF0[i+2] && !D_800E9EF0[(1-i)+2]) ||
                (D_8009B1B0[i] >= 0 && D_8009B1B0[1-i] == -1)) &&
               !(tri_restore&enemy_bit) && !(javelin_restore&enemy_bit) &&
               !(crab_restore&enemy_bit) && !coin_battle_protected(1-i)) {
                /* CPU-only functional fix: grant +500 immediately after a
                 * confirmed battle destruction. Do NOT change the human's
                 * established audio/popup-driven stat timing. */
                if(ritual_ai_side_is_cpu(side_for_record(idx)))
                    ++hungry_marks[idx].battle_kills;
                ++hungry_marks[idx].pending_stacks;
                hungry_visual_pending[idx]=hungry_marks[idx].data;
                hungry_visual_pending_mask|=1u<<idx;
                hungry_screen[idx].valid=0;
                if(host&&host->log)
                    host->log(host,"[Ritual Card Effects] Hungry Burger: slot=%d cpu=%d bonus_aplicado=%u mordidas_pendentes=%u",
                              idx,ritual_ai_side_is_cpu(side_for_record(idx)),hungry_marks[idx].battle_kills*HUNGRY_BONUS,hungry_marks[idx].pending_stacks);
            }
        }
    }
    /* Only participant zero initiated this battle. Direct attacks have no
     * defender and defensive victories never award an extra blade. */
    if (state11 && garma_live_hint) {
        int a=D_800E9EF0[0] ? D_800E9EF0[0]->field_6A : -1;
        int d=D_800E9EF0[1] ? D_800E9EF0[1]->field_6A : -1;
        /* A sixth attack uses the last sword. Keep its +1000 layer
         * through the whole native battle and clear only after state 11 ends. */
        if(garma_spend_extra(a) && garma_marks[a].blades_used==5)
            garma_marks[a].final_attack_cleanup=1;
        if(garma_award(a,d,D_800E9EF0[2]!=0, D_800E9EF0[3]!=0 ||
            (monster_slot(d) && ((javelin_restore|crab_restore|tri_restore)&(1u<<d))))) {
            /* Only an attacker that destroyed a real opponent qualifies. */
            if(garma_marks[a].strike_bonus < 1000u)garma_marks[a].strike_bonus+=200u;
        }
        if(monster_slot(a) && !D_800E9EF0[2]) memset(&garma_marks[a],0,sizeof(garma_marks[a]));
        if(monster_slot(d) && !D_800E9EF0[3]) memset(&garma_marks[d],0,sizeof(garma_marks[d]));
    }

    /* Never hide a participant from the other Ritual hooks: the substitution
     * is local to the single native card-preview creation call. */
    if(ultimate_trap_preview) D_800E9EF0[1]=0;
    original_battle();
    if(state11 && (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9 && garma_live_mask) {
        unsigned cleanup=garma_live_mask;
        while(cleanup) {
            int gi=take_mask_bit(&cleanup);
            if(garma_marks[gi].final_attack_cleanup) {
                garma_marks[gi].strike_bonus=0;
                garma_marks[gi].final_attack_cleanup=0;
            }
        }
    }
    if(ultimate_trap_preview)
        D_800E9EF0[1]=ultimate_trap_saved_defender;
    if(phase==9 && (state&0x0f)==3 && mirror_defense_direct_pending &&
       D_8009B22A==0 &&
       mirror_defense_direct_side>=0 && mirror_defense_direct_side<DUEL_SIDE_COUNT) {
        unsigned damage=mirror_defense_direct_pending;
        int target=mirror_defense_direct_side;
        unsigned life=D_800E9FF0[target].life_points.unsigned_value;
        unsigned other_life=D_800E9FF0[target^1].life_points.unsigned_value;
        mirror_defense_direct_pending=0; mirror_defense_direct_side=-1;
        /* Both LP changes are a single battle result. Prepay the reflection
         * even if the first hit is lethal; the subsequent requests only
         * present those two losses, and must never deduct a second time. */
        D_800E9FF0[target].life_points.unsigned_value=(u16)(damage>=life?0:life-damage);
        D_800E9FF0[target^1].life_points.unsigned_value=(u16)(damage>=other_life?0:other_life-damage);
        mirror_last_lp[target]=D_800E9FF0[target].life_points.unsigned_value;
        mirror_last_lp[target^1]=D_800E9FF0[target^1].life_points.unsigned_value;
        mirror_queue_prepaid(target,damage,-1);
        mirror_queue_prepaid(target^1,damage,visual_find_live_mirror(target));
        if(host&&host->log)host->log(host,
            "[Ritual Card Effects] Fiend's Mirror DEF recebeu ataque direto de %u LP (lado %d)",damage,target);
        /* The normal watcher still handles independent losses from other
         * sources; both direct/reflective hits above are already recorded. */
        mirror_watch_lp();
    }

    if(phase==9 && (state&0x0f)==3 && !ultimate_trap_cinematic &&
       (D_8009B174&0x0f)==10 && ultimate_live_hint &&
       D_8009B22A>=DUEL_ATTACK_TRAP_FIRST_CARD_ID &&
       (D_8009B22A<DUEL_ATTACK_TRAP_FIRST_CARD_ID+DUEL_ATTACK_TRAP_COUNT ||
        D_8009B22A==DUEL_FAKE_TRAP_CARD_ID)) {
        DisplayObject *attacker=D_800E9EF0[0];
        int idx=attacker?(int)attacker->field_6A:-1;
        if(idx>=0 && idx<DUEL_CARD_RECORD_COUNT && ultimate_effect_active(idx)) {
            ultimate_trap_cinematic=1;
            D_8009B1B9=1; /* Burn the revealed Trap preview, not Ultimate. */
            ultimate_ai_learn_category(D_8009B1D5^1,1);
            if(host&&host->log)
                host->log(host,"[Ritual Card Effects] Ultimate/Trap: terminou confronto visual, consumindo Trap %d",(int)D_8009B22A);
        }
    }
    if(phase==9 && ultimate_trap_cinematic && (state&0x0f)==10 &&
       (D_8009B174&0x0f)==11) {
        /* The trap disappeared and its effect/request completed.  BEFORE
         * retail state 11 closes the battle and marks the attacker used,
         * release the now-obsolete previews, clear only the trap cancel
         * token and restart the native two-card presentation.  State 1
         * requests the ACTUAL defender's artwork from the staged card; state
         * 3 subsequently calculates LP/damage exactly once against it. */
        func_80029528(0);
        D_800E9EF0[2]=0;
        D_800E9EF0[3]=0;
        D_8009B22A=0;
        D_8009B1B9=0;
        D_8009B174=1;
        ultimate_trap_cinematic=0;
        if(host&&host->log)
            host->log(host,"[Ritual Card Effects] Ultimate/Trap: segundo confronto iniciado contra o alvo original");
    }

    /* The native 3D battle scene decides whether a model burns away from
     * D_800EF658[n].field_02.  Javelin/Tri protection is resolved later in
     * state 11 so the normal LP/battle result remains untouched, but waiting
     * until state 11 is too late to stop the misleading destruction fire.
     * Once state 5 has prepared the animated-battle properties, clear only
     * that visual death flag for a Ritual instance that is eligible to spend
     * its protection.  The logical loser is still rebuilt/charged normally
     * by the existing state-11 survivor path. */
    if (phase == 9 && (state & 0x0F) == 5 && D_8009B22A == 0) {
        for (i = 0; i < 2; ++i) {
            DisplayObject *participant = D_800E9EF0[i];
            int idx = participant ? (int)participant->field_6A : -1;
            if (idx < 0 || idx >= DUEL_CARD_RECORD_COUNT ||
                D_800EF658[i].field_02 == 0)
                continue;
            if (ritual_card_effect_enabled(JAVELIN_BEETLE_ID) &&
                javelin_marks[idx].ritual &&
                !javelin_marks[idx].protection_used_turn &&
                D_801A7AD8[idx].card_id == JAVELIN_BEETLE_ID &&
                D_801A7AD8[idx].data == javelin_marks[idx].data) {
                D_800EF658[i].field_02 = 0;
                battle_visual_survivor_parts |= 1u << i;
                if (host && host->log)
                    host->log(host, "[Ritual Card Effects] Javelin: suprimiu morte 3D do participante %d (slot %d)", i, idx);
            } else if (crab_battle_protected(idx,
                       D_800E9EF0[1-i] ? (int)D_800E9EF0[1-i]->field_6A : -1)) {
                D_800EF658[i].field_02=0;
                battle_visual_survivor_parts|=1u<<i;
                if(host && host->log)
                    host->log(host,"[Ritual Card Effects] Crab/Umi: bloqueou morte 3D no slot %d",idx);
            } else if (ritual_card_effect_enabled(TRI_HORNED_ID) &&
                       tri_identity(idx) && tri_marks[idx].horns) {
                D_800EF658[i].field_02 = 0;
                battle_visual_survivor_parts |= 1u << i;
                if (host && host->log)
                    host->log(host, "[Ritual Card Effects] Tri-Horned: suprimiu morte 3D do participante %d (slot %d)", i, idx);
            }
        }
    }

    if(coin_work)coin_after_battle();

    while(sengenjin_trap_restore_battle) {
        DuelCardRecord *c;
        i=take_mask_bit(&sengenjin_trap_restore_battle);
        c=&D_801A7AD8[i];
        c->flags=sengenjin_marks[i].battle_flags|DUEL_CARD_FLAG_OCCUPIED;
        if(side_for_record(i)==D_8009B1D5)c->flags|=DUEL_CARD_FLAG_USED_THIS_TURN;
        if(c->object)DisplayObject_ReleaseIfPresent(c->object);
        c->object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
        if(c->object)((DuelCardDisplayObject*)c->object)->card_index=(u8)i;
    }
    while(ultimate_trap_restore_battle) {
        DuelCardRecord *c;
        i=take_mask_bit(&ultimate_trap_restore_battle);
        c=&D_801A7AD8[i];
        c->flags=ultimate_marks[i].battle_flags|DUEL_CARD_FLAG_OCCUPIED|DUEL_CARD_FLAG_USED_THIS_TURN;
        if(c->object)DisplayObject_ReleaseIfPresent(c->object);
        c->object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
        if(c->object)((DuelCardDisplayObject*)c->object)->card_index=(u8)i;
    }
    /* Same post-result survivor reconstruction as Javelin/Crab. LP damage
     * and the completed native battle remain untouched. Iterate only the bits
     * that actually need rebuilding instead of walking all 30 records. */
    while(tri_restore) {
        i=take_mask_bit(&tri_restore);
        {
            DuelCardRecord *c=&D_801A7AD8[i];
            c->flags=tri_marks[i].battle_flags|DUEL_CARD_FLAG_OCCUPIED;
            if(side_for_record(i)==D_8009B1D5)c->flags|=DUEL_CARD_FLAG_USED_THIS_TURN;
            if(c->object)DisplayObject_ReleaseIfPresent(c->object);
            /* If this same battle already reduced either duellist to zero LP,
             * Results takes over immediately. Keep the logical horn survivor,
             * but do not recreate its small field-card object behind the loss
             * screen. */
            if(D_800E9FF0[0].life_points.unsigned_value==0 ||
               D_800E9FF0[1].life_points.unsigned_value==0) {
                c->object=0;
            } else {
                c->object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
                if(c->object)((DuelCardDisplayObject*)c->object)->card_index=(u8)i;
                tri_battle_visuals|=1u<<i;
            }
        }
    }
    while(crab_restore) {
        i=take_mask_bit(&crab_restore);
        {
            DuelCardRecord *c=&D_801A7AD8[i];
            c->flags=crab_marks[i].battle_flags|DUEL_CARD_FLAG_OCCUPIED;
            if(side_for_record(i)==D_8009B1D5)c->flags|=DUEL_CARD_FLAG_USED_THIS_TURN;
            if(c->object)DisplayObject_ReleaseIfPresent(c->object);
            c->object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
            if(c->object)((DuelCardDisplayObject*)c->object)->card_index=(u8)i;
            if(!(crab_restore_visuals&(1u<<i))){visual_queue_activation(i);crab_restore_visuals|=1u<<i;}
        }
    }
    while(javelin_restore) {
        DuelCardRecord *c;
        i=take_mask_bit(&javelin_restore);
        c = &D_801A7AD8[i];
        c->flags = javelin_marks[i].battle_flags | DUEL_CARD_FLAG_OCCUPIED;
        if (side_for_record(i) == D_8009B1D5) c->flags |= DUEL_CARD_FLAG_USED_THIS_TURN;
        if (c->object) DisplayObject_ReleaseIfPresent(c->object);
        c->object = func_80024C1C(c->card_id, D_800908A0[i].x, D_800908A0[i].y);
        if (c->object) ((DuelCardDisplayObject *)c->object)->card_index = (u8)i;
        javelin_protected_visuals |= 1u << i;
    }

    /* A Mirror destroyed by this battle may already have left the live mask.
     * mirror_battle_grace is exactly the proof that its final battle-damage
     * reflection is still owed, so the battle exit must call the LP watcher
     * even when fiend_mirror_live_hint has just become false. */
    if(fiend_mirror_live_hint || mirror_battle_grace[0] || mirror_battle_grace[1] ||
       mirror_queue_count || mirror_reflection.active)mirror_watch_lp();
    phase = gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK;
    if (phase == 9) return;
    mirror_defense_direct_pending=0; mirror_defense_direct_side=-1;
    javelin_2d_survivor_parts=0;
    tri_2d_survivor_parts=tri_battle_charged_parts=0;
    dokurorider_evade_pending=0;dokurorider_evade_hold=0;

    /* Rebuild already happened above. Emit the shell-break feedback only
     * after the surviving Javelin is back on the field. */
    javelin_feedback_tick();
    if(tri_battle_visuals) {
        unsigned pending=tri_battle_visuals;
        while(pending) {
            i=take_mask_bit(&pending);
            /* Horn break already has its own SFX/counter change.  The generic
             * activation/fire animation is intentionally omitted because the
             * Tri-Horned instance survived the prevented destruction. */
            tri_horn_visuals&=~(1u<<i);
        }
        tri_battle_visuals=0;
    }
    if(tri_live_mask) {
        unsigned pending=tri_live_mask;
        while(pending) {
            i=take_mask_bit(&pending);
            if(tri_marks[i].ritual && !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)) {
                memset(&tri_marks[i],0,sizeof(tri_marks[i])); tri_live_mask&=~(1u<<i);
            }
        }
        tri_seen=tri_live_mask!=0;
    }
    /* Mask of Shine & Dark has no battle-time behavior. If another Ritual
     * already keeps this wrapper active, reconcile it here as before; when
     * Mask is the only live effect, defer this cleanup to FieldActions, which
     * runs mask_reconcile() before the next actionable field tick. This keeps
     * Mask-only battles on the native battle fast path without changing when
     * a Fusion can next be attempted. */
    if(mask_instance_count)mask_reconcile();
    if(psycho_controlling_mask)psycho_reconcile_controlled();
    if(hungry_live_mask) {
        unsigned pending=hungry_live_mask;
        while(pending) {
            DuelCardRecord *h;
            i=take_mask_bit(&pending); h=&D_801A7AD8[i];
            if (!hungry_marks[i].ritual || !(h->flags & DUEL_CARD_FLAG_OCCUPIED) ||
                h->data != hungry_marks[i].data || h->card_id != HUNGRY_BURGER_ID) {
                hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0;
                hungry_live_mask &= ~(1u<<i);
            }
        }
    }
    if(yamadron_live_mask) {
        unsigned pending=yamadron_live_mask;
        while(pending) {
            DuelCardRecord *y;
            i=take_mask_bit(&pending); y=&D_801A7AD8[i];
            if (!yamadron_marks[i].ritual || !(y->flags & DUEL_CARD_FLAG_OCCUPIED) ||
                y->data != yamadron_marks[i].data || y->card_id != YAMADRON_ID)
                forget_yamadron(y);
        }
    }
    /* Retail rebuilds survivors before leaving phase 9. A destroyed card
     * remains inactive; a fusion result has another identity. */
    if(stat_effect_hints & STAT_HINT_SHIELD) {
        unsigned attack_total=0, pending=shield_live_mask; int attack_total_ready=0;
        while(pending) {
            DuelCardRecord *card;
            i=take_mask_bit(&pending);
            card=&D_801A7AD8[i];
            if (!marks[i].ritual || !(card->flags & DUEL_CARD_FLAG_OCCUPIED) ||
                card->data != marks[i].data || card->card_id != marks[i].card_id) {
                forget(card);
            } else if (marks[i].in_battle) {
                /* A shield that participated already took its fresh reading at
                 * battle commit, so keep that value after the battle. */
                marks[i].in_battle = 0;
            } else {
                /* Every other ritual Millennium Shield takes a post-battle
                 * reading, after destroyed monsters have left the field. */
                unsigned old_count = marks[i].attack_count;
                unsigned self_attack;
                if(!attack_total_ready){attack_total=count_attack_monsters_except(-1);attack_total_ready=1;}
                self_attack=!(card->flags&DUEL_CARD_FLAG_DEFENSE_POSITION);
                marks[i].attack_count = attack_total-self_attack;
                if (marks[i].attack_count > old_count) {
                    if (!marks[i].def_visual_shown) { visual_queue_activation(i); marks[i].def_visual_shown = 1; }
                    visual_queue_stat(i, (int)((marks[i].attack_count - old_count) * BONUS));
                }
                if (host && host->log)
                    host->log(host, "[Ritual Card Effects] Millennium Shield releitura pos-combate: slot %d, atacantes=%u, bonus DEF=%u",
                              i, marks[i].attack_count, marks[i].attack_count * BONUS);
            }
        }
    }
    if (serpent_fear_battle_slot>=0) {
        serpent_fear_bonus[serpent_fear_battle_slot]=0; serpent_fear_battle_slot=-1;
    }
}

static void restore_protected_visuals(void)
{
    int i;
    unsigned survivors = effect_survivor_visuals | protected_visuals;
    if(!survivors){protected_visuals=0;effect_survivor_visuals=0;tri_horn_visuals=0;return;}
    while (survivors) {
        DuelCardRecord *card;
        DuelCardDisplayObject *obj;
        i = take_mask_bit(&survivors);
        card = &D_801A7AD8[i];
        if (!(card->flags & DUEL_CARD_FLAG_OCCUPIED) || !monster_slot(i)) continue;

        /* Shared effect-survival resync. Destruction animations are allowed to
         * run normally, but a card whose logical removal was suppressed can
         * have its display object left in the destruction workspace. Rebuild
         * that object only after the whole effect resolves. */
        if (card->object) DisplayObject_ReleaseIfPresent(card->object);
        obj = func_80024C1C(card->card_id, D_800908A0[i].x, D_800908A0[i].y);
        card->object = obj;
        if (obj) obj->card_index = (u8)i;

        /* Millennium Shield keeps its one-time explanatory flash. Other users
         * of the generic helper get only the invisible visual resync. */
        if ((protected_visuals & (1u << i)) && marks[i].ritual &&
            card->data == marks[i].data && card->card_id == marks[i].card_id &&
            obj && !marks[i].protect_visual_shown) {
            visual_queue_activation(i);
            marks[i].protect_visual_shown = 1;
        }
        /* A consumed Tri-Horned horn is represented by its own break SFX and
         * counter decrement.  Rebuild the survivor silently; no fire/activation
         * presentation for a card that was not destroyed. */
        if (host && host->log)
            host->log(host, "[Ritual Card Effects] effect-survival visual resync: slot %d card %u", i, (unsigned)card->card_id);
    }
    protected_visuals = 0;
    effect_survivor_visuals = 0;
    tri_horn_visuals=0;
}

static void reread_all_shields(const char *reason)
{
    unsigned attack_total, pending;
    if(!(stat_effect_hints & STAT_HINT_SHIELD))return;
    attack_total=count_attack_monsters_except(-1);
    pending=shield_live_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        DuelCardRecord *card = &D_801A7AD8[i];
        if (!marks[i].ritual || !(card->flags & DUEL_CARD_FLAG_OCCUPIED) ||
            card->data != marks[i].data || card->card_id != marks[i].card_id) {
            forget(card);
            continue;
        }
        {
            unsigned old_count = marks[i].attack_count;
            unsigned self_attack = !(card->flags&DUEL_CARD_FLAG_DEFENSE_POSITION);
            unsigned new_count = attack_total-self_attack;
            marks[i].attack_count = new_count;
            if (new_count > old_count) {
                if (!marks[i].def_visual_shown) { visual_queue_activation(i); marks[i].def_visual_shown = 1; }
                visual_queue_stat(i, (int)((new_count - old_count) * BONUS));
            }
        }
        if (host && host->log)
            host->log(host, "[Ritual Card Effects] Millennium Shield releitura %s: slot %d, atacantes=%u, bonus DEF=%u",
                      reason, i, marks[i].attack_count, marks[i].attack_count * BONUS);
    }
}

/* Wrap the three monster-destruction effect families exposed by the SDK.
 * The RemoveFromField hook above only suppresses removal while one of these
 * handlers is executing, so battle destruction remains normal. */
static int destruction_context_needed(void)
{ return (stat_effect_hints & (STAT_HINT_SHIELD|STAT_HINT_WHALE)) ||
    tri_resolution_active || ultimate_live_hint; }

/* Board-destruction request 0x11 owns the native "suck into the hole/jar"
 * presentation. Logical immunity used to be enforced only later, at
 * DuelCard_RemoveFromField(), so an immune survivor still got pulled into the
 * animation and then rebuilt. While request 0x11 itself updates, temporarily
 * present protected records as unoccupied. Their display objects remain
 * intact and the flag is restored before the frame continues, so they stay
 * visibly in place while every genuinely affected card keeps the native
 * animation. */
static int board_visual_request_active(void)
{
    int i;
    for(i=0;i<DUEL_EFFECT_REQUEST_COUNT;++i)
        if((D_800EAD88[i].flags&DUEL_EFFECT_REQUEST_FLAG_ACTIVE) &&
           D_800EAD88[i].id==0x11)return 1;
    return 0;
}

static unsigned board_visual_immune_mask(void)
{
    unsigned mask=0,pending;
    int i;
    if(!(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE))return 0;
    if(gDuel_wEffectCardID!=DARK_HOLE_ID &&
       gDuel_wEffectCardID!=DUEL_DRAGON_CAPTURE_JAR_CARD_ID)return 0;

    if(ultimate_card_effect.active)mask|=ultimate_card_effect.mask;

    /* Dark Hole uses the direct Ultimate destruction guard rather than the
     * generic Magic/Trap snapshot.  Hide every live effect-active Ritual
     * Ultimate from request 0x11 so the native hole animation never consumes
     * its field object, while remove/deactivate independently suppress the
     * logical destruction. */
    if(gDuel_wEffectCardID==DARK_HOLE_ID &&
       ritual_card_effect_enabled(BLUE_EYES_ULTIMATE_ID)) {
        unsigned u=ultimate_live_mask;
        while(u) {
            i=take_mask_bit(&u);
            if(ultimate_effect_active(i))mask|=1u<<i;
        }
    }

    /* The remaining board-destruction immunities matter to Dark Hole.
     * Dragon Capture Jar targets Dragons, so the generic non-Dragon guards
     * below naturally add no extra targets there. */
    if(gDuel_wEffectCardID==DARK_HOLE_ID) {
        mask|=whale_snapshot_protection();
        for(i=5;i<10;++i) {
            DuelCardRecord *c=&D_801A7AD8[i];
            if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
            if(chaos_hole_protected(i) || tri_guarded(i) ||
               (ritual_card_effect_enabled(SHIELD_ID) && marks[i].ritual &&
                marks[i].data==c->data && marks[i].card_id==c->card_id))
                mask|=1u<<i;
        }
        for(i=20;i<25;++i) {
            DuelCardRecord *c=&D_801A7AD8[i];
            if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
            if(chaos_hole_protected(i) || tri_guarded(i) ||
               (ritual_card_effect_enabled(SHIELD_ID) && marks[i].ritual &&
                marks[i].data==c->data && marks[i].card_id==c->card_id))
                mask|=1u<<i;
        }
    }
    /* Never mask a stale record that ceased to be a live field monster. */
    pending=mask;
    while(pending) {
        i=take_mask_bit(&pending);
        if(!monster_slot(i) || !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED))
            mask&=~(1u<<i);
    }
    return mask;
}

static s32 update_effect_requests(void)
{
    u16 saved[DUEL_CARD_RECORD_COUNT];
    unsigned mask,pending;
    int i;
    s32 result;
    if(!board_visual_request_active()) {
        result=original_update_effect_requests();
        chakra_advance_lightning();
        return result;
    }
    mask=board_visual_immune_mask();
    if(!mask) {
        result=original_update_effect_requests();
        chakra_advance_lightning();
        return result;
    }
    memset(saved,0,sizeof(saved));
    pending=mask;
    while(pending) {
        i=take_mask_bit(&pending);
        saved[i]=D_801A7AD8[i].flags;
        D_801A7AD8[i].flags&=~DUEL_CARD_FLAG_OCCUPIED;
    }
    result=original_update_effect_requests();
    pending=mask;
    while(pending) {
        i=take_mask_bit(&pending);
        D_801A7AD8[i].flags=saved[i];
    }
    chakra_advance_lightning();
    return result;
}
static void board_destruction(void)
{
    unsigned old;
    if(!destruction_context_needed()){original_board_destruction();return;}
    old=whale_destruction_guard;
    whale_destruction_guard|=whale_snapshot_protection();
    inside_effect_destruction++;
    original_board_destruction();
    inside_effect_destruction--;
    whale_destruction_guard=old;
}
/* Retail's monster-removal family (including Crush Card) scans one target
 * index per tick, and advances only after it sees that slot empty on its NEXT
 * tick. Simply rejecting RemoveFromField(immune Ultimate) therefore makes it
 * select the SAME surviving monster forever, replaying request 0xB endlessly.
 * Hide only the exact hostile/immune instance for the duration of retail's
 * scanner. On the next update retail sees an empty slot, advances its cursor,
 * and can still destroy any other eligible monster. Never hide/consume a real
 * card outside that synchronous scan. */
static unsigned monster_removal_ultimate_skip_mask(void)
{
    unsigned pending,mask=0;
    if(!ultimate_card_effect.active ||
       ultimate_effect_group(gDuel_wEffectCardID)!=DUEL_EFFECT_GROUP_MONSTER_REMOVAL)
        return 0;
    pending=ultimate_card_effect.mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        DuelCardRecord *card=&D_801A7AD8[i];
        if(monster_slot(i) && (card->flags&DUEL_CARD_FLAG_OCCUPIED) &&
           ultimate_effect_active(i) &&
           card->data==ultimate_card_effect.saved[i].data &&
           side_for_record(i)!=ultimate_card_effect.source_side)
            mask|=1u<<i;
    }
    return mask;
}

static void monster_removal(void)
{
    unsigned old,hidden,pending;
    if(!destruction_context_needed()) {original_monster_removal();return;}
    old=whale_destruction_guard;
    whale_destruction_guard|=whale_snapshot_protection();
    hidden=monster_removal_ultimate_skip_mask();
    pending=hidden;
    while(pending) {
        int i=take_mask_bit(&pending);
        D_801A7AD8[i].flags&=~DUEL_CARD_FLAG_OCCUPIED;
    }
    inside_effect_destruction++;
    original_monster_removal();
    inside_effect_destruction--;
    pending=hidden;
    while(pending) {
        int i=take_mask_bit(&pending);
        D_801A7AD8[i].flags|=DUEL_CARD_FLAG_OCCUPIED;
    }
    whale_destruction_guard=old;
}
static unsigned raigeki_immune_targets(void)
{
    unsigned mask=0,pending;
    int i,source=D_8009B1D5;
    if(source<0 || source>=DUEL_SIDE_COUNT)return 0;
    /* Raigeki remains completely generic. This mask is built from the target
     * cards' own active protections, then the native handler simply sees those
     * exact targets as absent for its one scan. No survivor can make Raigeki
     * stop resolving against a different monster. */
    if(ultimate_card_effect.active)mask|=ultimate_card_effect.mask;
    mask|=whale_destruction_guard;
    for(i=5;i<10;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED) || side_for_record(i)==source)continue;
        if(tri_guarded(i) ||
           (ritual_card_effect_enabled(SHIELD_ID) && marks[i].ritual &&
            marks[i].data==c->data && marks[i].card_id==c->card_id))mask|=1u<<i;
    }
    for(i=20;i<25;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED) || side_for_record(i)==source)continue;
        if(tri_guarded(i) ||
           (ritual_card_effect_enabled(SHIELD_ID) && marks[i].ritual &&
            marks[i].data==c->data && marks[i].card_id==c->card_id))mask|=1u<<i;
    }
    /* Defensive invariant: target-only immunity can never spill onto another
     * live card through a stale bit or a record that changed identity. */
    pending=mask;
    while(pending) {
        i=take_mask_bit(&pending);
        if(!monster_slot(i) || !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) ||
           side_for_record(i)==source)mask&=~(1u<<i);
    }
    return mask;
}

static void raigeki(void)
{
    unsigned old,mask,pending;
    u16 saved[DUEL_CARD_RECORD_COUNT];
    int i;
    if(!destruction_context_needed()){original_raigeki();return;}
    old=whale_destruction_guard;
    whale_destruction_guard|=whale_snapshot_protection();
    mask=raigeki_immune_targets();
    memset(saved,0,sizeof(saved));
    pending=mask;
    while(pending) {
        i=take_mask_bit(&pending);saved[i]=D_801A7AD8[i].flags;
        D_801A7AD8[i].flags&=~DUEL_CARD_FLAG_OCCUPIED;
        if(ultimate_card_effect.active && (ultimate_card_effect.mask&(1u<<i)))
            ultimate_card_effect.touched|=1u<<i;
        if(tri_guarded(i))effect_survivor_visuals|=1u<<i;
        if(marks[i].ritual && marks[i].data==D_801A7AD8[i].data &&
           marks[i].card_id==D_801A7AD8[i].card_id) {
            protected_visuals|=1u<<i;effect_survivor_visuals|=1u<<i;
        }
        if(whale_destruction_guard&(1u<<i))effect_survivor_visuals|=1u<<i;
    }
    inside_effect_destruction++;
    original_raigeki();
    inside_effect_destruction--;
    pending=mask;
    while(pending) {i=take_mask_bit(&pending);D_801A7AD8[i].flags=saved[i];}
    whale_destruction_guard=old;
}

/* Card effects are multi-frame. Refresh only on the tick where the active
 * flag actually clears, i.e. after the spell/effect has finished resolving. */
static s32 update_card_effect(void)
{
    int was_active;
    if(!tri_live_hint && !tri_resolution_active &&
       !(stat_effect_hints&(STAT_HINT_WHALE|STAT_HINT_SHIELD)) &&
       !fiend_mirror_live_hint && !mirror_queue_count && !mirror_reflection.active &&
       !sengenjin_live_hint && !sengenjin_trap.active &&
       !ultimate_live_hint && !ultimate_card_effect.active && !ultimate_ai_magic_attempt.active)
        return original_update_card_effect();
    tri_begin_resolution();
    was_active = (gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE) != 0;
    if(was_active) {
        ultimate_ai_magic_observe_begin();
        ultimate_card_effect_begin();
    }
    if(effect_is_trap())sengenjin_trap_begin();
    s16 old_atk[DUEL_CARD_RECORD_COUNT], old_def[DUEL_CARD_RECORD_COUNT], old_mod[DUEL_CARD_RECORD_COUNT];
    int side,k;
    int whale_guard = (stat_effect_hints & STAT_HINT_WHALE) && was_active && gDuel_bTerrain==UMI_TERRAIN && effect_is_magic_or_trap();
    unsigned whale_sides=0;
    if(whale_guard) {
        if(whale_live_mask & side_monster_mask(0)) whale_sides|=1u;
        if(whale_live_mask & side_monster_mask(1)) whale_sides|=2u;
        for(side=0;side<DUEL_SIDE_COUNT;++side) if(whale_sides&(1u<<side)) {
            int start=side?20:5;
            for(k=start;k<start+5;++k){old_atk[k]=D_801A7AD8[k].attack;old_def[k]=D_801A7AD8[k].defense;old_mod[k]=D_801A7AD8[k].stat_modifier;}
        }
    }
    s32 result = original_update_card_effect();
    if(ultimate_card_effect.active)
        ultimate_card_effect_restore(!(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE));
    if(was_active && !(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE))
        ultimate_ai_magic_observe_finish();
    if(sengenjin_trap.active)
        sengenjin_trap_restore(!(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE));
    if(whale_guard) for(side=0;side<DUEL_SIDE_COUNT;++side) if(whale_sides&(1u<<side)) {
        int start=side?20:5;
        unsigned sources=valid_whale_sources_for_side(side);
        if(!sources) continue;
        for(k=start;k<start+5;++k){DuelCardRecord*c=&D_801A7AD8[k]; if((c->flags&DUEL_CARD_FLAG_OCCUPIED)&&(sources&~(1u<<k))&&sea_family_type(card_type_id(c))){if(c->attack<old_atk[k])c->attack=old_atk[k];if(c->defense<old_def[k])c->defense=old_def[k];if(c->stat_modifier<old_mod[k])c->stat_modifier=old_mod[k];}}
    }
    mirror_watch_lp();
    if (was_active && !(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        restore_protected_visuals();
        reread_all_shields("pos-magia");
        if(tri_seen){tri_resolution_active=0;tri_guard=0;memset(tri_guard_data,0,sizeof(tri_guard_data));}
    }
    return result;
}

/* Black Chaos start-of-turn effects own the small window between the turn
 * switch and the native draw phase. Do not lie to the global effect-state
 * dispatcher: Black Hole, large-card presentation and every other native
 * effect must be allowed to report their real completion state.
 *
 * Instead, hold only phase 2 (DrawPhase) while the Chaos queue is active.
 * The outer DuelScene_Update still services effect requests/card effects/effect
 * states normally every frame; it simply cannot rebuild the hand or advance to
 * draw resolution until every queued Chaos has finished. */
static void draw_phase(void)
{
    if (chaos_turn.active) return;
    original_draw_phase();
}

static int side_has_ritual_fiend_mirror(int side)
{
    unsigned pending;
    if (!fiends_mirror_effect_on) return 0;
    pending=fiend_mirror_live_mask & side_monster_mask(side);
    while(pending) {
        int i=take_mask_bit(&pending); DuelCardRecord *c=&D_801A7AD8[i];
        /* Battle survival is represented by mirror_battle_grace, snapshotted
         * before retail stages/deactivates the records.  Never treat a stale
         * Ritual mark itself as a live Mirror during battle. */
        if (!fiend_mirror_marks[i].ritual || !(c->flags & DUEL_CARD_FLAG_OCCUPIED) ||
            c->card_id != FIENDS_MIRROR_ID || c->data != fiend_mirror_marks[i].data) {
            /* FieldActions has already deactivated both battle records when it
             * hands control to phase 9.  Preserve a Ritual Mirror's mark long
             * enough for battle() to snapshot mirror_battle_grace; it is not
             * considered a live field Mirror in this transient state. */
            if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
               fiend_mirror_marks[i].ritual &&
               c->card_id==FIENDS_MIRROR_ID && c->data &&
               c->data==fiend_mirror_marks[i].data &&
               battle_participant(i))continue;
            fiend_mirror_marks[i].ritual=0; fiend_mirror_marks[i].data=0;
            fiend_mirror_live_mask&=~(1u<<i);
            continue;
        }
        return 1;
    }
    return 0;
}

static void mirror_finish_reflection(void)
{
    if (!mirror_reflection.active) return;
    /* The request owns the burn presentation.  Only after it releases ACTIVE
     * do we commit the reflected LP loss, matching the requested visual order. */
    if (mirror_reflection.request &&
        (mirror_reflection.request->flags & DUEL_EFFECT_REQUEST_FLAG_ACTIVE)) {
        mirror_reflection.seen_active = 1;
        return;
    }
    /* CreateRequest first queues the visual and only afterwards marks the
     * request ACTIVE.  Waiting until ACTIVE has actually been observed keeps
     * our dynamic x10 value installed while the number is being built. */
    if (!mirror_reflection.seen_active) return;
    if (host && host->log)
        host->log(host, "[Ritual Card Effects] Fiend's Mirror terminou animacao de %u LP no lado %d",
                  mirror_reflection.damage, mirror_reflection.target_side);
    memset(&mirror_reflection, 0, sizeof(mirror_reflection));
}

static void mirror_begin_reflection(int target_side, unsigned damage, int prepaid)
{
    DuelEffectRequest *request;
    unsigned magnitude;
    u16 life;
    if (!damage || mirror_reflection.active) return;

    /* Effect request 2 is the retail battle-damage number presentation.  In
     * contrast with burn request 6 (whose number is selected from the five
     * fixed Magic values by field_1A), request 2 receives the exact amount in
     * field_12.  That makes the Mirror presentation truly dynamic. */
    request = DuelEffect_CreateRequest(2);
    if (!request) {
        if(!prepaid) {
            life = D_800E9FF0[target_side].life_points.unsigned_value;
            D_800E9FF0[target_side].life_points.unsigned_value =
                (damage >= life) ? 0 : (u16)(life - damage);
        }
        return;
    }
    request->field_00 = 0xA0;
    request->field_02 = 0x78;
    request->field_12 = (s16)damage;
    magnitude = damage / 1000;
    if (magnitude >= 3) magnitude = 2;
    request->field_1A = (s16)magnitude;
    SD_SEPlayFull(0x1C);

    mirror_reflection.active = 1;
    mirror_reflection.seen_active = 0;
    mirror_reflection.target_side = target_side;
    mirror_reflection.damage = damage;
    mirror_reflection.request = request;

    /* Keep the v0.25 mechanical timing: LP starts falling together with the
     * presentation, not after an additional post-animation delay. */
    if(!prepaid) {
        life = D_800E9FF0[target_side].life_points.unsigned_value;
        D_800E9FF0[target_side].life_points.unsigned_value =
            (damage >= life) ? 0 : (u16)(life - damage);
    }
    mirror_last_lp[0] = D_800E9FF0[0].life_points.unsigned_value;
    mirror_last_lp[1] = D_800E9FF0[1].life_points.unsigned_value;
}

static void mirror_queue_hit(int target_side, unsigned damage, int source_index)
{
    if (!damage) return;
    if (mirror_queue_count >= MIRROR_QUEUE_CAP) {
        unsigned newest = (mirror_queue_tail + MIRROR_QUEUE_CAP - 1) % MIRROR_QUEUE_CAP;
        mirror_queue[newest].damage += damage;
        return;
    }
    mirror_queue[mirror_queue_tail].target_side = target_side;
    mirror_queue[mirror_queue_tail].damage = damage;
    mirror_queue[mirror_queue_tail].source_index = source_index;
    mirror_queue[mirror_queue_tail].visual_queued = 0;
    mirror_queue[mirror_queue_tail].prepaid = 0;
    mirror_queue_tail = (mirror_queue_tail + 1) % MIRROR_QUEUE_CAP;
    ++mirror_queue_count;
}

static void mirror_queue_prepaid(int target_side,unsigned damage,int source_index)
{
    unsigned old_tail=mirror_queue_tail, old_count=mirror_queue_count;
    /* A full queue may coalesce hits, which would convert a prepaid loss
     * into an unpaid one and deduct LP twice. Drop only this cosmetic event. */
    if(mirror_queue_count>=MIRROR_QUEUE_CAP)return;
    mirror_queue_hit(target_side,damage,source_index);
    if(mirror_queue_count>old_count)
        mirror_queue[old_tail].prepaid=1;
}

static void mirror_watch_lp(void)
{
    u16 now0, now1, before[2], now[2];
    int side;
    /* A Mirror killed by the same battle that caused LP loss is no longer in
     * the live mask, but mirror_battle_grace deliberately keeps that one final
     * reflection legal until phase 9 ends.  Never let the fast-path gate hide
     * that last-sigh loss. */
    if(!fiend_mirror_live_hint && !mirror_battle_grace[0] && !mirror_battle_grace[1] &&
       !mirror_queue_count && !mirror_reflection.active)return;
    mirror_finish_reflection();
    now0 = D_800E9FF0[0].life_points.unsigned_value;
    now1 = D_800E9FF0[1].life_points.unsigned_value;
    if (!mirror_lp_ready) {
        mirror_last_lp[0] = now0; mirror_last_lp[1] = now1; mirror_lp_ready = 1;
        return;
    }
    before[0] = mirror_last_lp[0]; before[1] = mirror_last_lp[1];
    now[0] = now0; now[1] = now1;
    for (side = 0; side < 2; ++side) {
        if (mirror_commit_pending[side]) {
            if (now[side] == mirror_commit_before[side]) {
                /* Retail has not written the event result yet. Never rebase
                 * from the event's post-damage amount to its old LP. */
                continue;
            }
            if (now[side] == mirror_commit_after[side]) {
                /* The event already queued the reflection, if warranted. */
                mirror_commit_pending[side] = 0;
                continue;
            }
            /* The engine wrote another LP value before we observed its
             * expected result: fall back to the actual live delta. */
            mirror_commit_pending[side] = 0;
        }
        if (now[side] < before[side] && now[side] > 0 &&
            (side_has_ritual_fiend_mirror(side) || mirror_battle_grace[side])) {
            unsigned loss = (unsigned)(before[side] - now[side]);
            /* A lethal hit ends the duel normally: Fiend's Mirror does not
             * create a posthumous reflection when its controller reaches 0 LP. */
            mirror_queue_hit(side ^ 1, loss, visual_find_live_mirror(side));
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Fiend's Mirror enfileirou perda de %u LP (lado %d -> %d)", loss, side, side ^ 1);
        }
    }
    for (side = 0; side < 2; ++side)
        if (!mirror_commit_pending[side]) mirror_last_lp[side] = now[side];
    if (!mirror_reflection.active && mirror_queue_count &&
        (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9) {
        MirrorQueuedHit *hit = &mirror_queue[mirror_queue_head];
        /* If the Mirror survived, show its field activation first.  Last-sigh
         * reflections deliberately skip the marker because no live field card
         * exists to mark; their proven reflection semantics remain untouched. */
        if (hit->source_index >= 0 && !hit->visual_queued) {
            if (visual_queue_activation(hit->source_index)) hit->visual_queued = 1;
        }
        if (hit->source_index < 0 || (hit->visual_queued && !visual_activation_busy())) {
            int target_side = hit->target_side; unsigned damage = hit->damage;
            int prepaid=hit->prepaid;
            mirror_queue_head = (mirror_queue_head + 1) % MIRROR_QUEUE_CAP; --mirror_queue_count;
            mirror_begin_reflection(target_side, damage, prepaid);
            if (host && host->log) host->log(host, "[Ritual Card Effects] Fiend's Mirror iniciou reflexo de %u LP; fila restante=%u", damage, mirror_queue_count);
        }
    }
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9) {
        mirror_battle_grace[0] = 0; mirror_battle_grace[1] = 0;
    }
}
static void turn_switch(void)
{
    u8 before = D_8009B1D5;
    /* Permanent theft while Puppet lives. If Puppet died this turn, give
     * back only the surviving marionette at the END of the turn (not at the
     * instant of death). Both destroyed by Raigeki means nothing to return. */
    psycho_reconcile_controlled();
    psycho_return_after_master_death();
    if(javelin_live_mask) {
        unsigned pending=javelin_live_mask;
        while(pending) { int i=take_mask_bit(&pending); if(javelin_marks[i].ritual) { javelin_marks[i].protection_used_turn=0; javelin_marks[i].protection_visual_shown=0; } }
    }
    if(gate_live_mask) {
        unsigned pending=gate_live_mask;
        gate_state11_counted_mask=0;
        while(pending) { int i=take_mask_bit(&pending); if(gate_marks[i].ritual){gate_marks[i].attacks_this_turn=0;gate_marks[i].extra_attack_pending=0;} }
    }
    if(garma_live_mask) {
        unsigned pending=garma_live_mask;
        while(pending) { int i=take_mask_bit(&pending); garma_marks[i].extra_attack_ready=0; garma_marks[i].extra_attack_pending=0; garma_marks[i].strike_bonus=0; garma_marks[i].final_attack_cleanup=0; }
    }
    original_turn_switch();
    mirror_watch_lp();
    if(D_8009B1D5!=before && D_8009B1D5<DUEL_SIDE_COUNT)
        cosmo_turn_fulfilled[D_8009B1D5]=0;
    if (D_8009B1D5 != before && (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 2) {
        if(coin_marks_seen)chaos_begin_turn(D_8009B1D5);
        else memset(&chaos_turn,0,sizeof(chaos_turn));
        if(yamadron_live_mask)generate_for_side(D_8009B1D5, "inicio-do-turno");
        if(psycho_mark_mask)psycho_trigger_for_side(D_8009B1D5, "inicio-do-turno");
    }
}

static void reset_card_records(void)
{
    /* Game > Restart game longjmps back to the title without loading a saved
     * state, so a mod reset callback is not guaranteed before the next duel's
     * records are rebuilt.  Kill Ultimate's in-flight snapshot before retail
     * clears/reuses those records; otherwise an interrupted immunity restore
     * can write the previous duel's Ultimate into the new duel. */
    memset(&ai_position_diag,0,sizeof(ai_position_diag));
    memset(chakra_marks,0,sizeof(chakra_marks));chakra_live_mask=chakra_pending_mask=0;memset(&chakra_sequence,0,sizeof(chakra_sequence));
    memset(ultimate_marks,0,sizeof(ultimate_marks));
    ultimate_live_mask=0;
    ultimate_live_hint=0;
    memset(&ultimate_card_effect,0,sizeof(ultimate_card_effect));
    memset(ritual_ai_effect_known,0,sizeof(ritual_ai_effect_known));
    original_reset_card_records();
    native_vfx_upload();
}

static void populate(void)
{
    clear();
    native_vfx_upload();
    original_populate();
}
static void applied(int on)
{ recipe_export_active=on!=0; sync_global_settings(); refresh_settings(); clear(); selection_connect(); }

/* Attack Traps are selected entirely by retail, including when the CPU
 * already learned Ultimate's immunity. Learning never suppresses the
 * owner's activated Trap: it still reveals and is consumed as normal. */

static s32 test_ai_turn_action(void)
{
    if(test_cpu_prioritize_traps && D_8009B1D5==1 && Duel_SelectTrapPlay()==0)
        return 0;
    return original_ai_turn_action();
}

/* Reset deliberately clears classification: v0.1 requires a fresh ritual
 * after loading a save state. No unsaved pointer state is inferred/restored. */
static void overlay(void)
{
    /* Debug caption disabled for public gameplay; Ultimate AI unchanged. */
    int ultimate_swords_debug=0;
    /* When this mod has no visible overlay component, avoid even entering the
     * individual render helpers. The host cache will normally skip this call
     * as well, but this keeps the callback itself cheap on every backend. */
    if(!ritual_coin.active && !(coin_battle.dodge_object && coin_battle.dodge_ticks>=12) &&
       !serpent_ghost_state && !serpent_redirect_state &&
       !(visual_popup_frames>0 && visual_popup_kind==STAT_VISUAL_NORMAL) &&
       !ultimate_swords_debug && !(diagnostic_ai_positions && ai_position_diag.valid) &&
       !diagnostic_skull_ritual && !diagnostic_effect_titles)return;
    if(ritual_coin.active)ritual_coin_overlay();
    if(coin_battle.dodge_object && coin_battle.dodge_ticks>=12)performance_dodge_flash();
    if(serpent_ghost_state==2)serpent_ghost_draw();
    if(serpent_redirect_state==2)serpent_redirect_draw();
    if (host && host->draw_text) {
        if(diagnostic_effect_titles && host->overlay_size) {
            int width,height,scale;
            char line[144];
            host->overlay_size(host,&width,&height,&scale);
            if(scale<1)scale=1;
            snprintf(line,sizeof(line),"TITLE id:%d mode:%d slot:%d armed:%d active:%d",
                title_color_trace.id,title_color_trace.mode,title_color_trace.slot,
                title_color_trace.armed,title_color_trace.active);
            host->draw_text(host,12,12,line,0xffd866,scale);
            snprintf(line,sizeof(line),"DRAW ch:%d calls:%u matched:%u color:%d",
                title_color_trace.channel,title_color_trace.calls,
                title_color_trace.heading_matches,title_color_trace.text_color);
            host->draw_text(host,12,12+14*scale,line,0xffffff,scale);
        }
        if(diagnostic_skull_ritual && host->overlay_size) {
            int width,height,scale,vw,vh,left,top;
            char line[108];
            host->overlay_size(host,&width,&height,&scale);
            vw=width;vh=width*3/4;
            if(vh>height){vh=height;vw=height*4/3;}
            left=(width-vw)/2;top=(height-vh)/2;
            if(scale<1)scale=1;
            snprintf(line,sizeof(line),"SKULL RITUAL DIAG  #%u",skull_ritual_diag.attempts);
            host->draw_text(host,left+8*vw/320,top+6*vh/240,line,0xffe38f,scale);
            snprintf(line,sizeof(line),"NOVOX 679 -> 704  ON:%d ROW:%d",
                recipe_settings[16],skull_ritual_diag.row_found);
            host->draw_text(host,left+8*vw/320,top+17*vh/240,line,0xffffff,scale);
            snprintf(line,sizeof(line),"STEP:%d RESULT:%d S:%d PH:%d",
                skull_ritual_diag.stage,skull_ritual_diag.retail_result,
                skull_ritual_diag.side,skull_ritual_diag.phase);
            host->draw_text(host,left+8*vw/320,top+28*vh/240,line,0xffffff,scale);
            snprintf(line,sizeof(line),"MATCH F+H: %u / %u / %u",
                skull_ritual_diag.matches[0],skull_ritual_diag.matches[1],
                skull_ritual_diag.matches[2]);
            host->draw_text(host,left+8*vw/320,top+39*vh/240,line,0xbdeeff,scale);
            snprintf(line,sizeof(line),"ROW: %u / %u / %u  5=ORACLE OK",
                skull_ritual_diag.materials[0],skull_ritual_diag.materials[1],
                skull_ritual_diag.materials[2]);
            host->draw_text(host,left+8*vw/320,top+50*vh/240,line,0xbdeeff,scale);
            snprintf(line,sizeof(line),"DECK REMAIN: %u MATCH:%u/%u/%u",
                skull_ritual_diag.deck_candidates,skull_ritual_diag.deck_matches[0],
                skull_ritual_diag.deck_matches[1],skull_ritual_diag.deck_matches[2]);
            host->draw_text(host,left+8*vw/320,top+61*vh/240,line,0xbdeeff,scale);
            snprintf(line,sizeof(line),"ORACLE: CHECK:%u OK:%u FAIL:%u",
                skull_ritual_diag.oracle_checks,skull_ritual_diag.oracle_accepted,
                skull_ritual_diag.oracle_rejected);
            host->draw_text(host,left+8*vw/320,top+72*vh/240,line,0xffe38f,scale);
            snprintf(line,sizeof(line),"ORACLE LAST RESULT:%d (704=OK)",skull_ritual_diag.last_oracle_result);
            host->draw_text(host,left+8*vw/320,top+83*vh/240,line,0xffffff,scale);
            snprintf(line,sizeof(line),"PROBES: %u  SYNTHETIC: %u",
                skull_ritual_diag.oracle_candidates_seen,skull_ritual_diag.oracle_signature_hits);
            host->draw_text(host,left+8*vw/320,top+94*vh/240,line,0xffe38f,scale);
            snprintf(line,sizeof(line),"LAST TRIO: %u / %u / %u",
                skull_ritual_diag.oracle_last_trio[0],skull_ritual_diag.oracle_last_trio[1],skull_ritual_diag.oracle_last_trio[2]);
            host->draw_text(host,left+8*vw/320,top+105*vh/240,line,0xbdeeff,scale);
            snprintf(line,sizeof(line),"SLOTS A/B/C MASK: %u / %u / %u",
                skull_ritual_diag.oracle_requirement_masks[0],
                skull_ritual_diag.oracle_requirement_masks[1],
                skull_ritual_diag.oracle_requirement_masks[2]);
            host->draw_text(host,left+8*vw/320,top+116*vh/240,line,0xbdeeff,scale);
        }
        if(diagnostic_ai_positions && ai_position_diag.valid && host->overlay_size) {
            int width,height,scale,vw,vh,left,top;
            char line[112];
            host->overlay_size(host,&width,&height,&scale);vw=width;vh=width*3/4;
            if(vh>height){vh=height;vw=height*4/3;}
            left=(width-vw)/2;top=(height-vh)/2;if(scale<1)scale=1;
            snprintf(line,sizeof(line),"AI POS #%u PH%d CPU%d",ai_position_diag.count,ai_position_diag.phase,ai_position_diag.turn);
            host->draw_text(host,left+8*vw/320,top+5*vh/240,line,0xffe48c,scale);
            snprintf(line,sizeof(line),"PLAYED %d ATK %d DEF %d",ai_position_diag.own_id,ai_position_diag.own_atk,ai_position_diag.own_def);
            host->draw_text(host,left+8*vw/320,top+17*vh/240,line,0xffffff,scale);
            snprintf(line,sizeof(line),"VS %d ATK %d DEF %d DIFF %d",ai_position_diag.opp_id,ai_position_diag.opp_atk,ai_position_diag.opp_def,ai_position_diag.opp_atk-ai_position_diag.own_atk);
            host->draw_text(host,left+8*vw/320,top+29*vh/240,line,0xffa0a0,scale);
        }
        if(ultimate_swords_debug && host->overlay_size) {
            int width,height,scale,vw,vh,left,top;
            char label[64];
            host->overlay_size(host,&width,&height,&scale);
            vw=width;vh=width*3/4;
            if(vh>height){vh=height;vw=height*4/3;}
            left=(width-vw)/2;top=(height-vh)/2;
            if(scale<1)scale=1;
            snprintf(label,sizeof(label),"Ult/Swd AI %u/%u R%d F%d",
                     ultimate_swords_diag_scans,ultimate_swords_diag_issued,
                     ultimate_swords_diag_last_result,D_8009B174&0xf);
            host->draw_text(host,left+76*vw/320,top+8*vh/240,label,0xeeeeff,scale);
        }
        if (visual_popup_frames > 0 && host->overlay_size) {
            int width,height,scale,vw,vh,left,top,x,y,brightness;
            host->overlay_size(host,&width,&height,&scale); vw=width; vh=width*3/4;
            if(vh>height){vh=height;vw=height*4/3;} left=(width-vw)/2; top=(height-vh)/2;
            if(visual_popup_kind==STAT_VISUAL_NORMAL) {
                char text[16];
                x=left+visual_popup_x*vw/320; y=top+(visual_popup_y-(STAT_POPUP_FRAMES-visual_popup_frames)/2)*vh/240;
                if(scale<1)scale=1; brightness=visual_popup_frames*255/STAT_POPUP_FRAMES; snprintf(text,sizeof(text),"+%d",visual_popup_amount);
                host->draw_text(host,x+scale,y+scale,text,0x000000,scale);
                host->draw_text(host,x,y,text,(unsigned)brightness<<8,scale);
            }
        }
        /* Garma/Javelin/Tri counter sprites now render as native card packets. */
    }
}

/* The public overlay cache can retain an identical picture between frames.
 * In particular the middle of the 32-tick ghost has constant alpha, so it
 * need not rasterize the same artwork again every tick. Hash everything the
 * overlay reads: counters, identity/positions, cursor/modal gates and settings.
 * Native game drawing continues normally; only this mod's overlay is cached. */
static unsigned overlay_hash(unsigned hash, const void *data, unsigned bytes)
{
    const unsigned char *p=(const unsigned char *)data;
    while(bytes--)hash=(hash^*p++)*16777619u;
    return hash;
}

static unsigned overlay_signature(void)
{
    unsigned hash=2166136261u;
    /* Debug caption disabled for public gameplay; Ultimate AI unchanged. */
    int ultimate_swords_debug=0;
    /* overlay_signature is queried every frame even when the mod draws
     * nothing. Keep ordinary duel frames at one stable constant signature;
     * once any component can become visible, fall through to the exact state
     * hashing below. */
    if(!ritual_coin.active && !coin_battle.dodge_ticks && !serpent_ghost_state &&
       !serpent_redirect_state && !(visual_popup_frames>0 && visual_popup_kind==STAT_VISUAL_NORMAL) &&
       !ultimate_swords_debug &&
       !(diagnostic_ai_positions && ai_position_diag.valid) &&
       !diagnostic_skull_ritual && !diagnostic_effect_titles)return hash;
    if(diagnostic_effect_titles)
        hash=overlay_hash(hash,&title_color_trace,sizeof(title_color_trace));
    unsigned visible[8]={0,0,0,0,0,0,0,0};
    int coin_port=0;

    /* Hash only state that can change pixels produced by this mod. When no
     * overlay component is visible the signature remains constant, allowing
     * the host to keep one cached transparent overlay throughout ordinary
     * duel frames instead of invalidating it for unrelated game state. */
    if(ritual_coin.active) {
        coin_port=ritual_coin_port_ui();
        visible[0]=coin_port?0u:1u;
    }
    visible[1]=(coin_battle.dodge_object && coin_battle.dodge_ticks>=12)?1u:0u;
    visible[2]=(serpent_ghost_state==2 && serpent_art_loaded==1 && serpent_ghost_fade()>0)?1u:0u;
    visible[3]=(serpent_redirect_state==2)?1u:0u;
    visible[4]=(visual_popup_frames>0 && visual_popup_kind==STAT_VISUAL_NORMAL)?1u:0u;
    visible[5]=diagnostic_skull_ritual?1u:0u;
    visible[6]=ultimate_swords_debug?1u:0u;
    visible[7]=(diagnostic_ai_positions && ai_position_diag.valid)?1u:0u;

    hash=overlay_hash(hash,visible,sizeof(visible));
    if(visible[5])hash=overlay_hash(hash,&skull_ritual_diag,sizeof(skull_ritual_diag));

    if(visible[0]) {
        unsigned coin_state[]={ (unsigned)ritual_coin.style,(unsigned)ritual_coin.shown,(unsigned)ritual_coin.lift };
        hash=overlay_hash(hash,coin_state,sizeof(coin_state));
    }
    if(visible[1]) {
        int dodge_state[]={coin_battle.dodge_ticks,
            coin_battle.dodge_object->field_30.h.field_30,
            coin_battle.dodge_object->field_30.h.field_32};
        hash=overlay_hash(hash,dodge_state,sizeof(dodge_state));
    }
    if(visible[2]) {
        int ghost_state[]={serpent_ghost_x,serpent_ghost_y,serpent_ghost_fade()};
        hash=overlay_hash(hash,ghost_state,sizeof(ghost_state));
    }
    if(visible[3]) {
        int redirect_alpha_step=serpent_redirect_hold<6?serpent_redirect_hold:6;
        hash=overlay_hash(hash,&redirect_alpha_step,sizeof(redirect_alpha_step));
    }
    if(visible[4]) {
        int popup_state[]={visual_popup_frames,visual_popup_x,visual_popup_y,visual_popup_amount,visual_popup_kind};
        hash=overlay_hash(hash,popup_state,sizeof(popup_state));
    }
    if(visible[7])hash=overlay_hash(hash,&ai_position_diag.own_id,
        (unsigned)((char *)&ai_position_diag.count+sizeof(ai_position_diag.count)-(char *)&ai_position_diag.own_id));
    if(visible[6]) {
        unsigned counters[]={ultimate_swords_diag_scans,ultimate_swords_diag_issued,
            (unsigned)ultimate_swords_diag_last_result,(unsigned)(D_8009B174&0xf)};
        hash=overlay_hash(hash,counters,sizeof(counters));
    }
    return hash;
}

static void shutdown(void)
{
    selection_ready=0;
    if(host&&host->unsubscribe&&ritual_damage_token){host->unsubscribe(host,ritual_damage_token);ritual_damage_token=0;}
    if(host&&host->unsubscribe&&ritual_equip_token){host->unsubscribe(host,ritual_equip_token);ritual_equip_token=0;}
    if (host && host->unhook)
        while (token_count) host->unhook(host, tokens[--token_count]);
    clear();
    ritual_coin_free();
}

static int install(void *function, void *replacement, void **original)
{
    int token;
    if (!function || token_count >= (int)(sizeof(tokens)/sizeof(tokens[0]))) return 0;
    token = host->hook(host, function, replacement, original);
    if (!token) return 0;
    tokens[token_count++] = token;
    return *original != 0;
}

/* Only this MOD owns <Ritual Effect>. The native blue heading fades to
 * grey until its CURRENT field instance has armed its existing controller.
 * For non-duel card viewers it stays blue, as requested. */
static int ritual_title_last_record(void)
{
    /* Match the actual instance selected by the native field cursor,
     * not a same-ID card left in field_info_cache or on the board. */
    return fmr_title_selected_field_record(gDuel_wViewerCardID);
}
static int ritual_title_active(int id,int i)
{
    DuelCardRecord *r;
    if(i<0 || i>=DUEL_CARD_RECORD_COUNT || !monster_slot(i) ||
       !ritual_card_effect_enabled(id))return 0;
    r=&D_801A7AD8[i];
    if(r->card_id!=id || !(r->flags&DUEL_CARD_FLAG_OCCUPIED) ||
       (r->flags&DUEL_CARD_FLAG_FACE_DOWN))return 0;
    if(!test_ignore_ritual_requirement && !borrowed_effect_armed(i))return 0;
    /* Charges/one-use activation are queried from their EXISTING gameplay
     * ledgers: the viewer must never create its own usage counters. */
    switch(id) {
    case CHAKRA_ID: return 0; /* On Ritual Summon effect consumed. */
    case JAVELIN_BEETLE_ID:
        return !javelin_marks[i].protection_used_turn;
    case GATE_GUARDIAN_ID:
        return gate_marks[i].attacks_this_turn<3;
    case GARMA_SWORD_ID:
        return garma_marks[i].blades_used<5;
    case TRI_HORNED_ID:
        return tri_marks[i].horns>0;
    default:return 1;
    }
}
static void ritual_viewer_effect_title_draw(DisplayObject *obj,GsOT *ot)
{
    FmrTitleTint tint={0};
    DuelEffectChannel *box=fmr_title_draw_channel(obj,D_8009B250);
    int id=gDuel_wViewerCardID;
    if(obj && ot && box &&
       ritual_effect_index_for_card(id)>=0) {
        int duel=fmr_title_is_live_duel();
        int active= !duel || ritual_title_active(id,ritual_title_last_record());
        fmr_title_tint_effect(&tint,box,"<Ritual Effect>",2,active);
        if(!duel && box==D_8009B250 && tint.count==0)
            fmr_title_tint_first_line(&tint,box,2);
        if(diagnostic_effect_titles && id==BLUE_EYES_ULTIMATE_ID) {
            int record=ritual_title_last_record();
            title_color_trace.id=id;
            title_color_trace.mode=(int)(D_8009B26C[0]&31);
            title_color_trace.slot=record;
            title_color_trace.armed=(record>=0 && monster_slot(record)) ?
                borrowed_effect_armed(record) : 0;
            title_color_trace.active=active;
            title_color_trace.channel=(int)obj->field_67;
            title_color_trace.text_color=tint.count ? tint.entry[0]->field_16 : -1;
            ++title_color_trace.calls;
            title_color_trace.heading_matches+=(unsigned)tint.count;
        }
    }
    ritual_viewer_fit_draw(obj,ot);
    fmr_title_restore(&tint);
}

/* Optional, read-only stance rule for other effects selecting a DEF target. */
typedef struct {unsigned abi;int (*can_defend)(int);} RitualPositionRulesV1;
#include "owner_r.h"
#define EA_HOST host
#define EA_TERRAIN gDuel_bTerrain
#define EA_RITUAL_OWNER 1
#include "native.h"
#include "native_actions.h"
static int ritual_can_defend(int slot){return !recipe_export_active || !monster_slot(slot) || !chaos_attack_only(slot);}
static const RitualPositionRulesV1 ritual_position_api={1,ritual_can_defend};

static s32 selection_ai_run(void)
{ selection_connect();return serpent_ai_run(); }

int MemoriesModInit(const MemoriesModHost *from, MemoriesMod *mod)
{
    if (!from || !mod || from->api < 4 || !from->hook || !from->unhook) return 0;
    host = from;
    compat_lookup_recipe=compat_private_recipe;
    compat_lookup_requirements=compat_private_requirements;
    sync_global_settings();
    refresh_settings();
    clear();
    /* Garma's legacy icon runs now source its native texture. */
    garma_icon_prepare();
    if(!native_vfx_upload()){shutdown();return 0;}
    /* Other icon tables remain available for future native variants. */
    tri_icon_prepare();
    javelin_icon_prepare();
    /* Tri/Javelin/Hungry pilot visuals are native ordering-table packets now. */
    if(!host->subscribe||!host->unsubscribe){shutdown();return 0;}
    ritual_damage_token=host->subscribe(host,MEMORIES_EVENT_DAMAGE,-10000,coin_damage);
    if(!ritual_damage_token){shutdown();return 0;}
    ritual_equip_token=host->subscribe(host,MEMORIES_EVENT_EQUIP,500,mask_equip_permission);
    if(!ritual_equip_token){shutdown();return 0;}
    ritual_coin_load(0);ritual_coin_load(1);
    if(!ritual_coin_sets[0].count||!ritual_coin_sets[1].count){shutdown();return 0;}
    ritual_port_menu=(int (*)(void))host->symbol(host,"Menu_IsOpen");
    ritual_port_notice=(int (*)(void))host->symbol(host,"Menu_NoticeShown");
    if (!install((void *)Input_UpdatePads,(void *)coin_update_pads,(void **)&ritual_original_pads) ||
        !install((void *)Main_RunDuel,(void *)coin_run_duel,(void **)&ritual_original_run) ||
        !install((void *)Main_RunAnimatedBattle,(void *)coin_animated_battle,(void **)&ritual_original_animated) ||
        !install((void *)func_800559D4,(void *)coin_model_control,(void **)&ritual_original_model_control) ||
        !install((void *)DuelCard_UpdateDefenseRotation,(void *)chaos_defense_rotation,(void **)&original_defense_rotation) ||
        !install((void *)func_80017F04, (void *)serpent_stage_card, (void **)&serpent_original_stage) ||
        !install((void *)AiScript_Run, (void *)selection_ai_run, (void **)&serpent_original_ai_run) ||
        !install((void *)AiScript_FindWeakest, (void *)serpent_ai_find_weakest, (void **)&serpent_original_ai_weakest) ||
        !install((void *)Ai_IsCardInSets, (void *)serpent_ai_in_sets, (void **)&serpent_original_ai_in_sets) ||
        !install((void *)AiScript_FindBestAttack, (void *)serpent_ai_best_attack, (void **)&serpent_original_ai_best_attack) ||
        !install((void *)func_80037DA4, (void *)ritual_viewer_fit_command, (void **)&ritual_viewer_original_command) ||
        !install((void *)func_80035E20, (void *)ritual_viewer_effect_title_draw, (void **)&ritual_viewer_original_draw) ||
        !install((void *)func_80027508, (void *)test_ai_turn_action, (void **)&original_ai_turn_action) ||
        !install((void *)func_80024D34, (void *)puppet_safe_rebuild, (void **)&original_rebuild_field_card) ||
        !install((void *)DuelScene_Update, (void *)scene_update, (void **)&original_scene_update) ||
        !install((void *)func_80023144, (void *)field_card_info, (void **)&original_field_card_info) ||
        !install((void *)Duel_DrawFieldCards, (void *)chakra_draw_field_cards, (void **)&original_field_card_draw) ||
        !install((void *)func_80016784, (void *)visual_card_frame_draw, (void **)&original_card_frame_draw) ||
        !install((void *)DuelEffect_ApplyRitual, (void *)apply_ritual, (void **)&original_apply) ||
        !install((void *)Duel_CalcCardStats, (void *)calc_stats, (void **)&original_stats) ||
        !install((void *)Duel_CalcBattleAttack, (void *)battle_attack, (void **)&original_battle_attack) ||
        !install((void *)Duel_CalcBattleDefense, (void *)battle_defense, (void **)&original_battle_defense) ||
        !install((void *)Duel_CalcGuardianStarBonus, (void *)guardian_star_bonus, (void **)&original_guardian_star_bonus) ||
        !install((void *)func_8001EFD4, (void *)sengenjin_battle_compare, (void **)&original_battle_compare) ||
        !install((void *)func_8001B170, (void *)sengenjin_reverse_trap_step, (void **)&original_func_8001B170) ||
        !install((void *)DuelEffect_ApplyStatPenalty, (void *)sengenjin_track_stat_penalty, (void **)&original_stat_penalty) ||
        !install((void *)DuelEffect_ApplyCursebreaker, (void *)sengenjin_track_cursebreaker, (void **)&original_cursebreaker) ||
        !install((void *)DuelCard_RemoveFromField, (void *)remove_card, (void **)&original_remove) ||
        !install((void *)DuelCard_DeactivateRecord, (void *)deactivate_card, (void **)&original_deactivate) ||
        !install((void *)Duel_ResetCardRecords, (void *)reset_card_records, (void **)&original_reset_card_records) ||
        !install((void *)Duel_PopulateCombinedDeckData, (void *)populate, (void **)&original_populate) ||
        !install((void *)DuelScene_UpdateFieldActions, (void *)field_actions, (void **)&original_field_actions) ||
        !install((void *)DuelScene_UpdateCardPlacement, (void *)card_placement, (void **)&original_card_placement) ||
        !install((void *)DuelScene_UpdateHandActions, (void *)hand_actions, (void **)&original_hand_actions) ||
        !install((void *)func_8001BAF0, (void *)cosmo_hand_swap, (void **)&original_hand_swap) ||
        !install((void *)DuelScene_UpdateBattle, (void *)battle, (void **)&original_battle) ||
        !install((void *)DuelEffect_UpdateRequests, (void *)update_effect_requests, (void **)&original_update_effect_requests) ||
        !install((void *)DuelEffect_UpdateCardEffect, (void *)update_card_effect, (void **)&original_update_card_effect) ||
        !install((void *)DuelScene_UpdateDrawPhase, (void *)draw_phase, (void **)&original_draw_phase) ||
        !install((void *)DuelEffect_ApplyBoardDestruction, (void *)board_destruction, (void **)&original_board_destruction) ||
        !install((void *)DuelEffect_ApplyMonsterRemoval, (void *)monster_removal, (void **)&original_monster_removal) ||
        !install((void *)DuelEffect_ApplyRaigeki, (void *)raigeki, (void **)&original_raigeki) ||
        !install((void *)DuelScene_UpdateTurnSwitch, (void *)turn_switch, (void **)&original_turn_switch) ||
        !install((void *)Duel_CheckFusion, (void *)mask_check_fusion, (void **)&original_check_fusion) ||
        !install((void *)Duel_CheckRitual, (void *)mask_check_ritual, (void **)&original_check_ritual)) {
        shutdown(); return 0;
    }
    /* Optional read-only recipe catalogue; no other MOD is required.
     * Disabled MODs remain loaded, so get() checks recipe_export_active. */
    if(!install((void*)AiScript_PlayFieldCard,(void*)ea_play,(void**)&ea_old_play)||!install((void*)AiScript_LoadGuardianStarChoice,(void*)ea_star,(void**)&ea_old_star)||!install((void*)AiScript_FindKiller,(void*)ea_killer,(void**)&ea_old_killer)||!install((void*)AiScript_CalcCardPower,(void*)ea_power,(void**)&ea_old_power)||!install((void*)AiScript_FindBestAttack,(void*)ea_best,(void**)&ea_old_best))return 0;
    if(host->provide)host->provide(host,"effect_forecast_v2",(void*)&ea_owner);
    if(host->provide && !host->provide(host,"recipe_v1",(void *)&recipe_export_v1) && host->log)
        host->log(host,"Efeitos de Rituais: recipe_v1 export unavailable");
    /* This API is optional; it cannot make either MOD refuse to load. */
    if(host->provide && !host->provide(host,"effect_intelligence_v1",
        (void *)&ritual_effect_intelligence_v1) && host->log)
        host->log(host,"Efeitos de Rituais: effect_intelligence_v1 export unavailable");
    if(host->provide)host->provide(host,"position-rules-v1",(void *)&ritual_position_api);
    mod->api = 4;
    mod->name = "MOD Efeitos de Cartas Rituais";
    mod->reset = clear;
    mod->applied = applied;
    mod->shutdown = shutdown;
    mod->frame = coin_frame;
    mod->overlay = overlay;
    mod->overlay_signature = overlay_signature;
    selection_ready=1;selection_connect();
    return 1;
}
