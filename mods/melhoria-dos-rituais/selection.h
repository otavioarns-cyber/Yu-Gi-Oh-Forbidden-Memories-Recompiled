/* Included by rules.c after bridge state and source helpers. */

typedef struct RitualCandidate {
    int where;      /* RITUAL_SRC_* */
    int slot;       /* field slot, hand slot, or deck array position */
    int rec;        /* live DuelCardRecord for field/hand, else -1 */
    int id;
    int uid;        /* stable physical copy: data_block_index, else -1 */
    int attack;     /* printed stats, decoded once when the snapshot is built */
    int defense;
    int base;       /* Cards_BaseId(id), or 0 when unavailable */
    int type;       /* Cards_Type(id), or -1 when unavailable */
    int level;      /* Cards_Level(id), or -1 when unavailable */
} RitualCandidate;

#define RITUAL_CANDIDATE_CAPACITY (DUEL_FIELD_ROW_SIZE + HAND_SIZE + DECK_SIZE)
static int candidate_count;
static RitualCandidate candidates[RITUAL_CANDIDATE_CAPACITY];

static int candidate_reserved_for_result(const RitualCandidate *c)
{
    if (!c || !result_where) return 0;
    if (result_where == RITUAL_SRC_HAND)
        return c->where == RITUAL_SRC_HAND && c->slot == result_slot;
    if (result_where == RITUAL_SRC_DECK)
        return c->where == RITUAL_SRC_DECK && c->uid == result_uid;
    return 0;
}

static int candidate_matches_requirement(const RitualCandidate *c,
                                         const TablesRitualRequirement *q)
{
    if (!c || c->id <= 0 || c->id > CARD_TABLE_COUNT) return 0;
    if (c->type >= CARD_TYPE_MAGIC) return 0;
    if (q->card && c->id != q->card && c->base != q->card) return 0;
    if (q->type >= 0 && c->type != q->type) return 0;
    if (q->fusion_group && (!in_group || !in_group(c->id, q->fusion_group))) return 0;
    if (c->attack < q->min_attack || c->defense < q->min_defense) return 0;
    if (q->max_attack >= 0 && c->attack > q->max_attack) return 0;
    if (q->max_defense >= 0 && c->defense > q->max_defense) return 0;
    if ((q->min_level >= 0 || q->max_level >= 0) && c->level < 0) return 0;
    if (q->min_level >= 0 && c->level < q->min_level) return 0;
    if (q->max_level >= 0 && c->level > q->max_level) return 0;
    return !q->defense_gt_attack || c->defense > c->attack;
}

static int candidate_matches_fixed(const RitualCandidate *c, int wanted)
{
    return c && (c->id == wanted || c->base == wanted);
}

static int specificity(const TablesRitualRequirement *q)
{
    return (q->card ? 100 : 0) + (q->type >= 0) + !!q->fusion_group +
        !!q->min_attack + !!q->min_defense + (q->max_attack >= 0) +
        (q->max_defense >= 0) + (q->min_level >= 0) + (q->max_level >= 0) +
        !!q->defense_gt_attack;
}

static int source_rank(const RitualCandidate *c)
{
    /* Stable tie-break only. Aggregate source counts are compared first. */
    if (c->where == RITUAL_SRC_FIELD) return c->slot;
    if (c->where == RITUAL_SRC_HAND) return 16 + c->slot;
    return 32 + c->slot;
}

static void add_candidate(int where_value, int source_slot, int record,
                          int id, int uid_value)
{
    RitualCandidate c;
    int stats;

    if (candidate_count >= RITUAL_CANDIDATE_CAPACITY ||
        id <= 0 || id > CARD_TABLE_COUNT) return;

    c.where = where_value;
    c.slot = source_slot;
    c.rec = record;
    c.id = id;
    c.uid = uid_value;
    stats = gDuel_adwCardStats[id - 1];
    c.attack = (stats & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
    c.defense = ((stats >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
    c.base = base_id ? base_id(id) : 0;
    c.type = card_type ? card_type(id) : -1;
    c.level = card_level ? card_level(id) : -1;

    if (candidate_reserved_for_result(&c)) return;
    candidates[candidate_count++] = c;
}

/* One short-lived snapshot per Ritual decision. Expensive card metadata is
 * decoded here once, never again inside the combination loops. */
static void collect_candidates(void)
{
    int i, r, start, end;
    int field_base = side * DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE;
    int deck_base = side * DECK_SIZE;
    DuelSideState *state = &D_800E9FF0[side];

    candidate_count = 0;

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        r = field_base + i;
        if ((D_801A7AD8[r].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            D_801A7AD8[r].object && D_801A7AD8[r].card_id > 0)
            add_candidate(RITUAL_SRC_FIELD, i, r, D_801A7AD8[r].card_id, -1);
    }

    for (i = 0; i < HAND_SIZE; i++) {
        r = hand_record(side, i);
        if (state->hand[i] >= 0 && valid_record(r) &&
            (D_801A7AD8[r].flags & DUEL_CARD_FLAG_OCCUPIED) &&
            D_801A7AD8[r].card_id > 0)
            add_candidate(RITUAL_SRC_HAND, i, r, D_801A7AD8[r].card_id,
                          gDuel_aDeckCardRecords[state->hand[i]].data_block_index);
    }

    if (!setting_use_deck) return;
    start = deck_base + state->deck_draw_cursor;
    end = deck_base + DECK_SIZE;
    if (start < deck_base) start = deck_base;
    if (start > end) start = end;
    for (i = start; i < end; i++) {
        int id = gDuel_aDeckCardRecords[i].id;
        if (id > 0)
            add_candidate(RITUAL_SRC_DECK, i, -1, id,
                          gDuel_aDeckCardRecords[i].data_block_index);
    }
}

static int candidate_stats_better(const RitualCandidate *a,
                                  const RitualCandidate *b,
                                  int prefer_defense)
{
    if (!b) return 1;
    if (prefer_defense) {
        if (a->defense != b->defense) return a->defense < b->defense;
        if (a->attack != b->attack) return a->attack < b->attack;
    } else {
        if (a->attack != b->attack) return a->attack < b->attack;
        if (a->defense != b->defense) return a->defense < b->defense;
    }
    return source_rank(a) < source_rank(b);
}

/* Select three distinct candidates. Source priority is lexicographic:
 * Field count first, then Hand count, then Deck by remainder. Therefore once
 * a source-count tier has any valid assignment, lower tiers can never win and
 * are skipped entirely. Match masks keep card/rule checks out of the hot
 * permutation loop. */
static int select_candidates(const TablesRitualRequirement requirements[3],
                             int conditional)
{
    int best[3] = {-1, -1, -1};
    int order[3] = {0, 1, 2};
    /* Each requirement scans only its eligible candidate indices. Because
     * collect_candidates preserves Field/Hand/Deck order and each list is
     * filled in ascending index order, tie-breaking is byte-for-byte stable.
     * A candidate index fits in u8 (capacity is 50). */
    u8 eligible[3][RITUAL_CANDIDATE_CAPACITY];
    int eligible_count[3] = {0, 0, 0};
    int available_by_source[4] = {0, 0, 0, 0};
    int prefer_defense;
    int result_stats;
    int a, b, c, i, j, t;
    int ia, ib, ic;
    int target_field, target_hand;

    if (setting_min_field + (setting_use_deck ? setting_min_hand : 0) > 3)
        return 0;

    if (conditional) {
        for (i = 0; i < 3; i++)
            for (j = i + 1; j < 3; j++)
                if (specificity(&requirements[order[j]]) >
                    specificity(&requirements[order[i]])) {
                    t = order[i]; order[i] = order[j]; order[j] = t;
                }
    }

    collect_candidates();
    if (candidate_count < 3) return 0;

    for (i = 0; i < candidate_count; i++) {
        available_by_source[candidates[i].where]++;
        for (j = 0; j < 3; j++) {
            int match = conditional ?
                candidate_matches_requirement(&candidates[i], &requirements[j]) :
                candidate_matches_fixed(&candidates[i], matid[j]);
            if (match) eligible[j][eligible_count[j]++] = (u8)i;
        }
    }
    /* No possible assignment when even one material slot has no match. */
    if (!eligible_count[0] || !eligible_count[1] || !eligible_count[2]) return 0;

    result_stats = gDuel_adwCardStats[recipe_result - 1];
    prefer_defense =
        ((result_stats >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) >
        (result_stats & CARD_STAT_VALUE_MASK);

    for (target_field = 3; target_field >= setting_min_field; target_field--) {
        int min_hand = setting_use_deck ? setting_min_hand : 0;
        if (available_by_source[RITUAL_SRC_FIELD] < target_field) continue;
        for (target_hand = 3 - target_field; target_hand >= min_hand; target_hand--) {
            if (available_by_source[RITUAL_SRC_HAND] < target_hand ||
                available_by_source[RITUAL_SRC_DECK] < 3 - target_field - target_hand)
                continue;
            best[0] = best[1] = best[2] = -1;

            for (ia = 0; ia < eligible_count[0]; ia++) {
                a = eligible[0][ia];
                for (ib = 0; ib < eligible_count[1]; ib++) {
                    int field_ab, hand_ab;
                    b = eligible[1][ib];
                    if (b == a) continue;
                    field_ab = (candidates[a].where == RITUAL_SRC_FIELD) +
                               (candidates[b].where == RITUAL_SRC_FIELD);
                    hand_ab = (candidates[a].where == RITUAL_SRC_HAND) +
                              (candidates[b].where == RITUAL_SRC_HAND);
                    if (field_ab > target_field || hand_ab > target_hand) continue;

                    /* Only one material remains. An already impossible
                     * source count can skip this entire inner scan. */
                    if (target_field - field_ab > 1 ||
                        target_hand - hand_ab > 1) continue;
                    for (ic = 0; ic < eligible_count[2]; ic++) {
                        int field_count, hand_count, better = 0;
                        c = eligible[2][ic];
                        if (c == a || c == b) continue;
                        field_count = field_ab +
                            (candidates[c].where == RITUAL_SRC_FIELD);
                        hand_count = hand_ab +
                            (candidates[c].where == RITUAL_SRC_HAND);
                        if (field_count != target_field || hand_count != target_hand)
                            continue;

                        if (best[0] < 0) better = 1;
                        else {
                            int pick[3] = {a, b, c};
                            for (i = 0; i < 3; i++) {
                                int req = order[i];
                                const RitualCandidate *now = &candidates[pick[req]];
                                const RitualCandidate *old = &candidates[best[req]];
                                int now_better = candidate_stats_better(now, old, prefer_defense);
                                int old_better = candidate_stats_better(old, now, prefer_defense);
                                if (now_better != old_better) {
                                    better = now_better;
                                    break;
                                }
                            }
                        }

                        if (better) {
                            best[0] = a; best[1] = b; best[2] = c;
                        }
                    }
                }
            }

            if (best[0] >= 0) goto selected;
        }
    }
    return 0;

selected:
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
