# HAL-5 — Auditoria e inventário dos MODs — Fase 1

Data da auditoria: 11/11/2026. Publicação documental posterior: este PR adiciona somente o relatório; as afirmações de ausência de alterações referem-se à fase de auditoria anterior. Referência: [HAL-5](https://linear.app/half-programer/issue/HAL-5/aud-inventariar-mods-versoes-e-fontes-de-codigo).

## 1. Resultado e limites

A coleção completa do Prompt Mestre V3 **não está disponível em uma única revisão**. Foram inventariadas as dez branches remotas anunciadas pelo fork, a tag `ritual-editor-test`, o checkout restaurado e o conteúdo do ZIP de rituais. Há cinco MODs distribuídos na base, um MOD de campos em outra branch, duas versões de Ritual Effects e uma implementação parcial de Melhoria dos Rituais em uma tag. Dual Play e vários outros módulos da coleção não foram encontrados.

Esta entrega é análise estática: leitura de Git, manifests, fontes, documentação, headers e símbolos ELF. **Nenhuma compilação, teste, jogo, instalador ou código dos pacotes foi executado nesta fase.** Não houve checkout de outra branch, alteração de fonte, mecânica, configuração, workflow ou arquivo versionado. O único fetch atualizou objetos e referências Git. O relatório e as cópias de evidências estão em `/workspace/auditoria-HAL-5`, fora do checkout.

As instruções do anexo foram tratadas como referência para o inventário; a solicitação atual restringe o trabalho à Fase 1/HAL-5. Declarações de aprovação, benchmarks e testes dentro dos pacotes são histórico dos autores, não autorização atual nem testes reproduzidos aqui. Não há P0 comprovado. Os riscos abaixo distinguem fatos do código de hipóteses sobre execução.

## 2. Base real e cobertura de referências

- Repositório: `otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled`.
- Branch local: **`work`**; HEAD `0d78202729967a5bbdbc886aa128e48e82bef7ac`, igual a `origin/master`.
- A consulta Git ao upstream oficial confirmou que a tag **`v0.2.1-preview.1` aponta exatamente para esse HEAD**. Não é um checkout v0.3.1-preview.1.
- API local: **10**, em `src/pc/mods/mod_types.h:3`. API mínima declarada pelos MODs não identifica o SDK exato usado para compilar.
- O binário Linux retido tem `Memories_Version[] = ""` em `tmp/pc/game32/version.c:1`: build de desenvolvimento. O gerador usa variável/tag exata (`tools/pc/build_game32.py:854`); não foi alterado para fabricar uma versão.
- A tag upstream `v0.3.0` resolve ao commit `663626b1d38355dd11f5532894a2b94169e7f349`; `v0.3.1-preview.1`, a `10ee7e5ecf756f9e8f2dab71bfdf29b0e593c505`. Foram verificadas suas referências, sem mudar a base nem auditar esses dois snapshots como se fossem o checkout.

| Referência remota/tag | SHA abreviado | Material relevante no snapshot |
|---|---|---|
| `master` | `0d7820272996` | Cinco MODs da base; API 10 |
| `mod-field-effects` | `296c59194797` | Campos 0.19 + variante do AI Hard Mode; API 9 |
| `mod-ritual-effects` | `632cd012deef` | Ritual Effects 0.74; API 7 |
| `downloads/efeitos-rituais-v0.78.40` | `561d3d97818c` | Branch com somente o ZIP 0.78.40; não é uma árvore de build do jogo |
| `ritual-requirements` | `c2e8783aa2ba` | Extensões de receitas/seleção no core e editor; API 7; sem novo MOD de duelistas |
| `ritual-requirements-v014` | `8ba422b58b49` | Snapshot de base/API 8; sem pacote independente adicional nas pontas |
| `backup/master-pre-v0.2.0` | `4f58718bb897` | Base antiga/API 7; variantes anteriores dos cinco MODs |
| `backup/master-pre-v0.2.1-preview.1` | `7a8da6d964dd` | Base/API 9; sem novo MOD independente |
| `backup/mod-field-effects-pre-v0.2.0` | `985609424e7e` | Campos 0.1-dev; API 7 |
| `upstream-v0.2.0` | `0185f58df792` | Igual ao commit da tag oficial v0.2.0; API 9 |
| tag `ritual-editor-test` | `4162e8d3c862` | `user-mods/melhoria-dos-rituais` 1.1.0-test; fontes e fixture |

A tag de teste não é ancestral da ponta atual de `ritual-requirements`; por isso seu conteúdo foi examinado separadamente. Também foi pesquisado o histórico de adição de `mod.json`: há nomes antigos como `2d-monsters`, `card-name-color`, `drop-odds`, `menu-back-confirm` e `Free Duel Portraits HD`. Não são pacotes atuais encontrados nas pontas inventariadas; não foram confundidos com os MODs customizados solicitados. Fontes UI atualmente agrupadas em Yamyi e o modo Card art de 3D Monsters foram identificados pelo código atual.

As referências e versões verificadas estão registradas nas tabelas deste relatório. Os manifests completos podem ser consultados no GitHub nas respectivas revisões. Inventários JSON auxiliares foram gerados no ambiente de auditoria; não acompanham este PR, que contém somente o relatório.

## 3. Inventário verificado

### 3.1 MODs na base atual

Todos estão em `mods/<id>/`. Os quatro de código têm objetos ELF32 retidos em `tmp/pc/game32/mods/<id>/<library>.o`, produzidos no onboarding anterior; isso não prova que estejam ativos numa instalação do usuário. Nenhum desses objetos é versionado no master. O quinto é de dados e não necessita código C.

| Nome / ID / versão | Fontes | Artefato local | Configuração e compatibilidade declarada | Função / dependências |
|---|---|---|---|---|
| **3D Monsters** / `3d-monsters` / **1.0** | `field_models.c`, `field_art.c`, `field_art.h` | `3d-monsters.o`, 150.700 bytes | `enabled:true`; 15 opções de estilo, escala, brilho e batalha; sem `min_api`/`game` no manifest | Modelos ou arte sobre as cartas; usa dados do disco, memória/GTE/render do host; sem `requires` de outro MOD |
| **AI Hard Mode** / `ai-hard-mode` / **0.1.0** | `ai_hard_mode.c`, `planner.c`, `planner.h` | `ai-hard-mode.o`, 114.752 bytes | `enabled:false`; `min_api:4`, `slus_01411`; 17 opções; `search_budget:6000`, `deck_access:0`, `hidden_cards:0` | Planejamento de mão, fusão, estrelas, ataques e magias; consulta regras do jogo; sem dependência obrigatória de outro MOD |
| **Drop missing cards** / `drop-missing-cards` / **1.0** | `mod.json` com tabelas `drops`; sem C por desenho | Manifest de dados | `enabled:false`; sem `min_api`/`requires` | Redistribui pesos de drops de 82 cartas, conforme descrição; não implementa efeitos de monstros nem Ritual Duelists |
| **Hand camera** / `hand-camera` / **1.0** | `hand_camera.c` | `hand-camera.o`, 10.000 bytes | `enabled:true`; 6 opções; preserva `legacy_setting:hand_camera` | Câmera na seleção da mão; lê pad 0; sem `requires` |
| **Yamyi Mods** / `yamyi-mods` / **1.0** | `yamyi_mods.c`, `card_name_color.c`, `menu_back_confirm.c` | `yamyi-mods.o`, 134.392 bytes | `enabled:false`; `min_api:4`, `slus_01411`, `restart:false`; 13 opções; `card_text_colors` vazio no manifest atual | Confirmação de retorno ao título, Library/drop odds e infraestrutura de cores; não é Surrender em duelo nem TCG Frame Colors |

Evidências: [manifests da base](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/0d78202729967a5bbdbc886aa128e48e82bef7ac/mods). 3D Monsters, Hand camera e Yamyi compilados com esta árvore anunciam `MEMORIES_MOD_API` (=10) em seu init; AI Hard Mode anuncia 4. Não equiparar `min_api` omitida a compatibilidade universal.

**Variantes do AI Hard Mode:** o manifest continua 0.1.0 em várias bases, mas há pelo menos três conteúdos de fontes distintos: antigo (`ai_hard_mode.c` blob `cdb0d3d4bcf5…`), master (`aa112ed7cde8…`) e branch de campos (`811228308499…`). `planner.c` também muda. A variante de campos adiciona callback de terreno por identidade de carta; não é intercambiável copiando apenas um arquivo do planner. Os prefixos de blobs acima distinguem as variantes examinadas; os respectivos commits estão na tabela de referências.

### 3.2 Materiais recuperados de outras referências

| MOD / versão / ID | Origem e fontes | Binário/configuração | Estado verificado |
|---|---|---|---|
| **Field Effects + Secondary Types 0.19-adaptive-description-test** / `field-effects-secondary-types` | `mod-field-effects`, `mods/Yugioh-Operacao-MODs/Field-Effects-e-Tipos-Secundarios/dev/`; `field_effects.c` | `library:field_effects`, **sem objeto versionado nessa ponta**; API mínima 4; `slus_01411`; desativado; uma opção de destaque + `terrain_bonus` | Regras secundárias de terrenos e rodapé/descrição adaptativa; fornece `terrain-card-delta-v1`; sem `requires`. Docs citam build com SDK v0.2.1-preview.1 e ausência de validação Windows, não reproduzida aqui |
| **Field Effects 0.1-dev** / mesmo ID | Backup `985609424e7e…`, mesmo caminho relativo; um C | Sem objeto versionado; desativado; API mínima 4 | Variante antiga; não promover a versão atual nem instalar junto com 0.19 |
| **Ritual Effects 0.74.0-test-performance** / `ritual-card-effects-test` | `mod-ritual-effects`, `mods/Yugioh-Operacao-MODs/Efeitos-de-Rituais/dev/`; 12 arquivos C/H | `ritual-card-effects.o` versionado; ativo por padrão; API mínima 4; `slus_01411`; `restart:true`; 31 opções, 42 entradas de texto; `after:[melhoria-dos-rituais]` | Há cópia em `versions/v0.74-test-performance`. `mod.c`, manifest e objeto possuem exatamente os mesmos blobs nas duas pastas; não são dois MODs independentes |
| **Ritual Effects 0.78.40** / mesmo ID | Branch downloads, `Efeitos-de-Rituais-v0.78.40-otimizado.zip`, raiz `zzz Efeitos Rituais/`; `mod.c`, `png.c` e headers; 23 C/H incluindo fixtures | Objeto ELF32 i386 de 590.108 bytes; 63 assets; ativo; API mínima 4; `slus_01411`; `restart:true`; 45 opções, 65 entradas de texto | `after` opcional: `melhoria-dos-rituais`, `ai-hard-mode`, `time-wizard-time-magic`. Sem `requires`. Fornece `recipe_v1`. `SDK-PROVENANCE.json` declara SDK **v0.3.0**, commit `663626b1…`; não declara v0.3.1 |
| **Melhoria dos Rituais 1.1.0-test** / `melhoria-dos-rituais` | Tag `ritual-editor-test`; **`user-mods/melhoria-dos-rituais/`**; `rules.c`, `selection.h`, README | `library:rituals`; **sem objeto versionado na tag**; ativo; API mínima 4; `slus_01411`; sem `requires`/opções | Três tributos entre mão/campo, mínimo um no campo; procura funções opcionais do core. **Não contém Surrender**, configuração de origens ampliada nem menus do pacote completo descrito no prompt |

O ZIP 0.78.40 tem SHA-256 `ad8563c8597c038e8b8bcd12c384039e0247009b9a9094be10362649439a8f4d`; objeto `ec868ccd84844e256d56d735504a2e2b9d13e819893f905a2de09f06ca73fea3`. Os **160 nomes indefinidos do objeto existem nos 3.774 exports do SDK Linux retido**: resultado da inspeção estática por `nm` contra `tmp/pc/game32/sdk/exports.txt`. Isso verifica disponibilidade de nomes, não layout de structs, tipos de relocação aceitos, identidade entre fonte e objeto, efeitos em runtime ou compatibilidade Windows. A correspondência reproduzível fonte→objeto dos pacotes não foi revalidada.

Evidência persistente: [ZIP versionado no commit auditado](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/561d3d97818c3d964a39454e97cedcdf6e9163a3/Efeitos-de-Rituais-v0.78.40-otimizado.zip). O manifest, a proveniência e os fontes citados estão dentro desse arquivo. Cópias de fontes e inventários auxiliares foram produzidos fora do checkout para inspeção e não foram instalados como MODs nem incluídos neste PR.

**Exemplos, não MODs da coleção:** `examples/mods/` contém sete manifests, todos 1.0 e desativados: `audio-replace`, `example-cards`, `example-community-rules`, `gameplay-rules`, `example-tables`, `example-starter-deck`, `example-translation`. Só `gameplay-rules` tem C (`rules.c`, API mínima 3); áudio tem gerador Python. Fixtures em `tests/pc/mod_overlaps` não são MODs instalados. Não há evidência de que exemplos de recipes/decks sejam Ritual Duelists ou Fusion Effects do usuário.

## 4. Comparação com o Prompt Mestre V3 e material faltante

| Item do prompt | Disponibilidade real | O que fornecer separadamente para completar a auditoria |
|---|---|---|
| Melhorias de Rituais **+ Surrender** | Parcial: 1.1.0-test recuperável da tag, sem Surrender | **Pacote completo efetivamente usado**, sua versão e fontes; `mod.json`, C/H, assets/textos e biblioteca correspondente; preservar ID `melhoria-dos-rituais` se for o pacote homônimo |
| Ritual Duelists | Não localizado | Versão usada, manifests de duelistas/decks/drops e fontes de IA/seleção, se houver; lista de decks Vanilla e alterações aprovadas |
| Ritual Effects | 0.74 e 0.78.40 encontrados | Não precisa reenviar essas versões; indicar qual é a versão instalada/autorizada. Enviar apenas versão diferente ou dependências ausentes nela |
| Fusion Effects / balanceamento / efeitos auxiliares | Não localizado como pacote customizado | Versão e fontes completas, receitas, proveniência de fusão e configurações; incluir módulos de efeitos comuns/Metalmorph caso façam parte do conjunto usado |
| Time Wizard / Time Magic | Fontes customizados não localizados | Pacote e fontes reais. **0.19.0-test-performance é somente a referência citada em `LEIA-ME.txt:301–302` do ZIP**, não versão verificada/instalada. ID referenciado: `time-wizard-time-magic` |
| Vanilla Field Effects + DLC | MOD de campos 0.19 parcial encontrado; DLC de arenas/campos não localizado | DLC e assets/manifests usados; versão atual caso difira de 0.19. Não é necessário reenviar o C recuperado da branch |
| AI HARD MODE | Fontes 0.1.0 presentes, com variantes | Não precisa fornecer a versão da base/branch já inventariada. Se usa outro binário, fornecer pacote, fontes e hash para resolver qual variante está ativa |
| **Dual Play** | **Não localizado** | **Prioridade alta:** versão exata, `mod.json`, fontes C/H, objeto, opções de controles e dependências. Modo 2P do core não comprova a existência desse MOD |
| TCG Frame Colors | Não localizado | Pacote visual/fonte e versão; lista aprovada dos 48 Fusion Monsters e exceção Blue-Eyes Ultimate Dragon azul. Cores de nomes Yamyi não substituem molduras |
| Interface customizada / boxes HD / ícones / droplist | Yamyi + recursos do core disponíveis; customizações aprovadas e pack HD não localizados | Só os pacotes adicionais realmente utilizados, suas fontes/configuração e versões. Especificar o que já vem da base para não inventariar como MOD separado |

Para cada pacote ausente, basta uma pasta independente com manifest, fontes, dados/assets e o compilado realmente instalado (se existente), acompanhados do commit/release ou hash. Não é necessário enviar imagem do jogo nesta fase. Também falta definir a base executável para uma futura validação integrada: checkout v0.2.1-preview.1, alvo declarado pelo ZIP v0.3.0 ou instalação v0.3.1-preview.1 citada no prompt. Nenhuma migração foi presumida.

## 5. Hooks, estado e autoridade entre módulos

**Contrato do host:** hooks de função encadeiam em **ordem de registro, último por fora** (`src/pc/mods/hooks.c:12–16,96–111`). `original` deve ser lido a cada chamada. Eventos gerenciados executam prioridade maior primeiro; empate conserva registro; `handled` em BEFORE interrompe os demais (`events.c:31–48,124–152`). `after` ordena dependências presentes/ativadas e não as torna obrigatórias (`manager.c:121–130`). `provide/find` também encontra exports de MOD carregado mas desativado (`events.c:94–106`); por isso o provedor deve validar ativação. Não presumir que uma declaração `after` resolve sozinha conflitos semânticos.

| Contato | Implementações encontradas / autoridade | Risco e evidência |
|---|---|---|
| IA, decisões e materialização | AI Hard Mode registra **9 hooks**: `AiScript_Init`, `AiScript_Run`, `Ai_GetHandSize`, `func_8001BAF0` e cinco buscas; escreve registro de seleção de 12 bytes e troca cartas existentes | `mods/ai-hard-mode/ai_hard_mode.c:222–250,258–307`. Ritual 0.78.40 também envolve `AiScript_Run`, `AiScript_FindWeakest`, `func_8001BAF0` e outras buscas. Com `after:ai-hard-mode`, no carregamento inicial a intenção é Ritual → AI → core. Ritual pode rever a decisão **depois** do planner (`serpent_fear.h:443–455`; `mod.c:2320–2324`). Não há prova integrada de ausência de loops |
| Estatísticas, terreno e equip | Campos substitui `terrain_modifier` após setup/aplicar terreno; AI calcula projeções; Ritual envolve stats/batalha/penalidades | Campos `field_effects.c:64–85` usa tabela atual + delta secundário, sem acumular o delta em chamadas repetidas. Ritual `mod.c:3669–3696` chama original e acrescenta efeitos por instância. AI do master não consulta o provedor de tipos secundários; AI da branch de campos consulta |
| Checagem e consumo ritual | Melhoria 1.1 envolve `Duel_CheckRitual` e `DuelCard_DeactivateRecord`; Ritual envolve os mesmos e `DuelEffect_ApplyRitual` | Melhoria `rules.c:111–180,188–227,256–261` monta proxy para mão e redireciona consumo. Ritual `mask_effect.h:256–311` prepara receita e pode rejeitar resultado. A ordem não garante concordância entre seleção de materiais e aprovação final |
| Receita pública | Ritual 0.78.40 fornece `ritual-card-effects-test:recipe_v1`, ABI 1, `get/matches` somente leitura | `ritual_recipe_provider_v1.h:6–24`, `ritual_recipe_export.h:22–53`. Retorna indisponível se desativado. **Nenhum consumidor desse contrato foi encontrado no AI do master nem na Melhoria 1.1 da tag**; esta procura funções `Tables_*` do core (`rules.c:243–248`) |
| Turnos, destruição e instâncias | Ritual envolve switch, draw, placement, hand, battle, remove/deactivate/reset e mantém marcas por registro/data/card ID | `mod.c:6597–6649,6896–6937`. Há reconciliação e limpeza já implementadas; relatos de corrupção de outros MODs não são bugs comprovados deste pacote |
| Input, roleta e Dual Play | Ritual envolve `Input_UpdatePads`, pausa `Main_RunDuel`; câmera lê pad 0 | Ritual `performance_chaos.h:371–388`, `ritual_coin.h:68–83`; Hand camera `hand_camera.c:45–57`. Falta fonte Dual Play para decidir quem roteia confirmação, jogador ativo e Select/Surrender |
| Texto, cores e boxes | Campos e Yamyi envolvem `func_80037DA4`; Campos também `func_80035E20`; Yamyi envolve init/destroy de textbox e Library | Campos `field_effects.c:180–185,297–316`; Yamyi `card_name_color.c:1451–1463`. Ambos encadeiam, mas não há precedência explícita entre eles; layout/HD/cores precisam de regressão visual futura |
| Visuais 3D/2D | 3D Monsters desenha em callback `frame`; Ritual envolve `Duel_DrawFieldCards` e desenha VFX/overlay | Ritual `mod.c:1291–1320` zera objetos temporariamente durante um passe e restaura antes de 3D; 3D `field_models.c:1282–1337`. É uma mitigação existente, não prova visual de compatibilidade com instâncias criadas por outros MODs |
| Drops/Library | Drop missing cards altera dados; Yamyi lê tabelas de drops | Deve refletir pesos compostos e invalidação de cache. Não há um hook de combate concorrente nesses dados; ordenação de futuros pacotes de drops ainda precisa ser inventariada |
| Save/load | Host permite um buffer registrado por MOD; Yamyi registra cores; Ritual 0.78.40 não registra marcas | Host `modapi.h:125–130`; Yamyi `card_name_color.c:1449`; load chama `Mods_Reset` (`guest/state.c:860`). Reset de Ritual = `clear`, ver achado A3 |

## 6. Achados priorizados e propostas para avaliação

**A1 — P1: coleção e identidade de builds incompletas — lacuna confirmada de inventário.**
Sintoma: não é possível determinar a coleção instalada nem comparar AI/Dual Play com todos os efeitos. Causa: fontes ausentes e referências separadas; versão 0.1.0 de AI cobre fontes diferentes. Afetados: coleção completa. Evidências: tabelas 2–4 e prefixos de blobs registrados na seção 3.1. Impacto: integrar uma variante errada ou alegar compatibilidade inexistente. Proposta mínima: obter apenas os pacotes faltantes e registrar versão + commit/hash de fonte/binário antes de escolher base. Verificação futura: identidade do pacote instalado e inventário de hooks/imports de cada pacote; não requer jogo para fechar a proveniência.

**A2 — P1: projeção de terrenos do AI depende da variante — divergência estática confirmada; efeito em jogo não testado.**
No master, `HmRules.terrain` recebe somente `(type,terrain)` (`planner.h:27`); projeções de mudança de terreno/fusão usam essa função (`planner.c:185–190,383,419,465`). O provedor de campos calcula delta por carta (`field_effects.c:74–82`), e só o AI da branch campos o procura (`ai_hard_mode.c:126–145` nessa branch). Impacto potencial: avaliação de jogadas distinta do bônus realmente aplicado a cartas com tipos secundários; não se afirma que todo snapshot de campo esteja errado, pois o adapter lê stats efetivos. Proposta mínima: escolher explicitamente a variante e avaliar a integração já existente, sem alterar dificuldade nem copiar callbacks isolados. Teste futuro: mesma carta/tipo com subtipos distintos, Umi/Wasteland/Yami, mudança de campo, provedor desligado e AI ligado/desligado.

**A3 — P1: classificação Ritual Effects não sobrevive ao load — comportamento confirmado no código; defeito funcional depende do contrato esperado.**
`mod.c:6946` define `reset=clear`; `clear:1609–1680` apaga marcas, filas e memória de aprendizado; `mod.c:6667–6668` declara expressamente exigir ritual novo após load. Não há `register_state` desse MOD; o host chama reset após restaurar estado (`guest/state.c:860`). Impacto potencial: mesma carta e imagem restauradas, mas imunidades, proveniência e efeitos deixam de estar reconhecidos. Proposta mínima para fase aprovada: definir quais dados lógicos devem persistir, separar ponteiros/visuais e só então avaliar buffer versionado e reconstituição. Teste futuro: save/load com ritual ativo, carta transformada/emprestada, efeitos pendentes, início do próximo turno, MOD isolado e conjunto. **Não é corrupção de save comprovada.**

**A4 — P1: autoridade compartilhada em IA/checagem ritual — sobreposição confirmada; incompatibilidade é hipótese.**
Ritual 0.78.40 instala **42 hooks de função e um evento DAMAGE de prioridade -10000** (`mod.c:6890,6896–6937`). Compartilha alvos com AI e Melhoria da tag; o filtro ritual pode rejeitar um resultado da cadeia (`mask_effect.h:302–311`), enquanto Melhoria tem sua própria seleção/proxy (`rules.c:111–180`). `recipe_v1` existe, mas não é consumido pelas variantes recuperadas. Impacto potencial: proposta de IA que a execução recusa, consumo divergente, material repetido ou repetição de decisões; nenhum desses sintomas foi reproduzido. Proposta mínima: documentar contratos de consulta pura, autoridade de seleção e validação; comparar primeiro com a **versão completa realmente usada** de Melhoria/Ritual Duelists. Testes futuros: 3 instâncias físicas distintas, campo+mão, receita on/off/removida, restrições de resultado, AI e efeitos on/off. Não trocar essa versão antiga pela atual por inferência.

**A5 — P2: confirmação/input não tem integração Dual Play verificável — comportamento parcial confirmado, conflito hipotético.**
`performance_chaos.h:380–382` guarda `gInput_wPad1Pressed` e limpa pressed/repeat dos dois pads; `hand_camera.c:47` fixa porta 0. O código ritual distingue CPU em outros caminhos, mas isso não resolve o roteamento da moeda. Impacto potencial: controle do jogador 2 ignorado ou confirmação capturada indevidamente; moeda tem timeout (`ritual_coin.h:81`), portanto não há evidência de deadlock permanente. Proposta: auditar fonte Dual Play e definir dono de input/modal/Surrender antes de editar terceiros. Teste futuro: dois pads, ambos os lados, X/Quadrado, roleta, menus e Select; Surrender somente CPU, se esse for o contrato aprovado.

**A6 — P2: descoberta/precedência de pacotes — limitações estáticas confirmadas.**
`build_game32.py:755` só inclui `mods/*/mod.json`; loader lê filhos imediatos (`mods.c:1399–1432`). Pastas customizadas profundas, `user-mods/` e ZIP não são instaladas pelo build atual. 0.74 dev/versions e 0.78.40 compartilham ID; duplicados no mesmo diretório mantêm primeiro nome ordenado e geram aviso; cópia do usuário sobrepõe a distribuída (`mods.c:1433–1444`). Impacto: MOD inventariado mas ausente na execução, ou versão antiga sombreando a nova. Proposta: na fase autorizada, definir apenas uma pasta instalável por ID e pacote de procedência explícita; não achatar/mover pastas nesta auditoria. Verificação futura: lista do loader e objeto/hash carregado, sem instalar duas versões.

**A7 — P2: UI e layout compartilham hooks, alvo SDK difere — risco de integração, não falha comprovada.**
Campos/Yamyi compartilham comando de texto; Ritual fornece descrições e visuais próprios; o ZIP declara SDK v0.3.0 enquanto a base é v0.2.1-preview.1. Os 160 imports resolvem nominalmente, mas não certificam structs/HD/ordem dos passes. Proposta: fixar alvo e versões antes de verificar compatibilidade estrutural e visual; preservar chaves/IDs, nomes, molduras e valores como sistemas separados. Testes futuros: descrição longa/idiomas/box HD, nomes Yamyi e molduras TCG juntos, 3D e Card art, múltiplas cópias e criação/remoção de instância.

**A8 — P3: oportunidades de medição, sem ganho alegado.**
O planner faz busca limitada a orçamento configurável e pode consultar repetidamente `Duel_CheckFusion/Equip/Ritual` (`README AI:57–70`); a Melhoria 1.1 faz até 10³ combinações de três candidatos (`selection.h:30–61`). Ritual 0.78.40 já tem fast path em stats (`mod.c:3673–3676`), máscaras e cache de overlay (`6787–6799`), evitando declarar otimização inexistente. 3D usa callback por frame com guardas de cena (`field_models.c:1282–1307`); Yamyi aloca/cacheia tabelas de raridade, e essas alocações não foram classificadas como trabalho a cada frame. Proposta: depois de estabilizar contratos, medir contadores/tempo por decisão/frame em cenas reproduzíveis. Nenhum benchmark foi executado, nenhum ganho de FPS/latência foi estimado.

**Relatos históricos do prompt:** Skull Knight/Zombie Warrior, Cherubim, Phoenix, Mirror's Fiend/Spellbinding, Seiyaryu, Fusionist e Time Wizard não foram declarados bugs atuais. Alguns pertencem a MODs ausentes; Fiend's Mirror no pacote ritual possui fila e guardas (`mod.c:6546–6595`). É necessário associar cada relato a pacote/versão/instância antes de propor correção.

## 7. Matriz e testes não executados

**Nenhuma combinação recebeu selo de compatibilidade nesta fase.** Para cada MOD de efeitos, receitas, campos e UI que vier a ser fornecido, a matriz futura mínima é:

| AI Hard Mode | Dual Play | Estado nesta auditoria |
|---|---|---|
| OFF | OFF | Análise parcial dos fontes presentes; sem teste integrado |
| ON | OFF | Sobreposições e variante AI mapeadas; sem teste integrado |
| OFF | ON | Bloqueado por ausência dos fontes/versão Dual Play |
| ON | ON | Bloqueado pelos mesmos materiais; modo simultâneo precisa ser confirmado pelo desenho real |

### Não executados por escopo (sem afirmar impossibilidade técnica)

- Compilação Linux, testes portáteis, sanitizers, scripts de performance do ZIP e checks do editor: não repetidos, conforme solicitação.
- Histórico do onboarding anterior: build Linux com 609 unidades; **79 testes passaram, 2 foram skipped** (`pc_rank_art`, `pc_font_art`), e 9 de 32 bits foram excluídos. Esses resultados são da base; não incluem rituais/campos recuperados agora.
- Análise atual executada: manifests/refs/hashes, diff de fontes, headers e `nm/readelf`. São evidências estáticas, não aprovação de gameplay.

### Dependentes de execução 32 bits — bloqueio observado anteriormente, não retestado agora

`pc_yamyi_mods`, `pc_trap_effects`, `pc_deck_draft`, `pc_rank`, `pc_ai_hard_mode_adapter`, `pc_mips_varargs`, `pc_card_drops`, `pc_mods_lifecycle`, `pc_object_loader`. O onboarding registrou `Exec format error` inclusive para o loader i386 local. Isso não será resolvido nesta fase e não é uma falha de lógica dos MODs demonstrada.

A fixture `tools/pc/test_ritual_hand_mod.py` da tag também compila freestanding i386; possui fallback opcional Unicorn/pyelftools para ENOEXEC (`:8–43`). **Não foi tentada aqui**; a existência desse fallback não comprova execução nem exige instalar dependências agora.

### Dependentes da imagem/arquivos do jogo

Smoke do jogo, `smoke_ai_hard_mode.py`, replays com renderização/IA real, ritual misto com animação, visuais 3D/HD/roleta, batalha e save/load em jogo; `make match`/overlays também exigem entradas originais verificadas e toolchain MIPS. Imagem do jogo não está disponível no checkout. Nenhum arquivo proprietário foi solicitado ou distribuído nesta entrega.

### Dependentes de material externo / plataforma

Compatibilidade Dual Play, Surrender, Fusion Effects, Ritual Duelists, Time Wizard, TCG Frames e DLC depende dos respectivos fontes/versões. Windows e combinações de pacotes não foram executados. A disponibilidade futura de Wine, emulação ou hardware não foi diagnosticada porque está fora desta fase.

## 8. GitHub, Linear, CI e persistência do ambiente

- **Git:** leitura HTTPS autenticada da plataforma funciona; `ls-remote` e fetch das dez branches concluídos. Não implica acesso à API/push.
- **Linear:** não há ferramenta Linear habilitada na sessão; HAL-5 não foi lida remotamente nem atualizada; nenhuma issue foi criada, movida ou concluída. Os títulos/links são os fornecidos no prompt.
- **Actions:** API `https://api.github.com/repos/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/actions/runs?per_page=10` foi bloqueada pelo proxy (`Tunnel connection failed: 403 Forbidden`). A página pública `/actions` respondeu 200 e mostrou 375 runs; primeiro item listado: `Verify v0.19 on preview SDK`, SHA `296c591`, branch `mod-field-effects`. A página da [run 37518883452](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/actions/runs/37518883452) respondeu 200 e confirmou **Failure**, de 06/10/2026. Trata-se da mesma execução histórica, não nova falha. Anotações exibem saídas genéricas; a primeira mensagem de compilador/causa raiz não foi obtida nesta auditoria. O status foi observado na página pública da execução vinculada acima.
- **CI estático:** quatro workflows citados no prompt existem, mais `pc-release.yml`. `pc-build.yml` atual prevê Linux, Windows i686 e sanitizers; isso não demonstra execução dos MODs customizados. Não foi disparado workflow nem alterada permissão. Diagnóstico aprofundado continua separado em HAL-17.
- **Publicação:** o usuário informou publicação; o status da instância atual identifica `source_config_version_id` terminado em `cecfgver_6af4961d72c4819f8c4fc2a2f9748c2d`, observações atuais e revisão aplicada 1. Dependências/helpers e binários retidos estão presentes. Não se conclui que a publicação falhou.
- **Erro exato atual:** chamada somente de leitura **`mcp__codex_apps__cloud_environment_onboarding_read_environment_config_draft({})`** retorna `isError:true`, `error_code:"INVALID_ARGUMENT"`, mensagem **“The current environment draft could not be verified. Retry the read before editing the configuration.”** Assim, não foi possível ler/verificar os campos `install_script` e `start_skill` do novo rascunho. Isso não prova que estejam ausentes na versão publicada.
- **Erro histórico distinto:** na configuração anterior, **`mcp__codex_apps__cloud_environment_onboarding_update_environment_config_draft({config:{install_script:…,start_skill:…}})`** retornou `INVALID_ARGUMENT` com **“Setup could not be completed; saving the draft could not be confirmed.”** Essa operação de gravação **não foi repetida** nesta fase. Nenhuma configuração anterior foi reaplicada e nada foi publicado pelo agente.

## 9. Plano mínimo e encaminhamento

1. **Fechar HAL-5 como inventário de disponibilidade, sem alegar compatibilidade:** entregar este relatório, obter pacotes ausentes e identificar a instalação-alvo/fonte/binário efetivos. Sem modificar IDs, recipes ou mecânicas.
2. **HAL-6/7/10:** completar o mapa de hooks com Melhoria completa, Ritual Duelists, Dual Play e Time Wizard. Priorizar seleção/consumo ritual, ordem AI→efeitos, pureza de consultas e carga/desativação de providers.
3. **HAL-9/18:** definir dono de input/jogador ativo e matriz AI/Dual Play; especificar confirmação e Surrender sem modificar terceiros por antecipação.
4. **HAL-7/11:** decidir o contrato de persistência das marcas Ritual Effects. Propor reprodução pequena de save/load e correção mínima somente após aprovação.
5. **HAL-8:** completar campos/DLC/TCG/HD e reproduções visuais; avaliar reutilização da integração de terreno já existente, preservando dificuldade/receitas/decks.
6. **HAL-17 e otimização:** recuperar primeira falha real do CI e separar correção de ambiente/core de mudanças em MODs; só depois medir hot paths e aprovar otimizações com números.

**Próxima ação recomendada:** fornecer a versão completa de Melhoria dos Rituais + Surrender e o Dual Play efetivamente usados, junto dos demais pacotes ausentes da tabela 4. Com esses materiais, concluir o mapa de autoridade dos hooks antes de escolher a primeira reprodução/correção. Esta entrega não autoriza nem inicia Fase 2.

### Texto pronto para HAL-5

> Inventário estático concluído para 10 branches remotas + tag ritual-editor-test, HEAD 0d782027… = v0.2.1-preview.1/API 10. Encontrados: 5 MODs da base; Field Effects 0.19 (backup 0.1-dev); Ritual Effects 0.74 e ZIP 0.78.40 com fontes/ELF32; Melhoria dos Rituais 1.1.0-test na tag, sem Surrender. Faltam versões completas de Melhoria/Surrender, Ritual Duelists, Fusion Effects, Time Wizard, Dual Play, TCG Frames e DLC/HD usados. AI Hard Mode 0.1.0 tem variantes de fonte por branch; a integração de terreno por carta não está no master. Sem mudanças versionadas, execução de jogo, build ou testes nesta fase. Relatório MODS_AUDITORIA_E_PLANO.md com referências e hashes entregue; compatibilidade integrada pendente. Linear indisponível nesta sessão; status da issue não alterado.

Encaminhamentos prontos: **HAL-6** — cadeia de função/ordem de eventos e IDs duplicados; **HAL-7** — origem/consumo de tributos e perda de classificação no load; **HAL-8** — delta secundário e UI/HD; **HAL-9** — ausência Dual Play/Time Wizard e roteamento de input; **HAL-10** — variante AI e decisões filtradas por efeitos; **HAL-11** — consolidação neste relatório; **HAL-17** — mesma run histórica Failure confirmada por página, causa raiz não obtida. Nenhuma issue deve ser dada como Done com base em testes não executados.

### Acesso rápido às evidências de código

- [AI Hard Mode da base, init e hooks](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/0d78202729967a5bbdbc886aa128e48e82bef7ac/mods/ai-hard-mode/ai_hard_mode.c#L294).
- [AI da branch de campos, provedor opcional](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/296c5919479711b550d00aea8cd042b1c34ba306/mods/ai-hard-mode/ai_hard_mode.c#L126).
- [Campos: fonte 0.19 no commit auditado](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/296c5919479711b550d00aea8cd042b1c34ba306/mods/Yugioh-Operacao-MODs/Field-Effects-e-Tipos-Secundarios/dev/field_effects.c).
- [Ritual Effects 0.78.40: fonte principal](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/561d3d97818c3d964a39454e97cedcdf6e9163a3/Efeitos-de-Rituais-v0.78.40-otimizado.zip), [input/coin](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/561d3d97818c3d964a39454e97cedcdf6e9163a3/Efeitos-de-Rituais-v0.78.40-otimizado.zip), [receita pública](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/561d3d97818c3d964a39454e97cedcdf6e9163a3/Efeitos-de-Rituais-v0.78.40-otimizado.zip).
- [Melhoria dos Rituais 1.1.0-test: fonte da tag](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/4162e8d3c86236934a0179796b8be9f690c9da66/user-mods/melhoria-dos-rituais/rules.c).

Verificação final: HEAD permanece `0d78202729967a5bbdbc886aa128e48e82bef7ac`; `git status --porcelain=v1` vazio. Os cinco manifests retidos no build Linux são byte a byte iguais aos da base atual. Não se verificou identidade fonte→binário por rebuild nesta fase.
