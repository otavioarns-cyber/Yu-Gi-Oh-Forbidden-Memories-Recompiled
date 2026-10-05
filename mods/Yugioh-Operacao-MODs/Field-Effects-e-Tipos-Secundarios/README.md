# Field Effects e Tipos Secundarios

Workspace isolado do MOD para Yu-Gi-Oh! Forbidden Memories Recompiled.

## Regra de isolamento

- Todo o desenvolvimento deste MOD fica na branch `mod-field-effects`.
- Os arquivos do MOD ficam sob esta pasta dedicada.
- A branch `master` nao deve ser alterada pelo desenvolvimento deste MOD.
- O codigo-base da decompilacao serve como referencia/integracao; alteracoes do MOD devem permanecer isoladas.
- Versoes de teste serao preservadas em pastas/versionamento proprio para permitir acompanhar a evolucao.

## Estrutura planejada

- `dev/` - codigo-fonte em desenvolvimento.
- `versions/` - snapshots das versoes de teste relevantes (`v0.1`, `v0.2`, etc.).
- `docs/` - especificacao, mapeamentos e notas tecnicas do MOD.

## Status

Migrado para a base oficial v0.2.0. O nucleo de Field Effects + Tipos Secundarios e a ponte opcional de compatibilidade com AI Hard Mode compilam no SDK v0.2.0. A proxima etapa e validacao funcional dentro do jogo antes de congelar uma versao de teste em `versions/`.
