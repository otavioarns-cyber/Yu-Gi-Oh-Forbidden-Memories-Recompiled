# Continuidade do projeto — instruções para agentes e chats

## Preferência expressa do usuário

Em 11/11/2026, o usuário pediu que as informações das entregas e mudanças sejam publicadas em uma branch do GitHub para compartilhar com outro chat que ajuda na programação, e que esse procedimento seja mantido nas próximas etapas.

- Ao concluir uma etapa técnica, manter o registro correspondente na branch da tarefa: resultado real, arquivos afetados, decisões, validação executada, limitações e próximos passos.
- Publicar a documentação autorizada e fornecer links da branch, documento e PR. Usar PR em rascunho enquanto houver revisão/implementação pendente; não fazer merge automático.
- Distinguir sempre mudanças realizadas, diferenças encontradas em insumos do usuário e propostas ainda não implementadas. Não tratar achado de auditoria como correção aplicada.
- Atualizar o ponto de partida abaixo quando houver nova entrega. Preservar relatórios anteriores e referências de commits/PRs; não depender apenas do histórico de chat.
- Essa preferência autoriza documentação/rastreabilidade dentro do escopo da tarefa. Não amplia autorização para modificar mecânicas, contratos públicos, terceiros, workflows/permissões, ou promover branches à master.

## Ponto de partida atual

- Entrega experimental mais recente nesta branch: [HAL-20 — primeira integração controlada / HAL-6 e HAL-18](notes/HAL20_INTEGRACAO_INICIAL.md), [matriz](notes/hal-20/integracao-inicial/MATRIZ.md), [resultados](notes/hal-20/integracao-inicial/RESULTADOS.json). Branch `test/hal-20-integracao-inicial`, publicada no [PR #14 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14) para a integração. 25 perfis/28 execuções ELF, 480 PASS/29 FAIL, quatro bloqueios/quatro skips globais; não é gameplay Windows. F1/HAL-21 reproduzida e não corrigida. Riscos/condições de gates e pending documentados, sem patches de produção.
- Base preservada: [HAL-20 — base oficial atualizada e resultados](notes/HAL20_BASE_OFICIAL_V031_PREVIEW2.md), [PR #8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8).
- Componentes próprios importados exatamente, sem merge dos PRs de origem: [Melhoria/Surrender #9](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/9), [Ritual Effects #11](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/11), [Fusion Effects #12](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/12), [Ritual Duelists #13](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/13). Árvores/hashes no [inventário desta integração](notes/hal-20/integracao-inicial/INPUTS.json).
- Dual Play do [PR #10](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/10) somente como dependência local de teste; não publicar fonte/binário/manifest sem permissão verificável. Time Wizard, adaptador AI 0.1.1 e Droplist externo continuam bloqueios separados.
- Inventário anterior: [HAL-20 — inventário e plano](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md), [PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/7).
- Evidência integral: [495 arquivos do ZIP, tamanhos e SHA-256](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/hal-20/INVENTARIO_ARQUIVOS.csv).
- Branch de integração atual: `integration/hal-20-v0.3.1-preview.2`; branch documental anterior: `docs/hal-20-inventario-plano`. Preservar ambas e as branches antigas.
- Relatório anterior: [HAL-5](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-5-auditoria-mods/notes/MODS_AUDITORIA_E_PLANO.md), [PR #6](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/6).
- Primeira implementação autorizada concluída: base oficial `aceac3782902746f35ddb0148809450933327cb3` (v0.3.1-preview.2/API 24), com builds Linux/Windows i386 e SDKs. A master continua em `0d78202729967a5bbdbc886aa128e48e82bef7ac`. MODs customizados foram reunidos somente na branch experimental atual; não foram promovidos à master/integração. Próximas correções funcionais exigem escopo autorizado próprio.

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
