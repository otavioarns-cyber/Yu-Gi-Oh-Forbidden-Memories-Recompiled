# Compatibilidade com v0.2.1-preview.1

## Base alvo

- Upstream: `Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled` `v0.2.1-preview.1`.
- Branch do MOD: `mod-field-effects`.
- `master` permanece fora do desenvolvimento deste MOD.

## Verificacoes realizadas

O release oficial da preview informa que MODs feitos para v0.1.2 e v0.2.0 continuam funcionando. O MOD permanece em API 4.

A arvore v0.2.1-preview.1 foi inspecionada para as interfaces usadas pelo MOD:

- `Cards_Type(id)` para ler o tipo atual da carta em runtime.
- `Tables_TerrainBonus(terrain, type, &bonus)` para respeitar a tabela de terreno atualmente composta pelos MODs.
- `gDuel_aTerrainBoost` como fallback Vanilla.
- `Duel_SetupCardRecord` e `DuelEffect_ApplyTerrain` para recalcular o modificador de terreno.
- provedor API 4 `terrain-card-delta-v1` para consumidores opcionais.
- `func_80035E20` para o desenho do viewer e do rodape.
- `func_80037DA4` para detectar exatamente o comando F8 que troca o text box para a descricao da carta (`0x40`).
- `Cards_DescriptionText`/fluxo de texto atual do port, que faz a deteccao funcionar tambem quando outro MOD substitui a descricao.

## v0.19 - descricao adaptativa

O viewer constroi nome, tipo/Guardian Stars e descricao dentro do mesmo text box. Por isso a v0.19 nao reduz o objeto inteiro. O hook de `func_80037DA4` registra o primeiro `DuelEffectEntry` pertencente a descricao viva; no desenho, apenas esse intervalo e medido e, se necessario, redesenhado com fonte/pitch menores. O cabecalho continua no renderer original.

A posicao do rodape da v0.18 nao foi alterada. A faixa dele e tratada como area reservada e tem prioridade sobre a descricao.

## Compilacao

`field_effects.c` v0.19 foi compilado com sucesso pelo `build_mod.py` do SDK oficial distribuido com `yfm-redecomp-v0.2.1-preview.1-windows.zip`.

O processo tambem concluiu a verificacao de simbolos/imports do SDK sem erro. Portanto, a compatibilidade de compilacao com a preview foi verificada diretamente, sem depender apenas da compatibilidade declarada com v0.2.0.

A build Windows nao foi executada neste ambiente. Ainda e necessaria validacao visual/funcional dentro do jogo antes de considerar a versao definitiva.
