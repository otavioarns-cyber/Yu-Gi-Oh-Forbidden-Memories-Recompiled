# Rodada 03 — Beta.35 (22 de 22 Fusion Effects integrados) — 30/10/2026

## Escopo e validação

**Integração de código completa para as 22 especificações da Rodada 03.** Os dois efeitos antes pendentes (Flower Wolf #487 e Kaminari Attack #458) foram acrescentados sobre a Beta.34 Preview; os outros 20 permanecem sem alterações de mecânicas nesta revisão.

**Limite:** as duas novas regras passaram por testes de código, simulando seleções e transições de turno, e as quatro compilações (arquivos fonte mod.c/png.c nos alvos Windows i386 e x86_64) foram realizadas; NÃO foi executado duelo real no Windows. Em especial, a seleção de alvo da IA e o bloqueio de ataques requerem verificação in-game.

## Novos Fusion Effects

### Flower Wolf (#487) — Camuflagem Floral
- Com origem real de fusão ou liberação válida de Fusion Effect, Flower Wolf virado para cima não pode ser escolhido como alvo de ataque adversário enquanto existir pelo menos **outro Plant aliado revelado** em campo; a própria carta é Beast e não conta.
- A proteção termina quando o Plant aliado sai, a fonte perde a origem ou a Settings do efeito é desligada.
- A escolha do jogador é recusada antes da confirmação do alvo, preservando os demais alvos e as ações não ofensivas.
- Na seleção da IA, um alvo Flower Wolf protegido é trocado por outro monstro adversário ocupando uma zona legítima, se houver. Se não houver alvo válido, o ataque não é confirmado; não é criado um ataque direto falso.
- Não interfere com dano por Magic/Trap ou outros efeitos; não substitui proteções de batalha.

### Kaminari Attack (#458) — Choque Paralisante
- Ao aparecer em campo com Fusion Effect **de fato habilitado**, marca UMA única vez a restrição para o **próximo turno do adversário**.
- A restrição não depende de Kaminari permanecer em campo. Monstros já presentes e novos monstros do turno bloqueado não podem declarar ataques. Não bloqueia outras ações por regra deste efeito.
- A presença continuada não reaplica o bloqueio por turno; a entrada nova com origem válida pode agendar outra aplicação.
- Mantém registro por slot, ID e identidade de instância, não pelo ID compartilhado entre cópias. Uma carta comprada ou invocada sem origem válida não dispara.
- O mecanismo de seleção trata a proibição sem alterar permanentemente o contador Swords of Revealing Light, e as exceções Metalmorph/Swords não ignoram Kaminari.
- O título <Fusion Effect> é branco durante o duelo depois do uso único; fora do duelo sempre colorido.

## Configurações e descrições
- Acrescentados `flower_wolf_camouflage` e `kaminari_paralysis` em `Settings > Mods`, ativados por padrão (seguindo o padrão da Rodada 03).
- Adicionadas descrições curtas somente às cartas #487 e #458, mantendo a paleta do Fusion Effect, as demais descrições e o comportamento de cores fora do duelo.

## Intocáveis e pendências
- Base de comparação: Beta.34 PREVIEW. Todos os efeitos anteriores, os 17 pares específicos e as receitas Vanilla existentes foram preservados, inclusive a precedência restrita de Skelgon e Rare Fish.
- **Rose Spectre simultâneas:** continua pendente o caso de duas ou mais fontes, já assinalado na Beta.34; não foi inventada regra de empilhamento. O efeito com uma única fonte permanece integrado.
- Não foi modificado Ritual Duelists, Efeitos de Rituais, IA de decks, recompensas, campo Vanilla/DLC ou qualquer outro MOD.

## Arquivos alterados em relação à Beta.34
- `fusion_round3_rules.h`: inclui #487 e #458 como integrantes da rodada.
- `fusion_round3_runtime.h`: avaliação de alvo de Flower Wolf; gatilho e pendência/atividade do bloqueio Kaminari; rastreio por instância e nova entrada.
- `mod.c`: integração às seleções humanas e da IA no Field Actions, passagem de turno, Settings e renderização de título usado.
- `mod.json`: duas descrições, dois Settings e nova versão.
- Testes de manifesto, receitas e runtime, além dos dois objetos compilados Windows.
- Este status e a capa do README.

## Testes concluídos
- SDK v0.3.1-preview.1: `build_mod.py` para i386 e x86_64-windows: PASS.
- `test_round3_runtime.c` (ASAN/UBSAN): PASS, incluindo Flowers, origem, outros Plants, Settings, duas cópias, Kaminari próximo turno, duração, reinvocação e outra cópia sem origem.
- `test_round3_recipes.c`: PASS, 17 pares e ambas as ordens.
- `test_round2_runtime.c`: PASS, 16 efeitos da Rodada 02.
- `run_fusionist.py`, `test_rules.c`, `test_effect_title_instance.c`, `test_effect_title_visual.c`, `test_manifest.py`, `test_round2_manifest.py`, `test_visual_dispatch.py`: PASS.
- Testes automatizados não cobrem integralmente o percurso real de animação, UI, IA e gameplay do Windows.

## Instalação e testes prioritários
1. Fazer backup da Beta.34 ou Beta.33.
2. Substituir SOMENTE a pasta `BalanceamentoFusoesEfeitosAuxiliares`. Não instalar duas cópias do mesmo MOD. Reiniciar o Recompiled.
3. Testar Flower Wolf por fusão: com Plant aliado, sem Plant, Plant face-down, Flower Wolf não fundido, seleção humana e ataques da IA.
4. Testar Kaminari por fusão: nenhum ataque inimigo no próximo turno, inclusive de unidades recém-invocadas; saída de Kaminari antes desse turno; ausência de bloqueio nos turnos seguintes; nova entrada; Settings desativadas.
5. Testar os outros 20 efeitos conforme o prompt original, principalmente Kwagar, Mavelus, Karbonala, Charubin, Flame Ghost, e multiplicidade de Rose.

**Status:** Beta.35 candidata a testes in-game da Rodada 03; não é uma versão verificada por execução Windows.
