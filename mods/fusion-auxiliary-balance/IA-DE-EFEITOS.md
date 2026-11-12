# IA de efeitos — beta44 / Rituais 0.78.74 / adaptador AI Hard Mode 0.1.1

## Instalação
Feche o jogo. No diretório de MODs, substitua as pastas de mesmo nome que já usa. A pasta BalanceamentoFusoesEfeitosAuxiliares é a atualização da beta43. Substitua zzz Efeitos Rituais se usa esse MOD; a versão nova permite consultar a instância e o aprendizado do Ritual. Substitua ai-hard-mode se usa o planejador opcional. Ele continua desativado por padrão. A CPU vanilla não precisa dele. Não substitua Ritual Duelists, campos, decks ou o executável.

## Como a CPU decide
A interface opcional effect_forecast_v2 de cada dono lê origem, lease, recursos e settings da instância. Sem o dono, a consulta não concede a habilidade. A interface ritual effect_intelligence_v1 continua disponível para os consumidores anteriores. O modelo não lê texto, cor ou nome da carta. O snapshot contém a mão real da CPU, cinco zonas de cada lado e informação visível; uma carta oculta não expõe seu ID ao modelo. O Hard Mode conserva a sua política configurável de visibilidade e janela de busca. Cartas da janela não são contadas como tributos na mão.

A IA vanilla usa os opcodes FindBestAttack, FindKiller, CalcCardPower, PlayFieldCard, LoadGuardianStarChoice e a pontuação da busca nativa de fusões. O Hard Mode recebe a previsão dentro de seu próprio planner.c. Nenhum dos dois executa uma batalha para estimar seu resultado. Quando há moeda, comparam-se resultados possíveis sem consumir RNG. O peso de uma remoção é 400 mais o maior stat da carta; perda de LP vale três vezes a quantidade efetiva; derrota imediata pesa um milhão. Geração, cura, efeito posterior e perda de aura entram em utilidade separada. A busca tem horizonte e orçamento limitados.

## Mapa de implementação
| Efeitos / decisão | Fonte da regra | Previsão / consumidor |
| --- | --- | --- |
| Giltia, Skelgon, Flame Swordsman, Rare Fish, Deepsea Shark, Pragtical, Skullbird, Barox, Marine Beast, Karbonala | fusion_round3_runtime.h, secondary_dragon_types.h | ea_case / ea_fight: condições durante o combate; escolha do atacante, alvo e estrela |
| Bickuribox, Thousand Dragon, Ocean Snake, Crimson Sunbird, Black Shark, Tank, Eagle | fusion_round2_runtime.h | modificadores antes do dano; custo separado; penalidade permanente após a luta |
| Kaiser, Empress, Seiyaryu, Bracchio, Kwagar, Musician, Flower Wolf | mod.c e runtimes das duas rodadas | ea_guard / ea_case: proteção, alvo legal e dano de LP |
| THTD, Skull Knight, Soul Hunter, Aqua Dragon, Mammoth, Gaia, Zombie Warrior, Charubin, Flame Ghost, Mavelus, Warrior of Tradition | runtimes das duas rodadas e generated_card_restore.h | ea_death / ea_case: benefício posterior, espaço, retorno futuro e sobreviventes |
| Rose, Mystic Sand, Sparrow, Dragoness, Bugroth, Kaminari | runtimes das duas rodadas | reação, aura, campo e bloqueio por turno. A beta43 reflete somente com exatamente uma Rose ativa; o executor foi preservado. |
| Fusionist, Elegant Egotist, Metalmorph | fusionist_rules.h, fusionist_runtime.h, egotist_targets.h | ea_pair conserva ordem, origem, resultado condicionado e transformação por equipagem; empate de mesmo ATK/DEF usa representante determinístico na previsão |
| Darkfire, Mystical Elf, Dark Elf, B. Skull, Meteor B., Kamion, Encore, Battle Warrior, Rabid, Wall Shadow | mod.c, logic.h | ea_rank_combo, escolha de Magic/Equip, probabilidade e posição; efeitos de entrada não viram ATK permanente |
| Skull Guardian, Whale, Crab Turtle, Javelin, Tri-Horned, BLS, Zera, War-Lion, Cosmo Queen, Dokurorider | mod.c de Rituais, egotist_bridge.h, ritual_ai_effect_source.h | leitura da origem e das cargas; ea_aura e ea_ritvalue após materiais; restrições e ganho contextual |
| Mask, Ultimate, Serpent, Sengenjin, Psycho-Puppet | mask_effect.h, serpent_fear.h, mod.c | preservação do aprendizado; alvo redirecionado, controle temporário e imunidade conhecida |
| Gate Guardian, Garma, Mirror, Millennium Shield, Hungry Burger, Performance, Black Chaos, Chakra, Yamadron | mod.c de Rituais, performance_chaos.h e políticas nativas | ataque adicional condicionado, reflexão, posição, risco da moeda, valor de entrada; a obrigação de burn de Yamadron continua na política original |

## Bases, validação e limites
Fusões: ZIP beta43 com ícones HD. Rituais: ZIP v0.78.73 Psycho-Puppet. AI Hard Mode: código do commit 10ee7e5ecf da distribuição v0.3.1-preview.1. O SDK é o da mesma distribuição. Os objetos i386 e x86_64-windows foram recompilados de uma extração do pacote-fonte. Referência detalhada dos resultados em TESTES-IA.txt; hashes dos objetos nos manifests de proveniência.

Exemplos observados nos testes: Giltia 1850 contra DARK 2200 passa de evitar o ataque a atacar com margem +250. Labyrinth Tank 2400 contra DEF 3000 escolhe atacar com 4000 LP e preservar-se com 499 ou 500 LP. O teste de paridade compara diretamente o modelo e o executor em 88 combinações de origem, posição e lado. Passaram os 500 cenários originais do Hard Mode e as regressões de Fusionist, ícones, burn obrigatório e Rituais listadas no relatório.

Não foi executado um duelo no Windows. Valores futuros (carta aleatória, posição futura, vaga no turno seguinte e reação adversária) são estimativas, não garantias. A previsão de ritual do Hard Mode considera a receita publicada pelo dono e o campo; deixa a legalidade de combinações externas ao mecanismo já instalado. Não há nova busca pelos materiais privados de outros MODs, nem promessa de conhecer todos os resultados de hooks de terceiros. O planejador não tenta antecipar toda sequência de ações depois de um resultado aleatório. Os logs de decisão são opcionais e não criam HUD.

**Teste manual prioritário:** faça um Free Duel com Giltia contra DARK, Tank perto de 500 LP, dois monstros idênticos com origens diferentes e um adversário oculto. Depois confira a seleção de alvos do Puppet/War-Lion e o uso das mágicas geradas pelo Yamadron, com o Hard Mode desligado e ligado. Recarregar save durante essas interações e a combinação completa com Ritual Duelists também ficam para validação em jogo.
