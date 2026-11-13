# HAL-22 — reprodução de I2

Fixtures próprias, derivadas da infraestrutura própria do PR #14. Publicação de testes/diagnóstico; nenhum patch de produção. Leia [o relatório](../../notes/HAL22_AI_HARD_MODE_DUAL_PLAY.md) antes de interpretar FAIL ou usar a ordem como mitigação.

## Insumos locais exigidos

- AI HARD MODE 0.1.0 oficial recompilado na base do PR #8, `ai-hard-mode.o` e manifest original.
- Dual Play 0.5.7 recompilado API 24/i386 do PR #10, `dual-play.o` e manifest original. Dependência privada: não está nesta branch e não é baixada pelo runner.
- Dez objetos nativos i386 da compilação da base oficial, listados em [INPUTS.json](../../notes/hal-22/INPUTS.json), em `tmp/pc/game32/obj`. Reaproveitados da entrega da base; nenhum MOD foi recompilado nesta tarefa.
- Compilador C com libc i386 e executor i386 Linux (nativo ou QEMU). Os objetos de MOD i386 usam o mesmo container/ABI da base Windows i386, mas esta execução é Linux/QEMU.

O preflight exige os hashes dos insumos utilizados nesta entrega. Um build local com caminhos/debug diferentes pode gerar outro hash para objetos da engine: registre uma nova proveniência/revisão explícita antes de aceitar substitutos. Não remova a verificação nem publique o Dual para tornar o teste autossuficiente.

## Execução

```bash
export MEMORIES_AI_DIR=/caminho/local/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/privado/dual-play
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base/tmp/pc/game32/obj
export MEMORIES_I386_CC='gcc'
export MEMORIES_I386_RUNNER='qemu-i386 -L /caminho/sysroot-i386'
python3 tests/hal22/run.py --output /tmp/hal22-resultados
```

O runner compila somente dois executáveis próprios: harness e teste do manager. Não constrói nem substitui objetos de MOD/engine. `RESULTADOS.json` registra os comandos realmente usados nesta entrega. Saída **1** é esperada enquanto I2 estiver sem correção: mantém FAILs de invariantes visíveis. Saída **2** indica insumo bloqueado ou execução incompleta; não é diagnóstico de bug de produção.

## O que efetivamente executa

`harness.c` usa `ObjectLoader_Load`, `hooks.c`, `events.c` e `json.c` oficiais. Carrega os dois MODs sem patches de bytes e vincula corpos públicos selecionados dos objetos nativos: controller da mão, snapshot, inicialização da VM, janela nativa e materializador. Thunks próprios instrumentam entradas e callbacks, conservando a cadeia oficial; o alvo patchable é do teste. Nenhuma função estática privada do Dual é chamada por ABI presumida.

RAM sintética zero, com aliases/endereços do mapa público oficial; não contém imagem do jogo. `D_8009B360` e `gDuel_bOpponentID` compartilham os dois bytes reais. A mão de bônus e as quotas são produzidas pelo controller/hooks/frame reais; o teste **não escreve nos bytes privados registrados do Dual**. Esses bytes são apenas observados como estado 50/v10, com proveniência fixa.

O catálogo é construído pelo teste. A tabela pública de oponentes fornece a janela nativa de Rex (8); isso **não é o deck verdadeiro de Rex**. Render, animação, catálogo/tabelas não presentes e a ponte simples efeito/placement → campo são modelos. `func_8001B8B8` (cores), áudio e callbacks gráficos não são executados nativamente. A VM retail completa não roda: fallback `AiScript_Run` retorna zero; as escolhas examinadas vêm do planner oficial AI HARD MODE, sem seleção injetada nesses dois cenários.

Quatro cenários adversariais injetam seleção/snapshot **depois** da filtragem para testar a última barreira: categoria já gasta (Monster/Support), Ritual e plano vazio. Não os confunda com escolhas naturais do planner. No caminho invertido, a limpeza por rejeição fica BLOCKED porque o gate não foi executado.

O teste de dois próximos cards injeta um plano com **duas cópias de ID 35 e identidades físicas distintas** para verificar uma execução do worker, duas trocas e conservação dos 80 identificadores; não certifica receitas/fusões. OFF/ON verifica callbacks e o `Hooks_Relink` real; AI OFF verifica a cadeia do materializador, não a estratégia retail.

`order.c` chama `Mods_Compatible`/`Mods_Order` reais com manifests inalterados. A carga tardia representa a sequência D carregado → primeira ativação H confirmada pela leitura de `mods.c`; não executa a janela completa do manager. O0/O1 se reduzem ao mesmo grafo com somente dois MODs.
