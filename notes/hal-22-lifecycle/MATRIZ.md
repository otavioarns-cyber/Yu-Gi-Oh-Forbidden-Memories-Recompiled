# HAL-22 — matriz de ciclo de vida e reinício

22 casos / 41 processos / 19 reinícios entre fases. PASS 1618, FAIL 158, BLOCKED 36, SKIP 7; mais 1 PASS entre perfis. As falhas são os controles inseguros e contraexemplos não corrigidos. Nenhum ERROR.

| Caso | Fase/processo novo | restart AI | PASS | FAIL | BLOCKED | SKIP |
|---|---:|---|---:|---:|---:|---:|
| [safe-original](resultados/safe-original-p0.log) | 0 | original | 49 | 0 | 0 | 0 |
| [safe-restart](resultados/safe-restart-p0.log) | 0 | temporário true | 49 | 0 | 0 | 0 |
| [late-ai-apply-original](resultados/late-ai-apply-original-p0.log) | 0 | original | 29 | 17 | 4 | 0 |
| [late-ai-apply-original](resultados/late-ai-apply-original-p1.log) | 1 | original | 50 | 0 | 0 | 0 |
| [late-ai-direct-original](resultados/late-ai-direct-original-p0.log) | 0 | original | 29 | 17 | 4 | 0 |
| [late-ai-direct-original](resultados/late-ai-direct-original-p1.log) | 1 | original | 50 | 0 | 0 | 0 |
| [late-ai-reload-original](resultados/late-ai-reload-original-p0.log) | 0 | original | 29 | 17 | 4 | 0 |
| [late-ai-reload-original](resultados/late-ai-reload-original-p1.log) | 1 | original | 50 | 0 | 0 | 0 |
| [late-ai-apply-restart](resultados/late-ai-apply-restart-p0.log) | 0 | temporário true | 12 | 0 | 0 | 1 |
| [late-ai-apply-restart](resultados/late-ai-apply-restart-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [late-ai-direct-restart](resultados/late-ai-direct-restart-p0.log) | 0 | temporário true | 12 | 0 | 0 | 1 |
| [late-ai-direct-restart](resultados/late-ai-direct-restart-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [late-ai-reload-restart](resultados/late-ai-reload-restart-p0.log) | 0 | temporário true | 12 | 0 | 0 | 1 |
| [late-ai-reload-restart](resultados/late-ai-reload-restart-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [late-dual-original](resultados/late-dual-original-p0.log) | 0 | original | 50 | 0 | 0 | 0 |
| [late-dual-original](resultados/late-dual-original-p1.log) | 1 | original | 50 | 0 | 0 | 0 |
| [late-dual-restart](resultados/late-dual-restart-p0.log) | 0 | temporário true | 50 | 0 | 0 | 0 |
| [late-dual-restart](resultados/late-dual-restart-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [off-on-original](resultados/off-on-original-p0.log) | 0 | original | 65 | 0 | 0 | 0 |
| [off-on-original](resultados/off-on-original-p1.log) | 1 | original | 50 | 0 | 0 | 0 |
| [off-on-restart](resultados/off-on-restart-p0.log) | 0 | temporário true | 65 | 0 | 0 | 0 |
| [off-on-restart](resultados/off-on-restart-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [persist-off-restart](resultados/persist-off-restart-p0.log) | 0 | temporário true | 50 | 0 | 0 | 0 |
| [persist-off-restart](resultados/persist-off-restart-p1.log) | 1 | temporário true | 13 | 0 | 0 | 1 |
| [persist-off-restart](resultados/persist-off-restart-p2.log) | 2 | temporário true | 50 | 0 | 0 | 0 |
| [window-first-ai-original](resultados/window-first-ai-original-p0.log) | 0 | original | 29 | 17 | 4 | 0 |
| [window-first-ai-original](resultados/window-first-ai-original-p1.log) | 1 | original | 50 | 0 | 0 | 0 |
| [window-first-ai-restart](resultados/window-first-ai-restart-p0.log) | 0 | temporário true | 12 | 0 | 0 | 1 |
| [window-first-ai-restart](resultados/window-first-ai-restart-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [window-cancel-restart](resultados/window-cancel-restart-p0.log) | 0 | temporário true | 12 | 0 | 0 | 1 |
| [window-cancel-restart](resultados/window-cancel-restart-p1.log) | 1 | temporário true | 12 | 0 | 0 | 1 |
| [rank-invert-original](resultados/rank-invert-original-p0.log) | 0 | original | 50 | 0 | 0 | 0 |
| [rank-invert-original](resultados/rank-invert-original-p1.log) | 1 | original | 28 | 18 | 4 | 0 |
| [rank-invert-restart](resultados/rank-invert-restart-p0.log) | 0 | temporário true | 50 | 0 | 0 | 0 |
| [rank-invert-restart](resultados/rank-invert-restart-p1.log) | 1 | temporário true | 28 | 18 | 4 | 0 |
| [rank-repair-restart](resultados/rank-repair-restart-p0.log) | 0 | temporário true | 28 | 18 | 4 | 0 |
| [rank-repair-restart](resultados/rank-repair-restart-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [restart-fails-repair](resultados/restart-fails-repair-p0.log) | 0 | temporário true | 29 | 18 | 4 | 0 |
| [restart-fails-repair](resultados/restart-fails-repair-p1.log) | 1 | temporário true | 50 | 0 | 0 | 0 |
| [equal-rank-ai-first](resultados/equal-rank-ai-first-p0.log) | 0 | temporário true | 49 | 0 | 0 | 0 |
| [equal-rank-dual-first](resultados/equal-rank-dual-first-p0.log) | 0 | temporário true | 27 | 18 | 4 | 0 |

A fase 0 é a primeira inicialização do caso. Fases seguintes são novos processos lendo os mesmos settings; PIDs e conteúdo persistido no JSON. O pedido da plataforma é observado por stub; o runner realiza o reinício do harness.

FAILs: 135 invariantes de controller/gate em nove suítes inseguras + 23 probes de cadeia. BLOCKED: limpeza quando o gate foi contornado. SKIP: suíte controller quando os dois MODs não estavam ativos; os caminhos de materialização OFF foram exercitados.

[Respostas A–E e limites](../HAL22_LIFECYCLE_RESTART.md) · [CSV](resultados/MATRIZ.csv) · [Resultados/comandos](resultados/RESULTADOS.json)

Não é gameplay Windows nem comprovação de alcançabilidade com decks reais. A adoção permanente do manifest exige autorização.
