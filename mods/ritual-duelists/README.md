# Ritual Duelists — v0.4.0 Alpha.8

MOD independente para **Yu-Gi-Oh! Forbidden Memories Recompiled v0.3.0**.
Vinte e oito duelistas com Ritual Package. **Isis foi restaurada ao deck e à IA Vanilla** e não é mais gerenciada pelo MOD. A tabela mantém 28 posições para preservar os índices internos e auditar sua pool original.


## Alpha.8 — Visualizador de drops delegado ao MOD Droplist (2026-11-06)

A interface interna do Free Duel foi **removida**. A consulta via Triângulo agora pertence exclusivamente ao MOD independente **Droplist 1.4.0 de PauloH-Gudang**, que permanece sem alterações. O Ritual Duelists não captura inputs do Free Duel, não desenha o modal nem oferece `drop_list_enabled`. `Drop Missing Cards` continua habilitado antes do Ritual Duelists para compor as pools oficiais/expandidas. A versão externa consulta `Tables_PoolFor` para ler as tabelas finais; sua interface não participa do sorteio.

**Preservado:** conteúdo integral do campo `drops` (Pacotes 01/02/03), premiações especiais de História, limites e persistência dos Blue-Eyes de três fontes, decks, Ritual Packages, IA, seletor inicial, diagnóstico de rituais e savestates. O limite de uma cópia por fonte opera no sorteio/entrega, não pela tabela ponderada: o Droplist binário pode continuar exibindo a chance base de Blue-Eyes mesmo quando aquela fonte já foi esgotada. Isso NÃO significa que o Blue-Eyes poderá cair novamente. Uma eventual exibição dinâmica requer suporte no próprio Droplist externo, a ser combinado com seu autor; não alterar os binários dele unilateralmente.

**Instalação:** apenas substituir a pasta `Ritual Duelists`. O MOD externo `droplist` instala-se separadamente. Nunca mesclar as duas pastas. Teste com e sem Droplist, mais `Drop Missing Cards` ativo, e confira Triângulo, pools e recompensas.

## Alpha.7 — Pool Mountain Mage (2026-10-19)

No **deck** do Mountain Mage (#25), Gaia the Dragon Champion #37 (peso 144) foi substituído por **Seiyaryu #358**, no **mesmo peso 144**. Atenza (#26) mantém Gaia #37 com peso 32. O restante do deck Mountain (incluindo pacote ritual Yamadron) e **todas as drop pools dos Pacotes 01–03** continuam iguais. A interface de consulta dos drops foi posteriormente removida na Alpha.8.


## Pacote 03 — Drops e recompensas especiais (v0.4.0 Alpha.5)

- **Free Duel:** as tabelas de POW, B/C/D e TEC continuam ponderadas e disponíveis para o Droplist externo, já considerando Drop Missing Cards e os Pacotes 01/02. Para **Heishin 2nd**, Dark Magician #35 está em **S/A-POW peso 32** e **B/C/D peso 15**, conforme a última aprovação (Meteor B. Dragon #713 em POW mantém peso 4).
- **Blue-Eyes White Dragon:** Seto #7 peso10 em POW, Kaiba #17 peso16, Seto 2nd #32 peso25: cada um pode premiar #1 uma única vez por save. Seto 3rd segue ilimitado com peso26 em POW e peso20 em B/C/D. Os indicadores não dependem do inventário e são salvos por token de slot na área de dados do MOD, além dos savestates.
- **Modo História:** uma carta **especial garantida por vitória elegível**, exclusivamente na posição principal SPOILS; extras do menu Game > Card drops continuam normais. Labyrinth, Sebek, Neku, Heishin 2nd e Seto 3rd seguem a matriz `audit_story_rewards_03.csv`. DarkNite só tem TEC garantido (#709, monstro Chakra); Nitemare não ganha garantia.
- **Visualização (Alpha.8):** o MOD externo Droplist 1.4.0 é responsável por toda a UI. A regra de Blue-Eyes esgotado continua no sorteio/premiação do Ritual Duelists, mas a lista externa pode exibir sua chance base até receber suporte a essa informação dinâmica.
- Arquivos de auditoria deste pacote: `audit_drops_03.csv` e `audit_story_rewards_03.csv`. Nada foi alterado em decks, IA, efeitos, fusões ou MODs externos.
- **Observação de compatibilidade:** build local feito com SDK v0.3.1-preview.1, usando APIs de v0.3.0 (min_api 12); o runtime Windows v0.3.0 precisa ser verificado pelo jogador antes de considerar a atualização estável. Faça backup do MOD e do save antes do teste.

## Pacote 01 — Drops dos três rituais iniciais (v0.4.0 Alpha.3)

**Pré-requisito de balanceamento:** manter o MOD independente `drop-missing-cards` v1.0 **ativado**. A ordem de carregamento é `Drop Missing Cards` antes de `Ritual Duelists`, por meio da diretiva `after` no `mod.json`. O MOD amigo não foi modificado. O Droplist externo é o único visualizador e consulta as pools finais por `Tables_PoolFor`.

- **S/A-POW:** monstros prontos Yamadron #357, Performance of Sword #701 e Mask of Shine & Dark #720 — cada um com peso 16 em Simon Muran, 32 em Duel Master K, e peso 96 respectivamente em Mountain Mage, Mai Valentine e Shadi. Preservados explicitamente os drops de Pegasus #357 (30), Isis #701 (30) e High Mage Martis #720 (20) de Drop Missing Cards.
- **B/C/D:** preservados os pesos preexistentes de tributos; Ritual Magics #666/#676/#693 excluídas.
- **S/A-TEC:** somente Simon Muran (cada Ritual Magic 16), Duel Master K (cada 32), e Mountain Mage (#666=64), Mai (#676=64), Shadi (#693=64). Nenhum desses rituais mágicos pode ser obtido nas pools de outros duelistas.
- **Exclusividade dos 9 tributos:** #29, #31, #101, #249, #395, #511, #102, #182 e #220 ficam indisponíveis em S/A-POW e S/A-TEC de todos os duelistas, sem remover suas fontes B/C/D.
- Overrides declarativos de drops com pesos absolutos, sem `replace:true`. O SDK distribui os pontos restantes entre as demais cartas para somar 2048 por pool.
- **Alpha.8:** o Ritual Duelists não captura Triângulo/Círculo no Free Duel; a navegação é exclusivamente do MOD Droplist externo.

## Free Duel — consulta de drops (Alpha.8)

O visualizador embutido das Alphas.2–.7 foi **retirado** para evitar sobreposição com o MOD **Droplist 1.4.0 de PauloH-Gudang**. O botão Triângulo e todo o desenho da lista ficam sob responsabilidade do MOD amigo. O Ritual Duelists mantém somente os dados de recompensa e a mecânica que os produz, incluindo os limites de Blue-Eyes. A visualização das chances finais usa as tabelas compostas do jogo; ver limitação dinâmica descrita na abertura deste README.


## v0.3.5 Beta.7 — V3 Seto 3rd, DarkNite e Nitemare (2026-10-17)

- **Seto 3rd #36:** Harpie's Feather Duster #672: 160→130; pacote virtual de Ultimate Dragon #675/#380 peso 30. O Ultimate pronto #380 permanece com peso 80 e Blue-Eyes #1 com peso 120. Um pacote reserva exatamente três Blue-Eyes #1 e uma Magic #675; o limite de três cópias impede um segundo pacote completo ou Blue-Eyes adicionais no mesmo deck.
- **DarkNite #37:** Forest #330, Wasteland #331, Mountain #332, Sogen #333 e Umi #334: 80→74, sem alterar Yami #335 (80). Chakra #709 não entra pronto, e Resurrection of Chakra #694 só aparece via pacote virtual peso 30.
- **Nitemare #38:** Boulder Tortoise #518, Tyhone #2 #555, Megazowler #79, Gyakutenno Megami #90, Pumpking #99 e Sky Dragon #424: 8→3. Chakra pronto #709 permanece em 16; Ritual Magic #694 apenas via pacote. Pool normal continua 100% monstros.
- Cada pool aprovada soma **2018 pontos reais + 30 virtuais = 2048**, com **63/91/70** entradas reais respectivamente; os checksums exatos dos anexos foram auditados.
- **Receitas dinâmicas:** Chakra consulta o provedor ativo, com Wall Shadow + dois Fiends de nível 7+ (sem Chakra; sem teto genérico de ATK/DEF), ou a receita Vanilla se a personalização estiver desativada. Não houve codificação de efeitos nem alteração em outros MODs.
- Preservados os 25 registros anteriores (incluindo Isis integralmente Vanilla), todos os seus pesos e a geração existente em Story/Free Duel. Os caches internos foram ampliados de 25 para 28 entradas. Drops, mecânicas de efeitos e receitas não foram modificados.
- Compilado e verificado tecnicamente com o SDK Windows v0.3.0; o comportamento runtime do jogo Windows ainda precisa de confirmação do usuário.

## v0.3.5 Beta.6 — Labyrinth, Sebek, Neku e Isis Vanilla (2026-10-17)

- **Labyrinth Mage #31**: Gate Guardian #374 pronto com peso 30; Ritual Magic #667 apenas no pacote virtual de 30. Pool final: 97 entradas reais, soma 2018.
- **Guardian Sebek #33**: Skull Guardian #704 pronto com peso 30; Ritual Magic #679 apenas pelo pacote virtual de 30. Pool final: 76 entradas reais, soma 2018. Máquinas e outras cartas preservadas conforme anexos; os 21 IDs Skull são conferidos com `skull_monster.h` de Efeitos de Rituais. **Kamakiriman #535 e Yami #335 não estão na pool real.**
- **Guardian Neku #34**: Zera the Mant #360 pronto com peso 30; Ritual Magic #671 apenas no pacote virtual de 30. Pool final: 87 entradas reais, soma 2018. Yami #335 mantém peso 160.
- **Isis #16**: sem Ritual Package, sem planejamento de ritual nem garantia de deck pelo Ritual Duelists; recebe o deck pela rotina Vanilla do jogo. Pool de referência: 101 entradas, 2048 pontos reais, sem peso virtual. O setting exclusivo de Isis foi retirado; configurações gerais seguem iguais.
- Checksums das quatro pools idênticos ao prompt aprovado. Os outros 21 perfis de pacote foram preservados byte a byte. **Nenhum drop, efeito, receita, fusão ou outro MOD foi alterado.** As entradas anteriores deste documento são histórico, não regras atuais que se sobrepõem a esta versão.

## v0.3.5 Beta.5 — V3 Atenza, Meadow, Kepura (2026-10-17)

Somente três perfis novos, com pools praticamente Vanilla: **Atenza #26** (#680 8→0, #327 72→50; Serpent Night Dragon pronto #706 peso 32), **Meadow #29** (#653 128→98; Hamburger Recipe #677 peso 40) e **Kepura #30** (#697 32→0, #302 120→92, Black Luster Soldier pronto #364 peso 30). Pacote virtual 30 em cada, com magias #691/#678/#670 apenas nos pacotes. Os demais 19 perfis e seus pesos não foram alterados. As três pools têm respectivamente **148/70/165** cartas reais e **2018+30=2048** pontos. Receitas sempre consultadas em runtime; nenhum efeito ou drop alterado.

## v0.3.5 Beta.4 — Planejamento por capacidades reais dos efeitos (2026-10-17)

- Integração **opcional** `ritual-card-effects-test:effect_intelligence_v1` disponibilizada por Efeitos de Rituais v0.78.53. O MOD continua carregando sem ela.
- **Skull Guardian:** a previsão de bônus de ATK/DEF, exclusão dos tributos consumidos e avaliação de suporte da Isis vêm agora do MOD de Efeitos; quando ele ou o efeito está desligado, não há bônus imaginário.
- **Outros rituais:** Efeitos de Rituais informa bônus táticos moderados e apenas para efeitos que estão realmente ligados. Ritual Duelists usa essas informações para decidir como preparar/invocar, nunca executa o efeito.
- A consulta usa o **resultado da receita ativa** (inclusive resultado modificado por outro MOD), não um ID antigo fixado no perfil.
- A ativação das mágicas do Yamadron continua em Efeitos de Rituais. A proteção de Darkfire contra uso em fusões pela CPU continua no MOD Balanceamento, sem alteração desta versão.
- Modos: somente Ritual Duelists = planner de rituais sem vantagens de efeitos inexistentes; somente Efeitos de Rituais = efeitos e uso das mágicas ativos, sem perfis do Ritual Duelists; juntos = planejamento informado pelos efeitos; ambos desligados = Vanilla.

## v0.3.5 Beta.3 — Migração da IA específica do Yamadron (2026-10-17)

- A ativação automática de Final Flame/Ookazi/Tremendous Fire geradas pelo Yamadron saiu de `rd_ai()` e `ai_policy.c`; pertence agora ao MOD **Efeitos de Rituais v0.78.52**. Não exige `ai_planner`, perfil de duelista nem Ritual Duelists instalado.
- Este MOD continua com perfis de 19 duelistas, pacotes de rituais, busca de materiais e política geral de invocação; pesos e receitas preservados.
- **Registro histórico:** nesta Beta.3, a estimativa do Skull Guardian ainda residia em `profiles.c`. Ela foi transferida na Beta.4 acima; este item registra o estado anterior.
- Testes de unidade (23 suítes + auditoria) e build i386/x64 Windows; validar com ambos os MODs ativos e depois só Efeitos de Rituais.

## Instalação

Substitua apenas a pasta `Ritual Duelists` em `mods/`, sem misturá-la com a pasta `zzz Efeitos Rituais` ou Melhoria de Rituais. Ative o MOD e reinicie o jogo. Mantenha os dois MODs de receitas/efeitos instalados se desejar que o Ritual Duelists identifique os requisitos personalizados; este MOD não implementa nem altera os efeitos de Ritual.

## Correção v0.3.5 Beta.2 — IA: invocação ritual e Yamadron

- **Proibição universal de fusão pela CPU:** ao concluir uma invocação **por Ritual**, a IA registra o ID e a origem da carta no campo. Enquanto essa mesma instância estiver presente, `Duel_CheckFusion` rejeita pares envolvendo seu ID para os duelistas gerenciados pelo MOD. A trava atua **na escolha e na execução**, é independente de bônus/Beacon, não altera fusões do jogador e não se aplica a um monstro apenas comprado pronto. **Limitação deliberada do motor:** o verificador Vanilla recebe somente IDs, não instâncias; enquanto existir uma cópia ritualizada, outras cópias do mesmo ID da CPU também deixam de ser materiais de fusão.
- **Yamadron:** na fase de campo, antes da seleção normal, a IA tenta ativar primeiro toda `Final Flame #345`, `Ookazi #346` ou `Tremendous Fire #347` gerada pelo efeito ritualístico, detectada pela origem especial `deck_index = -1` e pelas flags de Magic ocupada/virada para baixo. Não consome cópias comuns dessas mágicas provenientes do Deck. A ativação usa a seleção padrão de magia na zona M/T e mantém a cadeia de outros MODs.
- **Sem alterações de pools, drops, receitas, efeitos, Settings de Ritual, outros duelistas ou MODs independentes.** Os 19 perfis mantêm 2018 pontos reais + 30 virtuais, total 2048.
- **Save states:** o único bloco de estado persistente do MOD foi ampliado (versão 2) para armazenar a identidade dos rituais. Saves antigos podem não restaurar os novos marcadores; para validar a proteção, comece um duelo novo com esta versão. Nenhuma alteração no estado do jogo/base é necessária.
- **Validação:** 23 suítes C e auditoria das 19 pools; builds ELF32 e ELF64 pelo SDK Windows v0.3.0. Não executado no jogo Windows.

## Novidades v0.3.5 Beta.1 — Forest Mage, Anubisius e Mountain Mage

- **Forest Mage #23:** Super War-lion #356 pronto peso 30, magia War-lion Ritual #673 somente no pacote de 30. Pool final **83 cartas reais**, com Leogun e Garvas 40, Firewing Pegasus 48 e seis exclusões Vanilla exatas.
- **High Mage Anubisius #24:** Javelin Beetle #717 pronto peso **60**, Javelin Beetle Pact #696 no pacote. Pool final **162 cartas reais**; Great Moth 64, Cocoon 50, Time Wizard 40 (carta comum, não material do Ritual Package), Hercules Beetle 50, Kwagar Hercules 60. Sengenjin não é Ritual deste duelista.
- **Mountain Mage #25:** Yamadron #357 pronto peso **16**, Yamadron Ritual #666 apenas no pacote. Pool final **60 cartas reais**; Gaia the Dragon Champion #37 peso **144**, Gaia the Fierce Knight #38 peso 88, Curse of Dragon #39 peso 96, Darkfire Dragon #168 peso 128 e Meteor B. Dragon #713 peso **15**.
- **19 perfis:** cada pool tem **2018 pontos reais + 30 virtuais = 2048**. Nenhum peso dos 16 perfis anteriores mudou. Um limite interno de perfis foi ampliado de 16 para 19; o gerador e a IA continuam os existentes.
- **Escopo exclusivo:** apenas pools/decks e sua configuração de perfis. **Nenhum drop foi alterado.** Os futuros efeitos de Gaia the Dragon Champion e Darkfire Dragon NÃO foram implementados; Efeitos de Rituais, Melhoria de Rituais e demais MODs permanecem intactos.
- **Validação:** 22 suítes C, auditoria independente das três listas positivas do anexo e preservação byte a byte das 16 pools anteriores, builds ELF32 e ELF64 pelo SDK v0.3.0. O jogo Windows ainda precisa de teste manual.

## Novidades v0.3.4 Beta.1 — Teana 2nd, Ocean Mage e Secmeton

- Teana 2nd #20: **69 entradas reais**, Cosmo Queen #708 pronta peso 30, magic #698 só pelo pacote 30, exatamente a pool positiva aprovada sem Harpies/Fields.
- Ocean Mage #21: **78 entradas reais**, Fortress Whale #718 pronta peso 30, magic #700 apenas pacote 30; preservados Umi, Swords e Eternal Draught Vanilla.
- High Mage Secmeton #22: **90 entradas reais**, Crab Turtle #710 permanece pronta peso **48** (Vanilla), magic #692 apenas no pacote 30; as 55 remoções e 20 aumentos aprovados estão documentados no CSV completo e histórico.
- Regras universais de IA, geração e Settings mantidas. Todos os 16 perfis somam **2018 reais + 30 virtuais = 2048**. Nome de registro no jogo de Tea 2nd: **Teana 2nd**.
- Para os efeitos especiais corretos de Cosmo Queen e Fortress Whale, instalar **Efeitos de Rituais v0.78.51** separadamente. Este MOD de decks NÃO altera efeitos de Ritual nem tipos/fusões Vanilla.
- A v0.3.3 Beta.3 anterior foi preservada integralmente para os treze primeiros perfis. Os testes são automatizados, não equivalem a gameplay Windows.

## Correção v0.3.3 Beta.2 — Ritual legal que não era ativado

Corrigido um impasse na IA: Fiend's Mirror (Bakura) e Psycho-Puppet (Pegasus) podiam ficar com a Ritual Magic preparada, mas a invocação era recusada porque seu ATK/DEF impresso parecia menos vantajoso do que a força dos monstros existentes. Além do teste da legalidade verdadeira, o código filtrava duas vezes por pontuação, inclusive anulando a exceção antiga do Tri-Horned. Agora, com **Ritual autorizado pelo jogo e Ritual Magic já baixada**, a IA não bloqueia a invocação apenas pela comparação de atributos. A proteção já existente para um ataque letal escolhido pela IA permanece, assim como a priorização de sobrevivência em situações urgentes.

Não foram alteradas receitas, pesos, decks, providers, Ritual Packages ou MODs externos. A correção foi testada em componentes e compilada, mas **a invocação real ainda precisa ser confirmada em uma partida Windows**.

## Como funcionam os Ritual Packages

Cada duelista gerenciado pelo MOD tem **2018 pontos em cartas reais + 30 pontos de uma entrada virtual**, somando **2048**. **Exceção: Isis é Vanilla, sem pacote (2048 pontos reais).** Selecionar a entrada virtual insere **quatro cartas reais**, juntas e indivisíveis: uma Ritual Magic e três materiais da receita ativa. Os materiais são escolhidos conforme o jogo e os MODs habilitados, dando prioridade às preferências do duelista e às cartas da sua pool. Os 40 IDs resultantes são embaralhados normalmente. Limite de 3 cópias por carta e 3 pacotes por Deck. Monstros Ritual já prontos são apenas cartas normais da pool, **nunca parte do pacote**.

**Story Mode:** pelo menos 1 pacote; até 3 no total, conforme o sorteio virtual.

**Free Duel:** opção `Guaranteed Ritual Package in Free Duel` é **OFF por padrão**, também quando o MOD é instalado pela primeira vez ou atualizado de uma versão antiga. OFF permite 0, 1, 2 ou 3 pacotes por sorteio. ON garante pelo menos um, exatamente como Story. A configuração persiste pelos Settings do jogo; mudanças pedem reinício, conforme a opção do MOD.

Não existem mais dificuldades Fácil/Médio/Difícil no Ritual Duelists.

## Compatibilidade e diagnósticos

Deixe `Deck ritual recipe source = Auto` para consumir os providers de Efeitos de Rituais e as APIs oficiais do jogo. Melhoria dos Rituais continua controlando origens legais de tributo na hora de invocar; o Ritual Package controla somente o **deck inicial da CPU**. Não cria cartas e não altera o Deck do jogador. Se uma receita não permitir três materiais distintos ou respeitar o limite de cópias, o gerador recorre de forma segura ao Deck Vanilla e registra o motivo no log.

`Ritual Duelists diagnostic overlay = Detailed` mostra pacotes, fonte da receita, decisões da IA e marca se um Ritual foi apenas solicitado ou efetivamente verificado. O log identifica cada pacote e seus quatro IDs.

A inteligência compartilhada de busca de Ritual e tributos no Deck real, proteção dos materiais, escolha adaptativa e a avaliação pós-Ritual do Skull Guardian foram preservadas. Outros efeitos continuam delegados ao MOD Efeitos de Rituais, com fallback conservador quando não há API de avaliação ativa.

## Novidades v0.3.3 Beta.1

- **Yami Bakura (ID 14)**: Fiend's Mirror #365 com peso real 30 e Beastry Mirror Ritual #674 apenas via pacote virtual de 30. São 12 cortes de -5 pontos em sua pool Vanilla de 128 cartas; agora 129 cartas reais.
- **Pegasus (ID 15)**: Psycho-Puppet #715 com peso real 30 e Puppet Ritual #695 só no pacote. Mantidas as 100 cartas Vanilla, total de 101 cartas reais com 12 cortes de -5.
- **Mage Soldier (ID 18)**: Millennium Shield #362 com peso real 30 e Curse of Millennium Shield #665 só no pacote. Mantidas as 150 cartas Vanilla, total de 151 cartas reais com 12 cortes de -5.
- **Compatibilidade:** todos os 10 perfis anteriores e suas pools continuam iguais à v0.3.2-beta.3, inclusive Tri-Horned pronto de Rex com peso 7. A IA genérica e a detecção de receitas ativas são reaproveitadas, sem implementar efeitos exclusivos aqui.
- **Validação:** suítes C, auditoria de 16 pools, simulações de geração Story/Free OFF/Free ON, build ELF32 e ELF64. A compilação não substitui teste dentro do jogo.

## Histórico anterior — expansão Rex/Martis/Desert

- **Rex Raptor (ID 9)** e **High Mage Martis (ID 28)** recebem Tri-Horned Dragon (#705), Ritual Magic #680 apenas em pacotes e cartas prontas com pesos distintos: 7 no Rex, 30 no Martis.
- **Desert Mage (ID 27)** recebe Dokurorider (#719) como Ritual; Revival of Skeleton Rider (#699) apenas no pacote, mesmo na receita Vanilla que exige materiais fora da pool.
- **Weevil (ID 10)** passa a sortear Time Wizard (#16) com peso 25 como carta NORMAL, não como material de Ritual Package. O MOD não altera as funções especiais de Time Wizard.
- Todas as outras estratégias, inclusive os dois Heishins, permanecem preservadas. Para efeitos pós-Ritual sem API confiável, a IA usa fallback conservador.

## Arquivos úteis

- `HISTORICO.md` — linha do tempo e mudanças de pesos aprovadas.
- `ARQUITETURA.md` — desenho técnico e tabelas completas dos Heishins.
- `TESTES.md` — resultados, limites e roteiro de validação no jogo.
- `tests/run_all_beta.sh` — suíte automatizada, usa os headers oficiais v0.3.0.
- `spec/Ritual-Duelists-Pools.csv` — tabelas exatas dos vinte e dois perfis, com pacote virtual de 30 pontos.

**Status Beta.5:** 22 perfis; auditoria Python das pools finais e builds Windows 32/64 bits. Os três novos duelistas exigem validação em partida real.


**Atualização Beta.3:** o preparo dos materiais frágeis do Fiend's Mirror do Bakura não é mais abandonado sob pressão não letal quando a Ritual Magic já está baixada. Nenhuma pool foi alterada. O efeito de combate do Fiend's Mirror pertence ao MOD separado Efeitos de Rituais v0.78.48. Teste em jogo ainda necessário.

## Rodada V4 — 2026-10-17

Os cinco perfis da rodada V4 são Mai Valentine, Shadi, High Mage Anubisius, Meadow Mage e Weevil. As pools e receitas virtuais estão no `profile_data.h` e `profiles.c`. Para histórico e deltas, consultar `HISTORICO.md`. Os efeitos pertencem exclusivamente a `Efeitos de Rituais`. Nenhuma receita das fusões especiais foi codificada aqui.


## Integração Deck Inicial + Simon Muran (v0.4.0-alpha.1, 18/10/2026)

Esta versão substitui **o MOD Ritual Duelists anterior e o protótipo RitualStarter**.
Não instale o MOD separado `ritual-starter-prototype` ao mesmo tempo: ambos
interceptam `NameEntry_BuildStarterDeck`.

- Nova campanha: seleção visual das três boxes aprovada na v0.2.5 do ChatGPT Work.
  Os 57 arquivos de assets foram copiados sem qualquer modificação.
- Azul (Mask of Shine & Dark), Verde (Performance of Sword), Vermelho (Yamadron).
- Seu deck tem **40 cartas**: **33 escolhidas pela geração Vanilla original** e
  7 garantidas: **3 materiais ativos**, Ritual Magic, Raigeki #337, Field e Equip.
- Sem outro MOD ritual, usa os três tributos EXATOS da imagem USA:
  Contruct of Mask #693: #102 / #182 / #220;
  Commencement Dance #676: #249 / #395 / #511;
  Yamadron Ritual #666: #29 / #31 / #101.
- Em modo Auto (`recipe_source=0`) usa primeiro provedor de receita ativo,
  depois tabelas do jogo, senão as receitas originais comprovadas no disco.
- Simon Muran sorteia UM dos outros dois temas ao criar a campanha e recebe
  um deck de 40 cartas com o mesmo gerador (33 Vanilla + 7 garantidas).
  A escolha e as 40 cartas de Simon são salvas no estado v3 deste MOD.
- Sua IA recebe os mesmos hooks de planner, busca de Ritual Magic e tributos,
  validação pelo motor e critérios de risco dos demais duelistas.
- As pools/pesos/receitas dos 29 perfis antigos não foram alteradas.
- Removida a seleção secreta de uma quarta box; a UI agora oferece três.

**Compatibilidade de save:** inicie uma NOVA CAMPANHA para escolher uma box.
Um save anterior sem esse evento não terá tema persistido para Simon e deverá
manter o Muran Vanilla, sem sortear retroativamente um tema.

**Limitação:** a estrutura garantida 7 cartas pressupõe exatamente três
tributos. Se um MOD muda esse ritual para outro número de tributos ou o
remove, o jogo mantém o deck inicial Vanilla em vez de fabricar materiais
ilegais; o motivo aparece no log. A IA do Simon usa o verificador real de
legalidade do Ritual, não força invocações ilegais.

**Validações executadas (sem runtime Windows disponível):**
- SDK oficial v0.3.0, .o i386 e .x86_64-windows compilados;
- teste local sob ASan/UBSan do deck inicial: 3 receitas Vanilla, provider
  simulado, modo Force Vanilla, 40 cards, limite 3 cópias, escolha de Simon;
- regressões existentes de pesos, pacotes e IA (incluindo os lotes V3/V4);
- identidade binária dos assets com RitualStarter_v025 corrigido;
- símbolos i386 verificados contra exports.i386.txt do SDK.

**Roteiro breve de teste no jogo**
1. Remover o antigo MOD separado RitualStarter; substituir o Ritual Duelists.
2. Iniciar campanha nova e testar cada box, confirmando arte, 40 cartas e
   materiais compatíveis com a receita Vanilla sem Efeitos de Rituais.
3. Reiniciar campanha 2 ou mais vezes com uma box fixa, conferir no duelo
   contra Simon que o tema alterna somente entre as outras duas opções.
4. Verificar que Simon prepara/ativa Ritual Magic quando os três tributos
   podem ser utilizados; preferir logs `diagnostics=1` e overlay detalhado.
5. Testar salvar/carregar e repetir com Efeitos de Rituais ligado; confirmar
   que os materiais seguem a receita modificada ativa.

### Pacote 02 — Drops ritualísticos (Alpha.4)

- Drop Missing Cards v1.0 deve estar **ativado**. Ritual Duelists carrega depois dele; seus drops são somados ao Vanilla expandido.
- Ritual Magics dos 28 perfis são exclusivas de seus usuários em S/A-TEC (64/2048, salvo Simon Muran 16/2048 e Duel Master K 32/2048 para os três rituais iniciais).
- Monstros rituais prontos entram em S/A-POW do duelista usuário (25/2048 por padrão, salvo pesos históricos do Vanilla expandido e exceções do Pacote 01). Fontes históricas por outros duelistas são preservadas.
- Drop List no Free Duel permanece disponível: **Triângulo abre/fecha**. Exibe pesos resultantes da composição real das tabelas, sem lista paralela.
- Os nove tributos iniciais permanecem exclusivos de B/C/D.
- Detalhes e diferenças efetivas Alpha.3→Alpha.4 em `audit_drops_02.csv`; histórico consolidado em `HISTORICO.md`.


## Drop List — interface egípcia 1.1 (v0.4.0-alpha.6)

No **Free Duel**, selecione um duelista desbloqueado e aperte **Triângulo** para abrir **ou fechar** a lista de drops. **Bolinha não fecha a lista** (para não sair do Free Duel). A versão 1.1 integra o tema dourado egípcio, símbolos Ankh/Olho de Hórus, abas de POW / BCD / TEC, barras de chance relativas ao peso mais alto, contagem de cartas com 3/3 verde, cursor com rolagem, e ajuste à tela.

Controles: **L1/R1** alternam as três pools (setas esquerda/direita também); **X** alterna filtro de menos de três cópias; **Quadrado** alterna a ordenação por chance, ID e nome; **Cima/Baixo** movem o cursor; **L2/R2** passam uma página; **Triângulo** fecha a janela.

**Dados efetivos:** o painel continua usando as tabelas do jogo combinadas com Drop Missing Cards e os Pacotes 01–03, e exclui Blue-Eyes de uma fonte única já esgotada no save. Não altera sorteios nem a geração dos decks. A barra de chance usa a maior carta da pool como referência visual; o número em porcentagem é o peso efetivo sobre 2048, ou sobre a soma elegível após esgotar aquela fonte Blue-Eyes.

**Crédito:** visual e ícones originais do **Drop List v1.1.0**, de **PauloH-Gudang**, fornecidos com autorização. Como o ZIP de referência só continha objetos compilados e PNGs, a interface foi reimplementada em C usando os serviços públicos de overlay do SDK; os PNGs estão guardados em `assets/ui/`, e as versões otimizadas para desenho fazem parte de `rd_dl11_sprites.h`. Não é necessário instalar o Droplist separadamente.

Compilação: SDK da prévia Windows v0.3.1 para i386/x86_64-windows. Android requer o compilador e o guest-IR pass específicos do build Android e ainda não foi validado. O funcionamento gráfico e o controle continuam pendentes de confirmação no jogo.
