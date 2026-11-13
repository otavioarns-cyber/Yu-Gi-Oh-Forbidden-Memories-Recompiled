# HAL-24 — matriz de diagnóstico

**Principal:150 processos,1284 PASS,98 FAIL diagnósticos,0 incompletos.** Os FAIL não são98 bugs diferentes. RD/I1 e AI/I2 estão separados abaixo; nenhum teste principal injeta pending, plano ou quota.

| Coorte disjunta | Processos | PASS | FAIL |
|---|---:|---:|---:|
| NO_FAILURE_EXERCISED | 101 | 858 | 0 |
| I1_RD_missed_gate | 22 | 164 | 60 |
| I2_AI_external_separate | 27 | 262 | 38 |

`NO_FAILURE_EXERCISED` significa apenas que as invariantes exercitadas passaram. Inclui controles Dual OFF e cenários sem commit bônus; não significa compatibilidade integrada. RD/I1 é identificado por pending real, worker RD e gate ausente. I2 é classificado separadamente quando o worker AI externo executa sem worker RD.

| Invariante FAIL | RD/I1 | AI/I2 separado |
|---|---:|---:|
| `first-ledger-matches-physical-category` | 22 | 5 |
| `bonus-category-authorized` | 19 | 6 |
| `bonus-ledger-matches-actions` | 19 | 27 |

O1 de carga inicial com Dual ON:18 processos/200 PASS/zero FAIL. O1 incluindo demais controles seguros:49 processos/455 PASS; a exceção é primeira carga tardia do AI com manifest original, antes do reinício (I2). Seis primeiras cargas tardias de Dual passaram; relançar com ranks O0/O2 salvos pode reconstruir ordem insegura.

110 bônus foram executados até a fase de planejamento. Em todos o snapshot do Dual ocultou a categoria registrada como gasta e os rituais. Em105 houve commit físico; cinco ficaram sem commit, com rollback observado PASS. Nenhum pending novo foi criado no bônus nesta matriz. Isso não impede a repetição de Support quando a primeira categoria já havia sido registrada incorretamente.

| Limite | Estado |
|---|---|
| Bytecode retail quando AI OFF | NOT_RUN: predecessor retorna status1 sem fornecer seleção; planner RD real. |
| Gameplay/UI/relançamento Windows | NOT_RUN: host Linux i386/QEMU e ponte de efeito/placement para campo. |
| Consumo ritual completo de3 materiais | NOT_RUN: layouts/checker/providers reais são parciais; invocação não executada. |
| Save/load completo, outros efeitos/turnos/cancelamento humano | NOT_RUN: não extrapolar seleção física e rollback examinados. |
| Guard PR17, HAL21/F1, HAL23 | NOT_IMPLEMENTED / fora deste diagnóstico. |

Controles históricos PR14:6 perfis,17 PASS/5 FAIL. Quatro FAIL são o gate RD sob pending/bônus artificialmente semeados; a quinta ocorrência é o pending remanescente do AI (I3), preservado no log original e fora da contagem HAL24. Esses resultados não se somam à matriz principal como fluxos naturais.

| Caso principal | Fase | Coorte | PASS | FAIL | Log |
|---|---|---|---:|---:|---|
| `O0-h0-d0-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h0-d0-v0-none-restart0-absent.log) |
| `O0-h0-d0-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h0-d0-v1-none-restart0-absent.log) |
| `O0-h0-d0-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h0-d0-v2-none-restart0-absent.log) |
| `O0-h0-d0-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h0-d0-v3-none-restart0-absent.log) |
| `O0-h0-d0-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h0-d0-v4-none-restart0-absent.log) |
| `O0-h0-d0-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h0-d0-v5-none-restart0-absent.log) |
| `O0-h0-d1-v0-none-restart0-absent` | main | I1_RD_missed_gate | 4 | 1 | [log](resultados/O0-h0-d1-v0-none-restart0-absent.log) |
| `O0-h0-d1-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h0-d1-v1-none-restart0-absent.log) |
| `O0-h0-d1-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h0-d1-v2-none-restart0-absent.log) |
| `O0-h0-d1-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h0-d1-v3-none-restart0-absent.log) |
| `O0-h0-d1-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h0-d1-v4-none-restart0-absent.log) |
| `O0-h0-d1-v5-none-restart0-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h0-d1-v5-none-restart0-absent.log) |
| `O0-h1-d0-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h1-d0-v0-none-restart0-absent.log) |
| `O0-h1-d0-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h1-d0-v1-none-restart0-absent.log) |
| `O0-h1-d0-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h1-d0-v2-none-restart0-absent.log) |
| `O0-h1-d0-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h1-d0-v3-none-restart0-absent.log) |
| `O0-h1-d0-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h1-d0-v4-none-restart0-absent.log) |
| `O0-h1-d0-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O0-h1-d0-v5-none-restart0-absent.log) |
| `O0-h1-d1-v0-none-restart0-absent` | main | I1_RD_missed_gate | 4 | 1 | [log](resultados/O0-h1-d1-v0-none-restart0-absent.log) |
| `O0-h1-d1-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v1-none-restart0-absent.log) |
| `O0-h1-d1-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v2-none-restart0-absent.log) |
| `O0-h1-d1-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v3-none-restart0-absent.log) |
| `O0-h1-d1-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v4-none-restart0-absent.log) |
| `O0-h1-d1-v5-none-restart0-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-none-restart0-absent.log) |
| `O1-h0-d0-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h0-d0-v0-none-restart0-absent.log) |
| `O1-h0-d0-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h0-d0-v1-none-restart0-absent.log) |
| `O1-h0-d0-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h0-d0-v2-none-restart0-absent.log) |
| `O1-h0-d0-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h0-d0-v3-none-restart0-absent.log) |
| `O1-h0-d0-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h0-d0-v4-none-restart0-absent.log) |
| `O1-h0-d0-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h0-d0-v5-none-restart0-absent.log) |
| `O1-h0-d1-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h0-d1-v0-none-restart0-absent.log) |
| `O1-h0-d1-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h0-d1-v1-none-restart0-absent.log) |
| `O1-h0-d1-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h0-d1-v2-none-restart0-absent.log) |
| `O1-h0-d1-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h0-d1-v3-none-restart0-absent.log) |
| `O1-h0-d1-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h0-d1-v4-none-restart0-absent.log) |
| `O1-h0-d1-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 9 | 0 | [log](resultados/O1-h0-d1-v5-none-restart0-absent.log) |
| `O1-h1-d0-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h1-d0-v0-none-restart0-absent.log) |
| `O1-h1-d0-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h1-d0-v1-none-restart0-absent.log) |
| `O1-h1-d0-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h1-d0-v2-none-restart0-absent.log) |
| `O1-h1-d0-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h1-d0-v3-none-restart0-absent.log) |
| `O1-h1-d0-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h1-d0-v4-none-restart0-absent.log) |
| `O1-h1-d0-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O1-h1-d0-v5-none-restart0-absent.log) |
| `O1-h1-d1-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v0-none-restart0-absent.log) |
| `O1-h1-d1-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v1-none-restart0-absent.log) |
| `O1-h1-d1-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v2-none-restart0-absent.log) |
| `O1-h1-d1-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v3-none-restart0-absent.log) |
| `O1-h1-d1-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v4-none-restart0-absent.log) |
| `O1-h1-d1-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-none-restart0-absent.log) |
| `O2-h0-d0-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h0-d0-v0-none-restart0-absent.log) |
| `O2-h0-d0-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h0-d0-v1-none-restart0-absent.log) |
| `O2-h0-d0-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h0-d0-v2-none-restart0-absent.log) |
| `O2-h0-d0-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h0-d0-v3-none-restart0-absent.log) |
| `O2-h0-d0-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h0-d0-v4-none-restart0-absent.log) |
| `O2-h0-d0-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h0-d0-v5-none-restart0-absent.log) |
| `O2-h0-d1-v0-none-restart0-absent` | main | I1_RD_missed_gate | 4 | 1 | [log](resultados/O2-h0-d1-v0-none-restart0-absent.log) |
| `O2-h0-d1-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O2-h0-d1-v1-none-restart0-absent.log) |
| `O2-h0-d1-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O2-h0-d1-v2-none-restart0-absent.log) |
| `O2-h0-d1-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O2-h0-d1-v3-none-restart0-absent.log) |
| `O2-h0-d1-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O2-h0-d1-v4-none-restart0-absent.log) |
| `O2-h0-d1-v5-none-restart0-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O2-h0-d1-v5-none-restart0-absent.log) |
| `O2-h1-d0-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h1-d0-v0-none-restart0-absent.log) |
| `O2-h1-d0-v1-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h1-d0-v1-none-restart0-absent.log) |
| `O2-h1-d0-v2-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h1-d0-v2-none-restart0-absent.log) |
| `O2-h1-d0-v3-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h1-d0-v3-none-restart0-absent.log) |
| `O2-h1-d0-v4-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h1-d0-v4-none-restart0-absent.log) |
| `O2-h1-d0-v5-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 4 | 0 | [log](resultados/O2-h1-d0-v5-none-restart0-absent.log) |
| `O2-h1-d1-v0-none-restart0-absent` | main | NO_FAILURE_EXERCISED | 5 | 0 | [log](resultados/O2-h1-d1-v0-none-restart0-absent.log) |
| `O2-h1-d1-v1-none-restart0-absent` | main | I2_AI_external_separate | 8 | 3 | [log](resultados/O2-h1-d1-v1-none-restart0-absent.log) |
| `O2-h1-d1-v2-none-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v2-none-restart0-absent.log) |
| `O2-h1-d1-v3-none-restart0-absent` | main | I2_AI_external_separate | 8 | 3 | [log](resultados/O2-h1-d1-v3-none-restart0-absent.log) |
| `O2-h1-d1-v4-none-restart0-absent` | main | I2_AI_external_separate | 9 | 2 | [log](resultados/O2-h1-d1-v4-none-restart0-absent.log) |
| `O2-h1-d1-v5-none-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-none-restart0-absent.log) |
| `O0-h0-d1-v6-none-restart0-field` | main | NO_FAILURE_EXERCISED | 10 | 0 | [log](resultados/O0-h0-d1-v6-none-restart0-field.log) |
| `O0-h0-d1-v7-none-restart0-hand` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O0-h0-d1-v7-none-restart0-hand.log) |
| `O0-h0-d1-v8-none-restart0-both` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O0-h0-d1-v8-none-restart0-both.log) |
| `O0-h1-d1-v6-none-restart0-field` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O0-h1-d1-v6-none-restart0-field.log) |
| `O0-h1-d1-v7-none-restart0-hand` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O0-h1-d1-v7-none-restart0-hand.log) |
| `O0-h1-d1-v8-none-restart0-both` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O0-h1-d1-v8-none-restart0-both.log) |
| `O1-h0-d1-v6-none-restart0-field` | main | NO_FAILURE_EXERCISED | 10 | 0 | [log](resultados/O1-h0-d1-v6-none-restart0-field.log) |
| `O1-h0-d1-v7-none-restart0-hand` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O1-h0-d1-v7-none-restart0-hand.log) |
| `O1-h0-d1-v8-none-restart0-both` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O1-h0-d1-v8-none-restart0-both.log) |
| `O1-h1-d1-v6-none-restart0-field` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O1-h1-d1-v6-none-restart0-field.log) |
| `O1-h1-d1-v7-none-restart0-hand` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O1-h1-d1-v7-none-restart0-hand.log) |
| `O1-h1-d1-v8-none-restart0-both` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O1-h1-d1-v8-none-restart0-both.log) |
| `O2-h0-d1-v6-none-restart0-field` | main | NO_FAILURE_EXERCISED | 10 | 0 | [log](resultados/O2-h0-d1-v6-none-restart0-field.log) |
| `O2-h0-d1-v7-none-restart0-hand` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O2-h0-d1-v7-none-restart0-hand.log) |
| `O2-h0-d1-v8-none-restart0-both` | main | NO_FAILURE_EXERCISED | 12 | 0 | [log](resultados/O2-h0-d1-v8-none-restart0-both.log) |
| `O2-h1-d1-v6-none-restart0-field` | main | I2_AI_external_separate | 9 | 3 | [log](resultados/O2-h1-d1-v6-none-restart0-field.log) |
| `O2-h1-d1-v7-none-restart0-hand` | main | I2_AI_external_separate | 9 | 3 | [log](resultados/O2-h1-d1-v7-none-restart0-hand.log) |
| `O2-h1-d1-v8-none-restart0-both` | main | I2_AI_external_separate | 9 | 3 | [log](resultados/O2-h1-d1-v8-none-restart0-both.log) |
| `O0-h1-d1-v5-late-ai-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O0-h1-d1-v5-late-ai-restart0-absent.log) |
| `O0-h1-d1-v5-late-ai-restart0-absent` | reboot | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-late-ai-restart0-absent-reboot.log) |
| `O0-h1-d1-v5-late-ai-restart1-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-late-ai-restart1-absent.log) |
| `O0-h1-d1-v5-late-ai-restart1-absent` | reboot | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-late-ai-restart1-absent-reboot.log) |
| `O0-h1-d1-v5-late-dual-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v5-late-dual-restart0-absent.log) |
| `O0-h1-d1-v5-late-dual-restart0-absent` | reboot | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-late-dual-restart0-absent-reboot.log) |
| `O0-h1-d1-v5-late-dual-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v5-late-dual-restart1-absent.log) |
| `O0-h1-d1-v5-late-dual-restart1-absent` | reboot | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-late-dual-restart1-absent-reboot.log) |
| `O0-h1-d1-v5-late-rd-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v5-late-rd-restart0-absent.log) |
| `O0-h1-d1-v5-late-rd-restart0-absent` | reboot | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-late-rd-restart0-absent-reboot.log) |
| `O0-h1-d1-v5-late-rd-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O0-h1-d1-v5-late-rd-restart1-absent.log) |
| `O0-h1-d1-v5-late-rd-restart1-absent` | reboot | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-late-rd-restart1-absent-reboot.log) |
| `O0-h1-d1-v5-cycle-ai-restart0-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-cycle-ai-restart0-absent.log) |
| `O0-h1-d1-v5-cycle-ai-restart1-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-cycle-ai-restart1-absent.log) |
| `O0-h1-d1-v5-cycle-dual-restart0-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-cycle-dual-restart0-absent.log) |
| `O0-h1-d1-v5-cycle-dual-restart1-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-cycle-dual-restart1-absent.log) |
| `O0-h1-d1-v5-cycle-rd-restart0-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-cycle-rd-restart0-absent.log) |
| `O0-h1-d1-v5-cycle-rd-restart1-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-cycle-rd-restart1-absent.log) |
| `O0-h1-d1-v5-reload-restart0-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-reload-restart0-absent.log) |
| `O0-h1-d1-v5-reload-restart1-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O0-h1-d1-v5-reload-restart1-absent.log) |
| `O1-h1-d1-v5-late-ai-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O1-h1-d1-v5-late-ai-restart0-absent.log) |
| `O1-h1-d1-v5-late-ai-restart0-absent` | reboot | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-ai-restart0-absent-reboot.log) |
| `O1-h1-d1-v5-late-ai-restart1-absent` | main | NO_FAILURE_EXERCISED | 9 | 0 | [log](resultados/O1-h1-d1-v5-late-ai-restart1-absent.log) |
| `O1-h1-d1-v5-late-ai-restart1-absent` | reboot | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-ai-restart1-absent-reboot.log) |
| `O1-h1-d1-v5-late-dual-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-dual-restart0-absent.log) |
| `O1-h1-d1-v5-late-dual-restart0-absent` | reboot | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-dual-restart0-absent-reboot.log) |
| `O1-h1-d1-v5-late-dual-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-dual-restart1-absent.log) |
| `O1-h1-d1-v5-late-dual-restart1-absent` | reboot | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-dual-restart1-absent-reboot.log) |
| `O1-h1-d1-v5-late-rd-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-rd-restart0-absent.log) |
| `O1-h1-d1-v5-late-rd-restart0-absent` | reboot | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-rd-restart0-absent-reboot.log) |
| `O1-h1-d1-v5-late-rd-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-rd-restart1-absent.log) |
| `O1-h1-d1-v5-late-rd-restart1-absent` | reboot | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-late-rd-restart1-absent-reboot.log) |
| `O1-h1-d1-v5-cycle-ai-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-cycle-ai-restart0-absent.log) |
| `O1-h1-d1-v5-cycle-ai-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-cycle-ai-restart1-absent.log) |
| `O1-h1-d1-v5-cycle-dual-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-cycle-dual-restart0-absent.log) |
| `O1-h1-d1-v5-cycle-dual-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-cycle-dual-restart1-absent.log) |
| `O1-h1-d1-v5-cycle-rd-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-cycle-rd-restart0-absent.log) |
| `O1-h1-d1-v5-cycle-rd-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-cycle-rd-restart1-absent.log) |
| `O1-h1-d1-v5-reload-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-reload-restart0-absent.log) |
| `O1-h1-d1-v5-reload-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O1-h1-d1-v5-reload-restart1-absent.log) |
| `O2-h1-d1-v5-late-ai-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-ai-restart0-absent.log) |
| `O2-h1-d1-v5-late-ai-restart0-absent` | reboot | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-ai-restart0-absent-reboot.log) |
| `O2-h1-d1-v5-late-ai-restart1-absent` | main | I1_RD_missed_gate | 8 | 3 | [log](resultados/O2-h1-d1-v5-late-ai-restart1-absent.log) |
| `O2-h1-d1-v5-late-ai-restart1-absent` | reboot | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-ai-restart1-absent-reboot.log) |
| `O2-h1-d1-v5-late-dual-restart0-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O2-h1-d1-v5-late-dual-restart0-absent.log) |
| `O2-h1-d1-v5-late-dual-restart0-absent` | reboot | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-dual-restart0-absent-reboot.log) |
| `O2-h1-d1-v5-late-dual-restart1-absent` | main | NO_FAILURE_EXERCISED | 11 | 0 | [log](resultados/O2-h1-d1-v5-late-dual-restart1-absent.log) |
| `O2-h1-d1-v5-late-dual-restart1-absent` | reboot | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-dual-restart1-absent-reboot.log) |
| `O2-h1-d1-v5-late-rd-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-rd-restart0-absent.log) |
| `O2-h1-d1-v5-late-rd-restart0-absent` | reboot | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-rd-restart0-absent-reboot.log) |
| `O2-h1-d1-v5-late-rd-restart1-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-rd-restart1-absent.log) |
| `O2-h1-d1-v5-late-rd-restart1-absent` | reboot | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-late-rd-restart1-absent-reboot.log) |
| `O2-h1-d1-v5-cycle-ai-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-cycle-ai-restart0-absent.log) |
| `O2-h1-d1-v5-cycle-ai-restart1-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-cycle-ai-restart1-absent.log) |
| `O2-h1-d1-v5-cycle-dual-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-cycle-dual-restart0-absent.log) |
| `O2-h1-d1-v5-cycle-dual-restart1-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-cycle-dual-restart1-absent.log) |
| `O2-h1-d1-v5-cycle-rd-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-cycle-rd-restart0-absent.log) |
| `O2-h1-d1-v5-cycle-rd-restart1-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-cycle-rd-restart1-absent.log) |
| `O2-h1-d1-v5-reload-restart0-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-reload-restart0-absent.log) |
| `O2-h1-d1-v5-reload-restart1-absent` | main | I2_AI_external_separate | 10 | 1 | [log](resultados/O2-h1-d1-v5-reload-restart1-absent.log) |

[CSV por verificação](resultados/MATRIZ.csv) · [JSON integral](resultados/RESULTADOS.json) · [classificação](CLASSIFICACAO.json) · [controles originais PR14](controles-pr14/RESULTADOS.json) · [relatório A–F](../HAL24_RITUAL_DUELISTS_DUAL_GATE.md).
