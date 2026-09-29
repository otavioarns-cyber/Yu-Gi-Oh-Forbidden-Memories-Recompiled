#include "../types.h"
#include "duel_check_ritual.h"
#define D_8009B1D5_IS_ABSOLUTE_SCALAR
#include "duel_side_state.h"
#include "duel_card.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#ifdef MEMORIES_PC
#include "pc/cards/cards.h"
#include "pc/cards/tables.h"
#endif

s32 Duel_CheckRitual(DuelRitualResult *out, s32 ritualId)
{
    DuelCardRecord *found[DUEL_RITUAL_TRIBUTE_COUNT];
    DuelCardRecord *cands[DUEL_FIELD_ROW_SIZE];
    DuelCardRecord *card;
#ifndef MEMORIES_PC
    DuelCardRecord **first;
    DuelCardRecord **dst;
#endif
    DuelCardRecord **w;
    DuelCardRecord *c;
    u16 *p;
#ifndef MEMORIES_PC
    u16 *q;
#endif
    s32 i;
    s32 j;
#ifdef MEMORIES_PC
    TablesRitualRequirement requirements[DUEL_RITUAL_TRIBUTE_COUNT];
    u16 conditional_result = 0;
    if (Tables_RitualRequirements(ritualId, requirements, &conditional_result)) {
        int used[DUEL_FIELD_ROW_SIZE] = {0};
        i = DUEL_FIELD_ROW_SIZE;
        if (D_8009B1D5 != 0) i = DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE;
        c = &D_801A7AD8[i];
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) cands[i] = (c[i].flags & DUEL_CARD_FLAG_OCCUPIED) ? &c[i] : 0;
        for (j = 0; j < DUEL_RITUAL_TRIBUTE_COUNT; j++) {
            found[j] = 0;
            for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                int stats, attack, defense, id;
                if (used[i] || !cands[i]) continue;
                id = cands[i]->card_id;
                stats = gDuel_adwCardStats[id - 1];
                attack = (stats & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defense = ((stats >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                if (requirements[j].card && id != requirements[j].card &&
                    Cards_BaseId(id) != requirements[j].card) continue;
                if (requirements[j].type >= 0 && Cards_Type(id) != requirements[j].type) continue;
                if (attack < requirements[j].min_attack || defense < requirements[j].min_defense) continue;
                if (requirements[j].defense_gt_attack && defense <= attack) continue;
                found[j] = cands[i];
                used[i] = 1;
                break;
            }
            if (!found[j]) return 0;
        }
        if (out != 0) {
            for (i = 0; i < DUEL_RITUAL_TRIBUTE_COUNT; i++) out->tribute_objects[i] = found[i]->object;
            out->field_0C = 0;
        }
        return conditional_result;
    }
#endif
#ifdef MEMORIES_PC
    /* A mod's recipe, laid out as the disc's table is, comes first. */
    u16 own[DUEL_RITUAL_RECIPE_HALFWORD_COUNT + 1];
    s32 ruled = Tables_Ritual(ritualId, own);

    if (ruled == 0) {
        return 0;
    }
    p = ruled > 0 ? own : gDuel_awRitualData;
#else
    p = gDuel_awRitualData;
#endif
    while (1) {
        if (p[0] == 0) {
            return 0;
        }
        if (p[0] == ritualId) {
            break;
        }
        p += DUEL_RITUAL_RECIPE_HALFWORD_COUNT;
    }

    i = DUEL_FIELD_ROW_SIZE;
    if (D_8009B1D5 != 0) {
        i = DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE;
    }
    c = &D_801A7AD8[i];
    i = 0;
    w = cands;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        *w = 0;
        if (c->flags & DUEL_CARD_FLAG_OCCUPIED) {
            *w = c;
        }
        w++;
        c++;
    }

    p++;
#ifdef MEMORIES_PC
    /* A copy of a tribute monster counts as it; a mod's recipe may also
       name the copy itself. Every tribute takes a monster that is exactly
       it first, and only then one that is a copy of it: taken in the
       recipe's order, a retail tribute could take the very copy a later
       one names while the retail monster stays on the field. */
    {
        s32 pass;

        for (j = 0; j < DUEL_RITUAL_TRIBUTE_COUNT; j++) {
            found[j] = 0;
        }
        for (pass = 0; pass < 2; pass++) {
            for (j = 0; j < DUEL_RITUAL_TRIBUTE_COUNT; j++) {
                if (found[j] != 0) {
                    continue;
                }
                for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                    card = cands[i];
                    if (card != 0 && (pass == 0 ? card->card_id == p[j] :
                                      Cards_BaseId(card->card_id) == p[j])) {
                        found[j] = card;
                        cands[i] = 0;
                        break;
                    }
                }
            }
        }
        for (j = 0; j < DUEL_RITUAL_TRIBUTE_COUNT; j++) {
            if (found[j] == 0) {
                return 0;
            }
        }
    }
#else
    j = 0;
    first = cands;
    dst = found;
    q = p;
    for (j = 0; j < DUEL_RITUAL_TRIBUTE_COUNT; j++) {
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            card = (c = first[i]);
            if (card != 0 && card->card_id == q[0]) {
                goto matched;
            }
        }
        return 0;
matched:
        *dst++ = card;
        first[i] = 0;
        q++;
    }
#endif

    if (out != 0) {
        for (i = 0; i < DUEL_RITUAL_TRIBUTE_COUNT; i++) {
            out->tribute_objects[i] = found[i]->object;
        }
        out->field_0C = 0;
    }
    return p[DUEL_RITUAL_TRIBUTE_COUNT];
}
