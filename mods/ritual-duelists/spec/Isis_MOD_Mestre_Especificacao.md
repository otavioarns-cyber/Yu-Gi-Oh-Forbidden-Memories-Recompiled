# MOD Mestre — Isis: especificação para implementação

**Status:** planejamento aprovado; a implementação e os testes ainda precisam ser realizados.

## Objetivo

Criar um MOD independente para Yu-Gi-Oh! Forbidden Memories Recompiled com geração de deck híbrido e IA de Ritual da Isis, projetado **desde o início como infraestrutura reutilizável para adaptar os outros duelistas no futuro**. A primeira entrega só altera a Isis.

## Pool comum (somente cartas extras)

- 83 cartas distintas, 2.048 pontos, **exatamente a mesma pool e os mesmos pesos** nas 3 dificuldades.
- 64 Aquatic (Aqua/Fish/Sea Serpent): 1.228 pontos; 13 Skulls aprovados + 1 Red-Eyes B. Dragon: 410; 5 magias: 410.
- Copiar os pesos do CSV anexo `Isis_Pool_83_Cartas_Pesos.csv` sem reinterpretar; os campos `id`, `name`, `weight`, `group` identificam cada opção. Os nomes de cartas seguem o catálogo de Forbidden Memories.
- Zero peso na pool para Novox's Prayer, Kamakiriman, Swordstalker e Skull Guardian normal: essas cartas só entram pelas vagas garantidas. Skelgon, Summoned Skull, Red-Eyes e B. Skull Dragon continuam na pool além das cópias garantidas.
- Não reintroduzir Turtle Oath, Soul of the Pure, Eternal Draught, Dragons genéricos ou outros cortes aprovados.

## Estrutura fixa das 3 dificuldades

| Carta fixada | ID | Fácil | Médio | Difícil |
|---|---:|---:|---:|---:|
| Novox's Prayer | 679 | 1 | 2 | 3 |
| Kamakiriman | 535 | 1 | 2 | 3 |
| Skelgon | 545 | 1 | 2 | 3 |
| Swordstalker | 378 | 1 | 2 | 3 |
| Skull Guardian (normal) | 704 | 1 | 2 | 3 |
| Summoned Skull | 22 | 0 | 0 | 1 |
| Red-eyes B. Dragon | 82 | 0 | 0 | 1 |
| B. Skull Dragon | 217 | 0 | 0 | 1 |

| | Fácil | Médio | Difícil |
|---|---:|---:|---:|
| Cartas fixas | 5 | 10 | 18 |
| Sorteadas da pool comum | 35 | 30 | 22 |
| Deck final | 40 | 40 | 40 |

- Sorteio do perfil **uma vez por duelo**; sempre 40 cartas; as cartas fixas são embaralhadas juntas às sorteadas.
- Story Mode: fácil 15%, médio 50%, difícil 35%.
- Free Duel: fácil 50%, médio 35%, difícil 15%.
- Quantidades fixas e sorteadas respeitam um limite **total** de até 3 cópias por ID (inclusive entre fixas e pool); repetir sorteio quando atingir o limite e evitar loops sem saída.
- Nunca usar o mesmo monstro como dois tributos da mesma invocação.
- As posições fixas reservam materiais de 1, 2, 3 invocações potenciais, **não asseguram que a IA as realize**.

## Ritual e efeito a verificar/implementar sem duplicar outros MODs

- `Novox's Prayer` (679) ritualiza `Skull Guardian` (704): tributos distintos **Kamakiriman (535) + 1 Skull Monster válido + 1 Warrior válido**.
- Critério de tributos genéricos: ATK impresso menor que 2050 **e** DEF impressa menor que 2500; validar os IDs e as condições contra a receita oficial do MOD de rituais que estiver ativa.
- `Skelgon (545)` 1700/1900 e `Swordstalker (378)` 2000/1600 são os materiais garantidos para essas duas posições.
- O usuário costuma ativar a regra de materiais mão+campo com **pelo menos um tributo no campo**: detectar/compatibilizar com o MOD existente; não aplicar o requisito duas vezes nem contorná-lo. Testar 1 campo + 2 mão e 2 campo + 1 mão.
- `Skull Guardian` invocado sem Ritual é monstro vanilla de 2050/2500 e possui a classificação invisível Skull para fortalecer **outro** Guardian ritualizado, mas não ativa o próprio efeito.
- Skull Guardian invocado corretamente por Ritual ganha **+1000 ATK e DEF por cada OUTRO Skull Monster no campo em ambos os lados; +500 ATK e DEF por Skull Monster nos 5 slots reais da própria mão**. Não conta a si; múltiplos Guardians ritualizados podem ajudar uns aos outros; recalcular dinamicamente, sem buffs duplicados e mantendo a proveniência do Ritual.
- A categoria Skull é a lista aprovada de 21 IDs: `9,22,24,30,59,132,146,153,154,215,217,228,241,359,540,545,548,556,564,704,707` (inclui Wood Remains #228).
- O ZIP fornecido `Efeitos-de-Rituais-v0.77.3-test(1).zip` não declara diretamente a receita/efeito Skull Guardian no `mod.json` ou `mod.c`. **Verificar** a versão-alvo e integrar ou implementar o comportamento faltante em módulo compatível, sem sobrescrever o MOD de efeitos existente.

## Inteligência artificial (projeto-base reutilizável)

- Não criar IA exclusiva inteiramente hardcoded para Isis. Criar `AI Ritual Planner` + dados declarativos/perfis dos duelistas + avaliação por efeito ritual (atualmente Skull Guardian). Outros duelistas devem poder ser adicionados por configuração, sem reimplementar o núcleo.
- Para **Isis apenas** nesta entrega: preservar materiais condicionados à proximidade real do Ritual; não sacrificar sem necessidade Swordstalker, Skelgon, Kamakiriman ou Novox's Prayer; no entanto, evitar paralisia/espera infinita e permitir ações emergenciais quando o jogador ameaça vencer.
- Reavaliar após cada mudança de estado: cartas reais disponíveis (mão física, campo, próxima janela de deck original), efeitos, slots, stats ATK/DEF dinâmicos, bônus Skull pós-tributo, outras ameaças, campo Umi e magias de defesa.
- Ao preparar invocação, reservar pelo menos um material no campo quando a regra mão+campo for exigida; proteger cartas reservadas contra Fusions automáticas, substituição por carta nova, descarte e troca indevida da mão pelo deck; respeitar as regras de compra/troca originais.
- Avaliar invocar agora versus preservar campo e adiar: valor **depois** de consumir os tributos, vantagem ofensiva, defensiva e letal, presença de Skulls nas 5 posições reais da mão e em ambos os campos. Nunca contar um tributo como buff se for consumido.
- Distinguir deck normal de Skull Guardian ritualizado. `B. Skull Dragon` é opção de Fusion com Summoned Skull + Red-Eyes, mas não forçar Fusion quando prejudicar um Ritual mais vantajoso.
- Preservar ataque e uso inteligente de Swords/Spellbinding Circle/Harpie's Feather Duster/Dark Energy e Umi sem provocar autoplay de magias no momento errado.
- Validar se o AI Hard Mode está ativo e evitar conflito de hooks; aproveitar estrutura do planejador existente se viável, com cadeia de hooks segura e fallback. Não substituir a IA de outros duelistas.

## Modo história e Free Duel

- Detectar origem na entrada do duelo, com registro estável até o fim. Na recompilação, o campo `D_8009B368` recebe 2 em História e 6 em Free Duel (verificar na versão distribuída).
- Manter lógica de IA coerente nas duas modalidades; a facilidade do Free Duel decorre principalmente das probabilidades da seleção, não de criar uma IA deliberadamente irracional. O difícil segue sendo ocasionalmente desafiador no Free Duel.

## Compatibilidade, qualidade e entrega

- Isolar como **MOD instalável independente** com `mod.json`, código-fonte, objeto compilado, README e configurações estritamente necessárias; não editar código-fonte upstream nem arquivos de outros MODs.
- Alvo inicial: `v0.2.1-preview.1` com SDK correspondente e jogo fornecido. Confirmar API, hooks e bibliotecas disponíveis antes de implementar.
- Não modificar upstream `Unchiga` nem branches `master`, `mod-field-effects` ou outros MODs existentes. Caso GitHub precise ser usado, trabalhar em branch nova e área isolada do fork `otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled`.
- Testes: 40 cartas, distribuição ~15/50/35 ou ~50/35/15 em grande amostra, 83 opções, pesos=2048, cap de 3, exatos 5/10/18 fixos, 1/2/3 materiais+magia+Guardian normal, rituais com 1 ou 2 tributos no campo, mão real, conservação e gasto condicional, ausência de deadlocks, save/load, efeitos do Guardian, coexistência de MOD de Rituais e AI Hard Mode, FPS e compilação.
- Logs de diagnóstico opcionais: sorteio de perfil/mode, cartas e peso, estado de preparação, consumo de materiais e razão do Ritual/adiamento. Não gerar logs excessivos por frame.
- Entregar ZIP testável, fontes, documentação e relatório honesto do que funcionou e do que ainda requer teste. **Não** afirmar teste bem-sucedido sem execução real.

## Fonte auditável da pool

Consultar `Isis_Pool_83_Cartas_Pesos.csv`; foi preparado a partir da pool original da Isis, das exclusões e dos novos pesos aprovados. A tabela original da recompilação é `notes/research/fusion-and-drop-tables/drops.csv`, e a lista de nomes/IDs é `notes/card-catalog.csv`.

### Peso por grupos

| Grupo | Opções únicas | Peso |
|---|---:|---:|
| Aquáticos | 64 | 1228 |
| Estratégia Skull (13 Skulls + Red-Eyes) | 14 | 410 |
| Magias | 5 | 410 |
| **Total** | **83** | **2048** |
