# HAL-20 — Inventário dos MODs atualizados e plano de migração

Data: 11/11/2026. **Estado: auditoria estática concluída; migração não implementada.**

## 1. Como usar este documento em outro chat

Este é o registro da primeira etapa da HAL-20. As diferenças abaixo foram encontradas nos pacotes fornecidos pelo usuário e no GitHub; não são mudanças de código feitas pelo Codex nesta auditoria. A publicação posterior acrescenta somente documentação e inventário.

Leia primeiro este relatório, depois o [inventário integral de arquivos e hashes](hal-20/INVENTARIO_ARQUIVOS.csv) e o [protocolo de continuidade](../AGENTS.md). O CSV contém 495 linhas de arquivos, além do cabeçalho. Os pacotes, fontes, assets e objetos não estão incluídos nesta publicação documental; para reproduzir a análise dos MODs é necessário obter do usuário o ZIP identificado abaixo.

- Desenvolvimento: [otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled).
- Original: [Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled).
- [HAL-20](https://linear.app/half-programer/issue/HAL-20/base-sincronizar-fork-e-versoes-atuais-dos-mods-com-upstream-v031).
- [Relatório HAL-5](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-5-auditoria-mods/notes/MODS_AUDITORIA_E_PLANO.md), preservado na branch original e no [PR #6](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/6).
- Insumo de referência: `PROMPT_CODEX_AUDITORIA_OTIMIZACAO_SINCRONIA_MODS_YFM_V3.txt`, fornecido nesta conversa. As instruções antigas de instalação e os testes relatados nos pacotes são documentação histórica, não autorização atual nem testes reproduzidos.

## 2. Resultado executivo e limites

O ZIP atualizado tem seis MODs, seis IDs distintos e 495 arquivos. Melhoria/Surrender, Ritual Duelists, Ritual Effects, Fusion Effects e Dual Play têm fontes estruturalmente completas para tentar recompilar. Time Wizard é parcial: seus adapters relinkam um objeto-base sem o fonte integral do controlador/roleta.

Há três bloqueios importantes para considerar a coleção integralmente migrada: o núcleo-fonte ausente de Time Wizard; o adaptador AI Hard Mode 0.1.1 citado na documentação mas não fornecido; e quatro hashes de objetos diferentes dos registrados nos arquivos de proveniência de Fusion/Ritual Effects. A ponte Time Wizard/Dual Play também depende de instruções do objeto i386 recebido e pode deixar de reconhecer uma recompilação.

A base atual é ancestral direta da tag-alvo, permitindo atualização por Git com histórico preservado. Isso não torna seguro mesclar integralmente branches antigas de MODs, baseadas em revisões diferentes.

Não foi executado código dos pacotes, compilação, teste dos MODs ou jogo. Foram realizadas leitura de fontes/manifests/documentos, extração e hashes, consulta Git/GitHub/Linear e inspeção estática de ELF, imports e disassembly. Resultados de testes citados pelos autores continuam sendo relatos históricos. Não há P0 comprovado nem garantia de compatibilidade integrada.

Nenhum código, mecânica, manifesto, configuração, save ou workflow foi alterado. A auditoria não criou branches/PRs nem atualizou issues. A publicação documental autorizada posteriormente é uma entrega separada; não implementa a migração e não altera a `master`.

## 3. Base, tag oficial e referências existentes

| Item | Estado verificado |
|---|---|
| Branch local durante a auditoria | `work`, árvore limpa |
| HEAD e fork `master` | `0d78202729967a5bbdbc886aa128e48e82bef7ac` |
| Base efetiva | `v0.2.1-preview.1`, API 10 |
| Tag-alvo oficial | `v0.3.1-preview.2` |
| Commit-alvo | `aceac3782902746f35ddb0148809450933327cb3` |
| API-alvo | 24, `src/pc/mods/mod_types.h:3` |
| Ancestralidade | `master` é ancestral direta; 907 commits até a tag-alvo |
| Windows oficial examinado | PE i386/32 bits |

Código oficial examinado: `mod_types.h`, `modapi.h`, `manager.c`, `hooks.c`, `events.c`, `mods.c`, `object_loader.c`, `guest/state.c`, `state_subsystems.c`, `tools/pc/build_mod.py`, geração de exports e fontes dos cinco MODs distribuídos. A API acrescenta serviços ao fim da estrutura do host; os MODs também acessam estruturas internas e código nativo, exigindo validação própria.

[Release oficial](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/releases/tag/v0.3.1-preview.2) e [árvore-fonte fixada](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/aceac3782902746f35ddb0148809450933327cb3).

| Branch/tag do fork | SHA abreviado | Conteúdo relevante |
|---|---|---|
| `master` | `0d7820272996` | Cinco MODs da base |
| `docs/hal-5-auditoria-mods` | `09c3530517a0` | Relatório anterior; PR #6 aberto |
| `downloads/efeitos-rituais-v0.78.40` | `561d3d97818c` | ZIP Ritual Effects 0.78.40 |
| `mod-field-effects` | `296c59194797` | Campos 0.19 e variante AI Hard Mode |
| `mod-ritual-effects` | `632cd012deef` | Ritual Effects 0.74, com cópia em versions |
| `ritual-requirements` | `c2e8783aa2ba` | Alterações antigas de core/editor; PR #1 aberto |
| `ritual-requirements-v014` | `8ba422b58b49` | Já ancestral da tag-alvo |
| `backup/master-pre-v0.2.0` | `4f58718bb897` | Backup histórico |
| `backup/master-pre-v0.2.1-preview.1` | `7a8da6d964dd` | Backup histórico |
| `backup/mod-field-effects-pre-v0.2.0` | `985609424e7e` | Campos 0.1-dev |
| `upstream-v0.2.0` | `0185f58df792` | Referência histórica |
| tag `ritual-editor-test` | `4162e8d3c862` | Melhoria 1.1.0-test; já ancestral da tag-alvo |

A ancestralidade de uma tag não implica que todos os seus arquivos permaneçam na árvore final. A branch `ritual-requirements` também modifica core, editor, testes e workflow; confrontar seus deltas com o suporte já existente na versão-alvo antes de qualquer reaplicação. Preservar os PRs #1 e #6 sem mescla automática.

## 4. Insumos e inventário dos seis pacotes

`mods att.zip`: 20.218.592 bytes compactados, 40.388.961 bytes descompactados, 523 entradas (495 arquivos e 28 diretórios explícitos).

SHA-256: `745f6fbbd5f3c9451ca37735626abbbc4c6704bc5326ab3691be33d86b66281b`.

| Pasta / nome | ID | Versão | Library | Arquivos | Fontes de produção |
|---|---|---|---|---:|---|
| MelhoriaDosRituais / Melhorias de Rituais + Surrender | `melhoria-dos-rituais` | `1.3.6` | `rituals` | 18 | 2 C + 4 H |
| Ritual Duelists | `ritual-duelists` | `0.4.0-alpha.8` | `ritual-duelists` | 148 | 16 C + 22 H |
| zzz Efeitos Rituais | `ritual-card-effects-test` | `0.78.74-effect-ai` | `ritual-card-effects` | 194 | 2 C + 29 H |
| BalanceamentoFusoesEfeitosAuxiliares | `fusion-auxiliary-balance` | `0.1.0-beta.44-effect-ai` | `fusion-auxiliary-balance` | 93 | 2 C + 23 H |
| TIME WIZARD | `time-wizard-time-magic` | `0.24.0-test` | `time-wizard` | 38 | 5 C de adapters; núcleo em objeto |
| dual-play / Dual Play: Monster + Support | `dual-play` | `0.5.7` | `dual-play` | 4 | 1 C |

Todos os seis manifests têm `enabled:true`; isso não comprova a configuração efetivamente instalada. "Fontes estruturalmente completas" significa disponibilidade dos fontes e ausência de includes locais faltantes na inspeção; não significa compilação ou correspondência fonte→objeto verificada.

| MOD | Declaração / recursos |
|---|---|
| Melhoria | `min_api:4`, `slus_01411`; 5 settings; objetos i386 e Windows x64; Surrender em fonte separado |
| Ritual Duelists | `min_api:12`, `slus_01411`, restart; 6 settings; i386/x64; 38 PNG + 19 buffers gráficos BIN |
| Ritual Effects | `min_api:4`, `slus_01411`, restart; 48 settings; i386/x64; 65 textos + 56 PNG + 5 WAV |
| Fusion Effects | `min_api:15`, `slus_01411`, restart; 43 settings; i386/x64; 59 entradas cards + 31 fusions; 34 PNG |
| Time Wizard | `min_api:4`, `slus_01411`, restart; 9 settings; i386; 4 WAV; skin personalizada opcional |
| Dual Play | Manifest sem min_api/game, restart false; fonte exige API >=4; i386; sem settings; estado registrado versão 10 |

Os quatro objetos `x86_64-windows.o` são ELF64, formato previsto pelo loader próprio. ELF64 sozinho não comprova a convenção de chamadas. A distribuição Windows oficial examinada é i386; x64 precisa ser tratado separadamente.

## 5. O que mudou nos candidatos em relação à HAL-5

| MOD | Comparação | Estado |
|---|---|---|
| Melhoria + Surrender | GitHub: 1.1.0-test, sem Surrender. ZIP: 1.3.6 com rules/selection, surrender, provider rules e cinco opções | Fontes estruturalmente completas; identidade fonte→objeto pendente |
| Ritual Effects | GitHub: 0.74 e 0.78.40. Contra ZIP 0.78.40: 123 arquivos idênticos, 38 modificados, 33 novos, zero removidos | Fontes estruturalmente completas; proveniência inconsistente |
| Fusion Effects | Pacote homônimo não localizado nas pontas examinadas anteriormente | Fontes estruturalmente completas; proveniência inconsistente |
| Ritual Duelists | Não localizado como pacote customizado na HAL-5 | Fontes estruturalmente completas; UI externa Droplist ausente |
| Dual Play | Não localizado na HAL-5; agora fonte e objeto 0.5.7 | Fonte estruturalmente completa; bridge Time Wizard depende do objeto |
| Time Wizard | Agora adapters, objeto final e objeto-base | Parcial: falta fonte integral do núcleo incorporado |

Ritual Effects adicionou `title_color_diagnostic`, `effect_ai` e `effect_ai_trace`; não é apenas substituição do objeto antigo. O ZIP 0.78.40 original tem SHA-256 `ad8563c8597c038e8b8bcd12c384039e0247009b9a9094be10362649439a8f4d` e continua preservado na [branch de downloads](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/downloads/efeitos-rituais-v0.78.40).

### MODs oficiais e materiais ainda ausentes

- 3D Monsters continua 1.0, mas passa de 15 para 23 settings e ganha `fight_sounds.c/.h` na tag-alvo.
- AI Hard Mode continua 0.1.0/17 settings, mas fontes oficiais mudaram. A versão da branch de campos também é diferente. Não identificar o SDK/conteúdo só pelo número do manifest.
- Hand camera continua 1.0/6 settings e tem ajuste de fonte; Yamyi continua 1.0 e passa de 13 para 14 settings; Drop missing cards continua 1.0.
- `IA-DE-EFEITOS.md` cita adaptador AI Hard Mode **0.1.1**, ausente do ZIP e das pontas examinadas. O AI oficial da tag-alvo não consulta `effect_forecast_v2` nem substitui esse adapter por inferência.
- Field Effects **0.19-adaptive-description-test** tem fonte na branch `mod-field-effects`; nenhum pacote atualizado dele no ZIP. DLC de campos/arenas não fornecido.
- TCG Frame Colors independente não fornecido. As 58 entradas `frame:Orange` de Fusion não comprovam o pacote de 48 Fusion Monsters nem a exceção BEUD azul.
- Ritual Duelists Alpha.8 removeu o visualizador interno e delegou a UI ao **Droplist 1.4.0 de PauloH-Gudang**, ausente deste ZIP. Sua ausência é lacuna de UI, não ausência do código de sorteio/drops.
- Os recursos HD do seletor inicial não comprovam que todos os packs de interface citados no Prompt estejam disponíveis.

## 6. Dependências, duplicações e integridade

Nenhum dos seis manifests declara `requires`. As diretivas `after` são:

| Consumidor | after |
|---|---|
| Ritual Duelists | drop-missing-cards |
| Ritual Effects | melhoria-dos-rituais, ai-hard-mode, time-wizard-time-magic |
| Time Wizard | ai-hard-mode, dual-play |
| Melhoria, Fusion, Dual Play | Não declarado |

A documentação de Ritual Duelists exige Drop missing cards ativo para preservar o balanceamento aprovado, embora `after` não imponha presença/ativação.

Contratos opcionais encontrados: Melhoria publica `rules_v1`; Ritual Effects publica `recipe_v1`, `effect_intelligence_v1`, `effect_forecast_v2` e `position-rules-v1`; Fusion publica `encore-v1`, `origin-unlock-v1`, `origin-effects-v1` e `effect_forecast_v2`. Ritual Duelists consome regras/receitas/inteligência. Time Wizard consulta Encore de Fusion. Dual Play não publica o contrato público usado pela ponte Time Wizard, que alcança estado privado por reconhecimento de instruções.

Não foram encontrados nomes repetidos ou colisões de caixa no ZIP. Há **428 conteúdos distintos**, **41 grupos duplicados**, **67 cópias adicionais**, totalizando **1.109.331 bytes repetidos**. Exemplos: headers recipe/rules/intelligence entre consumidores e providers, helpers model/snapshot/native/png e assets de moeda entre Fusion e Ritual Effects. Preservar independência; não criar biblioteca obrigatória apenas para remover essas cópias. Fusion inclui quatro arquivos de `.pytest_cache`, dispensáveis à instalação.

Os manifests de testes não são instalações adicionais. A branch antiga de Ritual Effects tem cópias do mesmo ID em dev/versions; não instalar ambas. As referências de assets examinadas e as sequências numeradas estavam presentes. Os 19 BIN de Ritual Duelists têm os tamanhos de buffers de pixels esperados pelo código. Não foi validada licença de redistribuição dos assets; não copiar automaticamente os pacotes para o repositório.

### Objetos e exports oficiais

Distribuição Windows oficial inspecionada: SHA-256 `0350e4560fc9f25d6ee09002734db07408374a70711ddca19be2a617080a632e`, correspondente ao digest do GitHub. O SDK contém 4.217 exports i386 e 4.206 x64.

Todos os imports dos 11 objetos constam nas listas oficiais da arquitetura correspondente; os nomes literais de `host->symbol` examinados também constam nos exports i386. Isso valida nomes, não tipos, layouts, relocação em runtime, calling convention, encadeamento ou identidade fonte→objeto. Os hashes de cada objeto estão no CSV e no apêndice.

## 7. Riscos priorizados e evidências

As referências aos pacotes abaixo usam o caminho relativo dentro do ZIP. Linhas do core referem-se ao commit-alvo fixado.

| ID / prioridade | Evidência / fato ou hipótese | Impacto e ação mínima proposta | Teste necessário |
|---|---|---|---|
| R1 / P1 | Time Wizard `source/build_v024.py` relinka objeto-base e cria aliases por offsets de BSS; `time_wizard_evolution.c:7–11` registra ausência do fonte do controlador | Obter fontes/controlador/roleta e build correspondentes. Não trocar o objeto-base por outra versão | Rebuild integral, cotejo de artefatos e roleta/transformações |
| R2 / P1 | `time_wizard_dual_play_compat.c:297–338` reconhece bytes/offsets 0x23/0x25/0x2C para acessar estado privado de Dual | A assinatura corresponde ao objeto recebido; recompilar pode alterá-la. Contrato público ou validação específica precisam de proposta/aprovação | Dual recompilado, ponte presente/ausente, quotas, save/load, X/Quadrado |
| R3 / P1 | Dois hashes de Fusion e dois de Ritual Effects não correspondem aos SDK-PROVENANCE.json; metadados citam beta43 e 0.78.73 | Rastreabilidade inconsistente confirmada; mistura funcional fonte/objeto não comprovada. Reconciliar fontes/build antes de promover | Build com proveniência atualizada e verificação de hashes |
| R4 / P1 | `IA-DE-EFEITOS.md` cita AI 0.1.1 ausente; AI oficial não consome effect_forecast_v2 | Obter o adapter se compõe a instalação pretendida; não presumir entendimento dos efeitos no planner oficial | AI OFF/ON, providers OFF/ON, decisões legais e visibilidade |
| R5 / P1 | Variante da branch de campos usa terrain(card,type,field); tag oficial usa terrain(type,field), `mods/ai-hard-mode/planner.h:29` | Não copiar planner antigo isoladamente. Planejar adapter mínimo em PR separado, preservando dificuldade | Terreno por tipo/identidade, efeito único, AI isolada e integrada |
| R6 / P1 | IA/materialização/mão compartilham hooks entre AI, RD, Ritual, Time Wizard e Dual | Potencial desacordo entre ação lógica, cartas físicas e quotas. Não há falha integrada reproduzida | Matriz AI/Dual, mão >5, segunda jogada, cancelamentos e turnos longos |
| R7 / P1 | Ritual `mod.c:7558` usa reset=clear; Fusion `mod.c:2236` limpa origem/marcas em aux_reset; não registram estado equivalente a Dual/RD. Core chama Mods_Reset após carregar, `state.c:585` | Hipótese de perda de classificação/recursos após load deve ser reproduzida; não alterar persistência nesta fase | Salvar/carregar instâncias de origens distintas, cargas, controle e retornos |
| R8 / P2 | TCG/DLC/Droplist externos ausentes; Fusion possui 58 frame:Orange | Coleção visual incompleta para o requisito completo | Molduras/nome/stats separados, 48 Fusion Monsters e BEUD azul |
| R9 / P2 | Fusion `owner_f.h`, ea_pair, aloca buffer proporcional ao catálogo; snapshot.h reconstrói snapshots/consulta atributos | Custo potencial, sem gargalo medido ou ganho alegado | Contagem de chamadas/alocações e perfil reproduzível |
| R10 / P2 | Ritual Duelists defaults diagnostics=1/debug_overlay=1 | Possível custo de log/UI em sessões longas; preservar preferências | Medir cenário longo sem modificar defaults antecipadamente |

### AI Hard Mode, Dual Play e autoridade dos hooks

Dual Play recebido é **Monster + Support**, duas quotas por turno; não é prova de um MOD que crie dois humanos com controles independentes. Continuam obrigatórios os testes do pacote recebido e do modo nativo de dois jogadores. Surrender preserva o caminho de oponente humano em `surrender.c:29–36`, mas não foi validado em jogo. Ritual Effects captura pad1 e zera pad1/pad2 durante moeda/dodge em `performance_chaos.h:398–407`; incluir roteamento dos dois controles.

Host: hooks de função executam o último registrado primeiro (`hooks.c:29`), seguindo `original` até o core. Eventos usam prioridade decrescente; handled em BEFORE interrompe consumidores (`events.c:31–48,124–152`). after ordena MODs presentes/ativos, não prova composição semântica. find pode retornar provider carregado e desativado; consumidores/provedores devem respeitar habilitação/ABI.

| Ponto compartilhado | MODs / autoridade a verificar |
|---|---|
| AiScript_Run, Ai_GetHandSize, func_8001BAF0 | AI, RD, Ritual, Time Wizard, Dual na materialização: quem escolhe/filtra/troca as cartas |
| Mão, campo, compra e quotas | Dual, Time Wizard, Melhoria, Fusion, Ritual: X/Quadrado, ação física e cancelamento |
| Ritual e consumo | Melhoria seleciona/consome; Ritual valida efeitos/receitas; RD planeja; core executa |
| Fusão/equip | Fusion, Ritual, RD, AI: origem, bloqueios, transformação e consultas sem efeitos colaterais |
| Stats/terreno/batalha | Fusion, Ritual, campos e AI: aplicação única, proteção e dano de LP |
| Turno/instâncias | Fusion, Ritual, Time Wizard: criação, transformação, retorno e limpeza por instância |
| Texto/render | Yamyi, Fusion, Ritual, Time Wizard, campos e 3D: descrições, título, moldura, valores e restauração |
| Drops | Drop missing, RD, Droplist/Yamyi: tabelas compostas, limites e exibição |
| Save/load | Core e todos: arquitetura, perfil, layouts, origem e estado pendente |

## 8. CI histórico: causa agora identificada

Consulta atual confirmou que a run mais recente retornada do fork continua sendo a [37518883452](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/actions/runs/37518883452), de 06/10/2026, branch mod-field-effects, commit `296c5919479711b550d00aea8cd042b1c34ba306`.

Foi possível ler os logs dos jobs core-linux (112458793229), core-windows (112458792770) e sanitizers (112458793292). O primeiro erro de compilação nos três está em `tests/pc/ai_hard_mode_test.c:29`: HmRules.terrain espera `int (*)(int,int,int)`, mas a fixture fornece função de dois argumentos. Os compiladores tratam a incompatibilidade como erro. A fixture terrain aparece na linha 25 do snapshot.

É a mesma falha histórica, não falha nova nem diagnóstico genérico de ambiente. O core oficial-alvo usa callback terrain de dois argumentos. Corrigir o teste do contrato da variante de campos deve ser trabalho isolado em HAL-17, sem restaurar a API antiga na base nova. Nenhum workflow foi disparado/alterado nem houve ampliação de permissões.

## 9. Sequência segura proposta — ainda não autorizada para implementação

1. Registrar SHAs/hashes e preservar referências de rollback. Cópias antigas fora da descoberta de MODs.
2. Partir da master atual numa branch de integração e trazer a tag exata por Git, preservando ancestralidade. Não resetar, forçar push ou substituir árvore pelo ZIP Windows.
3. Validar base oficial e seus cinco MODs com fontes da própria tag. Preservar IDs/settings e não sobrescrever com variantes antigas.
4. Fechar fontes/proveniência: núcleo Time Wizard, adapter AI citado e quatro hashes divergentes.
5. Recompilar pacotes completos isoladamente com SDK-alvo. Priorizar Windows i386 efetivamente distribuído; x64 separado.
6. Estabelecer Dual Play isolado e verificar efeito da recompilação sobre a ponte Time Wizard. Sem alterar terceiro automaticamente.
7. Integrar Melhoria, Ritual Effects, Fusion e Ritual Duelists em PRs independentes, com testes isolados e de contratos opcionais. RD com Drop missing ativo.
8. Campos/visuais em etapa separada. Portar somente delta necessário de campos; não substituir planner completo antigo. DLC/TCG/Droplist dependem dos pacotes reais.
9. Time Wizard por último, após fontes/ponte resolvidas e aprovadas.
10. Regressão integrada, revisão e só depois PR de promoção à master. Sem merge automático.

Esta é a sequência de engenharia. A ordem de carregamento final deve resultar das cadeias verificadas; não há uma ordem universal comprovada apenas pelos nomes das pastas.

| Branch proposta | Destino / conteúdo |
|---|---|
| docs/hal-20-inventario-plano | Publicação documental desta auditoria |
| integration/hal-20-v0.3.1-preview.2 | Integração para revisão futura |
| sync/hal-20-upstream-v0.3.1-preview.2 | PR da atualização oficial para integração |
| mods/hal-20-<id>-preview2 | Um PR por MOD independente |
| compat/hal-20-<integracao> | Ajuste mínimo de contrato após autorização |
| ci/hal-20-validacao | Workflows/verificação em PR separado |
| test/hal-20-regressao-integrada | Cenários/evidências |

Manter uma versão instalável por ID. Histórico no Git, fontes de teste e arquivos de auditoria fora da descoberta de MODs. Não incluir ROM/imagem do jogo, saves pessoais, segredos ou conteúdo proprietário; não publicar binários/assets de terceiros automaticamente.

## 10. Matriz de aceitação e materiais adicionais

| AI Hard Mode | Dual Play | Resultado nesta fase |
|---|---|---|
| OFF | OFF | Não executado |
| ON | OFF | Não executado |
| OFF | ON | Não executado |
| ON | ON | Não executado |

Executar cada cenário com MOD isolado, pares críticos e coleção completa; acrescentar modo nativo de dois humanos com controles independentes. Testar campo/mão/deck, custo real, receitas/provider ausente, mão lógica >5, quotas, Time Wizard X/Quadrado/face-down/roleta/Encore/evoluções, múltiplas origens, transformação/retorno, bônus único, LP/proteções, Surrender Sim/Não/cancelar, novo duelo, save/load, perfil/settings, 3D/HD, frames e drops limitados.

Não prometer migração de savestates da coleção antiga. Core verifica arquitetura, assinatura de MODs e layouts (`guest/state.c:860,920`; `state_subsystems.c:70–80`). Compatibilidade de memory-card saves e de savestates são questões distintas; nenhuma configuração/save pessoal foi alterada.

Materiais adicionais realmente necessários:

- Fonte integral do núcleo de `time-wizard-base-v0.23.1.o`, controlador/roleta e build correspondente, para migração integral de Time Wizard.
- Fonte/pacote AI Hard Mode 0.1.1 citado, caso faça parte da coleção pretendida.
- Build/fontes correspondentes aos objetos atuais Fusion/Ritual para reconciliar proveniência.
- DLC de campos, TCG Frame Colors e packs adicionais efetivamente usados, para concluir essa cobertura.
- Droplist 1.4.0 e sua proveniência/fontes disponíveis, para analisar a UI externa.

Não solicitar novamente fontes de Melhoria/Surrender, Dual, RD, Fusion ou Ritual já presentes. Não foi encontrada imagem completa do jogo ou save pessoal no ZIP; os BIN listados são buffers gráficos coerentes com o código, sem conclusão sobre licença de redistribuição.

Encaminhamentos: HAL-6 mapa de hooks; HAL-7 rituais/Surrender/save; HAL-8 Fusion/campos/visuais; HAL-9 Time Wizard/Dual; HAL-10 AI/adapter/terreno; HAL-17 erro do CI; HAL-18 regressão. Essas issues foram lidas, sem alteração de status ou criação de duplicatas.

## 11. Continuidade autorizada pelo usuário

Após a auditoria, o usuário solicitou publicar estas informações numa branch GitHub para compartilhar com outro chat e manter esse registro nas próximas entregas. Esta autorização cobre a publicação documental e a manutenção de rastreabilidade; não autoriza implementar o plano, mudar mecânicas, modificar terceiros ou mesclar a master.

Nas próximas etapas, registrar resultado real, arquivos afetados, validações e pendências na branch da tarefa; distinguir alterações realizadas de propostas; fornecer links da branch, relatório e PR ao usuário. Manter relatórios anteriores. O protocolo está em [AGENTS.md](../AGENTS.md).

## Apêndices gerados a partir dos insumos auditados

Os apêndices a seguir registram settings, fontes, recursos, objetos e pontos de hook encontrados. São inventário estático; arquivos de teste presentes não equivalem a testes executados. O CSV é a relação integral de caminhos, tamanhos e SHA-256 dos 495 arquivos.

### A. Configurações e fontes por pacote

#### BalanceamentoFusoesEfeitosAuxiliares — 0.1.0-beta.44-effect-ai

ID: `fusion-auxiliary-balance`; library: `fusion-auxiliary-balance`.

Fontes/headers de produção:

```text
battle_coin.h
battle_stat_tween.h
effect_title_instance.h
effect_title_visual.h
egotist_targets.h
fusion_round2_rules.h
fusion_round2_runtime.h
fusion_round3_rules.h
fusion_round3_runtime.h
fusionist_rules.h
fusionist_runtime.h
generated_card_restore.h
logic.h
mod.c
model.h
native.h
native_actions.h
origin_effects_v1.h
owner_f.h
png.c
png.h
ritual_effect_intelligence_v1.h
secondary_dragon_types.h
skull_monster.h
snapshot.h
```

| Chave | Tipo | Default | Limites / choices |
|---|---|---|---|
| `ignore_effect_conditions` | bool | 0 | {} |
| `fusionist_effects` | bool | 1 | {} |
| `elegant_egotist` | bool | 1 | {} |
| `thtd_discharge` | bool | 1 | {} |
| `skull_knight_army` | bool | 1 | {} |
| `tank_siege` | bool | 1 | {} |
| `thousand_flames` | bool | 1 | {} |
| `bickuribox_trick` | bool | 1 | {} |
| `kaiser_protection` | bool | 1 | {} |
| `sunbird_feathers` | bool | 1 | {} |
| `aqua_whirlpool` | bool | 1 | {} |
| `bracchio_forest` | bool | 1 | {} |
| `soul_hunter_feast` | bool | 1 | {} |
| `mammoth_curse` | bool | 1 | {} |
| `ocean_snake_tide` | bool | 1 | {} |
| `eagle_watch` | bool | 1 | {} |
| `sand_blessing` | bool | 1 | {} |
| `empress_protection` | bool | 1 | {} |
| `shark_blood` | bool | 1 | {} |
| `skullbird_breath` | bool | 1 | {} |
| `warrior_memories` | bool | 1 | {} |
| `deepsea_ambush` | bool | 1 | {} |
| `sparrow_beacon` | bool | 1 | {} |
| `pragtical_hunt` | bool | 1 | {} |
| `giltia_lance` | bool | 1 | {} |
| `bugroth_adaptation` | bool | 1 | {} |
| `flame_sword` | bool | 1 | {} |
| `skelgon_fury` | bool | 1 | {} |
| `marine_instinct` | bool | 1 | {} |
| `rare_fish_hunt` | bool | 1 | {} |
| `barox_twist` | bool | 1 | {} |
| `dragoness_armor` | bool | 1 | {} |
| `charubin_spark` | bool | 1 | {} |
| `flame_ghost_burst` | bool | 1 | {} |
| `rose_thorns` | bool | 1 | {} |
| `kwagar_shell` | bool | 1 | {} |
| `karbonala_inversion` | bool | 1 | {} |
| `mavelus_rebirth` | bool | 1 | {} |
| `zombie_sacrifice` | bool | 1 | {} |
| `flower_wolf_camouflage` | bool | 1 | {} |
| `kaminari_paralysis` | bool | 1 | {} |
| `effect_ai` | bool | 1 | {} |
| `effect_ai_trace` | bool | 0 | {} |

Distribuição integral por diretório (nomes completos também no CSV):

| Diretório | Arquivos diretos |
|---|---:|
| `.` | 38 |
| `.pytest_cache` | 3 |
| `.pytest_cache/v/cache` | 1 |
| `assets/attribute-icons` | 6 |
| `assets/coin-normal` | 29 |
| `tests` | 16 |

#### MelhoriaDosRituais — 1.3.6

ID: `melhoria-dos-rituais`; library: `rituals`.

Fontes/headers de produção:

```text
ritual_recipe_provider_v1.h
ritual_rules_provider_v1.h
rules.c
selection.h
surrender.c
surrender.h
```

| Chave | Tipo | Default | Limites / choices |
|---|---|---|---|
| `min_field` | int | 1 | {"min": 0, "max": 3, "step": 1} |
| `use_deck` | bool | 0 | {} |
| `min_hand` | int | 0 | {"min": 0, "max": 3, "step": 1} |
| `require_result` | choice | 0 | {"choices": ["Off", "Hand", "Hand or Deck"]} |
| `ritual_only_player` | bool | 0 | {} |

Distribuição integral por diretório (nomes completos também no CSV):

| Diretório | Arquivos diretos |
|---|---:|
| `.` | 13 |
| `tests` | 5 |

#### Ritual Duelists — 0.4.0-alpha.8

ID: `ritual-duelists`; library: `ritual-duelists`.

Fontes/headers de produção:

```text
adaptive_rules.c
adaptive_rules.h
ai_policy.c
ai_policy.h
box_art.h
deck_access.c
deck_access.h
decks.c
decks.h
decks_original.h
diagnostic_core.c
diagnostic_core.h
field_guard.c
field_guard.h
hand_play.c
hand_play.h
mod.c
native_assets.h
planner.c
planner.h
post_ritual.c
post_ritual.h
profile_data.h
profiles.c
rd_drop_rewards.c
rd_drop_rewards.h
recipe_adapter.c
recipe_adapter.h
ritual_duelists.h
ritual_effect_intelligence_v1.h
ritual_recipe_provider_v1.h
ritual_rules_provider_v1.h
starter_bridge.h
starter_choice_ui.c
starter_rules.c
starter_rules.h
tribute_fetch.c
tribute_fetch.h
```

| Chave | Tipo | Default | Limites / choices |
|---|---|---|---|
| `expansion_enabled` | bool | 1 | {"restart": true} |
| `guaranteed_package_free` | bool | 0 | {"restart": true} |
| `recipe_source` | choice | 0 | {"choices": ["Auto (providers, game tables, Vanilla)", "Force Vanilla", "Force configured Ritual Effects recipe"], "restart": true} |
| `ai_planner` | bool | 1 | {} |
| `diagnostics` | bool | 1 | {} |
| `debug_overlay` | choice | 1 | {"choices": ["Off", "Compact", "Detailed"]} |

Distribuição integral por diretório (nomes completos também no CSV):

| Diretório | Arquivos diretos |
|---|---:|
| `.` | 48 |
| `assets` | 57 |
| `spec` | 6 |
| `tests` | 35 |
| `tests/mocks` | 1 |
| `tests/mocks/game` | 1 |

#### TIME WIZARD — 0.24.0-test

ID: `time-wizard-time-magic`; library: `time-wizard`.

Fontes/headers de produção:

```text
source/time_wizard_ai.c
source/time_wizard_description_fit.c
source/time_wizard_dual_play_compat.c
source/time_wizard_encore.c
source/time_wizard_evolution.c
```

| Chave | Tipo | Default | Limites / choices |
|---|---|---|---|
| `ai_use` | bool | 1 | {} |
| `player_control` | bool | 0 | {} |
| `chance_50` | bool | 0 | {} |
| `dark_sage_id` | int | 0 | {"min": 0, "max": 4096, "step": 1} |
| `look` | choice | 0 | {"choices": ["Original by ZET", "Personalizado (pasta skin)"]} |
| `victory` | choice | 0 | {"choices": ["Explosao de luz com impact frames", "Explosao de luz, sem flashes", "Desligado"]} |
| `sound` | bool | 1 | {} |
| `volume` | int | 100 | {"min": 10, "max": 100, "step": 10} |
| `language` | choice | 1 | {"choices": ["English", "Português", "Español", "Deutsch", "Italiano", "Français"]} |

Distribuição integral por diretório (nomes completos também no CSV):

| Diretório | Arquivos diretos |
|---|---:|
| `.` | 16 |
| `skin` | 1 |
| `sounds` | 4 |
| `source` | 10 |
| `source/tests` | 7 |

#### dual-play — 0.5.7

ID: `dual-play`; library: `dual-play`.

Fontes/headers de produção:

```text
dual-play.c
```

Sem settings no manifest.

Distribuição integral por diretório (nomes completos também no CSV):

| Diretório | Arquivos diretos |
|---|---:|
| `.` | 4 |

#### zzz Efeitos Rituais — 0.78.74-effect-ai

ID: `ritual-card-effects-test`; library: `ritual-card-effects`.

Fontes/headers de produção:

```text
chakra_green_atlas.h
effect_title_instance.h
effect_title_visual.h
egotist_bridge.h
garma_visual.h
hungry_plus500_sprite.h
javelin_shell_sprite.h
mask_effect.h
mod.c
model.h
native.h
native_actions.h
native_vfx_textures.h
owner_r.h
performance_chaos.h
png.c
png.h
ritual_ai_burn_policy.h
ritual_ai_effect_source.h
ritual_coin.h
ritual_effect_intelligence_v1.h
ritual_recipe_compat.h
ritual_recipe_data.h
ritual_recipe_export.h
ritual_recipe_provider_v1.h
ritual_viewer_description_fit.h
serpent_fear.h
skull_monster.h
snapshot.h
tri_horn_sprite.h
tri_horned.h
```

| Chave | Tipo | Default | Limites / choices |
|---|---|---|---|
| `recipes_bulk` | choice | 0 | {"choices": ["Keep individual settings", "Set ALL ON", "Set ALL OFF"], "restart": true} |
| `effects_bulk` | choice | 0 | {"choices": ["Keep individual settings", "Set ALL ON", "Set ALL OFF"], "restart": true} |
| `title_color_diagnostic` | bool | 0 | {} |
| `millennium_shield_recipe` | bool | 1 | {"restart": true} |
| `millennium_shield` | bool | 1 | {} |
| `yamadron_recipe` | bool | 1 | {"restart": true} |
| `yamadron` | bool | 1 | {} |
| `psycho_puppet_recipe` | bool | 1 | {"restart": true} |
| `psycho_puppet` | bool | 1 | {} |
| `hungry_burger_recipe` | bool | 1 | {"restart": true} |
| `hungry_burger` | bool | 1 | {} |
| `fiends_mirror_recipe` | bool | 1 | {"restart": true} |
| `fiends_mirror` | bool | 1 | {} |
| `mask_shine_dark_recipe` | bool | 1 | {"restart": true} |
| `mask_shine_dark` | bool | 1 | {} |
| `javelin_beetle_recipe` | bool | 1 | {"restart": true} |
| `javelin_beetle` | bool | 1 | {} |
| `gate_guardian_recipe` | bool | 1 | {"restart": true} |
| `gate_guardian` | bool | 1 | {} |
| `crab_turtle_recipe` | bool | 1 | {"restart": true} |
| `crab_turtle` | bool | 1 | {} |
| `fortress_whale_recipe` | bool | 1 | {"restart": true} |
| `fortress_whale` | bool | 1 | {} |
| `garma_sword_recipe` | bool | 1 | {"restart": true} |
| `garma_sword` | bool | 1 | {} |
| `tri_horned` | bool | 1 | {} |
| `serpent_night_recipe` | bool | 1 | {"restart": true} |
| `serpent_night` | bool | 1 | {} |
| `performance_sword_recipe` | bool | 1 | {"restart": true} |
| `performance_sword` | bool | 1 | {"restart": true} |
| `black_chaos` | bool | 1 | {"restart": true} |
| `super_war_lion_recipe` | bool | 1 | {"restart": true} |
| `super_war_lion` | bool | 1 | {} |
| `sengenjin_recipe` | bool | 1 | {"restart": true} |
| `sengenjin` | bool | 1 | {} |
| `black_luster_soldier` | bool | 1 | {} |
| `blue_eyes_ultimate` | bool | 1 | {} |
| `cosmo_queen` | bool | 1 | {} |
| `dokurorider_recipe` | bool | 1 | {"restart": true} |
| `dokurorider` | bool | 1 | {} |
| `chakra_recipe` | bool | 1 | {"restart": true} |
| `chakra` | bool | 1 | {} |
| `zera` | bool | 1 | {} |
| `skull_guardian_recipe` | bool | 1 | {"restart": true} |
| `skull_guardian` | bool | 1 | {} |
| `effects_without_ritual_summon` | bool | 0 | {"restart": true} |
| `effect_ai` | bool | 1 | {} |
| `effect_ai_trace` | bool | 0 | {} |

Distribuição integral por diretório (nomes completos também no CSV):

| Diretório | Arquivos diretos |
|---|---:|
| `.` | 39 |
| `assets` | 9 |
| `assets/coin-atem` | 25 |
| `assets/coin-normal` | 29 |
| `tests` | 27 |
| `text` | 65 |

### B. Objetos: hashes, arquitetura e imports

Os hashes abaixo foram calculados dos anexos, sem rebuild. A disponibilidade dos imports foi comparada às listas do SDK oficial v0.3.1-preview.2.

| Objeto relativo ao ZIP | Bytes | Alvo | Imports | Ausentes | SHA-256 |
|---|---:|---|---:|---:|---|
| `BalanceamentoFusoesEfeitosAuxiliares/fusion-auxiliary-balance.o` | 536660 | i386 | 131 | 0 | `1d46c5f812759de21564e20ab88eddc79a268534c6e1055b7051b3dfefd88821` |
| `BalanceamentoFusoesEfeitosAuxiliares/fusion-auxiliary-balance.x86_64-windows.o` | 628512 | x86_64-windows | 130 | 0 | `252baeae938481810b1d78a4523e08c0c7babbe4efea828e1bb638d8495b68a5` |
| `MelhoriaDosRituais/rituals.o` | 87508 | i386 | 33 | 0 | `f9827330ced3dcbb94fb47d39687d7ce63d8ca50b355b1b4a2747c81e634111f` |
| `MelhoriaDosRituais/rituals.x86_64-windows.o` | 142664 | x86_64-windows | 33 | 0 | `19a5a4adb5558eedecfb56d105d8214b3289f8d5f8def0980c6732e8b294a4c1` |
| `Ritual Duelists/ritual-duelists.o` | 4510568 | i386 | 87 | 0 | `9b1bdb0474b27bf01995feaa96b8f30be42038de6367031bfd2437ffe4c59219` |
| `Ritual Duelists/ritual-duelists.x86_64-windows.o` | 4708008 | x86_64-windows | 87 | 0 | `d3d0e8adceb6d86a4edf7ff216bab7095343afdf1c6bfc54131f5ae0ad8e8538` |
| `TIME WIZARD/source/time-wizard-base-v0.23.1.o` | 150244 | i386 | 103 | 0 | `5b4252dafbc07532f859cda92e49e75d15e531a880f90cc7924371b7e37f9b4a` |
| `TIME WIZARD/time-wizard.o` | 277544 | i386 | 105 | 0 | `828a5879a81ee33078c7cd2e60ba7c8e87557f889b035455211ed700ca8d6b07` |
| `dual-play/dual-play.o` | 67832 | i386 | 1 | 0 | `e08e198aa84783b4c04b9c9f117de0f521240a1c19b8f12e8bc1d30fc0769b90` |
| `zzz Efeitos Rituais/ritual-card-effects.o` | 964464 | i386 | 185 | 0 | `f30934e77d9705b0e1122f9eed23e015533d47e85d0b01f98c0b37912c264170` |
| `zzz Efeitos Rituais/ritual-card-effects.x86_64-windows.o` | 1147880 | x86_64-windows | 181 | 0 | `3dfcd399fb5c3a48aa8c3839b5cfebfedb79b8e9717c5a7b27056bda94d39517` |

#### Divergências de SDK-PROVENANCE.json

| Pacote / objeto | Hash declarado | Hash recebido |
|---|---|---|
| `BalanceamentoFusoesEfeitosAuxiliares/fusion-auxiliary-balance.o` | `2ca29e9f763d81b5bbb6b83476dc83f872ae26baab3638405502418c410683cb` | `1d46c5f812759de21564e20ab88eddc79a268534c6e1055b7051b3dfefd88821` |
| `BalanceamentoFusoesEfeitosAuxiliares/fusion-auxiliary-balance.x86_64-windows.o` | `7fb6b1898ad05412bcfd0e53f1acad6cc124d83589969957f2e69d4f4fed323f` | `252baeae938481810b1d78a4523e08c0c7babbe4efea828e1bb638d8495b68a5` |
| `zzz Efeitos Rituais/ritual-card-effects.o` | `c501c5c5fe5406a2606651f8df9f5ba98f895242e10c50f116778c9ecc3361e6` | `f30934e77d9705b0e1122f9eed23e015533d47e85d0b01f98c0b37912c264170` |
| `zzz Efeitos Rituais/ritual-card-effects.x86_64-windows.o` | `b07075974d99a815c1cd4ae33916ca972c9f92021196f654aff0a4fa2365cc56` | `3dfcd399fb5c3a48aa8c3839b5cfebfedb79b8e9717c5a7b27056bda94d39517` |

Time Wizard: as 37 entradas de `SHA256SUMS.txt` conferem com os arquivos recebidos. Isso não supre os fontes ausentes nem prova compatibilidade com outro SDK.

### C. Pontos de hook encontrados nos fontes recebidos

Ocorrências extraídas dos fontes de produção; não são uma cadeia runtime comprovada. Time Wizard tem adapters e um núcleo compilado sem fonte, portanto esta lista não cobre integralmente seu núcleo. Os seis alvos dinâmicos de Dual Play foram resolvidos a partir das strings do init.

#### fusion-auxiliary-balance

| Alvo | Fonte no pacote | Linha |
|---|---|---:|
| `AiScript_FindBestAttack` | `mod.c` | 2361 |
| `AiScript_PlayFieldCard` | `mod.c` | 2362 |
| `AiScript_LoadGuardianStarChoice` | `mod.c` | 2363 |
| `AiScript_FindKiller` | `mod.c` | 2364 |
| `AiScript_CalcCardPower` | `mod.c` | 2365 |
| `Duel_CheckFusion` | `mod.c` | 2366 |
| `func_80023144` | `mod.c` | 2367 |
| `func_80035E20` | `mod.c` | 2368 |
| `DuelEffect_AppendEntry` | `mod.c` | 2369 |
| `Duel_CheckEquip` | `mod.c` | 2370 |
| `Ai_CompleteFusion` | `mod.c` | 2371 |
| `Duel_CalcCardStats` | `mod.c` | 2372 |
| `func_8001EFD4` | `mod.c` | 2373 |
| `DuelEffect_ApplyLifePointRecovery` | `mod.c` | 2374 |
| `DuelCard_RemoveFromField` | `mod.c` | 2375 |
| `DuelEffect_ApplyBoardDestruction` | `mod.c` | 2376 |
| `DuelEffect_ApplyRaigeki` | `mod.c` | 2377 |
| `DuelEffect_ApplyMonsterRemoval` | `mod.c` | 2378 |
| `DuelScene_UpdateBattle` | `mod.c` | 2379 |
| `DuelScene_UpdateTurnSwitch` | `mod.c` | 2380 |
| `DuelCard_UpdateDefenseRotation` | `mod.c` | 2381 |
| `DuelScene_UpdateResume` | `mod.c` | 2382 |
| `func_80024D34` | `mod.c` | 2383 |
| `DuelScene_Update` | `mod.c` | 2384 |
| `DuelScene_UpdateHandActions` | `mod.c` | 2385 |
| `DuelScene_UpdateCardPlacement` | `mod.c` | 2386 |
| `func_8001B170` | `mod.c` | 2387 |
| `DuelEffect_UpdateCardEffect` | `mod.c` | 2388 |
| `DuelScene_UpdateFieldActions` | `mod.c` | 2389 |
| `DuelEffect_UpdateRequests` | `mod.c` | 2390 |
| `Duel_GetBaseCardStat` | `mod.c` | 2391 |
| `AiScript_FindBestCombo` | `mod.c` | 2392 |
| `AiScript_EvaluateFusion` | `mod.c` | 2393 |

#### melhoria-dos-rituais

| Alvo | Fonte no pacote | Linha |
|---|---|---:|
| `Duel_CheckQuitInput` | `surrender.c` | 71 |
| `DuelScene_UpdateWithSideInput` | `surrender.c` | 74 |
| `Duel_CheckRitual` | `rules.c` | 1332 |
| `Duel_RitualFieldObjects` | `rules.c` | 1335 |
| `DuelCard_DeactivateRecord` | `rules.c` | 1338 |
| `DuelScene_UpdateHandActions` | `rules.c` | 1341 |
| `Duel_PopulateCombinedDeckData` | `rules.c` | 1344 |

#### ritual-duelists

| Alvo | Fonte no pacote | Linha |
|---|---|---:|
| `DuelEffect_ApplyRitual` | `mod.c` | 1242 |
| `Duel_CheckFusion` | `mod.c` | 1244 |
| `Duel_ShuffleDeck` | `mod.c` | 1246 |
| `Ai_GetHandSize` | `mod.c` | 1248 |
| `func_8001BAF0` | `mod.c` | 1250 |
| `AiScript_Run` | `mod.c` | 1252 |
| `Duel_SelectCardDrop` | `rd_drop_rewards.c` | 224 |

#### time-wizard-time-magic

| Alvo | Fonte no pacote | Linha |
|---|---|---:|
| `func_80037DA4` | `source/time_wizard_description_fit.c` | 235 |
| `func_80035E20` | `source/time_wizard_description_fit.c` | 237 |
| `AiScript_Run` | `source/time_wizard_ai.c` | 1139 |
| `DuelScene_UpdateCardUse` | `source/time_wizard_ai.c` | 1140 |
| `DuelScene_UpdateFieldActions` | `source/time_wizard_ai.c` | 1141 |
| `func_8001B938` | `source/time_wizard_ai.c` | 1142 |
| `DuelScene_UpdateDrawPhase` | `source/time_wizard_dual_play_compat.c` | 1191 |
| `DuelScene_UpdateHandActions` | `source/time_wizard_dual_play_compat.c` | 1194 |
| `DuelScene_UpdateCardUse` | `source/time_wizard_dual_play_compat.c` | 1197 |
| `func_8001B938` | `source/time_wizard_dual_play_compat.c` | 1200 |
| `AiScript_Run` | `source/time_wizard_dual_play_compat.c` | 1203 |
| `func_8001BAF0` | `source/time_wizard_dual_play_compat.c` | 1206 |

#### dual-play

| Alvo | Fonte no pacote | Linha |
|---|---|---:|
| `DuelScene_UpdateHandActions` | `dual-play.c` | 1377 |
| `DuelScene_UpdateDrawPhase` | `dual-play.c` | 1382 |
| `DuelScene_UpdateFieldActions` | `dual-play.c` | 1390 |
| `func_80028220` | `dual-play.c` | 1400 |
| `func_8001BAF0` | `dual-play.c` | 1415 |
| `Duel_InitScene` | `dual-play.c` | 1430 |

#### ritual-card-effects-test

| Alvo | Fonte no pacote | Linha |
|---|---|---:|
| `Input_UpdatePads` | `mod.c` | 7498 |
| `Main_RunDuel` | `mod.c` | 7499 |
| `Main_RunAnimatedBattle` | `mod.c` | 7500 |
| `func_800559D4` | `mod.c` | 7501 |
| `DuelCard_UpdateDefenseRotation` | `mod.c` | 7502 |
| `func_80017F04` | `mod.c` | 7503 |
| `AiScript_Run` | `mod.c` | 7504 |
| `AiScript_FindWeakest` | `mod.c` | 7505 |
| `Ai_IsCardInSets` | `mod.c` | 7506 |
| `AiScript_FindBestAttack` | `mod.c` | 7507 |
| `func_80037DA4` | `mod.c` | 7508 |
| `func_80035E20` | `mod.c` | 7509 |
| `func_80027508` | `mod.c` | 7510 |
| `func_80024D34` | `mod.c` | 7511 |
| `DuelScene_Update` | `mod.c` | 7512 |
| `func_80023144` | `mod.c` | 7513 |
| `Duel_DrawFieldCards` | `mod.c` | 7514 |
| `func_80016784` | `mod.c` | 7515 |
| `DuelEffect_ApplyRitual` | `mod.c` | 7516 |
| `Duel_CalcCardStats` | `mod.c` | 7517 |
| `Duel_CalcBattleAttack` | `mod.c` | 7518 |
| `Duel_CalcBattleDefense` | `mod.c` | 7519 |
| `Duel_CalcGuardianStarBonus` | `mod.c` | 7520 |
| `func_8001EFD4` | `mod.c` | 7521 |
| `func_8001B170` | `mod.c` | 7522 |
| `DuelEffect_ApplyStatPenalty` | `mod.c` | 7523 |
| `DuelEffect_ApplyCursebreaker` | `mod.c` | 7524 |
| `DuelCard_RemoveFromField` | `mod.c` | 7525 |
| `DuelCard_DeactivateRecord` | `mod.c` | 7526 |
| `Duel_ResetCardRecords` | `mod.c` | 7527 |
| `Duel_PopulateCombinedDeckData` | `mod.c` | 7528 |
| `DuelScene_UpdateFieldActions` | `mod.c` | 7529 |
| `DuelScene_UpdateCardPlacement` | `mod.c` | 7530 |
| `DuelScene_UpdateHandActions` | `mod.c` | 7531 |
| `func_8001BAF0` | `mod.c` | 7532 |
| `DuelScene_UpdateBattle` | `mod.c` | 7533 |
| `DuelEffect_UpdateRequests` | `mod.c` | 7534 |
| `DuelEffect_UpdateCardEffect` | `mod.c` | 7535 |
| `DuelScene_UpdateDrawPhase` | `mod.c` | 7536 |
| `DuelEffect_ApplyBoardDestruction` | `mod.c` | 7537 |
| `DuelEffect_ApplyMonsterRemoval` | `mod.c` | 7538 |
| `DuelEffect_ApplyRaigeki` | `mod.c` | 7539 |
| `DuelScene_UpdateTurnSwitch` | `mod.c` | 7540 |
| `Duel_CheckFusion` | `mod.c` | 7541 |
| `Duel_CheckRitual` | `mod.c` | 7542 |
| `AiScript_PlayFieldCard` | `mod.c` | 7547 |
| `AiScript_LoadGuardianStarChoice` | `mod.c` | 7547 |
| `AiScript_FindKiller` | `mod.c` | 7547 |
| `AiScript_CalcCardPower` | `mod.c` | 7547 |
| `AiScript_FindBestAttack` | `mod.c` | 7547 |

### D. Grupos de arquivos com conteúdo idêntico

Não remover automaticamente essas cópias: várias mantêm os MODs independentes.

`064f497e1a2e95efa9421bd7e424fd1c7641fb38f352b107e704f4e4bbc4b3dc`

- `zzz Efeitos Rituais/assets/coin-atem/11.png`
- `zzz Efeitos Rituais/assets/coin-atem/13.png`

`0d02c207134db14c400e4cf0eb743647d2bffeb4b9d7a4b05a7cddd8a0a3f319`

- `BalanceamentoFusoesEfeitosAuxiliares/tests/test_effect_title_instance.c`
- `zzz Efeitos Rituais/tests/test_effect_title_instance.c`

`125d6f82f6e722c50bf828c944371ceb05b6021179c49e61867d5dda281633c2`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/10.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/18.png`
- `zzz Efeitos Rituais/assets/coin-normal/10.png`
- `zzz Efeitos Rituais/assets/coin-normal/18.png`

`138ed4699e54bd5c0e54b5d0ef53020b5cce81da897984e526702fde59c23f7c`

- `MelhoriaDosRituais/ritual_recipe_provider_v1.h`
- `Ritual Duelists/ritual_recipe_provider_v1.h`
- `zzz Efeitos Rituais/ritual_recipe_provider_v1.h`

`1758e64d056cb1ba0131978e5e81a69ffbeecfdd06788def231d5a83c93c703d`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/05.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/23.png`
- `zzz Efeitos Rituais/assets/coin-normal/05.png`
- `zzz Efeitos Rituais/assets/coin-normal/23.png`

`1817dadedc65e5ec4aeb43709a6df60a0a5ac2e5a810d06c761fbb7b0fc712d9`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/13.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/15.png`
- `zzz Efeitos Rituais/assets/coin-normal/13.png`
- `zzz Efeitos Rituais/assets/coin-normal/15.png`

`1b7119ae2b76962f119730d3972e61e6b25b3eb7dbfba11ab50158481a5545ca`

- `zzz Efeitos Rituais/assets/coin-atem/03.png`
- `zzz Efeitos Rituais/assets/coin-atem/21.png`

`26a5ef08cf76239964ea952185c7f9428cd2aaed4e087f5d6feafc37b4a2272d`

- `BalanceamentoFusoesEfeitosAuxiliares/png.h`
- `zzz Efeitos Rituais/png.h`

`2c1785c3c074fc11672cf4aa0209f5f2849c5224039a6ae02fab86c1c1ac0ca1`

- `zzz Efeitos Rituais/assets/coin-atem/05.png`
- `zzz Efeitos Rituais/assets/coin-atem/19.png`

`2c69804f31229f622b5f504630b4b0b0ecb3fcd889a983680c6bee9e59bbf47c`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/04.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/24.png`
- `zzz Efeitos Rituais/assets/coin-normal/04.png`
- `zzz Efeitos Rituais/assets/coin-normal/24.png`

`31a3959eca966ff97c8392a943a0bc5665701ede56552806d29277e35a92b5b9`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/01.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/27.png`
- `zzz Efeitos Rituais/assets/coin-normal/01.png`
- `zzz Efeitos Rituais/assets/coin-normal/27.png`

`3301b0585c936cd90d3557c2d09d08d630540953ac0a511faa18fa5ff98da607`

- `zzz Efeitos Rituais/assets/coin-atem/06.png`
- `zzz Efeitos Rituais/assets/coin-atem/18.png`

`33b45eff6855d6da800367a35a6fd1f165b970fd800124a388be8b2d436a9982`

- `BalanceamentoFusoesEfeitosAuxiliares/snapshot.h`
- `zzz Efeitos Rituais/snapshot.h`

`365be3f2a2e4e7c28d90c357d609a0096bea730dedb4c37ae146c6dcbfa92e64`

- `zzz Efeitos Rituais/assets/coin-atem/08.png`
- `zzz Efeitos Rituais/assets/coin-atem/16.png`

`37291c8f07f327254b5531bb8142a457aad056806cb0b75662675710cddd699e`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/14.png`
- `zzz Efeitos Rituais/assets/coin-normal/14.png`

`3a671b6f849e52102db52389dece4ea660604eea8167b51172f774999968aa5b`

- `zzz Efeitos Rituais/assets/coin-atem/04.png`
- `zzz Efeitos Rituais/assets/coin-atem/20.png`

`3a8c889df2b8801fe2cc2c5965253fa138ebf2af13780c82b8f38d21c7aa478e`

- `BalanceamentoFusoesEfeitosAuxiliares/skull_monster.h`
- `zzz Efeitos Rituais/skull_monster.h`

`3d839cfc0be55e7c2f00a5b3559813a2d32971935d7f1ba92633b682bcf9088f`

- `zzz Efeitos Rituais/assets/coin-atem/02.png`
- `zzz Efeitos Rituais/assets/coin-atem/22.png`

`453bb326f4780d9222f8cf3dc9427f1750fba4c7f3ed202c494180d6587374b0`

- `zzz Efeitos Rituais/assets/coin-atem/07.png`
- `zzz Efeitos Rituais/assets/coin-atem/17.png`

`484b37201af8c606cdb9dde8d8885d5f1affde80acbecc6a9884ccf0cda258b8`

- `BalanceamentoFusoesEfeitosAuxiliares/effect_title_instance.h`
- `zzz Efeitos Rituais/effect_title_instance.h`

`497efdabc6ade9bd12dd92f2a479ac86a9564632c4ae04f33029c1c1a5f060df`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/00.png`
- `zzz Efeitos Rituais/assets/coin-normal/00.png`

`5684308a9c1ca94464bd08eca79d8b7b516565fe8efc0d72430b92af329c6500`

- `MelhoriaDosRituais/ritual_rules_provider_v1.h`
- `Ritual Duelists/ritual_rules_provider_v1.h`

`5890a4e33435d6515b571ca14ca6dbd483c765dcc0027d86c33dffde08b9d2d9`

- `BalanceamentoFusoesEfeitosAuxiliares/native.h`
- `zzz Efeitos Rituais/native.h`

`5d21f5dc31e51d0d625ced432b60d7a7e0d70c76f8bbfdd8eeb7e08e54021ccf`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/09.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/19.png`
- `zzz Efeitos Rituais/assets/coin-normal/09.png`
- `zzz Efeitos Rituais/assets/coin-normal/19.png`

`6a79338a43aa97ff3506d5fb93f9685769555abb1b6c9cdf9c8a85a649b95d22`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/coin.txt`
- `zzz Efeitos Rituais/assets/coin-normal/coin.txt`

`70749174b4208864f3c062b81c90f338f64421b4c801c30b289b8001787326e0`

- `BalanceamentoFusoesEfeitosAuxiliares/native_actions.h`
- `zzz Efeitos Rituais/native_actions.h`

`76b8438a521e3e52853c3d26253e9dcf7aef4267845282e2f00cffe4d840de07`

- `zzz Efeitos Rituais/text/gate_recipe_off.txt`
- `zzz Efeitos Rituais/text/gate_recipe_on.txt`

`7eb7821a79b080d660f9f816a400787445c8bb741d43aa0b91a5b55a5e53a4d2`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/11.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/17.png`
- `zzz Efeitos Rituais/assets/coin-normal/11.png`
- `zzz Efeitos Rituais/assets/coin-normal/17.png`

`9646f4c50c51dea2470400d7f01521bebf9e9d047e9356885b3a9536b2f376eb`

- `BalanceamentoFusoesEfeitosAuxiliares/model.h`
- `zzz Efeitos Rituais/model.h`

`a2ccc08933af7dd06db84a379382ad6f7e47687991b1c934a9935f180faef131`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/21.png`
- `zzz Efeitos Rituais/assets/coin-normal/21.png`

`a5a55b3b349046813fd8c8e26d347527acda6de856858e9aacb7028b95935d06`

- `BalanceamentoFusoesEfeitosAuxiliares/png.c`
- `zzz Efeitos Rituais/png.c`

`b5791082a2266892b8c8da0440b1a0ffea7223bd801fd2a1e60c1ab3378935c6`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/08.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/20.png`
- `zzz Efeitos Rituais/assets/coin-normal/08.png`
- `zzz Efeitos Rituais/assets/coin-normal/20.png`

`b9d3134f5a3eeaa94e351c4bebf992ef4915ca3dc174af3ed27a0775ae602c1e`

- `zzz Efeitos Rituais/assets/coin-atem/01.png`
- `zzz Efeitos Rituais/assets/coin-atem/23.png`

`c0ccf63d06a13a6c38721c98aafb49a7a400e881e4e559bde4d9e5ee615719cf`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/07.png`
- `zzz Efeitos Rituais/assets/coin-normal/07.png`

`c15326a25398bafe7411df9d5cadb1092e05c01b58551a1eb0e5ac78ae119e28`

- `zzz Efeitos Rituais/assets/coin-atem/10.png`
- `zzz Efeitos Rituais/assets/coin-atem/14.png`

`c97f438ed700671f0b33eaa0743eb90819777bef1d75e42fbcc0c39c97aee7d2`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/06.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/22.png`
- `zzz Efeitos Rituais/assets/coin-normal/06.png`
- `zzz Efeitos Rituais/assets/coin-normal/22.png`

`cd9d8c081d869336b9fbda52467034d8ac3637a65ab6f18dc931f566bc47efa4`

- `zzz Efeitos Rituais/assets/coin-atem/09.png`
- `zzz Efeitos Rituais/assets/coin-atem/15.png`

`d84fb96bfb0a5f38a3e7d4c231e1c36f60e52569f418b87e310631056dfb8a26`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/12.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/16.png`
- `zzz Efeitos Rituais/assets/coin-normal/12.png`
- `zzz Efeitos Rituais/assets/coin-normal/16.png`

`dc46868090ad84a2b4ece1dc40e4a6fe2898bfd2fd5b8f79c28bbca8f3a0e579`

- `BalanceamentoFusoesEfeitosAuxiliares/ritual_effect_intelligence_v1.h`
- `Ritual Duelists/ritual_effect_intelligence_v1.h`
- `zzz Efeitos Rituais/ritual_effect_intelligence_v1.h`

`ecddd31bfd2944643a220f729496354032a26b3b331eea0acb6635f6a10ae5a4`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/03.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/25.png`
- `zzz Efeitos Rituais/assets/coin-normal/03.png`
- `zzz Efeitos Rituais/assets/coin-normal/25.png`

`fcfd9721e9a855f7a80ab96505a09766da20af868f420f02a79bbaf5ec44587e`

- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/02.png`
- `BalanceamentoFusoesEfeitosAuxiliares/assets/coin-normal/26.png`
- `zzz Efeitos Rituais/assets/coin-normal/02.png`
- `zzz Efeitos Rituais/assets/coin-normal/26.png`

