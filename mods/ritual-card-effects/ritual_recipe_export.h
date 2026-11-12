/* Read-only interface for other mods. No hooks, no duel scan, no card
 * consumption and no changes to the original recipe chooser.
 * IMPORTANT: host->find can return exports from disabled mods. */
static int recipe_export_active;
static void recipe_export_empty(RitualRecipeRequirementV1 *r)
{
    memset(r,0,sizeof(*r));
    r->type=r->min_attack=r->max_attack=r->min_defense=r->max_defense=
        r->min_level=r->max_level=-1;
}
static void recipe_export_convert(RitualRecipeRequirementV1 *d,
                                  const TablesRitualRequirement *s)
{
    recipe_export_empty(d);
    d->exact_card=s->card; d->type=s->type;
    d->custom_group=s->fusion_group;
    d->min_attack=s->min_attack;d->max_attack=s->max_attack;
    d->min_defense=s->min_defense;d->max_defense=s->max_defense;
    d->min_level=s->min_level;d->max_level=s->max_level;
    d->defense_gt_attack=s->defense_gt_attack;
}
static int recipe_export_get(int ritual_magic, RitualRecipeRequirementV1 *out,
                             unsigned capacity, int *result_card)
{
    const RitualOptionDefinition *option;
    TablesRitualRequirement q[3];unsigned short result=0,row[6];
    unsigned i;
    if(!recipe_export_active || !out || !result_card || capacity<3)return -1;
    option=option_by_ritual(ritual_magic);
    if(!option || !recipe_settings[(unsigned)(option-ritual_options)])return -1;
    if(compat_private_requirements(ritual_magic,q,&result)){
        for(i=0;i<3;i++)recipe_export_convert(&out[i],&q[i]);
    }else if(compat_private_recipe(ritual_magic,row)>0){
        result=row[4];
        for(i=0;i<3;i++){recipe_export_empty(&out[i]);out[i].exact_card=row[i+1];}
    }else if(ritual_magic==MASK_RITUAL_ID || ritual_magic==GARMA_SWORD_OATH_ID ||
             ritual_magic==PERFORMANCE_RITUAL_ID){
        /* Named alternatives / gender classes are checked in matches(). */
        for(i=0;i<3;i++)recipe_export_empty(&out[i]);
        result=(unsigned short)option->monster_id;
    }else return -1;
    *result_card=result;
    return 3;
}
static int recipe_export_matches(int ritual_magic, unsigned role, int card_id)
{
    const RitualOptionDefinition *option;
    const CompatPrivateRecipe *recipe;
    TablesRitualRequirement q;
    if(!recipe_export_active||role>=3||!Cards_Valid(card_id)||
       Cards_Type(card_id)>=CARD_TYPE_MAGIC)return 0;
    option=option_by_ritual(ritual_magic);
    if(!option||!recipe_settings[(unsigned)(option-ritual_options)])return 0;
    recipe=compat_private_definition(ritual_magic);
    if(recipe) {
        if(!recipe->conditional)
            return card_id==recipe->q[role].card||Cards_BaseId(card_id)==recipe->q[role].card;
        q=recipe->q[role];
        if(ritual_magic!=CHAKRA_RITUAL_ID && recipe->result>=1 && recipe->result<=CARD_TABLE_COUNT) {
            unsigned stats=(unsigned)gDuel_adwCardStats[recipe->result-1];
            compat_limit_requirement(&q,(stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE,
                ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE);
        }
        return compat_requirement_matches(card_id,&q);
    }
    if(ritual_magic==MASK_RITUAL_ID)
        return role==0?mask_is_named(card_id):
            mask_generic(card_id,role==1?CARD_TYPE_FIEND:CARD_TYPE_SPELLCASTER);
    if(ritual_magic==GARMA_SWORD_OATH_ID)
        return role==0?garma_is_named(card_id):
            garma_generic(card_id,role==1?CARD_TYPE_WARRIOR:CARD_TYPE_FIEND);
    if(ritual_magic==PERFORMANCE_RITUAL_ID)
        return role==0?card_id==DANCING_ELF_ID:
            card_id!=DANCING_ELF_ID&&performance_generic(card_id,role==2);
    return 0;
}
static const RitualRecipeProviderV1 recipe_export_v1={
    RITUAL_RECIPE_PROVIDER_ABI,recipe_export_get,recipe_export_matches
};
