# HAL-25 — fronteiras do contrato e recuperação experimental

## ABI de seleção preservada

Referência integral: [contrato do PR #25](PR25_CONTRATO.md), commit `f6cea112db8d14be34ab2f28694b0121a94842c2`. Os headers `mandatory_hand_selection_v1.h` e `hand_selection_snapshot.h` permanecem byte a byte iguais à referência em ambos os pacotes. O rendezvous opcional continua `ritual-duelists:mandatory-hand-selection-v1`. Não há dependência obrigatória de R nem alias do seu ID legado.

| Estrutura i386 | Bytes | Política |
|---|---:|---|
| RhsRules | 12 | ABI1 e tamanho exato; callback não nulo |
| RhsRendezvous | 24 | ABI1 e tamanho exato; tabela residente |
| RhsRequest | 4.208 | snapshot por valor; action/generation não nulos |
| RhsResponse | 200 | echo, status, revisão, seleção e identidades validados |
| RhsLifecycle (nova, separada) | 28 | ABI1, tamanho exato, magic `0x5253484c` |
| RhsRecovery (nova, separada) | 72 | somente valores, sem ponteiros |

Não se acrescentaram campos às tabelas antigas. Novos significados incompatíveis exigem nova chave/ABI e adaptação explícita; `size >= conhecido` não é aceito. O companion `ritual-duelists:mandatory-hand-lifecycle-v1` também é **experimental**, não um contrato promovido. O produtor antigo ignora o companion; o consumidor antigo ignora sua existência. Os 12 processos mistos verificam somente a comunicação normal ABI1, não estendem as garantias de recuperação a binários antigos.

Os exports nativos continuam R=2/RD=44; imports de ambos permanecem exatamente iguais aos do PR #25 e pertencem ao SDK API24. O novo provider é registrado por chave, sem novo export ELF. RD conserva o state registrado de 288 bytes/v4; tokens/pending/ponteiros novos são BSS transitória, sem serialização inventada. [Comparação R](ABI_R.json), [RD](ABI_RD.json), [proveniência](BUILD.json).

## Descoberta, propriedade e callbacks

RD aceita uma única tabela de produtor, copia seus campos e rejeita mutação posterior. Outra tabela não substitui a conectada silenciosamente. A cópia não autoriza liberar a tabela original: o endereço original, seu contexto e código precisam permanecer válidos até `disconnect(original)` retornar 1. Uma chamada com ponteiro diferente retorna 0, mesmo que seu conteúdo seja igual. Desconectar durante avaliação/planejamento retorna 0, invalida a operação corrente e **não** autoriza unload. O teste mantém a memória viva; não fabrica uma demonstração de segurança com memória já liberada.

R procura o companion novamente a cada notificação/desconexão; não guarda callback do outro MOD entre processos. Reset/aplicação efetiva alteram a revisão e invalidam a transação. A preferência salva de `host->applied()` não substitui o estado efetivo do callback: `Mods_Apply` pode deixar o owner ativo com OFF pendente de reinício. Os manifests originais de R/RD exigem reinício e foram preservados.

Chamadas são síncronas no thread do controller. A avaliação verifica geração, conexão, interrupção, action corrente, resposta e snapshot antes de aceitar o resultado. Reentrância, reset dentro do callback ou desconexão concorrente cooperativa invalidam o resultado externo. Counters saturam: não há wrap para reutilizar tokens. Exaustão de serial/generation/epoch provoca recusa até reinício de processo. Reset de duelo não recria uma encarnação esgotada.

**Limite de segurança:** este ABI não é sandbox nem autenticação de código nativo. Não consegue validar um ponteiro arbitrário antes de lê-lo, impedir free/unload não cooperativo ou proteger um callback executando em outra thread. O shutdown normal serializado não introduz esse caminho. Qualquer futura API de unload concorrente deve respeitar o ACK e oferecer sua própria barreira; R não pode tornar uma operação de unload externa segura por copiar uma vtable.

## Cancelamento e recuperação

Uma proposta vazia continua diferente de um veto. Queen pode completar uma proposta vazia conforme sua regra original; `CANCELLED`, resposta inválida e snapshot obsoleto impedem o commit. O planner estratégico de RD permanece byte a byte igual ao do PR #25. A nova proteção atua no wrapper transacional, não escolhe outra estratégia para evitar veto.

Cada transação inválida conta uma tentativa; cancelamentos secundários do mesmo par action/generation não contam novamente. Até três transações inválidas consecutivas podem gerar replanejamento. Na terceira, o estado fica BLOCKED, sem novas chamadas à VM/provider por tick. Retirar a causa não retoma silenciosamente a ação vetada. Resultado normal de VM=0 é espera/NOT_REACHED, não erro nem autorização para inventar jogada.

Após veto, só se retorna o controller local ao início se mão/deck físicos continuarem íntegros. Mudança física produz QUARANTINE. Recuperação explícita exige sequence atual, ausência de avaliação em curso, fase/subestado permitido e igualdade exata com o checkpoint físico original. `recover` não repara mão/deck, não procura uma carta substituta pelo ID e não restaura pending. Ao aceitar, invalida a geração, descarta a transação vetada e permite uma decisão nova. O teste que restaura um checkpoint o faz explicitamente como instrumentação, antes de pedir recuperação; a produção não escreve essa restauração.

A fronteira acompanha seleção e identidades depois da delegação e até o primeiro consumo nativo. Alteração tardia dos bytes ou troca coerente entre duas cópias de mesmo ID é recusada/quarentenada. Não se adicionou `previous_swap` após materialização. O monitor é encerrado no primeiro consumo: ele **não** garante todos os consumos posteriores de um ritual nem desfaz uma escrita/consumo de terceiro já executado antes de uma fronteira própria. Esse caso permanece fora da qualificação local.

## Estado observável proposto para futura UI

`read(out,72)` é puro: não consulta o produtor, não tenta novamente e não muda tokens. A UI futura deverá verificar ABI/tamanho/magic, exibir estado/causa e reler depois de qualquer tentativa de recuperação. A chave é opcional; sua ausência significa que esse diagnóstico não está disponível, não que o duelo está liberado.

| Estado | Significado | Ação futura permitida |
|---|---|---|
| IDLE=0 | sem transação validada | observar |
| VALIDATED=1 | proposta capturada, ainda revogável | observar |
| RETRY=2 | transação recusada, dentro do limite | aguardar nova tentativa controlada |
| BLOCKED=3 | limite de transações inválidas | recuperação explícita se permitida |
| QUARANTINE=4 | checkpoint/seleção física suspeito | recuperar somente checkpoint exato; caso contrário reset |
| EXHAUSTED=5 | encarnação sem tokens seguros | reinício completo |
| HANDOFF=6 | controller recebeu a proposta | observar até primeiro consumo |

`cause` identifica o estado atual; `last_cause` conserva a causa raiz da recusa, inclusive quando o limite posterior passa a ser a causa atual. Códigos: veto=1, snapshot=2, protocolo=3, owner=4, reentrância=5, counter=6, identidade física=7, escrita tardia=8, limite=9 e consumer OFF=10. Nem todo código reservado foi alcançado por um fluxo normal; não inferir isso da enumeração.

`attempts/limit`, `action/generation/connection_epoch`, `connected/busy/effective`, `side` e `last_provider_status` permitem diagnóstico sem expor ponteiros. `recovery` informa NONE=0, EXPLICIT=1, DUEL_RESET=2 ou PROCESS_RESTART=3; a solicitação ainda pode falhar se a identidade mudou depois da leitura. O botão futuro de retomada deverá enviar a sequence observada; retorno 0 exige reler e explicar a causa, sem repetição automática. Não há UI definitiva nesta branch, nem promessa de reabrir um savestate antigo em estado coerente.
