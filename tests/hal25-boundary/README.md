# HAL-25 — reprodução da qualificação de fronteiras

[Relatório](../../notes/HAL25_BOUNDARY_QUALIFICATION.md). O host é parcial i386 e não certifica gameplay. Testes Windows automatizados estão autorizados agora; só a bateria pessoal do usuário fica para a fase final.

Use SDK Windows oficial API24, Python/Pillow, GCC/libs i386 e os objetos nativos da base fixados em INPUTS. Os seis pacotes de teste são fixados por SHA, nunca qualificados só pelo nome. Dual é privado/local e não acompanha esta branch. Não baixar/substituir por binário antigo para fazer um teste passar.

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
python tests/hal25-boundary/run.py --boundary --trace-returns --output /tmp/boundary128
python tests/hal25-boundary/run.py --boundary --bcase 12 --bcase 13 --trace-returns --output /tmp/quota16
python tests/hal25-boundary/run.py --output /tmp/main174
python tests/hal25-boundary/run.py --recovery-only --output /tmp/recovery64
python tests/hal25-boundary/run.py --reset-recovery-only --output /tmp/reset16
python tests/hal25-boundary/run.py --restrictions-only --output /tmp/position2
python tests/hal25-boundary/run.py --hardening --output /tmp/hardening72
python tests/hal25-boundary/run.py --composition --trace-returns --output /tmp/composition72
```

O resultado deve distinguir PASS, REFUSED e NOT_REACHED; assertions FAIL/incompletos causam saída 1/2. O uso de processos distintos não substitui um reinício completo de Windows/gameplay. Quotas/pending/planos não são injetados nas matrizes normais. Os probes `boundary-combined` têm um produtor público **de teste** que fornece propostas físicas controladas; são instrumentação adversarial do contrato, não decisões reais do planner retail.

`boundary.inc`: 1 tabela conectada→ACK→PROT_NONE; 2 disconnect ocupado; 3 reset durante evaluate; 4 OFF efetivo durante evaluate; 5 OFF/ON efetivo; 6 shutdown real de R entre planner/swap; 7 veto persistente/100 ticks; 8 retirada da causa+recuperação explícita; 10 três cartas combinadas; 11 ausência de rollback após commit; 12 cursor nativo alterado; 13 marca extra de seleção combinada; 20 Field; 21 Both; 22 Hand recusado por min_field=1; 23 Vanilla com M/R efetivamente OFF. Cada modo cruza AI/Dual OFF/ON e duas ordens. OFF efetivo/shutdown no meio da chamada é uma seam de teste, não uma janela UI normalmente disponível com manifests restart:true.

O wrapper próprio dos modos12/13 altera `DuelSelectionRecord`/`DuelHandSlot`, após aprovação e antes da coleta. O objeto do PR26 consome344 em vez de535 ou quatro cartas em vez de três. O novo RD deve entrar em QUARANTINE antes de consumir, rejeitar recover e preservar a quota. O código do wrapper e seu hash fazem parte da evidência; nenhum código Dual foi copiado.

A suíte ritual carrega mais quatro unidades nativas inalteradas (14 no total): efeito ritual, checker/match, lifecycle de records e flag de inicialização. O efeito nativo executa as três desativações. Só conclusão da requisição de animação e salto das etapas de portal são modelados; publicação inicial do resultado em G32 é real. Render/ativação final, duelo inteiro e bytecode retail não são certificados. As combinações AI/Dual nesses perfis indicam objetos carregados; seus gates de controller não são invocados por uma chamada direta do efeito ritual. A matriz de controller cobre quotas/bônus separadamente.

```sh
export MEMORIES_PR26_DIR=/caminho/checkout-preservado-dc711074
python tests/hal25-boundary/run.py --reference26 --boundary --bcase 12 --bcase 13 --quick --trace-returns --output /tmp/pr26-negative
python tests/hal25-boundary/build.py --sdk "$MEMORIES_SDK" --work /tmp/candidate-build
python tests/hal25-boundary/abi.py --allow-new-imports --original /caminho/pr26/mods/ritual-duelists/ritual-duelists.o --candidate mods/ritual-duelists/ritual-duelists.o --sdk-exports "$MEMORIES_SDK_EXPORTS" --output /tmp/abi.json
python tests/hal25-boundary/regressions.py --qualified-loader --output /tmp/canonical
python tests/hal25-boundary/audit.py --output /tmp/audit.json
python tests/hal25-boundary/inventory.py --suite /tmp/boundary128 --output /tmp/wrappers
python tests/hal25-boundary/guard_reference.py --reference /caminho/pr20-preservado --rd-code 3d26698f --output /tmp/guard-reference
```

O audit canônico RD mantém FAIL da imutabilidade histórica; não enfraquecer essa assertion. A expectativa8→9 do loader é adaptada somente numa cópia temporária, como no PR25. `audit.py` verifica o delta atual separado. O build remove os objetos das cópias e recompila todos2/16 C; caminho/DWARF pode mudar SHA inteiro. Conferir funções/relocations/imports e proveniência, sem substituir hashes desconhecidos.

O tracer identifica ponteiros efetivos, artefato, corpo e relocations; retorno só para cinco alvos cdecl conhecidos, incluindo DeactivateRecord. SELF exclui filhos críticos rastreados, não toda chamada C. Não somar pai+filho para contar materializações. Logs/índices e matrizes ficam em notes/hal-25-boundary. `summarize.py` agrega as pastas nomeadas no script e preserva falhas diagnósticas/de desenvolvimento separadas. [Automação Windows](windows/README.md).
