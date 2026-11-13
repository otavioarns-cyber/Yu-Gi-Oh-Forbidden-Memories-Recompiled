# HAL-24 — Ritual Duelists × Dual Play: materialização e quotas

**I1 deixou de ser apenas o pending artificial do PR14: o planner real do RD criou uma ação legal de Support, seu materializador pulou o gate Dual e a contabilidade registrou Monster.** A primeira divergência aparece no controller nativo, sem injetar pending, plano ou quotas e antes da ponte de conclusão da ação. Na sequência parcialmente modelada, o bônus permitiu consumir outro Support. **Isso é evidência funcional no motor parcial, não gameplay Windows comprovado.**

Branch `test/hal-24-ritual-duelists-dual-gate`, derivada diretamente da integração `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`; oficial `aceac3782902746f35ddb0148809450933327cb3`, API24. [HAL-24 no Linear](https://linear.app/half-programer/issue/HAL-24). Nenhuma alteração de produção, correção funcional, alteração de terceiro ou merge.

Continuidade lida: AGENTS.md da entrega anterior, HAL-24 no Linear e relatórios/matrizes dos [PR14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14), [PR16](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/16), [PR17](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/17) e [PR18](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/18). O guard do PR17 não foi incorporado. A HAL23 não é usada como janela de transição: nenhum toggle é inserido entre planner e swap.

## Respostas A–F

**A. É possível produzir I1 com planner e controller reais, sem injetar pending?**

**Sim para o bypass de materialização e a contabilidade errada. Não foi produzido um pending novo de ritual proibido durante o bônus.** A nova reprodução usa uma primeira ação normal, permitida, cuja categoria precisa ser capturada antes da troca:

1. O0: registro H→D→RD, cadeia externa RD→D→H. AI e Dual ON desde a inicialização, defaults estratégicos intactos. Mão sintética535,99,344,111,112, instâncias distintas; magia 680 no próximo registro não comprado. Receita original680/6/47/571/705.
2. Controller real monta snapshot e chama o planner oficial AI; o planner RD real decide buscar 680 e cria MAGIC pending com fonte lógica16 e reposição física1. **Zero chamadas ao stub de conclusão da VM nesse exemplo.** Não se fornece seleção de teste.
3. No swap, somente RD executa: uma troca offscreen, seleção16 vira12, pending consumido. Gate Dual=0 e worker AI=0 nesse hook. O controller remove a instância física680 (bloco45) e chega à fase7 com quatro cartas na mão.
4. Callback real de mão do Dual termina com `used=1,last_plan=1`, embora a categoria efetiva seja Support=2. A seleção inicial/cursor Monster permaneceu como categoria autoritativa porque o gate não capturou a seleção final do planner.
5. A ponte explícita de conclusão placement→entrada em campo deixa os callbacks reais de frame do Dual concederem bônus e reconstruírem a mão. O filtro oculta Monster, pois o ledger diz que Monster foi gasto;344 permanece disponível. O planner oficial escolhe 344, e o controller consome outro Support. Gate executa nessa segunda ação, mas autoriza com base na primeira categoria errada.

[Reprodução principal AI ON/O0](hal-24/resultados/O0-h1-d1-v5-none-restart0-absent.log) · [controle O1, mesmos insumos](hal-24/resultados/O1-h1-d1-v5-none-restart0-absent.log) · [AI OFF parcial](hal-24/resultados/O0-h0-d1-v5-none-restart0-absent.log).

O log separa `PLAN`, `ACTION_INSTANCE`, `ACTION` e `LEDGER`: selecionar logicamente16 não é consumir a posição16 da mão. A troca coloca uma instância existente no slot físico1; a categoria que deve ser contabilizada é a da carta680 efetivamente selecionada, não a do cursor antigo nem a da carta substituída.

O cenário inicial é sintético; decks/catálogo reais e ocorrência numa partida ainda precisam ser examinados. Com AI OFF, o stub retorna apenas status1, sem escrever seleção, e o RD decide sozinho a partir do snapshot. Essa variante não executa bytecode retail. O exemplo AI ON dispensa esse stub, mas continua usando catálogo, render e serviços auxiliares parciais.

**B. O Dual já impede a jogada ilegal numa etapa anterior?**

**Impede o ritual/categoria proibida na tabela lógica de bônus examinada, mas isso não garante contabilidade correta da ação anterior.** Nos 110 snapshots de bônus, seu filtro removeu os rituais e as categorias que o ledger considerava gastas. Nenhum dos planners criou pending novo no bônus nessa matriz. O contraponto do PR14 foi preservado: não se pode transformar a antiga injeção de pending em prova de ritual ilegal natural no bônus.

A nova falha antecede esse filtro: a magia ritual é Support numa ação normal, e a falta do snapshot do gate deixa o ledger como Monster. Assim, a filtragem opera corretamente sobre uma entrada de estado incorreta e permite outro Support. **O gate tem responsabilidade de capturar a categoria definitiva também na ação normal**, além de rejeitar ações bônus incompatíveis. Uma validação de disponibilidade anterior não substitui essa captura.

O1, com Dual externo, recebe a seleção lógica16 antes do RD, registra Support=2, permite a troca única do RD e depois concede o bônus Monster correto. Os18 perfis de carga inicial O1 com Dual ON passaram 200 verificações, sem FAIL. Os demais casos seguros O1 passaram; primeira carga tardia de AI original pode colocar AI externo e pertence a I2.

**C. Existe violação de quotas, consumo incorreto ou duplicação reproduzida?**

No recorte parcial, **sim para categoria contabilizada errada e repetição de Support**:

- 22 processos I1 registraram a primeira categoria incorretamente; desses,19 completaram uma segunda ação Support. São 60 FAIL diagnósticos:22 de primeira contabilidade +19 de categoria repetida +19 de ledger final. Não são 60 bugs distintos.
- 16 dos 22 processos I1 tiveram zero chamadas ao stub da VM;15 desses completaram a repetição de Support usando planners reais. O primeiro erro não depende da conclusão modelada de efeito/placement. A concessão do bônus e a segunda ação dependem dessa ponte, que não simula um duelo inteiro.
- Não houve duplicação de materialização:150 verificações de conservação da primeira ação e 110 do bônus passaram, sempre no máximo uma troca offscreen por ação. Os80 blocos de dados permaneceram únicos; os 150 checks dos registros não selecionados passaram. Duas cópias do mesmo ID foram tratadas como instâncias distintas.
- A primeira carta consumida correspondeu à seleção final do RD; a falha é a quota creditada, não um fallback antigo trocando seleção. Não se reproduz nem corrige HAL23 aqui. Materiais não selecionados não foram removidos; **consumo completo de três tributos não foi executado**.
- Cinco rejeições observadas de seleção vazia preservaram mão, deck e contagem de trocas antes da limpeza. São controles com predecessor retail parcial, não cancelamento humano validado na UI. Outros caminhos de rollback/efeitos não são certificados.

Separadamente,27 processos tiveram38 FAIL com worker AI externo, sem worker RD na materialização: **I2/HAL22**, fora da contagem I1. Alguns repetiram Monster/consumiram ritual em bônus no host parcial; não são evidência de materialização própria do RD. [Classificação por trace](hal-24/CLASSIFICACAO.json) mantém essa separação.

Os controles históricos sem modificação do PR14 reproduziram os quatro FAIL de gate sob pending/bônus artificialmente semeados. A saída original inclui ainda um FAIL de pending remanescente do AI (I3); ele é preservado como evidência histórica e não entra nas conclusões novas HAL24.

**D. Ordem estável é suficiente nos cenários executados?**

**Sim, no recorte testado, quando a ordem efetiva inicial é H→RD→Dual e a cadeia continua Dual→RD→AI/motor.** O Dual valida/captura antes de quem materializa; o RD faz sua própria troca uma vez; quando não tem pending, delega sem um segundo materializador posterior. Isso não é garantia para qualquer outro MOD futuro.

O1 de carga inicial passou. Considerando lifecycle,49 processos O1/455 PASS ficaram sem FAIL; o único processo O1 restante foi primeira carga tardia de AI original antes de reiniciar, quando o registro tardio põe AI externo (I2). Não basta olhar o rank salvo.

Primeira carga tardia de Dual registrou-o depois dos demais: seis processos passaram 66 checks. Relançar com ranks O0/O2 salvos reconstruiu as cadeias anteriores e voltou a expor I1/I2: nesses seis processos pós-relançamento houve58 PASS/8 FAIL. OFF/ON de biblioteca já registrada conserva a ordem de registros; não a repara. O RD original já exige reinício: primeira solicitação tardia ON ficou `wanted=1,active=0`; no processo novo ele inicializou na posição resolvida.

**O candidato temporário AI restart:true não resolve I1 sozinho:** impede a primeira ativação tardia do AI, mas RD externo continua podendo materializar antes do Dual, inclusive com AI OFF. Reiniciar com ordem O0/O2 não converte a cadeia em O1. Nenhum manifest de produção foi alterado. Não foi testado relançamento gráfico Windows, e esses resultados não autorizam tornar o campo permanente.

**E. Se necessária, qual a menor alteração própria sem modificar Dual?**

A evidência nova justifica tratar a passagem da categoria definitiva pelo gate como requisito de integração. A menor medida demonstrada é **validar a ordem efetiva do perfil e iniciar com H→RD→Dual**, concluindo os reinícios necessários. Não é necessário mudar estratégias, receitas, quotas ou o corpo de materialização para obter o comportamento correto nos cenários O1.

Para uma proteção automática, a proposta mais localizada é estender em tarefa autorizada a validação opt-in do perfil para exigir RD antes de Dual na inicialização e Dual externo ao worker RD quando ambos estão ativos, além do requisito AI/Dual já investigado. Consultar a ordem registrada e recusar a composição insegura antes de entrar no duelo, com mensagem e preferências preservadas; não reordenar globalmente hooks. **É proposta de validação na integração sob nosso controle, não um patch aplicado neste PR.** Não promover o protótipo PR17 automaticamente.

Não identifiquei um patch menor comprovadamente seguro **apenas dentro do worker RD** que conserve sua escolha de reposição física e também obrigue uma chamada a Dual já interno sem executar outro worker. Chamar `previous_swap` antes pode materializar pelo nativo/AI e perder a reposição protegida do RD; chamar depois pode duplicar a troca. Adicionar `before` ao manifest não resolve: a base não oferece esse contrato e ranks salvos podem sobrepor prioridades.

Se a integração precisar admitir outras ordens, será necessário desenhar uma fase explícita de validação/snapshot antes da única materialização, com responsabilidade de autorização mantida no Dual. Isso exige coordenação/contrato aprovado com o autor ou desenho autorizado do host; não se cria um provider de terceiro, não se acessa o bloco privado de quotas como solução e não se modifica Dual. A evidência atual não autoriza um contorno silencioso.

**F. Quais testes reais Windows ainda faltam?**

- Windows i386/v0.3.1-preview.2/API24 com imagem legal local, catálogo/decks reais e bytecode retail: alcançar a busca680 por RD em turno normal e observar a categoria definitiva antes/depois do swap. Variar cursor inicial, categoria da carta substituída e cópias do mesmo ID.
- Resolver a primeira ação integralmente, sem ponte de fase: animação/set da magia, concessão de bônus pelo fluxo normal, próxima seleção, efeito de344 e fim do turno. Conferir que Support+Support não ocorre em O1 e se ocorre numa ordem insegura. Não provocar a condição escrevendo quotas.
- AI OFF/ON, O0/O1/O2, primeira carga de cada biblioteca, OFF/ON de bibliotecas já carregadas, Apply/API/recarga, ranks persistidos e relançamento real, incluindo cancelamento/falha. Confirmar a cadeia efetiva; não confundir checkbox ON com estado ativo.
- Monster, Support e ritual no normal/bônus; modos MAGIC/FIELD/HOLD do RD quando alcançáveis; empate/fusão/equipamento com categoria definitiva; múltiplas instâncias e rollback/cancelamento real. Outros MODs só em etapa integrada autorizada.
- Field/Hand/Both com providers reais e três materiais distintos: seleção, invocação, consumo apenas dos materiais autorizados, identidade de resultado e preservação das cartas não selecionadas. Aqui os 18 layouts/provider passaram as consultas, mas não a invocação completa.
- Reset, novo duelo e save/load reais, quotas por turno/side e controles humanos, sem pressupor compatibilidade de savestates antigos. Tratar HAL22, HAL23 e F1 em seus escopos, sem embutir correções neste diagnóstico.

## Matriz, proveniência e limites

[Matriz por processo](hal-24/MATRIZ.md) · [CSV](hal-24/resultados/MATRIZ.csv) · [resultados/logs](hal-24/resultados/RESULTADOS.json) · [reprodução](../tests/hal24/README.md) · [insumos e hashes](hal-24/INPUTS.json) · [controles PR14](hal-24/controles-pr14/RESULTADOS.json).

| Coorte principal disjunta | Processos | PASS | FAIL |
|---|---:|---:|---:|
| Sem falha nas invariantes exercitadas |101|858|0|
| RD/I1 — worker RD com pending real e gate ausente |22|164|60|
| AI/I2 externo — separado da HAL24 |27|262|38|
| Total |150|1284|98|

Sem execução incompleta. 43 preflights e 43 verificações pós-execução passaram. Só harness e observer próprios foram compilados; os objetos dos MODs e dez unidades nativas anteriores foram reutilizados. Avisos de offset dos headers oficiais permaneceram nos logs, sem falha de build. Nenhum FAIL foi “corrigido” na fixture desligando defaults, chamando manualmente o gate ou apagando pending. Um acesso diagnóstico ao estado de Dual OFF foi ajustado durante a preparação para registrar biblioteca ausente; nenhuma mudança de produção decorreu disso.

AI SHA256 `9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc`; Dual `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`; RD `8df2e8b6e11bea0dd771bc5005848081191e7c468f5ee588976a6633129bb17e`; Melhoria PR9 opcional `c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0`. Drop Missing Cards1.0 oficial ficou ON; probabilidades de drops não foram testadas/alteradas.

As ordens O0/O1/O2 preservam a relação relativa dos três integrantes nos testes anteriores, reduzindo a cadeia principal. Melhoria PR9 fica antes e aparece somente nos layouts Field/Hand/Both, com min_field3/0/1 em settings temporários, sem mudar use_deck ou outros defaults. O provider real sempre informa fontes Field|Hand; o layout “Hand” não inventa uma política de somente mão. Os controles históricos usam os quatro pacotes exatos do PR14; nenhum arquivo deles foi alterado ou copiado como versão nova.

As 150 execuções são 132 perfis mais 18 processos novos lendo settings salvos. Processo novo reinicia a fixture, não retoma um duelo. Fases de setup/planejamento/materialização/seleção física e callbacks de contabilidade são reais; RAM/catálogo, renderer, serviços auxiliares e ponte efeito/placement→campo são modelos. Não há imagem, duelo completo, UI/áudio/controles Windows, save real ou compatibilidade integrada certificada.

Publicação limitada a testes próprios, relatório/matrizes/logs/proveniência e AGENTS.md. **Zero diff em src/mods/config/tools**. Nenhum fonte, manifest ou binário privado Dual, asset de MOD, ROM, segredo ou save pessoal foi publicado. [Índice](hal-24/PUBLICATION_FILES.json) e [refs preservadas](hal-24/REFS_PRESERVADAS.json). Master, integração, PRs e branches anteriores permanecem intactos. Não aprovar promoção de uma composição sem validar a ordem efetiva e executar as regressões reais; nenhuma correção ou merge foi feito.
