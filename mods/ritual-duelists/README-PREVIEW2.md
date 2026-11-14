# HAL-25 — hardening experimental (não release)

Esta cópia deriva do PR #25 e inclui limites de retry, recuperação explícita, desconexão cooperativa e proteção de identidade até o primeiro consumo. ABI1 de seleção, manifest, settings e assets preservados. O companion de lifecycle continua experimental.

[Relatório e limites](../../notes/HAL25_CONTRACT_HARDENING.md). Não promover à integração nem tratar os objetos como gameplay Windows aprovado.

---

Registro anterior do PR #25, preservado como histórico (seus hashes referem-se ao experimento anterior):

# HAL25 — contrato de seleção EXPERIMENTAL

Cópia experimental do PR13 com contrato real. Não instalar como versão final. Manifest/settings/assets preservados; fontes, objeto e proveniência atualizados. Leia [o relatório HAL25](../../notes/HAL25_SELECTION_CONTRACT_REAL_POC.md) e [o contrato](../../notes/hal-25-contract/CONTRATO.md). A descrição original abaixo registra a entrega anterior, não as mudanças atuais.

# Candidato HAL-20 — Ritual Duelists alpha.8 / API 24

Recompilado integralmente para Windows i386 pelo SDK v0.3.1-preview.2. Os 16 C, 22 H, mod.json e 57 assets são idênticos ao pacote recebido. O README original e os documentos spec são históricos; resultados atuais estão no [relatório](../../notes/HAL20_RITUAL_DUELISTS_PREVIEW2.md) e nos [testes](../../notes/hal-20/ritual-duelists/RESULTADOS.json).

Use somente uma instalação do ID `ritual-duelists`. O pacote contém apenas o novo `ritual-duelists.o`, ELF32 i386, sem o objeto x64 antigo. Não instalar junto do antigo RitualStarter separado. Manifest mantém seis settings, API mínima 12 e restart true; o objeto foi testado com host API 24, não certificado em hosts antigos.

Drop Missing Cards 1.0 ativo continua sendo pré-requisito do balanceamento descrito pelo autor. `after` apenas ordena MODs presentes/ativos e não instala ou ativa essa dependência. A integração precisa ativá-lo expressamente; nenhum default de outro MOD foi alterado aqui.

O visualizador Free Duel continua exclusivamente no Droplist 1.4.0 externo, que não acompanha esta entrega. Não foi reintroduzido um visualizador interno ou consumo de Triangle. O diagnóstico overlay existente foi preservado.

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py /caminho/ritual-duelists --target i386
MEMORIES_SDK=/caminho/sdk \
MEMORIES_I386_CC=/caminho/gcc-com-multilib \
MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot' \
MEMORIES_TEST_RESULTS=/tmp/rd-resultados.json \
  python tests/run_preview2.py
```

O runner precisa de checkout da base, SDK, Python/Pillow, GCC com sanitizadores e suporte i386. O prefixo QEMU é opcional em sistemas que executam ELF32 diretamente. A suíte registra 35 grupos PASS, zero FAIL e três grupos SKIP. Ela usa engine/providers simulados; não comprova gameplay Windows, render HD, controles, save/load real ou compatibilidade integrada. Há riscos de ordem dos hooks de IA/materialização com AI HARD MODE, Dual Play, Ritual Effects e Time Wizard, detalhados no relatório. A correção F1 de Fusion/HAL-21 não foi incorporada.

Veja [proveniência](SDK-PROVENANCE.json) e [origem das artes](NOTICES-HAL20.md). Não presumir compatibilidade de savestates antigos.
