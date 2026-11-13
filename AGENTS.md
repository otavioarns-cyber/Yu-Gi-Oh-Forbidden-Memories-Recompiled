# Continuidade do projeto — instruções para agentes e chats

## Preferência expressa do usuário

Em 11/11/2026, o usuário pediu que as informações das entregas e mudanças sejam publicadas em uma branch do GitHub para compartilhar com outro chat que ajuda na programação, e que esse procedimento seja mantido nas próximas etapas.

- Ao concluir uma etapa técnica, manter o registro correspondente na branch da tarefa: resultado real, arquivos afetados, decisões, validação executada, limitações e próximos passos.
- Publicar a documentação autorizada e fornecer links da branch, documento e PR. Usar PR em rascunho enquanto houver revisão/implementação pendente; não fazer merge automático.
- Distinguir sempre mudanças realizadas, diferenças encontradas em insumos do usuário e propostas ainda não implementadas. Não tratar achado de auditoria como correção aplicada.
- Atualizar o ponto de partida abaixo quando houver nova entrega. Preservar relatórios anteriores e referências de commits/PRs; não depender apenas do histórico de chat.
- Essa preferência autoriza documentação/rastreabilidade dentro do escopo da tarefa. Não amplia autorização para modificar mecânicas, contratos públicos, terceiros, workflows/permissões, ou promover branches à master.

## Ponto de partida atual

- PoC experimental mais recente nesta branch: [HAL-22 + HAL-24 — guard conjunto](notes/HAL22_24_THREE_WAY_GUARD_POC.md), [diff reversível](notes/hal-22-24/DIFF_EXPERIMENTAL.patch), [matriz](notes/hal-22-24/MATRIZ.md), [resumo](notes/hal-22-24/SUMMARY.json) e [reprodução](tests/hal2224/README.md). Branch `test/hal-22-24-three-way-guard-poc`, [PR #20 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/20), derivada da integração oficial. Opt-in novo `experimental.hal2224_order_guard=1`, default OFF. Cinco fontes PC alterados somente no protótipo; MODs, Hooks_Relink/Mods_Order e manifests permanentes intactos. Coortes protegidas: 255 processos/3058 PASS/zero FAIL/24 SKIP; controles OFF: 57 processos/171 FAIL conhecidos. A principal recusa 98 processos antes do controller e admite 106. Qualifica AI/RD/Dual i386 e exige H→RD→D, ou pares pertinentes. Wrappers críticos desconhecidos e primeira carga ao vivo desconhecida ficam bloqueados; alvo não relacionado preservado. Startup recusado exige relançamento mesmo após reparar/desligar opt-in. Não promover a integração/master ou fazer merge; UX, demais objetos e Windows real ainda pendentes. HAL21/F1 e pending HAL23 não foram corrigidos.
- Histórico de diagnóstico preservado: [PR #19/HAL-24](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/19), [PR #18/HAL-23](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/18), [PR #17](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/17), [PR #16](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/16), [PR #15](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/15) e [PR #14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14). Nada desses PRs foi alterado ou mesclado.
- Base oficial anterior: [HAL-20 — base oficial atualizada e resultados](notes/HAL20_BASE_OFICIAL_V031_PREVIEW2.md), [PR #8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8).
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
