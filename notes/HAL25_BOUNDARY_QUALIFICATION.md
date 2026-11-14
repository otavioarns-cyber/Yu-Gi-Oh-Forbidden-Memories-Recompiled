# HAL-25 — fronteiras restantes e integração automatizada

**Experimento independente, sem merge/release.** Base `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`; branch `test/hal-25-boundary-qualification`. Pacotes próprios derivados integralmente do [PR #26](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/26), commit `dc711074732e01275fb575aabda577c497ffe6d9`. As referências [#20](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/20), [#23](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/23), [#25](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/25) e #26 foram consultadas; nenhuma foi alterada.

## Resultado e decisão

Uma fronteira adicional foi reproduzida e corrigida experimentalmente **somente no header consumidor do RD**: o controller consome o cursor e as marcas ordenadas de mão, que podem divergir do pacote ABI1 depois de sua aprovação. O objeto original do PR26 consumiu344 em vez de535 ou quatro cartas em vez de três quando um wrapper próprio alterou esses estados derivados. O novo objeto recusa antes do consumo e exige novo duelo para sair dessa quarentena, sem rollback fictício.

A entrega final teve **544 processos de host parcial / 3.180 checks PASS**, classificados como **458 PASS, 76 REFUSED e 10 NOT_REACHED**, sem FAIL funcional final ou execução incompleta. As quatro reproduções negativas com os objetos preservados do PR26 mantêm **oito checks FAIL funcionais** separados. São falhas reais das invariantes sob interposição sintética, não prova de ocorrência natural em gameplay. Recusa segura não conta como ação concluída.

**Prontidão:** há uma correção localizada apta à revisão como candidata R/RD, no recorte automatizado exercitado. Não recomendo promover a HAL-25 inteira à integração completa ainda: faltam runtime PE32 funcional/imagem legítima para a execução integral, qualificação dos ramos ainda não percorridos e política de savestate durante uma ação em voo. O guard #20 continua conservador e não foi incorporado ou afrouxado.

## Diretriz atual de Windows

A orientação atual substitui o bloqueio amplo usado nos relatórios anteriores: **testes Windows automatizados estão autorizados agora**. Somente a bateria pessoal do usuário jogando fica para a última fase. Não foi pedido teste manual intermediário.

Foi preparado um backend local Wine/WoW64 10.0 e Xvfb, sem alterações de sistema. Wine é camada de compatibilidade Linux, não Windows nativo. O prefixo inicializa e o controle Win64 executa CreateProcess/Wait/Exit/GetCurrentProcessId. Entretanto, o controle PE32 independente e o jogo i386 falham antes da entrada do programa. Tentativas diretas encerram em SIGSEGV; o driver WinAPI observa falha de criação1359 no controle e731 no jogo. Portanto não se atribui a falha aos MODs.

Também foi tentado Wine32 sob QEMU i386: falha de exec do loader sem a opção interna de não reexecução; com ela, timeout25s repetindo mmap antes da entrada PE; outra base de guest termina em timeout30s. Isso documenta as tentativas neste ambiente, sem afirmar impossibilidade de automação em outro backend.

| Tentativa automática | Resultado |
|---|---|
| Prefixo Wine e controle PE64 próprios | 2 PASS de infraestrutura; não gameplay |
| Controle PE32 próprio | AUTOMATION_UNAVAILABLE, criação/execução falha antes de main |
| Jogo oficial PE32 headless e visível | AUTOMATION_UNAVAILABLE, sem janela de jogo utilizável |
| UI/input/restart/duelo/efeitos/save-load Windows | NOT_REACHED devido ao backend e recurso ausente |
| Executável Linux oficial/QEMU, headless | exit1: imagem do jogo ausente, conforme diagnóstico real |

Não há imagem legítima do jogo nos arquivos disponíveis. Nenhuma ROM foi baixada/fabricada. A inicialização Linux oficial confirma esse impedimento independentemente do Wine. O próximo ciclo automático precisa de um backend PE32 funcional e do recurso legítimo privado; não depende de participação pessoal jogando. [Procedimento](../tests/hal25-boundary/windows/README.md), [recibos/logs](hal-25-boundary/windows/RESULTADOS.json), [ambiente e alternativas](hal-25-boundary/windows/ENVIRONMENT.json).

## Nova reprodução: seleção derivada do controller

Os quatro processos negativos usam os objetos exatos R=`3c472220…` e RD=`92664e4a…` do PR26, nas duas ordens e com AI/Dual OFF para a reprodução mínima. O planner/controller e o código de RD são reais; um produtor público **de teste** fornece uma proposta controlada. Nenhum pending ou quota é escrito. Não se confunde essa proposta adversarial com uma decisão retail da IA.

1. Caso12: proposta física única535. No subestado3, um wrapper próprio muda o cursor em `DuelSelectionRecord` para a carta344. Os bytes ABI1 e as identidades no snapshot continuam iguais. PR26 consome344.
2. Caso13: proposta física com três cartas. O wrapper põe ordem4 na marca de uma quarta instância em `DuelHandSlot.active_09`. PR26 remove quatro cartas, embora a proposta aprovada contenha três.

O teste aponta para os tipos e símbolos oficiais do SDK, não para uma estrutura inferida de um MOD privado. [Logs negativos preservados](hal-25-boundary/logs/PR26-derived-native-negative.tar.gz). Os wrappers são instrumentação conhecida por hash; não foi encontrado esse comportamento nos ramos executados dos wrappers reais M/R/F/H/D. A alcançabilidade natural permanece não comprovada.

### Correção aplicada nesta branch

[Diff reversível sobre PR26](../experiments/hal25-boundary/controller-derived-selection.patch): **25 linhas adicionadas a `mods/ritual-duelists/mandatory_hand_consumer.h`**, nenhum outro C/H de produção alterado.

Antes de delegar ao controller quando a construção da seleção terminou, o RD confere:

- seleção única: cursor físico aponta para a fonte aprovada, contador de pilha é zero e não há marcas residuais;
- seleção combinada: contador e ordens1…N coincidem exatamente com as fontes aprovadas, sem material adicional;
- durante subestados0/1, a construção nativa legítima de cursor/pilha continua livre para progredir.

A verificação é somente leitura. Divergência usa a quarentena existente, causa física7. Como o controller já está numa fronteira posterior de coleta, `recover` não tenta consertar cursor/pilha/visuais: exige novo duelo. A mão permanece intacta e o ledger Dual permanece neutro nos oito checks adicionais com Dual ativo. Não se acrescentou previous_swap, não se consumiu uma segunda vez e não se mudou a estratégia para procurar uma jogada alternativa.

O único import novo é **`D_800E9F10`**, já exportado pelo SDK API24. `DuelSelectionRecord` e `DuelHandSlot` são layouts oficiais com verificações de tamanho/offset. Não há novo provider, alteração de ABI1, manifest, settings ou responsabilidade do Dual.

## Lifecycle, recuperação e limite de retries

O protocolo cooperativo do PR26 foi mantido. Testes novos conectam uma tabela própria, recebem ACK de disconnect e tornam sua página **PROT_NONE**. Reset, OFF/ON efetivo e novas ações continuam sem ler essa tabela nem chamar seu callback. A tabela é mantida viva quando disconnect retorna0 durante evaluate; a avaliação interrompida não comita e há ACK posterior antes de aposentar a tabela.

R real também executa seu shutdown num ponto de teste após planner/antes de swap: desconecta o produtor e invalida a ação antes de consumo. É uma chamada instrumentada ao callback real; não se afirma que a UI ofereça unload nesse ponto. O manager normal encerra os hooks/eventos antes do shutdown e não fornece unload concorrente arbitrário. A pureza/serialização do produtor real e os limites de vida útil continuam parte do contrato, sem sandbox contra código nativo que ignore ACK.

O limite experimental de três transações inválidas foi reavaliado: dois vetos transitórios ainda podem terminar numa nova ação; três vetos persistentes produzem BLOCKED. **Mais100 ticks não fazem novas chamadas à VM/provider nem consomem cartas ou alteram a sequence.** Retirar a causa não retoma silenciosamente o pending. Recuperação explícita com sequence atual permite uma nova transação; sequence antiga é recusada. Exaustão de tokens continua exigindo reinício de processo e não wrap.

O valor3 continua uma política experimental de orçamento de erro, não uma constante estratégica ou dificuldade. Os testes demonstram limitação e recuperação; não demonstram que3 seja um ótimo de UX/latência. Não se criou setting novo nem alterou default.

`RhsRecovery` continua72 bytes e observável por read puro. BLOCKED informa limite/causa raiz e aguarda recuperação explícita; QUARANTINE informa divergência física e recuperação permitida ou novo duelo; EXHAUSTED informa reinício de processo. O novo caso de cursor/pilha produz `state=4,cause=7,recovery=2`. [Observações completas](hal-25-boundary/FRONTEIRAS.csv) e [contrato anterior preservado](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/test/hal-25-contract-hardening/notes/hal-25-hardening/ABI_E_RECUPERACAO.md). A apresentação nesta entrega é diagnóstica/estruturada; a UI real não foi alcançada pelo backend PE32. Não foi criada interface gráfica definitiva.

## Materialização e consumo completo disponível

Foram ligados **14 objetos nativos oficiais inalterados**, quatro além do PR26: efeito ritual, checker/match, lifecycle de records e inicialização do efeito. O efeito nativo, em vez de um loop manual do harness, executa as três chamadas de desativação e avança de estado.

- Field: três instâncias corretas deixam o campo; outra cópia igual permanece.
- Both: um tributo de campo e dois da mão são consumidos; bystander e cópia igual não selecionada permanecem.
- Hand-only: recusa pelo default aprovado min_field=1, sem relaxá-lo.
- Vanilla: checker/consumidor nativos com M/R efetivamente OFF, mantendo três tributos de campo.
- Depois dos consumos, executa-se a publicação nativa inicial do resultado no anchor original. Só seu ID muda; ponteiro/índice/data_block e demais deck records permanecem íntegros.
- Ação combinada: o controller nativo remove três cartas em uma confirmação, com uma categoria Monster no ledger Dual; recover depois do commit não inventa rollback.

A conclusão da requisição de animação e o salto das etapas de portal1/2 são **modelos explícitos**. Render, ativação final, seleção de estrela, bytecode retail e duelo inteiro não foram executados. Os perfis rituais chamam diretamente o efeito em tabuleiro sintético do lado0; AI/Dual ON nesses perfis significam bibliotecas reais carregadas, não execução de seus gates naquele trecho. Quotas, bônus e decisões de planner são exercitados pela matriz de controller separada. Não se anuncia uma sequência completa retail controller→efeito→animação.

O monitor local termina no primeiro consumo observado. Nos casos combinados testados, as três remoções acontecem na mesma chamada do controller e são verificadas individualmente. O consumo ritual ocorre no materializador próprio de M/motor, com todos os hooks registrados rastreados. Um callback externo que altere identidade/material depois da última validação ou consuma antes de devolver controle continua exigindo cooperação ou recusa de admissão. Não se criou rollback de tributo já gasto nem garantia global de atomicidade.

## Testes finais e evidências

| Grupo final | Processos | Checks PASS | Classificação |
|---|---:|---:|---|
| Regressões normais/cancelamentos PR25 |174|1.066|PASS|
| Recuperação após aborto |64|400|PASS|
| Reset/novo duelo |16|112|PASS|
| Posição/contrato existente |2|12|PASS|
| Regressões de hardening PR26 |72|274|50 PASS /12 REFUSED /10 NOT_REACHED|
| Composição M/F/R/RD × AI/Dual e ordens |72|604|PASS nos ramos exercitados|
| Fronteiras novas |128|640|80 PASS /48 REFUSED|
| Rechecagem de cursor/pilha e quota após recusa |16|72|16 REFUSED|
| **Total final** |**544**|**3.180**|**458 PASS /76 REFUSED /10 NOT_REACHED**|

Os16 últimos processos reexecutam o subconjunto adversarial com a assertion adicional de quota, sem duplicá-lo como ação de gameplay. Contagens são de processos efetivamente executados, não de cenários todos distintos. Os metadados de cada suíte registram a versão dos testes, pre/postflight e o hash do harness.

Regressões canônicas: R23 PASS/3 SKIP; RD34 PASS/1 FAIL histórico de imutabilidade/3 SKIP. Mantida a adaptação8→9 apenas em cópia temporária do loader, sem editar os testes canônicos. O FAIL histórico é separado de regressão funcional; [audit do delta atual](hal-25-boundary/AUDIT_FINAL.json) passa. Fusion corrigido permanece16 grupos PASS/214 assertions de caracterização, sem alterações no pacote.

[Matriz](hal-25-boundary/MATRIZ.csv), [JSON consolidado](hal-25-boundary/RESULTADOS.json), [fronteiras/consumos/recuperação](hal-25-boundary/FRONTEIRAS.csv), [logs/índices SHA](hal-25-boundary/logs), [reprodução](../tests/hal25-boundary/README.md). Os testes Windows e o audit histórico ficam fora desse total de host parcial. [Riscos/impedimentos](hal-25-boundary/RISCOS.json) distinguem recursos ausentes de testes pessoais adiados.

Erros de desenvolvimento preservados, sem atribuí-los aos MODs: resposta do produtor de teste inicialmente tinha destination diferente da seleção; Blue-Eyes estava com stats genéricos em vez de3000/2500 e era tributo legal segundo a regra; um primeiro editor de cursor usava a view de side incorreta. As fixtures foram corrigidas, sem remover assertions, antes das reproduções/qualificações finais. O primeiro build do protótipo também usou um membro inexistente em DuelSideState; o código final usa o tipo oficial DuelSelectionRecord e recompila integralmente. Os logs preliminares são DEVELOPMENT_ONLY, separados dos quatro FAIL funcionais do controle PR26.

## Wrappers, ordem e guard

[Inventário da composição](hal-25-boundary/wrappers-composition/WRAPPERS.json):125 nós; [fronteiras/efeito nativo](hal-25-boundary/wrappers-boundary/WRAPPERS.json):139 nós. Zero registros não resolvidos. Cada ponteiro efetivamente registrado é vinculado ao artefato SHA256, hash do loader, seção/offset, hash do corpo/relocations e arestas executadas. Nomes/IDs são rótulos, não critério de liberação.

O tracer registra seleção/deck/consumo e efeitos inclusivos/exclusivos em cinco alvos cdecl conhecidos; SELF exclui filhos críticos rastreados, não toda chamada C. `CONSUME_ENTER/EXIT` identifica cada record nos rituais. Não somar pai e filho para contar materializações. Ramos humanos de Surrender/Select, Square/Fusion e todo estado possível de efeitos não foram qualificados por simples delegação observada no lado CPU.

M=`1fe34a1e`, Fusion F1=`afb75202`, AI=`3e0648cb`, Dual privado=`3028c68a` permanecem idênticos. R novo conserva code=`1e478c68`; RD novo tem code=`3d26698f`. R é -20/+40, M=-50, F=-30/+30, RD=10, Dual=20 e AI=0/+50 nos perfis pertinentes. O inventário guarda a ordem efetiva e chamadas, sem presumir segurança pelo rank numérico.

O predicado do manager **inalterado** do PR20 (`936c3513c9e6f8131b1eb4a5410d9c21c3c2c592`) continua recusando o novo RD com Dual ON:2 REFUSED. Com Dual OFF:2 NOT_APPLICABLE. É probe do gate de versão antes da travessia, não admissão da coleção nem execução de duelo com o guard. Nenhum wrapper novo foi liberado pelo nome, nenhuma whitelist foi ampliada e nenhum arquivo de core foi promovido.

## Builds, ABI, permissões e histórico

Builds completos SDK oficial Windows API24 `--target i386`, Clang23.1.2/llvm-mingw20260922,2/16 C. Fontes da cópia efetivamente compilada coincidem com os publicados. [Recibo](hal-25-boundary/BUILD.json), [ABI R](hal-25-boundary/ABI_R.json), [RD](hal-25-boundary/ABI_RD.json), [comparação de código](hal-25-boundary/CODE_DIFF.json).

| Objeto experimental | Bytes | SHA-256 |
|---|---:|---|
| R |737.280|`78ec3ce45694b1f583213530dfc0a527c8974056f67c49e7791fb6821f55ab45`|
| RD |4.568.940|`9f0d7354e6ff3f44776308b18e96b960480500c814ae22c3e7d1e6b57eafea34`|

R não tem alteração de produção e conserva todos178 corpos/relocations de funções; SHA/tamanho mudaram com caminho/debug do rebuild. RD conserva exports44 e passa de89 a90 imports, só acrescentando D_800E9F10 oficial. O inline/layout/relocations da compilação fazem oito fingerprints de funções diferirem; a única diferença de fonte funcional é o header mostrado no patch. Nenhum C estratégico/deck/drop foi alterado. Exports R=2, declaração mínima de API R=4/RD=12 e state RD288/v4 permanecem; o alvo qualificado é SDK/host24, sem promessa a hosts antigos.

ABI1 de seleção, snapshot e companion lifecycle permanecem byte a byte iguais em R/RD e ao PR26. Manifests,48/6 settings, receitas, decks, assets, efeitos, testes canônicos e NOTICES também. São200 arquivos herdados de R e154 de RD inalterados; as outras diferenças são header RD, objetos, README/proveniência. Arte da moeda/PeterIron e doação RitualStarter mantêm o fundamento de permissão registrado anteriormente, sem inventar licença aberta.

Dual exclusivamente local: SHA `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`. Não se publicou seu código, manifest ou binário. Não se publicam ROM, saves pessoais, executável do jogo, Wine/SDL ou toolchain. Master, integração e branches anteriores serão conferidas antes/depois do push sem force. Nenhum merge ou release autorizado.

## Fronteiras ainda abertas antes da integração completa

1. Execução automatizada integral no backend PE32 funcional com imagem legítima: duelos, UI/inputs, reinício e efeitos completos. É impedimento de recursos atual, não decisão de adiar toda automação Windows.
2. Admissão conjunta de hooks/HAL22: os novos fingerprints e ramos M/R/F ainda não estão autorizados no guard de referência. A nova checagem de RD não resolve I2 nem torna toda ordem segura.
3. Savestates em meio a handoff/coleta: os testes visitam buffers registrados e eventos reais sobre motor modelado; não restauram um snapshot completo de gameplay. Snapshot/pending transacional não são serializados por suposição. É necessária política explícita e qualificação de load em voo; estados antigos não são aprovados.
4. Consumidores/materializadores externos devem respeitar as fronteiras e ACK. Proteção contra escrita após a última validação, unload não cooperativo ou consumo entre callbacks rituais não é garantida globalmente.
5. UI final de estado/recuperação e latência do limite3 ainda precisam revisão; os ramos humanos e animações não percorridos permanecem fora da qualificação. Time Wizard, adaptador AI0.1.1 e Droplist externo continuam dependências separadas, sem workaround nesta branch.

Próximo passo mínimo: revisar o patch local e essa delimitação de candidata, executar a próxima bateria automatizada no backend PE32 com os recursos legítimos e decidir a política de load em voo/admissão dos wrappers antes da coleção completa. A bateria pessoal do usuário fica exclusivamente para a última fase; não foi solicitada agora.

## Publicação

Links de PR/branch/commit e preservação das referências serão registrados depois do push. Nenhum merge automático.
