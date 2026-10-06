# Compatibilidade com v0.2.1-preview.1

## Base alvo

- Upstream: `Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled` `v0.2.1-preview.1`.
- Branch do MOD: `mod-field-effects`.
- `master` permanece fora do desenvolvimento deste MOD.

## Verificacoes realizadas

O release oficial da preview informa que MODs feitos para v0.1.2 e v0.2.0 continuam funcionando. O MOD permanece em API 4 e preserva os hooks ja utilizados no nucleo, acrescentando apenas o hook visual do rodape que veio da versao de teste mais recente enviada pelo autor.

A arvore v0.2.1-preview.1 foi inspecionada para as interfaces usadas pela v0.18:

- `Cards_Type(id)` para ler o tipo atual da carta em runtime.
- `Tables_TerrainBonus(terrain, type, &bonus)` para respeitar a tabela de terreno atualmente composta pelos MODs.
- `gDuel_aTerrainBoost` como fallback Vanilla.
- `Duel_SetupCardRecord` e `DuelEffect_ApplyTerrain` para recalcular o modificador de terreno.
- provedor API 4 `terrain-card-delta-v1` para consumidores opcionais.

## Compilacao

`field_effects.c` foi compilado com sucesso pelo `build_mod.py` do SDK oficial v0.2.0, gerando um objeto portavel i386 aceito pela verificacao de simbolos do SDK.

A build da preview em si nao foi executada neste ambiente. Portanto, ainda e necessaria validacao funcional dentro do jogo antes de considerar a migracao encerrada.
