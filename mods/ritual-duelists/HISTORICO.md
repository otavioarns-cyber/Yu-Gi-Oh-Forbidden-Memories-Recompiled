## 2026-11-06 — v0.4.0-alpha.8 — Remoção do Drop List interno / compatibilidade com Droplist 1.4.0

- Por solicitação do usuário, o visualizador de Free Duel do Ritual Duelists foi totalmente retirado: `rd_droplist.c/.h`, `rd_dl11_sprites.h`, arte de UI exclusiva e teste do visualizador foram removidos. Configuração `drop_list_enabled` retirada.
- `mod.c` não registra callback `frame` nem captura/digerir Triângulo/Círculo. O overlay de diagnóstico de Ritual Duelists (`rd_overlay`) permanece; o seletor inicial permanece por hook próprio.
- `mod.json` mantém **id, drops, after drop-missing-cards, settings restantes e requisitos da API**. Não são alterados pesos, receitas, IA, recompensas, fontes esgotáveis de Blue-Eyes, saves/savestates, fluxo de Story/Free Duel.
- Interface/atalho de consulta exclusivamente do Droplist 1.4.0 de **PauloH-Gudang**, distribuído separadamente e **sem alterações no pacote do autor**. O objeto externo referencia `Tables_PoolFor` para consultar as pools ativas.
- Limitação: a probabilidade exibida pelo Droplist externo pode manter o Blue-Eyes de fonte única na lista após consumo, porque o bloqueio de repetição está na premiação em runtime do Ritual Duelists, fora dos pesos de `Tables_PoolFor`.
- Validações: comparar `drops` byte/lógica com Alpha.7, testar build Windows e tests restantes; gameplay Windows depende de teste do usuário.

## 2026-10-19 — v0.4.0-alpha.7: Mountain Mage — Seiyaryu no lugar de Gaia DC

- **Somente deck de Mountain Mage #25:** removido **Gaia the Dragon Champion #37 (peso 144)** da pool real e acrescentado **Seiyaryu #358 (peso 144)** no mesmo lugar de raridade, para acompanhar a migração do efeito protetor no MOD independente de Efeitos Auxiliares.
- **High Mage Atenza #26:** Gaia the Dragon Champion #37 continua normalmente com peso **32**, sem mudança na pool. Seiyaryu pré-existente em Atenza também foi mantido.
- A pool do Mountain Mage continua com **60 cartas reais**, somando **2018 pontos reais + Ritual Package Yamadron 30 = 2048**. Nenhum outro peso/ID de deck mudou.
- Drop pools dos Pacotes 01–03, a nova Drop List 1.1, Triângulo, IA, rituais, configurações, saves e outros MODs permanecem intactos. Alteração exclusivamente no `profile_data.h` e testes de auditoria do deck; objetos i386/x64 recompilados.
- Não altera o efeito do Seiyaryu (já implementado no MOD auxiliar). A mudança é apenas de disponibilidade no deck CPU, **não é um novo drop**.

## 2026-10-18 — v0.4.0-alpha.6: Drop List 1.1, tema egípcio

- Visualizador atualizado com moldura dourada, ícones de Ankh e Olho de Hórus, abas, barras de chance, coleção 0/3–3/3, cursor, adaptação ao tamanho da tela e navegação por L2/R2. Triângulo continua abrindo/fechando, Bolinha não fecha. Os objetos e assets de referência foram estudados; interface reimplementada em C, sem acoplar o binário original do Droplist.
- Integração com Drop Missing Cards, Pacotes 01–03 e fontes únicas de Blue-Eyes preservada. Somente interface, assets e binários recompilados; nenhuma pool de deck/drop alterada na Alpha.6.

## v0.4.0-alpha.5 — 18/10/2026 — Pacote 03: tributos, Blue-Eyes únicos e História

- Continua sobre a Alpha.4, mantendo Pacotes 01/02, todas as pools de decks e Ritual Packages, IA, seletor de três decks iniciais e Simon Muran. Drop Missing Cards permanece ATIVO, independente e carregado antes de Ritual Duelists.
- **Regra final expressamente aprovada para Heishin 2nd #35:** Dark Magician #35 **POW peso 32 e B/C/D peso 15** simultaneamente no Free Duel; isto substitui a antiga proposta de removê-lo do POW. Meteor B. Dragon #713 mantém peso 4 em POW. Na História, o prêmio garantido de POW continua sendo #713, e o de B/C/D é #35.
- B/C/D: cinco tributos Blue-Eyes #1, Tiger Axe #64, White Turtle #449, Garvas #483, Kamakiriman #535 recebem fontes e pesos apenas nos duelistas aprovados. Blue-Eyes POW em Seto #7=10, Kaiba #17=16, Seto 2nd #32=25, Seto 3rd #36=26 (sem limite), Seto 3rd B/C/D=20. Relocações exigidas de Tiger Axe, Garvas, White Turtle, Kamakiriman e King of Yamimakai preservadas, sem mexer em decks.
- **Blue-Eyes de fonte única:** Seto #7, Kaiba #17 e Seto 2nd #32 se esgotam independentemente após **entrega efetiva** de #1 no resultado; o simples sorteio ou visualização não consome o direito. Quando esgotados, novas rolagens #1 nessa fonte são rejeitadas, preservando prêmio válido. O marcador sobrevive a savestates por estado registrado e a saves normais por arquivo na área privativa do MOD, indexado pelo token renovável do slot (incluindo tokens com bit alto). Saves antigos não são inferidos pela coleção. Recarga de resultado anterior ao consumo reconcilia a carta pendente.
- **História: somente carta principal SPOILS:** Labyrinth #31 POW=#374, B/C/D=um entre #371/#372/#373 equiprováveis, TEC=#667; Sebek #33 POW=#217, B/C/D=#704, TEC=#679; Neku #34 POW=#360, B/C/D=#85, TEC=#671; Heishin 2nd #35 POW=#713, B/C/D=#35, TEC=#721; Seto 3rd #36 POW=#380, B/C/D=#1, TEC=#675; DarkNite #37 somente TEC=#709; Nitemare #38 sem qualquer recompensa garantida. Drops extras permanecem sorteios normais, assim como todas as pools do Free Duel.
- Drop List continua acessível com Triângulo para abrir/fechar. Depois do esgotamento de uma fonte única, omite #1 e exibe probabilidades normalizadas dos demais drops POW da fonte (sem falsificar a tabela física base do jogo).
- Novos fontes: `rd_drop_rewards.c/.h`. `mod.c` apenas integra esses eventos; `ritual_duelists.h` anexa três campos de estado (versão de savestate 3→4); `rd_droplist.c` acrescenta somente a regra visual dinâmica. Nenhum outro MOD modificado.
- Auditorias `audit_drops_03.csv` (Alpha.4→Alpha.5, 949 diferenças incluindo normalização) e `audit_story_rewards_03.csv` (matriz História separada). Todas as **117 pools** após composição Vanilla USA + Drop Missing Cards + Pacotes 01/02/03 somam **2048**; suite existente e novas simulações de prémios, múltiplos drops, limites por fonte/save, save token, savestate e Drop List aprovadas.
- **Build:** objetos Windows i386 e x86_64-windows gerados com o SDK disponível v0.3.1-preview.1, mantendo manifesto `min_api:12` e interfaces de API 12 já presentes no código-fonte v0.3.0. O SDK exato v0.3.0 não foi fornecido neste ambiente de construção; portanto a compatibilidade binária real deve ser validada em Windows v0.3.0. Nenhum gameplay Windows nem save real foi executado aqui.

## v0.4.0-alpha.4 — 18/10/2026 — Pacote 02 de drops ritualísticos

- Mantido integralmente o Pacote 01: as nove cartas tributárias seguem disponíveis em B/C/D, sem POW/TEC; Simon Muran e Duel Master K mantêm os pesos 16/32 aprovados para as três magias e monstros prontos; especialistas iniciais mantêm 64 TEC e 96 POW.
- 24 Ritual Magics de 28 perfis ativos: peso 0 em POW/BCD de todos os duelistas; TEC exclusiva aos usuários autorizados e com peso 64, exceto as permissões e pesos iniciais do Pacote 01.
- 28 perfis recebem seu monstro ritual pronto em POW: peso 25 por padrão, preservando exceções históricas de Vanilla + Drop Missing Cards e exceções do Pacote 01. Drop histórico de monstro pronto em outro duelista permanece acessível.
- Onde nova inclusão poderia deslocar o peso de outro monstro ritual pronto, seu peso histórico foi preservado explicitamente na mesma pool; não foram feitos ajustes manuais em cartas não relacionadas, deixando a normalização nativa agir.
- Drop Missing Cards continua independente e obrigatório/ativado como Vanilla expandido; regra `after: drop-missing-cards` preservada. Nenhuma alteração de decks, Ritual Packages, IA, seletor inicial, saves, assets, código C de gameplay ou visualizador de Drop List, incluindo Triângulo para abrir/fechar. O teste legado `tests/test_droplist.c` foi ajustado apenas para verificar o comportamento de Triângulo já aprovado na Alpha.3.
- Auditoria anexa em `audit_drops_02.csv`: diferenças por duelista/carta/categoria com pesos Alpha.3→Alpha.4, motivo e origem da exceção, incluindo diferenças por normalização.
- Simulação das 117 pools (39 duelistas × 3 categorias) com motor de normalização equivalente ao `Tables_Scale` da recompilação Windows v0.3.0; todas somam 2048 e passaram a matriz de permissões. Gameplay Windows aguarda validação do usuário.

## 2026-10-18 — v0.4.0 Alpha.3: Pacote 01 de drops + fechamento seguro da Drop List

- Base: Alpha.2 já validada em gameplay pelo usuário. Mantidos integralmente seletor dos decks iniciais, Simon, IA, 29 perfis e demais regras.
- Integração **declarativa** via `mod.json` com Drop Missing Cards v1.0 ativo como base Vanilla expandida; `after` garante aplicação posterior. MOD independente de terceiros não foi alterado.
- Globalmente, os nove tributos #29/#31/#101/#249/#395/#511/#102/#182/#220 deixam S/A-POW e S/A-TEC, preservando as fontes B/C/D. As Ritual Magics #666/#676/#693 saem de POW/BCD e são exclusivas dos S/A-TEC autorizados: Simon Muran 16 cada; Duel Master K 32 cada; Mountain Mage #666=64; Mai Valentine #676=64; Shadi #693=64.
- S/A-POW: Simon Muran #357/#701/#720=16 cada; Duel Master K mesmos IDs=32 cada; Mountain Mage #357=96; Mai #701=96; Shadi #720=96. Reafirmados os drops históricos Drop Missing Cards: Pegasus #357=30, Isis #701=30 e High Mage Martis #720=20.
- Janela de drops: **Triângulo alterna abrir/fechar**; Círculo é bloqueado dentro do modal. Proteção extra contra propagação da tecla ao menu quando a janela fecha.
- Não alterados: decks/IA de CPU, Ritual Packages, cartas, efeitos, fusões, drop-missing-cards, demais MODs ou branch do projeto.

## 2026-10-18 — v0.4.0 Alpha.2: Visualizador de drops no Free Duel

- Integrado ao próprio Ritual Duelists um menu **Drop List** ativado com **Triângulo** sobre duelistas desbloqueados no Free Duel. Inspirado, com autorização do usuário, no MOD independente **Drop List v1.0.0**, criado por **PauloH-Gudang**. O ZIP de referência possuía apenas objetos compilados; esta integração é uma implementação nova em C, sem alterar ou contornar o binário original.
- Exibe os drops atuais das categorias **S/A-POW, B/C/D e S/A-TEC**, com ID, nome, chance ponderada e cópias possuídas até o limite 3/3. Controles: L1/R1 para alternar pools; X para filtrar cartas com menos de três cópias; Quadrado para alternar a ordem (chance/ID/nome); cima/baixo para navegar; Círculo para fechar.
- Lê as linhas Vanilla de `WA_MRG.MRG` com os serviços de disco do SDK e consulta `Tables_PoolFor` para respeitar as alterações de drops habilitadas neste jogo, incluindo as futuras alterações realizadas no Ritual Duelists. Sem escrita de pools ou alteração nas probabilidades.
- Adicionado setting `drop_list_enabled` (ON por padrão) para desativar apenas o visualizador. O modal bloqueia os botões destinados ao menu original enquanto estiver aberto e se fecha ao sair do Free Duel.
- **Zero mudanças** em pesos de decks, Ritual Packages, 29 perfis, IA, rituais, drops, efeitos, receitas e outros MODs. Base da entrega: ZIP do usuário `Ritual Duelists(5).zip`, versão `0.4.0-alpha.1`.
- Validação interna: compilação Windows i386 + x86_64, testes simulados de leitura, filtros e abertura/fechamento, além de todas as suítes existentes e auditoria das 29 pools. Gameplay Windows permanece pendente.

## 2026-10-17 — v0.3.5 Beta.8: V4 Mai/Shadi/Anubisius/Meadow/Weevil

- Mai Valentine #11: nova pool Vanilla USA com os três cortes #54/#70/#416 (20→0), Performance of Sword #701 pronta +30 e Ritual Package #676→#701 de peso 30; 118 cartas reais positivas.
- Shadi #13: troca Garma Sword pronto #716 (20→0) por Mask of Shine & Dark #720 (0→20); troca pacote Garma #697 pelo Mask #693, mantendo intactos os outros 117 pesos reais e a reserva virtual já existente.
- Meadow Mage #29: troca somente Ritual Package Sengenjin #678→#703 por Garma Sword #697→#716; mantém Warrior Elimination #653=98 e Hamburger Recipe #677=40.
- High Mage Anubisius #24: pool restaurada diretamente do arquivo WA_MRG.MRG do disco USA, aplicando somente os materiais, cortes em não-Forest e retirada de #678 definidos na V4; 139 cartas reais positivas. Javelin Beetle pronto #717=48, pacote Sengenjin #678→#703 peso 30.
- Weevil #10: pool customizada preservada, Cocoon #72 42→30, Red Medicine #339 0→12; Time Wizard #16=25 e os cortes históricos mantidos. Sem novos cortes do pacote.
- Os cinco perfis agora somam individualmente 2018 reais + 30 virtuais = 2048. Os 24 registros anteriores que não pertencem à V4 permanecem idênticos; Isis segue estritamente Vanilla.
- Sem programação de receitas de fusão, mudanças nos drops, efeitos ou outros MODs. Os efeitos da V4 pertencem à entrega independente de Efeitos de Rituais.
- Testes de deck: suites C, auditoria de 29 registros, geração em Story/Free OFF/Free ON, 40 cartas e 3 cópias por ID. Windows SDK v0.3.0: objetos i386 e x86_64-windows compilados; jogo Windows ainda não executado.

### Deltas completos V4: ID | Vanilla | Beta.7 | Beta.8

Apenas as entradas com diferença entre Beta.7 e Beta.8 são listadas. Para Mai, que não possuía perfil anterior, a coluna Beta.7 é `—`. Os IDs restantes estão preservados de suas bases aprovadas.

**Mai #11** — 118 mudanças em cartas reais

```text
ID | Vanilla | Beta.7 | Beta.8
002 |   8 |      — |   8
006 |  16 |      — |  16
007 |  20 |      — |  20
013 |  64 |      — |  64
026 |  16 |      — |  16
027 |  16 |      — |  16
031 |  16 |      — |  16
032 |   8 |      — |   8
036 |  16 |      — |  16
041 |  20 |      — |  20
043 |  16 |      — |  16
044 |  20 |      — |  20
046 |  16 |      — |  16
049 |  16 |      — |  16
051 |  16 |      — |  16
055 |  16 |      — |  16
060 |   8 |      — |   8
062 |  80 |      — |  80
063 |   2 |      — |   2
064 |   8 |      — |   8
066 |  16 |      — |  16
071 |  16 |      — |  16
077 |  16 |      — |  16
081 |   8 |      — |   8
095 |  12 |      — |  12
115 |   8 |      — |   8
117 |  64 |      — |  64
121 |   8 |      — |   8
125 |  64 |      — |  64
136 |  16 |      — |  16
149 |  16 |      — |  16
151 |  16 |      — |  16
163 |  16 |      — |  16
166 |  16 |      — |  16
168 |  12 |      — |  12
170 |   8 |      — |   8
186 |  48 |      — |  48
193 |  16 |      — |  16
194 |  16 |      — |  16
207 |   1 |      — |   1
216 |  16 |      — |  16
230 |  16 |      — |  16
233 |   8 |      — |   8
234 |   8 |      — |   8
241 |   8 |      — |   8
249 |  16 |      — |  16
255 |  16 |      — |  16
272 |   1 |      — |   1
275 |  16 |      — |  16
281 |  16 |      — |  16
287 |  16 |      — |  16
297 |  16 |      — |  16
300 |   1 |      — |   1
316 |  20 |      — |  20
317 |  20 |      — |  20
318 |  80 |      — |  80
321 |  20 |      — |  20
332 |  20 |      — |  20
336 |  20 |      — |  20
340 |  20 |      — |  20
349 |  20 |      — |  20
351 |   8 |      — |   8
352 |   8 |      — |   8
353 |  16 |      — |  16
363 |   8 |      — |   8
377 |   8 |      — |   8
379 |   8 |      — |   8
381 |   8 |      — |   8
384 |  16 |      — |  16
386 |   1 |      — |   1
412 |  16 |      — |  16
415 |  20 |      — |  20
418 |  16 |      — |  16
430 |   8 |      — |   8
433 |  20 |      — |  20
440 |  16 |      — |  16
447 |  16 |      — |  16
454 |  16 |      — |  16
456 |  16 |      — |  16
459 |  16 |      — |  16
462 |   8 |      — |   8
466 |  40 |      — |  40
470 |  16 |      — |  16
474 |   8 |      — |   8
497 |  16 |      — |  16
502 |   8 |      — |   8
507 |   8 |      — |   8
511 |  16 |      — |  16
512 |  16 |      — |  16
515 |  16 |      — |  16
523 |   8 |      — |   8
532 |   8 |      — |   8
535 |  20 |      — |  20
538 |   1 |      — |   1
551 |   8 |      — |   8
552 |   1 |      — |   1
578 |  20 |      — |  20
581 |  40 |      — |  40
583 |  12 |      — |  12
587 |  16 |      — |  16
595 |  48 |      — |  48
596 |   8 |      — |   8
597 |  16 |      — |  16
607 |  20 |      — |  20
608 |   8 |      — |   8
623 |  16 |      — |  16
625 |  16 |      — |  16
626 |  16 |      — |  16
630 |  16 |      — |  16
631 |   8 |      — |   8
634 |  12 |      — |  12
636 |  32 |      — |  32
647 |   8 |      — |   8
649 |  12 |      — |  12
672 |  20 |      — |  20
683 |  20 |      — |  20
690 |  20 |      — |  20
701 |   0 |      — |  30
```
Ritual Package virtual (peso 30): novo Performance #701.

**Shadi #13** — 2 mudanças em cartas reais

```text
ID | Vanilla | Beta.7 | Beta.8
716 |   0 |     20 |   0
720 |   0 |      0 |  20
```
Ritual Package virtual (peso 30): Garma #716 → Mask #720.

**Anubisius #24** — 46 mudanças em cartas reais

```text
ID | Vanilla | Beta.7 | Beta.8
016 |   0 |     40 |  48
025 |   4 |      4 |   0
028 |   4 |      4 |   0
030 |   4 |      4 |   0
034 |   4 |      4 |   0
040 |   4 |      0 |   4
052 |  32 |     50 |  32
056 |   0 |      0 |  48
057 |  48 |     64 |  48
059 |   4 |      4 |   0
072 |   4 |     50 |  48
076 |   4 |      0 |   2
080 |   4 |      4 |   0
103 |   4 |      4 |   0
112 |   4 |      4 |   0
115 |   4 |      4 |   0
119 |   4 |      4 |   0
132 |   4 |      4 |   0
140 |   4 |      4 |   0
146 |   4 |      4 |   0
153 |   4 |      4 |   0
162 |   4 |      4 |   0
164 |   4 |      4 |   0
169 |   4 |      4 |   0
171 |   4 |      4 |   0
181 |   4 |      4 |   0
196 |   4 |      4 |   0
233 |   4 |      4 |   0
241 |   4 |      4 |   0
257 |   4 |      4 |   0
258 |   4 |      4 |   0
263 |   4 |      4 |   0
308 |  72 |     22 |  72
319 |  72 |     24 |  72
339 |   0 |      0 |  48
361 |   4 |      4 |   0
367 |   4 |     40 |   4
375 |  32 |     40 |  32
453 |  48 |     16 |  48
473 |  32 |     20 |   0
529 |  48 |     16 |   0
533 |  48 |     60 |  48
570 |   4 |      4 |   0
643 |   4 |      0 |   4
647 |   4 |      0 |   4
717 |  48 |     60 |  48
```
Ritual Package virtual (peso 30): Javelin #717 → Sengenjin #703.

**Meadow #29** — 0 mudanças em cartas reais

```text
ID | Vanilla | Beta.7 | Beta.8
```
Ritual Package virtual (peso 30): Sengenjin #703 → Garma #716.

**Weevil #10** — 2 mudanças em cartas reais

```text
ID | Vanilla | Beta.7 | Beta.8
072 |  32 |     42 |  30
339 |   0 |      0 |  12
```
Ritual Package virtual (peso 30): Javelin #717 preservado.

## 2026-10-17 — v0.3.5 Beta.5: V3 Atenza, Meadow e Kepura

- High Mage Atenza (#26): pacote virtual Serpent Night Dragon #706 / magia #691, peso 30; pool USA #680 8→0 e #327 72→50. Serpent Night Dragon pronto #706 preservado em peso 32. Pool real 148 entradas/2018 pontos.
- Meadow Mage (#29): pacote virtual Sengenjin #703 / magia #678, peso 30; somente #653 128→98 na pool real. Hamburger Recipe #677 fica 40. Pool real 70 entradas/2018 pontos.
- High Mage Kepura (#30): pacote virtual Black Luster Soldier #364 / magia #670, peso 30; #697 32→0, #302 120→92, BLS pronto #364 0→30 na pool real. Pool real 165 entradas/2018 pontos.
- As 22 pools têm 2018 pontos reais + 30 pontos virtuais = 2048; as três novas conferidas por SHA256 por par `ID:peso`, conforme prompt aprovado.
- Nenhum drop, efeito ritualístico, receita, parâmetro global de IA, outro perfil ou MOD alterado. As receitas em runtime permanecem sob o provider/tabelas atuais (e eventuais Settings); BLS pronto não recebe efeito próprio de Ritual.
- Testes: auditoria de 22 pools, cenários de material exato/dinâmico, Story/Free OFF/ON e limitação a 40 cartas/3 cópias/3 pacotes; build SDK Windows v0.3.0. Execução real no Windows pendente.

## v0.3.5 Beta.4 — Desacoplamento da inteligência dos efeitos

- Nova API opcional `ritual-card-effects-test:effect_intelligence_v1` consultada pela CPU na avaliação do ritual.
- Ritual Duelists não calcula mais os bônus do Skull Guardian: a fonte é Efeitos de Rituais.
- Quando a API não existe ou o efeito está desabilitado, o planner não atribui bônus imaginários. Invocação genérica preservada.
- Interface de avaliação de rituais é independente da receita ativa; não exige que a receita do MOD Efeitos esteja habilitada.
- Yamadron segue com ativação das mágicas no MOD Efeitos; Darkfire permanece no MOD Balanceamento.

## v0.3.5 Beta.2 — proteção global de Ritual e magia do Yamadron

- Base confirmada: v0.3.5 Beta.1 intacta; nenhuma implementação parcial encontrada após interrupção da conexão.
- Uma invocação ritual efetivamente concluída marca sua instância física; a CPU não pode usá-la em fusão (vetos na decisão e execução). Cópias prontas sem invocação Ritual não são marcadas; o verificador de fusões trabalha por ID, motivo da restrição conservadora a cópias idênticas enquanto o Ritual vive.
- Final Flame, Ookazi e Tremendous Fire geradas pelo Yamadron recebem prioridade de ativação no campo, identificadas por `deck_index == -1`; cópias normais não são forçadas.
- Registro de save-state ampliado numa única estrutura, versão 2, sem modificar regras ou decks dos 19 duelistas; usuários devem iniciar um duelo novo para testar após instalar.
- SDK v0.3.0: testes C + auditoria de pools, ELF32/i386 validado pelos exports e ELF64 compilado; partida real pendente.

## 2026-10-17 — Ritual Duelists v0.3.5 Beta.1 — Rodada V3

Somente decks/pesos dos três novos duelistas, reproduzindo exatamente os anexos ID:peso aprovados (2018 reais + Ritual Package 30). Forest Mage #23: **83** cartas reais, **17 diferenças** da tabela Vanilla, Super War-lion 30, Firewing Pegasus 48; Anubisius #24: **162** cartas, **19 diferenças**, Javelin Beetle 60, Time Wizard 40 sem tributo; Mountain Mage #25: **60** cartas, **57 diferenças**, Gaia the Dragon Champion 144, Darkfire Dragon 128, Meteor B. Dragon 15, Yamadron 16. As cartas rituais mágicas entram apenas por Ritual Package.

Limite de perfis elevado de 16 para 19, sem alteração em nenhum ID/peso dos 16 perfis anteriores; snapshots dos 16 perfis anteriores conferidos. Testes: **22 suites C** incluindo simulações de pacotes em três modos para todos 19 perfis, mais **18.000** decks para receitas da rodada V3 e auditoria Python do anexo. SDK v0.3.0 gera ELF32/i386 com exportações verificadas, e ELF64 Windows compilado sem lista de exportações para comparação. Sem testes de gameplay real. **Drops intocados; efeitos de Gaia/Darkfire não implementados.** Arquivos editados: `profile_data.h`, `profiles.c`, `ritual_duelists.h`, `mod.c` (apenas identificação), `mod.json`, os testes/CSV e documentação existente.

# 16/10/2026 — v0.3.4-beta.1 — Teana 2nd, Ocean Mage, High Mage Secmeton

- Base exata: v0.3.3-beta.3. Adicionados **somente** perfis duelist_id=20/21/22. Os 13 perfis anteriores e seus pesos são idênticos aos da Beta.3. O catálogo completo de pesos, incluindo pacote virtual, está em `spec/Ritual-Duelists-Pools.csv`.
- **Teana 2nd #20:** pool positiva aprovada de 69 cartas reais/2018 pontos; 45 Female (incluindo Cosmo Queen #708 pronta peso 30) = 1215; 17 Fairies de fusão = 287; 7 Magics/Equips/Trap = 516. Harpies #62/#63 e todos os Field Spells excluídos. Cosmo Queen's Prayer #698 aparece apenas no pacote virtual 30. Nome técnico do jogo é `Teana 2nd`, embora o documento de design use Tea 2nd.
- **Ocean Mage #21:** 87 cartas Vanilla → 78 reais/2018. Removidos os 9 Reptiles (#400/#51/#77/#630/#566/#76/#569/#381/#590), total -120, e Ritual Magic #700 da pool real (-48). Alterações de pesos: #565 48→73; #437 48→73; #440 16→36; #617 48→63; #439 48→63; #507 16→26; #60 8→18; #610 48→36. Fortress Whale #718 pronto peso 30. Ritual Package #700 peso 30. Todas as entradas Vanilla não citadas preservadas.
- **High Mage Secmeton #22:** 145 cartas Vanilla → 90 reais/2018. Remoção EXATA dos 55 monstros (−698), 20 aumentos aprovados (+668) em Aqua/Fish/Sea Serpent, preservando literalmente todas as demais entradas Vanilla. Crab Turtle #710 **já era Vanilla peso 48**, preservado; Turtle Oath #692 apenas no pacote virtual de peso 30. Umi #334=72 e seis outras magias Vanilla permanecem. Relação completa no CSV de pools.
- **Pacote universal e IA preservados:** cada pacote continua com Ritual Magic + 3 materiais da receita ativa em quatro cartas físicas; Story/Free ON 1–3, Free OFF 0–3 (default OFF), 40 cartas reais, máximo 3 por ID. A camada de IA, as configurações de compra assistida, diagnósticos e comportamento pós-Ritual não foram reescritos. O adaptador privilegia providers/tabelas e só usa a receita Vanilla efetiva na ausência de alterações.
- A integração Female e a correção da contagem de Fish do Fortress Whale foram realizadas **em versão separada** do MOD Efeitos de Rituais, por se tratarem de efeitos já pertencentes a ele. Nenhuma fusão ou tipo principal foi alterado no Ritual Duelists.
- Validação: 20 suítes C e auditoria Python; 115.200 decks na simulação principal (16×3×2400), 23.400 no teste de receitas novas (3×2×3×1300), 90.000 na regressão de Rex. ELF32/i386 com símbolos do SDK verificados; ELF64/Windows compilado sem lista de exportações x64. Não foi realizado gameplay Windows.

---

# 14/10/2026 — v0.3.3-beta.2 — Ativação de Ritual legal (Bakura/Pegasus)

- Correção da prioridade universal: o jogo pode declarar o Ritual legal, mas Fiend's Mirror #365 e Psycho-Puppet #715 eram recusados quando a pontuação do monstro novo ficava abaixo da força do campo existente. A mesma pontuação era verificada novamente dentro do hook final da IA. Foram corrigidas **as duas verificações**.
- O motor **só solicita ativação após o checker real confirmar a legalidade** e haver Ritual Magic ainda disponível na zona de mágicas. Se a CPU já escolher um ataque letal, aquela jogada segue preservada; em emergência, uma invocação com pontuação negativa não é forçada.
- Regressão específica de Yami Bakura #14, Pegasus #15, Mage Soldier #18 e High Mage Martis #28. Cenários de legalidade falsa, magia ainda na mão, campo mais forte e emergência testados.
- Zero alterações nas 13 pools, receitas, Ritual Packages, Settings e nos demais MODs. 17 executáveis C + auditoria Python de pools; 93.600 decks gerados pela suíte, além de 90.000 no teste de raridade de Rex. ELF32 i386 validado pelo SDK v0.3.0; ELF64 Windows compilado, sem exportações x64 verificáveis pelo SDK.
- **Não equivale a confirmação no jogo Windows**: se a CPU continuar sem completar o Ritual, inspecionar o diagnóstico `Engine`, `Matched` e `Magic` para descobrir se o bloqueio restante é disponibilidade física de tributos ou execução do Ritual.

---

# 14/10/2026 — v0.3.3-beta.1 — Bakura, Pegasus, Mage Soldier

Base: **v0.3.2-beta.3 Rex Rarity**, preservada. Três novos perfis na mesma IA, sem alterar qualquer outro MOD. Cada pool: 2.018 pesos reais + pacote virtual 30 = **2.048**. Todas as cartas Vanilla preservadas, sem novas exclusões.

- **Yami Bakura #14:** 128→129 cartas reais; pronto Fiend's Mirror #365 peso 30, Beastry Mirror Ritual #674 exclusivamente pelo pacote 30. Cortes exatos: #493 64→59; #128 48→43; #111 72→67; #131 32→27; #362 32→27; #349 80→75; #106 48→43; #633 48→43; #170 32→27; #89 32→27; #542 32→27; #323 32→27.
- **Pegasus #15:** 100→101 cartas reais; pronto Psycho-Puppet #715 peso 30, Puppet Ritual #695 só no pacote 30. Cortes: #577 64→59; #382 48→43; #528 48→43; #357 48→43; #712 48→43; #42 48→43; #337 160→155; #668 120→115; #672 120→115; #329 120→115; #684 120→115; #690 120→115.
- **Mage Soldier #18:** 150→151 cartas reais; pronto Millennium Shield #362 peso 30, Curse of Millennium Shield #665 só no pacote 30. Cortes: #233 48→43; #25 42→37; #164 40→35; #281 40→35; #600 32→27; #171 24→19; #60 24→19; #64 24→19; #430 24→19; #363 24→19; #234 24→19; #647 24→19. Mágicas Vanilla #335=12, #341=24, #346=12 intactas.
- Materiais Vanilla **apenas como preferências**: Bakura [186,261,595], Pegasus [166,405,416], Mage Soldier [255,296,629]. Receitas efetivas continuam vindas dos providers/tabelas/Vanilla. Monstros prontos não são incluídos nos pacotes, e não recebem efeitos exclusivos de invocação Ritual apenas por terem sido comprados.
- Capacidade interna da cache ampliada de 10 para 13, sem tocar nos controles de Free Duel, IA, receitas e proteção pós-Ritual. `guaranteed_package_free` mantém default e fallback **OFF**.
- Pools auditadas contra **o próprio disco USA** (`WA_MRG.MRG`, 3 setores por duelista): contagem, soma, todos os IDs e pesos; diff dos dez perfis antigos intacto. Tabelas completas atualizadas no CSV existente (nome genérico).
- Testes: 16 conjuntos C + 1 auditoria Python, 93.600 decks no teste central de pacotes (13×3×2.400), além do teste Rex com 90.000 decks; ELF32 e ELF64 SDK v0.3.0 gerados. Não há execução de gameplay Windows neste ambiente. ELF64 não validado quanto a símbolos por ausência de exports x64 no SDK.

---

# HISTÓRICO — Ritual Duelists

Linha do tempo consolidada das versões. O histórico anterior continua abaixo.

## 14/10/2026 — v0.3.2-beta.3 — Peso da carta pronta de Rex

- Redução aprovada pelo usuário: **Tri-Horned Dragon pronto #705 no Rex: 15→7** (−8 pontos). Compensação operacional em Wasteland #331: 155→163 (+8), preservando **2018 pontos reais + Ritual Package virtual 30 = 2048** e todos os demais pesos.
- Nova distribuição do Rex: Dinosaur 1041 / outros monstros 565 / Wasteland + Equip 412 / pacote 30. O peso do pacote e a receita de Tri-Horned permanecem inalterados. Os demais nove perfis e os outros MODs não são alterados por esta mudança.
- A redução visa até aproximadamente 15% de decks com um Tri-Horned pronto; a frequência é medida por simulação de decks, não a frequência real de jogadas.

## 14/10/2026 — v0.3.2-beta.2 — Proteção pós-Ritual e Martis

- **Desert Mage / Dokurorider:** proteção do resultado de Ritual contra a escolha Vanilla de fundi-lo/substituí-lo por uma carta jogada da mão, **mesmo que o Ritual não tenha avaliador Beacon**. A proteção identifica o resultado no campo em qualquer posição, não depende do flag visual de face-down e redireciona a fusão para zona vazia, ou para a zona expendável menos valiosa. O fallback Vanilla continua se todas as 5 zonas estiverem protegidas (o jogo exige jogada válida). O mesmo mecanismo compartilhado se aplica aos outros ritualistas.
- **Martis / Rex:** quando o verdadeiro controlador confirma a legalidade do Ritual de Tri-Horned #705, a IA pode ativá-lo mesmo que outro monstro já tenha maior ATK nominal. Preservados gate legal real, proteção contra emergência e prioridade de ataque letal da rotina anterior. Não foi reimplementado nenhum efeito.
- **Rex:** peso pronto #705 continua **15**; o Ritual Package contém só #680, #6, #47, #571 com receita Vanilla, jamais o monstro resultado. Em 30.000 decks por modo, apareceu Tri-Horned pronto em Story 7295/30000 (24,32%), Free OFF 8619/30000 (28,73%) e Free ON 7388/30000 (24,63%). Porcentagens amostrais, não garantias de jogo.
- 15 conjuntos C + auditoria Python aprovados. Compilação oficial ELF32 e ELF64 Windows; ELF64 ainda sem lista de símbolos do SDK para conferência. Teste dentro do jogo ainda pendente.


## 14/10/2026 — v0.3.2-beta.1 — Rex, Martis, Desert e Time Wizard

- Adicionados **Rex Raptor (ID 9)**, **High Mage Martis (ID 28)** e **Desert Mage (ID 27)** ao mesmo motor de Ritual Package, busca de cartas e IA já aprovado. Nenhum outro MOD foi modificado.
- **Rex**: 26 cartas reais, 2018 pontos, mais pacote virtual 30; 14 Dinosaurs 1049, outros monstros 565, Wasteland/Equip 404. Peso de Tri-Horned #705=15, Red-Eyes #82=20, Bracchio #509=20. Foram adotados os pesos complementares *propostos operacionalmente* no prompt (eles não haviam sido votados individualmente): Wasteland #331=155, Raise Body Heat #326=249 e os dez monstros não-Dinosaur conforme CSV consolidado.
- **Martis**: 121 cartas reais (120 Vanilla + #705). Dez monstros de peso 8 passaram a 1, Dokurorider #719 40→50; Tri-Horned pronto 30, pacote 30. Todos os outros pesos Vanilla preservados.
- **Desert**: 57 cartas reais; Zombies 932, Dinosaurs 283, Rocks 219, seis Magics/Traps 584, pacote 30. Excluídos da pool normal #479, #485 e #699; incluídos #98=35 e Dokurorider #719=30. Pacotes obtêm materiais conforme receita efetiva, inclusive os que não participam do sorteio normal.
- **Weevil**: Time Wizard #16 com peso 25, nenhuma garantia. Para cumprir **os pesos finais explícitos do novo design sem duplicar cortes já feitos na v0.3.1**, os cinco alvos finais são: #50=57 (58→57), #54=49 (50→49), #397=49 (50→49), #476=39 (44→39), #477=39 (44→39), um total de −13 frente à Beta anterior. Para completar os −25 exigidos, −12 pontos foram compensados em três outros Insects não protegidos: #478 44→40, #479 44→40 e #485 44→40. **Esses três cortes são reconciliação operacional, não uma decisão votada carta a carta.** Pool final: 72 cartas reais, 2018 pontos mais pacote de peso 30. Javelin, Moths, Jirai, Hercules e Kwagar mantêm os pesos especiais anteriores.
- Criada uma única tabela consolidada `spec/Beta2-Pools-Complete.csv` com todas as cartas e pesos dos dez perfis. O histórico e os testes foram **atualizados nos documentos existentes**, sem documentação nova por versão.
- Construções ELF32 i386 e ELF64 x86_64-windows pelo SDK v0.3.0; o carregamento e o comportamento em jogo ainda devem ser confirmados.

## 13/10/2026 — v0.3.1-beta.1 — Ritual Package universal e Heishins

- Eliminado do fluxo ativo o antigo sorteio de dificuldade Fácil/Médio/Difícil e suas garantias por faixa; o deck usa a entrada **virtual** Ritual Package de peso 30.
- Cada pacote selecionado reserva **1 Ritual Magic + 3 materiais da receita ATIVA**, em 4 cartas físicas; a reserva é atômica, com limite de 3 cópias por ID e até 3 pacotes. Cartas reais têm peso total 2018, o pacote virtual 30, total 2048. Deck 40 e shuffle finais.
- Story sempre começa com um pacote. Free Duel padrão OFF pode ter 0–3 pacotes por seleção da pool. Toggle persistente **Guaranteed Ritual Package in Free Duel**, padrão OFF inclusive sem chave previamente salva, deixa Free equivalente a Story quando manualmente ativado.
- Incluídos **Heishin (ID 8)** e **Heishin 2nd (ID 35)**, Dark Magic Ritual #721 e Magician of Black Chaos #722. A geração própria do MOD considera os dois IDs até 722 e não depende da varredura da pool Vanilla para selecioná-los.
- A Isis passa a ter 84 entradas reais: Skull Guardian #704 incluído com peso 30, sem garantia; suas 10 reduções de 6 pontos cada constam abaixo. A IA e os efeitos especiais da Isis não foram alterados.
- Jono 1st, Jono 2nd, Weevil e Shadi: apenas os descontos pontuais de pesos aprovados, com cartas Ritual prontas preservadas e as mesmas variedades de cartas.
- Heishin 1st: removidos #714 e #711 (32 cada), adicionado #722 (30), Dark Magician #35 32→36; pacote 30.
- Heishin 2nd: removida apenas Cosmo Queen's Prayer #698 (64), adicionado #722 (30), Dark Magician #35 16→20; pacote 30. Os materiais secundários de Cosmo Queen permanecem na pool quando já estavam nela.
- Provedor de receitas e controle real de Ritual continuam com a recompilação/MODs ativos. Condições impossíveis e receitas que não tenham três requisitos usam fallback seguro para o Deck Vanilla, com registro da causa. O gerador de pacote não consome tributos diretamente do Deck durante a invocação.
- Diagnóstico adaptado: exibe a **quantidade real de pacotes** e, no log, o ID da magia e dos três materiais de cada pacote. A preferência de Free é definida exclusivamente por Settings do MOD.
- Testes nativos: 14 conjuntos (incluindo regressão das estratégias antigas), 54.600 decks amostrados nesta bateria para os 7 perfis e 3 condições Story/Free OFF/Free ON; nenhum deck amostrado superou 40 cartas ou 3 por ID. Ver TESTES.md.
- Build oficial SDK v0.3.0: ELF32 i386 aprovado com 40 símbolos externos autorizados; ELF64 Windows x64 compilou, sem lista externa de validação de exports no SDK distribuído. **Teste real em jogo ainda pendente**.

### Redistribuições aprovadas (peso antigo → novo)

- **Isis:** #303/#348/#349/#672: 90→84; #89/#223/#442/#443/#449: 80→74; #334: 50→44; #704: 0→30. 84 reais / 2018 + pacote 30.
- **Jono 1st:** #9/#192/#289/#344: 120→117; #387/#397/#411/#428/#484/#504/#547/#548/#635: 120→118. Hungry Burger #702 permanece 20.
- **Jono 2nd:** #12/#15/#43/#51/#61/#64/#68/#78/#82/#389: 72→69. Hungry Burger #702 permanece 70.
- **Weevil:** #50 62→58, #53 70→66, #54 54→50, #397 54→50, #116/#141 50→47, #273/#274 48→44. Javelin #717 50, Great Moth #57 10, Perfectly Ultimate #67 5, Jirai Gumo #367 48, Hercules #52 31 e Kwagar #533 21 preservados; 71 cartas reais (36 Insects).
- **Shadi:** #444/#524 132→129; #436 100→97; #152/#549 64→61; #130/#137 45→42; #202/#237/#485 40→37. Garma Sword #716 permanece 20; 118 cartas reais.

---


Linha do tempo consolidada das versões. A especificação original e a pool aprovada permanecem intactas em `spec/`.

## v0.1.0-alpha — Primeiro gerador híbrido
- Primeiro protótipo de Isis: três perfis, 40 cartas, máximo de 3 cópias; pool ponderada de 83 cartas e 2.048 pontos.
- Perfil sorteado por duelo: Story 15/50/35 e Free Duel 50/35/15 (Fácil/Médio/Difícil).
- Ritual Planner inicial; decisões e invocações reais ainda exigiam testes.

## v0.1.1-alpha — Receitas variáveis
- Separação entre seleção de tributos para montar o deck e mecanismo que realmente executa o Ritual.
- Adaptador de receitas com interface opcional `recipe_v1` prevista, seleção manual e fallback seguro.

## v0.1.2-alpha — Diagnóstico
- Painel Off/Compact/Detailed e mensagens de IA/geração do deck; IDs das garantias e origem da receita.
- Diagnóstico baseado nos snapshots dos hooks; não deveria consultar receitas durante cada desenho.

## v0.1.3-alpha — Desempenho
- Cache do painel por `overlay_signature` e da verificação de Ritual por estado físico de duelo.
- Diferenciação entre materiais locais e resposta do motor com fontes externas (inclusive Deck).
- Lentidão anterior reportada; sem medição independente do FPS no Windows.

## v0.1.4-alpha — Independência da IA
- Respeito ao planejador anterior na cadeia de hooks, às cartas da mão real e às que já agiram.
- Compatibilidade defensiva com Melhoria de Rituais, Dual Play e AI Hard Mode, sem alterá-los.
- Estimativa conservadora de efeito quando os materiais efetivos vêm de fontes que a IA não inspeciona.

## v0.2.0-alpha — SDK oficial v0.3.0
- Compilação i386 com SDK 0.3.0; reconhecimento de `Tables_RitualRule()` (1 a 5 tributos Campo/Mão/ambos).
- Suporte à resolução de IDs acima de #722 sem mudar a pool aprovada.
- Compartilhamento do avaliador de requisitos entre IA e gerador.
- Usuário confirmou gameplay fluido nos testes disponíveis; não era homologação de compatibilidade total.

## v0.2.1-alpha — Integração automática com Efeitos de Rituais (2026-10-11)
- Exportação `ritual-card-effects-test:recipe_v1` pela versão 0.78.39 do Efeitos de Rituais.
- Regras privadas de 18 receitas expostas sem alterar sua execução: condições, teto de ATK/DEF, Skull Monster e exclusões de Chakra, mais três receitas de alternativas.
- Respeita desligamento do Efeitos de Rituais e interruptores individuais. Quando nenhuma receita privada está ativa, usa a detecção oficial da v0.3.0 e a tabela Vanilla.
- ABI compartilhada no cabeçalho `ritual_recipe_provider_v1.h`; diagnóstico agora identifica `Efeitos de Rituais API` e o número de versão do painel é atualizado.
- Testes: compilação i386 dos dois MODs; 600 mil decks simulados, variantes do adaptador, fornecedor com 18 receitas e verificações de desligamento/fallback.
- PENDENTE: validar detecção e execução real no Windows com os dois MODs; testar AI Hard Mode, Dual Play, Melhoria de Rituais e save states em jogo.

## v0.2.2-alpha — Instrumentação de validação da Isis (2026-10-11)
- Diagnóstico aprimorado sem alterar a lógica de seleção, o gerador, a receita ou os efeitos.
- Nova classificação de Novox's Prayer na **mão física, back-row, janela lógica ampliada** e ausente. Janelas lógicas não são tratadas como mão física para decidir ativação.
- Separação explícita entre verificação de Ritual **não consultada** (`Engine: NOT CHECKED`), permitida e negada. Não cria consultas adicionais a `Duel_CheckRitual` para desenhar o painel.
- Registra origem das escolhas: seleção preservada da cadeia anterior versus pedido do Ritual Duelists. Mantém última escolha importante no overlay após avaliações posteriores.
- Registra o motivo da decisão e esclarece que pedidos de ativação não comprovam a invocação concluída; requer vídeo do jogo para esse ponto.
- Novo teste isolado da classificação de cartas físicas/lógicas; regressões existentes continuam cobrindo 600 mil decks, 15 regras oficiais e provedores privados.
- PENDENTE: jogo Windows com combinações dos MODs, validação independente da IA e implementação da opção global **IA Rouba?** (não incluída por alterar decisões, fora desta versão diagnóstica).

---

## Referência histórica complementar (documentos anteriores consolidados)

Os relatórios temporários abaixo foram reunidos aqui para não deixar arquivos de versões antigas na raiz. Eles registram decisões e roteiros históricos, que podem estar superados pelas instruções atuais do README e TESTES.

### COMPATIBILIDADE-IA.md

# Ritual Duelists v0.1.4 — auditoria de compatibilidade

## O que cada MOD controla

| MOD | Hook/entrada | Papel | Limite
| --- | --- | --- | --- |
| Ritual Duelists | `Duel_ShuffleDeck`, `AiScript_Run` | Monta decks apenas de Isis; adapta/prepara decisões da IA, encadeando a IA anterior | Não executa Ritual
| Efeitos de Rituais | Hooks próprios de efeitos/receitas | Faz Skull Guardian ter receita/efeitos personalizados | v0.78.38 ainda não fornece interface de consulta `recipe_v1`
| Melhoria dos Rituais 1.2.9 | `Duel_CheckRitual` e hooks de consumo de tributos | Habilita material de mão/deck com mínimos; valida legalidade efetiva | A implementação enviada é específica do formato atual de três tributos
| Dual Play 0.5.7 | Draw/Hand/Field, `func_80028220` e `func_8001BAF0` | Gerencia quota monstro + suporte e segunda mão da CPU | Ritual não pode ser usado na segunda jogada opcional da CPU
| AI Hard Mode 0.1.0 | `AiScript_Run`, `Ai_GetHandSize`, seleção de mão e campo | Decide jogadas e Fusions, pode conhecer uma janela lógica do Deck | Há decisões que dependem de configuração Deck access

## Independência e fronteiras

O Ritual Duelists funciona sem AI Hard Mode, Dual Play, Melhoria e Efeitos de Rituais, pois utiliza as regras Vanilla quando legíveis. Um MOD pode alterar a receita em tabelas (`Tables_RitualRequirements` / `Tables_Ritual`); o leitor do Ritual Duelists consulta essas tabelas uma vez ao gerar seu primeiro deck. Um MOD que usa apenas hooks privados não pode ser completamente introspectado. Quando necessário, `recipe_source=Force configured Ritual Effects recipe` é a configuração manual provisória, **apenas para montar deck**.

Na primeira invocação real, o executor de Ritual de outro MOD é soberano; Ritual Duelists apenas consulta `Duel_CheckRitual` e não impõe material nem efeito. Com Melhoria `use_deck=ON`, `min_field=0`, `min_hand=0`, o resultado do verificador pode ser positivo sem um conjunto completo no Campo/Mão. A v0.1.4 aceita essa distinção.

## Segurança das decisões de IA

- Só cartas entre os cinco slots reais da mão são jogáveis imediatamente, mesmo quando AI Hard Mode vê uma janela do Deck mais ampla.
- O resultado dos hooks anteriores é sempre solicitado. Se a IA anterior já escolheu o mesmo Ritual, Ritual Duelists mantém a seleção. Também conserva ataque aparentemente letal.
- A condição `Local match` não substitui `Engine legal`: a primeira auxilia a preservação e a estimativa de bônus, a segunda autoriza a decisão de invocação.
- Se os materiais consumidos provêm do Deck, não são estimados bônus de Skull Guardian com base em uma seleção hipotética; usa apenas uma avaliação conservadora do poder base.
- Um Ritual já marcado como usado no back-row não é forçado a ativar de novo.

## Desempenho

- O gerador roda uma vez por duelo; cache de requisitos/garantidas é mantido enquanto a instalação e Settings ativos não mudarem (mudanças exigem reinício).
- Não há varredura de receitas no callback gráfico. `overlay_signature` permite reaproveitar renderizações.
- IA executa a consulta de legalidade somente com Ritual Magic fisicamente acessível na mão ou colocada em back-row, e só quando o estado físico mudar. Uma impressão digital única é compartilhada por verificação e plano.
- As fontes de candidatos locais para planejar tributos continuam limitadas aos cinco monstros em campo + cinco cartas físicas da mão. Não percorre todas as combinações do Deck por conta própria; deixa essa consulta para o MOD de Rituais quando necessário.

## Limitações honestas

- Não existe ainda leitura padronizada das quotas internas do Dual Play; seu comportamento com Ritual Duelists em duas jogadas da CPU requer teste real.
- Não há teste de FPS ou de completa interação de hooks no Windows. Compilação e testes unitários não demonstram ausência de lag ou funcionamento de todos os cenários.
- Não temos como conhecer todos os tributos ocultos consumidos no Deck para computar exatamente Skull Guardian antes da invocação; o efeito final é calculado pelo MOD Efeitos.
- O módulo de planejamento é experimental e não instala suporte para quantidades alternativas de tributos.


### GUIA-TESTE-RAPIDO.txt

RITUAL DUELISTS — ISIS v0.1.4-alpha | TESTE COM DIAGNÓSTICO

1) Extraia 'Ritual Duelists' para a pasta mods e substitua a alpha anterior.
2) No menu Mods > Ritual Duelists:
     Isis — Hybrid Decks & AI: ON
     Isis — Ritual Planner: ON
     Isis diagnostic overlay: Detailed
     Log diagnostic events: ON
3) ATENÇÃO — TESTE VANILLA:
     Efeitos de Rituais: OFF
     Deck ritual recipe source: Auto
     Reinicie o jogo e inicie Free Duel contra Isis.
     Grave o primeiro painel; ele deve mostrar origem Vanilla/disc e perfil sorteado.
4) TESTE COM RECEITA MODIFICADA:
     Efeitos de Rituais: ON
     Deck ritual recipe source: Force configured Ritual Effects recipe
     Reinicie o jogo e inicie outro duelo contra Isis.
     O painel deve mostrar fonte Manual Ritual Effects, tributos=3.
     Nos IDs garantidos deverão aparecer #535 (Kamakiriman), #545 (Skelgon)
     e #378 (Swordstalker), na quantidade do perfil sorteado.
     NOTA: Auto AINDA não lê as receitas privadas do nosso Efeitos de Rituais
     até ele publicar a interface opcional recipe_v1.
5) Registre 1 a 3 turnos completos da Isis com overlay Detailed.
     Tire prints na preparação de Ritual, quando ela evita uma Fusion,
     quando tenta ativar Novox e em qualquer comportamento estranho.
6) Repita com AI Hard Mode ON/OFF. Teste também um save state.

ENVIE JUNTO COM O VÍDEO:
- Print do painel inicial da Isis
- Print do painel durante uma decisão suspeita
- Settings: recipe source, planner, AI Hard Mode, Efeitos de Rituais ON/OFF
- Se disponível, trecho do log com "RD DIAG"

LEGENDA:
- RETAIL FALLBACK = falha ao adaptar o deck; jogo recebeu deck original.
- HYBRID GENERATED = deck próprio da Isis criado com sucesso.
- Engine legal = saída do verificador de Ritual consultado naquele instante.
- Matching = há candidatos distintos para os materiais observados no planejamento.
- Save state = dados não recapturados (hash/garantias) NÃO são inventados.

O painel é apenas um diagnóstico; não modifica receitas nem garante invocação.


TESTE DE DESEMPENHO: siga o arquivo TESTE-DESEMPENHO.txt antes de continuar os testes de IA.

OBS v0.1.4: Local match=N + Engine legal=Y significa que o verificador
permitiu o Ritual (possivelmente materiais do Deck). Grave a ativacao,
se acontecer. Confira tambem AI quando o Ritual existe apenas na janela
logica do Deck: nao deve aparecer como jogada imediata da mao.

Para testar o Dual Play, use o roteiro ROTEIRO-v014.txt.


### MIGRACAO-v0.3.0.md

# Ritual Duelists v0.2.0-alpha — Migração para YFM Recompiled v0.3.0

## Base e escopo

Este pacote atualiza **apenas o Ritual Duelists** v0.1.4 para o SDK oficial
`yfm-redecomp-v0.3.0-windows.zip`, commit informado pelo pacote:
`663626b1d3`. Mantém `id: ritual-duelists` e pasta `Ritual Duelists`.
`min_api: 12` impede instalação silenciosa numa recompilação antiga.

Não altera decomp, jogo, FM Editor, Efeitos de Rituais, Melhoria de Rituais,
Dual Play nem AI Hard Mode. Nenhum duelista além de Isis está ativado.

## Alterações de código

1. **Fonte das receitas**: `recipe_adapter.c` agora consulta primeiro
   `Tables_RitualRule()` — API oficial v0.3.0 para receitas declaradas com
   **1–5 tributos**, requisitos condicionais e origem `FIELD`, `HAND` ou
   `BOTH`. Em seguida tenta `Tables_RitualRequirements()`, `Tables_Ritual()`
   e, por último e apenas quando permitido, a receita tradicional carregada
   no jogo. Receitas removidas não são indevidamente tratadas como Vanilla.
2. **Fonte independente opcional**: se outro MOD publicar a interface
   `ritual-card-effects-test:recipe_v1`, ela precede as tabelas no modo Auto.
   O Efeitos de Rituais v0.78.38 **ainda não publica** essa interface:
   para sua receita privada, usar `Force configured Ritual Effects recipe`.
   MODs que alterem receitas sem publicar regras continuam opacos.
3. **Mesma interpretação em deck e IA**: o planejador da Isis recebe
   opcionalmente `Rd_MatchRecipeCard`, usado também pelo gerador. O
   avaliador verifica cópias, grupos de Fusion oficiais, estatísticas
   impressas, níveis e `DEF > ATK`, sem confundir grupos oficiais com
   grupos privados da Isis. O planejador respeita Field-only/Hand-only
   na sua estimativa LOCAL.
4. **Cartas de MODs**: removidos os limites fixos `id <= 722` do
   gerador e da escolha de materiais. O limite físico do PC é `0x7FFE`;
   o resolvedor usa `Cards_Valid()`, e a busca de representantes percorre
   apenas `gCard_nCount` cartas carregadas. O gerador limpa apenas a faixa
   de IDs realmente usada naquele perfil, sem gastar 32 KB por duelo.
5. **Atributos do resultado**: o planejador lê ATK/DEF impressos da carta
   de resultado realmente ativa, não pressupõe os valores constantes da
   definição original quando um MOD os alterar.
6. **Cache e diagnóstico**: a receita continua sendo capturada uma vez
   por perfil carregado; criação e IA usam o cache existente. Nova origem
   visual `v0.3 rules (1-5)`; o número de tributos é mostrado sem assumir
   que são três. O painel não pesquisa receitas a cada frame.

## O que não mudou

- 83 cartas e todos os **2.048 pesos aprovados** da Isis, sem redistribuição.
- Cartas garantidas adicionais, perfis Fácil/Médio/Difícil e 40 cartas/3 cópias.
- O hook de `Duel_ShuffleDeck`, que continua existindo na v0.3.0.
- Responsabilidade pela **invocação**: o jogo ou MOD de Ritual decide
  se tributos e fontes são aceitos. Ritual Duelists apenas prepara o deck
  e oferece estratégia experimental, sem criar regra de execução.
- O `ai_planner` pode ser desligado para isolar o gerador nos testes.

## Compatibilidade e limitações

- **AI Hard Mode** e **Dual Play**: os hooks continuam encadeados pela API
  oficial (`host->hook` e chamada do planejador anterior). Não há garantia
  de ausência de conflito em todas as ordens ou duelos sem teste no Windows.
- **Melhoria de Rituais**: tributos somente no Deck podem ser válidos para
  o seu motor; não há esse local na API de receitas `Tables_RitualRule()`.
  Se o MOD publicar as regras por outro meio, é necessária integração
  específica. A decisão real do motor segue soberana.
- **Efeitos de Rituais**: o modo Auto só detecta regras visíveis à API
  pública ou por um provider disponibilizado. **Não foi modificado.**
- **Receitas mais de cinco tributos**: o formato interno de cache aceita
  quantidades maiores fornecidas por terceiros. O SDK v0.3.0 oficial publica
  somente 1–5 materiais via `Tables_RitualRule`.
- O planejador local vê cinco slots de mão e cinco monstros no campo;
  suas estimativas não reconstroem materiais que outro MOD tenha tomado do
  Deck. O verificador efetivo do jogo permanece autoridade final.

## Testes realizados neste ambiente

- Compilação do objeto `ritual-duelists.o` para **i386 (Windows/Linux 32-bit)**
  pelo `sdk/tools/build_mod.py` oficial da v0.3.0: **PASSOU**.
- 600.000 decks simulados, probabilidades e cópias e regras da Isis:
  **PASSOU**.
- Suíte do planejador para tributos distintos e casos com acesso externo:
  **PASSOU**.
- 0–9 materiais no gerador, 40 cartas/3 cópias, fallback inviável:
  **PASSOU**.
- Teste v0.3 dedicado com adaptador e stubs para API oficial:
  **PASSOU** (15 combinações de 1–5 materiais × Field/Hand/Both;
  IDs novos #850–854; Vanilla e receita desativada; correspondência de
  grupo oficial; estimativa por origem). Os stubs correspondem às
  assinaturas da API, **não** equivalem a executar o motor do jogo.

## Ainda não testado

- Gameplay Windows v0.3.0, renderização do painel, FPS e save states.
- Combinações reais ON/OFF com os três MODs externos.
- Rituais executados na v0.3.0 usando receitas declaradas de 1/2/4/5
  tributos ou obtidas de hooks privados.
- Instalação em versões 64-bit do executável: este pacote tem objeto i386
  para o Windows da distribuição fornecida, **não** contém build x64/Android.

## Recomendação de teste inicial

1. Faça uma cópia do seu save. Substitua a pasta `Ritual Duelists` e reinicie.
2. Apenas Ritual Duelists ON; `recipe_source=Auto`, overlay Detailed,
   `ai_planner=OFF`. Enfrente Isis e confira a fonte Vanilla e as garantias.
3. Depois ligue Efeitos de Rituais e selecione `Force configured Ritual
   Effects recipe` até termos o provider de integração. Confira #535/#545/#378.
4. Teste `ai_planner=ON` com Melhoria de Rituais, AI Hard Mode e Dual Play,
   **um de cada vez**, anotando o resultado e o painel.
5. Deixe testes de desempenho para depois da validação funcional,
   como combinado.


### ROTEIRO-v014.txt

RITUAL DUELISTS v0.1.4 ALPHA — TESTES FOCADOS

USE SEU SAVE NORMAL. Para cada mudanca de MODs ou receita, reinicie o JOGO.
Comece um duelo NOVO com Isis; nao carregue um save state ja dentro do duelo para testar o deck.

1 — INDEPENDENCIA (Vanilla)
  Ritual Duelists ON; Efeitos OFF; Melhoria OFF; AI Hard Mode OFF; Dual Play OFF.
  Deck ritual recipe source=Auto; Overlay=Detailed; Planner=ON.
  Verifique se aparece Vanilla/disc e cartas garantidas correspondentes.

2 — RECEITA MODIFICADA SEM ACESSO AO DECK
  Efeitos ON; Melhoria OFF; AI Hard Mode OFF; Dual Play OFF.
  Recipe source=Force configured Ritual Effects recipe.
  Veja os tributos e se a Isis prepara/usa o Ritual com os tres monstros no campo.

3 — TRIBUTOS EXCLUSIVOS DO DECK (seu cenario)
  Efeitos ON; Melhoria ON; use_deck=ON, min_field=0, min_hand=0.
  Dual Play OFF; AI Hard Mode OFF; Recipe source=manual.
  Grave o momento em que Local match=N e Engine legal=Y; veja se a IA ativa
  Novox sem tributos locais. Se ela nao fizer isso, registre a outra jogada.

4 — AI HARD MODE E DUAL PLAY (cada um, depois juntos)
  Repita 3 com AI Hard Mode ON, Dual Play OFF.
  Repita 3 com AI Hard Mode OFF, Dual Play ON.
  Por fim os dois ON. Grave qualquer segundo-play rejeitado.
  Dual Play nao permite Ritual na segunda jogada opcional da CPU.

5 — PERFORMANCE APENAS DEPOIS DE VERIFICAR O COMPORTAMENTO
  Se houver lentidao, compare: Planner OFF/Overlay OFF; Planner OFF/Detailed;
  Planner ON/Detailed, mantendo todos os outros MODs/settings iguais.

ENVIAR: um print inicial (perfil/deck/receita), outro do momento de Ritual,
video de 1-3 turnos, lista de MODs ON e Settings. Save state opcional
apenas para repeticao de uma jogada.

LIMITES: testes unitarios e compilacao passaram. Invocacao real, FPS e
compatibilidade completa ainda nao foram validados nesta Alpha.


### ROTEIRO-v020.txt

RITUAL DUELISTS v0.2.0-alpha | Recompiled v0.3.0
================================================
1. Faça backup do seu save e substitua a pasta Ritual Duelists.
2. Reinicie o jogo v0.3.0 (a Alpha exige a API nova).
3. Free Duel > Isis, seu save anterior pode ser usado.
4. Settings: recipe_source AUTO, AI planner OFF, diagnóstico DETAILED.
   Com outros MODs OFF, informe perfil, receita e garantias mostradas.
5. Efeitos de Rituais ON: por enquanto selecione FORCE CONFIGURED RITUAL
   EFFECTS RECIPE. AUTO ainda não detecta os hooks privados desse MOD.
6. Depois AI planner ON e, separadamente, Melhoria de Rituais,
   AI Hard Mode e Dual Play. Teste ON/OFF separadamente antes de combinar.
7. Envie um print do começo do duelo, um trecho da IA e observações sobre
   carta Ritual ativada, campo, mão, Deck e lentidão.
8. Receitas de 1, 2, 4 e 5 tributos declaradas nas tabelas da v0.3.0
   são aceitas pelo adaptador. Sua execução ainda depende do MOD de origem.
NOTA: Sem gameplay Windows aqui, esta versão é Alpha para testes.


### TESTE-DESEMPENHO.txt

RITUAL DUELISTS v0.1.4-alpha - TESTE DE DESEMPENHO

Use seu save antigo. Nao e necessario comecar jogo novo.
Instale/substitua a pasta 'Ritual Duelists' e reinicie o jogo.
Mantenha o MOD Melhoria dos Rituais com as MESMAS configuracoes
nos tres cenarios: Deck permitido, minimo de Campo 0, minimo de Mao 0.
Mantenha AI Hard Mode desligado e Dual Play desligado nesta comparacao.

1) TESTE A - somente gerador de decks
   Ritual Duelists: Isis Hybrid Decks ON
   Isis diagnostic overlay = OFF
   Isis Ritual Planner = OFF
   Free Duel -> Isis. Observe fluidez. Deck continua sendo gerado.

2) TESTE B - diagnostico isolado
   Isis diagnostic overlay = DETAILED
   Isis Ritual Planner = OFF
   Reinicie o duelo e observe a mesma fase.

3) TESTE C - diagnostico + IA
   Isis diagnostic overlay = DETAILED
   Isis Ritual Planner = ON
   Reinicie o duelo e observe a mesma fase.

Nos conte: qual dos tres testes apresenta a lentidao?
Um video de 15-30 s ja e suficiente. Envie print do painel se houver
informacoes estranhas e o log com 'RD DIAG' se disponivel.

OBS: Local match = materiais vistos pelo planejador em Campo/Mao.
Engine legal = permissao do verificador ativo, que pode usar o Deck.
Um 'Local match: N' e 'Engine legal: Y' pode ser normal quando os
tributos sao aceitos a partir do Deck pelo Melhoria dos Rituais.

Dual Play: deixe OFF ate estabilizarmos desempenho. Depois testaremos ON,
pois altera a quantidade de jogadas e pode mudar decisoes de IA.

Esta versao NAO altera Melhoria, Efeitos de Rituais, Dual Play nem motor original.
Nao foi testada ao vivo no Windows; exige validacao do jogador.


### VERIFICACAO-BUILD.txt

Ritual Duelists v0.2.0-alpha
YFM Recompiled SDK v0.3.0
Commit 663626b1d3
Target i386 (32-bit Windows/Linux)
ritual-duelists.o: SHA256 ddeea14ceeecb424fc88789c8143c2ad6ed8d15e6462b380b9a0a69ef33ff249
mod.json: SHA256 8076b3ac383c548ccb83e02cb6346593ae25b481d348afac16a422e18e90c4a5
recipe_adapter.c: SHA256 5731f1660869f60ad1f59c32f338b6c86a7e1bf746c63585eb41dd1fbac625ad
planner.c: SHA256 39bf681a88e14a426d571acda92226bbda0087a69064c7d09aafbaff5fa27725
decks.c: SHA256 ef457f6758fb97d7998fc96ad12dad7bf83e7f477783f6dba7f82061ce759471
AUTOMATED PASS: original 600000 decks, variable recipes and v0.3.0 API tests.
GAMEPLAY WINDOWS: NOT PERFORMED.
No changes to other mods or stock game.


## v0.2.3 Alpha — Estratégia Ritual-first (2026-10-12)

- Corrigida a preparação passiva: agora a Isis começa a montar a receita com zero ou um material, não apenas quando tem 2/3. O objetivo não expira após cinco turnos.
- Novox's Prayer é baixada antecipadamente quando está na mão física e há zona de suporte livre, mesmo sem Ritual imediatamente autorizado. Isso **não** significa ativá-la antes de `Duel_CheckRitual` permitir.
- Para receitas que permitem materiais do campo, prepara um tributo **válido de um requisito ainda ausente no campo**, mantendo contagem de cópias físicas distintas; com campo cheio não força uma jogada impossível.
- Quando o motor permite tributos externos (Melhoria de Rituais), a avaliação dos bônus ignora materiais consumíveis para não fabricar bônus; Summoned Skull (#22) conta como Skull Monster de suporte, mas não como tributo da receita modificada.
- Preserva monstros/materiais de Ritual na mão diante de fusões evitáveis, respeitando emergências. Nunca muda cartas do Deck do jogador nem a regra de execução dos Rituais.
- Diagnóstico: `Play missing tribute`, `Goal`, `Matched` e estimativa ATK/DEF pós-Ritual, com Score e Trigger.
- Eficiência: consulta ao motor somente com magia fisicamente acessível; não varre Deck em ciclos de planejamento sem magia jogável.
- Arquivos da pool original, probabilidades e perfis aprovados foram preservados; hooks dos MODs externos permanecem intocados.
- Limitações: não manipula a janela lógica extra da CPU (futura opção global **IA Rouba?**); não inventa zonas livres num campo lotado e não recalcula ataques do jogo base ou Equip. Requer validação Windows para confirmar a execução das escolhas.

**Automação local:** compila i386 via SDK v0.3.0, testes de 600 mil decks, 15 formatos oficiais, provider, diagnóstico e novos cenários de estratégia. **Gameplay desta Alpha não testado no Windows.**


## v0.2.4 Alpha — ponte optativa e proteção de preparação

O MOD passou a consultar uma nova API somente leitura `melhoria-dos-rituais:rules_v1` (Melhoria v1.3.3), distinguindo mínimo no campo, tributos da mão ou Deck e requisito sobre a carta Ritual. O overlay exibe a regra reconhecida. A preparação do Ritual foi refinada para evitar baixa precipitada e duplicação de papéis de material. O motor de invocação e as receitas não foram modificados. Este lançamento mantém somente Isis como perfil ativo e exige teste em Windows para confirmar comportamento em duelo.


### v0.2.5 Alpha — Field Tribute Guard
- Corrigida lacuna: `protect_field[]` era calculado mas ignorado ao aceitar a seleção de mão da IA anterior.
- Adicionado redirecionamento de monstro para zona vazia/não reservada, sem interferir em Equip, com logs e diagnóstico.
- Reservas redundantes são removidas segundo requisitos distintos, mínimo de campo e materiais físicos disponíveis.
- Validação técnica e testes de regressão com SDK v0.3.0; comportamento no jogo Windows aguarda confirmação.

## v0.2.6-alpha — Flag de face-down preservado nas decisões próprias (2026-10-12)

Problema: a Isis revelava Skull Monsters fracos no campo após o Ritual Duelists montar do zero a seleção de uma jogada de preparação. A causa localizada foi apagar o vetor de 12 bytes sem recolocar o valor 1 em `[8]`, responsável por solicitar face-down.

Correção mínima e reutilizável: `Rd_SelectFaceDownHandPlay` em `hand_play.c/.h` para as decisões próprias de baixar Ritual Magic, montar material de campo e preservar tributo ao escolher alternativa. Nenhuma mudança no comportamento de outras IAs, receitas, regras de origem, geração de decks ou no Melhoria de Rituais. Teste novo para impedir regressão, demais regressões executadas, rebuild de dois alvos Windows via SDK v0.3.0. Ainda não confirmado em runtime Windows.

## v0.2.7 Alpha — CPU busca Ritual Magic no Deck por padrão

Decisão de design: retirar a opção "IA Rouba?" e tornar a vantagem original de seleção de cartas futuras do Deck comportamento padrão do Ritual Duelists. A Isis procura sua magia de Ritual em todo o Deck embaralhado ainda não comprado, usa a seleção nativa e uma troca segura de carta física na mão, preservando tributos reservados. Nenhuma carta é criada, os tributos diretamente do Deck continuam regidos pelo Melhoria e o ritual permanece autorizado pelo verificador real do jogo. Esta etapa não generaliza ainda a busca automática de todos os monstros de tributo.

A janela padrão de acesso do AI Script também é ampliada até 20 cartas reais (limitada pelas cartas restantes), sem afetar outros duelistas sem perfil de Ritual.

## v0.2.8 Alpha — busca ativa de tributos no Deck (2026-10-12)

- O Ritual Duelists mantém o acesso padrão da CPU ao Deck real e agora usa esse acesso para **materiais específicos do Ritual**, além de Novox's Prayer. Usa o perfil/receita ativa e a comparação por requisitos, não IDs fixos de monstros da Isis.
- Com `rules_v1` ativo, distingue os requisitos físicos de **Campo**, **Mão** e a permissão de utilizar o terceiro material diretamente do **Deck**. Com tributos do Deck desligados, o planejador procura obter os três materiais físicos antes de pedir a ativação; com Deck ligado, procura somente as posições físicas obrigatórias. `Duel_CheckRitual` continua autorizando toda invocação.
- **FETCH_FIELD**: procura material ausente no Deck embaralhado, troca por uma carta descartável da mão e solicita a jogada face-down em zona de monstro vazia, se seguro. **FETCH_HOLD**: troca o material por outra carta da mão não selecionada nem protegida; a carta recuperada **permanece na mão física**, enquanto a jogada anterior segura é executada.
- Evita duplicatas que não cobrem outro requisito, preserva tributos e a carta Ritual resultante quando exigida fisicamente, evita exposição contra ataques revelados superiores e se abstém sem alternativa segura.
- `adaptive_rules.c` não esvazia a última exigência de tributo na mão para satisfazer o mínimo no campo quando o Deck pode fornecer um monstro alternativo. Registra novos diagnósticos `Play tribute from Deck` e `Keep fetched tribute in Hand`.
- Não altera o Melhoria de Rituais v1.3.3 nem o Efeitos de Rituais. Não altera deck do jogador, não cria cartas, não muda regras de tributos e não instala qualquer toggle `IA Rouba?`.
- A busca de materiais especificamente adaptativa usa a ponte `rules_v1` de três tributos; receitas de outros tamanhos continuam no planejador existente, sem fingir que o sistema antigo possui as novas opções.
- Verificações: testes puros (combinações Campo/Mão/Deck, papéis distintos, hand/field, Settings, reserva, resultado, segurança, ausências), regressões legadas, build SDK v0.3.0 Windows i386/x86_64. Falta validar dentro do jogo, principalmente swaps que deixam o monstro na mão.

### v0.2.9-alpha — Post-Ritual Strategy
- Implementado planejador genérico orientado a mudanças de Beacon (comparação dos estados antes/depois), reconhecendo um Ritual efetivamente invocado.
- Isis valoriza a entrada de Summoned Skull ou de outros Skull Monsters, preservando o Skull Guardian e ponderando riscos e bônus.
- Se necessário, busca suporte já existente no Deck com troca nativa, sem criar cartas; seleção de apoio na mão é prioritária.
- Adicionados testes `test_post_ritual.c`, builds de Windows i386 e x86_64, manutenção dos 11 testes de regressão da v0.2.8.
- Teste efetivo em jogo Windows ainda não realizado por este ambiente.


### v0.3.3-beta.3 — preparação do Bakura (15/10/2026)
- **Yami Bakura / Fiend's Mirror:** com a Ritual Magic já baixada e ao menos dois materiais físicos diferentes identificados, permite preparar tributo obrigatório em campo virado para baixo sob pressão adversária **não letal**. Antes, a condição de emergência suprimia toda a preparação mesmo quando o Ritual estava prestes a ficar legal.
- Permite a execução do Fiend's Mirror legal nesse mesmo cenário, mantendo a verificação obrigatória do `Duel_CheckRitual` e sem sobrescrever ataque letal já escolhido pela IA.
- Não modifica receitas, pesos, deck, package, efeitos das cartas ou outros duelistas. **A causa específica das partidas reportadas não foi observada no executável Windows**: esta alteração corrige um bloqueio comprovado em teste de planejador, mas requer confirmação no jogo.


### v0.3.5-beta.6 — Rodada V3: Labyrinth, Sebek, Neku e restauração Isis (17/10/2026)
- Incluídos perfis #31 Gate Guardian, #33 Skull Guardian e #34 Zera the Mant, com pools exatamente iguais ao anexo aprovado (97/76/87 entradas, 2018 pontos reais + 30 virtuais por perfil).
- Guardian Sebek mantém máquinas, Fiends/Spellcasters e mágicas Vanilla, incorpora os 21 Skull Monsters oficiais, Red-Eyes, Summoned Skull e B. Skull Dragon; nem Yami nem Kamakiriman entram no sorteio normal.
- Isis #16 retorna ao comportamento integral Vanilla: pool 101 entradas/2048 pontos reais, sem Ritual Package, sem IA ou garantia de Ritual Duelists. A posição histórica 0 do perfil é mantida somente para auditoria e estabilidade dos demais índices.
- A fonte da receita continua sendo exclusivamente o jogo e os provedores de efeitos/rituais; nenhum efeito, tributo, drop ou outro MOD foi modificado.
- Validação por checksums canônicos das quatro pools, testes de geração e build SDK Windows v0.3.0. Confirmação de runtime no jogo ainda pendente.

## v0.3.5 Beta.7 — V3 Seto 3rd, DarkNite e Nitemare (2026-10-17)

- **Seto 3rd #36:** Harpie's Feather Duster #672: 160→130; pacote virtual de Ultimate Dragon #675/#380 peso 30. O Ultimate pronto #380 permanece com peso 80 e Blue-Eyes #1 com peso 120. Um pacote reserva exatamente três Blue-Eyes #1 e uma Magic #675; o limite de três cópias impede um segundo pacote completo ou Blue-Eyes adicionais no mesmo deck.
- **DarkNite #37:** Forest #330, Wasteland #331, Mountain #332, Sogen #333 e Umi #334: 80→74, sem alterar Yami #335 (80). Chakra #709 não entra pronto, e Resurrection of Chakra #694 só aparece via pacote virtual peso 30.
- **Nitemare #38:** Boulder Tortoise #518, Tyhone #2 #555, Megazowler #79, Gyakutenno Megami #90, Pumpking #99 e Sky Dragon #424: 8→3. Chakra pronto #709 permanece em 16; Ritual Magic #694 apenas via pacote. Pool normal continua 100% monstros.
- Cada pool aprovada soma **2018 pontos reais + 30 virtuais = 2048**, com **63/91/70** entradas reais respectivamente; os checksums exatos dos anexos foram auditados.
- **Receitas dinâmicas:** Chakra consulta o provedor ativo, com Wall Shadow + dois Fiends de nível 7+ (sem Chakra; sem teto genérico de ATK/DEF), ou a receita Vanilla se a personalização estiver desativada. Não houve codificação de efeitos nem alteração em outros MODs.
- Preservados os 25 registros anteriores (incluindo Isis integralmente Vanilla), todos os seus pesos e a geração existente em Story/Free Duel. Os caches internos foram ampliados de 25 para 28 entradas. Drops, mecânicas de efeitos e receitas não foram modificados.
- Compilado e verificado tecnicamente com o SDK Windows v0.3.0; o comportamento runtime do jogo Windows ainda precisa de confirmação do usuário.


### v0.4.0-alpha.1 — 2026-10-18
- Integração da UI Work v0.2.5, starter 33 Vanilla + 7 ritual, receitas Vanilla verificadas no disco, Simon com um de dois temas restantes, IA reaproveitada, persistência v3.


## v0.4.0-alpha.6 — Drop List 1.1, visual egípcio (2026-10-18)

- Substituído somente o desenho/fluxo de navegação do antigo painel de Free Duel por interface inspirada no Drop List v1.1.0 de PauloH-Gudang, autorizado: moldura dourada egípcia, cantos com Ankh/Olho de Hórus, três abas, barras proporcionais de chance, cursor destacado, scroll centralizado, L2/R2 página e rodapé com ícones. Imagens de referência preservadas em `assets/ui` e incorporadas ao renderizador de retângulos do SDK; não foram importados binários de outro MOD.
- **Triângulo abre e fecha**; Círculo continua sem ação sobre o painel e não sai do Free Duel enquanto ele estiver aberto. L1/R1, Quadrado e X mantêm as ações existentes. A janela só é desenhada quando aberta; layout se adapta à resolução e reserva margem inferior para controles touch.
- Permanecem intactas as regras de drops dos Pacotes 01, 02 e 03 e o filtro dinâmico de fonte esgotada de Blue-Eyes, a persistência por save, o seletor inicial, o Simon, a IA, os 29 perfis de deck, os manifests de drops e os outros MODs.
- Testes puros e regressões com SDK Windows, builds i386/x86_64-windows. Android não gerado neste ambiente porque o SDK disponível não contém `memories-guest-ir`. Teste de jogo Windows/Android ainda pendente.
