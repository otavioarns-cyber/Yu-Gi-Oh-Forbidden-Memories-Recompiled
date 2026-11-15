# HAL-9 / HAL-20 — Time Wizard × RD, bônus e lifecycle

Continuação do [PR32](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/32), HEAD `54234112bb26e0586d7a1779a33cafa806842d26`. Branch independente `test/hal-9-tw-rd-bonus-lifecycle`, destino `integration/hal-20-v0.3.1-preview.2`. [HAL-9](https://linear.app/half-programer/issue/HAL-9) e [HAL-20](https://linear.app/half-programer/issue/HAL-20). Nenhum merge, release, alteração dos PRs anteriores ou admissão de Time Wizard.

## Resultado e escopo

A candidata corrige duas incompatibilidades técnicas no **adapter TW disponível e efetivamente vinculado**: seleção antiga durante snapshot de bônus e redirecionamento posterior da segunda cópia TW para uma categoria já gasta. Não recompilamos nem reconstruímos o núcleo legado. Não refizemos os detectores HARD/Dual do PR31.

Com RD OFF, o controller/planner real disponível completa **TW Support → Monster**, H OFF/ON e Dual ON. A mão5→4→3, o ledger0→2→3 e as instâncias físicas permanecem únicas. Com duas cópias ID16, a primeira usa deck40, a segunda deck41; a segunda conserva Monster/row2 e os wrappers TW/F não a encaminham novamente a Time Magic. Sem Dual, não se inventa bônus.

**A incompatibilidade de seleção com RD continua recusada**, antes de consumo, inclusive sob Queen mandatory. Resolver sua arbitragem requer um handoff explícito e uma decisão sobre propostas não obrigatórias; nenhuma prioridade nova foi adotada. Reset em voo continua sem contrato terminal demonstrado. A coleção completa permanece fora do guard.

Produção (`src`, `mods`, `config`, workflows),507 arquivos canônicos de MODs, Queen, estratégias, settings, receitas, probabilidades, assets, ABI1, Dual e política B estão idênticos ao PR32. Só o pacote **privado de teste** muda: `source/time_wizard_dual_play_compat.c` e seu objeto relinkado. Publicamos o patch próprio, builder, testes, hashes e documentação; não o core/objeto/assets privados nem qualquer arquivo do Dual.

## A — Propriedade da seleção e RD

Os traces de `AiScript_Run` e `func_8001BAF0` observam a transação real, sem escrever pending, plano ou quotas:

1. `rd_ai_plan` delega e produz pending legítimo do ritual680, fonte lógica16. `rd_selection_prepare` captura seleção16/destino6, action/generation e snapshot. O produtor real não impõe mandatory nesse cenário.
2. O `ai_run` embutido TW, mais externo, muda a seleção para11/destino6 (ID16/Time Magic). O pending680 continua pertencendo à decisão anterior.
3. `cosmo_hand_swap` consulta `precommit` real. `rhs_live` detecta a diferença, causa SNAPSHOT2. O callback de swap seguinte não é delegado; nenhum tributo/cartão é consumido.
4. Três transações inválidas levam a BLOCKED3, cause LIMIT9, last_cause2, attempts3/limit3. Mão5 e deck íntegros.

Caso44 usa **somente a recuperação pública existente**, com sequence atual e snapshot físico consistente. Sequence antiga é recusada. A recuperação incrementa generation e permite novo planejamento; os planners reais discordam novamente e atingem BLOCKED após três tentativas. Nenhuma retomada silenciosa, limpeza privada ou restauração por ID foi acrescentada.

Caso45 coloca Queen real no mesmo cenário favorável a TW. RD captura a seleção Female395 (fonte15/destino1), `mandatory=1`, pending inativo. TW tenta trocá-la posteriormente por Support; a mesma validação recusa. Não se enfraqueceu Queen para obter PASS. A regressão histórica com Queen sem esse conflito permanece preservada.

### Proposta não implementada

ABI1 publica connect/rules/precommit/outcome; seu companion lifecycle tem disconnect/invalidate/recover. **Nenhum deles autoriza substituir genericamente uma seleção já capturada e seu pending.** `recover()` é uma ação explícita de recuperação e não deve ser usado pelo TW como cancelamento automático para vencer a disputa. `RHS_CANCEL_INTENT` produz veto, não transferência de propriedade.

Uma extensão deve ser um companion versionado separado, mantendo ABI1 congelada. Proposta técnica para revisão:

- Fase final de planejamento **antes de qualquer troca/materialização**; token action/generation/connection/producer revision e snapshot físico exatos.
- Owner mandatory preservado/reavaliado; uma proposta alternativa não pode suprimir Queen.
- A proposta externa descreve fonte física, seleção, destino e **categoria/ação definitiva**. ID16 isolado não distingue Monster e Time Magic; a validação nativa de `rhs_selected` não é uma autorização genérica para híbridos em M/T.
- Substituição deve negociar o destino do pending anterior de forma atômica antes de consumo, recapturar/revalidar e obter ACK. Ausência de ACK, identidade alterada, reentrância ou quotas incompatíveis significa recusa; jamais rollback após consumo.
- O materializador responsável executa uma única ação aprovada; Dual continua proprietário do gate de categorias/quotas. A proteção de cursor/pilha permanece até a coleta física.

Duas políticas de gameplay ainda exigem decisão: respeitar a última proposta não obrigatória de TW com handoff, ou preservar a proposta anterior RD e não executar TW nessa ação. Ambas mudam o vencedor do conflito atual; não adotamos nenhuma por ordem arbitrária. Comparar scores exigiria um contrato estratégico novo e não é a correção técnica mínima. Mudar só ranks pode mudar o vencedor, mas não prova que o pending pertença à seleção final nem resolve lifecycle; não foi promovido como solução.

## B — Bônus: causa, correções e fronteiras reais

### Snapshot

Após Support, ledger2 é correto. Na entrada do próximo controller, `D_800EAE88` ainda descreve TW Support. CPU hand mode2 pode entrar com o subestado antigo49155. **Dentro** do controller, o bit de inicialização0x80 ausente faz o motor zerar o subestado antes de chamar `func_80028220`.

O adapter antigo interpreta a seleção velha como ação atual e troca temporariamente used2→1. O `dual_ai_setup` real então oculta Monsters e mantém Support344. Quando o planner oferece344, o gate Dual o recusa. Não é exploração de quota demonstrada; é construção incorreta da disponibilidade pelo adapter.

O patch impede esse ajuste de confirmação no ciclo CPU que vai construir um novo snapshot: mode2 com bit0x80 ausente **ou** substate baixo0. `compat_ai_run` e `compat_ai_swap` continuam adaptando a proposta nova, como antes. Não há novo hook, mudança de quota, chamada adicional a swap ou alteração de score. O snapshot final mostra used2 e `99,0,111,112`; o planner real escolhe99/H OFF ou112/H ON e o gate permite Monster. Ledger3, uma única carta adicional consumida.

Uma primeira condição que verificava só substate0 falhou porque ignorava a inicialização interna. Uma tentativa de wrapper adicional foi descartada pela mesma evidência; não faz parte da candidata. O objeto contém símbolos estáticos duplicados, mas o callback registrado de draw tem40 bytes, não o primeiro símbolo homônimo23. A cópia de teste do pin usa o callback realmente registrado. Esses erros de desenvolvimento não são falhas novas do Dual/núcleo.

### Duas instâncias e categoria após o gate

O teste46, com cópias físicas40 e41, revelou uma segunda incompatibilidade que o caso de cópia única não alcança. O `compat_ai_run` existente converte corretamente a nova proposta TW para Monster após Support gasto, e o Dual valida essa categoria. Porém, na seleção de zona, o **`ai_select_zone` embutido** conserva `awaiting_tw_carduse` da proposta EFFECT e reescreve destino1→6/row3. Isso ocorre no retorno do node AI, antes da coleta, como provam as bordas e alterações de seleção. O adapter acabava contabilizando2 outra vez.

A segunda parte do patch, no `compat_select_zone` já existente, mantém o destino Monster **já validado pelo Dual** somente no bônus CPU com used2/last_plan1, seleção single-TW, mesma fonte, mesmo índice físico de mão e mesmo objeto antes/depois da delegação. Configura row2/col correspondente antes da coleta nativa. Não escolhe outra carta, não substitui instância por ID, não materializa e não modifica o núcleo ou a IA C não vinculada. Não interfere com a prioridade RD/Queen: quando essa transação está inválida, o swap já é recusado antes dessa etapa.

A variante só de snapshot conserva quatro FAIL no46; a candidata com ambas as fronteiras passa as mesmas condições. O teste confirma UID41, row2, ledger3, mão3 e nenhum reroute posterior dos wrappers para Time Magic/consumo adicional. **A apresentação nativa completa da colocação Monster continua modelada**, não certificada.

O47 tenta uma extensão por botões com duas cópias na mão humana. A primeira Square é real; o frame Dual não chega a conceder o bônus nessa fronteira parcial. Portanto o segundo Square/X humano fica NOT_REACHED, sem injetar pending_return ou bonus. Os testes recebidos de seleção/decisão cobrem recusas de categoria em modelo; não substituem esse caminho humano não alcançado.

## C — Reset, cancelamento e recuperação

`MemoriesMod.reset` é documentado como callback após carregar savestate; o chamador real encontrado é `state.c`, depois da restauração. Não é uma operação pública genérica de desfazer roleta. A política B continua recusando TW residente antes de qualquer sobrescrita. Logo, `Mods_Reset()` em voo, chamado diretamente pelo harness, **não demonstra um fluxo normal aceito pela UI ou um load completo**.

O `reset_loaded` do núcleo fixado lê a fase anterior, zera spin_id e revalida estados de animação conforme a fase; não contém uma garantia de fase0 para a fase1. O Encore limpa sua tentativa. Não aplicamos patch binário, não zeramos BSS e não editamos efeitos/animações.

- Quieto após resolução: Reset mantém fase0 e não retorna a carta; uma nova fixture de duelo executa planejamento/consumo normalmente.
- Em voo: fase1/spin0 permanece, inclusive cancelando o proxy real antes do Reset. Callback antigo não altera phase/chosen/won, nem retorna a carta consumida.
- Drive limitado de240 ticks após Reset: nenhuma segunda carta consumida, mas a fixture observa240 novas requisições ao transporte wheel, sem garantir progresso terminal. Não qualificamos isso como nova tentativa concluída nem extrapolamos para loop infinito de gameplay. O transporte não simula conclusão nessa experiência.
- Nova fixture de duelo: tentativa nova é alcançada; callback antigo é ignorado durante a nova roleta, sob IDs monotônicos do transporte. É uma pré-condição explícita, não prova de ABA se um provider reutilizar IDs no mesmo processo.
- Preferência OFF/restart pendente e OFF/ON efetivo instrumentado preservam os controles históricos. Reinício de processo não recebe ponteiros antigos; não inventamos persistência entre processos.

O FAIL histórico do34 continua FAIL. Casos41–43 têm checks de integridade PASS, mas recuperação/cancelamento terminal NOT_REACHED. Não chamamos a ausência de consumo duplicado de solução de lifecycle.

Proposta separada: especificar cancelamento terminal pelo owner, invalidação de geração/ACK do wheel e fronteira segura de retomada antes de qualquer tentativa de liberar load. O núcleo principal não está disponível integralmente e pode depender de fases de animação; a correção deve vir de fonte exata ou de uma alteração de objeto estritamente auditada e autorizada, nunca de limpar campos por tentativa. B permanece a mitigação atual.

## Artefatos e ABI

Baseline TW: `6470024d45bd099b7800bd5f84ccdcef1a895a8a2447ecef058af11ada97d5d7`.

Candidata privada: `46f63050a5761b6f7ca3ec44c7a267ee4a7f79ec1572fb3d41ecd1e6313d9093`.

O builder verifica que relinkar as seis unidades reaproveitadas mais o adapter original reproduz exatamente o baseline. Só recompila o adapter disponível em i386/API24. Exports públicos preservados, imports antigos preservados; acrescenta apenas `D_8009B20C`, export oficial API24. As três consultas privadas opt-in do PR31 continuam necessárias; `mod.api` legado continua4, host24/pointer4. Não é rebuild integral nem objeto instalável em core stock.

A cópia privada do harness usa o hash novo para permitir **imports**; os pins de produção e o guard não mudam. O teste8 continua recusando entrada com TW. Não requalificamos a coleção por ID/nome. [Build](hal-9-compat/BUILD_CANDIDATE.json), [auditoria](hal-9-compat/AUDIT.json), [patch](../experiments/hal9-compat/bonus-snapshot.patch), [reprodução](../tests/hal9-compat/README.md).

## Evidência, riscos e próximos passos

|Grupo|Processos|Checks|Classificações|
|---|---:|---|---|
|Candidata final combinada|184|2.032 PASS /8 FAIL|94 PASS /32 REFUSED /50 NOT_REACHED /8 FAIL|
|Regressões da candidata|112|1.144 PASS /8 FAIL|70 PASS /16 REFUSED /18 NOT_REACHED /8 FAIL|
|Qualificação final adicional|72|888 PASS|24 PASS /16 REFUSED /32 NOT_REACHED|
|Controle PR32|16|168 PASS /4 FAIL|4 PASS /8 REFUSED /4 FAIL|
|Controle somente snapshot, duas TW|8|96 PASS /4 FAIL|4 PASS /4 FAIL|
|Coleção sem TW|16|164 PASS|16 PASS|

As oito falhas da candidata são exclusivamente o assert histórico de fase terminal0 no34. Não surgiu FAIL funcional novo nos cenários finais executados. Os quatro FAIL do controle PR32 revelam a máscara incorreta; os quatro do controle snapshot-only revelam o redirect de zona. Não somar controles ou repetições como cobertura independente. As regressões recebidas completaram sete etapas de build/execução com retorno0, incluindo672 combinações de decisão,5.120 de seleção,18 asserts de evolução e controles ELF32 de Encore/direct. Isso não transforma fontes/modelos auxiliares em execução Windows.

As tabelas em [RESULTADOS](hal-9-compat/RESULTADOS.json) separam controle PR32, variante só snapshot, candidata final, regressões recebidas, coleção sem TW e política B. Matrizes, logs e fingerprints de callbacks acompanham o relatório. FAIL histórico de reset e FAIL de controles continuam visíveis. REFUSED preserva a proteção, mas significa bloqueio de integração. NOT_REACHED não vira PASS por um assert de integridade.

H/D OFF/ON e dois ranks R são executados. As dependências podem produzir a mesma ordem efetiva; consultar callbacks/arestas/fingerprints em vez de afirmar oito ordens distintas. O guard do PR20 não foi ampliado. Não há alegação de compatibilidade universal com interposições desconhecidas depois da última validação.

Nenhum `WA_MRG.MRG`, imagem legítima ou backend Windows utilizável foi encontrado entre os recursos selecionados. Transformações completas ficam NOT_REACHED; renderer/UI/duelo Windows ficam AUTOMATION_UNAVAILABLE/NOT_REACHED por recursos, não por proibição do usuário. Não repetimos Wine/QEMU PE sem mudança técnica. Somente a bateria pessoal jogada pelo usuário permanece para a fase final.

**Prontidão:** o bônus tem uma candidata técnica verificável no adapter, mas TW ainda não pode ser admitido à coleção completa. Próximo passo mínimo é revisar a proposta de finalização/handoff com RD, autorizar a política para propostas não obrigatórias e definir o contrato de cancelamento terminal do núcleo. Não é necessário repetir migrações/detectores. Até lá, preservar recusas, B e todos os artefatos anteriores.
