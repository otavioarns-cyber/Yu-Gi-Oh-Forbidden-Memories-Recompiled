#include "ritual_duelists.h"
#include "planner.h"
#include "ritual_effect_intelligence_v1.h"
#include "profile_data.h"

/* Group IDs are profile-private. Other duelists may use different callbacks,
   so the universal planner knows nothing of the Skull category. */
static int rd_group_match(const RdOpponentProfile *p,int card_id,int group)
{
    if(group==1)return Rd_IsSkull(p,card_id);
    return 0;
}
/* Ritual Duelists NEVER estimates a particular effect by itself.
 * The optional Ritual Effects provider supplies the active rule and value.
 * The CPU can still summon any ritual without this provider, but it gets
 * no imaginary stat buffs or extra strategic priority. */
#ifdef RD_TEST_PROVIDER
#include "tests/mock_effect_provider.h"
static const RitualEffectIntelligenceV1 *rd_effect_intel=&mock_intel;
#else
static const RitualEffectIntelligenceV1 *rd_effect_intel;
#endif
void Rd_ConnectEffectIntelligence(const RitualEffectIntelligenceV1 *provider)
{
    rd_effect_intel=provider && provider->abi==RITUAL_EFFECT_INTELLIGENCE_ABI &&
        provider->enabled && provider->estimate ? provider : 0;
}
int Rd_EffectIntelligenceEnabled(int monster)
{
    return rd_effect_intel && rd_effect_intel->enabled(monster)!=0;
}
static RdEffectEstimate rd_effect_forecast(const RdOpponentProfile *p,int ritual_result,const RdBoard *b,
                                     const int *selected,unsigned n,int mode)
{
    RdEffectEstimate e={0};RitualEffectAiBoardV1 snapshot={0};
    RitualEffectAiEstimateV1 out={0};int i;
    if(!p || !b || mode!=0 || !Rd_EffectIntelligenceEnabled(ritual_result))return e;
    for(i=0;i<5;i++) {
        snapshot.own[i]=b->own[i].id;
        snapshot.enemy[i]=b->enemy[i].id;
        snapshot.hand[i]=b->hand[i].id;
        snapshot.own_hidden[i]=(unsigned char)!!b->own[i].face_down;
        snapshot.enemy_hidden[i]=(unsigned char)!!b->enemy[i].face_down;
    }
    snapshot.hand_count=b->actual_hand<0?0:b->actual_hand>5?5:b->actual_hand;
    snapshot.own_lp=b->own_lp;
    snapshot.enemy_lp=b->enemy_lp;
    if(selected)for(i=0;i<(int)n && i<RD_MAX_RITUAL_MATERIALS;i++)
        if(selected[i]>=0 && selected[i]<10)snapshot.selected[selected[i]]=1;
    if(rd_effect_intel->estimate(ritual_result,&snapshot,&out)) {
        e.attack_bonus=out.attack_bonus;
        e.defense_bonus=out.defense_bonus;
        e.field_synergy=out.field_synergy;
        e.hand_synergy=out.hand_synergy;
        e.strategy_bonus=out.strategic_value;
    }
    return e;
}
/* The manual Jono recipe is a known option. Active providers/tables take
 * precedence, so these identifiers never override user-edited conditions. */
static const RdRitualRequirement jono_modified_materials[] = {
    {14,RD_ANY,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY},
    {0,19,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY},
    {0,17,0,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY,RD_ANY}
};
static const u16 jono_preferred[]={14,123,133};
static const u16 jono2_preferred[]={14,637,473};
static const u16 weevil_preferred[]={52,533};
/* Javelin's extra Insect stat conditions and Shadi's "Six-Armed Woman"
 * specification are provider-defined. Until a true active recipe is exposed,
 * requesting manual mode for those opponents safely uses the disc recipe. */
static const u16 heishin_materials[]={7,35,417};
static const u16 trihorn_materials[]={6,47,571};
static const u16 desert_materials[]={146,479,485};
/* Vanilla preferences only: active ritual providers/tables can override these. */
static const u16 bakura_materials[]={186,261,595};
static const u16 pegasus_materials[]={166,405,416};
static const u16 mage_soldier_materials[]={255,296,629};
static const u16 ocean_materials[]={436,441,542}; /* Vanilla: White Dolphin #436, Mech Bass #441, Misairuzame #542 */
static const u16 secmeton_materials[]={89,449,451};
/* Preferences only; all conditions still come from the live recipe provider. */
static const u16 forest_materials[]={403,483};
static const u16 labyrinth_baseline[]={371,372,373};
static const u16 sebek_baseline[]={535,22,38}; /* Kamakiriman only via package; category from provider */
static const u16 neku_baseline[]={85,298,377};
/* The three Blue-Eyes are physical copies of one card ID, not three names. */
static const u16 seto3rd_baseline[]={1,1,1};
static const RdOpponentProfile profiles[] = {
    /* Historical index 0 retained for stable profile numbering and audit only.
     * Runtime always delegates Isis to the original game logic. */
    {
      "Isis",rd_isis_pool,RD_ISIS_POOL_COUNT,
      {0,0,0,0,0,0},
      0,0,0,0,0,0
    },
    {
      "Jono",rd_jono_pool,RD_JONO_POOL_COUNT,
      {677,702,2000,1850,jono_modified_materials,3},
      jono_preferred,3,0,0,0,rd_effect_forecast
    },
    {
      "Jono 2nd",rd_jono2_pool,RD_JONO2_POOL_COUNT,
      {677,702,2000,1850,jono_modified_materials,3},
      jono2_preferred,3,0,0,0,rd_effect_forecast
    },
    {
      "Weevil Underwood",rd_weevil_pool,RD_WEEVIL_POOL_COUNT,
      {696,717,2450,2550,0,0},
      weevil_preferred,2,0,0,0,rd_effect_forecast
    },
    {
      "Shadi",rd_shadi_pool,RD_SHADI_POOL_COUNT,
      {693,720,2000,1800,0,0},
      0,0,0,0,0,rd_effect_forecast
    },
    {
      "Heishin",rd_heishin_pool,RD_HEISHIN_POOL_COUNT,
      {721,722,2800,2600,0,0},
      heishin_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Heishin 2nd",rd_heishin2_pool,RD_HEISHIN2_POOL_COUNT,
      {721,722,2800,2600,0,0},
      heishin_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Rex Raptor",rd_rex_pool,RD_REX_POOL_COUNT,
      {680,705,2850,2350,0,0},
      trihorn_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "High Mage Martis",rd_martis_pool,RD_MARTIS_POOL_COUNT,
      {680,705,2850,2350,0,0},
      trihorn_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Desert Mage",rd_desert_pool,RD_DESERT_POOL_COUNT,
      {699,719,1900,1850,0,0},
      desert_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Yami Bakura",rd_bakura_pool,RD_BAKURA_POOL_COUNT,
      {674,365,2100,1800,0,0},
      bakura_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Pegasus",rd_pegasus_pool,RD_PEGASUS_POOL_COUNT,
      {695,715,2000,2350,0,0},
      pegasus_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Mage Soldier",rd_mage_soldier_pool,RD_MAGE_SOLDIER_POOL_COUNT,
      {665,362,0,3000,0,0},
      mage_soldier_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Teana 2nd",rd_tea2_pool,RD_TEA2_POOL_COUNT,
      {698,708,2900,2450,0,0},
      0,0,0,0,0,rd_effect_forecast
    },
    {
      "Ocean Mage",rd_ocean_pool,RD_OCEAN_POOL_COUNT,
      {700,718,2350,2150,0,0},
      ocean_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "High Mage Secmeton",rd_secmeton_pool,RD_SECMETON_POOL_COUNT,
      {692,710,2550,2500,0,0},
      secmeton_materials,3,0,0,0,rd_effect_forecast
    },
    {
      "Forest Mage",rd_forest_pool,RD_FOREST_POOL_COUNT,
      {673,356,2300,2100,0,0},
      forest_materials,2,0,0,0,rd_effect_forecast
    },
    {
      "High Mage Anubisius",rd_anubisius_pool,RD_ANUBISIUS_POOL_COUNT,
      {678,703,2750,2500,0,0},
      0,0,0,0,0,rd_effect_forecast
    },
    {
      "Mountain Mage",rd_mountain_pool,RD_MOUNTAIN_POOL_COUNT,
      {666,357,1600,1800,0,0},
      0,0,0,0,0,rd_effect_forecast
    },
    /* V3: explicit virtual packages; all materials come from active runtime rules. */
    {
      "High Mage Atenza",rd_atenza_pool,RD_ATENZA_POOL_COUNT,
      {691,706,2350,2400,0,0},
      0,0,0,0,0,rd_effect_forecast
    },
    {
      "Meadow Mage",rd_meadow_pool,RD_MEADOW_POOL_COUNT,
      {697,716,2550,2150,0,0},
      0,0,0,0,0,rd_effect_forecast
    },
    {
      "High Mage Kepura",rd_kepura_pool,RD_KEPURA_POOL_COUNT,
      {670,364,3000,2500,0,0},
      0,0,0,0,0,rd_effect_forecast
    },
    /* V3: recipe contents come only from the live recipe provider/game. */
    {"Labyrinth Mage",rd_labyrinth_pool,RD_LABYRINTH_POOL_COUNT,
     {667,374,3750,3400,0,0},labyrinth_baseline,3,0,0,0,rd_effect_forecast},
    {"Guardian Sebek",rd_sebek_pool,RD_SEBEK_POOL_COUNT,
     {679,704,2050,2500,0,0},sebek_baseline,3,rd_sebek_skulls,21,rd_group_match,rd_effect_forecast},
    {"Guardian Neku",rd_neku_pool,RD_NEKU_POOL_COUNT,
     {671,360,2800,2300,0,0},neku_baseline,3,0,0,0,rd_effect_forecast},
    /* V3: only pools + packages. Chakra's conditional Fiend recipe and its
     * private exclusions are resolved by the active provider/tables, never
     * copied into these profiles. */
    {"Seto 3rd",rd_seto3rd_pool,RD_SETO3RD_POOL_COUNT,
     {675,380,4500,3800,0,0},seto3rd_baseline,3,0,0,0,rd_effect_forecast},
    {"DarkNite",rd_darknite_pool,RD_DARKNITE_POOL_COUNT,
     {694,709,2450,2000,0,0},0,0,0,0,0,rd_effect_forecast},
    {"Nitemare",rd_nitemare_pool,RD_NITEMARE_POOL_COUNT,
     {694,709,2450,2000,0,0},0,0,0,0,0,rd_effect_forecast},
    /* V4: Mai stays almost completely Vanilla, with a virtual Performance Package. */
    {"Mai Valentine",rd_mai_pool,RD_MAI_POOL_COUNT,
     {676,701,1950,1850,0,0},0,0,0,0,0,rd_effect_forecast}
};
const RdOpponentProfile *Rd_GetProfile(int index)
{
    return index>=0 && index<(int)(sizeof(profiles)/sizeof(profiles[0]))?&profiles[index]:0;
}
int Rd_ProfileCount(void) {return (int)(sizeof(profiles)/sizeof(profiles[0]));}
int Rd_IsSkull(const RdOpponentProfile *p, int id)
{
    int i;for(i=0;i<p->skull_count;++i)if(p->skull_ids[i]==id)return 1;
    return 0;
}
