/* New instance effects. Existing battle/LP/stat arithmetic and effect-survivor
 * reconstruction remain native or use the approved shared helpers. */
#include "game/main_modes.h"
#include "game/duel_battle_stats.h"
#include "game/duel_scene_resume.h"
#include "game/model_graphics_state.h"
#include "game/model_scene_states.h"
#include "game/model_control.h"
#include "game/func_800540B4.h"
#include "game/model_background.h"
#include "game/camera_view.h"

static int visual_activation_busy(void);
static void ritual_start_large_card_visual(int);
static void crab_umi_update_visual(void);
static void mirror_watch_lp(void);
static int ritual_damage_token;
static void (*ritual_original_animated)(void),(*ritual_original_model_control)(s32);
typedef struct { void *data; unsigned ritual; } CoinRitualMark;
static CoinRitualMark performance_marks[DUEL_CARD_RECORD_COUNT],chaos_marks[DUEL_CARD_RECORD_COUNT];
/* Live masks keep the per-field reconciliation proportional to actual Ritual
 * instances. Bits are field-record indices (all valid Monster Zones fit in a
 * 32-bit mask). coin_marks_seen remains the cheap shared hot-path gate used by
 * the surrounding hooks. */
static unsigned performance_live_mask,chaos_live_mask,coin_marks_seen;
static struct {
    int active,fatal[2],eligible[2],chaos[2],done[2],heads[2],pending_damage[2],pending_life[2],encore_used[2];
    int records[2],model_to_part[2],model_swapped,coin_part,dodge_ticks,dodge_part,dodge_x;
    DisplayObject *dodge_object;
} coin_battle;
static struct { int active,state,side,count,next,source,landed,encore_used; int slots[5];void *data[5]; } chaos_turn;
static int coin_identity(int i,int id)
{
    CoinRitualMark *m=id==PERFORMANCE_ID?performance_marks:chaos_marks;
    return monster_slot(i)&&m[i].ritual&&D_801A7AD8[i].card_id==id&&D_801A7AD8[i].data==m[i].data;
}
static void coin_reconcile(void)
{
    unsigned pending;
    if(!coin_marks_seen)return;
    pending=performance_live_mask;
    while(pending){
        int i=take_mask_bit(&pending); DuelCardRecord *c=&D_801A7AD8[i];
        if(performance_marks[i].ritual && coin_identity(i,PERFORMANCE_ID) &&
           (c->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
        memset(&performance_marks[i],0,sizeof(performance_marks[i]));
        performance_live_mask&=~(1u<<i);
    }
    pending=chaos_live_mask;
    while(pending){
        int i=take_mask_bit(&pending); DuelCardRecord *c=&D_801A7AD8[i];
        if(chaos_marks[i].ritual && coin_identity(i,CHAOS_ID) &&
           (c->flags&DUEL_CARD_FLAG_OCCUPIED))continue;
        memset(&chaos_marks[i],0,sizeof(chaos_marks[i]));
        chaos_live_mask&=~(1u<<i);
    }
    /* Re-arm occurs in coin_summon. Once no ritual coin monster remains,
     * ordinary field ticks never touch a Monster Zone for this subsystem. */
    coin_marks_seen=(performance_live_mask|chaos_live_mask)!=0;
}
static void coin_summon(int i)
{
    DuelCardRecord *c=&D_801A7AD8[i];CoinRitualMark *m;
    if(!(c->flags&DUEL_CARD_FLAG_OCCUPIED)||!c->data)return;
    if(c->card_id==PERFORMANCE_ID){m=&performance_marks[i];performance_live_mask|=1u<<i;}
    else if(c->card_id==CHAOS_ID){m=&chaos_marks[i];chaos_live_mask|=1u<<i;}
    else return;
    m->data=c->data;m->ritual=1;coin_marks_seen=1;
}

/* Only the activated Ritual instance pays this drawback.  Normal copies of
 * Magician of Black Chaos remain free to defend, as does the Ritual when its
 * per-card effect setting is disabled. */
static int chaos_attack_only(int i)
{
    return i>=0 && i<DUEL_CARD_RECORD_COUNT &&
        ritual_card_effect_enabled(CHAOS_ID) &&
        coin_identity(i,CHAOS_ID) &&
        (D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED);
}

/* Safety net for positions changed without the normal card rotation (AI,
 * other mods or saved field state).  Never modify staged battle records. */
static void chaos_enforce_attack_position(void)
{
    unsigned pending;
    if(!chaos_live_mask || !ritual_card_effect_enabled(CHAOS_ID) ||
       (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=5)return;
    pending=chaos_live_mask;
    while(pending){
        int i=take_mask_bit(&pending);
        DuelCardRecord *c=&D_801A7AD8[i];
        if(chaos_attack_only(i) && (c->flags&DUEL_CARD_FLAG_DEFENSE_POSITION)){
            c->flags&=~DUEL_CARD_FLAG_DEFENSE_POSITION;
            if(c->object)Duel_ApplyCardObjectFlags((DuelCardDisplayObject*)c->object);
            if(host&&host->log)
                host->log(host,"[Ritual Card Effects] Black Chaos: modo Defesa negado (slot %d)",i);
        }
    }
}

/* The player and CPU use the same native rotation to switch positions.
 * Cancel only the attack-to-defense rotation for an active Ritual Chaos;
 * every other monster continues along the original native path. */
static void chaos_defense_rotation(DisplayObject *o)
{
    if(o && chaos_attack_only((int)o->field_6A) &&
       !(D_801A7AD8[o->field_6A].flags&DUEL_CARD_FLAG_DEFENSE_POSITION)){
        o->field_6C=0;
        o->update=0;
        Duel_ApplyCardObjectFlags((DuelCardDisplayObject*)o);
        return;
    }
    original_defense_rotation(o);
}
static int coin_battle_protected(int n)
{
    return coin_battle.active && coin_battle.fatal[n] &&
        ((coin_battle.heads[n]&&ritual_card_effect_enabled(PERFORMANCE_ID)&&coin_battle.eligible[n]) ||
         (ritual_card_effect_enabled(CHAOS_ID)&&coin_battle.chaos[n]));
}
static int chaos_hole_protected(int i)
{
    return (gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE) && gDuel_wEffectCardID==DARK_HOLE_ID &&
        ritual_card_effect_enabled(CHAOS_ID)&&coin_identity(i,CHAOS_ID)&&(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED);
}
static void coin_clear_protected_replays(void)
{
    if(!coin_battle.active||!D_8009B229)return;
    for(int n=0;n<2;++n)if(coin_battle_protected(n))
        for(int k=0;k<2;++k)if(D_8009B208[k]==coin_battle.records[n])D_8009B208[k]=-1;
}
static void coin_restore_dodge(void)
{
    if(coin_battle.dodge_object && (gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9 &&
       D_800E9EF0[coin_battle.dodge_part+2]==coin_battle.dodge_object)coin_battle.dodge_object->field_30.h.field_30=(s16)coin_battle.dodge_x;
    coin_battle.dodge_object=0;coin_battle.dodge_ticks=0;
}
static void coin_new_battle(void)
{
    int n,result;
    memset(&coin_battle,0,sizeof(coin_battle));coin_battle.active=1;coin_battle.model_to_part[0]=0;coin_battle.model_to_part[1]=1;
    for(n=0;n<2;++n)coin_battle.records[n]=D_800E9EF0[n]?D_800E9EF0[n]->field_6A:-1;
    if(!D_800E9EF0[0]||!D_800E9EF0[1]||D_8009B22A)return;
    result=func_8001EFD4(D_800E9EF0[0],D_800E9EF0[1]);
    if(result==-1)coin_battle.fatal[0]=coin_battle.fatal[1]=1;
    else if(result>0)coin_battle.fatal[1]=1;
    else if(result<-1 && !(D_801A7AD8[coin_battle.records[1]].flags&DUEL_CARD_FLAG_DEFENSE_POSITION))coin_battle.fatal[0]=1;
    for(n=0;n<2;++n){
        coin_battle.eligible[n]=coin_battle.fatal[n]&&ritual_card_effect_enabled(PERFORMANCE_ID)&&coin_identity(coin_battle.records[n],PERFORMANCE_ID);
        coin_battle.chaos[n]=coin_identity(coin_battle.records[n],CHAOS_ID);
    }
}
/* Optional support from Fusion Auxiliary Balance. Only consult during a
   failed roll; there is no new dependency if that MOD is disabled. */
typedef struct { unsigned api; int (*eligible)(int side); } RitualEncoreV1;
static int ritual_encore_eligible(int side)
{
    const RitualEncoreV1 *api=host && host->find ?
        (const RitualEncoreV1 *)host->find(host,"fusion-auxiliary-balance:encore-v1") : 0;
    return api && api->api==1 && api->eligible && api->eligible(side);
}
static void performance_done(int result,void *context)
{
    int n=(int)(intptr_t)context,side=side_for_record(coin_battle.records[n]);
    if(!coin_battle.active)return;
    if(result==RCOIN_TAILS && !coin_battle.encore_used[n] &&
       ritual_encore_eligible(side)) {
        coin_battle.encore_used[n]=1;
        if(RitualCoinFlip_Request(RCOIN_NORMAL,-1,performance_done,context))
            return; /* First Tails has NO negative consequence. */
    }
    coin_battle.done[n]=1;coin_battle.heads[n]=(result==RCOIN_HEADS);
    if(result==RCOIN_HEADS){
        if(coin_battle.eligible[n])performance_queue_reward(side);
        coin_battle.pending_damage[n]=0;
        coin_clear_protected_replays();
        D_8009B1A4[n]=0;
        if(!D_8009B229){
            DisplayObject *o=D_800E9EF0[n+2];
            if(o){coin_battle.dodge_object=o;coin_battle.dodge_x=o->field_30.h.field_30;coin_battle.dodge_part=n;coin_battle.dodge_ticks=18;SD_SEPlayFull(0x21);}
        }
    }else if(coin_battle.pending_damage[n]){
        int life=D_800E9FF0[side].life_points.unsigned_value,loss=coin_battle.pending_damage[n];
        D_800E9FF0[side].life_points.unsigned_value=(u16)(loss>=life?0:life-loss);
        coin_battle.pending_damage[n]=0;
    }
}
static int performance_request(int n)
{
    if(!coin_battle.eligible[n]||coin_battle.done[n])return 0;
    coin_battle.coin_part=n;
    return RitualCoinFlip_Request(RCOIN_NORMAL,-1,performance_done,(void *)(intptr_t)n);
}
/* DAMAGE is the SDK-supported life-point interception; Mods_DamageLife
 * itself is not exported. Cache the already-computed native/event loss once,
 * then defer only the losing Performance's LP until its 3D coin completes. */
static void coin_damage(MemoriesModEvent *e)
{
    int n;
    int battle_phase=(gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)==9;

    /* Capture Mirror reflection at the SDK damage event itself.  During the
     * AFTER callback retail has computed event.result but has not yet copied
     * it back to DuelSideState, so this is the last reliable instant where a
     * Mirror destroyed by the same battle still has its battle grace and the
     * exact LP delta is known.  Seed mirror_last_lp with the post-damage value
     * so the legacy polling fallback cannot queue the same hit twice. */
    if(battle_phase && e->c==0 && e->phase==MEMORIES_AFTER &&
       e->a>=0 && e->a<DUEL_SIDE_COUNT) {
        unsigned before=D_800E9FF0[e->a].life_points.unsigned_value;
        unsigned after=e->result<0?0u:(unsigned)e->result;
        if(after<before && after>0 && fiends_mirror_effect_on &&
           (mirror_battle_grace[e->a] || side_has_ritual_fiend_mirror(e->a))) {
            mirror_queue_hit(e->a^1,before-after,-1);
            if(host&&host->log)
                host->log(host,"[Ritual Card Effects] Mirror capturou dano de batalha no evento: side=%d dano=%u grace=%d",
                          e->a,before-after,(int)mirror_battle_grace[e->a]);
        }
        if (after != before) {
            mirror_commit_pending[e->a] = 1;
            mirror_commit_before[e->a] = (u16)before;
            mirror_commit_after[e->a] = (u16)after;
        }
        mirror_last_lp[e->a]=(u16)after;
        mirror_lp_ready=1;
    }

    if(!coin_battle.active||!battle_phase||e->c!=0)return;
    for(n=0;n<2;++n)if(coin_battle.eligible[n]&&side_for_record(coin_battle.records[n])==e->a){
        if(e->phase==MEMORIES_BEFORE){
            coin_battle.pending_life[n]=e->result;
            if(coin_battle.heads[n]){e->b=0;e->handled=1;}
        }
        if(e->phase==MEMORIES_AFTER){
            if(coin_battle.heads[n])e->result=coin_battle.pending_life[n];
            else if(D_8009B229&&!coin_battle.done[n]){
                int loss=coin_battle.pending_life[n]-e->result;
                coin_battle.pending_damage[n]=loss>0?loss:0;e->result=coin_battle.pending_life[n];
            }
        }
    }
}
static int coin_before_battle(void)
{
    int state=D_8009B174&0x8f,n;
    if(!coin_battle.active&&state==3)coin_new_battle();
    if(ritual_coin.active||coin_battle.dodge_ticks)return 1;
    if(coin_battle.active&&!D_8009B229&&state==3){
        for(n=0;n<2;++n)if(performance_request(n))return 1;
    }
    /* State 10 only destroys the losing presentation. Keep the native result
     * object so state 11 rebuilds that SAME instance normally. Damage is
     * untouched for Chaos, but the Heads Performance result has zero damage. */
    if(coin_battle.active&&(D_8009B174&0x0f)==10&&D_8009B1B9<2&&coin_battle_protected(D_8009B1B9))D_8009B174=6;
    return 0;
}
static void coin_after_battle(void)
{
    int n;
    coin_clear_protected_replays();
    if(coin_battle.active)for(n=0;n<2;++n)if(coin_battle.heads[n]){D_8009B1A4[n]=0;D_8009B1B0[n]=0;}
    if((gDuel_wSceneStateFlags&DUEL_SCENE_PHASE_MASK)!=9){coin_restore_dodge();memset(&coin_battle,0,sizeof(coin_battle));coin_reconcile();}
}
static void coin_model_control(s32 slot)
{
    int target=slot^1,n,generic_protected;u16 saved,own;ModelSlot *a,*d;
    if(slot<0||slot>1){ritual_original_model_control(slot);return;}
    n=coin_battle.active?coin_battle.model_to_part[target]:target;
    generic_protected=(battle_visual_survivor_parts&(1u<<n))!=0;
    if(!generic_protected && (!coin_battle.active||!coin_battle_protected(n))){
        ritual_original_model_control(slot);return;
    }
    a=&D_800F2C40[slot];d=&D_800F2C40[target];
    /* Native func_800559D4 chooses death (6) versus hit/survive (5 or 8)
     * from this private MODEL scratch comparison. Set only its comparison,
     * never duel stats, and use the engine's own reaction path for each model. */
    own=a->field_CF8.prefix.values.field_00;
    saved=d->field_DFF?d->field_CF8.prefix.values.field_02:d->field_CF8.prefix.values.field_00;
    if(own==65535)a->field_CF8.prefix.values.field_00=65534;
    if(d->field_DFF)d->field_CF8.prefix.values.field_02=(own==65535)?65535:own+1;
    else d->field_CF8.prefix.values.field_00=(own==65535)?65535:own+1;
    ritual_original_model_control(slot);
    a->field_CF8.prefix.values.field_00=own;
    if(d->field_DFF)d->field_CF8.prefix.values.field_02=saved;else d->field_CF8.prefix.values.field_00=saved;
}
static ModelSlot coin_frozen_draw_save;
static void coin_draw_frozen_models(void)
{
    /* Public draw passes only: retain camera and model poses, without running
     * the controller, control handlers, animation ticks, or camera movement. */
    SetGeomOffset(160,120);SetGeomScreen(MODEL_DEFAULT_PROJECTION);GsSetRefView2(&D_800F56F0);
    for(int i=0;i<3;++i){
        memcpy(&coin_frozen_draw_save,&D_800F2C40[i],sizeof(ModelSlot));
        /* Native render mode 1 takes finish_ot before animation finalization
         * and palette-flash progression. Restore its complete scratch record
         * after submitting geometry; VRAM palettes and poses stay frozen. */
        D_800F2C40[i].field_E11=1;func_800540B4(i);
        memcpy(&D_800F2C40[i],&coin_frozen_draw_save,sizeof(ModelSlot));
    }
    func_8004DE24();
}
static void coin_animated_battle(void)
{
    int slot,n,phase;
    if(!coin_battle.active && !ritual_coin.active && !battle_visual_survivor_parts){
        ritual_original_animated();return;
    }
    coin_clear_protected_replays();
    if(coin_battle.active&&!(D_8009B26C[0]&0x40)&&!coin_battle.model_swapped){
        coin_battle.model_swapped=1;
        /* The native first model attacks first. For a sole losing Performance
         * that was the field defender, present its offensive part first, then
         * toss before the opponent's attack. Actual duel participants/LP and
         * flags remain in their original order. */
        if(coin_battle.eligible[1]&&!coin_battle.eligible[0]){
            AnimatedBattleModelProperties p=D_800EF658[0];D_800EF658[0]=D_800EF658[1];D_800EF658[1]=p;
            coin_battle.model_to_part[0]=1;coin_battle.model_to_part[1]=0;
        }
    }
    phase=D_8009AF9A;
    if((coin_battle.active||battle_visual_survivor_parts)&&D_8009AF94==15 && !ritual_coin.active){
        /* Phase 10 precedes first attack; phase 14 follows its completion and
         * precedes the second. Tied double-Performance needs the first target
         * to toss before its incoming hit, then the other on phase 14. */
        slot=phase==10?1:phase==14?0:-1;
        if(coin_battle.active&&slot>=0){n=coin_battle.model_to_part[slot];performance_request(n);}
        /* Native phase 17 is the both-idle checkpoint after the counterattack.
         * Retail expects one death here. Two survivors use its normal fade
         * phase 40, preserving both attacks and avoiding that retail dead-end. */
        if(phase==17 && (coin_battle_protected(0)||coin_battle_protected(1)||battle_visual_survivor_parts) &&
           D_800F2C40[0].field_E0E==2 && !D_800F2C40[0].field_E0F &&
           D_800F2C40[1].field_E0E==2 && !D_800F2C40[1].field_E0F)D_8009AF9A=40;
    }
    if(ritual_coin.active){coin_draw_frozen_models();return;}
    ritual_original_animated();
    if(!(D_8009B26C[0]&0x40))battle_visual_survivor_parts=0;
}
static void chaos_done(int landed,void *context)
{
    (void)context;
    if(chaos_turn.active){
        if(landed==RCOIN_TAILS && !chaos_turn.encore_used &&
           ritual_encore_eligible(chaos_turn.side)) {
            chaos_turn.encore_used=1;
            if(RitualCoinFlip_Request(RCOIN_ATEM,-1,chaos_done,context))
                return; /* Await final coin before activating or skipping. */
        }
        chaos_turn.landed=landed;
        chaos_turn.state=2;
    }
}
static void chaos_begin_turn(int side)
{
    int col; unsigned side_live;
    memset(&chaos_turn,0,sizeof(chaos_turn));coin_reconcile();
    if(!ritual_card_effect_enabled(CHAOS_ID))return;
    side_live=chaos_live_mask&side_monster_mask(side);
    if(!side_live)return;
    chaos_turn.side=side;
    /* Preserve the retail field-column order of multiple Chaos instances;
     * the live mask only makes the common no-Chaos side return immediately. */
    for(col=0;col<5;++col){int i=D_800907D8[side*20+10+col];
        if(i>=0&&i<DUEL_CARD_RECORD_COUNT&&(side_live&(1u<<i))&&coin_identity(i,CHAOS_ID)&&(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)){int n=chaos_turn.count++;chaos_turn.slots[n]=i;chaos_turn.data[n]=D_801A7AD8[i].data;}}
    chaos_turn.active=chaos_turn.count!=0;
}
static void chaos_sequence(void)
{
    int n,i;
    if(!chaos_turn.active||ritual_coin.active)return;
    if(!ritual_card_effect_enabled(CHAOS_ID)||D_8009B1D5!=chaos_turn.side){chaos_turn.active=0;return;}
    /* DrawPhase is held by the targeted draw_phase hook while this queue owns
     * the turn. Native card/effect dispatchers remain untouched, so wait for
     * their real idle state before starting or advancing another Chaos item. */
    if((gDuel_wCardEffectFlags&DUEL_CARD_EFFECT_FLAG_ACTIVE)||gDuel_bEffectState)return;
    if(chaos_turn.state==0){
        if(crab_umi_visual_state||crab_umi_sequence||tri_field_pending||bls_field_pending||visual_activation_busy()||mirror_reflection.active)return;
        while(chaos_turn.next<chaos_turn.count){n=chaos_turn.next++;i=chaos_turn.slots[n];
            if(!coin_identity(i,CHAOS_ID)||D_801A7AD8[i].data!=chaos_turn.data[n]||!(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED))continue;
            chaos_turn.source=i;chaos_turn.state=1;chaos_turn.encore_used=0;
            RitualCoinFlip_Request(RCOIN_ATEM,-1,chaos_done,0);return;}
        chaos_turn.active=0;return;
    }
    if(chaos_turn.state==2){
        if(chaos_turn.landed==RCOIN_TAILS){chaos_turn.state=0;return;}
        i=chaos_turn.source;if(!coin_identity(i,CHAOS_ID)||!(D_801A7AD8[i].flags&DUEL_CARD_FLAG_OCCUPIED)){chaos_turn.state=0;return;}
        ritual_start_large_card_visual(CHAOS_ID);chaos_turn.state=3;return;
    }
    if(chaos_turn.state==3&&!crab_umi_visual_state){
        DuelEffect_StartCardEffect(DARK_HOLE_ID,1);chaos_turn.state=4;return;
    }
    if(chaos_turn.state==4){
        /* The shared dispatcher has completed AND rebuilt survivors. Its
         * per-resolution snapshots are cleared before another Black Hole. */
        coin_reconcile();serpent_reconcile();mask_reconcile();chaos_turn.state=0;
    }
}
static void coin_update_pads(void)
{
    ritual_original_pads();
    /* Only the coin UI and the short dodge animation own player input.
     * chaos_turn already blocks DrawPhase, so swallowing input for the entire
     * Black-Chaos queue is unnecessary and can deadlock a native effect-state
     * presentation that appears after Black Hole/survivor resync and waits for
     * confirmation.  Let native effect states see the pad while the automatic
     * start-of-turn queue itself remains isolated by draw_phase(). */
    if(ritual_coin.active||coin_battle.dodge_ticks){
        if(ritual_coin.active)ritual_coin_pressed|=gInput_wPad1Pressed;
        gInput_wPad1Pressed=gInput_wPad1Repeat=0;gInput_wPad2Pressed=gInput_wPad2Repeat=0;
    }
}
static void coin_run_duel(void)
{
    if(ritual_coin.active||coin_battle.dodge_ticks)return;
    ritual_original_run();
}
static void coin_frame(void)
{
    if(ritual_coin.active)ritual_coin_frame();
    if(coin_battle.dodge_ticks){int elapsed=18-coin_battle.dodge_ticks,shift=elapsed<6?elapsed*3:elapsed<12?18:3*(18-elapsed);
        if(coin_battle.dodge_object)coin_battle.dodge_object->field_30.h.field_30=(s16)(coin_battle.dodge_x+(coin_battle.dodge_part?-shift:shift));
        if(!--coin_battle.dodge_ticks)coin_restore_dodge();}
}
static void performance_dodge_flash(void)
{
    int w,h,scale,vw,vh,x,y,alpha;DisplayObject *o=coin_battle.dodge_object;
    if(!o||!coin_battle.dodge_ticks||coin_battle.dodge_ticks<12)return;
    host->overlay_size(host,&w,&h,&scale);vw=w;vh=w*3/4;if(vh>h){vh=h;vw=h*4/3;}
    x=(w-vw)/2+(o->field_30.h.field_30+70)*vw/320;y=(h-vh)/2+(o->field_30.h.field_32+40)*vh/240;
    alpha=(coin_battle.dodge_ticks-11)*30;
    if(scale<1)scale=1;
    host->fill(host,x-10*scale,y,2*scale,34*scale,0xE6DBFF,alpha);
    host->fill(host,x-16*scale,y+8*scale,scale,18*scale,0xFFFFFF,alpha/2);
}
static void coin_reset(void)
{
    coin_restore_dodge();memset(&coin_battle,0,sizeof(coin_battle));memset(&chaos_turn,0,sizeof(chaos_turn));
    memset(performance_marks,0,sizeof(performance_marks));memset(chaos_marks,0,sizeof(chaos_marks));
    performance_live_mask=chaos_live_mask=coin_marks_seen=0;
    memset(&ritual_coin,0,sizeof(ritual_coin));ritual_coin_pressed=0;
}
