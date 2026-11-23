# HAL-9 / HAL-20 — Recuperação cooperativa e hardening do cliente TW

[PR #36 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/36). HAL-9/HAL-20 atualizadas por comentários, sem encerrar tarefas; [recibos](hal-9-recovery/LINEAR_UPDATES.json).

Branch experimental `test/hal-9-tw-recovery-hardening`, derivada do PR35/`e5c97876009e0e57127166039821426a617729db`. Destino do PR: `integration/hal-20-v0.3.1-preview.2`. Esta entrega não promove política, ABI, guard ou release. Os507 arquivos canônicos, Dual privado e seis link units legados permanecem intactos.

## Respostas objetivas

1. **É possível começar outro duelo sem reiniciar o processo?** Sim para as recusas locais anteriores ao commit e para o estado residual após uma ação consumida: o hook próprio em `Duel_InitScene` fecha cooperativamente a conexão e limpa apenas o contexto TW. A segunda ação exige planejamento, tokens e ACK novos. Não é uma solução universal: RD BLOCKED/QUARANTINE e seu monitor pós-handoff continuam recusando; troca de tabela antes do ACK também permanece recusada.
2. **O que invalidar?** A conexão do cliente deve receber ACK de desconexão; o epoch de conexão do companion avança, e uma nova conexão/offer gera client/action/nonce novos. O adapter avança seu epoch privado de duelo e limpa ticket/proposta/offer, flags de autorização e fence local. Não reinicia nonce, `HfClient.epoch`, contadores de RD, receitas, cartas ou pending. Mudança de geração/conexão do produtor continua responsabilidade do owner. Nenhum `HfStatus` concede permissão.
3. **As projeções continuam corretas?** Passam nas fronteiras exercitadas, com restauração somente dos campos emprestados. O swap não sobrescreve o tipo de outro writer; o snapshot não apaga alterações independentes de stats nem reclassifica registros preexistentes quando o predecessor retorna sem produzir snapshot. Reentrância e conflitos detectados fecham a ação. Isso não qualifica um wrapper que suprima o gate: retorno/restauração corretos não provam delegação ou gameplay.
4. **O que impede integração segura?** Falta uma fronteira própria de novo duelo para transação/monitor de RD, tratamento terminal da roleta e qualificação dos materializadores/wrappers que possam contornar a última validação. O guard continua recusando TW. A recuperação local demonstrada não libera o owner bloqueado nem a coleção inteira.
5. **Quais dependências são externas ou funcionais?** KEEP_RD versus ALLOW_TW continua decisão explícita futura. Para reconstruir/corrigir integralmente o núcleo da roleta faltam os fontes exatos0.23.1 e receita de link; os adapters disponíveis não os substituem. Gameplay/UI/transformações completos continuam dependentes de backend Windows PE32 funcional e recursos legítimos como `WA_MRG.MRG`. Não se repetiu Wine/QEMU PE/Actions sem mudança técnica, nem se pediu teste manual intermediário.

## Fronteira real e limites do motor parcial

A base chama `Duel_InitScene` em `Main_RunDuel`, case1, antes de atualizar o controller do novo duelo. O case2 executa a saída. `MemoriesMod.reset`, por outro lado, é explicitamente pós-savestate; `src/pc/guest/state.c` chama `Mods_Reset()` depois de restaurar a memória. Não é cancelamento genérico.

Os testes executam o corpo real de `Main_RunDuel` e a cadeia real de callbacks `Duel_InitScene`, com contabilização de `Mods_Reset` por wrapper que apenas conta/delega. Na segunda inicialização o delta é0. O corpo de InitScene que exige IO, renderer e imagem do jogo é modelado; o novo tabuleiro é outra fixture. Não foi restaurado o tabuleiro antigo nem escrita BSS/pending dos MODs.

O caso214 executa os cases1/2 do controller de saída e depois a entrada do duelo seguinte. O flag de término é entrada explícita da fixture; serviços frontend/IO são modelados. Isso demonstra a fronteira/callback disponível, não a alcançabilidade completa da UI Windows. Fim de duelo sozinho não limpa nem autoriza uma transação antiga.

```mermaid
sequenceDiagram
    participant Main as Main_RunDuel real
    participant TW as InitScene próprio TW
    participant HF as Companion RD real
    participant Engine as InitScene / IO modelado
    Main->>TW: entrada do novo duelo
    TW->>HF: disconnect(cliente exato)
    HF-->>TW: ACK; epoch antigo invalidado
    Note over TW,HF: STALE após desconexão anterior exige connect+disconnect reconhecidos, sem tomar outro cliente
    TW->>Engine: delegar a inicialização existente
    TW->>TW: avançar epoch privado e limpar só contexto TW
    Main-->>TW: novo planejamento real
    TW->>HF: connect, offer, prepare, authorize, confirm
    HF-->>TW: tokens novos e ACK
    Note over TW,HF: monitor/QUARANTINE de RD não é limpo pelo TW
```

## Implementação localizada

[Adapter incremental](../experiments/hal9-recovery/adapter.patch) registra um hook próprio em `Duel_InitScene` e impede avanço pelo hand callback durante a transição. [Cliente experimental](../experiments/hal9-recovery/tw_client.inc) contém a recuperação e as duas projeções endurecidas. O [diff contra o cliente PR35](hal-9-recovery/TW_CLIENT_DELTA.patch) separa o delta do fonte completo. Os arquivos dos experimentos anteriores não foram substituídos.

A desconexão normal exige ACK. Se o produtor já reconheceu uma desconexão independente e retorna STALE, a fronteira tenta `connect` da mesma tabela estática e `disconnect` imediatamente: são operações reais do protocolo, sem offer, cancelamento, consumo ou interpretação de status. Se houver outro cliente, busy ou tabela descoberta diferente, não existe ACK e a recusa permanece. Não persegue um ponteiro antigo atrás de `Mods_Find` nem esquece contexto ainda conectado.

O início reentrante durante authorize/confirm/projeção é recusado antes de delegar InitScene. Um marcador monotônico de interrupção invalida a negociação em voo; o callback não pode voltar e produzir ACK como se nada tivesse acontecido. É fault de timing instrumentado, não fluxo UI normal comprovado. Um pedido posterior de inicialização não reentrante pode completar a fronteira.

A limpeza ocorre somente depois da delegação do InitScene, com ACK, owner TW efetivo e contexto não interrompido. Contadores exauridos não voltam a zero. O estado aplicado vem do callback real `applied`; a preferência OFF com restart pendente não desativa o owner. OFF efetivo durante negociação é exercitado separadamente de preferência salva.

## O limite pertence ao RD, não ao TW

O RD desta composição não registra InitScene para invalidar seu consumer de seleção. `rd_deal`, em `Duel_ShuffleDeck`, limpa alguns estados de geração de deck, sob condições próprias, mas não substitui a invalidação completa de geração/monitor/QUARANTINE.

Após um veto local antes do commit, RD ainda pode estar VALIDATED e permitir novo planejamento. Nesse caso, o hook TW resolve a contaminação local, inclusive mantendo um pending RD observado intacto até que o próprio planner o trate.

Depois de ACK e precommit, o RD pode estar HANDOFF, com monitor da seleção física. No novo tabuleiro esse monitor detecta fontes/identidades diferentes e entra em QUARANTINE. Os casos202/207 exigem essa **recusa segura** com mão intacta. O caso206 OBSERVE mantém o BLOCKED pertencente ao RD. Nenhum deles é rotulado como recuperação concluída.

O próximo ajuste mínimo nessa fronteira seria uma entrada de novo duelo implementada pelo próprio RD: invalidar sua transação/monitor e avançar geração no ponto legítimo, separadamente de reset pós-load e sem alterar o planner. Não foi implementada porque esta tarefa preserva os507 arquivos canônicos e limita a correção de recuperação ao adapter próprio TW. Não se chama `recover()` nem se apresenta a tabela de regras de Ritual Effects como se o TW fosse seu produtor.

## Hardening das projeções

**Swap lógico.** O adapter continua descrevendo Time Magic como Support ao gate real Dual. Ao retornar, só libera o campo tipo emprestado se o record lógico ainda pertence à mesma referência física e o valor ainda é o projetado. O byte lógico `deck_index` da mão é comparado apenas como byte opaco não alterado; não é tratado como identidade física válida. Mão/record/data/object são as referências reais.

Se outro callback escreve seu próprio tipo, inclusive troca lógica com mesmo ID, esse valor não é sobrescrito pelo tipo antigo. O conflito impede a coleta na composição ativa. Se apenas a seleção muda, mas a identidade permanece, o tipo emprestado ainda é restaurado e a ação alterada é recusada. Não é rollback de carta nem restauração por ID.

**Snapshot bônus.** A restauração global modifica somente a máscara de tipo da entrada TW, preservando bits independentes escritos por outro callback. Só retorna o tipo se a máscara ainda contiver o valor emprestado. Na janela lógica, não reclassifica registros preexistentes quando o predecessor retorna sem reconstrução; escrita de tipo incompatível e reentrância geram recusa. Os flags de conflito são latched, não contadores que podem voltar a zero por overflow.

Os probes220–231 intercalam callbacks no slot de predecessor do adapter próprio, sem modificar Dual/core ou a semântica global de hooks. São testes isolados de fronteira. Quando há conflito latched pelo adapter, a classificação é REFUSED; quando a coleta não foi executada pelo probe, é NOT_REACHED. Nenhum PASS de restauração significa jogada concluída.

Um wrapper que retorna sem delegar pode liberar corretamente o tipo temporário e ainda não chamar Dual. `tw_final_gate=1` registra tentativa de delegação, não ACK do gate. Esse wrapper não é qualificado/admitido. O caso231 verifica restauração com OFF efetivo forçado depois do swap, enquanto o código permanece residente, e deixa a continuação física pós-OFF NOT_REACHED. O manifest com reinício mantém o OFF por preferência distinto dessa instrumentação.

Não existe prova genérica contra escrituras nativas indistinguíveis/ABA, callbacks arbitrários ou writers que modifiquem o campo emprestado com o mesmo valor. Exigem cooperação explícita/qualificação adicional; o guard não é afrouxado para admiti-los. O adapter também não desfaz alterações que pertencem a outro writer.

## Diagnósticos e permissões

|Observação|Significado|Não significa|
|---|---|---|
|`tw_recovery_state=LOCAL_CLEAN`|Conexão antiga fechada e contexto local limpo|RD recuperado, gate autorizado ou duelo integrado validado|
|`tw_final_block=1`|Fence/recusa local; consultar causa e owner efetivo|Consumo desfeito ou pending cancelado|
|`tw_final_gate=1`|Delegação do swap foi tentada|Callback Dual executado ou materialização confirmada|
|HfStatus CONFIRMED/MATERIALIZED/CONSUMED|Último estado do companion para seu token|Autorização atual após desconexão/novo duelo|
|RHS BLOCKED/QUARANTINE|Recusa pertencente ao consumer RD|Permissão para TW limpar ou recuperar o owner|

Os logs acrescentam estado/causa local, epoch de duelo e conflito de projeção. São informações diagnósticas, não uma nova interface pública nem capabilities. Tokens completos, callbacks na etapa correta, owner aplicado, revisão e identidade continuam necessários. Os testes invocam callbacks antigos **durante uma negociação nova ativa**: busy sozinho não torna um pacote antigo válido.

No caso201, o nonce do ticket do caller é adulterado. Ele pode coincidir com um nonce legítimo futuro sem tornar o ticket válido: action/client/contexto ainda diferem. Monotonicidade é verificada contra a oferta autêntica do produtor, não contra o valor forjado pelo caller. Isso é registrado como correção do oracle, mantendo o controle anterior.

## Resultados, controles e proveniência

Resultado final: **544 processos completos**, com224 PASS,264 REFUSED,48 NOT_REACHED e8 FAIL históricos da roleta. **5.978 verificações PASS e8 FAIL históricos**, sem novos FAIL funcionais na entrega. As fronteiras novas somam192 processos/2.470 verificações sem FAIL; os probes físicos não executados permanecem NOT_REACHED.

|Grupo|Processos|Classificação|Checks PASS / FAIL|
|---|---:|---|---:|
|lifecycle|104|72 PASS, 32 REFUSED|1528 / 0|
|end-boundary|8|8 PASS|128 / 0|
|projections|72|36 NOT_REACHED, 36 REFUSED|734 / 0|
|off-projection|8|8 NOT_REACHED|80 / 0|
|primary|88|24 REFUSED, 64 PASS|904 / 0|
|boundaries|112|108 REFUSED, 4 NOT_REACHED|1032 / 0|
|regressions|144|56 REFUSED, 80 PASS, 8 FAIL|1500 / 8|
|absence|8|8 REFUSED|72 / 0|

[Matrizes e contagens](hal-9-recovery/RESULTADOS.json), [oráculos independentes](hal-9-recovery/INVARIANTS.json), [callbacks/arestas e hashes](hal-9-recovery/WRAPPERS.json), [controles](hal-9-recovery/CONTROLS.json) e [manifesto dos logs originais](hal-9-recovery/RAW_LOGS.json). Apenas repetições exatamente idênticas após três ocorrências são comprimidas nos logs publicados; hashes/tamanhos/contagens preservam os originais. As versões próprias de fontes usadas em controles/harness anteriores também ficam registradas.

Os controles PR35 reproduzem a fence residual e sobrescritas das projeções. O primeiro protótipo desta etapa introduziu uma recusa de restauração quando só a seleção mudava: o controle230 conserva8 FAIL e a candidata separa identidade de seleção para corrigir o problema. O requisito inicial de recuperação universal após HANDOFF também conserva seu FAIL de hipótese, pois o bloqueio é do owner RD. Erros de harness por função privada inlined e proxy de fault de uso único ficam separados como INCOMPLETE; não são resultados dos MODs. Marcadores de recusa genéricos dos probes foram corrigidos para separar NOT_REACHED de recusa efetiva.

[Builder e reprodução](../tests/hal9-recovery/README.md), [BUILD_CANDIDATE](hal-9-recovery/BUILD_CANDIDATE.json), [BUILD_REPEAT](hal-9-recovery/BUILD_REPEAT.json), [ABI](hal-9-recovery/ABI.json) e [AUDIT](hal-9-recovery/AUDIT.json). Objeto privado candidato SHA-256 `03cc67b9ff6e54a096e0acfcb89451ca6d5da980e4ce0918f1923ce267869834`; dois builds i386/API24 byte-idênticos. Imports, exports, manifest, settings e assets não mudam; seis unidades legadas preservadas e relink da referência reproduzido. Compilação mantém os avisos preexistentes de headers/variáveis do adapter; não foram alterados terceiros para silenciá-los.

O núcleo permanece parcialmente binário e exige a bridge privada dos PR31–35. Não é recompilação integral nem instalação stock API24 universal. RD companion e Dual local mantêm seus hashes anteriores. Não se publica nenhum desses objetos privados, imagem, save, recurso do jogo ou asset proprietário.

As regressões preservam Queen com RD ON/OFF, ritual pendente, duas instâncias físicas, Monster→Support e Support→Monster, quotas já gastas, HARD/Dual, Encore/Fusion, ABI1, política B e guard. Os oito FAIL de reset34 continuam exclusivamente históricos e separados. Cancelamento terminal da roleta não foi modificado.

## Prontidão

A recuperação **local** pode ser estabilizada na fronteira disponível sem mudanças globais no core ou nas mecânicas. A recuperação de toda a composição ainda depende do owner RD e dos limites de materialização/roleta descritos. OBSERVE permanece padrão; KEEP_RD/ALLOW_TW só em testes explícitos. Guard continua recusando TW; política B preservada, A desligada.

Antes de integração completa: autorizar e revisar a fronteira de novo duelo do RD, qualificar o conjunto exato de callbacks/materializadores e resolver o lifecycle terminal do core TW com fontes ou contrato seguro. Decisão de prioridade funcional e validação Windows são pendências distintas. Nenhum merge/release ou atualização de PR anterior foi realizado.
