# Draft de finalização cooperativa — laboratório

Partir do PR33. Não recompilar TW: usar o objeto privado46f63050…9093 e seu BUILD.json. Preservar o pacote Dual77cd32ff…2378 exclusivamente local. Usar o SDK24/i386, objetos nativos/overlay COMMON, runner ELF32 e `MEMORIES_*` dos READMEs anteriores; apontar M/H/F para esta árvore.

```sh
python tests/hal9-handoff/build.py --sdk "$MEMORIES_SDK" --output "$HANDOFF_WORK/rd"
export HAL9_HANDOFF_BUILD="$HANDOFF_WORK/rd"
export HAL9_BONUS_RECEIPT="$PR33_WORK/local/final/BUILD.json"
python tests/hal20-field-state/run.py --inputs notes/hal-9/INPUTS.json --fence --timewizard-object "$PR33_WORK/local/final/package" --tw-case 50 --tw-case 51 --tw-case 52 --tw-case 53 --tw-case 54 --trace-returns --output "$HANDOFF_WORK/primary"
```

O builder copia RD para fora do repositório, inclui o companion, envolve os callbacks locais de materialização/hand monitor e fornece a tabela separada. Não altera planner, receita, efeitos, manifest, settings ou a ABI1. A escolha só existe no driver. OBSERVE não transfere nada. Remover `HAL9_HANDOFF_BUILD` e executar80 testa ausência opcional, conservando a recusa antiga. Modo24 canônico reproduz o controle PR33 sem o novo companion.

O runner valida o baseline RD e os hashes dos dois fontes do companion, verifica o objeto construído e registra o recibo da variante. O pin privado de imports TW já existente só é usado no harness, sem alterar guard ou permissões de produção. O guard8 continua recusando TW e a variante não é admitida.

|Caso|Política ou falha exercitada|
|---|---|
|50|OBSERVE: pending/seleção apenas observados|
|51|KEEP_RD: mantém pending680 e materializador RD|
|52|ALLOW_TW: handoff explícito e revogação apenas do pending anterior|
|53|KEEP_RD com Queen: preserva seleção/posição mandatória|
|54|ALLOW_TW com Queen: recusa obrigatória|
|55–58|Sem authorize, cliente veta, sem ACK, cancelamento antes do commit|
|59–62|Generation obsoleta, cursor alterado, identidade alterada, invalidação do produtor|
|63|Disconnect ACK, tabela PROT_NONE e nenhuma leitura posterior|
|64|Reentrância durante authorize|
|65–66|Size incorreto; desconexão real do produtor de regras|
|67–68|Replay de confirmação; tamanho de ticket incorreto|
|69–70|Categoria declarada incorreta; connection obsoleta|
|71–72|Cancelamento após consumo recusado; disconnect busy durante callback|
|73–75|Exaustão do nonce; ABI de cliente incorreta; callback obrigatório ausente|
|76–77|Escrita tardia de cursor/identidade após ACK e antes da coleta|
|78–79|Nonce adulterado; desconexão após confirmação antes do swap|
|80|Companion ausente, sem inventar fallback ou mudar a ABI1|

Os casos negativos alteram somente os metadados ou campos físicos explicitamente identificados e são INSTRUMENTED. Não injetam um plano/pending para simular alcance natural. Identidade corrompida não é reparada nem restaurada por ID. Os casos51/52 usam pending e seleção produzidos pelos planners reais. As políticas são hipóteses alternativas de laboratório, não defaults novos.

Regressões pertinentes:8 (guard),11 (Queen sem conflito),46 (Support→Monster, duas instâncias e correções de bônus PR33),34 (mantém o FAIL histórico de reset terminal). Não somar o controle falho do34 como regressão causada pelo companion. Não alegar gameplay Windows ou resolução completa de efeitos pela fixture.

O `abi.c` recompila as asserções das tabelas ABI1 e da draft. SDK/i386 gera ELF relocatable para o loader Windows; execução aqui é ELF32/QEMU, não backend PE32. O objeto de laboratório tem paths de debug do build; registrar seu hash exato, sem prometer identidade entre diretórios/toolchains diferentes.

Proposta e limites em `notes/hal-9-handoff/CONTRATO.md`. O snapshot/source do conjunto principal é o commit85166136; o caso80 foi acrescentado depois, sem alterar o objeto RD ou TW. Cada suite registra fontes e hash do harness efetivamente utilizados.
