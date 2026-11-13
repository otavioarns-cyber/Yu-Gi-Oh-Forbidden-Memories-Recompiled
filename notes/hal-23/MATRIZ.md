# HAL-23 — matriz de diagnóstico

228 processos: **1018 PASS, 52 FAIL de invariantes diagnósticas, 0 execução incompleta**. Os FAIL permanecem visíveis; não são 52 bugs distintos. Todos dependem da janela reentrante inserida no teste.

| Coorte | Processos | PASS | FAIL | Pending abandonado na primeira ação |
|---|---:|---:|---:|---:|
| Manifest original | 120 | 472 | 52 | 44 |
| Candidato restart=true | 108 | 546 | 0 | 0 |
| Fronteira antes do planner | 24 | 124 | 0 | 0 |
| Fronteira depois do tick | 42 | 236 | 0 | 0 |
| Gap instrumentado | 150 | 646 | 52 | 44 |
| Processo novo (somente BSS/settings) | 12 | 12 | 0 | 0 |

As coortes acima se sobrepõem. `phase=1` verifica inicialização de biblioteca e settings em processo novo; não simula a retomada do duelo.

| Invariante que falhou | Ocorrências |
|---|---:|
| `pending-consumed-by-materializer` | 44 |
| `next-plan-physical-selection-preserved` | 4 |
| `new-script-init-clears-pending` | 2 |
| `new-deal-clears-pending` | 2 |

O0 = H,D,RD; O1 = H,RD,D; O2 = D,RD,H (ordem inicial de registro, não cadeia externa). DMC está ON e antes de RD; observador próprio depois dos três. Primeira carga tardia pode registrar H por último mesmo em O0/O1.

Casos sem pending criado são controles negativos; não contam como reprodução da HAL-23. `next` inclui ponte de placement/próxima mão modelada. `cancel` chama AiScript_Init e não representa cancelamento de turno da IA pela UI.

| Caso | Fase | PASS | FAIL | Resultado pendente | Log |
|---|---:|---:|---:|---|---|
| `O0-d0-r0-apply-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O0-d0-r0-apply-gap-off-on-p0-none.log) |
| `O0-d0-r0-apply-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d0-r0-apply-gap-off-on-p1-none.log) |
| `O0-d0-r0-apply-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d0-r0-apply-gap-on-off-p0-none.log) |
| `O0-d0-r0-direct-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O0-d0-r0-direct-gap-off-on-p0-none.log) |
| `O0-d0-r0-direct-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d0-r0-direct-gap-off-on-p1-none.log) |
| `O0-d0-r0-direct-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d0-r0-direct-gap-on-off-p0-none.log) |
| `O0-d0-r0-reload-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O0-d0-r0-reload-gap-off-on-p0-none.log) |
| `O0-d0-r0-reload-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d0-r0-reload-gap-off-on-p1-none.log) |
| `O0-d0-r0-reload-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d0-r0-reload-gap-on-off-p0-none.log) |
| `O0-d0-r1-apply-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-apply-gap-off-on-p0-none.log) |
| `O0-d0-r1-apply-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-apply-gap-off-on-p1-none.log) |
| `O0-d0-r1-apply-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-apply-gap-on-off-p0-none.log) |
| `O0-d0-r1-direct-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-direct-gap-off-on-p0-none.log) |
| `O0-d0-r1-direct-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-direct-gap-off-on-p1-none.log) |
| `O0-d0-r1-direct-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-direct-gap-on-off-p0-none.log) |
| `O0-d0-r1-reload-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-reload-gap-off-on-p0-none.log) |
| `O0-d0-r1-reload-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-reload-gap-off-on-p1-none.log) |
| `O0-d0-r1-reload-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r1-reload-gap-on-off-p0-none.log) |
| `O0-d1-r0-apply-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O0-d1-r0-apply-gap-off-on-p0-none.log) |
| `O0-d1-r0-apply-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d1-r0-apply-gap-off-on-p1-none.log) |
| `O0-d1-r0-apply-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d1-r0-apply-gap-on-off-p0-none.log) |
| `O0-d1-r0-direct-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O0-d1-r0-direct-gap-off-on-p0-none.log) |
| `O0-d1-r0-direct-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d1-r0-direct-gap-off-on-p1-none.log) |
| `O0-d1-r0-direct-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d1-r0-direct-gap-on-off-p0-none.log) |
| `O0-d1-r0-reload-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O0-d1-r0-reload-gap-off-on-p0-none.log) |
| `O0-d1-r0-reload-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d1-r0-reload-gap-off-on-p1-none.log) |
| `O0-d1-r0-reload-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O0-d1-r0-reload-gap-on-off-p0-none.log) |
| `O0-d1-r1-apply-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-apply-gap-off-on-p0-none.log) |
| `O0-d1-r1-apply-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-apply-gap-off-on-p1-none.log) |
| `O0-d1-r1-apply-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-apply-gap-on-off-p0-none.log) |
| `O0-d1-r1-direct-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-direct-gap-off-on-p0-none.log) |
| `O0-d1-r1-direct-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-direct-gap-off-on-p1-none.log) |
| `O0-d1-r1-direct-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-direct-gap-on-off-p0-none.log) |
| `O0-d1-r1-reload-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-reload-gap-off-on-p0-none.log) |
| `O0-d1-r1-reload-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-reload-gap-off-on-p1-none.log) |
| `O0-d1-r1-reload-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r1-reload-gap-on-off-p0-none.log) |
| `O1-d0-r0-apply-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O1-d0-r0-apply-gap-off-on-p0-none.log) |
| `O1-d0-r0-apply-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d0-r0-apply-gap-off-on-p1-none.log) |
| `O1-d0-r0-apply-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d0-r0-apply-gap-on-off-p0-none.log) |
| `O1-d0-r0-direct-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O1-d0-r0-direct-gap-off-on-p0-none.log) |
| `O1-d0-r0-direct-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d0-r0-direct-gap-off-on-p1-none.log) |
| `O1-d0-r0-direct-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d0-r0-direct-gap-on-off-p0-none.log) |
| `O1-d0-r0-reload-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O1-d0-r0-reload-gap-off-on-p0-none.log) |
| `O1-d0-r0-reload-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d0-r0-reload-gap-off-on-p1-none.log) |
| `O1-d0-r0-reload-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d0-r0-reload-gap-on-off-p0-none.log) |
| `O1-d0-r1-apply-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-apply-gap-off-on-p0-none.log) |
| `O1-d0-r1-apply-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-apply-gap-off-on-p1-none.log) |
| `O1-d0-r1-apply-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-apply-gap-on-off-p0-none.log) |
| `O1-d0-r1-direct-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-direct-gap-off-on-p0-none.log) |
| `O1-d0-r1-direct-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-direct-gap-off-on-p1-none.log) |
| `O1-d0-r1-direct-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-direct-gap-on-off-p0-none.log) |
| `O1-d0-r1-reload-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-reload-gap-off-on-p0-none.log) |
| `O1-d0-r1-reload-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-reload-gap-off-on-p1-none.log) |
| `O1-d0-r1-reload-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r1-reload-gap-on-off-p0-none.log) |
| `O1-d1-r0-apply-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O1-d1-r0-apply-gap-off-on-p0-none.log) |
| `O1-d1-r0-apply-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d1-r0-apply-gap-off-on-p1-none.log) |
| `O1-d1-r0-apply-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d1-r0-apply-gap-on-off-p0-none.log) |
| `O1-d1-r0-direct-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O1-d1-r0-direct-gap-off-on-p0-none.log) |
| `O1-d1-r0-direct-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d1-r0-direct-gap-off-on-p1-none.log) |
| `O1-d1-r0-direct-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d1-r0-direct-gap-on-off-p0-none.log) |
| `O1-d1-r0-reload-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O1-d1-r0-reload-gap-off-on-p0-none.log) |
| `O1-d1-r0-reload-gap-off-on-p1-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d1-r0-reload-gap-off-on-p1-none.log) |
| `O1-d1-r0-reload-gap-on-off-p0-none` | 0 | 4 | 0 | consumido | [log](resultados/O1-d1-r0-reload-gap-on-off-p0-none.log) |
| `O1-d1-r1-apply-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-apply-gap-off-on-p0-none.log) |
| `O1-d1-r1-apply-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-apply-gap-off-on-p1-none.log) |
| `O1-d1-r1-apply-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-apply-gap-on-off-p0-none.log) |
| `O1-d1-r1-direct-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-direct-gap-off-on-p0-none.log) |
| `O1-d1-r1-direct-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-direct-gap-off-on-p1-none.log) |
| `O1-d1-r1-direct-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-direct-gap-on-off-p0-none.log) |
| `O1-d1-r1-reload-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-reload-gap-off-on-p0-none.log) |
| `O1-d1-r1-reload-gap-off-on-p1-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-reload-gap-off-on-p1-none.log) |
| `O1-d1-r1-reload-gap-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r1-reload-gap-on-off-p0-none.log) |
| `O2-d0-r0-apply-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-none.log) |
| `O2-d0-r0-apply-gap-off-on-p1-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p1-none.log) |
| `O2-d0-r0-apply-gap-on-off-p0-none` | 0 | 3 | 0 | sem pending criado | [log](resultados/O2-d0-r0-apply-gap-on-off-p0-none.log) |
| `O2-d0-r0-direct-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-direct-gap-off-on-p0-none.log) |
| `O2-d0-r0-direct-gap-off-on-p1-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-direct-gap-off-on-p1-none.log) |
| `O2-d0-r0-direct-gap-on-off-p0-none` | 0 | 3 | 0 | sem pending criado | [log](resultados/O2-d0-r0-direct-gap-on-off-p0-none.log) |
| `O2-d0-r0-reload-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-reload-gap-off-on-p0-none.log) |
| `O2-d0-r0-reload-gap-off-on-p1-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-reload-gap-off-on-p1-none.log) |
| `O2-d0-r0-reload-gap-on-off-p0-none` | 0 | 3 | 0 | sem pending criado | [log](resultados/O2-d0-r0-reload-gap-on-off-p0-none.log) |
| `O2-d0-r1-apply-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-none.log) |
| `O2-d0-r1-apply-gap-off-on-p1-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r1-apply-gap-off-on-p1-none.log) |
| `O2-d0-r1-apply-gap-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r1-apply-gap-on-off-p0-none.log) |
| `O2-d0-r1-direct-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r1-direct-gap-off-on-p0-none.log) |
| `O2-d0-r1-direct-gap-off-on-p1-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r1-direct-gap-off-on-p1-none.log) |
| `O2-d0-r1-direct-gap-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r1-direct-gap-on-off-p0-none.log) |
| `O2-d0-r1-reload-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r1-reload-gap-off-on-p0-none.log) |
| `O2-d0-r1-reload-gap-off-on-p1-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r1-reload-gap-off-on-p1-none.log) |
| `O2-d0-r1-reload-gap-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r1-reload-gap-on-off-p0-none.log) |
| `O2-d1-r0-apply-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-none.log) |
| `O2-d1-r0-apply-gap-off-on-p1-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p1-none.log) |
| `O2-d1-r0-apply-gap-on-off-p0-none` | 0 | 3 | 0 | sem pending criado | [log](resultados/O2-d1-r0-apply-gap-on-off-p0-none.log) |
| `O2-d1-r0-direct-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-direct-gap-off-on-p0-none.log) |
| `O2-d1-r0-direct-gap-off-on-p1-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-direct-gap-off-on-p1-none.log) |
| `O2-d1-r0-direct-gap-on-off-p0-none` | 0 | 3 | 0 | sem pending criado | [log](resultados/O2-d1-r0-direct-gap-on-off-p0-none.log) |
| `O2-d1-r0-reload-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-reload-gap-off-on-p0-none.log) |
| `O2-d1-r0-reload-gap-off-on-p1-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-reload-gap-off-on-p1-none.log) |
| `O2-d1-r0-reload-gap-on-off-p0-none` | 0 | 3 | 0 | sem pending criado | [log](resultados/O2-d1-r0-reload-gap-on-off-p0-none.log) |
| `O2-d1-r1-apply-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-none.log) |
| `O2-d1-r1-apply-gap-off-on-p1-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r1-apply-gap-off-on-p1-none.log) |
| `O2-d1-r1-apply-gap-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r1-apply-gap-on-off-p0-none.log) |
| `O2-d1-r1-direct-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r1-direct-gap-off-on-p0-none.log) |
| `O2-d1-r1-direct-gap-off-on-p1-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r1-direct-gap-off-on-p1-none.log) |
| `O2-d1-r1-direct-gap-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r1-direct-gap-on-off-p0-none.log) |
| `O2-d1-r1-reload-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r1-reload-gap-off-on-p0-none.log) |
| `O2-d1-r1-reload-gap-off-on-p1-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r1-reload-gap-off-on-p1-none.log) |
| `O2-d1-r1-reload-gap-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r1-reload-gap-on-off-p0-none.log) |
| `O0-d0-r0-apply-before-off-on-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O0-d0-r0-apply-before-off-on-p0-none.log) |
| `O0-d0-r0-apply-before-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r0-apply-before-on-off-p0-none.log) |
| `O0-d0-r0-apply-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r0-apply-after-off-on-p0-none.log) |
| `O0-d0-r0-apply-after-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d0-r0-apply-after-on-off-p0-none.log) |
| `O0-d0-r1-apply-before-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d0-r1-apply-before-off-on-p0-none.log) |
| `O0-d0-r1-apply-before-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d0-r1-apply-before-on-off-p0-none.log) |
| `O0-d0-r1-apply-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d0-r1-apply-after-off-on-p0-none.log) |
| `O0-d0-r1-apply-after-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d0-r1-apply-after-on-off-p0-none.log) |
| `O0-d1-r0-apply-before-off-on-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O0-d1-r0-apply-before-off-on-p0-none.log) |
| `O0-d1-r0-apply-before-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r0-apply-before-on-off-p0-none.log) |
| `O0-d1-r0-apply-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r0-apply-after-off-on-p0-none.log) |
| `O0-d1-r0-apply-after-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O0-d1-r0-apply-after-on-off-p0-none.log) |
| `O0-d1-r1-apply-before-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d1-r1-apply-before-off-on-p0-none.log) |
| `O0-d1-r1-apply-before-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d1-r1-apply-before-on-off-p0-none.log) |
| `O0-d1-r1-apply-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d1-r1-apply-after-off-on-p0-none.log) |
| `O0-d1-r1-apply-after-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O0-d1-r1-apply-after-on-off-p0-none.log) |
| `O1-d0-r0-apply-before-off-on-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O1-d0-r0-apply-before-off-on-p0-none.log) |
| `O1-d0-r0-apply-before-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r0-apply-before-on-off-p0-none.log) |
| `O1-d0-r0-apply-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r0-apply-after-off-on-p0-none.log) |
| `O1-d0-r0-apply-after-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d0-r0-apply-after-on-off-p0-none.log) |
| `O1-d0-r1-apply-before-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d0-r1-apply-before-off-on-p0-none.log) |
| `O1-d0-r1-apply-before-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d0-r1-apply-before-on-off-p0-none.log) |
| `O1-d0-r1-apply-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d0-r1-apply-after-off-on-p0-none.log) |
| `O1-d0-r1-apply-after-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d0-r1-apply-after-on-off-p0-none.log) |
| `O1-d1-r0-apply-before-off-on-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O1-d1-r0-apply-before-off-on-p0-none.log) |
| `O1-d1-r0-apply-before-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r0-apply-before-on-off-p0-none.log) |
| `O1-d1-r0-apply-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r0-apply-after-off-on-p0-none.log) |
| `O1-d1-r0-apply-after-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O1-d1-r0-apply-after-on-off-p0-none.log) |
| `O1-d1-r1-apply-before-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d1-r1-apply-before-off-on-p0-none.log) |
| `O1-d1-r1-apply-before-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d1-r1-apply-before-on-off-p0-none.log) |
| `O1-d1-r1-apply-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d1-r1-apply-after-off-on-p0-none.log) |
| `O1-d1-r1-apply-after-on-off-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O1-d1-r1-apply-after-on-off-p0-none.log) |
| `O2-d0-r0-apply-before-off-on-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r0-apply-before-off-on-p0-none.log) |
| `O2-d0-r0-apply-before-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r0-apply-before-on-off-p0-none.log) |
| `O2-d0-r0-apply-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r0-apply-after-off-on-p0-none.log) |
| `O2-d0-r0-apply-after-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d0-r0-apply-after-on-off-p0-none.log) |
| `O2-d0-r1-apply-before-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-apply-before-off-on-p0-none.log) |
| `O2-d0-r1-apply-before-on-off-p0-none` | 0 | 5 | 0 | sem pending criado | [log](resultados/O2-d0-r1-apply-before-on-off-p0-none.log) |
| `O2-d0-r1-apply-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-apply-after-off-on-p0-none.log) |
| `O2-d0-r1-apply-after-on-off-p0-none` | 0 | 5 | 0 | sem pending criado | [log](resultados/O2-d0-r1-apply-after-on-off-p0-none.log) |
| `O2-d1-r0-apply-before-off-on-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r0-apply-before-off-on-p0-none.log) |
| `O2-d1-r0-apply-before-on-off-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r0-apply-before-on-off-p0-none.log) |
| `O2-d1-r0-apply-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r0-apply-after-off-on-p0-none.log) |
| `O2-d1-r0-apply-after-on-off-p0-none` | 0 | 4 | 0 | sem pending criado | [log](resultados/O2-d1-r0-apply-after-on-off-p0-none.log) |
| `O2-d1-r1-apply-before-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-apply-before-off-on-p0-none.log) |
| `O2-d1-r1-apply-before-on-off-p0-none` | 0 | 5 | 0 | sem pending criado | [log](resultados/O2-d1-r1-apply-before-on-off-p0-none.log) |
| `O2-d1-r1-apply-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-apply-after-off-on-p0-none.log) |
| `O2-d1-r1-apply-after-on-off-p0-none` | 0 | 5 | 0 | sem pending criado | [log](resultados/O2-d1-r1-apply-after-on-off-p0-none.log) |
| `O2-d0-r0-window-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-window-gap-off-on-p0-none.log) |
| `O2-d0-r0-window-gap-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d0-r0-window-gap-off-on-p0-none-reboot.log) |
| `O2-d0-r0-window-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r0-window-after-off-on-p0-none.log) |
| `O2-d0-r0-window-after-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d0-r0-window-after-off-on-p0-none-reboot.log) |
| `O2-d0-r0-window-cancel-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-window-cancel-gap-off-on-p0-none.log) |
| `O2-d0-r0-window-cancel-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r0-window-cancel-after-off-on-p0-none.log) |
| `O2-d0-r0-window-fail-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d0-r0-window-fail-gap-off-on-p0-none.log) |
| `O2-d0-r0-window-fail-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r0-window-fail-after-off-on-p0-none.log) |
| `O2-d0-r1-window-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r1-window-gap-off-on-p0-none.log) |
| `O2-d0-r1-window-gap-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d0-r1-window-gap-off-on-p0-none-reboot.log) |
| `O2-d0-r1-window-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-window-after-off-on-p0-none.log) |
| `O2-d0-r1-window-after-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d0-r1-window-after-off-on-p0-none-reboot.log) |
| `O2-d0-r1-window-cancel-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r1-window-cancel-gap-off-on-p0-none.log) |
| `O2-d0-r1-window-cancel-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-window-cancel-after-off-on-p0-none.log) |
| `O2-d0-r1-window-fail-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d0-r1-window-fail-gap-off-on-p0-none.log) |
| `O2-d0-r1-window-fail-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-window-fail-after-off-on-p0-none.log) |
| `O2-d1-r0-window-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-window-gap-off-on-p0-none.log) |
| `O2-d1-r0-window-gap-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d1-r0-window-gap-off-on-p0-none-reboot.log) |
| `O2-d1-r0-window-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r0-window-after-off-on-p0-none.log) |
| `O2-d1-r0-window-after-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d1-r0-window-after-off-on-p0-none-reboot.log) |
| `O2-d1-r0-window-cancel-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-window-cancel-gap-off-on-p0-none.log) |
| `O2-d1-r0-window-cancel-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r0-window-cancel-after-off-on-p0-none.log) |
| `O2-d1-r0-window-fail-gap-off-on-p0-none` | 0 | 3 | 1 | abandonado | [log](resultados/O2-d1-r0-window-fail-gap-off-on-p0-none.log) |
| `O2-d1-r0-window-fail-after-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r0-window-fail-after-off-on-p0-none.log) |
| `O2-d1-r1-window-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r1-window-gap-off-on-p0-none.log) |
| `O2-d1-r1-window-gap-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d1-r1-window-gap-off-on-p0-none-reboot.log) |
| `O2-d1-r1-window-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-window-after-off-on-p0-none.log) |
| `O2-d1-r1-window-after-off-on-p0-none` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d1-r1-window-after-off-on-p0-none-reboot.log) |
| `O2-d1-r1-window-cancel-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r1-window-cancel-gap-off-on-p0-none.log) |
| `O2-d1-r1-window-cancel-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-window-cancel-after-off-on-p0-none.log) |
| `O2-d1-r1-window-fail-gap-off-on-p0-none` | 0 | 5 | 0 | consumido | [log](resultados/O2-d1-r1-window-fail-gap-off-on-p0-none.log) |
| `O2-d1-r1-window-fail-after-off-on-p0-none` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-window-fail-after-off-on-p0-none.log) |
| `O2-d0-r0-apply-gap-off-on-p0-next` | 0 | 4 | 2 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-next.log) |
| `O2-d0-r0-apply-gap-off-on-p0-reset` | 0 | 4 | 1 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-reset.log) |
| `O2-d0-r0-apply-gap-off-on-p0-cancel` | 0 | 3 | 2 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-cancel.log) |
| `O2-d0-r0-apply-gap-off-on-p0-deal` | 0 | 4 | 1 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-deal.log) |
| `O2-d0-r0-apply-gap-off-on-p0-deal-unregistered` | 0 | 3 | 2 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-deal-unregistered.log) |
| `O2-d0-r0-apply-gap-off-on-p0-save` | 0 | 5 | 1 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-save.log) |
| `O2-d0-r0-apply-gap-off-on-p0-save` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d0-r0-apply-gap-off-on-p0-save-reboot.log) |
| `O2-d0-r1-apply-gap-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-next.log) |
| `O2-d0-r1-apply-gap-off-on-p0-reset` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-reset.log) |
| `O2-d0-r1-apply-gap-off-on-p0-cancel` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-cancel.log) |
| `O2-d0-r1-apply-gap-off-on-p0-deal` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-deal.log) |
| `O2-d0-r1-apply-gap-off-on-p0-deal-unregistered` | 0 | 6 | 0 | consumido | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-deal-unregistered.log) |
| `O2-d0-r1-apply-gap-off-on-p0-save` | 0 | 7 | 0 | consumido | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-save.log) |
| `O2-d0-r1-apply-gap-off-on-p0-save` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d0-r1-apply-gap-off-on-p0-save-reboot.log) |
| `O2-d1-r0-apply-gap-off-on-p0-next` | 0 | 4 | 2 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-next.log) |
| `O2-d1-r0-apply-gap-off-on-p0-reset` | 0 | 4 | 1 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-reset.log) |
| `O2-d1-r0-apply-gap-off-on-p0-cancel` | 0 | 3 | 2 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-cancel.log) |
| `O2-d1-r0-apply-gap-off-on-p0-deal` | 0 | 4 | 1 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-deal.log) |
| `O2-d1-r0-apply-gap-off-on-p0-deal-unregistered` | 0 | 3 | 2 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-deal-unregistered.log) |
| `O2-d1-r0-apply-gap-off-on-p0-save` | 0 | 5 | 1 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-save.log) |
| `O2-d1-r0-apply-gap-off-on-p0-save` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d1-r0-apply-gap-off-on-p0-save-reboot.log) |
| `O2-d1-r1-apply-gap-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-next.log) |
| `O2-d1-r1-apply-gap-off-on-p0-reset` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-reset.log) |
| `O2-d1-r1-apply-gap-off-on-p0-cancel` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-cancel.log) |
| `O2-d1-r1-apply-gap-off-on-p0-deal` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-deal.log) |
| `O2-d1-r1-apply-gap-off-on-p0-deal-unregistered` | 0 | 6 | 0 | consumido | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-deal-unregistered.log) |
| `O2-d1-r1-apply-gap-off-on-p0-save` | 0 | 7 | 0 | consumido | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-save.log) |
| `O2-d1-r1-apply-gap-off-on-p0-save` | 1 | 1 | 0 | BSS novo | [log](resultados/O2-d1-r1-apply-gap-off-on-p0-save-reboot.log) |
| `O0-d0-r0-apply-after-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O0-d0-r0-apply-after-off-on-p0-next.log) |
| `O0-d0-r0-apply-gap-off-on-p1-next` | 0 | 6 | 0 | consumido | [log](resultados/O0-d0-r0-apply-gap-off-on-p1-next.log) |
| `O0-d1-r0-apply-after-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O0-d1-r0-apply-after-off-on-p0-next.log) |
| `O0-d1-r0-apply-gap-off-on-p1-next` | 0 | 6 | 0 | consumido | [log](resultados/O0-d1-r0-apply-gap-off-on-p1-next.log) |
| `O1-d0-r0-apply-after-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O1-d0-r0-apply-after-off-on-p0-next.log) |
| `O1-d0-r0-apply-gap-off-on-p1-next` | 0 | 6 | 0 | consumido | [log](resultados/O1-d0-r0-apply-gap-off-on-p1-next.log) |
| `O1-d1-r0-apply-after-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O1-d1-r0-apply-after-off-on-p0-next.log) |
| `O1-d1-r0-apply-gap-off-on-p1-next` | 0 | 6 | 0 | consumido | [log](resultados/O1-d1-r0-apply-gap-off-on-p1-next.log) |
| `O2-d0-r0-apply-after-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O2-d0-r0-apply-after-off-on-p0-next.log) |
| `O2-d0-r0-apply-gap-off-on-p1-next` | 0 | 4 | 2 | abandonado | [log](resultados/O2-d0-r0-apply-gap-off-on-p1-next.log) |
| `O2-d1-r0-apply-after-off-on-p0-next` | 0 | 7 | 0 | consumido | [log](resultados/O2-d1-r0-apply-after-off-on-p0-next.log) |
| `O2-d1-r0-apply-gap-off-on-p1-next` | 0 | 4 | 2 | abandonado | [log](resultados/O2-d1-r0-apply-gap-off-on-p1-next.log) |

[CSV de cada verificação](resultados/MATRIZ.csv) · [JSON integral](resultados/RESULTADOS.json) · [Limites e respostas A–E](../HAL23_PENDING_LIFECYCLE.md).

| Cenário adicional | Estado | Limitação |
|---|---|---|
| UI/duelo completo Windows i386 | BLOCKED | Sem imagem legal local e runtime gráfico em execução; QEMU não comprova gameplay. |
| VM retail/catalog/decks reais | NOT_RUN | Proposta vanilla e catálogo são modelos explícitos; planner RD e AI oficial são objetos reais. |
| Ritual summon e consumo de três tributos | NOT_RUN | Esta reprodução prepara/seta a magia 680; não executa uma invocação ritual completa. |
| Cancelar ação de IA pela UI | NOT_REPRODUCED | AiScript_Init testado separadamente; não foi encontrado evento normal entre planner e swap. |
| Savestate completo/antigo/entre perfis | NOT_RUN | Somente blocos registrados e callbacks reais de reset/load, com perfil idêntico. |
| Novo duelo completo | NOT_RUN | Chamadas reais ao dealer de Rex e fallback Isis; restante do novo duelo não executado. |
| Integração de todos os MODs / correção I2 do PR17 | NOT_RUN | Não incorporada; esta branch conserva produção oficial. |
