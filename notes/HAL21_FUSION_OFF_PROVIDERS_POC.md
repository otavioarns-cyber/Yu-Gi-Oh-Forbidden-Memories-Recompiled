# HAL-21 — Fusion Effects: providers após OFF, duas políticas experimentais

**2026-11-13 · PoC para revisão, sem promoção.** F1 foi reproduzida nos objetos originais API 18 e API 24 com os mesmos hashes e a mesma lease 77 do PR #12. As duas variantes passam a responder `0/0/0/0` após `applied(0)`. **Recomendo revogar as leases em OFF**: a variante passou as 98 verificações nomeadas do manager nos três perfis; ocultar preservou estado antigo em dois cenários de lifecycle, repetidos nos três perfis. Nenhuma política foi adotada definitivamente. Não houve merge.

## Rastreabilidade e escopo

- [HAL-21 no Linear](https://linear.app/half-programer/issue/HAL-21), consultada; permanece diagnóstico/proposta, sem encerramento automático.
- Base exclusiva: `integration/hal-20-v0.3.1-preview.2`, commit `d649647c159c91d615c07d6960928a37e270b461`; fonte oficial `aceac3782902746f35ddb0148809450933327cb3` / API 24.
- Branch nova: [`test/hal-21-fusion-off-providers-poc`](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/test/hal-21-fusion-off-providers-poc).
- Insumo autorizado: [PR #12](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/12), commit `907ed98cfb763a96f5aa1ad6369f809cceca5f1c`, [relatório original](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/mods/hal-20-fusion-effects-preview2/notes/HAL20_FUSION_EFFECTS_PREVIEW2.md).
- Lidos `AGENTS.md`, inventário HAL-20, relatório #12, [primeira integração #14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14) e registro da issue. A [PoC #20](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/20) continua separada: nenhum guard, manifest experimental ou mudança de ordem foi incorporado.
- PR desta entrega: link registrado após publicação no fim deste documento.

As alternativas estão como patches em `experiments/hal21/` e objetos em `artifacts/hal21/`. Não há pacote escolhido em `mods/`. PR #12, integração, master e branches anteriores foram preservados. Nenhum arquivo de produção do core/AI nem de Ritual Effects, Time Wizard, Dual Play ou outro MOD foi editado.

## Reprodução original e proveniência

| Objeto original | SHA-256 | Bytes | Hash do loader | Resultado após OFF |
|---|---|---:|---|---|
| API 18 recebido | `1d46c5f812759de21564e20ab88eddc79a268534c6e1055b7051b3dfefd88821` | 536660 | `7a3178de` | Encore 1, lease 77, extended 77, forecast 0 |
| API 24 PR #12 | `7e50372742ebd04090a907aa222f90d339d3122edf2fb06a589e297926c4f6c8` | 378508 | `e6f40fd4` | Encore 1, lease 77, extended 77, forecast 0 |

Execução do `preview2_loader.c` original, mudando apenas três caminhos de include na cópia temporária: objeto carregado pelo loader oficial, quatro providers, 33 hooks, quatro subscriptions. Card record 5 com Musician King 495, ocupado, identidade física e lease serial 77 semeadas como no teste original. Chamada real de `mod.applied(0)`, mesmos contratos consultados. **A lease é instrumentação do teste; não se alega que um duelo a criou.** Cada original executou 214 assertions de caracterização, terminou com código 2 porque o contrato OFF falhou. Esse FAIL foi preservado.

Evidências: [API 18](hal-21/contracts/original-api18-F1.log), [API 24](hal-21/contracts/original-api24-F1.log). O #14 já havia observado a mesma tupla via `Mods_Find`; a nova fixture confirma o resultado usando registry/manager reais da integração.

[INPUTS.json](hal-21/INPUTS.json) registra todos os arquivos do pacote PR #12, objetos e fontes consumidores utilizados. Os 98 checks de preflight e 98 de postflight passaram. Os 43 settings, 59 cards, 31 recipes de fusão, manifest, textos, 34 PNGs e `coin.txt` permanecem intactos. Autoria/coin-lib e permissão informada pelo usuário estão preservadas em [NOTICES-HAL20.md](../experiments/hal21/NOTICES-HAL20.md). Nenhum asset novo, código/binário de Dual Play, fonte/binário privado de Time Wizard, ROM ou save pessoal foi publicado. Ritual Effects é apenas insumo externo de testes.

## Causa e fronteira correta de estado

O core documenta que providers permanecem residentes e localizáveis até o processo terminar, inclusive quando o MOD está OFF (`src/pc/mods/modapi.h`, contrato de `provide/find`). A fixture mantém os mesmos ponteiros e chama suas funções depois do OFF; `Mods_Find` continua retornando os quatro registros. Não é necessário alterar o registry global.

| Callback | Estado original | Protótipos |
|---|---|---|
| `aux_encore_eligible` / `encore-v1.eligible` | Não consulta `ea_applied` | Retorna 0 se owner efetivamente OFF |
| `aux_unlock_lease` / `origin-unlock-v1.lease` | Não consulta `ea_applied` | Retorna 0 se owner OFF |
| `origin-effects-v1.lease` | Mesmo ponteiro `aux_unlock_lease` | Coberto pelo mesmo gate; nenhum ABI novo |
| `effect_forecast_v2.on/read/policy/pair` | Já passam por `ea_enabled` | Mesmo estado aplicado e setting `effect_ai`; respostas ON preservadas |
| `origin-unlock-v1.revoke` | Limpeza de identidade | Continua permitido OFF |
| `origin-effects-v1.register_provider` | Registro/unregister de catálogo externo | Continua permitido OFF; não concede um benefício por si só |

`ea_apply` já é o callback `mod.applied`. O helper comum consulta **`ea_applied && aux_host`**. Não consulta `host->applied()` para decidir a presença efetiva: no core desta base, `host_applied` lê `mod->enabled`, a preferência salva. Com `restart:true`, essa preferência pode estar OFF enquanto `Mods_Active` e os callbacks do MOD continuam ON até o reinício. Gating por essa preferência foi rejeitado no experimento por desligar providers antes da remoção efetiva. O código final das variantes preserva esse comportamento de espera, confirmado por `Mods_SetEnabled`, `Mods_Apply` e `Mods_Load`.

O manifest original **já exige reinício** e não foi alterado. Desmarcar no manager salva OFF e mantém a aplicação atual ON; responder ON nessa fase é esperado. Na próxima inicialização com preferência OFF, a biblioteca não é inicializada e os providers não estão presentes. A F1 aqui corrigida é o contrato **depois que `applied(0)` de fato foi entregue**. Os testes que forçam esse callback são explicitamente instrumentados; não provam que a UI permite OFF no meio de um duelo.

## Diff experimental e políticas

Ambos os patches partem somente do mesmo PR #12 e alteram três arquivos de produção **em cópias experimentais**:

1. `mod.c`: declaração do helper e gate de Encore.
2. `fusionist_runtime.h`: gate da lease compartilhada pelos dois contratos.
3. `owner_f.h`: helper, reutilização em `ea_enabled`, política em `ea_apply`.

[Ocultar](../experiments/hal21/hide.patch): OFF neutraliza respostas, mantendo a tabela privada. Em OFF → ON, usa `aux_unlock_sweep` existente para campo/ocupação/ID+ponteiro e revalida `aux_egotist_target`, incluindo setting e catálogo externo/Ritual. Não transfere uma lease para outra cópia por igualdade de card ID.

[Revogar](../experiments/hal21/revoke.patch): OFF neutraliza respostas e limpa somente `aux_unlocks`. Não zera `aux_unlock_serial`, catálogo de extensões, proveniência de fusão/Metalmorph nem executa reset geral. ON não recria as grants encerradas. O próximo bind real recebe novo serial (77 → 78 no teste), preservando a responsabilidade do materializador original. Encore normal volta a consultar a presença atual de Musician King; não é uma lease recuperada.

Não foram alterados efeitos, valores, receitas, Fusionist, Elegant Egotist, Metalmorph, probabilidades, settings, escolhas estratégicas, ABI, prioridades ou contratos públicos. As diferenças deliberadas de OFF → ON são justamente as políticas em avaliação. A paridade ON indicada abaixo vale para o estado ativo sem a revogação experimental de grants anteriores.

## Comparação de identidade e lifecycle

| Situação controlada | Ocultar | Revogar |
|---|---|---|
| Consultar OFF com ponteiros ainda no registry | Todos neutros | Todos neutros |
| ON → OFF → ON, mesma identidade válida | Reexpõe serial 77 | Mantém 0 até novo grant |
| Mesma identidade muda de slot/controlador enquanto OFF | Reexpõe 77 no destino válido | Não recupera grant antigo |
| Mesmo ID, outro ponteiro físico | 0, não herda | 0, não herda |
| Removida/desocupada ou permanece na mão ao reativar | Sweep retira lease | Lease já encerrada |
| Face-down em campo | Lease pode voltar; Encore continua 0 | Lease encerrada; Encore continua 0 |
| Setting Egotist OFF ou provider não elegível ao retornar | Não expõe grant; revalida | Não recupera grant |
| Remove e devolve **mesmo ponteiro+ID** durante OFF | **FAIL de preservação histórica**: reexpõe 77 | 0 |
| `Mods_Reset` com owner inativo, depois ON, record modelado reaproveitado | **FAIL de estado antigo**: reexpõe 77 | 0 |
| Reset aplicado, tick de novo duelo, SAVE/reset/LOAD simulado | Leases retiradas | Leases retiradas |

Os dois FAIL de ocultar são riscos demonstrados na fixture: não há geração/epoch no registro original e os callbacks de remoção/reset não são entregues ao owner inativo. O snapshot atual de ID+data pode ser igual após uma saída e retorno. Um snapshot anterior também não distingue isso. **Não foi demonstrado esse percurso num duelo Windows real**, nem que o restart obrigatório atual o permita pela UI. É razão para evitar prometer preservação segura de grants através de uma futura desativação efetiva em memória.

O teste de reset inativo conserva o record sintético para isolar o estado privado que o manager pula; não simula toda a reconstrução do campo de um novo duelo. O teste separado de novo duelo chama o hook real de Fusion para `DuelScene_Update` na fase inicial, com corpo do motor modelado. Nenhum reset arbitrário foi acrescentado para fazer a variante passar.

## Execuções e resultados

[Resultados completos](hal-21/contracts/RESULTADOS.json), [matriz CSV](hal-21/MATRIZ.csv), [regressões ON](hal-21/on/RESULTADOS.json), [ABI](hal-21/ABI.json).

| Grupo | Original API 24 | Ocultar | Revogar |
|---|---|---|---|
| Loader ON original | 2.652 assertions PASS | 2.652 PASS | 2.652 PASS |
| Loader OFF original | F1 FAIL; 214 checks de caracterização | 214 PASS; neutro | 214 PASS; neutro |
| Manager F → R (34 verificações nomeadas) | 26 PASS / 8 FAIL | 32 PASS / 2 FAIL | 34 PASS |
| Manager R → F (34 verificações) | 26 PASS / 8 FAIL | 32 PASS / 2 FAIL | 34 PASS |
| Fusion sem Ritual (30 verificações) | 24 PASS / 6 FAIL | 28 PASS / 2 FAIL | 30 PASS |
| 14 grupos originais ON, Python + C ASan/UBSan | 14 PASS | 14 PASS | 14 PASS |
| Imports/exports/ABI | Referência | PASS | PASS |

Manager total: baseline **76 PASS / 22 FAIL**; ocultar **92 PASS / 6 FAIL**; revogar **98 PASS / 0 FAIL**. Existem mais dois checks independentes PASS: Fusion ausente e novo processo iniciado com preferência Fusion OFF. As contagens são verificações nomeadas repetidas entre perfis, não 22 bugs distintos ou 98 duelos. A rodada diagnóstica retorna 1 porque inclui as falhas reais do baseline/hide; isso não foi disfarçado como sucesso global.

As fixtures ON exercitam regras/receitas, Round02/03, Fusionist, atributos, forecasts (88 comparações executor/forecast por variante), stats, títulos por instância e assets. O loader testa inicialização/rejeições, contratos opcionais, geração/restauração de cartas, reset/save simulado e shutdown. Os checks de lease validam cópias distintas, movimentos, remoção, cleanup OFF e serial monotônico. A suíte com manager executa `Mods_Find`, `Mods_SetEnabled`, `Mods_Apply`, reload, reset, registries, eventos e hooks reais contra os ELF reais. O motor permanece parcial e sintético.

Erros ocorridos ao construir a instrumentação: caminho de include de Fusion `png.h` inicialmente ocultava o header libpng do manager; foi trocado para `-iquote`. O primeiro acesso ao helper privado `aux_unlock_bind` usou cdecl e falhou: a função interna foi otimizada pelo Clang para fastcall, confirmado por disassembly; corrigiu-se somente o cast da fixture. O primeiro gate experimental por `host->applied` falhou na espera de reinício e foi substituído pelo estado do callback. Uma inspeção ELF tentou ler uma variável de versão inexistente; API é verificada por `MemoriesModInit` no loader. Essas rodadas de desenvolvimento não são falhas novas do jogo. Os logs publicados são da rodada completa dos candidatos finais, mantendo os FAIL reais.

## Consumidores

### Ritual Effects

Objeto PR #11 real, SHA-256 `c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1`. Não alterado ou republicado nesta branch.

- `performance_chaos.h:ritual_encore_eligible` resolve Encore e consulta no momento da necessidade, inclusive em `performance_done` após tails. A função literal foi exercitada contra os providers reais ON/OFF/ON e ausentes.
- `mod.c:borrowed_lease` consulta `origin-unlock-v1` a cada chamada. As duas variantes devolvem 0 OFF mesmo com `Mods_Find` disponível.
- O **`borrowed_reconcile` do próprio objeto** foi executado com estado borrowed explicitamente semeado: conserva Shield 362 ON; após OFF do Fusion, retira serial/marca/máscara futura em ambas as variantes. Baseline conserva, reproduzindo F1. Isso valida esse contrato/cleanup, não toda a animação de efeito ou aquisição da origem por gameplay.
- O provider real `effect_intelligence_v1` do Ritual informa elegibilidade 704. Desativá-lo enquanto Fusion está OFF não recupera lease na reativação de nenhum candidato. Carregamentos F→R e R→F e ausência de Ritual foram exercitados.
- Fonte de `borrowed_retire`: não desfaz LP resolvido, cartas já geradas, terreno ou ações permanentes. Ocultar pode permitir novo armamento de efeito depois que o consumidor já aposentou sua marca ao observar lease 0. Não foi testado o esgotamento de todas as cargas de todos os efeitos; exigir regressão real antes de aceitar preservação de leases.

### Time Wizard

A ponte `time_wizard_encore.c` disponível foi conferida por hash e compilada **somente localmente** com núcleo/wheel modelados. Seu `musician_present` usa o contrato Encore real; um início novo OFF resulta em `fresh_attempt_retry_OFF=0` com ambos os candidatos (baseline=1).

A ponte guarda `eligible` no começo de `encore_spin`. Se o teste força OFF depois desse começo e entrega tails, `encore_done` ainda usa o valor guardado: **`retry_after_OFF=1` em todas as variantes**, inclusive revogar. É uma limitação de cancelamento da tentativa em andamento, explicitamente caracterizada, sem contorno no Time Wizard. Com o manifest original, a mudança normal aguarda reinício; essa transição em voo foi forçada para testar o contrato. Exigir validação da fronteira segura se futuramente houver OFF efetivo em memória.

O núcleo completo v0.23.1/compilado da ponte, suas assinaturas/Dual e execução integral Windows continuam bloqueados. Os resultados não autorizam incorporar Time Wizard nem mudar sua ponte, wheel ou decisões aleatórias.

## Artefatos candidatos e ABI

| Variante | Bytes | SHA-256 | Loader hash |
|---|---:|---|---|
| [Ocultar](../artifacts/hal21/hide/fusion-auxiliary-balance.o) | 381024 | `1236eb5408b22c5fefecea748a5b9af6f925ef4fab1409ef869b36faff7571dd` | `8f442fbc` |
| [Revogar](../artifacts/hal21/revoke/fusion-auxiliary-balance.o) | 380268 | `3d85b23482682deb012b2d32458f077da0b4cfd4a3b3ee92d79f9bdae00bdcd9` | `afb75202` |

Ambos compilados de dois C/headers do pacote com SDK oficial `--target i386`, declarando API 24 na inicialização. Formato SDK ELF32 REL i386. Os **131 imports** são idênticos ao PR #12 e todos existem no SDK; exports globais continuam `MemoriesModInit` e `Png_Decode`. Quatro contratos, versões/records, 33 hooks e quatro subscriptions preservados. Layouts de headers públicos permanecem byte a byte iguais; nenhum slot ou provider público novo.

[PROVENIENCIA.json](hal-21/PROVENIENCIA.json) identifica Clang 23.1.2/llvm-mingw, builder, 808 arquivos de headers SDK, exports e patches por SHA-256. O SDK inclui caminhos DWARF de trabalho; SHA bruto depende desses caminhos. Esses hashes fixam **os candidatos efetivamente testados e publicados**, sem alegar reprodução bit a bit em qualquer diretório.

## Recomendação e regressões exigidas

**Propor para aprovação a variante revogar**, com gates de benefício pelo estado do callback e limpeza apenas da tabela de leases em `ea_apply(0)`, mantendo o serial. É menor e evita reexpor benefícios cujo histórico não foi observado. Custos: depois de uma remoção efetiva e nova aplicação em memória, efeitos emprestados anteriormente exigem um novo grant legítimo. Nada desfaz ações já resolvidas e não se transforma isso em uma regra nova de efeito nesta entrega. Ocultar exige controle de geração/histórico ou uma fronteira de lifecycle provada para tornar a retenção segura; não ampliar o protótipo sem necessidade/autorização.

O `restart:true` existente permanece. Preferência OFF pendente não significa owner removido: o usuário aguarda reinício para aplicar a mudança; essa espera não perde settings. No próximo processo OFF não há provider Fusion. Nenhum reinício automático, novo bloqueio de UI ou alteração global de manager foi introduzido.

Antes da adoção definitiva:

1. Revisão/autorização explícita da política OFF→ON pelo usuário e revisão dos três arquivos do patch escolhido.
2. Windows i386 real com imagem legítima local: grants genuínas de Egotist, cópias iguais, mudança de controlador/slot, remoção, transformação, retorno ao campo, outras cartas preservadas e consumer Ritual acompanhando a mesma identidade.
3. Verificar effects ON, encargos/cargas, nenhuma duplicação/rearmamento indevido, LP/cartas geradas/terreno já resolvidos preservados e cleanup de elegibilidade futura.
4. Repetir inicialização OFF/ON, preferências de restart/Apply/reload e cancelamento de reinício pela UI. Se houver novo caminho de desativação efetiva em memória, provar a fronteira durante roletas/efeitos pendentes.
5. Matriz integrada AI HARD MODE OFF/ON × Dual Play OFF/ON, demais MODs por ordem: hooks, seleção física, quotas, instâncias e reset. Esta entrega não modifica esses projetos e não fecha HAL-22/23/24.
6. Time Wizard somente após resolver proveniência/núcleo/ponte Dual; cobrir início OFF e tentativa iniciada ON ainda em voo. Não presumir que esta correção cancela callbacks já enfileirados.
7. SAVE/LOAD real e novo duelo com dados de teste próprios; registrar versões, sem prometer compatibilidade com savestates antigos.

Sem execução Windows/gameplay nesta etapa; sem promoção automática. A PoC entrega uma escolha testável e documentada, não aprovação da integração completa.

## Publicação

PR em rascunho: a registrar após criação. Branch de destino exclusiva: `integration/hal-20-v0.3.1-preview.2`. Índice de arquivos publicados e verificação das heads anteriores em `notes/hal-21/`.
