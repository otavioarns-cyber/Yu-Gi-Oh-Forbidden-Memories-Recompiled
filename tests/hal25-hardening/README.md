# Reprodução da revisão HAL-25

[Relatório](../../notes/HAL25_CONTRACT_HARDENING.md), [ABI/recuperação](../../notes/hal-25-hardening/ABI_E_RECUPERACAO.md), [wrappers](../../notes/hal-25-hardening/WRAPPERS.md). Host i386 parcial; nenhum teste deste diretório certifica gameplay Windows. A fase Windows manual permanece BLOCKED até o fim do projeto.

Dependências externas qualificadas por SHA em INPUTS.json: SDK oficial API24/Windows, dez objetos nativos Linux i386 da base oficial, GCC i386/libs, Python/Pillow, Melhorias do PR #9, Fusion corrigido do PR #22 e AI oficial. Dual recompilado do PR #10 é **privado/local**, nunca baixado/publicado pelo runner. Não substitua por hash desconhecido. Defina os caminhos sem incluir dependências privadas no repositório:

```sh
export MEMORIES_AI_DIR=/caminho/base/mods/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/privado/dual-api24
export MEMORIES_RITUALS_DIR=/caminho/pr9/melhoria-dos-rituais
export MEMORIES_FUSION_DIR=/caminho/pr22/fusion-auxiliary-balance
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base-linux-i386/obj
export MEMORIES_SDK_EXPORTS=/caminho/base-linux-i386/sdk/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot'
export MEMORIES_SDK=/caminho/sdk-windows
export MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang
```

O runner pode ser omitido em host que execute ELF32 diretamente. O host usa os manager/loader/hooks oficiais inalterados, os dez bindings nativos fixados por hash e os objetos reais dos MODs. Catálogo/mão/campo/ponte gráfica, término da VM e pós-colocação são modelos. `Duel_InitScene` do motor é **stub modelado**; o stack de hooks/init/reset dos MODs é real. Não chamar esse conjunto de execução completa do jogo.

```sh
python tests/hal25-hardening/run.py --output /tmp/main174
python tests/hal25-hardening/run.py --recovery-only --output /tmp/recovery64
python tests/hal25-hardening/run.py --reset-recovery-only --output /tmp/reset16
python tests/hal25-hardening/run.py --restrictions-only --output /tmp/restrictions2
python tests/hal25-hardening/run.py --hardening --output /tmp/hardening72
python tests/hal25-hardening/run.py --composition --trace-returns --output /tmp/composition72
python tests/hal25-hardening/regressions.py --qualified-loader --output /tmp/historical
python tests/hal25-hardening/audit.py --output /tmp/audit.json
```

A seleção vazia de casos falha. O runner sai 1 para FAIL e 2 para preflight/incompleto; REFUSED e NOT_REACHED são classes explícitas, não aprovações de gameplay. O runner histórico deve sair 1 pela assertion de imutabilidade RD: preserve esse resultado. `--qualified-loader` modifica somente uma **cópia temporária** da expectativa histórica hooks==8 para hooks==9; não muda a árvore nem remove assertions.

`hardening.inc`/`lifecycle.inc` contêm falhas adversariais nomeadas. Counters semeados, reset/reentrância dentro de callback, tabela mutável e editores de controller são instrumentação; o planner normal não recebe pending/plano/quota fabricados. Casos 2/3/4/22 editam seleção/identidade antes/depois da delegação e no retorno; caso 5 não delega e deve terminar NOT_REACHED. Caso 14 é espera de VM sem proposta, não um veto. Caso 11 restaura explicitamente um checkpoint na fixture antes de pedir recuperação e prova que trocar por cópia igual de ID não serve; a produção não repara cartas.

O caso 12 atual inclui três probes adicionais de tamanho/callback. O caso 13 inicializava incorretamente o buffer de erro do harness; o arquivo final corrige isso, sem mudar produção. Esses probes foram reexecutados isoladamente no recibo publicado. O resumo substitui seus quatro perfis anteriores e não duplica contagens.

Para reproduzir os objetos originais do PR #25 ou testar pares mistos, faça checkout separado do commit `f6cea112db8d14be34ab2f28694b0121a94842c2`, sem sobrescrever a árvore atual:

```sh
export MEMORIES_PR25_DIR=/caminho/pr25-preservado
python tests/hal25-hardening/run.py --pr25 --hardening --output /tmp/pr25-negative
python tests/hal25-hardening/run.py --mixed old-r --output /tmp/abi-old-r
python tests/hal25-hardening/run.py --mixed old-rd --output /tmp/abi-old-rd
```

A suíte negativa completa atual seleciona mais casos do que o comparativo histórico de 48; os oito probes físicos de mesmo ID foram medidos separadamente. Use os nomes/logs publicados para comparar o subconjunto exato. Pares mistos qualificam somente os casos normais ABI1, não a recuperação nova.

```sh
python tests/hal25-hardening/build.py --sdk "$MEMORIES_SDK" --work /tmp/novo-build
python tests/hal25-hardening/inventory.py --suite /tmp/composition72 --output /tmp/wrappers
python tests/hal25-hardening/guard_reference.py --reference /caminho/pr20-preservado --rd-code 98d3ee01 --output /tmp/guard-reference
```

O build remove os objetos das cópias e recompila todos os 2/16 C com `--target i386`, sem substituir os publicados. Caminhos/DWARF podem alterar o SHA inteiro: documente a nova proveniência. Compare com `abi.py` e requalifique fingerprints, sem presumir identidade pelo nome.

`inventory.py` publica hashes/arestas, nunca bytes de terceiros. O tracer de retorno é opt-in para quatro alvos cdecl conhecidos; `SELF` exclui só filhos críticos rastreados. A ausência de alteração em um ramo CPU não qualifica Select/Square humanos ou um wrapper futuro. O guard de referência não incorpora código no core e testa só a recusa de versão antes da travessia.

O agregador `summarize.py` recebe `--logs` com os diretórios de evidência nomeados no código e `--output`. Logs compactados e índices SHA estão em `notes/hal-25-hardening/logs`; RESULTADOS/MATRIZ/CANCELAMENTOS distinguem baseline negativo, sucesso, recusa e não alcance. Não combinar classes para anunciar compatibilidade universal.
