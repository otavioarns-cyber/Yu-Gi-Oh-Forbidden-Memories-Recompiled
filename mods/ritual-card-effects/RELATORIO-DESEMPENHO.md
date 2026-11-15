# Efeitos de Rituais v0.78.40 — desempenho e validação

Atualização de 2026-10-11 sobre o pacote v0.78.39 recebido. Alvo confirmado pelo usuário: **Yu-Gi-Oh! Forbidden Memories Recompiled v0.3.0, Windows**. A biblioteca foi compilada para o pacote oficial Windows de 32 bits, cujo executável e SDK foram verificados. As melhorias reduzem trabalho de CPU em callbacks do MOD; ainda não há medição de FPS no jogo.

## Mudanças implementadas

| Trecho | Redução do trabalho | Preservação verificada |
| --- | --- | --- |
| Prévias Sengenjin | Uma consulta pública síncrona compartilhada entre os dois atributos; cadeia pública 3 → 2 chamadas por prévia elegível. | Cópias independentes para ATK/DEF, ordem das chamadas restantes, clamps, debuffs, guard e Guardian Star. |
| Auras BLS/Dokurorider | Fontes validadas uma vez por lado em cada reconciliação. | Identidade, ocupação/presença em preparação, exclusão do próprio registro, destinatários e filas. |
| Assinatura do overlay | Retorno imediato quando só contadores nativos estão vivos: 32 → 0 bytes de hash nesse caso. | Estados que desenham no overlay continuam sendo incluídos na assinatura; os contadores usam seus packets nativos. |
| Contadores nativos | Consulta do cursor somente para cartas candidatas; fixture de 30 registros, um candidato: 30 → 1 consultas. | Mesmos contadores, seleção, posição, sprites e flags. |
| Popup Chakra | Texto e largura calculados por valor real; fixture de valor fixo por 60 frames: 60 → 1 formatações. | Cada chamada mantém posição, brilho, comandos e duração. Valores alternados atualizam o cache. |
| API opcional `recipe_v1` | Lookup de descritores imutáveis; matching copia e limita só o papel solicitado. | Todas as 18 receitas publicadas, Settings, clones, categorias privadas, limites e exceção Chakra. |

Snapshots de atributos/fontes pertencem à chamada atual. O índice de receitas contém apenas definições imutáveis; stats e Settings permanecem atuais em cada consulta. Não há cache de gameplay entre frames, nem reaproveitamento de identidades durante as consultas temporárias de Mão/Deck.

## Medições locais

Microbenchmarks com funções/headers reais e callbacks de engine simulados, GCC `-O2`, mediana de sete execuções com ordem alternada. A lógica/receitas também passaram com headers da v0.3.0. A tabela usa os resultados registrados nessa rodada; os tempos variam conforme o host.

| Operação medida | Antes | Depois | Redução local |
| --- | ---: | ---: | ---: |
| BLS, reconciliação de campo denso | 100,11 ns/chamada | 44,24 ns/chamada | 55,8% |
| Dokurorider, reconciliação de campo denso | 156,55 ns/chamada | 56,66 ns/chamada | 63,8% |
| Matching do provider, receitas/papéis mistos | 15,973 ns/consulta | 9,049 ns/consulta | 43,3% |
| Assinatura com efeitos apenas nativos, 3 milhões de chamadas | 0,070463 s | 0,010207 s | 85,5% |
| Callback de cartas com contador, 3 milhões de chamadas | 0,017774 s | 0,016945 s | 4,7% |
| Popup Chakra, mesmo valor, 3 milhões de chamadas | 0,126958 s | 0,054930 s | 56,7% |

Sengenjin teve variação considerável entre rodadas: o resultado sólido é a remoção de uma consulta pública duplicada, com chamadas nativas 5 → 4, e não uma porcentagem estável de aceleração. A consulta de cursor removida da assinatura antiga era inutilizada e poderia já ser eliminada pelo compilador; ela não foi contabilizada como ganho comprovado de código de máquina.

Estas medições excluem GPU, submissão real de packets, execução Windows, engine e os demais MODs. **Uma redução de 63,8% em um callback não significa aumento de 63,8% nos FPS.** O benefício no jogo depende de quanto esse trecho ocupa do tempo total de cada frame.

## Validação concluída

- **1.884.072 verificações de receitas:** 24 catálogos determinísticos, todos os IDs/papéis testados, parâmetros inválidos, buffers/capacidade, configurações desligadas, produtor desativado, getters privados, clones e stats modificados entre consultas. Resultado anterior/posterior com o mesmo digest `b2802b8f2a96973a`.
- **595.000 verificações de lógica:** prévias, hooks entre atributos, guards, ambos os lados, fontes inválidas, reutilização de slots, presença em preparação, configurações, fila cheia e ordem das consultas restantes.
- **231.548 comparações de comandos de render:** seleção em ambos os lados, limites do cursor, identidades, flags modais, timelines e valores `0–65535`/`UINT_MAX` do popup.
- **ASan/UBSan:** suites sem diagnósticos; lógica/receitas repetidas com headers da v0.3.0. Sanity test existente das 18 receitas também passou.
- **Compilação final:** Clang/llvm-mingw com o script do SDK publicado da v0.3.0, alvo i386; objeto ELF32 relocável e imports verificados contra os 3.964 exports publicados. Base e candidato foram compilados separadamente. Compilações adicionais da base inicial com GCC/Clang passaram.
- **Revisão independente:** diff sem bloqueios encontrados. Manifest conserva ID, library, `min_api=4`, ordem, Settings e defaults. Textos de cartas, áudio, imagens, modelos, headers de assets e receitas declaradas permanecem byte a byte iguais ao pacote original.
- **Auditoria v0.3.0 i386:** os 42 alvos de hooks permanecem disponíveis; layouts relevantes de `DuelCardRecord`, `DuelRitualResult`, `DisplayObject`, campo/batalha e câmera mantidos. API 12 conserva os campos usados da API 4. Getters de stats e guards de remoção continuam adequados à implementação atual.

O formato ELF32 `.o` é o formato oficial de code MOD para este executável Windows de 32 bits. Não é uma DLL nem um objeto para builds Windows x64 próprios.

## Compatibilidade e limites

| Situação | Evidência disponível | Ainda necessário |
| --- | --- | --- |
| v0.3.0 Windows oficial, MOD sozinho | SDK publicado, compilação, imports, auditoria de tipos/hooks e testes diferenciais. | Gameplay Windows e medições de frametime. |
| MelhoriaDosRituais / materiais de Mão e Deck | Provider e getters privados comparados; fronteiras do oracle preservadas. | Invocação real combinada, incluindo interação com o ledger de rituais estendidos da v0.3.0. |
| AI Hard Mode / Time Wizard | Encadeamento e ordem existentes preservados; APIs oficiais estudadas. | Combinações reais de duelo, resets, remoção e traps. |
| MOD Mestre, Vanilla Field Effects, Tipos Secundários | Sem mudanças nos contratos usados por esses sistemas. | Pacotes desses MODs e testes combinados; não foram fornecidos. |
| 3D Monsters / apresentação Chakra | Assets e câmera preservados; comandos nativos comparados. | Verificação visual e de desempenho com modelos reais. |

O BIN enviado ultrapassou o limite de transferência de 32 MiB e não chegou ao executor. Assim, o jogo não pôde carregar o disco para validar gameplay. A recompilação não foi executada no Windows; FPS e compatibilidade conjunta não estão comprovados. Getters de atributos precisam ser determinísticos para a reutilização síncrona: um MOD externo que dependa da quantidade de chamadas deve ser testado especificamente.

Na v0.3.0, regras oficiais estendidas para os mesmos IDs podem ser rejeitadas pelo guard privado de três materiais; o oracle não alimenta o novo ledger `last/active`. Attack Traps definidos por dados também introduzem saídas de batalha com participantes ausentes (`0x1B`/`BattleAbortMask`). Essas integrações exigem regressões reais de proteção, sobrevivência e ataques extras; não foram alteradas nesta atualização.

## Relação com a diretriz e a versão oficial

`AUDITORIA-OFICIAL.md` registra as novidades e sua classificação. A v0.3.0 já contém melhorias de VSync/OpenGL e cache de sprites no executável. Elas foram consideradas na análise, mas seus resultados oficiais não são medições deste MOD. SDK do alvo adotado; API mínima e funcionalidades próprias preservadas.

As novas regras oficiais de Ritual permitem 1–5 materiais Campo/Mão, mas não reproduzem integralmente Deck, categorias privadas, exclusões e alternativas deste MOD. Substituir os seletores agora teria risco de alterar regras; a atualização mantém os hooks e contratos existentes. Novas APIs de áudio e portabilidade x64 também ficaram como trabalho separado. Fork, repositório oficial e demais MODs mantiveram arquivos rastreados intactos. Histórico atualizado no único `CHECKPOINT-CONTINUACAO.md`.

## Como repetir a comparação

Os scripts em `tests/` recebem a pasta extraída do ZIP original em `--baseline`. Lógica/receitas recebem a raiz do source oficial v0.3.0 em `--sdk`; precisam de um compilador C nativo. Render usa `--compiler`, as outras suites usam `--cc`. Os executáveis temporários são removidos automaticamente.

```text
python tests/test_logic_performance.py --baseline "PASTA_ORIGINAL" --sdk "SOURCE_V030"
python tests/test_logic_performance.py --baseline "PASTA_ORIGINAL" --sdk "SOURCE_V030" --sanitize
python tests/test_recipe_performance.py --baseline "PASTA_ORIGINAL" --sdk "SOURCE_V030"
python tests/test_recipe_performance.py --baseline "PASTA_ORIGINAL" --sdk "SOURCE_V030" --sanitize
python tests/test_render_performance.py --baseline "PASTA_ORIGINAL" --benchmark --sanitize
```

Para comparar no Windows, guarde o MOD antigo fora de `mods`, use a mesma v0.3.0, save/duelo e configurações gráficas, e altere apenas esta pasta entre as medições. Compare frametimes nos efeitos Chakra, contadores, campo denso BLS/Dokurorider e prévias Sengenjin; repita com os mesmos MODs parceiros ativos. Confira também os efeitos, receitas, buffs, câmera e invocações antes de considerar essa combinação validada.
