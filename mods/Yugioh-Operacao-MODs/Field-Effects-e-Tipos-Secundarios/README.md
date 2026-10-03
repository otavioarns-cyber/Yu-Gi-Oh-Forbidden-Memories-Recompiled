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

Projeto inicializado. Proxima etapa: mapear o ponto de integracao da API v0.2.0 e implementar o nucleo de Field Effects + Tipos Secundarios.
