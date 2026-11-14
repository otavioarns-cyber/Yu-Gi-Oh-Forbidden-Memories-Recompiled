# HAL-20 / HAL-26 — consolidação de estado e preparação do Time Wizard

Entrega experimental de14/11/2026 no [PR #30 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/30), branch `test/hal-20-state-timewizard-readiness`. Continuação exata do PR29, commit `3284ec493d179f4b23cc7230a2979a1c2b122873`; base oficial `integration/hal-20-v0.3.1-preview.2`, `d649647c159c91d615c07d6960928a37e270b461`, SDK24/i386. [Referências #20/#22/#27/#28/#29](hal-26/REFERENCES.json) e [HAL-26 no Linear](https://linear.app/half-programer/issue/HAL-26) consultadas. Sem merge, release ou promoção de política permanente.

## Resultado e respostas

1. **B impede a perda de Queen e leases ao recusar o load antes de ler/restaurar memória.** A brecha com guard de ordem OFF foi fechada nesta candidata. OFF efetivo também não libera o load se a biblioteca continuar residente. Isso preserva o duelo corrente, não restaura o snapshot pedido.
2. **Não podemos habilitar load completo da coleção com segurança.** A avançou: Queen e leases Fusion são recuperadas juntas, com identidade física e piso do serial, no modelo restrito. Ainda faltam os demais estados R/F, buffers de cartas geradas, transações, identidade/proveniência do arquivo e contexto completo. A não substitui os objetos atuais e continua recusada pelo guard. Savestates antigos/cross-processo não estão certificados.
3. **Time Wizard continua fora da coleção admitida.** Os cinco C disponíveis compilam com SDK24; quatro recompõem um núcleo binário0.23.1 que não foi recompilado dos fontes. Controllers nativos selecionados e testes do pacote passam, mas o replay dos contratos encontra detecção falsa de AI HARD e dependência do predecessor direto do Dual. É necessário recuperar/reconstituir o fonte exato do núcleo, corrigir e verificar essas fronteiras e qualificar seus materializadores/renderização, preservando roleta/regras.
4. **Regressões pertinentes da coleção:188 processos/2.228 checks PASS**, além de96 processos/472 checks da A separada, manager1.048.576 perfis, preflight literal de load65 casos e testes oficiais de armazenamento/chunks. A qualificação dos consumidores Time Wizard é separada:36 PASS/48 FAIL/12 REFUSED no replay; essas falhas não viraram compatibilidade por estarem reproduzidas em testes diagnósticos positivos.
5. **Bloqueios externos concretos:** o GitHub suspendeu workflows deste fork por escala de uso do Actions; o controle PE32 próprio não recebeu execução. Não há backend Windows funcional selecionado nem imagem legítima do jogo acessível nesta tarefa. Faltam fontes completos do núcleo Time Wizard e qualificação de redistribuição antes de publicar componentes novos de terceiros. Persistência completa dos nossos owners é trabalho técnico interno pendente, não algo que dependa do usuário jogar agora.

## Coleção preservada e escopo do diff

M/R/RD/F/Field/H/D/DMC continuam nas versões do PR29. [Auditoria](hal-26/AUDIT.json): **507 arquivos herdados de MODs byte-idênticos**, incluindo fontes, objetos, manifests, settings, receitas, assets e opcionais oficiais. [Qualificação herdada](hal-20-field-state/QUALIFIED_ARTIFACTS.json) permanece inalterada: sete papéis/sete alvos/19 callbacks; nenhum objeto novo foi liberado por nome. A configuração privada monta oito IDs únicos e mantém `restart:true` somente na cópia experimental AI. Dual permanece privado e inalterado. Time Wizard não é incluído pelo stager.

| Objeto canônico | SHA-256 preservado |
|---|---|
| M1.3.6 | `c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0` |
| R0.78.74-effect-ai | `78ec3ce45694b1f583213530dfc0a527c8974056f67c49e7791fb6821f55ab45` |
| RD0.4.0-alpha.8 | `9f0d7354e6ff3f44776308b18e96b960480500c814ae22c3e7d1e6b57eafea34` |
| F beta44 +F1 Revogar +Field | `c259f789c5060623744bd28ab09de4044d055d9c02570a3f7f73032eb5190dd6` |
| Field0.19 Vanilla V2 | `729ff106787d477def41feb42a1a5d168f5bb050af870a25738c977e02c1cd21` |
| AI oficial +adaptador técnico Field | `05cf957bc4575503d257432cd927748367207258189856217bc1bf2adb0f7d19` |
| Dual privado0.5.7 API24 | `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378` |

Arquivos de produção alterados nesta continuação: `src/pc/mods/manager.c` (somente predicado de load), `mods.c/.h` (consulta interna de residência) e novo `state_policy.h`. `modapi.h`, Hooks_Relink, Mods_Order, controller, janela, `guest/state.c`, state_subsystems e src/pc/saves não mudaram em relação ao PR29. O guard de ordem permanece opt-in e com a semântica anterior. Nenhuma modificação de Queen Lock, probabilidade, estratégia, quota, deck, efeito ou valor.

Workflow novo executa somente um controle C próprio em Windows x86 com `contents:read`, restrito à branch experimental/PR para integração. Não alteramos os workflows globais anteriores ou as permissões/configurações do repositório. Os testes compartilhados receberam seleção de recibo/casos e probes adicionais, conservando os casos/asserts históricos.

## B — recusa segura independente da ordem

O PR29 retornava1 imediatamente quando `experimental.hal2224_order_guard` estava OFF. Essa brecha permitia o caminho legado de restore/reset que já perdia Queen/leases. Retirar somente essa condição ainda deixaria passar owners desativados mas residentes, e não cobriria estado temporário AI/Time Wizard.

A candidata usa o estado **efetivo** e a residência real da biblioteca (`object.image` ou `native_handle`), não preferência salva ou hash curto. A consulta `Mods_CodeResident` é somente leitura e inclui uma biblioteca mapeada cujo init falhou. Hash zero não é tratado como prova de ausência. Os IDs conhecidos marcam cobertura incompleta para **recusar**, não para qualificar código.

| Perfil corrente | Resultado B nesta branch |
|---|---|
| M/R/RD/F/Field/H/D ativo, guard ON ou OFF | REFUSED antes da leitura/restauração |
| Algum desses owners efetivamente OFF mas residente | REFUSED até um processo limpo |
| Preferência OFF esperando reinício, owner ainda ON | REFUSED; preferência não substitui estado aplicado |
| Time Wizard ativo/residente em teste futuro | REFUSED; isso não o admite para gameplay |
| MODs instalados mas OFF e nunca inicializados | Não bloqueiam por este predicado; loader legado continua com suas verificações |
| DMC sozinho/data-only ou ID desconhecido | Fora desta recusa específica; **não** implica persistência/compatibilidade certificada |

B não resolve os conflitos de materialização/ordem quando o guard está OFF. A composição padrão continua com o guard qualificado ON; a independência demonstrada aqui vale para a recusa de load.

A mensagem explícita informa que o duelo atual foi mantido, memory-card não mudou e processo novo com os MODs OFF só permite tentar o loader legado. Tem252 caracteres, cabendo no buffer256 da função comum. Não desligamos MODs silenciosamente, não perdemos preferências nem reconstituímos cartas. Gravar um snapshot diagnóstico continua possível; carregá-lo nesse perfil é recusado. Esse impacto precisa de revisão antes de adotar uma política permanente.

A função comum `state.c:load` já era usada pela UI/F7 e pelo controle `Memories_StateLoadHere`; o call site existente permanece antes de fseek/fread/parsing/remap e de `pending_image`. O teste executa **o corpo literal completo dessa função**, extraído sem edits, junto do manager atual. Para os16 perfis ativo/residente ×quatro arquivos corrompidos/incompatíveis, não há leitura, alocação, parsing, contexto ou alteração do arquivo. O caso65 confirma que OFF frio chega à validação legada e recusa o arquivo inválido. Os caminhos posteriores são traps: esse teste não simula um load bem-sucedido.

O escopo de runtime qualificado aqui é o backend nativo usado pelo alvo i386. `state_translated.c`/`translated_state_backend.c` são outra implementação ARM64; não portamos a coleção nem certificamos a mesma política nesse backend. Devem receber qualificação própria antes de qualquer alegação multiplataforma. Não se confunde falta dessa qualificação com bypass no caminho i386 exercitado.

## Inventário de estado por owner

[Inventário completo de símbolos graváveis](hal-26/STATE_INVENTORY.csv), [tipos e artefatos](hal-26/STATE_INVENTORY.json): **847 objetos**,294 com tipos que contêm ponteiros,150 com tipo ausente/inconclusivo. São metadados, não dumps de BSS, bytes ou snapshots. Inclui todos os símbolos STT_OBJECT graváveis disponíveis, mas não prova cobertura de heap, variáveis otimizadas sem símbolo ou semântica. Classes abaixo foram confrontadas com reset/registro/fontes; campos desconhecidos continuam sem autorização de serialização.

| Owner | Objetos graváveis | Estado existente e lacuna principal |
|---|---:|---|
| M |50|Ponte ritual: fontes físicas, etapas/preconsumed/consume_order, objetos visuais, result bits; sem bloco completo registrado. Não retomar ponte em consumo.|
| R |304|Queen/fulfilled, borrowed origins/leases, marcas de rituais, LP/reflexões, filas/visuais/moeda, conhecimento e animações; clear apaga amplas famílias. Só Queen está coberta pela variante restrita.|
| RD |95|save_state v4 cobre perfil/proveniência/Starter/Blue-Eyes; deck_pull, transação mandatory, callbacks, generation/action/sequence, safe snapshot, quarentena e caches não são um snapshot transacional completo.|
| F |174|aux_unlocks/serial, origens Fusion/Metal, gastos/cargas por instância, pendências, conhecimento Kamion, moeda/tween/visuais e buffers `r2_zombie_data`/`r3_summon_data`. Só leases com identidade no combined deck são cobertas pelo experimento.|
| Field |11|terrain_modifier e campo pertencem à imagem nativa; owner_effective é lifecycle, footer/description_start são caches/pointers. Evitar reaplicar bônus em record já restaurado; visual composto ainda não qualificado.|
| AI |7|Plano/caudas e estado de execução sem contrato completo; callbacks/host devem ser reconstruídos, não copiados.|
| Dual privado |37|Bloco registrado g_state50 bytes preserva categorias/bonus e não tem ponteiros; demais ponteiros/hooks são runtime, blocked_frames separado. Registro não é prova de todas as interações. Nenhum arquivo do terceiro publicado.|
| Time Wizard |169|Wheel, RNG, fase/targets/plan, retry/eligible, cache AI, papel híbrido, buffers gráficos, callbacks e tokens; fontes parciais e núcleo fixo. Sem esquema completo qualificado.|

DMC1.0 é data-only: depende das tabelas/perfil nativos, não possui objeto de código com BSS nessa coleção. O RNG comum de MODs já participa dos subsistemas nativos; isso não cobre automaticamente seeds/filas privadas do wheel.

### Por que o contrato atual não basta

`Mods_RegisterState` conserva um bloco por owner, visitado somente quando efetivamente ativo. Compatibilidade nativa confere assinatura mod-set e versão/tamanho dos chunks; não valida a semântica de efeitos não registrados. LOAD_AFTER ocorre depois que a memória foi restaurada e os MODs foram resetados. Recusar nesse callback já não equivale a preservar o duelo anterior. Os testes oficiais de chunks/mod-set reproduzem essas checagens, não uma validação completa de identidade de cada efeito.

Callbacks, tabelas cooperativas e ponteiros para display/card data precisam ser reconstruídos e negociados por ACK, jamais copiados do arquivo. Valores pointer-free também podem representar action/generation já aposentados. Um pending vazio ou todos os materiais com o mesmo card ID não são prova de uma fronteira restaurável.

## A — avanço conjunto, ainda incompleto

Fontes em `experiments/hal26`; patches/recibos em [state-A](hal-26/state-A). Builds separados SDK24 `--target i386`; **não substituem R/F canônicos**, não entram no staging e são recusados pelo guard por SHA divergente. Os providers de diagnóstico têm namespace próprio, não são contratos promovidos.

- R reutiliza a cápsula Queen260 bytes do PR29, incluindo identidade por índice físico/data block, fulfilled e bônus. Reset avança epoch; callback/action/generation não são ressuscitados.
- F acrescenta uma cápsula de leases380 bytes, não BSS. Captura grant ID/serial e identidade deck/block, valida todos os registros antes de escrever nos leases e restaura F antes de R nos eventos. Reconhece movimento de slot/controlador pela mesma instância; outra cópia do mesmo ID não herda grant.
- O piso do emissor nunca retrocede no processo: snapshot77, emissor corrente88 →lease restaurada77 e emissor88 →helper real de nova concessão89. OFF→ON continua Revogar; não recria lease automaticamente. O teste chama o helper compilado real com ABI ECX inspecionada; isso é aquisição instrumentada, não uma resolução Egotist completa.
- Preflight puro usa uma vista **da imagem candidata** fornecida pela fixture e verifica versão, elegibilidade, referências físicas, slots, data-ID, duplicatas, serial, campos reservados e exaustão antes de qualquer memcpy modelado.29 negativos/controles não alteram seus inputs. **Sempre** retorna INCOMPLETE quando solicitado load da coleção inteira.
- Há24 roundtrips positivos de escopo restrito entre os80 processos principais; os outros exercitam recusas/lifecycle. Globalmente, nenhuma restauração completa é autorizada. Leases são semeadas77 para isolar estado; não se alega concessão natural dessas leases em duelo.
- **Lacuna adicional reproduzida:** helper real `r2_summon_zombie` cria carta em buffer próprio Fusion; uma lease ligada a essa instância não pode ser descrita pelo índice de combined deck da cápsula. O capture marca ineligible, e oito perfis são REFUSED. Não serializamos ponteiro, não procuramos outra carta pelo ID30 nem copiamos BSS para contornar.

Na matriz parcial:96 processos/472 checks PASS,56 PASS e40 REFUSED na classificação diagnóstica. Isso inclui oito recusas por artefato desconhecido do guard; não significa56 loads completos. As falhas de cobertura de A permanecem **FAIL de prontidão**. A vista candidata também não substitui um prepare atômico do owner corrente: mudanças de lifecycle ou exaustão do emissor ainda podem fazer LOAD_AFTER recusar, sem rollback global. READY do escopo de diagnóstico não autoriza o loader. O comparador atual não valida um arquivo nativo completo, fingerprint de jogo/perfil ou cross-processo; o formato dos experimentos não deve ser usado como formato final.

| Critério | A restrita | B candidata |
|---|---|---|
|Queen +lease de instância nativa|Recupera juntas no modelo restrito|Evita perda ao não executar load|
|Carta gerada fora do combined deck|REFUSED: identidade owner-specific ausente|REFUSED antes de ler/restaurar|
|Planejamento/consumo/quarentena/OFF|Captura inelegível; sem replay de pending|REFUSED em qualquer fase protegida|
|Serial após rewind no processo|Piso preservado e nova concessão avança|Sem restauração de grants|
|Outros efeitos/visuais/heap/contexto|Não cobertos: INCOMPLETE|Sem overwrite; não precisa adivinhar estado|
|Guard de ordem OFF|Não habilita a coleção completa|Recusa permanece ativa|
|Cross-processo/estado antigo|Não qualificado|Sem promessa; perfil ativo/residente recusa|
|Impacto ao usuário|Ainda não instalável como solução de load|Load indisponível nesse perfil até revisão/implementação completa|

### Requisitos mínimos para completar A

Primeiro fechar os esquemas de cada família, inclusive buffers gerados e relações de borrowed origin; não apenas acrescentar mais alguns bytes à cápsula. O plano de restauração precisa:

1. Ler/verificar íntegra, sistema/arquitetura/build, perfil/settings e fingerprints dos owners numa imagem separada; validar limites e versões antes de tocar o duelo.
2. Produzir vistas físicas tipadas para native deck e pools de cada owner, incluindo geração; nunca procurar substituto por card ID.
3. Exigir fronteiras seguras tanto correntes quanto salvas, sem callbacks em voo/consumo/pending/quarentena não restaurável; resolver invalidação/ACK antes do commit sem perder uma ação corrente numa recusa.
4. Preparar todos os owners sem efeitos colaterais. Se algum não der cobertura completa, recusar. Não usar LOAD_AFTER como preflight.
5. Só então restaurar nativo/owners em ordem verificada (F e grants antes de borrowed state R; RD reconecta com epoch fresco; Field evita bônus duplicado). Nenhuma etapa pós-overwrite pode depender de rollback fictício ou concessão nova a outra identidade.
6. Definir sessão/epochs/serials para cross-processo e arquivos antigos. A validação do contexto/stack e de callbacks é separada da validação das cartas. Até demonstrá-la, recusar esse escopo explicitamente.

Esses requisitos não foram implantados como novo ABI global ou política permanente. B é a candidata utilizável para prevenção nesta branch, não uma declaração de HAL26 integralmente resolvida.

## Time Wizard0.24.0-test: proveniência e componentes

[Auditoria privada e testes](hal-26/timewizard/RESULTADOS.json), [núcleo preservado](hal-26/TIMEWIZARD_CORE_BOUNDARIES.json), [inventário dos heads consultados](hal-26/TIMEWIZARD_GIT_SEARCH.json). O pacote original permanece byte-idêntico. Publicamos metadados e testes próprios; não fontes/binários/sons do pacote Time Wizard.

| Insumo | SHA-256 / uso |
|---|---|
|time-wizard.o recebido|`828a5879a81ee33078c7cd2e60ba7c8e87557f889b035455211ed700ca8d6b07`; controle privado|
|time-wizard-base-v0.23.1.o|`5b4252dafbc07532f859cda92e49e75d15e531a880f90cc7924371b7e37f9b4a`; núcleo fixo sem fonte completo|
|Relink parcial SDK24 desta tarefa|`1c81f083feffe0c4615da1cc0f956e04670531ca41cf819935b2ff73b368ba79`; **não é rebuild integral** e não é instalado|

`build_v024.py` conserva o core por hash, renomeia entradas e acrescenta quatro fontes: Dual compat, description fit, evolution e Encore. O quinto C, `time_wizard_ai.c`, compila separadamente, mas o builder não o relinka: a camada AI já está incorporada no núcleo. Não substituímos esse trecho nem misturamos versões.

Estão disponíveis os adapters completos desses cinco arquivos e os testes que os acompanham. Faltam os fontes primários do controlador Time Magic, snapshot_targets/transform_card, resolução/destruição, animações e wheel incorporados no objeto-base. “Compila/linka” e103 imports presentes no SDK não provam equivalência ABI ou comportamento desses componentes.

### Dependências binárias exatas

O builder cria aliases na BSS do core: rng0x82c, phase0x830, chosen0x840, won0x844, plan_count0x84c, plan0x88c (10×20 bytes), original_retail_card_use0x97c, chance_50 0x998, test_result0x9a0 e spin_id0x9a8. Foram conferidos limites no objeto fixado. Os testes de entrada usam `.text:0x9210` open_wheel e0x7330 card_use. A comparação dos91 corpos originais no prefixo, mascarando operands de relocation, passa; **não** certifica os destinos dessas relocations ou ABI geral.

A ponte Dual lê assinaturas em+0x23/0x24/0x29/0x2a/0x2b/0x30 e dois endereços relocados em+0x25/+0x2c. O acesso ao estado da Roulette Lib também reconhece instruções em busy()+6…14. Tudo é i386/objeto/compilador específico, não contrato público. A mesma assinatura Dual reconhece o objeto API24 pinado quando seu callback é o predecessor direto; não generalizamos a outro rebuild ou wrapper.

### Interações testadas e riscos reais

[Qualificação separada](hal-26/TIMEWIZARD_QUALIFICATION.csv), [observações/hashes/callbacks](hal-26/TIMEWIZARD_CONSUMER_AUDIT.json):96 perfis com campos0/4/6, Field OFF/ON, H/D OFF/ON, duas posições de R/F e guard OFF/ON. O renderer/wheel é modelado; providers Fusion, callback de OFF e os callbacks usados como predecessores são reais. O inspetor remove apenas os thunks de trace para reaplicar os predicados aos corpos relocados exatos. Não carregamos o Time Wizard inteiro na composição nem o qualificamos por ID.

- **Encore/F1:** tentativa iniciada ON conserva eligible em cache após OFF efetivo de F; fonte real continua permitindo retry. Uma tentativa nova OFF responde sem benefício. Reset/cancelamento, spin ID errado, mudança de cena e caminho manual sem amostrar RNG passam. O teste confirma o comportamento atual; não escolhe uma nova regra para cancelar uma tentativa já iniciada.
- **AI:** o fonte usa `ai_original_run != AiScript_Run` para inferir HARD ativo. Com H OFF e predecessor RD/R, infere ON. No subconjunto guard ON isso gera24 FAIL de contrato nos48 perfis; no guard OFF há mais24 FAIL com observer de instrumentação, classificados separadamente. Não afirmamos que a instrução/estratégia do núcleo recebido seja equivalente a esse fonte ou que uma jogada BANK errada tenha ocorrido num duelo completo.
- **Dual:**24 perfis têm D ON mas outro callback real de draw como predecessor direto; o resolver recusa reconhecê-lo. No guard ON são12 observações de assinatura recusada; seis já coincidem com FAIL AI e seis ficam REFUSED na classificação agregada. O objeto Dual não foi modificado e a assinatura direta não quebrou. É dependência estrutural de ordem/delegação, não exploração de quota demonstrada.
- **R/RD:** mantêm seus contratos reais Queen/mandatory e providers; nenhuma integração seletiva Time Wizard foi promovida. Materialização híbrida, cancelamento, papel Monster/Support, ausência de dupla materialização e seleção Queen ainda exigem qualificação específica do Time Wizard completo.
- **Fusion/evoluções:** o adaptador altera snapshots-alvo do núcleo, não as funções completas de transformação. Buffers gerados/origem F/R, transforms, fade, limpeza e consumo final não estão cobertos por ABI de persistência ou gameplay dessa coleção.

O detector AI precisa distinguir o owner realmente efetivo, e a ponte Dual precisa de ordem direta demonstrada ou contrato cooperativo autorizado. A opção mínima futura é corrigir essas fronteiras no código próprio após recuperar/qualificar a fonte correspondente, sem alterar algoritmo/quotas/probabilidade. Um provider novo no Dual exigiria permissão do autor; não foi criado. Apenas ordenar manualmente não prova o detector AI quando há outros wrappers não nativos. O guard atual recusa wrappers Time Wizard desconhecidos; não liberamos a coleção inteira pelo nome.

### Verificações possíveis executadas

- Recebido e relink parcial: controllers ELF32 reais open_wheel/card_use/roulette_done, com transporte gráfico e serviços externos simulados: testes Encore e uso direto PASS nos dois objetos.
- Testes recebidos:672 combinações de decisões,5.120 de seleção física de mão e18 asserts de evolução/compatibilidade PASS. Parte desses testes usa helpers locais/modelos; não são todos planners de produção.
- Compilação API24 dos quatro adapters do relink e do C de AI separado PASS;103 imports resolvidos. Núcleo original não foi recompilado.91 funções do prefixo preservadas na comparação mascarada.
- Roda manual, replay/cancel/ID/fresh OFF e cache antigo ON→OFF: fonte real Encore junto ao Fusion atual, nas96 composições; falhas de qualificação AI/Dual conservadas na matriz separada.
- **NOT_REACHED:** animação e render completos, input Windows X/Quadrado, tempo de2s/10s e velocidade final, transformações/destruição completas com providers atuais, tentativas/savestates cross-processo, INIT/frame/shutdown do Time Wizard inteiro com todos os owners.

Sem fonte suficiente, duas opções honestas: obter o núcleo/build exatos e recompilar integralmente, ou autorizar futuramente uma experiência de runtime legado estritamente pinada, mantendo-a fora da versão integrada até qualificação. Não reconstruímos regras por suposição, não alteramos chances33,33%/50%, seleção manual, sons ou mecânicas aprovadas.

## Resultados, falhas e limites

[Resultados](hal-26/RESULTADOS.json), [matriz de checks](hal-26/MATRIZ.csv), [capacidades](hal-26/COMPATIBILIDADE.csv), [riscos](hal-26/RISCOS.json). Archives por suíte contêm CSV/JSON/logs de texto, com hashes. Contagens de processos têm cenários sobrepostos; não são duelos reais únicos.

| Grupo | Processos/checks | Resultado |
|---|---|---|
|B quieto/planner/consumo/quarentena, H/D×guard, Field ON|48/192|PASS|
|B +negativos de ordem efetiva com guard ON|20/92|PASS|
|Manager/restart/cancel/falha/reload|24/96|PASS; restart de plataforma modelado|
|Coleção/bonus +Time Wizard consumer probes +OFF residente|96/1.848|Checks de diagnóstico PASS; qualificação TW separada abaixo|
|**Coleção total pertinente**|**188/2.228**|**188 PASS**|
|A Queen+lease/rejeições e buffers gerados/guard|96/472|56 PASS/40 REFUSED de diagnóstico; load completo INCOMPLETE|
|Qualificação fonte/consumidor TW nos mesmos96 processos|96|**36 PASS/48 FAIL/12 REFUSED**; não adicionar como processos novos|
|Manager real, todos os masks ativo/residente|1.048.576 perfis|PASS; ausência de dependência do guard|
|Controle portátil de política|88 combinações|PASS sob ELF32/QEMU; PE32 só compilado|
|Load literal comum|65 casos|PASS para recusas/validação legada; nenhuma restauração bem-sucedida|
|Preflight tipado A|29 casos|PASS; inputs intactos; coleção inteira sempre INCOMPLETE|
|Storage oficial +chunks/mod-set|4 grupos|PASS no host; não gameplay/UI Windows|
|Build completo Windows i386/API24|628 unidades|PASS; executável privado|

R/RD/F/M e os objetos Field/AI não mudaram: não repetimos os rebuilds e todas as suítes isoladas já passadas no PR29. Suas evidências/hashes e falhas históricas permanecem preservadas. A repetição concentrou-se no manager, estado e composição relevante. O FAIL histórico de auditoria RD não foi removido nem declarado corrigido por esta entrega.

Falhas de desenvolvimento/ambiente foram separadas: primeira matriz B misturou os asserts de ordem dos casos4/5 com guard OFF, dando16 falhas de expectativa de uma proteção explicitamente desligada. Conservamos o log separado; a matriz final compara os casos de estado1–3 nos dois guards e reexecuta todos os asserts de ordem com guard ON. As primeiras montagens do probe TW usaram header SDK à frente do header core e não preencheram spin_id no mock do wheel; corrigimos as fixtures, não o MOD. Não contamos builds/perfis incompletos como PASS.

Os testes oficiais de slots/menu usam I/O real temporário e mantêm Werror no build default do CMake. A tentativa extra `-O1 -Werror` com GCC acusa format-truncation em save_slots.c. O mesmo problema foi reproduzido no fonte byte-idêntico do PR29, e os dois FAIL de build otimizado estão nos logs; não alteramos armazenamento nem suprimimos warning. Isso não foi convertido em falha funcional desta migração.

## Windows e recursos externos

[Status do runner](hal-26/windows/RUNNER_STATUS.json): a página pública do Actions informa **“Workflows are not currently being run on this fork due to the scale of GitHub Actions usage. A repository maintainer can choose to re-enable these.”** A consulta do commit do PR30 retornou zero execuções. Não reativamos globalmente Actions/permissões nem repetimos Wine/QEMU já indisponível.

O controle próprio foi compilado em PE32/i386 e verifica ponteiro4 bytes e cabeçalho I386 quando executado. O workflow usa MSVC x86 apenas para esse C portátil, não para compilar o jogo. **AUTOMATION_UNAVAILABLE** para execução Windows; build/identificação PE32 não são runtime PASS. Ativação seletiva desse workflow por maintainer ou runner Windows legitimamente disponível permite avançar sem o usuário jogar.

[Varredura atual](hal-26/windows/RESOURCE_SCAN.json):494 `.bin` pequenos, máximo76.800 bytes, nenhum candidato ISO/CUE/CHD acessível. Não presumimos acesso a imagem enviada em outro contexto. Imagem legítima deve ficar em caminho/recurso privado autorizado; nunca no Git/artefatos públicos. Mesmo com a imagem, a execução completa requer backend funcional. Somente gameplay com participação pessoal do usuário fica para a fase final; nada foi bloqueado por proibição ampla de automação.

## Próximo passo mínimo

Revisar B como prevenção na candidata e manter load recusado nos perfis incompletos, sem promover a política automaticamente. Para A, fechar os esquemas de efeitos/pools e o preflight de arquivo/lifecycle antes de permitir qualquer overwrite. Para Time Wizard, recuperar/qualificar o núcleo exato, resolver os dois contratos identificados e então exercitar controller/quotas/Queen/transformações/render com os atuais providers, em composição privada e por hashes.

DLC/Sanctuary seguem fora do escopo como recursos futuros. Droplist1.4.0 externo permanece separado/ausente; viewer interno RD continua removido. Adapter AI0.1.1 externo e outros opcionais continuam não qualificados. A coleção atual está preservada e sua prevenção de perda de estado ficou mais forte; persistência completa e integração Time Wizard ainda não estão autorizadas como versão final.
