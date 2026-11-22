# HAL-9 / HAL-20 — Time Magic Support e integração física

Publicado no [PR32 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/32). Continuação do [PR31](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/31), commit `377e8ca4e606da68b3c29f815eff4a646a5ba917`, branch independente `test/hal-9-time-magic-support`, destino `integration/hal-20-v0.3.1-preview.2`. [HAL-9](https://linear.app/half-programer/issue/HAL-9), [HAL-20](https://linear.app/half-programer/issue/HAL-20). Sem merge, release ou admissão automática.

## Resultado

**O planner e os controllers reais disponíveis alcançaram Support**, com RD frio OFF e M/R/F/Field presentes, HARD e Dual OFF/ON. A CPU escolhe TW para M/T, o controller consome uma instância física e o callback original de posicionamento entra em Time Magic. Com Dual ON, o ledger termina em Support=2. Quadrado humano face-up também chega ao caminho direto; X continua Monster e o cancelamento antes do consumo mantém mão/quota.

**Com RD ON foi reproduzida uma recusa real de seleção obsoleta**, antes de consumir: RD captura a ação ritual, TW muda a seleção depois, a validação encontra snapshot divergente e chega a BLOCKED após três tentativas. Não limpamos pending nem desligamos providers como workaround. Isso mantém o conjunto completo fora da admissão.

O bônus Support→Monster apresenta outro bloqueio técnico no adapter TW: uma seleção antiga é considerada durante a inicialização do snapshot do bônus, com quota temporária inadequada. A validação real do Dual recusa a ação seguinte; não foi comprovada violação de quotas. Reset em voo não garante fase terminal0. Transformações chegam aos alvos corretos, mas dependem de recurso do disco ausente. Esses pontos não viraram PASS de integração.

[Auditoria](hal-9-support/AUDIT.json): **nenhuma mudança em produção**,507 arquivos de MODs idênticos ao PR31, core/guard/ABI/configurações/receitas/estratégia/quotas/assets/policy B intactos. TW permanece no mesmo SHA `6470024d45bd099b7800bd5f84ccdcef1a895a8a2447ecef058af11ada97d5d7`; Dual no mesmo `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`. Nenhum binário privado publicado. Detectores anteriores não foram refeitos.

## Por que o PR31 não alcançou o caminho

1. **Geometria da fixture:** `func_80027DF8` lê monstros nos records5–9 por side. Os adversários antigos estavam em0–2, a área de mão; o snapshot não os via como ameaça. Caso20 mantém esse controle. Corrigir só os endereços já permite observar decisões EFFECT em alguns perfis.
2. **Condição estratégica real:** Dragons genéricos podem evoluir em vez de serem destruídos; nem todo campo adversário dá EV suficiente. Não alteramos score. Com três Fiends controlados na zona correta, o núcleo lê três ameaças e produz EV3135, reason7/choice1, seleção11/destino6. É decisão do planner sobre tabuleiro/catálogo controlado, não prova de ocorrência num duelo completo.
3. **Fronteira de fase errada:** a CPU passa por **Hand4 →Placement7 →Use6**. Exigir fase6 logo ao terminar a mão descartava um intermediário válido. Só chegar a7 também não prova ativação: agora executamos o callback real `TimeWizard_PlaceCardInvisible` e conferimos a chegada a6.
4. **Identidade do display:** `func_80017F04` preenche field6B com `card->data[2]`. O modelo anterior deixava zero; o núcleo usa isso ao preparar o record invisível. A fixture agora reproduz esse campo e a decodificação de slots do motor, sem atribuir plano/pending.
5. **Uso nativo:** carregamos o corpo oficial `DuelScene_UpdateCardUse` do objeto residente. Seu COMMON `D_8009B150` precisou de binding ao endereço guest existente; [recibo](hal-9-support/NATIVE_BINDING.json) prova que código/seções/relocations não mudaram. O loader não foi relaxado.

Essas correções são exclusivamente do harness. O objeto privado executado é o mesmo do PR31. O renderer é uma fronteira modelada com objetos válidos, clock de60 ticks/s simulado e transporte do wheel. VM/catálogo continuam parciais; HARD/TW/R/RD/F e os controllers descritos executam código real.

## Evidência física e categorias

No caso23, RD OFF, todos os oito perfis chegam a Placement7 com ID16, destino físico25, cursor row3, identidade de deck40. A mão passa5→4. O posicionador original cria o record invisível, mantém `data=&deck[40]`, o deixa fora de ocupação de M/T e entra em Use6. Os outros quatro índices de mão não são restaurados/consumidos. O record invisível é preparação de efeito da mesma instância, não uma segunda retirada da mão. A contagem de chamadas Setup não é usada isoladamente como contagem de cartas gastas.

O trace antes do consumo ainda mostra ID16/tipo Monster no gate de swap, com destino6 indicando M/T. O adapter híbrido original corrige a contabilização para Support na conclusão. Isso funciona na primeira ação com ambas as quotas livres; **não é prova de que o gate receba a categoria definitiva Support em todas as confirmações/bônus**. Essa fronteira permanece requisito de integração, especialmente diante da recusa do bônus descrita abaixo.

No caso25, o controller nativo executa sua apresentação, abre o wheel e resolve a tentativa automática. O transporte de teste devolve **somente o setor já escolhido pelo RNG do núcleo**; não escreve chosen/won. O primeiro setor é1, falha; o estado termina em scene5 com a mão ainda4 e sem nova materialização. Não há rollback nem retorno por ID.

No caso31, o provider Fusion real reconhece495. A primeira falha é interceptada pelo Encore real; o segundo setor escolhido pelo próprio RNG é0. O core monta o snapshot e remove os três Fiends: flags ocupadas1→0, sem nova retirada da mão. OFF fresco do Fusion não concede retry; OFF após início mantém o cache existente e permite a tentativa já concedida. Nenhuma regra de cache, roleta, chance, timer ou efeito foi modificada.

O catálogo da fixture é sintético e parcial, inclusive tipos genéricos herdados fora das cartas explicitamente tipadas. O teste qualifica a sequência e contratos sob esses dados, não todos os efeitos de cada ID Vanilla.495 é usada para o contrato Encore por ID; não se certifica sua evolução com o tipo genérico da fixture.

## Inputs humanos e face-down

Inputs `0x40`/`0x80` passam pelo controller nativo e wrappers reais, sem preencher seleção:

|Cenário|Observado|Limite|
|---|---|---|
|X|Uma carta consumida, alvo Monster record5, quota Monster1|Renderização final da colocação não certificada|
|Quadrado face-up|Uma carta consumida, cena6 direta, quota Support2; uso nativo e wheel chegam ao fim|Sem certificação visual/Windows|
|Quadrado +flip +confirmar|Face128, alvo M/T record10, quota Support2, mão4|Posicionamento gráfico completo e ativação posterior não executados: NOT_REACHED|
|Quadrado +Circle antes da confirmação|Retorna à mão, mantém5 cartas e quota0|Inputs simulados; não teste pessoal de controles|

O primeiro driver enviava flip/cancel no frame de inicialização, antes de o controller aceitar input; foi corrigido para aguardar READY, sem alterar o MOD. Não confundimos essa limitação inicial com falha de mecânica. Interferência manual na roleta, tempos visuais e modo de dois humanos continuam sem runtime completo.

## RD e contratos opcionais

Caso24: o planner real RD cria pending de ritual680; seu consumidor captura ação/identidades. Em seguida o `ai_run` embutido TW produz sua seleção Support. O gate `cosmo_hand_swap` consulta o contrato real; RD identifica **RHS_CAUSE_SNAPSHOT=2** e recusa. A recuperação atinge estado3/BLOCKED, causa9/LIMIT, last_cause2, attempts3/limit3. Mão5, zero materialização. O pending observado não foi injetado. O problema ocorre com HARD/Dual OFF/ON, não depende exclusivamente deles.

Isso demonstra incompatibilidade de propriedade/ordem da seleção nessa composição; **não demonstra** que RD ou Queen deva ser ignorado, nem que a solução seja limpar o pending. Os providers opcionais R/RD estão conectados, e a recusa é a proteção esperada contra uma troca posterior não negociada. A alternativa mínima é qualificar uma fronteira de decisão final/handoff cooperativo entre TW e o contrato R/RD, preservando a prioridade aprovada e as identidades, antes de qualquer consumo. ABI1 não tem operação genérica de substituir um plano já aprovado. Não implementamos uma extensão/limpeza arbitrária nesta tarefa.

Case26 preserva Queen: uma Female395 é realmente escolhida antes da tentativa bônus. M/R/F/Field estão ativos nos casos de Support; Fusion Encore usa provider real. As dependências/opções não foram substituídas por providers falsos.

## Bônus e quotas: recusa não é aprovação

O bônus é concedido pelo `dual_frame` real depois da ação normal; a conclusão de UI de campo é modelada pelo harness herdado, sem escrever used/bonus/pending. Os logs distinguem essa fronteira do controller real subsequente.

- Após Monster395, alguns perfis HARD ON conseguem uma segunda ação Support344 e ledger3. Isso prova categoria distinta/consumo único nessa ação; **não prova** TW Support no bônus. Nos demais caminhos o planner não conclui uma jogada e permanece NOT_REACHED.
- Após TW Support, o ledger real está2. O trace do callback Dual de setup registra **used1** temporariamente durante o snapshot do bônus, antes de existir uma nova seleção. Depois o ledger volta2. O snapshot resultante exibe Support344 e oculta Monsters; o planner oferece344 e o gate Dual recusa por Support já gasto. Mão não diminui novamente e ledger permanece2.

A fonte disponível do adapter TW contém uma alteração temporária `used=Monster` quando vê single-TW Support, inclusive no tick de inicialização com seleção antiga. O trace do objeto efetivo confirma esse ramo; o problema está no wrapper TW, não numa mudança do Dual. Não é evidência de quota ilegal aprovada: o gate efetivamente impede o segundo Support.

Correção mínima a investigar: impedir que a adaptação temporária use seleção da ação anterior ao construir um novo snapshot; preservar sua atuação no plano novo/confirmado e a responsabilidade do Dual. Não desligar a validação nem adicionar previous_swap. Ainda é preciso verificar toda a fronteira de inicialização/materialização antes de aplicar uma condição definitiva. Os dois ranks R não garantem ordens distintas devido às dependências: a ordem efetiva está registrada por callback/símbolo/code hash.

## Reset, OFF/ON e savestate

A preferência OFF de TW (`restart:true` original) não equivale a OFF efetivo: o manager conserva o owner ativo até reinício. A fixture testa também OFF/ON efetivo declarado via bridge. O fluxo continua sem outra retirada da mão; B recusa load com TW residente OFF. Não se declara essa transição forçada acessível pela UI.

Caso34 conserva o **FAIL da hipótese “reset em voo termina em fase0”**. O core usa `reset_loaded`, conserva fase1, zera spin_id e o Encore limpa pending. Uma conclusão antiga não altera chosen/phase nem retorna a carta consumida; o teste de stale callback passa. A política B permanece recusando load. Não há prova de materialização duplicada, mas também não há garantia de cancelamento terminal/reinício completo da ação apenas chamando Reset. Isso deve ser resolvido como contrato de lifecycle, não zerando BSS por tentativa.

Essas oito falhas de auditoria são apresentadas como FAIL; não são uma regressão introduzida por esta branch, que não altera produção, nem uma declaração de que todo reset do jogo está quebrado. O reset foi chamado artificialmente em voo para testar sua fronteira. Novo processo, novo duelo e load completo são operações distintas; não inventamos persistência ou restauração.

## Evoluções: alvo correto, recurso ausente

O caso39 usa os callbacks reais de snapshot/evolution:41→433 e72→67 aparecem nos targets antes da resolução. Não foi criada regra nova para Egg. `transform_card` retorna antes de alterar a instância ao consultar **`disc_file_start("\\DATA\\WA_MRG.MRG;1") == -1`**. O ramo foi confrontado com o objeto exato: sem o start do arquivo não há leitura de asset/cópia de card data. IDs físicos41/72 permanecem, com identidades preservadas.

Isso é **NOT_REACHED por recurso legítimo ausente**, não prova de evolução concluída nem motivo para simular dados proprietários. A alternativa mínima é executar com a imagem legítima privada e backend disponível; não reconstruir o conteúdo desse arquivo.

Os controles iniciais do caso38 conservam16 checks FAIL: oito objetivos de transformação não atingidos e oito expectativas incorretas de que transformação deveria chamar Setup duas vezes. A inspeção mostrou que o core transforma inline, não por Setup. O oracle de contagem foi corrigido no código atual para zero chamadas extras; o caso39 registra a fronteira de IO sem alegar êxito. Esses controles não são16 bugs funcionais. Permanecem separados da matriz principal para não apagar a investigação ou transformar ausência de recurso em PASS.

Guardian, todas as transformações/estatísticas/3D e animações completas não estão qualificados. A remoção simples dos Fiends foi observada nos records pelo core, com primitivas nativas/renderer parcialmente modeladas. Nenhuma dessas evidências certifica gameplay Windows.

## Matrizes e regressões

[Principal](hal-9-support/primary/MATRIZ.csv), [resultados](hal-9-support/RESULTADOS.json), [trace bônus/reset](hal-9-support/bonus-reset-trace/MATRIZ.csv), [assets](hal-9-support/asset-boundary/MATRIZ.csv), [controles de evolução](hal-9-support/evolution-goal-failures/MATRIZ.csv), [regressão da coleção](hal-9-support/collection/MATRIZ.csv).

|Grupo|Processos|Checks|Classificações de processo|
|---|---:|---|---|
|Principal|144|1.432 PASS /8 FAIL|82 PASS /24 REFUSED /30 NOT_REACHED /8 FAIL|
|Trace adicional bônus/reset|16|172 PASS /8 FAIL|4 PASS /4 NOT_REACHED /8 FAIL|
|Objetivos iniciais de evolução|8|76 PASS /16 FAIL|8 FAIL de objetivos/oracle, descritos acima|
|Fronteira de asset corrigida|8|84 PASS|8 NOT_REACHED de transformação completa|
|Coleção sem TW, Field/H/D/prioridades, fence real|16|164 PASS|16 PASS|

Não somar repetições diagnósticas como aumento de cobertura independente. Todos completaram o harness, sem processos INCOMPLETE nas matrizes publicadas. As falhas de reset/evolução permanecem visíveis. Logs publicados compactam apenas repetições textuais exatas após três ocorrências, com SHA e contagem dos originais em [RAW_LOGS](hal-9-support/RAW_LOGS.json). [Reprodução](../tests/hal9-support/README.md).

O fence agora identifica seu objeto por símbolo; não presume o último objeto carregado. A primeira tentativa com COMMON foi recusada pelo loader, e outra revisão do stub gráfico foi resolvida antes de Settings_Load, causando NULL de teste; ambas foram corrigidas no harness, sem edição de callbacks reais nem omissão de uma falha funcional de MOD. Os logs intermediários integrais continuam no workspace.

## Prontidão e próximos passos

**Support está demonstrado no motor parcial disponível, com RD OFF, nas condições controladas descritas. A coleção completa não está pronta para admitir TW.** Bloqueios internos: handoff de seleção com RD; seleção antiga/quota temporária durante snapshot de bônus; semântica de reset em voo. Bloqueios externos: asset legítimo para transformar e backend Windows para UI/renderer/duelo completo. O build/detectores do PR31 não substituem isso.

O próximo ajuste deve tratar o ponto exato de handoff/snapshot no código próprio, com testes das identidades e categorias já registrados. Não é necessário repetir os detectores. A correção de bônus pode ser localizada no adapter TW disponível; não exige editar Dual. A arbitragem com RD precisa de contrato explícito antes de qualquer cancelamento/substituição de pending. Nenhuma dessas propostas foi promovida como solução definitiva.

B do PR30/31 permanece idêntica e independente do guard; A continua desabilitada. Não certificamos states antigos. O core0.23.1 continua binário fixado, sem recompilação integral. Nenhuma mudança técnica tornou Wine/QEMU PE ou Actions suspenso utilizáveis nesta sessão; não repetimos tentativas idênticas. Toda automação Windows continua autorizada quando houver recursos legítimos. Somente gameplay pessoal fica para o final; não se pede teste manual intermediário.

Publicação: [branch](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/test/hal-9-time-magic-support), [verificação das branches](hal-9-support/PUBLICATION.json), [registros Linear](hal-9-support/LINEAR_UPDATES.json). HAL-9 e HAL-20 receberam atualização de progresso/bloqueios, sem fechamento. A implementação nova é `1f4b3146515` sobre PR31; o diff contra integração inclui a coleção herdada e não significa merge dos PRs anteriores.
