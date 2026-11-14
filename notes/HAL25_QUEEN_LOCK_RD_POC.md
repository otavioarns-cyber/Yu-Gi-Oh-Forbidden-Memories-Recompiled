# HAL-25 — Queen Lock × Ritual Duelists: alternativas experimentais

**Estado: prova de conceito parcial; não instalar nem promover os candidatos automaticamente.** A reprodução dos dois caminhos do PR #23 foi confirmada com o par mínimo. A alternativa A corrige somente a ordem externa. B funciona nas duas ordens com uma ponte de teste que executa a regra original, mas **o Ritual Effects do PR #11 não fornece o contrato proposto**. B sem essa ponte conserva a falha original. Não há correção completa, segura e independente de ordem demonstrada usando apenas os contratos públicos atuais.

[PR #24 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/24). [Branch publicada](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/test/hal-25-queen-lock-rd-poc): `test/hal-25-queen-lock-rd-poc`, derivada de `integration/hal-20-v0.3.1-preview.2` no commit `d649647c159c91d615c07d6960928a37e270b461`; motor oficial `aceac3782902746f35ddb0148809450933327cb3`. Nenhum merge, release ou alteração da integração/master. O guard do PR #20 não foi incorporado. Os diretórios canônicos dos dois MODs são snapshots **inalterados** dos PRs #11/#13; os patches e objetos alternativos ficam fora deles.

## Contexto e evidências consultadas

- [HAL-25 no Linear](https://linear.app/half-programer/issue/HAL-25). O pedido atual autoriza as alternativas experimentais; não autoriza adoção definitiva nem mudança de Queen Lock.
- [PR #11 — Ritual Effects](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/11), commit `2cf0de31e88e9f1c058b3747b2b2add3db0e347c`.
- [PR #13 — Ritual Duelists](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/13), commit `9a363f28f6dbfe779b1b901ddecab74ab1be57e9`.
- [PR #18 — pending/lifecycle](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/18): a troca de estado entre planner e materialização não foi demonstrada pela UI normal. HAL-25 reproduz conflito na mesma ação; não implementa a correção de lifecycle da HAL-23.
- [PR #20 — guard experimental](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/20): não usado como solução; seus fingerprints de objetos antigos não qualificam estes candidatos.
- [PR #23 — descoberta de Queen Lock](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/23), [relatório original](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/f407f3d427d70351ec9b711904c0414d38d6ccfb/notes/HAL20_INTEGRACAO_GUARD_F1.md).

## Causa e fronteiras de responsabilidade

O controller nativo constrói a mão lógica, chama `AiScript_Run`, captura a escolha e chama `func_8001BAF0`. O planner real do RD pode preparar `deck_pull` para buscar a Magic #680 do deck. Isso ainda é **planejamento**, não consumo de tributos. O pending guarda origem, substituição física, destino e o fallback #535 anterior.

Quando `rd_materialize` está acima de `cosmo_hand_swap`, o RD realiza a troca e retorna sem delegar. A regra de Queen Lock interna não é executada nesse callback: #680 é materializada, a Female #395 legal permanece na mão. Quando `cosmo_hand_swap` está acima, ele substitui a seleção por #395 e chama RD; a validação do pending falha, RD descarta esse pending e restaura seu fallback #535. A seleção obrigatória legítima é perdida antes da materialização final.

O efeito é responsável por decidir **se e como** uma restrição obrigatória se aplica. RD é responsável pelo plano/pending, pela identidade física da troca que possui e por não ressuscitar uma escolha obsoleta. Dual continua responsável por categoria/quota; o materializador não deve chamar `previous_swap` depois de já executar uma troca. O motor continua responsável pela seleção/consumo nativos. Nenhuma responsabilidade foi transferida ao core.

Em ambos os caminhos, Queen foi ativada por `DuelEffect_ApplyRitual` real, não por escrita em marks privados. A mão, as estatísticas e o tabuleiro são fixtures. O helper original `cosmo_lock_active` exige Female física e destino válido: sem Female ou com campo cheio e sem fusão legal na fixture, Queen não força a ação. Esses controles não desativam a regra para permitir um ritual.

## Contratos atuais e alternativas

`recipe_v1` descreve receitas; `effect_intelligence_v1` estima efeitos e não autoriza seleção; `effect_forecast_v2` do owner R não fornece `policy`/`pair` para essa finalidade; `position-rules-v1` trata `can_defend`, não seleção de mão. Nenhum desses contratos comunica um override obrigatório de Queen Lock ao RD. Consulta a estimativa estratégica não substitui validação de legalidade.

| Variante | Mudança experimental no RD | Resultado principal | Limite |
| --- | --- | --- | --- |
| Original PR #13 | Nenhuma | Falha nas duas ordens com pending | Referência preservada |
| A | Antes da troca, pending MAGIC/FIELD incompatível + seleção física válida: retirar pending e delegar uma vez | Resolve a ordem externa | Não executa uma regra interna; validade física não prova autoridade de um efeito |
| B, sem provider | Wrapper do planner consulta contrato opcional antes da captura/materialização | Mesmo comportamento original | PR #11 não oferece esse contrato |
| B + ponte **de teste** | A ponte chama o helper original de Queen e informa unchanged/replaced/cancelled | Queen respeitada nas duas ordens exercitadas | Demonstra transporte/timing da interface; não é provider de produção |

[Patch A](../experiments/hal25/alternative-a.patch) altera somente `mod.c`. Ele exige uma escolha de mão única, índice 11–15, objeto/registro ocupado, ponteiro para a instância real, ID coerente, destino Monster/Support válido e fusão legal quando o destino Monster estiver ocupado. Retira somente um pending incompatível ainda não materializado; chama o próximo materializador exatamente uma vez antes de qualquer troca própria. Não reconhece semanticamente um cancelamento bruto nem comprova a autoridade do wrapper. **Não é recomendado como solução completa.**

[Patch B](../experiments/hal25/alternative-b.patch) altera somente `mod.c` e acrescenta o [header experimental](../experiments/hal25/selection-rules-v1.h). Executa o planner original, procura `ritual-card-effects-test:selection-rules-v1`, valida a resposta antes da captura de categoria e permite que o owner substitua a seleção. Retira o pending porque o plano foi legitimamente substituído **antes de possuir qualquer troca física**. Ausência/ABI incompatível/provider indisponível sem mutação preservam o comportamento original. Cancelamento declarado ou resposta inconsistente impedem o commit, sem restaurar fallback. Não se acrescenta uma chamada pós-materialização.

Esse nome/ABI é uma **proposta não aprovada**, não uma extensão instalada em Ritual Effects. O harness registra a ponte no host de R e localiza o helper privado, de assinatura real `void cosmo_force_ai_selection(void)`, no objeto de hash qualificado. Ela verifica que deck e registros físicos permanecem idênticos antes/depois da chamada. O acesso privado aparece **somente na instrumentação**; nenhum patch de RD procura símbolos privados, offsets, assinaturas ou IDs 708/395. A ponte não deve ser distribuída como solução de MOD nem promovida ao runtime. Ritual Effects não foi modificado/recompilado.

## Reprodução e matriz

[Matriz por check](hal-25/matrix/MATRIZ.csv), [resultados completos](hal-25/matrix/RESULTADOS.json), [índice SHA dos logs](hal-25/matrix/LOGS_SHA256.json) e [logs integrais compactados](hal-25/logs/matrix.tar.gz). Os FAIL permanecem registrados; não foram convertidos em PASS por serem esperados.

| Conjunto | Processos | PASS | FAIL | Incompletos |
| --- | ---: | ---: | ---: | ---: |
| Original | 75 | 50 | 25 | 0 |
| A | 75 | 62 | 13 | 0 |
| B sem ponte | 74 | 50 | 24 | 0 |
| B com ponte | 91 | 90 | 1 | 0 |
| Total principal | 315 | 252 | 63 | 0 |
| Restrições adicionais: posição/instância/OFF/novo commit | 8 | 8 | 0 | 0 |

A matriz principal contém **2.355 checks PASS e 64 FAIL**: 61 violações da expectativa Queen nos controles originais/parciais e três probes de cancelamento bruto, sendo que o original falha em ambos os checks no mesmo processo. B com ponte não falha nos casos normais de Queen; sua única falha é o cancelamento bruto artificial posterior ao planner. Os oito processos adicionais têm 48 checks PASS. Preflight e postflight verificam os mesmos 396 insumos e passam integralmente.

Para os três casos de Female legal (uma, múltiplas e duas cópias iguais), a matriz AI OFF/ON × Dual OFF/ON, em cada ordem, contém 12 processos por célula:

| Variante | R interno ao RD (`before`) | R externo ao RD (`after`) |
| --- | --- | --- |
| Original | 12 FAIL | 12 FAIL |
| A | 12 FAIL | 12 PASS |
| B sem ponte | 12 FAIL | 12 FAIL |
| B com ponte | 12 PASS | 12 PASS |

`before`/`after` nomeiam a ordem **de carregamento**, não o topo efetivo. R recebe rank -20 ou 40, H 0, RD 10, Dual 20; o observador próprio tem rank 100. A ordem efetiva e os argumentos são emitidos em `REGISTER`, `GATE_INPUT`, `TRACE` e `CALLBACK`. Os caminhos mínimos efetivamente observados são:

- Original, R interno: `RD>`; #680, pending 1→0, uma troca de registros. `cosmo_hand_swap` não é chamado nesse swap.
- Original, R externo: `R>RD>engine>`; R seleciona #395 e RD restaura #535.
- A, R externo: `R>RD>engine>`; #395, pending 1→0, zero trocas próprias.
- B com ponte, R interno: `RD>R>engine>`; #395, pending retirado no wrapper do planner, zero trocas próprias.
- B com ponte e H/D ativos: a categoria definitiva de #395 chega ao gate real do Dual; há somente um consumo físico. Essa configuração não prova correção global de I1/I2 nem de outras ordens/ativação tardia.

Os logs registram ID, record, deck_index, data_block, igualdade de ponteiro, seleção lógica, pending e contador de trocas. Com duas #395, a identidade 43 é selecionada e a 44 permanece na mão. Com múltiplas Female (#708/#395), o helper original pode escolher #708: não se força #395 nem se substitui identidade por igualdade de ID.

## Cobertura dos cenários solicitados

| Cenário | Resultado e alcance |
| --- | --- |
| 1. Queen adversária/Female legal | Reproduzido sem pending/plano/quota injetados; original falha, B+ponte passa |
| 2. Múltiplas Female | B+ponte respeita a escolha do produtor; não impõe preferência nova |
| 3. Nenhuma Female | Sem override; plano original permanece executável |
| 4. Female sem vaga válida | Campo cheio e oracle de fusão inválida modelados; sem override, sem relaxar Queen |
| 5. Plano pendente | Planner real prepara busca da Magic #680; é preparação de ritual, não invocação/consumo já concluídos |
| 6. Sem pending | Perfil legado Isis sem plano no cenário 6; callbacks reais preservam a Female |
| 7. Pending compatível/incompatível | Sem override, o plano compatível é mantido; seleção incompatível substituída antes do commit no B+ponte. Um pending de busca de Female já compatível não surgiu naturalmente neste catálogo: não foi fabricado |
| 8. R ON/OFF/ausente | Casos próprios com original/A/B/ponte; OFF efetivo sem provider funcional, ausência sem biblioteca; sem dependência obrigatória |
| 9. AI HARD MODE OFF/ON | Matriz cruzada; sem alterar defaults, planejamento, preserve_hand ou deck_access |
| 10. Dual OFF/ON | Objeto privado API24 qualificado, gate/ledger reais; bonus filtra a categoria usada |
| 11. Duas ordens | Falha original em ambas, A parcial, B+ponte passa nas duas com os ranks exercitados |
| 12. Cópias físicas iguais | Identidade selecionada e cópia não selecionada conferidas; sem troca por ID |
| 13. Cancelamento/fallback | 16 respostas declaradas cancel/invalid de B+ponte impedem consumo; três cancelamentos brutos após planner continuam FAIL. Estado artificial explicitamente separado, sem comprovação de alcance pela UI |
| 14. Reset/novo duelo/save-load | Callbacks SAVE/LOAD, buffers registrados, reset e init nativos; não inventa persistência de BSS/pending nem certifica arquivo de save Windows |
| 15. Field/Hand/Both | Oito processos adicionais dentro da matriz principal usam seletor/consumidor real de Melhorias/RT. Field e Both passam, cópia spare preservada; Hand-only rejeitado pelo default aprovado `min_field=1` |
| 16. Outras restrições | Black Chaos: contrato público real `position-rules-v1`, ritual commit real, instâncias iguais distintas, OFF, novo commit ON e duas ordens com H/D ativos: 8 PASS. Regressões originais incluem Yamadron mandatory burn e Puppet. Isso não demonstra que o contrato de mão cobre posição ou alvo de ataque |

Das 307 execuções de controller, 291 chegaram à fase 7 e 16 permaneceram na fase 4 por veto declarado/entrada inválida. As oito restantes da matriz são probes de tributos. Em 69 processos o **bônus** não fez commit e o log diz `NOT_REACHED bonus no legal commit`; há verificação de rejeição sem consumo, não uma segunda jogada fictícia. Esses processos não são contados como gameplay de bônus executado. O contador global de PASS/FAIL é de assertions/execução e não substitui esta distinção de alcance.

## Regressões originais e correção da infraestrutura de auditoria

[Resultados brutos](hal-25/regressions/SUMMARY.json), [logs](hal-25/logs/regressions.tar.gz), [rechecagem do pacote](hal-25/package-audit/SUMMARY.json) e [proveniência própria](hal-25/PROVENIENCIA.json).

- Ritual Effects PR #11: **23 grupos PASS / 3 SKIP**; loader real API24, efeitos, contratos, instâncias, títulos e regressões originais. SKIP de performance por baseline histórico ausente.
- RD original/A/B: **34 grupos funcionais PASS por variante / 3 SKIP**, incluindo loader dos respectivos objetos, planejamento, receitas, escolha física, efeitos opcionais, decks, distribuição, starter, drops e assets. C nativo com ASan/UBSan; loader i386 com host simulado, sem sanitizador.
- A primeira execução do check histórico de pacote falhou nas três variantes porque a cópia temporária não continha `notes/hal-20/ritual-duelists/INSUMOS_SHA256.json`. Esse foi erro de preparação do teste, não bug do MOD. A metadata original agora acompanha a reprodução. Reexecutado **apenas esse grupo**, preservando os 34 grupos já concluídos e os logs iniciais: original PASS; A/B FAIL na assertion de imutabilidade de `mod.c`, como deve ocorrer em fontes alterados. Não se enfraqueceu nem alterou a assertion original para transformar candidatos em pacotes inalterados.
- A auditoria própria verifica os dois snapshots originais e o delta exato: A somente `mod.c`; B somente `mod.c` + header novo. Os outros C/H, manifest/seis settings, assets, receitas, decks e contratos existentes permanecem idênticos. Os candidatos não recebem `SDK-PROVENANCE.json` antigo como se fosse nova proveniência.
- SKIP do RD: gameplay/controles/render/áudio/save Windows, Droplist externo 1.4.0 não fornecido, ecossistema completo/savestates históricos/outros alvos. A matriz HAL25 qualifica somente as combinações descritas; não elimina esses SKIP amplos.

## Builds e ABI

[Builds/toolchain/comandos](hal-25/BUILD.json), [comparação ELF/ABI](hal-25/ABI.json), [inputs](hal-25/INPUTS.json). Compilação integral dos **16 C** de cada variante pelo builder oficial SDK24 `--target i386`, Clang 23.1.2/llvm-mingw20260922. Headers/exports/compiler/builder conferem com a proveniência original. Não houve ligação com objeto recebido antigo.

| Objeto | Bytes | SHA-256 | Hash loader |
| --- | ---: | --- | --- |
| RD PR #13 | 4.521.424 | `8df2e8b6e11bea0dd771bc5005848081191e7c468f5ee588976a6633129bb17e` | `57b53592` |
| A experimental | 4.523.592 | `833278805c4daaf0e83990d2095c5ab39c57de1eda2e708eefcaae793df3a176` | `22756a40` |
| B experimental | 4.525.608 | `36fd7cf3b303afcaa4a74302cb3ba92211f63f6a81c08175a9edb2ca068278a0` | `2a32ba9b` |

Formato ELF32 REL i386 esperado pelo loader Windows MOD; não são DLLs. Os **87 imports e 44 exports públicos** permanecem idênticos, todos os imports no SDK oficial. Init/API mínima 12 originais preservados, aceitos pelo host API24; isso não certifica hosts antigos. Estado RD registrado continua 288 bytes/v4; o pending privado de 40 bytes não foi acrescentado ao save. Nenhuma mudança de assets, receitas/settings ou versão/ID. SHA do objeto inteiro depende também de DWARF/caminhos absolutos; rebuild em outro diretório não implica necessariamente o mesmo SHA.

RT PR #11 segue com objeto `c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1` / code `52ce4829`. Dual local é exclusivamente o recompilado API24 do PR #10, SHA `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`, code `3028c68a`; nenhum fonte, manifest ou binário Dual foi publicado. AI e Melhorias locais e dez unidades nativas do motor estão identificados em INPUTS. Drop Missing Cards oficial está ativo nos ambientes que contêm RD. Droplist, Time Wizard e Fusion/F1 não foram incorporados.

## Limites e recomendação

Os objetos reais executam sob QEMU i386/Linux com manager/loader/hooks oficiais e dez unidades nativas selecionadas. Tabuleiro, catálogo, VM de proposta Vanilla, gráficos e resolução pós-colocação são modelados. O commit de ritual de Queen é real, mas seu caminho de animação nativo é substituído por uma fronteira sem IO. Não há disco, Windows, duelo completo, controle humano ou teste de rede/local multiplayer. Nenhum resultado é certificação Windows.

Uma ordem fixa não resolve os dois caminhos originais: há bypass interno ou fallback externo. A solução menor a investigar é uma **validação opcional publicada pelo owner do efeito**, após planejamento e antes da captura de categoria/materialização. B demonstra o ponto de integração sem duplicar a regra. Antes de implementar um produtor real, é necessário aprovar e fechar o contrato: domínio mão vs campo/ataque/posição, resultado explícito de cancelamento/indisponibilidade, seleção física e destino, proibição de materialização/mutação de deck, epoch/snapshot válido e revalidação se uma camada posterior mudar a escolha. A assinatura proposta usa globals por conveniência da PoC e não oferece autenticidade transacional nem proteção universal contra wrappers tardios.

O produtor real deve executar sua própria regra, respeitar OFF efetivo e não impor uma dependência obrigatória ao RD. O consumidor deve preservar plano quando unchanged, retirar somente o pending substituído ainda não executado, nunca restaurar uma decisão que recebeu veto e nunca materializar duas vezes. Ausência do produtor mantém independência, mas **não pode ser anunciada como Queen Lock corrigido**. O cache/ordem e todas as combinações reais ainda precisam de qualificação; o helper privado da ponte não é uma alternativa de produção autorizada.

## Respostas A–F

**A.** RT externo troca a seleção no `cosmo_hand_swap`; RD restaura o fallback ao invalidar o pending. RD externo materializa e retorna antes do callback de RT. Ambos ocorrem no par mínimo.

**B.** Parcialmente: A resolve o caminho externo alterando somente RD. Não foi demonstrada correção segura das duas ordens usando apenas RD e os contratos públicos publicados. B é consumidor experimental; precisa de um produtor público real, ausente no PR #11.

**C.** O contrato opcional do owner, com validação anterior à materialização, preserva melhor as responsabilidades. Ainda requer aprovação/interface final; não se recomenda copiar Queen Lock para RD, chamar função privada em produção ou exigir ordem como solução completa.

**D.** Não universalmente. Não há IDs de Queen/Female nos patches. B aceita uma decisão declarada do produtor no domínio da mão; posição/ataque/backrow têm outros momentos/contratos. Black Chaos, Yamadron e Puppet passaram em suas regressões, mas isso não prova interposição universal da interface proposta.

**E.** Nenhuma regressão funcional observada nos grupos originais e nos checks de identidade/consumo/quota executados. A e B sem provider continuam com falhas conhecidas; B+ponte não reconhece cancelamento bruto posterior à validação. Não há garantia geral sobre todos os hooks, lifecycle e savestates. A falha deliberada do audit de fontes imutáveis dos candidatos não é uma regressão de gameplay.

**F.** Falta autorizar e implementar o contrato real no owner, revisar sua semântica transacional, eliminar/qualificar o risco de cancelamento tardio, executar outras restrições em fluxo completo, rerodar regressões do ecossistema com hashes novos e testar Windows i386 com imagem legítima. Exigir testes de ambas as ordens, H/D OFF/ON, lifecycle/novo duelo, cópias físicas, animações, categorias de bônus, cancelamentos reais, render/controle e saves novos; savestates antigos em qualificação separada. Nenhum candidato desta branch está aprovado para merge/release.

## Publicação e continuidade

Fontes/assets são os pacotes próprios previamente autorizados. [Créditos de R](../mods/ritual-card-effects/NOTICES-HAL20.md) preservam a permissão informada de PeterIron para a moeda; [créditos do RD](../mods/ritual-duelists/NOTICES-HAL20.md) registram a doação de arte declarada pelo usuário. Isso não é licença aberta inventada nem autorização para Dual/Droplist. Os patches/ponte são trabalho próprio. Não há ROM, imagem do jogo, saves pessoais ou binários privados de terceiros nos acréscimos.

A auditoria de publicação identifica quatro objetos: os dois originais autorizados e A/B, mais os assets doados já publicados. Os `.bin` do RD são buffers gráficos desses assets, não uma imagem do jogo. `git diff --check` dos acréscimos próprios passa; aplicado aos snapshots integrais, acusa whitespace/CRLF já existentes, principalmente em `box_art.h` e CSVs históricos. Esses arquivos foram preservados byte a byte; nenhuma normalização foi feita para ocultar o diagnóstico.

[Reprodução dos testes](../tests/hal25/README.md), [uso exclusivo dos candidatos](../experiments/hal25/README.md), [hashes de publicação](hal-25/PUBLICACAO_SHA256.json). Os 28 heads remotos anteriores foram conferidos antes/depois do push e permanecem idênticos, incluindo master, integração e PRs anteriores. Os logs compactados foram reabertos e cada conteúdo conferido com seu índice SHA-256. Guardar as falhas e as diferenças de alcance ao continuar. A próxima etapa deve escolher/aprovar o contrato; esta entrega não autoriza promovê-lo.
