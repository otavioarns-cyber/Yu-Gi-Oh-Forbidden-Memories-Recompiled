# Integração experimental HAL-20 / HAL-6 / HAL-18

Este runner carrega os objetos i386 preservados dos PRs #9/#11/#12/#13 e duas dependências locais: AI HARD MODE oficial e Dual Play 0.5.7 recompilado do PR #10. Usa os fontes oficiais object_loader.c, hooks.c, events.c, manager.c e json.c. Engine, catálogo, render e controle são simulados: **nenhum resultado equivale a gameplay Windows**.

Os MODs importados são imutáveis nesta branch. Os testes novos ficam aqui, fora das árvores dos MODs. [INPUTS.json](../../notes/hal-20/integracao-inicial/INPUTS.json) identifica todos os insumos; o runner recusa árvores divergentes e objetos locais com hashes diferentes. Não baixa, recompila, substitui ou publica dependências automaticamente. O Dual não acompanha o repositório; obtenha o candidato autorizado para uso local, não um objeto antigo do ZIP.

```sh
export MEMORIES_AI_DIR=/caminho/local/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/local/dual-play
export MEMORIES_I386_CC=/caminho/gcc-com-suporte-i386
export MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot'
python tests/hal20-integration/run.py --output /tmp/hal20-integracao
```

GCC i386 pode substituir o wrapper e o prefixo QEMU pode ser omitido em um host capaz de executar ELF32. As dependências precisam de mod.json local e dos objetos esperados; o SDK/build da base é necessário para obtê-los. Não incluir o diretório local de Dual na árvore Git. Os objetos dos quatro MODs próprios já acompanham as árvores preservadas.

São 25 perfis: quatro etapas progressivas × AI OFF/ON × Dual OFF/ON na ordem O0; oito variantes do conjunto S4 nas ordens O1/O2; um contraponto mínimo AI/Dual em S1/O2. O programa de ordem utiliza os manifests reais e Mods_Order/Mods_Compatible oficiais. Drop Missing Cards 1.0 é ativado no perfil, mantendo seu manifest oficial intacto. O harness de ELF não implementa a composição integral das tabelas de drops; esse balanceamento continua bloqueado.

Os perfis textuais em profiles/ usam o formato do gerenciador oficial, com defaults extraídos dos manifests e flags/ranks experimentais. Outros MODs da base ficam OFF para isolamento. São insumos optativos de teste; não são aplicados aos dados pessoais do usuário. Importação no jogo, gerenciamento completo de restart e gameplay não foram executados. O harness ativa callbacks/relink diretamente para diagnosticar OFF/ON, inclusive janelas de transição entre planejamento e materialização.

Reproduções focadas:

```sh
# F1 conhecida: deve continuar FAIL/saída não zero.
python tests/hal20-integration/run.py --stage 3 --hm 0 --dual 0 --order 0 --scenario F1 --output /tmp/hal20-f1
# Ação própria injetada de RD: compara sua passagem pelo gate Dual.
python tests/hal20-integration/run.py --stage 4 --hm 1 --dual 1 --scenario rd-materialize --output /tmp/hal20-rd-gate
# Sequência setup -> planner -> swap, sem injetar pending.
python tests/hal20-integration/run.py --stage 4 --scenario planner --output /tmp/hal20-planner
# Ação criada pelo planner real com HM OFF; ligar HM antes do swap.
python tests/hal20-integration/run.py --stage 4 --hm 1 --dual 0 --scenario transition --output /tmp/hal20-transition
# Contraponto mínimo: HM/Dual, sem Ritual Effects, Fusion ou RD.
python tests/hal20-integration/run.py --stage 1 --hm 1 --dual 1 --order 2 --output /tmp/hal20-minimal
```

Os callbacks são rastreados por thunks de teste que preservam os registradores/flags i386 e encaminham à função original do MOD. O encadeamento e os ponteiros previous são geridos/verificados pelo código oficial. Nenhum helper estático é chamado por uma ABI adivinhada; campos privados são lidos/semeados apenas nos cenários que declaram isso. Relocações e hooks normais do loader ocorrem na memória de teste, sem alteração dos arquivos .o. Time Wizard não é carregado nem seu scanner de bytes é validado com esses wrappers.

O runner retorna não zero diante dos FAIL diagnósticos. Não interpretar o término bem-sucedido da coleta como aprovação da integração. [Relatório](../../notes/HAL20_INTEGRACAO_INICIAL.md), [matriz](../../notes/hal-20/integracao-inicial/MATRIZ.md), [resultados/logs](../../notes/hal-20/integracao-inicial/RESULTADOS.json).
