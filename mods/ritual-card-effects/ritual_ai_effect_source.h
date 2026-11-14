/* Effect evaluation is owned here, not inside the duelist planner.
 * This is a pure read-only forecast. No ritual action or card is changed.
 * Publishing is optional and a disabled MOD answers zero. */
static int ritual_effect_ai_enabled(int monster)
{
    return recipe_export_active && ritual_card_effect_enabled(monster);
}
static int ritual_effect_ai_estimate(int monster,const RitualEffectAiBoardV1 *b,
                                      RitualEffectAiEstimateV1 *out)
{
    int i,field=0,hand=0;
    if (!out || !b)return 0;
    memset(out,0,sizeof(*out));
    if (!ritual_effect_ai_enabled(monster))return 0;
    /* Skull Guardian's real buff: +1000 per other Skull on either field,
       +500 per Skull in its owner's hand. Materials are removed FIRST. */
    if(monster==SKULL_GUARDIAN_ID) {
        for(i=0;i<5;i++) {
            if(!b->selected[i] && RitualCategory_IsSkullMonster(b->own[i]))field++;
            if(!b->enemy_hidden[i] && RitualCategory_IsSkullMonster(b->enemy[i]))field++;
            if(i<b->hand_count && !b->selected[i+5] &&
               RitualCategory_IsSkullMonster(b->hand[i]))hand++;
        }
        out->field_synergy=field;
        out->hand_synergy=hand;
        out->attack_bonus=out->defense_bonus=field*SKULL_FIELD_BONUS+hand*SKULL_HAND_BONUS;
    }
    /* Tactical values are NOT stat buffs. Only effects enabled by this MOD
       earn additional priority. Conservative values prevent overcommitting
       tributes just to activate a weak or disabled effect. */
    switch(monster) {
      case YAMADRON_ID: out->strategic_value=600;break; /* renewable burn */
      case PSYCHO_PUPPET_ID: out->strategic_value=800;break; /* control */
      case FIENDS_MIRROR_ID: out->strategic_value=500;break; /* defense/direct */
      case FORTRESS_WHALE_ID: out->strategic_value=350;break;
      case CRAB_TURTLE_ID: out->strategic_value=350;break;
      case JAVELIN_BEETLE_ID: out->strategic_value=400;break;
      case TRI_HORNED_ID: out->strategic_value=400;break;
      case SERPENT_NIGHT_ID: out->strategic_value=350;break;
      case HUNGRY_BURGER_ID: out->strategic_value=300;break;
      case SKULL_GUARDIAN_ID: out->strategic_value=(field+hand)?300:0;break;
      default: out->strategic_value=200;break;
    }
    return 1;
}
static const RitualEffectIntelligenceV1 ritual_effect_intelligence_v1={
    RITUAL_EFFECT_INTELLIGENCE_ABI,
    ritual_effect_ai_enabled,ritual_effect_ai_estimate
};
