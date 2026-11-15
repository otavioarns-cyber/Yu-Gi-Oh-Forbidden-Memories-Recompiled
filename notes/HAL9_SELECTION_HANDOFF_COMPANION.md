# HAL-9 / HAL-20 — Fronteira cooperativa RD × Time Wizard

Continuação do [PR33](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/33), commit `c82e1efbfd0a49e37652364398adebdb85ecd043`. Branch experimental `test/hal-9-selection-handoff`, destino `integration/hal-20-v0.3.1-preview.2`. [HAL-9](https://linear.app/half-programer/issue/HAL-9) e [HAL-20](https://linear.app/half-programer/issue/HAL-20). Sem merge/release ou admissão no guard.

## Resultado e decisão ainda necessária

**A transferência é viável no laboratório, com confirmação explícita e recusa segura nos casos exercitados.** Ambas as políticas não obrigatórias completam uma ação pelos planners/controllers reais disponíveis: preservar RD executa seu pending original; permitir TW revoga esse pending antes de execução e preserva a proposta final de TW. Queen não pode ser apagada por essa transferência.

Não há política escolhida ou habilitada nos MODs canônicos. O cliente cooperativo é **um driver de teste**, não uma implementação já embutida no núcleo Time Wizard. O mecanismo foi compilado e executado numa cópia experimental de RD; não é somente uma simulação de seu código. Motor/catálogo/tabuleiro/renderer continuam parcialmente modelados, e isso não certifica gameplay Windows ou a coleção completa.

A decisão funcional permanece concreta: quando **não há seleção mandatória**, manter o ritual680 proposto por RD ou permitir Time Magic Support proposto depois por TW. As duas são jogadas diferentes. O companion não compara scores nem inventa prioridade; só verifica a política explicitamente selecionada pelo teste. Em observação, conserva a recusa original do PR33.

## O que mudou

- Novo companion draft `ritual-duelists:selection-finalization-v1`, separado da ABI1 antiga. Header, algoritmo de laboratório, [contrato e diagramas](hal-9-handoff/CONTRATO.md).
- Builder copia RD para diretório privado, inclui o companion, envolve os callbacks locais de materialização/monitor da mão e registra somente essa tabela adicional. O planner, o materializador original e os contratos ABI1 permanecem os mesmos fontes. [Diff exato da cópia](hal-9-handoff/RD_LAB.patch).
- Driver observa a saída dos planners reais e usa a nova interface por chamada explícita. Não injeta pending/plano para produzir o cenário positivo e nunca usa `recover()` como handoff.
- Metadados, testes de ABI, faults identificados, regressões e hashes permitem reproduzir cada variante. [Reprodução](../tests/hal9-handoff/README.md).

**Nenhuma mudança em src/mods/config/workflows canônicos;507 arquivos de MODs e os contratos R/RD antigos byte-idênticos ao PR33.** Manifest/settings/assets/receitas/Queen/estratégias/quotas preservados. TW continua exatamente SHA `46f63050a5761b6f7ca3ec44c7a267ee4a7f79ec1572fb3d41ecd1e6313d9093`; Dual privado `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`. Não publicamos objetos privados, imagem, saves ou assets novos.

Objeto RD de laboratório: `d044cfef6019c8a97c869bade40dfe91ae081fda797e62e6f2f53fbe2a7b2a97`, compilado com SDK24/i386 a partir dos fontes autorizados. [Recibo de build](hal-9-handoff/BUILD_CANDIDATE.json), [auditoria](hal-9-handoff/AUDIT.json) e [fingerprints dos métodos](hal-9-handoff/COMPANION_FUNCTIONS.json). O objeto permanece privado/local; não é uma nova distribuição instalável.

## Captura, proposta, precommit e consumo

1. RD real planeja ritual680 e cria pending source16/índice físico45. Sua seleção capturada é16/destino6, ainda antes da troca de records ou retirada da mão. O snapshot vincula action/generation e revisão do produtor R.
2. O `ai_run` embutido de TW propõe fonte11 da mão, TW16 de índice40, destino6, com ambas as quotas livres. Não alteramos sua avaliação estratégica.
3. O produtor R real pode impor Queen na volta da cadeia. No caso Queen, TW chegou a propor11/6, mas R já restaura Female395/15/1. Resta uma divergência de **byte8,1→0**, não necessariamente do ID final. Isso explica por que conferir apenas o ID não resolveria o snapshot.
4. Sem transferência confirmada, os precommits R/RD antigos continuam recusando a divergência antes do consumo. O cenário original chega a BLOCKED após três tentativas; observação não o mascara.
5. Com política de teste explícita: offer/prepare autorizam somente metadados verificáveis; authorize exige callback; confirm exige ACK, recaptura e nova avaliação do produtor. Somente então a seleção é finalizada e, se ALLOW_TW, o pending exatamente correspondente é revogado.
6. O gate Dual real permanece na cadeia. KEEP_RD usa o materializador RD e não chama H após executar sua troca. ALLOW_TW delega normalmente pelo owner sem pending; H ativo ou o motor executa seu caminho original. O controller retira uma carta; o monitor RD de cursor/pilha/identidades continua ativo.

|Política de laboratório|Ação observada|Consumo/troca|Dual ON|
|---|---|---|---|
|OBSERVE|Recusa original|Mão5; nenhuma troca/consumo|Quota0|
|KEEP_RD|Set do ritual680|Pending mantido até o materializador RD; uma troca original, mão4|Support2|
|ALLOW_TW|TW16 como Support|Pending680 revogado antes do fetch; ritual fica no deck45, mão4|Support2|
|KEEP_RD + Queen|Female395, seleção/posição anterior mandatória|Mão4, nenhuma troca de deck|Monster1|
|ALLOW_TW + Queen|Recusa mandatory|Mão5, nenhum consumo|Quota0|

Esses testes qualificam finalização e consumo da carta selecionada, não a execução posterior de todos os efeitos/tributos. A draft está limitada a proposta anterior/final de uma carta, pending RD MAGIC/FIELD e estado pré-consumo. HOLD, combos e rituais já consumindo múltiplos tributos ficam fora da autorização do companion. Não há rollback fictício.

## Segurança, identidade e ABI

O token inclui action, generation, connection, revisão[3] do produtor, nonce e conexão do cliente. Snapshot verificável, cursor/pilha, identidade física, categoria definitiva e destino são conferidos em todas as fases relevantes. Generation/connection/nonce obsoletos, size/ABI incorretos, produtor invalidado/desconectado e proposta de categoria indevida são recusados. Overflow recusa, sem wrap/reuso.

A mão é vinculada por side.hand, índice, record, objeto e `data`, não pela igualdade de ID. O campo deck_index não é inicializado pelo snapshot nativo para cartas da mão; ele só é preenchido no tail do deck. Não o usamos como prova de identidade da mão. A regressão de duas TW40/41 do PR33 passa intacta, mantendo Support→Monster, ledger3 e consumo de instâncias distintas.

A revisão encontrou uma falha **do protótipo anterior**, não dos objetos canônicos: emprestar ponteiros internos `const` permitia que um cliente defeituoso alterasse a proposta interna. O controle anterior, fixado por source/objeto, executou a carta99 após alteração explícita do argumento. O hardening passa cópias locais e rejeita qualquer alteração nos pacotes emprestados. Os mesmos faults ficam recusados sem consumo. Preservamos os oito FAIL do controle para tornar a correção falsificável.

Desconexão busy não autoriza liberar callbacks. Após ACK de disconnect, o teste protege a página da tabela com PROT_NONE; nenhuma leitura posterior ocorre. Reentrância durante authorize/confirm/disconnect recusa. Tickets confirmados não podem ser reutilizados, e cancelar após consumo não retorna cartas. O código não serializa ponteiros ou BSS e não declara compatibilidade de savestates.

ABI1 congelada: Rules12, Rendezvous24, Request4208, Response200 e Lifecycle28 bytes. Draft nova: token32, offer4316, proposal4312, ticket56, client20, status60 e tabela44. [ABI i386](hal-9-handoff/ABI.json). Não houve mudança na ABI pública do host. Layout/tabela desconhecidos são recusados; evolução incompatível exige novo key/version, não reinterpretar o contrato antigo.

## Cancelamento, recuperação e observabilidade

Faults de cursor e referência física antes do ACK são recusados sem alterar a ação física. Escritas tardias depois do ACK e antes da coleta são detectadas pelo monitor RD já existente. Não se corrige a identidade corrompida para fazer o teste passar.

A observação adicional do lifecycle real distingue:

- Cursor tardio: RHS QUARANTINE4, causa7, uma tentativa, recuperação2 (reset de duelo).
- ACK ausente: RHS BLOCKED3, causa9/last2, três tentativas, recuperação1 (explícita).

Nenhuma chamada a recover() é feita para retomar essas ações. A seleção vetada não reaparece silenciosamente. Uma nova transação tem de ser planejada/negociada de novo, sob a recuperação já existente do owner.

HfStatus registra a última fase e o último resultado de operação, não uma autorização permanente. A futura UI precisa cruzá-lo com token/effective/RHS lifecycle. MATERIALIZED sem consumo pode estar seguido de QUARANTINE do owner; um replay recusado pode coexistir com uma ação validamente CONSUMED. HF_EXHAUSTED por nonce/epoch persistente exige reinício de processo; recover() do RHS não o apaga. A draft não oferece uma UI definitiva nem revive pending depois de confirmação invalidada.

[Cancelamento terminal da roleta](hal-9-handoff/CANCELAMENTO_ROLETA.md) é uma proposta separada. Não alteramos o núcleo binário, phase/BSS, Encore ou regras de tentativas. Reset em voo continua instrumentação de callback pós-load, não fluxo normal demonstrado da UI. A política B permanece recusando load antes da sobrescrita; A não foi habilitada.

## Testes e comparação com PR33

|Grupo|Processos|Checks|Classificação dos perfis|
|---|---:|---|---|
|Protocolo + ausência opcional|280|2708 PASS / 0 FAIL|40 PASS / 240 REFUSED / 0 FAIL|
|Primários|120|1148 PASS / 0 FAIL|24 PASS / 96 REFUSED / 0 FAIL|
|Fronteiras|120|1184 PASS / 0 FAIL|16 PASS / 104 REFUSED / 0 FAIL|
|Recuperação/cópias adversariais|32|304 PASS / 0 FAIL|0 PASS / 32 REFUSED / 0 FAIL|
|Companion ausente|8|72 PASS / 0 FAIL|0 PASS / 8 REFUSED / 0 FAIL|
|Regressões PR33|32|324 PASS / 8 FAIL|16 PASS / 8 REFUSED / 8 FAIL|
|Entrega candidata total|312|3032 PASS / 8 FAIL|56 PASS / 248 REFUSED / 8 FAIL|
|Controle anterior de argumentos mutáveis|16|136 PASS / 8 FAIL|0 PASS / 8 REFUSED / 8 FAIL|

PASS designa verificações aprovadas; REFUSED é uma recusa segura, não gameplay concluído. Os oito FAIL de regressão são a hipótese histórica de reset terminal no34, conservada. O controle de argumentos mutáveis é separado da candidata. Resultados de construção sem processos não são contados como PASS. O histórico de desenvolvimento registra builds falhos/correções de harness e variantes superadas, sem apresentá-las como regressões do jogo.

Os invariantes de [traces físicos e quotas](hal-9-handoff/TRACE_INVARIANTS.json) são verificados somente em suites completas com callback trace. São280 perfis com invariantes físicos PASS e140 observações de quota PASS nos perfis Dual ON. Na ausência de Dual, quota é NOT_APPLICABLE. Corpo de callback, hash do artefato e relocations, ordem/delegação e mudanças efetivas estão em [WRAPPERS](hal-9-handoff/WRAPPERS.json), não qualificados apenas por nome/ID.

Comparado com PR33: OBSERVE/companion ausente mantêm a recusa; KEEP_RD/ALLOW_TW permitem cada alternativa somente após confirmação; Queen mantém mandatory e a seleção anterior; bônus e guard ficam intactos. O controle defeituoso do próprio protótipo ganhou uma correção localizada demonstrada, sem alterar outros MODs.

## Limites e prontidão

A viabilidade demonstrada é **cooperativa e pré-consumo**, no ambiente parcial disponível. Não há garantia contra um MOD nativo que ignore o protocolo, libere código sem ACK ou materialize fora do owner. As chamadas devem ser serializadas na thread do duelo; o busy guard não é um mutex contra chamadas concorrentes arbitrárias. Nenhuma tabela/ticket é persistível entre processos. Reuso ABA de slot/objeto com todos os bytes novamente iguais não foi qualificado: criação/destruição concorrente à negociação exige cooperação e invalidação do owner ou uma revisão física adicional, não igualdade de ID.

O gate Dual ainda vê o tipo nativo Monster antes de TW Support; a contabilização é corrigida pelo adapter existente. Os handoffs positivos começam com ambas as quotas livres. Portanto esta etapa não prova a entrega de categoria definitiva a todos os gates nem qualifica handoff Support em bônus. A regressão Support→Monster com duas cópias continua passando, mas não substitui essa cobertura.

O cliente ACK é o driver experimental. Ainda falta implementar e revisar um cliente cooperativo no adapter próprio TW, escolher uma política para propostas não obrigatórias e qualificar categorias/ordens/bônus/recuperação após invalidar uma confirmação. O owner R real e os planners não foram substituídos por providers falsos nos testes positivos.

Windows, interface/renderer e duelos completos continuam AUTOMATION_UNAVAILABLE/NOT_REACHED por ausência de backend PE32 e recursos legítimos selecionados, como no PR33. Não repetimos tentativas Wine/QEMU PE sem mudança técnica e não solicitamos gameplay manual intermediário. Transformações que dependem de WA_MRG.MRG não são requalificadas por esta prova.

**Conclusão:** o mecanismo pode ser revisto como draft testável de transferência; não está pronto para promoção automática ou integração completa. A decisão necessária é precisamente KEEP_RD versus ALLOW_TW nos conflitos não obrigatórios. Até essa revisão, a coleção preserva seu comportamento e suas recusas, e TW continua fora do guard.
