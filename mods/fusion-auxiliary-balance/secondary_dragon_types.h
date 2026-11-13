#ifndef FUSION_SECONDARY_DRAGON_TYPES_H
#define FUSION_SECONDARY_DRAGON_TYPES_H
/* Dragon-like creatures: primary Dragon type OR Dragon secondary subtype.
 * The subtype ID set is synchronized from the provided Vanilla Field Effects
 * MOD (6) `secondary_cards[]`, ST_DRAGON. Kept local on purpose: the fusion
 * MOD continues to work alone, even if Field Effects is disabled/absent.
 * When the Field Effects subtype mapping changes, update this list too. */
static int aux_dragon_like(int card_id)
{
    static const short secondary_dragon_ids[] = {
        11, 68, 73, 81, 97, 138, 294, 296, 353, 409, 425, 426, 442, 443, 448, 502, 545, 555, 613, 705
    };
    int left=0,right=(int)(sizeof(secondary_dragon_ids)/sizeof(secondary_dragon_ids[0]))-1;
    if (!Cards_Valid(card_id)) return 0;
    if (Cards_Type(card_id)==CARD_TYPE_DRAGON) return 1;
    while(left<=right) {
        int mid=left+(right-left)/2;
        if(secondary_dragon_ids[mid]==card_id) return 1;
        if(secondary_dragon_ids[mid]<card_id) left=mid+1;
        else right=mid-1;
    }
    return 0;
}
#endif
