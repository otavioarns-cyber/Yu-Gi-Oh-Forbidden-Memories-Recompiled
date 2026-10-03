# Arquitetura auditada da v0.74

- `mod.c`: inicialização MemoriesModInit, hooks de lifecycle, invocação,
  estatísticas, batalha, efeitos e cenas; marcas por registro e identidade
  de `data`; configurações em cache; miniaturas geradas e overlay opcional.
- `mod.json`: manifesto, biblioteca ritual-card-effects, API mínima 4,
  configurações individuais e em lote, substituições de textos.
- `ritual_recipe_data.h`: definições/consulta de receitas privadas.
- `ritual_recipe_compat.h`: seleção de três candidatos distintos, prioridade
  para campo, exigência de pelo menos um material no campo, publicação das
  receitas exatas e restauração condicional. Desempate usa DEF quando a DEF
  do resultado supera ATK; caso contrário usa ATK; depois o outro atributo.
- `performance_chaos.h`: marcas, moeda, proteção de batalha de Performance,
  fila de Black Chaos por turno, controle de entrada e continuação do duelo.
- `ritual_coin.h`: animação e callback da moeda; assets coin-atem/coin-normal.
- `mask_effect.h`: bloqueio de fusões e seleção específica de materiais.
- `garma_visual.h`: apresentação de Garma; `tri_horned.h` e
  `tri_horn_sprite.h`: estado/arte de Tri-Horned; `serpent_fear.h`: medo/visual.
- `png.c` e `png.h`: carregamento PNG; `assets/`: 53 PNGs e duas definições
  coin.txt; `text/`: 42 arquivos de texto; `LEIA-ME.txt`: instruções originais.
- `ritual-card-effects.o`: objeto ELF i386 recebido, necessário à instalação
  pronta. Não comprova sozinho equivalência entre fontes e binário compilado.

## Resolução e pontos de investigação

`apply_ritual()` captura D_8009B19C antes de chamar original_apply e marca
rituais após a transição 0x83 para estado 4/5. A propriedade deve ser conferida
no grid e no controlador; o fato de o hook observar um slot não prova que
este hook o moveu para o outro lado.

`chaos_sequence()` inicia DuelEffect_StartCardEffect(DARK_HOLE_ID,1), aguarda
que o indicador ACTIVE termine e depois reconcilia as marcas. Enquanto
chaos_turn.active estiver ligado, `coin_update_pads()` bloqueia entradas.
`remove_card()` impede destruição dos protegidos e acumula sobreviventes;
`update_card_effect()` chama restore_protected_visuals somente na transição
ACTIVE -> inativo. A restauração visual libera/recria objetos de sobreviventes.
Esses são pontos para diagnóstico, não uma causa confirmada do soft lock.

## Efeitos registrados

A tabela ritual_effects contém 15 monstros: Millennium Shield, Yamadron,
Psycho-Puppet, Hungry Burger, Fiend's Mirror, Mask of Shine & Dark,
Javelin Beetle, Gate Guardian, Crab Turtle, Fortress Whale, Garma Sword,
Tri-Horned Dragon, Serpent Night Dragon, Performance of Sword e
Magician of Black Chaos. Os textos e opções completos permanecem nos
arquivos originais. Recipe ON/OFF e Effect ON/OFF são controles separados.
