# HAL-22 — matriz da PoC de ordem

| Coorte | Processos | PASS | FAIL | BLOCKED | SKIP |
|---|---:|---:|---:|---:|---:|
| Guard OFF / controles | 45 | 1908 | 158 | 36 | 9 |
| Guard ON / protegido | 51 | 1834 | 0 | 0 | 24 |

Mais 3 PASS entre perfis/suítes, incluindo 3072 comparações de ordenação com manifests reais e a suíte oficial de hooks. Os 158 FAIL pertencem apenas aos controles I2 sem proteção.

| Caso | Fase | PASS | FAIL | BLOCKED | SKIP |
|---|---:|---:|---:|---:|---:|
| [guard-off-safe-original](resultados/guard-off-safe-original-p0.log) | 0 | 52 | 0 | 0 | 0 |
| [guard-off-safe-restart](resultados/guard-off-safe-restart-p0.log) | 0 | 52 | 0 | 0 | 0 |
| [guard-off-late-ai-apply-original](resultados/guard-off-late-ai-apply-original-p0.log) | 0 | 32 | 17 | 4 | 0 |
| [guard-off-late-ai-apply-original](resultados/guard-off-late-ai-apply-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-late-ai-direct-original](resultados/guard-off-late-ai-direct-original-p0.log) | 0 | 32 | 17 | 4 | 0 |
| [guard-off-late-ai-direct-original](resultados/guard-off-late-ai-direct-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-late-ai-reload-original](resultados/guard-off-late-ai-reload-original-p0.log) | 0 | 32 | 17 | 4 | 0 |
| [guard-off-late-ai-reload-original](resultados/guard-off-late-ai-reload-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-late-ai-apply-restart](resultados/guard-off-late-ai-apply-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-off-late-ai-apply-restart](resultados/guard-off-late-ai-apply-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-late-ai-direct-restart](resultados/guard-off-late-ai-direct-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-off-late-ai-direct-restart](resultados/guard-off-late-ai-direct-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-late-ai-reload-restart](resultados/guard-off-late-ai-reload-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-off-late-ai-reload-restart](resultados/guard-off-late-ai-reload-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-late-dual-original](resultados/guard-off-late-dual-original-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-off-late-dual-original](resultados/guard-off-late-dual-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-late-dual-restart](resultados/guard-off-late-dual-restart-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-off-late-dual-restart](resultados/guard-off-late-dual-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-off-on-original](resultados/guard-off-off-on-original-p0.log) | 0 | 73 | 0 | 0 | 0 |
| [guard-off-off-on-original](resultados/guard-off-off-on-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-off-on-restart](resultados/guard-off-off-on-restart-p0.log) | 0 | 73 | 0 | 0 | 0 |
| [guard-off-off-on-restart](resultados/guard-off-off-on-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-persist-off-restart](resultados/guard-off-persist-off-restart-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-off-persist-off-restart](resultados/guard-off-persist-off-restart-p1.log) | 1 | 16 | 0 | 0 | 1 |
| [guard-off-persist-off-restart](resultados/guard-off-persist-off-restart-p2.log) | 2 | 53 | 0 | 0 | 0 |
| [guard-off-window-first-ai-original](resultados/guard-off-window-first-ai-original-p0.log) | 0 | 32 | 17 | 4 | 0 |
| [guard-off-window-first-ai-original](resultados/guard-off-window-first-ai-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-window-first-ai-restart](resultados/guard-off-window-first-ai-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-off-window-first-ai-restart](resultados/guard-off-window-first-ai-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-window-cancel-restart](resultados/guard-off-window-cancel-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-off-window-cancel-restart](resultados/guard-off-window-cancel-restart-p1.log) | 1 | 15 | 0 | 0 | 1 |
| [guard-off-rank-invert-original](resultados/guard-off-rank-invert-original-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-off-rank-invert-original](resultados/guard-off-rank-invert-original-p1.log) | 1 | 31 | 18 | 4 | 0 |
| [guard-off-rank-invert-restart](resultados/guard-off-rank-invert-restart-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-off-rank-invert-restart](resultados/guard-off-rank-invert-restart-p1.log) | 1 | 31 | 18 | 4 | 0 |
| [guard-off-rank-repair-restart](resultados/guard-off-rank-repair-restart-p0.log) | 0 | 31 | 18 | 4 | 0 |
| [guard-off-rank-repair-restart](resultados/guard-off-rank-repair-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-restart-fails-repair](resultados/guard-off-restart-fails-repair-p0.log) | 0 | 32 | 18 | 4 | 0 |
| [guard-off-restart-fails-repair](resultados/guard-off-restart-fails-repair-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-off-equal-rank-ai-first](resultados/guard-off-equal-rank-ai-first-p0.log) | 0 | 52 | 0 | 0 | 0 |
| [guard-off-equal-rank-dual-first](resultados/guard-off-equal-rank-dual-first-p0.log) | 0 | 30 | 18 | 4 | 0 |
| [guard-off-only-ai-safe](resultados/guard-off-only-ai-safe-p0.log) | 0 | 14 | 0 | 0 | 1 |
| [guard-off-only-dual-safe](resultados/guard-off-only-dual-safe-p0.log) | 0 | 14 | 0 | 0 | 1 |
| [guard-off-observers-between](resultados/guard-off-observers-between-p0.log) | 0 | 75 | 0 | 0 | 0 |
| [guard-off-observers-around](resultados/guard-off-observers-around-p0.log) | 0 | 54 | 0 | 0 | 0 |
| [guard-on-safe-original](resultados/guard-on-safe-original-p0.log) | 0 | 52 | 0 | 0 | 0 |
| [guard-on-safe-restart](resultados/guard-on-safe-restart-p0.log) | 0 | 52 | 0 | 0 | 0 |
| [guard-on-late-ai-apply-original](resultados/guard-on-late-ai-apply-original-p0.log) | 0 | 16 | 0 | 0 | 1 |
| [guard-on-late-ai-apply-original](resultados/guard-on-late-ai-apply-original-p1.log) | 1 | 15 | 0 | 0 | 1 |
| [guard-on-late-ai-direct-original](resultados/guard-on-late-ai-direct-original-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-late-ai-direct-original](resultados/guard-on-late-ai-direct-original-p1.log) | 1 | 15 | 0 | 0 | 1 |
| [guard-on-late-ai-reload-original](resultados/guard-on-late-ai-reload-original-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-late-ai-reload-original](resultados/guard-on-late-ai-reload-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-late-ai-apply-restart](resultados/guard-on-late-ai-apply-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-late-ai-apply-restart](resultados/guard-on-late-ai-apply-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-late-ai-direct-restart](resultados/guard-on-late-ai-direct-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-late-ai-direct-restart](resultados/guard-on-late-ai-direct-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-late-ai-reload-restart](resultados/guard-on-late-ai-reload-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-late-ai-reload-restart](resultados/guard-on-late-ai-reload-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-late-dual-original](resultados/guard-on-late-dual-original-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-on-late-dual-original](resultados/guard-on-late-dual-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-late-dual-restart](resultados/guard-on-late-dual-restart-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-on-late-dual-restart](resultados/guard-on-late-dual-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-off-on-original](resultados/guard-on-off-on-original-p0.log) | 0 | 73 | 0 | 0 | 0 |
| [guard-on-off-on-original](resultados/guard-on-off-on-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-off-on-restart](resultados/guard-on-off-on-restart-p0.log) | 0 | 73 | 0 | 0 | 0 |
| [guard-on-off-on-restart](resultados/guard-on-off-on-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-persist-off-restart](resultados/guard-on-persist-off-restart-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-on-persist-off-restart](resultados/guard-on-persist-off-restart-p1.log) | 1 | 16 | 0 | 0 | 1 |
| [guard-on-persist-off-restart](resultados/guard-on-persist-off-restart-p2.log) | 2 | 53 | 0 | 0 | 0 |
| [guard-on-window-first-ai-original](resultados/guard-on-window-first-ai-original-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-window-first-ai-original](resultados/guard-on-window-first-ai-original-p1.log) | 1 | 15 | 0 | 0 | 1 |
| [guard-on-window-first-ai-restart](resultados/guard-on-window-first-ai-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-window-first-ai-restart](resultados/guard-on-window-first-ai-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-window-cancel-restart](resultados/guard-on-window-cancel-restart-p0.log) | 0 | 15 | 0 | 0 | 1 |
| [guard-on-window-cancel-restart](resultados/guard-on-window-cancel-restart-p1.log) | 1 | 15 | 0 | 0 | 1 |
| [guard-on-rank-invert-original](resultados/guard-on-rank-invert-original-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-on-rank-invert-original](resultados/guard-on-rank-invert-original-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-rank-invert-restart](resultados/guard-on-rank-invert-restart-p0.log) | 0 | 53 | 0 | 0 | 0 |
| [guard-on-rank-invert-restart](resultados/guard-on-rank-invert-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-rank-repair-restart](resultados/guard-on-rank-repair-restart-p0.log) | 0 | 12 | 0 | 0 | 1 |
| [guard-on-rank-repair-restart](resultados/guard-on-rank-repair-restart-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-restart-fails-repair](resultados/guard-on-restart-fails-repair-p0.log) | 0 | 13 | 0 | 0 | 1 |
| [guard-on-restart-fails-repair](resultados/guard-on-restart-fails-repair-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-equal-rank-ai-first](resultados/guard-on-equal-rank-ai-first-p0.log) | 0 | 52 | 0 | 0 | 0 |
| [guard-on-equal-rank-dual-first](resultados/guard-on-equal-rank-dual-first-p0.log) | 0 | 11 | 0 | 0 | 1 |
| [guard-on-only-ai-safe](resultados/guard-on-only-ai-safe-p0.log) | 0 | 14 | 0 | 0 | 1 |
| [guard-on-only-dual-safe](resultados/guard-on-only-dual-safe-p0.log) | 0 | 14 | 0 | 0 | 1 |
| [guard-on-observers-between](resultados/guard-on-observers-between-p0.log) | 0 | 75 | 0 | 0 | 0 |
| [guard-on-observers-around](resultados/guard-on-observers-around-p0.log) | 0 | 54 | 0 | 0 | 0 |
| [guard-on-arm-unsafe](resultados/guard-on-arm-unsafe-p0.log) | 0 | 11 | 0 | 0 | 1 |
| [guard-on-arm-repair-fails](resultados/guard-on-arm-repair-fails-p0.log) | 0 | 13 | 0 | 0 | 1 |
| [guard-on-arm-repair-fails](resultados/guard-on-arm-repair-fails-p1.log) | 1 | 53 | 0 | 0 | 0 |
| [guard-on-arm-repair-cancel](resultados/guard-on-arm-repair-cancel-p0.log) | 0 | 12 | 0 | 0 | 1 |
| [guard-on-arm-repair-cancel](resultados/guard-on-arm-repair-cancel-p1.log) | 1 | 12 | 0 | 0 | 1 |
| [guard-on-arm-pending-off](resultados/guard-on-arm-pending-off-p0.log) | 0 | 19 | 0 | 0 | 1 |

SKIP não significa compatibilidade aprovada: a suíte conjunta é omitida quando o perfil é recusado ou os dois MODs não estão ativos. A entrada modificada de Main_RunDuel foi exercitada para conferir a recusa antes do corpo.

[Relatório e cinco respostas](../HAL22_ORDER_GUARD_POC.md) · [CSV](resultados/MATRIZ.csv) · [Resultados e comandos](resultados/RESULTADOS.json) · [Diff experimental](DIFF_EXPERIMENTAL.patch)

Nenhuma certificação de gameplay Windows ou integração completa com outros objetos de produção.
