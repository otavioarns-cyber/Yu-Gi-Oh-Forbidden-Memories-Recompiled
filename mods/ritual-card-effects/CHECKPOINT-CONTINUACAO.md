### v0.78.49 — Controle permanente Psycho-Puppet / Fiend Mirror DEF
- Base v0.78.48, sem alteração de receitas ou MODs externos.
- Psycho-Puppet mantém a marionete até sua destruição; a devolução ocorre no
  fim do turno se a marionete ainda estiver viva. Se destruída por Raigeki,
  o vínculo expira e a carta não retorna.
- Identidade de deck preservada no hook `func_80024D34` com reconstrução
  corretiva da carta roubada somente quando necessário, independente de qual
  lado esteja controlando a marionete.
- Reconciliar perda de marionete por magias e armadilhas antes de permitir
  a próxima captura, com proteção contra desativação temporária em batalha.
- Mirror DEF: comparação neutra, proteção contra destruição; aplicação
  separada de ATK inteiro ao controlador e reflexo idêntico ao atacante,
  com apresentações enfileiradas mas dano já contabilizado uma só vez.
- Testes de regressão funcionais em C, compilação Windows por SDK. Sem
  confirmação de execução visual ou gameplay em Windows nesta versão.

# CHECKPOINT ÚNICO — MOD Efeitos de Rituais

> **Arquivo contínuo:** atualizar este documento, `CHECKPOINT-CONTINUACAO.md`, a cada mudança ou versão. Nunca criar um novo `CHECKPOINT-*-vX.md`. Preservar histórico cronológico abaixo e atualizar o estado atual no topo. Este documento substitui os checkpoints separados de otimização v0.75, continuidade v0.78.20 e continuidade v0.78.21.

## Estado atual / v0.78.40 — otimização de callbacks e receitas (2026-10-11)

- **Pedido atual:** eficiência e fluidez, seguindo a Diretriz Permanente enviada em PDF. Base v0.78.39 intacta; mudanças em área isolada, sem alterações rastreadas no fork, na referência oficial ou em outros MODs.
- **Alvo confirmado pelo usuário:** jogo v0.3.0. Biblioteca recompilada com o SDK do pacote oficial Windows i386 publicado; host API 12, `min_api=4`, ID `ritual-card-effects-test`, library e todos os Settings intactos. O pacote e seus exports foram verificados por SHA256 oficial. Novos sistemas de rituais/áudio e migração Windows x64 permanecem separados: não foram necessários para estas otimizações.
- **Sengenjin:** snapshot local e síncrono da consulta pública, reaproveitado nos dois atributos. Preservadas cópias independentes por eixo, ordem native-then-public, clamp, guard e Guardian Star. Sem cache de stats entre frames ou consultas de oracle.
- **BLS e Dokurorider:** validar fontes uma vez por lado a cada reconciliação; manter identidades, participantes em preparação, exclusão do próprio registro e filas de apresentação.
- **Render:** não calcular assinatura de host-overlay para contadores inteiramente nativos; consultar cursor apenas para cartas candidatas a esses contadores; reaproveitar texto/largura do popup Chakra por valor real. Comandos, posição, brilho e duração preservados.
- **Receitas:** índice de descritores imutáveis com fallback validado; `recipe_v1.matches` copia/limita só o papel solicitado. Stats e Settings continuam atuais a cada chamada; exceções Chakra, Skull Monster, clones e limites zero preservados.
- **Validação:** 1.884.072 verificações de receitas, 595.000 de lógica e 231.548 de render; versões comparadas com funções/headers reais. ASan/UBSan passaram. Sanity test das 18 receitas passou. Objeto ELF32 i386 compilado com Clang/llvm-mingw e GCC na base inicial, depois com Clang contra o SDK Windows publicado da v0.3.0; imports verificados pelo script oficial. Comparações de lógica/receitas também repetidas com os headers da v0.3.0. Auditoria independente do diff sem bloqueios.
- **Desempenho:** medições locais dos callbacks com mocks, não FPS do jogo. Auras densas BLS/Doku reduziram tempo aproximadamente 57%/63%; demais medições e metodologia em `RELATORIO-DESEMPENHO.md`.
- **Limites:** BIN indisponível por limite de transferência; sem gameplay Windows, GPU, frametime ou combinações reais de MODs. A redução de consultas pressupõe getters de atributos determinísticos; hooks de terceiros sensíveis à contagem de chamadas exigem teste específico.
- **Entrega:** `Efeitos-de-Rituais-v0.78.40-otimizado.zip`; manter backup do original e substituir somente a pasta deste MOD. Histórico contínuo preservado neste mesmo documento.

## RELEASE pública / v0.78.38 — Chakra Modified Recipe (2026-10-09)

- **Estado da retomada após interrupção:** a pasta auditada `ritual_final_build` estava byte-a-byte igual ao ZIP v0.78.37-test, sem qualquer alteração parcial de receita. A implementação começou numa cópia isolada e preservou a base original.
- **Chakra (709), Resurrection of Chakra (694):** nova receita com `Wall Shadow` (369) + dois tributos de tipo Fiend com `Cards_Level >= 7`. São **três registros físicos distintos**, em qualquer ordem; duas cópias do mesmo Fiend são permitidas se forem cartas físicas distintas. O próprio Chakra e identidades clonadas de Chakra não contam como Fiend elegível.
- **Exceção ATK/DEF apenas do Chakra:** `compat_private_requirements()` pula `compat_cap_tributes_below_result()` somente para ritual 694. Os três materiais do Chakra não têm teto de atributos; a regra antiga dos demais Rituais segue intacta, inclusive Skull Guardian.
- **Validação extra privada:** requisito `fusion_group=254` representa "Fiend que não é Chakra" exclusivamente neste MOD; `255` continua reservado ao Skull Group. Isso não modifica tipos, grupos de fusão nem outro MOD; `compat_requirement_matches()` aplica a exclusão também na chamada de oracle para materiais da Mão/Deck via MelhoriaDosRituais. O MOD não busca nem consome cartas da Mão ou Deck; o parceiro opcional mantém essa responsabilidade.
- **Settings:** `chakra_recipe` independente de `chakra`, ON por padrão, reinício requerido. ON substitui a receita original; OFF restaura a Vanilla. `EFFECTS - Ignore Ritual Summon` continua OFF por padrão, chave/ID existentes do MOD mantidos para não perder configurações de instalações anteriores. Nenhum diagnóstico recolocado.
- **Descrição dinâmica:** `[D3B6]` passa a ter arquivo `text/chakra_recipe_on.txt` e `text/chakra_recipe_off.txt`, ambos com condição do setting. O texto do efeito próprio do Chakra `[D3C5]` não muda.
- **Preservados:** câmera da v0.78.31, carta-base inimiga v0.78.36, modelos 3D, Raigeki, bônus, rituais anteriores e otimizações. Nenhuma alteração em MelhoriaDosRituais, 3D Monsters, decomp ou GitHub.
- **Testes locais:** compilação ELF i386 com SDK v0.2.1-preview.1; C harness compilado com funções originais de requisitos, matching e seleção, incluindo seis permutações, Wall Shadow com DEF 3000, Fiends acima dos stats do resultado, Chakra excluído e regressão de teto Skull Guardian; validação do manifest e das duas descrições. **Ainda não testado no runtime Windows / jogo:** fazer uma invocação real com e sem MelhoriaDosRituais antes de anúncio definitivo de compatibilidade.
- **Distribuição:** ZIP público com nome amigável `Efeitos-de-Rituais-Release.zip`. O ID `ritual-card-effects-test` e `library` não devem ser renomeados nesta versão, pois são identidade de configurações/hook já utilizada por jogadores antigos.

## Revisão v0.78.37-test — limpeza de Settings para distribuição (2026-10-09)

- **Base:** v0.78.36-test, sem alterar código de Chakra, câmera, monstros 3D, animações, efeitos, receitas ou otimizações.
- **Settings removidos:** `test_cpu_prioritize_traps`, `diagnostic_skull_ritual`, `diagnostic_ai_positions` removidos de `mod.json`. Em `refresh_settings()` os três são explicitamente fixados em `0`, sem ler preferências antigas: evita comportamento escondido caso o usuário tenha salvado `ON` em versões de teste. Código diagnóstico permanece no source para possível manutenção futura, mas inativo nesta build pública.
- **Overlays de diagnóstico:** a legenda automática `Ult/Swd AI` também foi desativada (`ultimate_swords_debug=0`) nas duas callbacks de overlay. É apenas texto de teste, sem impacto sobre a IA do Ultimate, Swords ou efeitos; overlays de gameplay (bônus, moedas etc.) continuam.
- **Exceção intencional:** o usuário pediu para MANTER a opção de remover a exigência de invocação Ritual, só com `OFF` por padrão. Exposta agora como `EFFECTS - Ignore Ritual Summon`, chave **nova** `effects_without_ritual_summon`, default `0`, leitura de host com fallback `0`. A chave antiga `test_ignore_ritual_requirement` NÃO é mais lida, então até usuários antigos com `ON` salvo voltam automaticamente para a experiência real (Ritual-only). Alternância ON opcional continua acessível.
- **Compatibilidade:** IDs, library, outras chaves de Settings (receitas/efeitos/Bulk), conteúdo e host hooks preservados. Não mexer no MOD MelhoriaDosRituais, 3D Monsters nem decomp. Avisar usuários antigos que a nova opção começará OFF mesmo que o antigo TEST estivesse ON.
- **Validação:** compilar objeto ELF i386 via SDK, inspecionar chaves únicas no manifest, assertar default OFF e ausência de diagnostics do manifesto e leitura de valores antigos; diff para v0.78.36 limitado a `mod.c`, `mod.json` e documentação/checkpoint; validar ZIP. Runtime Windows ainda requer verificação visual rápida.

## Revisão v0.78.36-test — carta-base Chakra em toda a linha inimiga + Hotpass (2026-10-09)

- **Base real:** ZIP v0.78.35-test. Usuário confirmou que a ocultação das CARTAS GRANDES nas cinco Monster Zones funcionou, mas identificou a mesma anomalia nas Magias/Armadilhas/Rituais da linha traseira. Modelos 3D e câmera 0.78.31 aprovados; preservar.
- **Correção isolada:** durante os mesmos estágios de câmera Chakra (1,2,3,6), o hook `Duel_DrawFieldCards` agora oculta transitoriamente os objetos dos **10 registros de campo inimigo** (cinco monstros e cinco back-row), em vez de cinco. O hook do sprite transitório usa a mesma abrangência. Restaura cada ponteiro após a chamada síncrona ao renderer. Não toca IDs, flags OCCUPIED, stats, objetos do jogador, tabuleiro, figuras do 3D Monsters, ângulo, Raigeki, destruições, retorno ou buff.
- **Auditoria orientada pelo Apêndice C / v0.75–v0.76:** mantidos todos os gates de estado vivo, máscaras, caches e fila visual; nenhuma consulta de Settings nem alocação adicionada em hot paths. **Hotpass seguro:** `bls_reconcile_aura_visuals` e `dokurorider_reconcile_aura_visuals` restringidos às 10 Monster Zones, em vez de percorrer os 30 registros em cada reconciliação. Os demais 20 registros não são elegíveis e antes apenas recebiam zeros; continuam zerados por inicialização/reset de estado. Sem alteração das regras de bônus ou do popup.
- **Política de risco:** os scans de 5/10 campos que dependem de estado atual e os hooks de proteção/imunidade do checkpoint não foram refeitos; não supor ganho global de FPS a partir de redução de iterações sem medir in-game.
- **Validação:** compilar SDK Windows i386, comparar simbolos/relocações e diff funcional com v0.78.35, testar exaustivamente filtros e restauração por quadro, verificar comportamento equivalente das auras, validar ZIP. Compilação e harness não substituem teste visual do usuário.
- **Pendência única:** confirmação em gameplay de que todas as cartas grandes (incluindo back-row) desaparecem durante a sequência, enquanto todas as FIGURAS 3D continuam presentes e o resto se mantém como v0.78.35.

## Revisão v0.78.31-test — enquadramento do Chakra (2026-10-09)

- **Base exata:** ZIP v0.78.30-test. Alteração de código única em `mod.c`: `chakra_start_camera` no início da sequência passa `view.vrz=+104` para Chakra do lado 0 e `-104` para lado 1, em vez de `0`. Os cinco slots adversários recebem enquadramento mais centralizado, não o meio do tabuleiro.
- **Preservado:** `DuelEffect_AllocateRequest(0x10)` (Raigeki Vanilla), tempo dos raios, efeitos `0x0B` de destruição, ordem, imunidades, cálculo, câmera de retorno para valores exatos salvos, buff verde e timeout/watchdog. Nenhuma alteração à seleção de tributos, Skull Guardian, configuração ou Melhoria dos Rituais.
- **Validação:** reconstrução pelo SDK Windows v0.2.1-preview.1, teste estático da câmera nos dois lados, verificação de símbolos/relocations, inspeção do diff, ZIP íntegro. **Sem validação visual em runtime Windows neste ambiente.**
- **Situação do usuário:** todos os rituais, Ultimate, beacons, desempenho e a sequência do Chakra foram aprovados. Única alteração pendente: verificar focalização da câmera durante Raigeki.

## Revisão v0.78.27-test — Chakra agora reutiliza Raigeki Vanilla; fontes de tributo permanecem independentes (2026-10-08)

- **Pedido do usuário:** Chakra deve usar toda a apresentação do Raigeki Vanilla (raio elétrico, cadência/duração, sons e destruição). A versão anterior só usava requests 0x0B e por isso não produzia o raio.
- **Implementado:** aloca o MESMO efeito de raio `DuelEffect_AllocateRequest(0x10)` usado em `DuelEffect_ApplyRaigeki`, com os mesmos parâmetros e som inicial; o hook existente de `DuelEffect_UpdateRequests` observa `field_1D` a cada frame (inclusive quando a animação bloqueia ações de campo) e sincroniza as remoções e requests 0x0B com o progresso real dos 5 raios. Reproduz os buffers de destruição separados (`strike * 0x3000`) e o áudio Vanilla. Não executa a carta Mágica Raigeki, evitando ativar outra regra do jogo.
- **Sequência:** câmera entra em visão geral -> efeitos Raigeki 0x10/0x0B -> aguarda todas as destruições visuais -> câmera retorna exatamente ao estado anterior -> popup verde `+N ATK` do Chakra. Ultimate e outras remoções bloqueadas continuam sem animação falsa e sem entrar na soma. Não cria popup +0 mesmo quando raio percorre campo vazio.
- **Segurança:** ao cancelar Chakra, libera apenas sua própria request; watchdog de request e câmera mantidos para evitar softlock. Estados Vanilla e outros MODs não são sobrescritos.
- **Independência de fontes Ritual:** receitas publicadas usando somente as 5 Monster Zones normais. Validador adicional aceita somente os três registros temporários apresentados como oracle de seleção externa, sem selecionar nem consumir cartas da Mão/Deck. Com Melhoria dos Rituais ausente não permite ritual sem três tributos no Campo. A interface pública (`Duel_CheckRitual`) continua independente do MOD externo.
- **Testes internos:** compilação com SDK real Windows v0.2.1-preview.1; 10 asserts de sincronização Chakra; 10 checks de limite público de fontes; regressões Skull (16), receitas v0.78.26 (24), Ultimate/Crush (14), beacons (14). Nenhum gameplay Windows executado aqui — solicitar confirmação do efeito visual integral e ritual usando somente Deck.
- **Base preservada:** v0.78.26-test, incluindo Wood Remains, limites de atributos de tributos, autocontagem Skull, Crush Card, buffs aprovados e configurações existentes.

## Revisão v0.78.26-test — implementação local, gameplay ainda pendente (2026-10-08)

- **Escopo imutável:** somente `zzz Efeitos Rituais`; não editar `MelhoriaDosRituais`, decomp ou branch original.
- **Skull Group:** Wood Remains #228 foi incluído (21 IDs). Ritual Skull Guardian com Novox confirmado em jogo pelo usuário na versão anterior; bônus do próprio Guardian +1000 incorreto foi corrigido pela comparação física `DuelCardRecord.data` além do índice (campo). Verificar no Windows se a mão lógica após uma invocação não mantém cópia fantasma.
- **Super War-Lion:** o usuário conseguiu utilizar Super War-Lion como tributo de outro War-Lion (INVÁLIDO). `compat_cap_tributes_below_result()` limita ATK e DEF impressos de TODAS as receitas condicionais em comparação ao resultado, em qualquer origem (Campo, Mão, Deck) quando o valor do resultado é >0; quando zero mantém o limite específico, para não inviabilizar Millennium Shield. Manter as seleções nomeadas e as regras de tipos/grupos.
- **Ultimate × Crush Card:** novo relato de loop infinito quando a CPU usa Crush Card contra Ultimate (caso inverso ainda não testado em jogo). Causa demonstrada na decomp `DuelEffect_ApplyMonsterRemoval()`: varredura só avança após alvo desaparecer do slot, porém hook de imunidade recusava sua remoção. `monster_removal_ultimate_skip_mask()` oculta a flag OCCUPIED apenas na duração do scanner Vanilla para as instâncias Ultimate do lado adversário, com snapshot/identidade vigente, restaurando-a imediatamente. Nenhum request de destruição para Ultimate imune; outros alvos continuam válidos. O hook `monster_removal()` permanece no MOD Efeitos de Rituais.
- **Validação técnica:** objeto do SDK Windows preview.1 compilado; harness `test_crush_loop.py` reproduziu o loop do comportamento sem máscara e passou o caso corrigido para Ultimate dos dois lados, alvos não imunes, campo só com Ultimate, identidade trocada e grupos diferentes. Suite de 24+ receitas/beacons do `test_v07826.py` aprovada após corrigir falso positivo do próprio teste (slot Warrior de Sengenjin, não Beast de War-Lion). Não declarar resolução confirmada em runtime até receber vídeo/relato do teste Windows.
- **Outras pendências de gameplay:** animação/câmera Chakra, texto de Skull Guardian, bônus de Fortress Whale, Ultimate × outras Attack Traps e desempenho 3D Monsters ON/OFF.

## Estado atual do desenvolvimento

- **Última versão preparada:** **v0.78.24-test**, para **Yu-Gi-Oh! Forbidden Memories Recompiled — Windows v0.2.1-preview.1**.
- **Alterações LOCAIS posteriores à v0.78.24-test, ainda não entregues/testadas em jogo (2026-10-08):** cinematografia Chakra com aproximação ao campo adversário, destruição Vanilla 0xB, retorno para a câmera original e só então popup +ATK; correção da linha final de `text/skull_guardian_on.txt`; correção da contagem de outros monstros no bônus DEF de Fortress Whale e regra compartilhada para buffs tipo beacon. O objeto de trabalho foi recompilado pelo SDK preview.1; **não declarar a câmera confirmada em runtime**. O vídeo `Falta a visualização do raigeki.mp4` mostra a destruição sem enquadramento adversário.
- **Pasta de trabalho:** `zzz Efeitos Rituais/`, contendo `mod.c`, headers, `assets/`, `text/`, `mod.json` e `ritual-card-effects.o`.
- **Base de trabalho:** evolução contínua da v0.78.20, com Chakra e Zera the Mant implementados na v0.78.21. Não reiniciar o MOD nem reverter efeitos já aprovados.
- **Validação:** SDK compilou v0.78.24-test. **Skull Guardian: funcionamento de efeito confirmado pelo usuário**; linha da descrição ainda requer teste visual pós-ajuste. **Zera: prévia da mão/campo/detalhes estável e aprovada pelo usuário**. **v0.78.23 REPROVADA** em jogo: a câmera do Chakra continuou provocando softlock e a prévia de mão do Zera ainda atrasava; as regressões de Zera foram depois aprovadas na v0.78.24. **v0.78.24 restaura a sequência Chakra da v0.78.21**, mantendo cálculo/solicitações de destruição/popup previamente aprovados; cinematografia especial segue PENDENTE, não apresentar como corrigida. **Ultimate × Widespread Ruin aprovado.**

## Ajustes locais em desenvolvimento — ainda sem versão entregue (2026-10-08)

- **Pedido aprovado:** durante o Ritual Effect do Chakra, mostrar os monstros adversários sendo destruídos com o VFX Vanilla Raigeki, voltar a câmera ao nosso lado e **somente depois** exibir o popup verde de buff ATK. Fórmula, arredondamento, imunidade Ultimate e execução de cada destruição ficam iguais aos da v0.78.24-test.
- **Implementação de câmera na cópia de trabalho:** `chakra_tick()` controla fases separadas: aguarda cursor/câmera nativos ficarem ociosos; faz tween para visão geral nativa `(0x258, 0x100, D_8009AF20[side], 0)`; executa as destruições em ordem com `DuelEffect_CreateRequest(0xB)`; espera terminar o último request; volta à posição exata salva antes do efeito; somente depois confirma o bônus e agenda o popup.
- **Causa técnica examinada na decomp:** `func_80022D94` **só agenda** o tween (`D_8009B204`), e `DuelScene_Update` só chama `func_800235C0` quando `D_8009B162 != 0`; com o flag zerado, aguardar `D_8009B204` pode travar. A nova tentativa usa `D_8009B162=0x8000` (tween sem operações de painel/cursor), mantendo o passo da câmera dentro do fluxo nativo. Há limites de espera e restauração de emergência se o próprio tween não avançar. Não afirmar resolução de softlock sem gameplay real.
- **Skull Guardian:** reformatada a descrição da carta (`text/skull_guardian_on.txt`) de `each Skull Monster in / your hand.` para `each Skull Monster / in your hand.`, sem alterar as regras, evitando ultrapassar a borda direita.
- **Fortress Whale / beacons (2026-10-08):** confirmado em gameplay que Fish consumida como tributo da mão NÃO entra na contagem, mas identificado +500 DEF incorreto por contar o próprio Fortress Whale. A função genérica de contagem de cartas da mão lógica e do campo aliado já permitia `excluded` (o War-Lion passava seu próprio índice corretamente); o helper `fish_in_hand_and_field` do Whale passava `-1`, contando todas inclusive a fonte. Agora as duas fontes utilizam `beacon_other_allied_type_hand_field(side,type,source)`, que EXIGE fonte válida na Monster Zone do lado correspondente, nunca conta o próprio registro, conta cópias distintas e mantém a regra dos cinco slots lógicos da mão. `beacon_is_other_record` é também usado pelas contagens de outros Skulls e Females. Demais beacons (BLS, Dokurorider, Tri-Horned, Javelin, Crab) já excluem a fonte como destinatária quando a regra diz “other”; foram auditados e preservados. O multiplicador de Javelin ("per allied Insect") inclui o próprio Javelin como Insect aliado para calcular o bônus DOS OUTROS insetos; isto NÃO é autoconceder bônus e é regra distinta. Compilação SDK e harness isolado com 14 verificações de contagem: PASSARAM; resultado dentro do jogo ainda PENDENTE.
- **Aprovados em gameplay em 2026-10-08:** Ultimate do jogador ataca sob Swords; War-Lion não conta Beast tributada; Whale não conta Fish tributada na mão (correção da autoinclusão de Whale ainda precisa gameplay); Zera −1500 ATK/DEF estável alternando telas, Fiends/Ultimate imunes e retorno correto após saída; BLS mostra buffs de Warriors na mão; Ultimate × Fake Trap e × Widespread mostram etapa sem dano e combate subsequente; IA do Ultimate aprende imunidade após Magic falhar.
- **Status:** fonte ajustada e objeto i386 recompilado com SDK `v0.2.1-preview.1`; comportamento novo de Whale/beacons verificado em harness, validação **runtime PENDENTE**. Usuário pediu **não entregar outra versão ainda**; esperar próximo conjunto de feedback antes de empacotar versão de teste.

## v0.78.24 — Skull Guardian e estabilização conservadora (2026-10-08)

- **Categoria interna Skull Monster** em `skull_monster.h`: 20 IDs exatamente como fornecidos: 9, 22, 24, 30, 59, 132, 146, 153, 154, 215, 217, 241, 359, 540, 545, 548, 556, 564, 704, 707. Predicado `RitualCategory_IsSkullMonster(id)` independente do tipo primário, UI, Fusion, Field e Female, reutilizável futuramente.
- **Receita Novox's Prayer #679:** Kamakiriman #535 + 1 Skull Monster + 1 Warrior, com três registros distintos e seleção existente de tributos (campo + mão). `fusion_group=255` é **somente sentinela privada** do comparador de receita Ritual; NÃO registrar grupo de Fusion nem mudar `Duel_CheckFusion`. `ritual_recipe_data.h`, `ritual_recipe_compat.h`, backup de receitas ampliado a 24. Ajuste só ligado a `skull_guardian_recipe`.
- **Skull Guardian #704:** bônus por instância invocada por Ritual = `1000 * outros Skulls no campo dos dois lados + 500 * Skulls na própria mão de cinco slots lógicos`, ATK e DEF. A própria instância NÃO conta; outras cópias sempre contam, independentemente de serem Ritual. Bônus vivo via hook `Duel_CalcCardStats`, sem alterar `stat_modifier`. Marcas vinculadas a `data` e slot. Reutiliza fila de popup existente para aumentos (deltas positivos); redução silenciosa e snapshot atualizado para não repetir popups. `skull_guardian` ON por padrão, desligável independentemente da receita.
- **Textos:** `text/skull_guardian_on.txt` (D3C0), cabeçalho Ritual Effect, valores positivos em verde; `text/novox_recipe.txt` (D3A7), receita explícita em quatro linhas. Nenhum texto de outro monstro foi alterado.
- **Chakra:** após teste real da v0.78.23 registrar reprovado: mesmo softlock. Rollback dirigido apenas ao controlador cinematográfico da v0.78.21 (destruição anteriormente funcional), mantendo o popup para bonus > 0. **Não há nesta versão nova sequência de câmera aprovada; criar integração não bloqueante futura.**
- **Zera:** prévia na mão novamente reprovada pelo usuário na v0.78.23 (atraso transitório). Novo ajuste deixa de exigir `DUEL_CARD_FLAG_OCCUPIED` se a carta ainda corresponde à identidade de um dos cinco slots lógicos reais na apresentação. Reteste obrigatório; não declarar resolvido.
- **Aprovado novo:** Ultimate × Widespread Ruin: Ultimate não destruído, segue atacando. Preservar.
- **Escopo:** nenhum efeito novo fora do Skull; demais efeitos preservados; teste no Windows ainda necessário.

## Correção em trabalho — Chakra após teste da v0.78.22 (2026-10-08)

- **Bug demonstrado pelo usuário em `bugado.mp4`:** a sequência acrescentada em v0.78.22 entrou na câmera errada, mudou a percepção do efeito visual Raigeki e deixou o duelo sem comandos (softlock). Não é mudança de regras do Chakra.
- **Causa de bloqueio identificada na revisão do código:** `field_actions()` fazia `if(chakra_tick())return;` durante a etapa em que `chakra_tick()` aguardava `visual_popup_frames` zerar. Mas quem reduz `visual_popup_frames` é `visual_update_stats()`, chamado só DEPOIS daquele `return`; portanto a própria sequência impedia seu popup de avançar. 
- **Correção implementada e compilada, gameplay pendente:** preservadas as solicitações de destruição `DuelEffect_CreateRequest(0xB)` e o cálculo aprovado da v0.78.21, mudado enquadramento inicial para a câmera de visão geral nativa `(0x258, 0x100, D_8009AF20[side], 0)` na perspectiva do invocador, sem inverter para o lado adversário, com retorno ao Chakra apenas depois de terminar a destruição; garantida atualização de `visual_update_activation()` e `visual_update_stats()` durante espera final do popup; limites de espera para câmera/sequência e retorno seguro ao turno.
- **Correção incluída na v0.78.23-test após pedido explícito do usuário.** Não declarar correção de runtime confirmada sem teste no jogo. **Não prometer resolução sem teste in-game.**

## Correção em trabalho — prévia dos atributos na mão / transições (2026-10-08)

- **Teste parcial do usuário:** o debuff do Zera é mostrado corretamente nas cartas da mão (−1500 ATK/DEF, Fiends e Ultimate imunes), porém ocasionalmente os números retornavam aos valores sem penalidade em mudanças rápidas de tela. **Ainda não aprovado como estável.**
- **Auditoria do caminho nativo de Field:** `Duel_SetupCardRecord` grava `terrain_modifier` no registro; `Duel_CalcCardStats` combina o modificador aos atributos, e o jogo lê essa função **tanto em `func_80016784` (desenho da carta)** como em **`func_80023144` (painel de informações)**. Nosso MOD estava injetando a previsão de aura apenas no desenho da carta, e ainda exigia `phase==4`, deixando transições e o painel sem a previsão.
- **Correção compilada, gameplay pendente:** compartilhado o mesmo marcador temporário `hand_aura_preview_record` nas duas rotinas nativas de apresentação; removida a dependência da fase no desenho, mantendo a identificação rigorosa de carta que **continua realmente na mão lógica**. Cada hook restaura o marcador imediatamente após chamar a rotina original. `Duel_CalcCardStats` continua devolvendo os atributos reais para cálculos do duelo, da IA, de fusão ou receita. **Não grava −1500 em stat_modifier/terrain_modifier**, porque Zera é dinâmico por instância.
- **Teste prioritário:** Zera ativo, navegar rapidamente mão ↔ visão de campo ↔ detalhes da carta, confirmando que número da carta e painel permanecem coerentes; depois sair Zera e conferir retorno imediato; repetir com duas fontes e com Fiend/Ultimate. A mesma correção cobre as auras BLS/Javelin/Crab/Tri/Dokurorider.
- **Correção incluída na v0.78.23-test após pedido explícito.**

## Regras permanentes de desenvolvimento

1. Alterar **somente** o que o usuário solicitar; preservar as regras e correções aprovadas.
2. **Não criar ZIP** a cada correção; publicar nova versão de teste apenas quando o usuário pedir expressamente.
3. **Um checkpoint só**: `CHECKPOINT-CONTINUACAO.md`. Editar o mesmo arquivo em todas as próximas versões; incluir registro cronológico de cada versão, sem criar checkpoints novos.
4. Um efeito só pertence à **instância realmente invocada por Ritual** quando assim especificado; preservar a identidade real da carta e a distinção entre desativação temporária e saída definitiva de campo. Opções TEST explicitamente habilitadas seguem suas regras próprias.
5. Evitar trabalho por frame desnecessário: usar máscaras de instâncias vivas, fast-paths, sprites cacheados e hooks somente quando necessários. Regras e invariantes técnicos detalhados no arquivo histórico de otimização incorporado ao final deste documento.

## Correções confirmadas pelo usuário — não reverter

- Textos das **21 descrições**: `<Ritual Effect>` e texto na linha imediatamente seguinte, **sem recuo** ou linha vazia.
- **Garma Sword:** contadores com o mesmo padrão de posicionamento/comportamento de Javelin e Tri-Horned.
- **Garma Sword (confirmado em gameplay em 2026-10-08):** animação de espada preservada ao bloquear ataque repetido, **sem flash/brilho**; não reintroduzir o efeito de brilho.
- **Magician of Black Chaos:** não entra em Defesa com Ritual Effect ativo; funcionamento normal quando desativado.
- **Javelin Beetle:** consome proteção, sobrevive e não exibe fogo de destruição.
- **Blue-Eyes Ultimate CPU + Swords:** ataca normalmente apesar do bloqueio das espadas.
- **Fiend's Mirror:** reflete ao inimigo o dano sofrido quando morre em batalha, desde que controlador sobreviva.
- **Chaos + Ultimate + Dark Hole:** destrói somente os demais monstros e não trava.
- **Hungry Burger:** recebe bônus por destruir monstro em batalha, não por ataque direto.
- **Rodapé ATK/DEF (confirmado em gameplay em 2026-10-08):** os atributos correspondem aos da carta no campo após mudanças de Field, incluindo bônus e penalidades.
- **Ultimate × Widespread Ruin (aprovado em gameplay em 2026-10-08):** ataque continua, Ultimate não morre; não reverter.
- **Tri-Horned Dragon (confirmado em gameplay em 2026-10-08):** consome um chifre, sobrevive e não exibe animação de fogo/destruição.
- **Zera the Mant (confirmado em gameplay em 2026-10-08):** efeito aprovado pelo usuário. Preservar Yami, debuff dinâmico, imunidades e receitas.
- **Zera (confirmado em gameplay em 2026-10-08):** efeito de −1500 estável inclusive alternando rapidamente mão/campo/detalhes; Fiends/Ultimate imunes nas prévias; retirada de Zera restaura atributos sem atraso.
- **BLS (confirmado em gameplay em 2026-10-08):** bônus dos Warriors aliados exibidos na mão.
- **Ultimate (confirmado em gameplay em 2026-10-08):** lado do jogador ataca sob Swords; Fake Trap e Widespread são reveladas e consumidas sem dano inicial, com confronto verdadeiro depois; IA aprende imunidade ao falhar Magic.
- **War-Lion (confirmado em gameplay em 2026-10-08):** Beast usada como tributo não entra na contagem de bônus.
- **Whale (parcialmente confirmado em gameplay em 2026-10-08):** Fish usada como tributo não entra na contagem da mão lógica; bug de fonte contar a si mesma em DEF está corrigido somente na cópia local, aguardando gameplay.
- **Chakra (confirmado parcialmente em gameplay em 2026-10-08):** cálculo de ATK aprovado; Blue-Eyes Ultimate Dragon não é destruído nem entra na soma; popup mostrando o bônus positivo após a destruição está correto. A encenação da câmera ainda não foi aprovada.
- **Chakra — flavor text (aprovado em 2026-10-08):** descrição curta `He destroys his foes, / devouring half of / their power.` aprovada. Não solicitar novo teste desse texto.
- **Zera — cores da descrição (aprovado em 2026-10-08):** número `1500` vermelho, texto restante branco; aprovado. Não solicitar novo teste dessa cor.
- Outras correções já aprovadas: Serpent aprende seu efeito após a primeira interação; Gate Guardian + Swords não congela; War-Lion não conta Beasts descartadas na mão lógica; contadores de Tri/Javelin na seleção inicial.

## Testes e riscos pendentes (v0.78.24)

### Zera the Mant — efeito aprovado; regressões opcionais

- Monstro **#360**; Ritual Magic **Zera Ritual #671**; Field **Yami #335** (terrain 6).
- Ao ser invocado por Ritual, ativa Yami com a apresentação nativa de Field usada pelo MOD.
- Para **cada instância Ritual viva no campo**, todos os monstros **não-Fiend** de ambos os jogadores recebem **−1500 ATK / −1500 DEF** continuamente, incluindo os que entrarem depois. Debuffs de múltiplos Zera se acumulam.
- Fiends (incluindo Zera) e **Blue-Eyes Ultimate Dragon #380** são imunes.
- O debuff desaparece proporcionalmente quando uma instância de Zera deixa o campo; não modificar permanentemente `stat_modifier`/`terrain_modifier`. Yami ativado não deve desaparecer só por Zera sair.
- Receita **original e inalterada**: **King of Yamimakai #85**, **Wicked Dragon with the Ersatz Head #298**, **Ryu-Kishin Powered #377**.
- Descrições: `text/zera_on.txt` e `text/zera_recipe.txt`; toggle `zera` ON por padrão. `1500` deve aparecer vermelho no texto do Zera (`{f8 0A 06}`), restaurando branco em seguida.
- **Aprovado pelo usuário em jogo.** Em caso de regressão, retestar Yami, ATK/DEF, novas entradas, imunidades, saída de Zera, várias instâncias, restrição Ritual e descrições.

### Chakra — cálculo e popup positivo aprovados; câmera pendente

- Monstro **#709**, efeito ativado apenas por invocação Ritual.
- Destrói monstros oponentes da **esquerda para a direita**, com apresentação inspirada no Raigeki.
- Soma **ATK efetivo no instante da destruição** apenas dos monstros **realmente destruídos**; divide por 2, arredonda para múltiplo de 10 (final **0–5 para baixo; 6–9 para cima**). Ex.: 3715 / 2 = 1857,5 → **+1860 ATK**.
- Bônus permanente **somente em ATK** daquela instância de Chakra; DEF inalterada.
- Popup **único**, verde e dinâmico `+N ATK`, atlas `assets/chakra_green_atlas.png`, apenas para bônus **maior que zero**. **Se Chakra não destruir nenhum monstro, não mostrar `+0 ATK`** (alterado na cópia de trabalho, ainda sem teste específico).
- **Ultimate** é imune e não participa do cálculo; **Tri-Horned** é destruído e conta; imunidade a magias não impede o efeito de monstro; **Sengenjin** pode ignorar bônus temporariamente só naquela batalha.
- Descrições: `text/chakra_on.txt` e `text/chakra_recipe.txt`. O texto do monstro foi encurtado a um flavor text de 3 linhas, para não ultrapassar a caixa azul: "He destroys his foes, / devouring half of / their power." As regras completas continuam no efeito e Settings.
- Já confirmado pelo usuário: fórmula de soma/arredondamento, imunidade de Ultimate e exclusão de seu ATK, popup para bônus positivo. Ainda testar: câmera/sequência Raigeki, caso sem destruições (nenhum popup), Tri-Horned, Sengenjin e persistência nas interações específicas.

### Regressões e correções recentes — ainda sem confirmação conclusiva

- **Ultimate × Fake Trap / Attack Traps:** primeiro confronto visual com a Trap sem dano; segundo confronto real contra o monstro originalmente atacado, apenas um cálculo de dano.
- **Ultimate × Widespread Ruin APROVADO no jogo:** Trap revela/consome, Ultimate imune sobrevive e continua ataque. Não retestar sem regressão.
- **Desempenho:** comparar campo lotado de monstros com e sem 3D Monsters; a otimização do renderer evita trabalho extra quando não há efeitos próprios.
- **Ultimate + Swords no lado do jogador**: APROVADO, pode atacar.
- **Aprendizado da IA:** APROVADO o reconhecimento da imunidade após a primeira magia inútil; preservar decisão.
- **Prévia ATK/DEF da mão (alteração pós-v0.78.21, código compilado, gameplay pendente):** quando há uma aura contínua ativa, as cartas de monstro na mão lógica devem apresentar os valores que teriam ao entrar em campo, mantendo o bônus de Field nativo. Cobertura: Zera (−1500 ATK/DEF não-Fiend dos dois lados; Ultimate imune), BLS (+1000 ATK/DEF Warriors aliados), Dokurorider (+1200 ATK Zombies aliados), Javelin Beetle (Insects aliados, inclusive contagem hipotética do Insect entrante), Tri-Horned (Dinos aliados exceto Tri-Horned) e Crab Turtle (família marítima aliada). Exclusivamente desenho, sem mutação dos registros da mão ou alteração das decisões de IA/ritual/fusão. Testar entrada/saída de fontes, vários beacons, lados e imunidades.
- **Chakra v0.78.22 e v0.78.23 REPROVADOS:** softlock após mudança de câmera. v0.78.24 reverte o takeover para sequência v0.78.21; confirmar ausência de bloqueio. Cinematografia como Raigeki (câmera → efeitos → Chakra → popup) continua TRABALHO FUTURO.
- **Super War-Lion:** APROVADO, não conta Beast consumida em material Ritual.
- **Fortress Whale:** APROVADO para Fish removida da mão lógica; **ainda testar** o novo conserto de não contabilizar o próprio Whale como Fish para seu DEF (+0 se sozinho; +500 por cada outro Fish aliado na mão/campo).

### Diagnóstico de posicionamento da IA — opcional (v0.78.22)

- Settings do **MOD Efeitos de Rituais**: ativar `TEST - AI Position Photo Diagnostic`, aplicar, reiniciar se solicitado. OFF por padrão.
- O monitor é **passivo**: registra o primeiro/último monstro novo colocado **face-up em Ataque** por CPU enquanto houver um monstro adversário no campo com ATK efetivo pelo menos 101 pontos maior.
- O overlay fica visível até a próxima ocorrência ou o fim do duelo: `AI POS #N PHx CPUy`, `PLAYED id ATK val DEF val`, `VS id ATK val DEF val DIFF val`.
- **Uma foto após a jogada** já serve para registrar as estatísticas e a posição; não requer vídeo. Diagnóstico registra o resultado no campo, não prova sozinho por qual motivo o AI Hard Mode tomou a decisão.
- Ao terminar, desativar a opção e aplicar; nenhuma regra/decisão da IA é modificada pelo diagnóstico.

## Linha do tempo das versões

| Versão | Alterações e decisões relevantes | Situação |
|---|---|---|
| v0.75.x | Auditoria de otimização: fast-paths, live masks/hints, caches, proteção dos invariantes de cada Ritual; diagnóstico provisório do Black Chaos na v0.75.3 | Arquivo técnico completo preservado abaixo |
| v0.76.0-optimized | Remoção dos hooks e ferramentas de diagnóstico temporários do Chaos; manutenção das otimizações e barreira de Draw Phase | Histórico documentado abaixo |
| v0.78.17-test | Tentativas de correção dos visuais de Javelin/Tri, Mirror, CPU Ultimate/Swords e Dark Hole | Correções depois confirmadas em builds posteriores, conforme lista de aprovados |
| v0.78.18-test | Nova abordagem Ultimate CPU/Swords; restrição DEF do Chaos; descrições alinhadas; rodapé ATK/DEF; proteções visuais | Ultimate CPU, restrição DEF Chaos e Javelin aprovados depois |
| v0.78.19-test | Traps contra Ultimate, Widespread Ruin, contadores Garma, descrições alinhadas à esquerda | Ajustes de Traps prosseguiram na v0.78.20 |
| v0.78.20-test | Separação das duas telas Fake Trap × Ultimate; proteção visual Widespread Ruin; Garma sem flash; otimização leve de renderer | Testes de gameplay das alterações pendentes |
| **v0.78.27-test** | **Chakra** passa a utilizar a request elétrica Vanilla Raigeki (0x10), sincronizando efeitos de destruição (0x0B), áudio e retorno ao popup. Interface opcional de leitura de tributos no Deck foi validada por oracle público; sem Melhoria dos Rituais, verificação continua exclusiva do Campo. Preservados v0.78.26 e regras aprovadas. | **Compilado com SDK Windows; testes internos aprovados; animação integral e Deck aguardam confirmação no jogo.** |
| **v0.78.21-test** | **Chakra** com destruição sequencial, ganho ATK e popup dinâmico; **Zera the Mant** ativa Yami e debuff contínuo por instância; receitas e descrições | **Compilado; gameplay de Zera e Chakra ainda pendente** |
| **v0.78.24-test** | Skull Guardian: categoria Skull Monster (20), Novox + Skull + Warrior, bônus contínuo +1000 por Skull de campo e +500 por Skull da própria mão, popup apenas positivo. Chakra regressou ao controlador estável v0.78.21 para retirar câmera que softlockou nas versões 22/23. Zera: relaxa bit visual de carta ocupada no desenho para reduzir atraso. Ultimate × Widespread aprovado. | **SDK compilado; Skull Guardian, ausência de softlock, atraso Zera e uso simultâneo de mods aguardam gameplay. Câmera nova NÃO implementada.** |
| **v0.78.23-test** | **Correção do softlock e câmera de Chakra:** preservada a destruição v0.78.21, overview nativo visto do lado do invocador, retorno para o buff, atualização do popup durante a espera e timeouts. **Prévia da mão:** auras passam pelos dois caminhos nativos (`func_80016784` e `func_80023144`) sem mudar os atributos reais. Flavor text do Chakra e `1500` vermelho do Zera aprovados; diagnóstico de IA passivo inalterado. | **SDK compilado; câmera, ausência de softlock e transições rápidas da mão requerem gameplay.** |
| **v0.78.22-test** | Chakra: flavor text curto, buff só após câmera retornar do Raigeki; popup `+0 ATK` suprimido; Zera: `1500` vermelho e efeito aprovado; prévia na mão das 6 auras contínuas; diagnóstico de IA por foto opcional (OFF) sem modificar decisões. Garma, rodapé, Tri-Horned, cálculo/imunidade/popup do Chakra preservados. | **SDK compilado; gameplay da câmera, prévia e diagnóstico pendente** |

**Alterações consolidadas na v0.78.22:** Zera: número `1500` vermelho com cor nativa e restante da descrição branco; Chakra: substituir descrição longa por flavor text de três linhas; não enfileirar nem exibir popup de `+0 ATK` quando nenhum bônus é concedido. Registros de testes de gameplay aprovados acima.

> **Para a próxima versão:** acrescentar uma nova linha à tabela, atualizar versão, pendências e aprovações acima; **nunca criar outro arquivo checkpoint**. Este arquivo deve ir dentro do ZIP do MOD, substituindo os antigos.

---

## Apêndice A — Histórico de continuidade da v0.78.20 (preservado)

Este registro é **histórico**; em caso de conflito, prevalece o estado atual no início do checkpoint. O nome do checkpoint antigo é mantido somente como referência cronológica, não como arquivo separado.

# CHECKPOINT - MOD Efeitos Rituais v0.78.20-test

## Base e compatibilidade
- Yu-Gi-Oh! Forbidden Memories Recompiled, Windows v0.2.1-preview.1.
- Codigo do MOD, arquivos de recursos e objeto compilado no mesmo ZIP, pasta `zzz Efeitos Rituais/`.
- Esta e uma continuacao da v0.78.19-test, nao um mod reiniciado.
- Na proxima conversa, usar este ZIP como base e preservar todos os efeitos/correcoes anteriores.

## Alteracoes da v0.78.20-test (precisam de teste dentro do jogo)
1. Ultimate com Ritual Effect ativo vs. Fake Trap/Attack Traps: primeira tela de confronto apenas para mostrar e consumir a Trap, sem dano de batalha; segunda tela de confronto normal contra o monstro originalmente atacado. A batalha com o defensor e calculada uma vez.
2. Widespread Ruin: nenhum fogo/destruicao deve ser animado sobre o Ultimate imune; a Trap ainda deve aparecer.
3. Garma Sword: mantida a animacao da espada quando concede o ataque extra, retirado so o flash/brilho que vinha junto.
4. Desempenho com varios monstros com efeitos: evitam-se rotinas extra de desenho quando nenhum contador/sprite especifico estiver presente. Ainda nao e possivel atribuir toda a lentidao ao MOD (pode envolver 3D Monsters).

## Resultados JA aprovados pelo usuario (nao reverter)
- Descricoes: 21 Ritual Effects com texto alinhado a esquerda, sem recuo nem linha vazia apos `<Ritual Effect>`.
- Contadores de Garma Sword padronizados com os de Javelin/Tri-Horned.
- Magician of Black Chaos com Ritual Effect nao pode entrar em Defesa; sem o efeito, regra normal.
- Javelin Beetle consome a protecao, sobrevive e nao apresenta fogo/destruicao.
- Blue-Eyes Ultimate da CPU ataca apesar de Swords of Revealing Light.
- Fiend's Mirror reflete dano ao inimigo mesmo quando morre em batalha, se o controlador sobreviver.
- Chaos + Ultimate: Dark Hole preserva imunes, destrói os demais e nao trava.
- Outros historicos no LEIA-ME.txt; preservar logica validada de Hungry Burger, Serpent, Gate Guardian + Swords, etc.

## Outros testes sem confirmacao final
- Tri-Horned Dragon: sem animacao de morte/fogo ao consumir um chifre.
- Ultimate Ritual + Swords quando controlado pelo jogador.
- IA aprende imunidade Ultimate apos primeira magia falhar e nao desperdica magias impossiveis.
- Rodape ATK/DEF da carta deve atualizar mudancas no bonus do Field.
- Super War-Lion: Beast usada como ingrediente de Ritual nao deve contar bonus.
- Fortress Whale: Fish removido da mao nao deve contar bonus.
- Fake Trap, Widespread Ruin, demais Attack Traps: regressao completa com as duas etapas visuais.
- Garma Sword sem flash e com espada visivel; desempenho com 3D Monsters ON/OFF.

## Instrucoes operacionais do usuario
- Somente alterar os pontos pedidos. Nao refazer o MOD nem apagar efeitos anteriores.
- Nao gerar nova versao/ZIP a cada erro relatado: aguardar o usuario pedir expressamente.
- Distinguir COMPILACAO e testes estaticos de testes reais dentro do jogo.
- Proxima etapa planejada pelo usuario: receber especificacoes dos ULTIMOS 3 monstros rituais.

## Arquivos tecnicos
- `mod.c`: callbacks, traps, Ritual Effects, UI e hooks.
- `garma_visual.h`: contadores, animacao da espada.
- `performance_chaos.h`, `serpent_fear.h` e demais headers: logica de outros efeitos.
- `mod.json`: versao e Settings.
- `ritual-card-effects.o`: objeto final construido pelo SDK Windows v0.2.1-preview.1.


---

## Apêndice B — Histórico de continuidade da v0.78.21 (preservado)

Este registro é histórico. O estado mais atualizado está no começo do arquivo.

# CHECKPOINT - MOD Efeitos Rituais v0.78.21-test

## Base
- Plataforma: Yu-Gi-Oh! Forbidden Memories Recompiled, Windows v0.2.1-preview.1.
- Base de desenvolvimento: v0.78.20-test + Chakra (implementação posterior) + Zera.
- Continuar a partir deste ZIP em outro chat sem refazer ou reverter mudanças.
- `mod.c`, headers, assets, `mod.json`, `text/` e `ritual-card-effects.o` estão no pacote.
- Compilação SDK não substitui teste real dentro do jogo.

## Chakra (implementado, teste de jogo pendente)
- Carta #709; ativação na invocação por Ritual, por instância.
- Destruição de monstros adversários da esquerda para a direita usando apresentação próxima ao Raigeki.
- Somar ATK efetivo dos monstros realmente destruídos, /2, arredondamento solicitado (unidades 0..5 para baixo; 6..9 para cima), +ATK permanente somente naquela instância (DEF não muda).
- Um popup final verde `+N ATK` construído com sprites do atlas `assets/chakra_green_atlas.png`.
- Ultimate imune, Tri-Horned destruído normalmente, Sengenjin ignora bônus positivo somente na batalha.
- Arquivos de descrição: `text/chakra_on.txt`, `text/chakra_recipe.txt`.

## Zera the Mant (NOVO, teste de jogo pendente)
- Monstro #360, Ritual Magic #671, Field Yami #335 (terrain 6).
- Só a instância realmente Ritual Summoned (ressalva: o Settings explícito TEST "ignore Ritual requirement" também pode armá-lo como faz para outros monstros).
- Após ritual, ativa Yami através da mesma sequência nativa de Field de Crab Turtle/BLS/Tri-Horned (pre, visual, post).
- Cada Zera com efeito ativo, vivo e no campo impõe -1500 ATK e -1500 DEF a todos monstros não-Fiend dos dois jogadores; cálculo vivo por stat hook. Múltiplos Zera somam suas penalidades.
- Fiends e qualquer Blue-Eyes Ultimate Dragon #380 são imunes. Zera é Fiend e não é afetado.
- Saída do campo limpa máscara da instância; batalha conserva fontes temporariamente desativadas pelo motor nativo.
- Sem gravar debuff permanentemente em stat_modifier/terrain_modifier.
- `text/zera_on.txt` possui descrição solicitada (chave [D268]); `text/zera_recipe.txt` com materiais originais (chave [D39F]); efeito com toggle `zera` (ON por padrão).
- Receita original NÃO alterada: #85 King of Yamimakai, #298 Wicked Dragon with the Ersatz Head, #377 Ryu-Kishin Powered.

## Testes imediatos (v0.78.21)
1. Zera Ritual #671 de ambos os lados (não invocação normal): Yami ativa com apresentação nativa.
2. Monstros não-Fiend de ambos os lados perdem 1500 ATK/DEF; Fiends e Ultimate não perdem.
3. Monstro não-Fiend colocado depois recebe penalidade; saída de Zera remove a penalidade sem mexer no Field Yami já ativado.
4. 2 Zeras simultâneos (se aplicável): cada um contribui -1500; um sair restaura apenas uma parcela.
5. Chakra: destruição em ordem, ATK efetivo, arredondamento, bônus permanente, popup único e interações com Ultimate/Tri-Horned/Sengenjin.
6. Regressão: Fake Trap/Widespread Ruin vs Ultimate; animação de Garma sem flash; desempenho com monstros de efeito.

## Aprovados anteriormente pelo usuário
- 21 descrições alinhadas à esquerda sob `<Ritual Effect>`.
- Garma Sword: contadores padronizados.
- Magician of Black Chaos: sem Defesa com efeito ON; com efeito OFF normal.
- Javelin: proteção consumida e sem fogo/destruição.
- Ultimate CPU ataca apesar de Swords.
- Fiend's Mirror: reflexão de dano.
- Chaos + Ultimate + Dark Hole: sem travar.
- Manter restante do histórico e regras da v0.78.20, na seção histórica deste checkpoint único.

## Instruções do usuário
- Alterar somente o que for pedido; não gerar ZIP até que seja solicitado.
- Distinguir compilação de confirmação de runtime. Desenvolver próximos efeitos como continuação.
- Próximo Ritual ainda a definir: Dragon Piper.


---

## Apêndice C — Histórico técnico integral de otimização v0.75–v0.76

As observações abaixo pertencem às versões em que foram registradas; alguns diagnósticos eram temporários e foram removidos. Preservadas integralmente para evitar repetir auditorias e restaurar regressões.

# CHECKPOINT DE OTIMIZACAO — MOD Efeitos de Cartas Rituais v0.75

## Objetivo deste arquivo
Este checkpoint existe para que uma futura rodada de expansao/polimento **nao recomece a auditoria de desempenho do zero** e, principalmente, para que nenhuma otimizacao aprovada seja revertida por engano.

Base de partida desta rodada: **v0.74.0-test-performance**.
Resultado da rodada: **v0.75.0-test-performance**.

Regra principal da rodada: **reduzir custo, scans, chamadas e trabalho por frame sem mudar o funcionamento efetivo dos Rituais**. Quando uma ideia de otimizacao trazia risco sem ganho relevante, ela foi deliberadamente deixada de fora.

---

## Principios usados

1. **Fast-path para o jogo original**
   - Hooks muito chamados retornam imediatamente para a funcao original quando nenhum efeito Ritual precisa interferir.
   - Nao simular o jogo quando nao e necessario; deixar o codigo original executar.

2. **Estado vivo em vez de historico**
   - Contadores como `*_summons` continuam existindo para diagnostico, mas nao devem manter hooks caros acordados.
   - Para hot paths, usar `*_live_mask`, `*_live_hint`, `stat_effect_hints` e contadores de estado realmente ativo.
   - Quando a ultima instancia sai do campo, o subsistema deve poder voltar a dormir.

3. **Bitmasks em vez de scans cegos**
   - Os 30 registros do duelo cabem em `unsigned` de 32 bits.
   - Filas, instancias vivas, pendencias e restauracoes usam bits e `take_mask_bit()` para visitar apenas entradas existentes.

4. **Cache de identidade/slot com fallback seguro**
   - Mask e Psycho-Puppet guardam o ultimo slot conhecido.
   - O cache so e aceito se `data`, `card_id`, ocupacao e demais invariantes ainda conferirem.
   - Se falhar, volta para a busca antiga por identidade. Nunca confiar apenas no slot.

5. **Precalcular arte estatica fora do renderer**
   - Sprites/arte imutavel devem ser convertidos em runs uma vez e reutilizados.
   - Evitar rescans de pixels por frame.

6. **Settings fora dos hot paths**
   - Valores sao lidos/cacheados na inicializacao e Apply.
   - Evitar `host->setting()` dentro de calculo de stats, battle, renderer, AI etc.

---

## O que ja foi otimizado nesta rodada

### Hooks e caminhos gerais
- `Duel_CalcCardStats`: caminho quase-vanilla quando nenhum efeito vivo altera stats/Fear.
- `DuelScene_UpdateFieldActions`: fast-path quando o MOD nao tem trabalho de campo pendente.
- `DuelScene_UpdateBattle`: fast-path quando nenhum efeito Ritual precisa tocar a batalha.
- `DuelScene_UpdateCardPlacement`: fast-path quando Mask/Psycho nao precisam interferir.
- `DuelEffect_UpdateCardEffect`: fast-path quando Tri/Whale/Shield/Mirror nao precisam da resolucao.
- destruicao de campo / remocao / Raigeki: fast-path sem contexto de protecao.
- `DuelScene_Update`: retorno imediato apos o update original quando nao ha sequencia visual Ritual ativa.
- overlay e `overlay_signature`: assinatura constante e retorno imediato em frames onde o MOD nao desenha nada.
- hooks de IA do Serpent: caminho original imediato sem Serpent Ritual vivo.
- hooks 3D/moeda: caminho original imediato quando Performance/Chaos nao estao ativos.

### Live masks / live hints
Implementados ou consolidados para:
- Millennium Shield
- Yamadron
- Hungry Burger
- Javelin Beetle
- Crab Turtle
- Fortress Whale
- Garma Sword
- Tri-Horned Dragon
- Gate Guardian
- Serpent Night Dragon
- Fiend's Mirror
- Performance of Sword
- Magician of Black Chaos
- Psycho-Puppet
- Mask of Shine & Dark (mascara de instancias)

### Millennium Shield
- Reconciliacao usa live mask em vez de scan completo.
- Snapshots de atacantes sao feitos somente quando Shield realmente precisa deles.
- Releituras pos-batalha/pos-magia visitam instancias marcadas.

### Yamadron
- Restauracao de miniaturas visita apenas slots gerados marcados.
- Upload para VRAM so ocorre quando o retangulo realmente mudou.
- Inicio de turno so executa geracao se existir Yamadron vivo.
- Limpeza pos-batalha usa live mask.

### Psycho-Puppet
- `psycho_mark_mask`, `psycho_controlling_mask` e `psycho_pending_mask` substituem buscas cegas.
- Source e monstro roubado usam slots cacheados com fallback por identidade.
- Registros mortos sao descartados quando nao sao mais necessarios.
- Logica de alvo aleatorio, devolucao e restricoes nao foi alterada.

### Hungry Burger
- Instancias vivas por bitmask.
- Limpeza pos-batalha visita somente Hungry realmente marcados.
- Bonus por kill e apresentacao continuam iguais.

### Javelin Beetle
- Instancias vivas por bitmask.
- Busca de fontes usa somente Javelins ativos.
- Reset de protecao por turno visita apenas instancias marcadas.
- Ordem de testes baratos foi antecipada.

### Crab Turtle
- Instancias vivas por bitmask.
- Aura procura apenas fontes Ritual ativas.
- Restauracao de sobrevivente/batalha usa bits relevantes.
- Umi e regras da aura permanecem iguais.

### Fortress Whale
- Instancias vivas por bitmask.
- Fontes validas sao calculadas uma vez por lado durante snapshots de protecao.
- Lado sem Whale nao percorre suas 5 zonas.
- Protecao continua exigindo Umi + Magia/Armadilha.
- Um Whale nao protege a si mesmo; Whales diferentes podem se proteger.
- Durante restauracao de stats, a validade das fontes **continua sendo checada apos o tick do efeito**, preservando a semantica anterior.

### Garma Sword
- Instancias vivas por bitmask.
- Fila visual e ataques extras visitam apenas bits ativos.
- Icone da espada foi convertido para uma tabela estatica de runs na inicializacao; nao escaneia mais o sprite 17x17 por frame.
- Geometria/cores/ordem de desenho foram preservadas.

### Tri-Horned Dragon
- Instancias vivas por bitmask.
- Fontes da aura usam somente Tri vivos.
- Restauracoes/guards usam bits relevantes.
- Icone do chifre foi convertido para runs uma vez na inicializacao; nao escaneia mais 1024 pixels por icone por frame.
- Snapshot de protecao de Magia permanece pre-resolucao e e mantido durante toda a resolucao, como antes.

### Gate Guardian
- Instancias vivas por bitmask.
- Reset/controle de ataques adicionais visita apenas Gate ativos.

### Serpent Night Dragon
- Live mask e `serpent_seen` voltam a zero quando a ultima instancia valida sai.
- IA entra direto no codigo original sem Serpent vivo.
- Reconciliacao visita somente Serpents marcados.
- Contadores de diagnostico em hooks quentes so incrementam com Debug Overlay ligado.
- Arte do Fear usa runs preparados, sem recomputar pixels por frame.
- Conhecimento da IA continua preso a identidade da instancia Ritual.

### Fiend's Mirror
- Live mask por instancias reais.
- Monitor de LP dorme quando nao existe Mirror vivo nem reflexo/fila pendente.
- Ao surgir uma nova janela de Mirror, a referencia de LP e rebaseada naquele momento: dano sofrido sem Mirror nao pode ser refletido retroativamente.
- Nao limpar prematuramente o mark durante batalha: preservar o mecanismo de `mirror_battle_grace`/ultimo suspiro.

### Performance of Sword / Magician of Black Chaos
- Live masks separados para Performance e Chaos.
- `coin_reconcile()` visita somente instancias realmente existentes.
- Hooks 3D usam fast-path quando moeda/efeito nao estao ativos.
- Black Chaos retorna imediatamente no inicio do turno quando aquele lado nao possui Chaos.
- Quando ha varios Chaos, a ordem original por coluna continua sendo respeitada.
- Escalador da moeda usa bordas inteiras cacheadas e agrupamento de pixels/runs; resultado visual permanece o mesmo.

### Mask of Shine & Dark
- Instancias usam bitmask de ocupacao.
- Slot conhecido fica cacheado com fallback completo por identidade.
- Fusion/IA visitam somente instancias registradas.
- **Hotpass final:** Mask sozinho nao mantem mais `DuelScene_UpdateBattle` acordado. Mask nao possui comportamento durante a batalha; se for o unico efeito ativo, sua reconciliacao e feita em `FieldActions` antes da proxima acao/Fusion possivel. Se outro Ritual ja mantiver o wrapper de batalha ativo, a reconciliacao pos-batalha continua acontecendo como antes.

### Filas visuais
- Fila de stats/ATK/DEF usa bitmask de 32 entradas em vez de procurar linearmente os 32 slots.
- Mantida a prioridade pelo menor indice ativo.

---

## Hotpass 18 — auditoria de encerramento

A ultima auditoria procurou principalmente:
- scans de 30/32 registros em funcoes chamadas por frame;
- scans de 10 Monster Zones em hooks quentes;
- settings consultados durante gameplay;
- renderizadores que percorriam pixels estaticos;
- contadores historicos usados como gates;
- wrappers que permaneciam acordados sem efeito real.

A unica alteracao adicional considerada claramente vantajosa/segura foi tirar **Mask of Shine & Dark** do gate do wrapper de batalha, porque Mask nao possui efeito durante battle e `FieldActions` ja reconcilia a instancia antes da proxima acao possivel.

Os scans restantes foram **deliberadamente mantidos**, pois sao pequenos, condicionais e/ou acontecem em eventos raros. Troca-los por caches globais aumentaria risco de estado obsoleto por ganho minimo.

---

## Scans restantes que sao INTENCIONAIS

Nao tratar estes loops como "bug de performance" sem medir primeiro:

- `snapshot_attack_monsters`: no maximo 10 Monster Zones, somente com Millennium Shield relevante.
- `allied_insect_count`: 5 zonas do lado, somente ao calcular aura de Javelin.
- `fish_in_hand_and_field`: 10 cartas (5 mao + 5 campo), somente para DEF do Fortress Whale.
- `tri_begin_resolution`: 10 Monster Zones, somente no inicio de resolucao de uma Magia relevante.
- snapshots/restauracao do Whale: ate 5 zonas por lado e somente durante efeito relevante.
- fallback de identidade de Mask/Psycho: ate 10 Monster Zones, somente quando o slot cacheado deixou de ser valido.
- escolha de tributos de receitas especiais: ate 10 candidatos, somente na checagem de Ritual; as classificacoes por candidato ja sao cacheadas dentro da busca.
- geracao do Yamadron: ate 5 zonas M/T, apenas quando ele realmente gera cartas.
- renderer da moeda: ainda desenha os pixels/runs necessarios enquanto a moeda esta visivel; fora da moeda, o caminho e praticamente vazio.

---

## Invariantes que futuras otimizacoes NAO devem quebrar

1. **Identidade Ritual = `data`/instancia, nao apenas `card_id`.**
2. Batalha pode temporariamente desativar/reconstruir registros; nao interpretar toda desativacao como morte definitiva.
3. `inside_field_actions` e deferred masks existem exatamente por esse motivo.
4. Fiend's Mirror usa `mirror_battle_grace` para a semantica de batalha; nao limpar sua instancia cedo demais.
5. Fortress Whale deve validar fontes no momento correto da resolucao; nao transformar protecao em snapshot eterno.
6. Serpent AI aprende por instancia/lado. Nao transformar conhecimento em flag global.
7. Psycho-Puppet precisa preservar propriedade/identidade do monstro roubado; nao tratar como material normal do controlador temporario.
8. Mask usa cache de slot somente como acelerador; sempre manter fallback por identidade.
9. `stat_effect_hints` deve ser armado na invocacao e retirado quando a ultima fonte correspondente some. Nao usar contadores cumulativos como substituto.
10. `overlay_signature` deve incluir todo estado que pode mudar pixels. Um hash incompleto pode deixar desenho velho em cache.
11. Settings de efeito/receita podem mudar via Apply; o cache precisa ser atualizado ali.
12. Nao remover os fast-paths ao adicionar novos efeitos. Em vez disso, incluir o novo estado no gate somente se aquele hook realmente precisar dele.

---

## Padrao recomendado para NOVOS Rituais

Ao adicionar um novo efeito:

1. Criar mark por instancia se o efeito precisa distinguir Ritual de copia comum.
2. Criar `live_mask` se o efeito persistir no campo.
3. Armar mask/hint na invocacao Ritual.
4. Limpar em `remove_card` quando a saida for definitiva.
5. Tratar deativacao temporaria de batalha separadamente.
6. Reconciliar em fronteira estavel de campo, visitando somente os bits ativos.
7. Adicionar o efeito a `*_hook_needed()` **somente nos hooks que realmente precisam dele**.
8. Se nao existir instancia viva, chamar a funcao original imediatamente.
9. Para arte estatica: converter em runs/cache uma vez, nunca revarrer imagem por frame.
10. Para Settings: adicionar ao cache de `refresh_settings`, nunca consultar host em hot path.
11. Para filas/pendencias com <=32 registros: preferir bitmask.
12. Antes de "otimizar" um scan de 5/10 itens raro, medir se ele realmente aparece no hot path.

---

## Como fazer a proxima rodada de polimento

Quando o MOD crescer novamente:

1. Partir desta v0.75/checkpoint, nao da v0.74.
2. Listar os novos hooks e perguntar: quantas vezes por frame/batalha sao chamados?
3. Procurar novos scans de 30 registros dentro de hooks frequentes.
4. Procurar novos `host->setting`, decode de imagem, alocacoes ou `snprintf` em hot paths.
5. Ver se algum novo contador de "ja apareceu alguma vez" esta mantendo um hook ligado para sempre.
6. Converter isso para live mask/hint quando aplicavel.
7. Conferir se o novo renderer recalcula dados imutaveis por frame.
8. Comparar comportamento antes/depois com casos deterministas.
9. Compilar pelos dois caminhos usados nesta rodada (SDK/Clang e GCC 32-bit).
10. So depois empacotar e testar no jogo real.

---

## Estado de validacao deste checkpoint

Validacoes de desenvolvimento usadas durante as Hotpasses:
- builds repetidos com SDK/Clang e GCC 32-bit;
- objetos ELF i386 validos;
- relocations esperadas `R_386_32` / `R_386_PC32`;
- conjunto de simbolos externos comparado entre Hotpasses consecutivas;
- comparacao deterministica da fila visual antiga/nova;
- verificacao de equivalencia das relacoes de protecao do Fortress Whale;
- comparacao dos retangulos produzidos pelos renderers antigos/novos de Garma e Tri-Horned.

A validacao acima **nao substitui teste dentro do jogo**. A v0.75 deve ser tratada como build de teste de performance ate confirmar em gameplay os efeitos e combinacoes com os demais MODs.

---

## Resumo para uma futura IA/programador

**Nao refaca a varredura inteira.** O MOD ja passou por uma rodada extensa de performance. Comece pelo codigo adicionado depois da v0.75 e pelos novos hot paths. Os grandes padroes que ja foram resolvidos sao: scans cegos, estado historico mantendo hooks acordados, Settings em hot paths, filas lineares, reconciliacao de campo completa e rasterizacao repetida de arte estatica.

Se uma nova mudanca parecer exigir desfazer live masks, fast-paths ou caches deste checkpoint, primeiro demonstre por que a nova mecanica exige isso e procure uma forma de estender o padrao existente em vez de voltar ao comportamento antigo.

## Addendum pos-checkpoint: v0.75.1-test-chaos-queue

Status no momento do pacote: **correcao compilada; teste em jogo ainda pendente**.
Performance of Sword foi aprovada pelo usuario na v0.75.

Um video com dois Magician of Black Chaos mostrou que a cena nativa podia
comecar a liberar a mao entre itens ainda pendentes da fila de inicio de turno;
um Black Hole bem-sucedido nessa condicao tambem terminou em soft-lock.

Correcao aplicada sem desfazer o polimento v0.75:
- hook leve em `DuelEffect_UpdateState` chama sempre a rotina original, mas
  reporta a cena como ocupada enquanto `chaos_turn.active` estiver verdadeiro;
- isso segura somente o callback da cena, mantendo os atualizadores nativos de
  CardEffect/EffectState executando e concluindo sua limpeza;
- `chaos_sequence()` tambem exige `gDuel_bEffectState == 0` e nenhum CardEffect
  ativo antes de iniciar/avancar outro item da fila;
- o turno so volta ao jogo depois que `chaos_turn` consumir todos os Chaos.

Ao retomar este projeto no futuro, **nao remover esse gate como "redundante"**:
ele e a barreira de serializacao entre varios Black Chaos e o fluxo nativo do
turno. Depois de aprovado em jogo, tratar v0.75.1 (ou sucessora) como parte do
checkpoint funcional.


## Addendum corretivo: v0.75.2-test-chaos-phase-gate

Status no momento do pacote: **compilado e validado estaticamente; teste em jogo pendente**.

O teste real da v0.75.1 confirmou que a serializacao melhorou (a mao deixou de
avancar entre Chaos pendentes), mas o soft-lock apos um Black Hole bem-sucedido
continuou. Portanto, o gate global documentado no addendum v0.75.1 NAO deve ser
tratado como solucao aprovada.

Correcao v0.75.2:
- REMOVIDO o hook que fazia `DuelEffect_UpdateState` retornar ocupado enquanto
  `chaos_turn.active` estivesse ativo;
- ADICIONADO um hook direcionado em `DuelScene_UpdateDrawPhase`; ele simplesmente
  retorna enquanto a fila Chaos possui o inicio do turno e chama o original
  assim que a fila termina;
- `DuelEffect_UpdateCardEffect`, `DuelEffect_UpdateState` e o processamento de
  requests continuam inteiramente nativos e podem concluir Black Hole/efeitos
  sem um estado ocupado artificial;
- `chaos_sequence()` continua aguardando `gDuel_wCardEffectFlags` e
  `gDuel_bEffectState` realmente ficarem ociosos antes de avancar a fila.

**Regra para futuras manutencoes:** a barreira correta da fila Black Chaos fica
na fronteira do DrawPhase, nao no dispatcher global de EffectState. Nao restaurar
o hook global da v0.75.1. Se a fila precisar esperar um novo subtipo de efeito,
estender as condicoes de idle da propria fila sem falsificar o retorno global do
engine.


## Addendum de diagnostico: v0.75.3-test-chaos-diagnostic

Status: a v0.75.2 manteve a fila serial correta, mas o soft-lock apos um
Black Hole HEADS continuou. Nao aplicar nova correcao por tentativa antes de
capturar o estado do engine.

Ferramentas temporarias adicionadas:
- `chaos_diag_overlay`: telemetria no canto superior esquerdo com frame do host,
  chamadas de `DuelScene_Update`, phase/scene/substate, `chaos_turn`,
  `gDuel_wCardEffectFlags`, `gDuel_wEffectCardID`, `gDuel_bEffectState`,
  `gDuel_bEffectRequestStatus`, `D_8009B112_abs`, `D_8009B0F4_abs`,
  `D_8009B134_abs`, ticks do handler de BoardDestruction, remocoes e protecoes
  de Chaos durante Dark Hole.
- `chaos_quick_draw`: hook TEMPORARIO em `DuelScene_UpdateDrawResolution`.
  Procura duas copias REAIS de 721/35/7/417 no deck do jogador e pede ao
  renderer/setup retail que compre esses indices primeiro. O cursor logico e
  devolvido a sequencia normal apos cada compra; nao sintetiza carta nem dados.
- `chaos_force_heads`: passa `RCOIN_HEADS` somente para a moeda de inicio de
  turno do Black Chaos para reproduzir o Dark Hole imediatamente.

Essas ferramentas nao fazem parte do design final e devem ser removidas ou
deixadas OFF depois que o soft-lock for isolado. A barreira v0.75.2 em
`DuelScene_UpdateDrawPhase` continua intacta nesta build.


## Addendum de release: v0.76.0-optimized

Status: efeitos e correcoes da linha v0.75 foram aprovados em gameplay pelo usuario.
Esta revisao e exclusivamente de limpeza/performance e nao muda regras de Ritual.

Removido da v0.75.3 diagnostica:
- `chaos_diag_overlay` e todos os contadores/branches de telemetria Chaos;
- `chaos_quick_draw`, indices/scan do deck e o hook de `DuelScene_UpdateDrawResolution`;
- `chaos_force_heads`; a moeda de Black Chaos volta sempre ao fluxo real;
- settings TEST correspondentes e texto de diagnostico no overlay.

Preservado:
- a barreira direcionada de Black Chaos em `DuelScene_UpdateDrawPhase` da v0.75.2;
- espera real por `gDuel_wCardEffectFlags`/`gDuel_bEffectState`;
- todos os fast-paths, live masks/hints, caches e scans intencionais documentados acima;
- `debug_overlay` geral, OFF por padrao, para diagnostico futuro sem fixture de gameplay.

Ganho estrutural desta revisao: DrawResolution deixa de ter qualquer wrapper deste MOD em jogo normal;
DuelScene_Update, BoardDestruction, CardEffect, RemoveFromField, coin frame e overlay deixam de executar
branches/contadores exclusivos da build diagnostica.


## Addendum v0.78.25-test — Skull Guardian e interface de Rituais

**Mudancas somente no MOD Efeitos de Rituais.** Base inicial: copia de
trabalho `rituais-working-unreleased.zip`, ja contendo a camera Chakra, a
quebra de linha do Skull e as correcoes de Beacon anteriores.

- Avaliada a fonte `rules.c`/`selection.h` do Melhoria dos Rituais 1.2.9,
  que revalida candidatos Campo/Mao/Deck por `Duel_CheckRitual` usando
  3 registros temporarios com objetos nao-renderizaveis. Essa implementacao
  nao foi alterada nem incorporada ao pacote Efeitos de Rituais.
- Skull Guardian / Novox Prayer 679: nova verificacao independente de
  tres materiais distintos no contexto da consulta temporaria. A categoria
  Skull Monster permanece local (fusion_group privado 255), sem afetar
  Fusion, tipos ou cartas fora da receita.
- **Regra critica:** nunca substituir uma rejeicao normal do Melhoria dos
  Rituais por sucesso local. Isso contornaria `Require Ritual Monster card`,
  minimos de Campo/Mao e possiveis outras restricoes. O fallback adicional
  usa exclusivamente a assinatura da validacao temporaria da v1.2.9;
  chamadas normais preservam o resultado da cadeia.
- Diagnostico `diagnostic_skull_ritual` (OFF) usa overlay com STEP:
  3 sucesso na cadeia, 4 sucesso pelo fallback de oracle; 1/2 sem sucesso.
  O diagnostico da origem Deck deve ser lido no overlay do Melhoria.
- Crab Turtle usa exclusao por instância com a mesma funcao de Beacon de
  Javelin/BLS/Dokurorider; duas instancias distintas podem beneficiar-se.
- SDK Windows v0.2.1-preview.1: build realizada. Harness Skull/Beacon
  aprovados internamente. Runtime Windows e validacao combinada pendentes;
  veja CHECKLIST-TESTE-v0.78.25.txt.


### v0.78.28-test — Skull Guardian (Deck) — pendente confirmação em jogo
- Relato: não completa a invocação com os três materiais apenas no Deck, mesmo
  com mínimos de Campo/Mão em zero e opção Deck ligada.
- Diagnóstico de código: o grupo Skull Monster é privado. A versão anterior
  consultava Vanilla antes de responder ao oracle público do MOD de sourcing.
- Implementação isolada: reconhecimento de três registros temporários apresentado
  via Duel_CheckRitual; valida diretamente os requisitos, antes de preparar ou
  mutar a tabela Vanilla. Nunca procura ou consome do Deck.
- Sem parceiro instalado: somente três tributos do Campo, como antes.
- Overlay expandido: cartas ainda não compradas do Deck por requisito e
  consultas Oracle aceitas/recusadas, para diagnóstico in-game se persistir.
- Testes: simulacao de pipeline 11/11, regressões anteriores passaram.
- In-game ainda pendente. Não declarar resolvido até teste do usuário.


### v0.78.29-test — Substituição exclusiva da receita e sprites na câmera do Chakra
- Base: arquivo entregue v0.78.28-test, sem mudanças no MOD Melhoria dos Rituais, no repo upstream ou em outros MODs.
- Receita modificada ON: o retorno final de Duel_CheckRitual somente é aceito se a seleção de três cartas do Campo pelo próprio MOD foi válida OU se houve validação positiva de uma combinação física via hook público de consulta temporária. Uma resposta da antiga receita Vanilla, sozinha, não autoriza a invocação.
- Receita modificada OFF: não interfere na receita Vanilla. A busca/consumo de Campo/Mão/Deck continua exclusivamente com Melhoria dos Rituais. Sem ele, requer três tributos no Campo.
- Se faltar definição privada na receita ativa, não republicar tributos Vanilla silenciosamente (fail closed).
- Mitigação de cartas visuais soltas durante a câmera de Chakra: filtrar no renderizador somente prévias lógicas da Mão e pequenos sprites de registros de monstros já removidos, exclusivamente enquanto Chakra controla a câmera. Não alterar objetos, dados, seleção, combate, efeitos do Raigeki ou outros MODs; monstros ainda vivos são desenhados.
- Validação interna: harness dos requisitos/Beacon/Crush + boundary oracle + cenários de substituição e filtro visual; recompilação Windows SDK. Nenhuma confirmação runtime foi obtida desta revisão; os resultados de jogo anteriores continuam aprovados.


### v0.78.30-test — Chakra aprovado, aproximação; Skull Guardian Deck em investigação
- Resultado real do usuário na v0.78.29: efeitos do Chakra (Raigeki, retorno e buff) aprovados; pedido apenas de enquadramento mais concentrado no campo adversário. A abertura da câmera passa de (0x258,0x100) a (0x218,0xE8); duração de 16 quadros e todos os estágios seguintes preservados.
- Ritual Skull Guardian com três tributos somente no Deck segue **não aprovado** na v0.78.29; causa exata ainda indeterminada porque falta captura dos diagnósticos dos DOIS MODs durante essa tentativa.
- A inspeção da fonte enviada de MelhoriaDosRituais v1.2.9 confirma que o verificador público recebe três objetos temporários em registros de campo, mas a v0.78.29 reconhecia os candidatos exigindo que os campos de renderização estivessem zerados. Esse detalhe incidental foi removido: a v0.78.30 identifica objetos sintéticos por estarem fora da pool real, por continuidade de memória e pela identidade `field_6A`; os requisitos privados permanecem intactos e a cadeia de hooks deve aprovar somente materiais válidos.
- O overlay de diagnóstico OFF por padrão também exibe `PROBES`, `SYNTHETIC`, `LAST TRIO` e máscara das regras para cada candidato. Capturas determinam se o problema foi upstream (mod de sourcing sem consulta), interface temporária, receita inválida ou etapa posterior de consumo.
- Regressão por harness C da função real de detecção: layout v1.2.9 e layout com campos gráficos extras passam; carta Vanilla real, menos de três cartas, quarto monstro, identidade inválida e ID inválido são recusados. Compilação SDK Windows realizada; runtime aguardando usuário.
- Não foi alterado Melhoria dos Rituais, SDK, decomp ou qualquer outro MOD.

## v0.78.50 — Fiend's Mirror DEF: sincronizacao do comparador
A correcao v0.78.49 nao funcionou em batalha real porque `DuelScene_UpdateBattle` troca `D_8009B174` de 3 para 4 antes de chamar `func_8001EFD4`. O guard em `sengenjin_battle_compare` testava 3. Em v0.78.50, apenas esse guard passou a exigir estado 4. `battle()` continua a aplicar LP no tick de entrada estado 3 apos o callback nativo. Teste `tests/test_mirror_defense.py` atualizado para impedir regressao. Sem modificacoes do Psycho-Puppet, de pools, receitas ou de MODs separados; ainda requer confirmacao no jogo.
