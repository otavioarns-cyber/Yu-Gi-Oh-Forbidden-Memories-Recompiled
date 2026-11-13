# Beta.36 — alterações e validação

## Alterados em relação à Beta.35
- `mod.c`: captura ATK/DEF antes e depois dos efeitos temporários da batalha; usa valores anteriores só na inicialização visual dos painéis e interpola campos de exibição após a fase nativa das Guardian Stars. Não toca nos registros de modificadores persistentes.
- `battle_stat_tween.h` (novo): cálculo puro e monotônico de passos da animação, incluindo reduções.
- `mod.json`: descrição breve de Labyrinth Tank #370, incremento da versão.
- `tests/test_battle_stat_tween.c` (novo): variação ATK/DEF, efeitos simultâneos, valores negativos e preservação de bônus nativos.
- `tests/test_manifest.py` e `tests/test_visual_dispatch.py`: nova versão e validação de linhas do Tank.
- `README.md`, estes apontamentos e os binários Windows recompilados.

## Preservado
22 efeitos da Rodada 03, 16 da Rodada 02, Fusionist, Metalmorph, Elegant Egotist, receitas, história, pools, drops, Settings e animações não relacionadas.

## Status de testes
- Compilação Windows i386/x86_64: PASS.
- Regressão 22 Round03 e 16 Round02: PASS.
- Receitas 17 pares, regras, título de efeitos por cópia e Fusionist: PASS.
- Unit-test da interpolação de números em passo nativo: PASS.
- Execução real no jogo Windows: NÃO FEITA; validar timing/visuais in-game.

## Testes sugeridos
Skullbird: +400 ATK e inimigo -400; Marine Beast: -600 ATK; Barox e Bickuribox: trocas ATK/DEF; Amphibious Bugroth: bônus contínuo de Field não devem gerar animações indevidas; Karbonala e Battle Warrior: moeda com ATK temporário; Labyrinth Tank: descrição inteira na caixa.
