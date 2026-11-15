#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../ritual_effect_intelligence_v1.h"
#include "../skull_monster.h"
#define SKULL_GUARDIAN_ID 704
#define YAMADRON_ID 357
#define PSYCHO_PUPPET_ID 142
#define FIENDS_MIRROR_ID 365
#define FORTRESS_WHALE_ID 436
#define CRAB_TURTLE_ID 507
#define JAVELIN_BEETLE_ID 717
#define TRI_HORNED_ID 705
#define SERPENT_NIGHT_ID 715
#define HUNGRY_BURGER_ID 702
#define SKULL_FIELD_BONUS 1000
#define SKULL_HAND_BONUS 500
static int recipe_export_active=1;
static int skull_on=1,yamadron_on=1;
static int ritual_card_effect_enabled(int id) {return id==704?skull_on:id==357?yamadron_on:0;}
#include "../ritual_ai_effect_source.h"
int main(void) {
    RitualEffectAiBoardV1 b={0}; RitualEffectAiEstimateV1 e={0};
    assert(ritual_effect_intelligence_v1.abi==1);
    b.own[0]=22;b.own[1]=545;b.enemy[0]=59;b.enemy_hidden[0]=1;
    b.hand_count=2;b.hand[0]=24;b.hand[1]=30;
    assert(ritual_effect_ai_estimate(704,&b,&e)==1);
    assert(e.field_synergy==2&&e.hand_synergy==2);
    assert(e.attack_bonus==3000&&e.defense_bonus==3000);
    b.selected[0]=b.selected[5]=1;
    assert(ritual_effect_ai_estimate(704,&b,&e)==1);
    assert(e.field_synergy==1&&e.hand_synergy==1&&e.attack_bonus==1500);
    b.enemy_hidden[0]=0;
    assert(ritual_effect_ai_estimate(704,&b,&e)==1&&e.field_synergy==2);
    assert(ritual_effect_ai_estimate(357,&b,&e)==1&&e.strategic_value>0);
    assert(e.attack_bonus==0&&e.defense_bonus==0);
    yamadron_on=0;
    assert(!ritual_effect_ai_enabled(357));
    assert(!ritual_effect_ai_estimate(357,&b,&e)&&e.strategic_value==0);
    skull_on=0;
    assert(!ritual_effect_ai_estimate(704,&b,&e)&&e.attack_bonus==0);
    skull_on=1;recipe_export_active=0;
    assert(!ritual_effect_ai_enabled(704));
    assert(!ritual_effect_ai_estimate(704,&b,&e));
    puts("PASS: effect owner live Settings/activation, stats, tribute selection, strategy (pure)");
    return 0;
}
