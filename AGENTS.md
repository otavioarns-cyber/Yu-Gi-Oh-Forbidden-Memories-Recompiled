# Continuidade do projeto — instruções para agentes e chats

## Preferência expressa do usuário

Em 11/11/2026, o usuário pediu que as informações das entregas e mudanças sejam publicadas em uma branch do GitHub para compartilhar com outro chat que ajuda na programação, e que esse procedimento seja mantido nas próximas etapas.

- Ao concluir uma etapa técnica, manter o registro correspondente na branch da tarefa: resultado real, arquivos afetados, decisões, validação executada, limitações e próximos passos.
- Publicar a documentação autorizada e fornecer links da branch, documento e PR. Usar PR em rascunho enquanto houver revisão/implementação pendente; não fazer merge automático.
- Distinguir sempre mudanças realizadas, diferenças encontradas em insumos do usuário e propostas ainda não implementadas. Não tratar achado de auditoria como correção aplicada.
- Atualizar o ponto de partida abaixo quando houver nova entrega. Preservar relatórios anteriores e referências de commits/PRs; não depender apenas do histórico de chat.
- Essa preferência autoriza documentação/rastreabilidade dentro do escopo da tarefa. Não amplia autorização para modificar mecânicas, contratos públicos, terceiros, workflows/permissões, ou promover branches à master.

## Ponto de partida atual

- Diagnóstico mais recente nesta branch: [HAL-22 — I2 / materialização AI HARD MODE × Dual Play](notes/HAL22_AI_HARD_MODE_DUAL_PLAY.md), [resultados](notes/hal-22/resultados/RESULTADOS.json), [matriz CSV](notes/hal-22/resultados/MATRIZ.csv) e [reprodução](tests/hal22/README.md). Branch `test/hal-22-ai-hard-mode-dual-play`, derivada da integração oficial. 138 PASS/30 FAIL/8 BLOCKED, mais 1 PASS entre perfis; controller/planner oficiais com catálogo/UI e ponte de efeitos simulados, sem gameplay Windows. A contabilidade incorreta foi reproduzida sem injetar planos nesses cenários; alcance em decks reais continua não comprovado. Nenhuma correção funcional, manifest ou objeto alterado.
- Evidência anterior preservada: [primeira integração / PR #14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14), commit `ae43436e0e23437642ab45eb4da2b4a9374c2e39`. HAL-22 contém apenas AI oficial e Dual local privado; outros MODs não foram importados. A ordem H→D é segura nas fixtures de carga inicial; primeira carga tardia do AI pode inverter a cadeia apesar da ordem configurada. Ler as propostas e limites antes de implementar; I1/I3/F1 permanecem separados.
- Base oficial de referência: [HAL-20 — base oficial atualizada e resultados](notes/HAL20_BASE_OFICIAL_V031_PREVIEW2.md), [PR #8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8).
- Inventário anterior: [HAL-20 — inventário e plano](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md), [PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/7).
- Evidência integral: [495 arquivos do ZIP, tamanhos e SHA-256](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/hal-20/INVENTARIO_ARQUIVOS.csv).
- Branch de integração atual: `integration/hal-20-v0.3.1-preview.2`; branch documental anterior: `docs/hal-20-inventario-plano`. Preservar ambas e as branches antigas.
- Relatório anterior: [HAL-5](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-5-auditoria-mods/notes/MODS_AUDITORIA_E_PLANO.md), [PR #6](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/6).
- Primeira implementação autorizada concluída: somente a base oficial `aceac3782902746f35ddb0148809450933327cb3` (v0.3.1-preview.2/API 24), com builds Linux/Windows i386 e SDKs. A master continua em `0d78202729967a5bbdbc886aa128e48e82bef7ac`. MODs customizados ainda não integrados; detalhes de testes/emulação/skips no relatório. Não executar as próximas fases sem escopo autorizado.

## Restrições para esta fase de implementação controlada

- Preservar a master e o histórico existente; não resetar, forçar push ou mesclar automaticamente.
- Não alterar mecânicas, receitas, decks, drops ou dificuldade sem autorização específica.
- Manter MODs independentes, uma versão instalável por ID e integrações opcionais com fallback.
- AI HARD MODE e Dual Play são requisitos explícitos de toda análise de compatibilidade. Dual Play recebido é Monster + Support; testar separadamente o modo nativo de dois humanos/controles quando pertinente.
- Não modificar código de terceiros desnecessariamente nem presumir autorização para isso.
- Não incluir imagem/ROM do jogo, saves pessoais, segredos ou conteúdo proprietário; não publicar automaticamente binários/assets de terceiros.
- Não presumir compatibilidade de objetos antigos com SDK novo nem alegar gameplay validado a partir de nomes de imports ou compilação.

## Orientação para o chat que continuar o trabalho

Leia a entrega mais recente, o relatório de inventário e o CSV antes de propor alteração. Os pacotes originais não acompanham a branch documental; peça somente os materiais que realmente faltarem, usando os hashes do inventário para identificar versões. Para a próxima implementação, confirme o escopo autorizado pelo usuário; a publicação documental não autoriza executar o plano inteiro. Registre também as lacunas e os testes não executados.
