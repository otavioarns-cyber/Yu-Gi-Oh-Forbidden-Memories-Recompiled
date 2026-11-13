# HAL-24 — controller/planners/materialização e quotas

[Relatório A–F](../../notes/HAL24_RITUAL_DUELISTS_DUAL_GATE.md) · [insumos por SHA256](../../notes/hal-24/INPUTS.json) · [matriz](../../notes/hal-24/MATRIZ.md).

O teste principal **não escreve pending, seleção/plano ou estado de quotas**. Usa manager/loader/hooks oficiais sem o diff do PR17, planner RD e planner AI dos objetos API24, dez unidades nativas reaproveitadas, observer próprio somente leitura e callbacks reais de hand/setup/swap/frame do Dual. A consulta ao estado registrado do Dual é somente leitura, vinculada ao objeto de hash exato; não constitui API pública.

Dependências locais: `MEMORIES_AI_DIR` (ai-hard-mode.o/mod.json oficiais), `MEMORIES_DUAL_DIR` (dual-play.o/mod.json privados do PR10, autorização local do operador), `MEMORIES_RD_DIR` (ritual-duelists.o/mod.json do PR13), `MEMORIES_RITUALS_DIR` (rituals.o/mod.json do PR9), `MEMORIES_ENGINE_OBJ_DIR` (dez unidades i386 da base), `MEMORIES_SDK_EXPORTS` (exports.i386.txt). O objeto Melhoria é usado somente nos layouts Field/Hand/Both; não se cria um provider do Dual. Todos os hashes são conferidos antes/depois.

```sh
export MEMORIES_AI_DIR=/caminho/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/dual-play-privado
export MEMORIES_RD_DIR=/caminho/ritual-duelists
export MEMORIES_RITUALS_DIR=/caminho/melhoria-dos-rituais
export MEMORIES_ENGINE_OBJ_DIR=/caminho/game32/obj
export MEMORIES_SDK_EXPORTS=/caminho/sdk/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='qemu-i386-static -L /caminho/sysroot'
python3 tests/hal24/run.py --output /tmp/hal24-resultados
```

GCC deve dispor de libc/libpng/pthread/math i386; runner pode ficar vazio para execução nativa i386. Usar saída nova. Somente harness/observer próprios são compilados. Os pacotes e os objetos da engine não são recompilados. Saída1 mantém FAIL diagnósticos; saída2 significa bloqueio/incompletude. Nenhum FAIL autoriza patch automático.

São132 perfis de ação, mais18 reexecuções em processo novo com os mesmos settings =150 processos. A fase `reboot` reinicia **o cenário sintético** com bibliotecas novas, não retoma um duelo/savestate. O0 inicializa H,D,RD; O1 H,RD,D; O2 D,RD,H, com DMC ON antes do RD e observer ao final. Primeiro carregar H/D/RD, OFF/ON, reload e candidato AI restart:true são testados entre preparação de perfis, nunca no intervalo planner→swap. O restart original do RD é preservado. Os logs distinguem preferência salva e atividade efetiva.

| Variante | Estado inicial sintético |
|---|---|
| 0 | 535,99,110,111,112 na mão; ritual680 e material571 nos próximos registros. |
| 1 | 680,6,47,344,112 na mão;571 adiante. |
| 2 | 6,6,47,344,680 na mão (cópias distintas);571 adiante. |
| 3 | 344,343,680,6,47 na mão;571 adiante. |
| 4 | 6,47,571,344,680 na mão;535 adiante. |
| 5 | 535,99,344,111,112 na mão;680/571 adiante. Contraponto principal para duas ações Support. |
| 6 | Materiais6/47/571 no campo;680,344,99,110,112 na mão. Provider PR9, min_field3. |
| 7 | Materiais6/47/571 na mão junto a680/344. Provider PR9, min_field0. |
| 8 | Material6 no campo e47/571 na mão com680/344/112. Provider PR9, min_field1. |

Receita Vanilla680/6/47/571/705 preservada. Não se altera o gerador de decks do MOD: RAM/catálogo são fixtures declaradas, não decks de produção. Field/Hand/Both descrevem **onde as instâncias estão na fixture**. O provider real mantém sources=Field|Hand; min_field0 não significa uma política exclusiva “só Hand”. `use_deck` permanece0. Esses layouts testam planejamento/checker e integridade, não invocação completa nem consumo visual de três tributos.

Com AI OFF, o predecessor da VM retail é um **stub de status de conclusão** (retorna1, sem tocar na seleção). O planner RD toma a decisão a partir do snapshot; não se fornece uma proposta stock fixa como na HAL23. Isso não executa bytecode retail. Com AI ON, os logs contam chamadas a esse stub: o exemplo principal O0/v5 executa ambos os planners reais com **zero chamadas ao stub**. Catálogo e serviços auxiliares continuam modelados em ambos.

O controller nativo prepara snapshot, inicializa o script, chama planners/swap, seleciona e remove a instância física até fase6/7. O observer lê a seleção antes da troca. Identidade da ação, posição no deck e bloco de dados são registrados separadamente. O callback real de mão do Dual atualiza suas quotas. Em seguida uma ponte explícita conclui placement/efeito e sinaliza entrada na fase de campo; os callbacks **reais** de frame do Dual decidem se concedem bônus e reconstroem a mão. Não se escreve `used`, `bonus` ou `pending_return` no teste principal. Essa ponte não é motor completo de efeitos/turnos.

Os testes conferem categoria real versus ledger, categorias repetidas, ritual em bônus, filtragem lógica, instâncias distintas, no máximo uma troca offscreen, conservação de80 identidades, registros não selecionados intactos e rollback de rejeições observadas. Gate ausente não é chamado artificialmente para produzir PASS. Pendings são lidos no símbolo privado do objeto RD exato e nunca semeados/limpos pela fixture principal. A saída distingue casos não alcançados e não substitui seleção vazia por uma jogada inventada.

Os controles históricos foram executados separadamente, com runner/fontes **não modificados do PR14** em ae43436e0e23437642ab45eb4da2b4a9374c2e39:

```sh
python3 tests/hal20-integration/run.py --stage 4 --dual 1 --scenario rd-materialize --output /tmp/hal24-controles-pr14
```

Esse comando deve ser executado na árvore original do PR14, com as dependências documentadas naquele PR, não nesta branch. Ele **injeta** pending e flags de bônus: seus resultados não pertencem aos150 processos principais. Os seis perfis e a saída bruta estão em [controles-pr14](../../notes/hal-24/controles-pr14/RESULTADOS.json). Nenhum fonte/manifest/binário Dual acompanha qualquer árvore publicada desta tarefa.

Não há gameplay Windows, imagem de jogo, bytecode retail, animações completas, savestate real ou integração com todos os MODs. I2/HAL22 e a ocorrência histórica I3 são classificados separadamente; não se implementa HAL21/F1, HAL23 ou guard do PR17.

A classificação separada I1/I2 pode ser regenerada sem reexecutar os testes:

```sh
python3 tests/hal24/classify.py /tmp/hal24-resultados/RESULTADOS.json /tmp/hal24-classificacao.json
```
