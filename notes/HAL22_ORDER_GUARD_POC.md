# HAL-22 — prova de conceito do guard de ordem AI HARD MODE × Dual Play

**A PoC impediu a execução de I2 em todos os cenários protegidos exercitados: 51 processos, 1834 verificações PASS, zero FAIL/ERROR. Entradas inseguras foram recusadas; o protótipo não reordena nem corrige os materializadores.** Os controles sem proteção continuam reproduzindo I2. Não há aprovação para adoção definitiva ou compatibilidade Windows comprovada.

Branch `test/hal-22-order-guard-poc`, derivada da integração oficial em `d649647c159c91d615c07d6960928a37e270b461`; núcleo `aceac3782902746f35ddb0148809450933327cb3`, API 24. `AGENTS.md` e relatórios/matrizes/logs dos PRs [#14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14), [#15](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/15) e [#16](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/16) foram lidos. Referências preservadas e hashes em [INPUTS.json](hal-22-order-guard/INPUTS.json). Escopo exclusivo de I2; I1, I3 e F1 não foram corrigidos ou incorporados à matriz funcional.

## Respostas para revisão

**1. A proteção eliminou I2 em todos os cenários exercitados?**

**Sim, no conjunto protegido e nos limites do host parcial.** Nos duelos admitidos, o Dual recebeu a validação/snapshot da categoria e o AI executou sozinho a materialização ativa; quotas e consumo físico corresponderam às ações. Foram observadas 22 recusas de entrada em oito processos, sem avançar o corpo de `Main_RunDuel`. Ranks invertidos, empate que descobria Dual primeiro e registros inseguros de um processo legado não puderam entrar em duelo.

Isso significa **prevenir a execução pela recusa**, não tornar uma cadeia invertida funcional. O usuário precisa corrigir o perfil/reiniciar quando indicado. Os 24 SKIP protegidos correspondem a situações em que a suíte conjunta não devia rodar, por recusa ou composição individual/pending; os caminhos individuais do materializador foram verificados por probes. Nenhum SKIP foi chamado de gameplay aprovado.

**2. Quais mudanças de código foram necessárias?**

Cinco arquivos de produção, exclusivamente nesta branch experimental, totalizando 128 linhas adicionadas e uma removida:

| Arquivo | Mudança experimental |
|---|---|
| [`src/pc/mods/manager.c`](../src/pc/mods/manager.c) | Guard opt-in para ordem resolvida/salva ou staged; projeção do estado realmente ativo após alterações; consulta somente leitura a `Hooks_At`; checagem antes de `Mods_Apply` escrever preferências |
| [`src/pc/mods/mods.c`](../src/pc/mods/mods.c) | Preflight antes de dados/bibliotecas no startup/recarga; rejeição explícita na API direta `Mods_SetEnabled`, preservando escolhas existentes |
| [`src/pc/mods/mods.h`](../src/pc/mods/mods.h) | Quatro declarações internas do experimento; nenhuma alteração no layout da API de MODs |
| [`src/pc/platform/mods_window.c`](../src/pc/platform/mods_window.c) | Validação dos **ranks staged** antes de salvar ou pedir reinício; evita avaliar a proposta usando só os ranks antigos |
| [`src/game/main_run_duel.c`](../src/game/main_run_duel.c) | Fence PC antes de init/controller, com aviso explícito uma vez por período bloqueado; não avança o duelo inseguro |

[Diff integral](hal-22-order-guard/DIFF_EXPERIMENTAL.patch), conferido com `git apply --check` contra a base exata e `git apply --reverse --check` contra o protótipo. Esses checks não alteraram outras branches. `Hooks_Relink`, `Mods_Order` e `order_ranked` permanecem com sua implementação/semântica original; `hooks.c` é byte a byte igual à base. Nenhuma chamada a `previous_swap` foi acrescentada. Fontes, manifest e objeto do AI e todos os arquivos Dual permanecem intactos.

**3. É possível adotar sem prejudicar os outros MODs?**

Os ensaios sustentam uma proteção localizada: **3072 comparações** com dez manifests reais conservaram exatamente a mesma ordem com guard ligado/desligado e após validar ranks staged. A regra rejeitou somente 256 combinações com a relação H/D insegura e aceitou 2816, incluindo 2304 composições em que um deles estava OFF/ausente do conjunto habilitado. A suíte oficial de hooks passou. Dois observadores próprios intercalados e um alvo independente conservaram delegação e OFF/ON.

Os manifests próprios utilizados foram Melhoria/Surrender, Ritual Effects, Fusion Effects e Ritual Duelists; oficiais adicionais: Drop Missing Cards, 3D Monsters, Hand Camera e Yamyi. São regressões de **ordenação e isolamento**, não execução integrada das mecânicas desses objetos. Não se afirma ausência de todo conflito em gameplay.

Existe um impacto deliberado a revisar: se o startup do perfil for recusado, **a transação inteira de dados/código fica sem inicialização** e o duelo permanece impedido até relançar. Não se faz um fallback para Vanilla nem se desliga um integrante silenciosamente. Em perfil válido ou com opt-in desligado, a ordem dos demais MODs não é reescrita. Essa política de recusa precisa de revisão de UX antes de adoção.

**4. Quais efeitos terá o reinício obrigatório?**

O candidato `restart: true` continua somente em cópias temporárias. Com ele, ON/OFF do AI salva a preferência, mas o estado ativo só muda no processo novo; a UI pede confirmação e avisa sobre progresso não salvo. Cancelar não aplica a mudança. Se o relançamento falhar, as preferências persistem e a cadeia antiga não é apresentada como reconstruída.

Se essa cadeia antiga for insegura, o fence impede o duelo até corrigir/reiniciar; novo duelo e reset não contornam a recusa. Se só um dos dois estiver efetivamente ativo, sem startup anteriormente recusado, o guard permite essa composição individual — inclusive enquanto o outro aguarda reinício — sem afirmar que a nova preferência já está aplicada. Os cenários apenas AI/apenas Dual passaram mesmo com ranks que seriam inseguros para o par.

Depois de um startup recusado, reparar o perfil na janela permite salvar a correção, mas não inicializa parcialmente a transação antiga; exige relançar. Isso é uma condição explícita do perfil recusado, não um bloqueio de perfis individuais iniciados corretamente.

**5. O que impede a adoção definitiva?**

Faltam execução Windows i386/API 24 com imagem legal local, catálogo/decks reais e partida completa; confirmação visual do erro, menu acessível durante a recusa e execução efetiva do relançamento da plataforma; integração com os objetos dos demais MODs; regressões de importação/atualização, overrides, falha de gravação e saves em escopo próprio. A UI/restart são observados por stubs neste host.

Também faltam revisar a política de recusa atômica, as mensagens/idioma, exposição da opção e seu armazenamento em perfis. A chave experimental ainda não é exportada pelo salvador de perfis `mod.*`. A identificação por IDs/registro não é uma certificação de versões futuras: os resultados se referem aos objetos com hashes conferidos. O fence fica no caminho PC `Main_RunDuel`; caminhos alternativos introduzidos por outros códigos nativos não foram certificados. Esses limites impedem tratar a PoC como correção definitiva.

## Como a proteção funciona

O opt-in é `experimental.hal22_order_guard=1` nos settings do ambiente de teste. Default: desligado. Nenhum MOD ou perfil padrão é alterado automaticamente.

1. **Proposta de configuração:** quando o par é solicitado, o guard usa o ordenador existente para exigir AI antes de Dual na ordem **efetiva**. Não muda ranks ou dependências para obter esse resultado. Empates são aceitos somente quando a ordem resolvida já é segura; se outra ordem de scan os inverter, o próximo startup os recusa.
2. **Transição ao vivo:** consulta registros já existentes e projeta o estado ativo, considerando que um MOD com reinício pode continuar ativo mesmo após preferência OFF. Não permite reativar Dual em uma cadeia conhecida como invertida só porque a preferência do AI já diz OFF.
3. **Startup:** uma ordem recusada retorna antes de inicializar dados ou bibliotecas, conserva preferências e registra a recusa. O processo fica marcado para relançamento; não tenta reparar objetos ao vivo.
4. **Entrada/continuação do duelo:** `Main_RunDuel` consulta a cadeia realmente registrada. Ambos ativos exigem um registro verificável de cada um no swap e AI registrado antes de Dual, resultando em Dual externo. Uma cadeia legada insegura, mesmo com ranks já reparados, recebe aviso e não executa init/controller.

A cadeia estrutural esperada é Dual→AI→motor. Na execução com AI ativo e `preserve_hand=1`, o worker AI substitui o nativo; não se chama o motor uma segunda vez. No fallback AI OFF, o Dual delega ao worker nativo. A PoC não conhece nem modifica o estado privado de quotas do Dual: ele continua sendo responsável pela autorização e contabilização; o AI mantém seu planejamento e sua política de seleção física.

## Resultados e comparação com a mitigação manual

| Configuração | Processos | PASS | FAIL | BLOCKED | SKIP |
|---|---:|---:|---:|---:|---:|
| Guard desligado, controles | 45 | 1908 | 158 | 36 | 9 |
| Guard ligado, incluindo ativação sobre processo legado | 51 | 1834 | 0 | 0 | 24 |
| Total | 96 | 3742 | 158 | 36 | 33 |

Mais três verificações entre perfis/suítes passaram: ordenação real, suíte oficial de hooks e igualdade das escolhas iniciais. Os 158 FAIL e 36 BLOCKED pertencem **somente aos controles sem proteção**, repetindo I2/limpeza cujo gate não executou. Não são falhas novas do guard ou 158 bugs distintos. Não houve ERROR/incompletude. O modo `all` do runner mantém saída 1 por esses controles, deliberadamente.

Foram compilados cinco insumos próprios de teste: o harness, a unidade experimental de entrada do duelo, um objeto observador próprio, o teste de ordem e o teste oficial de hooks. AI/Dual e as dez unidades anteriores da engine não foram recompilados. **49 preflights e 49 verificações pós-execução passaram.**

Em 63 processos a suíte conjunta pôde executar (36 controles e 27 protegidos); as escolhas iniciais do planner foram idênticas: Support ID 344 e Monster ID 35. Os cenários protegeram categoria definitiva, quotas, rituais proibidos no bônus, cancelamento, consumo físico, troca offscreen e duas cópias com identidades distintas. Para plano aceito, houve um worker e uma troca por instância que precisava sair do deck; para os planos adversariais rejeitados pelo Dual, o worker não executou. As seleções adversariais após filtragem continuam injeções de teste, não decisões atribuídas ao planner.

| Aspecto | Só ordem manual + reinício (PR #16) | PoC opt-in |
|---|---|---|
| Startup H antes de D | Seguro no recorte testado | Admitido, mesma execução |
| Ordem invertida salva | Reconstrói I2 após reiniciar | Perfil recusado antes de inicializar bibliotecas |
| Empate resolvido D antes de H | Permite I2 | Recusa explícita; não muda o empate |
| Primeira ativação tardia com manifest original | Pode permitir cadeia insegura | Recusa a transição conhecida como insegura |
| AI com restart pendente | Aplica no processo novo | Conserva esse comportamento e verifica o resultado |
| Reinício cancelado/falhou sobre cadeia antiga insegura | Depende de não continuar o duelo | Fence impede continuar; preferências conservadas |
| Outros wrappers | Ordem de registro original | Mesma ordem; apenas consulta e recusa |

[Matriz por caso/fase](hal-22-order-guard/MATRIZ.md) · [CSV integral](hal-22-order-guard/resultados/MATRIZ.csv) · [Resultados/comandos/PIDs/logs](hal-22-order-guard/resultados/RESULTADOS.json) · [Reprodução](../tests/hal22-order-guard/README.md).

Exemplos: [startup empatado recusado](hal-22-order-guard/resultados/guard-on-equal-rank-dual-first-p0.log), [inversão rejeitada pela janela](hal-22-order-guard/resultados/guard-on-rank-invert-restart-p0.log), [cadeia legada e restart falho](hal-22-order-guard/resultados/guard-on-arm-repair-fails-p0.log), [processo novo corrigido](hal-22-order-guard/resultados/guard-on-arm-repair-fails-p1.log), [OFF pendente não contorna o guard](hal-22-order-guard/resultados/guard-on-arm-pending-off-p0.log), [wrappers intercalados](hal-22-order-guard/resultados/guard-on-observers-between-p0.log).

## Limites da prova e autorização

O harness executa manager/host/loader/hooks/settings reais e as rotinas de Apply da janela com o diff experimental. A unidade `Main_RunDuel` é compilada a partir do fonte **modificado**; os testes observam sua recusa antes do avanço do corpo. As rotinas selecionadas de controller/snapshot/planner/materialização executam os objetos anteriores. RAM, catálogo, renderização e ponte simples efeito/placement→campo permanecem parciais. O corpo completo de init do jogo, a VM retail quando AI OFF e a alcançabilidade com decks reais não foram validados.

`Platform_ShowError`/`Platform_RestartGame` são observadores de teste. O reinício do harness cria outro processo e lê o mesmo settings; não é o relançamento Windows. A análise dos callbacks/ordens foi preservada nos logs. Os campos ADMISSION/TRANSACTION/VISIBLE_ERROR do JSON foram enriquecidos a partir desses logs, sem repetir o código de jogo; os resultados das verificações não foram alterados.

Problemas do scaffolding identificados durante a construção foram corrigidos **nos testes**, não nos MODs: flags `_LANGUAGE_C`/`LANGUAGE_C` exigidas para compilar a unidade nativa; código de retorno do observador de confirmação ao rejeitar ranks; helper de Apply que precisava conservar as flags dos observadores. As compilações finais passaram com warnings preexistentes dos headers. Nenhuma falha foi contornada desligando a estratégia ou modificando Dual.

Esta branch publica o **código experimental** porque os critérios delimitados passaram e o diff é reversível. A autorização atual cobre revisão da PoC; não cobre habilitá-la permanentemente, promover a integração ou fazer merge. Nenhum arquivo de Dual (fonte, manifest ou binário), ROM, asset proprietário ou save pessoal foi publicado. Os objetos originais seguem como dependências locais por hash. Master, integração, PRs anteriores e demais branches são preservados, conforme [refs](hal-22-order-guard/REFS_PRESERVADAS.json) e [índice público/hashes](hal-22-order-guard/PUBLICATION_FILES.json).
