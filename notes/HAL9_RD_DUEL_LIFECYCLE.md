# HAL-9 / HAL-20 — Lifecycle próprio de Ritual Duelists

[PR #37 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/37). HAL-9/HAL-20 atualizadas no Linear sem encerrar tarefas; [recibos](hal-9-rd-lifecycle/LINEAR_UPDATES.json). [Publicação](hal-9-rd-lifecycle/PUBLICATION.json) e [41 heads anteriores preservados](hal-9-rd-lifecycle/REFS_AUDIT.json).

Branch experimental `test/hal-9-rd-duel-lifecycle`, derivada do PR36/`c8a5605852707589d3d166ad153a88fdd0017b30`. Destino: `integration/hal-20-v0.3.1-preview.2`. A entrega não promove prioridade funcional, contrato, guard, política de savestate ou release.

## Resultado e alcance

A candidata estabelece uma fronteira **pertencente ao RD** em `Main_RunDuel → Duel_InitScene`. Ela invalida as referências do duelo anterior antes do primeiro planejamento e permite novas transações sem reiniciar o processo. BLOCKED, HANDOFF, QUARANTINE e pending pré-consumo não atravessam essa entrada como autorização. No mesmo duelo, continuam protegidos: InitScene isolado, cena, Settings, shuffle e fim de duelo não concedem recuperação.

Isso é recuperação no motor parcial, com objetos/callbacks reais e o corpo oficial de Main_RunDuel. Não é validação de gameplay Windows nem autorização de integrar TW. O corpo de InitScene dependente de IO/renderer, os serviços do frontend, a montagem do tabuleiro e o despacho posterior da fase de entrada permanecem modelados e estão identificados nos logs.

Há três resultados distintos:

- **TW LOCAL_CLEAN:** o adapter inalterado do PR36 reconhece o fechamento da própria conexão e invalida seu contexto local. Não limpa RD nem concede materialização.
- **RD LOCAL_CLEAN:** o owner avança geração/conexão e invalida somente sua transação, monitor e pending do duelo anterior. Mantém alocadores e tabelas cooperativas residentes; não fabrica ACK.
- **Composição recuperada:** requer os dois owners consistentes, uma nova oferta, novos tokens/contexto, authorize/confirm reais, última validação física, gate Dual e uma coleta nova. Ausência de ACK, tabela trocada, reentrância e exaustão permanecem recusadas.

No caso300, o owner é recuperado e a política padrão OBSERVE cria um **novo** BLOCKED: não é contaminação da transação antiga, nem jogada concluída. O caso321 muda somente a política explícita de laboratório para ALLOW: ainda bloqueia no duelo atual, mas conclui uma transação nova depois da fronteira legítima. Nenhuma escolha funcional é promovida.

## Fronteira real

`src/game/main_run_duel.c` inicializa o frontend quando falta0x40. No case1, estado exatamente1, passa a0x81 e chama InitScene. Nos ticks seguintes executa o controller; o flag de fim leva ao case2 e à saída. O fim não invalida automaticamente estados de MODs.

O protótipo envolve Main e abre uma permissão de uso único **apenas durante a chamada delegada** que tomou essa entrada. Init exige profundidade1, permissão e estado0x81. Flags parecidas fora desse escopo não bastam. Se uma callback tentar entrar durante planner/avaliação/negociação ou durante a própria inicialização, a interrupção é registrada e a composição não recebe autorização.

O jogo inicializa decks em etapas posteriores. `Duel_PopulateCombinedDeckData` é chamado em `duel_phase_entry.c`, case4. Seu wrapper real de Ritual Effects invalida suas referências e limpa seu próprio estado por duelo. A fixture despacha esses callbacks reais antes de instalar a nova Queen; sem essa fase modelada, o teste reutilizava indevidamente o flag de Queen já cumprida do duelo anterior. Não se modificou Queen nem se escreveu em seus campos internos.

`rd_deal` continua construindo/embaralhando seu deck somente nas condições originais e invalidando seus caches. Não é tomado como prova suficiente da transição completa do consumer de seleção. O teste negativo de shuffle usa o caminho do deck do jogador; não demonstra segurança de toda chamada arbitrária de shuffle da CPU. A autoridade nova não depende de nenhum deles.

`MemoriesMod.reset` é pós-savestate; `src/pc/guest/state.c` chama Mods_Reset após restaurar memória. O protótipo não chama reset/recover como cancelamento. A segunda entrada de todos os novos casos tem delta Mods_Reset=0. Política B continua recusando os loads não restauráveis antes da escrita; persistência A permanece desligada.

```mermaid
sequenceDiagram
    participant Main as Main_RunDuel real
    participant TW as TW InitScene inalterado
    participant RD as Owner RD experimental
    participant Engine as InitScene IO modelado
    Main->>RD: abrir escopo de entrada case1
    Main->>TW: InitScene da cadeia real
    TW->>RD: disconnect do cliente exato
    RD-->>TW: ACK, quando válido
    TW->>RD: delegar InitScene
    RD->>RD: consumir permissão; avançar geração/conexão
    RD->>RD: invalidar somente referências do duelo anterior
    RD->>Engine: delegar uma vez
    TW->>TW: fechar contexto local, se ACK e lifecycle válidos
    Note over Main,Engine: Populate/novo tabuleiro têm IO modelado e callbacks reais
    Main->>RD: novo planejamento/offer/authorize/confirm
    RD-->>TW: ticket novo ou recusa explícita
```

A cadeia observada contém Main de R → RD → motor e InitScene de TW → RD → motor; os logs/matrizes preservam os ranks solicitados e a ordem efetiva. Isso não qualifica uma cadeia arbitrária, nem um wrapper que suprima/repita a delegação. Código nativo arbitrário capaz de falsificar globals e chamadas não é isolado por esse protocolo.

## Delta experimental próprio

[Fonte do owner](../experiments/hal9-rd-lifecycle/rd_duel_lifecycle.inc), [patch incremental de registro](../experiments/hal9-rd-lifecycle/owner.patch) e [builder/reprodução](../tests/hal9-rd-lifecycle/README.md). Somente a cópia privada do pacote recebe `mod.c`, o include novo e o objeto recompilado. Os507 arquivos canônicos continuam byte-idênticos ao PR36.

O wrapper Main/Init é novo. Os callbacks registrados de AiScript_Run, func_8001BAF0 e UpdateHandActions passam por uma proteção de transição e depois chamam os mesmos corpos RD/companion anteriores. Não há alteração de planejamento, scoring, decks, receitas, categorias, dificuldade, efeitos ou quotas. ABI1 e companion do PR34 não mudam; TW03cc, seu núcleo legado e Dual77cd permanecem intactos.

|Estado próprio RD|Tratamento no novo duelo comprovado|
|---|---|
|generation, connection, interrupt|Avançam com saturação; nenhum wrap|
|serial, sequence e marcadores de contagem|Não voltam a zero; identidade anterior não autoriza outra ação|
|tx/request/answer/producer emprestado|Invalidados antes de delegar InitScene|
|monitor, safe_valid, safe, fontes físicas|Invalidados; nenhuma carta física restaurada|
|attempts/outcome/causa operacional|Reiniciados no novo contexto, não durante o duelo corrente|
|pending deck_pull e caches legal/plan|Invalidados pelo owner; sem consumo, troca ou restauração de cartas|
|rules/cópia de callbacks do produtor|Mantidas residentes e sujeitas ao protocolo existente; não descartadas sem desconexão|
|HF não consumido|ABORTED/STALE; snapshots e pending emprestado invalidados|
|HF consumido e ticket histórico|Histórico conservado; status não é autorização|
|nonce/client epoch/alocadores|Conservados; exaustão não se recupera por novo duelo|

Cada owner invalida somente seu contexto. O RD não chama callbacks internos de limpeza de TW. O TW não escreve em BSS/pending RD. Reentrância de Init deixa RD em QUARANTINE, em vez de anunciar recuperação.

## Testes e controles

Resultado final: **608 processos completos**,252 PASS,296 REFUSED,52 NOT_REACHED e8 FAIL históricos. **6.162 verificações PASS /8 FAIL históricos**, sem novos FAIL funcionais na entrega. A fronteira nova soma **168 processos:100 PASS,64 REFUSED e4 NOT_REACHED,1.712 checks PASS**. O grupo de ausência e as regressões restantes mantêm os resultados anteriores.

|Grupo|Processos|Classificação|Checks PASS / FAIL|
|---|---:|---|---:|
|owner|120|40 REFUSED, 80 PASS|1232 / 0|
|extra|32|8 PASS, 24 REFUSED|244 / 0|
|bonus|8|4 NOT_REACHED, 4 PASS|112 / 0|
|blocked|8|8 PASS|124 / 0|
|regressions|432|224 REFUSED, 152 PASS, 8 FAIL, 48 NOT_REACHED|4378 / 8|
|absence|8|8 REFUSED|72 / 0|

[Matrizes e resultados](hal-9-rd-lifecycle/RESULTADOS.json), [novos casos](hal-9-rd-lifecycle/owner/MATRIZ.csv), [exaustão/reconexão](hal-9-rd-lifecycle/extra/MATRIZ.csv), [bônus](hal-9-rd-lifecycle/bonus/MATRIZ.csv), [BLOCKED e política](hal-9-rd-lifecycle/blocked/MATRIZ.csv), [regressões](hal-9-rd-lifecycle/regressions/MATRIZ.csv), [riscos e runtime](hal-9-rd-lifecycle/RISCOS.json). Os casos314/319 iniciais ficam em controls: a matriz final usa suas repetições focais corrigidas, sem contá-los duas vezes.


As matrizes incluem BLOCKED real por snapshots divergentes, HANDOFF confirmado antes da coleta, QUARANTINE por cursor e referência física, pending ritual criado pelo planner sem injeção, ação consumida, ticket antigo, callbacks antigos durante negociação nova, cliente desconectado/reconectado, Queen nos dois duelos, RD-only/TW-only, HARD/Dual OFF/ON, reentrância e todos os alocadores relevantes saturados. O término usa flag explícito de fixture e os cases1/2 reais, não prova toda a UI de saída.

Support→Monster com RD ativo, duas cópias físicas TW40/41 e ledger Dual2→3 permanece válido depois do novo duelo. Sem Dual não existe bônus: a ação de novo duelo pode passar, mas esse cenário de bônus é NOT_REACHED. A regressão histórica Monster→Support também permanece, no escopo RD OFF que ela efetivamente suporta; não é prova de um handoff Monster com RD ON. HOLD, combos e rituais parcialmente consumidos continuam fora do companion.

O [controle antigo](hal-9-rd-lifecycle/CONTROLS.json) conserva40 processos FAIL/88 checks FAIL com o RD d044 e a mesma fixture corrigida: não há avanço de geração/conexão pelo owner e o monitor antigo pode contaminar o novo tabuleiro. É evidência do defeito anterior, separada da candidata.

Controles de desenvolvimento preservados: símbolos Main resolvidos para memória espelhada no primeiro harness; tentativa de ler TW frio OFF; recursão de um proxy de teste durante reentrada; oracle que classificava como não consumida uma ação já consumida; fase Populate ausente no caso Queen; fault de ACK mantido instalado na reconexão. Este último continua recusando corretamente e não foi corrigido em produção: o teste314 remove apenas seu fault após ACK real e exige nova conexão/negociação. Não se apagaram FAIL nem se modificou a candidata para contorná-los.

Os oito FAIL do reset34 são exclusivamente históricos: fase da roleta em voo não se torna terminal0 pela chamada pós-load, apesar da recusa de callback obsoleto e ausência de nova coleta. Nenhuma BSS/flag do core foi zerada, e nenhum desses FAIL foi convertido em PASS.

## Build, ABI, evidências e limitações

Dois builds integrais de16 fontes com SDK24/i386 geram o mesmo objeto privado RD: `d8123010bc0537eff83dc43edf16fc90e1168b97b72b474b3794fa21248e28df`. Imports/exports não mudam e todos os imports pertencem ao SDK. ABI1 e companion conservam os tamanhos anteriores em i386. O formato de objeto do loader Windows é ELF32 relocatable; os testes disponíveis executam ELF32/QEMU, não um backend PE32. TW03cc continua exigindo a bridge experimental API24/i386 por hash dos PRs anteriores: seu núcleo legado declara API4 e não foi reconstruído integralmente. Esta entrega não demonstra instalação stock universal.

[Build candidato](hal-9-rd-lifecycle/BUILD_CANDIDATE.json), [repetição](hal-9-rd-lifecycle/BUILD_REPEAT.json), [ABI/imports/exports](hal-9-rd-lifecycle/ABI.json), [layouts](hal-9-rd-lifecycle/ABI_LAYOUT.json), [auditoria507](hal-9-rd-lifecycle/AUDIT.json), [invariantes](hal-9-rd-lifecycle/INVARIANTS.json), [callbacks/arestas/hashes](hal-9-rd-lifecycle/WRAPPERS.json) e [hashes dos logs originais](hal-9-rd-lifecycle/RAW_LOGS.json). Os logs publicados omitem somente linhas exatamente repetidas após três ocorrências, com contagens/hashes originais conservados. Não publicam bytes de objetos privados.

Windows automatizado: **AUTOMATION_UNAVAILABLE** pelos recursos/ambiente já documentados no PR36; não apareceu runner PE32 funcional ou imagem legítima/WA_MRG.MRG acessível. Não se repetiram Wine/QEMU PE/Actions inalterados nem se pediu gameplay pessoal intermediário. IO, renderer, frontend completo e apresentação terminal real permanecem NOT_REACHED. Não há proibição do usuário a automação Windows; existe ausência dos recursos necessários nesta sessão.

## Prontidão e próximo passo

O limite de recuperação do owner RD pode ser estabilizado localmente sem alterar core global, ABI1, Dual ou estratégia. A candidata resolve a contaminação entre duelos nos caminhos qualificados, preserva a recusa no duelo atual e exige uma ação/token/ACK novos. Tabela trocada, ACK ausente, exaustão e reentrância não são liberados por esse novo hook.

Para integração segura ainda faltam a revisão desta candidata e da ordem exata dos wrappers/materializadores, lifecycle terminal da roleta, a decisão funcional KEEP_RD versus ALLOW_TW e a validação completa em runtime Windows com recursos legítimos. Para reconstruir/corrigir o núcleo TW integralmente faltam os fontes exatos0.23.1 e receita de link; o adapter não os substitui.

O próximo passo mínimo é revisar/adotar uma candidata do lifecycle RD no conjunto experimental e qualificar a fronteira terminal da roleta separadamente, sem refazer detectores/bônus já aprovados. Não promover ABI, ampliar o companion ou admitir TW no guard por causa deste resultado. OBSERVE padrão, política B, guard,507 canônicos e branches anteriores permanecem preservados; sem merge ou release.
