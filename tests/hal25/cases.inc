#include "../../mods/ritual-card-effects/model.h"
#include "../../mods/ritual-card-effects/ritual_recipe_provider_v1.h"
#include "../../experiments/hal25/selection-rules-v1.h"
extern void Hal25_ForceApplied(int,int);
extern const MemoriesModHost*Hal25_Host(int);
static int queen_required;
static void*local(int m,const char*n){for(size_t i=0;i<modules[m].object.symbol_count;i++)if(!strcmp(modules[m].object.symbols[i].name,n))return(void*)modules[m].object.symbols[i].address;return NULL;}
static void put_field(int,int,int);
static void critical_audit(void){for(int i=0;i<nr;i++)if(!strcmp(registrations[i].name,"func_8001BAF0")||!strcmp(registrations[i].name,"AiScript_Run"))printf("CALLBACK owner=%s target=%s calls=%u\n",modules[registrations[i].owner].id,registrations[i].name,registrations[i].calls);}
/* Not a production provider. Tests transport using the genuine zero-argument
 * producer helper, found in the exact qualified object, without reimplementing
 * Queen Lock. Never use this private-symbol access in a MOD. */
static int policy_bridge(int side){
 ++provider_calls;if(mi(RE)<0||!Mods_Active(mi(RE)))return HAL25_RULE_UNAVAILABLE;
 CHECK(side==*(uint8_t*)datum("D_8009B1D5"));
 int mode=Settings_GetNamed("test.bridge-mode",0);
 if(mode==1)return HAL25_RULE_CANCELLED;
 if(mode==2){((uint8_t*)datum("D_800EAE88"))[0]=99;return HAL25_RULE_REPLACED;}
 unsigned char before[12];memcpy(before,datum("D_800EAE88"),12);
 DuelDeckCardRecord deck[80];DuelCardRecord records[30];memcpy(deck,datum("gDuel_aDeckCardRecords"),sizeof deck);memcpy(records,datum("D_801A7AD8"),sizeof records);
 void(*rule)(void)=local(RE,"cosmo_force_ai_selection");CHECK(rule);rule();
 CHECK(!memcmp(deck,datum("gDuel_aDeckCardRecords"),sizeof deck)&&!memcmp(records,datum("D_801A7AD8"),sizeof records));
 printf("BRIDGE genuine Queen predicate/action; side=%d changed=%d materialized=0\n",side,memcmp(before,datum("D_800EAE88"),12)!=0);
 return memcmp(before,datum("D_800EAE88"),12)?HAL25_RULE_REPLACED:HAL25_RULE_UNCHANGED;
}
static const Hal25SelectionRulesV1 test_policy={1,policy_bridge};
static void install_test_bridge(void){if(mi(RE)<0||!Mods_CodeHash(mi(RE))){puts("NOT_AVAILABLE proposed provider, Ritual never loaded");return;}const MemoriesModHost*h=Hal25_Host(mi(RE));CHECK(h&&h->provide&&h->provide(h,"selection-rules-v1",(void*)&test_policy));puts("TEST_ONLY proposed selection-rules-v1 registration; real PR11 binary has NO such provider");}
static void queen_setup(void){
 int kind=Settings_GetNamed("test.condition",0);queen_required=0;
 if(kind==5){puts("QUEEN inactive control, no live Queen");return;}
 put_field(5,708,0);DuelDeckCardRecord*d=datum("gDuel_aDeckCardRecords");DuelSideState*s=datum("D_800E9FF0");DuelCardRecord*r;
 d[44].id=kind==2?112:395;s[1].hand[4]=44;r=setup_record(19,44);((DuelHandSlot*)datum("D_800EA030"))[4].object=spawn_hand(r,0,0);
 if(kind==1||kind==4){d[43].id=kind==4?395:708;s[1].hand[3]=43;r=setup_record(18,43);((DuelHandSlot*)datum("D_800EA030"))[3].object=spawn_hand(r,0,0);}
 if(kind==3){for(int i=0;i<5;i++)put_field(20+i,99,45+i);s[1].deck_draw_cursor=10;d[50].id=680;d[51].id=571;}
 if(kind==6){*(signed char*)datum("gDuel_bOpponentID")=16;puts("BOARD alternate opponent: inactive legacy profile may yield no pending");}
 *(uint8_t*)datum("D_8009B1D5")=0;*(uint8_t*)datum("D_8009B19C")=5;*(int16_t*)datum("D_8009B1A0")=708;*(uint16_t*)datum("D_8009B210")=0x83;
 if(mi(RE)>=0&&Mods_Active(mi(RE)))((void(*)(void))engine_function("DuelEffect_ApplyRitual"))();
 unsigned*hint=local(RE,"cosmo_live_hint"),*mask=local(RE,"cosmo_live_mask");queen_required=mi(RE)>=0&&Mods_Active(mi(RE))&&hint&&mask&&*hint&&(*mask&(1u<<5))&&kind!=2&&kind!=3;
 printf("QUEEN condition=%d effect_ON=%d actual_commit=%d legal_Female=%d identities=%u,%u; no private mark/pending/plan/quota written\n",kind,mi(RE)>=0&&Mods_Active(mi(RE)),hint&&*hint,queen_required,d[43].data_block_index,d[44].data_block_index);
 *(uint8_t*)datum("D_8009B1D5")=1;*(uint16_t*)datum("gDuel_wSceneStateFlags")=4;setup_count=0;for(int i=0;i<nr;i++)registrations[i].calls=0;
}
#include "ritual.inc"
static unsigned char chunks[NMOD][512];
static void save_chunk(int owner,void*p,size_t size,unsigned version,void*ctx){(void)ctx;int m=which(owner);CHECK(m>=0&&size<=512);memcpy(chunks[m],p,size);printf("STATE registered owner=%s size=%u version=%u\n",Mods_Id(owner),(unsigned)size,version);}
static void load_chunk(int owner,void*p,size_t size,unsigned version,void*ctx){(void)version;(void)ctx;CHECK(size<=512);memcpy(p,chunks[which(owner)],size);}
static void state_cases(void){int before=pull()->active;MemoriesModEvent e={MEMORIES_EVENT_SAVE,MEMORIES_BEFORE,0,0,0,0,0};Mods_Dispatch(&e);Mods_VisitState(save_chunk,NULL);Mods_Reset();Mods_VisitState(load_chunk,NULL);e=(MemoriesModEvent){MEMORIES_EVENT_LOAD,MEMORIES_AFTER,0,0,0,0,0};Mods_Dispatch(&e);result("reset-simulated-load-no-pending",!pull()->active,"Registered buffers only; pending BSS not invented as savestate. Real reset/LOAD callbacks.");((void(*)(void))engine_function("Duel_InitScene"))();result("new-duel-no-stale-pending",!pull()->active,"Native duel-init and prior real reset, not gameplay/save certification.");printf("STATE pending_before_reset=%d after=%d\n",before,pull()->active);}
/* Separate mandatory-position domain: the existing public contract is real.
 * Does not claim that the proposed hand-only contract covers attack targeting. */
static void other_restriction_cases(void){
 typedef struct{unsigned abi;int(*can_defend)(int);} Position;
 setup(5);begin("existing-position-rules-public-commit");
 Position*p=Mods_Find("ritual-card-effects-test:position-rules-v1");CHECK(p&&p->abi==1&&p->can_defend);
 put_field(20,722,45);put_field(21,722,46);
 *(uint8_t*)datum("D_8009B1D5")=1;*(uint8_t*)datum("D_8009B19C")=20;
 *(int16_t*)datum("D_8009B1A0")=722;*(uint16_t*)datum("D_8009B210")=0x83;
 ((void(*)(void))engine_function("DuelEffect_ApplyRitual"))();
 result("other-restriction-Black-Chaos-instance",!p->can_defend(20)&&p->can_defend(21),"Real ritual commit activates attack-only for that identity, not an ordinary equal-ID copy. No mark injection.");
 DuelCardRecord*r=datum("D_801A7AD8");DuelDeckCardRecord*old=r[20].data;r[20].data=r[21].data;
 result("other-restriction-distinct-identity",p->can_defend(20),"Explicit identity-replacement probe; equal card ID does not inherit the ritual restriction.");r[20].data=old;
 Hal25_ForceApplied(mi(RE),0);result("other-restriction-OFF-neutral",p->can_defend(20),"Resident public provider consults effective state.");
 Hal25_ForceApplied(mi(RE),1);*(uint16_t*)datum("D_8009B210")=0x83;((void(*)(void))engine_function("DuelEffect_ApplyRitual"))();
 result("other-restriction-new-commit-ON",!p->can_defend(20)&&p->can_defend(21),"New genuine ritual commit after ON; no assumption of persistence across OFF.");
 state_cases();
}
