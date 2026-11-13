# HAL-21 — provas isoladas de lifecycle/providers

Estas provas usam a base oficial API 24, o pacote exato do PR #12 e dois patches alternativos. Não são uma validação de gameplay Windows. Não aplicar os dois patches nem instalar dois objetos com o mesmo ID.

## Insumos

- Checkout da integração em `d649647c159c91d615c07d6960928a37e270b461` com esta pasta de testes.
- Pacote completo Fusion do PR #12, commit `907ed98cfb763a96f5aa1ad6369f809cceca5f1c`, sem alteração local. Os hashes de todos os arquivos estão em `notes/hal-21/INPUTS.json`.
- Objeto API 18 recebido, hash `1d46c5f812759de21564e20ab88eddc79a268534c6e1055b7051b3dfefd88821`, somente local.
- Pacote Ritual Effects do PR #11, com objeto `c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1`.
- Ponte-fonte `time_wizard_encore.c` recebida, conferida por hash. Não requer nem simula equivalência com o núcleo compilado completo do Time Wizard. Essa fonte e o executável que a inclui ficam somente no diretório temporário local.
- SDK oficial Windows/i386 API 24, Clang e Python; compilador/linker Linux i386 com libpng/pthread; runner i386 nativo ou QEMU com sysroot. Consulte `PROVENIENCIA.json` para identificar as ferramentas utilizadas.

## Execução

Defina os caminhos locais nos nomes abaixo; não copie fontes privados para este repositório:

```sh
export MEMORIES_FUSION_SOURCE=/caminho/fusion-auxiliary-balance
export MEMORIES_FUSION_API18=/caminho/original/fusion-auxiliary-balance.o
export MEMORIES_RITUAL_SOURCE=/caminho/ritual-card-effects
export MEMORIES_TIME_WIZARD_SOURCE=/caminho/time_wizard_encore.c
export MEMORIES_SDK_DIR=/caminho/sdk
export MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang
export MEMORIES_I386_CC=/caminho/gcc-i386
# Deixe vazio se o executável Linux i386 roda diretamente.
export MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot'
python tests/hal21/run.py --output /tmp/hal21-resultados --artifact-dir /tmp/hal21-candidatos
python tests/hal21/on_suite.py --output /tmp/hal21-on
python tests/hal21/abi.py \
  --original "$MEMORIES_FUSION_SOURCE/fusion-auxiliary-balance.o" \
  --candidate /tmp/hal21-candidatos/hide/fusion-auxiliary-balance.o \
  --candidate /tmp/hal21-candidatos/revoke/fusion-auxiliary-balance.o \
  --sdk-exports "$MEMORIES_SDK_DIR/exports.i386.txt" \
  --output /tmp/hal21-abi.json
```

`run.py` retorna **1** na rodada diagnóstica concluída porque mantém as falhas reais do baseline e os riscos de hide como FAIL. **2** indica etapa incompleta/bloqueada; uma exceção de pré-requisito também exige investigação. Não converter tudo em PASS. `on_suite.py` e `abi.py` retornam 0 somente se suas verificações passarem. O `--artifact-dir` evita alterar os candidatos publicados; o default é `artifacts/hal21`.

O runner confere os insumos, reproduz F1 nos dois objetos originais, monta cada cópia temporária a partir do PR #12, aplica somente seu patch e chama `SDK/tools/build_mod.py <cópia> --target i386`. Os `.o` são ELF32 relocáveis, o formato de MOD do SDK Windows i386; não são DLLs nem executáveis Windows. Os hashes brutos identificam exatamente os artefatos desta entrega; o SDK inclui caminhos DWARF de compilação, portanto outro diretório temporário pode mudar SHA-256 sem mudar o programa. Não prometer igualdade binária entre workspaces arbitrários.

## Fronteiras e instrumentação

- Loader legado: assertions originais de `tests/preview2_loader.c`, com apenas caminhos de três includes ajustados na cópia temporária. F1 usa Musician King físico e lease 77 semeada, exatamente como o #12. Não é criação dessa lease por gameplay.
- `manager_bridge.c` inclui o `mods.c` oficial sem edição. `Hal21_ForceApplied` chama `activate` para entregar `applied(0/1)` e relink oficiais, **contornando deliberadamente `restart:true`** somente no experimento. As provas separadas de `Mods_SetEnabled`, `Mods_Apply` e reload NÃO contornam essa política.
- `manager_fixture.c` usa loader, registry, manager, settings, reset, eventos e hooks reais. Engine/cartas/identidades/controlador são modelados. Leases e estado borrowed são semeados explicitamente nos casos de identidade; não há afirmação de reprodução de um duelo completo. O teste de novo grant chama o `aux_unlock_bind` real com a convenção fastcall privada observada no objeto Clang; isso não é uma nova API pública.
- Funções consumidoras de Encore/lease são extraídas literalmente das fontes externas conferidas. Além disso o objeto PR #11 real executa `borrowed_reconcile`, e fornece o catalogue `effect_intelligence_v1`.
- `tw_adapter.c` compila localmente a ponte recebida, com núcleo/wheel modelados. Exercita callbacks e cache da ponte; não carrega o núcleo privado, não resolve a ponte Dual e não distribui Time Wizard.
- `on_suite.py` executa 14 grupos originais Python/C. C roda nativo com ASan/UBSan; um helper sempre-ON preenche o owner ausente nos mocks isolados. Extrai os dois helpers atuais que a fixture Fusionist espera. OFF é testado nos objetos i386 reais, não nesse helper.
- SAVE/LOAD dispara eventos e visita os estados realmente registrados. Os MODs observados não registram chunks; não se serializa artificialmente BSS como formato do jogo. Reset/LOAD simulados não demonstram compatibilidade com savestates antigos.

Os resultados públicos são `notes/hal-21/contracts/`, `notes/hal-21/on/`, `MATRIZ.csv`, `ABI.json` e o relatório principal. Nenhum teste requer ROM ou save pessoal.
