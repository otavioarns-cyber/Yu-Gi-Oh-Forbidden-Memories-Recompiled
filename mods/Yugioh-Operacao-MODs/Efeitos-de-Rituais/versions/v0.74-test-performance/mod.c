#include "pc/mods/modapi.h"
#include "pc/render/texture_dump.h"
#include <stdio.h>
#include "psyq/libgte.h"
#include "psyq/libgpu.h"
#define D_80177EA4_VISIBLE
#include "unmatched.h"
#include "game/duel_card.h"
#include "game/duel_card_record_lifecycle.h"
#include "game/duel_card_layout.h"
#include "game/duel_card_display_state.h"
#include "game/duel_deck_card_data.h"
#include "game/duel_ritual_effect.h"
#include "game/duel_check_ritual.h"
#include "game/duel_ritual_controller.h"
#include "game/duel_action_lock.h"
#include "game/duel_side_state.h"
#include "game/duel_scene_field_actions.h"
#include "game/duel_scene_card_placement.h"
#include "game/duel_scene_hand_actions.h"
#include "game/duel_scene_battle.h"
#include "game/duel_scene_state.h"
#include "game/duel_selection_layout.h"
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "game/input.h"
#include "game/duel_scene_turn_switch.h"
#include "game/duel_deck_card.h"
#include "game/duel_card_effects.h"
#include "game/duel_effect_request.h"
#include "game/duel_effect_allocate_request.h"
#include "game/sound.h"
#include "game/duel_card_checks.h"
#include "game/duel_card_can_act_this_turn.h"
#include "game/duel_magic_effect_dispatch.h"
#include "game/display_object_work_slots.h"
#include "game/display_object_core.h"
#include "game/func_80016784.h"
#include "game/display_object_packet_submit.h"
#define DUEL_SCREEN_TABLES_TYPED_POSITIONS
#include "game/duel_screen_tables.h"
#include "game/card_constants.h"
#include "game/duel_terrain_boost.h"
#include "game/duel_scene_update.h"
#include "game/duel_effect_resource_record.h"
#include "game/duel_effect_resource_setup.h"
#include "game/func_800291E0.h"
#include "game/func_80029574.h"
#include "game/file_transfer.h"
#include "game/func_8001944C.h"
#include "game/duel_create_card_effect_overlay.h"
#include "game/display_object_helpers.h"
#include "psyq/libgs.h"
extern u8 gDuel_bTerrain;
#include "pc/cards/tables.h"
#include "pc/cards/cards.h"
#include "pc/audio/spu.h"
#include "pc/cards/art.h"
#include "pc/cards/disc_art.h"
#include "game/ai.h"
#include "game/ai_script_commands.h"
#include "game/duel_calc_guardian_star_matchup.h"
#include "game/duel_cursor_status.h"
#include "game/card_preview_callbacks.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Standalone ritual-card effects mod. Ritual recipes are declared in mod.json
 * so the public Tables_RitualRequirements interface is shared with
 * MelhoriaDosRituais (field + hand). The ritual sequencer's initialized
 * state 3 builds the actual result record; mark it only AFTER that call
 * reaches state 4 (player) or 5 (CPU). */
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
#define GARMA_SWORD_ID 716
#define GARMA_SWORD_OATH_ID 697
#define VISHWAR_RANDI_ID 239
#define SUCCUBUS_KNIGHT_ID 621
#define SWORD_DARK_DESTRUCTION_ID 302
extern u8 D_8009B174;
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
static const MemoriesModHost *host;
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
    { CHAOS_ID, "black_chaos", "Magician of Black Chaos" }
};
static int settings_cached, debug_overlay;
static unsigned char effect_settings[sizeof(ritual_effects)/sizeof(ritual_effects[0])];
static int ritual_effect_enabled(const char *key)
{
    unsigned i;
    if (settings_cached && key)
        for(i=0;i<sizeof(ritual_effects)/sizeof(ritual_effects[0]);++i)
            if(!strcmp(key,ritual_effects[i].setting_key))return effect_settings[i];
    if (!host || !host->setting) return 1;
    return !key || host->setting(host, key, 1) != 0;
}
static int ritual_card_effect_enabled(int card_id)
{
    unsigned i;
    for (i = 0; i < sizeof(ritual_effects)/sizeof(ritual_effects[0]); ++i)
        if (ritual_effects[i].card_id == card_id)
            return settings_cached ? effect_settings[i] : ritual_effect_enabled(ritual_effects[i].setting_key);
    return 0;
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
    { PERFORMANCE_RITUAL_ID, PERFORMANCE_ID, "performance_sword_recipe", "performance_sword" }
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
    debug_overlay=host&&host->setting&&host->setting(host,"debug_overlay",0)!=0;
    settings_cached=1;
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
    for (i = 0; i < sizeof(ritual_options)/sizeof(ritual_options[0]); ++i) {
        const char *key = recipes ? ritual_options[i].recipe_key : ritual_options[i].effect_key;
        host->set_setting(host, key, command == 1 ? 1 : 0);
    }
    if (!recipes) { host->set_setting(host, "black_chaos", command == 1 ? 1 : 0); host->set_setting(host, "tri_horned", command == 1 ? 1 : 0); host->set_setting(host, "serpent_night", command == 1 ? 1 : 0); }
    host->set_setting(host, command_key, 0);
}

static void sync_global_settings(void)
{
    apply_bulk_group("recipes_bulk", 1);
    apply_bulk_group("effects_bulk", 0);
}
static int recipe_enabled(const char *key)
{
    unsigned i;
    if(settings_cached && key)
        for(i=0;i<sizeof(recipe_settings);++i)
            if(!strcmp(key,ritual_options[i].recipe_key))return recipe_settings[i];
    if (!host || !host->setting) return 1;
    return !key || host->setting(host, key, 1) != 0;
}
static const RitualOptionDefinition *option_by_ritual(int id)
{
    unsigned i;
    for (i=0;i<sizeof(ritual_options)/sizeof(ritual_options[0]);++i)
        if (ritual_options[i].ritual_id==id) return &ritual_options[i];
    return 0;
}
static void (*original_apply)(void);
static s32 (*original_stats)(DuelCardRecord *);
static void (*original_remove)(DuelCardRecord *);
static void (*original_deactivate)(DuelCardRecord *);
static void (*original_populate)(void);
static void (*original_field_actions)(void);
static void (*original_card_placement)(void);
static void (*original_hand_actions)(void);
static void (*original_battle)(void);
static s32 (*original_update_card_effect)(void);
static void (*original_board_destruction)(void);
static void (*original_monster_removal)(void);
static void (*original_raigeki)(void);
static void (*original_turn_switch)(void);
static int tokens[40];
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
    int controlling;
    void *stolen_data;
    int stolen_card_id;
    int original_slot;
    int current_slot;
    int controller_side;
    int pending_summon_trigger;
    int visual_shown;
} PsychoMark;
static PsychoMark psycho_marks[DUEL_CARD_RECORD_COUNT];
static unsigned psycho_summons;
static unsigned psycho_steals;

typedef struct {
    void *data;
    int ritual;
    unsigned battle_kills;
} HungryMark;
static HungryMark hungry_marks[DUEL_CARD_RECORD_COUNT];
static unsigned hungry_summons;
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
static unsigned javelin_protected_visuals;

typedef struct {
    void *data;
    int ritual;
    unsigned attacks_this_turn;
} GateMark;
static GateMark gate_marks[DUEL_CARD_RECORD_COUNT];
static unsigned gate_summons;

typedef struct { void *data; int ritual; unsigned blades_used; unsigned extra_attack_pending; unsigned extra_attack_ready; } GarmaMark;
static GarmaMark garma_marks[DUEL_CARD_RECORD_COUNT];
static unsigned garma_summons;
static unsigned garma_visual_pending;

typedef struct { void *data; unsigned ritual; unsigned battle_flags; } SeaRitualMark;
static SeaRitualMark crab_marks[DUEL_CARD_RECORD_COUNT];
static SeaRitualMark whale_marks[DUEL_CARD_RECORD_COUNT];
typedef struct { void *data; unsigned ritual, learned_sides; } SerpentMark;
static SerpentMark serpent_marks[DUEL_CARD_RECORD_COUNT];
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
static unsigned crab_umi_pending;
static unsigned crab_umi_sequence; /* 0 idle, 1 wait ritual, 2 pre-effect, 3 visual, 4 post-effect */
static int crab_umi_visual_state;
static int ritual_large_visual_card_id = UMI_ID;
static int crab_umi_visual_hold;
static int crab_umi_camera_delay;
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
static unsigned fiend_mirror_summons;
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
typedef struct { int target_side; unsigned damage; int source_index; int visual_queued; } MirrorQueuedHit;
static MirrorQueuedHit mirror_queue[MIRROR_QUEUE_CAP];
static unsigned mirror_queue_head, mirror_queue_tail, mirror_queue_count;
static MirrorReflection mirror_reflection;
static u8 mirror_battle_grace[2];
static s32 (*original_check_fusion)(s32, s32);
static s32 (*original_check_ritual)(DuelRitualResult *, s32);

static int monster_slot(int index)
{ return (index >= 5 && index < 10) || (index >= 20 && index < 25); }

static int record_index(DuelCardRecord *card);
static void mirror_watch_lp(void);
static int side_has_ritual_fiend_mirror(int side);

static int magic_slot(int index)
{ return (index >= 10 && index < 15) || (index >= 25 && index < 30); }

static int side_for_record(int index)
{ return index >= DUEL_CARD_SIDE_RECORD_COUNT; }

/* Visual helper is defined below. Mask uses it only when a real Fusion is
 * blocked during placement; planner-only checks never animate. */
static int visual_queue_activation(int record_index);

#include "mask_effect.h"
#include "ritual_recipe_data.h"
#include "ritual_recipe_compat.h"
#include "garma_visual.h"
#include "tri_horned.h"
static void serpent_reconcile(void);
static void mirror_watch_lp(void);
#include "ritual_coin.h"
#include "performance_chaos.h"

static void forget_yamadron(DuelCardRecord *card)
{
    int i = record_index(card);
    if (i >= 0) {
        yamadron_marks[i].data = 0;
        yamadron_marks[i].card_id = 0;
        yamadron_marks[i].ritual = 0;
        yamadron_marks[i].visual_shown = 0;
    }
}

static unsigned count_attack_monsters_except(int excluded_index)
{
    unsigned count = 0;
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *card;
        if (!monster_slot(i) || i == excluded_index) continue;
        card = &D_801A7AD8[i];
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) &&
            !(card->flags & DUEL_CARD_FLAG_DEFENSE_POSITION))
            ++count;
    }
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

static void forget(DuelCardRecord *card)
{
    int i = record_index(card);
    if (i >= 0) {
        marks[i].data = 0; marks[i].card_id = 0;
        marks[i].ritual = 0; marks[i].in_battle = 0; marks[i].attack_count = 0; marks[i].def_visual_shown = 0; marks[i].protect_visual_shown = 0;
    }
}

/* Keep native HD stat rendering untouched. This wrapper is retained only
 * to capture the approved Garma/Tri presentation positions. */
static void (*original_card_frame_draw)(DisplayObject *, s32, s32, s32);
static void visual_card_frame_draw(DisplayObject *object,s32 a1,s32 a2,s32 a3)
{
    garma_capture(object,a2,a3);
    tri_capture(object,a2,a3);
    original_card_frame_draw(object,a1,a2,a3);
}

/* Shared Ritual presentation helpers, adapted from monster-effects 0.19.0. */
#define VISUAL_QUEUE_SIZE 32
#define STAT_POPUP_FRAMES 42
#define STAT_CHANGE_INTERVAL 16
#define VISUAL_ACTIVATION_DELAY 30

typedef struct { s16 x, y; s16 delay; } RitualActivationPos;
typedef struct { int active, record_index, card_id, amount; void *card_data; } RitualStatVisual;
static RitualActivationPos visual_activation_queue[VISUAL_QUEUE_SIZE];
static int visual_activation_head, visual_activation_count;
static RitualStatVisual visual_stat_queue[VISUAL_QUEUE_SIZE];
static int visual_stat_cooldown, visual_popup_amount, visual_popup_x, visual_popup_y, visual_popup_frames;
static void *hungry_visual_pending[DUEL_CARD_RECORD_COUNT];

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
    if(garma_request && garma_request->id==4 && (garma_request->flags & DUEL_EFFECT_REQUEST_FLAG_ACTIVE)) return;
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

static int visual_queue_stat(int record_index, int amount)
{
    DuelCardRecord *card; int i;
    if(record_index<0 || record_index>=DUEL_CARD_RECORD_COUNT || !amount) return 0;
    card=&D_801A7AD8[record_index];
    if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED)||!card->data||!card->object) return 0;
    for(i=0;i<VISUAL_QUEUE_SIZE;++i) if(!visual_stat_queue[i].active){
        visual_stat_queue[i].active=1; visual_stat_queue[i].record_index=record_index;
        visual_stat_queue[i].card_id=card->card_id; visual_stat_queue[i].card_data=card->data;
        visual_stat_queue[i].amount=amount; return 1;
    }
    return 0;
}

static void visual_update_stats(void)
{
    if(garma_request && garma_request->id==4 && (garma_request->flags & DUEL_EFFECT_REQUEST_FLAG_ACTIVE)) return;
    int i;
    if(visual_popup_frames>0){ --visual_popup_frames; if(visual_popup_frames>0)return; }
    if(visual_activation_busy()) return;
    if(visual_stat_cooldown>0){--visual_stat_cooldown;return;}
    for(i=0;i<VISUAL_QUEUE_SIZE;++i){
        RitualStatVisual *e=&visual_stat_queue[i]; DuelCardRecord *card; DisplayObject *obj;
        if(!e->active)continue; card=&D_801A7AD8[e->record_index];
        if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED)||card->card_id!=e->card_id||card->data!=e->card_data||!card->object){e->active=0;continue;}
        obj=(DisplayObject *)card->object;
        if(e->amount>0){ visual_popup_amount=e->amount; visual_popup_x=obj->field_30.h.field_30; visual_popup_y=obj->field_30.h.field_32; visual_popup_frames=STAT_POPUP_FRAMES; SD_SEPlayFull(0x21); e->active=0; visual_stat_cooldown=STAT_CHANGE_INTERVAL; return; }
        else { DuelEffectRequest *r=DuelEffect_CreateRequest(0xD); if(!r)return; r->field_00=obj->field_30.h.field_30; r->field_02=obj->field_30.h.field_32; r->field_04=obj->field_34.h.field_34; r->field_12=(s16)e->amount; SD_SEPlayFull(0x21); e->active=0; visual_stat_cooldown=STAT_CHANGE_INTERVAL; return; }
    }
}

static int visual_find_live_mirror(int controller_side)
{
    int i, start=controller_side?20:5, end=controller_side?25:10;
    for(i=start;i<end;++i){ DuelCardRecord *c=&D_801A7AD8[i];
        if(fiend_mirror_marks[i].ritual && (c->flags&DUEL_CARD_FLAG_OCCUPIED) && c->card_id==FIENDS_MIRROR_ID && c->data==fiend_mirror_marks[i].data && c->object) return i;
    }
    return -1;
}

static void visual_clear(void)
{
    memset(visual_activation_queue,0,sizeof(visual_activation_queue)); visual_activation_head=visual_activation_count=0;
    memset(visual_stat_queue,0,sizeof(visual_stat_queue)); visual_stat_cooldown=0; visual_popup_frames=0;
    memset(hungry_visual_pending,0,sizeof(hungry_visual_pending));
}

static void clear(void)
{
    compat_recipe_restore();
    coin_reset();
    visual_clear();
    memset(mask_instances, 0, sizeof(mask_instances));
    mask_inside_placement = mask_known_sides = 0;
    mask_blocked = mask_learned = mask_released = mask_summons = 0;
    memset(fiend_mirror_marks, 0, sizeof(fiend_mirror_marks));
    mirror_lp_ready = 0;
    fiend_mirror_summons = 0;
    memset(&mirror_reflection, 0, sizeof(mirror_reflection));
    memset(mirror_queue, 0, sizeof(mirror_queue));
    mirror_queue_head = mirror_queue_tail = mirror_queue_count = 0;
    memset(mirror_battle_grace, 0, sizeof(mirror_battle_grace));
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) { forget(&D_801A7AD8[i]); forget_yamadron(&D_801A7AD8[i]); }
    summons = 0;
    deferred_deactivation = 0;
    inside_field_actions = 0;
    inside_effect_destruction = 0;
    protected_visuals = 0;
    effect_survivor_visuals = 0;
    deferred_yamadron = 0;
    yamadron_summons = 0;
    yamadron_generations = 0;
    generated_thumbnail_valid = 0;
    generated_thumbnail_touched = 0;
    memset(psycho_marks, 0, sizeof(psycho_marks));
    psycho_summons = 0;
    psycho_steals = 0;
    memset(javelin_marks, 0, sizeof(javelin_marks));
    memset(gate_marks, 0, sizeof(gate_marks));
    memset(garma_marks, 0, sizeof(garma_marks));
    memset(tri_marks,0,sizeof(tri_marks));memset(tri_screen,0,sizeof(tri_screen));
    memset(tri_guard_data,0,sizeof(tri_guard_data));
    tri_guard=tri_horn_visuals=tri_battle_visuals=0;
    tri_resolution_active=tri_resolution_id=tri_field_pending=0;ritual_field_id=UMI_ID;
    memset(garma_screen,0,sizeof(garma_screen));
    memset(garma_pending,0,sizeof(garma_pending));
    garma_relevant=-1;garma_relevant_frames=0;garma_request=0;
    memset(crab_marks, 0, sizeof(crab_marks)); memset(whale_marks, 0, sizeof(whale_marks)); memset(serpent_marks,0,sizeof(serpent_marks)); memset(serpent_fear_bonus,0,sizeof(serpent_fear_bonus)); serpent_fear_battle_slot=-1; serpent_ghost_state=serpent_ghost_hold=0; serpent_art_loaded=0; serpent_visual_reset(); if(serpent_music_volume>=0){Spu_SetBusVolume(SPU_BUS_MUSIC,serpent_music_volume);serpent_music_volume=-1;}
    deferred_javelin = 0; javelin_summons = 0; javelin_protected_visuals = 0; gate_summons = 0; garma_summons = 0; garma_visual_pending = 0;
    crab_restore_visuals = 0; crab_summons = whale_summons = 0; crab_umi_pending = 0; crab_umi_sequence = 0; whale_destruction_guard = 0;
    crab_umi_visual_state = 0; crab_umi_visual_hold = 0; crab_umi_camera_delay = 0; crab_umi_overlay_add = crab_umi_overlay_sub = 0;
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
    int i;
    int any = 0;
    if(!generated_thumbnail_valid)return;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *card = &D_801A7AD8[i];
        if (!(generated_thumbnail_valid & (1u << i))) continue;
        if (!magic_slot(i) || !(card->flags & DUEL_CARD_FLAG_OCCUPIED) ||
            card->data != &generated_data[i]) {
            generated_thumbnail_valid &= ~(1u << i);
            continue;
        }
        any |= upload_slot_thumbnail_from_block(i, generated_thumbnail_cache[i]);
    }
    if (any) DrawSync(0);
}

static void restore_hand_thumbnails(void)
{
    int i;
    int any = 0;
    if(!generated_thumbnail_touched)return;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *card;
        DuelDeckCardRecord *deck;
        int off;
        if (!((i >= 0 && i < 5) || (i >= 15 && i < 20))) continue;
        card = &D_801A7AD8[i];
        if (!(card->flags & DUEL_CARD_FLAG_OCCUPIED) || !card->data) continue;
        deck = (DuelDeckCardRecord *)card->data;
        if (deck < gDuel_aDeckCardRecords ||
            deck >= gDuel_aDeckCardRecords + COMBINED_DECK_SIZE) continue;
        off = deck->data_block_index * DUEL_CARD_DATA_BLOCK_SIZE;
        if(deck->data_block_index>=COMBINED_DECK_SIZE)continue;
        any |= upload_slot_thumbnail_from_block(i, D_8018C2D8 + off);
    }
    if (any) DrawSync(0);
}

static int find_occupied_monster_by_data(void *data)
{
    int i;
    if (!data) return -1;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (!monster_slot(i)) continue;
        if ((D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            D_801A7AD8[i].data == data)
            return i;
    }
    return -1;
}

static PsychoMark *find_psycho_mark(void *data)
{
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i)
        if (psycho_marks[i].ritual && psycho_marks[i].psycho_data == data)
            return &psycho_marks[i];
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
            psycho_marks[i].original_slot = -1;
            psycho_marks[i].current_slot = -1;
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
    if (obj) obj->card_index = (u8)index;
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
    psycho_slot = find_occupied_monster_by_data(m->psycho_data);
    if (psycho_slot < 0) return 0;
    psycho = &D_801A7AD8[psycho_slot];
    if (psycho->card_id != PSYCHO_PUPPET_ID) return 0;
    side = side_for_record(psycho_slot);
    free_slot = first_free_monster_slot(side);
    if (free_slot < 0) return 0;
    target_slot = random_enemy_monster_slot(side);
    if (target_slot < 0) return 0;

    m->stolen_data = D_801A7AD8[target_slot].data;
    m->stolen_card_id = D_801A7AD8[target_slot].card_id;
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
    current = find_occupied_monster_by_data(m->stolen_data);
    if (current >= 0) {
        /* Brief marker at the borrowed monster before retail ownership is
         * restored.  The queued position survives the subsequent move. */
        visual_queue_activation(current);
        destination = m->original_slot;
        if (!monster_slot(destination) ||
            (D_801A7AD8[destination].flags & DUEL_CARD_FLAG_OCCUPIED))
            destination = first_free_monster_slot(m->controller_side ^ 1);
        if (destination >= 0)
            move_monster_record(current, destination);
    }
    m->controlling = 0;
    m->stolen_data = 0;
    m->stolen_card_id = 0;
    m->original_slot = m->current_slot = -1;
}

static void psycho_return_for_side(int side)
{
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i)
        if (psycho_marks[i].ritual && psycho_marks[i].controlling &&
            psycho_marks[i].controller_side == side)
            psycho_return_control(&psycho_marks[i]);
}

static void psycho_trigger_for_side(int side, const char *reason)
{
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        int slot;
        if (!psycho_marks[i].ritual || psycho_marks[i].controlling) continue;
        slot = find_occupied_monster_by_data(psycho_marks[i].psycho_data);
        if (slot >= 0 && side_for_record(slot) == side &&
            D_801A7AD8[slot].card_id == PSYCHO_PUPPET_ID)
            psycho_attempt_control(&psycho_marks[i], reason);
    }
}

static int data_is_stolen(void *data, int side)
{
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i)
        if (psycho_marks[i].ritual && psycho_marks[i].controlling &&
            psycho_marks[i].controller_side == side &&
            psycho_marks[i].stolen_data == data)
            return 1;
    return 0;
}



static s32 psycho_check_fusion(s32 a, s32 b)
{
    if (!ritual_card_effect_enabled(PSYCHO_PUPPET_ID)) return original_check_fusion(a, b);
    int i;
    /* Duel_CheckFusion only receives IDs, not record pointers. During the
     * temporary-control window reject a pair containing the stolen card's ID.
     * This prevents the borrowed monster from being consumed as Fusion
     * material; the restriction disappears as soon as it returns. */
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (psycho_marks[i].ritual && psycho_marks[i].controlling &&
            psycho_marks[i].controller_side == D_8009B1D5 &&
            (a == psycho_marks[i].stolen_card_id || b == psycho_marks[i].stolen_card_id))
            return 0;
    }
    return original_check_fusion(a, b);
}

static s32 mask_check_fusion(s32 a, s32 b)
{ return mask_filter_fusion(psycho_check_fusion(a, b)); }

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

static void hand_actions(void)
{
    if(chaos_turn.active)return;
    restore_hand_thumbnails();
    original_hand_actions();
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
    int i;
    int start = side ? 20 : 5;
    int end = side ? 25 : 10;
    for (i = start; i < end; ++i)
        if (yamadron_marks[i].ritual) generate_yamadron_burns(i, reason);
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
        mask_summon(card);
        coin_summon(index);
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id==TRI_HORNED_ID) {
            tri_marks[index].data=card->data;tri_marks[index].ritual=1;
            tri_marks[index].horns=3;tri_marks[index].battle_flags=card->flags;
            if(ritual_card_effect_enabled(TRI_HORNED_ID))tri_field_pending=1;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id==SERPENT_NIGHT_ID) {
            serpent_marks[index].data=card->data; serpent_marks[index].ritual=1; serpent_marks[index].learned_sides=0; ++serpent_dbg_ritual; serpent_dbg_last_slot=index;
            if(host&&host->log)host->log(host,"[Ritual Card Effects] Serpent Night Dragon Ritual detectado: lado %d, slot %d",index>=15,index);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == SHIELD_ID) {
            marks[index].data = card->data;
            marks[index].card_id = card->card_id;
            marks[index].ritual = 1;
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
            ++yamadron_summons;
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Yamadron Ritual detectado: lado %d, slot %d", index >= 15, index);
            generate_yamadron_burns(index, "invocacao");
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == PSYCHO_PUPPET_ID) {
            PsychoMark *m = alloc_psycho_mark(card->data);
            if (m) {
                ++psycho_summons;
                if (host && host->log)
                    host->log(host, "[Ritual Card Effects] Psycho-Puppet Ritual detectado: lado %d, slot %d", index >= 15, index);
                /* The Ritual result is detected before the Ritual effect has
                 * completely returned control to the field scene. At that
                 * instant the tribute records can still occupy monster slots,
                 * so attempting the steal here can incorrectly see a full
                 * field. Defer exactly one summon attempt to the first field
                 * tick after Ritual resolution. */
                m->pending_summon_trigger = 1;
            }
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == HUNGRY_BURGER_ID) {
            hungry_marks[index].data = card->data;
            hungry_marks[index].ritual = 1;
            hungry_marks[index].battle_kills = 0;
            ++hungry_summons;
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Hungry Burger Ritual detectado: lado %d, slot %d", index >= 15, index);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data &&
            card->card_id == FIENDS_MIRROR_ID && fiends_mirror_effect_on) {
            fiend_mirror_marks[index].data = card->data;
            fiend_mirror_marks[index].ritual = 1;
            ++fiend_mirror_summons;
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Fiend's Mirror Ritual detectado: lado %d, slot %d", index >= 15, index);
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == JAVELIN_BEETLE_ID) {
            javelin_marks[index].data = card->data; javelin_marks[index].ritual = 1;
            javelin_marks[index].protection_used_turn = 0; javelin_marks[index].protection_visual_shown = 0; javelin_marks[index].battle_flags = card->flags;
            ++javelin_summons;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == GATE_GUARDIAN_ID) {
            gate_marks[index].data = card->data; gate_marks[index].ritual = 1; gate_marks[index].attacks_this_turn = 0;
            ++gate_summons;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == GARMA_SWORD_ID) {
            garma_marks[index].data = card->data; garma_marks[index].ritual = 1; garma_marks[index].blades_used = 0; garma_marks[index].extra_attack_pending = 0; garma_marks[index].extra_attack_ready = 0;
            ++garma_summons;
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == CRAB_TURTLE_ID) {
            crab_marks[index].data = card->data; crab_marks[index].ritual = 1; crab_marks[index].battle_flags = card->flags;
            ++crab_summons;
            if (ritual_card_effect_enabled(CRAB_TURTLE_ID)) { crab_umi_pending = 1; crab_umi_sequence = 1; ritual_field_id=UMI_ID; }
        }
        if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) && card->data && card->card_id == FORTRESS_WHALE_ID) {
            whale_marks[index].data = card->data; whale_marks[index].ritual = 1; whale_marks[index].battle_flags = card->flags;
            ++whale_summons;
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

static unsigned active_javelins_for_target(int target)
{
    int i, side = side_for_record(target), start = side ? 20 : 5; unsigned n = 0;
    for (i = start; i < start + 5; ++i)
        if (i != target && ritual_card_effect_enabled(JAVELIN_BEETLE_ID) && javelin_marks[i].ritual &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].card_id == JAVELIN_BEETLE_ID &&
            D_801A7AD8[i].data == javelin_marks[i].data) ++n;
    return n;
}

static int sea_family_type(int t) { return t == CARD_TYPE_FISH || t == CARD_TYPE_AQUA || t == CARD_TYPE_SEA_SERPENT; }
static int side_has_crab(int side) {
    int i, start=side?20:5; if (!ritual_card_effect_enabled(CRAB_TURTLE_ID)) return 0;
    for(i=start;i<start+5;++i) if(crab_marks[i].ritual && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].card_id==CRAB_TURTLE_ID && D_801A7AD8[i].data==crab_marks[i].data) return 1;
    return 0;
}
static int side_has_whale(int side) {
    int i, start=side?20:5; if (!ritual_card_effect_enabled(FORTRESS_WHALE_ID)) return 0;
    for(i=start;i<start+5;++i) if(whale_marks[i].ritual && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].card_id==FORTRESS_WHALE_ID && D_801A7AD8[i].data==whale_marks[i].data) return 1;
    return 0;
}
/* Fortress Whale projects its Umi protection to allied sea-family monsters,
 * but never to itself. With multiple ritualized Whales, each Whale may be
 * protected by a different Whale. */
static int whale_protects_target(int target) {
    int i, side, start;
    if (!ritual_card_effect_enabled(FORTRESS_WHALE_ID) || target < 0 || !monster_slot(target)) return 0;
    side=side_for_record(target); start=side?20:5;
    for(i=start;i<start+5;++i) {
        if(i==target) continue;
        if(whale_marks[i].ritual && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           D_801A7AD8[i].card_id==FORTRESS_WHALE_ID && D_801A7AD8[i].data==whale_marks[i].data) return 1;
    }
    return 0;
}
static int effect_is_magic_or_trap(void);
static unsigned fish_in_hand_and_field(int side) {
    unsigned n=0; int i, hand=side?15:0, field=side?20:5;
    for(i=hand;i<hand+5;++i)
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) && card_type_id(&D_801A7AD8[i])==CARD_TYPE_FISH) ++n;
    for(i=field;i<field+5;++i)
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) && card_type_id(&D_801A7AD8[i])==CARD_TYPE_FISH) ++n;
    return n;
}
static unsigned whale_snapshot_protection(void) {
    unsigned mask=0; int i;
    if(gDuel_bTerrain!=UMI_TERRAIN || !effect_is_magic_or_trap()) return 0;
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        if(monster_slot(i) && (c->flags&DUEL_CARD_FLAG_OCCUPIED) && sea_family_type(card_type_id(c)) && whale_protects_target(i))
            mask |= 1u<<i;
    }
    return mask;
}
static int effect_is_magic_or_trap(void) {
    int id=gDuel_wEffectCardID, t; if(id<1) return 0; t=(int)(((unsigned)gDuel_adwCardStats[id-1]>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK); return t==CARD_TYPE_MAGIC || t==CARD_TYPE_TRAP;
}

static s32 calc_stats(DuelCardRecord *card)
{
    uint32_t packed = (uint32_t)original_stats(card);
    int i = record_index(card);
    if (ritual_card_effect_enabled(SHIELD_ID) && i >= 0 && monster_slot(i) && marks[i].ritual) {
        if (marks[i].data != card->data || marks[i].card_id != card->card_id) {
            forget(card);
        } else if ((card->flags & DUEL_CARD_FLAG_OCCUPIED) || marks[i].in_battle) {
            unsigned defense = (packed >> 16) + marks[i].attack_count * BONUS;
            if (defense > CARD_STAT_MAX) defense = CARD_STAT_MAX;
            packed = (packed & 0xffffu) | (defense << 16);
        }
    }
    if (ritual_card_effect_enabled(HUNGRY_BURGER_ID) && i >= 0 && monster_slot(i) && hungry_marks[i].ritual) {
        if (hungry_marks[i].data != card->data || card->card_id != HUNGRY_BURGER_ID) {
            hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0;
        } else {
            unsigned atk = (packed & 0xffffu) + hungry_marks[i].battle_kills * HUNGRY_BONUS;
            unsigned def = (packed >> 16) + hungry_marks[i].battle_kills * HUNGRY_BONUS;
            if (atk > CARD_STAT_MAX) atk = CARD_STAT_MAX;
            if (def > CARD_STAT_MAX) def = CARD_STAT_MAX;
            packed = atk | (def << 16);
        }
    }
    if (ritual_card_effect_enabled(JAVELIN_BEETLE_ID) && i >= 0 && monster_slot(i) &&
        (card->flags & DUEL_CARD_FLAG_OCCUPIED) && card_type_id(card) == CARD_TYPE_INSECT) {
        unsigned sources = active_javelins_for_target(i);
        if (sources) {
            unsigned bonus = allied_insect_count(side_for_record(i)) * JAVELIN_AURA_BONUS * sources;
            unsigned atk = (packed & 0xffffu) + bonus, def = (packed >> 16) + bonus;
            if (atk > CARD_STAT_MAX) atk = CARD_STAT_MAX; if (def > CARD_STAT_MAX) def = CARD_STAT_MAX;
            packed = atk | (def << 16);
        }
    }
    if (i >= 0 && monster_slot(i) && (card->flags & DUEL_CARD_FLAG_OCCUPIED)) {
        int side=side_for_record(i), t=card_type_id(card);
        if (ritual_card_effect_enabled(CRAB_TURTLE_ID) && side_has_crab(side) && sea_family_type(t) && card->card_id != CRAB_TURTLE_ID) {
            unsigned atk=(packed&0xffffu)+CRAB_AURA_ATK, def=(packed>>16)+CRAB_AURA_DEF;
            if(atk>CARD_STAT_MAX) atk=CARD_STAT_MAX; if(def>CARD_STAT_MAX) def=CARD_STAT_MAX; packed=atk|(def<<16);
        }
        if (ritual_card_effect_enabled(FORTRESS_WHALE_ID) && card->card_id==FORTRESS_WHALE_ID && whale_marks[i].ritual && whale_marks[i].data==card->data) {
            unsigned def=(packed>>16)+fish_in_hand_and_field(side)*FORTRESS_FISH_DEF; if(def>CARD_STAT_MAX) def=CARD_STAT_MAX; packed=(packed&0xffffu)|(def<<16);
        }
    }
    if (i>=0 && monster_slot(i) && (card->flags&DUEL_CARD_FLAG_OCCUPIED) && card_type_id(card)==CARD_TYPE_DINOSAUR) {
        unsigned bonus=1000u*tri_sources(i);
        if(bonus) {
            unsigned atk=(packed&0xffffu)+bonus,def=(packed>>16)+bonus;
            if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX;if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
            packed=atk|(def<<16);
        }
    }
    if (ritual_card_effect_enabled(SERPENT_NIGHT_ID) && i>=0 && i<DUEL_CARD_RECORD_COUNT && serpent_fear_bonus[i]) {
        ++serpent_dbg_bonus_reads;
        unsigned atk=(packed&0xffffu)+serpent_fear_bonus[i], def=(packed>>16)+serpent_fear_bonus[i];
        if(atk>CARD_STAT_MAX)atk=CARD_STAT_MAX; if(def>CARD_STAT_MAX)def=CARD_STAT_MAX;
        packed=atk|(def<<16);
    }
    return (s32)packed;
}

static int javelin_should_protect(DuelCardRecord *card)
{
    int i = record_index(card);
    if (!ritual_card_effect_enabled(JAVELIN_BEETLE_ID) || i < 0 || !monster_slot(i) ||
        (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9 || !javelin_marks[i].ritual ||
        javelin_marks[i].data != card->data || card->card_id != JAVELIN_BEETLE_ID ||
        javelin_marks[i].protection_used_turn) return 0;
    javelin_marks[i].protection_used_turn = 1;
    javelin_protected_visuals |= 1u << i;
    return 1;
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
    if(chaos_hole_protected(i)){effect_survivor_visuals|=1u<<i;return;}
    if (tri_guarded(i)) {
        effect_survivor_visuals |= 1u << i;
        return;
    }
    if (javelin_should_protect(card)) {
        if (host && host->log) host->log(host, "[Ritual Card Effects] Javelin Beetle usou protecao de batalha: slot %d", i);
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
    {
        int p;
        for (p = 0; p < DUEL_CARD_RECORD_COUNT; ++p) {
            if (psycho_marks[p].ritual && psycho_marks[p].controlling &&
                psycho_marks[p].stolen_data == card->data) {
                psycho_marks[p].controlling = 0;
                psycho_marks[p].stolen_data = 0;
                psycho_marks[p].stolen_card_id = 0;
                psycho_marks[p].original_slot = psycho_marks[p].current_slot = -1;
            }
        }
    }
    if (i >= 0 && hungry_marks[i].ritual && hungry_marks[i].data == card->data) {
        hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0;
    }
    /* Garma's five blades belong to the ritualized field instance, not to a
     * turn.  Clear them only when that instance really leaves the field. */
    if (i >= 0 && garma_marks[i].ritual && garma_marks[i].data == card->data)
        memset(&garma_marks[i], 0, sizeof(garma_marks[i]));
    if(i>=0 && tri_identity(i))memset(&tri_marks[i],0,sizeof(tri_marks[i]));
    if(i>=0){memset(&performance_marks[i],0,sizeof(performance_marks[i]));memset(&chaos_marks[i],0,sizeof(chaos_marks[i]));}
    if(i>=0)memset(&serpent_marks[i],0,sizeof(serpent_marks[i]));
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
    if(tri_identity(i) && (card->flags&DUEL_CARD_FLAG_OCCUPIED))tri_marks[i].battle_flags=card->flags;
    /* Field attack commit temporarily deactivates both records BEFORE
     * switching to battle phase 9. Defer classification until its return.
     * All other deactivations (including Ritual tributes) still clear it. */
    if (inside_field_actions && i >= 0 && marks[i].ritual)
        deferred_deactivation |= 1u << i;
    else
        forget(card);
    if (inside_field_actions && i >= 0 && yamadron_marks[i].ritual)
        deferred_yamadron |= 1u << i;
    else
        forget_yamadron(card);
    if (i >= 0 && hungry_marks[i].ritual) {
        if (inside_field_actions) deferred_hungry |= 1u << i;
        else { hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0; }
    }
    if (!inside_field_actions && (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9)
        mask_forget(card->data);
    if (i >= 0 && javelin_marks[i].ritual) {
        if (inside_field_actions) deferred_javelin |= 1u << i;
        else if (javelin_should_protect(card)) return;
        else memset(&javelin_marks[i], 0, sizeof(javelin_marks[i]));
    }
    if (i >= 0 && gate_marks[i].ritual && !inside_field_actions && (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9)
        memset(&gate_marks[i], 0, sizeof(gate_marks[i]));
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
    unsigned n=0; int i;
    if(!ritual_card_effect_enabled(SERPENT_NIGHT_ID))return 0;
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        int live;
        if(!monster_slot(i) || side_for_record(i)!=side || !serpent_marks[i].ritual) continue;
        if(c->card_id!=SERPENT_NIGHT_ID || c->data!=serpent_marks[i].data) continue;
        /* FieldActions deactivates both battle records before phase 9.
         * A defending Ritual Serpent is still a valid Fear source itself;
         * every other Serpent source must still be live on the field. */
        live=(c->flags&DUEL_CARD_FLAG_OCCUPIED)!=0;
        if(live || i==defender_slot) ++n;
    }
    return n;
}

#include "serpent_fear.h"

static void scene_update(void)
{
    original_scene_update();
    chaos_sequence();
    serpent_ghost_update();
    crab_umi_update_visual();
    if(!crab_umi_sequence && tri_field_pending && !(gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        tri_field_pending=0;
        if(ritual_card_effect_enabled(TRI_HORNED_ID)) {ritual_field_id=WASTELAND_ID;crab_umi_sequence=1;}
    }
    if (!crab_umi_sequence || !ritual_card_effect_enabled(ritual_field_id==UMI_ID?CRAB_TURTLE_ID:TRI_HORNED_ID)) return;
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

static void field_actions(void)
{
    if(chaos_turn.active)return;
    if(coin_battle.active){coin_restore_dodge();memset(&coin_battle,0,sizeof(coin_battle));}
    coin_reconcile();
    if(serpent_fear_battle_slot>=0){ serpent_fear_bonus[serpent_fear_battle_slot]=0; serpent_fear_battle_slot=-1; }
    serpent_reconcile();
    mask_reconcile();
    restore_generated_field_thumbnails();
    int i;

    unsigned attack_counts[DUEL_CARD_RECORD_COUNT];
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i)
        attack_counts[i] = (ritual_card_effect_enabled(SHIELD_ID) && marks[i].ritual) ? count_attack_monsters_except(i) : 0;
    deferred_deactivation = 0;
    deferred_yamadron = 0;
    deferred_hungry = 0;
    deferred_javelin = 0;
    /* Psycho-Puppet may change position, but while it controls a borrowed
     * monster it must never become an eligible attacker again on later turns.
     * Reassert the retail used-this-turn attack lock every field tick. */
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (psycho_marks[i].ritual && psycho_marks[i].controlling) {
            int ps = find_occupied_monster_by_data(psycho_marks[i].psycho_data);
            if (ps >= 0 && D_801A7AD8[ps].card_id == PSYCHO_PUPPET_ID)
                D_801A7AD8[ps].flags |= DUEL_CARD_FLAG_USED_THIS_TURN;
        }
    }
    inside_field_actions = 1;
    original_field_actions();
    inside_field_actions = 0;
    mirror_watch_lp();

    /* Ritual Summon is Psycho-Puppet's first once-per-turn activation.
     * Field actions only resume after the Ritual effect has finished cleaning
     * up its tributes, so this is the first reliable point at which free
     * Monster Zones can be evaluated. Consume the pending trigger regardless
     * of success: if there is no target or no free zone now, Psycho-Puppet
     * simply does not steal a monster this turn. */
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (psycho_marks[i].ritual && psycho_marks[i].pending_summon_trigger) {
            psycho_marks[i].pending_summon_trigger = 0;
            psycho_attempt_control(&psycho_marks[i], "invocacao-pos-ritual");
        }
    }
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (!(deferred_deactivation & (1u << i))) continue;
        if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 &&
            battle_participant(i)) {
            marks[i].in_battle = 1;
            marks[i].attack_count = attack_counts[i];
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Millennium Shield entrou em combate: slot %d, atacantes=%u, bonus DEF=%u",
                          i, marks[i].attack_count, marks[i].attack_count * BONUS);
        } else {
            forget(&D_801A7AD8[i]);
        }
    }
    deferred_deactivation = 0;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (!(deferred_yamadron & (1u << i))) continue;
        if (!((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 && battle_participant(i)))
            forget_yamadron(&D_801A7AD8[i]);
    }
    deferred_yamadron = 0;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (!(deferred_hungry & (1u << i))) continue;
        if (!((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 && battle_participant(i))) {
            hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0;
        }
    }
    deferred_hungry = 0;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (!(deferred_javelin & (1u << i))) continue;
        if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9 && battle_participant(i))
            javelin_marks[i].battle_flags = D_801A7AD8[i].flags | DUEL_CARD_FLAG_OCCUPIED;
        else
            memset(&javelin_marks[i], 0, sizeof(javelin_marks[i]));
    }
    deferred_javelin = 0;
    /* A persistent Garma mark survives temporary deactivation, but never a
     * genuinely different occupant taking the same slot. */
    for (i=0; i<DUEL_CARD_RECORD_COUNT; ++i) {
        if (!garma_marks[i].ritual) continue;
        if ((D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            (D_801A7AD8[i].card_id != GARMA_SWORD_ID ||
             (D_801A7AD8[i].data && D_801A7AD8[i].data != garma_marks[i].data)))
            memset(&garma_marks[i], 0, sizeof(garma_marks[i]));
    }

    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i) {
        if(tri_marks[i].ritual && (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED) && !tri_identity(i))
            memset(&tri_marks[i],0,sizeof(tri_marks[i]));
    }
    /* Gate Guardian may attack up to three times: retail's used flag is cleared
     * between attacks while charges remain. */
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (gate_marks[i].ritual && gate_marks[i].attacks_this_turn < 3 &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].card_id == GATE_GUARDIAN_ID &&
            D_801A7AD8[i].data == gate_marks[i].data) D_801A7AD8[i].flags &= ~DUEL_CARD_FLAG_USED_THIS_TURN;
    }
    /* Garma unlocks an extra attack after a battle kill; using it spends a blade.
     * The extra attack is granted immediately, but its Sword of Dark Destruction
     * presentation waits until the camera has returned to the field. */
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (ritual_card_effect_enabled(GARMA_SWORD_ID) && garma_marks[i].ritual && garma_marks[i].extra_attack_pending &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) && D_801A7AD8[i].card_id == GARMA_SWORD_ID && D_801A7AD8[i].data == garma_marks[i].data) {
            D_801A7AD8[i].flags &= ~DUEL_CARD_FLAG_USED_THIS_TURN;
            garma_marks[i].extra_attack_pending = 0;
            garma_marks[i].extra_attack_ready = 1;
            garma_pending[i] = garma_marks[i].data;
            garma_screen[i].valid = 0; /* Require fresh field draws after the battle camera. */
        }
    }
    garma_visual_tick();
    /* Present Hungry Burger only after retail has rebuilt its field object. */
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (hungry_visual_pending[i] && (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            D_801A7AD8[i].card_id == HUNGRY_BURGER_ID && D_801A7AD8[i].data == hungry_visual_pending[i] && D_801A7AD8[i].object) {
            visual_queue_activation(i); visual_queue_stat(i, HUNGRY_BONUS); hungry_visual_pending[i] = 0;
        }
    }
    visual_update_activation();
    visual_update_stats();
}


static void card_placement(void)
{
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

static void battle(void)
{
    ++serpent_dbg_battle_calls;
    /* Catch the ready defender panel before native state 3 advances, even
     * when a face-up replacement does not request the Fear ghost hold. */
    serpent_redirect_begin();
    /* Fear ambush: before retail reveals the defending card, snapshot a face-down
     * defender attacked by the opposing side. One +1000/+1000 layer is added per
     * live ritual-summoned Serpent on the defender's side. The battle state is
     * held while the short ghost presentation runs. */
    if(serpent_ghost_state==2)return;
    /* State 3 starts after the native defender reveal has finished, before
     * the fade/result calculation. Keep the working snapshot below intact. */
    if(serpent_ghost_state==1 && (gDuel_wSceneStateFlags&0x8000) &&
       (D_8009B174&0x8f)==3) {
        serpent_ghost_begin();
        if(serpent_ghost_state==2)return;
    }
    if(serpent_fear_battle_slot<0 && ritual_card_effect_enabled(SERPENT_NIGHT_ID)){
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
    if(coin_before_battle())return;
    unsigned tri_restore=tri_battle_survivors();
    /* Remember that a ritual Fiend's Mirror entered this battle alive.
     * Its final battle-damage reflection is still legal even if that same
     * battle removes it from the field before LP loss is observed. */
    if (fiends_mirror_effect_on && (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9) {
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
                fiend_mirror_marks[idx].ritual) {
                int side = idx >= 15;
                mirror_battle_grace[side] = 1;
            }
        }
        if (side_has_ritual_fiend_mirror(0)) mirror_battle_grace[0] = 1;
        if (side_has_ritual_fiend_mirror(1)) mirror_battle_grace[1] = 1;
    }
    int i;
    /* State 11 rebuilds survivors and releases the battle display objects.
     * Inspect the survivor slots before the original call releases them;
     * this also covers records rebuilt for a 3D battle preview. */
    if ((D_8009B174 & 0x8f) == 11) {
        /* Result objects i+2 exist for battle survivors. Hungry Burger gains
         * its bonus only when it survives and the opposing monster does not. */
        for (i = 0; i < 2; ++i) {
            DisplayObject *participant = D_800E9EF0[i];
            int idx = participant ? (int)participant->field_6A : -1;
            if (ritual_card_effect_enabled(HUNGRY_BURGER_ID) && idx >= 0 && idx < DUEL_CARD_RECORD_COUNT && hungry_marks[idx].ritual &&
                hungry_marks[idx].data == D_801A7AD8[idx].data &&
                D_800E9EF0[i + 2] && !D_800E9EF0[(1 - i) + 2] &&
                !(D_800E9EF0[1-i] && (tri_restore&(1u<<D_800E9EF0[1-i]->field_6A)))) {
                ++hungry_marks[idx].battle_kills;
                hungry_visual_pending[idx] = hungry_marks[idx].data;
                if (host && host->log)
                    host->log(host, "[Ritual Card Effects] Hungry Burger destruiu monstro em batalha: slot %d, bonus=%u",
                              idx, hungry_marks[idx].battle_kills * HUNGRY_BONUS);
            }
        }
        for (i = 0; i < 2; ++i) {
            if (D_800E9EF0[i] && !D_800E9EF0[i + 2]) {
                int index = D_800E9EF0[i]->field_6A;
                if (monster_slot(index)) { forget(&D_801A7AD8[index]); forget_yamadron(&D_801A7AD8[index]); }
            }
        }
    }
    if ((D_8009B174 & 0x8f) == 11) {
        for (i = 0; i < 2; ++i) {
            DisplayObject *participant = D_800E9EF0[i]; int idx = participant ? (int)participant->field_6A : -1;
            if (idx >= 0 && idx < DUEL_CARD_RECORD_COUNT && side_for_record(idx) == D_8009B1D5 &&
                gate_marks[idx].ritual && gate_marks[idx].data == D_801A7AD8[idx].data &&
                D_801A7AD8[idx].card_id == GATE_GUARDIAN_ID && gate_marks[idx].attacks_this_turn < 3)
                ++gate_marks[idx].attacks_this_turn;
        }
    }
    /* Battle state 11 already knows which participant survived.  Retail has
     * temporarily deactivated the field record by this point, so lifecycle
     * hooks alone are too late to prevent Javelin's battle destruction.
     * Detect the first losing result here, let retail finish all battle damage
     * and presentation normally, then rebuild that same field instance. */
    unsigned javelin_restore = 0;
    if ((D_8009B174 & 0x8f) == 11 && ritual_card_effect_enabled(JAVELIN_BEETLE_ID)) {
        for (i = 0; i < 2; ++i) {
            DisplayObject *participant = D_800E9EF0[i];
            int idx = participant ? (int)participant->field_6A : -1;
            if (idx >= 0 && idx < DUEL_CARD_RECORD_COUNT && !D_800E9EF0[i + 2] &&
                javelin_marks[idx].ritual && !javelin_marks[idx].protection_used_turn &&
                D_801A7AD8[idx].card_id == JAVELIN_BEETLE_ID &&
                D_801A7AD8[idx].data == javelin_marks[idx].data) {
                javelin_marks[idx].protection_used_turn = 1;
                javelin_restore |= 1u << idx;
                if (host && host->log) host->log(host, "[Ritual Card Effects] Javelin Beetle marcou protecao de batalha: slot %d", idx);
            }
        }
    }
    unsigned crab_restore = 0;
    if ((D_8009B174 & 0x8f) == 11 && ritual_card_effect_enabled(CRAB_TURTLE_ID) && gDuel_bTerrain == UMI_TERRAIN) {
        for (i=0;i<2;++i) {
            DisplayObject *p=D_800E9EF0[i], *enemy=D_800E9EF0[1-i]; int idx=p?(int)p->field_6A:-1, eidx=enemy?(int)enemy->field_6A:-1;
            if(idx>=0 && eidx>=0 && !D_800E9EF0[i+2] && crab_marks[idx].ritual && D_801A7AD8[idx].card_id==CRAB_TURTLE_ID && D_801A7AD8[idx].data==crab_marks[idx].data) {
                int et=card_type_id(&D_801A7AD8[eidx]);
                if(et!=CARD_TYPE_AQUA && et!=CARD_TYPE_FISH && et!=CARD_TYPE_SEA_SERPENT && et!=CARD_TYPE_THUNDER) crab_restore|=1u<<idx;
            }
        }
    }
    /* Only participant zero initiated this battle. Direct attacks have no
     * defender and defensive victories never award an extra blade. */
    if ((D_8009B174 & 0x8f) == 11) {
        int a=D_800E9EF0[0] ? D_800E9EF0[0]->field_6A : -1;
        int d=D_800E9EF0[1] ? D_800E9EF0[1]->field_6A : -1;
        garma_spend_extra(a);
        garma_award(a,d,D_800E9EF0[2]!=0, D_800E9EF0[3]!=0 ||
            (monster_slot(d) && ((javelin_restore|crab_restore|tri_restore)&(1u<<d))));
        if(monster_slot(a) && !D_800E9EF0[2]) memset(&garma_marks[a],0,sizeof(garma_marks[a]));
        if(monster_slot(d) && !D_800E9EF0[3]) memset(&garma_marks[d],0,sizeof(garma_marks[d]));
    }

    original_battle();
    coin_after_battle();
    /* Same post-result survivor reconstruction as Javelin/Crab. LP damage
     * and the completed native battle remain untouched. */
    if(tri_restore)for(i=0;i<DUEL_CARD_RECORD_COUNT;++i)if(tri_restore&(1u<<i)) {
        DuelCardRecord *c=&D_801A7AD8[i];
        c->flags=tri_marks[i].battle_flags|DUEL_CARD_FLAG_OCCUPIED;
        if(side_for_record(i)==D_8009B1D5)c->flags|=DUEL_CARD_FLAG_USED_THIS_TURN;
        if(c->object)DisplayObject_ReleaseIfPresent(c->object);
        c->object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y);
        if(c->object)((DuelCardDisplayObject*)c->object)->card_index=(u8)i;
        tri_battle_visuals|=1u<<i;
    }
    if (crab_restore) {
        for(i=0;i<DUEL_CARD_RECORD_COUNT;++i) if(crab_restore&(1u<<i)) { DuelCardRecord *c=&D_801A7AD8[i]; c->flags=crab_marks[i].battle_flags|DUEL_CARD_FLAG_OCCUPIED; if(side_for_record(i)==D_8009B1D5)c->flags|=DUEL_CARD_FLAG_USED_THIS_TURN; if(c->object)DisplayObject_ReleaseIfPresent(c->object); c->object=func_80024C1C(c->card_id,D_800908A0[i].x,D_800908A0[i].y); if(c->object)((DuelCardDisplayObject*)c->object)->card_index=(u8)i; if(!(crab_restore_visuals&(1u<<i))){visual_queue_activation(i);crab_restore_visuals|=1u<<i;} }
    }
    if (javelin_restore) {
        for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
            DuelCardRecord *c;
            if (!(javelin_restore & (1u << i))) continue;
            c = &D_801A7AD8[i];
            c->flags = javelin_marks[i].battle_flags | DUEL_CARD_FLAG_OCCUPIED;
            if (side_for_record(i) == D_8009B1D5) c->flags |= DUEL_CARD_FLAG_USED_THIS_TURN;
            if (c->object) DisplayObject_ReleaseIfPresent(c->object);
            c->object = func_80024C1C(c->card_id, D_800908A0[i].x, D_800908A0[i].y);
            if (c->object) ((DuelCardDisplayObject *)c->object)->card_index = (u8)i;
            javelin_protected_visuals |= 1u << i;
        }
    }
    mirror_watch_lp();
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 9) return;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (javelin_protected_visuals & (1u << i)) {
            DuelCardRecord *c = &D_801A7AD8[i];
            if (javelin_marks[i].ritual && (c->flags & DUEL_CARD_FLAG_OCCUPIED) && c->data == javelin_marks[i].data && c->card_id == JAVELIN_BEETLE_ID) {
                if (c->object) DisplayObject_ReleaseIfPresent(c->object);
                c->object = func_80024C1C(c->card_id, D_800908A0[i].x, D_800908A0[i].y);
                if (c->object) ((DuelCardDisplayObject *)c->object)->card_index = (u8)i;
                if (!javelin_marks[i].protection_visual_shown) { visual_queue_activation(i); javelin_marks[i].protection_visual_shown = 1; }
            }
        }
    }
    javelin_protected_visuals = 0;
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i)if(tri_battle_visuals&(1u<<i)) {
        if(tri_live(i))visual_queue_activation(i);
        tri_horn_visuals&=~(1u<<i);
    }
    tri_battle_visuals=0;
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i)if(tri_identity(i)&&!(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED))
        memset(&tri_marks[i],0,sizeof(tri_marks[i]));
    mask_reconcile();
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (psycho_marks[i].ritual && psycho_marks[i].controlling &&
            find_occupied_monster_by_data(psycho_marks[i].stolen_data) < 0) {
            psycho_marks[i].controlling = 0;
            psycho_marks[i].stolen_data = 0;
            psycho_marks[i].stolen_card_id = 0;
            psycho_marks[i].original_slot = psycho_marks[i].current_slot = -1;
        }
    }
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *h = &D_801A7AD8[i];
        if (hungry_marks[i].ritual &&
            (!(h->flags & DUEL_CARD_FLAG_OCCUPIED) || h->data != hungry_marks[i].data || h->card_id != HUNGRY_BURGER_ID)) {
            hungry_marks[i].ritual = 0; hungry_marks[i].data = 0; hungry_marks[i].battle_kills = 0;
        }
    }
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *y = &D_801A7AD8[i];
        if (yamadron_marks[i].ritual &&
            (!(y->flags & DUEL_CARD_FLAG_OCCUPIED) || y->data != yamadron_marks[i].data || y->card_id != YAMADRON_ID))
            forget_yamadron(y);
    }
    /* Retail rebuilds survivors before leaving phase 9. A destroyed card
     * remains inactive; a fusion result has another identity. */
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *card = &D_801A7AD8[i];
        if (!marks[i].ritual) continue;
        if (!(card->flags & DUEL_CARD_FLAG_OCCUPIED) ||
            card->data != marks[i].data || card->card_id != marks[i].card_id) {
            forget(card);
        } else if (marks[i].in_battle) {
            /* A shield that participated already took its fresh reading at
             * battle commit, so keep that value after the battle. */
            marks[i].in_battle = 0;
        } else {
            /* Every other ritual Millennium Shield takes a post-battle
             * reading, after destroyed monsters have left the field. */
            {
                unsigned old_count = marks[i].attack_count;
                unsigned new_count = count_attack_monsters_except(i);
                marks[i].attack_count = new_count;
                if (new_count > old_count) {
                    if (!marks[i].def_visual_shown) { visual_queue_activation(i); marks[i].def_visual_shown = 1; }
                    visual_queue_stat(i, (int)((new_count - old_count) * BONUS));
                }
            }
            if (host && host->log)
                host->log(host, "[Ritual Card Effects] Millennium Shield releitura pos-combate: slot %d, atacantes=%u, bonus DEF=%u",
                          i, marks[i].attack_count, marks[i].attack_count * BONUS);
        }
    }
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) != 9 && serpent_fear_battle_slot>=0) {
        serpent_fear_bonus[serpent_fear_battle_slot]=0; serpent_fear_battle_slot=-1;
    }
}

static void restore_protected_visuals(void)
{
    int i;
    unsigned survivors = effect_survivor_visuals | protected_visuals;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *card;
        DuelCardDisplayObject *obj;
        if (!(survivors & (1u << i))) continue;
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
        if((tri_horn_visuals&(1u<<i)) && tri_live(i))visual_queue_activation(i);
        if (host && host->log)
            host->log(host, "[Ritual Card Effects] effect-survival visual resync: slot %d card %u", i, (unsigned)card->card_id);
    }
    protected_visuals = 0;
    effect_survivor_visuals = 0;
    tri_horn_visuals=0;
}

static void reread_all_shields(const char *reason)
{
    int i;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        DuelCardRecord *card = &D_801A7AD8[i];
        if (!marks[i].ritual) continue;
        if (!(card->flags & DUEL_CARD_FLAG_OCCUPIED) ||
            card->data != marks[i].data || card->card_id != marks[i].card_id) {
            forget(card);
            continue;
        }
        {
            unsigned old_count = marks[i].attack_count;
            unsigned new_count = count_attack_monsters_except(i);
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
static void board_destruction(void)
{ unsigned old=whale_destruction_guard; whale_destruction_guard|=whale_snapshot_protection(); inside_effect_destruction++; original_board_destruction(); inside_effect_destruction--; whale_destruction_guard=old; }
static void monster_removal(void)
{ unsigned old=whale_destruction_guard; whale_destruction_guard|=whale_snapshot_protection(); inside_effect_destruction++; original_monster_removal(); inside_effect_destruction--; whale_destruction_guard=old; }
static void raigeki(void)
{ unsigned old=whale_destruction_guard; whale_destruction_guard|=whale_snapshot_protection(); inside_effect_destruction++; original_raigeki(); inside_effect_destruction--; whale_destruction_guard=old; }

/* Card effects are multi-frame. Refresh only on the tick where the active
 * flag actually clears, i.e. after the spell/effect has finished resolving. */
static s32 update_card_effect(void)
{
    tri_begin_resolution();
    int was_active = (gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE) != 0;
    s16 old_atk[DUEL_CARD_RECORD_COUNT], old_def[DUEL_CARD_RECORD_COUNT], old_mod[DUEL_CARD_RECORD_COUNT]; int k; int whale_guard = was_active && gDuel_bTerrain==UMI_TERRAIN && effect_is_magic_or_trap();
    if(whale_guard) for(k=0;k<DUEL_CARD_RECORD_COUNT;++k){old_atk[k]=D_801A7AD8[k].attack;old_def[k]=D_801A7AD8[k].defense;old_mod[k]=D_801A7AD8[k].stat_modifier;}
    s32 result = original_update_card_effect();
    if(whale_guard) for(k=0;k<DUEL_CARD_RECORD_COUNT;++k){DuelCardRecord*c=&D_801A7AD8[k]; if(monster_slot(k)&&(c->flags&DUEL_CARD_FLAG_OCCUPIED)&&whale_protects_target(k)&&sea_family_type(card_type_id(c))){if(c->attack<old_atk[k])c->attack=old_atk[k];if(c->defense<old_def[k])c->defense=old_def[k];if(c->stat_modifier<old_mod[k])c->stat_modifier=old_mod[k];}}
    mirror_watch_lp();
    if (was_active && !(gDuel_wCardEffectFlags & DUEL_CARD_EFFECT_FLAG_ACTIVE)) {
        restore_protected_visuals();
        reread_all_shields("pos-magia");
        tri_resolution_active=0;tri_guard=0;memset(tri_guard_data,0,sizeof(tri_guard_data));
    }
    return result;
}

static int side_has_ritual_fiend_mirror(int side)
{
    int i, start;
    if (!fiends_mirror_effect_on) return 0;
    start = side ? 20 : 5;
    { int end = side ? 25 : 10;
    for (i = start; i < end; ++i) {
        DuelCardRecord *c = &D_801A7AD8[i];
        if (!fiend_mirror_marks[i].ritual) continue;

        /* Battle survival is represented by mirror_battle_grace, snapshotted
         * before retail stages/deactivates the records.  Never treat a stale
         * Ritual mark itself as a live Mirror during battle: otherwise a Mirror
         * destroyed in the previous battle can become a permanent ghost source. */
        if (!(c->flags & DUEL_CARD_FLAG_OCCUPIED) || c->card_id != FIENDS_MIRROR_ID ||
            c->data != fiend_mirror_marks[i].data) {
            fiend_mirror_marks[i].ritual = 0;
            fiend_mirror_marks[i].data = 0;
            continue;
        }
        return 1;
    }
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

static void mirror_begin_reflection(int target_side, unsigned damage)
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
        life = D_800E9FF0[target_side].life_points.unsigned_value;
        D_800E9FF0[target_side].life_points.unsigned_value =
            (damage >= life) ? 0 : (u16)(life - damage);
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
    life = D_800E9FF0[target_side].life_points.unsigned_value;
    D_800E9FF0[target_side].life_points.unsigned_value =
        (damage >= life) ? 0 : (u16)(life - damage);
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
    mirror_queue_tail = (mirror_queue_tail + 1) % MIRROR_QUEUE_CAP;
    ++mirror_queue_count;
}

static void mirror_watch_lp(void)
{
    u16 now0, now1, before[2], now[2];
    int side;
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
    mirror_last_lp[0] = now0; mirror_last_lp[1] = now1;
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
            mirror_queue_head = (mirror_queue_head + 1) % MIRROR_QUEUE_CAP; --mirror_queue_count;
            mirror_begin_reflection(target_side, damage);
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
    /* Return every borrowed monster before retail hands the turn to the
     * opponent, then trigger the new controller's once-per-turn Ritual
     * effects after the side switch has completed. */
    psycho_return_for_side(before);
    { int i; for (i = 0; i < DUEL_CARD_RECORD_COUNT; ++i) {
        if (javelin_marks[i].ritual) javelin_marks[i].protection_used_turn = 0;
        if (gate_marks[i].ritual) gate_marks[i].attacks_this_turn = 0;
        garma_marks[i].extra_attack_ready = 0;
        garma_marks[i].extra_attack_pending = 0;
    }}
    original_turn_switch();
    mirror_watch_lp();
    if (D_8009B1D5 != before && (gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK) == 2) {
        chaos_begin_turn(D_8009B1D5);
        generate_for_side(D_8009B1D5, "inicio-do-turno");
        psycho_trigger_for_side(D_8009B1D5, "inicio-do-turno");
    }
}

static void populate(void)
{ clear(); original_populate(); }
static void applied(int on)
{ (void)on; sync_global_settings(); refresh_settings(); clear(); }

/* Reset deliberately clears classification: v0.1 requires a fresh ritual
 * after loading a save state. No unsaved pointer state is inferred/restored. */
static void overlay(void)
{
    ritual_coin_overlay();
    performance_dodge_flash();
    serpent_ghost_draw();
    serpent_redirect_draw();
    if (host && host->draw_text) {
        if (visual_popup_frames > 0 && host->overlay_size) {
            int width,height,scale,vw,vh,left,top,x,y,brightness; char text[16];
            host->overlay_size(host,&width,&height,&scale); vw=width; vh=width*3/4;
            if(vh>height){vh=height;vw=height*4/3;} left=(width-vw)/2; top=(height-vh)/2;
            x=left+visual_popup_x*vw/320; y=top+(visual_popup_y-(STAT_POPUP_FRAMES-visual_popup_frames)/2)*vh/240;
            if(scale<1)scale=1; brightness=visual_popup_frames*255/STAT_POPUP_FRAMES; snprintf(text,sizeof(text),"+%d",visual_popup_amount);
            host->draw_text(host,x+scale,y+scale,text,0x000000,scale);
            host->draw_text(host,x,y,text,(unsigned)brightness<<8,scale);
        }
        garma_icons();
        tri_icons();
        if(debug_overlay){
        char mask_dbg[120];
        snprintf(mask_dbg, sizeof(mask_dbg), "Mask: Fusion bloqueada=%u | IA aprendeu=%u", mask_blocked, mask_learned);
        host->draw_text(host, 12, 54, mask_dbg, 0xffff80, 1);
        snprintf(mask_dbg, sizeof(mask_dbg), "IA Fusion liberada=%u | IA conhece lados=%d", mask_released, mask_known_sides);
        host->draw_text(host, 12, 66, mask_dbg, 0xffff80, 1);
        host->draw_text(host, 12, 18, (summons || yamadron_summons || psycho_summons || hungry_summons || fiend_mirror_summons || mask_summons || serpent_dbg_ritual) ?
            "Efeitos Rituais v0.74 TEST: Ritual detectado" :
            "Efeitos Rituais v0.74 TEST: ativo; aguardando Ritual", 0x80ff80, 1);
        host->draw_text(host, 12, 30, fiends_mirror_effect_on ?
            "Fiend Mirror setting: ON" : "Fiend Mirror setting: OFF",
            fiends_mirror_effect_on ? 0x80ff80 : 0xff8080, 1);
        { char dbg[96]; snprintf(dbg, sizeof(dbg), "Mirror ritual=%u queue=%u", fiend_mirror_summons, mirror_queue_count); host->draw_text(host, 12, 42, dbg, 0xffffff, 1); }
        }
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
    unsigned values[]={
        gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK,
        gDuel_wCardEffectFlags,gDuel_bEffectState,
        (unsigned)crab_umi_visual_state,D_8009B1D5,D_8009B229,
        summons,yamadron_summons,psycho_summons,hungry_summons,
        fiend_mirror_summons,mask_summons,serpent_dbg_ritual,
        mask_blocked,mask_learned,mask_released,(unsigned)mask_known_sides,
        (unsigned)fiends_mirror_effect_on,mirror_queue_count,
        (unsigned)visual_popup_frames,(unsigned)visual_popup_x,
        (unsigned)visual_popup_y,(unsigned)visual_popup_amount,
        (unsigned)serpent_ghost_state,(unsigned)serpent_art_loaded,
        (unsigned)serpent_ghost_x,(unsigned)serpent_ghost_y,
        serpent_ghost_state==2?(unsigned)serpent_ghost_fade():0,
        (unsigned)serpent_redirect_state,
        serpent_redirect_state==2 && serpent_redirect_hold<6?(unsigned)serpent_redirect_hold:6,
        (unsigned)ritual_card_effect_enabled(GARMA_SWORD_ID),
        (unsigned)ritual_card_effect_enabled(TRI_HORNED_ID),
        (unsigned)ritual_card_effect_enabled(SERPENT_NIGHT_ID)
    };
    int i;
    { unsigned coin_visible[]={ritual_coin.active,ritual_coin.style,ritual_coin.shown,
            ritual_coin.lift,ritual_coin_port_ui(),debug_overlay};
      hash=overlay_hash(hash,coin_visible,sizeof(coin_visible)); }
    hash=overlay_hash(hash,&coin_battle.dodge_ticks,sizeof(coin_battle.dodge_ticks));
    hash=overlay_hash(hash,values,sizeof(values));
    hash=overlay_hash(hash,&D_8009B1B4,sizeof(D_8009B1B4));
    hash=overlay_hash(hash,D_800E9F10,sizeof(DuelSelectionSide)*DUEL_SIDE_COUNT);
    hash=overlay_hash(hash,D_800907D8,DUEL_FIELD_SIDE_GRID_SLOT_COUNT*DUEL_SIDE_COUNT*sizeof(D_800907D8[0]));
    hash=overlay_hash(hash,garma_marks,sizeof(garma_marks));
    hash=overlay_hash(hash,tri_marks,sizeof(tri_marks));
    hash=overlay_hash(hash,garma_screen,sizeof(garma_screen));
    hash=overlay_hash(hash,tri_screen,sizeof(tri_screen));
    for(i=0;i<DUEL_CARD_RECORD_COUNT;++i) {
        DuelCardRecord *c=&D_801A7AD8[i];
        if(!garma_marks[i].ritual && !tri_marks[i].ritual)continue;
        hash=overlay_hash(hash,&c->flags,sizeof(c->flags));
        hash=overlay_hash(hash,&c->card_id,sizeof(c->card_id));
        hash=overlay_hash(hash,&c->data,sizeof(c->data));
        hash=overlay_hash(hash,&c->object,sizeof(c->object));
    }
    return hash;
}

static void shutdown(void)
{
    if(host&&host->unsubscribe&&ritual_damage_token){host->unsubscribe(host,ritual_damage_token);ritual_damage_token=0;}
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

int MemoriesModInit(const MemoriesModHost *from, MemoriesMod *mod)
{
    if (!from || !mod || from->api < 4 || !from->hook || !from->unhook) return 0;
    host = from;
    compat_lookup_recipe=compat_private_recipe;
    compat_lookup_requirements=compat_private_requirements;
    sync_global_settings();
    refresh_settings();
    clear();
    if(!host->subscribe||!host->unsubscribe){shutdown();return 0;}
    ritual_damage_token=host->subscribe(host,MEMORIES_EVENT_DAMAGE,-10000,coin_damage);
    if(!ritual_damage_token){shutdown();return 0;}
    ritual_coin_load(0);ritual_coin_load(1);
    if(!ritual_coin_sets[0].count||!ritual_coin_sets[1].count){shutdown();return 0;}
    ritual_port_menu=(int (*)(void))host->symbol(host,"Menu_IsOpen");
    ritual_port_notice=(int (*)(void))host->symbol(host,"Menu_NoticeShown");
    if (!install((void *)Input_UpdatePads,(void *)coin_update_pads,(void **)&ritual_original_pads) ||
        !install((void *)Main_RunDuel,(void *)coin_run_duel,(void **)&ritual_original_run) ||
        !install((void *)Main_RunAnimatedBattle,(void *)coin_animated_battle,(void **)&ritual_original_animated) ||
        !install((void *)func_800559D4,(void *)coin_model_control,(void **)&ritual_original_model_control) ||
        !install((void *)func_80017F04, (void *)serpent_stage_card, (void **)&serpent_original_stage) ||
        !install((void *)AiScript_Run, (void *)serpent_ai_run, (void **)&serpent_original_ai_run) ||
        !install((void *)AiScript_FindWeakest, (void *)serpent_ai_find_weakest, (void **)&serpent_original_ai_weakest) ||
        !install((void *)Ai_IsCardInSets, (void *)serpent_ai_in_sets, (void **)&serpent_original_ai_in_sets) ||
        !install((void *)AiScript_FindBestAttack, (void *)serpent_ai_best_attack, (void **)&serpent_original_ai_best_attack) ||
        !install((void *)DuelScene_Update, (void *)scene_update, (void **)&original_scene_update) ||
        !install((void *)func_80016784, (void *)visual_card_frame_draw, (void **)&original_card_frame_draw) ||
        !install((void *)DuelEffect_ApplyRitual, (void *)apply_ritual, (void **)&original_apply) ||
        !install((void *)Duel_CalcCardStats, (void *)calc_stats, (void **)&original_stats) ||
        !install((void *)DuelCard_RemoveFromField, (void *)remove_card, (void **)&original_remove) ||
        !install((void *)DuelCard_DeactivateRecord, (void *)deactivate_card, (void **)&original_deactivate) ||
        !install((void *)Duel_PopulateCombinedDeckData, (void *)populate, (void **)&original_populate) ||
        !install((void *)DuelScene_UpdateFieldActions, (void *)field_actions, (void **)&original_field_actions) ||
        !install((void *)DuelScene_UpdateCardPlacement, (void *)card_placement, (void **)&original_card_placement) ||
        !install((void *)DuelScene_UpdateHandActions, (void *)hand_actions, (void **)&original_hand_actions) ||
        !install((void *)DuelScene_UpdateBattle, (void *)battle, (void **)&original_battle) ||
        !install((void *)DuelEffect_UpdateCardEffect, (void *)update_card_effect, (void **)&original_update_card_effect) ||
        !install((void *)DuelEffect_ApplyBoardDestruction, (void *)board_destruction, (void **)&original_board_destruction) ||
        !install((void *)DuelEffect_ApplyMonsterRemoval, (void *)monster_removal, (void **)&original_monster_removal) ||
        !install((void *)DuelEffect_ApplyRaigeki, (void *)raigeki, (void **)&original_raigeki) ||
        !install((void *)DuelScene_UpdateTurnSwitch, (void *)turn_switch, (void **)&original_turn_switch) ||
        !install((void *)Duel_CheckFusion, (void *)mask_check_fusion, (void **)&original_check_fusion) ||
        !install((void *)Duel_CheckRitual, (void *)mask_check_ritual, (void **)&original_check_ritual)) {
        shutdown(); return 0;
    }
    mod->api = 4;
    mod->name = "MOD Efeitos de Cartas Rituais";
    mod->reset = clear;
    mod->applied = applied;
    mod->shutdown = shutdown;
    mod->frame = coin_frame;
    mod->overlay = overlay;
    mod->overlay_signature = overlay_signature;
    return 1;
}
