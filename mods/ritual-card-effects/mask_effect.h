/* Private extension of the v0.39 mod, not an engine/framework patch.
 * Identity is the deck-record pointer, never the card id or field slot.
 * Planning only consults learned knowledge; placement records discovery. */
#define MASK_ID 720
#define MASK_RITUAL_ID 693
typedef struct { void *data; unsigned learned; int slot; } MaskInstance;
static MaskInstance mask_instances[10];
static unsigned mask_instance_count, mask_instance_mask;
static int mask_inside_placement;
static unsigned mask_blocked, mask_learned, mask_released, mask_summons;
static int mask_known_sides;
static int mask_materials[3];
static s32 psycho_check_ritual(DuelRitualResult *, s32);

static int mask_slot_for(MaskInstance *m)
{
    int i, slot;
    if (!m || !m->data) return -1;
    /* The Ritual result already tells us its field record.  Keep that slot as
     * a hot-path cache and only fall back to the ten-zone identity search if
     * retail has rebuilt/moved the record.  The identity checks are identical
     * to the old scan, so the cache cannot make a stale instance look live. */
    slot=m->slot;
    if (monster_slot(slot) && D_801A7AD8[slot].data == m->data &&
        D_801A7AD8[slot].card_id == MASK_ID &&
        (D_801A7AD8[slot].flags & DUEL_CARD_FLAG_OCCUPIED)) return slot;
    for (i=5; i<10; ++i)
        if (D_801A7AD8[i].data == m->data && D_801A7AD8[i].card_id == MASK_ID &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED)) { m->slot=i; return i; }
    for (i=20; i<25; ++i)
        if (D_801A7AD8[i].data == m->data && D_801A7AD8[i].card_id == MASK_ID &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED)) { m->slot=i; return i; }
    m->slot=-1;
    return -1;
}
static void mask_forget(void *data)
{
    unsigned pending;
    if(!mask_instance_count || !data)return;
    pending=mask_instance_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        if (mask_instances[i].data == data) {
            mask_instances[i].data=0;
            mask_instances[i].learned=0;
            mask_instances[i].slot=-1;
            mask_instance_mask&=~(1u<<i);
            if(mask_instance_count)--mask_instance_count;
        }
    }
}
static void mask_summon(DuelCardRecord *card)
{
    unsigned free_mask; int i;
    if (card->card_id != MASK_ID || !card->data ||
        !(card->flags & DUEL_CARD_FLAG_OCCUPIED)) return;
    mask_forget(card->data);
    free_mask=(~mask_instance_mask)&0x3ffu;
    if(!free_mask)return;
    i=__builtin_ctz(free_mask);
    mask_instances[i].data=card->data;
    mask_instances[i].learned=0;
    mask_instances[i].slot=record_index(card);
    mask_instance_mask|=1u<<i;
    ++mask_instance_count;
    ++mask_summons;
}
static int mask_known(void)
{
    int slot, sides=0; unsigned pending;
    if(!mask_instance_count)return 0;
    pending=mask_instance_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        slot=mask_slot_for(&mask_instances[i]);
        if (slot>=0) sides |= mask_instances[i].learned & (1u << (1-side_for_record(slot)));
    }
    return sides;
}
/* Run only at stable field boundaries, never while battle hides records. */
static void mask_reconcile(void)
{
    int slot,now=0; unsigned pending;
    if(!mask_instance_count){if(mask_known_sides)++mask_released;mask_known_sides=0;return;}
    /* Visit only occupied MaskInstance entries.  Cached field slots avoid the
     * old repeated ten-zone lookup while preserving a full identity fallback. */
    pending=mask_instance_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        slot=mask_slot_for(&mask_instances[i]);
        if(slot<0) {
            mask_instances[i].data=0;
            mask_instances[i].learned=0;
            mask_instances[i].slot=-1;
            mask_instance_mask&=~(1u<<i);
            if(mask_instance_count)--mask_instance_count;
        } else {
            now |= mask_instances[i].learned & (1u << (1-side_for_record(slot)));
        }
    }
    if (mask_known_sides & ~now) ++mask_released;
    mask_known_sides=now;
}
static int mask_is_real_equip_transform(int a,int b,int result)
{
    /* All recipes with REAL Metalmorph/Egotist cards are Equip outcomes.
     * A Fusionist's virtual substitution does not qualify by itself. */
    return result>0 && (a==658 || b==658 || a==318 || b==318);
}
static s32 mask_filter_fusion(s32 result,int a,int b)
{
    int slot, side=D_8009B1D5, blocked=0, visual_slot=-1; unsigned pending;
    if (mask_is_real_equip_transform(a,b,result))return result;
    if (!result || side<0 || side>1 || !ritual_card_effect_enabled(MASK_ID) || !mask_instance_count) return result;
    pending=mask_instance_mask;
    while(pending) {
        int i=take_mask_bit(&pending);
        slot=mask_slot_for(&mask_instances[i]);
        if (slot<0 || side_for_record(slot)==side) continue;
        if (mask_inside_placement) {
            blocked=1;
            if (visual_slot < 0) visual_slot = slot;
            /* Execution is the evidence of discovery. Do not require a
             * planner hook: internal calls may bypass those entry points.
             * Knowledge is side-specific; only later Fusion queries from
             * that side are filtered. Equip queries are never intercepted. */
            if (!(mask_instances[i].learned & (1u << side))) {
                mask_instances[i].learned |= 1u << side;
                ++mask_learned;
            }
        } else if (mask_instances[i].learned & (1u << side)) blocked=1;
    }
    if (blocked && mask_inside_placement) {
        /* Same native field marker used by the shared visual layer. Queue
         * exactly one opposing Mask per failed Fusion, even if several are
         * active. Equip resolution is untouched because this function only
         * receives a successful Fusion result. */
        if (visual_slot >= 0) visual_queue_activation(visual_slot);
        ++mask_blocked;
        mask_known_sides=mask_known();
        if (host && host->log) host->log(host, "[Mask] Fusion bloqueada: lado %d; IA aprendeu", side);
    }
    return blocked ? 0 : result;
}
/* Three explicit card alternatives, not a new group. Only the field is
 * inspected when publishing a normal ritual recipe. Another mod, if present,
 * may separately propose and validate an arbitrary trio through the public
 * ritual checker. Without it, stock ritual execution needs three on field. */
static int mask_is_named(int id) { return id==102 || id==182 || id==220; }
static int mask_generic(int id, int type)
{
    unsigned stats;
    if (id<=0) return 0;
    stats=(unsigned)gDuel_adwCardStats[id-1];
    return ((stats>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK)==(unsigned)type &&
        (stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1999 &&
        ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1799;
}
static int mask_choose_tribute(void)
{
    int rec[DUEL_FIELD_ROW_SIZE], named[DUEL_FIELD_ROW_SIZE];
    int fiend[DUEL_FIELD_ROW_SIZE], spellcaster[DUEL_FIELD_ROW_SIZE],rank[DUEL_FIELD_ROW_SIZE];
    int count=0, side=D_8009B1D5;
    int i,j,k,best_stat=0x7fffffff,found=0;
    if (side<0 || side>1) return 0;
    for (i=side*15+5;i<side*15+10;++i)
        if (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) {
            rec[count++]=i;
        }
    /* Classification depends only on the candidate card, not on the chosen
     * triple. Calculate it once instead of re-running stat/type tests inside
     * the O(n^3) search. */
    for(i=0;i<count;++i) {
        int id=D_801A7AD8[rec[i]].card_id;
        unsigned st=(id>0)?(unsigned)gDuel_adwCardStats[id-1]:0;
        named[i]=mask_is_named(id);
        fiend[i]=mask_generic(id,CARD_TYPE_FIEND);
        spellcaster[i]=mask_generic(id,CARD_TYPE_SPELLCASTER);
        rank[i]=named[i]?(int)((st&CARD_STAT_VALUE_MASK)*512+((st>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)):0;
    }
    for (i=0;i<count;++i) {
        int id=D_801A7AD8[rec[i]].card_id;
        if (!named[i]) continue;
        for (j=0;j<count;++j) if (j!=i && fiend[j])
            for (k=0;k<count;++k) if (k!=i && k!=j && spellcaster[k]) {
                if (!found || rank[i]<best_stat) {
                    found=1; best_stat=rank[i];
                    mask_materials[0]=id;
                    mask_materials[1]=D_801A7AD8[rec[j]].card_id;
                    mask_materials[2]=D_801A7AD8[rec[k]].card_id;
                }
            }
    }
    return found;
}

static int garma_materials[3];
static int garma_is_named(int id) { return id==VISHWAR_RANDI_ID || id==SUCCUBUS_KNIGHT_ID; }
static int garma_generic(int id, int type)
{
    unsigned stats; if(id<=0) return 0; stats=(unsigned)gDuel_adwCardStats[id-1];
    return ((stats>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK)==(unsigned)type &&
        (stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=2549 &&
        ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=2149;
}
static int garma_choose_tribute(void)
{
    int rec[DUEL_FIELD_ROW_SIZE],named[DUEL_FIELD_ROW_SIZE],warrior[DUEL_FIELD_ROW_SIZE],fiend[DUEL_FIELD_ROW_SIZE];
    int count=0,side=D_8009B1D5,i,j,k;
    if(side<0||side>1)return 0;
    for(i=side*15+5;i<side*15+10;++i)
        if(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)rec[count++]=i;
    for(i=0;i<count;++i){int id=D_801A7AD8[rec[i]].card_id;named[i]=garma_is_named(id);warrior[i]=garma_generic(id,CARD_TYPE_WARRIOR);fiend[i]=garma_generic(id,CARD_TYPE_FIEND);}
    for(i=0;i<count;++i){int id=D_801A7AD8[rec[i]].card_id;if(!named[i])continue;
      for(j=0;j<count;++j)if(j!=i&&warrior[j])
       for(k=0;k<count;++k)if(k!=i&&k!=j&&fiend[k]){garma_materials[0]=id;garma_materials[1]=D_801A7AD8[rec[j]].card_id;garma_materials[2]=D_801A7AD8[rec[k]].card_id;return 1;}}
    return 0;
}

static int data_is_stolen(void *,int);
static int performance_materials[3];
static unsigned compat_oracle_validations;
static int (*compat_lookup_recipe)(int,unsigned short *);
static int (*compat_lookup_requirements)(int,TablesRitualRequirement *,unsigned short *);
static int compat_recipe_prepare(int id, unsigned short row[6]);
static int compat_skull_field_fallback(DuelRitualResult *out);
static int compat_oracle_private_ritual(DuelRitualResult *out,int ritual_id);
static int compat_skull_is_oracle_row(void);
static int compat_requirement_matches(int id,const TablesRitualRequirement *q);
static int performance_generic(int id,int female)
{
    unsigned stats;if(id<=0)return 0;stats=(unsigned)gDuel_adwCardStats[id-1];
    return (stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1949 &&
        ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1849 &&
        (female?Cards_InFusionGroup(id,CARD_FUSION_GROUP_FEMALE):Cards_Type(id)==CARD_TYPE_WARRIOR);
}
static int performance_choose_tribute(void)
{
    int rec[DUEL_FIELD_ROW_SIZE],elf[DUEL_FIELD_ROW_SIZE],warrior[DUEL_FIELD_ROW_SIZE],female[DUEL_FIELD_ROW_SIZE];
    int count=0,side=D_8009B1D5,i,j,k;
    if(side<0||side>1)return 0;
    for(i=side*15+5;i<side*15+10;++i)
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)&&!data_is_stolen(D_801A7AD8[i].data,side))rec[count++]=i;
    for(i=0;i<count;++i){int id=D_801A7AD8[rec[i]].card_id;elf[i]=id==DANCING_ELF_ID;warrior[i]=!elf[i]&&performance_generic(id,0);female[i]=!elf[i]&&performance_generic(id,1);}
    for(i=0;i<count;++i)if(elf[i])
      for(j=0;j<count;++j)if(j!=i&&warrior[j])
       for(k=0;k<count;++k)if(k!=i&&k!=j&&female[k]&&D_801A7AD8[rec[k]].card_id!=D_801A7AD8[rec[j]].card_id){
            performance_materials[0]=DANCING_ELF_ID;performance_materials[1]=D_801A7AD8[rec[j]].card_id;performance_materials[2]=D_801A7AD8[rec[k]].card_id;return 1;}
    return 0;
}

/* Fail closed for modified recipes. A valid field-only assignment is
 * always local. A mixed/Deck assignment can only be accepted after another
 * sourcing MOD re-enters our public oracle with a legal physical trio. */
static int compat_modified_recipe_accepts(const RitualOptionDefinition *option,
    int prepared,const unsigned short row[6],int result,unsigned before,unsigned after)
{
    if(!option || result!=option->monster_id)return 0;
    if(prepared && row[1] && row[2] && row[3])return 1;
    return before!=after;
}

static s32 mask_check_ritual(DuelRitualResult *out, s32 id)
{
    unsigned short row[6];
    s32 result;
    int fallback_used=0;
    int prepared=0;
    unsigned oracle_before=compat_oracle_validations;
    const RitualOptionDefinition *configured=option_by_ritual(id);
    int recipe_on=configured && recipe_settings[(unsigned)(configured-ritual_options)];
    /* A sourcing MOD's public-oracle query is a validation request, NOT a
     * field ritual. Validate its three supplied physical identities directly,
     * before the retail recipe publisher can overwrite the runtime table or
     * force the query through a field-only selector. No Deck/Hand access,
     * consumption, or partner-MOD calls happen here. */
    /* Count potential synthetic probes separately from recognized oracle
     * triplets. This distinguishes a missing hook dispatch from a different
     * external-mod candidate format, without modifying its implementation. */
    if(id==NOVOX_PRAYER_ID && diagnostic_skull_ritual) {
        int first=D_8009B1D5*DUEL_CARD_SIDE_RECORD_COUNT+DUEL_FIELD_ROW_SIZE;
        if(D_8009B1D5<DUEL_SIDE_COUNT &&
           (D_801A7AD8[first].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           (D_801A7AD8[first+1].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           (D_801A7AD8[first+2].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           !(D_801A7AD8[first+3].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           !(D_801A7AD8[first+4].flags&DUEL_CARD_FLAG_OCCUPIED))
            ++skull_ritual_diag.oracle_candidates_seen;
    }
    if(compat_skull_is_oracle_row()) {
        const RitualOptionDefinition *option=option_by_ritual(id);
        if(id==NOVOX_PRAYER_ID && diagnostic_skull_ritual)
            ++skull_ritual_diag.oracle_signature_hits;
        if(option && recipe_settings[(unsigned)(option-ritual_options)]) {
            result=compat_oracle_private_ritual(out,id);
            if(id==NOVOX_PRAYER_ID && diagnostic_skull_ritual) {
                ++skull_ritual_diag.attempts;
                ++skull_ritual_diag.oracle_checks;
                if(result)++skull_ritual_diag.oracle_accepted;
                else ++skull_ritual_diag.oracle_rejected;
                skull_ritual_diag.last_oracle_result=result;
                skull_ritual_diag.stage=result?5:6;
                skull_ritual_diag.retail_result=result;
                skull_ritual_diag.side=D_8009B1D5;
            }
            return result;
        }
    }
    prepared=compat_recipe_prepare(id,row);
    /* Non-oracle check: only a trio selected from real field cards OR a
     * validated nested public-oracle query can accept a modified recipe.
     * Do not accept the old disc recipe as a fallback when the field lacks
     * materials, even if another hook/retail returned a positive result.
     * The optional sourcing MOD itself owns any Hand/Deck selection and its
     * separate minimum/result-card restrictions. */
    result=psycho_check_ritual(out,id);
    if(recipe_on && !compat_modified_recipe_accepts(configured,prepared,row,
           result,oracle_before,compat_oracle_validations))result=0;
    if(id==NOVOX_PRAYER_ID && recipe_settings[16]) {
        /* The fallback is also the public oracle used by the independent
         * Field/Hand/Deck sourcing mod.  Never short-circuit a successful
         * result from the existing chained hooks. */
        /* Preserve MelhoriaDosRituais rejection (minimum tribute sources,
         * missing required result card, etc.) in ordinary/probe calls. */
        if(diagnostic_skull_ritual) {
            TablesRitualRequirement q[3];
            unsigned short expected=0;
            int have_requirements=compat_lookup_requirements &&
                compat_lookup_requirements(id,q,&expected);
            int side=D_8009B1D5, first,j,k;
            ++skull_ritual_diag.attempts;
            skull_ritual_diag.side=D_8009B1D5;
            skull_ritual_diag.phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
            skull_ritual_diag.recipe_on=1;
            skull_ritual_diag.row_found=prepared;
            skull_ritual_diag.retail_result=result;
            skull_ritual_diag.stage=result==SKULL_GUARDIAN_ID ?
                (fallback_used?4:3) : (prepared?2:1);
            skull_ritual_diag.candidates=0;
            for(j=0;j<3;++j) {
                skull_ritual_diag.matches[j]=0;
                skull_ritual_diag.materials[j]=prepared?row[j+1]:0;
            }
            skull_ritual_diag.deck_candidates=0;
            for(j=0;j<3;++j)skull_ritual_diag.deck_matches[j]=0;
            if(have_requirements && side>=0 && side<DUEL_SIDE_COUNT) {
                int start=side*DECK_SIZE+D_800E9FF0[side].deck_draw_cursor;
                int end=(side+1)*DECK_SIZE;
                if(start<side*DECK_SIZE)start=side*DECK_SIZE;
                if(start>end)start=end;
                /* Read-only diagnostics. Sources and consumption remain with
                 * the independent sourcing MOD. */
                for(j=start;j<end;++j) {
                    int card_id=gDuel_aDeckCardRecords[j].id;
                    if(card_id<=0 || card_id>CARD_TABLE_COUNT)continue;
                    ++skull_ritual_diag.deck_candidates;
                    for(k=0;k<3;++k)
                        if(compat_requirement_matches(card_id,&q[k]))
                            ++skull_ritual_diag.deck_matches[k];
                }
                first=side*DUEL_CARD_SIDE_RECORD_COUNT+DUEL_FIELD_ROW_SIZE;
                for(j=first;j<first+DUEL_FIELD_ROW_SIZE;++j) {
                    DuelCardRecord *card=&D_801A7AD8[j];
                    if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
                    ++skull_ritual_diag.candidates;
                    for(k=0;k<3;++k)
                        if(compat_requirement_matches(card->card_id,&q[k]))
                            ++skull_ritual_diag.matches[k];
                }
                for(j=0;j<HAND_SIZE;++j) {
                    int r=D_800907CC[side*HAND_SIZE+j];
                    DuelCardRecord *card;
                    if(D_800E9FF0[side].hand[j]<0 || r<0 || r>=DUEL_CARD_RECORD_COUNT)continue;
                    card=&D_801A7AD8[r];
                    if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
                    ++skull_ritual_diag.candidates;
                    for(k=0;k<3;++k)
                        if(compat_requirement_matches(card->card_id,&q[k]))
                            ++skull_ritual_diag.matches[k];
                }
            }
        }
    }
    return result;
}
