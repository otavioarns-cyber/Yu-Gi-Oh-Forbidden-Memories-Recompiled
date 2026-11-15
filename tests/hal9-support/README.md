# Qualificação Support sobre PR31

Reutiliza o objeto privado **6470024d…d5d7**, o Dual privado77cd32ff…2378 e a coleção canônica do PR31. Não recompila detectores nem altera MODs/core/ABI/guard. O loader API24 continua com a extensão opt-in anterior; core TW declara API4. Os hosts externos/renderer, catálogo, clock e transporte wheel continuam modelos, não Windows.

Preparar `MEMORIES_*` conforme `tests/hal9-timewizard-contracts/README.md`; apontar AI/M/F para esta branch. Usar pyelftools. `prepare_native.py` produz um overlay **privado** dos objetos nativos: transforma somente o símbolo COMMON `D_8009B150` do objeto residente `func_80019608` em import. Seus bytes de código e todas as relocations ficam idênticos. É binding para o harness, não patch da produção.

```sh
python tests/hal9-support/prepare_native.py --engine "$MEMORIES_ENGINE_OBJ_DIR" --output "$HAL_SUPPORT_WORK/engine"
export MEMORIES_ENGINE_OBJ_DIR="$HAL_SUPPORT_WORK/engine"
python tests/hal20-field-state/run.py --inputs notes/hal-9/INPUTS.json --fence --timewizard-object "$HAL_SUPPORT_TW" --tw-case 8 --tw-case 20 --tw-case 21 --tw-case 22 --tw-case 23 --tw-case 24 --tw-case 25 --tw-case 26 --tw-case 27 --tw-case 28 --tw-case 29 --tw-case 30 --tw-case 31 --tw-case 32 --tw-case 33 --tw-case 34 --tw-case 35 --tw-case 36 --output "$HAL_SUPPORT_WORK/final"
```

A matriz é H OFF/ON ×D OFF/ON ×duas prioridades R. As dependências podem produzir a mesma ordem efetiva: confiar nos `REGISTER`/`CALLBACK_NODE`, não no rótulo before/after. Para trace de retorno/quota, acrescentar `--trace-returns` e selecionar27/34. `--tw-case 39` examina alvos de evolução e o bloqueio de disco, sem simular o recurso ausente. Para regressão da coleção sem TW: `--field-bonus --terrain 0 --guard 1 --fence` sem `--timewizard-object`.

|Caso|Escopo|
|---|---|
|8|Guard original recusa antes de duelo|
|20|Geometria antiga0–2, controle de ausência de ameaça|
|21/24|RD ON: snapshot capturado antes da seleção TW, três recusas e BLOCKED|
|22|Ameaça Dragon do modelo não é necessariamente bom alvo para Time Magic|
|23|Campo Fiend correto5–7; RD frio OFF; seleção/consumo/ativação Support|
|25|Uso nativo e resolução automática sem Encore|
|26|Queen exige Female primeiro; tentativa de bônus seguinte|
|27|Support primeiro, tentativa de bônus Monster|
|28|X por inputs no controller humano|
|29|Quadrado, flip e confirmação M/T face-down; ativação posterior não executada|
|30|Quadrado e cancelamento antes de consumir|
|31/32/33|Provider Encore ON/OFF e OFF depois do início|
|34|Reset durante roleta; mantém o FAIL da hipótese de fase terminal0|
|35|Preferência restart pendente e OFF/ON efetivo instrumentado|
|36|Quadrado face-up direto e uso nativo/roleta|
|38|Controle inicial com objetivos de transformação não atingidos; logs FAIL conservados|
|39|Fronteira de asset: snapshot real433/67, disc lookup=-1, NOT_REACHED|

`support.inc` nunca escreve plano, pending, quota ou resultado da roleta. Botões são inputs; clock avança16.667us/tick; o wheel transporta `forced` que o núcleo já escolheu. Isso não injeta um resultado para fingir alcance natural. Reset/OFF efetivo via bridge são instrumentação declarada; a preferência comum restart é testada separadamente.

O catálogo/tabuleiro são controlados. São preservados campos genéricos herdados para cartas não tipadas no cenário; em particular495 é identificada pelo provider por ID, mas seu tipo genérico não qualifica a evolução Vanilla dessa carta. Os casos de evolução tipam41 e72 e fornecem somente seus nomes conhecidos. Não se trata de deck, catálogo completo ou gameplay do usuário.

As fixtures corrigem field6B conforme `duel_init_scene.c:func_80017F04`, decodificam argumentos de slot conforme `Duel_SetupCardRecord` e leem a Monster Zone nos registros5–9. O fence procura seu objeto por símbolo em vez de assumir que é o último objeto carregado. Nenhuma dessas correções muda código de produção.

`collect.py` publica só JSON/CSV/logs próprios. Compacta repetições exatas de trace após três ocorrências e registra SHA dos logs integrais; não copia binários/disco/assets privados. Um FAIL de objetivo não equivale automaticamente a bug funcional: consultar a classificação causal do relatório. Não remover os controles falhos para apresentar uma coleção aprovada.
