# Fusion Effects beta.44-effect-ai — candidato API 24 / HAL-20

Build isolado para **Windows i386**, SDK oficial v0.3.1-preview.2. ID/library `fusion-auxiliary-balance`, 43 settings, fontes/headers de produção, manifest, receitas e assets preservados. O objeto ELF32 `.o` é o formato correto do loader Windows i386 e também serve ao Linux i386. Não é um build x64. Os objetos antigos foram excluídos da pasta instalável; seus hashes e a proveniência antiga foram conservados como evidência.

**Pendência confirmada:** Encore e as consultas de lease continuam respondendo depois de OFF; effect_forecast_v2 desliga corretamente. O mesmo comportamento foi reproduzido no objeto antigo. Não foi corrigido nesta migração, que preserva a lógica recebida. A suíte conserva esse teste como FAIL e retorna status não zero. Não apresentar o pacote como integralmente validado ou pronto para integração sem essa ressalva.

Consulte o [relatório técnico](../../notes/HAL20_FUSION_EFFECTS_PREVIEW2.md), [resultados](../../notes/hal-20/fusion-effects/RESULTADOS.json), SDK-PROVENANCE.json e NOTICES-HAL20.md. Os documentos históricos do pacote continuam históricos; a descrição original do manifest ainda menciona beta.43.

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py /caminho/fusion-auxiliary-balance --target i386
python mods/fusion-auxiliary-balance/tests/run_preview2.py
```

Testes exigem Python/Pillow, GCC e suporte de compilação/execução i386. MEMORIES_I386_CC seleciona um wrapper/sysroot; MEMORIES_I386_RUNNER pode prefixar QEMU com seu sysroot. MEMORIES_TEST_RESULTS recebe o caminho de saída JSON. O runner não executa a imagem do jogo ou os outros MODs.

Para teste manual autorizado no Windows i386, use a pasta inteira e somente uma instalação deste ID, preservando sua instalação/saves anteriores. `restart:true` e o mínimo do manifest 15 foram conservados, mas este objeto declara API 24 e é destinado à base preview.2. Reinicie o jogo e comece duelo novo. Gameplay, áudio/renderização e a coleção integrada ainda não foram validados.

Não presumir compatibilidade de savestates antigos: o hash do objeto mudou e o reset original limpa origens, leases e estados dos efeitos. A migração não implementa persistência desses dados e não altera essa regra.
