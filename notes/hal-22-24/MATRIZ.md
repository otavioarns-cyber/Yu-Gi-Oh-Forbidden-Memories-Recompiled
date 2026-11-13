# HAL-22 + HAL-24 — matriz da PoC conjunta

As coortes protegidas não têm FAIL. Recusa antes do controller é um resultado esperado, distinto de gameplay executado. Os controles OFF preservam falhas conhecidas. A matriz principal não injeta planos, pending ou quotas; a suíte do par mantém controles adversariais separados.

| Coorte | Processos | PASS | FAIL | BLOCKED | SKIP |
|---|---:|---:|---:|---:|---:|
| main_protected | 204 | 1224 | 0 | 0 | 0 |
| main_controls | 12 | 105 | 13 | 0 | 0 |
| pair_protected | 51 | 1834 | 0 | 0 | 24 |
| pair_controls | 45 | 1908 | 158 | 36 | 9 |

Principal protegida:106 processos admitidos,98 recusados. Admissão não significa execução de bytecode retail/duelo completo. Alguns singles sem planner disponível não produzem ação. Par protegido:24 SKIP de composição individual/recusada; controles OFF:36 BLOCKED de limpeza cujo gate não rejeitou. Não há execução incompleta.

| Composição aplicada | Ordem inicial exigida | Resultado/limite |
|---|---|---|
| H+RD+D | H→RD→D | D externo antes de qualquer worker; versões qualificadas e sem wrappers desconhecidos nos alvos protegidos. |
| H+D | H→D | Matriz HAL22 e regressões de quotas/materialização passaram. |
| RD+D | RD→D | Cenários RD/controller do PR19 passaram. |
| H+RD sem D | Sem restrição de quota Dual | Não é correção da HAL23; lifecycle entre planner/swap não foi alterado. |
| Apenas um | Sem restrição de ordem conjunta | Inicialização/admissão independentes; retail VM permanece não executada. |
| Wrapper desconhecido em swap/controller da mão | Composição não qualificada | Recusada, inclusive wrapper transparente; nenhum hook é reordenado. |
| Wrapper em alvo não relacionado | Ordem original | Callback observado normalmente, sem bloqueio adicional. |
| Empates | Somente ordem resolvida correta | Um empate seguro admitido, quatro resoluções inseguras recusadas. |

3072 ordenações reais de manifests × guard OFF/ON, mais 3072 validações de ranks staged; 2304 aceitas/768 recusadas. Ordem geral idêntica; 1920 configurações sem par de quotas não receberam restrição. Três regressões entre perfis passaram, incluindo suíte oficial de hooks e igualdade das primeiras escolhas do AI.

## Principal — todos os processos

| Perfil | Fase | Guard | Admissão | PASS | FAIL | Log |
|---|---|---|---|---:|---:|---|
| `g1-O0-h0-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h0-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h0-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h0-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h0-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h0-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h0-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h0-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h0-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h0-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h0-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h0-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h0-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h0-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h0-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h0-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h0-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h0-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 12 | 0 | [log](resultados/g1-O1-h0-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h1-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h1-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h1-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h1-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h1-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h1-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h0-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h0-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h0-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h0-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h0-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h0-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d0-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d0-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d0-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d0-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d0-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v0-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v1-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v2-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v3-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v4-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h0-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O1-h0-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O1-h0-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O1-h0-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O1-h1-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O1-h1-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O1-h1-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h0-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v6-none-restart1-field-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v7-none-restart1-hand-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v8-none-restart1-both-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-cycle-ai-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-cycle-ai-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-cycle-ai-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-cycle-ai-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-cycle-dual-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-cycle-dual-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-cycle-dual-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-cycle-dual-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-cycle-rd-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-cycle-rd-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-cycle-rd-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-cycle-rd-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-reload-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-reload-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-reload-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-reload-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 13 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 12 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 12 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-cycle-ai-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-cycle-ai-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-cycle-ai-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-cycle-ai-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-cycle-dual-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-cycle-dual-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-cycle-dual-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-cycle-dual-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-cycle-rd-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-cycle-rd-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-cycle-rd-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-cycle-rd-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-reload-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-reload-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-reload-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-reload-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-ai-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O2-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-ai-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O2-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-dual-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O2-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-dual-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O2-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50` | reboot | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-rd-restart0-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O2-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-late-rd-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O2-h1-r1-d1-v5-cycle-ai-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-cycle-ai-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-cycle-ai-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-cycle-ai-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-cycle-dual-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-cycle-dual-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-cycle-dual-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-cycle-dual-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-cycle-rd-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-cycle-rd-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-cycle-rd-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-cycle-rd-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-reload-restart0-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-reload-restart0-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-reload-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-reload-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h0-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h0-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 12 | 0 | [log](resultados/g1-O0-h0-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O0-h1-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O0-h1-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h0-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h0-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 12 | 0 | [log](resultados/g1-O1-h0-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O1-h1-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h0-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h0-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 12 | 0 | [log](resultados/g1-O2-h0-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 5 | 0 | [log](resultados/g1-O2-h1-r0-d0-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O2-h1-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r0-d1-v5-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x1-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x1-p50.log) |
| `g1-O2-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x1-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x1-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x2-p50` | main | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t1-wnone-x2-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wlate-swallow-x0-p50` | main | 1 | admitido | 15 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wlate-swallow-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wlate-transparent-x0-p50` | main | 1 | admitido | 15 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wlate-transparent-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p1` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p1.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p1` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p1.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wcontroller-x0-p1` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wcontroller-x0-p1.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p3` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p3.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p3` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p3.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wcontroller-x0-p3` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wcontroller-x0-p3.log) |
| `g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wunrelated-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-none-restart1-absent-t0-wunrelated-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wunrelated-x0-p50` | main | 1 | admitido | 15 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-none-restart1-absent-t0-wunrelated-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wtransparent-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wswallow-x0-p50.log) |
| `g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wunrelated-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O2-h1-r1-d1-v5-none-restart1-absent-t0-wunrelated-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-window-invert-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 15 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-window-invert-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-window-ai-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 12 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-window-ai-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-window-ai-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-window-ai-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-window-ai-cancel-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 12 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-window-ai-cancel-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-window-ai-cancel-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 12 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-window-ai-cancel-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O1-h1-r1-d1-v5-window-ai-fail-restart1-absent-t0-wnone-x0-p50` | main | 1 | admitido | 13 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-window-ai-fail-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O1-h1-r1-d1-v5-window-ai-fail-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O1-h1-r1-d1-v5-window-ai-fail-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-window-repair-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-window-repair-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-window-repair-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-window-repair-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-window-repair-cancel-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-window-repair-cancel-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-window-repair-cancel-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | recusado | 2 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-window-repair-cancel-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g1-O0-h1-r1-d1-v5-window-repair-fail-restart1-absent-t0-wnone-x0-p50` | main | 1 | recusado | 3 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-window-repair-fail-restart1-absent-t0-wnone-x0-p50.log) |
| `g1-O0-h1-r1-d1-v5-window-repair-fail-restart1-absent-t0-wnone-x0-p50` | reboot | 1 | admitido | 14 | 0 | [log](resultados/g1-O0-h1-r1-d1-v5-window-repair-fail-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g0-O0-h1-r1-d1-v5-arm-none-restart1-absent-t0-wnone-x0-p50` | main | ON após legacy | recusado | 2 | 0 | [log](resultados/g0-O0-h1-r1-d1-v5-arm-none-restart1-absent-t0-wnone-x0-p50.log) |
| `g0-O0-h1-r1-d1-v5-arm-window-repair-restart1-absent-t0-wnone-x0-p50` | main | ON após legacy | recusado | 2 | 0 | [log](resultados/g0-O0-h1-r1-d1-v5-arm-window-repair-restart1-absent-t0-wnone-x0-p50.log) |
| `g0-O0-h1-r1-d1-v5-arm-window-repair-restart1-absent-t0-wnone-x0-p50` | reboot | ON após legacy | admitido | 14 | 0 | [log](resultados/g0-O0-h1-r1-d1-v5-arm-window-repair-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g0-O0-h1-r1-d1-v5-arm-window-repair-cancel-restart1-absent-t0-wnone-x0-p50` | main | ON após legacy | recusado | 2 | 0 | [log](resultados/g0-O0-h1-r1-d1-v5-arm-window-repair-cancel-restart1-absent-t0-wnone-x0-p50.log) |
| `g0-O0-h1-r1-d1-v5-arm-window-repair-cancel-restart1-absent-t0-wnone-x0-p50` | reboot | ON após legacy | recusado | 2 | 0 | [log](resultados/g0-O0-h1-r1-d1-v5-arm-window-repair-cancel-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g0-O0-h1-r1-d1-v5-arm-window-repair-fail-restart1-absent-t0-wnone-x0-p50` | main | ON após legacy | recusado | 3 | 0 | [log](resultados/g0-O0-h1-r1-d1-v5-arm-window-repair-fail-restart1-absent-t0-wnone-x0-p50.log) |
| `g0-O0-h1-r1-d1-v5-arm-window-repair-fail-restart1-absent-t0-wnone-x0-p50` | reboot | ON após legacy | admitido | 14 | 0 | [log](resultados/g0-O0-h1-r1-d1-v5-arm-window-repair-fail-restart1-absent-t0-wnone-x0-p50-reboot.log) |
| `g0-O0-h0-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 5 | 1 | [log](resultados/g0-O0-h0-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O0-h0-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 9 | 3 | [log](resultados/g0-O0-h0-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O0-h1-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 5 | 1 | [log](resultados/g0-O0-h1-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O0-h1-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 9 | 3 | [log](resultados/g0-O0-h1-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O1-h0-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 12 | 0 | [log](resultados/g0-O1-h0-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O1-h0-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 10 | 0 | [log](resultados/g0-O1-h0-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O1-h1-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 12 | 0 | [log](resultados/g0-O1-h1-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O1-h1-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 12 | 0 | [log](resultados/g0-O1-h1-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O2-h0-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 5 | 1 | [log](resultados/g0-O2-h0-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O2-h0-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 9 | 3 | [log](resultados/g0-O2-h0-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O2-h1-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 6 | 0 | [log](resultados/g0-O2-h1-r1-d1-v0-none-restart0-absent-t0-wnone-x0-p50.log) |
| `g0-O2-h1-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50` | main | 0 | admitido | 11 | 1 | [log](resultados/g0-O2-h1-r1-d1-v5-none-restart0-absent-t0-wnone-x0-p50.log) |

## Par HAL22 — todos os processos

| Perfil | Fase | Guard | PASS | FAIL | BLOCKED | SKIP | Log |
|---|---|---|---:|---:|---:|---:|---|
| `guard-off-safe-original` | 0 | False | 52 | 0 | 0 | 0 | [log](par-resultados/guard-off-safe-original-p0.log) |
| `guard-off-safe-restart` | 0 | False | 52 | 0 | 0 | 0 | [log](par-resultados/guard-off-safe-restart-p0.log) |
| `guard-off-late-ai-apply-original` | 0 | False | 32 | 17 | 4 | 0 | [log](par-resultados/guard-off-late-ai-apply-original-p0.log) |
| `guard-off-late-ai-apply-original` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-ai-apply-original-p1.log) |
| `guard-off-late-ai-direct-original` | 0 | False | 32 | 17 | 4 | 0 | [log](par-resultados/guard-off-late-ai-direct-original-p0.log) |
| `guard-off-late-ai-direct-original` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-ai-direct-original-p1.log) |
| `guard-off-late-ai-reload-original` | 0 | False | 32 | 17 | 4 | 0 | [log](par-resultados/guard-off-late-ai-reload-original-p0.log) |
| `guard-off-late-ai-reload-original` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-ai-reload-original-p1.log) |
| `guard-off-late-ai-apply-restart` | 0 | False | 15 | 0 | 0 | 1 | [log](par-resultados/guard-off-late-ai-apply-restart-p0.log) |
| `guard-off-late-ai-apply-restart` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-ai-apply-restart-p1.log) |
| `guard-off-late-ai-direct-restart` | 0 | False | 15 | 0 | 0 | 1 | [log](par-resultados/guard-off-late-ai-direct-restart-p0.log) |
| `guard-off-late-ai-direct-restart` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-ai-direct-restart-p1.log) |
| `guard-off-late-ai-reload-restart` | 0 | False | 15 | 0 | 0 | 1 | [log](par-resultados/guard-off-late-ai-reload-restart-p0.log) |
| `guard-off-late-ai-reload-restart` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-ai-reload-restart-p1.log) |
| `guard-off-late-dual-original` | 0 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-dual-original-p0.log) |
| `guard-off-late-dual-original` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-dual-original-p1.log) |
| `guard-off-late-dual-restart` | 0 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-dual-restart-p0.log) |
| `guard-off-late-dual-restart` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-late-dual-restart-p1.log) |
| `guard-off-off-on-original` | 0 | False | 73 | 0 | 0 | 0 | [log](par-resultados/guard-off-off-on-original-p0.log) |
| `guard-off-off-on-original` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-off-on-original-p1.log) |
| `guard-off-off-on-restart` | 0 | False | 73 | 0 | 0 | 0 | [log](par-resultados/guard-off-off-on-restart-p0.log) |
| `guard-off-off-on-restart` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-off-on-restart-p1.log) |
| `guard-off-persist-off-restart` | 0 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-persist-off-restart-p0.log) |
| `guard-off-persist-off-restart` | 1 | False | 16 | 0 | 0 | 1 | [log](par-resultados/guard-off-persist-off-restart-p1.log) |
| `guard-off-persist-off-restart` | 2 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-persist-off-restart-p2.log) |
| `guard-off-window-first-ai-original` | 0 | False | 32 | 17 | 4 | 0 | [log](par-resultados/guard-off-window-first-ai-original-p0.log) |
| `guard-off-window-first-ai-original` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-window-first-ai-original-p1.log) |
| `guard-off-window-first-ai-restart` | 0 | False | 15 | 0 | 0 | 1 | [log](par-resultados/guard-off-window-first-ai-restart-p0.log) |
| `guard-off-window-first-ai-restart` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-window-first-ai-restart-p1.log) |
| `guard-off-window-cancel-restart` | 0 | False | 15 | 0 | 0 | 1 | [log](par-resultados/guard-off-window-cancel-restart-p0.log) |
| `guard-off-window-cancel-restart` | 1 | False | 15 | 0 | 0 | 1 | [log](par-resultados/guard-off-window-cancel-restart-p1.log) |
| `guard-off-rank-invert-original` | 0 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-rank-invert-original-p0.log) |
| `guard-off-rank-invert-original` | 1 | False | 31 | 18 | 4 | 0 | [log](par-resultados/guard-off-rank-invert-original-p1.log) |
| `guard-off-rank-invert-restart` | 0 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-rank-invert-restart-p0.log) |
| `guard-off-rank-invert-restart` | 1 | False | 31 | 18 | 4 | 0 | [log](par-resultados/guard-off-rank-invert-restart-p1.log) |
| `guard-off-rank-repair-restart` | 0 | False | 31 | 18 | 4 | 0 | [log](par-resultados/guard-off-rank-repair-restart-p0.log) |
| `guard-off-rank-repair-restart` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-rank-repair-restart-p1.log) |
| `guard-off-restart-fails-repair` | 0 | False | 32 | 18 | 4 | 0 | [log](par-resultados/guard-off-restart-fails-repair-p0.log) |
| `guard-off-restart-fails-repair` | 1 | False | 53 | 0 | 0 | 0 | [log](par-resultados/guard-off-restart-fails-repair-p1.log) |
| `guard-off-equal-rank-ai-first` | 0 | False | 52 | 0 | 0 | 0 | [log](par-resultados/guard-off-equal-rank-ai-first-p0.log) |
| `guard-off-equal-rank-dual-first` | 0 | False | 30 | 18 | 4 | 0 | [log](par-resultados/guard-off-equal-rank-dual-first-p0.log) |
| `guard-off-only-ai-safe` | 0 | False | 14 | 0 | 0 | 1 | [log](par-resultados/guard-off-only-ai-safe-p0.log) |
| `guard-off-only-dual-safe` | 0 | False | 14 | 0 | 0 | 1 | [log](par-resultados/guard-off-only-dual-safe-p0.log) |
| `guard-off-observers-between` | 0 | False | 75 | 0 | 0 | 0 | [log](par-resultados/guard-off-observers-between-p0.log) |
| `guard-off-observers-around` | 0 | False | 54 | 0 | 0 | 0 | [log](par-resultados/guard-off-observers-around-p0.log) |
| `guard-on-safe-original` | 0 | True | 52 | 0 | 0 | 0 | [log](par-resultados/guard-on-safe-original-p0.log) |
| `guard-on-safe-restart` | 0 | True | 52 | 0 | 0 | 0 | [log](par-resultados/guard-on-safe-restart-p0.log) |
| `guard-on-late-ai-apply-original` | 0 | True | 16 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-apply-original-p0.log) |
| `guard-on-late-ai-apply-original` | 1 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-apply-original-p1.log) |
| `guard-on-late-ai-direct-original` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-direct-original-p0.log) |
| `guard-on-late-ai-direct-original` | 1 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-direct-original-p1.log) |
| `guard-on-late-ai-reload-original` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-reload-original-p0.log) |
| `guard-on-late-ai-reload-original` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-ai-reload-original-p1.log) |
| `guard-on-late-ai-apply-restart` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-apply-restart-p0.log) |
| `guard-on-late-ai-apply-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-ai-apply-restart-p1.log) |
| `guard-on-late-ai-direct-restart` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-direct-restart-p0.log) |
| `guard-on-late-ai-direct-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-ai-direct-restart-p1.log) |
| `guard-on-late-ai-reload-restart` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-late-ai-reload-restart-p0.log) |
| `guard-on-late-ai-reload-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-ai-reload-restart-p1.log) |
| `guard-on-late-dual-original` | 0 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-dual-original-p0.log) |
| `guard-on-late-dual-original` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-dual-original-p1.log) |
| `guard-on-late-dual-restart` | 0 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-dual-restart-p0.log) |
| `guard-on-late-dual-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-late-dual-restart-p1.log) |
| `guard-on-off-on-original` | 0 | True | 73 | 0 | 0 | 0 | [log](par-resultados/guard-on-off-on-original-p0.log) |
| `guard-on-off-on-original` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-off-on-original-p1.log) |
| `guard-on-off-on-restart` | 0 | True | 73 | 0 | 0 | 0 | [log](par-resultados/guard-on-off-on-restart-p0.log) |
| `guard-on-off-on-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-off-on-restart-p1.log) |
| `guard-on-persist-off-restart` | 0 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-persist-off-restart-p0.log) |
| `guard-on-persist-off-restart` | 1 | True | 16 | 0 | 0 | 1 | [log](par-resultados/guard-on-persist-off-restart-p1.log) |
| `guard-on-persist-off-restart` | 2 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-persist-off-restart-p2.log) |
| `guard-on-window-first-ai-original` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-window-first-ai-original-p0.log) |
| `guard-on-window-first-ai-original` | 1 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-window-first-ai-original-p1.log) |
| `guard-on-window-first-ai-restart` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-window-first-ai-restart-p0.log) |
| `guard-on-window-first-ai-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-window-first-ai-restart-p1.log) |
| `guard-on-window-cancel-restart` | 0 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-window-cancel-restart-p0.log) |
| `guard-on-window-cancel-restart` | 1 | True | 15 | 0 | 0 | 1 | [log](par-resultados/guard-on-window-cancel-restart-p1.log) |
| `guard-on-rank-invert-original` | 0 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-rank-invert-original-p0.log) |
| `guard-on-rank-invert-original` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-rank-invert-original-p1.log) |
| `guard-on-rank-invert-restart` | 0 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-rank-invert-restart-p0.log) |
| `guard-on-rank-invert-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-rank-invert-restart-p1.log) |
| `guard-on-rank-repair-restart` | 0 | True | 12 | 0 | 0 | 1 | [log](par-resultados/guard-on-rank-repair-restart-p0.log) |
| `guard-on-rank-repair-restart` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-rank-repair-restart-p1.log) |
| `guard-on-restart-fails-repair` | 0 | True | 13 | 0 | 0 | 1 | [log](par-resultados/guard-on-restart-fails-repair-p0.log) |
| `guard-on-restart-fails-repair` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-restart-fails-repair-p1.log) |
| `guard-on-equal-rank-ai-first` | 0 | True | 52 | 0 | 0 | 0 | [log](par-resultados/guard-on-equal-rank-ai-first-p0.log) |
| `guard-on-equal-rank-dual-first` | 0 | True | 11 | 0 | 0 | 1 | [log](par-resultados/guard-on-equal-rank-dual-first-p0.log) |
| `guard-on-only-ai-safe` | 0 | True | 14 | 0 | 0 | 1 | [log](par-resultados/guard-on-only-ai-safe-p0.log) |
| `guard-on-only-dual-safe` | 0 | True | 14 | 0 | 0 | 1 | [log](par-resultados/guard-on-only-dual-safe-p0.log) |
| `guard-on-observers-between` | 0 | True | 75 | 0 | 0 | 0 | [log](par-resultados/guard-on-observers-between-p0.log) |
| `guard-on-observers-around` | 0 | True | 54 | 0 | 0 | 0 | [log](par-resultados/guard-on-observers-around-p0.log) |
| `guard-on-arm-unsafe` | 0 | True | 11 | 0 | 0 | 1 | [log](par-resultados/guard-on-arm-unsafe-p0.log) |
| `guard-on-arm-repair-fails` | 0 | True | 13 | 0 | 0 | 1 | [log](par-resultados/guard-on-arm-repair-fails-p0.log) |
| `guard-on-arm-repair-fails` | 1 | True | 53 | 0 | 0 | 0 | [log](par-resultados/guard-on-arm-repair-fails-p1.log) |
| `guard-on-arm-repair-cancel` | 0 | True | 12 | 0 | 0 | 1 | [log](par-resultados/guard-on-arm-repair-cancel-p0.log) |
| `guard-on-arm-repair-cancel` | 1 | True | 12 | 0 | 0 | 1 | [log](par-resultados/guard-on-arm-repair-cancel-p1.log) |
| `guard-on-arm-pending-off` | 0 | True | 19 | 0 | 0 | 1 | [log](par-resultados/guard-on-arm-pending-off-p0.log) |

[JSON principal](resultados/RESULTADOS.json) · [CSV principal](resultados/MATRIZ.csv) · [JSON par](par-resultados/RESULTADOS.json) · [CSV par](par-resultados/MATRIZ.csv) · [Resumo](SUMMARY.json) · [Relatório A–G](../HAL22_24_THREE_WAY_GUARD_POC.md).
