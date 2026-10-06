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

Versao de desenvolvimento atual: **v0.18 Vanilla V2 test**.

O MOD foi consolidado com as regras da especificacao Vanilla V2: herancas adicionais de subtipos, sobreposicao aquatica, bloqueio de subtipo igual ao tipo principal, resolucao/cancelamento de multiplos subtipos, casos especiais de Metal Fish/Mech Bass, rodape dinamico e provedor de compatibilidade `terrain-card-delta-v1`.

Base alvo: `v0.2.1-preview.1`. O release oficial informa compatibilidade com MODs feitos para v0.2.0; o objeto v0.18 foi compilado com sucesso pelo SDK oficial v0.2.0 e as APIs utilizadas foram conferidas na arvore v0.2.1-preview.1. A validacao final dentro do jogo continua necessaria antes de congelar a versao como definitiva.
