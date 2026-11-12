# HAL-20 — Fusion Effects beta.44-effect-ai / SDK API 24

## Resultado

**Build i386 concluído com o SDK oficial v0.3.1-preview.2, sem qualquer alteração nos dois C, 23 headers de produção, manifest, receitas, valores ou decisões da IA.** A suíte isolada registra **15 grupos aprovados e uma falha preexistente de desativação dos contratos**, mantida sem correção. O pacote é um candidato para revisão, com pendência explícita; não está aprovado para integração completa ou lançamento.

Base: `integration/hal-20-v0.3.1-preview.2` em `d649647c159c91d615c07d6960928a37e270b461`, fonte oficial `aceac3782902746f35ddb0148809450933327cb3`. Branch independente `mods/hal-20-fusion-effects-preview2`. Pasta `mods/fusion-auxiliary-balance/`, ID/library `fusion-auxiliary-balance`. Nenhum outro MOD, core, workflow, master ou integração foi modificado. Não houve merge.

Foram lidos AGENTS.md, o [inventário HAL-20/PR #7](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/docs/hal-20-inventario-plano/notes/HAL20_INVENTARIO_E_PLANO.md), a migração da base [PR #8](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/8), Melhoria/Surrender [PR #9](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/9), Dual [PR #10](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/10) e Ritual Effects [PR #11](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/11). Os quatro PRs de implementação/análise estavam em rascunho e sem merge. Documentos e prompts históricos do pacote foram tratados como evidência, sem ampliar a autorização atual.

Evidências: [resultados e comandos](hal-20/fusion-effects/RESULTADOS.json), [93 arquivos recebidos e hashes](hal-20/fusion-effects/INSUMOS_SHA256.json), [nova proveniência](../mods/fusion-auxiliary-balance/SDK-PROVENANCE.json), [runner](../mods/fusion-auxiliary-balance/tests/run_preview2.py) e [orientação do candidato](../mods/fusion-auxiliary-balance/README-PREVIEW2.md).

## 1. Inventário, identidade e redistribuição

Pacote `BalanceamentoFusoesEfeitosAuxiliares` de `mods att.zip`, SHA-256 `745f6fbbd5f3c9451ca37735626abbbc4c6704bc5326ab3691be33d86b66281b`. A árvore contém 93 arquivos, incluindo dois objetos antigos e quatro arquivos de .pytest_cache. Manifest: versão `0.1.0-beta.44-effect-ai`, autor Otavio, enabled true, restart true, slus_01411, min_api 15; **43 configurações, 59 entradas cards e 31 fusions**. Sem requires/after declarados. Há 28 receitas explícitas e três remoções de resultados retail, conforme o teste do manifest. As chaves, defaults, ordem, condições e todos os dados permanecem iguais ao ZIP.

Assets: 34 PNGs (28 frames da moeda normal e seis ícones customizados de atributos) e um coin.txt; nenhum WAV próprio. Os textos estão no manifest e documentos recebidos, sem pasta de overlays de texto independente. As seis referências de tipos/ícones usam IDs 249–254 e dimensões 256×256, mantendo tamanho lógico de 16×16; nenhum monstro foi reclassificado por esta migração. As 58 entradas frame Orange recebidas foram preservadas, sem substituir o projeto independente de molduras TCG.

`battle_coin.h` credita PeterIron/coin-flip-lib 1.0. A permissão de redistribuição da moeda já foi confirmada pelo usuário na etapa Ritual Effects. Conferência adicional: os 28 frames deste pacote são byte a byte idênticos aos correspondentes daquele pacote autorizado. Crédito e fundamento registrados em [NOTICES-HAL20.md](../mods/fusion-auxiliary-balance/NOTICES-HAL20.md), sem inventar licença aberta ou termos adicionais. Os seis ícones são identificados nos testes/documentação recebidos como customizações fornecidas pelo usuário. Nenhum recurso adicional foi extraído do jogo e nenhum save/imagem de disco foi incluído.

## 2. Divergências dos objetos e proveniência

| Objeto recebido | SHA declarado no JSON antigo | SHA efetivo |
|---|---|---|
| fusion-auxiliary-balance.o | 2ca29e9f763d81b5bbb6b83476dc83f872ae26baab3638405502418c410683cb | 1d46c5f812759de21564e20ab88eddc79a268534c6e1055b7051b3dfefd88821 |
| fusion-auxiliary-balance.x86_64-windows.o | 7fb6b1898ad05412bcfd0e53f1acad6cc124d83589969957f2e69d4f4fed323f | 252baeae938481810b1d78a4523e08c0c7babbe4efea828e1bb638d8495b68a5 |

A divergência foi confirmada nos dois alvos. A proveniência recebida cita preview.1, commit abreviado 10ee7e5ecf, nota beta.43 e afirma binários idênticos à beta.42; o manifest é beta.44-effect-ai e sua descrição ainda menciona beta.43. O DWARF identifica GCC 13.3.0 no objeto i386 e Clang 19.1.7/zig-bootstrap no x64. O i386 recebido declara API **18**, medido em init com o loader. Esses fatos não vinculam os C atuais aos hashes declarados nem identificam a revisão exata que produziu cada objeto.

Metadados antigos incompletos/desatualizados são uma explicação possível; não foi comprovada a origem de cada diferença funcional. O comportamento OFF coincidente nos dois objetos, descrito abaixo, também não prova equivalência integral. Não se adotou o objeto antigo como referência incontestável das mecânicas aprovadas.

Os dois objetos originais continuam preservados no diretório de insumos local e identificados integralmente por hash neste relatório/JSON. Eles não entram no build e não são distribuídos junto da candidata. SDK-PROVENANCE.json foi renovado para o artefato novo; o JSON recebido foi conservado literalmente em SDK-PROVENANCE.preview1.recebido.json. BASE-BETA31-SHA256.txt e os demais documentos continuam evidências históricas, não certificação deste build.

## 3. Recompilação e diferenças verificadas

SDK oficial gerado da tag preview.2, com headers e exports API 24, Clang 23.1.2/llvm-mingw 20260922, `build_mod.py --target i386`. Não foram acrescentados flags próprios ou objetos pré-compilados ao link. O builder compilou `mod.c` e `png.c` da raiz; testes ficam em subpasta e não participam do MOD de produção.

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py /caminho/fusion-auxiliary-balance --target i386
```

Artefato: **378.508 bytes**, ELF32 relocável EM_386, SHA-256 **`7e50372742ebd04090a907aa222f90d339d3122edf2fb06a589e297926c4f6c8`**, hash do loader **e6f40fd4**. É o formato lido pelo jogo Windows i386, também suportado pelo Linux i386; não é uma DLL. Não houve build/runtime x64, ARM ou macOS.

O objeto i386 antigo tem 536.660 bytes, .text 82.740 e .bss 56.900; o novo tem .text 90.976 e .bss 56.385. Ambos exportam MemoriesModInit/Png_Decode e têm 131 imports diretos. No novo saem os thunks ecx/edx/esi e entram memcmp/memcpy/memset; o thunk eax oficial permanece. Diferenças de compilador, otimização, debug e headers impedem atribuir redução do arquivo a ganho de performance ou equivalência semântica.

**Todos os 131 imports novos existem nos exports i386 de Windows e Linux.** Nenhuma incompatibilidade comprovada de API/tipo exigiu patch de produção. O novo init declara API 24 por MEMORIES_MOD_API; o min_api 15 original foi preservado no manifest, mas não autoriza instalar este binário em host antigo. O gerenciador também verifica a API declarada no objeto.

A nova proveniência identifica commit oficial, compilador/builder por SHA-256, digest dos headers, exports, 25 C/H e manifest. Conferência de preservação: **84 dos 93 arquivos recebidos permanecem idênticos**, quatro têm conteúdo novo (objeto i386, proveniência e dois testes), e cinco foram omitidos (objeto x64 antigo e quatro caches pytest). Arquivos adicionais são testes/documentação/proveniência histórica. Todos os 34 PNGs e coin.txt são idênticos ao pacote autorizado.

## 4. Testes e alcance real

```sh
MEMORIES_I386_CC=/caminho/gcc-com-sysroot \
MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot' \
MEMORIES_TEST_RESULTS=/tmp/fusion-results.json \
  python mods/fusion-auxiliary-balance/tests/run_preview2.py
```

O runner exige Python/Pillow e GCC com suporte i386. Nesta sessão, o kernel não executa ELF32 diretamente; foi utilizado QEMU i386 5.2/sysroot Debian 11. As fixtures de lógica são nativas e as do loader executam o objeto i386 real. Os componentes de loader/thunks vêm da base oficial. Nenhuma imagem do jogo, Wine ou execução Windows foi usada.

| Teste | Resultado / cobertura |
|---|---|
| Suíte final | **15 grupos PASS e um FAIL**, zero skips; nove passos de compilação explícitos separados da contagem |
| Loader novo | **2.652 assertions**, dez ciclos aceitos com providers/reset/OFF/ON/shutdown e duas recusas esperadas (assets ausentes e serviço provide ausente); host simulado |
| Registros do MOD | 33 hooks, quatro subscriptions e quatro providers observados; não equivale a despachar a cadeia completa de hooks no jogo |
| OFF dos contratos | FAIL/código 2 no objeto novo **e no recebido**; observação detalhada abaixo. A suíte termina não zero intencionalmente |
| Fusionist/encadeamento | Runtime source-backed: ordem dos materiais, origem real, Metalmorph versus substituto virtual, duas cópias/IDs, Egotist, finalização única, Tower no-equips/no-fusions, escopo recursivo da IA, ranking e empates distintos |
| Round02/03 | Fixtures de todos os 16 efeitos Round02 e 22 Round03, condições/origem, instâncias, LP, equipamentos, restauração e regras; 17 receitas simétricas Round03 |
| Previsão de efeitos | **88 comparações forecast/executor**, ambos os lados, ATK/DEF, cópia direta/fusionada. Verifica o modelo nos cenários existentes, não o planner completo em duelo |
| Títulos/visuais | Helpers de título por instância, cores/restauração e isolamento de seleção; tween de stats e wiring de render/descrições; não demonstra HD/3D real |
| Assets | 34 PNGs com CRC/estrutura e decode Pillow; 28 frames runtime pelo Png_Decode de produção sob ASan/UBSan; seis ícones e suas referências/dimensões; 59 entradas cards com texto íntegro |

As oito fixtures C independentes e run_fusionist (que compila sua própria fixture C) usam ASan/UBSan. O novo teste de PNG também usa sanitizadores. LeakSanitizer foi explicitamente desligado. O loader i386 não foi executado sob sanitizadores; o restante da suíte inclui asserts estáticos, modelos e estruturas simplificadas. Não afirmar que todos os caminhos de produção ou todos os efeitos foram executados dentro do motor real.

No objeto novo, foram exercitados os callbacks publicados e hooks de ABI pública: elegibilidade Encore por lado/face-down, leases por identidade e mudança de controle, revogação e remoção sem contaminar outra cópia, registro/substituição/remoção de providers externos (limite 16), Ritual ausente/OFF/ABI incompatível, forecast opcional, Harpie+Egotist, transformação Metalmorph configurada e reset. O hook de reconstrução de carta gerada foi chamado para os dois lados: deck_index sintético 255 vira apenas fonte temporária 0 no stub nativo, preservando Zombie Warrior #30, data/instância, stats/modificadores, posição e objeto reconstruído. As miniaturas dependentes do disco não foram verificadas. A criação completa de cartas é coberta pelas fixtures source-backed Round02/03; não por um duelo real do ELF.

Os hooks usam funções/dados nativos simulados e o catálogo sintético contém uma receita conhecida do manifest. Consultas source-backed não substituem a aplicação real de todos os dados do manifest pela engine. Os testes de transformações/instâncias não incluem rendering, áudio ou 3D Monsters ativo.

### Falhas de teste e instrumentação

Na primeira execução, dois testes recebidos ainda esperavam beta.43/41 opções e 38 novas settings em relação à base beta31. Foram atualizados **somente tests/test_manifest.py e test_round2_manifest.py** para beta.44/43 opções e as duas chaves novas recebidas effect_ai/effect_ai_trace. Seus asserts de receitas/efeitos históricos permanecem. O MOD e seu manifest não foram alterados para satisfazer esses testes.

A primeira chamada de test_rules.c omitiu o include path do checkout: erro do comando de teste, corrigido no runner. Uma primeira fixture do loader não oferecia a receita CardRules_Fusion necessária ao cenário Metalmorph; o stub foi completado com a receita já declarada no manifest. Não foi uma falha do MOD. Os asserts de offset dos headers oficiais geram warnings com GCC na fixture, sem impedir sua compilação. A suíte final está registrada integralmente, inclusive a falha OFF; não foi mascarada como PASS ou SKIP.

## 5. Achado confirmado F1 — providers ainda respondem após OFF (P1 para integração)

Reprodução determinística, disponível no mesmo loader com `--strict-off`:

1. Carregar/init e preencher uma instância face-up de Musician King #495 no slot 5 com identidade física própria.
2. Preparar uma lease de origem dessa mesma instância, serial 77, no estado privado da fixture. Isso isola a consulta da API; não pretende simular a animação Egotist.
3. Chamar applied(0) com host marcado inativo e consultar os quatro providers que continuam registrados.

```text
esperado pelo teste de isolamento OFF: encore=0 lease=0 extended=0 forecast=0
observado no i386 recebido (API 18, hash 7a3178de): encore=1 lease=77 extended=77 forecast=0
observado no rebuild (API 24, hash e6f40fd4):      encore=1 lease=77 extended=77 forecast=0
```

O teste executa os dois objetos, não só uma reimplementação. Ambas as execuções devolvem código 2 e 214 assertions de caracterização; a falha é a diferença entre o resultado esperado de indisponibilidade e as consultas retornadas. O comportamento já existia antes da migração. Não foi reproduzido um duelo completo de Time Wizard/Ritual após OFF.

Evidência de causa: owner_f.h:ea_apply atualiza apenas ea_applied; Encore em mod.c:aux_encore_eligible e lease em fusionist_runtime.h:aux_unlock_lease não consultam essa ativação. origin-effects-v1 reutiliza a mesma lease. Na base, Mods_Find pode devolver providers de módulos desativados; o caminho de desativação em mods.c:apply chama applied(0), sem chamar reset automaticamente. Não é correto presumir que o host limpe essas respostas.

Impacto sustentado pelo código: Ritual Effects performance_chaos.h:ritual_encore_eligible e Time Wizard time_wizard_encore.c:musician_present consultam Encore e não verificam a ativação do owner separadamente. Ritual também consulta origin-unlock-v1 para elegibilidade emprestada. Isso pode manter benefícios indevidos de um módulo OFF. Falta validar o impacto exato em gameplay e a influência de restart:true no fluxo real de aplicação; o teste isola a semântica dos callbacks, não a UI do gerenciador.

**Proposta separada, não implementada:** condicionar Encore e a consulta compartilhada de lease ao estado ativo do owner, preservando ABI, receitas e todas as respostas quando ON. Decidir explicitamente se OFF apenas oculta ou também revoga leases antes de reativação; essa decisão não deve ser tomada pela portabilidade. Depois validar os dois consumidores, ON/OFF/ON, mudança de controle, reentrada e ausência de provider. Não substituir interfaces públicas, modificar consumidores ou mudar a lógica de produção sem autorização específica.

## 6. Contratos e dependências opcionais

| Contrato | Resultado isolado |
|---|---|
| encore-v1 | API 1; lados inválidos/sem Musician/face-down não elegíveis; face-up no lado certo elegível sem exigir fusão, conforme fonte. **OFF não bloqueia**, ver F1 |
| origin-unlock-v1 | API 1; consulta segue card ID + data e mudança de slot/controlador; cópia com outra identidade não herda; revoke/reset/remoção limpam a lease. **OFF não bloqueia**, ver F1 |
| origin-effects-v1 | ABI 1; owner não vazio e até 63 bytes, máximo 16 registros, substituição e unregister; Ritual intelligence ausente/desativada/ABI errada e provider externo OFF não liberam o alvo. A consulta de lease sofre F1 |
| effect_forecast_v2 | ABI 2, on/pair respeitam OFF e settings; consultas não devem armar cartas ou consumir RNG de gameplay. Conexão com Ritual ausente/OFF/ABI errada tem fallback. API de previsão não é nova decisão estratégica criada nesta etapa |

Esses contratos são opcionais entre MODs, mas o próprio Fusion init exige que o host ofereça provide e aceite seus registros; sem o serviço, a fixture confirma recusa. Isso não exige instalar Ritual Effects ou outro parceiro. A fixture libera os recursos de moeda após suas tentativas; não comprova rollback/leak-free do gerenciador completo. O shutdown de produção libera a moeda, enquanto o gerenciador é responsável pelos registros/vida do módulo. Nenhuma dependência obrigatória nova foi introduzida.

Ritual Effects também pode fornecer position-rules-v1 ao helper r2_can_defend, que valida ABI/callback e usa fallback quando ausente. Esse contato foi inspecionado estaticamente; os providers reais dos dois MODs não foram carregados juntos nesta etapa.

## 7. Reset, save/load e compatibilidade futura

Não há registro de estado nativo equivalente ao Dual Play. O teste simula um round-trip de marcas, chama reset e confirma que origens Fusion/Metal e leases deixam de valer. aux_reset limpa filas/instâncias, r2/r3 e origens. Isso conserva o comportamento recebido, não implementa persistência de efeitos, invocações ou cargas após load. Não tratar esses bytes privados como formato de save.

O hash do código mudou; a base inclui Mods_CodeHash na assinatura e recusa conjuntos incompatíveis. Savestates do objeto/SDK anterior não são presumidos compatíveis. Não foram abertos saves pessoais ou exercitados memory card/save completo do jogo.

Análise estática dos pares, sem importar ou alterar seus projetos:

- **Ritual Effects/PR #11:** ambos envolvem mão/campo/batalha, stats, remoção e finalização. Forecast usa modelo/ABI 2 compartilhados; Ritual cede o caminho nativo de previsão quando Fusion está ativo, mas suas políticas específicas de efeitos permanecem próprias. Testar aplicação única de bônus, proteções, posição DEF e contratos OFF (F1), Egotist/leases e Ritual verdadeiro. Não duplicar efeitos no planner.
- **Melhoria/Surrender/PR #9:** HandActions é hook comum; Melhoria envolve DeactivateRecord/Populate, enquanto Fusion envolve RemoveFromField e reconstrução por func_80024D34. Seleção de tributos não deve ser confundida com destruição nem criar origem Fusion. Select/Surrender pode ocorrer com moeda/efeito pendente; testar confirmação/cancelamento e novo duelo. Nenhum código de rituais foi incorporado.
- **AI HARD MODE oficial:** Fusion envolve AiScript_FindBestAttack/PlayFieldCard/LoadGuardianStarChoice/FindKiller/CalcCardPower, FindBestCombo e EvaluateFusion, além de Duel_CheckFusion. AI Hard pode usar planejador próprio em vez da VM; a existência desses hooks não prova que todos os caminhos receberão a previsão. Manter suas estratégias e dificuldade; validar recursão/origem, Fusionist sem RNG em preview, Tower/equip e execução final.
- **Dual Monster+Support:** compartilham HandActions, FieldActions, placement e turnos. Testar fusões só Monster versus mistura/equip, quotas, aliases da mão extra, cancelamento, fim de turno e instâncias geradas. Fusion não envolve DrawPhase, portanto não muda diretamente os bytes do Dual ou o alvo da assinatura Time Wizard. Isso não prova convivência completa.
- **Time Wizard:** Encore sofre F1 quando o owner fica OFF; X/Quadrado pode compartilhar o hand controller. Fusion limita sua adaptação Square a Sparks, Hinotama e Red Medicine; o caso híbrido do Time Wizard continua no seu próprio MOD. Validar ordem de hooks, papel da ação, quotas e tentativa única da roleta; não alterar esse projeto para contornar problema não reproduzido.

Matriz ainda pendente de gameplay: AI OFF/Dual OFF, AI ON/Dual OFF, AI OFF/Dual ON, ambos ON; pares com Ritual/Melhoria e depois Time Wizard. Repetir modo nativo de dois humanos com controles independentes, separado de Dual Monster+Support. O adapter AI 0.1.1 ausente e o núcleo-fonte parcial do Time Wizard continuam lacunas próprias da HAL-20.

## 8. Limitações e próximos passos

Não houve imagem do jogo, Windows/Wine, duelo completo, UI/controles físicos, áudio, 3D/HD real, encaixe visual de todas as descrições, planner integral ou save/load completo. Não houve medição de FPS/performance. Os resultados são locais e a suíte contém uma falha explícita; não apresentá-los como checks verdes do GitHub Actions.

1. Revisar a reprodução F1 e autorizar separadamente a política de desativação dos providers antes da correção. Nenhuma alteração funcional foi aplicada aqui.
2. Reproduzir o build/suíte publicados e validar o mesmo hash no Windows i386, mantendo uma instalação por ID e começando duelo novo. Preservar restart/configurações originais.
3. Validar Fusionist, Egotist, Metalmorph, 38 efeitos Round02/03 e efeitos anteriores, criação/remoção/reconstrução e títulos no motor real; depois executar a matriz integrada com ordem de carga registrada.
4. Não promover master/integração nem mesclar este PR automaticamente. A nova proveniência fecha a rastreabilidade do candidato, sem resolver retroativamente a origem dos hashes antigos.

Rastreabilidade: [HAL-20](https://linear.app/half-programer/issue/HAL-20/base-sincronizar-fork-e-versoes-atuais-dos-mods-com-upstream-v031), HAL-8/9/10/18 para pares e regressões. Sem alterações de status de issues, workflows ou permissões.
