# Melhorias de Rituais + Surrender — v1.3.6

MOD independente para **Yu-Gi-Oh! Forbidden Memories Recompiled**.
Versão v1.3.6 compilada com o SDK oficial **v0.3.1-preview.1** (base
v1.3.5 originária de v0.3.0). Mantém as regras e a
sequência de materiais aprovadas; v1.3.5 corrige a sincronização
entre os três objetos visuais escolhidos e o efeito nativo do Ritual.
Expande de onde os três materiais de Ritual podem vir sem assumir propriedade
sobre as receitas. Receitas Vanilla e receitas alteradas por outros MODs usam o
mesmo resolvedor público de Ritual.

## Regras

- Todo Ritual continua exigindo **exatamente 3 tributos**.
- Fontes possíveis: **Campo -> Mão -> Deck**.
- A prioridade de seleção é sempre Campo primeiro, depois Mão, depois Deck.
- Só depois de empatar a origem entram os critérios de economia: o Ritual gasta
  os materiais válidos mais fracos, usando como referência o maior atributo
  impresso do monstro resultante (DEF quando DEF > ATK; caso contrário ATK).
- `Minimum tributes on Field`: 0..3.
- `Use tributes from Deck`: libera o Deck não comprado como fonte.
- `Minimum tributes in Hand`: mínimo exigido na mão quando o Deck está habilitado.
- `Require Ritual Monster card`:
  - `Off`: comportamento normal do resultado.
  - `Hand`: a cópia física do monstro Ritual precisa estar na mão e é consumida.
  - `Hand or Deck`: aceita a cópia física na mão ou no Deck não comprado.
- `Player must Ritual Summon Ritual Monsters`: impede apenas o jogador humano de
  baixar diretamente da mão monstros que sejam resultados de Ritual. A CPU não
  é afetada.
- Quando `Player must Ritual Summon Ritual Monsters = ON` e
  `Require Ritual Monster card = Hand or Deck`, os monstros resultantes de Ritual
  do jogador são movidos de forma estável para o fundo do baralho antes da mão
  inicial. Uma única cópia fica como a última carta do Deck.

## Compatibilidade de receitas

O MOD não depende de nenhum editor/MOD de receitas. O fluxo é único:

1. A cadeia normal de `Duel_CheckRitual` é a autoridade final.
2. Uma receita visível nas tabelas atuais é usada apenas como caminho rápido.
3. O trio físico escolhido é validado pela mesma cadeia pública de hooks.
4. Quando uma receita não pode ser lida diretamente, o MOD usa a cadeia pública
   como oráculo para descobrir quais três cartas são válidas.

Assim, outro MOD pode mudar uma receita sem virar dependência deste MOD.
Sem outro MOD, as receitas Vanilla continuam funcionando normalmente.

## Integração opcional com Efeitos de Rituais v0.78.40

- Usa a API **somente leitura** `ritual-card-effects-test:recipe_v1` via `host->find`,
  com versão de ABI conferida e busca tardia para respeitar a ordem de inicialização.
- A API nunca invoca efeitos, decide invocações, consome cartas nem muda Settings.
- A validação permanece com a **cadeia original de `Duel_CheckRitual`**.
- O caminho opcional é usado **apenas** quando a receita não pôde ser resolvida
  pelo mecanismo atual, existe **exatamente um trio físico candidato** no
  Campo/Mão/Deck, as três funções de papel correspondem ao trio (em alguma
  ordem), e o hook público retorna o mesmo monstro que a API informa.
- Em todos os demais casos, inclusive receitas privadas ambíguas, outro MOD
  ausente/desativado, restrições sobre a carta do resultado, ABI desconhecida
  ou rejeição pelo hook, o algoritmo de v1.3.1 continua intacto.
- Sem Efeitos de Rituais instalado, funciona sozinho. Com ele desativado, o
  produtor retorna ausência de receita e segue o algoritmo anterior.
- A ponte reduz uma chamada redundante à cadeia pública em casos únicos;
  **não há ainda prova de melhora de FPS/frametime**.

## Animação

- **Campo:** usa o comportamento Vanilla original.
- **Mão:** a carta entra em linha reta pela parte inferior, mostrando o verso,
  e converge para o Ritual.
- **Deck:** a carta entra pelo lado do baralho; a iluminação/apresentação
  permanece a do pipeline Vanilla aprovado, incluindo o aspecto mais escuro
  observado durante o Ritual. Quando dois ou três materiais vêm do Deck, eles
  agora partem de posições levemente separadas para não se sobreporem.
- Os modelos 3D dos monstros usados como tributo são ocultados durante a cena
  para não permanecerem de pé dentro da animação.
- Com zero tributos no campo, o MOD cria apenas o anchor necessário para o
  controlador Vanilla colocar corretamente o monstro resultante.

## Otimização v1.3.0

A versão final remove o overlay e todo o estado de diagnóstico das builds de
teste. Os hooks quentes usam fast-paths, os metadados dos candidatos são
capturados uma vez por decisão, os requisitos são pré-calculados antes da busca
de combinações e a busca para assim que encontra o melhor nível possível de
origem Campo/Mão/Deck.

A classificação de monstros Ritual para a regra de fundo do Deck também é feita
uma vez por preparação do Deck, em vez de repetir uma varredura completa para
cada uma das 40 cartas.

Detalhes e invariantes preservados estão em `OPTIMIZATION-CHECKPOINT-v1.3.0.md`.

## Otimização v1.3.1 — processamento menor, mesma seleção

A busca por receita conhecida agora cria listas de candidatos elegíveis por
requisito, em vez de percorrer o mesmo conjunto inteiro a cada posição do
trio. Também descarta antecipadamente níveis de Campo/Mão/Deck inviáveis.
No fallback de compatibilidade, a busca ignora pares que não podem completar
a distribuição de origens desejada; a ordem das chamadas ao oráculo externo
continua a mesma.

Comparação automatizada old-vs-new: 50.000 estados aleatórios para cada um dos
dois caminhos, nenhuma escolha divergente. No fallback, mesmo número e ordem
das chamadas externas. Benchmarks e limites no arquivo
`PERFORMANCE-REPORT-v1.3.1.md`. Não foi realizada validação de FPS no jogo.

## Build

Compilado nesta atualização com o SDK oficial `v0.3.1-preview.1` para PC de 32 bits e Windows de 64 bits:

```sh
python sdk/tools/build_mod.py --target i386 MelhoriaDosRituais
```

## v1.3.3 — comunicação opcional com IAs de Ritual

Sem alterar seleção, efeitos, animação, fontes, ordem dos materiais ou as cinco opções de Settings, o MOD publica **`melhoria-dos-rituais:rules_v1`**, via `host->provide`, após instalar os hooks.

O contrato versionado está no cabeçalho `ritual_rules_provider_v1.h` e possui uma função **somente de leitura** `read(snapshot, sizeof(snapshot))`. Ela retorna a política vigente: exatamente 3 materiais, fontes permitidas (Campo, Mão, Deck), `min_field`, `min_hand`, `require_result` e restrição que se aplica exclusivamente ao jogador humano. Os valores são lidos dos Settings deste MOD em cada chamada, nunca copiados permanentemente pelo produtor.

O callback retorna 0 quando Melhoria não está aplicado, mesmo que a tabela de serviços ainda esteja carregada. Outros MODs podem ignorá-lo completamente. A interface não pode executar invocações nem mudar Settings. Sua única finalidade é permitir planejamento por outra IA sem copiar a lógica do Melhoria.

**Verificação:** compilações i386 e x86_64-windows com o SDK v0.3.0; testes da API de leitura (valor padrão, variação de Settings, MOD desativado, tamanho de snapshot/ABI) e testes existentes do adaptador. Execução dentro do jogo Windows ainda não confirmada nesta atualização.

## v1.3.4 — visual dos Rituais com múltiplos tributos do Deck

Corrigido caso visto com Heishin 1st/2nd e Dark Magic Ritual: duas cartas
oriundas do Deck eram animadas com as mesmas coordenadas x/z e a mesma
trajetória, aparentando ser um único tributo. Agora cada fonte Deck
recebe uma trajetória distinta, sem modificar o trio selecionado ou consumido.
Com um único tributo do Deck, as coordenadas originais permanecem idênticas.

Escopo: somente `rules.c` e a versão do MOD. Não modifica Ritual Duelists,
Efeitos de Rituais, regras de seleção, cartas do Deck ou outros MODs.
Verificação: teste automatizado com 128 combinações de lado/fontes,
testes anteriores da API e adaptador, e build do SDK. A confirmação visual
no jogo Windows será feita após instalação.

## v1.3.5 — sincronização dos objetos visuais do Ritual (Heishin)

O controlador PC da animação (`effect_22`) faz uma **segunda consulta**,
`Duel_RitualFieldObjects`, depois da seleção do Ritual. Essa função originalmente
retorna apenas monstros de campo do casamento nativo, que pode não representar
o trio **Campo + Deck + Deck** validado por este MOD. Se esse retorno contém
dois objetos, a animação move só duas cartas e pode arrastar um monstro
**não selecionado** (como B. Skull Dragon), deixando seu objeto 2D fora
da zona até outra ação atualizar a posição.

A ponte de v1.3.5 faz essa segunda consulta receber os **mesmos três
objetos** já preparados e preservados pela seleção do MOD, exclusivamente
enquanto um Ritual expandido está ativo. Para Rituais Vanilla/retail e demais
chamadas, usa a função nativa inalterada. A correção de entradas distintas
para cartas do Deck, feita em v1.3.4, foi mantida.

**Sem mudanças funcionais:** não altera receita, identificação dos materiais,
origem dos tributos, consumo, Settings, IA, modelos 3D ou efeitos dos monstros.
Os testes automatizados verificam que o Ritual híbrido usa três objetos únicos
sem arrastar o B. Skull Dragon, que o caminho Vanilla é delegado, e mantêm
os testes de trajetórias, API de regras e integração opcional de receitas.
Compilado com o SDK v0.3.0 para ELF32/i386 e ELF64/x86-64 Windows.
**A confirmação visual definitiva ainda depende de teste dentro do jogo.**


## v1.3.6 — Select-to-Surrender (duelos contra a CPU)

- **SELECT durante a seleção de cartas no duelo** abre a caixa de confirmação
  original do jogo, usada pela função Vanilla `DuelScene_Update` no modo de
  dois jogadores. Não há nova interface ou alteração de fontes/texturas.
- **NÃO / cancelar**: fecha a caixa e retorna ao duelo normalmente.
- **SIM**: converte a confirmação em uma derrota do jogador pela sequência
  normal `DuelScene_UpdateResultOutro` (`gDuel_bWinnerSide = 1`, fase `0xC`),
  permitindo a tela **LOSE** e o registro de derrota usual (quando aplicável).
- A alteração vale para duelos contra a CPU; o quit nativo de dois jogadores
  permanece inalterado. Não abre durante a vez da CPU nem por segurar SELECT.
- Implementado em `surrender.c`/`surrender.h`, com hooks encadeados em
  `Duel_CheckQuitInput` e `DuelScene_UpdateWithSideInput`. A lógica de rituais,
  suas 5 configurações, contratos e seus objetos visuais foram preservados.
- O id `melhoria-dos-rituais`, a biblioteca `rituals` e o nome da pasta
  `MelhoriaDosRituais` **não foram modificados**, para manter Settings e
  compatibilidade de instalações anteriores. O nome exibido mudou para
  **Melhorias de Rituais + Surrender**.
- Validar em runtime: Select→NÃO; Select→SIM→LOSE; campanha e Free Duel;
  duelos de dois jogadores; outras configurações/MODs já usados. Os testes
  de compilação e simulação não substituem a execução do jogo no Windows.
