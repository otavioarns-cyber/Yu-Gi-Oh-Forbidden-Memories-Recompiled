# Continuidade do projeto — instruções para agentes e chats

## Preferência expressa do usuário

Em 11/11/2026, o usuário pediu que as informações das entregas e mudanças sejam publicadas em uma branch do GitHub para compartilhar com outro chat que ajuda na programação, e que esse procedimento seja mantido nas próximas etapas.

- Ao concluir uma etapa técnica, manter o registro correspondente na branch da tarefa: resultado real, arquivos afetados, decisões, validação executada, limitações e próximos passos.
- Publicar a documentação autorizada e fornecer links da branch, documento e PR. Usar PR em rascunho enquanto houver revisão/implementação pendente; não fazer merge automático.
- Distinguir sempre mudanças realizadas, diferenças encontradas em insumos do usuário e propostas ainda não implementadas. Não tratar achado de auditoria como correção aplicada.
- Atualizar o ponto de partida abaixo quando houver nova entrega. Preservar relatórios anteriores e referências de commits/PRs; não depender apenas do histórico de chat.
- Essa preferência autoriza documentação/rastreabilidade dentro do escopo da tarefa. Não amplia autorização para modificar mecânicas, contratos públicos, terceiros, workflows/permissões, ou promover branches à master.

## Ponto de partida atual

- Entrega mais recente nesta branch: [HAL-25 — qualificação de fronteiras e automação](notes/HAL25_BOUNDARY_QUALIFICATION.md), branch `test/hal-25-boundary-qualification`. RD valida cursor/pilha nativos antes da coleta; único delta funcional sobre PR26 é25 linhas no mandatory_hand_consumer.h. Builds Windows SDK24/i386 completos; 544 processos/3.180 checks PASS, classificados458 PASS/76 REFUSED/10 NOT_REACHED. Regressões canônicas57 PASS/1 auditoria imutável FAIL esperado/6 SKIP; Fusion16 grupos PASS. Quatro controles PR26 mantêm8 FAIL funcionais sob wrapper adversarial. Guard#20 não promovido, terceiros/core/estratégias/manifests/settings/assets intactos.
- **Diretriz atual de testes:** só os testes que exigem o usuário pessoalmente jogando ficam para a fase final. Automatizar Windows agora sempre que houver recursos legítimos; não solicitar testes manuais intermediários nem alegar proibição ampla de gameplay Windows. Tentativas Wine nesta entrega: controle PE64 PASS, controle/jogo PE32 indisponíveis antes de main; Wine32/QEMU também não iniciou. Imagem legítima ausente (startup Linux confirma). UI/restart/duelos/save-load integrais NOT_REACHED por esses impedimentos reais. Ver RISCOS.json; load em voo e admissão da coleção ainda não qualificados.
- Referência imediata preservada: [PR #26](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/26), commit `dc711074732e01275fb575aabda577c497ffe6d9`. A candidata local ainda exige revisão; não promover ABI/companion, guard, master ou integração sem autorização.
- Referência direta preservada: [PR #25](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/25), commit `f6cea112db8d14be34ab2f28694b0121a94842c2`, [contrato original](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/test/hal-25-contract-hardening/notes/hal-25-hardening/PR25_CONTRATO.md). Os objetos originais continuam na branch anterior; o hardening não modifica seus PRs. Não promover ABI/companion ou guard #20 sem revisão e autorização.
- Referência anterior preservada: [PR #24](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/24), [alternativas A/B e ponte de teste](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/test/hal-25-queen-lock-rd-poc/notes/HAL25_QUEEN_LOCK_RD_POC.md). Não incorporar guard #20 ou promover contrato/candidatos sem autorização.
- Base oficial anterior: [HAL-20 — base oficial atualizada e resultados](notes/HAL20_BASE_OFICIAL_V031_PREVIEW2.md), [PR #8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8).
- Inventário anterior: [HAL-20 — inventário e plano](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md), [PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/7).
- Evidência integral: [495 arquivos do ZIP, tamanhos e SHA-256](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/hal-20/INVENTARIO_ARQUIVOS.csv).
- Branch de integração atual: `integration/hal-20-v0.3.1-preview.2`; branch documental anterior: `docs/hal-20-inventario-plano`. Preservar ambas e as branches antigas.
- Relatório anterior: [HAL-5](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-5-auditoria-mods/notes/MODS_AUDITORIA_E_PLANO.md), [PR #6](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/6).
- Primeira implementação autorizada concluída: somente a base oficial `aceac3782902746f35ddb0148809450933327cb3` (v0.3.1-preview.2/API 24), com builds Linux/Windows i386 e SDKs. A master continua em `0d78202729967a5bbdbc886aa128e48e82bef7ac`. MODs customizados não integrados à branch oficial; cópias experimentais dos PRs #11/#13 acompanham esta branch com o contrato HAL-25; os originais permanecem nas branches anteriores; detalhes de testes/emulação/skips no relatório. Não executar as próximas fases sem escopo autorizado.

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
