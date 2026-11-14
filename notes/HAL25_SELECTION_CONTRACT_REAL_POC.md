# HAL-25 — contrato público real de seleção obrigatória (PoC)

**Integração experimental implementada nos dois MODs, recompilada para Windows i386/API24 e validada no host parcial. Sem ponte privada de Queen. Não é correção definitiva nem release.** Queen Lock passou nas duas ordens; cancelamentos posteriores à aprovação não recuperaram fallback nem consumiram o cursor antigo nos casos exercitados. A proteção precisou de uma fronteira local adicional no RD, pois simplesmente zerar a seleção não cancela o controller nativo.

Branch `test/hal-25-selection-contract-real-poc`, derivada diretamente de `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`; motor oficial `aceac3782902746f35ddb0148809450933327cb3`. PR independente em rascunho, sem merge/release. Master, integração e PRs anteriores não foram alterados. AI HARD MODE, Dual Play, Fusion/F1, Time Wizard e o core permanecem intactos; o guard #20 não foi incorporado.

## Resultado e evidências

| Suíte atual | Processos | Checks PASS | FAIL/incompletos |
| --- | ---: | ---: | ---: |
| Matriz principal, provider real | 174 | 1.066 | 0/0 |
| Retomada ou quarentena após veto | 64 | 400 | 0/0 |
| Reset/novo duelo após identidade corrompida | 16 | 112 | 0/0 |
| Restrição de posição/Black Chaos | 2 | 12 | 0/0 |
| Total | **256** | **1.590** | **0/0** |

Os **80 primeiros movimentos normais de controller**, incluindo os controles sem RD, chegaram efetivamente à fase7, com uma carta física consumida. Não se contou somente reset/limpeza como comprovação de Queen. Veto explícito/resposta inválida, ausência de jogada de bônus e quarentena são resultados distintos de uma ação executada.

- [Matriz principal](hal-25-contract/matrix/MATRIZ.csv) e [JSON](hal-25-contract/matrix/RESULTADOS.json).
- [Retomada/quarentena](hal-25-contract/recovery/MATRIZ.csv) e [reset de quarentena](hal-25-contract/reset-recovery/MATRIZ.csv).
- [Cancelamentos por cenário](hal-25-contract/CANCELAMENTOS.csv), [comparação normalizada com #24](hal-25-contract/COMPARATIVO.csv).
- [Regressões originais](hal-25-contract/regressions/SUMMARY.json), [auditoria de fontes](hal-25-contract/SOURCE_AUDIT.json), [auditoria final](hal-25-contract/AUDIT_FINAL.json).
- [Builds e hashes](hal-25-contract/BUILD.json), [ABI R](hal-25-contract/ABI_R.json), [ABI RD](hal-25-contract/ABI_RD.json), [insumos executados](hal-25-contract/INPUTS.json).
- Logs integrais compactados em [logs/](hal-25-contract/logs), com índice SHA-256 em cada suíte.

## Referências e escopo lido

AGENTS.md, [HAL-25 no Linear](https://linear.app/half-programer/issue/HAL-25), [PR #11](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/11), [#13](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/13), [#18](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/18), [#20](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/20), [#23](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/23) e [#24](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/24).

R parte do commit `2cf0de31e88e9f1c058b3747b2b2add3db0e347c`; RD de `9a363f28f6dbfe779b1b901ddecab74ab1be57e9`. Os hashes originais e as proveniências anteriores foram preservados em [ORIGINAL_INPUTS_PR24.json](hal-25-contract/ORIGINAL_INPUTS_PR24.json), [PR11](hal-25-contract/PR11_SDK_PROVENANCE_ORIGINAL.json) e [PR13](hal-25-contract/PR13_SDK_PROVENANCE_ORIGINAL.json). Não se promoveu A/B automaticamente nem se misturaram versões dos MODs.

A descrição histórica do Linear ainda pede diagnóstico sem patch; a solicitação mais recente do usuário autoriza explicitamente esta implementação experimental nos dois projetos. Essa autorização não abrange adoção definitiva.

## Contrato público proposto

[Especificação](hal-25-contract/CONTRATO.md), [header ABI1](../mods/ritual-card-effects/mandatory_hand_selection_v1.h). As cópias do header/adaptador nos dois MODs são byte a byte iguais.

O manifest original de R possui ID `ritual-card-effects-test`. O core vincula `Mods_Find` ao ID de quem chama `provide`; não existe alias global. Como o manifest deve permanecer intacto, o contrato usa um **rendezvous opcional em `ritual-duelists:mandatory-hand-selection-v1`**. R conecta sua tabela `RhsRules` real nesse ponto, e `rules()` permite consultá-la publicamente. O callback pertence a R e executa sua regra; RD não a copia. Não se usa a chave temporária no novo protocolo nem se altera o registro global para inventar um alias.

O wrapper AI de R conecta a tabela antes de chamar o fluxo anterior; isso ocorre antes do planejamento RD nas duas ordens. Init/applied também tentam a conexão quando disponível. Só se publica a tabela qualificada após o init de R completo. Sem RD não há rendezvous: R continua com seus hooks originais. Sem R, RD preserva o plano normal. Uma tabela incompatível não substitui a já qualificada; não há dependência `requires`, alteração de settings ou novo MOD intermediário.

| Parte | Dados/semântica |
| --- | --- |
| Request | ABI/tamanho, intenção `PROPOSE` ou `CANCEL_INTENT`, action/generation e snapshot por valor |
| Snapshot | Seleção lógica12 bytes, lado/fase/turnos/draw, deck80 registros, stats722, mão física5, campo10, janela lógica45 |
| Identidade física | Slot, record, índice do deck, ID, data pointer, objeto e flags; ponteiros são tokens efêmeros do processo |
| Response | Echo de action/generation, status, mandatory separado, seleção/destino, referências físicas selecionadas e revisão do owner |
| Status | UNAVAILABLE, UNCHANGED, REPLACED, CANCELLED; INVALID/retorno0/resposta malformada impedem commit |
| Revisão | Geração do owner, máscara de Queen efetivamente viva e flags relevantes de estado aplicado/configuração/cumprimento |
| Fronteira local | `precommit()` valida ação vigente; `outcome()` expõe aprovação, veto, retry ou quarentena |

Um `UNCHANGED` pode conter uma restrição obrigatória já satisfeita; `mandatory` distingue isso de ausência de restrição. `REPLACED` exige override obrigatório e referências físicas válidas. `UNAVAILABLE`/OFF não impõem Queen. Uma resposta rejeitada não restaura o fallback.

O snapshot é comparado exatamente, sem depender somente de um hash. A revisão detecta mudança de estado do owner; action/generation evitam respostas de outra ação. Reset invalida o estado efêmero; overflow não reutiliza tokens. Não se persiste essa transação no savestate.

**Importante:** bytes vazios antes de qualquer validação não são um veto autenticado. O próprio Queen original pode completar uma proposta vazia da VM. Um cancelamento explícito é representado por intenção/status, e uma mudança depois da aprovação invalida o snapshot. Não se inventou um evento de cancelamento da UI entre planner/swap.

## Implementação nos fontes reais

| Projeto | Mudanças de produção |
| --- | --- |
| R | `mod.c` + `mandatory_hand_provider.h`, `mandatory_hand_selection_v1.h`, `hand_selection_snapshot.h` |
| RD | `mod.c` + `mandatory_hand_consumer.h`, os mesmos dois headers compartilhados |

Diffs isolados: [R.patch](../experiments/hal25-contract/R.patch), [RD.patch](../experiments/hal25-contract/RD.patch). Não se alterou outro C/H já existente. READMEs e SDK-PROVENANCE foram atualizados para não anunciar falsamente fontes/objeto imutáveis.

Em R, o corpo original foi extraído para uma função de seleção por bytes. `cosmo_lock_active`, definição Female, preferência entre opções e destino continuam originais. O wrapper antigo usa a mesma função; o provider recebe/devolve buffers próprios. Não procura funções privadas por endereço/offset, não troca deck records nem consome/materializa. O consumidor verifica o snapshot também após o callback. Isso é um contrato de código nativo confiável, não um sandbox capaz de impedir qualquer escrita de um MOD malicioso.

Em RD, o planner original permanece intacto. O wrapper consulta o produtor depois do planejamento, antes da captura definitiva do controller. Um override retira somente o pending incompatível ainda não materializado e instala a escolha física aprovada. Antes do swap, confere novamente seleção, identidade, revisão e resposta; não permite retarget depois da captura da categoria. Um veto encerra o pending, sem recuperar escolha antiga e sem chamar outro materializador.

O materializador original permanece responsável por suas trocas. Não se acrescenta `previous_swap` depois de uma materialização concluída. O caso original HOLD, que prepara um material separado antes de encaminhar a ação ainda não executada, não foi reescrito.

## Descoberta adicional: zero não é cancelamento do controller

O controller real chama o swap de retorno void e avança do subestado0 para1. Mesmo com os bytes zerados, o subestado2 pode usar a carta que permanecia no cursor. A primeira iteração experimental reproduziu consumo de #535 nesse caminho. Portanto, só proteger `rd_materialize` e zerar a seleção não satisfazia o requisito de não consumir carta incorreta.

A PoC acrescenta **um hook local RD de `DuelScene_UpdateHandActions`**, limitado à própria transação de mão CPU. Depois de uma transição abortada 0→1:

- Se ator/fase e mão/deck físicos permaneceram intactos, devolve o subestado a0. O caminho nativo de snapshot/`AiScript_Init` prepara uma **nova ação** no próximo tick; nenhuma carta é restaurada por ID nem se reaproveita o fallback.
- Se a mão física mudou, não tenta repará-la. Mantém quarentena e impede avanço do controller até reset/novo duelo. Três ticks adicionais foram exercitados sem consumo; 16 cenários adicionais confirmaram liberação após reset/init e um novo tabuleiro/commit ritual modelado.
- Caminhos normais não são reordenados. Não há mudança em `Hooks_Relink`, `Mods_Order`, controller/core ou código Dual/AI.

O handler anterior de applied do RD continua chamado uma vez. O loader RD passa de8 para9 hooks; subscriptions continuam6 e o estado registrado continua288 bytes/v4. A fronteira usa estado específico desse controller; sua adoção depende de revisão cuidadosa e da qualificação final de gameplay. A quarentena ainda não possui UI própria amigável: a causa fica no log e no estado público do protótipo. Isso impede apresentá-la como solução final pronta para usuários.

## Cancelamentos, retomada e alcance

| Probe | Resultado atual |
| --- | --- |
| Zero depois da aprovação | Veto antes de consumo; nenhuma seleção antiga recuperada |
| Wrapper AI altera carta/destino/categoria | Snapshot rejeitado; nova decisão somente se estado físico intacto |
| Remoção de record/duas cópias com mesmo ID e pointer diferente | Quarentena; não troca uma identidade por outra |
| Owner OFF/reaplicado/reset/primeiro ON tardio | Revisão ou conexão invalida ação antiga; sem reaproveitar benefício |
| Stats alterados depois da validação | Veto e nova seleção sobre snapshot novo |
| Resposta ABI inválida, status desconhecido, action errado, lado errado, seleção inconsistente, referências forjadas, retorno0 | Ação impedida; nenhuma quota artificialmente resetada |
| CANCELLED declarado pelo contrato | Impede commit, sem fallback |
| Zero na VM antes de existir veto/transação aprovada | Queen original pode escolher Female; verificado também sem RD. Não é ressurreição de cancelamento |
| Replanejar após aborto intacto | Uma nova ação física, quotas corretas; Queen continua respeitada quando ainda ativa |
| Reset de quarentena | Novo duelo modelado, callbacks reais e commit novo; não se reparou silenciosamente a mão corrompida anterior |

O controller chama planner e swap sincronamente, sem UI/event pump entre eles nesse trecho. Os probes de alteração no meio da chamada são **instrumentação adversarial**, não prova de cancelamento pela UI ou mudança de MOD normalmente alcançável ali. Ambos os manifests exigem reinício. A ativação/desativação reentrante foi exercitada pelo callback real aplicado, não pela interface Windows.

Há uma garantia limitada às fronteiras próprias exercitadas. Um terceiro que materialize ou reescreva depois da última verificação sem aderir ao contrato continua fora dela. Não foi criado um lock global nem resolvida toda ordem possível de AI/RD/Dual. I1/I2/HAL-23 e outros wrappers precisam de suas próprias qualificações; o guard #20 não aceita automaticamente os novos hashes.

## Comparação com PR #24

O [comparativo](hal-25-contract/COMPARATIVO.csv) usa os mesmos três estados de Female legal × AI OFF/ON × Dual OFF/ON em cada ordem. Original/A/B anteriores são resultados históricos do commit `e2605687e9f71862c9fc67f8358baead5f2396cc`; não foram reexecutados, copiados ou promovidos nesta branch. A coluna real contém novas execuções.

| Variante | R interno ao RD | R externo ao RD |
| --- | --- | --- |
| Original | 12 FAIL | 12 FAIL |
| A | 12 FAIL | 12 PASS |
| B sem provider | 12 FAIL | 12 FAIL |
| B com ponte privada de teste (#24) | 12 PASS | 12 PASS |
| **Provider real atual** | **12 PASS** | **12 PASS** |

R recebe rank -20 ou40, AI0, RD10, Dual20; o observador próprio fica em100. Os logs registram ordem efetiva/callbacks, categoria entregue ao gate, carta/record/deck_index/data_block, trocas e ledger reais do Dual. Nenhum pending/plano/quota é injetado nos casos normais. `bridge0` e um contador fixo0 são apenas rótulos herdados; não existe `policy_bridge`/`install_test_bridge` nem chamada externa de helper Queen no harness novo.

## Regressões e invariantes

- Female única/múltiplas/duas cópias físicas, ausência de Female e campo cheio sem fusão legal: condições originais de Queen preservadas. Sem restrição, o RD continua preparando #680; R OFF/ausente não bloqueia esse planejamento.
- R sem RD: oito combinações normais e dois controles de proposta VM vazia. RD sem R: matriz de ausência, sem dependência obrigatória.
- Dual: categoria definitiva Monster/Support, ledger, bônus, rejeição sem consumo e quota neutra em cancelamentos. Nenhuma injeção de quota nem alteração de Dual. AI usa seus settings/defaults/decisões originais.
- Field e Both: seletor/consumidor real de Melhorias/RT, materiais corretos e cópia spare preservada. Hand-only continua rejeitado por `min_field=1`; não se relaxou o default. São controles separados sem Queen ativa, não desativação de Queen para permitir uma jogada proibida.
- Black Chaos: contrato existente de posição, commit real, identidade física, OFF/novo commit ON. Yamadron/Puppet e demais efeitos continuam nas regressões originais. O novo contrato de mão não cobre posição, campo ou alvo de ataque universalmente.
- Save/load: somente buffers registrados e callbacks reais SAVE/reset/LOAD; não inventa persistência do pending ou do novo snapshot. Sem certificação de savestates antigos.

Regressões originais no objeto atual: **R 23 grupos PASS/3 SKIP**; **RD 34 grupos PASS,1 FAIL histórico e3 SKIP**. O FAIL é a auditoria antiga que exige fonte/objeto e catálogo de insumos imutáveis do pacote recebido; isso não se aplica a uma implementação autorizada. O delta exato é verificado separadamente, sem enfraquecer esse audit.

Para qualificar a única hook nova, o runner aplica **somente na cópia temporária de testes** o [diff de expectativa8→9](../experiments/hal25-contract/loader-expectation.patch). Mantém todas as demais assertions, dez loads independentes, recusas esperadas e testes funcionais. O teste original canônico não foi reescrito para mascarar essa mudança: a adaptação é explícita em `--qualified-loader`. Loader R:7.427 assertions,50 hooks,4 providers existentes, API4. Loader RD:1.534 assertions,9 hooks,6 subscriptions,API12,state288/v4.

Os SKIP originais incluem baselines de performance ausentes, pacote Droplist externo e qualificação de ecossistema/outros alvos. Todos os testes manuais exclusivos de Windows estão registrados como **BLOCKED até a última fase**, por instrução expressa do usuário, em [BLOQUEADOS.json](hal-25-contract/BLOQUEADOS.json). Nenhum teste manual intermediário é solicitado.

## Builds, ABI e integridade

SDK oficial preview.2/API24, Clang23.1.2/llvm-mingw20260922, builder oficial `--target i386`; compilação integral dos2 C de R e16 C do RD. Não se usou objeto antigo como insumo de link. Formato ELF32 REL i386 esperado pelo loader MOD Windows, não DLL.

| Objeto atual | Bytes | SHA-256 | Hash loader |
| --- | ---: | --- | --- |
| R | 733.368 | `a34c3e0999beb5a2d8c0ab8e09a332bad3a3d97b8da39c8b4a2b24776823f4c1` | `d9ed6ece` |
| RD | 4.541.712 | `b988755c2a4041988d53b21d2737eac68b4081728ad32fa6a05683d226469c01` | `bc5cc1e9` |

Exports existentes idênticos: R2/RD44. R passa de181 para182 imports (`D_800EA030`, necessário à identidade física); RD de87 para89 (`D_8009B20C`, `DuelScene_UpdateHandActions`, necessários à fronteira local). Todos no SDK oficial; nenhuma remoção. Os contratos existentes de R permanecem4, sem renomear suas chaves. ABI1 nova é exposta pelo rendezvous, não por um novo export nativo global.

Manifest/ID/versão e todos os settings são byte a byte iguais: R48 e RD6. Assets/textos/configurações/receitas/decks e os outros C/H existentes foram preservados. No snapshot inteiro, R195 arquivos antigos permanecem iguais e RD150; cada pacote altera mod.c/objeto/README/proveniência e acrescenta3 headers. Custos de snapshot/frame e latência de cancelamento não têm benchmark de gameplay nesta etapa.

Dual local API24 é o SHA `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378` / code3028c68a do PR #10. Seu fonte, manifest e objeto não acompanham a branch. AI oficial e Melhorias PR #9 são dependências locais por hash; Drop Missing Cards1.0 fica ativo. SDK, toolchain, ROM/disco e saves pessoais não são publicados.

## Erros e correções de desenvolvimento, sem esconder controles negativos

Os [logs negativos de desenvolvimento](hal-25-contract/logs/development-negative.tar.gz) são de iterações anteriores, **não** do candidato atual. Mostram o cursor antigo consumido quando somente o swap é vetado e a retomada bloqueada por tratar proposta vazia como cancelamento. Houve também erros de preparação corrigidos: declaração de scene ausente no adaptador inicial e metadata antiga duplicada para o objeto RD. Nenhum foi tratado como bug da base/SDK oficial.

Uma matriz preliminar contou reset/limpeza como PASS mesmo sem primeira jogada. Ela foi excluída da qualificação; o runner final exige uma ação normal efetiva e a matriz foi reexecutada após retirar o bloqueio indevido do planner. Os resultados atuais acima são do código/objetos finais, com80/80 primeiras ações normais executadas. Os arquivos negativos não devem ser somados aos1.590 checks do candidato nem usados como alegação de compatibilidade.

## Respostas A–F e próxima decisão

**A.** Sim. A ponte privada foi substituída por callbacks reais nos fontes/objetos de R/RD, com descoberta pública, snapshots e respostas versionadas. Fixtures malformadas testam a ABI; não executam a regra de Queen por fora de R.

**B.** Sim, nas duas ordens e nas combinações/condições executadas. Predicados, preferência e condições originais de Queen foram preservados; sem whitelist genérica no RD.

**C.** Os cancelamentos tardios exercitados foram contidos e a retomada segura foi testada. Foi necessário tratar o avanço0→1 do controller em hook local. Cancelamento bruto antes de haver transação não é veto protocolado; alcance pela UI ainda não foi provado. Não há garantia contra alterações depois da última fronteira própria ou concorrência arbitrária.

**D.** Sim, nos controles executados. R mantém seu comportamento solo; RD opera com produtor ausente/OFF. Manifest e dependências continuam opcionais. Não se trata de um registro global novo nem de um MOD intermediário obrigatório.

**E.** Identidades, consumo único, quotas, efeitos e planejamento passaram no recorte testado; fontes/settings/assets não relacionados permanecem iguais. Isso não certifica todo ecossistema, outras ordens/lifecycle, savestates antigos ou gameplay Windows.

**F.** Ainda é preciso revisar e autorizar ABI/descoberta pública, fronteira local de abort/retry, quarentena e apresentação ao usuário; qualificar escritas posteriores de outros wrappers e domínios campo/posição/ataque; avaliar custo; e executar a fase final Windows já adiada pelo usuário. Não promover esta PoC automaticamente a candidata definitiva, integração ou release.

## Reprodução e publicação

[Guia de testes](../tests/hal25-contract/README.md), [patches](../experiments/hal25-contract/README.md), [hashes de publicação](hal-25-contract/PUBLICACAO_SHA256.json). Os pacotes só devem ser usados como **um par experimental de uma instalação por ID**, sem misturar objetos #11/#13 ou A/B #24 com estes fontes.

As permissões anteriores da moeda de PeterIron e da arte doada do RitualStarter continuam registradas nos [créditos R](../mods/ritual-card-effects/NOTICES-HAL20.md) e [RD](../mods/ritual-duelists/NOTICES-HAL20.md). Não se inventou licença aberta nem autorização para redistribuir Dual/Droplist. Os `.bin` presentes são buffers desses assets gráficos autorizados, não ROM. Não houve adição de recurso proprietário extraído do jogo.
