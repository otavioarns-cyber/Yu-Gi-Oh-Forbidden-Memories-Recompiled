# Fusion Effects beta.44-effect-ai — candidata HAL-21 / API 24

Esta branch prepara a correção F1 aprovada para revisão: **revogar leases no OFF efetivo**. Base `integration/hal-20-v0.3.1-preview.2`; fontes do PR #12 mais o patch `revoke.patch` do PR #21. Não é publicação de versão final nem validação de gameplay Windows.

- Pacote Windows **i386/API 24**, ID/library `fusion-auxiliary-balance`. Manifest, versão, 43 settings, receitas, valores e assets originais preservados.
- `applied(0)` neutraliza Encore e ambas as consultas de lease, encerrando somente `aux_unlocks`; o contador `aux_unlock_serial` e estados independentes permanecem. ON não restaura grants antigos; novo grant legítimo recebe novo serial.
- `restart:true` permanece. A preferência salva OFF pode aguardar reinício com o MOD ainda efetivamente ON. Os gates usam o callback real, sem consultar `host->applied()` como substituto.
- Objeto: `384e11eeeae32412e23afbbbd9ebc308761674b8a2181238fd60e17e315b865c`, loader hash `afb75202`. O ELF32 `.o` é o formato de MOD lido pelo Windows i386 e Linux i386; não é DLL ou executável do jogo.

[Relatório e limites HAL-21](../../notes/HAL21_FUSION_OFF_REVOKE_PREVIEW2.md) · [resultados](../../notes/hal-21/fix/MATRIZ.csv) · [proveniência atual](SDK-PROVENANCE.json) · [créditos/permissões](NOTICES-HAL20.md).

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py /caminho/fusion-auxiliary-balance --target i386
MEMORIES_TEST_RESULTS=/tmp/fusion-tests.json \
  python mods/fusion-auxiliary-balance/tests/run_preview2.py
```

O runner exige Python/Pillow, GCC e suporte i386. `MEMORIES_I386_CC` seleciona compilador/sysroot; `MEMORIES_I386_RUNNER` pode prefixar QEMU. Suite desta candidata: 16 grupos PASS/0 FAIL, sem imagem do jogo. [Testes de lifecycle/consumidores](../../tests/hal21/README.md) executam também o objeto Ritual Effects real, com motor parcial. Os controles originais API18/API24 continuam falhando F1 nos resultados históricos/diagnósticos; a candidata passa o contrato OFF.

Para teste manual no Windows use a pasta completa, uma instalação por ID e comece duelo novo após reiniciar, preservando os dados pessoais fora do repositório. Não instalar junto com outra variante do mesmo MOD. O mínimo 15 do manifest original não torna este objeto API24 compatível com hosts antigos. Não presumir compatibilidade de savestates anteriores.

Time Wizard guarda elegibilidade em tentativas já iniciadas ON: a correção do provider não cancela esse cache. Seu núcleo/ponte Dual e a matriz integrada AI/Dual continuam pendentes. Nenhum desses projetos foi alterado.

Histórico: [relatório original do PR #12](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/907ed98cfb763a96f5aa1ad6369f809cceca5f1c/notes/HAL20_FUSION_EFFECTS_PREVIEW2.md); a falha F1 ali registrada corresponde ao objeto antigo. `SDK-PROVENANCE.hal20-pr12.json` preserva exatamente os metadados anteriores; `SDK-PROVENANCE.preview1.recebido.json` é evidência recebida ainda mais antiga. A proveniência atual é exclusivamente `SDK-PROVENANCE.json`.
