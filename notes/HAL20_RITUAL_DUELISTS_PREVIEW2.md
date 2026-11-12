# HAL-20 — Ritual Duelists v0.4.0-alpha.8 / SDK API 24

## Resultado e escopo

**Recompilação i386 concluída, 35 grupos de testes isolados PASS, zero FAIL e três grupos SKIP. Nenhuma adaptação dos fontes de produção foi necessária.** Entrega candidata para revisão e testes integrados posteriores; não certifica gameplay Windows nem convivência completa com os demais MODs.

Base: `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`, derivada do commit oficial `aceac3782902746f35ddb0148809450933327cb3`. Branch independente: `mods/hal-20-ritual-duelists-preview2`. Somente `mods/ritual-duelists`, este relatório/evidências e o ponto de continuidade AGENTS.md foram acrescentados/atualizados. Sem merge, alteração de master/integração, core, workflows ou outros MODs. **F1 de Fusion/HAL-21 não foi aplicada.**

Leituras de continuidade: AGENTS.md, [inventário/PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md), [base/#8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8), [Melhoria/Surrender/#9](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/9), [Dual/#10](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/10), [Ritual Effects/#11](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/11), [Fusion/#12](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/12). Relatórios/fontes locais dessas entregas foram usados para a análise estática. Prompts/spec históricos do pacote são evidência, não autorização para reaplicar funcionalidades antigas.

Evidências: [148 insumos e SHA-256](hal-20/ritual-duelists/INSUMOS_SHA256.json), [objetos antigos/novo, símbolos e compiladores](hal-20/ritual-duelists/OBJETOS.json), [comandos/resultados completos](hal-20/ritual-duelists/RESULTADOS.json), [log do build](hal-20/ritual-duelists/BUILD.log), [falha inicial da fixture recebida](hal-20/ritual-duelists/TESTE_ORIGINAL.log), [proveniência nova](../mods/ritual-duelists/SDK-PROVENANCE.json), [runner](../mods/ritual-duelists/tests/run_preview2.py).

## Inventário e preservação

Insumo: pasta `Ritual Duelists` do ZIP SHA-256 `745f6fbbd5f3c9451ca37735626abbbc4c6704bc5326ab3691be33d86b66281b`. São 148 arquivos: 16 C e 22 H de produção na raiz, manifest, 57 assets (38 PNG + 19 BIN), dois objetos antigos, testes e documentação/spec/CSVs históricos. Não havia SDK-PROVENANCE.json no pacote.

Manifest preservado integralmente: ID/library `ritual-duelists`, versão `0.4.0-alpha.8`, autor Otavio, slus_01411, enabled true, restart true, min_api 12. Seis configurações intactas: expansion_enabled, guaranteed_package_free, recipe_source, ai_planner, diagnostics, debug_overlay. Há 35 entradas de drops incluindo `all`, com 215 pares declarados de carta/peso (incluindo remoções por peso zero), sem modificação. Não há requires; after contém somente drop-missing-cards.

**144 dos 148 arquivos recebidos permanecem idênticos**, três foram substituídos (objeto i386 e duas fixtures), e um foi omitido da instalação (objeto x64 antigo). Todos os 38 C/H da raiz, manifest, decks, receitas, inteligência e assets permanecem byte a byte iguais. Acrescentados somente testes, orientação/proveniência e documentação. Os objetos recebidos continuam preservados localmente e identificados por hash; não entram no link nem na instalação candidata.

Há uma inconsistência documental recebida: a label de expansion_enabled ainda diz “27 ritual duelists”; profiles.c contém 29 entradas, sendo Isis um índice histórico inativo e 28 perfis restantes, além do Simon temático dinâmico. Nenhuma label/perfil foi corrigida por preferência. Os testes históricos de planner usam um perfil sintético baseado no antigo Isis; **isso não reativa Isis no runtime**, que delega ao Vanilla. A fixture do objeto confirma esse fallback.

As artes das caixas foram identificadas pelo usuário como doação de um amigo para o MOD, em resposta à consulta sobre origem/autorização de publicação. [Registro da declaração e créditos](../mods/ritual-duelists/NOTICES-HAL20.md). Não se inventou licença aberta ou autoria de PeterIron. O Droplist externo não foi copiado, nem quaisquer arquivos novos extraídos do jogo, ROM ou saves pessoais.

## Build, API e proveniência

SDK oficial gerado de preview.2/API 24 e Clang 23.1.2/llvm-mingw 20260922, builder oficial `--target i386`. Compilação integral dos 16 C, sem objetos antigos ou fontes de parceiros. O comando reproduzível está na proveniência/README-PREVIEW2. Headers/exports, builder, compilador, insumos e artefato são identificados por SHA-256.

| Objeto | Bytes | SHA-256 |
|---|---:|---|
| i386 recebido | 4.510.568 | `9b1bdb0474b27bf01995feaa96b8f30be42038de6367031bfd2437ffe4c59219` |
| x64 recebido | 4.708.008 | `d3d0e8adceb6d86a4edf7ff216bab7095343afdf1c6bfc54131f5ae0ad8e8538` |
| i386 recompilado | 4.521.424 | `8df2e8b6e11bea0dd771bc5005848081191e7c468f5ee588976a6633129bb17e` |

Novo formato: ELF32 relocável EM_386, carregável pelo jogo Windows i386; não é DLL. Hash do loader: `57b53592`. O init declara API **12** explicitamente, preservada, aceita pelo host API 24; não há motivo técnico comprovado para alterar min_api ou o init apenas para recompilar. Nenhuma certificação de hosts antigos é inferida.

Os objetos antigos identificam Clang 17.0.0 no DWARF; o novo identifica Clang 23.1.2. i386 antigo/novo têm os mesmos 87 imports e 44 símbolos globais definidos. Todos os 87 imports novos constam nos exports oficiais Windows e Linux i386; a consulta dinâmica NameEntry_BuildStarterDeck também existe. .text passa de 65.166 a 65.870 bytes; .bss permanece 501.751, .rodata passa de 4.195.944 a 4.195.976. Isso não demonstra equivalência semântica ou melhoria de desempenho. Sem proveniência antiga que vincule fonte/revisão/SDK aos objetos, **não se assume que eles correspondem exatamente aos fontes recebidos**.

## Testes executados e alcance

Runner `tests/run_preview2.py`: 35 grupos PASS, zero FAIL, três SKIP; passos de compilação são registrados separadamente. Fixtures C nativas com ASan/UBSan, leak detection desligada. Loader i386 real executado por QEMU i386/sysroot Linux com engine/host/providers simulados, sem sanitizadores. Não houve Wine, disco ou execução Windows.

| Cobertura | Evidência / limite |
|---|---|
| Decks/pacotes/perfis | Distribuições determinísticas Story/Free, garantias ON/OFF, 40 cartas, máximo três cópias, materiais dinâmicos/atômicos, cortes/pesos/hashes aprovados; regras intactas. A auditoria mantém 29 pools históricos, Isis sem pacote. |
| Planner e IA | Receitas variáveis 0–10 na fixture, adapters oficiais 1–5 tributos × Field/Hand/Both (15 combinações), resultados elevados, prioridades, emergências, proteção de materiais, ritual legal/ilegal, avaliação de efeitos e exclusão dos materiais selecionados. Fixtures incluem perfil histórico sintético; não são duelos reais. |
| Seleção e instâncias | Helpers reais de busca/troca e seleção física, proteção de mão, cartas já sacadas, origens distintas e exigência de instâncias. No ELF novo, materialização em ambos os lados troca registros reais de deck mantendo índices, rejeita seleção obsoleta e não repete a troca. A ação pendente foi semeada na fixture, sem alegar que o planner completo a produziu. |
| Commit ritual | Hook real do ELF chama a cadeia uma vez na transição 0x83 → 4 do motor simulado, marca a instância/origem correta, distingue outra origem e carta removida. O veto conservador de fusão por ID compartilhado permanece como recebido. Isis continua Vanilla. |
| Contratos | recipe_v1 de Ritual Effects, rules_v1 de Melhoria, effect_intelligence_v1: ausente, presente ativo, carregado OFF e ABI incompatível, fallback e Force Vanilla. Headers compartilhados são idênticos aos dos PRs #9/#11. Validam consumidores com providers simulados; não carregam os parceiros. |
| Lifecycle/loader | 1.500 assertions; dez cargas independentes, oito hooks e seis subscriptions, cinco ciclos OFF/ON/reset por carga; duas recusas esperadas (register_state negado e símbolo NameEntry ausente). Sem crash nos cenários exercitados. Não executa o gerenciador completo/relink real ou fechamento durante animação. |
| Save/load simulado | Registro real de 288 bytes, versão 4, buffer sem ponteiros. Round-trip/restauração + reset preserva deck de Simon, marcas e dados registrados; eventos SAVE/LOAD do modal. Fixture de drops testa tokens uint32, checksum/persistência temporária, rollback e separação por save. Não valida arquivos do jogo nem migra savestates antigos. |
| Starter/Simon | Três receitas Vanilla, override dinâmico, Force Vanilla, 40 cartas/cap três e Simon entre os outros dois temas; teste recebido fora do runner antigo foi incluído. Não exercita a UI de confirmação no motor completo. |
| Drops | SPOILS Story/extras, Free Duel, fontes independentes Blue-Eyes, consumo somente após entrega confirmada, Seto 3rd ilimitado, tokens e rollback em fixture real de rd_drop_rewards.c. Não comprova distribuição final combinada com outros manifests/disco. |
| Assets/textos | 38 PNGs com CRC/decode/dimensões, 19 buffers BIN com tamanhos esperados, três arrays nativos e 16 camadas/333.632 spans dentro dos limites. UTF-8 íntegro nos fontes/headers/manifest/README; identidade dos 57 assets e headers gráficos. Sem render HD/CRT, áudio ou inspeção visual integral no jogo. |

Três grupos SKIP: gameplay Windows/render/áudio/controles/save real; pacote/runtime externo Droplist 1.4.0 não fornecido; integração do ecossistema/savestates antigos/outros alvos fora desta etapa. Não interpretar “múltiplas instâncias” como instalação simultânea de duas cópias do mesmo ID.

### Falha inicial e alterações exclusivas dos testes

O runner original abortou em tests/test_drop_rewards.c:94 (`bits==1`). A fixture gravava em `/mnt/data/rd_p03_work`, inexistente; a falha subsequente de leitura limpava bits. Trata-se de caminho fixo da fixture, não falha comprovada da persistência de produção/API 24. Foi alterada apenas a fixture para mkdtemp, assert de criação e limpeza dos dados sintéticos. O mesmo cenário passou sob ASan/UBSan; rd_drop_rewards.c ficou intacto.

test_external_droplist_compat.py exigia o objeto x64 recebido e anunciava a verificação do binário externo mesmo sem ZIP. Agora verifica somente o i386 recompilado e registra explicitamente SKIP da inspeção externa ausente. Não foi copiado um objeto velho para satisfazer o teste. O script audit_pools.py regenerou o CSV histórico com conteúdo idêntico. O runner recebido foi mantido como histórico.

Na construção da fixture nova, uma primeira versão forneceu effect_intelligence sem callback estimate; o consumidor corretamente recusou o contrato e um assert da fixture falhou. O provider simulado foi completado, sem patch de produção. GCC emite warnings dos asserts de offsets de headers oficiais nas fixtures; não são erro do build Clang do MOD. As falhas iniciais não foram escondidas como correções de mecânica. A checagem textual git diff --check também aponta CRLF dos CSVs/headers recebidos e dois espaços finais reproduzidos literalmente no log original; não foram normalizados para preservar os insumos e a evidência. Aceitando CRLF, restam somente esses dois espaços do log, sem erro de compilação ou teste.

## Drop Missing Cards e Droplist

`after: ["drop-missing-cards"]` ordena somente MODs presentes/ativos, conforme manager.c; não constitui requires, não instala nem liga Drop Missing Cards. O MOD oficial 1.0 está **enabled false** por default na integração. O balanceamento recebido exige sua ativação; essa ativação deve ser feita na fase integrada, sem alteração do manifest de terceiros aqui.

Tables_PoolFor aplica edições na ordem, substitui pesos explicitados e redistribui o restante para o total da tabela. RD declara remoções globais e adições específicas; ordem e combinação com Drop Missing Cards são relevantes. Não se inventou uma validação de probabilidades finais sem catálogo/disco e cadeia real. Sua ausência não impede o build/carregamento isolado, mas não representa o balanceamento aprovado.

Droplist 1.4.0 de PauloH-Gudang continua externo. Nenhum rd_droplist.c/.h, rd_dl11_sprites.h, assets/ui, drop_list_enabled ou callback de frame interno foi reintroduzido. O diagnóstico overlay existente permanece. O teste confirma a fronteira no fonte/manifest/novo objeto; pacote do Droplist ausente impede validar Triangle e leitura real das tabelas. Conforme documentação recebida, uma chance-base exibida pode não refletir o esgotamento por save de Blue-Eyes, embora o award seja bloqueado pelo código de RD; isso requer teste integrado, sem alterar drops/UI por iniciativa desta migração.

## Análise estática de convivência — nenhuma integração declarada

Oito hooks de RD: DuelEffect_ApplyRitual, Duel_CheckFusion, Duel_ShuffleDeck, Ai_GetHandSize, func_8001BAF0, AiScript_Run, NameEntry_BuildStarterDeck e Duel_SelectCardDrop. Estado/seleção de IA compartilhados exigem verificar a ordem real, mesmo quando os hooks usam previous.

| Parceiro | Risco/contrato e teste integrado necessário |
|---|---|
| AI HARD MODE oficial 0.1.0 | Compartilha AiScript_Run, Ai_GetHandSize e func_8001BAF0. HM pode escolher e retornar sem delegar ao run anterior; RD chama a cadeia antes de refinar. A janela RD pode substituir o valor anterior e HM também possui deck_access/deck_window. Testar ambas as ordens, seleções físicas/virtuais, margens e turnos. Adaptador effect-aware 0.1.1 citado na auditoria permanece ausente; não foi criado. |
| Dual Play 0.5.7 Monster + Support | Compartilha func_8001BAF0 e o snapshot/seleção física da IA. Dual rejeita ritual no bônus e valida quotas Monster/Support; RD pode buscar Magic/tributo. **No caminho próprio MAGIC/FIELD bem-sucedido, rd_materialize não chama previous_swap**, pois materializa por si; se Dual estiver abaixo, sua validação nesse hook não será chamada nesse caminho. Se Dual estiver acima, pode rejeitar a ação antes. Evidência estática de risco de ordem, sem reprodução de partida ou afirmação de exploit. Exigir quotas, bônus, turnos, dois controles e OFF/ON integrados antes de aprovar. Não mudar Dual/RD automaticamente. |
| Melhoria/Surrender #9 | Consome rules_v1; Melhoria executa/consome materiais e RD planeja, consultando o verificador nativo encadeado. Não há hook-alvo direto em comum nesse par; há estado ritual/mão/deck compartilhado. Testar fontes Field/Hand/Deck, mínimos, require_result, consumo físico e Surrender/Select em Dual. Não há consumidor de receitas sob ID Melhoria em RD; recipe_v1 consultado é do Ritual Effects. |
| Ritual Effects #11 | Compartilha DuelEffect_ApplyRitual, Duel_CheckFusion, AiScript_Run e func_8001BAF0; consome recipe_v1/intelligence. Verificar ordem commit/marcas/efeitos, Cosmo e trocas de mão, mudança de origem, matérias realmente consumidas e providers OFF. Receitas são copiadas/cacheadas por perfil até reset/reinicialização (o início de outro duelo, sozinho, não garante reconsulta); alternar provider sem invalidar o cache não foi certificado. Não limpar decks ou alterar decisões para contornar isso. |
| Fusion Effects #12 | Compartilha Duel_CheckFusion; RD protege a proveniência ritual, Fusion arbitra suas fusões. Também há mutação de instância/origem por transformações. Testar veto/instância normal do mesmo ID, controle, remoção/recriação e restauração, sem mudar resultados. RD não consulta diretamente encore/origin/effect_forecast_v2. **F1 OFF dos providers de Fusion pertence à HAL-21 e permanece fora desta entrega.** |
| Time Wizard | Adapters compartilham AiScript_Run e func_8001BAF0/seleção. Ponte Dual depende de fingerprint do objeto/cadeia; RD não a modifica. Mais wrappers podem alterar a ponte efetiva e a validação das quotas. Núcleo-fonte de Time Wizard continua ausente; análise limitada aos adapters e relatórios anteriores. Não aplicar patch por hipótese. |

O core oficial retira hooks de MODs OFF via Hooks_Relink; callbacks isolados OFF/ON não equivalem a executar esse gerenciamento completo. Manifest restart true foi preservado. Nenhuma nova decisão estratégica, mudança de deck/drop, provider público ou regra funcional foi introduzida.

## Sequência segura e pendências

1. Revisar este candidato e os hashes/proveniência, mantendo todos os PRs independentes e sem merge automático.
2. Testar Windows i386 com imagem legítima do usuário: novo jogo/seletor/Simon, perfis (Isis Vanilla), duelos, animações, áudio, reset e saves novos. Savestates antigos exigem avaliação separada.
3. Ativar Drop Missing Cards 1.0 no perfil de teste, carregar RD depois e validar tabelas/Story/Free/Blue-Eyes. Instalar Droplist 1.4.0 externo autorizado e validar Triangle/exibição, sem visualizador interno.
4. Integrar para teste, em branch própria autorizada, Melhoria #9 e Ritual Effects #11: providers ativos/ausentes/OFF, seleção física, consumo de instâncias e restauração. Nenhum desses PRs foi modificado.
5. Exercitar matrizes de ordem com AI HARD MODE, Dual Monster + Support e Time Wizard; registrar reproduções dos riscos antes de propor qualquer ajuste funcional. F1 de Fusion aguarda sua tarefa HAL-21, sem cherry-pick nesta migração.
6. Somente após esses resultados avaliar convivência completa e eventual promoção. **SDK API 24 não exigiu adaptação de produção; compatibilidade integrada permanece pendente.**
