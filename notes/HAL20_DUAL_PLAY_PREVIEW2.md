# HAL-20 — Dual Play 0.5.7 / API 24

## Resultado e escopo da entrega

O fonte recebido recompila, sem alterações, com o SDK oficial v0.3.1-preview.2 para i386, o objeto utilizado pelos jogos Windows e Linux de 32 bits. O objeto novo carrega pelo loader oficial API 24 em uma fixture Linux i386 sob QEMU. Não foi encontrada adaptação obrigatória de fonte para esse build. Isso estabelece um **candidato local para testes integrados**, não comprova preservação integral do gameplay em Windows.

**Redistribuição não verificada. Esta entrega publica somente documentação e metadados de análise. Não publica fonte, manifest, README ou binário do terceiro, nem pacote instalável.** O usuário autorizou expressamente esse caminho caso a permissão não pudesse ser comprovada. A ausência de licença identificada não é afirmação de proibição jurídica; significa que falta evidência suficiente para publicar neste repositório público.

Base preservada: `integration/hal-20-v0.3.1-preview.2`, commit `d649647c159c91d615c07d6960928a37e270b461`, descendente do oficial `aceac3782902746f35ddb0148809450933327cb3`. Branch documental: `docs/hal-20-dual-play-preview2`, derivada diretamente dessa base. Nenhuma lógica de produção, controle, quota, configuração, receita, dificuldade ou asset foi alterado. Nenhum merge ou alteração de master, integração, PR #9, Time Wizard, AI HARD MODE ou outro MOD.

Referências lidas:

- [Inventário e plano / PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md), incluindo o CSV de arquivos.
- [Migração da base / PR #8](HAL20_BASE_OFICIAL_V031_PREVIEW2.md).
- [Melhoria + Surrender 1.3.6 / PR #9](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/9), head `0d28d844c3b88b38ab0db71635959ddd223d8c14`, ainda em rascunho e sem merge na consulta.
- AGENTS.md da base, Prompt Mestre V3 e issues [HAL-20](https://linear.app/half-programer/issue/HAL-20/base-sincronizar-fork-e-versoes-atuais-dos-mods-com-upstream-v031) e [HAL-9](https://linear.app/half-programer/issue/HAL-9/aud-verificar-time-wizard-e-compatibilidade-com-dual-play). Os documentos anexados foram tratados como contexto; a autorização específica desta etapa delimitou o trabalho.

[Evidência estruturada: hashes, imports/exports, layouts e resultados](hal-20/dual-play/RESULTADOS.json). Os resultados abaixo são locais, não checks aprovados no GitHub Actions.

## 1. Pacote, completude e licença

Entrada: pasta `dual-play` do `mods att.zip`, SHA-256 do ZIP `745f6fbbd5f3c9451ca37735626abbbc4c6704bc5326ab3691be33d86b66281b`. Quatro arquivos: `dual-play.c` (fonte único, 1.469 linhas), `dual-play.o`, `mod.json` e `README.md`. O fonte é estruturalmente suficiente para recompilação; não houve fonte ou biblioteca adicional ausente nesse build. Assets próprios e settings: nenhum. ID e biblioteca: `dual-play`; versão: `0.5.7`; autor declarado: `Community mod`; enabled true, restart false, data vazio. Manifest sem min_api, game, requires ou ordenação declarada. Init exige host API >=4; objeto recebido declara API 5, novo declara API 24. README menciona Mod API 4: distinguir mínimo usado, versão compilada e versão do jogo.

Não há LICENSE/COPYING/aviso de permissão nos quatro arquivos, cabeçalho autoral/licença no C, URL canônica ou autor identificável além de “Community mod”. A listagem integral do ZIP também não contém arquivo com nome LICENSE/LICENCE/COPYING/PERMISSION. A busca de código por dual-play/Monster nos dois repositórios de referência não retornou resultados; a árvore oficial da tag não distribui esse MOD. Isso não é uma busca exaustiva de todos os possíveis autores externos. A licença da recompilação não foi presumida como licença do pacote independente.

**Material adicional realmente necessário:** identificação da origem/autoria e licença aplicável a esses hashes, ou permissão escrita do titular para redistribuir fonte e binário recompilado, com condições de atribuição/distribuição. Não faltam os fontes do Dual Play; não é necessário reenviar o mesmo C para tentar compilar. A íntegra do núcleo do Time Wizard continua sendo uma lacuna distinta, já registrada no inventário.

## 2. Build e comparação de objetos

SDK Windows gerado da árvore oficial, com include/game, include/pc/mods, tools/build_mod.py e exports.i386.txt. Compilador llvm-mingw 20260922 / Clang 23.1.2; builder oficial com `--target i386`, sem flags próprias ou ajustes no fonte. Comando efetivamente utilizado, com paths relativos equivalentes:

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py /caminho/copia-local/dual-play --target i386
```

Saída: `build_mod: built i386`, um fonte. O arquivo resultante é ELF32 relocável EM_386, **não uma DLL ou PE**: esse é o formato que o loader Windows i386 oficial espera. Não foi copiado o objeto antigo para satisfazer a recompilação. C, manifest e README da cópia de build continuam idênticos aos recebidos. Não houve compilação x64/ARM/macOS nem conversão de alvo implícita.

| Evidência | Recebido | Recompilado API 24 |
|---|---|---|
| SHA-256 | `e08e198aa84783b4c04b9c9f117de0f521240a1c19b8f12e8bc1d30fc0769b90` | `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378` |
| Tamanho total | 67.832 bytes | 68.196 bytes |
| .text | 13.138 bytes | 13.644 bytes |
| Export global definido | MemoriesModInit | MemoriesModInit |
| Imports diretos | func_8001B780 | func_8001B780; __x86_indirect_thunk_eax |
| API declarada em execução | 5 | 24 |
| Hash oficial do loader | cb16d37a | 3028c68a |
| Estado registrado | 50 bytes / versão 10 | 50 bytes / versão 10 |
| dual_draw_phase na .text | 0x1a30 | 0x1b80 |
| Relocações alocadas R_386_32 / R_386_PC32 | 537 / 2 | 533 / 97 |

Ambos identificam Clang 23.1.2 no .comment. Isso não prova que tenham usado os mesmos flags, headers ou builder: o build original não veio acompanhado de receita/proveniência completa. O novo builder inclui `-mretpoline-external-thunk`, encaminhando chamadas indiretas pelos thunks oficiais de guest branch. A diferença não pode ser reduzida a debug info ou nome do arquivo: há mudança real de código, relocações e API declarada. Não afirmar equivalência binária ou equivalência semântica completa com base nesses números.

Os 28 nomes consultados dinamicamente por host->symbol e os dois imports diretos novos existem nos exports **de ambos** os SDKs i386 Linux/Windows. O JSON contém a lista integral. A fixture de loader usa funções/dados simulados para esses símbolos: a existência dos nomes foi verificada separadamente nos SDKs, não demonstrada por um resolver permissivo.

DWARF dos dois objetos: 19 aggregates com nomes comuns comparados por tamanho, nomes/offsets de campos e bit sizes. Dezessete descrições idênticas; duas diferenças explicadas: MemoriesModHost cresce de 120 para 216 bytes, conservando os campos anteriores e acrescentando serviços a partir de +120; DuelCardRecord mantém 28 bytes e campos anteriores, acrescentando o nome defense_modifier em +26. Não há deslocamento identificado nos campos de engine usados pelo Dual. Este check não é uma prova recursiva de equivalência de todos os tipos, nem substitui o jogo. DualPlayState mantém os 50 bytes e o prefixo consumido pelo Time Wizard.

## 3. Testes executados e alcance real

Fixtures privadas, criadas para esta análise, sem patch nos fontes recebidos. Execução Linux i386 via QEMU 5.2, com sysroot Debian 11; o kernel do ambiente não executa ELF32 diretamente. Headers e loader/eventos/hooks pertencem à base oficial. Não há imagem do jogo, Wine ou execução Windows nesta etapa.

| Teste | Resultado | O que demonstra / limite |
|---|---|---|
| Fonte original incluído numa fixture API 24 | 269 assertions aprovadas | Funções reais do C, motor e callbacks nativos simulados; não executa duelo |
| Objeto original + object_loader.c oficial | 2.903 assertions, 20 ciclos aprovados | Load/init, seis hooks, API 5, callbacks, draw/new-duel, OFF/ON, registro de estado; host simulado |
| Objeto recompilado + mesmo loader | 2.903 assertions, 20 ciclos aprovados | Mesmo alcance, API 24 e thunk oficial; não valida UI/partida |
| Estado registrado via events.c oficial | Visit/restore de 50 bytes v10 aprovado nos dois objetos | Round-trip sintético do bloco e exclusão quando MOD inativo; não é save/load completo do jogo |
| Ponte sobre bytes relocados | Predicate + endereço do estado aprovados nos dois objetos | Seis bytes de opcode, dois endereços e limites do prefixo conferidos |
| hooks.c oficial, alvo sintético patchable | 53 assertions aprovadas no objeto novo | Ponte direta reconhece; Dual OFF ou hook intermediário deixam de reconhecer; Time Wizard completo não carregado |

A fixture de fonte cobre as duas ordens Monster/Support, os quatro tipos Support (Magic, Trap, Ritual, Equip), ambos os lados, fusão só Monster e consumo misto. As transições nativas são simuladas; nenhum efeito real de Magic/Ritual foi executado. Cobre bloqueio de confirmação de categoria já usada, restauração dos bits de input após o callback, Right bloqueado apenas na navegação da última carta e permitido em modo de posicionamento, cursor limitado à mão real, aliases restritos a um callback, Circle para cancelar/reabrir e retorno sem comprar. Start/Select permanecem nos bits de entrada da fixture.

Há 64 combinações de seleção física (32 por lado), com alvo de campo e objeto repetido: as categorias são calculadas a partir das instâncias de mão selecionadas e os alvos de campo/duplicatas são ignorados no custo. Não é teste de cartas reais/arte. CPU: filtro da mão lógica de oito cartas, exclusão de Ritual na segunda jogada, plano permitido, rejeição de plano inválido, passagem pelo swap simulado e limites numéricos de tamanho 0/42. Não houve partida com 42 cartas, busca real da IA ou stress de turnos longos. Retorno automático da CPU, bloqueio do campo durante a transição e fim de duelo por LP zero também foram exercitados em estados sintéticos.

Loader: ausência de cada um dos 28 símbolos, falha em cada um dos seis registros de hook, recusa do registro de estado e host API 3 foram injetados; o init falha e desfaz os hooks já instalados nos casos exercitados. OFF limpa pendências/bonus e restaura mão oculta sintética; ON conserva quotas usadas. O novo duelo limpa o estado inteiro; draw não inicializado limpa o lado atual, draw já inicializado conserva quotas. Isso é comportamento existente, não correção aplicada. O MOD não registra callback reset; depende do bloco de estado gerenciado e dos seus hooks de inicialização.

Os primeiros erros na criação da fixture foram assinaturas incorretas de dois stubs de teste (setup/spawn). Corrigidos apenas na fixture. Nenhum erro de compilação do MOD API 24 foi encontrado; nenhum bug de gameplay da migração foi reproduzido. Não houve medição de performance, execução com sanitizadores desta fixture i386, teste de memória gráfico ou garantia de ausência de travamento em jogo.

As fixtures e os artefatos ficaram locais e **não acompanham esta publicação documental**. O JSON registra seus hashes e os componentes oficiais usados. Os resultados são verificáveis pelos insumos identificados e cenários descritos, mas a branch pública sozinha não contém tudo necessário para reproduzir automaticamente as fixtures. Após esclarecer redistribuição, a próxima entrega deve empacotar os testes/receita autorizados junto do candidato.

## 4. Time Wizard / Dual Play: risco confirmado e resultado desta recompilação

Evidência recebida: `TIME WIZARD/source/time_wizard_dual_play_compat.c:297–338`, função resolve_dual_state. Ela supõe que compat_original_draw é diretamente dual_draw_phase, testa posições fixas +0x23/+0x24/+0x29 e +0x2a/+0x2b/+0x30, lê os endereços em +0x25/+0x2c e exige segundo endereço = primeiro +2. O primeiro deve apontar ao estado privado; used <=3 e bonus_hand <=1. Essas dependências de instruções são reais, não uma API pública.

**Resultado medido:** embora dual_draw_phase tenha mudado de endereço na .text, os offsets locais verificados foram preservados no candidato Clang/API 24. Nos dois objetos carregados e relocados, os dois operandos apontam para o bloco registrado e para bloco+2; o predicate aceita. Portanto, a hipótese de que *esta* recompilação necessariamente quebraria a assinatura **não se confirmou**.

O teste complementar com hooks.c oficial confirmou a condição estrutural: Dual abaixo diretamente do observador -> reconhecido; Dual inativo -> rejeitado; outro hook de draw entre Dual e observador -> rejeitado; remoção desse intermediário -> reconhecido novamente. Não foi carregado o MOD Time Wizard completo: o observador aplica a mesma condição de reconhecimento e confere o endereço, sem executar roleta, papéis híbridos ou as transformações. A leitura de offsets fixos não ganha garantia geral de segurança por esse teste.

Impacto potencial quando o reconhecimento falha: as seções dependentes de state em compat_hand_actions (aprox. 808–1024), compat_ai_run e compat_ai_swap (1091–1150) deixam de ajustar a contabilidade híbrida do card ID 16. Monster/Support, papel por X/Quadrado, disponibilidade da segunda ação e ledger podem divergir. Não afirmar que o Time Wizard inteiro fica inerte: o arquivo também contém caminhos independentes desse state, incluindo seleção de papel, entrada e Magic direta. A falha integrada de quota ainda precisa de reprodução no jogo.

Não é necessário modificar o Time Wizard para este artefato específico apenas para atualizar offsets. Permanecem P1 a fragilidade por recompilador/flags/arquitetura, ordem de hooks e validação em jogo. Manter teste do predicate sobre cada artefato aprovado, sem congelar um objeto antigo como substituto do rebuild. Um eventual contrato opcional versionado, fornecido pelo Dual em vez de leitura de BSS, é uma proposta futura que requer autorização do usuário/titular e desenho de fallback; não foi criado aqui. Não ampliar o scanner ou alterar bytes para forçar reconhecimento.

## 5. Convivência futura com PR #9 e AI HARD MODE

**Análise estática; os pares não foram instalados/carregados juntos. PR #9 preservado.**

| Contato | Evidência | Conclusão e próximo teste |
|---|---|---|
| Melhoria e Dual na mão | Melhoria rules.c:1261–1292 / Dual dual_hand_actions:815–1045 | Ambos envolvem DuelScene_UpdateHandActions e mascaram/restauram Pad1Pressed. Testar as duas ordens, recusa ritual_only_player, confirmação/cancelamento e contabilização exatamente uma vez. Não impor ordem por suposição |
| Tributos da mão e quota | Melhoria consume_selected_source:920–944 e ritual_deactivate:1085 em diante / Dual snapshot, physical_action_masks e commit da saída da mão | Dual fecha sua observação na transição Hand->Effect/Place; Melhoria também consome tributos durante a animação. Há risco de diferenças de momento e snapshot. Testar ritual com 0/1/2/3 materiais da mão e complementos de campo, cartas duplicadas e segundas ações. Ainda não está provado que precise alterar código ou regra de quota |
| Campo/âncora sintética | Melhoria prepare_visuals / Dual physical_action_masks:467–525 | Dual ignora alvos fora da faixa física da mão; Melhoria cria visuais auxiliares. Testar se o consumo real e os visuais mantêm as identidades esperadas; não classificar âncora visual como nova carta jogada |
| Surrender | surrender.c:29–74, Select e DuelScene_UpdateWithSideInput / Dual Circle na mão/frame | Sem colisão direta de botão ou alvo de hook identificada. Testar Sim/Não/cancelar com bonus e transição pendentes, derrota normal, novo duelo e retorno de menus. Surrender conserva o caminho nativo de oponente humano; isso não comprova controle integrado |
| Dois humanos nativo | src/game/duel_scene_update.c:16–33 e input_pad1_backup.c / Dual dual_frame:1059 em diante | A base troca Pad2 por Pad1 só durante DuelScene_Update. Dual lê Pad1 também no callback frame, invocado por GsDrawOt via Mods_DrawFrame. Verificar contexto/ordem de render e roteamento no lado 1: testes de lado sintético não equivalem a dois controles. Risco estático, sem falha real reproduzida |
| AI HARD MODE oficial | mods/ai-hard-mode/ai_hard_mode.c:74–94, 160–181, 268–315 | Ai_GetHandSize/window e func_8001BAF0 são contatos diretos. Com preserve_hand ativo, swap_selection implementa o swap e não chama o anterior nesse ramo; se estiver por fora do Dual, sua validação pode ser contornada. Ordem inversa conserva o wrapper Dual, mas ainda exige testar índices materializados, mão compactada e limites da janela |

AI HARD MODE é o oficial desta base, não o adapter 0.1.1 ausente mencionado pelos outros pacotes. Seu available retorna apenas a mão visível quando ela tem menos de cinco cartas; Dual filtra a janela retornada por Ai_GetHandSize. Isso muda as possibilidades de planejamento em relação ao fake de mão lógica usado no teste isolado. Não alterar deck_access, dificuldade, preserve_hand ou limite para “resolver” algo sem reprodução e autorização. Não há conclusão de compatibilidade integrada da IA.

Matriz ainda obrigatória em runtime: AI OFF/Dual OFF, AI ON/Dual OFF, AI OFF/Dual ON, AI ON/Dual ON. Repetir com Melhoria/Surrender e depois Time Wizard, em ordem de carga registrada, mantendo um ID instalado por versão. Os cenários locais isolados não preenchem essa matriz de gameplay.

## 6. Save/load, limitações e adaptações necessárias

O estado do MOD contém índices/flags, tem mesmo layout de 50 bytes/v10 e passou pelo registro/visit/restore oficial na fixture. Isso **não autoriza reaproveitar save states antigos**. `src/pc/mods/manager.c:428–455` inclui Mods_CodeHash em Mods_Signature; `src/pc/guest/state_subsystems.c:70–81` exige igualdade da assinatura e versão/tamanho dos chunks. Os hashes cb16d37a/3028c68a diferem. Com o restante igual, substituir o objeto muda a assinatura e impede a aceitação normal do snapshot modificado. Não desativar essa proteção nem tratar erro de assinatura como bug novo. Save de progresso/memory card é outro mecanismo e não foi exercitado.

Para SDK/API 24, **nenhuma adaptação de produção do Dual se mostrou necessária nos checks executados**. São necessários processo de build novo, flags oficiais com thunk e verificação por alvo. O init passa a declarar API 24 porque usa MEMORIES_MOD_API dos headers atuais; seu teste interno api>=4 e o manifest original permanecem iguais. Não vender o objeto como compatível com hosts antigos só por esse mínimo no fonte. Não foi alterado min_api ou acrescentada dependência para mudar o pacote sem autorização.

Nos nossos MODs:

1. **Time Wizard:** nenhum patch de assinatura necessário neste candidato. Recompilar/obter o núcleo completo identificado no inventário antes de portabilidade integral; validar X/Quadrado, face-down, humano/CPU, primeira/segunda ação e OFF/ON. Só propor mudança de ponte depois de falha concreta ou decisão autorizada de contrato.
2. **Melhoria + Surrender:** nenhuma alteração exigida de SDK identificada além do rebuild já entregue no PR #9. Necessidade de adapter de ação/consumo depende do teste de tributos tardios e ordem de mão; não há justificativa para alterar receitas, regras ou PR #9 agora.
3. **Outros MODs/AI:** não portados nesta etapa. Registrar autoridade de planner/swap e providers de custos antes do teste integrado. Adapter AI ausente e fonte parcial do Time Wizard continuam bloqueios próprios, sem contornar com objetos antigos.

Faltam gameplay Windows i386 com a imagem legítima mantida fora do Git, load/unload real pelo menu, turnos longos/CPU completa, novo duelo após finalização, render/Fusion Helper sem aliases, dois gamepads e save/load completo com o **mesmo** conjunto novo. Testar OFF/ON em mão extra, animação, retorno ao campo e diálogo Surrender; snapshots antes/durante/depois dessas fases; rejeição limpa de snapshots com outra assinatura. Não houve trava reproduzida; também não há prova de ausência de travas nessas fases.

## 7. Sequência segura e rastreabilidade

1. Obter evidência de licença/permissão vinculada à versão/hash. Até lá manter a entrega pública documental e o candidato local sem redistribuição.
2. Com redistribuição comprovada, criar PR de portabilidade independente e em rascunho para integração, com C/manifest aprovados preservados, build/proveniência e fixtures autorizados. Nenhum merge automático.
3. Validar Dual isolado em Windows i386: ambas as ordens/categorias, fusões e ações mistas, quota negada, seleção física/lógica, mão reduzida, turnos e duelos novos, toggles, dois humanos, save/load e UI. Usar sempre o mesmo hash candidato identificado.
4. Validar com PR #9 sem modificá-lo: ambas as ordens de hook, materiais campo/mão e Surrender em todos os estados pendentes. Se falhar, registrar reprodução e propor correção mínima no dono correto; não mudar regra de tributo/quota por preferência.
5. Validar AI oficial e depois pares com Time Wizard, incluindo signature check no objeto final e cadeia de draw. Não concluir coleção pronta antes da matriz de quatro cenários e dos casos específicos híbridos.

Prioridades: P1 licença/proveniência para publicação; P1 validação real de input/estado e cadeia AI/Time Wizard; P2 empacotar testes reproduzíveis após liberação. A fragilidade de assinatura é confirmada, mas sua quebra pelo candidato atual não ocorreu. Os demais riscos de convivência permanecem hipóteses sustentadas por código até teste integrado.

Alterações publicadas nesta etapa: este relatório, seu JSON de evidências e o apontador de continuidade em AGENTS.md. Sem scripts de terceiros, binários, ROM, saves, assets, workflows ou permissões. As limitações ambientais e a falta de licença não são regressões introduzidas na migração.

GitHub/Linear acessíveis para leitura. HAL-20 permanece In Progress e HAL-9 Backlog, sem mudança de status. A consulta Actions da integração retornou zero runs para essa branch; ausência de execução não prova workflow desativado. Consultar os checks do PR documental separadamente; os testes locais deste relatório não foram executados no Actions.
