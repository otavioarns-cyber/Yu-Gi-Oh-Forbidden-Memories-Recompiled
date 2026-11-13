# HAL-23 — reprodução isolada do lifecycle do pending

[Relatório](../../notes/HAL23_PENDING_LIFECYCLE.md) · [matriz](../../notes/hal-23/MATRIZ.md) · [insumos por SHA-256](../../notes/hal-23/INPUTS.json).

O runner compila somente o harness e o observador próprios, com os fontes **oficiais não modificados** do manager/loader/hooks/settings e a rotina real de Apply da janela. Reutiliza os objetos API24 de AI, RD e Dual e dez unidades nativas já compiladas. Não recompila nem redistribui os MODs. Não requer ROM ou save pessoal.

Dependências locais obrigatórias, todas conferidas antes e depois:

- `MEMORIES_AI_DIR`: `ai-hard-mode.o` e `mod.json` oficiais (PR8).
- `MEMORIES_DUAL_DIR`: objeto API24 i386 e manifest exatos do PR10, obtidos legalmente pelo operador. Não acompanham esta branch.
- `MEMORIES_RD_DIR`: objeto e manifest exatos do PR13.
- `MEMORIES_ENGINE_OBJ_DIR`: dez objetos i386 nativos listados em INPUTS.json, build oficial da base API24.
- `MEMORIES_SDK_EXPORTS`: `exports.i386.txt` do SDK oficial.
- `MEMORIES_I386_CC`: comando GCC com headers/libs i386 (libc, libpng, pthread, math).
- `MEMORIES_I386_RUNNER`: vazio para execução i386 nativa ou comando QEMU com sysroot compatível.

Exemplo (substituir caminhos pelos insumos locais):

```sh
export MEMORIES_AI_DIR=/caminho/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/dual-play-privado
export MEMORIES_RD_DIR=/caminho/ritual-duelists
export MEMORIES_ENGINE_OBJ_DIR=/caminho/game32/obj
export MEMORIES_SDK_EXPORTS=/caminho/sdk/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='qemu-i386-static -L /caminho/sysroot'
python3 tests/hal23/run.py --output /tmp/hal23-resultados
```

O diretório de saída deve ser novo. Código de saída **1 é esperado no diagnóstico original**: mantém os 52 FAIL das invariantes expostas. `0` significa todos os checks passaram; `2` ou execução sem SUMMARY significa bloqueio/incompletude. Um FAIL não autoriza correção automática. Os logs completos e o CSV mantêm o resultado de cada verificação.

216 execuções de ação + 12 processos novos de inicialização = 228 processos. Ranks distintos O0=H,D,RD; O1=H,RD,D; O2=D,RD,H. Drop Missing Cards ON; observador último. Dual OFF/ON, primeira carga e biblioteca já carregada, OFF→ON/ON→OFF, Apply/SetEnabled/recarga e Apply da janela, cancelamento/falha observada do restart. O candidato `restart:true` é escrito apenas no manifest temporário do AI. Os arquivos de entrada permanecem intactos.

A preparação constrói RAM/catálogo e mão sintéticos com instâncias distintas. O planner **real** do RD escolhe buscar a magia ritual 680 em um índice real não comprado e cria seu próprio `deck_pull`. O harness resolve o símbolo privado somente para **leitura**, conferindo objeto e tamanho; não é um ABI público. Nunca escreve nesse pending. Não desliga defaults estratégicos do AI.

`before` e `after` aplicam a mudança fora do trecho planner→swap do controller nativo. `gap` usa um MOD observador próprio para inserir a chamada ao manager depois do retorno dos planners e antes do swap. **Essa reentrância é artificial; não existe um evento de UI equivalente demonstrado.**

O corpo vanilla de AiScript_Run é uma proposta modelada (seleção12; próxima ação14); não executa bytecode retail. O planner RD, planner AI oficial, snapshot, inicializador da VM, controller da mão, marcação/consumo e materializadores selecionados são reais. Graphics/audio/disc/tabelas auxiliares são stubs/modelos rastreados. O controller chega à fase6/7 e remove a seleção física da mão; resolução de efeitos/invocação completa não foi executada.

No caso `next`, placement e reconstrução/compra da próxima mão são modelos explícitos. O segundo planner RD e controller são reais. Isso delimita a evidência de fallback antigo consumindo uma seleção diferente; não é prova de exploit numa partida. `save` visita/restaura somente buffers realmente registrados e chama Mods_Reset + LOAD AFTER na ordem oficial; não inventa serialização do pending nem carrega um savestate completo. O processo novo verifica BSS zerado, não retoma o turno.

A janela usa rotinas originais, mas Platform_RestartGame observa sucesso/falha: não relança o aplicativo Windows. Na fase1, o runner cria de fato outro processo do harness lendo os mesmos settings. Nada desta suíte certifica gameplay Windows, compatibilidade de saves antigos ou integração completa. Nenhum arquivo de produção foi alterado.
