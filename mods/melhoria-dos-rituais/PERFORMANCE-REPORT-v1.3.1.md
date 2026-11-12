# Melhoria dos Rituais — Performance Checkpoint v1.3.1

Data: 2026-10-11  
Base imutável de referência: `Melhoria-dos-Rituais-v1.3.0-Final.zip` (SHA-256 `fad2f93e1a3e2624a2401c24a628e5734b0d052014826a5d04d6b2791ba4f3e3`).

## Objetivo e escopo

Reduzir custo de CPU nas verificações de receitas de Ritual e fallback por
hooks sem alterar nem as regras nem a ordem de consulta às receitas de outros
MODs. **Somente `rules.c` e `selection.h` mudaram no código de jogo.**
Configurações, modo de animação, consumo das cópias físicas, UI e fontes de
tributos permanecem inalterados. Não existe telemetria, instrumentação ou
verificação adicional executada por frame na build entregue.

## Mudanças comprováveis

1. `select_candidates`: constrói em ordem ascendente três listas pequenas,
   cada uma contendo apenas os candidatos que satisfazem o requisito daquele
   tributo. Os loops internos percorrem essas listas, em vez de reavaliar
   candidatos sabidamente inelegíveis. Candidatos múltiplos continuam
   distintos por sua identidade física/slot.
2. `select_candidates`: descarta tiers de origem inviáveis com os totais
   disponíveis em Campo, Mão e Deck, e descarta pares que, com apenas a
   terceira carta restante, jamais alcançariam o tier.
3. `select_candidates_via_chain`: antes do loop de trios, calcula quantos
   materiais ainda faltam de Campo e Mão; testa como terceiro material apenas
   candidatos da origem necessária. O *oráculo externo é invocado para os
   mesmos trios, na mesma ordem da v1.3.0*.

**Não modificados:** `Duel_CheckRitual`/hook chain, a chamada final para
restaurar o publisher, snapshot/restauração do campo, algoritmos de consumo,
limites ou exigências de cartas, detecção de resultados no fundo do deck,
matéria de efeitos e animação. Não adotei automaticamente
`Tables_RitualRule` como novo resolvedor: suas receitas de 1–5 tributos e
restrições de fonte são diferentes do contrato aprovado de exatamente três
tributos e poderiam mudar o resultado sem validação no jogo.

## Medidas

Teste comparativo em C contendo o corpo real antigo e o novo dos dois
seletores, com estado sintético e oráculo determinístico. Compilado no host
x86-64 com GCC `-O2`; **não é medição do jogo nem do SDK i386**.

- 50.000 estados pseudoaleatórios com/sem Deck, até 45 candidatos;
  teste das receitas legíveis: zero diferenças em sucesso, fontes,
  slots, IDs e UIDs escolhidos.
- Os mesmos 50.000 estados com fallback: zero diferenças; número e
  sequência das chamadas ao oráculo idênticos (rastreados em hash sequencial).
- Sanitizers AddressSanitizer e UndefinedBehaviorSanitizer: sem alertas.
- Caso fixo (45 candidatos, três requisitos exatos, 100.000 decisões):
  0,1142 s antes → 0,0133 s depois (**8,6×**).
- Caso fallback (45 candidatos no Deck, 350 decisões):
  0,0780 s antes → 0,0325 s depois (**2,4×**).
- O tamanho da seção código reportado por `size` foi de 15.731 para
  15.955 bytes (+224 B; +1,4%): leve aumento de código em troca de menos
  trabalho executado. O `.o` completo cresceu de 74.868 para 75.160 B
  (+292 B). BSS ficou inalterada em 6.664 B.

Os multiplicadores acima refletem **cenários escolhidos para pressionar o
algoritmo**, não ganho universal. Nos Rituais de 3 tributos já presentes no
Campo, a lógica Vanilla retorna antes do novo seletor. FPS e possíveis
microtravadas exigem teste dentro do jogo, inclusive com outros MODs.

## Compilação e proteção

SDK oficial `yfm-redecomp-v0.3.0`, opção `--target i386`:
**PASS**. ELF relocável i386 com apenas `R_386_32` / `R_386_PC32`.
O SDK em seu modo padrão também tenta alvos de 64 bits, mas esses
são tratados separadamente e **não são validados por este pacote**.

Não alterar o repositório Unchiga, master do fork nem outros MODs.
O pacote contém apenas `MelhoriaDosRituais/`. Manter v1.3.0 disponível
como restauração imediata em caso de regressão.

## Smoke test no jogo antes de divulgação final

1. Ritual Vanilla usando três cartas em Campo, para garantir a animação normal.
2. Ritual com 1 Campo + 2 Mão e outro com materiais do Deck.
3. Ritual de receita condicional alterada por outro MOD ativado;
   a carta resultante e os tributos devem ser os mesmos da v1.3.0.
4. Settings `Require Ritual Monster` Hand, Hand or Deck,
   `Player must Ritual Summon` e minimo Campo 0; verificar consumo correto.
5. Testar com outros MODs relevantes, especialmente receitas e AI Hard Mode.

Não anunciar "sem lag" nem compatibilidade total antes destes testes.
