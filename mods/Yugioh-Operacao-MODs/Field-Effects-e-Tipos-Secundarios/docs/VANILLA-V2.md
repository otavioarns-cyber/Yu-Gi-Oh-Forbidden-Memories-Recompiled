# Especificacao implementada - Vanilla V2

Este documento registra as regras consolidadas implementadas na v0.18 do MOD Field Effects + Secondary Types.

## Regras centrais

- Um subtipo representa uma categoria adicional do monstro.
- Quando uma regra de Fusion exige explicitamente uma categoria/tipo como material, o resultado pode herdar essa categoria como subtipo. Materiais exigidos como cartas especificas nao transmitem automaticamente o proprio tipo.
- Dois subtipos favoraveis no mesmo Field continuam limitados a +200 ATK/DEF secundario; dois desfavoraveis continuam limitados a -200. Um favoravel e um desfavoravel no mesmo Field se cancelam e produzem 0.
- Um subtipo igual ao tipo principal atual nao gera modificador secundario.
- Se o tipo principal atual for Fish, Aqua ou Sea Serpent, subtipos Fish/Aqua/Sea Serpent permanecem como identidade, mas nao geram bonus/debuff secundario.
- Beast-Warrior principal nao recebe o efeito secundario de Beast nem Warrior; outros subtipos continuam funcionando.
- Interacoes bloqueadas, redundantes ou anuladas por cancelamento nao aparecem no rodape. Se nenhuma interacao funcional restar, nao existe rodape de Field Effects.

## Tabela de Field por subtipo

| Subtipo | Field | Modificador |
| --- | --- | ---: |
| Dragon | Mountain | +200 |
| Winged Beast | Mountain | +200 |
| Thunder | Umi / Mountain | +200 |
| Spellcaster | Yami | +200 |
| Fiend | Yami | +200 |
| Zombie | Yami / Wasteland | +200 |
| Warrior | Sogen | +200 |
| Beast | Forest | +200 |
| Plant | Forest | +200 |
| Insect | Forest | +200 |
| Rock | Wasteland | +200 |
| Dinosaur | Wasteland | +200 |
| Fish | Umi / Wasteland | +200 / -200 |
| Aqua | Umi / Wasteland | +200 / -200 |
| Sea Serpent | Umi / Wasteland | +200 / -200 |
| Machine | Umi | -200 |
| Pyro | Umi | -200 |

## Alteracoes Vanilla globais

- Wasteland passa a desfavorecer Fish, Aqua e Sea Serpent como tipo principal em -500 ATK/DEF.
- Yami passa a favorecer Zombie como tipo principal em +500 ATK/DEF.

## Novas herancas da revisao

- Sword Arm of Dragon: adiciona Warrior.
- Charubin the Fire Knight: adiciona Warrior.
- Vermillion Sparrow: adiciona Warrior.
- Lord of the Lamp: adiciona Spellcaster.
- Kairyu-shin: adiciona Aqua como identidade aquatica bloqueada enquanto o tipo principal continuar aquatico.
- Spike Seadra: adiciona Aqua como identidade aquatica bloqueada enquanto o tipo principal continuar aquatico.
- Ice Water: adiciona Fish como identidade aquatica bloqueada enquanto o tipo principal continuar aquatico.

## Casos especiais

- Metal Fish e Mech Bass: Machine com subtipo Fish. Em Umi, o debuff principal de Machine e removido intrinsecamente e o subtipo Fish aplica +200. Em Wasteland, Fish aplica -200. A remocao do debuff principal nao e descrita no rodape.
- Summoned Skull: Thunder + Zombie. Rodape funcional esperado: `Viewed as [Thunder]/[Zombie], +200 ATK on Umi/Mountain/Yami/Wasteland.`
- Excecoes aprovadas permanecem sem o efeito indicado: Fire Kraken, Misairuzame, 30,000-Year White Turtle, Rare Fish, Marine Beast, Amphibious Bugroth e Giant Turtle Who Feeds on Flames.

## Rodape

- Sem prefixo `PS:`.
- Usa o simbolo grafico do tipo, nao o nome textual do subtipo.
- `+200` usa a cor verde e `-200` usa a cor vermelha.
- O texto curto menciona ATK, embora a mecanica altere ATK e DEF.
- Fields equivalentes sao agrupados com `/`.
