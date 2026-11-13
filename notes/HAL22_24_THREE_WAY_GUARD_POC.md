# HAL-22 + HAL-24 — PoC conjunta de proteção da ordem efetiva

**A PoC impediu I1 e I2 nos cenários protegidos exercitados: 255 processos, 3058 verificações PASS, zero FAIL e 24 SKIP de composição individual/recusada na suíte do par.** O protótipo recusa composições não verificáveis antes do controller; não corrige nem reescreve os materializadores. Os controles sem proteção continuam reproduzindo as falhas. Não há aprovação para adoção definitiva ou gameplay Windows comprovado.

Branch independente `test/hal-22-24-three-way-guard-poc`, derivada diretamente da integração `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`; oficial `aceac3782902746f35ddb0148809450933327cb3`, API24. Escopo autorizado: protótipo reversível, opt-in, sem merge. Continuidade: AGENTS.md e relatórios dos PRs [14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14), [15](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/15), [16](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/16), [17](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/17), [18](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/18) e [19](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/19), com commits/hashes em [REFERENCIAS.json](hal-22-24/REFERENCIAS.json). Vinculada a [HAL-22](https://linear.app/half-programer/issue/HAL-22) e [HAL-24](https://linear.app/half-programer/issue/HAL-24).

## Respostas A–G

**A. O novo guard impediu I1 e I2 nos cenários protegidos?**

**Sim, no recorte e nas versões qualificadas.** Na suíte principal, 204 processos protegidos passaram 1224 verificações: 106 foram admitidos e 98 recusados antes de setup/controller. A suíte do par, reaproveitada do PR17, teve 51 processos protegidos/1834 PASS/zero FAIL/24 SKIP. As recusas são resultados esperados da proteção, não duelos executados com sucesso. Não houve execução incompleta.

A regra exige AI→RD→Dual quando os três estão aplicados; AI→Dual ou RD→Dual nos pares correspondentes. Ela valida a ordem resolvida e a posição real dos hooks. O guard anterior do PR17 só conhecia AI/Dual e não protegia RD externo ao Dual; não foi promovido automaticamente. Os cinco pontos de intervenção foram reavaliados, conforme [revisão por arquivo](hal-22-24/REAVALIACAO_PR17.md).

Os 57 processos de controle OFF mantêm 171 FAIL diagnósticos (13 na suíte principal, 158 no par). A coleta completa termina1 por esses controles, deliberadamente. Não foram transformados em PASS/xfail nem contornados com alterações de quotas ou estratégia.

**B. AI→RD→Dual preservou seleção física e categoria correta?**

**Sim nos cenários executados, incluindo a reprodução do PR19.** O exemplo principal usa objetos reais, AI ON, RD ON, Dual ON, sem chamadas ao stub retail:

1. O planner RD cria seu próprio pending MAGIC para buscar 680 na posição lógica16; nenhum pending/plano/quota é escrito pela fixture.
2. A cadeia real executa Dual antes do RD. Dual captura Support=2; RD troca uma instância existente uma vez, passa a seleção ao slot físico12 e consome o pending. O worker AI não executa nessa primeira troca.
3. O controller real remove 680 da mão e o callback do Dual registra Support=2, em vez do Monster=1 incorreto do PR19/O0.
4. Após a ponte modelada de conclusão de placement, callbacks reais de frame concedem bônus. Support 344 é ocultado pela quota já gasta; o planner escolhe Monster 535 e o controller consome essa instância, ficando `used=3`.

[Exemplo protegido](hal-22-24/resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) · [controle OFF que repete Support](hal-22-24/resultados/g0-O0-h1-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50.log).

Os testes comparam callbacks **executados**, categoria do plano lógico, carta física removida, ledger e identidades dos registros. Não usam apenas presença em Hooks_At. Os checks de conservação, seleção única e registros não selecionados passaram nos cenários admitidos; não houve segunda troca indevida. As primeiras escolhas do AI no par continuaram 344/35, como antes; os defaults, preserve_hand e deck_access foram conferidos pelo manager real. A estratégia não foi alterada.

A primeira ação usa controller/snapshot/planners/materializadores reais; catálogo, render e serviços auxiliares são parciais. A concessão do bônus depende da ponte explícita efeito/placement→campo. Portanto isso não demonstra que o fluxo completo Windows ou toda invocação ritual foi validado.

**C. Continua funcionando com pares e MODs individuais?**

| Composição aplicada | Restrição do guard | Evidência/limite |
|---|---|---|
| AI+RD+Dual | AI→RD→Dual | Gate Dual executa antes do worker responsável. |
| AI+Dual, RD OFF | AI→Dual | Regressão HAL22 natural/adversarial e lifecycle passou. |
| RD+Dual, AI OFF | RD→Dual | Planner RD cria a ação; gate e contabilidade passam nos cenários admitidos. Predecessor retail é um stub de status sem seleção. |
| AI+RD sem Dual | Nenhuma restrição de quotas Dual | Ordem entre os dois não é imposta pelo guard; não se declara HAL23 corrigida. |
| Apenas AI, RD ou Dual | Nenhuma restrição conjunta de ordem | Inicialização/admissão independentes preservadas; os probes do par exercitam os workers. Dual sozinho não equivale a gameplay com VM retail. |

Startup anteriormente recusado é uma exceção deliberada: a transação inteira continua marcada para relançamento, mesmo após editar o perfil ou desligar o opt-in. Não é um bloqueio novo de um singleton iniciado corretamente, mas a recusa de retomar uma transação que não inicializou.

**D. Alguma carga tardia, OFF/ON, wrapper ou reinício deixou passar uma cadeia insegura?**

**Nenhuma nos caminhos protegidos exercitados.** Startup seguro/invertido/empatado, primeira carga de H/RD/D, OFF/ON, Apply, API direta, recarga, reparo de perfil e processos novos foram testados. As preferências salvas e o estado efetivamente aplicado são observados separadamente. Reinício não conserta ranks errados: o startup inseguro é recusado, em vez de reconstruir a falha silenciosamente.

Um empate com pastas temporárias resolvidas H→RD→D foi admitido; quatro resoluções empatadas inseguras foram recusadas. Não se deduz segurança do valor numérico do rank ou nome da pasta isoladamente.

Wrappers desconhecidos já registrados no swap ou no controller da mão são recusados, inclusive os transparentes. Foram testadas posições externas/interpostas e um wrapper que deliberadamente não delega. A recusa ocorre antes de qualquer ação; não se tenta reordená-lo ou desligá-lo silenciosamente. Um wrapper em `Duel_DrawFieldCards` continuou executando normalmente, inclusive na regressão OFF/ON do par.

Também foi identificado o risco de uma primeira carga desconhecida registrar hooks **depois** do fence. A PoC recusa essa primeira ativação de código enquanto a composição protegida está aplicada, antes de inicializar a biblioteca ou alterar sua preferência. Dois casos de callback próprio (`late-swallow`/`late-transparent`) inserem a solicitação depois do planner e antes do swap; o pedido fica `wanted=0,active=0,code_hash=0`, pending/seleção permanecem intactos, e o Dual executa o gate normalmente. Essa reentrância é instrumentação, não uma janela de UI demonstrada e não uma correção da HAL23.

[Primeira carga recusada antes de init](hal-22-24/resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wlate-swallow-x0-p50.log) · [wrapper interposto recusado](hal-22-24/resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p3.log) · [wrapper não relacionado](hal-22-24/resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wunrelated-x0-p50.log).

**Limite:** isso não é prova contra qualquer código nativo, alteração de memória ou novo hook registrado arbitrariamente no meio de uma ação por callback já inicializado. A PoC qualifica os objetos específicos e os registros observados; alvos sensíveis desconhecidos ficam bloqueados quando detectados. Uma composição com esse comportamento ainda não qualificado não deve ser aprovada. Não se mudou globalmente Hooks_Add/Relink para tentar garantir execução de código arbitrário.

**E. Efeitos para o usuário da recusa e do reinício obrigatório**

- Opt-in novo `experimental.hal2224_order_guard=1`, default0. A chave antiga do PR17 não o habilita. Sem opt-in e sem recusa anterior do processo, permanece o comportamento original, inclusive os controles inseguros.
- Ordem insegura proposta na janela é recusada antes de gravar ranks/confirmar restart. A UI conserva preferências e mostra o motivo. A API direta também conserva o valor anterior quando recusa uma ativação; recarga manual de settings mantém o pedido no arquivo e não aplica a composição insegura.
- Startup recusado não inicializa a transação de dados/código. O PC Main_RunDuel retorna antes dos controllers e mostra erro uma vez por período bloqueado. Não há fallback silencioso para Vanilla. Reparar/salvar o perfil não autoriza continuar naquele processo: é preciso relançar, inclusive após desligar o opt-in.
- AI restart:true existe somente nos manifests temporários. Junto ao restart original do RD, ON/OFF desses MODs fica adiado até um processo novo. Cancelar a confirmação não aplica a mudança; falha de restart conserva a cadeia antiga e mostra a instrução de relançamento manual. A UI avisa sobre progresso não salvo. Se a cadeia antiga for insegura, permanece bloqueada.
- Código desconhecido ainda não inicializado não pode ser ligado ao vivo enquanto o par protegido está ativo. O pedido recusado não é agendado como ON: preparar um perfil completo antes do relançamento, ou carregar o MOD antes de ativar Dual, exige um fluxo de UX ainda a revisar. Essa limitação é intencional no protótipo e impede promovê-lo como solução pronta para usuários finais.

A janela executada é a rotina real de staging/Apply com o preflight experimental. A apresentação de erro e Platform_RestartGame são stubs observadores neste host; o relançamento gráfico e a disponibilidade visual do menu durante a recusa ainda precisam de teste Windows.

**F. Quais arquivos seriam necessários e o que impede adoção definitiva?**

Somente cinco arquivos de produção foram alterados **nesta branch experimental**, com 166 linhas adicionadas e uma removida:

| Arquivo | Intervenção experimental |
|---|---|
| `src/pc/mods/manager.c` | Regras por subconjunto, ordem resolvida/registrada, projeção de restart, versões qualificadas, wrappers críticos e primeira carga ao vivo. |
| `src/pc/mods/mods.c` | Preflight de startup/recarga e SetEnabled; recusa antes de dados/código; inclui RD nos status. |
| `src/pc/mods/mods.h` | Quatro funções internas; não muda o ABI de host dos MODs. |
| `src/pc/platform/mods_window.c` | Validação dos ranks staged antes de gravar/pedir reinício. |
| `src/game/main_run_duel.c` | Fence PC de entrada/continuação do duelo e erro explícito. |

[Diff reversível](hal-22-24/DIFF_EXPERIMENTAL.patch) conferido com aplicação contra o índice da base e reverse-check contra a árvore experimental. [Reavaliação PR17](hal-22-24/REAVALIACAO_PR17.md). Hooks_Relink, Hooks_Add, Mods_Order e seu ordenador permanecem byte a byte oficiais. Nenhum materializador/estratégia de AI/RD, receita, quota, fonte/manifest/binário Dual foi alterado; nenhum previous_swap foi adicionado. O campo restart não foi promovido a manifest permanente.

Impedimentos: gameplay Windows e relançamento real ausentes; política de recusa atômica e UX ainda experimental; fingerprints restritos aos objetos i386 qualificados; versões futuras/outros alvos exigem qualificação; perfis exportados atualmente priorizam `mod.*`, e a chave experimental não ganhou um fluxo próprio de exportação/UI; wrappers e callbacks novos precisam de análise. O fence cobre o caminho PC Main_RunDuel, não toda possível chamada nativa externa ao controller. As leituras de estado privado usadas nos testes não são API nem solução de produção.

A tabela de fingerprints vem do loader existente e é vinculada aos SHA256 verificados dos insumos. Não é uma fronteira de segurança criptográfica contra código malicioso/colisões. A adoção definitiva exige revisão explícita dessas políticas; os cinco pontos não foram promovidos à integração/master.

**G. Regressões ainda obrigatórias com outros MODs e Windows**

Foram reexecutadas 3072 comparações de ordem com dez manifests reais (AI, Dual, Melhoria, Ritual Effects, Fusion Effects, RD, Drop Missing Cards, 3D Monsters, Hand Camera e Yamyi), mais 3072 validações de ranks staged. 2304 configurações foram admitidas e 768 recusadas; 1920 sem composição de quotas não receberam a restrição. A ordem geral foi idêntica com guard OFF/ON. Suíte oficial de hooks e comparação das primeiras escolhas do AI também passaram: três regressões entre perfis PASS.

Isso é regressão de ordenação/isolation, não execução integrada de todos esses objetos. Permanecem obrigatórios:

1. Windows i386/API24 com imagem legal local: startup/menus/erro visível/recuperação, confirmação/cancelamento/falha e execução efetiva do relançamento. Conferir que o usuário consegue reparar o perfil sem perder preferências ou entrar em duelo recusado.
2. Duelo completo e catálogo/decks/bytecode reais: sequência680 normal,344 disponível, quota Support correta e bônus Monster; cenários HAL22 de344/35, quotas gastas, ritual no bônus, cancelamento, seleção física, múltiplas instâncias e exatamente uma materialização.
3. Melhoria/Surrender, Ritual Effects, Fusion Effects e RD juntos: todos os hooks efetivos, providers presentes/ausentes/OFF, Field/Hand/Both e invocação/consumo completo. Manter F1/HAL21 e pending/HAL23 em tarefas próprias; esta PoC não os corrige.
4. Importar/substituir pacotes, overrides, falha de gravação, novos registros de hooks em callbacks e outras arquiteturas. Qualificar ou manter bloqueados wrappers/versões não demonstrados seguros.
5. Reset/novo duelo/save-load reais, dois controles e caminhos humanos. Não presumir compatibilidade de savestates antigos. O teste parcial de providers/layouts e a ordenação por manifest não certificam efeitos, drops ou gameplay.

## Evidências e execução

[Matriz completa](hal-22-24/MATRIZ.md) · [Resumo](hal-22-24/SUMMARY.json) · [principal JSON/logs](hal-22-24/resultados/RESULTADOS.json) · [par JSON/logs](hal-22-24/par-resultados/RESULTADOS.json) · [reprodução](../tests/hal2224/README.md).

| Coorte | Processos | PASS | FAIL | BLOCKED | SKIP |
|---|---:|---:|---:|---:|---:|
| Principal protegida, incluindo guard armado após legado |204|1224|0|0|0|
| Par protegido |51|1834|0|0|24|
| Principal OFF |12|105|13|0|0|
| Par OFF |45|1908|158|36|9|

Total 312 processos. Os 36 BLOCKED pertencem somente à limpeza de cenários de controle cujo gate foi contornado; não são execução incompleta ou falha nova da proteção. Os SKIP do par correspondem a suíte não aplicável/recusada. A matriz principal registra NOT_RUN/NOT_REACHED nos logs para ações não executadas; não as converte em gameplay PASS.

43 preflights/43 pós-checks da principal e 49/49 do par passaram. MODs e dez unidades nativas anteriores não foram recompilados; foram compilados apenas harnesses/observadores próprios, a unidade experimental Main_RunDuel e testes de ordenação/hooks. AI `9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc`, RD `8df2e8b6e11bea0dd771bc5005848081191e7c468f5ee588976a6633129bb17e` e Dual `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378` permaneceram idênticos. Defaults de AI foram conferidos; Drop Missing Cards1.0 ficou ON na principal. Os manifests próprios só informam a regressão de ordem, além do objeto opcional Melhoria nos layouts admitidos.

Durante a preparação foram corrigidas ligações do harness entre a unidade nativa e os serviços reais do guard, sentinelas do mock de ordenação e o predicado staged do novo trio. Eram erros de instrumentação, não falhas dos MODs; a coleta final acima é posterior às correções. Os logs brutos mantêm warnings de offsets dos headers oficiais. Não houve ajuste de mecânica ou limpeza artificial de pending para passar testes.

A publicação contém os cinco fontes experimentais, diff, testes próprios, logs e documentação. Não contém código/manifest/binário Dual, assets de MOD, ROM, saves pessoais ou segredos. [Índice por hash](hal-22-24/PUBLICATION_FILES.json) · [refs preservadas](hal-22-24/REFS_PRESERVADAS.json). Nenhum PR anterior, master ou integração foi alterado e nenhum merge foi feito. A entrega é uma proposta testável para revisão, não correção definitiva.
