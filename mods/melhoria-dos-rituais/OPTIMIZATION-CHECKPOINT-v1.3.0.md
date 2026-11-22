# Melhoria dos Rituais — Optimization Checkpoint v1.3.0

Base funcional: **v1.2.9 Result Card Rules**, aprovada operacionalmente em jogo.
Objetivo desta rodada: reduzir custo e tamanho sem alterar regras, prioridade,
settings, compatibilidade entre MODs ou animação aprovada.

## Arquitetura preservada

- Exatamente 3 tributos.
- Prioridade absoluta de origem: **Field > Hand > Deck**.
- Critério de fraqueza só desempata depois da origem.
- Receitas Vanilla e alteradas passam pelo mesmo pipeline; a cadeia pública de
  `Duel_CheckRitual` continua sendo a autoridade final.
- Outro MOD de receitas continua opcional: compatível, nunca obrigatório.
- Resultado físico `Hand` / `Hand or Deck`, restrição player-only e bottom-deck
  continuam com a mesma semântica da v1.2.9.
- Campo mantém animação Vanilla; Mão entra por baixo com verso; Deck entra pela
  direita com o visual/iluminação Vanilla aprovado.

## Hotpass 1 — remover instrumentação de teste

- Removidos overlay, strings, contadores e estado de diagnóstico.
- Removido o setting `Diagnostics overlay` da versão final.
- O MOD final não desenha nada por frame.

## Hotpass 2 — hook quente da mão

Antes, `DuelScene_UpdateHandActions` consultava todos os settings do MOD em cada
frame. Agora existe fast-path:

- sem CONFIRM -> chama o controller original imediatamente;
- fora da fase de placement -> imediato;
- CPU -> imediato;
- só quando um humano realmente tenta baixar uma carta é consultado o único
  setting relevante, `ritual_only_player`;
- a varredura de receitas só acontece nessa tentativa real.

Os settings continuam live; não foi necessário transformar o MOD em
`restart: true`.

## Hotpass 3 — snapshot de candidatos

Cada candidato de Campo/Mão/Deck agora captura uma vez por decisão:

- ATK/DEF impressos;
- Base ID;
- Tipo;
- Level;
- origem, slot, record e UID físico.

Esses valores não são mais decodificados repetidamente dentro dos loops de
combinação.

Para receitas legíveis, cada candidato recebe uma máscara de quais dos três
requisitos satisfaz. O loop interno só visita candidatos que podem ocupar
realmente aquele requisito.

## Hotpass 4 — busca por tiers de origem

A própria regra garante que contagem de Campo domina qualquer outro critério e,
em empate, contagem de Mão domina Deck. Portanto a busca agora visita tiers na
ordem:

`3F -> 2F/... -> 1F/... -> 0F/...`, sempre com Mão decrescente dentro do tier.

Assim que um tier contém uma solução, tiers inferiores não são avaliados porque
matematicamente nunca poderiam vencer pelo desempate atual.

Isso foi aplicado tanto ao seletor de receita legível quanto à fase de ranking
do fallback/oráculo. A ordem usada para **descobrir o resultado** no fallback
não foi alterada, preservando comportamento de hooks privados.

Exemplo limite com 45 candidatos e uma receita ampla:

- busca antiga legível: até `45 * 44 * 43 = 85.140` atribuições;
- com 5 materiais válidos já no Campo, o melhor tier exige apenas as 60
  permutações dos 5 cards de Campo.

No fallback, a fase antiga podia validar `C(45,3) = 14.190` trios para ranking;
com três materiais válidos no Campo, o tier superior contém apenas `C(5,3)=10`.
Esses números são limites teóricos do caso amplo, não benchmark de FPS.

## Hotpass 5 — classificação dos resultados de Ritual

A regra de colocar monstros Ritual no fundo do Deck antes da mão inicial antes
chamava `is_ritual_result_card()` para cada uma das 40 cartas. Essa função ainda
percorria o espaço `CARD_TABLE_COUNT` do PC (`0x7FFE`) ao procurar receitas
adicionadas.

Agora:

- apenas IDs de cartas realmente existentes nesta execução (`CARD_COUNT_LIVE`)
  são sondados;
- a preparação do Deck constrói uma bitset de resultados uma única vez;
- as 40 cartas viram consultas O(1) nessa bitset;
- o check de placement direto também deixa de sondar IDs impossíveis acima de
  `CARD_COUNT_LIVE`.

Custo extra deliberado: ~4 KiB de BSS para a bitset, em troca de eliminar uma
quantidade muito maior de chamadas repetidas às tabelas de Ritual.

## Pontos deliberadamente NÃO otimizados

Não remover sem nova prova de equivalência:

- o snapshot/restauração dos 5 Monster Zones usado pelo oráculo;
- a revalidação do trio escolhido pela cadeia pública de hooks;
- a chamada final do oráculo que deixa publishers downstream coerentes;
- a busca por UID físico no Deck e a rotação estável ao consumir uma carta;
- a reconstrução/anchor visual necessária para Hand/Deck/zero-field;
- o isolamento da regra player-only da CPU;
- a preferência de uma cópia do resultado na Mão antes do Deck em `Hand or Deck`.

## Invariantes que futuras versões devem preservar

1. Nunca trocar `Field > Hand > Deck` por "mais fraco globalmente".
2. `Minimum Field/Hand` são restrições; não mudam a prioridade de origem.
3. Continuar exigindo exatamente 3 tributos.
4. Não tornar o MOD de receitas uma dependência.
5. Não alterar CPU por causa da regra player-only.
6. Não consumir a mesma cópia física como tributo e resultado.
7. Não mudar a ordem relativa das outras cartas quando uma carta é retirada do
   Deck ou quando resultados Ritual são mandados para o fundo.
8. Não alterar a animação aprovada sob pretexto de performance.

## Validação desta rodada

- SDK oficial `v0.2.1-preview.1`: **PASS**.
- Compilação 32-bit relocatable pelo `build_mod.py`: **PASS**.
- Relocations: somente `R_386_32` e `R_386_PC32`: **PASS**.
- Harness old-vs-new do seletor legível: **100.000 estados aleatórios, PASS**.
- Harness old-vs-new do ranking do fallback: **100.000 estados aleatórios, PASS**.
- `.text`: **19.648 -> 15.512 bytes** (~21,1% menor).
- `rituals.o`: **91.196 -> 74.868 bytes** (~17,9% menor).
- BSS aumenta em ~4,9 KiB por caches/bitset deliberados; o ganho é remover
  recomputações e chamadas repetidas nos caminhos quentes/por decisão.
