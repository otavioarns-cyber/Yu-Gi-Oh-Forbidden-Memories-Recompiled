# HAL-20 / HAL-26 — estado e preparação do Time Wizard

Entrega em execução na branch experimental `test/hal-20-state-timewizard-readiness`, continuação exata do PR29/commit `3284ec493d179f4b23cc7230a2979a1c2b122873`, derivado da integração oficial `d649647c159c91d615c07d6960928a37e270b461`. Sem merge/release, objetos atuais preservados. O relatório será completado nesta mesma branch com a matriz final.

## Candidata B: recusa independente do guard de ordem

O manager deixou de condicionar segurança de load a `experimental.hal2224_order_guard`. Nesta branch experimental, recusa load quando um dos owners conhecidos com cobertura incompleta está efetivamente ativo **ou com biblioteca residente**, inclusive após OFF. Abrange M/R/RD/F/Field/H/D e Time Wizard se futuramente carregado, sem admitir este último na coleção. Uma consulta interna somente leitura identifica biblioteca mapeada (`image` ou `native_handle`), sem usar hash curto como prova de residência. Nenhum algoritmo, setting, manifest ou objeto de MOD foi alterado.

Essa é uma candidata experimental de política B, não uma decisão permanente: desligar os MODs residentes não libera load no processo atual. Um processo novo com todos esses MODs OFF mantém o loader legado e suas checagens existentes; isso não certifica estados antigos, outros MODs ou perfis desconhecidos. Memory-card não passa pelo predicado de savestate.

O preflight comum existente em `guest/state.c` permanece antes de leitura/parse/remap/overwrite; mensagem explícita preserva o duelo atual. Não há rollback inventado nem restauração de cartas por ID. Nenhuma semântica global de hooks/ordenação foi alterada.

## Validação inicial

Manager real: **1.048.576 perfis PASS**, incluindo ativo/residente/OFF frio/H/D-only/TW. Controle portátil:88 combinações PASS. A matriz conjunta pertinente e a avaliação A/Time Wizard estão em andamento; não extrapolar estes predicados para load integral ou gameplay Windows.

## Controle Windows independente

Workflow restrito à branch experimental/PR para integração executa somente C próprio em PE32 numa máquina Windows, sem ROM, binários de jogo ou Dual privado, com permissão contents:read. Usa toolchain x86 do runner; não instala nem altera runtime global. Resultado será registrado quando disponível. Wine/QEMU não está sendo repetido.
