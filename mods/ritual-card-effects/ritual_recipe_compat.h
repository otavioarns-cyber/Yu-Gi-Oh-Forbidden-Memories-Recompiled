/* Publish recipe requirements as regular, field-only ritual materials.
 * This mod never chooses tribute source outside the five active monster zones.
 * Another (optional) mod may propose its own trio using the public ritual
 * checker, but only that mod may choose or consume Hand/Deck sources.
 * Keep the published row after a failed field check so chained hooks can
 * still inspect it. Recompute on every query. */
typedef struct { unsigned short *row, saved[5], published[5]; } CompatRecipeBackup;
static CompatRecipeBackup compat_recipe_backups[24];
static int compat_recipe_backup_count;
/* A sourcing MOD may re-enter the public ritual checker with three physical
 * candidates from Field/Hand/Deck. Only a positively validated candidate
 * combination can authorize a non-field ritual; a retail recipe never can. */
static unsigned compat_oracle_validations;

static void compat_recipe_restore(void)
{
    int i,j;
    /* The engine reuses this memory for the result screen. Restore only
     * while it still contains exactly the recipe we published. */
    for(i=0;i<compat_recipe_backup_count;++i) {
        for(j=0;j<5;++j)if(compat_recipe_backups[i].row[j]!=compat_recipe_backups[i].published[j])break;
        if(j==5)memcpy(compat_recipe_backups[i].row,compat_recipe_backups[i].saved,10);
    }
    compat_recipe_backup_count=0;
    compat_oracle_validations=0;
}

static unsigned short *compat_recipe_row(int id)
{
    int n,i;
    for(n=0;n<256;++n) {
        unsigned short *row=&gDuel_awRitualData[n*DUEL_RITUAL_RECIPE_HALFWORD_COUNT];
        if(!row[0])break;
        if(row[0]!=id)continue;
        for(i=0;i<compat_recipe_backup_count;++i)
            if(compat_recipe_backups[i].row==row)return row;
        if(compat_recipe_backup_count>=24)return 0;
        compat_recipe_backups[compat_recipe_backup_count].row=row;
        memcpy(compat_recipe_backups[compat_recipe_backup_count].saved,row,10);
        memcpy(compat_recipe_backups[compat_recipe_backup_count++].published,row,10);
        return row;
    }
    return 0;
}

static int compat_requirement_matches(int id,const TablesRitualRequirement *q)
{
    unsigned stats;int atk,def;
    if(id<1||id>CARD_TABLE_COUNT||Cards_Type(id)>=CARD_TYPE_MAGIC)return 0;
    stats=(unsigned)gDuel_adwCardStats[id-1];
    atk=(stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    def=((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    return (!q->card||id==q->card||Cards_BaseId(id)==q->card) &&
        (q->type<0||Cards_Type(id)==q->type) &&
        (!q->fusion_group ||
         (q->fusion_group==255 ? RitualCategory_IsSkullMonster(id) :
          q->fusion_group==254 ? (id!=CHAKRA_ID && Cards_BaseId(id)!=CHAKRA_ID) :
          Cards_InFusionGroup(id,q->fusion_group))) &&
        atk>=q->min_attack&&def>=q->min_defense &&
        (q->max_attack<0||atk<=q->max_attack) &&
        (q->max_defense<0||def<=q->max_defense) &&
        (q->min_level<0||Cards_Level(id)>=q->min_level) &&
        (q->max_level<0||Cards_Level(id)<=q->max_level) &&
        (!q->defense_gt_attack||def>atk);
}

static int compat_specificity(const TablesRitualRequirement *q)
{
    return (q->card?100:0)+(q->type>=0)+!!q->fusion_group+
        !!q->min_attack+!!q->min_defense+(q->max_attack>=0)+
        (q->max_defense>=0)+(q->min_level>=0)+(q->max_level>=0)+!!q->defense_gt_attack;
}

static int compat_choose(const TablesRitualRequirement q[3],int result,unsigned short out[3])
{
    int recs[DUEL_FIELD_ROW_SIZE],count=0,side=D_8009B1D5;
    int best[3]={-1,-1,-1},order[3]={0,1,2};
    int i,j,a,b,c,r,prefer_def;
    unsigned stats,matched[3]={0,0,0};
    if(side<0||side>1||result<1||result>CARD_TABLE_COUNT)return 0;
    for(i=0;i<3;++i)for(j=i+1;j<3;++j)
        if(compat_specificity(&q[order[j]])>compat_specificity(&q[order[i]]))
            {r=order[i];order[i]=order[j];order[j]=r;}
    for(i=side*15+5;i<side*15+10;++i)
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)&&D_801A7AD8[i].object &&
           !data_is_stolen(D_801A7AD8[i].data,side))
            recs[count++]=i;
    for(i=0;i<3;++i)for(j=0;j<count;++j)
        if(compat_requirement_matches(D_801A7AD8[recs[j]].card_id,&q[i]))matched[i]|=1u<<j;
    stats=(unsigned)gDuel_adwCardStats[result-1];
    prefer_def=((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)>(stats&CARD_STAT_VALUE_MASK);
    for(a=0;a<count;++a)for(b=0;b<count;++b)for(c=0;c<count;++c) {
        int chosen[3]={a,b,c},better;
        if(a==b||a==c||b==c)continue;
        for(i=0;i<3;++i)if(!(matched[i]&(1u<<chosen[i])))break;
        if(i<3)continue;
        better=best[0]<0;
        for(i=0;i<3&&!better;++i) {
            int x=chosen[order[i]],y=best[order[i]];
            unsigned xs=(unsigned)gDuel_adwCardStats[D_801A7AD8[recs[x]].card_id-1];
            unsigned ys=(unsigned)gDuel_adwCardStats[D_801A7AD8[recs[y]].card_id-1];
            int xa=xs&CARD_STAT_VALUE_MASK,ya=ys&CARD_STAT_VALUE_MASK;
            int xd=(xs>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK,yd=(ys>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK;
            int xp=prefer_def?xd:xa,yp=prefer_def?yd:ya;
            int xt=prefer_def?xa:xd,yt=prefer_def?ya:yd;
            if(xp!=yp){better=xp<yp;break;}if(xt!=yt){better=xt<yt;break;}
            if(x!=y){better=x<y;break;}
        }
        if(better)memcpy(best,chosen,sizeof(best));
    }
    if(best[0]<0)return 0;
    for(i=0;i<3;++i)out[i]=(unsigned short)D_801A7AD8[recs[best[i]]].card_id;
    return 1;
}

static int compat_recipe_prepare(int id,unsigned short out[6])
{
    const RitualOptionDefinition *o=option_by_ritual(id);
    TablesRitualRequirement q[3];unsigned short result=0,*row;
    int i,valid=0;
    if(!o)return 0;
    row=compat_recipe_row(id);if(!row)return 0;
    if(!recipe_settings[(unsigned)(o-ritual_options)]) {
        for(i=0;i<compat_recipe_backup_count;++i)if(compat_recipe_backups[i].row==row)
            {memcpy(row,compat_recipe_backups[i].saved,10);memcpy(compat_recipe_backups[i].published,row,10);}
        return 0;
    }
    memset(out,0,12);out[0]=(unsigned short)id;out[4]=(unsigned short)o->monster_id;
    if(id==GARMA_SWORD_OATH_ID) {
        valid=garma_choose_tribute();if(valid)for(i=0;i<3;++i)out[i+1]=garma_materials[i];
    } else if(id==MASK_RITUAL_ID) {
        valid=mask_choose_tribute();if(valid)for(i=0;i<3;++i)out[i+1]=mask_materials[i];
    } else if(id==PERFORMANCE_RITUAL_ID) {
        valid=performance_choose_tribute();if(valid)for(i=0;i<3;++i)out[i+1]=performance_materials[i];
    } else if(compat_lookup_requirements(id,q,&result)) {
        valid=compat_choose(q,result,&out[1]);out[4]=result;
    } else {
        int ruled=compat_lookup_recipe(id,out);
        /* A modified recipe must never silently fall through to the disc's
         * former tributes. Without a private recipe, publish NO materials. */
        if(ruled<=0){memset(&out[1],0,8);}
        valid=ruled>0;
    }
    /* No valid conditional assignment publishes zero materials, preventing
     * the legacy fallback from accepting an unrelated retail recipe. */
    if(!valid)out[1]=out[2]=out[3]=0;
    memcpy(row,out,10);
    for(i=0;i<compat_recipe_backup_count;++i)if(compat_recipe_backups[i].row==row)
        memcpy(compat_recipe_backups[i].published,row,10);
    return 1;
}

/* Independent oracle fallback for Novox's Prayer.  MelhoriaDosRituais asks the
 * public Duel_CheckRitual hook chain whether a proposed trio is legal; for
 * Hand/Deck candidates it temporarily presents those three *physical* cards
 * as ordinary field records.  Check that same interface directly rather
 * than teaching the sourcing mod our private Skull Monster category.
 *
 * The regular published recipe is still tried first.  This only runs when
 * that path returns no result, for example if the runtime disc row has not
 * been populated or cannot represent our conditional fusion-group #255.
 * It does not touch or consume any field, hand, or deck cards.
 *
 * Crucial: do NOT invoke this as an unconditional fallback after the other
 * mod's ordinary check has returned 0.  That 0 may mean the user enabled
 * "Require Ritual Monster card", minimum Hand/Field tributes, or another
 * restriction.  An unconditional success here would bypass those settings.
 * The public oracle's temporary 3-card layout is recognizable from the
 * non-rendering DisplayObjects it constructs, and is the only context in
 * which the fallback below may supply an independent answer. */
/* Recognize an external source selector's temporary three-card view by
 * ownership of its display objects, not by incidental visual flags. Older
 * MelhoriaDosRituais zeroes update/flags/position, but those are rendering
 * details and may change between releases. What does not change is that its
 * three candidates are backed by private, contiguous, non-pool display
 * objects, with field_6A identifying the synthesized record. Real field
 * objects are allocated from D_800EFE48 and MUST NOT enter this path.
 * This is only a read-only validation interface: no Hand/Deck scan or
 * ownership/consumption logic is introduced into Efeitos de Rituais. */
static int compat_skull_is_oracle_row(void)
{
    int first,i;
    uintptr_t pool_start=(uintptr_t)&D_800EFE48[0];
    uintptr_t pool_end=(uintptr_t)&D_800EFE48[DISPLAY_OBJECT_POOL_CAPACITY];
    uintptr_t first_object=0;
    if(D_8009B1D5>=DUEL_SIDE_COUNT)return 0;
    first=D_8009B1D5*DUEL_CARD_SIDE_RECORD_COUNT+DUEL_FIELD_ROW_SIZE;
    for(i=0;i<3;++i) {
        DuelCardRecord *card=&D_801A7AD8[first+i];
        DisplayObject *obj=(DisplayObject *)card->object;
        uintptr_t pointer=(uintptr_t)obj;
        if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED) || !obj ||
           card->card_id<1 || card->card_id>CARD_TABLE_COUNT ||
           (pointer>=pool_start && pointer<pool_end) ||
           obj->field_6A!=first+i)return 0;
        if(i==0)first_object=pointer;
        else if(pointer!=first_object+(uintptr_t)i*sizeof(DisplayObject))return 0;
    }
    return !(D_801A7AD8[first+3].flags&DUEL_CARD_FLAG_OCCUPIED) &&
           !(D_801A7AD8[first+4].flags&DUEL_CARD_FLAG_OCCUPIED);
}

static int compat_skull_field_fallback(DuelRitualResult *out)
{
    TablesRitualRequirement requirements[3];
    unsigned short result=0;
    int first, a,b,c, slots[3], i;
    if(!compat_private_requirements(NOVOX_PRAYER_ID,requirements,&result) ||
       result!=SKULL_GUARDIAN_ID)return 0;
    if(D_8009B1D5<0 || D_8009B1D5>=DUEL_SIDE_COUNT)return 0;
    first=D_8009B1D5*DUEL_CARD_SIDE_RECORD_COUNT+DUEL_FIELD_ROW_SIZE;
    for(a=0;a<DUEL_FIELD_ROW_SIZE;++a)
        for(b=0;b<DUEL_FIELD_ROW_SIZE;++b)
            for(c=0;c<DUEL_FIELD_ROW_SIZE;++c) {
                if(a==b || a==c || b==c)continue;
                slots[0]=first+a;slots[1]=first+b;slots[2]=first+c;
                for(i=0;i<3;++i) {
                    DuelCardRecord *card=&D_801A7AD8[slots[i]];
                    if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED) || !card->object ||
                       data_is_stolen(card->data,D_8009B1D5) ||
                       !compat_requirement_matches(card->card_id,&requirements[i]))break;
                }
                if(i!=3)continue;
                if(out) {
                    for(i=0;i<3;++i)out->tribute_objects[i]=D_801A7AD8[slots[i]].object;
                    out->field_0C=0;
                }
                return result;
            }
    return 0;
}


/* Public-hook oracle compatibility for Field / Hand / Deck sourcing.
 * An optional sourcing mod may construct three temporary (non-rendering) field records
 * from physical candidates and asks Duel_CheckRitual whether that trio is
 * legal. Our recipe publisher normally picks from Field only; when all
 * materials live in the Deck it must not turn that legitimate oracle probe
 * into a rejected ritual just because there are no live Field/Hand cards.
 *
 * Never call this for an ordinary player/CPU check. In particular, a normal
 * zero return from the sourcing mod must NOT bypass its result-card or source
 * minima settings. The oracle-only synthetic record check is the boundary.
 * Neither this path nor its caller allocates, destroys or consumes cards. */
static int compat_oracle_private_ritual(DuelRitualResult *out,int ritual_id)
{
    const RitualOptionDefinition *option=option_by_ritual(ritual_id);
    TablesRitualRequirement q[3];
    unsigned short result=0, fixed[6];
    int conditional=0, fixed_recipe=0, a,b,c,i,chosen[3],first;
    int ids[3];

    if(!option || !recipe_settings[(unsigned)(option-ritual_options)] ||
       !compat_skull_is_oracle_row())return 0;
    if(D_8009B1D5<0 || D_8009B1D5>=DUEL_SIDE_COUNT)return 0;
    first=D_8009B1D5*DUEL_CARD_SIDE_RECORD_COUNT+DUEL_FIELD_ROW_SIZE;
    for(i=0;i<3;++i) {
        DuelCardRecord *card=&D_801A7AD8[first+i];
        if(!(card->flags&DUEL_CARD_FLAG_OCCUPIED) || !card->object ||
           data_is_stolen(card->data,D_8009B1D5))return 0;
        ids[i]=card->card_id;
        if(ritual_id==NOVOX_PRAYER_ID && diagnostic_skull_ritual)
            skull_ritual_diag.oracle_last_trio[i]=(unsigned short)ids[i];
    }
    if(compat_private_requirements(ritual_id,q,&result))conditional=1;
    else if(compat_private_recipe(ritual_id,fixed)>0) {
        result=fixed[4];
        fixed_recipe=1;
    } else if(ritual_id==MASK_RITUAL_ID || ritual_id==GARMA_SWORD_OATH_ID ||
            ritual_id==PERFORMANCE_RITUAL_ID)result=(unsigned short)option->monster_id;
    else return 0;
    if(result<1 || result>CARD_TABLE_COUNT)return 0;
    if(ritual_id==NOVOX_PRAYER_ID && diagnostic_skull_ritual) {
        for(i=0;i<3;++i) {
            int j;
            skull_ritual_diag.oracle_requirement_masks[i]=0;
            if(conditional)for(j=0;j<3;++j)
                if(compat_requirement_matches(ids[i],&q[j]))
                    skull_ritual_diag.oracle_requirement_masks[i]|=(unsigned char)(1u<<j);
        }
    }

    for(a=0;a<3;++a)for(b=0;b<3;++b)for(c=0;c<3;++c) {
        int valid=0;
        if(a==b||a==c||b==c)continue;
        if(conditional) {
            valid=compat_requirement_matches(ids[a],&q[0]) &&
                  compat_requirement_matches(ids[b],&q[1]) &&
                  compat_requirement_matches(ids[c],&q[2]);
        } else if(fixed_recipe) {
            valid=(ids[a]==fixed[1]||Cards_BaseId(ids[a])==fixed[1]) &&
                  (ids[b]==fixed[2]||Cards_BaseId(ids[b])==fixed[2]) &&
                  (ids[c]==fixed[3]||Cards_BaseId(ids[c])==fixed[3]);
        } else if(ritual_id==MASK_RITUAL_ID) {
            valid=mask_is_named(ids[a]) &&
                  mask_generic(ids[b],CARD_TYPE_FIEND) &&
                  mask_generic(ids[c],CARD_TYPE_SPELLCASTER);
        } else if(ritual_id==GARMA_SWORD_OATH_ID) {
            valid=garma_is_named(ids[a]) &&
                  garma_generic(ids[b],CARD_TYPE_WARRIOR) &&
                  garma_generic(ids[c],CARD_TYPE_FIEND);
        } else if(ritual_id==PERFORMANCE_RITUAL_ID) {
            valid=ids[a]==DANCING_ELF_ID &&
                  ids[b]!=DANCING_ELF_ID && ids[c]!=DANCING_ELF_ID &&
                  ids[b]!=ids[c] &&
                  performance_generic(ids[b],0) &&
                  performance_generic(ids[c],1);
        }
        if(!valid)continue;
        chosen[0]=a;chosen[1]=b;chosen[2]=c;
        if(out) {
            for(i=0;i<3;++i)
                out->tribute_objects[i]=D_801A7AD8[first+chosen[i]].object;
            out->field_0C=0;
        }
        ++compat_oracle_validations;
        return result;
    }
    return 0;
}
