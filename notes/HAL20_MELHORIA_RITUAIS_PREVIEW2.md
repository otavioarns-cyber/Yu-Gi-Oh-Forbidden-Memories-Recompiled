# HAL-20 — Melhorias de Rituais + Surrender 1.3.6 / preview.2

## Resultado

MOD recompilado isoladamente pelo SDK oficial v0.3.1-preview.2, alvo i386. Pronto como candidato para teste integrado; gameplay Windows e Dual Play real ainda não validados. Nenhuma alteração nos dois fontes de produção, nos quatro headers ou no mod.json foi necessária. ID, versão 1.3.6, cinco settings, defaults e mecânicas permanecem byte a byte iguais ao ZIP original.

Base obrigatória: integration/hal-20-v0.3.1-preview.2 em d649647c159c91d615c07d6960928a37e270b461. Branch desta entrega: mods/hal-20-melhoria-dos-rituais-preview2. Nenhum merge, alteração de master/integração, core ou outros MODs.

## Arquivos e proveniência

Pacote importado de MelhoriaDosRituais em mods att.zip (SHA-256 745f6fbbd5f3c9451ca37735626abbbc4c6704bc5326ab3691be33d86b66281b) para mods/melhoria-dos-rituais/. Incluídos os fontes, manifest, testes e documentos históricos. Excluídos ambos os objetos antigos: rituals.o e rituals.x86_64-windows.o. Publicado somente rituals.o novo, em ELF32 i386, e SDK-PROVENANCE.json com hashes de insumos, compilador, SDK e artefato.

Fonte de produção: rules.c, selection.h, surrender.c, surrender.h, ritual_recipe_provider_v1.h, ritual_rules_provider_v1.h. API mínima 4 preservada: o host API 24 oferece os serviços usados; não há razão para exigir API 24 no manifest apenas por recompilar. Biblioteca rituals, slus_01411, enabled e todos os settings preservados. Não há requires; recipe_v1 de Ritual Effects é consulta opcional com fallback. rules_v1 é publicado pelo próprio MOD. Não foram instalados parceiros.

Mudanças de teste: test_rules_provider.py usa MEMORIES_SDK para localizar headers em vez do caminho histórico sdk_build; novos preview2_instances.c, preview2_loader.c e run_preview2.py. Nenhum desses testes entra na compilação de produção, que compila apenas os dois C da raiz. Documentos antigos permanecem históricos, não resultados atuais.

## Build e validação

Executado a partir do checkout de fontes da base:

```sh
export PATH="$PWD/tmp/pc/llvm-mingw/bin:$PATH"
python3 tmp/pc/win32/sdk/tools/build_mod.py mods/melhoria-dos-rituais --target i386 --game tmp/pc/win32/sdk --out mods/melhoria-dos-rituais
python3 mods/melhoria-dos-rituais/tests/run_preview2.py
```

O runner exige GCC multilib para os harnesses i386 e executável ELF32 no host. Nesta sessão foram usados MEMORIES_I386_CC com wrapper GCC/sysroot já preparado e MEMORIES_I386_RUNNER com qemu-i386-static -L seguido do sysroot da base; MEMORIES_SDK aponta ao SDK Windows gerado da própria tag. Isso é emulação Linux, não execução Windows. O pacote não inclui SDK/toolchains nem arquivos proprietários. Fonte completo e SDK-alvo são necessários para repetir os testes; o objeto pronto serve ao Windows i386.

Resultados: build de 2 fontes passou com Clang 23.1.2/llvm-mingw 20260922. Os 33 imports do novo objeto constam nos exports i386 de ambos os builds Linux e Windows. Hash novo difere do objeto recebido, conforme proveniência. Os nomes encontrados não comprovam sozinhos compatibilidade de runtime.

- Provider rules_v1: defaults, limites, ABI/tamanho, mudanças de settings e desativação, função de produção com UBSan.
- Surrender: 9 cenários compilando corpos exatos dos hooks, com UBSan: Select contra CPU, confirmação Sim seguindo fase normal 0xC/CPU vencedor, Não, ausência de Select, MOD desativado, turno CPU, caminho nativo de dois humanos, diálogo existente, reset e prevenção de pedido duplicado. A contagem/agrupamento é a do teste original; não são partidas reais.
- Provider de receitas: fixture de 15 cenários; executada com UBSan no runner e também com ASan/UBSan em chamada complementar (leak detection desligada). Provider ausente/desativado, ABI/callbacks inválidos, restrições e confirmação pela cadeia.
- Visuais sintéticos: 128 combinações de origem/lado; três objetos dos materiais escolhidos, exclusão de monstro alheio e fallback nativo.
- Novo harness inclui rules.c inteiro e chama o seletor/consumidor reais: campo+mão, preservação de cópia idêntica não selecionada, três tributos no campo, preservação de monstro alheio, exigência de cópias físicas distintas, mínimo no campo e prioridade campo; reset de referências em OFF/ON. Engine/render não são executados.
- Novo harness carrega o rituals.o recompilado com object_loader.c oficial: 20 ciclos load/init/reset/OFF/ON e sete registros de hooks, host simulado. Não testa o gerenciamento completo, despacho dos hooks em duelo nem desativação durante animação.

Os testes exercitam a colocação Vanilla e fallback dos objetos; preservação de todas as receitas Vanilla e animações no core completo ainda exige jogo. A resolução e os efeitos visuais não foram simulados integralmente. Não se afirma ausência universal de travamentos.

## Limitações, compatibilidade e próximos testes

Não foi encontrada incompatibilidade que exigisse patch de produção. O primeiro protótipo do harness declarou uma função stub com tipo diferente do header; corrigido somente no teste para void*. A chamada inicial de compilação do harness falhou por esse tipo; a suíte final passou. test_rules_provider tinha caminho antigo de SDK; corrigido somente nesse teste. Não são correções de mecânica.

Não houve imagem do jogo, execução real Windows, controles físicos, animação/resultado de derrota completos, save/load durante ritual ou teste integrado com AI HARD MODE/Dual Play. Surrender delega o caminho de dois humanos e turno CPU ao original, mas o MOD Dual Play (Monster + Support) não foi carregado nesta etapa. Por isso sua não interferência real é pendente, não comprovada pela simulação de dois humanos. AI oficial/custom e outros parceiros permanecem inalterados.

Para teste manual posterior, manter uma única instalação do ID melhoria-dos-rituais e usar a pasta completa com rituals.o novo; não combinar objetos antigos/x64 nesta candidata. Testar três tributos em campo, 2 campo+1 mão, 1 campo+2 mão, cópias iguais, mínimos configurados, rituais Vanilla, provider ausente e presente, consumo/visuais, Select Sim/Não/cancelar, turnos CPU e humanos, quotas do Dual Play, novo duelo, ativar/desativar fora de ação pendente e save/load. Não promover a master antes da revisão e da regressão integrada.

## Resultados registrados


```text
mods/melhoria-dos-rituais/rituals.o: 2 sources (tmp/pc/mod-build/melhoria-dos-rituais-d7847267e3e355a6)
build_mod: built i386
PASS Melhoria read-only rules_v1 production callback: defaults, live changes, 0-3, result, disabled and ABI size
PASS: 9 surrender cases (exact production hook bodies)
PASS: 128 source/side combinations; distinct Deck entrances; single-Deck preserved
PASS: extended flight uses 3 selected tributes; B. Skull excluded; native fallback preserved
PASS: 15 provider integration scenarios (compiled exact production function)
PASS production selector/consumption: mixed sources, Vanilla placement, duplicate identity, minima, reset OFF/ON
PASS isolated preview2 suite; gameplay and actual Dual Play integration remain untested
PASS rebuilt ELF: 20 load/init/reset/OFF/ON cycles, seven hook registrations; simulated host, no duel hooks executed
```

Objeto novo SHA-256: `c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0`.
