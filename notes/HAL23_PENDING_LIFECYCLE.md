# HAL-23 — pending do Ritual Duelists após mudança do AI HARD MODE

**A HAL-23 foi reproduzida com o planner real do RD, mas somente ao inserir uma chamada reentrante ao manager entre o planner e o swap. Não foi demonstrada essa janela na UI normal.** O controller oficial executa os dois na mesma chamada, sem bombeamento de eventos entre eles. Sob a janela instrumentada, um pending abandonado pode restaurar um fallback antigo numa ação posterior e alterar a carta física consumida. Não foi observada materialização duplicada.

Branch independente `test/hal-23-pending-lifecycle`, derivada diretamente de `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461` (núcleo oficial `aceac3782902746f35ddb0148809450933327cb3`, API24). Diagnóstico ligado à [HAL-23 no Linear](https://linear.app/half-programer/issue/HAL-23). Sem alteração de produção, correção funcional ou merge.

Continuidade consultada: AGENTS.md, inventário HAL-20 e [PR14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14), [PR15](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/15), [PR16](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/16), [PR17](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/17). As referências e hashes estão em [INPUTS.json](hal-23/INPUTS.json). O guard e os cinco fontes experimentais do PR17 **não foram incorporados**.

## Respostas A–E

**A. A transição é alcançável num fluxo normal ou depende do teste?**

Nos caminhos examinados, **depende da inserção artificial do manager no intervalo planner→swap**. O planner real do Ritual Duelists 0.4.0-alpha.8 criou `deck_pull` sem injeção; a mudança de estado naquele intervalo foi feita pelo observador próprio do harness.

Em `src/game/duel_scene_hand_actions.c:176–184`, o controller chama `AiScript_Run`, verifica sucesso, prepara metadados de seleção e chama `func_8001BAF0` antes de retornar. Não há evento de UI nesse intervalo. O RD só passa a planejar após resultado não zero do predecessor; os caminhos de staging examinados retornam sucesso. Portanto o caso reproduzido não corresponde a um pending deliberadamente criado pelo planner durante um tick que ainda retornaria “aguarde”.

Os chamadores encontrados de Apply/SetEnabled/recarga estão no manager, janela de MODs e aplicação das configurações do menu. O bombeamento de eventos SDL está na plataforma, fora desse pequeno trecho do controller. Não foi identificado um chamador normal que altere o AI reentrantemente ali. Os controles que mudam o estado antes do planejamento ou após o tick planner+swap não abandonaram pending: **66 processos, 360 PASS, zero FAIL**.

Isso exclui a interpretação de que o simples usuário abrir o menu “entre dois frames” reproduza o caso antigo. Não prova impossibilidade para futuros callbacks, novos MODs, outra thread ou outro caminho nativo não exercitado. Sem imagem/runtime do jogo, a alcançabilidade com catálogo, bytecode e decks reais permanece não validada.

**B. O reinício obrigatório impede a transição em todos os caminhos exercitados?**

**Sim, para as mudanças efetivas de estado testadas neste diagnóstico:** candidato temporário `restart:true`, Apply, SetEnabled, recarga por Mods_Load e rotinas reais da janela. Foram **108 processos, 546 PASS, zero FAIL** nessa coorte; 86 ações criaram pending real e o consumiram. Não se trata apenas de um teste que deixou de planejar.

Primeira ativação: a preferência passa a ON, mas AI continua inativo e sua biblioteca não inicializa até o novo processo. AI já ativo: solicitar OFF não o desativa ao vivo. Em particular, o preparo “biblioteca já carregada → OFF → ON” não consegue criar um OFF efetivo sob essa política; os logs deixam esse controle negativo explícito.

Cancelar a confirmação da janela não aplica a alteração. Confirmar salva e pede relançamento. Na falha observada do relançamento, o manager preserva a cadeia ativa e a janela informa que é necessário relançar manualmente. As APIs diretas salvam a preferência sem implementar por si uma confirmação gráfica. Foram criados 12 processos novos do harness com os mesmos settings; todos iniciaram sem pending anterior.

O impacto seria ON/OFF do AI aplicado somente no relançamento, com aviso de perda de progresso não salvo. **O campo não foi alterado no manifest de produção.** Essa conclusão se limita à HAL-23: como demonstrado no PR16, restart sozinho não corrige uma ordem inicial inadequada para I2. A proteção do PR17 não está ativa aqui. Importação/substituição de pacote, overrides e relançamento gráfico real não foram certificados.

**C. Existe risco comprovado de duplicação, consumo errado ou persistência?**

Há **evidência condicional de seleção/consumo errado**, não uma jogada ilegal comprovada numa partida completa:

1. Mão física sintética com cinco instâncias distintas; magia 680 no próximo índice real não comprado. A proposta vanilla é modelada, mas o RD decide e cria seu pending real: origem lógica16, reposição física1, modo MAGIC, fallback12.
2. No callback de teste, AI OFF→ON. O worker oficial do AI materializa680 uma vez; o callback RD não executa e seu pending permanece ativo. O controller real remove a magia da mão e chega à fase7.
3. Após uma ponte **modelada** de placement/próxima compra e AI ON→OFF por API real, o RD processa outra seleção física14. O pending velho ainda está ativo. `rd_materialize` invalida a origem velha e restaura fallback12; o controller real consome110, embora a nova seleção14 correspondesse a112.
4. O pending então fica inativo. Não houve segunda troca offscreen; os 80 identificadores de registros continuaram conservados.

Isso ocorreu com Dual OFF e ON, em quatro follow-ups (primeira carga e biblioteca preexistente O2). Nos controles O0/O1 preexistentes e nas mudanças após o tick, a seleção14 permaneceu14 e foi consumido112. O caso não modifica o planner, não escreve pending e não cria novas cartas; a proposta stock12/14 e a reconstrução da próxima mão são modelos declarados.

[Log principal com Dual OFF](hal-23/resultados/O2-d0-r0-apply-gap-off-on-p0-next.log) · [Dual ON](hal-23/resultados/O2-d1-r0-apply-gap-off-on-p0-next.log) · [controle preexistente O0](hal-23/resultados/O0-d0-r0-apply-gap-off-on-p1-next.log).

| Limite de lifecycle após o caso instrumentado | Resultado observado |
|---|---|
| Próxima seleção sem reset | Pode restaurar fallback antigo e consumir outra carta física, conforme a sequência condicionada acima. Não observamos dupla materialização. |
| Inicialização real de novo script (`AiScript_Init`) | Limpa a seleção, mas não é reset do RD e não elimina sozinho o pending velho. Não representa botão de cancelar turno da IA pela UI. |
| `Mods_Reset` real | O callback `rd_reset` limpa pending. |
| Dealer real de Rex com geração bem-sucedida | Limpa pending. Catálogo e entrada do dealer são sintéticos; não é um novo duelo completo. |
| Dealer real com perfil inativo Isis/fallback | Delega antes do ponto de limpeza; conserva pending no teste condicionado. Não foi demonstrado erro numa partida nova. |
| Save/load simulado, mesmo perfil | Buffer RD registrado288/v4 não inclui `deck_pull`40B; restauração dos blocos + `Mods_Reset` + LOAD AFTER limpa pending. |
| Processo novo | Pending BSS começa0; settings não o persistem. |

Não há evidência de serialização indevida desse pending em save. `state_subsystems.c:70–81` também rejeita assinaturas/blocos incompatíveis; logs registram as assinaturas reais antes/depois de toggles, mas **não executamos carregamento de um arquivo savestate entre perfis**. Não presumimos compatibilidade de estados antigos ou serialização de toda a BSS.

**D. Qual a causa e o menor ajuste próprio, caso necessário?**

É uma combinação de **ordem efetiva, propriedade da ação e lifecycle**. Quando AI está externo ao RD no swap, seu caminho ativo `preserve_hand` materializa sem delegar. A ação foi planejada pelo RD antes da mudança, mas não há handshake de conclusão que consuma seu pending. O RD, quando volta a executar, valida os registros físicos, porém o fallback não identifica a qual decisão pertencia. Uma ação velha pode assim substituir a seleção nova pelo fallback antigo.

Não é exclusivamente o rank O2: primeira carga tardia registra AI depois do RD até em ranks salvos O0/O1, reproduzindo o abandono no gap artificial. Com biblioteca já registrada e O0/O1, RD executa primeiro e consome seu pending no recorte testado. Dual não é necessário para reproduzir HAL-23. Não deduzimos disso que RD externo valide quotas; esse é o problema separado I1, que não foi corrigido ou certificado aqui.

**Não há necessidade demonstrada de uma correção funcional para a UI normal nesta entrega.** A opção de menor superfície a avaliar primeiro é uma **fronteira segura de ativação**: não mudar o estado efetivo do AI no intervalo de uma ação; a política de reinício existente do manager já satisfez isso em todas as entradas executadas quando aplicada ao manifest temporário. Sua adoção exige aprovação e testes Windows, e deve coexistir com a validação de ordem HAL-22, não substituí-la.

Se for comprovado um chamador normal capaz de cruzar essa fronteira, a alternativa localizada é uma **transação/época de decisão no código próprio do RD** (`mod.c`, staging, `rd_materialize` e fronteira de nova decisão/duelo). O fallback só poderia ser restaurado se ainda pertencesse à mesma decisão. Ao iniciar uma decisão nova, a época anterior seria encerrada; a seleção nova jamais seria sobrescrita pelo fallback velho. Deve preservar o fallback válido da ação atual, HOLD/MAGIC/FIELD e o cancelamento legítimo. `AiScript_Init` é um candidato de fronteira a validar, não uma instrução para limpar pending em qualquer chamada.

Essa alternativa é maior que reutilizar a política de reinício e **não foi implementada**. Não se propõe chamar `previous_swap` após o AI já ter trocado, limpar pending a cada frame, alterar estratégia, mudar quotas nem modificar Dual. Uma revisão do fallback/new-deal pode ser necessária como robustez futura, mas não está autorizada por esta evidência condicionada como correção automática de gameplay.

**E. Quais regressões serão exigidas antes da integração?**

- Manter os controles reentrantes e de frame desta suíte; obter uma reprodução por chamador real antes de justificar correção transacional. Não transformar NOT_REACHED em aprovação do cenário de pending.
- Windows i386/API24: UI real, abertura/fechamento do menu, primeira carga, biblioteca já carregada, Apply/API/recarga, OFF→ON/ON→OFF, confirmação/cancelamento/falha e relançamento efetivo. Observar que o estado aplicado corresponde à mensagem e ao perfil salvo.
- AI+RD com Dual OFF/ON, O0/O1/O2 e histórico de ativação. Preservar o problema I2 e sua proposta como tarefa separada; regressar a validação de categoria/quota Monster/Support na cadeia efetiva correta.
- Planos RD MAGIC/FIELD/HOLD, fallback válido e inválido, nova decisão, cancelamento real quando existir, uma e múltiplas instâncias do mesmo ID. Conferir origem, seleção física, conservação e exatamente uma troca por instância que sai do deck.
- Rituais Vanilla/modificados e execução completa com materiais aprovados; efeitos/receitas/providers ausentes, OFF e ON no ecossistema integrado. Esta suíte não executa a invocação ritual nem integra Ritual Effects/Fusion Effects.
- Novo duelo real com Rex, perfil inativo, receita inviável/fallback e modo diferente, reset/load no momento certo. Não presumir que todo dealer sempre passa pela limpeza de geração bem-sucedida.
- Save/load completo no mesmo perfil e recusa de perfil incompatível, após pending recém-criado e após commit, respeitando callbacks/reset do motor. Sem prometer saves antigos.
- Conferir defaults e igualdade das decisões estratégicas antes/depois de qualquer proposta futura. `preserve_hand`, `deck_access`, decks, dificuldade e quotas devem continuar intactos.

## Evidências, execução e publicação

[Matriz por processo](hal-23/MATRIZ.md) · [CSV integral](hal-23/resultados/MATRIZ.csv) · [JSON de resultados](hal-23/resultados/RESULTADOS.json) · [reprodução](../tests/hal23/README.md) · [insumos/hashes](hal-23/INPUTS.json) · [inspeção de fontes](hal-23/SOURCE_EVIDENCE.json).

| Resultado | Quantidade |
|---|---:|
| Processos de ação / inicialização nova | 216 / 12 |
| Verificações PASS / FAIL / execução incompleta | 1018 / 52 / 0 |
| Pendings abandonados na primeira ação | 44, todos no gap instrumentado com manifest original |
| Fallback antigo alterou seleção posterior | 4, condicionados ao gap e ponte de próxima mão |
| AiScript_Init / dealer fallback não limparam pending velho | 2 / 2, condicionados ao gap |
| Preflight / pós-execução de insumos | 41 / 41 PASS |
| Builds de testes próprios | 2 PASS; nenhum MOD recompilado |

Os 52 FAIL preservam os achados: 44 de pending abandonado +4 de seleção posterior +2 de init +2 de dealer. Não representam 52 bugs distintos ou falhas da política candidata. A suíte retorna1 por eles, deliberadamente. Os avisos de compilação são asserções de offset antigas dos headers oficiais; os builds terminaram0.

Os objetos foram reutilizados sem alteração: AI SHA256 `9171b85bf3f1e99d560c0d4b7820960d1310168036b0437f31bdc4161b41b5bc`, Dual `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`, RD `8df2e8b6e11bea0dd771bc5005848081191e7c468f5ee588976a6633129bb17e`. Identidade e ordem de callbacks estão nos logs. Drop Missing Cards1.0 oficial ficou ON; não se fez teste de probabilidades/drops. Nenhum default estratégico foi alterado.

Limites: proposta vanilla, catálogo, renderer e serviços externos são modelos; o planner RD, planner AI e unidades selecionadas de snapshot/controller/materializadores são reais. A primeira ação seta a magia680 e termina a seleção física; resolução gráfica/efeitos foi omitida. A próxima mão/placement são pontes explícitas. Apply da janela é real, mas o callback de relançamento da plataforma é observado por stub. Não existe validação de gameplay Windows, duelo completo, bytecode retail, outros MODs ou savestates antigos.

Arquivos publicados: testes próprios em `tests/hal23`, relatório/matrizes/logs/proveniência em `notes/hal-23` e continuidade em AGENTS.md. **Zero arquivos de produção alterados**, nenhum fonte/binário/manifest Dual, nenhum asset de MOD, ROM, segredo ou save pessoal. [Índice de publicação](hal-23/PUBLICATION_FILES.json) e [refs preservadas](hal-23/REFS_PRESERVADAS.json). Master, integração, branches e PRs anteriores preservados. Propostas acima continuam sem implementação.
