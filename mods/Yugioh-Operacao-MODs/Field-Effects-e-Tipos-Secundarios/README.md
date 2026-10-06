# Field Effects e Tipos Secundarios

Workspace isolado do MOD para Yu-Gi-Oh! Forbidden Memories Recompiled.

## Regra de isolamento

- Todo o desenvolvimento deste MOD fica na branch `mod-field-effects`.
- Os arquivos do MOD ficam sob esta pasta dedicada.
- A branch `master` nao deve ser alterada pelo desenvolvimento deste MOD.
- O codigo-base da decompilacao serve como referencia/integracao; alteracoes do MOD devem permanecer isoladas.
- Versoes de teste sao preservadas em pastas/versionamento proprio para permitir acompanhar a evolucao.

## Estrutura

- `dev/` - codigo-fonte e manifesto em desenvolvimento.
- `versions/` - snapshots das versoes de teste relevantes.
- `docs/` - especificacao, mapeamentos e notas tecnicas do MOD.

## Status

Versao de desenvolvimento atual: **v0.19 Adaptive Description test**.

O nucleo Vanilla V2 permanece o mesmo da v0.18: herancas adicionais de subtipos, sobreposicao aquatica, bloqueio de subtipo igual ao tipo principal, resolucao/cancelamento de multiplos subtipos, casos especiais de Metal Fish/Mech Bass, rodape dinamico e provedor de compatibilidade `terrain-card-delta-v1`.

A v0.19 mantem o rodape na posicao aprovada e adiciona ajuste adaptativo apenas para a descricao da carta. Quando a descricao viva ocuparia a area reservada ao rodape, o MOD reduz progressivamente a fonte/pitch da descricao; nome, tipo e Guardian Stars continuam no tamanho normal. A analise e feita sobre os `DuelEffectEntry` que o jogo realmente construiu, portanto tambem abrange descricoes substituidas por outros MODs ou traducoes compativeis.

Base alvo: `v0.2.1-preview.1`. O objeto v0.19 foi compilado com sucesso usando o `build_mod.py` do SDK oficial distribuido com essa propria preview e passou pela verificacao de simbolos do SDK. A validacao visual/funcional dentro do jogo continua necessaria antes de congelar a versao como definitiva.
