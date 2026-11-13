# HAL-21 — candidata de correção F1: Revogar leases no OFF efetivo

**2026-11-13 · Preparada para revisão, sem merge ou versão final.** A política Revogar foi autorizada explicitamente pelo usuário para esta branch independente. O Fusion Effects do PR #12 foi incorporado à base oficial e recebeu **exatamente o patch aprovado do PR #21**, sem outros ajustes funcionais.

**F1 eliminada nos cenários exercitados:** depois do callback real `applied(0)`, Encore, origin-unlock e origin-effects retornam 0; forecast permanece neutro. A candidata passou **16 grupos da suíte original, 113 verificações nomeadas com manager real/motor parcial, 2.652 assertions ON e 214 de caracterização OFF**. Não houve regressão ON observada. Isso não certifica gameplay Windows nem todos os caminhos do motor completo.

## Origem, autorização e branch

- [HAL-21 no Linear](https://linear.app/half-programer/issue/HAL-21), consultada; não encerrada automaticamente. A descrição histórica da issue ainda registra a fase anterior à autorização desta candidata.
- Base exclusiva: `integration/hal-20-v0.3.1-preview.2`, **`d649647c159c91d615c07d6960928a37e270b461`**. SDK v0.3.1-preview.2 / API24 do oficial **`aceac3782902746f35ddb0148809450933327cb3`**.
- Fonte do Fusion: [PR #12](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/12), commit **`907ed98cfb763a96f5aa1ad6369f809cceca5f1c`**, [relatório original](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/907ed98cfb763a96f5aa1ad6369f809cceca5f1c/notes/HAL20_FUSION_EFFECTS_PREVIEW2.md).
- Evidência de integração: [PR #14](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/14). F1 original independente de AI/Dual; os conflitos de hooks da integração permanecem tarefas separadas.
- Patch e testes: [PR #21](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/21), commit **`f380d33bee48ce21c8a3c71c610d9c622f7cdb83`**, [relatório da PoC](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/f380d33bee48ce21c8a3c71c610d9c622f7cdb83/notes/HAL21_FUSION_OFF_PROVIDERS_POC.md), `experiments/hal21/revoke.patch`.
- Nova branch: [`fix/hal-21-fusion-off-revoke-preview2`](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/fix/hal-21-fusion-off-revoke-preview2). PR desta candidata: [#22 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/22).

Foram lidos AGENTS.md, HAL-21, inventário e relatórios #12/#14/#21. A autorização escolhe Revogar para preparação/testes; não autoriza merge, release, nova regra de efeito ou promoção à integração. Não foi incorporada a PoC de hooks #20.

## Mudança funcional exata

Somente os três arquivos de produção permitidos diferem do PR #12:

| Arquivo | Mudança |
|---|---|
| `mod.c` | Declara `aux_owner_applied`; Encore retorna 0 com owner efetivamente OFF. |
| `fusionist_runtime.h` | A consulta compartilhada de lease retorna 0 com owner OFF. Cobre os dois contratos públicos de origem. |
| `owner_f.h` | Helper `ea_applied && aux_host`; `ea_apply(0)` limpa somente `aux_unlocks`; forecast reutiliza o mesmo estado aplicado. |

A aplicação foi literal, sem adaptação adicional ao patch. A comparação de fontes exige igualdade de todos os C/H de produção ao **PR12 + patch aprovado**; resultado PASS. Os dois C/23 H originais mantêm o mesmo inventário. Os 22 C/H que não pertencem aos três arquivos acima são byte a byte iguais; headers públicos e ABI não mudaram.

`aux_unlock_serial` não é zerado no OFF. Nenhum reset geral foi introduzido. Registros de providers, catálogo de extensões, marcas Fusion/Metal, estados independentes, configurações, receitas e decisões estratégicas permanecem. `register_provider`/unregister e cleanup explícito `revoke` continuam utilizáveis OFF. Não se concede uma lease a outra identidade por compartilhar card ID.

ON → OFF encerra grants antigos. ON posterior não os recupera; um novo bind legítimo ganha serial novo (**77 → 78** no teste). Encore ON volta a consultar a presença atual do Musician King, conforme a regra original. LP, cartas geradas ou outras ações já resolvidas não são revertidos por esta correção.

Não foi usado `host->applied()` como substituto do callback: nesta base ele lê a preferência `enabled`. Com `restart:true`, essa preferência pode ser OFF enquanto o MOD continua ativo. A candidata mantém os providers ON durante essa espera, até uma remoção efetiva. `Mods_SetEnabled`, `Mods_Apply` e reload confirmam isso. Um processo novo com preferência OFF não inicializa os providers. **Manifest, `restart:true`, versão, 43 settings e defaults permanecem idênticos.**

## Reprodução F1 antes/depois

Mesma fixture original do #12: record 5 face-up Musician King 495, identidade própria e lease serial 77 semeadas; callback `applied(0)`; consultas aos quatro providers. Isso isola a API e não representa uma grant obtida em duelo completo.

| Artefato | Encore | Lease | Extended | Forecast | Status do contrato |
|---|---:|---:|---:|---:|---|
| Original API18 recebido | 1 | 77 | 77 | 0 | FAIL original, código 2 |
| Original API24 do #12 | 1 | 77 | 77 | 0 | FAIL original, código 2 |
| Revogar reconstruído do #21 | 0 | 0 | 0 | 0 | PASS |
| **Objeto desta candidata** | **0** | **0** | **0** | **0** | **PASS, código 0** |

Hashes originais foram conferidos: API18 `1d46c5f812759de21564e20ab88eddc79a268534c6e1055b7051b3dfefd88821`; API24 `7e50372742ebd04090a907aa222f90d339d3122edf2fb06a589e297926c4f6c8`. Os FAIL históricos e os riscos da alternativa Ocultar continuam nos resultados como controles explícitos; não foram apagados ou convertidos em PASS.

Logs: [original API18](hal-21/fix/contracts/original-api18-F1.log), [original API24](hal-21/fix/contracts/original-api24-F1.log), [candidata OFF](hal-21/fix/contracts/candidate-F1.log), [candidata ON](hal-21/fix/contracts/candidate-ON-characterization.log).

## Build, hashes e comparação com referências

```sh
MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang \
  python /caminho/sdk/tools/build_mod.py mods/fusion-auxiliary-balance --target i386
```

Build inicial compilou `mod.c` e `png.c`; a reconferência do builder marcou o mesmo artefato como atualizado, sem recompilar. SDK oficial, Clang **23.1.2 / llvm-mingw 20260922**, sem flags próprios nem objetos antigos no link. As ferramentas/headers/exports por SHA foram reconferidos contra a proveniência do #12. API24 foi medida em `MemoriesModInit` pelo loader real. O `.o` é ELF32 relocável EM_386, formato de MOD para Windows/Linux i386; não é executável Windows.

| Artefato | Bytes | SHA-256 | Hash de código do loader |
|---|---:|---|---|
| API24 original PR #12 | 378508 | `7e50372742ebd04090a907aa222f90d339d3122edf2fb06a589e297926c4f6c8` | `e6f40fd4` |
| Ocultar PR #21 | 381024 | `1236eb5408b22c5fefecea748a5b9af6f925ef4fab1409ef869b36faff7571dd` | `8f442fbc` |
| Revogar PR #21 | 380268 | `3d85b23482682deb012b2d32458f077da0b4cfd4a3b3ee92d79f9bdae00bdcd9` | `afb75202` |
| **Candidata desta branch** | **380288** | **`384e11eeeae32412e23afbbbd9ebc308761674b8a2181238fd60e17e315b865c`** | **`afb75202`** |

**Todas as seções alocáveis são idênticas às de Revogar do #21**, com mesmo hash do loader. A diferença bruta de 20 bytes está restrita a `.debug_str_offsets`, `.debug_str`, `.debug_line`, `.debug_line_str`, que contêm caminhos de compilação. Não há diferença de código/dados carregados em relação à candidata Revogar aprovada. Ocultar tem sua lógica de retenção/revalidação diferente e foi usada somente como controle histórico, sem pacote alternativo instalável nesta branch.

[ABI.json](hal-21/fix/ABI.json): **131 imports idênticos**, todos presentes no SDK; exports públicos continuam `MemoriesModInit` e `Png_Decode`. Quatro providers com mesmos records/versões, 33 hooks e quatro subscriptions. Não há provider novo, alteração de assinatura, BSS de persistência ou nova dependência obrigatória.

[COMPARACAO.json](hal-21/fix/COMPARACAO.json): source patch exato, inventário, manifest e assets PASS. Manifest SHA `e8ff14b500189b1e6bca253677756535c5f8e3c00b27d2904f3640d870fadd7a`, 59 cards, 31 fusions, 43 settings, 34 PNGs e `coin.txt` preservados. Textos/receitas/efeitos/valores permanecem no manifest original; os demais C/H são conferidos por hash.

Proveniência atual: [SDK-PROVENANCE.json](../mods/fusion-auxiliary-balance/SDK-PROVENANCE.json) e [cópia nos resultados](hal-21/fix/PROVENIENCIA.json). A proveniência anterior do #12 foi preservada literalmente em `SDK-PROVENANCE.hal20-pr12.json`; a recebida preview.1 continua histórica. Apenas o objeto novo está na pasta instalável. Nenhum binário antigo foi usado como substituto do build.

## Testes executados

[Como reproduzir](../tests/hal21/README.md) · [Matriz CSV](hal-21/fix/MATRIZ.csv) · [Resultados do manager](hal-21/fix/contracts/RESULTADOS.json) · [Suíte original completa](hal-21/fix/ORIGINAL_SUITE.json) · [Paridade ON](hal-21/fix/on/RESULTADOS.json).

| Grupo | Resultado da candidata |
|---|---|
| Suíte original empacotada | **16 grupos PASS, 0 FAIL**, nove builds explícitos separados |
| Loader ON original | **2.652 assertions PASS**, ciclos de init/rejeições esperadas/reset/shutdown/contratos |
| Loader F1 OFF | **214 assertions de caracterização PASS**, tupla neutra |
| Manager Fusion → Ritual | **39 PASS / 0 FAIL** |
| Manager Ritual → Fusion | **39 PASS / 0 FAIL** |
| Manager Fusion sem Ritual | **35 PASS / 0 FAIL** |
| Fusion ausente e processo novo com preferência OFF | **2 PASS** independentes |
| Comparação ON baseline/hide/revoke/candidata | **14 grupos PASS por perfil**, Python/C ASan/UBSan |
| Patch, manifest, assets, seções alocáveis, ABI | **PASS** |

Os 113 checks nomeados do manager são 39+39+35, incluindo cinco checks novos repetidos por perfil. Não são 113 duelos distintos. As assertions internas incluem infraestrutura e engine modelada. Os 14 grupos ON incluem regras, Fusionist, Round02/03, receitas, títulos por instância, atributos, tween e **88 comparações forecast/executor por perfil**. Nenhuma assertion de receita/efeito original foi removida ou enfraquecida.

Os mesmos checks do #21 também foram reexecutados contra os controles: baseline 76 PASS/22 FAIL; Ocultar 92 PASS/6 FAIL; Revogar reconstruído 98 PASS/0 FAIL. A candidata empacotada passa os 98 existentes e os 15 adicionais. O runner novo retorna 0 pela candidata completa; os FAIL dos controles continuam visíveis no JSON/CSV, sem alegar que o baseline foi corrigido.

### Estado, identidades e escopo da revogação

- ON, OFF, ON→OFF→ON; grant antigo não volta; bind real novo incrementa serial; OFF repetido idempotente.
- Mesma carta/ponteiro muda de slot/controlador; cópias com mesmo ID e outro ponteiro; remoção; movimento campo/mão; face-down; saída/retorno do mesmo ponteiro durante OFF. Nenhum grant antigo é recuperado pela candidata.
- Cleanup explícito OFF não contamina outra cópia. Provider externo desativado e Ritual desligado durante OFF não restauram lease.
- Teste novo semeia marcas Fusion/Metal não zero, serial e registro externo; copia a **imagem inteira carregada** e entrega somente o callback OFF. Byte a byte, só `ea_applied`/`aux_unlocks` podem mudar. PASS, com serial 77 e estados independentes preservados.
- Outro teste conserva preferência/estado do manager ON e entrega callback OFF isoladamente: as respostas seguem o owner OFF, demonstrando que não dependem de `host->applied`. Isso é instrumentação do contrato, não fluxo normal de UI.
- Forecast OFF: `on/read/policy/pair` neutros, sem mutação da carta ou dos outputs de resultado; ON volta ao comportamento existente.
- Reset aplicado e inativo, novo duelo pelo hook real de cena, SAVE/reset/LOAD simulado. Não se inventa formato persistente: são visitados apenas os chunks realmente registrados. Não é certificação de save/load completo ou de savestates antigos.

O harness usa loader, hooks, registry, manager, settings, eventos e reset oficiais sem editar o core. `activate` e callback-only são expostos por uma ponte **exclusiva de testes**; as transições que contornam `restart:true` são rotuladas. Mão/campo/identidades/motor e wheel são parcialmente modelados. **Nenhum duelo Windows foi executado.**

### Mudanças somente nos testes/documentos

- `tests/test_fusionist_runtime.c`: helper do owner sempre-ON no mock que não inclui `owner_f.h`; validação OFF ocorre no objeto real. Duas linhas adicionadas, sem mudar assertions de fusões/valores.
- `tests/run_preview2.py`: docstring atualizada; o contrato OFF passa a ser regressão obrigatória, como a lógica do runner já exigia. Nenhum FAIL foi mascarado.
- `tests/hal21/`: reaproveita #21, adiciona perfil do pacote real e cinco checks de escopo, comparador e documentação. Patches de referências em `reference/` são históricos e não são novos MODs.
- README/proveniência atuais atualizados; metadados anteriores do #12 preservados. Essas alterações não participam do código do MOD.

Não houve incompatibilidade técnica nova ou falha da candidata que exigisse desviar do patch aprovado. Warnings de offsets dos headers oficiais continuam nos logs GCC das fixtures, como no #12/#21. LeakSanitizer está explicitamente desativado; os C nativos usam ASan/UBSan, o loader i386 usa QEMU/sysroot sem esses sanitizadores.

## Consumidores reais e limitações conhecidas

**Ritual Effects PR #11:** objeto real `c95cbf3c9e0396b6b42b6226aa7f42ad3e1036f49e1f19a624a67311c13374b1`, sem alterações. `borrowed_reconcile` real conserva a marca Shield 362 com lease ON e aposenta marca/máscara/serial futuro após OFF. O `effect_intelligence_v1` real valida 704 e sua desativação é respeitada. As funções de consulta Encore/lease literais usam `Mods_Find` real nas duas ordens e com providers ausentes/desativados. Grants e borrowed origins desses casos são semeados pela fixture; não representam aquisição completa por gameplay. Ações já resolvidas não são revertidas.

**Time Wizard:** ponte disponível compilada apenas localmente com núcleo/wheel modelados, sem publicação desses fontes/executável. Nova tentativa OFF fica sem benefício (`fresh_attempt_retry_OFF=0`). Tentativa iniciada ON continua usando o cache depois do OFF forçado (`retry_after_OFF=1`), exatamente como no #21. Esta correção de producer não cancela tentativa já em voo e não altera Time Wizard. Núcleo completo, assinaturas da ponte Dual e execução Windows continuam bloqueios separados.

**AI HARD MODE e Dual Play:** permanecem integralmente inalterados; a correção não registra/reordena hooks nem altera qualquer escolha estratégica/quota. A matriz integrada de gameplay AI OFF/ON × Dual OFF/ON permanece pendente, assim como controles nativos de dois humanos. PR #20 e conflitos HAL-22/23/24 não foram incorporados ou declarados resolvidos.

## Publicação e revisão pendente

Esta branch contém uma única versão instalável do ID `fusion-auxiliary-balance`, completa com fontes autorizados, manifest/assets originais e objeto i386 novo. [NOTICES-HAL20.md](../mods/fusion-auxiliary-balance/NOTICES-HAL20.md) conserva créditos e a permissão informada pelo usuário para a coin-lib/frames de PeterIron. Não houve assets novos, imagem do jogo, saves pessoais, arquivos privados de Dual ou Time Wizard, alteração de workflows/permissões, merge ou release.

O pacote está pronto **para revisão e futura validação integrada**. A política foi autorizada somente como candidata. Antes de promover: revisão do diff dos três arquivos, gameplay Windows i386 com grants reais de Egotist/instâncias/controle/remoção/retorno, cargas e efeitos já resolvidos, restart/Apply/UI, roletas em voo, SAVE/LOAD real e matriz com Ritual/AI/Dual. Não presumir compatibilidade de savestates antigos a partir do hash de código igual à PoC.

Publicado no [PR #22 em rascunho](https://github.com/otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled/pull/22), destino `integration/hal-20-v0.3.1-preview.2`. [Índice de publicação](hal-21/fix/PUBLICACAO.json) e [26 heads anteriores preservadas](hal-21/fix/REFS_PRESERVADAS.json). Push normal, sem force.
