# HAL-22 — matriz resumida

| Perfil | PASS | FAIL | BLOCKED |
|---|---:|---:|---:|
| O0 | 44 | 0 | 0 |
| O1 | 44 | 0 | 0 |
| O2 | 25 | 15 | 4 |
| O0-late-AI | 25 | 15 | 4 |

138 PASS / 30 FAIL / 8 BLOCKED; mais 1 PASS entre perfis. Os FAILs ficam visíveis: runner exit 1 esperado enquanto I2 não for corrigido. Não são 30 bugs distintos.

O0/O1: Dual valida/atualiza categoria antes de AI materializar. O2 e primeira carga tardia: AI contorna o swap Dual; o controller pode consumir categoria diferente da contabilizada. Limpeza de rejeição fica BLOCKED quando não houve gate/rejeição.

[CSV completo](resultados/MATRIZ.csv) · [Resultados/comandos](resultados/RESULTADOS.json) · [Relatório e limites](../HAL22_AI_HARD_MODE_DUAL_PLAY.md)

Esta é uma execução ELF32/QEMU com controller/planner oficiais e host parcial. Não comprova gameplay Windows, decks reais ou integração com outros MODs.
