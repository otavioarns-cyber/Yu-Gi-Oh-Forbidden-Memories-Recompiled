/* Experimental exact-artifact qualification; see notes/hal-20-field-state. */
enum{HAL20_ROLE_COUNT=7,HAL20_TARGET_COUNT=7};
static const char*const hal20_targets[]={"func_8001BAF0","DuelScene_UpdateHandActions","AiScript_Run","Duel_SetupCardRecord","DuelEffect_ApplyTerrain","func_80037DA4","func_80035E20"};
typedef struct{const char*id;unsigned code;const char*sha;} Hal20Role;
static const Hal20Role hal20_roles[]={
{"ai-hard-mode",0x00049e34u,"05cf957bc4575503d257432cd927748367207258189856217bc1bf2adb0f7d19"},
{"ritual-duelists",0x3d26698fu,"9f0d7354e6ff3f44776308b18e96b960480500c814ae22c3e7d1e6b57eafea34"},
{"dual-play",0x3028c68au,"77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378"},
{"ritual-card-effects-test",0x1e478c68u,"78ec3ce45694b1f583213530dfc0a527c8974056f67c49e7791fb6821f55ab45"},
{"melhoria-dos-rituais",0x1fe34a1eu,"c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0"},
{"fusion-auxiliary-balance",0x69a6af85u,"c259f789c5060623744bd28ab09de4044d055d9c02570a3f7f73032eb5190dd6"},
{"field-effects-secondary-types",0xcfd0dcf3u,"729ff106787d477def41feb42a1a5d168f5bb050af870a25738c977e02c1cd21"},
};
typedef struct{int role;const char*target,*function;size_t bytes;} Hal20Callback;
static const Hal20Callback hal20_callbacks[]={
{0,"AiScript_Run","run",3820},
{0,"func_8001BAF0","swap_selection",1503},
{1,"func_8001BAF0","rd_materialize",1318},
{1,"AiScript_Run","rd_ai",774},
{1,"DuelScene_UpdateHandActions","rd_selection_hand_tick",1020},
{2,"DuelScene_UpdateHandActions","dual_hand_actions",4854},
{2,"func_8001BAF0","dual_ai_swap",2410},
{3,"AiScript_Run","selection_ai_run",3217},
{3,"func_80037DA4","ritual_viewer_fit_command",183},
{3,"func_80035E20","ritual_viewer_effect_title_draw",3584},
{3,"DuelScene_UpdateHandActions","hand_actions",1767},
{3,"func_8001BAF0","cosmo_hand_swap",187},
{4,"DuelScene_UpdateHandActions","hand_actions_hook",242},
{5,"func_80035E20","aux_viewer_draw",2051},
{5,"DuelScene_UpdateHandActions","aux_hand_actions",314},
{6,"Duel_SetupCardRecord","setup_hook",49},
{6,"DuelEffect_ApplyTerrain","apply_terrain_hook",323},
{6,"func_80037DA4","text_command_hook",137},
{6,"func_80035E20","text_draw_hook",4659},
};
static const size_t hal20_producer_bytes=513;
