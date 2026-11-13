# PoC conjunta HAL-22 + HAL-24

[Relatório A–G](../../notes/HAL22_24_THREE_WAY_GUARD_POC.md) · [matriz](../../notes/hal-22-24/MATRIZ.md) · [diff experimental](../../notes/hal-22-24/DIFF_EXPERIMENTAL.patch).

Proteção experimental **opt-in**, default desligado. A chave nova é `experimental.hal2224_order_guard=1`; a chave antiga do PR17 não habilita esta PoC. Não existe checkbox novo nesta entrega. Os scripts criam settings temporários com o opt-in; não alteram o perfil normal nem o manifest de produção do AI/RD.

São duas suítes complementares:

- `tests/hal2224/run.py`: evolução dos casos de planner/controller reais do PR19, sem injetar seleção/plano, pending ou quotas. Usa os objetos exatos de AI, RD, Dual e, nos layouts Field/Hand/Both, Melhoria PR9. Executa a entrada nativa `Main_RunDuel` compilada com o diff experimental antes de chamar setup/controller. Ranks, enabled preferences, primeira carga, OFF/ON, recarga, janelas e processo novo são observados pelo manager real.
- `tests/hal2224-pair/run.py`: regressão HAL22 do PR17, reduzida ao par AI/Dual. Mantém controles naturais com planner/controller reais e **controles adversariais/probes com seleção sintética**, identificados nos nomes. Não usar esses probes como prova de escolha ilegal em gameplay. Inclui ordenação de dez manifests, staged ranks e a suíte oficial de hooks.

Dependências locais por SHA256 em INPUTS.json/PAIR_INPUTS.json: AI oficial API24 (`ai-hard-mode.o`/manifest), Dual privado do PR10 (`dual-play.o`/manifest), RD PR13 (`ritual-duelists.o`/manifest), Melhoria PR9 (`rituals.o`/manifest), dez unidades i386 nativas anteriores e exports do SDK. Não são baixadas nem recompiladas. O operador deve obter legalmente a cópia Dual privada; nenhum arquivo Dual acompanha esta branch.

```sh
export MEMORIES_AI_DIR=/caminho/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/dual-play-privado
export MEMORIES_RD_DIR=/caminho/ritual-duelists
export MEMORIES_RITUALS_DIR=/caminho/melhoria-dos-rituais
export MEMORIES_OWN_MODS_DIR=/caminho/arvore-propria-do-PR14/mods
export MEMORIES_ENGINE_OBJ_DIR=/caminho/game32/obj
export MEMORIES_SDK_EXPORTS=/caminho/sdk/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='qemu-i386-static -L /caminho/sysroot'
python3 tests/hal2224/run.py --output /tmp/hal2224-main
python3 tests/hal2224-pair/run.py --output /tmp/hal2224-pair
```

Usar diretórios novos. GCC precisa de libc/libpng/pthread/math i386. Runner pode ficar vazio em host i386 nativo. O diretório OWN_MODS_DIR fornece somente os manifests próprios exatos de Melhoria, Ritual Effects, Fusion Effects e RD para a regressão de ordem.

Ambos os runners terminam1 no modo completo por manterem os controles OFF falhando. `--cohort on` existe no runner do par e deve retornar0. Na matriz principal, separar os perfis de guard ativo e os controles `arm-*`, que habilitam o guard depois de carregar um processo legado. Saída2/incompletude deve ser tratada como bloqueio técnico, não aprovação. Recusa do perfil é PASS de proteção e NOT_RUN do controller, não gameplay bem-sucedido.

O observer principal lê seleção/pending depois do planner. Os casos `late-swallow` e `late-transparent` **inserem uma chamada ao manager por callback próprio** para testar uma primeira carga de código antes do swap: a PoC recusa antes de inicializar o wrapper, sem escrever pending/plano/quota. Isso é reentrância instrumentada, não uma janela de UI demonstrada. Wrappers já registrados em swap/controller são recusados conservadoramente; wrappers em alvos não relacionados continuam normais.

Com AI OFF, a VM retail é um stub de status de conclusão, sem fornecer seleção. O planner RD toma a decisão a partir do snapshot real. No exemplo AI ON/ordem H→RD→D/variante5, os planners reais produzem680 e o stub não executa; o bônus é Monster535, enquanto Support344 fica oculto por sua quota já gasta. O par conserva seus controles reais de344/35. Os dados de catálogo/RAM são sintéticos.

O controller nativo prepara, seleciona, materializa e consome a instância física até fase6/7. Conclusão de efeito/placement e entrada no campo são pontes declaradas; callbacks reais do Dual concedem bônus e recompõem a mão. Não se executa um duelo completo, bytecode retail, invocação ritual integral, savestate real ou UI Windows. `Platform_ShowError` e `Platform_RestartGame` são observadores/stubs, enquanto as rotinas de Apply/confirmation da janela são originais com o preflight experimental. O runner cria processos novos de fato, mas não relança o aplicativo Windows.

Somente harnesses, observadores próprios e a unidade experimental de entrada do duelo são compilados. Os checks de gate comparam **callbacks executados**, não apenas Hooks_At. As leituras de blocos privados dependem dos hashes e não são API pública nem mecanismo de correção.

O resumo/matriz publicados podem ser regenerados a partir dos JSONs sem reexecutar jogo/testes:

```sh
python3 tests/hal2224/summarize.py
```

O diff da PoC pode ser revertido independentemente dos testes. Não é autorização para merge ou adoção permanente. A política de recusa, os fingerprints qualificados, o opt-in e a UX precisam de revisão Windows e regressões integradas antes de promoção.
