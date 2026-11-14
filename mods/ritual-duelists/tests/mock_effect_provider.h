/* Unit-test adapter. NEVER included in a production build. The actual
   rules reside in Ritual Effects. This fixture asserts consumer behavior. */
#ifndef RD_EFFECT_PROVIDER_MOCK_H
#define RD_EFFECT_PROVIDER_MOCK_H
static int test_intel_enabled(int card) {return card==704;}
static int test_intel_skull(int id) {
    static const int ids[]={9,22,24,30,59,132,146,153,154,215,217,228,241,359,540,545,548,556,564,704,707};
    unsigned i;for(i=0;i<sizeof(ids)/sizeof(ids[0]);i++)if(ids[i]==id)return 1;
    return 0;
}
static int test_intel_estimate(int monster,const RitualEffectAiBoardV1 *b,RitualEffectAiEstimateV1 *e) {
    int i,field=0,hand=0;
    if(monster!=704)return 0;
    for(i=0;i<5;i++) {
        if(!b->selected[i]&&test_intel_skull(b->own[i]))field++;
        if(!b->enemy_hidden[i]&&test_intel_skull(b->enemy[i]))field++;
        if(i<b->hand_count&&!b->selected[i+5]&&test_intel_skull(b->hand[i]))hand++;
    }
    e->field_synergy=field;e->hand_synergy=hand;
    e->attack_bonus=e->defense_bonus=field*1000+hand*500;
    e->strategic_value=(field+hand)?300:0;
    return 1;
}
static const RitualEffectIntelligenceV1 mock_intel={RITUAL_EFFECT_INTELLIGENCE_ABI,
                                                     test_intel_enabled,test_intel_estimate};
#endif
