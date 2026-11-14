# Reprodução — HAL25 contrato real

[Relatório](../../notes/HAL25_SELECTION_CONTRACT_REAL_POC.md) e [contrato](../../notes/hal-25-contract/CONTRATO.md). Estes testes não certificam gameplay Windows. Testes manuais Windows estão BLOCKED até a última fase por instrução do usuário; não são pré-requisito para repetir os testes automatizados.

Dependências: Python/Pillow, GCC com sanitizadores/libs i386, SDK oficial API24, os dez objetos nativos Linux i386 e dependências qualificadas em `INPUTS.json`. Dual API24 é privado/local; não acompanha esta branch. Não substituir por objeto recebido antigo ou hash desconhecido. QEMU/sysroot podem ser omitidos em host que executa ELF32 diretamente.

```sh
export MEMORIES_AI_DIR=/caminho/base-build/mods/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/local/dual-api24
export MEMORIES_RITUALS_DIR=/caminho/pr9/melhoria-dos-rituais
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base-build/obj
export MEMORIES_SDK_EXPORTS=/caminho/sdk-linux/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot'
export MEMORIES_SDK=/caminho/sdk-windows
python tests/hal25-contract/run.py --output /tmp/hal25-real-matrix
python tests/hal25-contract/run.py --recovery-only --output /tmp/hal25-real-recovery
python tests/hal25-contract/run.py --reset-recovery-only --output /tmp/hal25-real-reset
python tests/hal25-contract/run.py --restrictions-only --output /tmp/hal25-real-position
python tests/hal25-contract/regressions.py --qualified-loader --output /tmp/hal25-real-regressions
python tests/hal25-contract/audit.py --output /tmp/hal25-real-audit.json
```

O runner sai1 em assertion FAIL e2 em execução bloqueada/incompleta. O audit histórico de pacote RD deve falhar porque exige os insumos antigos imutáveis; não representa falha de gameplay. As outras34 regressões passam com o objeto novo. A adaptação explícita do loader reconhece somente o hook adicional8→9 e preserva os demais testes; não altera o teste canônico nem a assertion de imutabilidade.

`harness.c`/`manager_bridge.c` usam manager, loader e Hooks_Relink oficiais; o bridge do nome é a instrumentação **do manager**, não uma ponte privada de Queen. A regra vem somente do callback real conectado por R. Acesso a pending/marks é observação; não há chamada externa de helper Queen. `public_contract.inc` consulta o rendezvous público, verifica pureza, schema/ABI, status, OFF/revisão e intenção explícita de cancelamento. `observer.c` marca os probes adversariais após os planners, distinguindo-os da UI. Fake providers são somente respostas malformadas de ABI, sem implementar Queen.

Mão/campo/catálogo, conclusão VM Vanilla, gráficos e resolução pós-colocação são modelos. Os dez bindings nativos de controller/snapshot/materialização, planners reais e gates/ledger estão qualificados por hash. Nenhuma quota/plano/pending é injetada nos cenários normais. `bridge0` e o contador fixo0 nos nomes/logs são metadados herdados sem implementação de ponte privada.

Primeiras ações normais devem chegar realmente ao commit. Probes de cancelamento terminam a medição antes de planejar nova ação; `--recovery-only` remove apenas a falha **da instrumentação** e acompanha uma decisão nova. Não repara a mão física corrompida: testa três ticks em quarentena. `--reset-recovery-only` substitui o duelo modelado inteiro após reset/init e testa um commit novo, sem consertar a instância anterior pelo ID.

Para reconstruir em cópias frescas sem substituir os objetos publicados:

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
python tests/hal25-contract/build.py --sdk /caminho/sdk-windows --work /tmp/hal25-real-build
```

O script remove somente os objetos das cópias antes de compilar2/16 C com `--target i386`. Novo diretório/DWARF pode produzir SHA inteiro diferente; registre proveniência e compare ABI/seções. Não misturar objetos atuais com fontes originais ou variantes A/B.

Logs em `notes/hal-25-contract/logs/*.tar.gz`, índices SHA em cada suíte. `development-negative.tar.gz` contém somente exemplos de iterações rejeitadas, não evidência do candidato atual. A comparação #24 é histórica e usa24 casos normalizados por variante; não reexecuta a ponte antiga.
