/* Invisible, reusable Skull Monster category. Independent of primary type,
 * descriptions, terrain/Field bonuses and the retail Fusion groups.
 * Gameplay ID classification is deliberately explicit and exhaustive. */
#ifndef RITUAL_SKULL_MONSTER_H
#define RITUAL_SKULL_MONSTER_H
static inline int RitualCategory_IsSkullMonster(int card_id)
{
    switch(card_id) {
    case 9: case 22: case 24: case 30: case 59:
    case 132: case 146: case 153: case 154: case 215:
    case 217: case 228: case 241: case 359: case 540: case 545:
    case 548: case 556: case 564: case 704: case 707:
        return 1;
    default: return 0;
    }
}
#endif
