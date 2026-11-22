#include "ai_policy.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void)
{
    RdAiRitualMark tags[RD_AI_RECORDS]={{0}};
    RdAiLiveCard live[RD_AI_RECORDS]={{0}};
    int front[5]={20,21,22,23,24};
    RdAi_MarkRitual(tags,20,357,59);
    live[20]=(RdAiLiveCard){357,59,0x8000};
    assert(RdAi_MarkIsLive(tags,live,20));
    assert(RdAi_FusionUsesRitual(tags,live,front,357,122));
    assert(RdAi_FusionUsesRitual(tags,live,front,343,357));
    assert(!RdAi_FusionUsesRitual(tags,live,front,168,122));
    assert(!RdAi_FusionUsesRitual(tags,live,front,0,357));
    live[20].origin=58;assert(!RdAi_FusionUsesRitual(tags,live,front,357,122));
    live[20].origin=59;live[20].card_id=168;assert(!RdAi_FusionUsesRitual(tags,live,front,357,122));
    live[20].card_id=357;live[20].flags=0;assert(!RdAi_MarkIsLive(tags,live,20));
    live[20].flags=0x8000;assert(RdAi_MarkIsLive(tags,live,20));
    puts("PASS: real ritual instance fusion block, identity,");
    return 0;
}
