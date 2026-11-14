# HAL-20 — integração experimental com guard #20 e Fusion F1 #22

**14/11/2026 · Diagnóstico para revisão; sem merge/release. A coleção completa ainda não está pronta para gameplay protegido.** O guard original do PR20 recusa os wrappers próprios de Melhoria, Ritual Effects e Fusion na fronteira do duelo. F1 continua corrigida nos52 perfis admitidos que consultaram seus contratos. Foi reproduzido um conflito adicional **Ritual Duelists × Queen Lock**, inclusive sem AI/Dual, com motor parcial e sem injetar pending/plano/quota.

## Proveniência e escopo preservado

Base única `integration/hal-20-v0.3.1-preview.2`, commit `d649647c159c91d615c07d6960928a37e270b461`, oficial `aceac3782902746f35ddb0148809450933327cb3` / SDK API24. Branch independente [`test/hal-20-integracao-guard-f1`](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/test/hal-20-integracao-guard-f1), [PR #23 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/23). Lidos AGENTS.md e relatórios dos PRs9/10/11/13/14/18/19/20/22. Nenhum desses PRs foi alterado ou mesclado.

| Componente próprio | PR / commit | Objeto SHA-256 preservado |
|---|---|---|
| Melhoria/Surrender1.3.6 | [9](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/9), `0d28d844c3b88b38ab0db71635959ddd223d8c14` | `c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0` |
| Ritual Effects0.78.74-effect-ai | [11](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/11), `2cf0de31e88e9f1c058b3747b2b2add3db0e347c` | `c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1` |
| Ritual Duelists0.4.0-alpha.8 | [13](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/13), `9a363f28f6dbfe779b1b901ddecab74ab1be57e9` | `8df2e8b6e11bea0dd771bc5005848081191e7c468f5ee588976a6633129bb17e` |
| Fusion F1 candidata | [22](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/22), `e089a10b6fccbfebf756438407b87657062914d9` | `384e11eeeae32412e23afbbbd9ebc308761674b8a2181238fd60e17e315b865c` |

**469 arquivos próprios preservados byte a byte**, incluindo fontes, objetos, manifests, defaults e assets. Há uma única pasta/versão de `fusion-auxiliary-balance`; o objeto antigo do #12 não foi incorporado. AI oficial: SHA `9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc`, código `3e0648cb`. Dual0.5.7 local: SHA `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`, código `3028c68a`, não publicado. RD código `57b53592`. Drop Missing Cards1.0 oficial ON em todos os perfis; distribuição final de drops não foi medida.

Os **cinco fontes de produção PC** do [PR20](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/20), commit `936c3513c9e6f8131b1eb4a5410d9c21c3c2c592`, foram copiados exatamente: `manager.c`, `mods.c`, `mods.h`, `mods_window.c`, `main_run_duel.c`. Não há ajuste novo nesses arquivos. Hooks_Relink/Mods_Order permanecem oficiais. Opt-in `experimental.hal2224_order_guard=1`, default OFF no core; perfis de teste escolhem OFF/ON explicitamente. AI mantém seu manifest original, sem restart:true permanente. Não mudamos mecânicas, receitas, valores, decks, defaults ou quotas.

[INPUTS.json](hal-20/guard-f1/INPUTS.json) registra origens e hashes. Os517 preflights e517 postflights passaram por coorte; os quatro pacotes são também conferidos contra seus commits, não apenas contra uma lista gerada da árvore. Créditos/permissões anteriores de moeda e arte do RitualStarter foram preservados nos NOTICES dos pacotes. Não houve assets novos nem redistribuição de Dual/Time Wizard, ROM, saves pessoais ou dependências privadas.

## A. Os MODs coexistem com Fusion F1 corrigido?

**Parcialmente no ambiente testado; a composição completa protegida é recusada.** A matriz principal exercitou todos os63 subconjuntos não vazios dos seis MODs, guard OFF/ON, além de providers, tributos, estado e ordens alternativas:280 processos. Não houve execução incompleta.

| Coorte principal | Processos por classificação | Verificações |
|---|---|---|
| Guard OFF |136 PASS,4 FAIL |1433 PASS /8 FAIL |
| Guard ON |89 PASS,51 REFUSED |971 PASS /0 FAIL |
| Total principal |225 PASS,51 REFUSED,4 FAIL |2404 PASS /8 FAIL |

REFUSED significa fence acionado e controller **NOT_RUN**, não duelo aprovado. Os4 perfis FAIL são controles O0/O2 com guard OFF: reproduzem I1/I2/contabilidade já conhecidos, com e sem os demais MODs; não foram ocultados. A composição completa na ordem segura passou os checks regulares com guard OFF, sem provar segurança geral — a coorte Queen abaixo demonstra uma condição adicional que essa bateria básica não exercitava.

[Resumo](hal-20/guard-f1/SUMMARY.json) · [JSON principal](hal-20/guard-f1/main/RESULTADOS.json) · [matriz integral](hal-20/guard-f1/main/MATRIZ.csv). Os arquivos RAW_LOGS.tar.gz nos diretórios de resultados contêm todos os logs; LOG_INDEX.json identifica cada original por hash. Logs representativos também estão disponíveis diretamente como texto.

## B. O guard aceita a composição real?

**Não. Nenhum wrapper foi liberado ou reordenado para obter PASS.** Na composição completa, o primeiro bloqueio é `melhoria-dos-rituais` em `DuelScene_UpdateHandActions`. Isolando os outros, o guard também recusa Ritual Effects nesse controller/`func_8001BAF0` e Fusion no controller da mão.

A regra atual qualifica apenas AI/RD/Dual para swap e permite somente Dual no controller da mão quando existe o par protegido. Ela consulta registros reais e fingerprints dos três objetos. Os registros desconhecidos permanecem bloqueados mesmo quando a delegação parece simples. A recusa aparece antes de setup/controller; a transação não é continuada desligando MODs silenciosamente.

Na matriz principal, primeiras recusas observadas:32 por Melhoria,5 por Ritual Effects,10 por Fusion e4 por ordem insegura antes de haver hooks ativos. O guard também recusa os6 perfis Queen que contêm Dual + materializador e wrapper Ritual. A dupla Ritual+RD **sem Dual** não está no escopo do guard e pode ser admitida mesmo com o conflito de efeito descrito adiante.

Evidências: [composição recusada](hal-20/guard-f1/main/M-R-F-H-RD-D-g1-controller-safe-v5-before.log), [controle completo guard OFF](hal-20/guard-f1/main/M-R-F-H-RD-D-g0-controller-safe-v5-before.log), [inventário real dos114 registros](hal-20/guard-f1/HOOK_INVENTORY.json).

## C. Captura de categoria, delegações e qualificação necessária

| Alvo | MODs que registraram hooks nesta composição |
|---|---|
| `func_8001BAF0` | AI, RD, Dual, Ritual Effects |
| `DuelScene_UpdateHandActions` | Dual, Melhoria, Ritual Effects, Fusion |
| `AiScript_Run` | AI, RD, Ritual Effects; observer próprio de leitura |
| `func_80028220` | Dual |
| `Duel_CheckRitual` | Melhoria, Ritual Effects |
| `DuelEffect_ApplyRitual` | Ritual Effects, RD |
| `Duel_PopulateCombinedDeckData` | Melhoria, Ritual Effects |
| FieldActions / CardPlacement | Ritual Effects, Fusion; Dual também em FieldActions |
| DeactivateRecord / RemoveFromField | Melhoria/Ritual e Ritual/Fusion, respectivamente |

Com ordem inicial **AI→RD→Dual**, a cadeia swap observada é **Dual→RD→AI→motor** no trio, e **Dual→RD→Ritual→AI→motor** com os próprios anteriores ao trio. Os traces contam callbacks executados. RD pode materializar diretamente e retornar sem chamar o predecessor; assim Ritual interno não executa nesse caminho. Isso é relevante mesmo quando Dual conta a categoria corretamente.

O trio qualificado reproduziu **Support680 → bônus Monster535**, com Support344 ocultado após quota Support gasta, uma troca/consumo por ação, seleção física e ledger corretos. Com Ritual Effects ativo, o planner pode escolher Monster571 no bônus porque suas receitas/providers mudam os materiais requeridos; não se forçou535 substituindo estratégias. O papel Monster continua contabilizado corretamente nos perfis regulares seguros. O0/O2 com guard OFF repetem as falhas conhecidas; ON recusa.

Nos novos cenários seguros com wrappers próprios e guard OFF **não se reproduziu categoria final diferente da capturada por Dual**. Na coorte Queen houve divergência de **carta física**, mesmo quando a categoria Monster permaneceu igual. Não confundir validade de categoria/quota com obediência a uma condição de efeito.

Caminho de qualificação proposto, **não implementado**:

- **Melhoria** (`1fe34a1e`, SHA c36d...): provar delegação única do `hand_actions_hook` nos caminhos CPU e nos caminhos humanos/default/ritual_only_player/confirmar/cancelar. Ele pode suprimir um input humano autorizado por suas regras; não basta chamá-lo transparente pelo nome.
- **Fusion** (`afb75202`, SHA384e...): provar todas as saídas de `aux_hand_actions`, incluindo Square nos feitiços aprovados, mudança/restauração temporária de face e interação com a categoria/placement. Não tem hook próprio de swap; seu controller ainda exige qualificação.
- **Ritual Effects** (`52ce4829`, SHAc95c...): seu hand controller pode não delegar durante `chaos_turn.active`; `cosmo_hand_swap` pode mudar a seleção antes de delegar. Exigir rastreamento de categoria **e identidade** antes/depois de cada camada e exatamente uma materialização. A reprodução Queen impede conceder uma qualificação genérica nesta etapa.

Qualquer proposta deve amarrar hash de objeto/código, alvos/callbacks e semântica demonstrada, revisar todas as condições e repetir lifecycle/ordens. Não criar whitelist só pelo ID, não acrescentar previous_swap depois de ação concluída, não alterar Dual nem globalmente o linker de hooks.

## D. F1, contratos e efeitos ON com os demais MODs

**F1 passou em52 perfis admitidos de providers**, com Fusion isolado, pares/trios e composições maiores. Os12 outros perfis dessa suíte foram recusados antes da execução pelo guard. O objeto efetivo mantém código `afb75202` e SHA do PR22.

Os testes confirmam: Encore/leases ON; preferência OFF aguardando restart com aplicação ainda ON; callback OFF efetivo neutraliza `encore-v1`, `origin-unlock-v1`, `origin-effects-v1`, `effect_forecast_v2`; Mods_Find continua disponível; serial77 preservado; ON não ressuscita grant; bind novo gera78; reset retira a lease. Lease77 é semeada explicitamente para testar o contrato e o callback efetivo é entregue pela ponte de teste — não é uma UI no meio do duelo.

Egotist/704 consulta o provider **real** de Ritual Effects e respeita seu OFF; forecast Metalmorph retorna412/origem Metal. O provider recipe_v1 real666 continua disponível/ativo conforme o owner. Os16 grupos originais do Fusion, incluindo Fusionist, Round02/03, Egotist/Metalmorph, forecasts, instâncias e assets, foram reexecutados e passaram. Essa regressão é isolada: não se afirma que todos os efeitos/execuções/animações tenham sido executados ao vivo com a coleção inteira.

Logs: [providers M+R+F](hal-20/guard-f1/main/M-R-F-g1-providers-safe-v5-before.log), [providers completos guard OFF](hal-20/guard-f1/main/M-R-F-H-RD-D-g0-providers-safe-v5-before.log), [regressão Fusion](hal-20/guard-f1/FUSION_REGRESSION.json). O guard não foi contornado em composições recusadas para rodar esses probes.

Time Wizard não foi incorporado ou alterado. Seu cache de elegibilidade de tentativas iniciadas ON, descrito nos PR21/22, continua limitação conhecida. A correção de provider não cancela um consumidor já em voo. Núcleo/assinaturas da ponte Dual, adaptador AI0.1.1 e Droplist externo seguem separados/bloqueados.

## E. Novo conflito reproduzido: RD × Queen Lock

Coorte adicional de20 processos: **152 PASS /8 FAIL**,6 PASS,8 FAIL,6 REFUSED; nenhuma execução incompleta. As8 falhas são a mesma invariante em composições/ordens diferentes, não oito bugs novos. [Resultados Queen](hal-20/guard-f1/queen/RESULTADOS.json) · [matriz](hal-20/guard-f1/queen/MATRIZ.csv).

Preparação: campo/mão sintéticos; Cosmo Queen708 no campo adversário, marcada pelo **hook público real de commit ritual**, Female395 na mão física e vaga livre. O catálogo modela395 no grupo Female. Não foram escritos a marca privada, pending, plano ou quota. O planner RD produz sua própria busca680. A descrição aprovada de Queen exige jogar uma Female da mão se legalmente possível.

- **Ritual antes de RD:** RD materializa680 diretamente, sem chamar `cosmo_hand_swap`. A Female válida continua na mão; a regra de Queen não foi aplicada nessa primeira ação.
- **Ritual externo ao RD:** o wrapper real transforma a seleção em395. RD identifica seu pending como incompatível com a nova seleção e vai ao fail/fallback (`mod.c:711–775`), restaurando a seleção anterior535. A carta consumida é535; pending termina0. A captura Dual pode continuar Monster correta, mas a identidade/elegibilidade do efeito está errada.
- O par mínimo **Ritual+RD, AI OFF/Dual OFF** reproduz ambos. Guard ON também permite esse par, porque sua proteção cobre pares com Dual, não todas as condições dos efeitos. **Não é F1 nem uma janela artificial HAL23**: ocorre dentro da chamada síncrona normal do controller usada pela fixture.
- O controle **Ritual+AI sem RD** consome395 nos dois posicionamentos e passa. Com Dual presente e guard ON, as variantes com Ritual são recusadas, sem ação executada.

[Par mínimo, Ritual interno](hal-20/guard-f1/queen/R-RD-g1-controller-safe-v5-before.log) · [par mínimo, Ritual externo](hal-20/guard-f1/queen/R-RD-g1-controller-safe-v5-after.log) · [controle R+AI](hal-20/guard-f1/queen/R-H-g1-controller-safe-v5-before.log).

**Causa provável a tratar em tarefa específica:** RD tem materialização própria que pode contornar a validação do efeito; no caminho de falha, seu fallback pode substituir uma seleção legitimamente transformada por outra camada. Propor investigar o menor ajuste no código próprio de RD para preservar/validar a decisão final e a responsabilidade do materializador, com ordem e contrato explícitos. Não inventar nova estratégia, não trocar a regra de Queen, não chamar previous_swap cegamente e não modificar os terceiros. Nenhuma correção foi implementada.

A alcançabilidade em duelo Windows real ainda precisa ser demonstrada. O tabuleiro é preparado pela fixture, a resolução de efeitos é parcial, e não houve disco/bytecode retail integral. A evidência é suficiente para bloquear qualificação automática do wrapper e exigir regressão específica, não para afirmar exploração comprovada numa partida completa.

## F. Combinações, tributos e lifecycle

- Todos os subconjuntos foram carregados em processos separados, com manifests/defaults intactos e DMC ON. A matriz integral distingue PASS, FAIL, REFUSED e cenários NOT_RUN/NOT_REACHED nos logs.
- Na suíte de tributos,52 perfis passaram e12 foram recusados pelo guard. Field consome três instâncias corretas; Both usa campo+mão, preservando outra cópia igual e bystanders. Hand-only é recusado com o default min_field=1, sem relaxá-lo. Rituais Vanilla e receita/provider modificados foram comparados. Animação/invocação completa continuam pendentes.
- Estado:13 perfis passaram e1 foi recusado. SAVE/reset/LOAD visita só buffers realmente registrados, não serializa BSS por suposição. Reset e entrada nativa de novo duelo não deixaram pending no recorte; savestates antigos não foram aprovados.
- HAL23: a alteração do manager acontece **após retorno completo do controller**, com pending já consumido. Não há chamada reentrante entre planner e swap, nem teste que fabrica disponibilidade de menu. Não se reproduziu pending abandonado por essa transição normal examinada; #18 continua a referência dos gaps instrumentados. Queen/fallback é achado separado com estado AI estável.
- OFF/ON/relink e primeira carga do AI são observados pelo manager real; quando uma mudança resulta em ordem não verificável, a chamada Ready permanece capaz de recusá-la. Preferência salva e aplicação efetiva são registradas separadamente. UI Windows, confirmações/falha de restart e execução integral de turnos continuam pendentes; os testes prévios #20 de janela não foram convertidos em gameplay real.

Erros de instrumentação corrigidos somente nos testes: stats genéricos de Yamadron inicialmente faziam a receita condicional rejeitar materiais por estarem acima do limite relativo ao resultado; foi restaurado1600/1800 do catálogo de referência. O primeiro probe Queen não oferecia o serviço de grupo Female; foi acrescentada a classificação395 e seu include correto antes da coleta final. Esses protótipos não são bugs de produção. Nenhum FAIL final foi apagado, corrigido mudando defaults ou resolvido por relaxar o guard.

## G. Windows: build e empacotamento, ainda sem gameplay

**Build Windows i386 completo passou**, usando dependências abertas já preparadas e os cinco fontes exatos do guard. PE32/EM_i386,628 unidades de jogo e773 símbolos de dados fixados. Os12 stubs outside_resident_image e18 sdk_asm são idênticos ao relatório de link da base; não são novos problemas desta migração. [Build/log](hal-20/guard-f1/windows-build.log) · [comparação estruturada](hal-20/guard-f1/WINDOWS_BUILD.json).

Foi gerado/verificado **somente localmente** um ZIP experimental de23.963.156 bytes,490 arquivos, SHA `efaea448fe9e42bc16091033184d89663e99f3ac4e3c48d2d0c94236746b7fee`. [Inventário e prova do pacote](hal-20/guard-f1/PACKAGING.json). O pacote usa objetos próprios pinados, AI oficial qualificado e DMC; não as cópias recompiladas automaticamente pelo builder com outros caminhos de debug. Não contém Dual, Time Wizard, ROM ou saves pessoais. O ZIP/executável não foram publicados como release; scripts e hashes permitem repetir a preparação local.

[Procedimento Windows completo](../tests/hal20-guard-f1/WINDOWS.md): instalação isolada, verificação de hashes, dependência Dual local, nove perfis, casos de rituais/Fusion/quotas/Queen, reset/save e coleta de logs. O launcher PowerShell não altera a instalação habitual ou perfis pessoais e nunca desliga o guard automaticamente. `full-refused` serve para verificar a recusa/UX, não para jogar contornando a proteção.

Para iniciar gameplay real: obter imagem legítima local e ambiente Windows i386, criar dados de teste novos, possuir o Dual API24 qualificado para seus perfis, validar primeiros subconjuntos e mensagens. Para jogar a coleção completa protegida, **antes** será necessário resolver o conflito Queen/RD e qualificar rigorosamente os wrappers M/R/F, com autorização específica para eventuais correções. Logs Windows atuais não fornecem a mesma contagem de cada callback da fixture; essa observabilidade também deve ser planejada sem registrar wrappers críticos novos.

**Decisão desta entrega:** manter a coleção em diagnóstico; F1 preservada, guard inalterado, novo bloqueio próprio documentado. Sem alteração em master/integração/PRs anteriores, sem merge, release ou declaração de gameplay aprovado. [Índice de publicação](hal-20/guard-f1/PUBLICACAO.json) e [27 heads anteriores preservadas](hal-20/guard-f1/REFS_PRESERVADAS.json). Push normal, sem force; PR23 permanece em rascunho.
