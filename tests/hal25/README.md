# Reprodução HAL25

Leia primeiro o [relatório](../../notes/HAL25_QUEEN_LOCK_RD_POC.md). Os FAIL de referência/candidatos parciais são resultados esperados de investigação e continuam FAIL nos arquivos. `run.py` sai 1 quando qualquer assertion falha, 2 quando há bloqueio/incompletude. Não interprete QEMU/host sintético como gameplay Windows.

Dependências: Python, Pillow, GCC/ASan/UBSan, compilador/libs i386, SDK oficial API24, os dez objetos nativos Linux i386 de `INPUTS.json`, QEMU/sysroot se necessário. AI oficial, Melhorias PR #9 e Dual privado devem estar nas versões de hash exigidas. O código/manifest/objeto Dual não acompanha esta branch; obtenha localmente com autorização. O preflight recusa hashes diferentes, não troca por objeto recebido antigo.

```sh
export MEMORIES_AI_DIR=/caminho/base-build/mods/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/local/dual-api24
export MEMORIES_RITUALS_DIR=/caminho/pr9/melhoria-dos-rituais
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base-build/obj
export MEMORIES_SDK_EXPORTS=/caminho/sdk-linux/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot'
export MEMORIES_SDK=/caminho/sdk-windows
python tests/hal25/run.py --output /tmp/hal25-matrix
python tests/hal25/run.py --restrictions-only --output /tmp/hal25-other
python tests/hal25/regressions.py --output /tmp/hal25-regressions
python tests/hal25/provenance.py --output /tmp/hal25-provenance.json
```

O runner copia manifests/objetos/assets para diretórios temporários e limpa somente esses diretórios. Não modifica os pacotes de origem. `--smoke` limita o teste aos oito caminhos mínimos. `--restrictions-only` executa oito probes adicionais de `position-rules-v1`/Black Chaos com commit real, sem repetir a matriz. A UI/plano/pending não são injetados nos testes principais; `test.cancel-after-planner` é um probe adversarial **artificial** identificado nos logs. A ponte proposta e suas respostas cancel/invalid são fixtures de contrato, não funcionalidade do PR #11.

`harness.c` usa manager, Hooks_Relink e loader oficiais, registra encadeamento/callbacks e lê o pending privado para observar. `manager_bridge.c` inclui `mods.c` da base e expõe host/aplicação efetiva para os testes; `mods.c` não é compilado separadamente nem alterado. `observer.c` apenas chama o observador após os planners; só o probe de cancelamento explicitamente identificado altera seleção. Há uma fronteira modelada de resolução da colocação, catálogo/VM/gráficos sintéticos e ausência de disco/Windows.

As regressões mantêm as assertions originais. A metadata histórica de insumos do PR #13 é copiada para a estrutura esperada. A auditoria antiga que exige fontes e objeto imutáveis deve falhar nos candidatos A/B: não é um teste de semântica de gameplay. Use `provenance.py` para auditar o delta experimental exato. A primeira rodada preservada omitiu essa metadata; reexecutou-se somente o grupo afetado com `regressions.py --package-only`, sem repetir os 34 grupos funcionais aprovados. Preserve ambos os resultados ao comparar.

Para reconstruir **novos** candidatos em cópias isoladas (não foi necessário repetir os builds já concluídos):

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
python tests/hal25/build_candidates.py --sdk /caminho/sdk-windows --work /tmp/hal25-new-build
python tests/hal25/abi.py \
  --original mods/ritual-duelists/ritual-duelists.o \
  --candidate artifacts/hal25/ritual-duelists-alternative-a.o \
  --candidate artifacts/hal25/ritual-duelists-alternative-b.o \
  --sdk-exports /caminho/sdk-windows/exports.i386.txt --output /tmp/hal25-abi.json
```

`build_candidates.py` apaga o objeto PR #13 **somente na cópia** antes de compilar os 16 C com `--target i386`. Não substitui automaticamente os artefatos publicados. Registre o SHA dos novos objetos; paths/DWARF podem mudar o hash completo. Não instale múltiplos objetos/variantes do mesmo ID.

Logs integrais: `notes/hal-25/logs/*.tar.gz`; JSON/CSV e os índices de hashes ficam nas pastas de cada suíte. Os arquivos compactados contêm somente logs de testes próprios, sem o Dual. Para leitura, extraia em diretório temporário; não importe conteúdo compactado como código de produção.
