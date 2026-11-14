#include "rd_drop_rewards.h"
#define D_8009B360_AS_SIDE_ARRAY
/* Ritual Duelists — isolated API 4 code mod.
   Hooks chain through the game's API so AI Hard Mode and other mods are not
   overwritten. Only the registered duelists receive deck/AI changes. */
#define D_800EAE88_VISIBLE
#define D_800EAE88_AS_BYTES
#define AI_GET_HAND_SIZE_RETURNS_S32
#define FUNC_80018004_AMBIENT_POSITION_ARGS
#include "types.h"
#include "game/ai.h"
#include "game/card_constants.h"
#include "game/duel_check_ritual.h"
#include "game/duel_shuffle_deck.h"
#include "game/duel_deck_card.h"
#include "game/duel_hand.h"
#include "game/duel_grid.h"
#include "game/duel_card.h"
#include "game/duel_card_display_state.h"
#include "game/func_80018004.h"
#include "game/func_8001B938.h"
#include "game/display_object_core.h"
#include "game/duel_side_state.h"
#include "game/ai_opponent_data.h"
#include "game/rand_get_interval.h"
#include "pc/free_duel/duelists.h"
#include "pc/cards/cards.h"
#include "pc/mods/modapi.h"
#include "unmatched.h"
#include "planner.h"
#include "recipe_adapter.h"
#include "adaptive_rules.h"
#include "field_guard.h"
#include "hand_play.h"
#include "diagnostic_core.h"
#include "deck_access.h"
#include "tribute_fetch.h"
#include "post_ritual.h"
#include "ai_policy.h"
#include "starter_rules.h"
#include "starter_bridge.h"
#include "game/duel_action_lock.h"
#include "game/duel_ritual_controller.h"
#include "game/duel_ritual_effect.h"
#include "game/duel_card_checks.h"
#include "game/duel_scene_hand_actions.h"
#include <string.h>
#include <stdio.h>

#define RD_SIMON_PROFILE (RD_PROFILE_CAPACITY-1)
static int simon_id=-2;
static const MemoriesModHost *host;
static void *previous_deal, *previous_ai, *previous_swap, *previous_window;
static void *previous_ritual, *previous_fusion;
static int deal_hook,ai_hook,swap_hook,window_hook,ritual_hook,fusion_hook;
/* The host allows only ONE pointer-free state buffer per MOD. A version-2
   extension to RdSaveState tracks the identity of ritual-summoned cards. */
/* One imminent CPU hand action; cleared before the next deck materialization. */
enum { RD_PULL_MAGIC=1, RD_PULL_FIELD=2, RD_PULL_HOLD=3 };
static struct {int active,side,source,replace,card,mode,target;u8 fallback[12];} deck_pull;
static RdSaveState save_state;
#define ritual_marks (save_state.ritual_marks)
static int cached_id[RD_PROFILE_CAPACITY];
static RdRecipeCache recipe_cache[RD_PROFILE_CAPACITY];
static unsigned char recipe_ready[RD_PROFILE_CAPACITY];
static struct {unsigned key;int valid,result,ritual;} legal_cache;
static struct {unsigned key;int valid;RdPlan result;} plan_cache;
static int planned_turns;
static int last_turn_seen=-1;
extern u8 D_801A8000[];
extern u8 D_801A9800[];
extern s32 gDuel_adwCardStats[];

/* Optional on-screen diagnostics. Populated by duel creation and AI calls,
   never by scanning recipes or card records on every rendered frame. */
typedef struct {
    int valid, fallback, restored, tier, mode, recipe_source, materials, fixed_cards;
    int deck_size, duelist_id, ritual_magic, events, last_ai_turn, phase, hand_count;
    int near, prep, urgent, legal, can_match, score, trigger;
    int matched_materials, ritual_goal, predicted_attack, predicted_defense;
    int field_count, back_count, action, selected_id;
    int magic_hand, magic_back, magic_window, logical_hand;
    int checked_legal, reason, origin;
    int sticky_action, sticky_turn, sticky_phase, sticky_card, sticky_origin;
    unsigned deck_fingerprint;
    int fixed_id[RD_DECK_SIZE], fixed_qty[RD_DECK_SIZE];
    int role_id[RD_MAX_RITUAL_MATERIALS];
    RdAdaptiveRules rule_snapshot;
} RdDiagnostics;
static RdDiagnostics diag;
static RdAdaptiveRules diag_current_rules;
static int rd_setting(const char *key,int fallback);
static int rd_profile_for_opponent(void);
static int rd_diag_level(void)
{
    int level=host->setting(host,"debug_overlay",1);
    return level<0?0:level>2?2:level;
}
static const char *rd_diag_tier(int tier)
{
    return tier==0?"0 PACKAGES":tier==1?"1 PACKAGE":tier==2?"2 PACKAGES":tier==3?"3 PACKAGES":"UNKNOWN";
}
static const char *rd_diag_source(int source)
{
    switch(source){
    case 1:return "Efeitos de Rituais API";
    case 2:return "Tables (conditional)";
    case 3:return "Tables (exact)";
    case 4:return "Vanilla/disc";
    case 5:return "Manual Ritual Effects";
    case 6:return "v0.3 rules (1-5)";
    default:return "Unknown";
    }
}
static const char *rd_diag_action(int action)
{
    switch(action){
    case 1:return "Previous AI kept";
    case 2:return "Set Ritual Magic";
    case 3:return "Preserve material";
    case 4:return "Activate Ritual";
    case 5:return "No Ritual Magic seen";
    case 6:return "Planner disabled";
    case 7:return "Ritual legal via external sources";
    case 8:return "Previous AI chose Ritual";
    case 9:return "Previous AI lethal preserved";
    case 10:return "Play missing tribute";
    case 11:return "Protect field tribute";
    case 12:return "No safe field target";
    case 13:return "Set Ritual Magic from Deck";
    case 14:return "Play tribute from Deck";
    case 15:return "Keep fetched tribute in Hand";
    case 16:return "Play Post-Ritual Beacon support";
    case 17:return "Fetch Post-Ritual Beacon support";
    default:return "Waiting for AI";
    }
}
static void rd_diag_deck(int idx,int mode,const RdPackageStats *packs,const u16 *cards,int count,int fallback)
{
    int i,source=recipe_cache[idx].source;
    const RdRitualRecipe *recipe=recipe_cache[idx].status?&recipe_cache[idx].recipe:0;
    memset(&diag,0,sizeof(diag));
    diag.valid=1;diag.fallback=fallback;diag.tier=packs?packs->packages:0;
    diag.mode=mode;diag.duelist_id=(int)gDuel_bOpponentID;
    diag.recipe_source=source;diag.deck_size=count;diag.ritual_magic=recipe?recipe->ritual_magic:0;
    diag.materials=recipe?(int)recipe->material_count:-1;
    diag.last_ai_turn=-1;
    diag.magic_hand=diag.magic_back=diag.magic_window=-1;
    diag.sticky_turn=-1;
    if(recipe){
        for(i=0;i<(int)recipe->material_count && i<RD_MAX_RITUAL_MATERIALS;i++)
            diag.role_id[i]=recipe->materials[i].exact_card;
    }
    if(!fallback && packs){
        int pidx,k,at=0;
        for(pidx=0;pidx<packs->packages;pidx++)for(k=0;k<4;k++){
            diag.fixed_id[at]=packs->included[pidx][k];
            diag.fixed_qty[at]=1;
            diag.fixed_cards++;
            at++;
        }
    }
    if(cards && count==RD_DECK_SIZE){
        unsigned hash=2166136261u;
        for(i=0;i<count;i++){
            hash^=(unsigned)cards[i];hash*=16777619u;
        }
        diag.deck_fingerprint=hash;
    }
    if(rd_setting("diagnostics",0)){
        host->log(host,"RD DIAG | Duel %d | %s | packages=%d | deck=%d | recipe=%s | roles=%d | package_cards=%d | fallback=%d | hash=%08x",
           (int)save_state.generated+(!fallback),mode==2?"Story":"Free",diag.tier,count,
           rd_diag_source(source),diag.materials,diag.fixed_cards,fallback,diag.deck_fingerprint);
        if(!fallback && packs){
            int q;
            for(q=0;q<packs->packages;q++)host->log(host,
              "RD DIAG | Ritual Package #%d: Magic #%d materials #%d #%d #%d",
              q+1,packs->included[q][0],packs->included[q][1],
              packs->included[q][2],packs->included[q][3]);
        }
    }
}
/* The checker was not necessarily called on every AI tick. "?" in the
   overlay means not queried, NOT that a legal Ritual was rejected. */
static void rd_diag_ai(int hand,const RdBoard *board,const RdPlan *plan,int legal,
                       int action,int selected,int ritual_magic)
{
    int i,fields=0,back=0,turn=(int)D_800E9FF0[D_8009B1D5].rank.turns_taken;
    int phase=hand?1:2,reason=RD_WHY_NONE,origin=RD_ORIGIN_NONE;
    RdMagicAccess access;
    int noteworthy,changed;
    if(!diag.valid||!board||!plan)return;
    Rd_FindRitualMagic(board,ritual_magic,&access);
    for(i=0;i<5;i++){fields+=board->own[i].id>0;back+=board->back[i].id>0;}
    switch(action){
        case 2:reason=RD_WHY_SET_REQUEST;origin=RD_ORIGIN_DUELISTS;break;
        case 3:reason=RD_WHY_PRESERVE_MATERIAL;origin=RD_ORIGIN_DUELISTS;break;
        case 4:reason=RD_WHY_ACTIVATE_REQUEST;origin=RD_ORIGIN_DUELISTS;break;
        case 5:reason=ritual_magic<=0?RD_WHY_NO_RECIPE:
            access.window_slot>=0?RD_WHY_LOGICAL_ONLY:RD_WHY_MAGIC_UNSEEN;break;
        case 7:reason=RD_WHY_EXTERNAL_MATERIALS;break;
        case 8:reason=RD_WHY_PREVIOUS_RITUAL;origin=RD_ORIGIN_CHAIN;break;
        case 9:reason=RD_WHY_PREVIOUS_LETHAL;origin=RD_ORIGIN_CHAIN;break;
        case 10:reason=RD_WHY_TRIBUTE_SETUP;origin=RD_ORIGIN_DUELISTS;break;
        case 11:reason=RD_WHY_FIELD_GUARD;origin=RD_ORIGIN_DUELISTS;break;
        case 12:reason=RD_WHY_FIELD_NO_SAFE;origin=RD_ORIGIN_DUELISTS;break;
        case 13:reason=RD_WHY_DECK_MAGIC_PULL;origin=RD_ORIGIN_DUELISTS;break;
        case 14:reason=RD_WHY_DECK_TRIBUTE_FIELD;origin=RD_ORIGIN_DUELISTS;break;
        case 15:reason=RD_WHY_DECK_TRIBUTE_HAND;origin=RD_ORIGIN_DUELISTS;break;
        case 16:reason=RD_WHY_POST_RITUAL_SUPPORT;origin=RD_ORIGIN_DUELISTS;break;
        case 17:reason=RD_WHY_DECK_POST_RITUAL_SUPPORT;origin=RD_ORIGIN_DUELISTS;break;
        default:
            reason=legal==0?RD_WHY_ENGINE_REJECTED:
                    legal<0?RD_WHY_NONE:
                    plan->score<0?RD_WHY_SCORE_LOW:RD_WHY_NO_TRIGGER;
            origin=selected>0?RD_ORIGIN_CHAIN:RD_ORIGIN_NONE;
            break;
    }
    changed=(turn!=diag.last_ai_turn||phase!=diag.phase||action!=diag.action||
             reason!=diag.reason||
             access.hand_slot!=diag.magic_hand||access.back_slot!=diag.magic_back||
             access.window_slot!=diag.magic_window||legal!=diag.legal||
             (legal>=0)!=diag.checked_legal ||
             Rd_AdaptiveRuleKey(&diag.rule_snapshot)!=Rd_AdaptiveRuleKey(&diag_current_rules));
    if(rd_setting("diagnostics",0)&&changed)
        host->log(host,"RD AI | Turn=%d phase=%s | source=%s | decision=%s | why=%s | magic=hand:%d back:%d window:%d | physical=%d logical=%d | local=%d checker=%s | score=%d selected=#%d",
           turn,hand?"HAND":"FIELD",Rd_OriginText((RdDecisionOrigin)origin),rd_diag_action(action),
           Rd_WhyText((RdDecisionWhy)reason),access.hand_slot+1,access.back_slot+1,
           access.window_slot+1,board->actual_hand,board->hand_count,
           plan->legal_set,legal<0?"NOT_QUERIED":legal?"YES":"NO",plan->score,selected);
    diag.ritual_magic=ritual_magic;diag.phase=phase;diag.hand_count=board->actual_hand;diag.logical_hand=board->hand_count;
    diag.magic_hand=access.hand_slot;diag.magic_back=access.back_slot;
    diag.magic_window=access.window_slot;
    diag.near=plan->near;diag.prep=plan->prep;diag.urgent=plan->urgent;
    diag.matched_materials=plan->matched_materials;diag.ritual_goal=plan->ritual_goal;
    diag.predicted_attack=plan->predicted_attack;diag.predicted_defense=plan->predicted_defense;
    diag.legal=legal;diag.checked_legal=legal>=0;diag.can_match=legal<0?-1:plan->legal_set;
    diag.score=plan->score;diag.trigger=plan->trigger;
    diag.field_count=fields;diag.back_count=back;
    diag.action=action;diag.selected_id=selected;
    diag.reason=reason;diag.origin=origin;
    /* Preserve the latest meaningful choice while routine AI evaluations
       continue. This is a selection/request, not proof of a summon. */
    noteworthy=action==2||action==3||action==4||action==8||action==9||action==10||action==11||action==12||action==13||action==14||action==15||action==16||action==17;
    if(noteworthy&&(diag.sticky_action!=action||diag.sticky_turn!=turn||
       diag.sticky_phase!=phase||diag.sticky_card!=selected)){
        diag.sticky_action=action;diag.sticky_turn=turn;diag.sticky_phase=phase;
        diag.sticky_card=selected;diag.sticky_origin=origin;
        if(rd_setting("diagnostics",0))
            host->log(host,"RD TRACE | T%d %s | %s | selected=#%d | WARNING: intent/selection only; confirm summon from gameplay",
                turn,phase==1?"HAND":"FIELD",rd_diag_action(action),selected);
    }
    diag.last_ai_turn=turn;
    diag.rule_snapshot=diag_current_rules;
    if(changed)diag.events++;
}
static void rd_diag_text(int x,int y,int scale,const char *s,unsigned color)
{
    host->draw_text(host,x,y,s,color,scale);
}
static void rd_overlay(void)
{
    int width=0,height=0,scale=1,level,lines=0,i,x,y,boxw,boxh,lineh;
    char text[196];
    if(!host||(level=rd_diag_level())==0)return;
    if(!diag.valid||gDuel_bOpponentID!=diag.duelist_id||
       (D_8009B368!=2&&D_8009B368!=6))return;
    host->overlay_size(host,&width,&height,&scale);
    if(scale<1)scale=1;
    lineh=16*scale;
    boxw=(level==2?385:335)*scale;
    if(boxw>width-12*scale)boxw=width-12*scale;
    if(boxw<150*scale)return;
    /* The frame is drawn fresh but reads only cached diagnostics. */
    lines=level==2?25:6;
    boxh=(lines*16+11)*scale;
    if(boxh>height-10*scale)boxh=height-10*scale;
    x=width-boxw-8*scale;y=8*scale;
    host->fill(host,x,y,boxw,boxh,0x071421,205);
    host->fill(host,x,y,boxw,3*scale,diag.fallback?0xE4A33B:0x4CCFC1,255);
    x+=9*scale;y+=13*scale;
#define RD_DIAG_LINE(COLOR) do {rd_diag_text(x,y,scale,text,(COLOR));y+=lineh;}while(0)
    snprintf(text,sizeof(text),"RD BETA v0.3.5b1 | %.18s",Duelists_Name(diag.duelist_id)); RD_DIAG_LINE(0x71E3D5);
    snprintf(text,sizeof(text),"%s / %s  | Duel #%u",diag.mode==2?"STORY":"FREE DUEL",rd_diag_tier(diag.tier),save_state.generated); RD_DIAG_LINE(0xFFFFFF);
    if(diag.fallback)snprintf(text,sizeof(text),"Deck: RETAIL FALLBACK | check recipe");
    else if(diag.restored)snprintf(text,sizeof(text),"Deck: restored save (hash unavailable)");
    else snprintf(text,sizeof(text),"Deck: %d/40 | pkg cards %d | %08x",diag.deck_size,diag.fixed_cards,diag.deck_fingerprint);
    RD_DIAG_LINE(0xFFFFFF);
    snprintf(text,sizeof(text),"Recipe: %s  | %d tributes",rd_diag_source(diag.recipe_source),diag.materials); RD_DIAG_LINE(diag.recipe_source?0xBEEFD8:0xFFC17A);
    if(level==2){
        if(diag.rule_snapshot.available)
            snprintf(text,sizeof(text),"Rules F>=%d H>=%d Deck:%s Result:%d",
                     diag.rule_snapshot.min_field,diag.rule_snapshot.min_hand,
                     (diag.rule_snapshot.sources&RITUAL_RULES_SOURCES_DECK)?"ON":"OFF",
                     diag.rule_snapshot.require_result);
        else snprintf(text,sizeof(text),"Rules: no bridge (legacy planner)");
        RD_DIAG_LINE(diag.rule_snapshot.available?0xBEEFD8:0xFFC17A);
    }
    snprintf(text,sizeof(text),"Deck status: %s",diag.fallback?"RETAIL FALLBACK / CHECK LOG":diag.restored?"RESTORED SAVE":"HYBRID GENERATED"); RD_DIAG_LINE(diag.fallback?0xFFC17A:0xBEEFD8);
    snprintf(text,sizeof(text),"AI: %s",rd_setting("ai_planner",1)?rd_diag_action(diag.action):"Planner OFF"); RD_DIAG_LINE(0xFFFFFF);
    if(level==2 && y+lineh*7<height){
        snprintf(text,sizeof(text),"Turn: %d  Phase: %s  Planner: %s",diag.last_ai_turn,diag.phase==1?"HAND":diag.phase==2?"FIELD":"WAIT",rd_setting("ai_planner",1)?"ON":"OFF"); RD_DIAG_LINE(0xFFFFFF);
        snprintf(text,sizeof(text),"Hand: %d physical / %d logical",diag.hand_count,diag.logical_hand); RD_DIAG_LINE(0xFFFFFF);
        snprintf(text,sizeof(text),"Field: %d  Back: %d",diag.field_count,diag.back_count); RD_DIAG_LINE(0xFFFFFF);
        if(diag.magic_hand>=0)snprintf(text,sizeof(text),"Ritual #%d: PHYSICAL hand slot %d",diag.ritual_magic,diag.magic_hand+1);
        else if(diag.magic_back>=0)snprintf(text,sizeof(text),"Ritual #%d: SET back-row slot %d",diag.ritual_magic,diag.magic_back+1);
        else if(diag.magic_window>=0)snprintf(text,sizeof(text),"Ritual #%d: LOGICAL window slot %d",diag.ritual_magic,diag.magic_window+1);
        else snprintf(text,sizeof(text),"Ritual Magic: not accessible / not observed");
        RD_DIAG_LINE(diag.magic_hand>=0||diag.magic_back>=0?0xBEEFD8:0xFFC17A);
        snprintf(text,sizeof(text),"Reason: %s",Rd_WhyText((RdDecisionWhy)diag.reason)); RD_DIAG_LINE(0xFFFFFF);
        snprintf(text,sizeof(text),"Decision from: %s",Rd_OriginText((RdDecisionOrigin)diag.origin)); RD_DIAG_LINE(0xFFFFFF);
        snprintf(text,sizeof(text),"Goal: %s  Setup: %s  Matched: %d/%d",diag.ritual_goal?"Y":"N",diag.prep?"Y":"N",diag.matched_materials,diag.materials); RD_DIAG_LINE(0xFFFFFF);
        snprintf(text,sizeof(text),"Local: %s  Engine: %s",!diag.checked_legal?"?":diag.can_match?"Y":"N",!diag.checked_legal?"NOT CHECKED":diag.legal?"Y":"N"); RD_DIAG_LINE(diag.checked_legal?0xFFFFFF:0xFFC17A);
        snprintf(text,sizeof(text),"Result ATK/DEF floor: %d/%d",diag.predicted_attack,diag.predicted_defense); RD_DIAG_LINE(0xFFFFFF);
        snprintf(text,sizeof(text),"Score: %d  Trigger: %d  Card: #%d",diag.score,diag.trigger,diag.selected_id); RD_DIAG_LINE(0xFFFFFF);
        if(diag.sticky_action)
            snprintf(text,sizeof(text),"Last choice T%d: %s",diag.sticky_turn,rd_diag_action(diag.sticky_action));
        else snprintf(text,sizeof(text),"Last choice: none observed");
        RD_DIAG_LINE(0xBEEFD8);
        snprintf(text,sizeof(text),"Last source: %s",Rd_OriginText((RdDecisionOrigin)diag.sticky_origin)); RD_DIAG_LINE(0xB3BBC5);
        snprintf(text,sizeof(text),"Choice != confirmed summon (use video)"); RD_DIAG_LINE(0xFFC17A);
        /* First three recipe roles. The full guarantee list goes to log. */
        for(i=0;i<3;i++){
            if(i<diag.materials)
                snprintf(text,sizeof(text),"Material %d: %s%d",i+1,diag.role_id[i]?"card #":"conditional group ",diag.role_id[i]?diag.role_id[i]:i+1);
            else snprintf(text,sizeof(text),"Material %d: --",i+1);
            RD_DIAG_LINE(0xB3BBC5);
        }
        /* Package contents are real cards; details and counts in log. */
        for(i=0;i<2;i++){
            int k,used=0,pos=0;
            pos+=snprintf(text+pos,sizeof(text)-pos,"Pkg cards: ");
            for(k=i*4;k<i*4+4&&k<RD_DECK_SIZE;k++){
                if(diag.fixed_id[k]<=0 || diag.fixed_qty[k]<=0)continue;
                used++;
                pos+=snprintf(text+pos,sizeof(text)-pos,"#%d x%d ",diag.fixed_id[k],diag.fixed_qty[k]);
                if(pos>=(int)sizeof(text)-16)break;
            }
            if(!used)snprintf(text,sizeof(text),"Pkg cards: %s",diag.restored?"not captured on load":"none");
            RD_DIAG_LINE(0xB3BBC5);
        }
        if(diag.materials>3 && y+lineh<height){snprintf(text,sizeof(text),"... %d more materials (see log)",diag.materials-3);RD_DIAG_LINE(0xB3BBC5);}
    }
#undef RD_DIAG_LINE
}

/* Mod overlay canvas is cached by the host while this signature is unchanged.
   Hash only values displayed; transient diagnostics.events deliberately does
   not affect it, so an unchanged panel costs no redraw every rendered frame. */
static unsigned rd_overlay_signature(void)
{
    unsigned hash=2166136261u;
    int i,level=rd_diag_level();
    const int values[]={
        level,diag.valid,diag.fallback,diag.restored,diag.tier,diag.mode,
        diag.recipe_source,diag.materials,diag.fixed_cards,diag.deck_size,
        diag.duelist_id,diag.last_ai_turn,diag.phase,diag.hand_count,
        diag.near,diag.prep,diag.urgent,diag.legal,diag.can_match,
        diag.ritual_goal,diag.matched_materials,diag.predicted_attack,diag.predicted_defense,
        diag.checked_legal,diag.magic_hand,diag.magic_back,diag.magic_window,
        diag.logical_hand,diag.reason,diag.origin,
        diag.sticky_action,diag.sticky_turn,diag.sticky_phase,diag.sticky_card,
        diag.sticky_origin,
        diag.score,diag.trigger,diag.field_count,diag.back_count,
        diag.rule_snapshot.available,diag.rule_snapshot.sources,
        diag.rule_snapshot.min_field,diag.rule_snapshot.min_hand,
        diag.rule_snapshot.require_result,
        diag.action,diag.selected_id,(int)diag.deck_fingerprint,
        (int)save_state.generated,(int)gDuel_bOpponentID,(int)D_8009B368,
        rd_setting("ai_planner",1)
    };
    if(!level || !diag.valid)return (unsigned)level;
    for(i=0;i<(int)(sizeof(values)/sizeof(values[0]));++i){
        hash^=(unsigned)values[i];hash*=16777619u;
    }
    for(i=0;i<RD_DECK_SIZE;i++){
        hash^=(unsigned)diag.fixed_id[i];hash*=16777619u;
        hash^=(unsigned)diag.fixed_qty[i];hash*=16777619u;
    }
    for(i=0;i<RD_MAX_RITUAL_MATERIALS;i++){
        hash^=(unsigned)diag.role_id[i];hash*=16777619u;
    }
    return hash ? hash : 1u;
}

static int rd_random(int maximum, void *context)
{
    (void)context;
    if(maximum<=0)return 0;
    save_state.seed_calls++;
    return Rand_GetInterval(maximum);
}
static int rd_setting(const char *key,int fallback)
{
    return host->setting(host,key,fallback);
}
static const RdOpponentProfile *rd_profile(int idx){
    if(idx==RD_SIMON_PROFILE){
        static RdOpponentProfile simon;
        const RdOpponentProfile *base=Rd_StarterThemeProfile(save_state.simon_theme);
        if(!base)return 0;
        simon=*base; /* original planner/effect callback, dynamic ritual theme */
        simon.duelist_name="Simon Muran";
        return &simon;
    }
    return Rd_GetProfile(idx);
}
RdStarterModal *Rd_StarterModal(void){return &save_state.starter_modal;}
/* New-game callback from the preserved Starter Choice selector.
   The opponent receives one of the other TWO full 40-card starter decks. */
void Rd_StarterBegin(void){
    Rd_DropRewardsNewCampaign();
    save_state.starter_ready=0;
    save_state.starter_theme=-1;
    save_state.simon_theme=-1;
    memset(save_state.simon_deck,0,sizeof(save_state.simon_deck));
    memset(recipe_ready,0,sizeof(recipe_ready));
}
int Rd_StarterAccept(int theme,const u16 vanilla[40],u16 result[40]){
    u16 cpu[40];int other,choice,mode;
    if(!host||theme<0||theme>=3||!vanilla||!result)return 0;
    mode=rd_setting("recipe_source",0);
    if(!Rd_BuildThemedStarter(host,theme,mode,vanilla,result,rd_random,0))return 0;
    choice=Rand_GetInterval(2);
    other=Rd_StarterSimonTheme(theme,choice);
    if(!Rd_BuildThemedStarter(host,other,mode,vanilla,cpu,rd_random,0))return 0;
    memcpy(save_state.simon_deck,cpu,sizeof(cpu));
    save_state.starter_theme=theme;
    save_state.simon_theme=other;
    save_state.starter_ready=1;
    memset(recipe_ready,0,sizeof(recipe_ready));
    if(rd_setting("diagnostics",0))host->log(host,
       "RD NEW GAME | player theme=%d | Simon Muran theme=%d (one of remaining two)",theme,other);
    return 1;
}
static int rd_profile_for_opponent(void)
{
    int i,id;
    if(!host)return -1;
    id=gDuel_bOpponentID;
    if(id<1)return -1;
    if(save_state.starter_ready && save_state.simon_theme>=0 && save_state.simon_theme<3){
        if(simon_id==-2)simon_id=Duelists_Named("Simon Muran");
        if(simon_id>0 && id==simon_id && rd_setting("expansion_enabled",1))
            return RD_SIMON_PROFILE;
    }
    for(i=0;i<Rd_ProfileCount();++i) {
        const RdOpponentProfile *p=Rd_GetProfile(i);
        if(cached_id[i]<=0){cached_id[i]=Duelists_Named(p->duelist_name);}
        if(id==cached_id[i] && id>0){
            /* Isis is fully Vanilla in this round, including deck, AI and
               ritual guarantees. The inactive profile 0 is audit data only. */
            if(i==0)return -1;
            if(!rd_setting("expansion_enabled",1))return -1;
            return i;
        }
    }
    return -1;
}
static const RdRitualRecipe *rd_active_recipe(int idx)
{
    int choice;
    if(idx<0||idx>=RD_PROFILE_CAPACITY)return 0;
    if(recipe_ready[idx])return recipe_cache[idx].status?&recipe_cache[idx].recipe:0;
    choice=rd_setting("recipe_source",0);
    if(choice<0||choice>2)choice=0;
    recipe_ready[idx]=1;
    if(Rd_ResolveRecipe(host,rd_profile(idx),choice,&recipe_cache[idx])){
        const int result_id=recipe_cache[idx].recipe.result_card;
        if(Cards_Valid(result_id)){
            const unsigned stats=(unsigned)gDuel_adwCardStats[result_id-1];
            recipe_cache[idx].recipe.result_attack=(int)(stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
            recipe_cache[idx].recipe.result_defense=(int)((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
        }
        /* Material selection happens atomically inside deck generation.
           Cache only the active recipe, never old difficulty guarantees. */
    }else if(rd_setting("diagnostics",0))
        host->log(host,"Ritual Duelists: no accessible active recipe for %s",
                  rd_profile(idx)->duelist_name);
    return recipe_cache[idx].status?&recipe_cache[idx].recipe:0;
}
static void rd_deal(s32 src,u8 *out16,u8 *out8)
{
    if(src==0 && (D_8009B368==2 || D_8009B368==6)) Rd_DropRewardsNewDuel();
    int profile_index=rd_profile_for_opponent(),mode=(int)D_8009B368;
    /* src!=0 is the player's deck (or other fixed list); never touch it. */
    if(src==0 && profile_index>=0 && (mode==2||mode==6))
        memset(ritual_marks,0,sizeof(ritual_marks));
    if(src!=0||profile_index<0||!out16||!out8|| (mode!=2 && mode!=6)){
        ((void (*)(s32,u8*,u8*))previous_deal)(src,out16,out8);
        return;
    }
    {
        u16 cards[RD_DECK_SIZE];
        const RdOpponentProfile *p=rd_profile(profile_index);
        const RdRitualRecipe *recipe=rd_active_recipe(profile_index);
        RdPackageStats packs={0};
        int i,generated=0;
        /* Missing configuration MUST mean OFF: no silent Free Duel guarantee. */
        int guarantee_free=rd_setting("guaranteed_package_free",0)==1;
        if(profile_index==RD_SIMON_PROFILE && save_state.starter_ready){
            memcpy(cards,save_state.simon_deck,sizeof(cards));
            generated=40; /* complete themed starter generated at campaign creation */
        }else if(recipe)
            generated=Rd_GeneratePackageDeck(p,recipe,Rd_MatchRecipeCard,
                       &recipe_cache[profile_index],gCard_nCount,mode,
                       guarantee_free,cards,rd_random,0,&packs);
        if(!generated && rd_setting("diagnostics",0))
            host->log(host,"Ritual Duelists: recipe unknown/infeasible; stock duelist deck retained");
        if(generated!=40){
            rd_diag_deck(profile_index,mode,&packs,0,0,1);
            ((void (*)(s32,u8*,u8*))previous_deal)(src,out16,out8);
            if(rd_setting("diagnostics",0))host->log(host,"Ritual Duelists: fallback to original deck (recipe or profile could not be adapted)");
            return;
        }
        for(i=0;i<RD_DECK_SIZE;i++){((u16 *)out16)[i]=cards[i];out8[i]=(u8)i;}
        /* Match vanilla's 160 paired swaps: shuffle-index metadata and IDs
           remain associated. The generator's first shuffle does not change
           the uniformity of the final permutation. */
        for(i=0;i<DUEL_DECK_SHUFFLE_SWAP_COUNT;i++){
            int a=Rand_GetInterval(RD_DECK_SIZE),b=Rand_GetInterval(RD_DECK_SIZE);
            u16 *ids=(u16 *)out16,old_id=ids[a];u8 old_idx=out8[a];
            ids[a]=ids[b];ids[b]=old_id;out8[a]=out8[b];out8[b]=old_idx;
        }
        deck_pull.active=0;
        legal_cache.valid=0; /* New duel, new deck and sourcing state. */
        plan_cache.valid=0;
        rd_diag_deck(profile_index,mode,&packs,cards,generated,0);
        save_state.profile=(unsigned)packs.packages;
        save_state.mode=(unsigned)mode;
        save_state.opp_id=(unsigned)gDuel_bOpponentID;
        save_state.generated++;
        planned_turns=0;
        last_turn_seen=-1;
        if(rd_setting("diagnostics",0))host->log(host,"Ritual Duelists: %s mode=%d packages=%d deck=40 recipe=%d roles=%d",p->duelist_name,mode,packs.packages,recipe_cache[profile_index].source,recipe?recipe->material_count:-1);
    }
}
static int rd_card_is_valid(int id)
{
    /* Future card packs may use IDs beyond the original 722 cards. */
    return id>0 && Cards_Valid(id);
}
static RdCard rd_make_card(const AiActiveCard *card)
{
    RdCard c;unsigned stats;memset(&c,0,sizeof(c));
    if(!rd_card_is_valid(card->card_id))return c;
    c.id=card->card_id;
    stats=(unsigned)gDuel_adwCardStats[c.id-1];
    c.atk=(int)(stats&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    c.def=(int)((stats>>CARD_STAT_DEFENSE_SHIFT)&CARD_STAT_VALUE_MASK)*CARD_STAT_SCALE;
    c.type=(int)((stats>>CARD_STAT_TYPE_SHIFT)&CARD_STAT_TYPE_MASK);
    c.level=Cards_Level(c.id);
    /* Enemy face-down identity is never consulted by our evaluator. */
    c.face_down=(card->flags & 0x1000)!=0; /* conceal hidden opponent information */
    c.used=(card->flags & 0x4000)!=0; /* card has acted this turn */
    return c;
}
static RdBoard rd_snapshot(void)
{
    RdBoard b;int i,side=D_8009B1D5,limit=0;
    memset(&b,0,sizeof(b));
    for(i=0;i<5;i++){
        b.own[i]=rd_make_card(&gDuel_aActiveCards[1+i]);
        b.enemy[i]=rd_make_card(&gDuel_aActiveCards[56+i]);
        b.back[i]=rd_make_card(&gDuel_aActiveCards[6+i]);
        /* Board tactics must use current effective power, including the
           bonus of a Ritual, rather than the printed base values. */
        if(b.own[i].id){
            b.own[i].atk=gDuel_aActiveCards[1+i].attack;
            b.own[i].def=gDuel_aActiveCards[1+i].defense;
        }
        if(b.enemy[i].id && !b.enemy[i].face_down){
            b.enemy[i].atk=gDuel_aActiveCards[56+i].attack;
            b.enemy[i].def=gDuel_aActiveCards[56+i].defense;
        }
    }
    for(i=0;i<5;i++)if(D_800E9FF0[side].hand[i]>=0)limit++;
    b.actual_hand=limit;
    /* Ai_GetHandSize may be overridden by AI Hard Mode; clamp to 20 like it
       does, without scanning cards beyond its real configured window. */
    limit=Ai_GetHandSize();if(limit<0)limit=0;if(limit>RD_WINDOW)limit=RD_WINDOW;
    if(limit<b.actual_hand)limit=b.actual_hand;
    b.hand_count=limit;
    for(i=0;i<limit;i++)b.hand[i]=rd_make_card(&gDuel_aActiveCards[11+i]);
    b.own_lp=D_800E9FF0[side].life_points.unsigned_value;
    b.enemy_lp=D_800E9FF0[side^1].life_points.unsigned_value;
    b.turns_waited=planned_turns;
    return b;
}
/* Melhoria dos Rituais can search many different Field/Hand/Deck triples
   when probing a Ritual, especially when the recipe lives behind another
   mod's hook. Cache the result per *physical duel situation*, not per frame
   or AI VM instruction. Reads here are bounded (40 deck records, 20 hand,
   5 zones); the expensive engine oracle runs only when the state changes. */
static unsigned rd_mix(unsigned h,unsigned word){return (h^word)*16777619u;}
static unsigned rd_legal_key(const RdBoard *b,int magic,int include_deck)
{
    unsigned key=2166136261u;
    int i,side=D_8009B1D5;
    key=rd_mix(key,(unsigned)gDuel_bOpponentID);
    key=rd_mix(key,(unsigned)side);
    key=rd_mix(key,(unsigned)magic);
    key=rd_mix(key,(unsigned)D_800E9FF0[side].rank.turns_taken);
    key=rd_mix(key,(unsigned)D_800E9FF0[side].deck_draw_cursor);
    key=rd_mix(key,(unsigned)b->hand_count);
    key=rd_mix(key,(unsigned)b->actual_hand);
    key=rd_mix(key,(unsigned)b->own_lp);
    key=rd_mix(key,(unsigned)b->enemy_lp);
    for(i=0;i<5;i++){
        key=rd_mix(key,(unsigned)b->own[i].id);
        key=rd_mix(key,(unsigned)b->own[i].atk);
        key=rd_mix(key,(unsigned)b->own[i].def);
        key=rd_mix(key,(unsigned)b->own[i].used);
        key=rd_mix(key,(unsigned)b->own[i].face_down);
        key=rd_mix(key,(unsigned)b->back[i].id);
        key=rd_mix(key,(unsigned)b->back[i].used);
        key=rd_mix(key,(unsigned)b->enemy[i].id);
        key=rd_mix(key,(unsigned)b->enemy[i].atk);
        key=rd_mix(key,(unsigned)b->enemy[i].def);
        key=rd_mix(key,(unsigned)b->enemy[i].face_down);
    }
    for(i=0;i<b->hand_count;i++)key=rd_mix(key,(unsigned)b->hand[i].id);
    /* The deck may be altered by other mods without advancing its cursor.
       Read both identity and deck index to invalidate a former permission. */
    if(include_deck)for(i=0;i<RD_DECK_SIZE;i++){
        const DuelDeckCardRecord *c=&gDuel_aDeckCardRecords[side*RD_DECK_SIZE+i];
        key=rd_mix(key,(unsigned)c->id);
        key=rd_mix(key,(unsigned)c->deck_index);
    }
    return key;
}
static int rd_legal_cached(int ritual,unsigned key)
{
    if(legal_cache.valid && legal_cache.key==key && legal_cache.ritual==ritual)
        return legal_cache.result;
    legal_cache.result=Duel_CheckRitual(0,ritual)!=0;
    legal_cache.key=key;
    legal_cache.ritual=ritual;
    legal_cache.valid=1;
    return legal_cache.result;
}

/* Search for a best tribute assignment only once per unchanged board,
   not whenever the AI's bytecode asks for a fresh action. */
static RdPlan rd_plan_cached(int idx,const RdOpponentProfile *p,const RdRitualRecipe *r,
                             const RdBoard *b,int mode,int legal,unsigned key,
                             const RdAdaptiveRules *rules)
{
    key=rd_mix(key,Rd_AdaptiveRuleKey(rules));
    key=rd_mix(key,(unsigned)(idx+1)); /* Jono / Jono 2nd share Ritual Magic */
    key=rd_mix(key,(unsigned)mode);
    key=rd_mix(key,(unsigned)legal);
    key=rd_mix(key,(unsigned)b->turns_waited);
    if(plan_cache.valid && plan_cache.key==key)return plan_cache.result;
    /* Use the exact same condition matcher as deck construction, including
       official fusion groups, copied cards and printed ATK/DEF. */
    {
        RdRitualRecipe runtime_recipe=*r;
        /* Melhoria authorizes Field+Hand independently of a base table's
           source declaration. Apply its live policy without rewriting the
           actual recipe and without changing the summoning checker. */
        if(rules->available && rules->material_count==r->material_count)
            runtime_recipe.material_sources=3;
        plan_cache.result=Rd_PlanRitualMatched(p,&runtime_recipe,b,mode,legal,
                            Rd_MatchRecipeCard,&recipe_cache[idx]);
    }
    Rd_RefineRitualPlan(p,r,b,legal,Rd_MatchRecipeCard,&recipe_cache[idx],
                        rules,&plan_cache.result);
    plan_cache.key=key;
    plan_cache.valid=1;
    return plan_cache.result;
}

/* Default vanilla-style CPU advantage for all registered ritualists:
   search up to twenty actual upcoming cards, but never phantom/exhausted
   slots. Other AI mods can still hook/override this separately. */
static s32 rd_ai_window(void)
{
    int side,physical=0,i,remaining;
    s32 original=((s32 (*)(void))previous_window)();
    if(!host || !rd_setting("ai_planner",1) ||
       rd_profile_for_opponent()<0)return original;
    side=D_8009B1D5;
    if(side!=0 && side!=1)return original;
    if(gAiScript_State.script_base!=D_801A8000 &&
       gAiScript_State.script_base!=D_801A9800)return original;
    for(i=0;i<5;i++)physical+=D_800E9FF0[side].hand[i]>=0;
    remaining=RD_DECK_SIZE-(int)D_800E9FF0[side].deck_draw_cursor;
    return Rd_DefaultAiWindow(physical,remaining);
}

static int rd_active_ai(void)
{
    int side=D_8009B1D5;
    if(side>=2||gDuel_bOpponentID<1||D_8009B360[side]<0)return 0;
    return gAiScript_State.script_base==D_801A8000||gAiScript_State.script_base==D_801A9800;
}
/* The stock game swaps selected cards from the upcoming Deck into the first
   unselected physical hand position. For OUR Ritual fetch only, perform the
   same native record/display exchange in a safe, unreserved physical slot.
   Every other AI action retains the original hook chain unchanged. */
static int rd_swap_safe_selection(int replace,int physical)
{
    int i;
    if(D_800EAE88[0]<11 || D_800EAE88[0]>=11+physical)return 0;
    for(i=0;i<5 && D_800EAE88[i];i++){
        if(D_800EAE88[i]<11 || D_800EAE88[i]>=11+physical ||
           D_800EAE88[i]==11+replace)return 0;
    }
    return 1;
}
#include "mandatory_hand_consumer.h"
static void rd_materialize_plan(void)
{
    int side,src,replace,index,held_index,record_id,mode,card,target;
    DuelDeckCardRecord *a,*other;
    DuelCardDisplayObject *old;
    if(!deck_pull.active){((void (*)(void))previous_swap)();return;}
    side=deck_pull.side;src=deck_pull.source;replace=deck_pull.replace;
    mode=deck_pull.mode;card=deck_pull.card;target=deck_pull.target;
    deck_pull.active=0; /* exactly once, even on failures */
    if(D_8009B1D5!=side || side<0 || side>1 ||
       src<16 || src>55 || replace<0 || replace>=5 ||
       D_800E9FF0[side].hand[replace]<0 ||
       gDuel_aActiveCards[src].card_id!=card ||
       !D_800EA030[replace].object){
        goto fail;
    }
    if(mode==RD_PULL_HOLD){
        int physical=0,i;
        for(i=0;i<5;i++)physical+=D_800E9FF0[side].hand[i]>=0;
        /* The old AI action stays intact; do not exchange its source or
           intercept an earlier mod's own virtual Deck materialization. */
        if(!rd_swap_safe_selection(replace,physical)){
            goto fail;
        }
    }else if((mode==RD_PULL_MAGIC || mode==RD_PULL_FIELD) &&
        (D_800EAE88[0]!=src || D_800EAE88[1]!=0 ||
         (mode==RD_PULL_MAGIC && D_800EAE88[8]!=1) ||
         D_800EAE88[6]!=target ||
         (mode==RD_PULL_MAGIC ? (target<6||target>10) :
                                (target<1||target>5)))){
        goto fail;
    }
    index=gDuel_aActiveCards[src].deck_index;
    held_index=D_800E9FF0[side].hand[replace];
    if(index<side*RD_DECK_SIZE+D_800E9FF0[side].deck_draw_cursor ||
       index>=(side+1)*RD_DECK_SIZE ||
       held_index<side*RD_DECK_SIZE || held_index>=(side+1)*RD_DECK_SIZE ||
       index==held_index || gDuel_aDeckCardRecords[index].id!=card){
        goto fail;
    }
    a=&gDuel_aDeckCardRecords[held_index];other=&gDuel_aDeckCardRecords[index];
    Rd_ExchangeDeckRecords(a,other);
    record_id=D_800907CC[replace+side*5];
    old=(DuelCardDisplayObject *)D_800EA030[replace].object;
    Duel_SetupCardRecord(record_id,a->deck_index);
    D_800EA030[replace].object=(u8 *)func_80018004(&D_801A7AD8[record_id],old->out_x,old->out_y);
    DisplayObject_ReleaseIfPresent(old);
    D_800E9FF0[side].hand[replace]=a->deck_index;
    if(mode==RD_PULL_HOLD){
        /* Previously selected physical play remains selected. The fetched
           material stays in an entirely different physical hand position. */
        ((void (*)(void))previous_swap)();
    }else D_800EAE88[0]=(u8)(replace+11);
    if(rd_setting("diagnostics",0))
        host->log(host,"RD Deck fetch: card #%d from real undrawn index %d into physical hand %d; %s",
                  card,index,replace+1,mode==RD_PULL_HOLD?"RETAIN IN HAND":
                               mode==RD_PULL_FIELD?"SET TRIBUTE": "SET RITUAL");
    return;
fail:
    /* A stale snapshot must never cause an unverified Deck card to be
       played. Restore the earlier AI choice before its native swap. */
    if(mode==RD_PULL_MAGIC || mode==RD_PULL_FIELD)
        memcpy((void *)D_800EAE88,deck_pull.fallback,12);
    ((void (*)(void))previous_swap)();
}

static void rd_materialize(void)
{
    if(rd_selection_precommit()<0)return;
    /* A retained-material exchange owns its own commit. No late validator
       may interpret the intentional physical swap as an external rewrite. */
    if(deck_pull.active)rd_selection_tx.state=0;
    rd_materialize_plan();
    rd_selection_tx.state=0;
}

static int rd_stage_tribute_fetch(const RdTributeFetch *pick,int side)
{
    int index;
    if(!pick || pick->mode==RD_FETCH_NONE || pick->card_id<=0 ||
       pick->source_slot<16 || pick->source_slot>55 ||
       pick->replace_slot<0 || pick->replace_slot>=5)return 0;
    index=gDuel_aActiveCards[pick->source_slot].deck_index;
    if(index<side*RD_DECK_SIZE+D_800E9FF0[side].deck_draw_cursor ||
       index>=(side+1)*RD_DECK_SIZE ||
       gDuel_aActiveCards[pick->source_slot].card_id!=pick->card_id ||
       gDuel_aDeckCardRecords[index].id!=pick->card_id)return 0;
    memcpy(deck_pull.fallback,(const void *)D_800EAE88,12);
    deck_pull.active=1;
    deck_pull.side=side;deck_pull.source=pick->source_slot;
    deck_pull.replace=pick->replace_slot;
    deck_pull.card=pick->card_id;
    deck_pull.mode=pick->mode==RD_FETCH_FIELD?RD_PULL_FIELD:RD_PULL_HOLD;
    deck_pull.target=pick->destination;
    return 1;
}


/* Recognize the actual ritual's placement transition, not merely a card ID
   that came from a deck pool. The game's 3|0x80 -> 4/5 transition is the
   frame that commits func_80024D34 + MonsterEffects_Placed. Efeitos de
   Rituais also hooks this function; call its chain exactly once. */
static void rd_ritual_commit(void)
{
    unsigned before=(unsigned)D_8009B210;
    int index=(int)D_8009B19C;
    int result_card=(int)D_8009B1A0;
    ((void (*)(void))previous_ritual)();
    if((before&15u)==3u && (before&0x80u) &&
       ((D_8009B210&15u)==4u || (D_8009B210&15u)==5u) &&
       index>=0 && index<RD_AI_RECORDS && result_card>0){
        DuelCardRecord *c=&D_801A7AD8[index];
        if((c->flags&DUEL_CARD_FLAG_OCCUPIED) && c->data &&
           c->card_id==result_card){
            RdAi_MarkRitual(ritual_marks,index,c->card_id,
                         ((DuelDeckCardRecord *)c->data)->deck_index);
            if(host && rd_setting("diagnostics",0))
                host->log(host,"RD AI law: actual Ritual summoned card #%d record=%d origin=%d",
                          c->card_id,index,(int)((DuelDeckCardRecord *)c->data)->deck_index);
        }
    }
}
static int rd_tagged_field(int record)
{
    const RdAiRitualMark *mark;
    const DuelCardRecord *c;
    if(record<0||record>=RD_AI_RECORDS)return 0;
    mark=&ritual_marks[record];c=&D_801A7AD8[record];
    return mark->active && (c->flags&DUEL_CARD_FLAG_OCCUPIED) && c->data &&
           c->card_id==mark->card_id &&
           ((const DuelDeckCardRecord *)c->data)->deck_index==mark->origin;
}
/* Global AI fusion veto, using the actual field record to check provenance.
   The original Duel_CheckFusion and any other MODs stay in the hook chain.
   A normal copy of the same ID is conservatively held while a Ritual copy is
   alive because the retail checker takes only two IDs, not record pointers.
   The CPU check remains active during the *execution* of a selected fusion,
   not only during the AiScript_Run planning call. */
static s32 rd_fusion_decision(s32 first,s32 second)
{
    if(D_8009B1D5<DUEL_SIDE_COUNT && D_8009B360[D_8009B1D5]>=0 &&
       host && rd_profile_for_opponent()>=0){
        int side=(int)D_8009B1D5,j;
        for(j=0;j<5;j++){
            int rec=D_800907D8[side*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+
                                 DUEL_FIELD_ROW_SIZE+j];
            if(rd_tagged_field(rec) &&
               (D_801A7AD8[rec].card_id==first ||
                D_801A7AD8[rec].card_id==second)){
                return 0;
            }
        }
    }
    return ((s32 (*)(s32,s32))previous_fusion)(first,second);
}
static s32 rd_ai_plan(void)
{
    s32 result=((s32 (*)(void))previous_ai)();
    int idx,hand,mode,legal=0,j,has_ritual=0,selected_id=0;
    RdMagicAccess magic_access;
    RdAdaptiveRules ritual_rules;
    unsigned state_key;
    const RdOpponentProfile *profile;
    const RdRitualRecipe *recipe;
    RdBoard board;RdPlan plan;RdPostPlan post;
    if(!host||!rd_active_ai()||!rd_setting("ai_planner",1))return result;
    idx=rd_profile_for_opponent();if(idx<0)return result;
    hand=(gAiScript_State.script_base==D_801A8000);
    if(!result)return result;
    profile=rd_profile(idx);
    recipe=rd_active_recipe(idx);
    /* May be loaded after us, disabled, or toggled in Settings. A disabled
       exporter stays in memory, so availability alone is NOT enough. */
    Rd_ConnectEffectIntelligence(host->find ?
        (const RitualEffectIntelligenceV1 *)host->find(host,
           "ritual-card-effects-test:effect_intelligence_v1") : 0);
    mode=recipe && Rd_EffectIntelligenceEnabled(recipe->result_card)?0:1;
    /* The real hook chain (Efeitos/Melhoria/Retail) is authoritative about
       recipe legality and tribute locations. It is never bypassed here. */
    board=rd_snapshot();
    /* The extended logical hand may include the undrawn Deck. A Ritual
       found only there is NOT playable from hand. It is pointless (and
       potentially expensive with Melhoria) to probe the oracle for it. */
    Rd_FindRitualMagic(&board,recipe?recipe->ritual_magic:profile->recipe.ritual_magic,&magic_access);
    has_ritual=magic_access.hand_slot>=0 || magic_access.back_slot>=0;
    if(!recipe){
        RdPlan zero;memset(&zero,0,sizeof(zero));
        rd_diag_ai(hand,&board,&zero,-1,5,0,recipe?recipe->ritual_magic:0);
        return result;
    }
    /* One physical-state fingerprint is shared by the legality and planner
       caches; no duplicate 40-card-deck scan for the same AI decision. */
    /* Only an actually playable Ritual requires expensive Deck-source
       invalidation for the authority check. Strategic setup still uses a
       lightweight physical-state fingerprint on every AI decision. */
    Rd_ReadAdaptiveRules(host,&ritual_rules);
    diag_current_rules=ritual_rules;
    state_key=rd_legal_key(&board,recipe->ritual_magic,has_ritual);
    /* Live changes to another mod's Settings must invalidate BOTH caches.
       The game's Ritual checker continues to be the final authority. */
    state_key=rd_mix(state_key,Rd_AdaptiveRuleKey(&ritual_rules));
    legal=has_ritual?rd_legal_cached(recipe->ritual_magic,state_key):-1;
    if(hand){
        int turn=D_800E9FF0[D_8009B1D5].rank.turns_taken;
        if(last_turn_seen!=turn){
            RdPlan current=rd_plan_cached(idx,profile,recipe,&board,mode,legal==1,state_key,&ritual_rules);
            planned_turns=current.ritual_goal?planned_turns+1:0;
            last_turn_seen=turn;
            board.turns_waited=planned_turns;
        }
    }
    plan=rd_plan_cached(idx,profile,recipe,&board,mode,legal==1,state_key,&ritual_rules);
    /* Separate post-summon plan: the same recipe/profile Beacon callback now
       evaluates a REAL already-summoned result, not a hypothetical summon. */
    post=Rd_PlanPostRitual(profile,recipe,&board,mode);
    if(hand && D_800EAE88[0]>=11 && D_800EAE88[0]<11+board.actual_hand)
        selected_id=board.hand[D_800EAE88[0]-11].id;
    if(hand){
        int selected=(u8)D_800EAE88[0]-11;
        /* Once a Ritual exists, evaluate its live Beacon synergy before
           yielding to the general-purpose AI. Favor a strong support from
           the physical hand on the next legal hand action. The earlier
           hand-phase choice is never asked to play another card mid-turn. */
        if(post.active && post.hand_slot>=0 && post.destination>=0 &&
           post.score>0 && !plan.urgent){
            int chosen=post.hand_slot;
            int target=post.destination+1;
            int enemy_present=0,lethal=0,k;
            for(k=0;k<5;k++)if(board.enemy[k].id)enemy_present=1;
            if(!enemy_present && selected>=0 && selected<board.actual_hand &&
               board.enemy_lp>0 && board.hand[selected].atk>=board.enemy_lp)
                lethal=1;
            if(lethal)goto post_physical_skipped;
            /* A previous AI play of the same support into a free monster
               zone is already good. Preserve its other valid parameters. */
            if(selected==chosen && D_800EAE88[6]>=1 && D_800EAE88[6]<=5 &&
               !board.own[D_800EAE88[6]-1].id){
                rd_diag_ai(hand,&board,&plan,legal,16,board.hand[chosen].id,recipe->ritual_magic);
                return result;
            }
            Rd_SelectFaceDownHandPlay((unsigned char *)D_800EAE88,chosen,target);
            if(post.face_up)D_800EAE88[8]=0;
            rd_diag_ai(hand,&board,&plan,legal,16,board.hand[chosen].id,recipe->ritual_magic);
            if(rd_setting("diagnostics",0))
                host->log(host,"RD Post-Ritual: field support #%d for Ritual #%d; predicted Beacon gain ATK %d DEF %d; slot=%d face_up=%d",
                          board.hand[chosen].id,recipe->result_card,
                          post.attack_gain,post.defense_gain,target,post.face_up);
            return 1;
        }
post_physical_skipped:
        /* If the physical hand offers no useful Beacon support, reuse the
           CPU's existing real Deck-card swap, not the Ritual engine's
           tribute-from-Deck setting. Never trade away a protected tribute,
           another Ritual Magic, or the only support already held. */
        if(post.active && post.hand_slot<0 && post.destination>=0 &&
           !plan.urgent && board.actual_hand>0 &&
           !(D_800EAE88[0]>=16 && D_800EAE88[0]<=55)){
            int left=RD_DECK_SIZE-(int)D_800E9FF0[D_8009B1D5].deck_draw_cursor;
            int best=0,src=-1,replace=-1,chosen_face=0;
            if(left<0)left=0;if(left>RD_DECK_SIZE)left=RD_DECK_SIZE;
            for(j=0;j<board.actual_hand;j++){
                int k;
                int hold=board.hand[j].id,protected_card=plan.protect_hand[j];
                if(hold==recipe->ritual_magic || hold==recipe->result_card)continue;
                /* Avoid losing existing Beacon support during the swap. */
                if(profile->skull_count && Rd_IsSkull(profile,hold))continue;
                if(protected_card)continue;
                {int u;int selected_held=0;
                 for(u=0;u<5 && D_800EAE88[u];u++)
                     if(D_800EAE88[u]==11+j)selected_held=1;
                 if(selected_held)continue;}
                for(k=0;k<left;k++){
                    int slot=11+board.actual_hand+k,index,score,face=0;
                    RdCard candidate;
                    if(slot<16||slot>55)continue;
                    candidate=rd_make_card(&gDuel_aActiveCards[slot]);
                    if(candidate.id<=0 || candidate.type>=CARD_TYPE_MAGIC)continue;
                    index=gDuel_aActiveCards[slot].deck_index;
                    if(index<D_8009B1D5*RD_DECK_SIZE+
                          D_800E9FF0[D_8009B1D5].deck_draw_cursor ||
                       index>=(D_8009B1D5+1)*RD_DECK_SIZE ||
                       gDuel_aDeckCardRecords[index].id!=candidate.id)continue;
                    score=Rd_PostSupportScore(profile,recipe,&board,mode,j,
                                             &candidate,post.destination,&face);
                    /* A Deck swap really displaces a held card. Avoid
                       trading a powerful combat option for a marginal buff. */
                    {int power=board.hand[j].atk>board.hand[j].def?
                               board.hand[j].atk:board.hand[j].def;
                     if(power>0)score-=power/3;}
                    if(score>best){best=score;src=slot;replace=j;chosen_face=face;}
                }
            }
            if(src>=16 && replace>=0){
                memcpy(deck_pull.fallback,(const void *)D_800EAE88,12);
                Rd_SelectFaceDownHandPlay((unsigned char *)D_800EAE88,
                                          src-11,post.destination+1);
                if(chosen_face)D_800EAE88[8]=0;
                deck_pull.active=1;deck_pull.side=D_8009B1D5;
                deck_pull.source=src;deck_pull.replace=replace;
                deck_pull.card=gDuel_aActiveCards[src].card_id;
                deck_pull.mode=RD_PULL_FIELD;deck_pull.target=post.destination+1;
                rd_diag_ai(hand,&board,&plan,legal,17,deck_pull.card,recipe->ritual_magic);
                if(rd_setting("diagnostics",0))
                    host->log(host,"RD Post-Ritual: fetch Beacon support #%d from real Deck slot=%d, score=%d, replacement hand=%d",
                              deck_pull.card,src,best,replace+1);
                return 1;
            }
        }
        /* Default CPU advantage: if Ritual Magic is still undrawn, select its REAL
           existing copy from the remaining shuffled Deck. The original game
           already supports a deck-sourced hand action; the materializer
           below only chooses a safe hand card to exchange instead of
           blindly discarding a protected Ritual tribute. No source rule
           from Melhoria dos Rituais is changed by fetching the MAGIC. */
        if(plan.ritual_back_slot<0 && plan.ritual_hand_slot<0 &&
           board.actual_hand>0 && !plan.urgent){
            int upcoming[RD_DECK_SIZE]={0};
            int left=RD_DECK_SIZE-(int)D_800E9FF0[D_8009B1D5].deck_draw_cursor;
            RdDeckPick pick;
            if(left<0)left=0;if(left>RD_DECK_SIZE)left=RD_DECK_SIZE;
            for(j=0;j<left;j++){
                int src=11+board.actual_hand+j;
                if(src>55)break;
                upcoming[j]=gDuel_aActiveCards[src].card_id;
            }
            pick=Rd_ChooseDeckRitual(&board,&plan,recipe->ritual_magic,upcoming,left);
            if(pick.source_slot>=16){
                int deck_index=gDuel_aActiveCards[pick.source_slot].deck_index;
                if(deck_index>=D_8009B1D5*RD_DECK_SIZE+
                                 D_800E9FF0[D_8009B1D5].deck_draw_cursor &&
                   deck_index<(D_8009B1D5+1)*RD_DECK_SIZE &&
                   gDuel_aDeckCardRecords[deck_index].id==recipe->ritual_magic){
                    memcpy(deck_pull.fallback,(const void *)D_800EAE88,12);
                    Rd_SelectFaceDownHandPlay((unsigned char *)D_800EAE88,
                                              pick.source_slot-11,pick.target_slot);
                    deck_pull.active=1;
                    deck_pull.side=D_8009B1D5;
                    deck_pull.source=pick.source_slot;
                    deck_pull.replace=pick.replace_slot;
                    deck_pull.card=recipe->ritual_magic;
                    deck_pull.mode=RD_PULL_MAGIC;deck_pull.target=pick.target_slot;
                    rd_diag_ai(hand,&board,&plan,legal,13,recipe->ritual_magic,recipe->ritual_magic);
                    return 1;
                }
            }
        }
        /* Never interfere if the previously hooked AI already picked the
           same Ritual. This preserves AI Hard Mode and other planners. */
        if(selected>=0 && selected<board.actual_hand &&
           board.hand[selected].id==recipe->ritual_magic){
            rd_diag_ai(hand,&board,&plan,legal,8,recipe->ritual_magic,recipe->ritual_magic);
            return result;
        }
        if(plan.trigger==1 && plan.ritual_hand_slot>=0 && plan.ritual_hand_slot<board.actual_hand){
            Rd_SelectFaceDownHandPlay((unsigned char *)D_800EAE88,
                                      plan.ritual_hand_slot,
                                      6+plan.ritual_back_destination);
            rd_diag_ai(hand,&board,&plan,legal,2,board.hand[plan.ritual_hand_slot].id,recipe->ritual_magic);
            if(rd_setting("diagnostics",0))host->log(host,"RD Ritual-first: prepare ritual magic score=%d",plan.score);
            return 1;
        }
        /* Actively build missing field tributes, rather than waiting for
           two of three to be assembled by the retail AI by chance. The
           selected card must be a REAL physical hand monster and the zone
           must actually be free. A previous choice of that same card is
           preserved to avoid stepping on another planner's decision. */
        if(plan.prep&&plan.setup_hand_slot>=0&&
           plan.setup_hand_slot<board.actual_hand&&plan.setup_destination>=0&&
           board.own[plan.setup_destination].id<=0 &&
           (selected!=plan.setup_hand_slot || D_800EAE88[6]!=(u8)(plan.setup_destination+1))){
            /* The earlier version forgot selection[8] here, exposing a
               weak Ritual material face-up instead of setting it. */
            Rd_SelectFaceDownHandPlay((unsigned char *)D_800EAE88,
                                      plan.setup_hand_slot,
                                      plan.setup_destination+1);
            rd_diag_ai(hand,&board,&plan,legal,10,board.hand[plan.setup_hand_slot].id,recipe->ritual_magic);
            if(rd_setting("diagnostics",0))host->log(host,"RD Ritual-first: active tribute setup card #%d / field slot %d",board.hand[plan.setup_hand_slot].id,plan.setup_destination+1);
            return 1;
        }
        /* Do not consume protected hand material in an avoidable fusion.
           Leave all emergency or unavailable-alternative retail decisions. */
        if(plan.prep&&selected>=0&&selected<board.actual_hand&&
           plan.protect_hand[selected]&&plan.prefer_hand_slot>=0&&plan.prefer_destination>=0){
            if(board.own[plan.prefer_destination].id<=0){
                Rd_SelectFaceDownHandPlay((unsigned char *)D_800EAE88,
                                          plan.prefer_hand_slot,
                                          plan.prefer_destination+1);
                rd_diag_ai(hand,&board,&plan,legal,3,board.hand[plan.prefer_hand_slot].id,recipe->ritual_magic);
                if(rd_setting("diagnostics",0))host->log(host,"RD Ritual-first: preserve selected ritual material");
                return 1;
            }
        }
    } else if(Rd_ShouldActivateRitual(&plan,legal)){
        /* Keep a previous AI's valid activation of this Ritual intact. */
        if(D_800EAE88[9]==plan.ritual_back_slot+6 && result==2){
            rd_diag_ai(hand,&board,&plan,legal,8,recipe->ritual_magic,recipe->ritual_magic);
            return result;
        }
        /* If a previous planner selected a potentially lethal attack, do
           not replace it with a Ritual of uncertain extra value. */
        if(result==2 && D_800EAE88[9]>=1 && D_800EAE88[9]<=5 &&
           D_800EAE88[10]>=56 && D_800EAE88[10]<=60 &&
           board.own[D_800EAE88[9]-1].atk>=board.enemy_lp &&
           board.enemy_lp>0){
            rd_diag_ai(hand,&board,&plan,legal,9,board.own[D_800EAE88[9]-1].id,recipe->ritual_magic);
            return result;
        }
        /* Hm_PlanField selection contract: back-row activation = result 2. */
        for(j=0;j<12;j++)D_800EAE88[j]=0;
        D_800EAE88[9]=(u8)(plan.ritual_back_slot+6);
        D_800EAE88[10]=(u8)(plan.ritual_back_slot+6);
        rd_diag_ai(hand,&board,&plan,legal,4,recipe->ritual_magic,recipe->ritual_magic);
        if(rd_setting("diagnostics",0))host->log(host,"RD Ritual-first: activate Ritual score=%d",plan.score);
        return 2;
    }
    /* Protect a valuable field tribute even when the PREVIOUS AI (retail
       or another mod) chose to stack a new monster on top of that zone.
       The normal hand phase uses selection[6]=1..5 for monster placement;
       a single reroute preserves its chosen card, fusion chain and flags. */
    if(hand){
        RdPlan guarding=plan;
        int g,protect_result=0;
        /* Do not use a summoned Ritual result as fusion material merely
           because another AI values the fusion's printed ATK more highly.
           This guard must NOT depend on the post-Ritual Beacon evaluator:
           several ritualists (including Dokurorider) have no callback yet,
           and the display face-down bit may lag behind ritual presentation.
           A normally drawn copy of the result is also rare enough to save. */
        protect_result=Rd_ProtectRitualResult(&board,&guarding,recipe->result_card);
        if(post.active){
            protect_result=1;
            for(g=0;g<5;g++)if(post.protect_field[g])guarding.protect_field[g]=1;
        }
        /* A Ritual summoned from any active recipe is protected, not just
           the profile's printed result. The field grid, not the card ID
           alone, establishes which instance is genuinely ritualized. */
        for(g=0;g<5;g++){
            int rec=D_800907D8[D_8009B1D5*DUEL_FIELD_SIDE_GRID_SLOT_COUNT+
                                 DUEL_FIELD_ROW_SIZE+g];
            if(rd_tagged_field(rec)){
                guarding.protect_field[g]=1;
                protect_result=1;
            }
        }
        if(protect_result){
            guarding.prep=1;
            guarding.urgent=0;
        }
        int target=Rd_FieldGuardDestination(&board,&guarding,(const unsigned char *)D_800EAE88);
        if(target>0){
            int old=(int)D_800EAE88[6];
            D_800EAE88[6]=(u8)target;
            rd_diag_ai(hand,&board,&plan,legal,11,selected_id,recipe->ritual_magic);
            if(rd_setting("diagnostics",0))
                host->log(host,"RD Ritual-first: protected field material #%d at zone %d; redirected monster #%d to zone %d",
                          board.own[old-1].id,old,selected_id,target);
            return result;
        }
        if(target<0){
            /* Never fabricate a 'pass': Forbidden Memories requires a hand
               play and an empty selection could break its state machine.
               The old AI remains in charge in an unavoidable full field. */
            rd_diag_ai(hand,&board,&plan,legal,12,selected_id,recipe->ritual_magic);
            if(rd_setting("diagnostics",0))
                host->log(host,"RD Ritual-first: all field zones reserved; prior action retained, cannot safely redirect");
            return result;
        }
    }
    /* A generalized search for MATERIALS, not just Ritual Magic. Keep
       physical minima distinct from Deck permissions; this is the CPU's
       native Deck knowledge, NOT use-of-Deck as a Ritual tribute. */
    if(hand && !legal && plan.prep && ritual_rules.available &&
       ritual_rules.material_count==3 && recipe->material_count==3){
        RdCard upcoming[RD_DECK_SIZE];
        int left=RD_DECK_SIZE-(int)D_800E9FF0[D_8009B1D5].deck_draw_cursor;
        RdTributeFetch fetch;
        if(left<0)left=0;if(left>RD_DECK_SIZE)left=RD_DECK_SIZE;
        memset(upcoming,0,sizeof(upcoming));
        for(j=0;j<left;j++){
            int source=11+board.actual_hand+j;
            if(source>55)break;
            upcoming[j]=rd_make_card(&gDuel_aActiveCards[source]);
        }
        fetch=Rd_ChooseDeckTribute(profile,recipe,&ritual_rules,&board,&plan,
                   upcoming,left,(const unsigned char *)D_800EAE88,
                   legal==1,Rd_MatchRecipeCard,&recipe_cache[idx]);
        if(fetch.mode!=RD_FETCH_NONE &&
           rd_stage_tribute_fetch(&fetch,D_8009B1D5)){
            if(fetch.mode==RD_FETCH_FIELD){
                Rd_SelectFaceDownHandPlay((unsigned char *)D_800EAE88,
                                          fetch.source_slot-11,fetch.destination);
                rd_diag_ai(hand,&board,&plan,legal,14,fetch.card_id,recipe->ritual_magic);
                return 1;
            }
            /* HOLD: previous AI selected a different real hand card. Execute
               its original action after replacing an unselected hand slot. */
            rd_diag_ai(hand,&board,&plan,legal,15,fetch.card_id,recipe->ritual_magic);
            return result;
        }
    }
    /* One record for the final decision, not a speculative "kept previous"
       record followed by another record if our planner overrides it. */
    rd_diag_ai(hand,&board,&plan,legal,
               legal==1&&!plan.legal_set?7:1,selected_id,recipe->ritual_magic);
    return result;
}
static s32 rd_ai(void)
{
    rd_selection_tx.state=0;
    rd_selection_outcome=0;
    return rd_selection_prepare(rd_ai_plan());
}
static void rd_reset(void)
{
    rd_selection_reset();
    Rd_DropRewardsReset();
    Rd_StarterUIReset();
    legal_cache.valid=0;
    plan_cache.valid=0;
    deck_pull.active=0;
    /* Ritual identity is the tail of our saved state. Do not clear it on
       load: rd_deal clears it when a NEW duel is created. */
    memset(recipe_ready,0,sizeof(recipe_ready));
    memset(recipe_cache,0,sizeof(recipe_cache));
    /* Fresh recipes after load/reset: other Mods may have changed active rules. */
    /* The save-state record is restored by the host; only transient caching
       and turn heuristics are reset, never the already-shuffled deck. */
    planned_turns=0;
    last_turn_seen=-1;
    /* Keep save-state diagnostics honest: the restored deck is real, but
       the in-memory hash and chosen guarantees were not reobserved. */
    memset(&diag,0,sizeof(diag));
    if(save_state.generated && save_state.opp_id==(unsigned)gDuel_bOpponentID){
        diag.valid=1;diag.restored=1;
        diag.duelist_id=(int)save_state.opp_id;
        diag.tier=(int)save_state.profile;diag.mode=(int)save_state.mode;
        diag.deck_size=40;diag.materials=-1;diag.last_ai_turn=-1;
        diag.magic_hand=diag.magic_back=diag.magic_window=-1;
        diag.sticky_turn=-1;
    }
}
int MemoriesModInit(const MemoriesModHost *from,MemoriesMod *mod)
{
    if(!from||from->api<4)return 0;
    host=from;mod->api=4;mod->name="Ritual Duelists";mod->reset=rd_reset;
    mod->overlay=rd_overlay;
    mod->overlay_signature=rd_overlay_signature;
    if(!host->register_state(host,&save_state,sizeof(save_state),4))return 0;
    ritual_hook=host->hook(host,(void *)DuelEffect_ApplyRitual,(void *)rd_ritual_commit,&previous_ritual);
    if(!ritual_hook)return 0;
    fusion_hook=host->hook(host,(void *)Duel_CheckFusion,(void *)rd_fusion_decision,&previous_fusion);
    if(!fusion_hook){host->unhook(host,ritual_hook);ritual_hook=0;return 0;}
    deal_hook=host->hook(host,(void *)Duel_ShuffleDeck,(void *)rd_deal,&previous_deal);
    if(!deal_hook){host->unhook(host,fusion_hook);host->unhook(host,ritual_hook);fusion_hook=ritual_hook=0;return 0;}
    window_hook=host->hook(host,(void *)Ai_GetHandSize,(void *)rd_ai_window,&previous_window);
    if(!window_hook){host->unhook(host,deal_hook);host->unhook(host,fusion_hook);host->unhook(host,ritual_hook);deal_hook=fusion_hook=ritual_hook=0;return 0;}
    swap_hook=host->hook(host,(void *)func_8001BAF0,(void *)rd_materialize,&previous_swap);
    if(!swap_hook){host->unhook(host,window_hook);host->unhook(host,deal_hook);host->unhook(host,fusion_hook);host->unhook(host,ritual_hook);window_hook=deal_hook=fusion_hook=ritual_hook=0;return 0;}
    ai_hook=host->hook(host,(void *)AiScript_Run,(void *)rd_ai,&previous_ai);
    if(!ai_hook){host->unhook(host,swap_hook);host->unhook(host,window_hook);host->unhook(host,deal_hook);host->unhook(host,fusion_hook);host->unhook(host,ritual_hook);swap_hook=window_hook=deal_hook=fusion_hook=ritual_hook=0;return 0;}
    if(!Rd_StarterUIInit(host,mod)){
        host->log(host,"Ritual Duelists: starter selector initialization failed");
        return 0;
    }
    if(!Rd_DropRewardsInit(host,&save_state.blue_eye_source_bits,
                           &save_state.blue_eye_slot_token,
                           &save_state.blue_eye_result_pending))return 0;
    mod->name="Ritual Duelists";
    rd_selection_old_applied=mod->applied;
    mod->applied=rd_selection_applied;
    if(!host->hook(host,(void*)DuelScene_UpdateHandActions,
        (void*)rd_selection_hand_tick,&rd_selection_old_hand))return 0;
    if(host->provide)host->provide(host,"mandatory-hand-selection-v1",(void*)&rd_selection_rendezvous);
    mod->frame=0; /* Free Duel viewer belongs to PauloH-Gudang's separate Droplist MOD. */
    mod->api=12;
    mod->reset=rd_reset;
    mod->overlay=rd_overlay;
    mod->overlay_signature=rd_overlay_signature;
    return 1;
}
