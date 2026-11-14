# Reprodução da coleção Field/state

Leia [relatório e limites](../../notes/HAL20_FIELD_STATE_INTEGRATION.md). Os scripts derivam do harness PR28 e preservam fixtures históricas. Fontes M/R/RD intactas; F/AI/Field possuem os deltas técnicos descritos. Dual só local, nunca baixado ou publicado pelo teste.

## Insumos

Python/Pillow, GCC/sysroot i386, QEMU se ELF32 não executar diretamente, SDK Windows API24/Clang e objetos Linux i386 da base. `INPUTS.json` exige hashes exatos; caminhos abaixo são exemplos, não recursos incluídos no Git.

```sh
export MEMORIES_AI_DIR="$PWD/mods/ai-hard-mode"
export MEMORIES_DUAL_DIR=/privado/dual-api24
export MEMORIES_RITUALS_DIR="$PWD/mods/melhoria-dos-rituais"
export MEMORIES_FUSION_DIR="$PWD/mods/fusion-auxiliary-balance"
export MEMORIES_ENGINE_OBJ_DIR=/base/game32/obj
export MEMORIES_SDK_EXPORTS=/base/game32/sdk/exports.i386.txt
export MEMORIES_I386_CC=/tools/gcc-i386
export MEMORIES_I386_RUNNER='/tools/qemu-i386-static -L /tools/sysroot'
export MEMORIES_SDK=/base/win32/sdk
export MEMORIES_MOD_CC=/tools/llvm-mingw/bin/clang
```

`SDK-PROVENANCE.json` por pacote descreve builds canônicos. Recompilar com `python "$MEMORIES_SDK/tools/build_mod.py" /pasta/mod --target i386` pode mudar debug paths/SHA; não atualizar a allowlist só para aceitar binário diferente sem repetir a qualificação. `qualify.py` gera recibos a partir de registros reais do loader. `audit.py --output /tmp/audit.json` verifica escopo e proveniência.

## Staging e build

```sh
python tests/hal20-field-state/stage.py --output /privado/colecao-nova --dual "$MEMORIES_DUAL_DIR"
python tools/pc/build_game32.py --target windows --release
```

O stager recusa destino existente ou dentro do Git; cria oito IDs únicos M/R/RD/F/E/H/D/DMC com objetos fixados. Só o manifest AI da cópia privada recebe restart:true. `--profile owned-only` exclui H/D, inclui cinco pacotes próprios e DMC. Aponte `MEMORIES_MODS_DIR`, `MEMORIES_SETTINGS`, `MEMORIES_USER_DIR` para os caminhos privados do recibo. Não publique essa pasta. O pacote oficial do build não substitui o staging customizado.

## Matrizes principais

Cada comando escreve RESULTADOS.json/MATRIZ.csv/logs; não usar o mesmo output de outra execução.

```sh
python tests/hal20-field-state/run.py --field --guard 0 --output /tmp/field-offguard
python tests/hal20-field-state/run.py --field --guard 1 --output /tmp/field-guard
python tests/hal20-field-state/run.py --field-bonus --guard 1 --output /tmp/field-bonus
python tests/hal20-field-state/run.py --field --guard 1 --trace-returns --quick --output /tmp/field-trace
python tests/hal20-field-state/run.py --with-field --test-terrain 2 --guard 1 --output /tmp/ritual-field
python tests/hal20-field-state/run.py --composition --guard both --output /tmp/composition
python tests/hal20-field-state/run.py --providers --guard 1 --output /tmp/providers
python tests/hal20-field-state/run.py --consolidation --guard 1 --output /tmp/state-B
python tests/hal20-field-state/run.py --hardening --output /tmp/hardening
python tests/hal20-field-state/run.py --boundary --output /tmp/boundary
python tests/hal20-field-state/run.py --recovery-only --output /tmp/recovery
python tests/hal20-field-state/run.py --reset-recovery-only --output /tmp/reset
python tests/hal20-field-state/run.py --restrictions-only --output /tmp/restrictions
HAL20_OUTPUT=/tmp/AI-parity python tests/hal20-field-state/ai_parity.py
```

As opções guard/quick/terrain dos ensaios finais estão também nos campos `command`/`args` e logs dos arquivos por suíte. A matriz publicada contém execuções suplementares sobrepostas, não duelos únicos. `inventory.py --help` reconstrói os metadados de wrappers; `summarize.py --logs /logs --output /saida` agrega somente os nomes finais explícitos e conserva os FAIL dos controles.

Controles: `--original-field` requer `MEMORIES_FIELD_ORIGINAL_DIR` com a fonte histórica recompilada; `--original-fusion` requer apontar MEMORIES_FUSION_DIR ao PR22; `--reference26` requer `MEMORIES_PR26_DIR`. Esses controles devem preservar as falhas documentadas. Não usá-los no staging.

## A: variante limitada de persistência

```sh
python tests/hal20-field-state/build_state_preview.py --sdk "$MEMORIES_SDK" --output /privado/state-preview-novo
export MEMORIES_STATE_PREVIEW_DIR=/privado/state-preview-novo/ritual-card-effects
python tests/hal20-field-state/run.py --state-preview --guard 0 --output /tmp/state-A
python tests/hal20-field-state/run.py --state-preview --guard 1 --output /tmp/state-A-refused
```

A variante não substitui o R canônico, não é admitida pelo guard e não oferece persistência completa: recupera Queen no modelo, mas perde lease Fusion e não cobre todo estado. Não serializa ponteiros/BSS. Rejeição do owner após memória restaurada não é rollback global. B recusa antes da sobrescrita; nenhuma política foi promovida definitivamente.

## Regressões e Windows

As regressões canônicas R/RD/F são preservadas nas pastas anteriores e seus resultados estão em `notes/hal-20-field-state/regressions`; o adaptador de runner i386 é o mesmo do PR28. `field_text_test.c`, `state_policy_probe.c` e `ai_field_parity.c` são probes próprios, não gameplay.

[Backend Windows](windows/README.md): executar controle mínimo antes de afirmar jogo inicializado. Somente gameplay com participação pessoal fica para a fase final. Nunca publicar jogo/imagem/prefixos/Wine/Dual privado.
