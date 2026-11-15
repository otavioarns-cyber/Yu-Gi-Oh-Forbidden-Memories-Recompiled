/* Approved v0.72 manifest recipes, kept private because native table functions
 * cannot be hooked. Public publication uses the duel recipe table only. */
typedef struct { int id; unsigned short result; int conditional; TablesRitualRequirement q[3]; } CompatPrivateRecipe;
static const CompatPrivateRecipe compat_private_recipes[] = {
{665,362,1,{{.card=296,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=1000,.max_attack=-1,.max_defense=2999,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=1},{.card=0,.min_attack=0,.min_defense=1000,.max_attack=-1,.max_defense=2999,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=1}}},
{666,357,1,{{.card=101,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=1599,.max_defense=1799,.type=CARD_TYPE_DRAGON,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=1599,.max_defense=1799,.type=CARD_TYPE_DRAGON,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{695,715,1,{{.card=166,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=1999,.max_defense=2349,.type=CARD_TYPE_MACHINE,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=1999,.max_defense=2349,.type=CARD_TYPE_FIEND,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{677,702,1,{{.card=14,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=1999,.max_defense=1849,.type=CARD_TYPE_PLANT,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=1999,.max_defense=1849,.type=CARD_TYPE_PYRO,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{674,365,0,{{.card=186,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=261,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=595,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{696,717,1,{{.card=52,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=533,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=2449,.max_defense=2549,.type=CARD_TYPE_INSECT,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{667,374,0,{{.card=371,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=372,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=373,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{692,710,1,{{.card=451,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=2549,.max_defense=2499,.type=CARD_TYPE_AQUA,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=2549,.max_defense=2499,.type=CARD_TYPE_AQUA,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{700,718,1,{{.card=441,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=2349,.max_defense=2149,.type=CARD_TYPE_FISH,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=2349,.max_defense=2149,.type=CARD_TYPE_FISH,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{691,706,1,{{.card=294,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=2349,.max_defense=2399,.type=CARD_TYPE_FIEND,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=2349,.max_defense=2399,.type=CARD_TYPE_DRAGON,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
/* v0.77: exact named materials plus conditional third material. Three
 * distinct physical records and field+hand handling remain owned by
 * compat_choose()/MelhoriaDosRituais. */
{673,356,1,{{.card=403,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=483,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_BEAST,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
{678,703,1,{{.card=453,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=1500,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_BEAST,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=1500,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_WARRIOR,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
/* Public release: Chakra / Resurrection of Chakra (#694) uses Wall Shadow
 * (#369) plus two distinct Fiends of Level 7+, excluding Chakra itself.
 * Group 254 is PRIVATE to this MOD and represents the "not Chakra" rule.
 * Unlike all other conditional recipes, Chakra has NO printed ATK/DEF cap. */
{CHAKRA_RITUAL_ID,CHAKRA_ID,1,{{.card=WALL_SHADOW_ID,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1},
                           {.card=0,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_FIEND,.min_level=7,.max_level=-1,.fusion_group=254},
                           {.card=0,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_FIEND,.min_level=7,.max_level=-1,.fusion_group=254}}},
/* v0.78: Temple of Skulls + two distinct Zombies. */
{699,719,1,{{.card=146,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_ZOMBIE,.min_level=-1,.max_level=-1,.defense_gt_attack=0},{.card=0,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_ZOMBIE,.min_level=-1,.max_level=-1,.defense_gt_attack=0}}},
/* Skull Monster is private fusion_group 255 ONLY to the ritual chooser.
 * It is not passed to Cards_InFusionGroup or added to global Fusion rules. */
{679,704,1,{{.card=535,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1},
            {.card=0,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=-1,.min_level=-1,.max_level=-1,.fusion_group=255},
            {.card=0,.min_attack=0,.min_defense=0,.max_attack=-1,.max_defense=-1,.type=CARD_TYPE_WARRIOR,.min_level=-1,.max_level=-1}}},
};
/* Conditional recipes EXCEPT Chakra share a strict printed-stat ceiling: a material
 * cannot have ATK >= the Ritual result's printed ATK, or DEF >= its printed
 * DEF.  A zero-valued result stat (Millennium Shield has 0 ATK) has no
 * meaningful smaller positive material; in that one dimension, retain the
 * recipe's explicit limits instead of making the ritual impossible.
 *
 * Restrict the returned requirements, not just our local selector: the
 * MelhoriaDosRituais mod reads the same requirements for Field/Hand/Deck. */
static void compat_limit_requirement(TablesRitualRequirement *q,int atk,int def)
{
    if(atk>0 && (q->max_attack<0 || q->max_attack>=atk))q->max_attack=atk-1;
    if(def>0 && (q->max_defense<0 || q->max_defense>=def))q->max_defense=def-1;
}
/* Immutable descriptors only. Printed stats and Settings are still read for
 * every query: another MOD/editor can change them between two predicates. */
static const CompatPrivateRecipe *compat_private_definition(int id)
{
    static const unsigned char indices[700-665+1]={
        [665-665]=1,[666-665]=2,[695-665]=3,[677-665]=4,[674-665]=5,
        [696-665]=6,[667-665]=7,[692-665]=8,[700-665]=9,[691-665]=10,
        [673-665]=11,[678-665]=12,[CHAKRA_RITUAL_ID-665]=13,
        [699-665]=14,[679-665]=15
    };
    unsigned offset=(unsigned)id-665u,i;
    if(offset<sizeof(indices) && indices[offset]) {
        i=(unsigned)indices[offset]-1u;
        if(i<sizeof(compat_private_recipes)/sizeof(compat_private_recipes[0]) &&
           compat_private_recipes[i].id==id)return &compat_private_recipes[i];
    }
    /* Keep newly added/reordered definitions usable until their index is
     * updated. This fallback is never needed for the current descriptors. */
    for(i=0;i<sizeof(compat_private_recipes)/sizeof(compat_private_recipes[0]);++i)
        if(compat_private_recipes[i].id==id)return &compat_private_recipes[i];
    return 0;
}
static void compat_cap_tributes_below_result(TablesRitualRequirement q[3],int result_id)
{
    unsigned stats;
    int atk,def,j;
    if(result_id<1 || result_id>CARD_TABLE_COUNT)return;
    stats=(unsigned)gDuel_adwCardStats[result_id-1];
    atk=(stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    def=((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    for(j=0;j<3;++j)compat_limit_requirement(&q[j],atk,def);
}
static int compat_private_requirements(int id,TablesRitualRequirement *q,unsigned short *result)
{
    const CompatPrivateRecipe *recipe=compat_private_definition(id);
    if(recipe && recipe->conditional) {
        memcpy(q,recipe->q,sizeof(recipe->q));
        *result=recipe->result;
        /* Chakra intentionally accepts tributes stronger than itself. */
        if(id!=CHAKRA_RITUAL_ID)compat_cap_tributes_below_result(q,*result);
        return 1;
    }
    return 0;
}
static int compat_private_recipe(int id,unsigned short *row)
{
    const CompatPrivateRecipe *recipe=compat_private_definition(id);
    int j;
    if(!recipe || recipe->conditional)return -1;
    row[0]=id;
    for(j=0;j<3;++j)row[j+1]=recipe->q[j].card;
    row[4]=recipe->result;row[5]=0;
    return 1;
}
