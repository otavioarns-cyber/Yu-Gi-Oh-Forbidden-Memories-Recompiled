# HAL-20 — Consolidação das correções e qualificação da coleção

**Entrega experimental, sem merge/release.** Branch `test/hal-20-consolidacao-candidatos`, derivada de `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`. Engine oficial `aceac3782902746f35ddb0148809450933327cb3`, v0.3.1-preview.2 / SDK API24 / alvo Windows i386. Registro de 14/11/2026.

## Resultado e respostas principais

**A composição M + R + RD + Fusion F1 + AI HARD MODE + Dual Play, com Drop Missing Cards1.0 ativo, funciona nos caminhos exercitados pelo host parcial e pelo controller nativo selecionado. O guard atualizado admite seus perfis seguros.** Há seleção Queen obrigatória, categoria/quota conferida no Dual, conservação de identidades físicas e uma materialização por ação nos ramos observados. Isso não certifica um duelo completo Windows, toda estratégia ou qualquer wrapper externo.

**846 processos candidatos: 717 PASS, 112 REFUSED e 17 NOT_REACHED; 5.829 checks PASS, sem FAIL funcional da candidata nas matrizes finais.** Os checks incluem recusas e reproduções diagnósticas: não significam846 duelos aprovados. Separadamente, quatro controles com RD do PR26 reproduzem oito FAIL funcionais esperados; a auditoria histórica de imutabilidade RD permanece FAIL esperado. O load legado de efeitos tem falha funcional reproduzida, mitigada pela recusa explícita no perfil candidato.

O que permanece:

1. **Load de savestate não é seguro para esta coleção**, inclusive em fronteira quieta: restaurar memória/chunks e depois resetar os MODs perde o estado temporário Queen. A candidata recusa o load antes de parsing/remap/overwrite enquanto algum dos quatro MODs próprios está efetivamente ON e o guard opt-in está ativo. Suporte completo de load ainda exige contrato de estado/efeitos e revalidação por identidade; não foi inventado aqui.
2. **Backend PE32 indisponível neste ambiente.** Novo Wine11 e Wine10 sob QEMU x86_64 passam controle PE64, mas PE32 mínimo e jogo não iniciam. UI, reinício real, duelos completos e load integral são NOT_REACHED por ambiente/recurso, não por decisão do usuário. Imagem legítima não foi fornecida.
3. **Materializadores não cooperativos depois da última validação continuam fora da garantia.** Os wrappers atuais foram ligados a objetos/callbacks exatos. Novos componentes precisam de qualificação própria; a tolerância do guard a hooks não críticos não os torna compatíveis.
4. Time Wizard, Field Effects e Droplist externo ainda não integram a composição qualificada. Dual continua privado/local. Não se misturaram essas pendências com correções de mecânicas.

Não é necessário abrir outra PoC para os quatro pacotes próprios atuais: eles podem permanecer numa **única candidata integrada experimental** com estes limites. Antes da versão completa faltam a política definitiva de estado/load, a qualificação dos componentes adicionais, execução automatizada em backend funcional com recurso legítimo privado e, por último, a bateria pessoal do usuário.

## Referências e composição exata

Foram lidos AGENTS e relatórios/entregas [#20](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/20), [#21](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/21), [#22](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/22), [#23](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/23), [#25](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/25), [#26](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/26) e [#27](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/27). Inventário original preservado na [branch documental HAL20](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md).

| Papel | Versão/origem | SHA256 do objeto canônico | Loader code |
|---|---|---|---|
| M — Melhorias/Surrender |1.3.6, PR9 `0d28d844`|`c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0`|`1fe34a1e`|
| R — Ritual Effects |0.78.74-effect-ai, PR27 `1f3fb6f1`|`78ec3ce45694b1f583213530dfc0a527c8974056f67c49e7791fb6821f55ab45`|`1e478c68`|
| RD — Ritual Duelists |0.4.0-alpha.8, PR27 `1f3fb6f1`|`9f0d7354e6ff3f44776308b18e96b960480500c814ae22c3e7d1e6b57eafea34`|`3d26698f`|
| F — Fusion Effects |0.1.0-beta.44-effect-ai + Revogar, PR22 `e089a10b`|`384e11eeeae32412e23afbbbd9ebc308761674b8a2181238fd60e17e315b865c`|`afb75202`|
| H — AI HARD MODE |oficial da base/API24|`9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc`|`3e0648cb`|
| D — Dual Play |0.5.7 API24 do PR10, privado/local|`77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`|`3028c68a`|

[OWNED_TREES.json](hal-20-consolidacao/OWNED_TREES.json) verifica **477 arquivos idênticos** aos respectivos commits: M21, F95, R203, RD158. Inclui todos os fontes, objetos, configurações, receitas, testes, imagens e buffers autorizados; nenhuma mecânica, Queen Lock, estratégia, setting ou asset desses pacotes foi alterado. IDs permanecem originais, inclusive `ritual-card-effects-test`.

Não houve merge/cherry-pick dos PRs anteriores. As árvores autorizadas foram incorporadas à branch nova, e os commits de proveniência constam do recibo. Master, integração e32 branches remotas anteriores são preservadas. [Auditoria](hal-20-consolidacao/AUDIT.json).

### Auditoria da HAL25 e Fusion

O patch cursor/pilha do PR27 permanece exato: RD compara cursor nativo, pilha/quantidade e ordem de `DuelHandSlot.active_09` antes da coleta, além do contrato público de seleção. Não foi substituído por seleção por ID nem por um wrapper ad hoc. ABI1 e companion continuam byte-idênticos entre R/RD; o contrato público **ainda é candidato**, não promovido.

As regressões repetem callback ativo→disconnect/ACK→PROT_NONE, reset/OFF/reentrância, geração,3 tentativas inválidas, BLOCKED/quarentena, recuperação explícita por sequência/identidade e exaustão. Não há retomada automática do pending vetado. A UI definitiva de recuperação não foi criada; consumidores podem observar o companion e pedir recuperação explicitamente. Quarentena tardia exige novo duelo; EXHAUSTED exige processo novo.

Fusion é exatamente a candidata Revogar do PR22. OFF **efetivo**, não preferência salva, neutraliza Encore/consultas de lease e revoga somente `aux_unlocks`, conservando serial, efeitos/regras ON e providers residentes. Reativação não recria lease antiga; nova concessão recebe serial novo. Cache de uma tentativa Time Wizard já iniciada não foi reescrito nem declarado resolvido.

## Guard atualizado: o que foi implementado

Mantém a chave opt-in `experimental.hal2224_order_guard=1`, default OFF, e o modelo do PR20. Não altera globalmente a semântica de `Hooks_Add`, `Hooks_Relink` ou `Mods_Order`.

1. Guarda SHA256 completo da biblioteca lida, antes de sua inicialização, além do code hash do loader.
2. Consulta somente leitura `Hooks_Inspect`: owner, target, replacement, predecessor efetivo e corpo nativo. O arquivo original de gerenciamento de hooks permanece integralmente igual, acrescido dessa consulta.
3. Exige o objeto exato e o endereço/tipo/tamanho/símbolo esperado do callback, quantidade esperada de registros, ausência de duplicatas e predecessor correspondente à cadeia instalada. IDs localizam papéis; **não qualificam código**.
4. Protege `func_8001BAF0`, `DuelScene_UpdateHandActions` e `AiScript_Run`. H deve registrar antes de RD; ambos antes de D quando presentes. A ordem efetiva de chamadas é o inverso: Dual valida antes dos materializadores internos. R/M/F podem envolver essas camadas somente como callbacks das versões qualificadas.
5. Conexão opcional ABI1 ausente/desconectada é válida; quando conectada, o evaluator deve pertencer ao R qualificado, com shape/tamanho correto. Provider residente efetivamente OFF pode responder neutralmente.
6. Mantém validação de preferência/ordem no manager e janela e fence real em `Main_RunDuel`, antes de init/controller, sem desligar MODs silenciosamente. Predecessores reais são conferidos em Ready; uma projeção de ativação não é confundida com cadeia já instalada.

[Qualificação de objetos/callbacks](hal-20-consolidacao/QUALIFIED_ARTIFACTS.json), [inventário de wrappers](hal-20-consolidacao/wrappers/WRAPPERS.csv), [grafo, delegação e métricas](hal-20-consolidacao/wrappers/WRAPPERS.json): **125 nós, zero sem vínculo**,72 perfis na execução final do fence. Callbacks nunca exercitados continuam `REGISTERED_NOT_RETURN_QUALIFIED`; métricas `self` excluem filhos críticos instrumentados, não toda chamada C. Nenhum nome/ID, registro isolado ou hash sozinho certifica todos os ramos.

Os negativos recusam ordem invertida, callback errado/duplicado, predecessor corrompido, wrapper crítico desconhecido e Fusion de ID/código executável iguais com bytes extras no arquivo. O último demonstra que não basta o hash curto do código. As recusas ocorrem antes do controller/materialização nos respectivos testes.

**Limites:** não é sandbox contra código nativo malicioso, verificação contínua de toda instrução ou whitelisting global de todo hook. Bibliotecas adicionais com hooks apenas fora dos três alvos podem não ser recusadas por esse guard; **não estão qualificadas**. O stager monta exclusivamente os sete IDs previstos. Não acrescentar outros pacotes e interpretar ausência de recusa como aprovação. Ainda falta testar opções/ramos e plataformas além do perfil i386 observado.

## Ordem e experiência de uso

O stager usa DMC−100, M−50, Fusion−30, R−20, H0, RD10 e D20, respeitando também `after`. A dependência declarativa de R pode colocá-lo depois de H apesar do rank; logs registram a ordem real, não apenas os números. A restrição relevante é registro H→RD→D; não é uma nova prioridade estratégica da IA.

AI recebe `restart:true` **somente na cópia local experimental**, sem tocar seus fontes/manifest no repositório. R/RD/F mantêm seus manifests. Primeira ativação tardia do AI fica diferida; cancelar confirmação não salva preferências; falha de reinício fica visível e o estado efetivo continua sujeito a Ready. Testes de falha/cancelamento usam o código real da janela e um stub do resultado de plataforma; não equivalem a reinício Windows.

[stage.py](../tests/hal20-consolidacao/stage.py) verificou sete IDs únicos e hashes exatos numa pasta privada fora da árvore Git. O recibo local inclui a diferença do manifest AI, sem alterar o pacote Dual. Default/settings estratégicos são preservados. DMC1.0 ficou ON nos testes de balanceamento RD; seu `after` continua ordenação, não dependência funcional.

## Materialização e testes combinados

| Matriz final | Processos | Execução | Checks PASS |
|---|---:|---|---:|
|Coleção, R/F em duas posições, H/D OFF/ON, ranks seguros/invertidos, guard OFF/ON|144|120 PASS /24 REFUSED|1172|
|Todos os63 subconjuntos, guard ON|63|56 PASS /7 NOT_REACHED|519|
|Providers reais nos63 subconjuntos|63|63 PASS|382|
|Load/fronteiras/negativos/delegação/estado efetivo|20|20 PASS|92|
|Janela, cancelamento/falha de restart, late AI/reload|24|24 PASS|96|
|Arquivo Fusion adulterado em cópia descartável|4|4 REFUSED|8|
|Fence **Main_RunDuel real** com guard ON|72|48 PASS /24 REFUSED|592|
|Regressão contrato/controller|174|174 PASS|1240|
|Hardening ABI/lifecycle|72|50 PASS /12 REFUSED /10 NOT_REACHED|346|
|Fronteiras, coleta combinada, rituais|128|80 PASS /48 REFUSED|776|
|Cancelamento/recuperação|64|64 PASS|464|
|Reset após quarentena|16|16 PASS|128|
|Contrato de posições por instância|2|2 PASS|14|

[Matriz por check](hal-20-consolidacao/MATRIZ.csv), [agregado e hashes dos arquivos de logs](hal-20-consolidacao/RESULTADOS.json). Os archives por suíte contêm JSON completo, CSV e logs crus.

O planner RD real produz pending nos caminhos principais, sem injeção de planos/pending/quotas. Controller, seleção física, materializadores, gate Dual e ledger são observados. Rituais Field/Both consomem3 materiais e preservam a cópia física não selecionada; Hand-only é recusado pelo default `min_field=1`, que não foi relaxado. Vanilla é controle separado. Após commit, não há restauração fictícia de cartas.

Propostas combinadas artificiais, falhas de identidade/cursor/pilha, interrupções dentro de callbacks, leases semeadas77 e contadores próximos de exaustão estão identificados como instrumentação. Não são prova de alcançabilidade natural. As recusas sob wrappers externos e materiais inválidos não são classificadas como gameplay bem-sucedido.

Os7 subconjuntos NOT_REACHED não tinham H/RD ou obrigação Queen para produzir a primeira ação neste host com VM retail incompleta. Nos10 NOT_REACHED do hardening a janela adversarial não foi alcançada. Presença/provider/reset podem ter passado no mesmo processo; isso não transforma a ação não alcançada em PASS.

### Regressões originais e controles

R:23 PASS/3 SKIP. RD:34 PASS/1 **FAIL de auditoria imutável esperado**/3 SKIP. Os testes originais não foram editados; o loader foi qualificado para9 hooks só em cópia temporária, mantendo demais asserts. O FAIL de pacote compara com os arquivos anteriores à alteração autorizada HAL25, não indica erro de gameplay nem foi ocultado.

Fusion:16 grupos PASS,214 asserts de caracterização; F1 OFF neutral confirmado. M: suíte preview2 PASS, incluindo128 combinações de fonte/lado,15 cenários de provider, seleção/consumo de instâncias e20 ciclos loader/reset/OFF/ON. Hooks oficiais e manager transacional oficiais passam contra o core atual, com guard default OFF. Logs e resumos acompanham esta pasta.

**Controles históricos:** [quatro processos PR26](hal-20-consolidacao/CONTROLES_PR26.json) mantêm8 FAIL funcionais sob o wrapper adversarial de cursor/pilha, conforme PR27. A candidata atual recusa antes de consumir. Não enfraqueceram testes nem se afirmou que tais estados eram gameplay normal.

Ocorreram erros de montagem do novo harness durante desenvolvimento: Fusion era copiado sem a lista de imports nos modos antigos e recebia enabled default; faltava recibo PR13; o marcador interno negativo de subconjuntos era confundido com badmode. Foram corrigidos **nos testes**, sem workaround nos MODs, e as suítes finais foram reexecutadas integralmente. Perfis vazios/incompletos não foram contados como PASS. A primeira coleta do grafo não vinculava o objeto nativo novo do fence; o runner passou a registrar seu ELF real e a matriz final foi repetida, chegando a zero nós sem vínculo.

## Save/load: alternativas e decisão experimental

O caminho real de `state.c:load` restaura RAM/regiões/chunks e depois chama `Mods_Reset` e LOAD_AFTER. R e Fusion não serializam todo o estado temporário de efeitos/leases. RD registra um bloco de identidade, não uma persistência transacional completa.

| Alternativa | Evidência/classificação | Decisão nesta branch |
|---|---|---|
|Permitir load apenas com pending vazio/fronteira quieta|**FAIL funcional**: commit Queen real→snapshot de RAM/dados/chunks→ordem de restore/reset/event→evaluator público deixa de exigir a carta obrigatória|Insuficiente; não adotada|
|Serializar/restaurar BSS de MODs ou recuperar cartas por ID|NOT_REACHED e sem contrato de persistência válido|Não implementada|
|Limpar pending depois de restaurar/consumir|Não fornece rollback real nem preserva efeitos aprovados|Não implementada|
|Recusar antes de tocar o estado com guard e M/R/RD/F efetivamente ON|REFUSED, checks PASS nos limites quieto/planner/consumo/quarentena e preferência OFF diferida|Mitigação experimental adotada|

A reprodução quieta usa a memória guest completa e o armazenamento de dados do host modelado, mais chunks realmente registrados; **não é um arquivo nativo completo nem um teste Windows de savestate**. Mesmo sem modificar cartas entre snapshot/restore, o reset perde a obrigação pública, demonstrando que um teste só de pending é insuficiente. No teste com consumo o load é somente recusado; nenhuma carta já consumida é recriada.

A recusa é comum à UI/F7 e ao canal de controle, ocorre antes de fseek/parse/remap e informa que o jogo atual foi mantido. Usa estado aplicado, não `host->applied()`/preferência salva. Não modifica saves normais do memory card nem desativa o botão de salvar estado globalmente. Arquivos de estado antigos não se tornam compatíveis. Com guard OFF o caminho legado permanece: **a falha diagnosticada não está corrigida nesse modo**. A escolha permanente entre recusa e suporte de persistência ainda precisa de revisão; esta branch não a promove à versão final.

## Build, proveniência e Windows

Build completo Windows i386/API24: **PASS**,628 unidades de jogo. [Recibo e hash do executável](hal-20-consolidacao/BUILD_WINDOWS.json), [log](hal-20-consolidacao/build-windows.log). O executável/SDL/SDK não são publicados.

R/RD foram recompilados integralmente em cópias novas com SDK oficial, --target i386. Os hashes de arquivo diferem por metadados não alocados/debug/caminho; **todas as seções alocadas, funções e relocations são idênticas**, imports/exports inalterados e presentes no SDK. [Equivalência](hal-20-consolidacao/REBUILD_EQUIVALENCE.json), [ABI R](hal-20-consolidacao/ABI-ritual-card-effects.json), [ABI RD](hal-20-consolidacao/ABI-ritual-duelists.json). Mantiveram-se os objetos canônicos testados do PR27, sem misturar versões nem aceitar automaticamente outro SHA no guard. Os mínimos públicos4/12 de R/RD foram preservados; SDK API24 não obriga mudar o manifest mínimo arbitrariamente.

Wine11.0 WOW64 novo, release e SHA conferidos: prefixo e PE64 PASS; PE32 mínimo e jogo falham antes de main com erro731. Chamadas diretas com cache gravável também não entram em PE32. Wine10 sob QEMU x86_64 foi nova tentativa adicional: PE64 passa, PE32 falha. [Evidências Windows](hal-20-consolidacao/windows/RESULTADOS.json), [controle direto](hal-20-consolidacao/windows/DIRECT.json), [procedimento](../tests/hal20-consolidacao/windows/README.md). Isso é **AUTOMATION_UNAVAILABLE**, não FAIL funcional dos MODs. O preview Linux estático encontrou dependências de desenvolvimento ausentes; não foi usado para alegar UI validada.

Sem imagem legítima fornecida, mesmo um backend corrigido ainda dependeria desse recurso privado para completar duelos. Não foi solicitada bateria manual intermediária nem baixada imagem de jogo. Toda automação possível continua autorizada, deixando apenas a participação pessoal jogando para a última fase.

## Componentes adicionais e próximos passos automáticos

[Inventário adicional](hal-20-consolidacao/ADICIONAIS.json):

- **Field Effects0.19-adaptive-description-test:** fonte público da branch `mod-field-effects`/`296c5919`, ausente do ZIP atualizado. Foi compilado em cópia local com SDK24/i386, PASS; não foi integrado nem publicado objeto. Seus hooks de setup/terreno/texto e provider `terrain-card-delta-v1` precisam de regressão de instâncias/stats/IA/textos com a coleção. Compilar não demonstra versão mais recente nem compatibilidade funcional.
- **Time Wizard0.24.0-test:** adapters disponíveis, núcleo0.23.1 compilado sem fonte integral, relink com offsets e bridge Dual por assinaturas de instruções. Não tratar como recompilação integral API24. Resolver proveniência/fonte ou contrato autorizado; testar cache de tentativas/Encore/origin no mesmo conjunto antes de incluí-lo. Nenhuma mudança ao Time Wizard foi feita.
- **Dual0.5.7:** objeto local exato testado, terceiros intactos, publicação impedida pela ausência de permissão verificável. Isso não impede continuar testes locais automatizados.
- **Droplist1.4.0 externo/adapter AI0.1.1:** recursos não recebidos para qualificar; o visualizador interno removido de RD não foi reintroduzido. DMC não é substituto do Droplist.
- Outros MODs oficiais e coleção visual completa ainda não qualificados por estes testes. Nome oficial ou ausência de hook crítico não equivalem a aprovação.

Pode ser feito automaticamente, sem envolver o usuário: continuar regressões combinadas nesta mesma composição, qualificar Field Effects a partir de fonte disponível, desenvolver/testar uma política real de estado a partir da falha demonstrada, repetir PE32 em backend legítimo funcional e automatizar duelos quando o recurso privado existir. Recursos ausentes e direitos de terceiros são dependências reais; não se resolvem declarando PASS ou reutilizando binário antigo.

## Arquivos de produção e limites de publicação

Pacotes próprios:477 arquivos originais de referência, sem mudanças funcionais nesta consolidação. Créditos/permissões preservados em NOTICES: moeda PeterIron autorizada pelo usuário; arte RitualStarter doada pelo amigo, conforme declaração em resposta à consulta de publicação. Não se inventou licença aberta ou autoria adicional.

Core experimental alterado: `src/pc/mods/manager.c`, `mods.c`, `mods.h`, novo `hal20_qualified.h`; consulta somente leitura em `hooks.c/.h`; fence `src/game/main_run_duel.c`; pré-validação `src/pc/platform/mods_window.c`; recusa de load `src/pc/guest/state.c`. Helpers de build `tools/pc/test_mods_lifecycle.py`, `preview_mods.sh` e `preview_panels.sh` recebem o SHA256 já existente como dependência de link. Público `modapi.h`, código AI/DMC e implementação anterior de ordenação/relink permanecem inalterados.

Testes próprios em `tests/hal20-consolidacao`, relatórios, matrizes, hashes e logs em `notes/hal-20-consolidacao`. Nenhum código, manifest ou binário Dual, imagem/ROM, save pessoal, Wine/runtime ou binário do jogo é publicado. A branch e o PR são experimentais; não houve merge, release ou promoção automática de ABI/guard/política de load.
