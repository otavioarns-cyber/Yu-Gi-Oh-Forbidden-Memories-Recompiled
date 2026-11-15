# v1.3.2 — Integração opcional de receitas (teste)

**Base:** MelhoriaDosRituais(5).zip (v1.3.1).  **Parceiro estudado:** Efeitos-de-Rituais-v0.78.40-otimizado.zip.

## Motivação e decisões

Não se comprovou incompatibilidade funcional nas versões anteriores. A v0.78.40 publica uma interface de receitas (`ritual-card-effects-test:recipe_v1`) que não toma parte na seleção física nem consome tributos. A v1.3.2 a consulta de maneira conservadora, preservando os resultados decididos pela cadeia de `Duel_CheckRitual`.

A v1.3.1 mantém a prioridade Campo > Mão > Deck, fontes e mínimos, resultado físico opcional, restrição só ao humano, animações, ordenação e fallback por hook. Nada disso foi alterado.

## Alterações reais

- Cópia exata do cabeçalho `ritual_recipe_provider_v1.h` do fornecedor, com ABI 1, sem dependência binária obrigatória.
- Busca dinâmica tardia `host->find(...)`, validação de ABI e dos callbacks `get/matches`; sem cache de estado ON/OFF e sem requisito em `mod.json`.
- Atalho **somente quando existem exatamente três candidatos físicos**, os papéis da receita podem ser alocados sem repetir candidatos, as regras de fonte são satisfeitas, `Require Ritual Monster` está OFF e a cadeia de hooks confirma o mesmo resultado. O trio é mantido na mesma ordem física usada pelo fallback antigo. Demais estados continuam pelo algoritmo já aprovado.
- `get` e `matches` não podem autorizar uma invocação independentemente: falha, conflito, mod inativo, receita desativada, assinaturas inválidas e quadros com 4+ candidatos voltam à v1.3.1.

## Validação

- Compilação **SDK oficial v0.3.0**, alvo **i386 ELF32**, imports validados pelo builder.
- **15 cenários isolados**, gerados a partir da função C de produção, compilados com sanitizadores **ASan/UBSan**: ausência e desativação do fornecedor, ponteiros incompletos, ABI errada, resultado inválido, 2/4 candidatos, exigências Campo/Mão, resultado físico obrigatório, rejeição dos hooks e permissões com papéis permutados.
- Manifest: Settings, padrões, tipos, valores e ordem sem mudanças; só versão e descrição atualizadas.
- Comparação: todo o código de seleção existente e todas as rotinas de animação/IA permanecem inalterados fora da integração opcional.

**Limitação:** não houve execução da recompilação Windows real com os dois MODs ativados, nem teste de frame time. Não afirmamos compatibilidade completa comprovada. Essa é uma candidata de teste, e a v1.3.1 deve ser guardada para rollback. O atalho não melhora os casos complexos com múltiplos trios: nesses, deliberadamente permanece o fallback antigo para proteger o gameplay e terceiros.

### Teste manual sugerido

1. Melhoria isolada: Rituais Vanilla com 3 Campo, mistos e tributos do Deck.
2. Melhoria + Efeitos 0.78.40: receitas fixas, privadas e especiais (Mask, Garma, Performance, Novox/Skull, Chakra); inverter ordem dos materiais; sem/ com número mínimo.
3. Desativar apenas Efeitos e repetir. Conferir que não mudou o resultado.
4. Ativar `Require Ritual Monster: Hand` e `Hand or Deck`, testar consumo real.
5. Ativar restrição de invocação direta de Ritual Monsters apenas para o jogador, conferir IA.
6. Repetir com terceiros relevantes (Fusion Helper, AI Hard Mode e Time Wizard).
