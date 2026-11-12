# HAL-20 — Primeira implementação: somente a base oficial

**Resultado:** a branch `integration/hal-20-v0.3.1-preview.2` foi atualizada para o commit oficial `aceac3782902746f35ddb0148809450933327cb3`, preservando o histórico. Há builds Linux i386 e Windows i386 concluídos, API 24 e SDKs gerados. A base está preparada para iniciar a portabilidade controlada dos MODs, mas a coleção customizada ainda não foi integrada ou validada.

- [PR #8 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8) — não mesclar automaticamente.
- [Branch de integração](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/integration/hal-20-v0.3.1-preview.2).
- [Resultados estruturados de todos os 179 casos CTest e validações complementares](hal-20/base-oficial/RESULTADOS.json).
- [Comandos, limitações e trechos dos logs](hal-20/base-oficial/EXECUCAO.md).
- [Inventário anterior / PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/7), preservado na branch documental; [relatório](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md) e [CSV de 495 arquivos](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/hal-20/INVENTARIO_ARQUIVOS.csv).
- [HAL-20 no Linear](https://linear.app/half-programer/issue/HAL-20/base-sincronizar-fork-e-versoes-atuais-dos-mods-com-upstream-v031).

Documentação registrada em 2026-11-11T22:42:03.748791-03:00 (UTC−03:00).

## 1. Escopo implementado e preservação

Foram lidos AGENTS.md, o relatório e o CSV da branch `docs/hal-20-inventario-plano` antes da implementação. A autorização desta etapa cobre exclusivamente a atualização da recompilação-base oficial e sua validação/documentação.

| Referência | SHA / estado |
|---|---|
| Master preservada | `0d78202729967a5bbdbc886aa128e48e82bef7ac` |
| Base documental lida | `2fb97b7f9b1c5b9e1ed360570ecc9f3026fcbcaa` |
| Tag oficial | `v0.3.1-preview.2` |
| Commit oficial importado | `aceac3782902746f35ddb0148809450933327cb3` |
| Árvore oficial | `01b6c45a046af957b6609f5a47d50736bea74828` |
| Operação Git | Avanço direto (fast-forward) de 907 commits, sem rebase/squash/reset/force-push |

O checkout original em `work` permaneceu limpo e no HEAD antigo. Foi usada uma cópia Git isolada para compilar/testar. Todas as branches antigas devem manter os SHAs registrados em RESULTADOS.json. A publicação da documentação acrescenta um commit filho do commit oficial; os arquivos de código continuam idênticos à tag.

Não foram mescladas branches antigas de MODs nem copiados os pacotes/objetos recebidos do usuário. Nenhum patch local foi aplicado a src/, mods/, tools/, CMakeLists.txt, configs ou workflows. A atualização inclui as revisões **oficiais** de IA, dados e MODs distribuídos que já pertencem à tag solicitada; não há adaptações próprias sobre elas.

Os únicos arquivos adicionais desta entrega são este relatório, AGENTS.md atualizado para continuidade e os dois arquivos de evidência em notes/hal-20/base-oficial/. Não há executáveis, objetos, imagens do jogo, saves pessoais ou assets adicionais na publicação. Os artefatos de build e fixtures ficaram em diretórios temporários ignorados.

## 2. Compilações e testes efetivamente executados

| Validação | Resultado |
|---|---|
| Build completo Linux i386 | Concluído: 628 unidades de jogo, 773 símbolos de dados fixados; versão v0.3.1-preview.2 |
| Build completo Windows i386 por cross-compilação | Concluído: 628 unidades, 773 símbolos; executável PE32 i386, versão v0.3.1-preview.2 |
| MODs oficiais distribuídos | 3D Monsters, AI Hard Mode, Hand camera e Yamyi compilados em cada build; Drop missing cards copiado como dados |
| SDKs Linux e Windows | Gerados com API 24, include/, tools/, examples/, notes/ e exports.i386.txt/exports.txt |
| Suíte CMake Linux | Compilação completa concluída |
| Suíte CMake Windows | Cross-compilação completa concluída; executáveis de teste não rodados em Windows/Wine |
| CTest Linux original, 179 registros | 164 aprovados, 13 falhas, 2 ignorados |
| 12 casos com bloqueio de execução i386 | Todos passaram em execução complementar sob QEMU |
| Ciclo de vida de MODs / pthread | Passou sob QEMU com -pthread acrescentado somente pelo launcher temporário; script oficial inalterado |
| Cobertura combinada dos mesmos 179 registros | 177 com execução bem-sucedida (13 dependem dos ajustes de ambiente descritos), 2 ignorados por falta de dados do disco; não é uma rodada CTest original inteiramente verde |
| ASan/UBSan: seleção de componentes nativos | 19 registros CTest passaram; LeakSanitizer desabilitado explicitamente |
| Layout ABI i386 Linux/Windows | 698 headers comparados, 72 não compilam isoladamente, zero estruturas divergentes |
| Layout ABI Windows x64/mod x64 | 759 headers comparados, 11 não compilam isoladamente, zero estruturas divergentes; não equivale a build/runtime completo x64 |
| Exports do executável Linux | Verificador oficial passou sob QEMU: 4.089 exports do jogo confrontados com o link |
| Exemplo gameplay-rules pelo SDK gerado | Compilou em i386 usando o SDK Linux e o SDK Windows separadamente |
| PC audit / VT check / Model gate | Audit gerou inventário; VT: 19 testes OK; Model gate: 22 testes OK |
| Regressão LIBGS de prioridade | Passou sob QEMU usando sysroot para compilar o harness |

Os 179 registros CTest não são a contagem de todos os testes unitários internos executados pelos scripts Python. Dentro dos grupos que retornaram sucesso, houve **79 ocorrências internas de skip**: 45 nos testes de launchers de SDK e 34 em grupos do FM Editor. Alguns grupos de GUI não executaram nenhum caso real. A cobertura gráfica/Tk/Windows não deve ser inferida do status do grupo. RESULTADOS.json preserva esses detalhes.

A seleção com sanitizadores cobre arquivos de jogo/picker, core, regras/fusões, AI Hard Mode oficial, tabelas, gerenciador de MODs, state IO/remap e a fixture scratch_sweep exigida pelo CTest. Não cobre todos os binários i386, GUI ou gameplay e não valida detecção de leaks (`ASAN_OPTIONS=detect_leaks=0`).

## 3. Erros reais e classificação

### Ambiente: execução i386 nativa indisponível

O kernel desta sessão rejeita ELF32 com `OSError: [Errno 8] Exec format error`. Doze falhas da rodada original decorreram disso. Foi baixado QEMU i386 estático de um pacote Debian verificado por SHA-256, extraído no workspace, sem sudo ou alteração do sistema. Um launcher externo ao repositório encaminhou apenas filhos ELF32 ao QEMU, conservando comandos, fontes e assertions. Os doze casos passaram; o loader reportou 55/55 verificações e rejeitou 10.363 cópias danificadas na fixture.

Isso é cobertura em emulação; não demonstra execução nativa no host nem valida gameplay Windows.

### Teste oficial: pthread com glibc 2.31

`tools/pc/test_mods_lifecycle.py:46–48` liga o harness Linux com `-lpng -lm`, sem `-pthread`. O src/pc/mods/mods.c da tag usa pthread_sigmask/create/detach. No sysroot glibc 2.31, anterior à incorporação de pthread na libc, o link falhou com símbolos pthread indefinidos. O build completo do jogo já inclui `-lpthread` e passou.

Trata-se de uma limitação de portabilidade do harness **presente no fonte oficial**, manifestada por este ambiente; não foi introduzida por um patch de migração. Em execução complementar, um wrapper temporário acrescentou `-pthread` ao compilador: lifecycle, overlay, hooks e compatibilidade de estado passaram sob QEMU. O teste oficial permaneceu sem alteração. Uma correção permanente do harness fica separada em HAL-17, fora desta entrega.

### Dados e plataformas ausentes

- pc_rank_art e pc_font_art ignorados por ausência de game/DATA/WA_MRG.MRG. Não foi solicitado nem publicado dado proprietário.
- Testes gráficos/Tk têm skips internos; não há confirmação de UI/3D/HD no jogo.
- Windows foi compilado por cross-compilação; Wine não está disponível, e não houve execução real no Windows.
- macOS, Android e runtime x64 não foram compilados/executados nesta etapa; seus SDKs/toolchains de plataforma não foram preparados. Só a comparação de layouts x64 foi executada.
- Matching/overlays que dependem de entradas originais e toolchain MIPS, duelos, save/load real, áudio e input em jogo não foram executados.
- A matriz AI HARD MODE/Dual Play dos MODs customizados continua pendente: os pacotes não foram instalados nesta etapa. Foram exercitados apenas os testes disponíveis do AI oficial da base.

### Erro histórico separado e correções da própria orquestração

A falha histórica da branch mod-field-effects (run 37518883452, callback terrain de três argumentos contra fixture de dois) não foi transportada: essa branch não entrou na integração, e o teste AI oficial desta base compilou/passou. Não atribuir essa falha antiga à atualização atual.

Houve uma chamada inicial do verificador de layouts com `--target windows`, opção inválida; ela foi corrigida para os alvos documentados i386/x86_64-windows. Na primeira seleção ASan, CTest incluiu automaticamente a fixture scratch_sweep, que ainda não havia sido compilada; ela foi compilada e a seleção passou. O primeiro verificador de exports tentou execução i386 nativa e foi reexecutado sob QEMU. Essas ocorrências são de orquestração/ambiente, sem correção de fonte e sem falha restante nesses checks corrigidos.

## 4. SDK: pronto e adaptações para a próxima fase

Confirmado `MEMORIES_MOD_API 24` tanto em src/pc/mods/mod_types.h quanto nos headers dos dois SDKs gerados. Os scripts e headers vieram desta própria árvore oficial, sem reutilizar SDK antigo. O toolchain Windows foi obtido pelo builder oficial com arquivos/pins verificados: llvm-mingw 20260922, Clang 23.1.2; Linux usou GCC 14.2 e o sysroot suportado de Debian 11.

O SDK Linux gerado lista 4.175 exports i386; o Windows lista 4.217. A diferença corresponde a nomes específicos das plataformas (incluindo wrappers Win32), portanto comparar imports com **cada alvo suportado**. Os 4.089 exports impressos pelo executável Linux não incluem a lista adicional de libc emprestada pelo loader.

Para receber os MODs atualizados:

1. Usar o SDK/header/builder da versão-alvo e indicar `--target i386` explicitamente para o Windows oficial desta entrega. Não copiar objetos antigos só porque min_api é baixo ou nm encontra os nomes.
2. Manter ID, library e chaves/padrões aprovados; declarar min_api conforme os recursos realmente exigidos, sem subir todos para 24 por conveniência.
3. O builder compila os C da pasta do MOD e produz objetos por arquitetura; pacotes com adapters em subpastas ou processo de relink próprio precisam de build completo conhecido. Time Wizard ainda depende do núcleo-fonte ausente e de offsets de objeto.
4. Objetos Windows x64 são outro alvo (`<library>.x86_64-windows.o`), com calling convention própria, prelude64/G32 e tag `.memories.abi`. Não os carregar no executável i386. Revisar ponteiros, casts e layouts antes de suportar x64.
5. Usar as declarações efetivas dos headers atuais e validar imports, ABI e hooks. A variante de campos ainda usa callback terrain diferente do AI oficial; adapter AI 0.1.1 continua ausente. Não transplantar planners antigos.
6. Revalidar a ponte Time Wizard/Dual Play após qualquer recompilação: seu reconhecimento depende dos bytes do objeto Dual Play 0.5.7 recebido. Um eventual contrato público exige proposta e autorização separadas.
7. Testar save/load e estado por MOD, providers ausentes/desativados e as quatro combinações AI/Dual antes de declarar a coleção compatível.

Não houve adaptação de API de MOD customizado neste PR. O suporte da API 24 e os serviços adicionados estão disponíveis; a portabilidade dos clientes é a próxima fase, sujeita a autorização específica.

## 5. Prontidão e próxima decisão

**Pronta como base oficial de desenvolvimento para começar a integração isolada dos MODs. Não pronta como distribuição da coleção completa.** Código, histórico, API, SDKs e builds principais estão disponíveis; permanecem as limitações de runtime/plataforma e as lacunas de fontes/proveniência do inventário anterior.

Próximo passo proposto: escolher um MOD de fontes completas para recompilar isoladamente com o SDK-alvo, mantendo AI/Dual como requisitos de teste. Não iniciar automaticamente adaptações de Time Wizard, AI, campos, receitas, decks, drops ou mecânicas. O PR #8 deve continuar em rascunho até revisão; a master permanece preservada.

O estado remoto do CI e a preservação das branches são registrados em RESULTADOS.json. Ausência de runs retornadas para a branch não comprova workflow desativado; os resultados locais publicados não devem ser apresentados como checks verdes do GitHub Actions.
