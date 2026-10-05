# Migração para Yu-Gi-Oh! Forbidden Memories Recompiled v0.2.0

## Objetivo

Migrar o MOD Field Effects + Secondary Types para a base oficial v0.2.0 sem alterar o comportamento aprovado do MOD aos olhos do jogador.

## Base

- Upstream alvo: Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled v0.2.0.
- Branch de desenvolvimento: `mod-field-effects`.
- Estado anterior preservado em `backup/mod-field-effects-pre-v0.2.0`.
- O `master` permanece como base limpa sincronizada com a v0.2.0.

## Alterações técnicas da migração

1. O código do Field Effects foi adaptado ao SDK/API 4 da v0.2.0.
2. Os hooks continuam limitados a `Duel_SetupCardRecord` e `DuelEffect_ApplyTerrain`.
3. O bônus/debuff principal continua vindo do sistema original/manifesto; os tipos secundários continuam sendo aplicados por Card ID.
4. Foi removido o hook visual temporário de nomes de cartas usado apenas para diagnóstico, evitando sobreposição desnecessária com outros MODs.
5. O MOD publica o provedor opcional `terrain-card-delta-v1` para consumidores que precisem consultar o delta de terreno específico de uma carta.

## Compatibilidade com AI Hard Mode

A branch contém uma adaptação opcional do AI Hard Mode para consultar:

`field-effects-secondary-types:terrain-card-delta-v1`

A consulta é feita via `host->find` e é preguiçosa, portanto a ordem de carregamento não cria dependência rígida.

Regras de compatibilidade:

- Sem o Field Effects carregado, o AI Hard Mode usa exatamente seu cálculo de terreno anterior.
- Com o Field Effects instalado mas desativado, o provedor retorna zero e o AI Hard Mode mantém seu comportamento anterior.
- Com ambos ativos, o planejador passa a considerar os mesmos deltas por Card ID que serão aplicados pelo duelo real.
- Nenhuma configuração, efeito, regra ou interface do AI Hard Mode foi alterada.
- O Field Effects não depende do AI Hard Mode para funcionar.

## Validação de compilação

Validado contra o SDK distribuído com `yfm-redecomp-v0.2.0-windows.zip`:

- `field_effects.c` -> compilação concluída com sucesso.
- `ai_hard_mode.c` + `planner.c` -> compilação concluída com sucesso.

Isso confirma compatibilidade de compilação/linkagem do SDK. Ainda é necessária validação dentro do jogo para considerar a migração funcionalmente fechada.

## O que não mudou

- Tabela aprovada de tipos secundários.
- Regras de bônus/debuff secundário.
- Exceções individuais já aprovadas.
- Regras globais de Wasteland/Yami presentes no manifesto.
- Comportamento Vanilla quando o MOD está desativado.
- Operação normal do AI Hard Mode quando usado sem o Field Effects.
