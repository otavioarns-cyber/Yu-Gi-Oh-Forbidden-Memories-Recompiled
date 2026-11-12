# HAL-20 — Ritual Effects 0.78.74-effect-ai / API 24

## Resultado

**Recompilado isoladamente para Windows i386 com o SDK oficial v0.3.1-preview.2/API 24, sem alterar os dois C, os 29 headers de produção, o manifest, efeitos, receitas ou decisões da IA.** O candidato passou por 23 grupos de testes locais; três comparações históricas ficaram SKIP por falta da versão-base antiga. Pronto para a próxima validação integrada; ainda não validado em gameplay Windows.

Base: `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`, com fonte oficial `aceac3782902746f35ddb0148809450933327cb3`. Branch independente: `mods/hal-20-ritual-effects-preview2`. Pacote em `mods/ritual-card-effects/`, preservando ID `ritual-card-effects-test` e library `ritual-card-effects`. Não foi incorporado qualquer outro MOD customizado. Master, integração, branches antigas e PRs #7–#10 preservados, sem merges.

Antes da implementação foram lidos AGENTS.md, os relatórios/entregas dos [PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/7), [PR #8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8), [PR #9](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/9) e [PR #10](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/10). Todos estavam abertos/em rascunho na consulta. Os documentos do pacote são histórico e evidência, não autorização para ampliar o escopo desta etapa.

Evidência: [RESULTADOS.json](hal-20/ritual-effects/RESULTADOS.json), [inventário dos 194 arquivos recebidos](hal-20/ritual-effects/INSUMOS_SHA256.json), [proveniência do build](../mods/ritual-card-effects/SDK-PROVENANCE.json), [runner reproduzível](../mods/ritual-card-effects/tests/run_preview2.py) e [orientação para o candidato](../mods/ritual-card-effects/README-PREVIEW2.md).

## Pacote e investigação dos hashes divergentes

O novo upload `mods att.zip` tem o mesmo SHA-256 da HAL-20 anterior: `745f6fbbd5f3c9451ca37735626abbbc4c6704bc5326ab3691be33d86b66281b`. Os 194 arquivos de `zzz Efeitos Rituais` são idênticos aos anteriormente inventariados. O manifest declara 0.78.74-effect-ai, autor Otavio, API mínima 4, slus_01411, restart true, 48 settings, 65 textos, cinco substituições de SFX e ordenação after de Melhoria, AI Hard Mode e Time Wizard; sem requires. Nenhuma configuração/default foi modificada. A descrição do manifest e documentos antigos ainda citam 0.78.73: essa inconsistência textual recebida foi preservada e não é uma mudança desta migração.

| Objeto recebido | SHA declarado no JSON antigo | SHA efetivo recebido |
|---|---|---|
| ritual-card-effects.o | c501c5c5fe5406a2606651f8df9f5ba98f895242e10c50f116778c9ecc3361e6 | f30934e77d9705b0e1122f9eed23e015533d47e85d0b01f98c0b37912c264170 |
| ritual-card-effects.x86_64-windows.o | b07075974d99a815c1cd4ae33916ca972c9f92021196f654aff0a4fa2365cc56 | 3dfcd399fb5c3a48aa8c3839b5cfebfedb79b8e9717c5a7b27056bda94d39517 |

A divergência foi confirmada novamente. O SDK-PROVENANCE recebido descreve preview.1, commit abreviado 10ee7e5ecf, data 2026-11-05 e nota 0.78.73. O DWARF do ELF32 identifica GCC 13.3.0 e paths de um diretório work44/release; o ELF64 identifica Clang 19.1.7 do zig-bootstrap. Não há checksums de fonte utilizáveis nesses metadados para amarrar os objetos aos C atuais. O JSON antigo também não contém hashes dos fontes/toolchain completos. Portanto **não foi estabelecida equivalência fonte/objeto**, nem identificada com certeza a revisão que gerou os hashes declarados. JSON desatualizado é hipótese plausível, não causa comprovada de todas as diferenças.

Solução desta etapa: nova compilação dos fontes identificados por SHA-256, com SDK/toolchain oficiais identificados, e proveniência nova. Os dois objetos antigos não são usados como componentes do build nem publicados como alternativas instaláveis. O JSON antigo foi preservado literalmente em SDK-PROVENANCE.preview1.recebido.json; os objetos recebidos continuam identificados por hash no relatório, sem substituir a evidência original.

O pacote atribui a moeda/frames à coin-flip-lib 1.0 de PeterIron. O usuário confirmou nesta tarefa possuir permissão de redistribuição da moeda disponibilizada por PeterIron. O crédito foi mantido e a base dessa confirmação consta em [NOTICES-HAL20.md](../mods/ritual-card-effects/NOTICES-HAL20.md). Não foi inventada uma licença aberta nem alegada verificação independente junto ao autor. Os demais assets são os fornecidos no pacote autorizado; nenhum recurso adicional foi extraído do jogo.

## Build e escopo dos arquivos

Compilado por SDK `tools/build_mod.py --target i386`, Clang 23.1.2 / llvm-mingw 20260922, headers e exports da árvore oficial preview.2. O builder usou os dois C da raiz, `mod.c` e `png.c`, sem flags próprias. SDK API 24 confirmado; o MOD continua declarando `mod->api=4` conforme seu fonte aprovado. A versão do host/SDK não exige mudar o mínimo do manifest apenas por conveniência.

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py /caminho/ritual-card-effects --target i386
```

Resultado: ELF32 relocável EM_386, **715.520 bytes**, SHA-256 **`c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1`**, hash do loader **52ce4829**. Esse ELF é o formato carregado pelo jogo Windows i386, não uma DLL/PE. Não foi gerado build x64/ARM. Há somente o objeto i386 novo nesta entrega; não misturar com o x64 recebido.

O objeto recebido i386 tem 964.464 bytes, .text 160.788 e .bss 167.588; o novo tem .text 179.088 e .bss 165.101. Ambos exportam MemoriesModInit e Png_Decode. Imports: 185 no recebido e 181 no novo. Saem Duel_SelectTrapPlay e cinco thunks de registradores usados pelo GCC; entram memcpy/memset, permanecendo o thunk eax usado pelo Clang. A opção diagnóstica antiga de priorização de traps continua desativada no fonte; eliminação de código morto e escolha de compilador podem explicar parte das diferenças, sem provar identidade do fonte antigo. Não alegar ganho de memória/performance pela redução do tamanho total: debug, seções e código foram gerados por toolchains distintos.

Os 181 imports novos existem nos exports **dos dois SDKs i386 Linux e Windows**. Nenhuma declaração de engine, chamada, layout ou hook precisou de patch para compilar. A fixture de loader resolve a engine simuladamente; a verificação contra os exports reais foi independente desse resolver. Proveniência inclui hashes do compilador, builder, exports, digest dos headers e todos os 31 fontes/headers mais manifest.

Arquivos de produção, todos os 56 PNGs, cinco WAVs, 65 textos, dois coin.txt, settings/defaults e documentos históricos permanecem iguais ao ZIP. Foram alterados somente três testes antigos, o objeto i386 e sua proveniência. Foram acrescentados loader/runner/teste de assets, README de portabilidade, aviso de autoria/permissão e cópia histórica da proveniência. O JSON de análise lista os 188 arquivos recebidos preservados byte a byte e as cinco substituições; o único arquivo recebido omitido é o objeto x64 antigo. Os testes ficam em subpasta e não entram no build de produção.

## Testes executados

Comando reproduzível no checkout desta branch:

```sh
MEMORIES_I386_CC=/caminho/gcc-com-sysroot \
MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot' \
MEMORIES_TEST_RESULTS=/tmp/ritual-results.json \
  python mods/ritual-card-effects/tests/run_preview2.py
```

No ambiente local, o kernel rejeita execução ELF32 direta. A fixture i386 foi executada sob QEMU 5.2 com sysroot Debian 11, como na base oficial. Os demais testes são fixtures nativas, source-backed ou verificações estáticas; não são emulação completa do jogo. GCC nativo foi usado nos testes; o objeto publicado foi produzido pelo Clang oficial indicado acima. Requisitos do runner: Python, Pillow, GCC nativo e compilação i386, com runner opcional. O JSON registra comandos e saídas reais.

| Grupo | Resultado e cobertura |
|---|---|
| Suíte final | **23 grupos PASS, zero FAIL, três SKIP**; sete compilações de fixtures registradas separadamente |
| Loader do ELF recompilado | **7.415 assertions**, dez ciclos completos com providers/OFF/ON/reset/shutdown; init adicional sem serviço provide; init sem assets rejeitado |
| Registros e recursos | 50 hooks e duas subscriptions observados; quatro providers; shutdown remove os 50 e as duas subscriptions na fixture |
| Contratos reais do ELF | APIs recipe/intelligence/forecast/position exercitadas através dos ponteiros publicados; ativo, OFF, settings OFF, entradas inválidas e reativação |
| Efeitos reais do ELF | Hook público CalcCardStats: Fortress Whale +500 por outro Fish da mão e +1000 por outro Fish do campo, ambos os lados, mão lógica consumida ignorada, ATK preservado. Position API: Chaos vivo recusa DEF, mesma carta com outra identidade não herda marca, reset limpa |
| Fontes/fixtures existentes | 18 receitas e três especiais, Skull, Chakra, toggles, Hungry humano/CPU, Mirror/LP/DEF/reflexo, Puppet/controle/rebuild/identidade, Cosmo/Female, Whale/Crab, Tri, Zera/Ultimate, fila de terreno, retenção em batalha, Mask Equip e burn obrigatório Yamadron |
| Integração opcional Egotist | Ativação/revogação source-backed das 24 famílias, preservação de Ritual verdadeiro e gates de configuração/face-down; Fusion real não carregado |
| Textos e assets | 56 PNGs com CRC/estrutura e decode Pillow; 52 frames runtime de moeda pelo Png_Decode de produção sob ASan/UBSan; cinco WAVs estruturalmente íntegros; 65 referências de texto presentes/UTF-8/chaves balanceadas; nenhuma edição |

Seis fixtures C independentes foram executadas com ASan/UBSan: provider de receitas, intelligence, títulos por instância/visual, política Yamadron e Mask Equip. O teste Egotist já usa sanitizadores, assim como o novo teste de PNGs. LeakSanitizer foi desabilitado explicitamente. O loader i386 e os demais testes não devem ser descritos como inteiramente cobertos por sanitizadores.

Essas fixtures têm escopos diferentes. Alguns testes extraem funções reais para um motor mínimo; outros conferem wiring/texto por strings, usam modelos ou tipos simplificados. O loader executa o objeto novo e callbacks/efeitos selecionados com dados nativos falsos, sem o gerenciador completo e sem aplicar patches em funções reais do jogo. Não comprova execução integral dos 24 efeitos, ordem real de todos os 50 hooks, duelos/IA completos, animações, perda normal, áudio ou renderização.

### Falhas encontradas e tratamento

Rodada original dos testes recebidos: 24 grupos, 18 aprovados e seis falhas. Nenhuma falha era uma incompatibilidade de compilação do MOD com API 24.

- test_hungry_cpu_battle extraía também um bloco Garma acrescentado depois de sua fronteira histórica; faltavam seis declarações na fixture. A extração foi limitada à região Hungry, mantendo os cenários e asserts de gameplay existentes.
- test_tea_cosmo_whale exigia o nome antigo fish_in_hand_and_field e uma fórmula removida do fonte. O wiring passou a exigir whale_fish_defense_bonus.
- test_whale_crab_v07855 exigia helper, multiplicador único e descrição antigos. Foi adaptado para extrair os dois helpers atuais, verificar os valores já presentes (+500 mão / +1000 campo), o texto com códigos de cor/ícones e a mesma proteção Crab. Um stub sem uso foi removido. Nenhuma condição do C ou texto de produção foi alterada para fazer o teste passar.
- test_logic_performance, test_recipe_performance e test_render_performance dependem de uma pasta baseline histórica de 0.78.39 ausente. Continuam disponíveis e inalterados; o runner os marca SKIP até MEMORIES_RITUAL_BASELINE ser fornecido. Não substituir a baseline pela própria versão nem alegar otimização.

Durante a criação da nova fixture, um resolver que reutilizava o mesmo endereço para várias funções e uma tentativa de invocar helper estático pela ABI pública produziram asserts incorretos. A fixture foi corrigida para usar identidades distintas e chamar o hook público CalcCardStats; nenhum patch de produção decorreu dessas falhas de instrumentação. GCC também emite avisos dos asserts de offset dos headers oficiais ao compilar a fixture, sem impedir o build; o build Clang do MOD concluiu sem erros.

A validação inicial tentou passar todos os PNGs pelo decoder runtime. Hungry_plus500.png é uma referência de 1983×793, maior que MAX_SIDE=1024 desse decoder; não é carregada por ele em produção, que usa o sprite já embutido em header para esse efeito. O teste foi separado corretamente: todas as imagens por Pillow/CRC e as 52 moedas pelo decoder realmente usado em runtime. Não redimensionar a arte nem ampliar o decoder por esse motivo. Nenhum erro de integridade foi encontrado.

## Contratos opcionais

| Contrato | ABI e comportamento verificado |
|---|---|
| recipe_v1 | ABI 1, get retorna três requisitos/result ou -1 indisponível; matches rejeita papel/ID inválido; receita Yamadron e toggles exercitados no ELF; fixture de produtor cobre 18 receitas e especiais. OFF deixa o ponteiro potencialmente acessível, mas retorna indisponível |
| effect_intelligence_v1 | ABI 1, enabled depende de provider ativo e setting do efeito; estimate puro não executa Ritual nem altera o board. Testados Yamadron e Skull, materiais selecionados/exclusão de ocultos, argumentos nulos e OFF/settings OFF |
| effect_forecast_v2 | ABI 2, on respeita efeito/ativação; read indica possibilidade/estado sem armar uma carta por consulta. Conexão do caminho nativo testada com Fusion ausente, provider desativado e ABI incorreta; provider ativo válido faz o owner Ritual ceder o planejamento conforme código existente. IA estratégica real não executada |
| position-rules-v1 | ABI 1, can_defend conserva permissões normais quando OFF/slot não aplicável; só nega DEF para a instância Chaos ativa correspondente |

O init também aceita host sem provide, confirmando que ausência desse serviço opcional não impede carregamento na fixture. Não foi adicionada dependência obrigatória de Melhoria, Ritual Duelists ou Fusion; after permanece apenas ordenação. Consumidores devem validar ABI/disponibilidade e respeitar on/enabled/get, pois o host pode manter exports de módulos desativados. Os testes não modificam ou substituem esses consumidores.

## Reset e save/load simulado

**Não há register_state de produção para os marcadores nativos deste MOD.** A fixture confirmou zero registros e a limpeza efetiva das marcas pelo callback reset após uma restauração sintética. O fonte já documenta, antes de overlay, que é necessário um novo Ritual após load. Esta etapa conserva essa regra; não implementa persistência de ativações, cooldowns, efeitos únicos, controle Puppet ou filas de efeitos. Rodar um round-trip de bytes falsos e depois reset não equivale a restaurar efeitos em jogo.

Além disso, o hash de código mudou. A base inclui Mods_CodeHash na assinatura do conjunto (src/pc/mods/manager.c, Mods_Signature), e StateCompatibleMods exige assinatura correspondente. Não presumir que savestates do objeto/SDK anterior possam ser carregados, nem remover essa proteção. Saves de progresso/memory card e snapshots de jogo completos não foram exercitados; nenhum save pessoal foi aberto ou modificado.

## Conflitos de hooks e integração futura — análise estática

Nenhum dos parceiros foi modificado, importado para esta branch ou carregado junto na fixture. PR #9 e o Dual do PR #10 permanecem isolados.

| Contato | Evidência e risco | Próximo teste |
|---|---|---|
| Melhoria / receita e instâncias | Ritual mask_effect.h:263 em diante e ritual_recipe_compat.h reconhecem trios oracle físicos; Melhoria PR #9 envolve Duel_CheckRitual, seleção e consumo campo/mão. recipe_v1 não transfere posse dos materiais ao provider | Seleção de três instâncias distintas com 0/1/2/3 origens de mão, campo, duplicatas, receita OFF/provider ausente; contagem/consumo único e efeito armado somente após Ritual real |
| Melhoria / hand e deactivation | Ritual hand_actions:2558, deactivate_card e populate compartilham mão, desativação e deck; Melhoria também envolve esses pontos | Ambas as ordens plausíveis, material sintético, resultado preservado, reset e Surrender com filas pendentes |
| Surrender / input | PR #9 usa Select e UpdateWithSideInput; Ritual performance_chaos.h:coin_update_pads captura/zera pads 1/2 durante moeda/dodge, Main_RunDuel pode ficar suspenso | Sim/Não/cancelar antes/durante/depois da moeda, retorno e derrota normal, sem roubar entrada do outro humano |
| Dual / mão, campo e quotas | Ritual hand_actions pode pausar por Chaos ou mascarar confirmação por Cosmo; Dual também observa seleção/consumo e mantém snapshots | Monster/Support em ambas as ordens, Queen Lock, tributos tardios, mesma ID com outra instância, aliases só transitórios, mão extra e cancelamento |
| Dual / draw e turnos | Ritual draw_phase:6907 posterga original enquanto chaos_turn.active; Dual limpa quotas no draw e no init | Vários Chaos, abertura/turno seguinte, nenhum reset antecipado ou quota herdada; jogar sem compra extra |
| AI HARD MODE | AiScript_Run, FindWeakest e materialização são compartilhados; Ritual after AI preservado. serpent_ai_run envolve o anterior e aplica regras existentes de efeitos, incluindo burn obrigatório. cosmo_hand_swap chama o swap anterior após sua seleção | Matriz AI/Dual OFF/ON, mão lógica/visível, preserve_hand, materialização fora da tela, Cosmo e burn sem duplicar ações. Não alterar prioridade, dificuldade ou decisões nesta portabilidade |
| Time Wizard / assinatura Dual | Ritual também envolve draw. Com ordem Dual -> Time -> Ritual, Ritual fica por fora e não se interpõe ao ponteiro original_draw do Time. Ordem Dual -> Ritual -> Time pode impedir a assinatura do PR #10 | Registrar cadeia real após todas as ordenações, ON/OFF e load; validar X/Quadrado/Support. Nenhum patch no Time ou no Dual foi aplicado |

AI HARD MODE considerado é o oficial da base preview.2; o adapter 0.1.1 ausente do inventário continua uma lacuna distinta, não resolvida aqui. Não concluir compatibilidade integrada por existência de imports ou pela fixture de forecast. Preservar a matriz obrigatória AI OFF/Dual OFF, AI ON/Dual OFF, AI OFF/Dual ON, ambos ON, com pares Melhoria/Ritual e depois coleção. Repetir modo nativo de dois humanos: Dual Monster+Support não é o mesmo que dois controles independentes.

## Limitações e sequência segura

Não há ROM/imagem legítima do jogo neste ambiente, Wine ou execução Windows real. Não foram exercitados duel completo, render HD/3D, interação visual de câmera/moeda, som, FPS, pior caso do planner, restauração de save completo ou a coleção integrada. A checagem de textos assegura integridade e referências, não encaixe visual de cada descrição. Os cinco WAVs são PCM 16-bit/44,1 kHz, mono ou estéreo conforme recebidos; não houve reprodução de áudio.

1. Revisar este PR em rascunho e reproduzir a suíte com os insumos/proveniência publicados. Nenhum merge automático.
2. Testar o mesmo hash candidato isolado no Windows i386, duelo novo, settings aprovados, Ritual verdadeiro, desligamento/reinício e reset/load. Verificar os 24 efeitos e as filas/instâncias que as fixtures não cobrem completamente.
3. Testar o par com Melhoria/Surrender do PR #9, depois AI HARD MODE/Dual nas quatro combinações, com ordem de hooks registrada. Não alterar os PRs parceiros para contornar falha ainda não reproduzida.
4. Qualquer problema que exija nova regra de jogo, persistência de efeito após load, nova estratégia ou alteração de receita deve ter reprodução, proposta e autorização próprias. Não houve implementação desse tipo nesta etapa.

Pontos de rastreabilidade: [HAL-20](https://linear.app/half-programer/issue/HAL-20/base-sincronizar-fork-e-versoes-atuais-dos-mods-com-upstream-v031), HAL-7/HAL-9/HAL-10/HAL-18 para pares e regressão. Os resultados são locais, não prova de checks aprovados no GitHub Actions. Não foram alterados workflows, permissões ou status de issues. A nova proveniência resolve a rastreabilidade **deste artefato**, mantendo explícita a origem não comprovada dos hashes antigos.
