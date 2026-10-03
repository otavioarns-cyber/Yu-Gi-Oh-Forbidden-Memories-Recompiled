# Arquitetura v0.1 - Field Effects e Tipos Secundarios

## Base alvo

Yu-Gi-Oh! Forbidden Memories Recompiled v0.2.0.

## Descoberta principal

`Duel_GetTerrainBoost(s32 cardType)` nao recebe Card ID. Ele recebe somente o tipo principal e consulta o terreno ativo. Portanto, hookar apenas essa funcao nao permite distinguir cartas individuais nem aplicar os subtipos arbitrarios definidos pelo MOD.

Os dois pontos importantes do fluxo sao:

1. `Duel_SetupCardRecord`: aqui existe `DuelCardRecord.card_id` e o `terrain_modifier` inicial e calculado.
2. `DuelEffect_ApplyTerrain`: quando o Field muda, o jogo percorre os `DuelCardRecord` ocupados e recalcula `terrain_modifier`.

Consequencia: o MOD deve calcular o bonus secundario por Card ID nesses pontos, preservando o bonus principal original.

## Direcao de implementacao

- API 4+ para hooks oficiais do Recompiled.
- Nao alterar a decomp original/master.
- Tabela esparsa `Card ID -> bitmask de subtipos` para apenas as cartas aprovadas.
- O bonus principal continua vindo da mecanica original.
- O MOD soma/subtrai o bonus secundario ao `terrain_modifier` do registro.
- Recalculo apenas na criacao/entrada do registro e quando o Field muda; sem polling por frame.
- Multiplos subtipos sao representados por bitmask.
- Excecoes de sobreposicao e cartas especiais ficam concentradas na funcao de calculo, sem espalhar condicionais pelo jogo.

## Casos obrigatorios

- Summoned Skull: Thunder + Zombie.
- Metal Fish / Mech Bass: remover especificamente o debuff principal Machine em Umi e aplicar o comportamento secundario Fish.
- Aquatic overlap em Wasteland: se o tipo principal ja for Fish/Aqua/Sea Serpent, nao somar outro -200 aquatico.
- Beast-Warrior principal: nao receber +200 por subtipo Beast/Warrior.
- Excecoes individuais do documento devem permanecer sem o efeito indicado.

## Compatibilidade

A implementacao nao deve hookar os pontos usados pelo MOD Melhoria dos Rituais (`Duel_CheckRitual` e `DuelCard_DeactivateRecord`) se isso nao for necessario. O objetivo e manter os MODs independentes e reduzir conflitos.

## Versionamento

Este documento descreve a arquitetura antes da primeira build jogavel. A primeira build de teste sera preservada em `versions/v0.1/` quando o nucleo estiver compilavel.
