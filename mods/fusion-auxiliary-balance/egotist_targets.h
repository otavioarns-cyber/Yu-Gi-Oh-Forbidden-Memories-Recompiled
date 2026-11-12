#include "origin_effects_v1.h"
#include "ritual_effect_intelligence_v1.h"
typedef struct { char owner[64]; int (*enabled)(int); } AuxEffectProvider;
static AuxEffectProvider aux_effect_providers[16];
static int aux_register_effect_provider(const char *owner,int (*enabled)(int))
{
    int i,empty=-1;
    if(!owner || !*owner || strlen(owner)>=64)return 0;
    for(i=0;i<16;i++) {
        if(!aux_effect_providers[i].owner[0]) {if(empty<0)empty=i;continue;}
        if(!strcmp(aux_effect_providers[i].owner,owner)) {
            if(!enabled)memset(&aux_effect_providers[i],0,sizeof(aux_effect_providers[i]));
            else aux_effect_providers[i].enabled=enabled;
            return 1;
        }
    }
    if(!enabled)return 1;
    if(empty<0)return 0;
    strcpy(aux_effect_providers[empty].owner,owner);
    aux_effect_providers[empty].enabled=enabled;
    return 1;
}
static int aux_egotist_target(int card)
{
    int i,id;
    const RitualEffectIntelligenceV1 *ritual;
    if(!aux_enabled("elegant_egotist") || !aux_catalog_monster(card))return 0;
    id=Cards_BaseId(card);
    if(aux_is_metal_family(id) ||
       (aux_is_fusion_family(id) && r2_setting_enabled(id) && (id!=AUX_FUSIONIST || aux_enabled("fusionist_effects"))))return 1;
    /* Resolve on demand: load order and disabled Ritual MOD are respected.
     * This uses its existing catalogue, never a duplicate list of Ritual IDs. */
    ritual=aux_host && aux_host->find ? (const RitualEffectIntelligenceV1 *)
        aux_host->find(aux_host,"ritual-card-effects-test:effect_intelligence_v1"):0;
    if(ritual && ritual->abi==1 && ritual->enabled && ritual->enabled(id))return 1;
    for(i=0;i<16;i++)if(aux_effect_providers[i].enabled &&
        aux_effect_providers[i].enabled(id))return 1;
    return 0;
}
static const OriginEffectsV1 aux_origin_effects_api={1,aux_register_effect_provider,aux_unlock_lease};
