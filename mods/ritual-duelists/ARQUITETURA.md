> **Estado atualizado (Beta.7, 17/10/2026):** 28 registros declarativos: 27 duelistas com Ritual Package de peso virtual 30 e Isis apenas como referência Vanilla; `RD_PROFILE_CAPACITY=28`. Modificações exclusivas de pool/perfil para #36, #37 e #38. As seções históricas abaixo documentam versões anteriores.

> **Estado atualizado (Beta.6, 17/10/2026):** 25 registros declarativos: 24 duelistas com Ritual Package de peso virtual 30 e uma Isis meramente referencial, sem gerenciamento em runtime. `RD_PROFILE_CAPACITY=25` acomoda caches e perfis sem deslocar índices antigos. Outros trechos abaixo documentam a arquitetura de versões históricas, não devem substituir esse estado.

## Integração opcional de inteligência dos efeitos — Beta.4

A avaliação dos efeitos é propriedade do MOD Efeitos de Rituais, publicado por `effect_intelligence_v1`: habilitação e forecast de ATK/DEF/sinergia/valor tático por resultado ativo. Ritual Duelists continua proprietário do planner, dos decks, das receitas ativas e dos materiais; não atribui mais bônus específicos de rituais localmente. A conexão via `host->find` ocorre na IA, não exige ordem de carregamento nem `requires`. O código fornecedor continua no processo quando seu MOD é desativado, mas `enabled()` retorna falso nesse caso. As tabelas de grupo Skull preservadas nos perfis existem para interpretação da *receita*, e não para conferir efeito.

## Extensão de capacidade v0.3.5-beta.1

A tabela declarativa `profiles.c` contém 19 perfis; `RD_PROFILE_CAPACITY=19` acomoda caches da receita, IDs e estado do módulo sem mudar a IA nem os hooks. Novas pools literais em `profile_data.h`, auditadas contra o anexo integral (SHA-256 por par ID:peso). As fontes de receita permanecem o provider atual, tabelas v0.3.0 ou Vanilla; os IDs de `baseline_materials` são mera preferência, jamais uma nova receita.

# Ritual Duelists — Arquitetura Beta v0.3.2

## Responsabilidades separadas

- `profiles.c`, `profile_data.h`: dezesseis registros independentes, receitas de referência, preferências de material e pools reais com soma 2018 por oponente.
- `recipe_adapter.c`: encontra e interpreta a **receita ativa** pela API `ritual-card-effects-test:recipe_v1` ou pelas tabelas oficiais e pelo disco. O callback `Rd_MatchRecipeCard` respeita cartas específicas, tipos, stats, níveis, grupos e as condições expostas pelo provider. Sem API não presumir efeitos ou receitas desconhecidas.
- `decks.c`: substitui integralmente a lógica anterior Fácil/Médio/Difícil. Entrada de pacote **virtual e única de peso 30**; escolhe/reserva 4 cartas reais de uma vez, com busca limitada de materiais e limite máximo de 3 cópias, 3 pacotes, 40 cartas. A soma por perfil é 2018+30=2048; Story ou Free-ON recebem primeiro pacote antes das escolhas normais. Free-OFF sorteia todos, incluindo a possibilidade de zero. A opção não altera a pool real.
- `mod.c`: lê `guaranteed_package_free` com fallback explícito **0**, evitando garantia silenciosa após instalação/atualização. Conecta o hook já existente `Duel_ShuffleDeck`, salva somente número de pacotes, mantém os demais hooks de IA intactos e registra os quatro IDs de cada pacote. Os Settings persistem pela plataforma.
- `planner.c`, `adaptive_rules.c`, `deck_access.c`, `tribute_fetch.c`, `post_ritual.c`: IA independente do mecanismo de geração, conserva o acesso real ao Deck da CPU, a consulta de regras do Melhoria de Rituais, campo seguro, magia virada para baixo e o Beacon de Isis. O jogador e as invocações originais não são alterados.

Receitas com cardinalidade diferente de três continuam disponíveis para **planejamento/execução na IA**, mas não são falsificadas como um pacote de quatro cartas. Nesses casos o gerador usa fallback seguro do jogo original, com log. O gerador não modifica a receita original e **não** autoriza usar tributos diretamente do Deck. O limite de backtracking impede loops longos em receitas impossíveis.

Os IDs `#721` e `#722` são lidos e gravados como IDs reais no caminho de geração do MOD, em vez de depender do limite legado de varredura de pesos da CPU. Sua seleção prática no jogo ainda depende da validação da nova Beta.

## Heishin 1st e Heishin 2nd — pools completas aprovadas

**Heishin (ID 8)**: 63 cartas reais, 2018 pontos + Ritual Package virtual 30. Removidos #714 e #711; acrescentado #722=30, Dark Magician #35=36.

**Heishin 2nd (ID 35)**: 74 cartas reais, 2018 pontos + Ritual Package virtual 30. Removido somente #698; acrescentado #722=30, Dark Magician #35=20. Não remover #266, #635 ou #638 nem outras cartas sem aprovação.

| Heishin 1st — ID | Peso | Heishin 2nd — ID | Peso |
|---:|---:|---:|---:|
| 17 | 32 | 1 | 32 |
| 18 | 32 | 17 | 64 |
| 19 | 32 | 18 | 64 |
| 20 | 32 | 19 | 64 |
| 21 | 32 | 20 | 64 |
| 35 | 36 | 21 | 64 |
| 37 | 32 | 35 | 20 |
| 38 | 32 | 37 | 16 |
| 57 | 32 | 38 | 16 |
| 67 | 32 | 42 | 8 |
| 69 | 32 | 63 | 16 |
| 217 | 32 | 69 | 16 |
| 320 | 32 | 74 | 8 |
| 337 | 32 | 79 | 8 |
| 342 | 32 | 82 | 16 |
| 347 | 32 | 90 | 8 |
| 356 | 32 | 99 | 8 |
| 358 | 32 | 106 | 8 |
| 360 | 32 | 213 | 8 |
| 369 | 32 | 217 | 32 |
| 370 | 32 | 223 | 8 |
| 371 | 32 | 266 | 64 |
| 372 | 32 | 337 | 120 |
| 373 | 32 | 348 | 64 |
| 385 | 32 | 358 | 16 |
| 386 | 32 | 360 | 32 |
| 390 | 32 | 366 | 8 |
| 391 | 32 | 369 | 16 |
| 401 | 32 | 370 | 16 |
| 407 | 32 | 371 | 16 |
| 426 | 32 | 372 | 16 |
| 427 | 32 | 373 | 16 |
| 442 | 32 | 374 | 64 |
| 453 | 32 | 385 | 16 |
| 467 | 32 | 386 | 16 |
| 471 | 32 | 390 | 16 |
| 500 | 32 | 391 | 16 |
| 509 | 32 | 392 | 32 |
| 564 | 32 | 401 | 16 |
| 613 | 32 | 407 | 16 |
| 645 | 32 | 426 | 16 |
| 657 | 32 | 427 | 16 |
| 661 | 32 | 442 | 16 |
| 669 | 32 | 453 | 16 |
| 672 | 32 | 465 | 8 |
| 686 | 32 | 467 | 16 |
| 687 | 32 | 471 | 16 |
| 688 | 32 | 495 | 8 |
| 689 | 32 | 500 | 16 |
| 703 | 32 | 509 | 16 |
| 704 | 32 | 522 | 8 |
| 705 | 32 | 525 | 8 |
| 706 | 32 | 529 | 8 |
| 707 | 32 | 531 | 8 |
| 708 | 32 | 545 | 8 |
| 709 | 32 | 564 | 16 |
| 710 | 32 | 571 | 8 |
| 713 | 32 | 572 | 8 |
| 715 | 32 | 594 | 8 |
| 716 | 32 | 613 | 16 |
| 717 | 32 | 619 | 8 |
| 718 | 32 | 622 | 8 |
| 722 | 30 | 627 | 8 |
| — | — | 632 | 8 |
| — | — | 635 | 64 |
| — | — | 638 | 64 |
| — | — | 645 | 16 |
| — | — | 657 | 160 |
| — | — | 672 | 80 |
| — | — | 686 | 120 |
| — | — | 690 | 48 |
| — | — | 712 | 8 |
| — | — | 713 | 32 |
| — | — | 722 | 30 |
| **Total real** | **2018** | **Total real** | **2018** |
| **Virtual Ritual Package** | **30** | **Virtual Ritual Package** | **30** |
| **Pool final** | **2048** | **Pool final** | **2048** |

O pacote de ambos usa **Dark Magic Ritual #721**, três materiais da receita ativa e não inclui o Magician of Black Chaos pronto #722. O Ritual secundário Cosmo Queen não gera pacote no Heishin 2nd.

## Protocolos e limites

Mantidos sem alteração os provedores `ritual_recipe_provider_v1.h` e `ritual_rules_provider_v1.h`. Não há alteração nas receitas, efeitos, MODs externos, master ou decompilação. A versão é distribuída com objetos compilados pelo build oficial do SDK v0.3.0: i386 (ELF32) e Windows x64 (ELF64).

## Perfis Rex, Martis e Desert (v0.3.2)

O índice local do vetor de perfis é: Isis 0, Jono 1, Jono 2nd 2, Weevil 3, Shadi 4, Heishin 5, Heishin 2nd 6, Rex 7, Martis 8, Desert 9. Os IDs *do jogo* são respectivamente 16, 3, 19, 10, 13, 8, 35, 9, 28 e 27. A identificação ocorre por `Duelists_Named`, não supondo que índice interno seja duelist_id. A capacidade dos caches foi ampliada para 10.

Rex e Martis só recebem pacotes de Tri-Horned (#680/#705); o Dokurorider pronto do Martis (#719=50) não possui pacote próprio. Desert só recebe pacotes de Dokurorider (#699/#719). Os materiais preferenciais Vanilla, quando atenderem à receita ativa, são Tri-Horned: #6/#47/#571 e Dokurorider: #146/#479/#485. O `Rd_ResolveRecipe` atual decide a receita antes dessas preferências. Não há nova regra de efeitos ou alteração no uso de tributos no campo/mão/Deck.

Time Wizard no deck de Weevil é apenas uma entrada ponderada; `rd_eligible` exclui #16 dos pacotes de Javelin para não tratá-lo como tributo. A interpretação de Monster/Magic Zone e Time Magic permanece exclusiva do MOD Time Wizard.

A tabela completa de IDs e pesos está em `spec/Ritual-Duelists-Pools.csv`. O ajuste compensatório em três Insects de Weevil está descrito no `HISTORICO.md` e precisa ser considerado na revisão de design.
