# Decisoes tecnicas consolidadas

- O MOD deve permanecer separado do codigo oficial/recompilado e usar exportacoes publicas do SDK sempre que possivel.
- Branch exclusiva: `mod-ritual-effects`.
- Area exclusiva: `mods/Yugioh-Operacao-MODs/Efeitos-de-Rituais/`.
- Nao alterar `master`, `mod-field-effects` nem arquivos do MOD Field Effects e Tipos Secundarios.
- A decomp/SDK v0.2.0 pode ser usada como referencia tecnica; nao incorporar alteracoes ao executavel oficial apenas para suportar este MOD.
- Prioridade de engenharia: codigo compacto e eficiente, mas nunca sacrificando efeitos/mecanicas ja aprovados.
- Preservar versoes funcionais anteriores quando uma nova versao de teste comecar.
- Diagnostico visual/textual deve ser opcional por Settings; nao deve impor custo permanente quando desligado.
- Compatibilidade de receitas: preservar restauracao quando Recipe OFF, materiais exatos e convivencia com MelhoriaDosRituais.
- Para rituais modificados, exigir tres registros/materiais distintos e pelo menos um material em campo.
- v0.74: sem efeitos novos; somente limpeza/desempenho em cima da v0.73.
