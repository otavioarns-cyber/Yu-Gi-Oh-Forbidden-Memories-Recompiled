# HAL-21 — regressão da candidata Revogar

Base oficial `d649647c159c91d615c07d6960928a37e270b461`. Reaproveita as assertions do [PR #21](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/21) e acrescenta um perfil que carrega **o objeto efetivamente publicado** em `mods/fusion-auxiliary-balance/`. Não há novo MOD ou provider obrigatório.

## Insumos externos

Hashes dos insumos em `notes/hal-21/fix/INPUTS.json`:

- Fusion original do PR #12, commit `907ed98cfb763a96f5aa1ad6369f809cceca5f1c`, pacote completo intacto.
- Objeto API18 recebido, SHA `1d46c5f812759de21564e20ab88eddc79a268534c6e1055b7051b3dfefd88821`, somente local.
- Ritual Effects PR #11: objeto `c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1` e fontes consumidores conferidos.
- `time_wizard_encore.c` recebido, conferido por hash, somente local. A ponte é compilada temporariamente com núcleo/wheel modelados; não distribuímos sua fonte/executável nem o núcleo completo.
- SDK oficial Windows i386 API24 e Clang identificados em `SDK-PROVENANCE.json`; Python/Pillow, GCC ASan/UBSan e compilador Linux i386 com libpng/pthread; runner nativo ou QEMU/sysroot.

## Comandos

```sh
export MEMORIES_FUSION_SOURCE=/pacote/original/PR12/fusion-auxiliary-balance
export MEMORIES_FUSION_API18=/original/fusion-auxiliary-balance.o
export MEMORIES_RITUAL_SOURCE=/pacote/PR11/ritual-card-effects
export MEMORIES_TIME_WIZARD_SOURCE=/privado/time_wizard_encore.c
export MEMORIES_SDK_DIR=/sdk/oficial/API24
export MEMORIES_MOD_CC=/llvm-mingw/bin/clang
export MEMORIES_I386_CC=/compilador/gcc-i386
export MEMORIES_I386_RUNNER='/runner/qemu-i386-static -L /sysroot'
python "$MEMORIES_SDK_DIR/tools/build_mod.py" mods/fusion-auxiliary-balance --target i386
MEMORIES_TEST_RESULTS=/tmp/hal21-original.json python mods/fusion-auxiliary-balance/tests/run_preview2.py
python tests/hal21/run.py --candidate-source mods/fusion-auxiliary-balance \
  --artifact-dir /tmp/hal21-references --output /tmp/hal21-contracts
python tests/hal21/on_suite.py --output /tmp/hal21-on
python tests/hal21/abi.py --original "$MEMORIES_FUSION_SOURCE/fusion-auxiliary-balance.o" \
  --candidate mods/fusion-auxiliary-balance/fusion-auxiliary-balance.o \
  --sdk-exports "$MEMORIES_SDK_DIR/exports.i386.txt" --output /tmp/hal21-abi.json
```

Deixe `MEMORIES_I386_RUNNER` vazio se o kernel executar i386. `--artifact-dir` mantém objetos alternativos apenas fora da árvore instalável. Os patches em `reference/` são referências históricas byte a byte do PR #21 para os testes; **ocultar não é política desta candidata**. Não aplicar ambos nem publicar/copiar objetos privados de consumidores.

Para comparar com os artefatos aprovados do PR #21, disponibilize sua pasta `artifacts/hal21` localmente:

```sh
python tests/hal21/compare.py --source "$MEMORIES_FUSION_SOURCE" \
  --references /checkout/PR21/artifacts/hal21 --output /tmp/hal21-comparacao.json
```

O comparador exige fonte igual ao PR12+patch aprovado, inventário intacto, manifest/assets idênticos, seções alocáveis iguais ao Revogar de referência e diferenças brutas restritas a debug. `abi.py` verifica os 131 imports, dois exports públicos e exports do SDK; o loader verifica API24 e o hash de código. SDK/DWARF inclui caminhos de trabalho: SHA bruto pode variar entre diretórios.

## Interpretação e fronteiras

`run.py` mantém os FAIL do baseline/API18/API24 e riscos de ocultar no JSON/CSV. Retorna **0 somente se os três perfis da candidata terminarem sem falhas**; 1 é falha da candidata; 2 é etapa incompleta. Uma exceção de insumo também não é sucesso. Os outros runners retornam 0 somente com seus checks aprovados. Não confundir os FAIL esperados dos controles antigos com regressão da candidata, nem apagá-los.

- Fixture carrega ELF/manager/registry/settings/reset/hooks/eventos oficiais reais, com engine parcial sintética. Sem gameplay Windows.
- `manager_bridge.c` inclui core original sem editá-lo. `Hal21_ForceApplied` contorna `restart:true` deliberadamente para entregar `applied(0/1)` e relink reais. `Hal21_CallbackOnly` isola o contrato do callback com preferência ainda ON. Esses caminhos instrumentados não são prova de que a UI permite a transição durante um duelo.
- As provas separadas de `Mods_SetEnabled`, `Mods_Apply` e reload preservam restart. Um novo processo com preferência OFF não inicializa providers.
- Leases, registros físicos e borrowed origins são semeados explicitamente nos casos de identidade; nada disso é apresentado como aquisição por duelo. `aux_unlock_bind` e `borrowed_reconcile` são funções reais; a primeira usa fastcall privado observado no objeto, não um novo contrato público.
- Cinco checks adicionais da candidata comparam a imagem carregada antes/depois de callback OFF, permitindo mutação apenas em `ea_applied` e `aux_unlocks`; conferem serial, marcas Fusion/Metal não zero e registro externo; preferência ON/owner OFF; todos os callbacks forecast OFF; OFF idempotente; retorno ON sem recuperar grant.
- O `borrowed_reconcile` do objeto Ritual PR #11 e seu provider intelligence são reais. As funções Encore/lease consumidoras são extraídas literalmente das fontes conferidas para includes temporários.
- Time Wizard: ponte real disponível com wheel/núcleo modelados. Novas consultas OFF são neutras; tentativa iniciada ON ainda usa seu cache. Não é validação do núcleo compilado ou da ponte Dual.
- SAVE/LOAD simulado despacha eventos e visita chunks realmente registrados, com reset; nenhum formato BSS persistente foi inventado. Não há certificação de savestates antigos.
- `on_suite.py` repete 14 grupos em baseline/hide/revoke/candidate. Para referências usa o helper sempre-ON do PR #21; a candidata tem o mesmo modelo explicitamente dentro de sua fixture isolada. `run_preview2.py` mantém todas as assertions originais e agora exige PASS OFF; sua única adaptação de lógica de fixture é declarar o novo helper de owner ON em `test_fusionist_runtime.c`.

Resultados públicos: [matriz](../../notes/hal-21/fix/MATRIZ.csv), [relatório](../../notes/HAL21_FUSION_OFF_REVOKE_PREVIEW2.md). Não há ROM, saves pessoais ou arquivos privados do Dual/Time Wizard nesta entrega.
