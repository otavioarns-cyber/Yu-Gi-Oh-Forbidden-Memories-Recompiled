/* Included after the original recipe gate. Only the live placement wrapper
 * commits provenance. AI recursion has an independent, scoped state. */
typedef struct { unsigned api; unsigned (*lease)(int); void (*revoke)(int); } AuxUnlockV1;
typedef struct { int id; void *data; unsigned serial; } AuxUnlock;
static AuxUnlock aux_unlocks[30];
static unsigned aux_unlock_serial;
static AuxChainCard aux_chain,aux_pair_before,aux_ai_card,aux_ai_next;
static int aux_chain_active,aux_pair_a,aux_pair_b,aux_recipe_probe,aux_ai_depth;
static s32 (*original_equip)(s32,s32);
static void (*original_complete_fusion)(s32);
static int aux_enabled(const char *key)
{ return !aux_host || !aux_host->setting || aux_host->setting(aux_host,key,1)!=0; }
static unsigned aux_unlock_lease(int slot)
{
    DuelCardRecord *r;
    int i;
    if(!aux_owner_applied() || !aux_monster_slot(slot) || !aux_enabled("elegant_egotist"))return 0;
    r=&D_801A7AD8[slot];
    if(!r->data)return 0;
    /* Deck-data identity follows a control change to another field slot. */
    for(i=0;i<30;i++)if(aux_unlocks[i].serial && aux_unlocks[i].id==r->card_id &&
                       aux_unlocks[i].data==r->data)return aux_unlocks[i].serial;
    return 0;
}
static void aux_unlock_revoke(int slot)
{
    int i;
    if(slot<0||slot>=30)return;
    for(i=0;i<30;i++)if(aux_unlocks[i].data==D_801A7AD8[slot].data)
        memset(&aux_unlocks[i],0,sizeof(aux_unlocks[i]));
}
static const AuxUnlockV1 aux_unlock_api={1,aux_unlock_lease,aux_unlock_revoke};
static void aux_unlock_bind(int slot)
{
    AuxUnlock *u;
    if(!aux_enabled("elegant_egotist") || aux_unlock_lease(slot))return;
    u=&aux_unlocks[slot];u->id=D_801A7AD8[slot].card_id;
    u->data=D_801A7AD8[slot].data;
    if(++aux_unlock_serial==0)++aux_unlock_serial;
    u->serial=aux_unlock_serial;
}
static AuxChainCard aux_chain_from_record(int index,int id)
{
    AuxChainCard s={0};s.id=id;
    if(index>=0 && index<30) {
        DuelCardRecord *r=&D_801A7AD8[index];
        if(r->card_id==id) {
            s.fused=fusion_origin[index] && fusion_bound_id[index]==id && fusion_bound_data[index]==r->data;
            s.metal=metal_immune[index] && metal_bound_id[index]==id && metal_bound_data[index]==r->data;
            s.egotist=aux_unlock_lease(index)!=0;
        }
    }
    return s;
}
static int aux_retail_metal_equip(int a,int b)
{
    int result=aux_metal_equip_recipe(Cards_BaseId(a),Cards_BaseId(b));
    int overridden=0;
    if(!result)return 0;
    /* Explicit data-mod removes/replacements still outrank retail recipes. */
    if(Tables_Fusion(a,b,&overridden) && overridden!=result)return 0;
    if(!Tables_FilterFusion(result))return 0;
    return result;
}
static int aux_plain_fusion(int a,int b)
{
    int result,exact;
    if(!Cards_Valid(a)||!Cards_Valid(b))return 0;
    if(Cards_BaseId(a)==AUX_FUSIONIST && Cards_BaseId(b)==AUX_FUSIONIST)return 0;
    exact=aux_added_exact_fusion(Cards_BaseId(a),Cards_BaseId(b));
    /* The engine would run this pair through Duel_CheckFusion, but its
     * Metalmorph result is an Equip action.  Avoid foreign Fusion-only
     * filters in the original_fusion chain (including Mask). */
    result=exact?exact:aux_retail_metal_equip(a,b);
    if(!result) {
        int mapped=0;
        /* A different explicit data-mod recipe is still a normal Fusion. */
        if(!aux_metal_equip_recipe(Cards_BaseId(a),Cards_BaseId(b)) ||
           (Tables_Fusion(a,b,&mapped) && mapped>0))
            result=original_fusion(a,b);
    }
    if((result==AUX_DARKFIRE_DRAGON || result==AUX_TWIN_HEADED_THUNDER_DRAGON ||
        result==AUX_ANCIENT_ELF) &&
       !aux_fusion_result_allowed(result,aux_authorized_fusion(a,b)))result=0;
    return result;
}
static int aux_catalog_monster(int id)
{ return Cards_Valid(id) && Cards_Type(id)<CARD_TYPE_MAGIC; }
#include "egotist_targets.h"
static int aux_catalog_stat(int id,int which)
{
    unsigned value;
    if(!Cards_Valid(id))return 0;
    /* Despite its name Duel_GetBaseCardStat ADDS terrain. Read the printed
     * card-table bits directly, after the engine has applied card mods. */
    value=(unsigned)gDuel_adwCardStats[id-1];
    if(which)value>>=CARD_STAT_DEFENSE_SHIFT;
    return (int)(value&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
}
static unsigned aux_catalog_draw(unsigned n)
{
    unsigned value,limit;
    if(!metal_placement_check)return n-1; /* AI/preview never consumes RNG. */
    limit=(unsigned)RAND_MAX-((unsigned)RAND_MAX%n);
    do {value=(unsigned)rand();}while(value>=limit);
    return value%n;
}
static int aux_wildcard(int partner,int strong,int *substitute)
{
    AuxFusionCatalog c;
    unsigned char *seen;
    int result,old_scoring=ai_scoring;
    if(gCard_nCount<=0 || gCard_nCount>32767)return 0;
    seen=(unsigned char *)malloc((size_t)gCard_nCount+1);
    if(!seen)return 0;
    c.count=gCard_nCount;c.recipe=aux_plain_fusion;
    c.monster=aux_catalog_monster;c.stat=aux_catalog_stat;c.draw=aux_catalog_draw;
    ++aux_recipe_probe;ai_scoring=0;
    result=aux_fusionist_pick(&c,partner,strong,seen,substitute);
    ai_scoring=old_scoring;--aux_recipe_probe;free(seen);
    return result;
}
static void aux_publish_chain(void)
{
    fusion_pending_result=aux_chain.fused?aux_chain.id:0;
    metal_pending_result=aux_chain.metal?aux_chain.id:0;
    fusion_pending_side=metal_pending_side=D_8009B1D5;
    placement_was_fusion=aux_chain.fused;
}
static s32 aux_check_fusion(s32 a,s32 b)
{
    AuxChainCard prior={0},next;
    int result,substitute=0,aa,bb;
    if(!Cards_Valid(a)||!Cards_Valid(b))return 0;
    aa=Cards_BaseId(a);bb=Cards_BaseId(b);
    if(aux_recipe_probe)return aux_plain_fusion(a,b);
    if(aux_cpu_keep_darkfire(a,b))return 0;
    if(metal_placement_check) {
        if(!aux_chain_active || aux_chain.id!=a) {
            int slot=D_800E9EF0[0]?(int)D_800E9EF0[0]->field_6A:-1;
            aux_chain=aux_chain_from_record(slot,a);aux_chain_active=1;
        }
        prior=aux_chain;aux_pair_before=prior;aux_pair_a=a;aux_pair_b=b;
    } else if(aux_ai_depth)prior=aux_ai_card;
    if(aa==AUX_FUSIONIST && bb==AUX_FUSIONIST)result=0;
    /* Preserve the native Harpie transformation before equipment handling. */
    else if((aa==62 && bb==AUX_EGOTIST) || (bb==62 && aa==AUX_EGOTIST))result=63;
    else if(aa==AUX_EGOTIST || bb==AUX_EGOTIST)result=0;
    else if(aux_enabled("fusionist_effects") && bb==AUX_FUSIONIST)
        result=aux_wildcard(a,0,&substitute);
    else if(aux_enabled("fusionist_effects") && aa==AUX_FUSIONIST &&
            (prior.fused || prior.egotist || aux_ignore_conditions()))
        result=aux_wildcard(b,1,&substitute);
    else result=aux_plain_fusion(a,b);
    if(result) {
        int equip=aux_real_equip_transformation(aa,bb,result);
        if(Tower_Refuses(equip?TOWER_RULE_NO_EQUIPS:TOWER_RULE_NO_FUSIONS,a,b,result))
            result=0;
        /* Kamion Wizard's Equip veto must apply to these visual fusions as
         * well. Preserve its CPU first-failure discovery policy. */
        else if(equip && D_8009B1D5>=0 && D_8009B1D5<2 &&
                aux_kamion_refuses_equip(D_8009B1D5,aux_has_kamion(D_8009B1D5^1)) &&
                (D_8009B1D5!=1 || metal_placement_check ||
                 kamion_cpu_learned[D_8009B1D5])) {
            if(metal_placement_check && D_8009B1D5==1)
                kamion_cpu_learned[1]=1;
            result=0;
        }
    }
    next=aux_chain_fused(a,b,result,substitute,aux_is_metal_family(result));
    if(!result) {
        /* Retail failure discards the earlier monster, except when the
         * later card is non-monster. A following successful equip overrides. */
        if(aux_catalog_monster(a) && !aux_catalog_monster(b))next=prior;
        else {memset(&next,0,sizeof(next));next.id=b;}
    }
    if(metal_placement_check){aux_chain=next;aux_publish_chain();}
    if(aux_ai_depth)aux_ai_next=next;
    return result;
}
static void aux_egotist_event(MemoriesModEvent *event)
{
    if(event->phase!=MEMORIES_BEFORE || event->handled ||
       Cards_BaseId(event->a)!=AUX_EGOTIST)return;
    event->handled=1;
    event->result=aux_egotist_target(event->b);
}
static s32 aux_check_equip(s32 a,s32 b)
{
    int result;
    AuxChainCard s;
    /* Enforce the target restriction even if another equip hook grants
     * blanket compatibility. Native denial/Tower/Kamion still runs below. */
    if(Cards_BaseId(a)==AUX_EGOTIST && !aux_egotist_target(b))return 0;
    if(Cards_BaseId(b)==AUX_EGOTIST && !aux_egotist_target(a))return 0;
    /* Retail AI asks only one orientation. Only this new equip gains the
     * reverse query; live placement already asks both orientations. */
    if(aux_ai_depth && Cards_BaseId(b)==AUX_EGOTIST && aux_catalog_monster(a))
        result=original_equip(b,a);
    else result=original_equip(a,b);
    if(!result || aux_recipe_probe)return result;
    if(metal_placement_check) {
        if(aux_pair_before.id==result)s=aux_pair_before;
        else {
            int slot=D_800E9EF0[1]?(int)D_800E9EF0[1]->field_6A:-1;
            s=aux_chain_from_record(slot,result);
        }
        if(Cards_BaseId(a)==AUX_EGOTIST || Cards_BaseId(b)==AUX_EGOTIST){s.unlock_new|=!s.egotist && !s.fused && !aux_ignore_conditions();s.egotist=1;}
        aux_chain=s;aux_chain_active=1;aux_publish_chain();
    }
    if(aux_ai_depth) {
        s=aux_ai_card;
        if(s.id!=result){memset(&s,0,sizeof(s));s.id=result;}
        if(Cards_BaseId(a)==AUX_EGOTIST || Cards_BaseId(b)==AUX_EGOTIST){s.unlock_new|=!s.egotist && !s.fused && !aux_ignore_conditions();s.egotist=1;}
        aux_ai_next=s;
    }
    return result;
}
static void aux_complete_fusion(s32 id)
{
    AuxChainCard saved=aux_ai_card,saved_next=aux_ai_next;
    if(aux_ai_depth && aux_ai_next.id==id)aux_ai_card=aux_ai_next;
    else {
        int root=gAiScript_State.fusion_path[0];
        int slot=(gAiScript_State.fusion_depth==1 && root>=1 && root<=5)?19+root:-1;
        aux_ai_card=aux_chain_from_record(slot,id);
    }
    ++aux_ai_depth;original_complete_fusion(id);--aux_ai_depth;
    aux_ai_card=saved;aux_ai_next=saved_next;
}
static void aux_new_effects_reset(void)
{
    memset(aux_unlocks,0,sizeof(aux_unlocks));
    memset(&aux_chain,0,sizeof(aux_chain));
    aux_chain_active=aux_ai_depth=aux_recipe_probe=0;
}

/* FM consumes equips into the monster: there is no persistent Equip-zone
 * object to remove. Retire the lease when its monster leaves play, changes
 * identity, or the new effect is switched off. Deactivation during battle
 * is excluded by calling this only in the stable hand/field phases. */
static void aux_unlock_sweep(void)
{
    int i,j;
    for(i=0;i<30;i++)if(aux_unlocks[i].serial) {
        if(aux_enabled("elegant_egotist")) {
            for(j=0;j<30;j++)if(aux_monster_slot(j) &&
                (D_801A7AD8[j].flags&DUEL_CARD_FLAG_OCCUPIED) &&
                D_801A7AD8[j].data==aux_unlocks[i].data &&
                D_801A7AD8[j].card_id==aux_unlocks[i].id)break;
        } else j=30;
        if(j==30)memset(&aux_unlocks[i],0,sizeof(aux_unlocks[i]));
    }
}
