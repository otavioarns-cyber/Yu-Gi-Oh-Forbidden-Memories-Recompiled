#include "ai_policy.h"
#define RD_OCCUPIED 0x8000u
#define RD_USED 0x4000u
#define RD_FACE_DOWN 0x1000u

void RdAi_MarkRitual(RdAiRitualMark marks[RD_AI_RECORDS], int record,
                     int card_id, int origin)
{
    if (!marks || record<0 || record>=RD_AI_RECORDS) return;
    marks[record].active=(card_id>0 && origin>=-1 && origin<=127);
    marks[record].card_id=(short)card_id;
    marks[record].origin=(signed char)origin;
}

int RdAi_MarkIsLive(const RdAiRitualMark marks[RD_AI_RECORDS],
                     const RdAiLiveCard live[RD_AI_RECORDS], int record)
{
    const RdAiRitualMark *tag;
    const RdAiLiveCard *card;
    if(!marks||!live||record<0||record>=RD_AI_RECORDS)return 0;
    tag=&marks[record];card=&live[record];
    return tag->active && (card->flags & RD_OCCUPIED) &&
           card->card_id==tag->card_id && card->origin==tag->origin;
}
int RdAi_FusionUsesRitual(const RdAiRitualMark marks[RD_AI_RECORDS],
                         const RdAiLiveCard live[RD_AI_RECORDS],
                         const int field_records[5],int id_a,int id_b)
{
    int i,record;
    if(!field_records || id_a<=0 || id_b<=0)return 0;
    for(i=0;i<5;i++){
        record=field_records[i];
        if(RdAi_MarkIsLive(marks,live,record) &&
           (live[record].card_id==id_a || live[record].card_id==id_b))return 1;
    }
    return 0;
}
