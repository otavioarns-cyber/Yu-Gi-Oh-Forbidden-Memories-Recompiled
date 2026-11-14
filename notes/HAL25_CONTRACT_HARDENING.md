# HAL-25 — revisão de segurança, ABI e recuperação

**Entrega experimental para revisão. Sem merge, release ou promoção do ABI1.** Branch `test/hal-25-contract-hardening`, baseada em `integration/hal-20-v0.3.1-preview.2` (`d649647c159c91d615c07d6960928a37e270b461`). [Linear HAL-25](https://linear.app/half-programer/issue/HAL-25). Links de publicação serão registrados no fim deste documento.

## Resultado e prontidão

O contrato pode ser fortalecido **localmente em R/RD**, sem mudar core, estratégia, Queen Lock ou MODs de terceiros, para os fluxos cooperativos e pontos de interposição exercitados. O experimento limita vetos consecutivos, invalida callbacks interrompidos, bloqueia reutilização de tokens, exige recuperação explícita e impede consumo obsoleto após as escritas tardias testadas. Não está pronto para integrar automaticamente toda a coleção: a vida útil de callbacks exige cooperação, a fronteira física termina no primeiro consumo e um materializador externo que consome antes da próxima fronteira própria continua fora da garantia.

Foram reexecutados os 256 processos/1.590 checks do PR #25. A entrega final reúne **412 processos distintos e 2.552 checks PASS**, classificados como **390 PASS, 12 REFUSED e 10 NOT_REACHED**. REFUSED/NOT_REACHED preservam checks de segurança, mas não contam como ações de gameplay concluídas. As 57 regressões funcionais históricas passaram; a assertion histórica de pacote imutável RD continua FAIL esperado e seis testes históricos continuam SKIP. Nenhuma falha foi removida para liberar a entrega.

Não houve validação de gameplay Windows. Todos os testes manuais Windows permanecem **BLOCKED até a última fase do projeto**, conforme instrução do usuário. Não se solicitou execução intermediária.

## Insumos e preservação

Referência principal: [PR #25](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/25), commit `f6cea112db8d14be34ab2f28694b0121a94842c2`, [relatório anterior](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/test/hal-25-selection-contract-real-poc/notes/HAL25_SELECTION_CONTRACT_REAL_POC.md). Os pacotes próprios R/RD foram incorporados à nova árvore a partir desse commit, sem merge nem alteração do PR. Os insumos originais dos PRs #11/#13 permanecem como proveniência histórica, não como fonte alternativa para substituir a PoC.

| Componente de teste | Referência | SHA-256 do objeto |
|---|---|---|
| R do PR #25 | baseline preservado | `a34c3e0999beb5a2d8c0ab8e09a332bad3a3d97b8da39c8b4a2b24776823f4c1` |
| RD do PR #25 | baseline preservado | `b988755c2a4041988d53b21d2737eac68b4081728ad32fa6a05683d226469c01` |
| Melhorias/Surrender | PR #9, somente referência local | `c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0` |
| Fusion F1 corrigido | PR #22, commit `e089a10b6fccbfebf756438407b87657062914d9` | `384e11eeeae32412e23afbbbd9ebc308761674b8a2181238fd60e17e315b865c` |
| AI HARD MODE | base oficial, sem patch | `9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc` |
| Dual Play | PR #10, privado/local | `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378` |

Dual não é distribuído: nem fonte, manifest ou objeto. Time Wizard não foi incorporado nem modificado. O guard #20 só foi compilado em probe isolado de referência; não há mudança do organizador de hooks. [INPUTS](hal-25-hardening/INPUTS.json) fixa fontes/objetos/SDK/assets realmente utilizados; [árvore PR #25](hal-25-hardening/PR25_PACKAGE_TREE.json) permite auditar também os arquivos herdados e [AUDIT_FINAL](hal-25-hardening/AUDIT_FINAL.json) comprova os deltas permitidos.

## Problemas reproduzidos nas fronteiras anteriores

Os objetos preservados do PR #25 foram executados em 48 processos adversariais: 30 FAIL/18 PASS, 120 checks PASS/30 FAIL. Mais oito processos de permuta física coerente entre cópias de mesmo ID produziram oito FAIL (16 checks PASS). Esses **38 FAIL de diagnóstico são preservados**, separados dos resultados do novo objeto.

As falhas incluem repetição não limitada de transações vetadas, exaustão artificial de tokens, mutação da vtable conectada, reset durante avaliação e alteração de seleção/identidade depois da validação. O caso de duas cópias coerentes mostrou que comparar apenas os bytes de seleção não basta após o handoff. Uma primeira iteração do hardening também falhou nesse caso; foi rejeitada antes da recompilação final.

Esses testes inserem falhas em pontos nomeados: counters em UINT32_MAX, callbacks adversariais, reset reentrante e wrappers próprios. Não demonstram que tais condições acontecem naturalmente na UI. Os cenários normais continuam usando planner/controller/materializadores reais disponíveis, sem pending, planos ou quotas injetados. A pendência HAL-22/I2 não foi declarada resolvida por este trabalho.

## Alterações experimentais mínimas por responsabilidade

O [patch relativo ao PR #25](../experiments/hal25-hardening/hardening-vs-pr25.patch) contém somente seis arquivos de produção próprios:

- R: `mod.c`, `mandatory_hand_provider.h` e o novo `mandatory_hand_lifecycle_v1.h`.
- RD: `mod.c`, `mandatory_hand_consumer.h` e o novo header idêntico de lifecycle.

No produtor, revisão saturante e notificação de invalidação acompanham aplicação/reset reais; o shutdown solicita desconexão cooperativa. No consumidor, a tabela é copiada/validada, chamadas são cercadas por tokens de geração/conexão/interrupção, e o wrapper recusa reentrância. O limite de três transações inválidas é política de recuperação de erro, **não mudança de estratégia ou dificuldade**; sua adoção ainda precisa de revisão. A recuperação exige checkpoint físico e confirmação atual. O monitor local acompanha seleção e identidades até o primeiro consumo, detectando alterações antes/depois da delegação ou no retorno do controller.

Não foi adicionada chamada materializadora depois de uma troca. A função estratégica `rd_ai_plan` permanece idêntica ao PR #25. `mandatory_hand_selection_v1.h`, `hand_selection_snapshot.h`, manifests, settings, receitas, decks, assets e testes canônicos dos pacotes permanecem byte a byte iguais. São 197 arquivos herdados de R e 152 de RD inalterados; as outras mudanças são objetos recompilados e documentação/proveniência. Hooks R/RD não foram ampliados nesta fase; RD conserva seus nove hooks da PoC anterior.

## ABI e lifecycle

A [auditoria detalhada e especificação de recuperação](hal-25-hardening/ABI_E_RECUPERACAO.md) define tamanhos, discovery, propriedade, desconexão, reentrância e futura evolução. O ABI1 de seleção permanece congelado. Um companion opcional separado fornece status de 72 bytes e operações de disconnect/invalidate/recover, sem aumentar as tabelas existentes.

O estado aplicado efetivo prevalece sobre preferência pendente: o teste usa `Mods_Apply`/`Mods_SetEnabled` reais e observa R e RD ativos enquanto o OFF salvo exige reinício. Ausência e primeira carga OFF não criam dependência obrigatória. O teste de disconnect ocupado mantém tabela/código vivos até ACK; não há alegação de proteção contra unload arbitrário de callback em execução.

## Testes finais e classificação

| Grupo | Processos | Checks | Resultado |
|---|---:|---:|---|
| Regressões PR #25 principais | 174 | 1.066 | PASS |
| Cancelamento e nova ação | 64 | 400 | PASS |
| Reset/novo duelo após quarentena | 16 | 112 | PASS |
| Outras restrições públicas de posição | 2 | 12 | PASS |
| Hardening adversarial, com probes finais de tamanho/manager | 72 | 274 | 50 PASS, 12 REFUSED, 10 NOT_REACHED |
| Composição M/F/R/RD × AI/Dual, ordens variadas | 72 | 604 | PASS nos ramos exercitados |
| Pares mistos ABI1 antigo/novo | 12 | 84 | PASS de comunicação normal |
| **Total final** | **412** | **2.552** | sem FAIL funcional nos checks finais |

O total final conta os probes de tamanho e manager em substituição às respectivas versões anteriores, não duas vezes. Os logs/summaries originais permanecem identificados. No probe inicial do manager, o buffer de erro não era inicializado pelo harness e o manager não o preenche em sucesso: isso gerou saída inválida para UTF-8 e interrompeu o runner, sem assertion funcional FAIL. O harness passou a inicializar o buffer e preserva bytes inválidos por escape no transporte de logs; somente esses dois casos foram reexecutados por essa causa. O probe final de tamanho acrescentou seis checks sem remover os anteriores.

As regressões canônicas próprias tiveram R=23 PASS/3 SKIP e RD=34 PASS/1 FAIL esperado/3 SKIP. A falha `test_preview2_package` é `StopIteration` na busca de arquivos novos no recibo original imutável; não foi suprimida. O loader histórico foi executado numa **cópia temporária**, reconhecendo somente a expectativa oito→nove hooks já justificada no PR #25. Os testes canônicos na árvore não foram editados. SKIPs de R são performance de lógica/receitas/render; RD registra Windows, Droplist externo e ecossistema/savestates/outros targets.

[Matriz integral](hal-25-hardening/MATRIZ.csv), [cancelamentos/retomadas/quarentena](hal-25-hardening/CANCELAMENTOS.csv), [resultados estruturados](hal-25-hardening/RESULTADOS.json) e [logs compactados e índices SHA](hal-25-hardening/logs) distinguem baseline FAIL, regressão, recusa segura e caminho não alcançado. [Bloqueios](hal-25-hardening/BLOQUEADOS.json) não são contabilizados como PASS.

## Outros wrappers e guard

[Inventário por callback](hal-25-hardening/WRAPPERS.md) registra hash do artefato/corpo/relocações, pointer binding, ordem, filhos efetivamente chamados e alterações inclusivas/exclusivas. Não qualifica por nome ou ID. A matriz inclui provider real R, consumidor real RD, Melhorias/Surrender, Fusion corrigido, AI oficial e Dual privado. Drop Missing Cards permanece ativo no ambiente de balanceamento. Os 72 perfis variam R/F acima/abaixo e AI em rank inicial/tardio; não incluem todos os possíveis ramos humanos e controladores externos.

O predicado original do guard #20 recusa o novo objeto RD quando Dual está ativo: duas REFUSED; sem Dual, duas NOT_APPLICABLE. Não se ampliou a whitelist, não se liberou a coleção e não se incorporou a PoC do guard nesta entrega. Compatibilidade com qualquer wrapper futuro depende de repetir sua qualificação por hash/comportamento e dos limites descritos na auditoria.

## Builds, assets e proveniência

Builds completos, em cópias frescas e removendo o objeto copiado antes de compilar: SDK Windows oficial API24, `--target i386`, Clang 23.1.2/llvm-mingw 20260922; duas unidades C de R e dezesseis de RD. [BUILD](hal-25-hardening/BUILD.json) fixa comandos, fontes e hashes da toolchain/SDK. Não houve edição de produção depois desses builds.

| Objeto final experimental | Bytes | Loader code | SHA-256 |
|---|---:|---|---|
| R | 736.908 | `1e478c68` | `3c47222002283db726dfb5a82bcfad57d39deff85c9fd454c0566b4a7927038a` |
| RD | 4.564.172 | `98d3ee01` | `92664e4a83bb4f0bcc0307e9cb8e980ae8b707f316631d1c976ac84c97ed2ca3` |

O ABI/imports/exports ELF é igual ao PR #25. As declarações históricas de API mínima continuam R=4 e RD=12, aceitas pelo host API24; não foram falsamente reetiquetadas como API24. Os objetos foram reconstruídos com o SDK24, não reutilizados dos pacotes antigos. Prefixos de hash desta documentação não substituem as entradas SHA completas dos recibos.

NOTICES e assets autorizados permanecem intactos: moeda de PeterIron com permissão expressa do usuário; arte do RitualStarter doada por amigo do autor, conforme registro anterior. Não se inventa licença aberta nem se amplia essa autorização a outros terceiros. Nenhuma ROM, imagem de jogo, save pessoal, SDK/toolchain ou pacote privado do Dual acompanha a publicação.

## Riscos e próximo passo mínimo

A recuperação local é estabilizável sem alterações globais para os cenários cooperativos qualificados. Ainda exigem decisão/revisão: limite de três transações inválidas, companion de lifecycle, protocolo de ACK para qualquer unload futuro, alcance do monitor físico e regras de apresentação/recuperação pela futura UI. O protótipo não substitui um contrato explícito com materializadores externos nem soluciona I2 por nome/hash.

O próximo passo mínimo é revisar o diff e esses contratos de fronteira e autorizar uma **correção candidata R/RD separada**, mantendo os headers ABI1 congelados e a política de recuperação explícita. Antes de integração completa, repetir por hash a matriz com os artefatos candidatos e as ordens realmente autorizadas; ampliar a observação dos consumos posteriores ao primeiro quando houver ritual com vários materiais; manter refusals/unqualified wrappers bloqueados. A fase Windows final deverá cobrir UI/controller, cancelamentos humanos, reinício real, render/áudio e save/load de processo. Não presumir compatibilidade de savestates antigos.

## Publicação

Branch/PR/commit e verificação de preservação das referências: a completar após push sem force. Nenhum merge autorizado.
