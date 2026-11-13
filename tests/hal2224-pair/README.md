# Regressão do par AI + Dual

Adaptação da suíte própria do PR17 para a API interna da PoC conjunta. Leia [o README principal](../hal2224/README.md) e o relatório A–G.

Os controles naturais continuam executando planner/controller reais; adversarial/probe injeta seleção após filtragem, deliberadamente e de forma identificada, para verificar rejeição sem worker/consumo. Não há Dual fonte/manifest/binário publicado. Os observadores desta suíte agora usam apenas `Duel_DrawFieldCards`: um wrapper desconhecido em swap é recusado na nova PoC, mesmo se transparente. Os testes dessa recusa ficam na suíte principal.

`order.c` usa manifests reais e mocks apenas para a regressão de metadados/ordenação. Consulta os mesmos IDs e preserva a ordem resolvida; os alvos de hooks são sentinelas, não callbacks de gameplay. Cada uma das3072 combinações compara guard OFF/ON e valida também ranks staged sem alterar o rank salvo. A execução de hooks/controller acontece no harness separado, não nesse mock.
