# Contrato terminal da roleta — proposta separada, sem implementação

O companion de seleção termina antes da materialização. Ele não é um cancelador do Time Magic que já consumiu uma carta. Nenhum byte do núcleo TW, roleta, probabilidades, Encore, animações ou evolução foi alterado nesta etapa.

No SDK, reset é callback pós-load. `Mods_Reset()` em voo do PR33 é instrumentação, não prova de fluxo normal de UI. A política B continua recusando o load antes da sobrescrita, inclusive com owner residente OFF. O resultado phase1/spin0 e as requisições repetidas do transporte permanecem evidências anteriores, sem serem apagadas ou reclassificadas como cancelamento bem-sucedido.

Uma interface terminal futura precisa distinguir: seleção ainda não consumida; carta consumida e tentativa aberta; confirmação de resultado; efeitos em execução; conclusão terminal. Deve carregar owner, action/generation, wheel epoch/ID e motivo de cancelamento. O owner deve invalidar a geração antes de solicitar cancel ao wheel; o wheel confirma que não executará novos callbacks dessa geração ou fornece uma regra explícita para ignorá-los. Busy/NACK não é autorização para destruir tabelas/contexto. Reutilização de ID não pode transformar callback antigo em resultado da tentativa nova.

Após consumo não se retorna a carta à mão por ID. Cancelamento não implica desfazer destruição, evolução, Encore já concedido ou efeitos parcialmente aplicados. Cada owner precisa declarar sua fronteira segura e a política para uma tentativa interrompida; decisões de mecânica não são inferidas desta proposta.

Adoção depende de fonte integral exata do núcleo ou de uma alteração de objeto estritamente identificada, auditada e autorizada. Não zerar BSS, phase ou flags por tentativa. Permanecem pendentes testes de backend/renderer/UI Windows legítimos, cancelamento terminal cooperativo, falhas de restart e eventuais efeitos já em curso. A presente tarefa não reexecuta reset em voo como se fosse gameplay normal.
