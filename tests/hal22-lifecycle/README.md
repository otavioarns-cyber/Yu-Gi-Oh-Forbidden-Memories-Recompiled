# HAL-22 — ciclo de vida, ordem e reinício

Experimento de diagnóstico derivado dos testes próprios do [PR #15](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/15). Nenhuma alteração em produção. Leia as respostas A–E no [relatório](../../notes/HAL22_LIFECYCLE_RESTART.md).

## Executar

Exige as mesmas bibliotecas i386 e dez objetos da engine utilizados no PR #15, além de `sdk/exports.i386.txt`, compiler/libc/libpng/pthread i386 e executor Linux i386. Os hashes estão em [INPUTS.json](../../notes/hal-22-lifecycle/INPUTS.json). O Dual é dependência privada local e não é baixado, recompilado ou publicado.

```bash
export MEMORIES_AI_DIR=/caminho/local/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/privado/dual-play
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base/tmp/pc/game32/obj
export MEMORIES_SDK_EXPORTS=/caminho/base/tmp/pc/game32/sdk/exports.i386.txt
export MEMORIES_I386_CC='gcc'
export MEMORIES_I386_RUNNER='qemu-i386 -L /caminho/sysroot-i386'
python3 tests/hal22-lifecycle/run.py --output /tmp/hal22-lifecycle-resultados
```

O runner compila **somente o executável de testes**, ligado ao código oficial sem edições: `mods.c`, `manager.c`, `hooks.c`, `events.c`, loader, settings, paths e suporte. Copia os insumos para diretórios temporários. Nos candidatos, acrescenta unicamente `restart: true` ao JSON temporário do AI; confere a igualdade semântica das demais propriedades, a cópia byte a byte do manifest Dual e os hashes dos objetos. Os diretórios são removidos ao terminar. Nenhum candidato instalável ou manifest alterado acompanha a entrega.

Código de saída **1** é esperado: controles inseguros e contraexemplos conservam as falhas I2. Código **2** significa bloqueio/execução incompleta; não é bug de produção. Há verificações de hash antes/depois. `RESULTADOS.json` registra comandos, PIDs, estados, configurações persistidas e logs. Configurações são próprias de teste, sem saves pessoais.

## O que é real e o que é modelo

- Reais: scan de diretórios/manifests, `Mods_Load`, `Mods_Apply`, `Mods_SetEnabled`, `Mods_Order`, `Mods_Validate`, ativação, load_library, host API, `ObjectLoader_Load`, inicialização de cada MOD, registro/relink oficial, callbacks, persistência em arquivo e leitura por processo novo.
- `window_bridge.c` inclui o fonte oficial **inalterado** da janela e oferece acesso próprio às suas rotinas estáticas `adopt`, `needs_restart` e `apply`. Staging e confirmação são exercitados sem mouse ou renderização; não se substitui sua lógica. `Platform_RestartGame` é um observador de sucesso/falha. A chamada real de restart da plataforma Windows não é executada.
- Um reinício do experimento encerra o processo do harness e cria outro, com PID distinto, lendo o mesmo arquivo salvo. Não se finge reinício por `Mods_Reset`, `Mods_Shutdown` ou relink no mesmo processo. Quando o observador retorna sucesso, o processo antigo é mantido por alguns checks para observar o estado antes da saída; isso não representa uma janela real permitindo gameplay depois de um restart bem-sucedido.
- A tabela própria de exports é preenchida antes do manager carregar bibliotecas. Contém imports ELF e identificadores solicitados pelos objetos que também existem na lista oficial do SDK. `Mods_Lookup` e o host são reais; os endereços da engine apontam ao ambiente parcial herdado do PR #15.
- `--wrap` instrumenta apenas os limites de registro de hooks/estado e carregamento de objetos. Não altera `Hooks_Relink`, `Mods_Order`, o manager ou bytes dos MODs. A cadeia real despacha thunks próprios que preservam o ABI e registram callbacks.
- Engine parcial do PR #15: controller/snapshot/planner e materializador selecionados executam código nativo existente. RAM/catálogo/render e a ponte simples efeito/placement→campo são sintéticos. O hook Dual de início de duelo roda, mas o corpo completo de `Duel_InitScene` é modelo. AI OFF testa o materializador, sem bytecode da VM retail. Não há certificação de gameplay Windows.

Os oito cenários herdados incluem Support/Monster escolhidos pelo planner sem injeção de plano; bônus construído pelo Dual; categorias gastas/Ritual/plano vazio injetados após a filtragem como testes de barreira; uma troca offscreen e duas cópias de mesmo ID com identidades distintas. As injeções adversariais não são atribuídas ao planner. O estado privado 50/v10 do Dual é observado, não escrito.

Os 22 casos usam 41 processos, incluindo 19 reinícios entre fases. Controles unsafe ficam visíveis; a matriz não transforma um contraexemplo em solução aprovada. O candidato `restart: true` não é permanente e seu eventual uso exige decisão do usuário.
