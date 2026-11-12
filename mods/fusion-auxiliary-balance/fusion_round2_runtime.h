/* Round 02. Eligibility always belongs to a physical card instance.
 * Temporary arithmetic is derived, never written into lasting modifiers. */
typedef struct {
    int active,prepared,resolved,negated,pending,settle,in_resolution,siege,cost_paid;
    int slot[2],id[2],eligible[2],death[2],shark,eagle_penalty;
    void *data[2];
} R2Battle;
static R2Battle r2_battle;
static DuelDeckCardRecord r2_zombie_data[30];
/* A generated monster owns its synthetic deck record for its whole lifetime.
   Other native/3D presentation paths may reconstruct a field card later,
   even though it was never part of either physical deck. */
static void *r2_zombie_lease[30];
static void aux_queue_stat_effect(int record,int amount);
static s32 (*r2_original_compare)(DisplayObject*,DisplayObject*);
static int aux_darkfire_card_thumbnail(int,int);
static int r2_original_atk(int id)
{ return Cards_Valid(id)?((unsigned)gDuel_adwCardStats[id-1]&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE:0; }
static int r2_original_def(int id)
{ return Cards_Valid(id)?(((unsigned)gDuel_adwCardStats[id-1]>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE:0; }
static int r2_identity(int slot,int part)
{
    return aux_monster_slot(slot)&&slot==r2_battle.slot[part]&&
        D_801A7AD8[slot].card_id==r2_battle.id[part]&&D_801A7AD8[slot].data==r2_battle.data[part];
}
static int r2_origin(int slot)
{
    DuelCardRecord *c;
    if(!aux_monster_slot(slot))return 0;
    c=&D_801A7AD8[slot];
    return r2_setting_enabled(c->card_id) &&
        ((fusion_origin[slot]&&fusion_bound_id[slot]==c->card_id&&fusion_bound_data[slot]==c->data)||
         aux_unlock_lease(slot)||aux_ignore_conditions());
}
static int r2_effect_source(int slot,int id)
{
    DuelCardRecord *c;
    if(!aux_monster_slot(slot))return 0;
    c=&D_801A7AD8[slot];
    if(c->card_id!=id || (c->flags&DUEL_CARD_FLAG_FACE_DOWN))return 0;
    if(c->flags&DUEL_CARD_FLAG_OCCUPIED)return aux_fusion_special(slot,0);
    return r2_battle.active && (r2_identity(slot,0)||r2_identity(slot,1)) &&
        (r2_member(id)?r2_origin(slot):aux_fusion_special(slot,1));
}
static int r2_continuous_bonus(int slot)
{
    int i,bonus=0,id;
    if(!aux_monster_slot(slot))return 0;
    id=D_801A7AD8[slot].card_id;
    if(id==509 && gDuel_bTerrain[0]==1 && r2_effect_source(slot,509))bonus+=800;
    if(Cards_Attribute(id)==Cards_AttributeNamed("Earth")) {
        for(i=slot/15*15+5;i<slot/15*15+10;i++)
            if(i!=slot&&r2_effect_source(i,531))bonus+=200;
    }
    return bonus;
}
static s32 r2_stats(DuelCardRecord *c,s32 packed)
{
    uintptr_t address=(uintptr_t)c,base=(uintptr_t)D_801A7AD8;
    int slot,at=(u16)packed,df=(unsigned)packed>>16;
    if(address<base || address>=base+sizeof(D_801A7AD8[0])*30 ||
       (address-base)%sizeof(D_801A7AD8[0]))return packed;
    slot=(int)((address-base)/sizeof(D_801A7AD8[0]));
    int bonus,cap_at=Tables_StatCap(0),cap_df=Tables_StatCap(1);
    if(!aux_monster_slot(slot))return packed;
    bonus=r2_continuous_bonus(slot);
    if(bonus) {
        /* Preserve the native zero floor after adding our continuous layer,
         * rather than turning -100 + 200 into 200 by clamping too early. */
        int raw_at=c->attack+c->stat_modifier+c->terrain_modifier;
        int raw_df=c->defense+c->stat_modifier+c->defense_modifier+c->terrain_modifier;
        if(!at&&raw_at<0)at=raw_at;
        if(!df&&raw_df<0)df=raw_df;
        at=r2_limit(at+bonus,cap_at);df=r2_limit(df+bonus,cap_df);
    }
    if(r2_battle.active&&r2_battle.prepared&&!r2_battle.negated) {
        if(r2_identity(slot,0)) {
            int i,penalty=0,d=r2_battle.slot[1];
            if(aux_monster_slot(d))for(i=d/15*15+5;i<d/15*15+10;i++)
                if(r2_adjacent(i,d)&&r2_effect_source(i,465))penalty+=400;
            at-=penalty;df-=penalty;
            if(r2_battle.shark&&r2_origin(slot))at+=1000;
        }
        if(r2_identity(slot,1)&&r2_battle.eligible[0]&&r2_origin(r2_battle.slot[0])) {
            if(r2_battle.id[0]==385){int swap=at;at=df;df=swap;}
            if(r2_battle.id[0]==69 && (D_8009B178[1]&DUEL_CARD_FLAG_DEFENSE_POSITION))df-=300;
        }
        /* Crimson Sunbird: apply the -200 ATK/DEF DURING battle so the
           combat calculation and native stat tween see it. The surviving
           victim's real modifier is decreased exactly once in r2_post_battle,
           after the temporary battle layer has been removed. */
        if(r2_battle.prepared&&!r2_battle.negated&&r2_identity(slot,1)&&
           r2_battle.eligible[0]&&r2_battle.id[0]==467){
            at-=200;df-=200;
        }
    }
    at=r2_limit(at,cap_at);df=r2_limit(df,cap_df);
    return (s32)(((unsigned)df<<16)|(unsigned)at);
}
static int r2_guard(int slot)
{
    int part,other,id,i,side;
    if(!r2_battle.active||r2_battle.negated||!aux_monster_slot(slot))return 0;
    part=r2_identity(slot,0)?0:r2_identity(slot,1)?1:-1;
    if(part<0)return 0;
    other=r2_battle.slot[part^1];id=r2_battle.id[part];side=slot/15;
    if(id==427&&r2_battle.eligible[part]&&r2_origin(slot)) {
        for(i=side*15+5;i<side*15+10;i++)
            if(i!=slot&&(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)&&
               D_801A7AD8[i].card_id!=427&&Cards_Type(D_801A7AD8[i].card_id)==CARD_TYPE_DRAGON)return 1;
    }
    if(id==509&&r2_battle.eligible[part]&&r2_origin(slot)&&gDuel_bTerrain[0]==1&&
       aux_monster_slot(other)&&Cards_Type(r2_battle.id[part^1])==CARD_TYPE_INSECT)return 1;
    if(Cards_InFusionGroup(id,CARD_FUSION_GROUP_FEMALE)) {
        for(i=side*15+5;i<side*15+10;i++)if(i!=slot&&r2_effect_source(i,572))return 1;
    }
    return 0;
}
static void r2_begin(void)
{
    int p;
    memset(&r2_battle,0,sizeof(r2_battle));r2_battle.active=1;
    for(p=0;p<2;p++) {
        r2_battle.slot[p]=battle_record[p];r2_battle.id[p]=battle_card[p];r2_battle.data[p]=battle_saved_data[p];
        r2_battle.eligible[p]=aux_monster_slot(battle_record[p])&&r2_member(battle_card[p])&&r2_origin(battle_record[p]);
    }
    for(p=0;p<2;p++)if(r2_guard(battle_record[p]))battle_guarded[p]=1;
}
static void r2_prepare(int was)
{
    int a=r2_battle.slot[0],d=r2_battle.slot[1],i,id;
    if(!r2_battle.active)return;
    if(D_8009B22A){r2_battle.negated=1;return;}
    if(was!=2||r2_battle.prepared||!r2_identity(a,0)||!r2_identity(d,1))return;
    id=r2_battle.id[0];
    /* Snapshot the target's attack BEFORE any battle-only changes. */
    if(id==565&&r2_battle.eligible[0])
        r2_battle.shark=r2_shark(1,(u16)Duel_CalcCardStats(&D_801A7AD8[d]),r2_original_atk(r2_battle.id[1]));
    for(i=d/15*15+5;i<d/15*15+10;i++)
        if(r2_adjacent(i,d)&&r2_effect_source(i,465))r2_battle.eagle_penalty+=400;
    if(id==223&&r2_battle.eligible[0]&&(D_8009B178[1]&DUEL_CARD_FLAG_DEFENSE_POSITION)) {
        D_801A7AD8[d].flags&=~DUEL_CARD_FLAG_DEFENSE_POSITION;
        D_8009B178[1]&=~DUEL_CARD_FLAG_DEFENSE_POSITION;
        battle_saved_flags[1]&=~DUEL_CARD_FLAG_DEFENSE_POSITION;
    }
    r2_battle.prepared=1;
}
static s32 r2_compare(DisplayObject *left,DisplayObject *right)
{
    int a=r2_battle.slot[0],d=r2_battle.slot[1];
    if(r2_battle.active&&r2_battle.in_resolution&&!D_8009B22A&&left&&right&&
       left->field_6A==a&&right->field_6A==d&&r2_battle.id[0]==370&&
       r2_battle.eligible[0]&&r2_identity(a,0)&&r2_identity(d,1)&&
       r2_siege(1,(D_8009B178[1]&DUEL_CARD_FLAG_DEFENSE_POSITION)!=0,
          Duel_CalcBattleAttack(&D_801A7AD8[a],&D_801A7AD8[d]),
          (unsigned)Duel_CalcCardStats(&D_801A7AD8[d])>>16)) {
        r2_battle.siege=1;
        return 1; /* Native defended-loss path: defender destruction, no normal LP damage. */
    }
    return r2_original_compare(left,right);
}
static void r2_after_battle_step(int was)
{
    int p;
    r2_battle.in_resolution=0;
    if(!r2_battle.active)return;
    if(D_8009B22A)r2_battle.negated=1;
    if(was==3&&(D_8009B174&15)!=3&&!r2_battle.negated) {
        r2_battle.resolved=1;
        if(r2_battle.siege&&!r2_battle.cost_paid) {
            int side=r2_battle.slot[0]/15,life=D_800E9FF0[side].life_points.unsigned_value;
            int after=r2_loss_cost(life);
            r2_battle.cost_paid=1;
            D_800E9FF0[side].life_points.unsigned_value=(u16)after;
            D_8009B1A4[0]=-(life-after); /* Presentation only: LP already paid exactly once. */
        }
    }
    if(r2_battle.resolved&&!r2_battle.negated&&(was==3||was==5))
        for(p=0;p<2;p++)if(D_8009B1B0[p]<0)r2_battle.death[p]=1;
}
static void r2_end(void)
{
    if(!r2_battle.active)return;
    r2_battle.active=0;r2_battle.pending=r2_battle.resolved&&!r2_battle.negated;r2_battle.settle=0;
}
static int r2_survived(int part)
{
    int s=r2_battle.slot[part];
    return r2_identity(s,part)&&(D_801A7AD8[s].flags&DUEL_CARD_FLAG_OCCUPIED);
}
static void r2_permanent_debuff(int slot,int amount)
{
    DuelCardRecord *c;
    if(!aux_monster_slot(slot)||!(D_801A7AD8[slot].flags&DUEL_CARD_FLAG_OCCUPIED))return;
    c=&D_801A7AD8[slot];c->stat_modifier=(s16)r2_lower_modifier(c->stat_modifier,amount);
}
static int r2_can_defend(int slot)
{
    typedef struct {unsigned abi;int (*can_defend)(int);} PositionRulesV1;
    const PositionRulesV1 *rules;
    if(!aux_monster_slot(slot)||!(D_801A7AD8[slot].flags&DUEL_CARD_FLAG_OCCUPIED)||
       (D_801A7AD8[slot].flags&DUEL_CARD_FLAG_DEFENSE_POSITION))return 0;
    if(D_801A7AD8[slot].card_id==AUX_MUSICIAN_KING&&aux_fusion_special(slot,0))return 0;
    rules=aux_host&&aux_host->find?aux_host->find(aux_host,"ritual-card-effects-test:position-rules-v1"):0;
    return !rules||rules->abi!=1||!rules->can_defend||rules->can_defend(slot);
}
static unsigned r2_draw(unsigned n)
{
    unsigned v,limit=(unsigned)RAND_MAX-((unsigned)RAND_MAX%n);
    do{v=(unsigned)rand();}while(v>=limit);return v%n;
}
static void r2_summon_zombie(int side)
{
    int i;
    for(i=side*15+5;i<side*15+10;i++)if(!(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)) {
        DuelCardRecord *c=&D_801A7AD8[i];DuelCardDisplayObject *obj;
        int pool,j;
        for(pool=0;pool<30;pool++) {
            for(j=0;j<30;j++)if((D_801A7AD8[j].flags&DUEL_CARD_FLAG_OCCUPIED)&&
                D_801A7AD8[j].data==&r2_zombie_data[pool])break;
            if(j==30)break;
        }
        if(pool==30)return;
        /* Fresh effect summon; never inherit the old occupant's origin/lease. */
        aux_unlock_revoke(i);fusion_origin[i]=metal_immune[i]=0;
        fusion_bound_id[i]=metal_bound_id[i]=0;fusion_bound_data[i]=metal_bound_data[i]=0;
        aux_elf_bound_id[i]=0;aux_elf_healed[i]=0;aux_elf_bound_data[i]=0;
        memset(&r2_zombie_data[pool],0,sizeof(r2_zombie_data[pool]));
        r2_zombie_data[pool].id=30;
        /* 0 is a safe placeholder for native battle restore. The restore
           hook ALWAYS reinstates this card; -1 becomes 0xFF in card objects
           and can select a non-existent source in the 3D battle return. */
        r2_zombie_data[pool].deck_index=0;
        r2_zombie_data[pool].data_block_index=(u8)i;
        memset(c,0,sizeof(*c));c->data=&r2_zombie_data[pool];c->card_id=30;c->table_index=(u8)i;
        c->attack=(s16)r2_original_atk(30);c->defense=(s16)r2_original_def(30);
        c->terrain_modifier=(s16)Duel_GetTerrainBoost(Cards_Type(30));
        c->flags=DUEL_CARD_FLAG_OCCUPIED;
        obj=func_80024C1C(30,D_800908A0[i].x,D_800908A0[i].y);c->object=obj;if(obj)obj->card_index=(u8)i;
        r2_zombie_lease[i]=c->data;
        aux_darkfire_card_thumbnail(i,30);
        return;
    }
}
/* Repair only the SAME synthetic instance. Never recreate a zombie after
   destruction, fusion or replacement by a normal hand card. Do not wipe
   legitimate Equip/field/stat effects when the card itself is still sound. */
static void r2_reconcile_zombies(int refresh_turn)
{
    int i;
    for(i=0;i<30;i++)if(r2_zombie_lease[i]) {
        DuelCardRecord *c=&D_801A7AD8[i];
        DuelDeckCardRecord *data=(DuelDeckCardRecord *)r2_zombie_lease[i];
        int corrupted,redraw;
        DuelCardDisplayObject *obj;
        if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED)||c->data!=data) {
            int phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
            /* Battle temporarily lifts both records; keep the lease until
               the native resume/3D restore can repopulate their slots. */
            if(c->data==data&&(phase==9||phase==11))continue;
            r2_zombie_lease[i]=0;
            continue;
        }
        corrupted=c->card_id!=30 || data->id!=30;
        redraw=refresh_turn||corrupted||!c->object ||
               ((DuelCardDisplayObject *)c->object)->card_index!=i;
        if(corrupted) {
            c->card_id=30;
            data->id=30;
            c->attack=(s16)r2_original_atk(30);
            c->defense=(s16)r2_original_def(30);
            c->stat_modifier=0;
            c->defense_modifier=0;
            c->terrain_modifier=(s16)Duel_GetTerrainBoost(Cards_Type(30));
        }
        if(!redraw)continue;
        if(c->object)DisplayObject_ReleaseIfPresent(c->object);
        c->object=0;
        obj=func_80024C1C(30,D_800908A0[i].x,D_800908A0[i].y);
        c->object=obj;
        if(obj) {
            obj->card_index=(u8)i;
            Duel_ApplyCardObjectFlags(obj);
        }
        /* Native deck thumbnails at data_block_index are NOT the generated
           card's art. Reupload the right WA_MRG block after phase/turn redraw. */
        aux_darkfire_card_thumbnail(i,30);
    }
}
static void r2_full_reset(void)
{
    memset(r2_zombie_lease,0,sizeof(r2_zombie_lease));
}
static void r2_heal(int side,int amount)
{
    int before=D_800E9FF0[side].life_points.unsigned_value,cap=D_800E9FF0[side].max_life_points,after;
    if(!before)return; /* Never revive a lost duel. */
    if(cap<before)cap=before;
    after=r2_limit(before+amount,cap);D_800E9FF0[side].life_points.unsigned_value=(u16)after;
    aux_dark_elf_react_to_healing(side,after-before);
}
static void r2_post_battle(void)
{
    int a=r2_battle.slot[0],d=r2_battle.slot[1],id=r2_battle.id[0],i;
    int alive[2]={r2_survived(0),r2_survived(1)},dead[2];
    dead[0]=r2_battle.death[0]&&!alive[0];dead[1]=r2_battle.death[1]&&!alive[1];
    r2_battle.pending=0;
    if(!aux_monster_slot(a)||!aux_monster_slot(d))return;
    if(r2_battle.eligible[0]&&r2_setting_enabled(id)&&(!alive[0]||aux_fusion_special(a,0))) {
        if(dead[1]) {
            if(id==613)for(i=d-1;i<=d+1;i+=2)if(aux_monster_slot(i)&&i/15==d/15&&
                (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)) {
                r2_permanent_debuff(i,200);
                aux_queue_stat_effect(i,200);
            }
            if(id==707)r2_summon_zombie(a/15);
            if(id==471)r2_heal(a/15,r2_original_atk(r2_battle.id[1])/2);
            if(id==442) {
                int candidates[5],count=0;
                for(i=d/15*15+5;i<d/15*15+10;i++)if(i!=d&&r2_can_defend(i))candidates[count++]=i;
                if(count) {
                    DuelCardRecord *c=&D_801A7AD8[candidates[r2_draw((unsigned)count)]];
                    c->flags|=DUEL_CARD_FLAG_DEFENSE_POSITION;
                    if(c->object)Duel_ApplyCardObjectFlags((DuelCardDisplayObject*)c->object);
                }
            }
        }
        if(id==467&&alive[1])r2_permanent_debuff(d,200);
    }
    for(i=0;i<2;i++)if(dead[i]&&alive[i^1]&&r2_battle.eligible[i]&&
        r2_battle.id[i]==564&&r2_setting_enabled(564))r2_permanent_debuff(r2_battle.slot[i^1],500);
}
static void r2_stable_tick(void)
{
    int phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
    if(!r2_battle.pending)return;
    if(phase!=4&&phase!=5)return;
    if(gDuel_bEffectState||gDuel_wCardEffectFlags)return;
    /* First stable scene tick: battle/resume wrappers, including outer
     * Ritual protection, have finished restoring their physical survivors. */
    r2_post_battle();
}
static void r2_reset(void){memset(&r2_battle,0,sizeof(r2_battle));}
