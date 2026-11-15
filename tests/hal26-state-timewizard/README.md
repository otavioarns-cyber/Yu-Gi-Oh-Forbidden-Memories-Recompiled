# HAL26 — reprodução da continuação PR29

[Relatório](../../notes/HAL26_STATE_TIMEWIZARD_READINESS.md). Referência `3284ec493d179f4b23cc7230a2979a1c2b122873`; SDK24/i386. Sem imagem, saves pessoais ou pacote Dual no Git. Os objetos canônicos herdados não são recompilados/substituídos por estes testes.

## Ambiente

Python (pyelftools para os inventários), SDK Windows oficial, LLVM/Clang e GCC/sysroot i386. Se o kernel não executa ELF32, usar QEMU apenas para fixtures Linux; isso não valida Windows.

```sh
export MEMORIES_AI_DIR="$PWD/mods/ai-hard-mode"
export MEMORIES_RITUALS_DIR="$PWD/mods/melhoria-dos-rituais"
export MEMORIES_FUSION_DIR="$PWD/mods/fusion-auxiliary-balance"
export MEMORIES_DUAL_DIR=/privado/dual-api24-exato
export MEMORIES_ENGINE_OBJ_DIR=/base/game32/obj
export MEMORIES_SDK_EXPORTS=/base/game32/sdk/exports.i386.txt
export MEMORIES_I386_CC=/tools/gcc-i386
export MEMORIES_I386_RUNNER='/tools/qemu-i386-static -L /tools/sysroot'
export MEMORIES_SDK=/base/win32/sdk
export MEMORIES_MOD_CC=/tools/llvm-mingw/bin/clang
```

`notes/hal-26/INPUTS.json` fixa os insumos atuais, inclusive o core alterado. O runner compartilhado mantém o default antigo; usar `--inputs` nesta branch, sem reescrever os recibos históricos. Artefato divergente é BLOCKED, não automaticamente aceito.

## B e arquivo de load real

```sh
python tests/hal26-state-timewizard/policy_tests.py --output /tmp/h26-policy-novo
python tests/hal26-state-timewizard/load_prewrite_test.py --output /tmp/h26-load-novo
python tests/hal26-state-timewizard/storage_regressions.py --output /tmp/h26-storage-novo
python tests/hal20-field-state/run.py --inputs notes/hal-26/INPUTS.json \
  --consolidation --consolidation-ai-matrix --with-field \
  --consolidation-case 1 --consolidation-case 2 --consolidation-case 3 \
  --guard both --output /tmp/h26-B-novo
python tests/hal20-field-state/run.py --inputs notes/hal-26/INPUTS.json \
  --consolidation --with-field --guard 1 --output /tmp/h26-order-novo
python tests/hal20-field-state/run.py --inputs notes/hal-26/INPUTS.json \
  --manager --with-field --guard 1 --output /tmp/h26-manager-novo
```

O teste de load extrai literalmente a função atual `state.c:load`, não a reimplementa. Contadores e traps posteriores provam recusa antes de parsing/contexto; não oferecem caminho de restauração bem-sucedida. Testes oficiais de storage usam arquivos sintéticos temporários. Default estrito/Werror passa; `-O1/Werror` GCC falha igualmente no fonte antigo/candidato por truncation, registrado separadamente.

## A isolada, nunca instalável como persistência completa

```sh
python tests/hal26-state-timewizard/build_pair_preview.py --sdk "$MEMORIES_SDK" --output /privado/h26-pair-novo
export MEMORIES_STATE_PREVIEW_DIR=/privado/h26-pair-novo/R/ritual-card-effects
export MEMORIES_FUSION_STATE_PREVIEW_DIR=/privado/h26-pair-novo/fusion-auxiliary-balance
python tests/hal20-field-state/run.py --inputs notes/hal-26/INPUTS.json \
  --state-pair-preview --consolidation-case 1 --consolidation-case 2 --consolidation-case 3 \
  --consolidation-case 4 --consolidation-case 5 --consolidation-case 6 --consolidation-case 7 \
  --consolidation-case 8 --consolidation-case 9 --consolidation-case 10 \
  --guard 0 --output /tmp/h26-A-novo
python tests/hal20-field-state/run.py --inputs notes/hal-26/INPUTS.json \
  --state-pair-preview --consolidation-case 11 --guard 0 --output /tmp/h26-generated-novo
python tests/hal20-field-state/run.py --inputs notes/hal-26/INPUTS.json \
  --state-pair-preview --consolidation-case 1 --guard 1 --output /tmp/h26-A-refused-novo
```

A retorna INCOMPLETE para a coleção inteira. Somente o teste solicita escopo parcial para exercitar Queen+leases; não registrar esse helper como autorização do loader. Nenhum ponteiro/BSS é serializado. Pools de cartas geradas são recusados. Capturas durante pending/consumo/quarentena não retomam ações. O guard recusa os objetos experimentais por SHA. Não copiar as variantes para o staging canônico.

`preflight_negative_test.c` pode ser compilado com o mesmo GCC i386 e executado pelo runner. São29 casos tipados sem mutação de inputs. Os formatos260/380 bytes não são uma versão final do formato de save.

## Time Wizard privado

```sh
python tests/hal26-state-timewizard/timewizard_audit.py \
  --package '/privado/TIME WIZARD' --sdk "$MEMORIES_SDK" --work /privado/h26-tw-novo
python tests/hal26-state-timewizard/timewizard_boundaries.py \
  --core '/privado/TIME WIZARD/source/time-wizard-base-v0.23.1.o' \
  --relinked /privado/h26-tw-novo/package/time-wizard.o --output /tmp/tw-boundaries.json
python tests/hal26-state-timewizard/state_inventory.py \
  --dual "$MEMORIES_DUAL_DIR" --timewizard '/privado/TIME WIZARD' --output /tmp/state-inventory
python tests/hal20-field-state/run.py --inputs notes/hal-26/INPUTS.json \
  --field-bonus --terrain 0 --terrain 4 --terrain 6 --guard both \
  --timewizard-source '/privado/TIME WIZARD/source' --output /tmp/h26-tw-consumers-novo
```

O audit copia o pacote antes de executar seu builder/testes, preservando a origem. Compila adapters e relinka o core fixo; **não é rebuild completo**. O quinto C AI é compilado separadamente e não misturado ao núcleo. Os testes freestanding recebidos só têm o transporte de execução roteado para QEMU; fonte/asserts inalterados.

O probe combinado inclui o Encore real e extrai literalmente os predicados AI/Dual para um include temporário. Wheel/base init/fase TW são modelados; Fusion/provider e corpos relocados de predecessor são reais. Inspeção retira thunks de trace para aplicar a assinatura ao corpo correspondente. Guard0 inclui observer próprio: não afirmar que todo resultado desse caso é wrapper de produção. Guard1 tem hashes/callbacks vinculados ao inventário herdado.

**Conservar os FAIL de `AUDIT TW_AI_DETECTOR` e recusas da assinatura em matriz própria.** Um check positivo pode reproduzir um defeito; não aprova Time Wizard. `summarize.py` publica essa qualificação separada dos checks da coleção. Nenhum objeto TW é instalado/admitido pelo guard; nenhuma chance/regra de roleta alterada.

## Staging, build, Windows e auditoria

```sh
python tests/hal20-field-state/stage.py --output /privado/h26-colecao-nova --dual "$MEMORIES_DUAL_DIR"
python tools/pc/build_game32.py --target windows --release
python tests/hal26-state-timewizard/audit.py --output /tmp/h26-audit.json
```

O stager cria8 IDs e o recibo por hashes. Apontar MEMORIES_MODS_DIR/settings/userdir para o perfil privado; pacote oficial do build inclui opcionais não qualificados e não substitui esse staging. Nenhuma pasta com Dual, imagem do jogo, PE, core dump ou saves deve ser publicada.

O workflow `hal26-windows-state-control.yml` prepara controle PE32 independente no Windows. O fork está com Actions suspenso pelo GitHub; não houve execução. Build PE32 não significa runtime PASS. Não repetir Wine/QEMU comprovadamente indisponível para o mesmo controle. Somente gameplay pessoal fica para o final; a automação pode avançar com runner/recurso legítimo privado disponível.
