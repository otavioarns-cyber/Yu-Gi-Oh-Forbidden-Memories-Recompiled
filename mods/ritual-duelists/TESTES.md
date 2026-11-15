## 2026-11-06 — Alpha.8 (remoção do visualizador de drops interno)

- Base exata recebida: `Ritual Duelists(9).zip` (Alpha.7). Terceiro verificado: `droplist 1.4.0.zip` (PauloH-Gudang), **não modificado**.
- Removido somente o viewer e suas dependências visuais/opção de Settings; `rd_drop_rewards.c`, `mod.json.drops`, perfis, IA e persistência preservados. Sem mudança nos campos do savestate.
- Teste de regressão: `tests/run_all_beta.sh` completo, aprovado. O mock de persistência de Blue-Eyes exige a pasta `/mnt/data/rd_p03_work` presente; a primeira execução sem essa pasta falhou também no ZIP original, e a suíte completa passou após prepará-la.
- `tests/test_external_droplist_compat.py`: ausência de callbacks/arquivos/UI antigos e imports `Tables_PoolFor` do Droplist 1.4.0 confirmados; importações dos dois objetos ELF para i386/x86_64-windows presentes nas listas oficiais de exports do SDK v0.3.1-preview.1. Nenhuma exportação global colide além do `MemoriesModInit` esperado para MODs distintos.
- Compilação `sdk/tools/build_mod.py` com SDK Windows v0.3.1-preview.1 (commit `10ee7e5ecf`): i386 e x86_64-windows concluídas.
- **Não executado no jogo Windows**: verificar Triângulo/fecha, mudança de pool, diagnósticos, save/slot, carregamento simultâneo e timing da UI. A lista externa pode exibir probabilidades ponderadas de Blue-Eyes mesmo após esgotamento dinâmico da fonte (o hook de award continua bloqueando a entrega adicional).

## 2026-10-19 — Alpha.7 (Mountain Mage, ajuste de deck)

- `tests/test_v3_forest_anubisius_mountain.c` valida `Mountain Mage` sem Gaia #37, com Seiyaryu #358 peso 144, ritual preservado e `High Mage Atenza` com Gaia #37 peso 32.
- `tests/audit_pools.py` recalcula hash da pool Mountain e mantém hashes originais das outras 28 pools, 60 entradas e total 2018+30. O arquivo `spec/Ritual-Duelists-Pools.csv` é regenerado pela auditoria.
- Não há ajuste de `drops` no `mod.json`; não mudou tela de visualização, controles, IA nem efeitos.

## 2026-10-18 — Drop List v0.4.0 Alpha.2

- Teste unitário `tests/test_droplist.c`: API do SDK simulada, leitura dos três setores do duelist, IDs/pesos Vanilla, consulta ao `Tables_PoolFor` com drop alterado, contagem de cópias no deck/trunk, filtro `<3`, troca de pool, modal/input, falha de leitura sem crash. Executar pela suíte `tests/run_all_beta.sh`.
- No jogo Windows (pendente): Free Duel → duelista desbloqueado → Triângulo → conferir porcentagens, filtros, troca de pool e retorno ao menu; entrar e sair de Free Duel repetidas vezes; testar com outro MOD que altere uma drop pool.
- Nenhum teste de execução real Windows foi realizado neste ambiente.

## IA — v0.3.5 Beta.2 (nova suíte)

- `tests/test_ai_policy.c`: identidade de Ritual por registro/ID/origem, veto de fusão nos dois ordenamentos, invalidação ao sair/trocar carta; prioridade apenas a mágicas Yamadron criadas (`deck_index == -1`) e viradas para baixo, sem incluir cópias normais ou cartas já usadas.
- `tests/run_all_beta.sh`: 23 suítes C e auditoria Python dos 19 perfis. **Não substitui um teste real no Windows:** testar fusão planejada e efetiva após Ritual, e três Magics geradas no início de cada turno com Yamadron presente.
- Novo marcador dentro da estrutura única de save do MOD exige versão 2. Abrir duelos novos para validar, não reinterpretar saves anteriores como prova de atribuição ritual.

## Verificação v0.3.5 Beta.1 — Forest / Anubisius / Mountain

- `tests/run_all_beta.sh`: **22 suítes C aprovadas** (21 existentes + testes de pools e receitas V3), mais auditoria Python. Incluem os 19 perfis e os modos Story/Free Duel ON/OFF, bundles indivisíveis, 40 cartas, cópia máxima três e receitas editadas.
- `tests/test_v3_forest_anubisius_mountain.c`: magias e resultados corretos, 83/162/60 cartas reais, pesos-chave exatos, exclusões e Magic apenas no pacote.
- `tests/test_v3_recipes.c`: **18.000 decks** dos três novos perfis, receitas fixas e provider-style genéricas, Time Wizard fora de Javelin, invalid recipe sem pacote parcial.
- `tests/audit_pools.py`: exatamente **2018+30** para cada um dos 19; digests SHA-256 das pools completas do anexo (incluindo novos perfis), CSV consolidado.
- Auditoria extra de diferenças: **17 / 19 / 57** IDs alterados conforme prompt; os **16 perfis anteriores não tiveram qualquer ID/peso modificado**.
- Build oficial `sdk/tools/build_mod.py`: **i386 ELF32** validado por exports do SDK; **x86_64-windows ELF64** compilado sem lista de exports x64 para checagem. Alvo game buildid `ebe530a3`, commit `663626b1d3`.
- **Pendente:** teste visual e de comportamento no Windows. Não foram alterados drops, Gaia/Darkfire auxiliary effects ou outros MODs.

### Verificação manual sugerida

1. Com `Guaranteed Ritual Package in Free Duel` ON provisoriamente, testar Forest / Anubisius / Mountain e verificar a invocação com receita realmente ativa (Melhoria de Rituais).
2. Observar os decks: Forest (Leogun/Garvas/Firewing Pegasus); Anubisius (Moths/Cocoon/Time Wizard); Mountain (Gaia the Dragon Champion, mágicas de dano e Yamadron).
3. Confirmar Meteor B. Dragon raro e que NÃO há novos efeitos Gaia the Dragon Champion/Darkfire Dragon ou drops alterados.

## v0.3.4-beta.1 — três perfis novos

- **20 executáveis C e uma auditoria Python**, executados com SDK v0.3.0. O teste principal gera **115.200 decks** (16 perfis × 3 modos × 2400), o teste de receitas genéricas deste lote gera **23.400** (3 perfis × 2 conjuntos de condições × 3 modos × 1300), e a regressão de raridade Rex executa **90.000**.
- Cada pool: 2018 reais + pacote virtual 30. Confirmação de 69/78/90 IDs reais e ausência de duplicatas, 40 cartas físicas por deck, máximo 3 cópias por ID e no máximo 3 pacotes completos, nenhuma Ritual Magic ponderada normalmente, Story e Free ON com 1–3, Free OFF com 0–3 e default OFF intocado.
- Teana #20: pool positiva exata, 45 Female (incluindo #531/#708), 17 Fairies de suporte e 7 magias/traps; sem #62/#63/#330–335/#348. Ocean #21: Reptiles e #700 excluídos da pool normal, Whale pronta #718=30, mudanças Vanilla exatas. Secmeton #22: 55 exclusões, 20 aumentos, #710 pronta=48, #692 apenas pelo pacote. Todos os perfis antigos preservados byte a byte nas tabelas de pesos.
- Novas simulações de receitas usando roles dinâmicos (Fish e Mech Bass #441; Kanikabuto #451 + Turtle + Aqua), inclusive carta obrigatória fora da pool normal. Receita impossível (material igual ao resultado Ritual) rejeitada de forma atômica, sem pacote parcial.
- **Limites:** simuladores C não executam a cena de duelo, aplicação dos Fields nem os efeitos exclusivos, que pertencem ao MOD Efeitos de Rituais. Builds ELF32/i386 (símbolos aprovados pelo SDK) e ELF64/Windows (sem lista de exports x64). Não houve execução do jogo no Windows.
- **Teste manual:** instalar as duas atualizações separadas, habilitar Ritual Duelists, deixar `Deck ritual recipe source = Auto`, usar `Guaranteed Ritual Package in Free Duel = ON` temporariamente. Duelo Tea/Teana #20: Cosmo ritualizada e pronta, auto-bônus +500 por Female de ambos lados incluindo #531/#708; Ocean #21: Fortress Whale, bônus DEF por Fish da mão + Deck restante e proteção sob Umi inclusive Whale; Secmeton #22: Crab Turtle por Ritual, Umi e bônus +200/+800 nos outros monstros aquáticos. Repetir com Free OFF. Verificar que os efeitos não disparam para monstro pronto não ritualizado.

## v0.3.3-beta.2 — prioridade de ativação

- **17 suites C** com `tests/run_all_beta.sh` e auditoria Python passaram. O novo `tests/test_activation_priority.c` testa Bakura, Pegasus, Mage Soldier e Martis com um monstro aliado mais forte que o Ritual, magia já colocada e `Duel_CheckRitual` simulado como legal: a IA solicita a invocação mesmo com `score<0`.
- Cobertos também: checker recusando Ritual, magia somente na mão, emergência com desvantagem, proteção existente de escolhas ofensivas letais (sem modificação no fluxo anterior).
- A suíte existente revalida 93.600 decks de 40 cartas / 13 perfis / 3 modos e regras de 0–3 ou 1–3 pacotes; teste da raridade Rex reexecuta 90.000 decks. Nenhum peso da pool alterado.
- Compilação SDK oficial v0.3.0 em ELF32 i386 com verificação de símbolos; ELF64 Windows compilado, com ressalva do SDK sem lista de símbolos de exportação x64. Execução de gameplay Windows **não realizada**.

## v0.3.3-beta.1 — novos perfis (Bakura, Pegasus, Mage Soldier)

- Testes `tests/run_all_beta.sh`: 16 executáveis C e 1 auditoria Python passaram. Foram gerados 93.600 decks no teste de pacotes (13 × 3 modos × 2.400), cobrindo o limite de 3 cópias por ID, 40 cartas, pacotes indivisíveis e Story / Free OFF / Free ON. Free Duel OFF gerou decks sem pacote; Story e Free ON nunca geraram zero.
- Comparação independente da pool Vanilla no disco USA para cada novo perfil: somente 12 deltas de −5 por personagem, 1 novo monstro pronto de peso 30 e 1 pacote de peso 30. Os outros 10 perfis permaneceram bit a bit idênticos em suas pools.
- Pegasus com receita modificada genérica (Machine/Fiend); Mage Soldier com receita defensiva genérica; cartas obrigatórias fora da pool; fallback seguro da arquitetura e proteção de cartas prontas vs ritualizadas.
- Builds oficiais v0.3.0: ELF32 i386 aprovado com validação de símbolos pelo script oficial; ELF64 x86_64-windows compilado, sem exports x64 no SDK. Não foram executadas partidas reais no Windows.

# Ritual Duelists — Testes da v0.3.2-beta.2

## Execução automatizada

SDK oficial: `yfm-redecomp-v0.3.0-windows/sdk`. Rodar com `SDK_INCLUDE=/caminho/para/sdk/include bash tests/run_all_beta.sh` na pasta do MOD. **15 conjuntos de testes passaram** (Package, pools, planner Isis, receitas, adaptadores de providers, diagnóstico, estratégia, regras adaptativas, proteção do campo, mão, acesso ao deck, busca de tributos e Beacon).

**Simulação Ritual Package:** 7 duelistas × 3 modos (Story/Free OFF/Free ON) × 2600 decks = **54.600 decks**. Cada deck gerado tinha 40 cartas, no máximo 3 cópias por ID e de zero a três pacotes atômicos conforme a configuração. Modos que garantem pacote nunca retornaram zero no teste. As frequências abaixo são observações da implementação, NÃO percentuais fixados como regra.

```text
Isis Story: 0=0 1=1549 2=914 3=137 / 2600
Isis Free OFF: 0=1376 1=1026 2=161 3=37 / 2600
Isis Free ON: 0=0 1=1545 2=930 3=125 / 2600
Jono Story: 0=0 1=1742 2=791 3=67 / 2600
Jono Free OFF: 0=1497 1=934 2=161 3=8 / 2600
Jono Free ON: 0=0 1=1730 2=807 3=63 / 2600
Jono 2nd Story: 0=0 1=1676 2=856 3=68 / 2600
Jono 2nd Free OFF: 0=1497 1=946 2=153 3=4 / 2600
Jono 2nd Free ON: 0=0 1=1660 2=865 3=75 / 2600
Weevil Underwood Story: 0=0 1=1645 2=911 3=44 / 2600
Weevil Underwood Free OFF: 0=1446 1=983 2=169 3=2 / 2600
Weevil Underwood Free ON: 0=0 1=1671 2=878 3=51 / 2600
Shadi Story: 0=0 1=1571 2=868 3=161 / 2600
Shadi Free OFF: 0=1472 1=925 2=188 3=15 / 2600
Shadi Free ON: 0=0 1=1551 2=888 3=161 / 2600
Heishin Story: 0=0 1=1606 2=882 3=112 / 2600
Heishin Free OFF: 0=1459 1=969 2=167 3=5 / 2600
Heishin Free ON: 0=0 1=1607 2=885 3=108 / 2600
Heishin 2nd Story: 0=0 1=1599 2=829 3=172 / 2600
Heishin 2nd Free OFF: 0=1445 1=936 2=209 3=10 / 2600
Heishin 2nd Free ON: 0=0 1=1601 2=845 3=154 / 2600
PASS Ritual Package distributions, legal decks, dynamic materials, cap and IDs 721/722
```

Testes especiais: três pacotes forçados, receita com três cópias de uma mesma carta por pacote, impossibilidade de pacote extra por limite de cópias, carta resultado proibida como tributo, receita de cardinalidade diferente de três rejeitada sem falsificação, preferências de materiais genéricos fora da pool, saída da pool normal com Magician of Black Chaos #722, inclusão de Dark Magic Ritual #721 e pesos dos sete perfis.

**Settings:** mod.json declara `guaranteed_package_free` booleano com `default:0`; mod.c também solicita ao host o valor com fallback zero. A simulação `Free OFF` usa este valor e mostrou decks com 0 pacotes. A plataforma é responsável pela persistência da preferência; sua verificação ao reiniciar exige teste no jogo.

**Compilação oficial:** `sdk/tools/build_mod.py` para i386 (ELF32) e x86_64-windows (ELF64). i386: símbolos indefinidos resolvidos na lista oficial exports.i386.txt (40/40). O SDK não inclui exports para Windows x64; embora a compilação tenha passado, o carregamento x64 não foi testado. **Nenhum gameplay Windows foi executado para esta nova Beta.**

## Roteiro de validação manual

1. Instale somente a pasta do Ritual Duelists substituindo a versão antiga. Mantenha os MODs Efeitos de Rituais e Melhoria dos Rituais que já estavam funcionando; nunca misture pastas.
2. Em `Settings`, deixe `Deck ritual recipe source = Auto` e `Guaranteed Ritual Package in Free Duel = OFF`; abra **um novo** Free Duel e verifique no diagnóstico pacotes de 0 a 3 (nem sempre haverá um); `Story` deve mostrar ao menos 1.
3. Habilite manualmente o toggle no Settings, reinicie como solicitado e faça Free Duel novo: deve mostrar de 1 a 3. Desative novamente e teste. Confirme que a escolha manual persiste ao fechar/abrir o jogo.
4. Repita primeiro Isis, Jono, Weevil e Shadi para observar regressões, depois Heishin 1st e Heishin 2nd. Verifique a presença de Dark Magic Ritual #721, a obtenção de materiais corretos e a possível aparição do Magician of Black Chaos #722 pronto por sorteio normal.
5. Heishin 2nd não deve receber pacote de Cosmo Queen; esse Ritual saiu da pool como aprovado. Um Magician of Black Chaos pronto não deve ativar efeitos reservados à invocação por Ritual.
6. Se ocorrer fallback, envie log de `RD DIAG` e vídeo: registrar qual receita estava ativa e qual carta faltava. Diferencie execução de Ritual da preparação proposta pela IA.

## Limites

- Interface de leitura de receitas privada depende da versão efetiva do Efeitos de Rituais e Settings ativos.
- 1–5 tributos continuam suportados pelo planejador, mas Ritual Package universal tem **exatamente 3 materiais**; receita incompatível provoca fallback do gerador, não inventa uma receita alternativa.
- Confirmar em runtime se os IDs 721 e 722 são reconhecidos em todas as ações da CPU.
- Não foram modificados MODs externos, a decompilação de Unchiga nem a AI de combate Vanilla.

## 14/10/2026 — v0.3.2-beta.1 (expansão)

- Discos conferidos: pools USA Vanilla por `WA_MRG.MRG` (Rex ID 9: 83 entradas, 2048; Martis ID 28: 120 entradas, 2048; Desert ID 27: 58 entradas, 2048).
- **14 suítes C + auditoria Python** aprovadas. Simulação: **72.000 decks** (10 perfis × 3 modos × 2.400), 40 cartas, até três cópias por ID. Story e Free ON sempre tiveram 1–3 pacotes; Free OFF produziu 0–3 dependendo do sorteio, com zero efetivamente observado.
- **Rex:** 26 cartas reais, 2018 pontos; 14 Dinosaurs somando 1049, outros monstros somando 565, Wasteland/Equip somando 404. Tri-Horned #705=15, Red-Eyes #82=20, Bracchio #509=20.
- **Martis:** 121 cartas reais e 2018 pontos; dez reduções 8→1, #705=30 e #719=50.
- **Desert:** 57 cartas reais e 2018 pontos; Zombies/Dinosaurs/Rocks/Magic-Trap = 932/283/219/584. Simulação com receita Vanilla (Temple of Skulls + Insects fora da pool) e receita modificada (Temple + dois Zombies da pool).
- **Weevil:** 72 cartas reais; Time Wizard #16=25. Alvos dos cinco cortes do design atingidos; compensação de 12 pontos em #478/#479/#485 descrita no HISTORICO. Nenhum Ritual Package usa Time Wizard.
- **SDK v0.3.0:** compilação ELF32 i386 e ELF64 x86_64-windows; os símbolos de ELF32 foram validados pelo SDK oficial. O SDK não oferece tabela correspondente de exportações x64 e registrou compilação x64 sem essa verificação.
- **Não validado:** execução real no Windows; efeitos específicos de Tri-Horned/Dokurorider sob Efeitos de Rituais, cálculos de tipos secundários do Field Effects instalado e Time Magic em campo da CPU. A distribuição foi validada em fonte e simulador, não em partidas completas.

### Roteiro manual da próxima rodada

1. Teste Rex com o Ritual Package no Story e verifique #680 e tributos reais; confirme que Tri-Horned *pronto* aparece apenas por sorteio e não ativa efeitos de Ritual.
2. Teste Martis: Tri-Horned por Ritual, mas Dokurorider pronto apenas como carta normal, nunca como Ritual desse perfil.
3. Teste Desert: com Efeitos de Rituais em Auto, pacote deve adaptar os dois Zombies; sem Efeitos de Rituais, deve usar #479 e #485 *via pacote* apesar de ausentes da pool normal.
4. Teste Weevil: Time Wizard pode surgir na pool e usa suas mecânicas normais; não deve contaminar o pacote Javelin. Revise a compensação de −12 dos pesos no CSV.
5. Regredir Isis/Jono/Heishin e alternar Free Duel Guaranteed Ritual Package OFF/ON; default OFF preservado.

## 14/10/2026 — Testes adicionais da beta.2

- `test_field_guard.c`: protege Dokurorider #719 ritualizado contra fusão da CPU, inclusive com estado visual transitório e condição de emergência; com campo cheio escolhe a zona expendável de menor valor.
- `test_ritual_strategy.c`: Martis com Ritual #680 setado, checker verdadeiro e monstro de 3500 ATK em campo tem decisão de invocação ativa; com checker falso não ativa.
- `test_rex_ready_distribution.c`: 90.000 decks Rex com 40 cartas, até 3 por ID e #705 exclusivamente como carta normal (peso 15, nunca pacote). Frequências medidas no HISTORICO.
- Regressão `tests/run_all_beta.sh`: 15 testes C e auditoria de 10 pools; compilação ELF32 i386 com símbolos conferidos pelo SDK, ELF64 Windows compilada (sem lista de símbolos x64).
- Faltam testes reais com Efeitos de Rituais e Melhoria de Rituais ativos, em especial 719 protegido e 705 ativado por Martis.


### Beta.3 — Bakura
- Teste `tests/test_bakura_emergency.c`: três materiais nomeados reais disponíveis, magia baixada, adversário mais forte mas sem letal, proposta de tributo no campo permitida; legalidade externa continua obrigatória, emergência letal bloqueia preparo arriscado.
- 18 suítes C e auditoria de pools passando em ambiente simulado (incluindo 13 perfis). ELF32/i386 validado pelo SDK e ELF64/Windows compilado sem export list x64.
- Pendência: repetir em jogo Bakura com Free Duel Package garantido ON, registrar diagnóstico Detailed se `Duel_CheckRitual` continuar NEGATIVE.


### v0.4.0-alpha.6 — Verificação visual da Drop List 1.1

No Free Duel: Triângulo abre; verificar abas douradas em S/A-POW, B/C/D e S/A-TEC, barras de chance e 3/3 com sinal verde. Usar L1/R1 para mudar pool, X para filtrar, Quadrado para ordenar, setas para selecionar e L2/R2 para pular página. Triângulo fecha sem retornar ao menu anterior; Círculo não fecha enquanto a janela está aberta. Conferir Blue-Eyes esgotado no Seto/Kaiba/Seto 2nd, e os drops atuais de Heishin 2nd nas duas categorias. Confirmar legibilidade em tela cheia e janela menor. Não confundir build automatizado com teste em gameplay.
