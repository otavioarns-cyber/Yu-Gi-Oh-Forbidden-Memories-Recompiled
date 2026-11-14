#include "types.h"
#include "unmatched.h"
#include "pc/mods/modapi.h"
#include "ritual_recipe_provider_v1.h"
#include "ritual_rules_provider_v1.h"
#include "surrender.h"
#include "game/duel_card.h"
#include "game/duel_check_ritual.h"
#include "game/duel_hand.h"
#define D_8009B360_AS_SIDE_ARRAY
#include "game/duel_side_state.h"
#include "game/duel_grid.h"
#include "game/duel_ritual_controller.h"
#include "game/duel_card_record_lifecycle.h"
#include "game/duel_deck_card.h"
#include "game/duel_deck_card_data.h"
#include "game/duel_card_display_state.h"
#include "game/display_object.h"
#include "game/display_object_core.h"
#include "game/duel_scene_hand_actions.h"
#include "game/duel_selection_layout.h"
#include "game/input.h"
#include "game/card_constants.h"
#include "pc/cards/tables.h"
#include <string.h>

/* Optional lookups keep this mod independent of the recipe editor/data mod. */
static int (*custom_requirements)(int, TablesRitualRequirement *, unsigned short *);
static int (*custom_recipe)(int, unsigned short *);
static int (*custom_has_ritual)(int);
static int (*effect_id)(int);
static int (*base_id)(int);
static int (*card_type)(int);
static int (*card_level)(int);
static int (*in_group)(int, int);

static s32 (*o_check)(DuelRitualResult *, s32);
static s32 (*o_field_objects)(void *G32 *);
static s32 (*public_check)(DuelRitualResult *, s32);
static int oracle_mode;
static DisplayObject oracle_objects[3];
static void (*o_deact)(DuelCardRecord *);
static void (*o_hand_actions)(void);
static void (*o_populate)(void);
static const MemoriesModHost *mod_host;
/* Optional, read-only provider: pointers stay loaded while the producer is disabled. */
static const RitualRecipeProviderV1 *recipe_provider;

enum { RITUAL_SRC_NONE, RITUAL_SRC_FIELD, RITUAL_SRC_HAND, RITUAL_SRC_DECK };
enum { RESULT_OFF, RESULT_HAND, RESULT_HAND_OR_DECK };
enum { BRIDGE_NONE, BRIDGE_EXTENDED, BRIDGE_RETAIL_RESULT };

static int bridge_mode;
static int deact_step;
static int side;
static int where[3], slot[3], rec[3], matid[3], source_uid[3];
static int visual_rec[3], preconsumed[3], consume_order[3];
static void *obj[3];
static int recipe_result;
static int active_result;
static void *cached_tribute_objects[3];
static int synthetic_anchor_rec = -1;

static int result_where;
static int result_slot;
static int result_uid;

static int setting_min_field;
static int setting_use_deck;
static int setting_min_hand;
static int setting_require_result;

#define DECK_VISUAL_X 210
#define DECK_VISUAL_Z 190
#define HAND_VISUAL_X 0
#define HAND_VISUAL_Z 230

static int clamp_setting(int value, int low, int high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

static void read_ritual_settings(void)
{
    setting_min_field = clamp_setting(mod_host->setting(mod_host, "min_field", 1), 0, 3);
    setting_use_deck = !!mod_host->setting(mod_host, "use_deck", 0);
    setting_min_hand = clamp_setting(mod_host->setting(mod_host, "min_hand", 0), 0, 3);
    setting_require_result = clamp_setting(mod_host->setting(mod_host, "require_result", 0), 0, 2);
}

/* Expose effective, live options to any future Ritual-aware AI. The host
 * settings API is scoped to this MOD, so consumers must not guess names,
 * duplicate Settings or read another mod's configuration file. */
static int read_rules_snapshot(RitualRulesSnapshotV1 *out, unsigned out_size)
{
    if (!out || out_size < sizeof(*out) || !mod_host ||
        !mod_host->applied(mod_host)) return 0;
    out->abi_version = RITUAL_RULES_PROVIDER_ABI_V1;
    out->material_count = 3u;
    out->sources = RITUAL_RULES_SOURCES_FIELD | RITUAL_RULES_SOURCES_HAND;
    out->min_field = clamp_setting(mod_host->setting(mod_host,"min_field",1),0,3);
    if (mod_host->setting(mod_host,"use_deck",0))
        out->sources |= RITUAL_RULES_SOURCES_DECK;
    out->min_hand = (out->sources & RITUAL_RULES_SOURCES_DECK) ?
        clamp_setting(mod_host->setting(mod_host,"min_hand",0),0,3) : 0;
    out->require_result = clamp_setting(mod_host->setting(mod_host,"require_result",0),0,2);
    out->ritual_only_player = !!mod_host->setting(mod_host,"ritual_only_player",0);
    return 1;
}
static const RitualRulesProviderV1 published_rules = {
    RITUAL_RULES_PROVIDER_ABI_V1, read_rules_snapshot
};

static int valid_record(int r)
{
    return r >= 0 && r < DUEL_CARD_RECORD_COUNT;
}

static int hand_record(int s, int h)
{
    return (int)D_800907CC[s * HAND_SIZE + h];
}

static int deck_begin(int s)
{
    return s * DECK_SIZE;
}

static int deck_end(int s)
{
    return deck_begin(s) + DECK_SIZE;
}

static int deck_position_for_uid(int s, int uid)
{
    int i;
    for (i = deck_begin(s); i < deck_end(s); i++)
        if (gDuel_aDeckCardRecords[i].data_block_index == uid) return i;
    return -1;
}

static int remaining_deck_position_for_uid(int s, int uid)
{
    int i = deck_begin(s) + D_800E9FF0[s].deck_draw_cursor;
    int end = deck_end(s);
    if (i < deck_begin(s)) i = deck_begin(s);
    if (i > end) i = end;
    for (; i < end; i++)
        if (gDuel_aDeckCardRecords[i].data_block_index == uid) return i;
    return -1;
}

/* Remove one arbitrary card from the undrawn deck without disturbing the
 * relative order of every other undrawn card. The chosen record is rotated
 * into the current draw cursor and the cursor advances over it. Returns the
 * global deck-record position that now holds the consumed card. */
static int consume_deck_uid(int s, int uid)
{
    int base = deck_begin(s);
    int cursor = D_800E9FF0[s].deck_draw_cursor;
    int first = base + cursor;
    int found = remaining_deck_position_for_uid(s, uid);
    s16 chosen_id;
    u8 chosen_data;
    int i;

    if (cursor < 0 || cursor >= DECK_SIZE || found < first) return -1;
    /* deck_index and the flag bytes are slot identity/state. The game
       itself moves card identity by moving id + data_block_index while
       leaving those bytes in their slots. */
    chosen_id = gDuel_aDeckCardRecords[found].id;
    chosen_data = gDuel_aDeckCardRecords[found].data_block_index;
    for (i = found; i > first; i--) {
        gDuel_aDeckCardRecords[i].id = gDuel_aDeckCardRecords[i - 1].id;
        gDuel_aDeckCardRecords[i].data_block_index =
            gDuel_aDeckCardRecords[i - 1].data_block_index;
    }
    gDuel_aDeckCardRecords[first].id = chosen_id;
    gDuel_aDeckCardRecords[first].data_block_index = chosen_data;
    D_800E9FF0[s].deck_draw_cursor = cursor + 1;
    return first;
}

static int find_empty_monster_record(int s)
{
    int i, r;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        r = s * DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE + i;
        if (!(D_801A7AD8[r].flags & DUEL_CARD_FLAG_OCCUPIED)) return r;
    }
    return -1;
}

static int selected_record(int r)
{
    int i;
    for (i = 0; i < 3; i++) if (rec[i] == r) return 1;
    return 0;
}

static int visual_record_in_use(int r)
{
    int i;
    if (r == synthetic_anchor_rec) return 1;
    for (i = 0; i < 3; i++) if (visual_rec[i] == r) return 1;
    return 0;
}

static int record_has_hand_visual(int r)
{
    int h;
    for (h = 0; h < HAND_SIZE; h++) {
        DisplayObject *visual = (DisplayObject *)D_800EA030[h].object;
        if (visual && visual->field_6A == r) return 1;
    }
    return 0;
}

/* Prefer unused records on the acting side. This is only for a temporary
 * visual of a deck tribute; gameplay ownership remains in the deck. */
static int scratch_record_free(int r)
{
    if (!valid_record(r) || selected_record(r) || visual_record_in_use(r) ||
        record_has_hand_visual(r)) return 0;
    return !(D_801A7AD8[r].flags & DUEL_CARD_FLAG_OCCUPIED);
}

static int find_scratch_record(void)
{
    int first = side * DUEL_CARD_SIDE_RECORD_COUNT;
    int r;

    /* Vanilla-style ritual visuals need a record Duel_DrawFieldCards actually
       visits. Prefer empty Magic/Trap zones: they are on the field renderer
       but the optional 3D Monsters mod never treats them as monster models. */
    for (r = first + DUEL_FIELD_ROW_SIZE * 2;
         r < first + DUEL_CARD_SIDE_RECORD_COUNT; r++)
        if (scratch_record_free(r)) return r;

    /* Then an unused monster zone (suppressed from 3D below), and only as a
       last compatibility fallback an unused hand record. */
    for (r = first + DUEL_FIELD_ROW_SIZE; r < first + DUEL_FIELD_ROW_SIZE * 2; r++)
        if (scratch_record_free(r)) return r;
    for (r = first; r < first + DUEL_FIELD_ROW_SIZE; r++)
        if (scratch_record_free(r)) return r;
    return -1;
}

static void clear_hand_source(int s, int h, int keep_visual)
{
    int r;
    DuelHandSlot *hs;
    if (h < 0 || h >= HAND_SIZE) return;
    hs = &D_800EA030[h];
    if (hs->child) {
        DisplayObject_ReleaseIfPresent(hs->child);
        hs->child = 0;
    }
    if (!keep_visual && hs->object) DisplayObject_ReleaseIfPresent(hs->object);
    hs->object = 0;
    hs->active_09 = 0;
    D_800E9FF0[s].hand[h] = -1;
    r = hand_record(s, h);
    if (valid_record(r)) {
        D_801A7AD8[r].flags = 0;
        D_801A7AD8[r].object = 0;
        D_801A7AD8[r].data = 0;
    }
}

static void reset_result_source(void)
{
    result_where = RITUAL_SRC_NONE;
    result_slot = -1;
    result_uid = -1;
}

static int reserve_result_source(int result)
{
    int h, r, i, start, end;
    reset_result_source();
    if (setting_require_result == RESULT_OFF) return 1;

    for (h = 0; h < HAND_SIZE; h++) {
        r = hand_record(side, h);
        if (D_800E9FF0[side].hand[h] >= 0 && valid_record(r) &&
            (D_801A7AD8[r].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            D_801A7AD8[r].card_id == result) {
            result_where = RITUAL_SRC_HAND;
            result_slot = h;
            result_uid = gDuel_aDeckCardRecords[D_800E9FF0[side].hand[h]].data_block_index;
            return 1;
        }
    }

    if (setting_require_result != RESULT_HAND_OR_DECK) return 0;
    start = deck_begin(side) + D_800E9FF0[side].deck_draw_cursor;
    end = deck_end(side);
    if (start < deck_begin(side)) start = deck_begin(side);
    if (start > end) start = end;
    for (i = start; i < end; i++) {
        if (gDuel_aDeckCardRecords[i].id == result) {
            result_where = RITUAL_SRC_DECK;
            result_slot = i;
            result_uid = gDuel_aDeckCardRecords[i].data_block_index;
            return 1;
        }
    }
    return 0;
}

static void consume_result_source(void)
{
    int deckpos = -1;
    int dest = (int)D_8009B19C;

    if (result_where == RITUAL_SRC_HAND) {
        if (result_slot >= 0 && result_slot < HAND_SIZE)
            deckpos = D_800E9FF0[side].hand[result_slot];
        clear_hand_source(side, result_slot, 0);
    } else if (result_where == RITUAL_SRC_DECK) {
        deckpos = consume_deck_uid(side, result_uid);
    }

    /* The vanilla ritual later rewrites card->data into the Ritual result and
       calls Duel_SetupCardRecord with that deck record's slot. Pointing the
       destination at the required physical copy makes the summon genuinely
       move that copy from Hand/Deck to the field instead of merely checking
       ownership and generating another one. */
    if (valid_record(dest) && deckpos >= deck_begin(side) &&
        deckpos < deck_end(side))
        D_801A7AD8[dest].data = &gDuel_aDeckCardRecords[deckpos];

    reset_result_source();
}

static int disc_has_recipe(int ritual)
{
    int n;
    for (n = 0; n < 256; n++) {
        u16 *q = &gDuel_awRitualData[n * DUEL_RITUAL_RECIPE_HALFWORD_COUNT];
        if (!q[0]) break;
        if (q[0] == ritual) return 1;
    }
    return 0;
}

static int normalize_ritual_id(int ritual)
{
    int effect;
    if (!effect_id) return ritual;
    effect = effect_id(ritual);
    if (effect == ritual) return ritual;
    if (custom_has_ritual && custom_has_ritual(ritual)) return ritual;
    if (disc_has_recipe(ritual)) return ritual;
    return effect;
}

/* Retail ritual records: ritual, material 1, material 2, material 3, result. */
static int load_disc_recipe(int ritual)
{
    int n, i;
    for (n = 0; n < 256; n++) {
        u16 *q = &gDuel_awRitualData[n * DUEL_RITUAL_RECIPE_HALFWORD_COUNT];
        if (!q[0]) break;
        if (q[0] != ritual) continue;
        for (i = 0; i < 3; i++) matid[i] = q[i + 1];
        recipe_result = q[4];
        if (recipe_result <= 0 || recipe_result > CARD_TABLE_COUNT) return 0;
        for (i = 0; i < 3; i++)
            if (matid[i] <= 0 || matid[i] > CARD_TABLE_COUNT) return 0;
        return 1;
    }
    return 0;
}

/* Resolve the recipe exactly as the current runtime tables expose it. */
static int resolve_recipe(int ritual, TablesRitualRequirement requirements[3],
                          int *conditional)
{
    unsigned short result = 0, recipe[6];
    int i, ruled;
    *conditional = 0;
    ritual = normalize_ritual_id(ritual);

    if (custom_requirements && custom_requirements(ritual, requirements, &result)) {
        recipe_result = result;
        *conditional = 1;
        return recipe_result > 0 && recipe_result <= CARD_TABLE_COUNT;
    }

    ruled = custom_recipe ? custom_recipe(ritual, recipe) : -1;
    if (ruled == 0) return 0;
    if (ruled > 0) {
        recipe_result = recipe[4];
        if (recipe_result <= 0 || recipe_result > CARD_TABLE_COUNT) return 0;
        for (i = 0; i < 3; i++) {
            matid[i] = recipe[i + 1];
            if (matid[i] <= 0 || matid[i] > CARD_TABLE_COUNT) return 0;
        }
        return 1;
    }
    return load_disc_recipe(ritual);
}

#include "selection.h"


/* The runtime tables are the fast, readable recipe source. The public
 * Duel_CheckRitual hook chain is the final authority. This keeps Melhoria dos
 * Rituais independent: it does not need to know whether a recipe is Vanilla,
 * edited in a table, or implemented privately by another mod. A visible
 * recipe gives us a cheap proposal; the hook chain validates that proposal
 * and becomes the generic fallback when the proposal is unavailable/wrong. */
static void *candidate_data_pointer(const RitualCandidate *c)
{
    int deckpos;
    if (!c) return 0;
    if ((c->where == RITUAL_SRC_FIELD || c->where == RITUAL_SRC_HAND) &&
        valid_record(c->rec) && D_801A7AD8[c->rec].data)
        return D_801A7AD8[c->rec].data;
    if (c->where == RITUAL_SRC_HAND && c->slot >= 0 && c->slot < HAND_SIZE) {
        deckpos = D_800E9FF0[side].hand[c->slot];
        if (deckpos >= deck_begin(side) && deckpos < deck_end(side))
            return &gDuel_aDeckCardRecords[deckpos];
    }
    if (c->where == RITUAL_SRC_DECK && c->slot >= deck_begin(side) &&
        c->slot < deck_end(side))
        return &gDuel_aDeckCardRecords[c->slot];
    return (void *)c;
}

static int chain_oracle_candidates(int ritual, const RitualCandidate *pick[3])
{
    DuelCardRecord saved[DUEL_FIELD_ROW_SIZE];
    void *data[3];
    int first = side * DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE;
    int i;
    s32 result;

    if (!public_check || !pick[0] || !pick[1] || !pick[2]) return 0;

    /* Capture physical backing before the field row is replaced. This matters
       for hooks that care about the actual card copy, not only its id. */
    for (i = 0; i < 3; i++) data[i] = candidate_data_pointer(pick[i]);

    memcpy(saved, &D_801A7AD8[first], sizeof saved);
    memset(&D_801A7AD8[first], 0, sizeof saved);
    memset(oracle_objects, 0, sizeof oracle_objects);
    for (i = 0; i < 3; i++) {
        DuelCardRecord *r = &D_801A7AD8[first + i];
        r->card_id = (s16)pick[i]->id;
        r->flags = DUEL_CARD_FLAG_OCCUPIED;
        r->data = data[i];
        r->object = &oracle_objects[i];
        oracle_objects[i].field_6A = (u8)(first + i);
    }

    oracle_mode++;
    result = public_check(0, ritual);
    oracle_mode--;
    memcpy(&D_801A7AD8[first], saved, sizeof saved);
    return (int)result;
}

static int chain_oracle_triplet(int ritual, int ia, int ib, int ic)
{
    const RitualCandidate *pick[3] = {
        &candidates[ia], &candidates[ib], &candidates[ic]
    };
    return chain_oracle_candidates(ritual, pick);
}

static int chain_oracle_selected(int ritual)
{
    RitualCandidate chosen[3];
    const RitualCandidate *pick[3];
    int i;
    for (i = 0; i < 3; i++) {
        chosen[i].where = where[i];
        chosen[i].slot = slot[i];
        chosen[i].rec = rec[i];
        chosen[i].id = matid[i];
        chosen[i].uid = source_uid[i];
        pick[i] = &chosen[i];
    }
    return chain_oracle_candidates(ritual, pick);
}

static int combo_source_counts_ok(int a, int b, int c, int *field_count, int *hand_count)
{
    int f = (candidates[a].where == RITUAL_SRC_FIELD) +
            (candidates[b].where == RITUAL_SRC_FIELD) +
            (candidates[c].where == RITUAL_SRC_FIELD);
    int h = (candidates[a].where == RITUAL_SRC_HAND) +
            (candidates[b].where == RITUAL_SRC_HAND) +
            (candidates[c].where == RITUAL_SRC_HAND);
    if (f < setting_min_field) return 0;
    if (setting_use_deck && h < setting_min_hand) return 0;
    if (field_count) *field_count = f;
    if (hand_count) *hand_count = h;
    return 1;
}

/* A strictly conservative acceleration of the private recipe fallback.
 * A producer can describe an active three-role recipe through recipe_v1,
 * including categories not representable in the retail five-halfword row.
 * We use that information ONLY if there are exactly three physical candidates
 * in total. Then there is one possible unordered trio; the ordinary hook
 * chain must still positively confirm its result. Any ambiguous case, missing
 * producer, disabled recipe, changed ABI, or disagreement takes the identical
 * legacy public-hook search. Never let the provider consume materials or
 * override the authority of Duel_CheckRitual.
 *
 * The provider is looked up lazily: this mod may initialize before the
 * producer (its directory sorts before "zzz Efeitos Rituais"). Do not cache
 * negative lookups or the producer's ON/OFF state. */
static int select_unique_provider_trio(int ritual)
{
    RitualRecipeRequirementV1 requirements[3];
    const RitualRecipeProviderV1 *provider = recipe_provider;
    int result, published_ritual, a, b, c, matched = 0, i;

    if (setting_require_result != RESULT_OFF || !public_check ||
        !mod_host || !mod_host->find) return 0;
    if (!provider) {
        provider = (const RitualRecipeProviderV1 *)mod_host->find(
            mod_host, "ritual-card-effects-test:recipe_v1");
        if (provider) recipe_provider = provider;
    }
    if (!provider || provider->abi != RITUAL_RECIPE_PROVIDER_ABI ||
        !provider->get || !provider->matches) return 0;

    result = 0;
    published_ritual = normalize_ritual_id(ritual);
    if (provider->get(published_ritual, requirements, 3,
                      &result) != 3 || result < 1 || result > CARD_TABLE_COUNT)
        return 0;

    reset_result_source();
    collect_candidates();
    if (candidate_count != 3 || !combo_source_counts_ok(0, 1, 2, 0, 0))
        return 0;

    /* A private recipe's roles can be interchangeable; verify all six role
     * allocations without assuming the cards' current field/hand/deck order. */
    for (a = 0; a < 3 && !matched; a++)
        for (b = 0; b < 3 && !matched; b++) {
            if (b == a) continue;
            c = 3 - a - b; /* 0+1+2 == 3, leaving the third distinct index. */
            if (provider->matches(published_ritual, 0, candidates[a].id) &&
                provider->matches(published_ritual, 1, candidates[b].id) &&
                provider->matches(published_ritual, 2, candidates[c].id))
                matched = 1;
        }
    if (!matched) return 0;

    /* The original fallback's first legal trio is exactly (0,1,2) here.
     * It must agree with the provider; other hooks always win. */
    if (chain_oracle_triplet(ritual, 0, 1, 2) != result) return 0;
    recipe_result = result;
    /* As in the generic fallback, finish with a validation against the
     * selected physical cards to leave hook publishers coherent. */
    (void)chain_oracle_triplet(ritual, 0, 1, 2);
    for (i = 0; i < 3; i++) {
        const RitualCandidate *cand = &candidates[i];
        where[i] = cand->where;
        slot[i] = cand->slot;
        rec[i] = cand->rec;
        matid[i] = cand->id;
        source_uid[i] = cand->uid;
        visual_rec[i] = -1;
        preconsumed[i] = 0;
        obj[i] = 0;
    }
    return 1;
}

static void sort_combo_weakest(int order[3], int a, int b, int c, int prefer_defense)
{
    int i, j, t;
    order[0] = a; order[1] = b; order[2] = c;
    for (i = 0; i < 2; i++)
        for (j = i + 1; j < 3; j++)
            if (candidate_stats_better(&candidates[order[j]],
                                       &candidates[order[i]], prefer_defense)) {
                t = order[i]; order[i] = order[j]; order[j] = t;
            }
}

static int chain_combo_better(int a, int b, int c, const int best[3],
                              int field_count, int hand_count,
                              int best_field, int best_hand, int prefer_defense)
{
    int now[3], old[3], i;
    if (best[0] < 0) return 1;
    if (field_count != best_field) return field_count > best_field;
    if (hand_count != best_hand) return hand_count > best_hand;
    sort_combo_weakest(now, a, b, c, prefer_defense);
    sort_combo_weakest(old, best[0], best[1], best[2], prefer_defense);
    for (i = 0; i < 3; i++) {
        int nb = candidate_stats_better(&candidates[now[i]], &candidates[old[i]], prefer_defense);
        int ob = candidate_stats_better(&candidates[old[i]], &candidates[now[i]], prefer_defense);
        if (nb != ob) return nb;
    }
    return 0;
}

/* Generic fallback through the public Ritual checker. It makes no distinction
 * between Vanilla and edited recipes: the first source-legal trio accepted by
 * the live hook chain tells us the current result, then all legal physical
 * trios are ranked with the same Field > Hand > Deck policy. */
static int select_candidates_via_chain(int ritual)
{
    int best[3] = {-1, -1, -1};
    int result = 0, result_stats, prefer_defense;
    int a, b, c, f, h, got, i;
    int target_field, target_hand;

    if (setting_min_field + (setting_use_deck ? setting_min_hand : 0) > 3)
        return 0;

    reset_result_source();
    collect_candidates();
    if (candidate_count < 3) return 0;

    /* Discover the effective result exactly as before: the first source-legal
       physical trio accepted by the public hook chain defines this Ritual's
       result. Keeping this order preserves private-hook behaviour. */
    for (a = 0; a < candidate_count && !result; a++)
        for (b = a + 1; b < candidate_count && !result; b++)
            for (c = b + 1; c < candidate_count; c++) {
                if (!combo_source_counts_ok(a, b, c, 0, 0)) continue;
                got = chain_oracle_triplet(ritual, a, b, c);
                if (got > 0 && got <= CARD_TABLE_COUNT) {
                    result = got;
                    break;
                }
            }
    if (!result) return 0;

    recipe_result = result;
    if (!reserve_result_source(result)) return 0;

    /* Reserving the physical Ritual Monster may remove one candidate. */
    if (setting_require_result != RESULT_OFF) {
        collect_candidates();
        if (candidate_count < 3) return 0;
    }

    result_stats = gDuel_adwCardStats[result - 1];
    prefer_defense =
        ((result_stats >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) >
        (result_stats & CARD_STAT_VALUE_MASK);

    /* Source counts dominate every other tie-break. Search those tiers in the
       exact priority order (Field, then Hand, then Deck) and stop after the
       first tier that contains an accepted trio. Lower tiers can never win. */
    for (target_field = 3; target_field >= setting_min_field; target_field--) {
        int min_hand = setting_use_deck ? setting_min_hand : 0;
        for (target_hand = 3 - target_field; target_hand >= min_hand; target_hand--) {
            best[0] = best[1] = best[2] = -1;
            for (a = 0; a < candidate_count; a++)
                for (b = a + 1; b < candidate_count; b++) {
                    int field_ab = (candidates[a].where == RITUAL_SRC_FIELD) +
                                   (candidates[b].where == RITUAL_SRC_FIELD);
                    int hand_ab = (candidates[a].where == RITUAL_SRC_HAND) +
                                  (candidates[b].where == RITUAL_SRC_HAND);
                    int last_where;
                    if (field_ab > target_field || hand_ab > target_hand ||
                        target_field - field_ab > 1 || target_hand - hand_ab > 1)
                        continue;
                    last_where = target_field > field_ab ? RITUAL_SRC_FIELD :
                        target_hand > hand_ab ? RITUAL_SRC_HAND : RITUAL_SRC_DECK;
                    for (c = b + 1; c < candidate_count; c++) {
                        if (candidates[c].where != last_where) continue;
                        /* These equations imply the original three-source
                         * count check (3 cards; Field+Hand+Deck=3). */
                        f = target_field;
                        h = target_hand;
                        got = chain_oracle_triplet(ritual, a, b, c);
                        if (got != result) continue;
                        if (chain_combo_better(a, b, c, best, f, h,
                                               target_field, target_hand,
                                               prefer_defense)) {
                            best[0] = a; best[1] = b; best[2] = c;
                        }
                    }
                }
            if (best[0] >= 0) goto selected;
        }
    }
    return 0;

selected:
    /* Leave downstream recipe publishers coherent with the chosen trio. */
    (void)chain_oracle_triplet(ritual, best[0], best[1], best[2]);

    for (i = 0; i < 3; i++) {
        const RitualCandidate *cand = &candidates[best[i]];
        where[i] = cand->where;
        slot[i] = cand->slot;
        rec[i] = cand->rec;
        matid[i] = cand->id;
        source_uid[i] = cand->uid;
        visual_rec[i] = -1;
        preconsumed[i] = 0;
        obj[i] = 0;
    }
    return 1;
}

static int deck_setup_index_for_source(int source_index)
{
    if (where[source_index] == RITUAL_SRC_HAND)
        return deck_position_for_uid(side, source_uid[source_index]);
    if (where[source_index] == RITUAL_SRC_DECK)
        return remaining_deck_position_for_uid(side, source_uid[source_index]);
    return -1;
}

/* Give each selected Deck tribute a distinct entrance trajectory. Previously
 * all deck sources spawned at exactly the same x/z, so two deck materials
 * looked like one card (Heishin: Field + Deck + Deck). Keep the single-Deck
 * trajectory unchanged and keep every source on its original Deck side.
 * This affects visuals only, never where the materials are selected/spent. */
static void deck_visual_origin(int source_index, int *x, int *z)
{
    int i, order = 0, count = 0, delta;
    for (i = 0; i < 3; i++) {
        if (where[i] != RITUAL_SRC_DECK) continue;
        if (i < source_index) order++;
        count++;
    }
    /* Each extra Deck source fans 64 world units away from its neighbor. */
    delta = (2 * order - (count - 1)) * 32;
    *x = (side ? -DECK_VISUAL_X : DECK_VISUAL_X) + delta;
    *z = (side ? DECK_VISUAL_Z : -DECK_VISUAL_Z) + delta / 2;
}

/* Build a field-style card object for a non-field Ritual source. Both Hand
 * and Deck must use the same world-coordinate object type as Vanilla's field
 * tributes because effect_22 moves those coordinates linearly to (0,-96,0).
 * The source itself stays in Hand/Deck until the normal tribute cleanup. */
static void *make_nonfield_visual(int source_index, int target_record, int deckpos)
{
    int x, z;
    int hand_source;
    DuelCardDisplayObject *visual;
    DuelCardRecord *record;

    if (source_index < 0 || source_index >= 3 || !valid_record(target_record) ||
        deckpos < 0) return 0;
    hand_source = where[source_index] == RITUAL_SRC_HAND;
    if (!hand_source && where[source_index] != RITUAL_SRC_DECK) return 0;

    Duel_SetupCardRecord(target_record, deckpos);
    record = &D_801A7AD8[target_record];

    if (hand_source) {
        /* A Hand tribute enters vertically from the player's edge. Giving all
           Hand cards x=0 makes effect_22's x velocity zero, so the flight is
           a straight bottom-to-centre line. Use the game's own face-down card
           state so the player sees the card back, not hidden hand information. */
        x = HAND_VISUAL_X;
        z = side ? HAND_VISUAL_Z : -HAND_VISUAL_Z;
        record->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
        record->flags |= DUEL_CARD_FLAG_FACE_DOWN;
    } else {
        /* Keep the approved Deck-side entrance. The card is staged face-up,
           while the stock field/Ritual lighting is deliberately left untouched
           so its darker look stays consistent with the Vanilla presentation. */
        deck_visual_origin(source_index, &x, &z);
        record->flags &= ~(DUEL_CARD_FLAG_FACE_DOWN |
                           DUEL_CARD_FLAG_DEFENSE_POSITION);
    }

    visual = func_80024C1C(record->card_id, x, z);
    if (!visual) {
        record->flags = 0;
        return 0;
    }
    visual->card_index = (u8)target_record;
    /* func_80024C1C allocates a fresh object, but make the intended pose
       explicit before applying the standard field-card state. */
    visual->field_21 = 0;
    visual->field_22 = 0;
    record->object = visual;
    func_80017E3C(visual);
    visual_rec[source_index] = target_record;
    return visual;
}

static void *make_nonfield_scratch_visual(int source_index)
{
    int scratch = find_scratch_record();
    int deckpos = deck_setup_index_for_source(source_index);
    if (scratch < 0 || deckpos < 0) return 0;
    return make_nonfield_visual(source_index, scratch, deckpos);
}

/* With no field tribute, the retail ritual controller still needs a monster
 * record to reuse for the result. Materialize one selected Hand/Deck source
 * there, but keep its entrance pose determined only by its real source. */
static int prepare_synthetic_anchor(int *anchor_index)
{
    int dest = find_empty_monster_record(side);
    int i, chosen = -1, deckpos;
    DuelCardDisplayObject *visual;

    if (dest < 0) return 0;
    for (i = 0; i < 3; i++)
        if (where[i] == RITUAL_SRC_HAND) { chosen = i; break; }
    if (chosen < 0)
        for (i = 0; i < 3; i++)
            if (where[i] == RITUAL_SRC_DECK) { chosen = i; break; }
    if (chosen < 0) return 0;

    if (where[chosen] == RITUAL_SRC_HAND) {
        deckpos = deck_position_for_uid(side, source_uid[chosen]);
        if (deckpos < 0) return 0;
    } else {
        /* The anchor's deck slot must stop moving before Vanilla stores its
           field record as the future Ritual result destination. */
        deckpos = consume_deck_uid(side, source_uid[chosen]);
        if (deckpos < 0) return 0;
        preconsumed[chosen] = 1;
    }

    visual = make_nonfield_visual(chosen, dest, deckpos);
    if (!visual) return 0;
    synthetic_anchor_rec = dest;
    obj[chosen] = visual;
    *anchor_index = chosen;
    return 1;
}

/* 3D Monsters renders every occupied, face-up monster-zone record each
 * frame. During a mixed/deck Ritual those records are only staging/tributes,
 * so keeping them face-up makes their full monster models stand inside the
 * Ritual animation. Mark just the selected monster-row records face-down
 * after their 2D card objects are prepared. The Ritual effect draws from the
 * display objects themselves, while the result setup later replaces the
 * record and clears this flag. */
static void suppress_tribute_3d_models(void)
{
    int i;
    for (i = 0; i < 3; i++) {
        int r = visual_rec[i];
        int local;
        if (!valid_record(r)) continue;
        local = r % DUEL_CARD_SIDE_RECORD_COUNT;
        if (local >= DUEL_FIELD_ROW_SIZE && local < DUEL_FIELD_ROW_SIZE * 2)
            D_801A7AD8[r].flags |= DUEL_CARD_FLAG_FACE_DOWN;
    }
}

static int prepare_visuals(DuelRitualResult *out)
{
    int i, anchor = -1, others[2], n = 0;
    void *anchor_obj;

    synthetic_anchor_rec = -1;
    for (i = 0; i < 3; i++) {
        obj[i] = 0;
        visual_rec[i] = -1;
        preconsumed[i] = 0;
    }

    /* First choice: a real field tribute, exactly as retail. */
    for (i = 0; i < 3; i++) {
        if (where[i] == RITUAL_SRC_FIELD && valid_record(rec[i]) &&
            D_801A7AD8[rec[i]].object) {
            anchor = i;
            obj[i] = D_801A7AD8[rec[i]].object;
            visual_rec[i] = rec[i];
            break;
        }
    }
    if (anchor < 0 && !prepare_synthetic_anchor(&anchor)) return 0;
    anchor_obj = obj[anchor];
    if (!anchor_obj) return 0;

    for (i = 0; i < 3; i++) {
        if (i == anchor) continue;
        if (where[i] == RITUAL_SRC_FIELD) {
            obj[i] = valid_record(rec[i]) ? D_801A7AD8[rec[i]].object : 0;
            visual_rec[i] = rec[i];
        } else if (where[i] == RITUAL_SRC_HAND ||
                   where[i] == RITUAL_SRC_DECK) {
            obj[i] = make_nonfield_scratch_visual(i);
        }
        /* If no independent Deck visual record is free, retain the old safe
           proxy fallback for that Deck-only visual. Hand tributes must keep
           their own bottom-entry/card-back presentation. */
        if (!obj[i]) {
            if (where[i] != RITUAL_SRC_DECK) return 0;
            obj[i] = anchor_obj;
            visual_rec[i] = -1;
        }
        others[n++] = i;
    }
    if (n != 2) return 0;

    /* Slot 1 is special: vanilla saves its field_6A as the destination for
       the summoned monster. Keep the anchor there regardless of recipe order. */
    out->tribute_objects[0] = obj[others[0]];
    out->tribute_objects[1] = anchor_obj;
    out->tribute_objects[2] = obj[others[1]];
    out->field_0C = 0;
    consume_order[0] = others[0];
    consume_order[1] = anchor;
    consume_order[2] = others[1];
    suppress_tribute_3d_models();
    return 1;
}

static void consume_selected_source(int i)
{
    if (i < 0 || i >= 3) return;
    if (where[i] == RITUAL_SRC_FIELD) {
        if (valid_record(rec[i])) o_deact(&D_801A7AD8[rec[i]]);
        return;
    }

    if (where[i] == RITUAL_SRC_HAND) {
        /* The animation uses an independent field-style visual; always release
           that staging record, then consume the real hand slot separately. */
        if (visual_rec[i] >= 0) o_deact(&D_801A7AD8[visual_rec[i]]);
        clear_hand_source(side, slot[i], 0);
        return;
    }

    if (where[i] == RITUAL_SRC_DECK) {
        if (visual_rec[i] >= 0 && visual_rec[i] != synthetic_anchor_rec)
            o_deact(&D_801A7AD8[visual_rec[i]]);
        else if (visual_rec[i] == synthetic_anchor_rec && synthetic_anchor_rec >= 0)
            o_deact(&D_801A7AD8[synthetic_anchor_rec]);
        if (!preconsumed[i]) (void)consume_deck_uid(side, source_uid[i]);
    }
}

static void cache_ritual_output(const DuelRitualResult *out, int result)
{
    int i;
    active_result = result;
    for (i = 0; i < 3; i++) cached_tribute_objects[i] = out->tribute_objects[i];
}

static void copy_cached_ritual_output(DuelRitualResult *out)
{
    int i;
    if (!out) return;
    for (i = 0; i < 3; i++) out->tribute_objects[i] = cached_tribute_objects[i];
    out->field_0C = 0;
}

static void finish_bridge(void)
{
    int i;
    if (setting_require_result != RESULT_OFF) consume_result_source();
    bridge_mode = BRIDGE_NONE;
    deact_step = 0;
    active_result = 0;
    for (i = 0; i < 3; i++) cached_tribute_objects[i] = 0;
    synthetic_anchor_rec = -1;
}

/* One recipe pipeline for every Ritual. A readable runtime recipe is only a
 * fast proposal; the public hook chain validates it. If it cannot be read or
 * the live chain disagrees, the generic oracle searches Field/Hand/Deck.
 * Nothing here asks which mod supplied the rule or whether it is Vanilla. */
static int select_effective_recipe(int ritual_id,
                                   TablesRitualRequirement requirements[3])
{
    int conditional = 0;
    int got;

    if (resolve_recipe(ritual_id, requirements, &conditional)) {
        if (reserve_result_source(recipe_result) &&
            select_candidates(requirements, conditional)) {
            if (!public_check) return 1;
            got = chain_oracle_selected(ritual_id);
            if (got == recipe_result) return 1;
        }
        reset_result_source();
    }

    /* The read-only recipe_v1 provider may avoid the generic triple scan
     * only for a unique, positively verified physical trio. All other
     * decisions use the exact pre-existing public-hook fallback. */
    if (select_unique_provider_trio(ritual_id)) return 1;
    return select_candidates_via_chain(ritual_id);
}

static s32 ritual_check(DuelRitualResult *out, s32 ritual_id)
{
    s32 retail;
    TablesRitualRequirement requirements[3];
    int i, field_count = 0;

    if (oracle_mode) return o_check(out, ritual_id);

    /* Effect 22 asks Duel_CheckRitual a second time to obtain the three card
       pointers it animates. Once ApplyRitual has prepared a mixed/deck
       summon, replay that exact prepared output instead of selecting again
       (which could otherwise consume a Deck anchor twice). */
    if (bridge_mode != BRIDGE_NONE && active_result != 0) {
        copy_cached_ritual_output(out);
        return active_result;
    }

    read_ritual_settings();
    side = D_8009B1D5 & 1;
    reset_result_source();

    /* Let every hook below us run first. If another independent mod publishes
       a concrete recipe row while checking the current field, resolve_recipe
       below sees that post-hook state. */
    retail = o_check(out, ritual_id);

    /* A retail-valid three-field ritual already satisfies every possible
       min-field value. Keep the game's exact path unless a configured hand
       minimum forces a mixed search. Result-card ownership is layered on it. */
    if (retail != 0 && !(setting_use_deck && setting_min_hand > 0)) {
        if (!reserve_result_source(retail)) {
                return 0;
        }
        if (out && setting_require_result != RESULT_OFF) {
            bridge_mode = BRIDGE_RETAIL_RESULT;
            deact_step = 0;
            cache_ritual_output(out, retail);
        }
        return retail;
    }

    if (!select_effective_recipe(ritual_id, requirements)) return 0;

    for (i = 0; i < 3; i++)
        if (where[i] == RITUAL_SRC_FIELD) field_count++;

    /* No field tribute means retail has no record to recycle for the result. */
    if (field_count == 0 && find_empty_monster_record(side) < 0) {
        return 0;
    }

    /* StartRitual probes without an output structure. Never mutate hand/deck
       or allocate temporary visuals during that eligibility query. */
    if (!out) {
        return recipe_result;
    }
    if (!prepare_visuals(out)) {
        return 0;
    }

    bridge_mode = BRIDGE_EXTENDED;
    deact_step = 0;
    cache_ritual_output(out, recipe_result);
    return recipe_result;
}

/* The PC ritual effect (effect_22.c) reads Duel_RitualFieldObjects after
 * Duel_CheckRitual. A mixed Field/Hand/Deck ritual uses its own three visual
 * objects, but the game's native match remembers only its own field cards.
 * If we let the native query win, it can animate a bystander (and leave its
 * 2D display object at the portal), while never flying the two Deck cards.
 * Return the exact already-prepared objects during THIS extended summon.
 * Retail three-field rituals and unrelated callers are left untouched. */
static s32 ritual_field_objects(void *G32 *objects)
{
    int i;
    if (bridge_mode == BRIDGE_EXTENDED && active_result != 0 && objects) {
        for (i = 0; i < 3; i++)
            if (!cached_tribute_objects[i])
                return o_field_objects(objects);
        for (i = 0; i < 3; i++)
            objects[i] = cached_tribute_objects[i];
        return 3;
    }
    return o_field_objects(objects);
}

static void ritual_deactivate(DuelCardRecord *passed_record)
{
    if (bridge_mode == BRIDGE_NONE || deact_step >= 3) {
        o_deact(passed_record);
        return;
    }

    if (bridge_mode == BRIDGE_RETAIL_RESULT) {
        o_deact(passed_record);
        deact_step++;
        if (deact_step == 3) finish_bridge();
        return;
    }

    consume_selected_source(consume_order[deact_step]);
    deact_step++;
    if (deact_step == 3) finish_bridge();
}

/* Whether id is currently the result of any active ritual recipe. This uses
 * the same runtime tables as ritual execution, so recipe edits are respected. */
static int is_ritual_result_card(int id)
{
    int n, ritual, ruled;
    u16 *q;
    unsigned short recipe[6], result;
    TablesRitualRequirement requirements[3];

    if (id <= 0 || id > CARD_TABLE_COUNT) return 0;

    for (n = 0; n < 256; n++) {
        q = &gDuel_awRitualData[n * DUEL_RITUAL_RECIPE_HALFWORD_COUNT];
        ritual = q[0];
        if (!ritual) break;
        result = 0;
        if (custom_requirements && custom_requirements(ritual, requirements, &result)) {
            if (result == id) return 1;
            continue;
        }
        ruled = custom_recipe ? custom_recipe(ritual, recipe) : -1;
        if (ruled == 0) continue;
        if ((ruled > 0 ? recipe[4] : q[4]) == id) return 1;
    }

    /* Also catch recipes added to cards that have no retail ritual entry. */
    for (ritual = 1; ritual <= CARD_COUNT_LIVE; ritual++) {
        result = 0;
        if (custom_requirements && custom_requirements(ritual, requirements, &result)) {
            if (result == id) return 1;
            continue;
        }
        if (custom_recipe) {
            ruled = custom_recipe(ritual, recipe);
            if (ruled > 0 && recipe[4] == id) return 1;
        }
    }
    return 0;
}

#define RITUAL_RESULT_BIT_BYTES ((CARD_TABLE_COUNT + 8) / 8)
static u8 ritual_result_bits[RITUAL_RESULT_BIT_BYTES];

static void mark_ritual_result(int id)
{
    if (id > 0 && id <= CARD_TABLE_COUNT)
        ritual_result_bits[id >> 3] |= (u8)(1u << (id & 7));
}

/* The bottom-deck rule classifies up to forty cards at once. Build the active
 * result set once instead of rescanning every ritual for every deck slot. */
static void build_ritual_result_bits(void)
{
    int n, ritual, ruled;
    u16 *q;
    unsigned short recipe[6], result;
    TablesRitualRequirement requirements[3];

    memset(ritual_result_bits, 0, sizeof ritual_result_bits);

    for (n = 0; n < 256; n++) {
        q = &gDuel_awRitualData[n * DUEL_RITUAL_RECIPE_HALFWORD_COUNT];
        ritual = q[0];
        if (!ritual) break;
        result = 0;
        if (custom_requirements &&
            custom_requirements(ritual, requirements, &result)) {
            mark_ritual_result(result);
            continue;
        }
        ruled = custom_recipe ? custom_recipe(ritual, recipe) : -1;
        if (ruled == 0) continue;
        mark_ritual_result(ruled > 0 ? recipe[4] : q[4]);
    }

    /* Added ritual cards have no row in the retail table. Only live card ids
       can ever be played, so do not probe the unused 0x7FFF-sized id space. */
    for (ritual = 1; ritual <= CARD_COUNT_LIVE; ritual++) {
        result = 0;
        if (custom_requirements &&
            custom_requirements(ritual, requirements, &result)) {
            mark_ritual_result(result);
            continue;
        }
        if (custom_recipe) {
            ruled = custom_recipe(ritual, recipe);
            if (ruled > 0) mark_ritual_result(recipe[4]);
        }
    }
}

static int ritual_result_marked(int id)
{
    return id > 0 && id <= CARD_TABLE_COUNT &&
        (ritual_result_bits[id >> 3] & (u8)(1u << (id & 7))) != 0;
}

/* When Ritual Monsters are player-Ritual-only and their required physical
 * copy may come from Hand or Deck, keep those result cards at the very bottom
 * of each human player's shuffled deck before the opening hand is drawn.
 * This preserves every non-Ritual card's relative order and every Ritual
 * result's relative order. Only id + data_block_index move, exactly like the
 * game's own deck rearrangement: deck_index/flags remain slot state. */
static int bottom_ritual_results_for_side(int s)
{
    s16 ids[DECK_SIZE];
    u8 data[DECK_SIZE];
    u8 result_flags[DECK_SIZE];
    int i, write = 0, moved = 0;
    int base = deck_begin(s);

    if (s < 0 || s >= DUEL_SIDE_COUNT || D_8009B360[s] >= 0) return 0;

    for (i = 0; i < DECK_SIZE; i++) {
        ids[i] = gDuel_aDeckCardRecords[base + i].id;
        data[i] = gDuel_aDeckCardRecords[base + i].data_block_index;
        result_flags[i] = ritual_result_marked(ids[i]);
        if (result_flags[i]) moved++;
    }
    if (!moved) return 0;

    for (i = 0; i < DECK_SIZE; i++)
        if (!result_flags[i]) {
            gDuel_aDeckCardRecords[base + write].id = ids[i];
            gDuel_aDeckCardRecords[base + write].data_block_index = data[i];
            write++;
        }
    for (i = 0; i < DECK_SIZE; i++)
        if (result_flags[i]) {
            gDuel_aDeckCardRecords[base + write].id = ids[i];
            gDuel_aDeckCardRecords[base + write].data_block_index = data[i];
            write++;
        }
    return moved;
}

static void populate_hook(void)
{
    int s;
    int ritual_only;
    int require_result;

    o_populate();
    ritual_only = !!mod_host->setting(mod_host, "ritual_only_player", 0);
    require_result = clamp_setting(
        mod_host->setting(mod_host, "require_result", 0), 0, 2);
    if (!ritual_only || require_result != RESULT_HAND_OR_DECK) return;

    build_ritual_result_bits();
    for (s = 0; s < DUEL_SIDE_COUNT; s++)
        (void)bottom_ritual_results_for_side(s);
}

/* Player-only rule. Suppress just the single-card confirm press; the original
 * hand controller then remains in place, so no card or turn is consumed.
 * Multi-card/fusion selection is deliberately untouched, and CPU paths never
 * enter this guard. */
static void hand_actions_hook(void)
{
    u16 pressed = gInput_wPad1Pressed;
    int s = D_8009B1D5 & 1;

    /* Hot-path first: in almost every HandActions frame there is no confirm
       to police. Do not cross the Settings API, scan recipes, or inspect the
       selected card until a human actually tries to place one. */
    if (!(pressed & PAD_BUTTON_CONFIRM_MASK) ||
        (D_8009B174 & 0xF) != 1 || D_8009B360[s] >= 0 ||
        !mod_host->setting(mod_host, "ritual_only_player", 0)) {
        o_hand_actions();
        return;
    }

    {
        DuelSelectionRecord *selection =
            (DuelSelectionRecord *)(D_800E9F10 + s * DUEL_SELECTION_SIDE_SIZE);
        int h = (s8)selection->field_0E;
        if (selection->field_15 == 0 && h >= 0 && h < HAND_SIZE) {
            DisplayObject *visual = (DisplayObject *)D_800EA030[h].object;
            int r = visual ? visual->field_6A : hand_record(s, h);
            if (valid_record(r) &&
                (D_801A7AD8[r].flags & DUEL_CARD_FLAG_OCCUPIED) &&
                is_ritual_result_card(D_801A7AD8[r].card_id))
                gInput_wPad1Pressed = pressed & ~PAD_BUTTON_CONFIRM_MASK;
        }
    }

    o_hand_actions();
    gInput_wPad1Pressed = pressed;
}

static void reset_bridge(void)
{
    int i;
    Surrender_Reset();
    bridge_mode = BRIDGE_NONE;
    deact_step = 0;
    active_result = 0;
    for (i = 0; i < 3; i++) cached_tribute_objects[i] = 0;
    synthetic_anchor_rec = -1;
    reset_result_source();
}

static void applied(int on)
{
    (void)on;
    reset_bridge();
}

int MemoriesModInit(const MemoriesModHost *host, MemoriesMod *mod)
{
    if (host->api < 4) return 0;
    mod_host = host;
    recipe_provider = 0;
    public_check = (void *)host->symbol(host, "Duel_CheckRitual");

    custom_requirements = (void *)host->symbol(host, "Tables_RitualRequirements");
    custom_recipe = (void *)host->symbol(host, "Tables_Ritual");
    custom_has_ritual = (void *)host->symbol(host, "Tables_HasRitual");
    effect_id = (void *)host->symbol(host, "Cards_EffectId");
    base_id = (void *)host->symbol(host, "Cards_BaseId");
    card_type = (void *)host->symbol(host, "Cards_Type");
    card_level = (void *)host->symbol(host, "Cards_Level");
    in_group = (void *)host->symbol(host, "Cards_InFusionGroup");

    mod->api = 4;
    mod->reset = reset_bridge;
    mod->applied = applied;

    if (!host->hook(host, (void *)Duel_CheckRitual,
                    (void *)ritual_check, (void **)&o_check))
        return 0;
    if (!host->hook(host, (void *)Duel_RitualFieldObjects,
                    (void *)ritual_field_objects, (void **)&o_field_objects))
        return 0;
    if (!host->hook(host, (void *)DuelCard_DeactivateRecord,
                    (void *)ritual_deactivate, (void **)&o_deact))
        return 0;
    if (!host->hook(host, (void *)DuelScene_UpdateHandActions,
                    (void *)hand_actions_hook, (void **)&o_hand_actions))
        return 0;
    if (!host->hook(host, (void *)Duel_PopulateCombinedDeckData,
                    (void *)populate_hook, (void **)&o_populate))
        return 0;
    /* Publish only after every hook has been installed successfully. */
    if (!host->provide(host, "rules_v1", (void *)&published_rules)) return 0;
    if (!Surrender_Init(host)) return 0;

    return 1;
}
