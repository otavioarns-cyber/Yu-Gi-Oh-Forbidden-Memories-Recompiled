/* Private extension of the v0.39 mod, not an engine/framework patch.
 * Identity is the deck-record pointer, never the card id or field slot.
 * Planning only consults learned knowledge; placement records discovery. */
#define MASK_ID 720
#define MASK_RITUAL_ID 693
typedef struct { void *data; unsigned learned; } MaskInstance;
static MaskInstance mask_instances[10];
static int mask_inside_placement;
static unsigned mask_blocked, mask_learned, mask_released, mask_summons;
static int mask_known_sides;
static int mask_materials[3];
static s32 psycho_check_ritual(DuelRitualResult *, s32);

static int mask_slot_for(void *data)
{
    int i;
    for (i=0; i<DUEL_CARD_RECORD_COUNT; ++i)
        if (monster_slot(i) && D_801A7AD8[i].data == data &&
            D_801A7AD8[i].card_id == MASK_ID &&
            (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED)) return i;
    return -1;
}
static void mask_forget(void *data)
{
    int i;
    for (i=0; i<10; ++i) if (mask_instances[i].data == data) {
        mask_instances[i].data=0;
        mask_instances[i].learned=0;
    }
}
static void mask_summon(DuelCardRecord *card)
{
    int i;
    if (card->card_id != MASK_ID || !card->data ||
        !(card->flags & DUEL_CARD_FLAG_OCCUPIED)) return;
    mask_forget(card->data);
    for (i=0; i<10; ++i) if (!mask_instances[i].data) {
        mask_instances[i].data=card->data;
        mask_instances[i].learned=0;
        ++mask_summons;
        return;
    }
}
static int mask_known(void)
{
    int i, slot, sides=0;
    for (i=0; i<10; ++i) if (mask_instances[i].data) {
        slot=mask_slot_for(mask_instances[i].data);
        if (slot>=0) sides |= mask_instances[i].learned & (1u << (1-side_for_record(slot)));
    }
    return sides;
}
/* Run only at stable field boundaries, never while battle hides records. */
static void mask_reconcile(void)
{
    int i, now;
    for (i=0; i<10; ++i)
        if (mask_instances[i].data && mask_slot_for(mask_instances[i].data)<0)
            mask_forget(mask_instances[i].data);
    now=mask_known();
    if (mask_known_sides & ~now) ++mask_released;
    mask_known_sides=now;
}
static s32 mask_filter_fusion(s32 result)
{
    int i, slot, side=D_8009B1D5, blocked=0, visual_slot=-1;
    if (!result || side<0 || side>1 || !ritual_card_effect_enabled(MASK_ID)) return result;
    for (i=0; i<10; ++i) if (mask_instances[i].data) {
        slot=mask_slot_for(mask_instances[i].data);
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
/* Three explicit card alternatives, not a new group. Choose a distinct
 * triple, preferring field tributes. Pass its IDs through the normal ritual
 * checker under a scoped retail recipe substitution; restore before return.
 * Manifest after=[melhoria-dos-rituais] keeps that optional mod INSIDE this
 * hook. Its own existing bridge handles hand materials, without any edits.
 * Without that mod, the stock checker still requires all three on field. */
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
    int rec[10], hands[10], count=0, side=D_8009B1D5;
    int i,j,k,r,best_hands=4,best_stat=0x7fffffff;
    if (side<0 || side>1) return 0;
    for (i=side*15+5;i<side*15+10;++i)
        if (D_801A7AD8[i].flags & DUEL_CARD_FLAG_OCCUPIED) {
            rec[count]=i; hands[count++]=0;
        }
    for (i=0;i<5;++i) {
        r=D_800907CC[side*5+i];
        if (D_800E9FF0[side].hand[i]<0 || r<0 || r>=DUEL_CARD_RECORD_COUNT) continue;
        for (j=0;j<count && rec[j]!=r;++j) {}
        if (j<count) continue;
        rec[count]=r; hands[count++]=1;
    }
    for (i=0;i<count;++i) {
        int id=D_801A7AD8[rec[i]].card_id;
        if (!mask_is_named(id)) continue;
        for (j=0;j<count;++j) if (j!=i && mask_generic(D_801A7AD8[rec[j]].card_id,CARD_TYPE_FIEND))
            for (k=0;k<count;++k) if (k!=i && k!=j && mask_generic(D_801A7AD8[rec[k]].card_id,CARD_TYPE_SPELLCASTER)) {
                int h=hands[i]+hands[j]+hands[k];
                unsigned s=(unsigned)gDuel_adwCardStats[id-1];
                int rank=(int)((s&CARD_STAT_VALUE_MASK)*512+((s>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK));
                if (h<3 && (h<best_hands || (h==best_hands && rank<best_stat))) {
                    best_hands=h; best_stat=rank;
                    mask_materials[0]=id;
                    mask_materials[1]=D_801A7AD8[rec[j]].card_id;
                    mask_materials[2]=D_801A7AD8[rec[k]].card_id;
                }
            }
    }
    return best_hands<3;
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
    int rec[10],hands[10],count=0,side=D_8009B1D5,i,j,k,r,best_hands=4;
    if(side<0||side>1)return 0;
    for(i=side*15+5;i<side*15+10;++i) if(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED){rec[count]=i;hands[count++]=0;}
    for(i=0;i<5;++i){r=D_800907CC[side*5+i]; if(D_800E9FF0[side].hand[i]<0||r<0||r>=DUEL_CARD_RECORD_COUNT)continue; for(j=0;j<count&&rec[j]!=r;++j){} if(j<count)continue; rec[count]=r;hands[count++]=1;}
    for(i=0;i<count;++i){int id=D_801A7AD8[rec[i]].card_id;if(!garma_is_named(id))continue;
      for(j=0;j<count;++j)if(j!=i&&garma_generic(D_801A7AD8[rec[j]].card_id,CARD_TYPE_WARRIOR))
       for(k=0;k<count;++k)if(k!=i&&k!=j&&garma_generic(D_801A7AD8[rec[k]].card_id,CARD_TYPE_FIEND)){int hh=hands[i]+hands[j]+hands[k];if(hh<3&&hh<best_hands){best_hands=hh;garma_materials[0]=id;garma_materials[1]=D_801A7AD8[rec[j]].card_id;garma_materials[2]=D_801A7AD8[rec[k]].card_id;}}}
    return best_hands<3;
}

static int data_is_stolen(void *,int);
static int performance_materials[3];
static int (*compat_lookup_recipe)(int,unsigned short *);
static int (*compat_lookup_requirements)(int,TablesRitualRequirement *,unsigned short *);
static int compat_recipe_prepare(int id, unsigned short row[6]);
static int performance_generic(int id,int female)
{
    unsigned stats;if(id<=0)return 0;stats=(unsigned)gDuel_adwCardStats[id-1];
    return (stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1949 &&
        ((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE<=1849 &&
        (female?Cards_InFusionGroup(id,CARD_FUSION_GROUP_FEMALE):Cards_Type(id)==CARD_TYPE_WARRIOR);
}
static int performance_choose_tribute(void)
{
    int rec[10],hands[10],count=0,side=D_8009B1D5,i,j,k,r,best=4;
    if(side<0||side>1)return 0;
    for(i=side*15+5;i<side*15+10;++i)if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)&&!data_is_stolen(D_801A7AD8[i].data,side)){rec[count]=i;hands[count++]=0;}
    for(i=0;i<5;++i){r=D_800907CC[side*5+i];if(D_800E9FF0[side].hand[i]<0||r<0||r>=DUEL_CARD_RECORD_COUNT)continue;
        for(j=0;j<count&&rec[j]!=r;++j){}if(j<count)continue;rec[count]=r;hands[count++]=1;}
    for(i=0;i<count;++i)if(D_801A7AD8[rec[i]].card_id==DANCING_ELF_ID)
      for(j=0;j<count;++j)if(j!=i&&D_801A7AD8[rec[j]].card_id!=DANCING_ELF_ID&&performance_generic(D_801A7AD8[rec[j]].card_id,0))
       for(k=0;k<count;++k)if(k!=i&&k!=j&&D_801A7AD8[rec[k]].card_id!=DANCING_ELF_ID&&
         D_801A7AD8[rec[k]].card_id!=D_801A7AD8[rec[j]].card_id&&performance_generic(D_801A7AD8[rec[k]].card_id,1)){
            int h=hands[i]+hands[j]+hands[k];if(h<3&&h<best){best=h;performance_materials[0]=DANCING_ELF_ID;performance_materials[1]=D_801A7AD8[rec[j]].card_id;performance_materials[2]=D_801A7AD8[rec[k]].card_id;}}
    return best<3;
}

static s32 mask_check_ritual(DuelRitualResult *out, s32 id)
{
    unsigned short row[6];
    compat_recipe_prepare(id,row);
    return psycho_check_ritual(out,id);
}
