#ifndef AUX_CARD_TERRAIN
#define AUX_CARD_TERRAIN(id) Duel_GetTerrainBoost(Cards_Type(id))
#endif
#ifndef FUSION_ROUND3_RUNTIME_H
#define FUSION_ROUND3_RUNTIME_H
/* All runtime bonuses are derived from the original packed stats, never written
   into persistent attack/defense modifiers. The battle key is physical slot +
   card data pointer, so duplicate IDs cannot share origin or state. */
typedef struct {
 int active,prepared,resolved,negated,pending;
 int slot[2],id[2],eligible[2],death[2],target_down;
 int attack_bonus,attacker_penalty,target_penalty,marine_penalty;
 int barox_swap,coin_checked,coin_pending,coin_swap,coin_atk[2];
 int kwagar_guard[2],battle_damage[2],rose_sources[2],plant_fighting[2];
 void *data[2];
} R3Battle;
static R3Battle r3_battle;
static DuelDeckCardRecord r3_summon_data[30];
static void *r3_summon_lease[30];
static s16 r3_summon_id[30];
/* Per-duel leases: attached to physical card data, not field position or ID. */
static void *r3_kwagar_spent[30],*r3_mavelus_spent[30];
static unsigned r3_kwagar_count,r3_mavelus_count;
typedef struct {int side,active;void *origin;} R3MavelusPending;
static R3MavelusPending r3_mavelus_pending[30];
/* Only the official side switch opens a return window. The card must actually
   enter before ANY hand / AI action, but after the native draw stage is fully
   finished. Never summon during the switch animation / draw callbacks: they
   may not yet reflect the same board used by the player's zone selection. */
static int r3_mavelus_due_side=-1;
/* Kaminari is a one-shot entry trigger. A pending prohibition is scheduled
   for the NEXT turn of the opposing side, never renewed by its mere presence.
   The active prohibition expires at that side's next turn boundary. */
typedef struct {int id,triggered;void *data;} R3KaminariSeen;
static R3KaminariSeen r3_kaminari_seen[30];
static unsigned char r3_kaminari_pending[2],r3_kaminari_active[2];

static int r3_identity(int slot,int p){
 return aux_monster_slot(slot)&&slot==r3_battle.slot[p]&&
 D_801A7AD8[slot].card_id==r3_battle.id[p]&&D_801A7AD8[slot].data==r3_battle.data[p];
}
static int r3_source(int slot,int id){
 const DuelCardRecord *c;
 if(!aux_monster_slot(slot))return 0;
 c=&D_801A7AD8[slot];
 return c->card_id==id&&!(c->flags&DUEL_CARD_FLAG_FACE_DOWN)&&
 ((c->flags&DUEL_CARD_FLAG_OCCUPIED)||r3_identity(slot,0)||r3_identity(slot,1))&&
 aux_fusion_special(slot,!(c->flags&DUEL_CARD_FLAG_OCCUPIED));
}
static int r3_ally_count(int slot,int attribute,int type){
 int i,n=0,side=slot/15;
 for(i=side*15+5;i<side*15+10;i++)if(i!=slot&&
  (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)&&
  !(D_801A7AD8[i].flags&DUEL_CARD_FLAG_FACE_DOWN)&&
  Cards_Valid(D_801A7AD8[i].card_id)&&
  (attribute<0||Cards_Attribute(D_801A7AD8[i].card_id)==attribute)&&
  (type<0||Cards_Type(D_801A7AD8[i].card_id)==type))n++;
 return n;
}
/* Flower Wolf itself is Beast; count OTHER, revealed Plant allies only.
   Source provenance and per-card Settings are resolved by r3_source. */
static int r3_flower_untargetable(int slot){
 return aux_monster_slot(slot)&&r3_source(slot,487)&&
        r3_ally_count(slot,-1,CARD_TYPE_PLANT)>0;
}
static int r3_kaminari_attacks_blocked(int side){
 return side>=0&&side<2&&r3_kaminari_active[side]&&r2_setting_enabled(458);
}
/* This is the real grid's opponent-monster row (not an ID lookup). */
static int r3_grid_opponent_target(int side,int col){
 if(side<0||side>1||col<0||col>=5)return -1;
 return D_800907D8[side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+5+col];
}
/* Do not allow a direct attack while a protected Flower Wolf remains in the
   opponent's occupied monster row. The caller must choose another occupied
   target or reject this particular attempted attack. */
static int r3_find_attackable_enemy(int side,int preferred){
 int col,index;
 if(preferred>=0&&preferred<5){
   index=r3_grid_opponent_target(side,preferred);
   if(aux_monster_slot(index)&&
      (D_801A7AD8[index].flags&DUEL_CARD_FLAG_OCCUPIED)&&
      !r3_flower_untargetable(index))return preferred;
 }
 for(col=0;col<5;col++){
   index=r3_grid_opponent_target(side,col);
   if(aux_monster_slot(index)&&
      (D_801A7AD8[index].flags&DUEL_CARD_FLAG_OCCUPIED)&&
      !r3_flower_untargetable(index))return col;
 }
 return -1;
}
/* Seen per physical card record. The card ID alone never transfers an
   entry trigger to another copy. A removal clears the seen slot explicitly. */
static int r3_kaminari_used(int slot){return slot>=0&&slot<30&&r3_kaminari_seen[slot].triggered;}
static void r3_kaminari_forget(int slot){
 if(slot>=0&&slot<30)memset(&r3_kaminari_seen[slot],0,sizeof(r3_kaminari_seen[slot]));
}
static void r3_kaminari_scan(void){
 int slot;
 for(slot=0;slot<30;slot++)if(aux_monster_slot(slot)){
   DuelCardRecord *c=&D_801A7AD8[slot];
   R3KaminariSeen *seen=&r3_kaminari_seen[slot];
   if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED)||c->card_id!=458||!c->data){
      r3_kaminari_forget(slot);continue;
   }
   if(seen->id!=c->card_id||seen->data!=c->data){
      seen->id=c->card_id;seen->data=c->data;seen->triggered=0;
   }
   if(!seen->triggered&&r3_source(slot,458)){
      r3_kaminari_pending[(slot/15)^1]=1;
      seen->triggered=1;
   }
 }
}
static int r3_continuous(int slot,int *atk,int *def){
 int id,i,fire,dragon,warrior;
 if(!aux_monster_slot(slot))return 0;
 id=D_801A7AD8[slot].card_id;
 if(id==473&&r3_source(slot,id)){
   fire=Cards_AttributeNamed("Fire");
   *atk+=200*r3_ally_count(slot,fire,-1);*def+=200*r3_ally_count(slot,fire,-1);
 }
 if(id==639&&r3_source(slot,id)){
   if(gDuel_bTerrain[0]==5)*atk+=800; /* UMI */
   if(gDuel_bTerrain[0]==2)*def+=800; /* WASTELAND */
 }
 if(id==294&&r3_source(slot,id)){
   dragon=r3_ally_count(slot,-1,CARD_TYPE_DRAGON)>0;
   warrior=r3_ally_count(slot,-1,CARD_TYPE_WARRIOR)>0;
   *atk+=500*(dragon+warrior);*def+=500*(dragon+warrior);
 }
 return 0;
}
static s32 r3_stats(DuelCardRecord *c,s32 packed){
 uintptr_t addr=(uintptr_t)c,base=(uintptr_t)D_801A7AD8;
 int slot,at=(u16)packed,df=(unsigned)packed>>16;
 if(addr<base||addr>=base+30*sizeof(*c)||(addr-base)%sizeof(*c))return packed;
 slot=(int)((addr-base)/sizeof(*c));
 if(!aux_monster_slot(slot))return packed;
 r3_continuous(slot,&at,&df);
 if(r3_battle.active&&r3_battle.prepared&&!r3_battle.negated){
   if(r3_identity(slot,0)){
     at+=r3_battle.attack_bonus;
     at-=r3_battle.attacker_penalty;
     if(r3_battle.barox_swap){int t=at;at=df;df=t;}
   if(r3_battle.coin_swap)at=r3_battle.coin_atk[1];
   }else if(r3_identity(slot,1)){
     at-=r3_battle.target_penalty+r3_battle.marine_penalty;
     if(r3_battle.coin_swap)at=r3_battle.coin_atk[0];
   }
 }
 at=r2_limit(at,Tables_StatCap(0));df=r2_limit(df,Tables_StatCap(1));
 return (s32)(((unsigned)df<<16)|(unsigned)at);
}
static int r3_seen_data(void *const *values,unsigned count,void *data){
 unsigned i;for(i=0;i<count;i++)if(values[i]==data)return 1;return 0;
}
static void r3_mark_data(void **values,unsigned *count,void *data){
 if(!data||r3_seen_data((void *const *)values,*count,data))return;
 if(*count<30)values[(*count)++]=data;
}
static int r3_guard(int slot){
 int p;
 for(p=0;p<2;p++)if(r3_battle.active&&r3_identity(slot,p)&&r3_battle.kwagar_guard[p])return 1;
 return 0;
}
static void r3_begin(void){
 int p;
 memset(&r3_battle,0,sizeof(r3_battle));r3_battle.active=1;
 for(p=0;p<2;p++){
   int slot=battle_record[p],id=battle_card[p];
   r3_battle.slot[p]=slot;r3_battle.id[p]=id;
   r3_battle.data[p]=battle_saved_data[p];
   r3_battle.eligible[p]=r3_member(id)&&aux_monster_slot(slot)&&
      aux_fusion_special(slot,1);
   if(id==533&&r3_battle.eligible[p]&&
      !r3_seen_data((void *const *)r3_kwagar_spent,r3_kwagar_count,r3_battle.data[p]))
      r3_battle.kwagar_guard[p]=1;
   if(Cards_Valid(id)&&Cards_Type(id)==CARD_TYPE_PLANT)
      r3_battle.plant_fighting[slot/15]=1;
 }
 r3_battle.target_down=(battle_saved_flags[1]&DUEL_CARD_FLAG_FACE_DOWN)!=0;
 for(p=0;p<2;p++){
   int side=p,i; /* Each actual Rose Spectre is a source of Plant reactions. */
   for(i=side*15+5;i<side*15+10;i++)if(r3_source(i,594))r3_battle.rose_sources[side]++;
 }
}
static void r3_prepare(int was){
 int a=r3_battle.slot[0],d=r3_battle.slot[1];
 int id,target,source_a,source_d,target_atk,source_atk,raw_a,raw_d;
 if(!r3_battle.active||r3_battle.prepared||was!=2||D_8009B22A||
    !r3_identity(a,0)||!r3_identity(d,1))return;
 id=r3_battle.id[0];target=r3_battle.id[1];
 source_a=r3_battle.eligible[0];source_d=r3_battle.eligible[1];
 raw_a=(u16)Duel_CalcCardStats(&D_801A7AD8[a]);
 raw_d=(u16)Duel_CalcCardStats(&D_801A7AD8[d]);
 source_atk=raw_a;target_atk=raw_d;
 if(source_a){
   r3_battle.attack_bonus=r3_attack_target_bonus(id,1,Cards_Type(target),
      Cards_Attribute(target)==Cards_AttributeNamed("Dark"),r3_battle.target_down,
      r3_ally_count(a,-1,CARD_TYPE_DINOSAUR));
   /* Skelgon attacks Dragon-like creatures: Dragon primary type or the
      Dragon secondary subtype defined in Vanilla Field Effects. Do not
      change other attackers' primary-type matching. */
   if(id==545 && aux_dragon_like(target))r3_battle.attack_bonus=800;
   if(id==521){r3_battle.attack_bonus=400;r3_battle.target_penalty=400;}
   if(id==86)r3_battle.barox_swap=1;
   if(id==43&&r3_battle.coin_checked&&r3_battle.coin_swap){
      r3_battle.coin_atk[0]=raw_a;r3_battle.coin_atk[1]=raw_d;
   }
 }
 /* Marine Beast checks CURRENT ATK before any bonuses specific to this fight.
    It triggers on either side, including when it is defending in DEF. */
 if((source_a&&id==617)||(source_d&&target==617)){
   int marine=source_a&&id==617?0:1;
   int its=marine?target_atk:source_atk,enemy=marine?source_atk:target_atk;
   if(enemy>its){
     if(marine)r3_battle.attacker_penalty+=600;
     else r3_battle.target_penalty+=600;
   }
 }
 r3_battle.prepared=1;
}
static void r3_coin_finished(int result,void *opaque){
 (void)opaque;
 r3_battle.coin_pending=0;
 r3_battle.coin_swap=(result!=0);
}
static int r3_coin_pause(void){
 int a=r3_battle.slot[0],d=r3_battle.slot[1];
 int atk,enemy;
 if(!r3_battle.active||r3_battle.coin_checked)return r3_battle.coin_pending;
 r3_battle.coin_checked=1;
 if(r3_battle.id[0]!=43||!r3_battle.eligible[0]||
    !r3_identity(a,0)||!r3_identity(d,1)||D_8009B22A)return 0;
 atk=(u16)Duel_CalcCardStats(&D_801A7AD8[a]);
 enemy=(u16)Duel_CalcCardStats(&D_801A7AD8[d]);
 if(enemy<=atk)return 0;
 if(AuxBattleCoinFlip_Request(0,-1,r3_coin_finished,0)){
     r3_battle.coin_pending=1;return 1;
 }
 r3_battle.coin_swap=(rand()%2)==0;
 return 0;
}
static void r3_schedule_mavelus_data(int slot,void *data){
 int i,side=slot/15;
 if(!aux_monster_slot(slot)||!data||
    r3_seen_data((void *const *)r3_mavelus_spent,r3_mavelus_count,data))return;
 r3_mark_data(r3_mavelus_spent,&r3_mavelus_count,data);
 for(i=0;i<30;i++)if(!r3_mavelus_pending[i].active){
    r3_mavelus_pending[i].active=1;
    r3_mavelus_pending[i].side=side;
    r3_mavelus_pending[i].origin=data;
    return;
 }
}
static void r3_schedule_mavelus(int slot){
 void *data=r3_battle.active&&r3_identity(slot,0)?r3_battle.data[0]:
            r3_battle.active&&r3_identity(slot,1)?r3_battle.data[1]:D_801A7AD8[slot].data;
 r3_schedule_mavelus_data(slot,data);
}
static int r3_effect_summon(int side,int id);
static void r3_turn_begin(int side){
 if(side<0||side>1)return;
 /* Even if more Kaminari enter during a locked turn, they can affect only a
    subsequent opposing turn. Do not count down Swords or reuse its timer. */
 r3_kaminari_active[side^1]=0;
 r3_kaminari_active[side]=r3_kaminari_pending[side];
 r3_kaminari_pending[side]=0;
 /* No card is created while the engine is still switching sides and dealing
    the hand. Mark exactly one next-turn window for the actions-phase boundary. */
 r3_mavelus_due_side=side;
}
/* Called BEFORE the original scene handler can accept the first hand/AI
   action. The draw is finished (phase 4), so both field occupancy and card
   visuals are authoritative. Consume this window even with a full field:
   the return never steals a newly played monster or slips to a later turn. */
static void r3_turn_entry_before_actions(int side){
 int i;
 if(side<0||side>1||r3_mavelus_due_side!=side ||
    (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=4)return;
 r3_mavelus_due_side=-1;
 for(i=0;i<30;i++)if(r3_mavelus_pending[i].active&&
    r3_mavelus_pending[i].side==side){
    r3_mavelus_pending[i].active=0;
    r3_effect_summon(side,272);
 }
}
static int r3_effect_summon(int side,int id){
 int i,j,pool;
 for(i=side*15+5;i<side*15+10;i++)if(!(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)){
   DuelCardRecord *c=&D_801A7AD8[i];DuelCardDisplayObject *obj;
   for(pool=0;pool<30;pool++){
     for(j=0;j<30;j++)if((D_801A7AD8[j].flags&DUEL_CARD_FLAG_OCCUPIED)&&
           D_801A7AD8[j].data==&r3_summon_data[pool])break;
     if(j==30)break;
   }
   if(pool==30)return 0;
   aux_unlock_revoke(i);fusion_origin[i]=metal_immune[i]=0;
   fusion_bound_id[i]=metal_bound_id[i]=0;
   fusion_bound_data[i]=metal_bound_data[i]=0;
   aux_elf_bound_id[i]=aux_elf_healed[i]=0;aux_elf_bound_data[i]=0;
   memset(&r3_summon_data[pool],0,sizeof(r3_summon_data[pool]));
   r3_summon_data[pool].id=id;r3_summon_data[pool].deck_index=0;
   r3_summon_data[pool].data_block_index=(u8)i;
   memset(c,0,sizeof(*c));c->data=&r3_summon_data[pool];c->card_id=id;c->table_index=(u8)i;
   c->attack=(s16)r2_original_atk(id);c->defense=(s16)r2_original_def(id);
   c->terrain_modifier=(s16)AUX_CARD_TERRAIN(id);
   c->flags=DUEL_CARD_FLAG_OCCUPIED;
   obj=func_80024C1C(id,D_800908A0[i].x,D_800908A0[i].y);
   c->object=obj;if(obj)obj->card_index=(u8)i;
   r3_summon_lease[i]=c->data;
   r3_summon_id[i]=(s16)id;
   aux_darkfire_card_thumbnail(i,id);
   if(id==272)r3_queue_rebirth_fire(i,id);
   return 1;
 }
 return 0;
}
/* Keep an effect-created Hinotama Soul / Mavelus stable when the native
   turn renderer rebuilds the field. Never revive a removed instance: the
   physical data lease must still be the SAME one, and its field record must
   still be occupied. Battle transition 9/11 temporarily deactivates both
   fighters, so defer lease expiry there until the restore wrapper runs. */
static void r3_reconcile_summons(int refresh_turn){
 int i;
 for(i=0;i<30;i++)if(r3_summon_lease[i]){
   DuelCardRecord *c=&D_801A7AD8[i];
   DuelDeckCardRecord *data=(DuelDeckCardRecord *)r3_summon_lease[i];
   int id=r3_summon_id[i],corrupted,redraw;
   DuelCardDisplayObject *obj;
   if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED)||c->data!=data){
      int phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
      if(c->data==data&&(phase==9||phase==11))continue;
      r3_summon_lease[i]=0;r3_summon_id[i]=0;
      continue;
   }
   /* A genuine field fusion/transform changes BOTH the live card ID and
      its backing deck record ID. That is a new card, NOT corruption: drop
      the synthetic lease instead of undoing the player's fusion. */
   if(c->card_id!=id&&data->id!=id){
      r3_summon_lease[i]=0;r3_summon_id[i]=0;
      continue;
   }
   corrupted=(c->card_id!=id||data->id!=id);
   redraw=refresh_turn||corrupted||!c->object||
          ((DuelCardDisplayObject *)c->object)->card_index!=i;
   if(corrupted){
      c->card_id=(s16)id;data->id=(s16)id;
      c->attack=(s16)r2_original_atk(id);
      c->defense=(s16)r2_original_def(id);
      c->terrain_modifier=(s16)AUX_CARD_TERRAIN(id);
   }
   if(!redraw)continue;
   if(c->object)DisplayObject_ReleaseIfPresent(c->object);
   c->object=0;
   obj=func_80024C1C(id,D_800908A0[i].x,D_800908A0[i].y);
   c->object=obj;
   if(obj){obj->card_index=(u8)i;Duel_ApplyCardObjectFlags(obj);}
   aux_darkfire_card_thumbnail(i,id);
 }
}
static void r3_post_battle(void){
 int p,side;
 int alive[2],dead[2];
 r3_battle.pending=0;
 for(p=0;p<2;p++){
   int s=r3_battle.slot[p];
   alive[p]=r3_identity(s,p)&&(D_801A7AD8[s].flags&DUEL_CARD_FLAG_OCCUPIED);
   dead[p]=r3_battle.death[p]&&!alive[p];
 }
 if(!r3_battle.resolved||r3_battle.negated)return;
 /* Warrior of Tradition: the source survives, another Female dies by battle. */
 for(p=0;p<2;p++)if(dead[p]&&aux_monster_slot(r3_battle.slot[p])&&
   Cards_InFusionGroup(r3_battle.id[p],CARD_FUSION_GROUP_FEMALE)){
   int slot=r3_battle.slot[p],side=slot/15,i;
   for(i=side*15+5;i<side*15+10;i++)if(i!=slot&&
      r3_source(i,618)){
     DuelCardRecord *c=&D_801A7AD8[i];
     {int value=c->stat_modifier+400;if(value>32767)value=32767;
     c->stat_modifier=(s16)value;}
   }
 }
 /* Reflected damage is based on verified battle-only LP loss, and a
    surviving Rose source at battle entry (Rose may die during battle). */
 for(side=0;side<2;side++)if(r3_battle.plant_fighting[side] &&
       r3_battle.rose_sources[side]==1&&r3_battle.battle_damage[side]>0){
   int suffered=battle_lp_before[side]-D_800E9FF0[side].life_points.unsigned_value;
   int reflect;
   if(suffered>r3_battle.battle_damage[side])suffered=r3_battle.battle_damage[side];
   reflect=suffered>0?suffered/2:0;
   if(reflect)aux_queue_lp(side^1,reflect,345,0);
 }
 for(p=0;p<2;p++)if(dead[p]&&r3_battle.eligible[p]){
   int id=r3_battle.id[p],side=r3_battle.slot[p]/15;
   if(id==133)r3_effect_summon(side,210);
   if(id==272)r3_schedule_mavelus_data(r3_battle.slot[p],r3_battle.data[p]);
   if(id==30){
     int s,side_begin=side*15+5;
     for(s=side_begin;s<side_begin+5;s++)if(s!=r3_battle.slot[p]&&
       (D_801A7AD8[s].flags&DUEL_CARD_FLAG_OCCUPIED)&&
       RitualCategory_IsSkullMonster(D_801A7AD8[s].card_id)){
       DuelCardRecord *c=&D_801A7AD8[s];int next=c->stat_modifier+300;
       if(next>32767)next=32767;c->stat_modifier=(s16)next;
     }
   }
   /* Additional battle damage is independent of native battle damage. */
   if(id==215)aux_queue_lp(side^1,500,345,0);
 }
}
static void r3_after_step(int was){
 int p;
 if(!r3_battle.active)return;
 if(D_8009B22A)r3_battle.negated=1;
 if(was==3&&(D_8009B174&15)!=3&&!r3_battle.negated){
    int attacking=r3_battle.slot[0]/15;
    if(D_8009B1A4[0]<-1)r3_battle.battle_damage[attacking]=-D_8009B1A4[0];
    if(D_8009B1A4[1]>0)r3_battle.battle_damage[attacking^1]=D_8009B1A4[1];
    r3_battle.resolved=1;
 }
 if(r3_battle.resolved&&!r3_battle.negated&&(was==3||was==5))
    for(p=0;p<2;p++)if(D_8009B1B0[p]<0){
     r3_battle.death[p]=1;
     if(r3_battle.kwagar_guard[p])
       r3_mark_data(r3_kwagar_spent,&r3_kwagar_count,r3_battle.data[p]);
   }
}
static void r3_end(void){
 if(!r3_battle.active)return;
 r3_battle.pending=r3_battle.resolved&&!r3_battle.negated;
 r3_battle.active=0;
}
static void r3_stable_tick(void){
 int phase=gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK;
 if(!r3_battle.pending||(phase!=4&&phase!=5)||
    gDuel_bEffectState||gDuel_wCardEffectFlags)return;
 r3_post_battle();
}
static void r3_reset(void){memset(&r3_battle,0,sizeof(r3_battle));}
static void r3_full_reset(void){
 r3_reset();
 memset(r3_kwagar_spent,0,sizeof(r3_kwagar_spent));r3_kwagar_count=0;
 memset(r3_mavelus_spent,0,sizeof(r3_mavelus_spent));r3_mavelus_count=0;
 memset(r3_mavelus_pending,0,sizeof(r3_mavelus_pending));
 r3_mavelus_due_side=-1;
 memset(r3_kaminari_seen,0,sizeof(r3_kaminari_seen));
 memset(r3_kaminari_pending,0,sizeof(r3_kaminari_pending));
 memset(r3_kaminari_active,0,sizeof(r3_kaminari_active));
 memset(r3_summon_lease,0,sizeof(r3_summon_lease));
 memset(r3_summon_id,0,sizeof(r3_summon_id));
}
#endif
