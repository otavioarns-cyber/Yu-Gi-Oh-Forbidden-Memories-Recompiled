# Reavaliação dos cinco pontos do PR17

Não houve merge ou promoção do PR17. A nova branch parte da integração oficial. As cinco intervenções foram revistas no novo escopo e continuam somente no protótipo:

| Arquivo | Por que foi mantido | Evolução nesta PoC |
|---|---|---|
| `src/pc/mods/manager.c` | Validar composição e transação antes de preferências/ativações. | H/RD/D por subconjunto, projeção de estado adiado, posição real dos registros, fingerprints dos objetos qualificados, recusa de wrappers críticos desconhecidos e primeira carga ao vivo não qualificada. |
| `src/pc/mods/mods.c` | Startup/recarga e SetEnabled não passam sempre por Apply. | Mesma regra em todos esses caminhos; status inclui RD; recusa de startup antes da transação de dados/código. |
| `src/pc/mods/mods.h` | Declarar a API interna compartilhada pelo manager, UI e fence. | Nomes/key novos `Hal2224`; não muda a struct de host/ABI dos MODs. |
| `src/pc/platform/mods_window.c` | Os ranks staged precisam ser validados antes de gravar e confirmar reinício. | Regra conjunta e mensagens do novo guard; rotina restante de confirmação/cancelamento/falha preservada. |
| `src/game/main_run_duel.c` | Rank salvo não prova ordem registrada; startup recusado precisa impedir duelo. | Fence PC revalida composição aplicada antes de init/controller; erro visível; recusa conservadora antes da ação. |

`Hooks_Relink`, `Hooks_Add`, `Mods_Order` e o ordenador interno não foram modificados. Fontes, objetos e manifests de AI/RD/Dual permaneceram intactos; não foi acrescentado previous_swap. Um campo restart do AI foi usado somente nas cópias temporárias. RD conserva seu restart original.

A PoC mudou uma política do teste anterior: wrappers swap transparentes antes admitidos pelo PR17 agora são considerados **não qualificados** quando há Dual + materializador. Permanecem bloqueados, pois presença na lista não prova delegação. Alvos não relacionados não recebem essa restrição.

A primeira carga ao vivo de código não qualificado também é recusada enquanto o par protegido está aplicado: seu init poderia registrar hooks depois da inspeção. Um callback próprio de teste exercita a recusa antes do swap. Código já inicializado e sem wrapper crítico pode passar pelo fluxo normal, como os observadores da suíte do par. Não há prova universal sobre futuros hooks registrados por callbacks arbitrários; isso continua sendo limite de qualificação, não mudança global no linker.

A qualificação usa IDs + fingerprints do loader existentes, associados aos SHA256 dos insumos verificados. Não é uma fronteira de segurança contra código nativo malicioso, colisões de hash ou alterações de memória. Outras versões/plataformas da composição protegida exigem qualificação própria.
