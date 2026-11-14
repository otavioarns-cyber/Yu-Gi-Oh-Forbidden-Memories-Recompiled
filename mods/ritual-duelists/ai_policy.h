#ifndef RD_AI_POLICY_H
#define RD_AI_POLICY_H
/* Small independent AI-policy helpers: no gameplay hooks and no card tables. */
#define RD_AI_RECORDS 30

typedef struct {
    short card_id;
    signed char origin;
    unsigned char active;
} RdAiRitualMark;

typedef struct {
    int card_id;
    int origin;
    unsigned flags;
} RdAiLiveCard;

/* Marks identify one physical card, not every copy of the ritual result ID. */
void RdAi_MarkRitual(RdAiRitualMark marks[RD_AI_RECORDS], int record,
                     int card_id, int origin);
int RdAi_MarkIsLive(const RdAiRitualMark marks[RD_AI_RECORDS],
                     const RdAiLiveCard live[RD_AI_RECORDS], int record);
/* Refuse an AI fusion if EITHER operand names a protected ritual result
 * actually still present on the current side's field. Because vanilla
 * Duel_CheckFusion takes only IDs, this is deliberately conservative if a
 * separate, normal copy of the same ID is also present. */
int RdAi_FusionUsesRitual(const RdAiRitualMark marks[RD_AI_RECORDS],
                         const RdAiLiveCard live[RD_AI_RECORDS],
                         const int field_records[5],int id_a,int id_b);
#endif
