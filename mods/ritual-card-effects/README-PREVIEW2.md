# HAL-25 — hardening experimental (não release)

Esta cópia deriva do PR #25 e inclui limites de retry, recuperação explícita, desconexão cooperativa e proteção de identidade até o primeiro consumo. ABI1 de seleção, manifest, settings e assets preservados. O companion de lifecycle continua experimental.

[Relatório e limites](../../notes/HAL25_CONTRACT_HARDENING.md). Não promover à integração nem tratar os objetos como gameplay Windows aprovado.

---

Registro anterior do PR #25, preservado como histórico (seus hashes referem-se ao experimento anterior):

# HAL25 — contrato de seleção EXPERIMENTAL

Cópia experimental do PR11 com contrato real. Não instalar como versão final. Manifest/settings/assets preservados; fontes, objeto e proveniência atualizados. Leia [o relatório HAL25](../../notes/HAL25_SELECTION_CONTRACT_REAL_POC.md) e [o contrato](../../notes/hal-25-contract/CONTRATO.md). A descrição original abaixo registra a entrega anterior, não as mudanças atuais.

# Ritual Effects 0.78.74-effect-ai — candidato API 24 / HAL-20

Esta entrega recompila os dois C originais para **Windows i386**, usando o SDK oficial v0.3.1-preview.2. O formato correto do MOD é ELF32 `.o`, lido pelo próprio jogo. O mesmo objeto i386 também pode ser carregado pela base Linux i386. Não é um build x64 e não inclui o objeto x64 antigo.

ID `ritual-card-effects-test`, library `ritual-card-effects`, versão, manifest, 48 settings, fontes/headers de produção, textos e assets foram preservados. As mudanças de código desta etapa estão somente em testes. Os documentos históricos recebidos continuam históricos; a entrega atual está no [relatório técnico](../../notes/HAL20_RITUAL_EFFECTS_PREVIEW2.md) e em SDK-PROVENANCE.json. A proveniência antiga divergente foi conservada em SDK-PROVENANCE.preview1.recebido.json. Consulte também NOTICES-HAL20.md.

Para recompilar, aponte para o SDK gerado do commit oficial aceac3782902746f35ddb0148809450933327cb3 e para o Clang do toolchain suportado:

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py /caminho/ritual-card-effects --target i386
```

Para executar os testes a partir de um checkout da branch:

```sh
python mods/ritual-card-effects/tests/run_preview2.py
```

Requer Python, Pillow e GCC com suporte i386. Em ambiente que usa sysroot/QEMU, configure MEMORIES_I386_CC e MEMORIES_I386_RUNNER. MEMORIES_TEST_RESULTS recebe opcionalmente o caminho de saída JSON. Os três testes diferenciais históricos exigem MEMORIES_RITUAL_BASELINE da versão antiga correspondente e são marcados SKIP quando ela não está disponível. Não compará-los com a própria versão para anunciar ganhos.

Para teste manual no Windows i386, preserve sua instalação/saves anteriores, mantenha somente uma pasta ativa com este ID e use o diretório inteiro com seus assets/textos. `restart:true` foi mantido: reinicie o jogo e inicie um duelo novo. Não instale ao lado de outra versão com o mesmo ID. O candidato ainda precisa de teste com a imagem legítima do jogo, fora do Git.

Save states de outra base/objeto não são garantidos. O comportamento original de reset limpa as classificações/estados dos efeitos e exige novo Ritual após load; esta etapa não implementa persistência desses marcadores. Os testes usam host/motor simulados e não validam o conjunto com outros MODs, controles reais, áudio ou renderização Windows.
