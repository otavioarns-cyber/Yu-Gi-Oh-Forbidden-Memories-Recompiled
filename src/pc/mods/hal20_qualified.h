/* Generated from exact candidate objects; see notes/hal-20-consolidacao/QUALIFIED_ARTIFACTS.json. */
typedef struct{const char*id;unsigned code;const char*sha;} Hal20Role;
static const Hal20Role hal20_roles[]={
{"ai-hard-mode",0x3e0648cbu,"9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc"},
{"ritual-duelists",0x3d26698fu,"9f0d7354e6ff3f44776308b18e96b960480500c814ae22c3e7d1e6b57eafea34"},
{"dual-play",0x3028c68au,"77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378"},
{"ritual-card-effects-test",0x1e478c68u,"78ec3ce45694b1f583213530dfc0a527c8974056f67c49e7791fb6821f55ab45"},
{"melhoria-dos-rituais",0x1fe34a1eu,"c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0"},
{"fusion-auxiliary-balance",0xafb75202u,"384e11eeeae32412e23afbbbd9ebc308761674b8a2181238fd60e17e315b865c"},
};
typedef struct{int role;const char*target,*function;size_t bytes;} Hal20Callback;
static const Hal20Callback hal20_callbacks[]={
{0,"func_8001BAF0","swap_selection",1503},
{0,"AiScript_Run","run",3820},
{1,"func_8001BAF0","rd_materialize",1318},
{1,"DuelScene_UpdateHandActions","rd_selection_hand_tick",1020},
{1,"AiScript_Run","rd_ai",774},
{2,"func_8001BAF0","dual_ai_swap",2410},
{2,"DuelScene_UpdateHandActions","dual_hand_actions",4854},
{3,"func_8001BAF0","cosmo_hand_swap",187},
{3,"DuelScene_UpdateHandActions","hand_actions",1767},
{3,"AiScript_Run","selection_ai_run",3217},
{4,"DuelScene_UpdateHandActions","hand_actions_hook",242},
{5,"DuelScene_UpdateHandActions","aux_hand_actions",314},
};
static const size_t hal20_producer_bytes=513;
