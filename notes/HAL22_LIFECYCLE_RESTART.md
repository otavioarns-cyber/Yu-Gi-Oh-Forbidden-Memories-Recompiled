# HAL-22 — ordem de carregamento e reinício controlado

**A mitigação funciona sob um protocolo explícito de ordem segura e reinício completo. `restart: true` impede a primeira ativação tardia insegura do AI nos caminhos executados, mas não impõe uma ordem segura. O manager ainda aceita uma ordem invertida — inclusive depois de reiniciar. Nenhuma correção definitiva foi aplicada.**

Branch independente `test/hal-22-lifecycle-restart`, derivada da integração oficial `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`. Núcleo oficial `aceac3782902746f35ddb0148809450933327cb3`, API 24. [PR #16 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/16), direcionado à integração, sem merge.

Continuidade lida: `AGENTS.md`, [diagnóstico PR #15](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/15) no commit `9d21f19a8edf9c12ce5e2de419a7ef8037c6cf50`, [primeira integração PR #14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14), base/inventário HAL-20 e tarefas [HAL-22](https://linear.app/half-programer/issue/HAL-22), [HAL-20](https://linear.app/half-programer/issue/HAL-20), [HAL-6](https://linear.app/half-programer/issue/HAL-6), [HAL-18](https://linear.app/half-programer/issue/HAL-18). O relatório cobre apenas I2, com AI oficial e Dual local. Não incorpora outros MODs ou pendências I1/I3/F1.

## Respostas A–E

**A. Podemos evitar I2 sem modificar código de produção, apenas com ordem e reinício controlados?**

**Sim, nos caminhos executados, desde que o protocolo seja cumprido:** o novo processo precisa carregar AI HARD MODE antes de Dual Play e nenhuma primeira aplicação tardia insegura pode chegar ao duelo. Com os manifests originais, a API permite a aplicação tardia; portanto esse controle é operacional, não uma proteção automática já existente. Reiniciar após corrigir/persistir a ordem reconstruiu a cadeia segura, independentemente da história anterior de ativação testada.

Dos 19 processos pós-reinício, 15 executaram a suíte com ambos ativos sem falhas I2; dois ficaram corretamente só com Dual, por preferência OFF/ativação ainda pendente; dois reconstruíram I2 porque a ordem invertida havia sido salva deliberadamente. Reiniciar sozinho não corrige uma configuração de ordem errada.

Para o protocolo, usar ranks **distintos** que resolvam H antes de D, confirmar a ordem efetiva na inicialização e concluir o reinício antes de qualquer ação de duelo. Não confiar em empate, nome da pasta, preferência ON ou botão de novo duelo. Com o manifest original, salvar uma primeira ativação e continuar jogando antes de reiniciar é inseguro; cancelar/fracassar o reinício exige manter o duelo suspenso e relançar o processo manualmente. Isso não é autorização para instalar uma solução definitiva.

**B. O campo `restart: true` é suficiente? Qual o impacto para o usuário?**

**É suficiente para adiar a primeira ativação tardia nas três entradas exercitadas; não é suficiente para eliminar I2 sozinho.** `Mods_Apply`, `Mods_SetEnabled` e uma segunda chamada a `Mods_Load` (recarga) conservaram `enabled=1`, `active=0`, biblioteca ainda não inicializada. Só o processo novo aplicou o AI. A ordem H→D foi então respeitada quando configurada.

Na janela real de MODs, a primeira aplicação pede confirmação de reinício; cancelar não altera a preferência nem inicializa a biblioteca. Confirmar salva a preferência e chama a plataforma para reiniciar. O impacto observado é que ON/OFF do AI deixa de ser imediato: pedir OFF em um AI ativo mantém `active=1` até o processo terminar. A UI informa que progresso não salvo será perdido. Pela leitura de `needs_restart`, alterações nas opções de um MOD com esse campo também pedem reinício; nenhuma opção estratégica foi alterada neste experimento.

O candidato conserva os 17 defaults, `preserve_hand=1`, `deck_access=0` e as decisões iniciais examinadas. Existe somente em cópias temporárias, com o mesmo objeto AI. **Não foi tornado permanente no manifest do repositório.**

**C. O manager ainda permite a cadeia insegura após reinício?**

**Sim, com duas reproduções independentes:**

- Ranks salvos H=1, D=0: uma mudança pela rotina real da janela pediu reinício, salvou a ordem e, no processo seguinte, carregou D antes de H. O gate não executou, mesmo no candidato com `restart: true`.
- Ranks empatados H=0, D=0: inverter apenas os nomes/ordem de scan das pastas temporárias mudou qual MOD foi registrado primeiro. Com Dual descoberto primeiro, o processo iniciou inseguro. Não se alterou o manifest Dual.

O manager valida manifests/dependências gerais, mas não conhece o contrato específico de I2. A lista **resolvida**, a preferência **salva** e a cadeia **ativa** são estados distintos. Corrigir o rank salvo no processo inseguro não reordena os registros que já existem. Um novo duelo também não os reordena. O teste de falha de restart observou a mensagem de relançamento manual e a cadeia antiga ainda presente; isso é um processo que **não reiniciou**, não uma falha de reconstrução de um processo novo.

**D. Se a mitigação não resolver totalmente, qual a menor mudança necessária e onde?**

Para um ambiente operado pelo protocolo, **não há necessidade demonstrada de mudar o organizador de hooks**: carga inicial correta + reinício funcionaram, preservando os dois materializadores e todos os defaults.

Para tornar a proteção automática, a menor proposta é **validar e exigir a ordem do perfil conjunto antes da aplicação/inicialização**, mantendo `Hooks_Relink` e o contrato de ordem de registro intactos. Se o campo de reinício for aprovado depois, ele cobre a primeira aplicação tardia; falta impedir que ranks/empates reintroduzam D→H no startup.

O ponto de aplicação seria uma regra **opt-in do perfil de integração**, compartilhada pelos caminhos de validação/aplicação (`src/pc/mods/manager.c`, `Mods_Validate`/`Mods_Apply`) e pelo startup (`src/pc/mods/mods.c`, `Mods_Load`, após resolver a ordem e antes de inicializar bibliotecas). Seu contrato proposto:

1. Quando o perfil quiser os dois ativos, exigir H antes de D na ordem efetiva; não inferir segurança de prioridade isolada ou empate.
2. Recusar a composição insegura com diagnóstico visível, preservando preferências e sem desligar silenciosamente o AI, mudar sua dificuldade ou trocar quotas.
3. Se o processo precisar de reinício para concretizar a composição, não apresentar a alteração como já ativa. Tratar cancelamento e falha de restart de forma explícita antes de autorizar novo duelo.

Um teste apenas em `Mods_Validate` seria insuficiente: `Mods_Load` também inicializa bibliotecas diretamente. Uma prioridade menor apenas no manifest AI também seria insuficiente: ranks salvos a sobrepõem. Não se propõe adicionar `before` a um JSON nesta base — esse campo não é suportado — nem editar o manifest Dual.

Essa proposta ainda exige definir a interface do perfil e o tratamento de uma inicialização recusada; não é um patch implementado ou uma decisão de UX aprovada. Reordenar hooks ao vivo seria uma intervenção maior e **não é necessário para a mitigação demonstrada**. Nenhuma chamada a `previous_swap` foi acrescentada, antes ou depois de materializar.

**E. Quais regressões executar antes da correção definitiva?**

- Manter esta matriz completa, incluindo os controles atualmente inseguros. Uma proteção automática deve recusá-los antes do duelo, não esconder seus FAILs ou executar um segundo materializador.
- Startup com ambos ON, cada MOD sozinho, primeira ativação de cada um por Apply/API/recarga, ambos já carregados OFF/ON e preferências pendentes de reinício. Testar importação/substituição de pacote e overrides de ambiente em escopo adicional: não foram exercitados aqui.
- Ranks seguros, invertidos, empatados e sobreposição a prioridades do manifest; ordem de scan; validação tanto no startup como nas transações. Conferir rollback se uma ordem for recusada ou a gravação de settings falhar.
- Janela real da plataforma: confirmação/cancelamento, execução efetiva de `Platform_RestartGame`, falha ao relançar, persistência do perfil e aviso de progresso não salvo. O callback da plataforma foi simulado neste host; a próxima validação Windows precisa exercitá-lo de verdade.
- Support/Monster do controller, Ritual no bônus, cancelamento, snapshot autoritativo, categoria contabilizada e instâncias distintas. Uma ação aceita deve executar um worker; rejeitada, zero; uma troca por instância que precise sair do deck. Nenhuma mudança nas escolhas estratégicas ou nos defaults.
- Duelo completo Windows i386/API 24 com catálogo/decks reais e imagem legal local, seguido de regressão com os demais MODs. Não promover a mitigação a compatibilidade integrada com base neste host. Savestates antigos continuam sem presunção de compatibilidade.

## Execução e evidências

**22 casos, 41 processos ELF32, 19 reinícios reais entre processos do harness.** Foram executados o manager completo e as rotinas de staging/confirmação/Apply da janela, além dos cenários próprios herdados do PR #15. Resultado: **1.618 PASS, 158 FAIL, 36 BLOCKED e 7 SKIP**, mais **1 PASS entre perfis**.

As falhas são esperadas nos controles e contraexemplos ainda sem correção: nove processos com cadeia insegura repetem 135 invariantes de controller/gate do PR #15 e 23 probes de cadeia. Não são 158 bugs distintos. Os 36 BLOCKED são limpezas de rejeição que não aconteceu (quatro por suíte insegura). Os sete SKIP são suítes de controller com ambos os MODs não ativos naquele processo; as combinações OFF tiveram seus caminhos do materializador testados por probes. Todas as verificações de política de ciclo de vida executadas passaram; não houve ERROR ou execução incompleta.

**39 preflights e 39 verificações pós-execução passaram.** Os hashes de objetos, manifests originais, SDK e fontes oficiais permaneceram iguais. Os 34 processos que executaram as escolhas iniciais do planner produziram os mesmos pares ID/categoria: Support 344 e Monster 35. As entradas de bônus variam nos controles inseguros por causa da contabilização errada já diagnosticada, não por mudança no algoritmo.

- [Matriz resumida por caso/fase](hal-22-lifecycle/MATRIZ.md) · [CSV por verificação](hal-22-lifecycle/resultados/MATRIZ.csv)
- [Resultados, comandos, PIDs, settings e logs](hal-22-lifecycle/resultados/RESULTADOS.json)
- [Insumos e proveniência](hal-22-lifecycle/INPUTS.json)
- [Runner, compilação e limites](../tests/hal22-lifecycle/README.md)

Exemplos:

| Reprodução | Evidência | Resultado |
|---|---|---|
| Primeira aplicação AI original, Dual já ativo | [Apply, processo 0](hal-22-lifecycle/resultados/late-ai-apply-original-p0.log) | H ativo imediatamente; gate contornado |
| Mesma história após reinício, ranks corretos | [Processo 1](hal-22-lifecycle/resultados/late-ai-apply-original-p1.log) | Dual→AI, contabilização correta |
| Candidato impede primeira aplicação | [Apply com restart](hal-22-lifecycle/resultados/late-ai-apply-restart-p0.log) | H desejado ON, ativo OFF, biblioteca ainda sem init |
| Janela confirma e solicita restart | [Window candidato](hal-22-lifecycle/resultados/window-first-ai-restart-p0.log) | Primeiro Apply confirma; segundo salva e solicita restart |
| Inversão de ranks persiste após reiniciar | [Processo novo candidato](hal-22-lifecycle/resultados/rank-invert-restart-p1.log) | D→H na carga, execução somente AI no swap |
| Empate depende da descoberta | [Dual descoberto primeiro](hal-22-lifecycle/resultados/equal-rank-dual-first-p0.log) | I2 mesmo com restart ligado |
| Falha ao reiniciar | [Processo antigo](hal-22-lifecycle/resultados/restart-fails-repair-p0.log) · [relançamento](hal-22-lifecycle/resultados/restart-fails-repair-p1.log) | Antigo conserva cadeia; novo corrige com ranks salvos seguros |

## Método e limites

O harness liga `mods.c`, `manager.c`, `hooks.c`, `settings.c`, `paths.c`, loader e eventos oficiais **sem edições**. Carregamento de biblioteca, host API e registros são reais. A ponte da janela inclui o fonte oficial sem alterações e acessa as rotinas internas para preparar/executar a transação; não é automação visual ou teste de mouse. O retorno da plataforma ao pedido de restart é observado por stub; o runner faz o reinício em outro processo, com PID diferente, lendo o mesmo arquivo. As observações após um restart solicitado com sucesso mantêm o processo antigo artificialmente parado no teste; não representam gameplay real posterior a um restart bem-sucedido.

A cadeia estrutural segura é Dual→AI→motor. Com AI ativo e `preserve_hand=1`, a execução para no worker AI: **não se chama o materializador nativo novamente**. Com AI OFF, o Dual delega ao worker nativo. Os probes e a suíte verificaram uma execução e conservação das instâncias. O ciclo de novo duelo executa `Mods_Reset` e o hook de init do Dual; o corpo completo do init do jogo é modelo e não há reinício do processo nessa transição.

Controller/snapshot/planner/materializador selecionados continuam usando os objetos nativos oficiais já existentes. RAM, catálogo, renderização e a ponte simples de efeito/placement→campo são parciais, como no PR #15. Não há imagem de jogo, bytecode retail completo, partida Windows ou certificação dos decks reais. As seleções adversariais de categorias gastas, Ritual e plano vazio permanecem injeções após filtragem e não são escolhas atribuídas à IA. O estado privado 50/v10 do Dual é somente observado.

Os fontes decisivos são `mods.c:4202–4205` (ativar/relink), `4397–4524` (startup versus recarga), `4555–4584` (preferência versus aplicação), `manager.c:112–149` (ordem por dependências/ranks), `177–239` (Apply) e `mods_window.c:992–1084` (decisão, confirmação e pedido de restart). Os logs registram o que efetivamente executou; a interpretação não depende só da leitura estática dessas rotinas.

A compilação do harness inicialmente precisou de `-lpthread`, dependência do host oficial. O comando final inclui a biblioteca e passou; o log final preserva warnings preexistentes dos headers de layout/PsyQ. Não houve incompatibilidade nova de SDK ou recompilação dos MODs. O runner retorna 1 para manter os FAILs diagnósticos visíveis.

## Publicação e preservação

Foram acrescentados somente `tests/hal22-lifecycle/`, este relatório, `notes/hal-22-lifecycle/` e o ponto de continuidade em `AGENTS.md`. Nenhum fonte de produção, configuração estratégica, manifest instalável, SDK ou objeto foi alterado. A experiência `restart: true` é gerada pelo runner em cópia temporária e removida ao terminar.

AI oficial local: SHA-256 `9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc`. Dual local: `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`, loader `3028c68a`, mesma recompilação API 24 do PR #10. Nenhum fonte, manifest ou binário Dual foi publicado. Não há assets, ROM, saves pessoais ou executáveis novos na entrega.

As branches anteriores e os PRs #8–#15 são preservados; esta branch não promove alterações à integração/master. [Índice público e hashes](hal-22-lifecycle/PUBLICATION_FILES.json) · [Refs preservadas](hal-22-lifecycle/REFS_PRESERVADAS.json). PR em rascunho, sem merge. A decisão sobre adotar o campo permanentemente ou implementar uma validação de perfil fica para autorização separada após revisão destes resultados.
