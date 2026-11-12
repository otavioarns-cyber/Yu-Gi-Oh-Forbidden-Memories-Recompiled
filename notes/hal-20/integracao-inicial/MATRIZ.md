# Matriz executada — HAL-20 / HAL-6 / HAL-18

PASS/FAIL são verificações em fixtures integradas, não partidas. BLOCKED/SKIP aparecem na seção de limitações do relatório e no CSV/JSON.

| Perfil | Etapa | AI | Dual | Ordem | PASS | FAIL | Falhas |
|---|---:|---:|---:|---:|---:|---:|---|
| s1-h0-d0-o0 | 1 | 0 | 0 | O0 | 13 | 0 | — |
| s1-h0-d1-o0 | 1 | 0 | 1 | O0 | 13 | 0 | — |
| s1-h1-d0-o0 | 1 | 1 | 0 | O0 | 13 | 0 | — |
| s1-h1-d1-o0 | 1 | 1 | 1 | O0 | 13 | 0 | — |
| s2-h0-d0-o0 | 2 | 0 | 0 | O0 | 15 | 0 | — |
| s2-h0-d1-o0 | 2 | 0 | 1 | O0 | 15 | 0 | — |
| s2-h1-d0-o0 | 2 | 1 | 0 | O0 | 15 | 0 | — |
| s2-h1-d1-o0 | 2 | 1 | 1 | O0 | 15 | 0 | — |
| s3-h0-d0-o0 | 3 | 0 | 0 | O0 | 19 | 1 | F1-fusion-provider-OFF |
| s3-h0-d1-o0 | 3 | 0 | 1 | O0 | 19 | 1 | F1-fusion-provider-OFF |
| s3-h1-d0-o0 | 3 | 1 | 0 | O0 | 19 | 1 | F1-fusion-provider-OFF |
| s3-h1-d1-o0 | 3 | 1 | 1 | O0 | 19 | 1 | F1-fusion-provider-OFF |
| s4-h0-d0-o0 | 4 | 0 | 0 | O0 | 25 | 1 | F1-fusion-provider-OFF |
| s4-h0-d1-o0 | 4 | 0 | 1 | O0 | 23 | 2 | RD-own-materialization-Dual-gate, F1-fusion-provider-OFF |
| s4-h1-d0-o0 | 4 | 1 | 0 | O0 | 25 | 1 | F1-fusion-provider-OFF |
| s4-h1-d1-o0 | 4 | 1 | 1 | O0 | 23 | 2 | RD-own-materialization-Dual-gate, F1-fusion-provider-OFF |
| s4-h0-d0-o1 | 4 | 0 | 0 | O1 | 25 | 1 | F1-fusion-provider-OFF |
| s4-h0-d1-o1 | 4 | 0 | 1 | O1 | 23 | 1 | F1-fusion-provider-OFF |
| s4-h1-d0-o1 | 4 | 1 | 0 | O1 | 25 | 1 | F1-fusion-provider-OFF |
| s4-h1-d1-o1 | 4 | 1 | 1 | O1 | 23 | 1 | F1-fusion-provider-OFF |
| s4-h0-d0-o2 | 4 | 0 | 0 | O2 | 25 | 1 | F1-fusion-provider-OFF |
| s4-h0-d1-o2 | 4 | 0 | 1 | O2 | 23 | 2 | RD-own-materialization-Dual-gate, F1-fusion-provider-OFF |
| s4-h1-d0-o2 | 4 | 1 | 0 | O2 | 23 | 3 | RD-pending-after-other-materializer, F1-fusion-provider-OFF, RD-pending-after-HM-ON-transition |
| s4-h1-d1-o2 | 4 | 1 | 1 | O2 | 19 | 6 | quota-Monster, quota-Support, quota-Ritual-in-bonus, RD-own-materialization-Dual-gate, RD-pending-after-other-materializer, F1-fusion-provider-OFF |
| s1-h1-d1-o2 | 1 | 1 | 1 | O2 | 10 | 3 | quota-Monster, quota-Support, quota-Ritual-in-bonus |

Etapas: S1 Melhoria; S2 + Ritual Effects; S3 + Fusion Effects; S4 + Ritual Duelists. AI/Dual são carregados localmente e ativados conforme cada coluna. Drop Missing Cards ON no perfil; aplicação integral de tabelas/catálogo do disco continua BLOCKED. O2/S1 é o contraponto mínimo AI/Dual, sem Ritual/Fusion/RD.

Totais: **480 PASS, 29 FAIL; quatro bloqueios e quatro skips globais**. As 29 ocorrências FAIL representam F1 repetida e três condições de interação, não 29 bugs diferentes.

Veja [RESULTADOS.json](RESULTADOS.json), [MATRIZ.csv](MATRIZ.csv) e o [relatório](../../HAL20_INTEGRACAO_INICIAL.md).
