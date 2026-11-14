# HAL-20 — Field Effects e recuperação de estado na coleção candidata

Entrega experimental de 14/11/2026. Branch `test/hal-20-field-state-integration`, derivada da consolidação PR #28 (`414a1b7cd149582d674792dd66b375ca46a7fcd2`), cuja base é a integração oficial `d649647c159c91d615c07d6960928a37e270b461`. Sem merge/release. Preservar os PRs anteriores; esta composição ainda requer revisão.

## Resultado e respostas

1. **Field funciona nas condições automatizadas exercitadas.** A coleção privada contém oito IDs únicos: Melhorias de Rituais (M), Ritual Effects (R), Ritual Duelists (RD), Fusion Effects (F), Field Effects (E), AI HARD MODE (H), Dual Play (D) e Drop Missing Cards (DMC). Foram exercitados bônus primários/secundários, substituição de campo, identidades, efeitos permanentes, controller Queen/Dual e criação própria do Fusion. A composição visual real e o duelo Windows completo não foram alcançados.
2. **A IA passou a consultar o bônus individual real do Field.** O algoritmo e os defaults foram preservados; um adaptador técnico opcional fornece o delta por carta. Sem o provider, 2.000 decisões comparadas com o planner original foram idênticas. Com o provider real, um caso sintético próximo ao limiar mudou a escolha conforme o bônus aprovado. Isso valida a entrada de terreno e o cálculo nesse fluxo, não toda decisão de gameplay.
3. **A recusa B antes da sobrescrita é a opção experimental segura disponível para esta coleção. Não é persistência resolvida nem política permanente aprovada.** A variante A restaura Queen e seu bônus no roundtrip modelado, mas perde a lease do Fusion e não cobre todos os estados temporários. Load integral/cross-processo não foi demonstrado. Não admitir A na coleção nem prometer savestates antigos.
4. **O guard admite os artefatos novos nas ordens seguras testadas, mantendo recusas.** Exige SHA completo, callbacks, endereços, contagens e predecessores em sete alvos. Objetos antigos, callback duplicado e predecessor adulterado são recusados. Não há autorização por nome nem mudança global em Hooks_Relink/Mods_Order.
5. **Ainda faltam Time Wizard, Droplist externo e outros componentes não qualificados**, além do load integral e da execução Windows. Dual continua exclusivamente local/privado. DLC e Sanctuary são recursos de um MOD futuro: o usuário confirmou expressamente que ainda não foram implementados; estão fora desta entrega e não são bloqueios atuais.
6. **A automação pode continuar sem intervenção pessoal:** completar a persistência por owner, qualificar callbacks/renderização restantes, obter backend PE32 que execute o controle mínimo e usar recursos legítimos disponíveis em caminho privado. Só a bateria pessoal jogando fica para a última fase; não foi solicitada nesta etapa.

## Fonte e mudanças localizadas

Field 0.19-adaptive-description-test foi localizado no histórico público do próprio projeto, branch `mod-field-effects`, commit `296c5919479711b550d00aea8cd042b1c34ba306`, diretório `mods/Yugioh-Operacao-MODs/Field-Effects-e-Tipos-Secundarios/dev`. Não veio no ZIP atualizado. Conferimos fontes, manifest e especificação Vanilla V2; não presumimos equivalência com outros binários. Não há seletor DLC/Sanctuary nessa fonte, em acordo com a clarificação posterior do usuário. A integração publica a fonte própria autorizada, não ROM ou novos assets de terceiros.

Três incompatibilidades técnicas receberam correções experimentais comprovadas:

- **Field, `field_effects.c`:** o provider consultava `host->applied()`, preferência salva. O manifest com `terrain_bonus` exige reinício pelo core; portanto preferência OFF com owner ainda efetivamente ON removia o delta indevidamente. O inverso também respondia após callback OFF. Agora o provider consulta flag atualizada exclusivamente pelo callback real `mod_applied`. São três edições; tabela de tipos secundários, regras, UI e manifest permanecem idênticos. OFF pendente mantém o benefício efetivo até reiniciar; não afirmamos desfazer modificadores físicos só por chamar OFF à força.
- **AI, `ai_hard_mode.c`, `planner.c`, `planner.h`:** callback opcional `terrain_card` acrescentado ao final da estrutura interna HmRules. Retorna o bônus nativo mais `field-effects-secondary-types:terrain-card-delta-v1`; ausência/OFF usa o fallback original. Apenas as entradas de terreno do planner foram adaptadas. Defaults, deck_access, preserve_hand, visibilidade, seleção/swap e política estratégica não foram alterados. O objeto é **base oficial mais adaptador técnico**, não o objeto oficial puro anteriormente qualificado. HmRules é interno; o ABI público do host não mudou.
- **Fusion, `mod.c`, `fusion_round2_runtime.h`, `fusion_round3_runtime.h`, `generated_card_restore.h`:** cinco atribuições de terreno em caminhos próprios usam o mesmo bônus nativo mais provider opcional. Reprodução com o helper compilado real `r2_summon_zombie`: Zombie Warrior, ID30, em Sogen ficava com delta0 no PR22 e passa a +200 nesta candidata. Esse materializador não passa pelo Setup nativo onde Field já intercepta. Não alteramos receitas/resultados/valores; corrigimos aplicação do bônus já aprovado. `owner_f.h` e `fusionist_runtime.h` permanecem idênticos ao PR22, incluindo F1/Revogar. Headers conservam fallback para uso isolado sem provider.

M/R/RD preservam integralmente os arquivos e hashes do PR28: 21/203/158 arquivos, respectivamente. Contratos HAL25, Queen Lock, settings, assets e mecânicas permanecem iguais. Fusion preserva 43 settings, manifest, receitas, assets e todos os demais arquivos de produção. AI e Field também mantêm manifests originais. Nenhum arquivo privado do Dual foi alterado ou incluído.

No core desta continuação mudaram somente `src/pc/mods/manager.c` e `hal20_qualified.h`: expansão dos papéis/alvos/recibos exatos e inclusão de E no predicado B. `hooks.c`, Mods_Order, host ABI, controller, janela e `guest/state.c` estão byte-idênticos ao PR28. As proteções anteriores são herdadas, não recém-promovidas à integração.

## Artefatos e composição reproduzível

Objetos ELF32 REL/i386 produzidos pelo SDK Windows API24; esse é o formato de MOD do loader, não DLL PE. [Recibos de qualificação](hal-20-field-state/QUALIFIED_ARTIFACTS.json), [insumos](hal-20-field-state/INPUTS.json), [árvores próprias](hal-20-field-state/OWNED_TREES.json) e SDK-PROVENANCE por pacote identificam fontes, compilador, imports/exports e bytes exatos.

| Componente | SHA-256 do objeto | Código do loader |
|---|---|---|
| M | `c36d033fb1dd2e9cbe38aec85409cc4f7bb849e9fdb350d1f1f737d4a8d9efc0` | `1fe34a1e` |
| R | `78ec3ce45694b1f583213530dfc0a527c8974056f67c49e7791fb6821f55ab45` | `1e478c68` |
| RD | `9f0d7354e6ff3f44776308b18e96b960480500c814ae22c3e7d1e6b57eafea34` | `3d26698f` |
| F | `c259f789c5060623744bd28ab09de4044d055d9c02570a3f7f73032eb5190dd6` | `69a6af85` |
| E | `729ff106787d477def41feb42a1a5d168f5bb050af870a25738c977e02c1cd21` | `cfd0dcf3` |
| H | `05cf957bc4575503d257432cd927748367207258189856217bc1bf2adb0f7d19` | `00049e34` |
| D privado | `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378` | `3028c68a` |

DMC1.0 é o oficial da base. Stager cria fora do Git a coleção com ranks DMC−100/M−50/E−40/F−30/R−20/H0/RD10/D20 e dependências `after`; registra a cadeia real. Apenas a cópia privada do manifest AI recebe `restart:true`. Field já exige reinício por conter `terrain_bonus`. Arquivos em `mods/` mantêm manifests originais. Uma versão por ID; a pasta privada contém Dual e **não deve ser publicada**. O build incremental após staging reconhece Field e os pacotes próprios, mas também empacota opcionais oficiais não qualificados. O executável recompilado sozinho não monta esta coleção customizada: usar o stager e `MEMORIES_MODS_DIR`/settings/userdir privados, preservando os objetos canônicos por SHA.

[ABI AI](hal-20-field-state/ABI_AI.json), [ABI Fusion](hal-20-field-state/ABI_FUSION.json) e [ABI Field](hal-20-field-state/ABI_FIELD.json): exports/imports públicos preservados, imports disponíveis no SDK. O controle Field foi recompilado nesta tarefa a partir da fonte histórica, SHA `71915bc5f119961d064d8f86269706727910bfded0684b5f89dc0bbacdaa0100`; não representa um suposto binário recebido. Diferenças de debug path podem mudar o hash entre recompilações; somente o objeto fixado é admitido.

## Guard, callbacks e materialização

Opt-in `experimental.hal2224_order_guard=1`, sete papéis, sete alvos e 19 callbacks qualificados. Preserva a restrição de registro H→RD→D, cujo encadeamento efetivo chama Dual antes dos materializadores. Os alvos adicionais são SetupCardRecord, ApplyTerrain e os dois callbacks de texto. Uma recusa não consome cartas nem altera a seleção nos casos exercitados.

[Inventário de wrappers](hal-20-field-state/wrappers/WRAPPERS.json): 130 nós, zero bindings não resolvidos, 48 perfis de trace. Registra SHA do objeto/corpo/relocações, ordem, predecessores, chamadas e mudanças observadas; callbacks privados aparecem somente como metadados/hashes, sem seus arquivos. `MODEL_OR_UNBOUND` identifica fronteiras modeladas, não uma função nativa alegadamente executada.

[Field boundaries](hal-20-field-state/wrappers/FIELD_BOUNDARIES.json): cada Setup e ApplyTerrain retorna 96 vezes. Nos contadores exclusivos, ambos têm zero mudança de identidade/seleção/deck, zero consumo e zero materialização própria; os deltas de terreno observados são não nulos onde esperado. Setup delega ao factory modelado. Contadores exclusivos descontam filhos instrumentados, não todos os possíveis C calls.

Os dois wrappers de texto foram vinculados por artefato/endereço/contagem, mas não executados na composição gráfica real. Testes próprios da fonte exercitam símbolos, agrupamento, cores, pitch, footer e delegação; isso não certifica a renderização final com R/F. Bibliotecas desconhecidas fora dos alvos críticos não estão automaticamente qualificadas por tolerância estrutural. O staging exclui extras; não apresentar o guard como sandbox universal.

## Savestates: comparação experimental A × B

O PR28 reproduziu perda de Queen mesmo em fronteira quieta: o load repõe memória nativa, chama Mods_Reset e perde estado temporário que R não registrou. Pending vazio não significa snapshot completo.

| Aspecto | A: persistência parcial por owner | B: recusa antes de sobrescrever |
|---|---|---|
| Implementação | Apenas em `experiments/hal20-field-state`; variante R separada | Predicado experimental herdado do PR28, expandido para Field |
| Estado | Cápsula Queen tipada/versionada260 bytes; índices físicos, contadores, estado efetivo | Não restaura estado; recusa load se M/R/RD/F/E efetivamente ativos e guard ON |
| Identidades | Índice do combined deck e data block, não card ID; valida tudo antes de escrever no owner | Não toca memória do duelo numa recusa |
| Planner/consumo/quarentena | Não elegíveis; não serializa pending, ponteiros nem BSS arbitrário | Recusa também quieto, ON/OFF efetivo e preferência OFF pendente |
| Queen | Máscara, referências e bônus recuperados no modelo | Evita o load destrutivo; não promete recuperar Queen |
| Fusion | Lease77 não recupera após reset: cobertura funcional **FAIL** para A completa | Impede load com F ativo |
| Limite | Recusa do owner ocorre depois da restauração nativa modelada; não fornece rollback global | Não cobre H/D-only, guard OFF ou outros owners como política universal |
| Impacto | Precisa completar inventário/serialização de todos os owners antes de permitir load | Usuário não pode carregar savestate neste perfil ativo; saves normais memory card não têm código alterado |

A usa register_state version1 e eventos SAVE/LOAD, somente em phase4 + lifecycle RD IDLE/not busy + owner efetivo ON. Testes validam versão e rejeitam a mesma carta-ID com identidade física diferente. Reset avança o epoch; não ressuscita actions/generations nem cartas consumidas. A variante tem SHA `cfc564ef0fdce50a3bc172d89cc3a67c357f3ebf5780b1d856f594579f86ee72` e **não substitui o R canônico**; o guard a recusa por SHA desconhecido. Publicamos patch/esquema/recibo, não instalamos esse objeto.

O roundtrip copia RAM guest/data/chunks registrados, depois reset e LOAD; é simulado. Não inclui contexto/stack/file loader completo, persistência de todos os efeitos R/F, animações nem save cross-processo. A lease do teste é preparada por instrumentação própria e explicitamente não representa a demonstração de concessão natural nessa rodada. Não reconstituímos por ID.

B atua na função comum de load depois de abrir o arquivo e **antes de fseek/leitura/remapeamento/sobrescrita**. Código comum e mensagem existentes do PR28 não mudaram. Preferência salva OFF enquanto reinício pendente não torna owner efetivo OFF. O predicado considera o estado aplicado real. Fluxo de memory card não passa por essa alteração; gameplay de memory card ainda não alcançado. A política B está documentada como mitigação experimental a revisar, não adotada permanentemente. Não há compatibilidade presumida com savestates antigos.

## Testes e classificação

[Resultados agregados](hal-20-field-state/RESULTADOS.json), [matriz por check](hal-20-field-state/MATRIZ.csv), [capacidades](hal-20-field-state/COMPATIBILIDADE.csv), arquivos `.logs.tar.gz` por suíte e [riscos](hal-20-field-state/RISCOS.json). Contagens são execuções, com cenários parcialmente sobrepostos; não são número de duelos reais únicos.

- **Coleção canônica:** 1.131 processos, 10.122 checks PASS; classificações 1.025 PASS /96 REFUSED /10 NOT_REACHED. Inclui Field guard OFF/ON112+112, bônus112, negativos28, trace48, rituais nativos32, B20 e regressões conjuntas. Hand sem ritual legal fica REFUSED, não convertido em sucesso fictício.
- **A separada, não admitida:**80 processos,304 checks PASS;64 PASS e16 REFUSED pelo guard. Os checks confirmam a limitação de lease; não significam persistência completa PASS. Cobertura integral de A é FAIL.
- **Controles históricos:**12 processos,4 PASS/8 FAIL;106 checks PASS/**14 FAIL esperados**. Preservam provider Field por preferência, terreno ausente na criação Fusion e cursor/pilha PR26. Não são regressões novas da candidata.
- **R/RD originais:** R23 PASS/3 SKIP; RD34 PASS/1 FAIL esperado na auditoria de imutabilidade histórica/3 SKIP. Nenhuma falha removida para melhorar o resultado.
- **Fusion:**16 grupos PASS,214 asserts de caracterização; F1 continua Revogar, contratos OFF neutros. **AI:**500 casos originais +fixture adapter +2.000 comparações exatas sem provider; cenário próximo com provider real. **Field texto/regras:**27 asserts PASS. **Predicado B:**18 perfis PASS.
- **Build completo Windows i386/API24:** PASS; artefatos próprios recompilados, ABI imports/exports PASS. Build não equivale a executar jogo PE32. Os objetos fixados pelo stager prevalecem sobre recompilações com debug paths distintos.

No host parcial foram usados objetos reais de controller/ritual/seleção/loader/mod manager e os providers reais. O packer de stats e a conclusão nativa de campo foram exercitados; início/animação do campo, catálogo de cartas, renderer, partes do factory/VM e pontes de fase são modelados. O controller verifica Queen, Dual categorias/quotas, seleção física, cancelamentos e materialização única no subconjunto alcançado. Não extrapolar para duelo completo.

Os controles finais corrigem erros das primeiras fixtures: setup do oracle primário de terreno, probe de Fusion com subtipo/assinatura assumidos e objeto físico nulo no ensaio Queen. O controle final usa Zombie Warrior real e ABI do helper inspecionada; os testes canônicos e os FAIL históricos permanecem preservados. Não contamos execuções exploratórias incompletas nem versões intermediárias como evidência positiva final.

## Automação PE32 e recursos privados

[Evidência de ambiente](hal-20-field-state/windows/ENVIRONMENT.json), [WOW64](hal-20-field-state/windows/QEMU11_WOW64.json) e logs. Testamos uma nova versão legítima Debian QEMU11.1.2, além dos backends anteriores:

- QEMU11 x86_64 +Wine11 WOW64: controle PE64 PASS; PE32 termina antes do marcador de entrada (ShellExecuteEx/exit1). Executável PR28 também não iniciou. O build atual não foi executado porque o backend mínimo falhou.
- QEMU11 i386 +Wine10 x86: exec do wineserver ELF32 fixo falha errno8/exit127. Uma cópia privada com wrappers explícitos para os executáveis superou esse exec, mas o bootstrap Wine encontrou illegal instruction em0x00403a00 e SIG11; CPU max não resolveu. Não identificamos com segurança o módulo/instrução causadora; não atribuímos a falha aos MODs.
- Encerramos os processos auxiliares próprios. Logs incluem tentativa preliminar de cleanup com caminho wineserver inadequado, corrigida posteriormente. Não alteramos o runtime original nem publicamos Wine, QEMU, PE do jogo, prefixes, core dumps ou saves.

**AUTOMATION_UNAVAILABLE** para o backend PE32; UI/reinício/duelos/save-load reais da coleção **NOT_REACHED**. A varredura dos caminhos acessíveis encontrou prompts e ZIPs de MODs, não uma imagem legítima do jogo. Os418 `.bin` examinados eram pequenos buffers gráficos (máximo76.800 bytes). Recursos fornecidos em outro contexto não estão presumidamente acessíveis aqui. Uma referência legítima da tarefa ou caminho/mount privado acessível permite teste privado; não buscamos ROM de terceiros. Não há VM Windows selecionada. Essas são limitações concretas de ambiente/recursos, não proibição de automação pelo usuário.

## Componentes adicionais e próximos passos

[Inventário atualizado](hal-20-field-state/ADICIONAIS.json). Time Wizard0.24.0-test continua com núcleo compilado0.23.1 e fontes parciais/offsets fixos; precisa fonte/proveniência e qualificação ABI/runtime/assinaturas. Não presumir que novos objetos AI/Fusion preservem o que sua ponte espera. Droplist1.4.0 externo não está disponível; visualizador interno RD continua removido. Adapter AI0.1.1 externo não foi fornecido e não é o adaptador de dados Field implementado aqui. 3d-monsters/hand-camera/yamyi e composição visual completa não estão admitidos por nome. Dual mantém restrição de redistribuição e staging privado.

Próximo passo mínimo: revisar estas três correções e o alcance do guard, completar inventário versionado de estado de R/F antes de qualificar A, e executar o loader real com contexto restaurado em backend funcional. Até lá, conservar B como recusa experimental explícita, sem promover política, contrato ou coleção à integração. Renderização composta e gameplay Windows permanecem pendentes; todas as outras regressões automatizadas possíveis continuam autorizadas.
