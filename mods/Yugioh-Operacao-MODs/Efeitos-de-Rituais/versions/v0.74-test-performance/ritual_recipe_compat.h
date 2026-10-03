/* Publish exact materials in the public AND disc-style tables. The unchanged
 * hand-tribute mod owns proxy creation/consumption. Keep the row published
 * after a failed field-only check: that mod may be the outer hook and read
 * the row only after our checker has returned. Recompute on every query. */
typedef struct { unsigned short *row, saved[5], published[5]; } CompatRecipeBackup;
static CompatRecipeBackup compat_recipe_backups[16];
static int compat_recipe_backup_count;

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
        if(compat_recipe_backup_count>=16)return 0;
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
        (!q->fusion_group||Cards_InFusionGroup(id,q->fusion_group)) &&
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
    int recs[10],hands[10],count=0,side=D_8009B1D5;
    int best[3]={-1,-1,-1},order[3]={0,1,2},best_hands=4;
    int i,j,a,b,c,r,prefer_def;
    unsigned stats,matched[3]={0,0,0};
    if(side<0||side>1||result<1||result>CARD_TABLE_COUNT)return 0;
    for(i=0;i<3;++i)for(j=i+1;j<3;++j)
        if(compat_specificity(&q[order[j]])>compat_specificity(&q[order[i]]))
            {r=order[i];order[i]=order[j];order[j]=r;}
    for(i=side*15+5;i<side*15+10;++i)
        if((D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)&&D_801A7AD8[i].object &&
           !data_is_stolen(D_801A7AD8[i].data,side))
            {recs[count]=i;hands[count++]=0;}
    for(i=0;i<HAND_SIZE;++i) {
        r=D_800907CC[side*HAND_SIZE+i];
        if(D_800E9FF0[side].hand[i]<0||r<0||r>=DUEL_CARD_RECORD_COUNT)continue;
        for(j=0;j<count&&recs[j]!=r;++j){} if(j<count)continue;
        recs[count]=r;hands[count++]=1;
    }
    for(i=0;i<3;++i)for(j=0;j<count;++j)
        if(compat_requirement_matches(D_801A7AD8[recs[j]].card_id,&q[i]))matched[i]|=1u<<j;
    stats=(unsigned)gDuel_adwCardStats[result-1];
    prefer_def=((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)>(stats&CARD_STAT_VALUE_MASK);
    for(a=0;a<count;++a)for(b=0;b<count;++b)for(c=0;c<count;++c) {
        int chosen[3]={a,b,c},h=hands[a]+hands[b]+hands[c],better;
        if(a==b||a==c||b==c||h==3||h>best_hands)continue;
        for(i=0;i<3;++i)if(!(matched[i]&(1u<<chosen[i])))break;
        if(i<3)continue;
        better=best[0]<0||h<best_hands;
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
        if(better){memcpy(best,chosen,sizeof(best));best_hands=h;}
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
    if(!recipe_enabled(o->recipe_key)) {
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
        if(ruled<0) {
            for(i=0;i<compat_recipe_backup_count;++i)if(compat_recipe_backups[i].row==row)
                memcpy(out,compat_recipe_backups[i].saved,10);
        } else if(!ruled){memset(&out[1],0,8);}
        valid=ruled!=0;
    }
    /* No valid conditional assignment publishes zero materials, preventing
     * the legacy fallback from accepting an unrelated retail recipe. */
    if(!valid)out[1]=out[2]=out[3]=0;
    memcpy(row,out,10);
    for(i=0;i<compat_recipe_backup_count;++i)if(compat_recipe_backups[i].row==row)
        memcpy(compat_recipe_backups[i].published,row,10);
    return 1;
}
