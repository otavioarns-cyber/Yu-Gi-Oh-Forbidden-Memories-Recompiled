# HAL-20 — Primeira integração controlada / HAL-6 e HAL-18

## Resultado e decisão

**Ambiente experimental publicado com os quatro pacotes próprios preservados. Foram executados 25 perfis em 28 execuções de ELF: 480 verificações PASS e 29 ocorrências FAIL, quatro bloqueios e quatro skips globais. Não aprovar a integração para lançamento.** As ocorrências repetem F1 e três condições de interação; não são 29 bugs distintos. Nenhuma correção de produção foi aplicada.

Branch: `test/hal-20-integracao-inicial`, derivada diretamente de `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`. Base oficial `aceac3782902746f35ddb0148809450933327cb3`, API 24. Os commits dos PRs não foram mesclados; suas árvores de MODs foram copiadas exatamente, com proveniência. Master, integração e PRs de origem permanecem separados.

Vínculos de aceite: [HAL-6 — hooks/globais/eventos/recursos](https://linear.app/half-programer/issue/HAL-6/aud-mapear-hooks-globais-eventos-e-recursos-compartilhados) e [HAL-18 — regressão isolada/integrada](https://linear.app/half-programer/issue/HAL-18/test-matriz-de-regressao-isolada-e-integrada-ai-hard-mode-dual-play). Foram consultados AGENTS.md, inventário e relatórios/entregas dos PRs [#8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8), [#9](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/9), [#10](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/10), [#11](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/11), [#12](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/12) e [#13](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/13). Nenhum status de issue foi alterado. A F1 está vinculada à [HAL-21](https://linear.app/half-programer/issue/HAL-21/bug-fusion-effects-encore-e-leases-continuam-ativos-apos-off), sem correção incorporada.

Evidências: [matriz resumida](hal-20/integracao-inicial/MATRIZ.md), [CSV incluindo bloqueios/skips](hal-20/integracao-inicial/MATRIZ.csv), [resultados/comandos/logs](hal-20/integracao-inicial/RESULTADOS.json), [113 hooks/recursos e referências por arquivo/linha](hal-20/integracao-inicial/RECURSOS.json), [insumos e hashes](hal-20/integracao-inicial/INPUTS.json), [integridade das árvores](hal-20/integracao-inicial/INTEGRIDADE.json), [perfis](hal-20/integracao-inicial/PERFIS.json), [runner e reproduções](../tests/hal20-integration/README.md). Os relatórios anteriores copiados continuam sendo resultados das etapas isoladas, não novos testes desta integração.

## Componentes e preservação

| Componente | Commit de origem | Objeto SHA-256 |
|---|---|---|
| Melhoria/Surrender #9 | `0d28d844c3b88b38ab0db71635959ddd223d8c14` | `c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0` |
| Ritual Effects #11 | `2cf0de31e88e9f1c058b3747b2b2add3db0e347c` | `c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1` |
| Fusion Effects #12 | `907ed98cfb763a96f5aa1ad6369f809cceca5f1c` | `7e50372742ebd04090a907aa222f90d339d3122edf2fb06a589e297926c4f6c8` |
| Ritual Duelists #13 | `9a363f28f6dbfe779b1b901ddecab74ab1be57e9` | `8df2e8b6e11bea0dd771bc5005848081191e7c468f5ee588976a6633129bb17e` |
| AI HARD MODE oficial 0.1.0, local | Base preview.2/SDK Windows i386 | `9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc` |
| Dual Play 0.5.7, local | Candidato recompilado identificado no #10 | `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378` |

As quatro árvores próprias têm **468 arquivos** e os mesmos Git tree hashes dos respectivos commits; incluem fontes, manifests, configs, assets, objetos e testes/documentos anteriores. Não se editou nenhum arquivo dessas árvores. Não se recompilaram nem substituíram objetos. Novos testes ficam exclusivamente em tests/hal20-integration; novas evidências em notes/hal-20/integracao-inicial. Os manifests oficiais de AI e Drop Missing Cards também não mudaram.

Dual permanece fora da árvore Git: sem código, manifest ou binário de terceiro nesta publicação. Seus offsets de estado usados no teste vêm do bloco registrado de 50 bytes/v10 já auditado; o runner exige o hash específico. Esses dados privados não constituem API pública estável. Ausência de autorização de redistribuição continua limitando o empacotamento, mas não impediu os testes locais autorizados. Créditos/permissões dos assets próprios permanecem nos NOTICES-HAL20.md originais, incluindo a moeda doada por PeterIron e as artes do seletor cuja doação foi informada pelo usuário. Nenhum recurso novo foi extraído de disco/ROM e nenhum save pessoal foi usado ou publicado.

## Método, ordem e instrumentação

O harness executa os objetos ELF32 reais por object_loader.c oficial, com **hooks.c, events.c e json.c oficiais**. Um segundo programa usa **Mods_Order/Mods_Compatible de manager.c** contra os manifests reais. A ordem calculada foi comparada com os módulos realmente inicializados; os ponteiros previous foram conferidos após relink, e as entradas sintéticas do motor são chamadas para despachar os hooks. Não se chamou diretamente cada replacement para fingir uma cadeia integrada.

Engine, catálogo e render são sintéticos. Entre os serviços modelados estão um oráculo limitado de ritual de três materiais, spawns de objetos, remoção, stats/caps e duas consultas de fusão/equipamento. A sentinela de fusão 42 demonstra delegação, não uma receita adicionada ao jogo. O catálogo parcial não certifica validade estratégica ou todas as jogadas. Os thunks de rastreio preservam registradores/flags i386 e apenas registram/encaminham callbacks; os arquivos dos MODs continuam intactos. Não se declara que esses wrappers são adequados para o scanner de Time Wizard.

Execução Linux i386 por QEMU/sysroot, com GCC para as fixtures. Não houve gameplay Windows, disco, áudio ou controlador real. O runner e os logs registram comandos/cwd, saídas e códigos de retorno; seis compilações de harness/ordem em três lotes concluíram. A coleta termina não zero por causa dos FAIL diagnósticos, deliberadamente. Não são checks verdes de GitHub Actions; nenhum workflow foi alterado.

**O0**, ordem inicial de registro: Melhoria → AI → Dual → Fusion → Ritual Effects → Ritual Duelists. **O1**: Melhoria → AI → Fusion → Ritual Effects → Ritual Duelists → Dual. **O2**: Melhoria → Dual → Fusion → Ritual Duelists → AI → Ritual Effects. Componentes OFF/ausentes não entram na cadeia ativa; módulos presentes mas OFF são inicializados para testar providers e relink. Drop Missing Cards precede RD no perfil de dados. As três ordens respeitam after dos manifests; a alternativa com AI acima de RD ainda mantém Ritual Effects depois do AI.

Um hook registrado por último fica acima dos anteriores. Por exemplo, func_8001BAF0 em S4/AI ON/Dual ON:

| Ordem | Cadeia ativa, de fora para dentro |
|---|---|
| O0 | RD → Ritual Effects → Dual → AI → motor |
| O1 | Dual → RD → Ritual Effects → AI → motor |
| O2 | Ritual Effects → AI → RD → Dual → motor |

Os logs CHAIN registram a estrutura e TRACE registra o que realmente executou. Todas as combinações passaram pelo ciclo OFF/ON antes dos gates principais; eventos SAVE com todos OFF não executaram callbacks, e o relink restaurou as cadeias. O despacho de dano também percorreu os subscribers reais e conservou 8000 − 500 = 7500 na ausência de efeitos armados. **O gerenciador completo de aplicação/restart não foi executado**: toggles chamam callbacks/relink de diagnóstico diretamente. Manifests restart true foram preservados; isso não comprova que toda transição forçada pelo teste seja oferecida pela UI real no mesmo instante.

Foram observados 35 callbacks distintos de hook/evento nos traces. Registrar 113 hooks e 12 subscriptions não significa executá-los todos: os eventos diretamente despachados incluem DAMAGE e SAVE/LOAD; INPUT, REWARD, SLOT_SAVE/LOAD, FUSION e EQUIP não tiveram seu fluxo de controller completo exercitado nesta integração. Equipamentos/fusão tiveram consultas, forecast e wrappers específicos executados, conforme descrito abaixo. O inventário de recursos distingue referências estáticas de execução observada e não classifica toda referência como escrita.

| Recurso compartilhado | Efeito esperado / evidência |
|---|---|
| D_800EAE88 e gDuel_aActiveCards | Seleção/snapshot de IA compartilhados por materializadores; gates, origem e validade de slots medidos nos traces. Referências por arquivo/linha em RECURSOS.json. |
| gDuel_aDeckCardRecords, D_800E9FF0 e D_800EA030 | Instâncias físicas, mão/cursores e objetos; troca única, índices e consumo de materiais verificados em cenários limitados. |
| D_801A7AD8 e D_800907D8 | Identidade de campo, flags, posição e proveniência; commit ritual e remoção/reconstrução testados. |
| gDuel_awRitualData e oráculo público | Receita/consulta compartilhada por Melhoria, Ritual e RD; fonte do provider e fallback preservados, engine de ritual parcial. |
| gDuel_wSceneStateFlags / D_8009B1D5 | Fase/lado são condições de entrada comuns; setup e flags sintéticos, sem alegar controller real. |
| Blocos de estado / providers | Dual 50/v10 e RD 288/v4 registrados; demais estados transitórios não viram chunks por presunção. Mods_Find conserva providers OFF. |
| GPU/textos/overlay | Referências estáticas e hooks visuais inventariados; moedas Ritual usam banco 13 e seletor RD banco 14. Não comprova convivência gráfica, títulos ou ausência de conflito de sprites em jogo. |

## Matriz e resultados que passaram

S1 contém Melhoria; S2 acrescenta Ritual Effects; S3 acrescenta Fusion Effects; S4 acrescenta Ritual Duelists. As quatro combinações AI OFF/ON × Dual OFF/ON foram executadas em O0 para cada etapa. S4 também foi executada em O1 e O2. O contraponto S1/AI ON/Dual ON/O2 exclui Ritual/Fusion/RD e isola a fronteira AI/Dual. Três execuções adicionais verificaram a janela HM OFF no planejamento → ON na materialização nas ordens de S4 com Dual OFF.

**25 perfis, 509 verificações nomeadas: 480 PASS / 29 FAIL.** Parte dos checks valida ordem/integração a partir dos logs; não são 509 partidas ou testes independentes de gameplay. Há mais quatro BLOCKED e quatro SKIP globais, separados. A [matriz integral](hal-20/integracao-inicial/MATRIZ.md) identifica cada resultado e seu log.

Passaram nos cenários executados:

- Inicialização conjunta: até 113 hooks e 12 subscriptions dos seis objetos; oito providers (quatro Ritual, quatro Fusion), mais rules_v1 de Melhoria. Estado registrado por Dual (50/v10) e RD (288/v4). OFF remove callbacks da cadeia e conserva providers disponíveis para consulta, conforme o core oficial.
- Contratos reais: rules_v1 guardado quando Melhoria OFF; recipe_v1/intelligence de Ritual ativos/OFF; ausência de Ritual/Fusion nos estágios anteriores; consumidor de RD usando receita do Ritual e fallback Vanilla após OFF; conexão forecast_v2 Ritual↔Fusion com parceiro ativo/OFF. Isso não equivale a descarregar Melhoria da árvore ou testar todas as combinações de ABI; os testes isolados anteriores continuam referência para ABIs inválidas.
- Rituais: Vanilla com três tributos de campo; mistura de campo/mão Vanilla e modificada; probe de elegibilidade não consumindo mão; três objetos visuais correspondentes; consumo pelos hooks reais de Melhoria; preservação de outra cópia idêntica e monstro não selecionado. Receita de três Blue-Eyes exige três instâncias físicas: duas falham, três permitem. O motor/oráculo e a animação completa não são executados.
- Commit compartilhado de Skull Guardian: transição sintética 0x83 → 4 chama Ritual e RD uma vez, ambos marcam a mesma instância. O veto de fusão de RD bloqueia essa origem; trocar pela outra origem do mesmo ID volta a delegar (sentinela 42). Não se alterou a política conservadora por ID.
- Efeitos/instâncias: Fortress Whale marcado recebe stats 3000 contra cópia não marcada 1000 no cenário com dois aliados Fish. A cadeia Ritual/Fusion é real. Egotist consulta o provider real de Ritual: par sintético autorizado retorna 704 quando ativo e 0 quando OFF. Forecast Metalmorph entrega resultado 412/origem EA_METAL. Remover uma instância limpa seu lease e conserva o da outra. Reconstrução de carta gerada #30 preserva identidade nos dois lados, com spawn/render simulados.
- IA: seleções do pequeno tabuleiro pertencem a instâncias físicas/lógicas existentes e não repetem slots. Isso prova estrutura, não optimalidade ou legalidade completa de cada combinação no jogo. O controle positivo do planner de RD busca o ritual **680 de Rex** com Dual OFF e produz pending real; com setup Dual ON, esse slot é ocultado e a busca não ocorre no cenário de bônus.
- Save/load: eventos e VisitState reais restauram somente blocos registrados ativos; reset limpa efeitos transitórios dos MODs que não registram estado. Novo duelo limpa o bloco Dual. O deal real de RD produz 40 cartas Story, cap três, metadados de shuffle como permutação e limpa marcas rituais. Não se serializou BSS arbitrária como se fosse save oficial.

## Achados reproduzidos, condições e propostas não implementadas

### F1 — Fusion OFF, preexistente / HAL-21

**16 ocorrências FAIL**, nas etapas com Fusion, independentemente de AI/Dual. Provider Encore e leases continuam visíveis/responsivos após OFF: encore=1, lease=77, extended=77, forecast=0. O comportamento é o já registrado no #12/HAL-21, agora usando Mods_Find oficial durante a integração. Não é regressão criada pela cópia das árvores e não foi corrigido.

Evidência: [s3-h0-d0-o0.log](hal-20/integracao-inicial/s3-h0-d0-o0.log), caso F1-fusion-provider-OFF; resultados em todos os S3/S4. Prioridade **P1 antes de promoção**, política de lease no OFF/ON ainda requer decisão própria. Proposta continua sendo guardar respostas OFF sem alterar ABI/respostas ON e decidir separadamente esconder versus revogar leases; nenhuma adaptação de consumidores foi usada como contorno.

### I1 — RD pode não passar pelo gate Dual em sua própria materialização

**Quatro FAIL** no cenário com ação pendente explicitamente semeada, Dual ON e RD acima do Dual (O0 e O2 com AI OFF). Em O0, mesmo com AI ON, RD materializa o ritual no slot físico 0, pending vira 0, mas Dual e AI recebem zero chamadas nesse hook; o gate de rejeição de ritual de bônus não executa. Em O1, Dual fica acima, recebe uma chamada, rejeita, não move a carta e RD não executa.

Reprodução: `--stage 4 --hm 1 --dual 1 --order 0 --scenario rd-materialize`; [log](hal-20/integracao-inicial/s4-h1-d1-o0.log), caso RD-own-materialization-Dual-gate. Causa demonstrada no ramo MAGIC/FIELD de **mods/ritual-duelists/mod.c:711–774**: RD faz a troca e só delega no modo HOLD ou fallback; nenhum novo patch foi introduzido. O trace distingue a cadeia registrada da realmente executada.

**Limite essencial:** esse pending é injetado. Na sequência setup → planner → swap sem injeção, o setup real do Dual oculta o ritual de bônus e não houve ritual obtido nos cenários Dual ON. Com Dual OFF, o mesmo planner efetivamente obtém o ritual, validando o controle positivo. Portanto há um caminho que contorna o gate sob o estado testado; **não foi demonstrado que um turno completo do jogo produz esse estado ou que ocorre exploração de quota**.

Prioridade **P1 para investigação de alcance/ordem antes de integração**, não correção automática de mecânica. Dono do ramo: RD; contrato compartilhado de validação: integração/Dual. Proposta mínima a desenhar separadamente: validar o plano antes de qualquer troca e garantir uma única materialização, com decisão explícita de quem valida/consome. Acrescentar simplesmente previous_swap depois da troca poderia repetir a materialização e não é uma correção segura. Não acessar novo estado privado de Dual como solução, nem criar provider de terceiro sem autorização.

### I2 — materializador AI acima do Dual não chama seu gate

**Seis FAIL** de rejeição Monster/Support/Ritual em dois perfis: conjunto S4/AI ON/Dual ON/O2 e contraponto mínimo S1/AI ON/Dual ON/O2. No mínimo não há Ritual Effects, Fusion ou RD. Três planos adversariais de bônus são fornecidos ao hook: categoria Monster já gasta, Support já gasta e ritual em bônus. O AI materializa e não delega; Dual tem zero chamadas, rejected=0 e seleção física não zerada. Em O0/O1 o gate é alcançado e rejeita esses planos.

Evidências: [contraponto mínimo](hal-20/integracao-inicial/s1-h1-d1-o2.log), [conjunto](hal-20/integracao-inicial/s4-h1-d1-o2.log), casos quota-*. Dono do ramo: **AI oficial, mods/ai-hard-mode/ai_hard_mode.c:271–307**, que possui seu próprio swap. A licença/recompilação de Dual não foi usada para alterar o terceiro.

**Isto é uma falha reproduzida da expectativa de passagem pelo gate na fixture**, não demonstra uma segunda ação ilegal em gameplay. Estado de bônus, mão e seleção são sintéticos; o controller completo que constrói a mão de bônus e credita quotas não foi executado. Prioridade **P1 de compatibilidade/validação integrada**. Proposta: coordenar a validação antes do materializador ou uma ordem/contrato testado que preserve exatamente uma troca, sem mudar quotas ou dificuldade. Não propor uma chamada cega à cadeia após já mover cartas.

### I3 — pending de RD sobrevive quando outro materializador move a carta

Há dois FAIL no caso de pending injetado com AI ON em O2 (Dual OFF e ON). O AI move a carta, RD não executa, e pending continua 1. **Uma reprodução adicional dispensa a injeção:** com Dual OFF, o planner real de RD cria pending enquanto AI está OFF; ligar AI antes do swap em O2 faz a carta ser movida com RD callbacks=0, AI callbacks=1 e pending_after=1. O0/O1 consomem pending corretamente. São **três ocorrências FAIL** no total dessa condição.

Evidência principal: [transition-s4-h1-d0-o2.log](hal-20/integracao-inicial/transition-s4-h1-d0-o2.log), caso RD-pending-after-HM-ON-transition. Controles [O0](hal-20/integracao-inicial/transition-s4-h1-d0-o0.log) e [O1](hal-20/integracao-inicial/transition-s4-h1-d0-o1.log). O teste chama o planner e a cadeia reais; somente a ativação do AI é alterada entre os dois passos. Reset de RD limpa a pendência.

Dono/causa provável: **interação entre deck_pull de RD e a propriedade da materialização pelo AI**; não há notificação transacional quando a cadeia superior consome a ação. Isso não prova duplicação posterior de carta ou que a UI permite aplicar o toggle exatamente entre esses passos do motor; o teste exerce a janela de lifecycle explicitamente. Prioridade **P1 para coordenar OFF/ON/transações; impacto de gameplay pendente**. Proposta mínima: definir invalidação/consumo do pending quando a materialização é feita por outra camada, ou aplicar mudanças de planner apenas em fronteira segura, preservando a seleção física e sem executar a troca duas vezes. Não limpar estado arbitrariamente nem alterar a regra de busca sem autorização específica.

## Drop Missing Cards, estado e bloqueios separados

Drop Missing Cards 1.0 está ON nos 25 perfis; o teste oficial de ordem confirma que precede RD. Seu default enabled false na base permanece intacto. Desligá-lo no teste de compatibilidade não invalida RD: after ordena os presentes/ativos, não exige presença/ativação. Essa distinção não elimina o pré-requisito funcional de balanceamento descrito pelo autor.

**Balanceamento integral de drops está BLOCKED nesta entrega.** O programa de ordem lê manifests e flags; o harness ELF não aplica a composição completa de Tables_PoolFor com catálogo/tabelas do disco. O deal de 40 cartas usa os pools próprios de RD e não comprova probabilidades finais de drops, fontes Blue-Eyes ou apresentação do Droplist com o ecossistema completo. Não há resultado de balanceamento apresentado como PASS apenas porque a flag está ON.

| Cenário | Status / razão |
|---|---|
| Time Wizard | BLOCKED: núcleo-fonte/proveniência pendentes. Não carregado, sem adaptação de assinaturas/ponte Dual. |
| Adaptador AI 0.1.1 effect-aware | BLOCKED: pacote ausente. Foi usado exclusivamente AI oficial 0.1.0; seus números de versão iguais a variantes antigas não justificam misturá-las. |
| Droplist 1.4.0 externo | BLOCKED: pacote ausente. Nenhum visualizador interno reintroduzido; Triangle/UI não testados. |
| Balanceamento final com Drop Missing Cards | BLOCKED: composição integral/catálogo/disco não exercitados. Perfil e ordem verificados, sem probabilidades certificadas. |
| Gameplay Windows, áudio, render/HD/CRT e dois controles | SKIP: nenhuma imagem/runtime de partida utilizados. QEMU aqui executa fixtures Linux, não o jogo Windows. |
| Loop completo de bônus/quota/award/turno | SKIP: gates/callbacks e estado parcial executados; construção/consumo completo da ação no controller não simulado. |
| Savestates antigos | SKIP: nenhuma promessa de compatibilidade; assinatura/hash/versões do host não foram contornados. |
| Todos os efeitos e ecossistema completo | SKIP: cenários selecionados não equivalem a toda a matriz de efeitos, animações, títulos e estratégias. |

Ritual e Fusion não registram blocos nativos no host; seus resets limpam proveniência transitória. O round-trip testado não restaura esses efeitos como se tivessem sido serializados. Melhoria também limpa referências temporárias da ponte ritual. Não classificar essa limitação histórica como regressão da montagem experimental nem criar persistência de efeito sem decisão aprovada.

## Calibração das fixtures e limites das conclusões

Ocorreram erros de instrumentação durante a construção, corrigidos exclusivamente nos testes: nome de campo da union de LP e include do dispatcher; banco GPU de moedas ausente no host simulado; cap de stats e tabela sintética de Egotist inicialmente não fornecidos; um “bystander” Dragon fraco que era material legítimo na receita modificada; e o primeiro controle de planner que usava magia diferente da receita de Rex/sem sua linha Vanilla. O controle final usa a receita 680/6/47/571/705 já presente no pacote e confirma a busca real com Dual OFF. Nenhum desses erros motivou patch de MOD ou foi publicado como bug de produção.

Os avisos de offsets e nomes matemáticos dos headers oficiais aparecem nos logs de build das fixtures GCC, sem impedir a compilação. A suíte final não tem erro de fixture ou execução incompleta; os 29 FAIL são diagnósticos explicitados acima. Os testes novos não foram executados sob sanitizadores; assertions/loader/hooks/eventos são verificações próprias, enquanto os sanitizadores das etapas isoladas permanecem evidência histórica separada.

[AMBIENTE_TESTES.json](hal-20/integracao-inicial/AMBIENTE_TESTES.json) registra compilador, QEMU e hashes dos testes/fontes oficiais usados. Logs de compilação foram preservados literalmente, incluindo três marcadores de diagnóstico que terminam com espaço; diffs de código/documentação novos não exigiram normalização. Arquivos CRLF e logs históricos copiados dos PRs permanecem byte a byte iguais aos originais.

## Próxima sequência proposta, sem implementação automática

1. Revisar F1/HAL-21 e sua política OFF/ON em tarefa própria; não aplicar o patch por esta integração.
2. Reproduzir I1/I2 no controller real com mão de bônus construída pelo Dual e input/turnos completos. Comparar as ordens válidas e registrar o primeiro ponto em que a categoria/plano diverge, sem culpar os MODs que só aparecem na cadeia mas não executam o ramo.
3. Reproduzir I3 na aplicação real de configurações e definir uma fronteira transacional para OFF/ON/materialização. A evidência atual já dispensa injeção de pending, mas ainda usa host/motor sintéticos.
4. Só após autorização específica implementar correções mínimas no responsável apropriado, preservando quotas, receitas, valores, efeitos, decks e decisões aprovadas. Um contrato público com terceiro depende de autorização própria; não usar leitura de estado privado como solução geral.
5. Obter separadamente Time Wizard completo/proveniência, adaptador AI 0.1.1 e Droplist autorizado. Validar tabelas finais com Drop Missing Cards ON e imagem legítima do usuário em ambiente de teste próprio.
6. Reexecutar regressões afetadas e gameplay Windows antes de qualquer proposta de promoção. **Nenhum merge é autorizado por este diagnóstico e nenhuma compatibilidade integrada completa foi declarada.**
