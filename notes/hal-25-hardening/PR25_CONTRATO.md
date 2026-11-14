# Contrato experimental de seleção de mão — ABI 1 (especificação anterior à implementação)

Escopo: seleção de mão da IA, synchronous, mesma chamada do controller, antes da captura final e do swap. Não é contrato de escolha de campo, posição, alvo de ataque, quota ou estratégia. Não muda Queen Lock. Sem dependência obrigatória.

## Descoberta e identidade

O core só descobre `owner-id:nome`. Preservar o manifest de R implica manter seu ID legado; não há alias global autorizado. A descoberta pública nova usa `ritual-duelists:mandatory-hand-selection-v1`, um rendezvous opcional publicado pelo consumidor. O produtor R conecta sua tabela ABI1 nesse rendezvous no início de seu callback AI e após init. A consulta é anterior ao planner RD em ambas as ordens. Nenhuma chamada pública nova usa o ID temporário de R; não se altera core/manifest para criar alias. A tabela real de R é consultável por `rules()` desse rendezvous. R conserva seus quatro providers existentes; não publica um novo alias sob o ID legado e RD não procura esse caminho temporário.

O rendezvous aceita uma única tabela qualificada; versões/tamanhos incompatíveis são rejeitados. Não é um sistema global de arbitragem de múltiplos efeitos. Ponte privada, offsets, assinaturas e whitelist de cartas são proibidos na implementação.

## Mensagens e validação

Entrada: tamanho/ABI, par action/generation não nulo, snapshot por valor de lado, fase, turnos, mão física, deck, campo, janela lógica e seleção de 12 bytes. Identidades incluem índice/ID/data_block, endereço do deck record, slot físico, objeto e flags. O snapshot é reconstruído e comparado exatamente; não se usa apenas um hash. Ponteiros são tokens de identidade no processo, nunca persistidos ou transferidos de alvo.

Saída: echo de action/generation, status `UNAVAILABLE`, `UNCHANGED`, `REPLACED`, `CANCELLED` ou `INVALID`, indicação separada de obrigatoriedade, seleção/destino e referências físicas selecionadas, revisão opaca do owner. `UNCHANGED` não significa que exista restrição; mandatory distingue isso. `REPLACED` exige decisão obrigatória do produtor, dados físicos consistentes e nenhum consumo. `CANCELLED` veta a ação sem fallback; `INVALID` ou resposta malformada também impedem commit. Unavailable/OFF não impõem Queen; mantêm o plano normal.

O produtor usa o predicado e a preferência existentes de Queen, extraindo apenas uma função que recebe/devolve bytes de seleção, sem alterá-los globalmente. Não troca/consome/materializa cartas. A pureza é propriedade do código e verificada por snapshots antes/depois; não há sandbox de memória entre MODs nativos.

## Transação local e cancelamento

RD valida depois do planner, antes de o controller capturar seleção/categoria. Somente pending incompatível ainda não materializado é retirado por override. Mantém snapshot da escolha aprovada e revisão do produtor. Antes de seu swap, revalida igualdade exata, owner/revisão e decisão sem permitir retarget depois da captura; alteração/cancelamento/invalidação implica zero seleção, pending abandonado e nenhuma delegação materializadora. O wrapper de mão de R consulta a mesma fronteira local antes de impor Queen, impedindo que um zero posterior seja ressuscitado quando R está externo. Não se acrescenta previous_swap após uma troca concluída.

Reset/aplicação do owner mudam a geração; estado de política que afeta Queen participa da revisão. Tokens são efêmeros e não estendem save state. Respostas antigas/outro lado/instância igual de ID, mas distinta, são inválidas. A serialização de chamadas é pressuposta; reentrância e overflow não emitem tokens reutilizados.

A proteção termina na última fronteira própria antes de delegar ao materializador. Um terceiro que modifique/materialize depois dessa fronteira sem contrato continua fora da garantia; não se altera controller/hooks globais para impedir estados artificiais. O teste deverá delimitar esse caso, o cancelamento bruto e a ausência de janela UI normal entre planner/swap.

## Qualificação e limites

Versão experimental; não ABI definitiva. Validar manifests/settings/assets/receitas/ABI existente, estados OFF/ausente/versão incompatível, duas ordens, H/D on/off, cancelamentos, identidade, quotas e regressões. Testes Windows exclusivamente manuais ficam BLOCKED até a última fase do projeto, conforme diretriz expressa do usuário; não solicitar teste intermediário.

## Qualificação posterior da PoC

O request distingue `RHS_PROPOSE` de `RHS_CANCEL_INTENT`. Proposta vazia **não** é veto: o código original de Queen pode completá-la. Cancelamento declarado e alteração após aprovação têm semânticas distintas. RD envia propostas e trata `CANCELLED`/resposta inválida como veto; não inventa uma origem de cancelamento UI que o controller não oferece. O campo intent é suportado e testado pelo produtor para clientes explícitos.

O controller nativo avança do subestado0 para1 após o callback void de swap; bytes zerados, isoladamente, ainda levam ao consumo do cursor antigo no subestado2. Por isso a implementação acrescenta um único hook local do RD em `DuelScene_UpdateHandActions`: somente na transição abortada 0→1, com ator/fase e mão/deck físicos intactos, devolve o subestado a0 para o snapshot/VM nativos prepararem **nova ação**. Se a identidade física mudou, mantém quarentena até reset/novo duelo, sem reparar a mão. `outcome()` torna esse resultado observável (`2` retry, `-2` quarentena, `-1` veto antes do avanço). O núcleo/controller original não é alterado e caminhos normais não são reordenados.

A enumeração `bridge0` em nomes de perfis e um contador fixo0 são metadados herdados do runner comparativo. Nenhuma implementação de ponte privada foi copiada; o único callback de regra pertence ao objeto real de R e é obtido pelo rendezvous público. A resolução gráfica pós-colocação continua sendo um modelo, não a ponte privada de Queen.

Limite: chamadas síncronas serializadas, Windows i386/API24, uma tabela de produtor por rendezvous. Não é sandbox de MODs nativos, autenticação de terceiros ou garantia contra escrita/materialização não cooperativa depois da última fronteira própria. O contrato de mão não arbitra campo, posição ou ataque. Qualificar um desses domínios exigirá interface própria ou nova ABI e autorização.
