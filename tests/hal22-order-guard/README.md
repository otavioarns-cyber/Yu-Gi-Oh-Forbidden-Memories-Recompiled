# HAL-22 — order guard POC

Protótipo experimental, derivado das reproduções próprias dos PRs #15/#16. Leia [relatório e cinco respostas](../../notes/HAL22_ORDER_GUARD_POC.md) e [diff de produção reversível](../../notes/hal-22-order-guard/DIFF_EXPERIMENTAL.patch). Não é uma versão instalável aprovada.

## Executar os testes

Mesmos objetos locais i386 de AI oficial, Dual 0.5.7 e dez unidades nativas da engine usados nos PRs anteriores; SDK API 24; compilador/libc/libpng/pthread i386; Linux i386 ou QEMU. A regressão de ordenação também requer as quatro árvores próprias já publicadas dos PRs #9/#11/#12/#13, **somente para ler manifests**, com os hashes de [INPUTS.json](../../notes/hal-22-order-guard/INPUTS.json). Nenhum fonte/manifest/binário Dual acompanha esta branch ou é baixado pelo runner.

```bash
export MEMORIES_AI_DIR=/caminho/local/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/privado/dual-play
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base/tmp/pc/game32/obj
export MEMORIES_SDK_EXPORTS=/caminho/base/tmp/pc/game32/sdk/exports.i386.txt
export MEMORIES_OWN_MODS_DIR=/caminho/local/mods-proprios-do-PR14
export MEMORIES_I386_CC='gcc'
export MEMORIES_I386_RUNNER='qemu-i386 -L /caminho/sysroot-i386'
python3 tests/hal22-order-guard/run.py --output /tmp/hal22-order-guard
# Para executar só os cenários protegidos:
python3 tests/hal22-order-guard/run.py --cohort on --output /tmp/hal22-order-guard-on
```

O modo `all` mantém os controles sem proteção: **exit 1 esperado pelos 158 FAILs de I2**. `--cohort on` seleciona os casos protegidos; não mascara FAILs, e uma falha própria torna o resultado não zero. Exit 2 é bloqueio/incompletude. Resultados da entrega: 51 processos protegidos, 1834 PASS, zero FAIL/ERROR e 24 SKIP de suíte que não devia executar com perfil recusado ou só um MOD ativo; controles separados na matriz.

O runner compila o harness contra os cinco fontes experimentais e uma unidade nativa do `Main_RunDuel` **modificado**. Testa a entrada real dessa função: recusada, o corpo não avança e o aviso é observado; admitida, entra no primeiro passo. As unidades de controller/planner/materialização são reaproveitadas. O renderer, catálogo, ponte simples de efeitos e corpo completo de init do duelo continuam parciais como no PR #15.

`window_bridge.c` executa as rotinas de staging/confirmação/Apply do fonte da janela, incluindo o guard. Os callbacks `Platform_ShowError` e `Platform_RestartGame` são observadores de teste; o runner implementa reinício entre processos com PIDs distintos e settings persistidos. Não certifica a janela visual, relançamento nativo de Windows ou gameplay. `--wrap` registra loader/hooks/estado sem mudar sua ordem ou bytes de MODs.

`order.c` compara 3072 ordens/subconjuntos com manifests reais: cinco oficiais (AI, Drop Missing Cards, 3D Monsters, Hand Camera, Yamyi), quatro próprios e o Dual privado. Não carrega os objetos dos outros MODs de produção. `observer.c` fornece dois wrappers próprios delegantes, intercalados com AI/Dual e em um alvo independente. A suíte oficial `tests/pc/hooks_test.c` também é executada sem alterações.

## Configuração experimental, fora de produção

A proteção começa **desligada**. Sua ativação é a preferência de host:

```text
experimental.hal22_order_guard=1
```

O perfil de referência usa ordem efetiva AI antes de Dual e `restart: true` **apenas na cópia experimental** do manifest AI. O runner gera essa cópia em diretório temporário e a remove ao terminar. Casos com manifest original são controles/ensaios de rejeição; não há alteração permanente de `mods/ai-hard-mode/mod.json` nem de seus 17 settings.

A preferência não tem UI própria e ainda não é serializada pelo exportador de perfis, que salva chaves `mod.*`. Portanto a PoC não deve ser distribuída como perfil pronto. Alterar/desativar o opt-in exige configurar/reiniciar conscientemente o ambiente experimental; com a proteção desligada, I2 continua reproduzível.

Inicialização com ordem recusada bloqueia a **transação inteira do perfil**, antes de inicializar bibliotecas/dados. Preferências ficam intactas, e o duelo é impedido com mensagem explícita. Reparar os ranks na janela permite salvá-los, mas o processo cujo startup foi recusado precisa ser relançado. Não há fallback silencioso para Vanilla ou desligamento automático de um MOD. Um perfil independente apenas com AI ou apenas com Dual não recebe esse bloqueio.

Sem recusa anterior de startup, um MOD ativo e outro apenas pendente de reinício continuam sendo uma composição individual válida. O guard não declara que o MOD pendente já está ativo. Se ambos estão efetivamente ativos, verifica a posição dos registros existentes no swap, não só ranks ou ordem da última ativação.
