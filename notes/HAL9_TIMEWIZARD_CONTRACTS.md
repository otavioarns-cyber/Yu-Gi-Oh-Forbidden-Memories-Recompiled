# HAL-20 / HAL-9 — correções executáveis de detecção Time Wizard

Continuação do [PR30](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/30), commit `eca0453cee9f4ac91082b73de756cd6afb7609b3`, na branch independente `test/hal-20-time-wizard-contracts`. Destino: `integration/hal-20-v0.3.1-preview.2` (`d649647c159c91d615c07d6960928a37e270b461`). [HAL-9](https://linear.app/half-programer/issue/HAL-9). Entrega experimental; sem merge/release e sem promoção de políticas.

## Resultado

**Os dois detectores receberam correções que executam no objeto final**, com controles originais negativos, manager real e transições efetivas. Não alteramos apenas o `time_wizard_ai.c` não vinculado. As consultas deixam de confundir wrapper com HARD e encontram Dual qualificado além do predecessor direto.

**Time Wizard continua fora da coleção admitida.** A correção é estritamente vinculada ao núcleo recebido, API24/i386 e três imports experimentais. Não recuperamos os fontes integrais, nem qualificamos todos os caminhos de Time Magic, consumo híbrido/bônus, efeitos e renderer. O guard antigo continua recusando o objeto; nenhuma whitelist foi ampliada por nome.

A coleção M/R/RD/F/Field/H/D/DMC do PR30 foi preservada: [auditoria](hal-9/AUDIT.json),507 arquivos de MODs byte-idênticos, Dual privado intocado. A política B e o call site de recusa de savestate permanecem exatamente iguais. Queen Lock, estratégias, probabilidades, recipes/settings/assets e efeitos não receberam mudança funcional.

## Fontes recuperadas e código realmente vinculado

[Busca reproduzível](hal-9/SOURCE_RECOVERY.json), [35 heads consultados](hal-9/REFS_BEFORE.tsv), [38 arquivos de entrada](hal-9/TIMEWIZARD_INPUTS.json). A busca de conteúdo em todo o histórico alcançável desses heads por `TimeWizardOriginalMemoriesModInit|snapshot_targets` nos C/H retornou zero hits. As cópias privadas disponíveis também não fornecem os corpos C de gameplay/snapshot/transformação do núcleo. Isso cobre os históricos/insumos acessíveis; não prova que um backup externo inexista.

Disponíveis: AI, Dual-compat, description-fit, evolution e Encore. O builder original recompõe um objeto fixo0.23.1 mais **quatro** adapters; o C de AI continua separado e não substitui o AI embutido. DWARF do núcleo contém somente uma compilation unit de description-fit; não permite recuperar o fonte principal. Não reconstruímos efeitos por inferência.

| Artefato | SHA-256 |
|---|---|
|Recebido0.24.0-test|`828a5879a81ee33078c7cd2e60ba7c8e87557f889b035455211ed700ca8d6b07`|
|Core fixo0.23.1|`5b4252dafbc07532f859cda92e49e75d15e531a880f90cc7924371b7e37f9b4a`|
|Candidata privada245.016 bytes|`6470024d45bd099b7800bd5f84ccdcef1a895a8a2447ecef058af11ada97d5d7`|
|Dual privado recompilado, preservado|`77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`|

O entrypoint executado é `MemoriesModInit` próprio → Encore → evolution → description-fit → Dual-compat recompilado → `TimeWizardV019MemoriesModInit` embutido → core original/roulette. Os wrappers legados duplicados continuam como evidência, com nomes preservados, mas seus inicializadores legados não são o caminho de registro corrente. [Logs](hal-9/runtime), [fingerprints e ABI](hal-9/AUDIT.json) identificam cada callback registrado por símbolo/tamanho/code hash e o arquivo completo.

O host real registra **API24, ponteiro4 bytes**, enquanto o core mantém **`mod.api=4`**, seu prefixo negociado original. Não falsificamos a versão declarada nem alegamos uma recompilação integral API24. A candidata roda no loader API24 modificado desta branch; não é carregável no core oficial sem a extensão.

## Correção mínima executada do detector HARD

[Builder](../tests/hal9-timewizard-contracts/build.py), [probes i386](../experiments/hal9/ai_probe_i386.S), [entrypoint](../experiments/hal9/contracts_entry.c), [recibo](hal-9/BUILD_CANDIDATE.json).

O core tem seis referências relocadas ao `ai_original_run`. Conservamos a usada no registro e as duas usadas para delegar. Somente três leituras Booleanas embutidas viraram calls de mesmo comprimento, sem mover funções:

|Função embutida|Início .text|Comprimento|Destino|
|---|---:|---:|---|
|ai_run|0x10ffe|5|Hal9_ReadHardEAX|
|ai_field_actions, descoberta BANK|0x121d9|5|Hal9_ReadHardEAX|
|ai_field_actions, ativação BANK|0x122ff|6|Hal9_ReadHardECX +NOP|

São16 bytes substituídos; os três registros de relocação correspondentes mudam para PC32. As comparações com `AiScript_Run` permanecem; readiness garante que seu endereço não é0/1. Os probes produzem0/1, preservando os outros registradores, flags, pilha e estado FP. FXSR usa fxsave/fxrstor; alternativa i686 sem FXSR usa fnsave/frstor. Ambas passaram. Isso não amplia os requisitos de CPU do jogo Windows nem certifica universalmente todas as unidades SIMD.

A consulta usa o estado efetivamente ativo do owner H, SHA completo/code hash e o callback real `run` de3820 bytes na cadeia ativa. Preferência salva durante restart pendente não determina o resultado. Ausente/OFF retorna0; ativo não qualificado retornaUNKNOWN(-1). Inicialização recusa UNKNOWN. Probes não transformam UNKNOWN em autorização; em diagnósticos sem guard, UNKNOWN não habilita BANK, mas não é garantia global de segurança. O guard continua obrigatório e não admite TW.

[Comparação de91 funções do prefixo](hal-9/CORE_BOUNDARIES.json):89 permanecem iguais após mascarar operandos relocados; as únicas duas diferentes são `ai_run` e `ai_field_actions`, com os patches acima. O builder também verifica byte a byte o restante de .text no core remendado. Isso preserva as instruções de RNG, snapshot/transformação, animações e resolução; não substitui teste funcional completo desses subsistemas.

## Ponte Dual executada

[Diff do adapter Time Wizard](../experiments/hal9/dual-source.patch), [ponte somente leitura no manager](../src/pc/mods/hal9_runtime.h). Não há edição/cópia pública de código do Dual, novo provider do terceiro ou alteração de quotas.

O adapter recompilado consulta `Hal9_CoreDualDrawV1`, em vez de ler exclusivamente `compat_original_draw`. O core percorre a cadeia instalada de draw, valida SHA completo do owner, símbolo/tamanho do callback e predecessor realmente relinkado. Só aceita os callbacks conhecidos D/R/TW daquela cadeia; wrapper desconhecido, ligação quebrada, duplicação, arquivo divergente ou owner OFF deixam a consulta neutra. Mantém a assinatura de instruções original **do callback Dual qualificado** e as checagens do prefixo de estado; não tenta extrair estado de outro wrapper.

O cache agora é indexado pelo callback encontrado e consulta a qualificação a cada resolução; OFF/ligação inválida limpa o cache. Reconhece ativação tardia pelo `Mods_SetEnabled` real. As regras originais Monster/Support, retyping temporário e adaptação híbrida continuam no adapter recebido; isso não as qualifica automaticamente para todas as ordens/materializadores.

Três imports privados V1: `Hal9_CoreHardStateV1`, `Hal9_CoreDualDrawV1`, `Hal9_CoreOwnersReadyV1`. Binding exige flag opt-in, API24/i386, ID do chamador **e** SHA completo da candidata. O único delta em `mods.c` é esse resolver e mover o SHA do mesmo buffer para antes da relocação. Hooks_Relink, Mods_Order, controller, host ABI público, guard e policy B não mudam. Os três exports próprios acrescentados são os dois probes e o entry Encore renomeado; todos os exports anteriores permanecem. Dois imports de thunk ECX/EDX deixam de existir após recompilar adapters com o compilador do SDK; os103 imports oficiais restantes e os3 privados são contabilizados. Não são novos providers públicos intercambiáveis.

Dois diretórios de build produziram o mesmo arquivo completo graças a paths de debug normalizados. Não se deve atualizar o pin automaticamente para aceitar um compilador/objeto divergente. Inputs e artefatos anteriores ficam privados/preservados.

## Matriz executada

[Resultados consolidados](hal-9/RESULTADOS.json), [matriz principal](hal-9/runtime/MATRIZ.csv), [controles antigos](hal-9/original/MATRIZ.csv), [coleção](hal-9/collection/MATRIZ.csv), [CPU sem FXSR](hal-9/probe-no-fxsr/MATRIZ.csv). PASS se refere ao cenário descrito; não equivale a gameplay completo. Logs publicados compactam apenas repetições textuais exatas de trace após a terceira ocorrência, conservando ordem das ocorrências exibidas, linhas distintas de estado e todos os resultados. O footer conta cada repetição; [RAW_LOGS.json](hal-9/RAW_LOGS.json) identifica SHA/tamanho dos logs integrais preservados no workspace.

|Grupo|Resultado|
|---|---|
|Candidata,14 cenários ×H OFF/ON ×D OFF/ON ×duas prioridades R|112 processos;1.270 checks PASS;56 PASS,40 REFUSED,16 NOT_REACHED|
|Coleção preservada,Field OFF/ON,terrenos0/5,H/D e duas prioridades|32 processos;296 checks PASS|
|Probe/objeto real em CPU sem FXSR/SSE|8 processos;132 checks PASS|
|Total candidato/coleção, sem somar controles negativos|152 processos;1.698 checks PASS;96 PASS,40 REFUSED,16 NOT_REACHED|
|Objeto recebido, controle histórico|8 processos;12 checks PASS e8 FAIL esperados, distribuídos em6 perfis FAIL/2 PASS|
|Controllers recebidos sobre objeto final|open_wheel/card_use e direct-hand/fade PASS, com transporte visual/wheel modelado|
|Regressões de adapters recebidas|672 combinações de decisão,5.120 seleções físicas modeladas e18 asserts de evolução PASS|
|Testes oficiais Linux, assertions intactas|lifecycle, overlay4000 passes, hooks e mod save-state compatibility PASS|
|Política B real e load literal|1.048.576 perfis e65 casos de prewrite PASS|
|Build completo PE32 Windows/i386|PASS; execução Windows AUTOMATION_UNAVAILABLE|

Os controles originais executam o callback embutido real: com H OFF e predecessor RD, ele descobre BANK indevidamente; após ativar D tardiamente pelo manager, seu resolver continua sem reconhecer D. As oito falhas são conservadas como evidência, não apagadas pelo total da candidata.

Nos casos0–10 testamos os três sites executados no core, estado aplicado, cancelamento de restart/preferência, primeira ativação efetiva instrumentada, OFF/ON, cache, owner ausente/frio, hashes divergentes, flag OFF, caller errado, wrapper draw desconhecido e predecessor corrompido. O ponteiro inválido é **somente inspecionado**, nunca executado. Mudança efetiva forçada via test bridge é distinguida de caminho normal de UI. Não usamos fixtures para afirmar que toda transição é acessível ao jogador.

O caso11 usa Queen real, o consumidor RD real e o controller nativo disponível: oito perfis escolhem a Female395, consomem uma única instância, preservam os outros materiais/deck e encerram pending. Com D ON há passagem pelo gate e ledger Monster=1; com H ON o callback H também executa. Número de callbacks não é número de materializações. A contagem física/mão e identidades são os critérios.

Casos12/13 tentam Time Magic sem impor plano/pending/seleção. Com RD ON o planner prefere a ação ritual680; com RD frio OFF, alguns perfis fazem TW como Monster e outros escolhem112. Os quatro perfis TW-Monster preservam consumo/quota1, mas a ativação Support/Time Magic **não foi alcançada**:16 NOT_REACHED permanecem na matriz. Não trocamos defaults/estratégia para forçar êxito. Bônus híbrido, mudanças de categoria durante confirmação e materialização Time Magic junto da coleção ainda exigem qualificação.

Há cenários de fronteira nos quais o controller modelado não completa uma ação; são avisados em `coverage_not_reached`. Neles PASS significa que o detector realmente executou, não que uma materialização passou. O caso11 separado é a evidência de consumo completo disponível. A fixture adiciona wrappers de trace; `Hooks_Inspect` desembrulha apenas esses wrappers próprios para mostrar os callbacks reais. Essa instrumentação não é um backend Windows.

## Encore e efeitos preservados

O caso9 executa o Encore **do objeto final** com provider Fusion real; somente o transporte do wheel é modelado. Tentativa iniciada ON mantém `eligible` em cache após OFF efetivo e agenda o retry existente. Nova tentativa OFF observa o provider residente neutro, não ganha retry e entrega uma única conclusão. Cancelamento limpa a tentativa. A política B recusa load mesmo durante pending, sem limpá-lo para passar o teste.

Não escolhemos outra regra de cache. F1/Revogar permanece no Fusion; mudar uma tentativa já iniciada seria uma decisão funcional separada. Source Encore, presets, RNG/probabilidades33,33%/50%, interferência, roleta, imagens/sons e evolution estão byte-idênticos no pacote privado. Leases/efeitos dos demais MODs não receberam alteração.

Os testes nativos disponíveis cobrem retry, stale IDs, cancel/reset, distribuição de RNG, fade/consumo direto uma vez e preservação do mapping/cartas de backrow. Evoluções Elf→Ancient Elf/Cocoon→PGM e fallbacks têm18 asserts de adapter. Seleção UI por X/Quadrado, Monster Zone/M/T Zone, Guardian/destruição/transformação completas no jogo, timings/animações e controles de dois humanos continuam NOT_REACHED como integração completa, embora seus corpos originais não tenham sido alterados. Não deduzimos aprovação visual a partir de hashes ou testes auxiliares.

## Savestates, Windows e limitações reais

Policy B do PR30 fica integralmente preservada: recusa antes de ler/restaurar memória, com guard ON/OFF e bibliotecas residentes OFF, inclusive Time Wizard. Não ligamos A, não serializamos ponteiros/BSS, não restauramos cartas por ID e não certificamos estados antigos. Memory-card não recebe mudança.

[Build Windows/limitações](hal-9/WINDOWS.json), [log](hal-9/BUILD_WINDOWS.log). O jogo completo compilou628 unidades/773 símbolos de dados. A sessão é Linux; nenhum backend PE32/VM/runner Windows legítimo está disponível aqui, nem imagem legítima acessível. A suspensão de Actions foi observada autenticadamente no PR30; consulta atual continua sem execução nova (último run listado06/10). O workflow PE32 independente anterior está preservado, sem aumentar permissões. Não repetimos Wine/QEMU PE já falhos nem inventamos uma VM/licença. Automação Windows permanece autorizada; só a bateria pessoal do usuário é reservada ao final.

A tentativa oficial Linux inicial precisou de `-lpthread` no sysroot Debian11, cuja libc ainda separa pthread; o wrapper de execução registra esse ajuste, sem enfraquecer asserts. Um comando inicial combinando `--field-bonus --fence` não é suportado pelo harness herdado (o último objeto nativo era stats, não Main_RunDuel); as regressões Field foram executadas no modo suportado e o fence real foi exercitado separadamente em todos os perfis da candidata. Erros intermediários de instrumentação/build ficam no workspace; não são falhas funcionais dos MODs nem resultados omitidos da matriz final.

A qualificação assume objetos legítimos/preservados e alterações de lifecycle no thread do jogo. Não cria sandbox contra um MOD que reescreva memória de outro, nem protocolo de unload concorrente. Consulta UNKNOWN em ambiente diagnóstico com guard OFF não autoriza continuidade segura. Core binário legado, renderer/transporte real, materializadores interpostos e todas as categorias bônus ainda limitam a admissão.

## Publicação e próximo passo mínimo

Publicados: código próprio do bridge/probes/builder, patch mínimo do adapter Time Wizard, testes, hashes, metadados e logs próprios. O objeto Time Wizard recomposto e seu núcleo/sons ficam privados porque a redistribuição das partes incorporadas de terceiros não foi verificada; [auditoria](hal-9/AUDIT.json). Nenhum arquivo do Dual, imagem de jogo ou save é incluído. Os pacotes canônicos já autorizados continuam idênticos.

O próximo passo é qualificar os callbacks físicos restantes do **mesmo objeto**: Time Magic Support, normal/bônus Dual, ordem/interposição, consumo e efeitos reais, mantendo a recusa do guard até haver evidência. Os dois detectores não precisam ser novamente auditados do zero. Um runner Windows legítimo com imagem privada permitirá executar as sequências UI/duelo automaticamente; não exige gameplay manual intermediário.

Para eliminar definitivamente a dependência do binário fixo, o insumo específico é o fonte principal exato0.23.1 (gameplay/AI/snapshot_targets/transform_card/card_use e roulette), seus headers e receita/toolchain de link, com origem/termos das partes incorporadas. Apenas `time_wizard_ai.c` atual ou uma fonte de outra versão não basta. Sem isso, a opção demonstrada é manter esta reparação estritamente por hash/API, com limitações e sem alegar rebuild integral. Não há justificativa para promover a candidata à coleção completa ou fazer merge agora.
