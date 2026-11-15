# Beta.43 — atributos HD nas descrições (06/11/2026)

Base: **Beta.42**, que já incluía as seis artes customizadas aprovadas pelo usuário.


- **Bug visual confirmado por screenshot de Giltia the D. Knight (#389):** o ícone DARK na descrição estava em baixa resolução, embora o símbolo no canto da carta fosse nítido. A Beta.42 havia reduzido o próprio PNG para 16×16, provocando pixelização no painel da descrição quando a resolução interna do jogo era ampliada.

- **Correção:** restauradas as seis artes em 256×256 RGBA, com transparência real, margens e alinhamento uniformes. A renderização de texto mantém **exatamente os mesmos marcadores F8 e a caixa lógica de 16×16**. O código já existente usa `TypeIcons_Cell`/`TexturePack_AddBankSprite` e disponibiliza o PNG completo à composição de alta resolução, enquanto `CardArt_IconFromImage` gera automaticamente uma versão reduzida para compatibilidade em resolução nativa. Nenhum sprite é ampliado ou deslocado dentro da descrição.

- **Escopo:** somente `assets/attribute-icons/*.png`, versão e documentação; `mod.c`, objetos compilados, receitas, IA, ressurreição de Mavelus e textos são **idênticos** à Beta.42. O MOD de Efeitos de Rituais **não foi alterado**.

- **Verificação:** imagens RGBA 256×256, alpha transparente nas bordas, padronização centralizada, nomes/IDs dos seis símbolos, limites das descrições, e ZIP íntegro. Os objetos Windows da Beta.42 foram mantidos, pois o código não mudou. A confirmação final da nitidez fica para o jogo Windows em resolução interna maior que 1×, preferivelmente 4×.

**Instalação:** substituir somente a pasta `BalanceamentoFusoesEfeitosAuxiliares` pela Beta.43 e reiniciar o jogo. Para melhor nitidez, use `Video > Resolution > Internal 4x` (a qualidade final também depende das configurações do renderer).

---

# Beta.41 — símbolos nativos de atributos e Fênix com fogo (05/11/2026)

- **Atributos nas descrições:** os efeitos de monstros #531 (EARTH), #473 (FIRE) e #389 (DARK) trocam os nomes de atributos pelo ícone original correspondente; IDs e nomes efetivos devem ser confirmados pela lista do mod.json, e não pelo exemplo deste histórico.
- **Renderização de ícones:** a textura dos seis atributos foi extraída diretamente do WA_MRG.MRG USA enviado pelo usuário. Em vez de trocar o renderer, o MOD acrescenta apenas registros de ícone de tipo reservados (IDs 249–254) e converte seus marcadores F8 0B F0–F5 para o caminho de ícones nativos. Nenhuma carta muda de tipo, e o efeito continua independente dos outros MODs. Se outro MOD declarar os mesmos IDs de tipo, existe risco de conflito de assets; nesses casos, realocar os IDs deste canal visual.
- **Mavelus (#272):** na ressurreição, pede o efeito de explosão de fogo 0x0B usado na destruição do Raigeki, sem a animação de raio 0x10; a ressurreição continua antes da primeira ação do controlador.
- **Testes:** compilações SDK Windows i386/x86_64; testes de descrição, manifestos, componentes nativos e regressões; validação visual da explosão ainda depende de jogo Windows.
- **Distribuição:** substituir apenas a pasta BalanceamentoFusoesEfeitosAuxiliares; o pacote de Rituais v0.78.73 é separado.

---

# Beta.39 — ícones de tipos / Crimson Sunbird / Skelgon (05/11/2026)

Base: **Beta.38**, aprovada pelo usuário quanto a animações, efeitos e uso pela IA. **Somente este MOD** foi atualizado. O MOD independente de Rituais possui pacote próprio, e o MOD de Field Effects foi consultado como referência sem ser modificado.

- **Descrições:** referências genéricas a tipos de monstros substituídas pelos ícones nativos do Forbidden Memories (`{f8 0B XX}`), inclusive em efeitos com vários tipos. Nomes de cartas (como Zombie Warrior) e categorias que não são tipos (como Female) permanecem por extenso. O motor desenha esses ícones diretamente no texto, com o mesmo visual já usado pelo MOD de rodapé; não há nova dependência de execução.
- **Crimson Sunbird (#467):** o alvo sofre −200 ATK/DEF já **durante** o cálculo da batalha. Se sobreviver, a penalidade passa a permanente e pode acumular com ataques posteriores; a camada temporária não soma −200 extra com a penalidade definitiva. Preservada a exigência de origem de fusão e a ativação de Settings.
- **Skelgon (#545):** bônus +800 ATK ao atacar **Dragon-like creatures**, tanto de tipo Dragon principal quanto com **subtipo Dragon**. O conjunto de 20 IDs foi obtido da lista `ST_DRAGON` do arquivo `field_effects.c` no ZIP de referência *Vanilla Field Effects MOD(6)*. A consulta local não precisa daquele MOD ativo. Se novos subtipos forem introduzidos em futuras versões, sincronizar a lista `secondary_dragon_types.h`.
- **Validação técnica:** SDK Windows `yfm-redecomp-v0.3.1-preview.1` com objetos i386 e x86_64-windows; simulações específicas de efeitos e conferência das descrições. Usuário deverá confirmar ícones e cálculos em execução real no jogo. Os demais efeitos, animações, receitas e IA não foram alterados.

**Teste no jogo:** (1) Crimson Sunbird ataca monstro de DEF alta; observe os −200 na batalha e a permanência após sobreviver; (2) Skelgon ataca Dragon de tipo principal e monstro com subtipo Dragon, contrastando com um não-Dragon; (3) confira os símbolos nas descrições e se todas as linhas cabem nas caixas.

---

# Beta.38 — Charubin / Hinotama Soul e retorno seguro de Mavelus (30/10/2026)

Base: **Beta.37**, mantendo THTD (animação Spellbinding Circle −200), Labyrinth Tank e demais efeitos. **Atualizar apenas `BalanceamentoFusoesEfeitosAuxiliares`.**

- **Charubin the Fire Knight (#133) — Hinotama Soul (#210):** confirmada a mesma rota de invocação artificial de Zombie Warrior. A restauração após batalha já passava pelo hook da Beta.37; agora o MOD também mantém sua identidade, carta e miniatura consistentes nas trocas de turno e reconstruções de cena. A proteção vale somente para a *mesma instância ainda ocupando o campo*, não reanima destruídos ou substituídos. Não altera o gatilho original, que exige destruição de Charubin em batalha.
- **Mavelus (#272), “Rise from Ashes” (Fênix):** o turno do controlador continua sendo a condição de retorno, mas a invocação agora ocorre **depois que a compra do turno terminou e antes que o handler de ações da mão ou IA receba a primeira ação**. Não é mais criada enquanto o jogo está trocando turno e distribuindo cartas. A janela fica reservada no turno correto e é consumida uma única vez nessa entrada; sem espaço, a ressurreição é descartada, nunca reagendada para um turno posterior. A verificação de vaga consulta os registros reais das cinco Monster Zones e não substitui monstros ocupantes.
- **Restauração segura das duas cartas criadas por efeito:** preserva nome/ID, ATK/DEF impressos, miniatura/objeto e modificadores legítimos da instância ativa; não intercepta cartas comuns nem reverte uma **fusão real** que transforme ambas as referências do card no novo ID.
- **Validações:** compilação com SDK oficial `v0.3.1-preview.1` i386 e x86_64-windows; testes automatizados Charubin como atacante/defensor e após reconstrução, Mavelus na fase de compra vs entrada de ações, lado humano e IA, campo cheio, retorno único, não substituição de carta normal e preservação da transformação genuína. Verificações estáticas e testes dos demais efeitos passaram. **Não houve execução real do jogo Windows neste ambiente**.

**Checklist de runtime:** (1) destruir Charubin fusionado em batalha; confirmar Hinotama Soul, passar o turno e lutar usando Soul; (2) destruir Mavelus fusionada por batalha ou efeito, passar até o próximo turno do controlador; ela deve aparecer **antes de selecionar uma carta da mão**, sem sobrepor nenhuma outra; (3) repetir com todas as Monster Zones preenchidas, com Mavelus destruída novamente após ressuscitar, e com IA controlando Mavelus.

---

# Beta.37 — estabilidade das invocações por efeito / THTD / Labyrinth Tank (30/10/2026)

Base: **Beta.36**. Atualização restrita ao MOD `BalanceamentoFusoesEfeitosAuxiliares`; nenhuma outra pasta ou MOD precisa ser substituído.

- **Skull Knight / Zombie Warrior:** identificada uma rota de reconstrução de cartas após combate (`func_80024D34`), também utilizada por `3D Monsters`, que interpreta o índice do baralho de uma invocação gerada artificialmente como se fosse uma carta comum. O MOD agora guarda a identidade da instância, intercepta essa reconstrução, usa um índice temporário válido, repõe o registro/atributos/objeto visual e reenvia a arte da carta correta antes de o modelo 3D ser atualizado. Há também recuperação protegida para o Zombie Warrior no início de turno quando **a mesma instância** persiste. Não revive monstros destruídos nem altera cartas comuns. As invocações de Charubin e Mavelus compartilham a proteção de reconstrução, por utilizarem a mesma infraestrutura.
- **Twin-headed Thunder Dragon:** após aplicar os **−200 ATK/DEF permanentes** nos monstros adjacentes ainda presentes, enfileira para cada um a animação nativa do **Spellbinding Circle**, usando o número exibido **−200**. A redução não é aplicada novamente pela animação. O mecanismo visual já existente do B. Skull Dragon mantém seus −500, sem alteração nas regras.
- **Labyrinth Tank:** texto reorganizado em quatro linhas no corpo, com condição **DEF do alvo em defesa >= ATK do Labyrinth Tank**, ausência de dano de batalha e custo de até 500 LP, incluindo derrota ao chegar a zero. Mecânica de combate inalterada.
- **Verificações internas:** compilações da SDK oficial `yfm-redecomp-v0.3.1-preview.1` para **i386** e **x86_64-windows**; testes de regras Round02/Round03 e retorno simulado da carta gerada, incluindo identidade, atributos, imagem solicitada e preservação de modificadores; testes de manifestos e visuais; validação do ZIP final. A recompilação limpa do ZIP produziu código, símbolos e relocations idênticos, desconsiderando metadados de depuração que incluem o caminho local. **Não houve execução real do jogo Windows**, por isso as animações e o retorno de turno ainda precisam ser confirmados no duelo.

**Checklist no jogo:** (1) faça a IA de Skull Knight gerar Zombie Warrior e avance turnos / lute com ele, verificando nome, imagem, modelo 3D e ATK/DEF; (2) destrua uma carta com THTD e confira a animação de −200 nos dois vizinhos, incluindo acúmulo; (3) abra a descrição de Labyrinth Tank para conferir visualmente o espaço e o texto.

---

# Beta.36 — animação de atributos na batalha / Labyrinth Tank

- Base: Beta.35 (Rodada 03, 22/22 efeitos). Não foram alteradas receitas, regras, dano, IA ou origem dos efeitos.
- Para efeitos temporários de ATK e/ou DEF que ativam na batalha, os números nos painéis dos dois combatentes agora são interpolados usando `Stars_DisplayStep` (o ritmo nativo da animação de Guardian Stars). O cálculo efetivo da batalha permanece imediato e correto; a interpolação altera somente os campos de apresentação `D_800EA0E8[*].field_36/field_38`.
- A animação começa depois da apresentação de Guardian Stars, preservando bônus nativos e permitindo bônus, penalidades e trocas ATK/DEF em ambos os participantes. A apresentação é ignorada se não houver alteração de atributos; armadilhas que cancelam a batalha não geram a animação.
- Battle Warrior com moeda também participa da transição visual da troca de ATK.
- Texto de Labyrinth Tank encurtado para 4 linhas no corpo, sem alterar a habilidade ou o custo real de LP.
- O painel de diagnóstico mostrado no vídeo pertence a **Ritual Duelists**, não a este MOD; foi removido em pacote separado Alpha.8.
- Compilado Windows i386 e x86_64; testes automatizados de regras e interpolação aprovados. **A animação ainda requer confirmação visual num duelo real.**

# BETA.35 — Rodada 03 integrada (22 de 22 efeitos) — TESTE IN-GAME PENDENTE

Consulte `RODADA03-STATUS.md`: Flower Wolf e Kaminari Attack foram integrados à Beta.34 Preview. Os 22 efeitos possuem código e testes simulados, mas o combate real no Windows, principalmente a escolha de alvo humano/IA e o próximo turno do adversário, **ainda não foi validado**. O caso de múltiplas Rose Spectres continua pendente de regra aprovada. Para instalação, substitua **somente esta pasta**.

---

# ESTADO VIGENTE — beta.33: correções visuais

Esta atualização parte da beta32. O texto do Labyrinth Tank foi reduzido e quebrado em linhas curtas para caber na caixa. A cor agora é aplicada somente ao título reconhecido do efeito; o recurso que tingia a primeira linha de qualquer canal foi removido para evitar o rosa em Spellcaster e no menu Free Duel. Os demais efeitos e receitas permanecem iguais.

## Instalação

Substitua apenas a pasta `BalanceamentoFusoesEfeitosAuxiliares` por esta versão e reinicie o jogo. `zzz Efeitos Rituais` não foi alterada nesta atualização.

---

# ESTADO VIGENTE — beta.32: rodada de 16 Fusion Effects

Base: **Fusionist-ElegantEgotist-Beta31-Cores-PreDuelo(1).zip**, enviado pelo usuário.
Esta seção prevalece sobre o histórico abaixo. Implementação conforme o prompt
“Prompt_Programador 16 effects.txt” e as quatro decisões finais do usuário.

## Instalação

Feche o jogo. Substitua as duas pastas deste ZIP no diretório de MODs:
`BalanceamentoFusoesEfeitosAuxiliares` e `zzz Efeitos Rituais`.
Mantenha somente uma cópia ativa de cada MOD e reinicie o jogo.
Inclui os objetos compilados de 32 bits e Windows 64 bits, fontes e testes para
**yfm-redecomp v0.3.1-preview.1**, SDK/API vigente do pacote fornecido.
O jogo, a imagem do disco e o SDK não estão incluídos.

O MOD de Rituais recebeu apenas uma consulta opcional de permissão de posição:
Aqua Dragon não tenta colocar em DEF um Magician of Black Chaos cujo efeito
ritual imponha ATK. A consulta é somente leitura e não altera os Ritual Effects.
Sem esse provedor, o efeito mantém compatibilidade com o restante do jogo.

## Efeitos acrescentados

Todos exigem origem real de fusão ou liberação válida pela Elegant Egotist,
conforme a infraestrutura da beta31. Uma cópia pronta do deck não recebe o efeito
só pelo ID. Cada efeito tem uma opção própria, ON por padrão, em Settings > Mods.
As receitas continuam disponíveis ao desligar o efeito correspondente.

| Monstro | Fusion Effect |
|---|---|
| Twin-headed Thunder Dragon | Ao destruir em batalha, −200 ATK/DEF permanentes nos vizinhos imediatos da zona destruída. |
| Skull Knight | Ao destruir em batalha, invoca 1 Zombie Warrior em uma Monster Zone livre. Sem origem de fusão para o gerado. |
| Labyrinth Tank | Ao atacar DEF ≥ seu ATK efetivo, força a destruição normal do defensor e paga até 500 LP, sem dano normal desse confronto. O custo pode zerar seus LP e perder o duelo. Proteções de batalha existentes continuam sendo respeitadas. |
| Thousand Dragon | −300 DEF no alvo em DEF somente durante o ataque. |
| Bickuribox | Troca ATK/DEF efetivos do alvo somente durante a batalha em que ataca. |
| Kaiser Dragon | Protegido de destruição em batalha se houver outro Dragon aliado que não seja Kaiser. Dano de LP normal. |
| Crimson Sunbird | Após atacar, o alvo sobrevivente recebe −200 ATK/DEF permanentes, mesmo se Sunbird morreu. |
| Aqua Dragon | Ao destruir em batalha, muda outro inimigo elegível aleatório de ATK para DEF. |
| Bracchio-raidus | Em Forest, +800 ATK/DEF além do Field normal e proteção contra destruição em batalha por Insects. |
| Soul Hunter | Ao destruir em batalha, cura metade do ATK original impresso da vítima, automaticamente. |
| Great Mammoth of Goldfine | Ao morrer em batalha, −500 ATK/DEF permanentes no destruidor sobrevivente. Funciona atacando ou defendendo. |
| Roaring Ocean Snake | Muda o alvo de DEF para ATK antes do cálculo; a posição permanece. |
| Punished Eagle | O inimigo que ataca uma zona aliada adjacente perde 400 ATK/DEF só nessa batalha. Não protege a própria zona. |
| Mystical Sand | +200 ATK/DEF para os outros aliados EARTH; não beneficia o adversário nem a própria fonte. |
| Empress Judge | Outros aliados Female não são destruídos em batalha; LP continuam normais. |
| Man-eating Black Shark | Ao atacar um alvo cujo ATK anterior à batalha é menor que o impresso, +1000 ATK somente nessa batalha. |

Buffs contínuos são derivados e desaparecem com a fonte/condição. Modificadores
temporários não são gravados nos atributos permanentes nem alteram stats impressos.
Reduções permanentes empilham e continuam depois da saída da fonte. Aplicação
pós-batalha verifica as identidades e sobreviventes efetivos depois do retorno nativo.
Ataques negados e monstros preservados por proteção não geram falsas destruições.

Decisões finais aplicadas:
- Sand: apenas outros aliados EARTH; identificação por Cards_Attribute, não Type/Star.
- Bracchio: imunidade contra Insects exclusivamente em batalha e somente em Forest.
- Snake Hair + Dragon Zombie: somente esse par foi autorizado a mudar de Pumpking
  para Great Mammoth. Todas as outras receitas foram mantidas.
- Shark: usa o ATK líquido antes das alterações exclusivas da batalha. Se +1000 de
  Equip compensou −500, deixando o alvo acima ou igual ao ATK impresso, não ativa.
  Guardian Stars e debuffs aplicados apenas durante a batalha não criam esse gatilho.

## Descrições e cores

As 16 cartas receberam descrições curtas em inglês com o título
`{f8 0A 07}<Fusion Effect>{f8 0A 00}`: lilás no padrão atual, números
benéficos em verde e reduções em vermelho. O renderer de títulos da beta31 foi
preservado, incluindo Library, Build Deck e a arrumação do deck antes do duelo.
No duelo, o título consulta a instância selecionada e a opção do efeito.

Meteor B. Dragon, B. Skull Dragon e Gaia the Dragon Champion foram preservados.
Rose Spectre of Dunn não recebeu efeito nem receita nesta rodada.
Não foram alterados os efeitos de Trap Master, Disk Magician, Holograph,
Elegant Egotist, Fusionist, Time Wizard, decks, pools ou drops.

## Receitas

Auditoria completa resumida em `AUDITORIA-RECEITAS-RODADA2.md`.
Há 13 entradas no manifesto: 12 adições e a substituição autorizada do par do mamute.
A receita de Mystical Sand já existia e não foi duplicada. Todas são comutativas.
Também estão no resolvedor de receitas exatas, para funcionar nas cadeias de Fusionist.
Não foram acrescentadas remoções globais de receitas ou materiais intermediários.

## Validação realizada

- Os quatro objetos (2 MODs × 2 arquiteturas) foram compilados e seus imports
  conferidos contra os exports do SDK. SHA-256 em SDK-PROVENANCE.json.
- O teste C `tests/test_round2_runtime.c` compila o header real de execução com
  o motor simulado, sob ASan/UBSan. Exercita os 16 efeitos, origens e Egotist,
  custo único/fatal do Tank, fronteiras de DEF, permanência e limpeza de stats,
  campo cheio, cópias, lados, Forest, EARTH/Female, negação e sobrevivência protegida.
- `test_round2_manifest.py` verifica textos/opções, as receitas e a preservação
  das cartas e regras anteriores; `test_manifest.py` verifica as regressões herdadas.
- Regressões herdadas de Fusionist, regras auxiliares, seleção/cor dos títulos,
  ponte de 24 Ritual Effects, retenção de marcadores de batalha e Zera/Ultimate passaram.

**Não houve execução do jogo Windows neste ambiente.** Esses testes validam o
código em um motor simulado e a compilação; não substituem o teste integrado no jogo.
Conferir especialmente apresentação do custo e derrota do Tank com 500/499 LP,
visual do Zombie gerado, interação com proteções de outros MODs e ajuste dos textos.

---

## Beta.31 — organização pré-duelo (29/10/2026)

- A fase 0 do Main_RunDuel reutiliza o modo 3 para a edição pré-duelo. Corrigido o detector de contexto para exigir a fase 1 (0x81) antes de consultar ativação/consumo.
- Antes do duelo, no Build Deck e na Library, títulos sempre coloridos; durante o duelo, condições e origens por cópia continuam como na Beta.30.
- Nenhuma alteração de lógica de cartas. Ver NOTAS-BETA31.txt no pacote.

# Beta 27 — condição dos títulos dentro do duelo (29/10/2026)

- Corrigido o canal de renderização da carta em campo: agora o título também é atualizado quando a UI usa um canal nativo diferente do ponteiro global do visualizador.
- `<Fusion Effect>` sem fusão de origem válida é branco no duelo, inclusive quando a carta já está no campo; o mesmo caminho trata `<Effect>` e `<Metalmorph Effect>` quando desabilitados ou consumidos.
- Fora do duelo, as cores de categoria permanecem sempre visíveis.
- A classificação, os cálculos e os efeitos de gameplay não foram alterados.

# Beta 26 — títulos sempre coloridos fora do duelo (29/10/2026)

- Corrigido o caso observado no Build Deck: agora `<Effect>`, `<Fusion Effect>` e `<Metalmorph Effect>` são reconhecidos **pelo conteúdo completo do título**, mesmo se a tela os fornecer inicialmente em branco.
- Fora do duelo, títulos usam suas cores de categoria em qualquer visualizador: laranja, lilás e prata.
- Durante o duelo, títulos sem efeito habilitado/condição cumprida ou consumidos ficam brancos; ativos mantêm a cor.
- Não alterado o texto do corpo, as paletas de cartas, as receitas, o Fusionist, nem a animação de fusão.
- Inclui testes isolados para os cabeçalhos brancos do Build Deck e títulos mistos no mesmo texto; teste in-game ainda necessário.

# Beta 25 — títulos de efeitos (29/10/2026)

- **ATIVO**: `<Metalmorph Effect>` prateado (rampa 4), `<Fusion Effect>` lilás/rosa (`#D78BE8`, rampa 7), `<Ritual Effect>` azul (rampa 2, no MOD de rituais), `<Effect>` laranja (rampa 5).
- **INATIVO / já consumido**: apenas o título fica branco (rampa 0), como o corpo da descrição.
- A rampa 7 era reservada/sem uso no jogo original e agora é definida no `palette_ramps` deste MOD. Isso não altera o azul dos rituais nem a paleta das cartas.
- **Metalzoa via Fusionist + Metalmorph real:** com origem válida, seu `<Metalmorph Effect>` permanece ativo e agora é visível em prata, corrigindo a inversão visual da Beta 24. Nenhuma receita ou regra de invocação foi alterada.
- A animação e os bloqueios equipagem/fusão da Beta 24 seguem inalterados.
- Testes de cor e compilações não substituem o teste visual no jogo.

# ESTADO VIGENTE — Beta.24: títulos de efeitos e equipagens (29/10/2026)

Esta seção substitui as declarações históricas conflitantes abaixo.

- Durante o duelo, somente os títulos de efeitos nas informações da carta mudam de brilho: cor original quando o efeito está habilitado, cinza discreto quando não cumpre a origem/condição ou foi consumido. Fora do duelo, títulos sempre destacados.
- `<Effect>` permanece destacado normalmente; usos únicos já consumidos perdem destaque. `<Fusion Effect>`, `<Metalmorph Effect>` e `<Ritual Effect>` dependem da condição de origem e dos controladores já implementados.
- Metalmorph #658 ou Elegant Egotist #318 como material REAL classificam a transformação como **equipagem** para os bloqueios de regras. Bloqueio de fusões não impede essas transformações; bloqueio de equipagens impede. Fusões ordinárias continuam fusões.
- Harpie Lady + Elegant Egotist ainda resulta em Harpie Lady Sisters nas duas ordens, com a animação original. Transformações com Metalmorph preservam resultados e animação. Não alteramos a animação de fusão.
- O MOD de Rituais recebeu suporte complementar; substituir as **duas pastas** para assegurar compatibilidade. Compatível com Recompiled v0.3.1-preview.1, objetos Windows i386 e x86_64 incluídos.
- Testes automatizados e compilação passaram para os mecanismos alterados; teste visual/funcional no jogo Windows permanece pendente. A detecção dos títulos usa os campos nativos de renderização, sem modificar o corpo descritivo.

# ESTADO VIGENTE — beta.23: ajustes da Elegant Egotist (29/10/2026)

Esta seção prevalece sobre o histórico. Base: ZIP beta.22 entregue e testado pelo usuário.

- Elegant Egotist equipa apenas monstros com efeitos condicionados compatíveis:
  Fusion Effects implementados, Metalmorph Effects implementados, Ritual Effects
  ativos informados pelo próprio MOD de Rituais e novos efeitos registrados.
  Estar na lista de possíveis fusões ou ter qualquer efeito comum não basta.
- Harpie Lady #62 + Elegant Egotist #318 → Harpie Lady Sisters #63, nas duas ordens,
  tem prioridade sobre o equip. A receita continua funcionando com a opção do
  desbloqueio de Egotist desligada, respeitando bloqueios normais de fusão.
- Animação de equip: brilho comum (efeito visual 303), sem espada/escudo.
  Bônus continua ZERO. Não altera ATK/DEF.
- Descrição aprovada: “Achieving true potential never goes out of style. A touch
  of elegance is all it takes to let yours shine.”
- Fusionist e o MOD de Rituais permanecem com suas mecânicas da beta.22.

## Instalação

Feche o jogo e substitua `BalanceamentoFusoesEfeitosAuxiliares` pela pasta deste ZIP.
`zzz Efeitos Rituais` está incluído sem alterações, exatamente como na beta.22;
se já usa a versão 0.78.63-egotist-bridge, não precisa substituí-lo.
Não mantenha cópias duplicadas ativas. Reinicie o jogo.
Inclui objetos de 32 bits e Windows 64 bits para v0.3.1-preview.1 e fontes.

## Efeitos futuros

A API `origin-effects-v1`, descrita em `EFEITOS-FUTUROS.md`, permite registrar
novos efeitos condicionados sem editar a Elegant Egotist. A carta precisa ter
seu efeito implementado e integrado ao contrato; mudar só a descrição não cria
um efeito. Novas cartas no catálogo de Ritual Effects já são consultadas pelo
catálogo do próprio MOD, sem duplicar IDs no MOD de Egotist.

## Validação desta versão

Compilação dos dois objetos de Balanceamento; todos os imports resolvidos pelo SDK.
Testes C de regras, manifest, runtime com ASan/UBSan e ponte dos 24 Ritual Effects.
Cobertura nova: alvo sem efeito negado mesmo com permissão ampla de equips,
registro/remoção de efeito futuro, ativação do lease para esse efeito, opções
ativadas/desativadas, proteção anterior preservada e Harpie nas duas ordens.
Regressões existentes de Fusionist, origem real, desempates e IA passaram.
Não houve execução do jogo Windows neste ambiente. Conferir no jogo o brilho,
a descrição e as duas ordens da fusão de Harpie Lady.

---

# ESTADO VIGENTE — beta.22: Fusionist + Elegant Egotist (22/10/2026)

Esta seção prevalece sobre o histórico abaixo. Base: beta.21 fornecida pelo usuário.

## Instalação

Feche o jogo. Substitua as pastas `BalanceamentoFusoesEfeitosAuxiliares` e
`zzz Efeitos Rituais` pelas duas pastas deste ZIP no diretório de MODs que você já usa.
Mantenha apenas uma cópia de cada MOD ativada e reinicie o jogo.
Os objetos de 32 bits e Windows 64 bits já estão incluídos; não é necessário compilar.
Esta entrega é para **v0.3.1-preview.1**. Não inclui o jogo, a imagem do disco ou o SDK.

O segundo MOD recebeu somente uma ponte opcional para o Elegant Egotist liberar os
Ritual Effects que ele já controla. Sua versão é **0.78.63-egotist-bridge**, baseada
na **0.78.62-encore-clean** fornecida anteriormente. Time Wizard, Ritual Duelists,
Dual Play, campos, decks e drops não foram editados.

## Regras novas

- **Petit Angel #208 + Mystical Sheep #2 #282 → Fusionist #189**, nas duas ordens.
  Esta é a única receita nova/substituída, autorizada explicitamente pelo usuário.
- **Petit Angel #208 + Mystical Sheep #1 #642 → Garvas #483** permanece vanilla.
  A auditoria do disco encontrou 25.131 receitas e nenhuma produzindo Fusionist;
  por isso a exceção acima foi necessária para o exemplo do prompt.
- **Fusionist em segundo lugar:** consulta as receitas ativas, incluindo MODs,
  e escolhe o resultado-monstro de menor ATK impresso; menor DEF desempata.
- **Fusionist em primeiro lugar:** escolhe o maior ATK/DEF somente com origem
  real de fusão, liberação pelo Elegant Egotist ou bypass de diagnóstico.
  Sem isso, conserva a resolução normal. Fusionist + Fusionist nunca funde.
- Empates completos: sorteio uniforme entre IDs de resultado distintos. Receitas
  duplicadas não aumentam a chance. Consulta ambos os sentidos das receitas;
  resultados Magic/Trap/Equip não entram na seleção. Não há lista fixa de parceiros.
- **ATK/DEF:** leitura direta da tabela impressa, após alterações de cartas dos
  MODs. Não usa `Duel_GetBaseCardStat`, pois essa função soma o terreno.
- **Elegant Egotist #318:** Equip sem bônus de ATK/DEF. Libera somente poderes
  Fusion/Ritual/Metalmorph já implementados no monstro; não lhe inventa poderes.
  Os outros requisitos de cada efeito e as configurações individuais continuam valendo.
- Elegant Egotist + Fusionist + material e Fusionist + Elegant Egotist + material
  funcionam na cadeia da mão. A origem de fusão intermediária também é imediata.
- Fusionist habilitado + Metalmorph real pode gerar Metalzoa com seu efeito metal;
  Zoa + Fusionist, substituindo Metalmorph virtualmente, não concede essa origem.
- Efeitos de entrada recém-liberados pelo Equip disparam uma vez, conforme a decisão
  do usuário. Uma segunda consulta de frame não dispara de novo, nem reequipar
  um monstro já habilitado reinicia seu efeito. Ações concluídas não são desfeitas.
- Ultimate #380 pode usar seu Ritual Effect pelo Equip; não recebeu Fusion Effect.

## Estado, remoção e integração

`fusionist_rules.h` contém a seleção dos resultados; `fusionist_runtime.h` integra
as consultas, os intermediários, a IA e a liberação por instância. Consultas de IA
não consomem números aleatórios nem gravam origens no campo. A execução real faz
seu próprio sorteio entre empates de igual força.

Uma nova fusão cria uma nova origem real e não transfere o Equip do material anterior.
A participação de Metalmorph é verificada nos materiais físicos daquela etapa,
não na receita virtual usada na busca. Os registros finais são vinculados ao ID e
à identidade da carta, para evitar herança por reutilização de zona.

No FM, o Equip é consumido na combinação: não existe uma carta Equip separada
permanentemente na zona para retirar depois. A liberação fica associada ao monstro
resultante; termina ao ele sair, ser substituído/fundido ou perder essa associação.
A mudança de controle da mesma instância pode manter a associação. A desativação
visual temporária em batalha não é uma remoção. Reverse Trap inverte bônus de
estatística; como este Equip dá zero, isso não é tratado como retirada do Equip.

A interface opcional `fusion-auxiliary-balance:origin-unlock-v1` oferece consulta de
um número de aplicação e revogação explícita por registro. A ponte do MOD de Rituais
acompanha essa fonte separadamente das marcas internas de efeitos: não chama a
invocação Ritual, não consome tributos e não falsifica o histórico/ranking do motor.
Reutiliza os inicializadores de efeitos existentes. Quando a fonte acaba, retira
só a elegibilidade emprestada; um efeito de Ritual genuíno não depende do Equip.

Settings trazem `Fusionist: Weakest / Strongest Fusion` e
`Elegant Egotist: unlock origin effects`, ambos ligados inicialmente. Desligar
Elegant Egotist encerra as liberações emprestadas; é necessário equipar novamente
para uma nova aplicação. A carta continua sendo Equip de bônus zero.
O bypass anterior de origem permanece disponível, desligado por padrão.

## Verificação realizada nesta entrega

- Compilação com headers/flags do SDK enviado v0.3.1-preview.1: ambos os MODs,
  i386 e x86_64-windows. Os quatro objetos importam somente símbolos exportados.
- i386: GCC. x64: Clang do Zig 0.14.1, triple `x86_64-w64-windows-gnu-elf`,
  convenção Windows e ponteiros G32 preservados; seção `.memories.abi` verificada.
- Teste C do resolvedor e do finalizador real, com motor simulado e sanitizadores
  de endereço/comportamento indefinido: cadeias nas duas ordens, Weakest/Strongest,
  Metalmorph físico/virtual, origem no campo, Ultimate sem Fusion Effect, gatilho
  único, instância incorreta, categorias não-monstro, receitas de MODs assimétricas,
  configurações, escopo da IA e distribuição de 12.000 desempates entre 3 resultados.
- Ponte Ritual: execução do inicializador real e da ponte com motor simulado;
  as 24 famílias liberam uma vez, revogam a origem emprestada e preservam a genuína.
  Efeito desativado e monstro virado para baixo não ganham nova ativação.
- Testes anteriores aprovados: regras/manifesto do Balanceamento, contratos V4
  dos Rituais, Zera/Ultimate e preservação das marcas na preparação da batalha.
- Um teste histórico `test_whale_crab_v07855.py` já falha na base recebida, pois
  procura uma assinatura de função antiga; não foi usado como aprovação desta entrega.
  O benchmark histórico `test_recipe_performance.py` requer informar seu baseline;
  sua chamada padrão aponta para um diretório ausente. Não foi contado como aprovado.
- Fontes de receitas, moedas, arte e demais controladores de Rituais mantidas;
  alterações novas concentradas na ponte e nos pontos de inicialização/reconciliação.

**Não houve teste dentro do executável Windows.** Os testes acima não validam FPS,
animações, ordem de carga de todos os MODs do usuário ou comportamento visual.
As advertências de compilação restantes vêm de variáveis/indentação preexistentes.
LeakSanitizer foi desativado por incompatibilidade com o ambiente; ASan e UBSan ativos.

## Checklist de aceitação no Windows

- [ ] Confirmar as duas receitas Sheep/Petit Angel, nas duas ordens.
- [ ] Sheep #2 + Petit Angel + Metalmorph → Metalzoa com efeito metal.
- [ ] Petit Angel + Sheep #2 + Metalmorph: mesmo comportamento.
- [ ] Elegant Egotist + Fusionist + Metalmorph e Fusionist + Elegant Egotist + Metalmorph.
- [ ] Fusionist comum primeiro: sem Strongest; segundo: Weakest; dois Fusionists: falha normal.
- [ ] Zoa + Fusionist → Metalzoa sem imunidade metal; testar depois Equip no Metalzoa.
- [ ] Repetir as combinações sobre monstros no campo e em ambos os lados/Dual Play.
- [ ] Testar parceiros Magic/Trap com receitas válidas, receitas de outros MODs,
  ausência de receitas, empates completos e cadeias de quatro ou mais cartas.
- [ ] Trocar terreno e adicionar buffs: a escolha por ATK/DEF impresso não muda.
- [ ] Equip em Meteor/Skull/Darkfire: efeito de entrada uma vez; Equip repetido não duplica.
- [ ] Equip em Ultimate comum: Ritual Effect ativo; sem Equip e sem Ritual: inativo.
- [ ] Equip em Yamadron e demais Rituais: gatilhos existentes, contadores e pré-requisitos.
- [ ] Remoção, fusão posterior, substituição de zona, mudança de controle, flip e batalha.
- [ ] Desligar os novos Settings e testar a revogação; testar bypass antigo separadamente.
- [ ] Kamion adversário rejeita Egotist; duplicação dos demais Equips, moeda do Battle
  Warrior, Dark/Mystical Elf, Encore, Time Wizard e os Rituais mantêm o comportamento.
- [ ] Verificar animações, consumo das cartas e tempo de busca da IA com a lista real de MODs.

Fontes e testes estão nas próprias pastas dos MODs. Para recompilar com a toolchain
oficial no Windows, usar `sdk/tools/build_mod.py <pasta-do-mod> --target i386
--target x86_64-windows`. O registro `SDK-PROVENANCE.json` identifica os binários.

---

# ESTADO VIGENTE — v0.1.0-beta.21 (reorganização de Effects, 21/10/2026)

> **Esta seção prevalece sobre os históricos Beta 14–20 abaixo.** Os textos
> antigos sobre Battle Warrior e Mystical Sheep foram preservados como
> cronologia e NÃO são regras em vigor. Um só README contém estado atual,
> cronologia, banco de efeitos e pendências.

## Alterações efetivamente programadas nesta Beta 21

| Monstro | Effect normal | Fusion Effect |
|---|---|---|
| **Kamion Wizard #115** | Rejeita novos Equips adversários, descarte Vanilla de incompatibilidade; preserva Equips antigos. IA mantém aprendizagem de uma falha. | Duplica bônus de Equip aliados, incluindo Kamion e Equips anteriores; **não acumula**; reverte ao sair. |
| **Mystical Sheep #1 #642** | Nenhum custom ativo. | Nenhum custom ativo. |
| **Battle Warrior #100** | **Moeda de 50%**, contra oponente Level >=4; troca apenas ATK até terminar a batalha, respeita imunidades e Encore. **Não exige fusão.** | Nenhum custom ativo. |
| **Dark Elf #551** | Converte **50% da cura real** do controlador em dano direto ao adversário; sem requisito de fusão. Não é Explosive Damage. | Nenhum novo. |
| **Mystical Elf #002** | Quando face-up, cura **1000 LP uma vez por permanência no campo**, respeitando o limite real de LP. Não reinicia ao virar face-down/face-up; uma nova entrada pode ativar. | Nenhum novo. |
| **Musician King #495** | Encore!!! continua disponível como interface independente para Time Wizard e efeitos de Ritual. | Show Must Go On! apenas com origem válida de fusão. |

**Dark Elf + Mystical Elf**: 1000 LP efetivamente recuperados geram 500 de dano por Dark Elf ativa; com cura real menor, o dano é metade da recuperação efetiva. Não aplicar bônus explosivo de Darkfire a esse dano. O visual da cura da Mystical Elf usa a apresentação nativa de 1000 LP; o efeito não invoca magia/ritual ou retira cartas da mão.

**Sem modificações neste lote:** Time Wizard, Ritual Effects, Duelists, Dual Play, drops, decks, fusões e receitas Vanilla, exceto as alterações independentes que já existiam neste MOD. A moeda do Battle Warrior pertence somente ao Balanceamento.

## Inventário das cartas planejadas, mas sem implementação anterior na Beta 20

- **Trap Master #224:** *não havia código ativo desses poderes* na Beta 20. A imunidade geral a efeitos Trap e a geração de uma Trap por turno **continuam pendentes**: não existe controlador completo de imunidade genérica a todas as Trap neste MOD, e o projeto não definiu pool, momento exato, como escolher a carta e tratamento da zona cheia para a geração. Não foi inventada uma lista de Traps nem adicionada uma receita. **Não considerar Trap Master implementado** nesta Beta 21.
- **Disk Magician #643** e **Holograh #283:** *não tinham efeitos personalizados ativos* nesta versão-base. Portanto, não foi removida lógica inexistente, e nenhuma carta Vanilla/receita foi apagada. O Banco de Efeitos do Disk continua inativo.
- **Dragoness the Wicked Knight #294:** nenhuma atribuição nova; cancelada a ideia anterior de transferir moeda/cura para ela.

## Fusion Effect: lista FECHADA de 48 elegíveis

A lista identifica apenas monstros que **podem** receber Fusion Effect no futuro; não concede automaticamente poder. A implementação só marca origem de fusão para os monstros com Effect já programado. **Blue-eyes Ultimate Dragon #380 está expressamente excluído**. A origem especial do Fusionist é respeitada; não foi alterada neste lote.

| ID | Nome | ID | Nome |
|---:|---|---:|---|
| 015 | Flame Swordsman | 030 | Zombie Warrior |
| 037 | Gaia the Dragon Champion | 043 | Karbonala Warrior |
| 069 | Thousand Dragon | 086 | Barox |
| 092 | Rabid Horseman | 115 | Kamion Wizard |
| 133 | Charubin the Fire Knight | 168 | Darkfire Dragon |
| 189 | Fusionist | 215 | Flame Ghost |
| 217 | B. Skull Dragon | 223 | Roaring Ocean Snake |
| 230 | Rare Fish | 272 | Mavelus |
| 294 | Dragoness the Wicked Knight | 370 | Labyrinth Tank |
| 385 | Bickuribox | 389 | Giltia the D. Knight |
| 409 | Metal Dragon | 427 | Kaiser Dragon |
| 437 | Deepsea Shark | 442 | Aqua Dragon |
| 458 | Kaminari Attack | 465 | Punished Eagle |
| 467 | Crimson Sunbird | 471 | Soul Hunter |
| 473 | Vermillion Sparrow | 482 | Pragtical |
| 487 | Flower Wolf | 495 | Musician King |
| 508 | Cyber Saurus | 509 | Bracchio-raidus |
| 521 | Skullbird | 531 | Mystical Sand |
| 533 | Kwagar Hercules | 545 | Skelgon |
| 564 | Great Mammoth of Goldfine | 565 | Man-eating Black Shark |
| 572 | Empress Judge | 594 | Rose Spectre of Dunn |
| 613 | Twin-headed Thunder Dragon | 617 | Marine Beast |
| 618 | Warrior of Tradition | 639 | Amphibious Bugroth |
| 707 | Skull Knight | 713 | Meteor B. Dragon |

**Elegibilidade não interfere em** `<Effect>`, `<Metalmorph Effect>`, `<Ritual Effect>` ou receitas Vanilla. Cartas elegíveis não recebem `<Fusion Effect>` sem aprovação específica do design. A lista não adiciona efeito para #613 por si só.

## Banco de Efeitos — inativo, SEM destinatário

- **Banco A (ex-Disk Magician):** geração ponderada de Equip — pesos gerais **4**, Bright Castle **2**, Megamorph **1**; Metalmorph/Elegant Egotist excluídos. Pool completo, timing, local de geração e tratamento de zona cheia pendentes. Estes pesos não são drops.
- **Banco B (ex-Disk Magician):** compartilhamento de bônus de Equips entre cartas; não confundir com dobragem de Kamion. Lados beneficiados, acumulação, timing, remoção e alcance pendentes.

A conversão de cura em dano **não** está no banco (dona: Dark Elf). Cura única também **não** (dona: Mystical Elf).

## Validação estática realizada e runtime pendente

- Compilação de MOD `i386` e `x86_64-windows`: executada com SDK **v0.3.1-preview.1**.
- Testes C de regras e testes Python de manifesto atualizados para a nova organização.
- **Não foi executado o jogo no Windows neste ambiente.** O comportamento dos objetos de Equip, animações e eventos do turno deve ser validado pelo usuário.

Checklist de jogo: (1) Sheep sem efeitos; (2) Kamion normal rejeita Equip inimigo mas mantém antigo; (3) Kamion fusionado dobra Megamorph em si e noutros, inclusive Equip anterior, e reverte; (4) Battle Warrior de deck aciona moeda contra Level >=4, somente ATK; (5) Dark Elf converte metade da cura real; (6) Mystical Elf face-down só cura quando revelada e uma vez, inclusive se for virada novamente; (7) as duas Elves juntas recuperam 1000 e causam 500; (8) Encore/Time Wizard/Ritual Effects intactos quando instalados separadamente.

---

# Beta 20 — Musician King, texto de Fusion Effect

- Musician King: `SHOW MUST GO ON!!!` em linha propria, seguido da regra
  `Battle-proof. Can't switch to Defense Mode.`
- O Effect continua sendo Encore!!!: `Encore!!! One retry.` (texto curto para
  manter a caixa legivel). Nenhuma outra regra do Balanceamento foi alterada.
- A ponte do Encore com Time Wizard e corrigida no pacote Time Wizard v0.23.9,
  que deve ser instalado separadamente. Ritual Effects continua v0.78.62.

# Beta 19 — descricao Battle Warrior (21/10/2026)

- Descricao da carta #100 encurtada para caber na caixa do jogo.
- Effect: "Recover LP: deal half as damage."
- Fusion Effect: "50%: swap ATK vs Level 4+ foes."
- Nao altera as rotinas de combate, moeda, Sheep ou outros efeitos desta beta.

# Beta 17 — correções da fase de Equip, batalha e posição (2026-10-21)

- **Mystical Sheep #1:** o jogo executa a aplicação animada do Equip na **fase 7** (`DuelScene_UpdateCardPlacement`), não na fase 8 (`func_8001B170`), como a Beta 16 esperava. O tracker agora lê a fase 7, registra os ganhos nativos reais e aplica a dobragem no registro de batalha. Em cada quadro, remove apenas a parcela extra do quadro anterior antes de acompanhar o próximo passo do Equip; isso evita que o bônus se multiplique indevidamente durante a animação. Considera Megamorph, Equip da mão e Equip direto no campo. A reversão quando Sheep sai permanece com a ledger por instância.
- **Musician King:** cancela a animação de tentativa de posição de defesa da mesma forma utilizada pelo Magician of Black Chaos do MOD Efeitos Rituais: limpa `field_6C` e `update` do DisplayObject, atualiza as flags da carta, sem começar uma rotação que não deve ser concluída. O travamento foi reportado pelo usuário e precisa ser retestado; nenhuma garantia de runtime até validar.
- **Battle Warrior:** a troca temporária de ATK compensa o `defense_modifier` de cada participante, já que o port PC usa `stat_modifier + defense_modifier` na DEF. Salva/restaura também `gDuel_awSavedDefenseModifier` na cena de batalha 3D para não alterar DEF nem herdar valores aleatórios no retorno. Conserva DEF inclusive quando atacado em posição de defesa.
- O MOD do Time Wizard acompanha outra versão, v0.23.6, para controle X / Quadrado e Encore!!! com roleta manual. Efeitos de Rituais v0.78.62 ENCORE CLEAN não é alterado.

**Verificação:** build SDK i386 + x86_64-windows, regressões e ZIP. Sem execução do jogo Windows nesta sessão; todos os pontos acima ainda requerem confirmação durante o duelo.

---

# Beta 16 — reparos de Equips e compatibilidade de roleta

- Mystical Sheep #1: altera a animacao de Equip no calculo *guest* `Duel_CalcCardStats` enquanto a cena de Equip esta ativa. Bônus de campo e da mão; evita hook em `Tables_EquipBonuses` porque e funcao PC nao interceptavel. A origem de fusao e respeitada.
- Preserva Battle Warrior, Musician King, Darkfire, Seiyaryu e regras do Beta 15.
- VERIFICACAO: compilado em i386/x86_64 Windows, testes estaticos. Duelo real necessario.
- BUG AINDA EM INVESTIGACAO: travamento ao colocar Dokurorider na Defesa. Nao declarar corrigido sem reproduzir.

---

# Beta 15 — correções de gameplay e novo Darkfire (2026-10-20)

- **Musician King #495:** descrição reduzida para `Battle-proof. Cannot enter Defense Position.` e trava no comando nativo de rotação, em vez de rodar e desfazer a rotação. O controle de LP da batalha agora confere o valor anunciado pelo duelo e normaliza um caso de débito duplicado contra Musician protegido.
- **Time Wizard:** a ponte Encore está no pacote separado `TIME-WIZARD-v0.23.4-ENCORE-SINGLE-RETRY.zip`; impede que uma ação de Time Magic ganhe mais de uma repetição e expõe o resultado final ao controlador original. O MOD dos Rituais **v0.78.61 ENCORE permanece igual**, continuando a fornecer Encore à Performance of Sword/Magician of Black Chaos.
- **Battle Warrior #100:** a animação da moeda e todos os 28 PNGs de `coin-normal` pertencem agora **ao Balanceamento**, sem dependência da API visual do Ritual Effects. O dano por cura usa a animação numérica de batalha nativa (request tipo 2), imprimindo o valor exato, inclusive valores não múltiplos de 100. Continua sendo metade da cura *efetiva*. Dano de Battle Warrior NÃO pertence ao grupo Explosive Damage e não recebe bônus de Darkfire.
- **Mystical Sheep #1 #642:** o tracker de bônus foi movido para o subestado real do Equip, cobre hand-combo e aplicação em campo. Registra bônus anteriores por instância, duplica apenas uma vez enquanto existir Sheep validamente fusionado, reverte quando ela sai. AI e negativa prospectiva permanecem.
- **Soul of the Pure #341:** permite que cura nativa alcance o teto de 9999 LP mesmo quando o lado está com max_life_points original menor.
- **Darkfire Dragon #168:** `<Effect> Doubles Explosive Damage.` mantém os acertos adicionais anteriores para Sparks, Hinotama, Final Flame, Ookazi, Tremendous Fire, Meteor B. Dragon e Gaia the Dragon Champion (cada cópia adiciona um acerto-base). **Novo `<Fusion Effect>`**, exclusivo de cópia validamente fusionada: gera até 2 Magics de dano explosivo por turno, em zonas Magic/Trap vagas, individualmente 60% Sparks #343 e 40% Hinotama #344. Também gera no turno da invocação fusionada. O código e o carregamento de arte da Magic gerada ficam dentro do Balanceamento, independentes do Ritual Effects.

**Verificação:** compilações SDK em Windows i386/x86_64 e testes unitários não substituem jogar no Windows. Testar especialmente interação com cartas geradas pelo Yamadron, Sheep+equips prévios e as animações de moedas/roletas. Não deixar duas versões do mesmo MOD ativadas.

---

# Balanceamento de Fusões e Efeitos Auxiliares — v0.1.0-beta.13

**Alvo:** Yu-Gi-Oh! Forbidden Memories Recompiled, SLUS_01411 (Windows, builds i386 e x86_64-windows). Compilado contra o SDK oficial **v0.3.1-preview.1**. O jogo Windows não foi executado neste ambiente; as animações e o combate devem ser validados em runtime.

## Instalação

1. Feche o jogo.
2. Extraia a pasta **`BalanceamentoFusoesEfeitosAuxiliares`** para `mods/` e **substitua** integralmente a versão anterior. Não mantenha duas cópias do mesmo MOD.
3. Abra o jogo e confira o MOD habilitado na janela de Mods. As descrições e opções usam o manifesto do MOD; reinicie depois de alterar opções se necessário (`restart: true`).

## Settings — modo de teste (NOVO)

Na janela **Mods**, abra as configurações de **Balanceamento de Fusões e Efeitos Auxiliares**:

**`Ignore Metalmorph / Fusion summon conditions`**

- **OFF / desmarcado (DEFAULT):** mantém a condição normal. `<Metalmorph Effect>` só ativa para a instância realmente fusionada usando Metalmorph; `<Fusion Effect>` só funciona para a instância realmente criada por fusão.
- **ON / marcado (TESTE):** ignora APENAS a condição da origem. Um Metalzoa comprado do deck passa a ter imunidade; um Meteor B. Dragon comprado do deck invocado face-up causa 1000 LP; B. Skull Dragon comprado do deck debuffa ao entrar; Rabid Horseman e Gaia comprados do deck ganham as habilidades de batalha. Efeitos continuam exclusivos das cartas listadas.
- O modo teste **não cria novas receitas** nem modifica a força dos efeitos. Para voltar ao balanceamento, desmarque a opção.

A opção é persistente nas configurações nativas do jogo e o código consulta `host->setting(..., "ignore_effect_conditions", 0)`. O próprio valor default do manifesto e do fallback do código é **0**.

## Família `<Metalmorph Effect>` — preservada

Sete monstros: **Launcher Spider #390, Metalzoa #392, Metal Dragon #409, Metal Fish #438, Giga-tech Wolf #412, Mech Bass #441 e Cyber Saurus #508**. Imunidade a Magic Cards do adversário, durante a permanência face-up no campo; armadilhas, efeitos de monstros e combate permanecem normais. Todas as receitas especiais da Beta 11 foram preservadas.

Descrição prateada/cinza (exatamente):

    <Metalmorph Effect>
    This monster is unaffected by the opponent's Magic Cards.

## Família `<Fusion Effect>` — NOVO

As quatro cartas possuem **descrições em inglês, moldura laranja e título `<Fusion Effect>`**. O jogo base possui paleta nativa de texto branco, amarelo, azul, verde, cinza, laranja e vermelho; nesta versão usamos **azul (`{f8 0A 02}`)** para diferenciar o título do metal prateado. Lilás/roxo exato não está na paleta nativa sem implementar renderização própria.

### Meteor B. Dragon #713

    <Fusion Effect>
    A blazing meteor scorches the sky. Inflicts 1000 damage to the opponent's LP.

- Após **cada invocação válida**, um impacto de **1000 LP** no oponente, uma única vez por entrada.
- Reutiliza a animação de dano direto de **Tremendous Fire** (#347, índice 4). O dano é **efeito de monstro** e faz parte da categoria **Direct LP Damage**: cada Darkfire aliado face-up acrescenta mais um impacto de 1000, com sua própria animação de Tremendous Fire.

### B. Skull Dragon #217

    <Fusion Effect>
    Its terrifying aura weakens all foes. All opposing monsters lose 500 ATK and DEF.

- Ao entrar via fusão válida, reduz **500 ATK e 500 DEF** de **cada monstro adversário presente no campo**, incluindo os virados para baixo. Monstros invocados depois não sofrem o debuff.
- O debuff persiste no registro de cada alvo até sair de campo, e **acumula** se outra cópia de B. Skull Dragon ativar o efeito posteriormente.
- Usa a animação de **Spellbinding Circle** (efeito nativo 0xD, variante 2), sequencialmente em cada alvo. O jogo representa DEF como `stat_modifier + defense_modifier`, por isso diminuir o modificador comum em 500 reduz ATK e DEF sem duplicar a redução.

### Rabid Horseman #092 — Investida Brutal

    <Fusion Effect>
    Its fierce charge shatters defenses. Inflicts piercing battle damage against monsters in Defense Position.

- Se ataca um monstro em defesa e seu ATK superar a DEF, inflige a diferença como **dano de batalha perfurante**, mesmo se o defensor sobreviver por uma habilidade de proteção.
- Não aplica contra monstros em ataque ou se a batalha foi cancelada. Usar a apresentação nativa do dano de batalha, sem ampliar com Darkfire.

### Gaia the Dragon Champion #037 — Cavaleiro Implacável

    <Fusion Effect>
    Its charge knows no mercy. Inflicts 500 damage to the opponent's LP after destroying a monster in battle.

- Após **destruir efetivamente** um monstro adversário por batalha, causa **500 LP adicionais** ao oponente, com animação de **Ookazi** (#346, índice 3). Pode ocorrer uma vez por batalha concluída.
- Não aplica a ataques diretos, ataques cancelados nem monstros adversários que sobrevivam à batalha. É dano de efeito de monstro da categoria **Direct LP Damage**: cada Darkfire aliado face-up acrescenta mais um impacto de 500, com sua própria animação de Ookazi.

## Categoria interna Direct LP Damage (NOVO)

O MOD agora centraliza a elegibilidade de danos diretos aos LP em `aux_direct_lp_registered()` e `aux_direct_lp_extra_hits()` (`logic.h`), com a origem identificada separadamente. Isso permite adicionar novas habilidades futuramente sem misturar dano mágico, efeito de monstro e dano de batalha.

| Origem | Cartas / habilidade | Valor original | Com 2 Darkfires face-up |
|---|---|---:|---:|
| Magias de dano | Sparks, Hinotama, Final Flame, Ookazi, Tremendous Fire | 20/100/200/500/1000 | dano original + 2 impactos iguais |
| Efeito de monstro | Meteor B. Dragon (`<Fusion Effect>`) | 1000 | 3000 LP (3 animações Tremendous Fire) |
| Efeito de monstro | Gaia the Dragon Champion (`<Fusion Effect>`) | 500 | 1500 LP (3 animações Ookazi) |
| Dano de batalha | Rabid Horseman (piercing) | diferença ATK−DEF | SEM bônus de Darkfire |

**Regra aditiva, nunca exponencial:** `dano_base × (1 + quantidade de Darkfires)`. Cada impacto extra reutiliza a animação do golpe que o originou; o dano de Meteor/Gaia continua tipado como **efeito de monstro**, sem forjar eventos de Magic Card. O modo `ignore_effect_conditions` continua OFF por padrão e não altera esse cálculo.

## Reparo de apresentação após magias destrutivas (NOVO)

Seiyaryu continua evitando a destruição lógica dos seus outros Dragons de Nível ≤6. O jogo Vanilla pode, porém, iniciar uma animação de destruição que move a carta visual para o meio do tabuleiro, mesmo quando o hook de remoção impede a morte. A Beta 13:

1. Fotografa os **registros e identidades exatas** dos Dragons protegidos na entrada dos efeitos destrutivos.
2. Deixa a animação de Dragon Capture Jar / Dark Hole / Raigeki e efeitos nativos de remoção terminar; **não** interfere no cálculo de sobrevivência.
3. Só quando o mecanismo de requisições nativas fica ocioso, reconstrói o sprite da carta sobrevivente na zona correta, mantendo ATK/DEF, posição, flags, origem de fusão e estado de jogo.
4. Verifica `OCCUPIED`, ID e ponteiro de dados antes de restaurar, evitando ressuscitar monstros destruídos de verdade ou alterar uma nova carta na mesma zona.

É uma correção de **apresentação**, não uma nova regra de imunidade. Ainda requer teste no Windows com os efeitos visuais em 2D/3D.

## Transplante da proteção: Gaia → Seiyaryu #358

A habilidade antiga de Gaia foi **transferida integralmente para Seiyaryu**: enquanto Seiyaryu estiver face-up, protege outros Dragons aliados de nível 6 ou menor contra destruição por batalha, magia e efeito de monstro. Não protege a si próprio; a forma de obtenção de Seiyaryu não importa. Gaia agora possui exclusivamente o seu `<Fusion Effect>` condicionado.

A lógica de preservação do registro de batalha 2D e 3D, efeitos destrutivos e IA foi mantida e ajustada ao ID #358.

## Testes prioritários (Beta 13)

1. **Visual Seiyaryu:** coloque Seiyaryu + um Dragon de Nível ≤6 no campo; o adversário usa **Dragon Capture Jar**. O Dragon fraco deve sobreviver **e permanecer na sua zona**, sem carta presa no centro. Repetir com **Dark Hole** e **Raigeki**, se possível com 2 ou 3 Dragons. Seiyaryu pode ser destruído normalmente.
2. **Darkfire + Meteor:** 0/1/2 Darkfires e um Meteor B. Dragon válido. Esperado **1000/2000/3000 LP**, respectivamente, em **1/2/3 animações Tremendous Fire**.
3. **Darkfire + Gaia:** 0/1/2 Darkfires e Gaia válido destruindo um monstro em batalha. Esperado **500/1000/1500 LP adicionais**, em **1/2/3 animações Ookazi**. Não há bônus se o adversário sobreviver.
4. Verifique as descrições na tela em **Meteor, Gaia, Rabid e B. Skull**: nenhuma linha deve sair da caixa.
5. **Regressão:** Sparks/Hinotama com Darkfire continuam aditivos; Rabid não é amplificado; Seiyaryu não se protege; Settings continua OFF e a imunidade Metalmorph continua restrita às Magic Cards adversárias.

### Checklist legado



1. **Com opção OFF:** invocar Meteor, B. Skull, Rabid ou Gaia diretamente do deck: nenhum deles deve ativar `<Fusion Effect>`. Fusioná-los por seus materiais específicos: devem ganhar suas habilidades. Invocar metal diretamente também não fornece imunidade; usando Metalmorph, fornece.
2. **Com opção ON:** invocar esses mesmos monstros diretamente do deck: devem obter o efeito. Conferir dano de Meteor (1000) e de Gaia (500), estatísticas -500/-500 nos cinco alvos do B. Skull, dano perfurante do Rabid, imunidade de metal comprado do deck.
3. Desmarcar a opção e confirmar que a condição normal voltou. Faça o teste em duelo novo, para não misturar estados persistentes de efeitos anteriormente aplicados.
4. Testar invocação por fusão **sobre monstro já no campo** usando a escolha por Quadrado; o marcador de origem não pode sumir só porque o alvo estava no campo.
5. Testar B. Skull com 1 a 5 adversários, além de 2 B. Skulls seguidos; conferir animações uma de cada vez.
6. Testar Rabid/Gaia em combates **2D e 3D**, contra monstros em ataque e defesa, com e sem proteção contra destruição. Testar uma batalha cancelada.
7. Regressão: Darkfire com 2 cópias, Red Medicine e Sparks com X/Quadrado, Wall Shadow, Ancient Elf, Moths e Twin-Headed Thunder Dragon.

**Validação local:** testes de regras e do manifesto passaram; binários compilados com SDK v0.3.1-preview.1. As animações e a restauração de sprites em duelos reais **ainda não foram validadas aqui**; use os testes prioritários da Beta 13.

---

# Histórico — Beta 11 (estado anterior ao transplante para Seiyaryu)

Esta é uma atualização incremental da Beta 10, preservando os ajustes anteriores (Darkfire, Gaia, Wall Shadow, Moths, Ancient Elf, Twin-Headed Thunder Dragon e controle Quadrado nas três magias).

## Metalmorph Effect — descrição visual corrigida

O título aparece literalmente **`<Metalmorph Effect>`**, em cinza/prateado, seguindo a convenção dos cabeçalhos de efeitos. O texto de cada monstro metálico é:

    <Metalmorph Effect>
    This monster is unaffected by the opponent's Magic Cards.

A condição de invocação **não** aparece na descrição, mas continua obrigatória no código: a cópia precisa ter sido produzida **realmente através de uma fusão que consumiu Metalmorph**. Um monstro colocado no deck ou invocado por outro método não ganha imunidade. A proteção permanece limitada às mágicas adversárias, não a armadilhas, batalhas ou efeitos de monstro.

## Novas receitas (Metalmorph)

| Materiais (qualquer ordem) | Resultado | ID |
| --- | --- | --- |
| Wolf + Metalmorph | Giga-tech Wolf | #412 |
| Silver Fang + Metalmorph | Giga-tech Wolf | #412 |
| Misairuzame + Metalmorph | Mech Bass | #441 |
| White Dolphin + Metalmorph | Mech Bass | #441 |
| Anthrosaurus + Metalmorph | Cyber Saurus | #508 |
| Little D + Metalmorph | Cyber Saurus | #508 |

Os resultados de Metalmorph agora são **sete**: Metalzoa, Launcher Spider, Metal Dragon, Metal Fish, Giga-tech Wolf, Mech Bass e Cyber Saurus. Todos seguem o mesmo critério de imunidade condicionada à origem da fusão.

As seis novas receitas não aparecem no More Fusions v1.4 fornecido; testar a integração em runtime ainda é recomendado.

## Teste rápido da Beta 11

1. Confirmar o cabeçalho literal `<Metalmorph Effect>` e sua cor nos sete monstros.
2. Fundir Wolf e Silver Fang com Metalmorph separadamente: os dois devem produzir Giga-tech Wolf. Repetir com as alternativas para Mech Bass e Cyber Saurus.
3. Invocar cada um desses três monstros diretamente do deck em duelo separado: a imunidade não pode estar ativa. Em outro duelo, invocá-los por Metalmorph e verificar a imunidade às mágicas **do oponente** (Raigeki, Spellbinding Circle).
4. Confirmar que armadilhas, efeitos de monstro, batalha e equipamentos próprios continuam atuando como antes; verificar que ao remover o monstro do campo a condição é perdida.
5. Repetir os testes anteriores de Darkfire, Gaia, Ancient Elf e Wall Shadow apenas para regressão.

**Verificação local:** testes automatizados de receitas, descrições e condições executados. A execução visual e de combate no jogo Windows ainda depende de teste manual.

---

## Beta 10 — Metalmorph Effect + evolução dos Elves

Esta versão parte **integralmente da Beta 9** (incluindo Wall Shadow, rota de Great Moth, Darkfire com impactos animados e Quadrado para combinar Sparks/Hinotama/Red Medicine com monstros do campo).

## Metalmorph Effect

Quatro fusões possuem a habilidade **somente quando a invocação realmente utiliza Metalmorph como material**:

| Materiais (qualquer ordem) | Resultado | ID |
| --- | --- | --- |
| Zoa + Metalmorph | Metalzoa | 392 |
| Jirai Gumo + Metalmorph | Launcher Spider | 390 |
| Baby Dragon + Metalmorph | Metal Dragon | 409 |
| Tongyo + Metalmorph | Metal Fish | 438 |

Cópias obtidas diretamente do deck **não** recebem imunidade. O estado de origem está associado à instância que ficou no campo e é perdido quando ela sai do campo ou outra carta ocupa sua vaga. A proteção é contra **Magic/Equip do adversário**, não contra armadilhas, efeitos de monstro nem batalhas. Buffs próprios continuam aplicáveis.

O texto de cada uma das quatro cartas é **exatamente**:

    Metalmorph Effect
    This monster is unaffected by the opponent's Magic Cards.

Somente o título recebe coloração cinza/prateada; não se expõe a condição de invocação na descrição.

A lógica usa um hook na resolução da fusão somente durante a animação de combinação real, e outro no momento em que o monstro é colocado no campo. Não conta fusões simuladas pela CPU. A prevenção de mágicas usa o ciclo de efeito original: protege contra remoção e restaura modificadores/posição do alvo enquanto a magia inimiga é executada. O bypass de Swords of Revealing Light foi tratado também no comando de ataque do jogador.

## Ancient Elf — novas receitas

Removida a receita antiga **Magician of Faith + Metalmorph**. Ancient Elf #433 passa a resultar de **Time Wizard #016 + qualquer monstro do grupo Elf com ATK base estritamente menor que 1450**:

- Dancing Elf #395 (300)
- Wing Egg Elf #264 (500)
- Mystical Elf #002 (800)
- Celtic Guardian #041 (1400) — incluído no grupo Elf das fusões do Forbidden Memories

**Ancient Elf (1450), Gemini Elf (1900), Dark Elf (2000)** ficam fora. O ATK impresso/base é consultado no momento da avaliação, então alterações permanentes de outro mod respeitam o limite. Bonificações temporárias de Field/Equip não alteram o critério. Funciona com a ordem dos materiais invertida.

O manifesto desativa o resultado antigo #433 e o código só o permite nas combinações autorizadas acima. Nenhuma receita de More Fusions 1.4 envolve Time Wizard com um desses quatro Elves, então não há conflito conhecido nesses pares.

## Testes recomendados (jogo real ainda não executado neste ambiente)

1. Produzir cada um dos quatro metais com Metalmorph; testar com Raigeki, Dark Hole, Spellbinding Circle e Swords of Revealing Light pelo oponente, com o monstro face-up. Observar se a carta permanece, stats e posição não mudam, e o monstro protegido ainda consegue atacar sob Swords.
2. Colocar a mesma carta metálica diretamente do deck. As mesmas mágicas devem funcionar normalmente nela.
3. Verificar que armadilhas, combate e efeitos de monstros continuam funcionando nos quatro metais, mesmo quando fusionados com Metalmorph.
4. Criar Ancient Elf com cada um dos quatro Elves válidos + Time Wizard, em ambas as ordens; negar Magician of Faith + Metalmorph, Magician of Faith + Time Wizard e Elves de 1450 ATK ou mais.
5. Testar em campo (incluindo a seleção especial Quadrado) e também na mão; verificar que as outras receitas da Beta 9 continuam funcionando.
6. Testar ambos os duelistas / CPU e com More Fusions 1.4 instalado; observar conflitos com outros mods que substituam as mesmas rotinas de efeito.

**Validação técnica**: testes locais de regras/manifesto passam; objetos compilados em i386 (Windows 32/Linus 32) e x86_64-windows. Não há garantia de teste em runtime no PC aqui.

---

# Balanceamento de Fusões e Efeitos Auxiliares — v0.1.0 Beta.9

**Alvo:** Yu-Gi-Oh! Forbidden Memories Recompiled v0.3.0 (Windows, `slus_01411`).

**Instalação:** extrair a pasta `BalanceamentoFusoesEfeitosAuxiliares` para a pasta `mods/` do jogo, substituindo a versão anterior (não deixe duas cópias). O MOD deve estar habilitado; reinicie o jogo, pois as descrições das cartas são carregadas na inicialização.

## Beta.9 — Quadrado para fusao dirigida (2026-10-17)

**Mudanca pontual de controle, sem alteracoes de gameplay.**
Apos os testes positivos da Beta.8, a selecao especial de material foi transferida
do comando **L1 + confirmar** para **Quadrado (□)**, evitando conflito
com a camera 3D. A tecla **X** preserva a ativacao normal da carta.

- Somente **Sparks #343, Hinotama #344 e Red Medicine #339**, face-up
  na mao do jogador e sem outro material ja selecionado,
  usam **Quadrado** para escolher um monstro aliado no campo.
- **X** continua ativando o efeito Vanilla diretamente da mao: dano ou cura.
- A selecao de alvo, combinacoes de fusao, confirmacao/cancelamento no
  cursor e consumo dos materiais nao foram alterados.
- Os demais botoes e cartas seguem Vanilla; L1/R1 permanecem livres
  para a camera. A CPU continua com a mesma rotina da Beta.8.
- Os impactos Darkfire, Gaia, Wall Shadow, receitas e descricoes permanecem
  exatamente como na Beta.8.

**Teste rapido:** X em Hinotama deve causar 100 LP; Quadrado em Hinotama
seguido de Yamatano Dragon Scroll no campo deve gerar Darkfire sem causar
dano. X em Red Medicine deve curar; Quadrado + Great Moth no campo deve
gerar Perfectly Ultimate Great Moth sem curar. Conferir a camera com L1/R1
e a selecao normal das demais magias.

## Beta.8 — Dano animado por Darkfire e magias direcionaveis (2026-10-17; historico, atalho substituido na Beta.9)

**Esta atualizacao e incremental.** Mantem todas as receitas, protecao de
Gaia, IA, Wall Shadow e animacoes ja presentes na Beta.7. Os dois pontos
abaixo pertencem exclusivamente ao MOD de Fusoes/Efeitos Auxiliares.

### Darkfire Dragon — bonus aditivo com animacoes separadas

- Toda Sparks, Hinotama, Final Flame, Ookazi ou Tremendous Fire elegivel
  aplica primeiro **o dano Vanilla** e a sua animacao nativa.
- Para **cada Darkfire Dragon face-up no lado de quem ativou a magia**,
  a mesma animacao de **impacto/dano** (`DuelEffect_CreateRequest(6)`, com
  indice correspondente a carta) toca **mais uma vez**, e **depois de cada
  animacao** o adversario perde novamente o **dano BASE daquela magia**.
- Nao ha multiplicacao exponencial. Com Hinotama (100 de dano):
  0 Darkfires: 100; 1 Darkfire: 200; 2 Darkfires: 300; 3 Darkfires: 400.
  Com Sparks (50): 50 / 100 / 150 / 200, respectivamente.
- As repeticoes sao enfileiradas **sequencialmente** por um hook no sistema
  nativo de animacoes (`DuelEffect_UpdateRequests`), mantendo o duelo
  bloqueado durante o replay. Nao sao criadas cinco animacoes sobrepostas.
- Se o adversario chegar a 0 LP, os impactos restantes nao ocorrem.
  Magias refletidas, dano autoinfligido, cartas nao listadas e danos de
  batalha continuam sem o bonus.
- Magias emitidas por efeitos de monstros (ex.: Yamadron) continuam aptas
  desde que o jogo as encaminhe ao evento de dano direto de magia.
- A descricao do Darkfire foi atualizada no proprio `mod.json`.

### Sparks, Hinotama e Red Medicine — duas maneiras de usar da mao

**Modo Vanilla:** selecione uma dessas cartas virada para cima e confirme
normalmente. Ela ativa seu dano (Sparks/Hinotama) ou cura (Red Medicine)
diretamente da mao, com a animacao e efeito normais.

**Modo material de fusao (Beta.8, substituido na Beta.9):** com a carta face-up na mao, **segure L1 e confirme (X ou Quadrado)**. Em vez de ser ativada imediatamente, ela entra
no modo de selecionar um alvo no campo, como um equipamento. Selecione um
monstro aliado que ja esteja na Monster Zone e confirme para tentar fundi-lo
com a magia. A combinacao sera resolvida pelo verificador de fusoes nativo;
nao e necessario alterar a classificacao de Magic da carta e nao ha efeito
de dano/cura quando o material e consumido pela fusao.

O MOD utiliza a transicao nativa de selecao de campo (sem implementar outra
interface de cursor). So este trio aceita o atalho L1; as demais magias e as
jogadas normais de face-down permanecem Vanilla. A IA nao e forçada a
escolher o atalho; suas jogadas automaticas originais sao preservadas.

**Verificacao no Windows, necessaria antes de aprovar como estavel:**

1. Com 0/1/2/3 Darkfires, ative Hinotama e conte 0/1/2/3 impactos
   adicionais, conferindo 100/200/300/400 de dano total e a mudanca de LP
   entre os impactos (teste tambem Sparks, 50/100/150/200).
2. Confira cada uma das outras tres magias de dano, com e sem Darkfire.
   Faca um teste de Yamadron disparando o efeito de magia de dano.
3. Teste Goblin Fan e um Darkfire face-down: nao pode haver impactos bonus.
   Confira que 0 LP interrompe novos impactos.
4. Sem segurar L1, use Sparks, Hinotama e Red Medicine **diretamente da mao**.
   Confirme que a ativacao e as animacoes Vanilla continuam funcionando.
5. Coloque Great Moth no campo. Segure L1 ao confirmar Red Medicine face-up
   na mao, selecione Great Moth e confirme: deve resultar em Perfectly Ultimate
   Great Moth (#67), sem curar LP.
6. Coloque Yamatano Dragon Scroll no campo. Use L1 + confirmar com Sparks e
   com Hinotama, em duelos diferentes: deve resultar em Darkfire Dragon (#168)
   sem causar dano direto.
7. Teste cancelamento da selecao (Circulo), materia inexistente, outra
   magia nao incluida e jogadas da CPU. Repita com 3D Monsters e More Fusions.

**Limite de verificacao:** compilacoes i386 e x86_64 Windows, testes unitarios
sobre a formula e o filtro de magias, e testes existentes das receitas foram
executados; nao foi possivel executar o jogo Windows nem validar visualmente
as animacoes ou a interacao do cursor no runtime. O procedimento alternativo
L1 foi implementado em um hook especifico do controlador da mao, nao por
alteracao global das tabelas de cartas. Se outros MODs sobrescreverem esse
mesmo controlador, a ordem de carga precisa ser testada.

## Beta.7 — Evolucao das Moths, Wall Shadow e posicao defensiva (2026-10-17)

Esta versao preserva a protecao Gaia, o dano ampliado Darkfire, as receitas
restritas anteriores e a logica da CPU da Beta.6. Novas receitas exatas:

- Time Wizard #16 + Cocoon of Evolution #72 -> Perfectly Ultimate Great Moth #67.
- Cocoon of Evolution #72 + Larvae Moth #56 -> Great Moth #57.
- Great Moth #57 + Red Medicine #339 -> Perfectly Ultimate Great Moth #67.
- Shadow Ghoul #368 + Labyrinth Wall #366 -> Wall Shadow #369.

Todas funcionam nas duas ordens. Nao sao removidas receitas Vanilla ou de
More Fusions em massa. **Conflito especifico detectado:** More Fusions v1.4
faz Larvae Moth + Cocoon of Evolution -> Kuwagata alpha #480. Este MOD da
prioridade a Great Moth #57 nessa combinacao, por meio do hook da consulta
final de fusao, mesmo se More Fusions for carregado primeiro. Os outros pares
do More Fusions nao sao alterados por essa regra. O comportamento combinado
precisa ser testado no jogo (sem execucao Windows neste ambiente).

**Wall Shadow — efeito sempre ativo:** apos realizar um ataque e sobreviver,
passa automaticamente para Defense Position. Funciona para jogador e CPU,
independentemente de ter vindo de fusao. O MOD aguarda o jogo restaurar a
carta apos a batalha, incluindo o modo 3D, antes de trocar os flags da carta
e atualizar sua representacao. Um ataque negado por armadilha nao deve
ativar a mudanca. Se Wall Shadow for destruido, nao se recria o monstro.
Descricao visual em ingles usa o mesmo padrao das cartas Gaia e Darkfire.

**Testes recomendados no Windows:** experimentar as quatro receitas nos dois
sentidos; atacar diretamente e atacar um monstro com Wall Shadow sobrevivendo;
confirmar a mudanca visual e funcional para DEF, no modo 2D e com 3D Monsters;
repetir com a CPU, destruir Wall Shadow em batalha, e cancelar seu ataque
com armadilha; reiniciar o duelo para verificar ausencia de efeito residual.
Os testes automatizados inspecionam receitas, manifesto e precondicoes, mas
nao substituem os testes em tempo real.

## Beta.6 — CPU evita sacrificar Darkfire com sinergia (2026-10-17)

- Confirmação em jogo do usuário: a proteção do Gaia na Beta.5 funcionou. Nenhum hook de proteção do Gaia, receita exclusiva, descrição ou dano foi modificado.
- A CPU não deverá escolher uma fusão que consuma um **Darkfire face-up** quando há Yamadron ativo ou magias de dano na mão/zona M/T. Intercepta planejamento e consulta final de fusão; jogador permanece livre.
- O checker de fusão recebe apenas IDs, portanto, enquanto houver Darkfire face-up sob sinergia, cópias adicionais do mesmo ID também podem ser evitadas pela CPU. Se a sinergia desaparecer, a decisão volta ao mecanismo original.
- Compilação i386/x64 Windows; testar em campanha com Mountain Mage e Efeitos de Rituais, sem alterar os pesos/decks.

## Correções da Beta.5 — proteção do Gaia inspirada no Javelin Beetle

**Resultado dos testes do usuário:** a Beta.4 corrigiu a fusão inválida de Gaia the Dragon Champion + Thunder -> Twin-Headed Thunder Dragon, mas não protegeu Yamadron em combate. As descrições permanecem aprovadas. Não alteramos esses elementos.

**A causa remanescente:** as versões anteriores interceptavam a remoção lógica (`DuelCard_RemoveFromField`), o sinal de morte 3D e a lista de replay, mas não o caminho de apresentação 2D que ocorre antes: `DuelScene_UpdateBattle` estado 10 pode queimar a carta mesmo sem passar pelo hook de remoção. O MOD **Efeitos de Rituais** fornecido pelo usuário já resolveu essa situação para o **Javelin Beetle**, redirecionando o estado 10 para a trajetória sobrevivente (estado 6), mantendo intactos o dano e o cálculo de batalha.

A Beta.5 replica **somente a arquitetura de prevenção da destruição**, de maneira independente: registra cada participante protegido quando a batalha começa, preserva sua identidade e flags de campo; redireciona o estado 10 (sem criar fogo 2D); suprime `field_02` dos modelos após a preparação do estado 5 (sem morte 3D); verifica no estado 11 se faltou um resultado nativo e, se necessário, restaura a carta protegida no registro de campo com o mesmo ID, posição, flags e indicador de turno; continua bloqueando entradas de replay destruídas. Não copia recursos de Javelin, contador, consumo de proteção, som ou overlays.

**Dano aos LP permanece Vanilla.** Nenhuma intervenção altera o comparador, ATK, DEF ou cálculo de dano: a prevenção opera sobre visualização e persistência do registro depois que o confronto foi calculado. O Gaia deve continuar face-up; ele não se protege sozinho e sua presença é revalidada durante a batalha.

**Diagnóstico:** o código gera logs breves sob `[Fusion Auxiliary Balance] Gaia battle`, `Gaia protected 2D card`, `Gaia suppressed 3D death` e `Gaia restored battle survivor`, apenas para duelos em que as condições ocorrerem. Se uma carta voltar a morrer, esses eventos ajudam a localizar o problema sem instrumentação global.

**Validação:** compilações Windows 32 e 64 bits, testes das regras do manifesto, limites de fusão e teste isolado do desvio de 2D — não equivale a executar um duelo no jogo Windows. A interação real com o MOD 3D Monsters e Efeitos de Rituais requer o próximo teste do usuário.

## Histórico da Beta.4 (após teste em jogo)

**Fusão inválida do Mountain Mage:** a Beta.3 tentava editar `event->result` em `MEMORIES_AFTER`. No Recompiled v0.3.0, o despachante (`src/pc/mods/events.c`) RESTAURA o resultado em `AFTER`: o evento é apenas para observar. Portanto, a restrição nunca bloqueava fusões Vanilla. O FM Editor ensina o mecanismo nativo `fusions`: `{"remove": C}` cancela todos os resultados **retail** C, mantendo todas as outras fusões, e `{"with": [A,B], "result": C}` cria as receitas aprovadas para ambas as ordens e para a CPU.

Agora `mod.json` remove os resultados retail **168** e **613** e registra explicitamente as três receitas com pares fixos. A condição *Two-mouth Darkruler + Dragon/Thunder (ATK base 1600–2799)* permanece calculada em `MEMORIES_BEFORE` para respeitar mudanças de tipo e ATK por outros MODs. Um hook de resultado sobre a função original/guest `Duel_CheckFusion` faz verificação final contra efeitos personalizados de outros MODs que tentem recolocar receitas proibidas. A Beta.4 **não altera** todas as outras fusões Vanilla.

**Gaia não protegeu Yamadron:** em `DuelScene_UpdateFieldActions`, o jogo chama `DuelCard_DeactivateRecord` nos dois monstros envolvidos antes de iniciar `DuelScene_UpdateBattle`. Isso limpa `DUEL_CARD_FLAG_OCCUPIED`. Na Beta.3, o filtro de proteção sempre exigia `OCCUPIED`, então a carta atacada não era reconhecida no momento de sua destruição. A Beta.4 identifica ambos os participantes pelos registros e `card_id` preservados, mesmo enquanto estão temporariamente fora do campo; impede a sinalização de morte no combate 3D e cancela as entradas do replay de destruição ao entrar e ao continuar em `DuelScene_UpdateResume`. A proteção se desliga se Gaia sair de campo. O cálculo normal de LP continua intocado.

A correção da Beta.3 para a inicialização (`MonsterEffects_Update` não deve ser alvo de hook) permanece, assim como `min_api:11`, frames laranja e as descrições já aprovadas. **Não foi executado o jogo Windows neste ambiente**; testes em runtime continuam necessários para confirmar a interação com o MOD 3D Monsters.

## Regras aprovadas

### Gaia the Dragon Champion #37

Enquanto estiver face-up na Monster Zone, Gaia protege **os outros Dragons aliados** de nível de 1 a 6 contra **destruição por batalha, efeitos de Magic Cards e efeitos de monstros**. O Gaia não protege a si mesmo. A proteção não anula dano aos LP, alterações de ATK/DEF, fusões, rituais ou tributos voluntários.

Nesta beta, os participantes de combate são capturados mesmo quando o registro está temporariamente desativado. O replay 2D e os indicadores de derrota do modelo 3D também são filtrados. A proteção é verificada dinamicamente enquanto Gaia está face-up. Não foi confirmado em runtime Windows; deve ser validado no jogo com 3D Monsters ativo.

### Darkfire Dragon #168 (regra antiga — substituida na Beta.8)

Enquanto estiver face-up, dobra uma única vez o dano direto **ao adversário de seu controlador** causado pelas Magic Cards Sparks #343, Hinotama #344, Final Flame #345, Ookazi #346 e Tremendous Fire #347. Dano de batalha, dano autoinfligido e outros tipos de dano não são dobrados. Magias resolvidas por efeitos como Yamadron continuam elegíveis quando o motor as apresenta como magia de dano direto.

**Suas únicas receitas agora são:**

- Yamatano Dragon Scroll #122 + Sparks #343
- Yamatano Dragon Scroll #122 + Hinotama #344

Essas receitas funcionam em ambos os sentidos. **Todas as outras receitas Vanilla que resultem em Darkfire Dragon deixam de funcionar**; demais fusões não mudam por conta desta restrição.

### Twin-Headed Thunder Dragon #613

**Suas únicas receitas agora são:**

- Thunder Dragon #425 + Thunder Dragon #425.
- Two-mouth Darkruler #218 + um monstro do tipo **Thunder** ou **Dragon**, com **ATK base >= 1600 e < 2800** (de 1600 até 2799, inclusive).

A segunda receita aceita os materiais em ambos os sentidos e consulta o tipo e o ATK base atuais da carta, já com alterações permanentes de outros MODs, mas sem bônus de Field/Equip. O ATK de planejamento virtual da CPU não interfere nesse limite.

**Todas as receitas Vanilla de Twin-Headed Thunder Dragon que não se enquadrem nessas duas regras são removidas.** Outras fusões Vanilla continuam como estavam. A restrição da tabela Vanilla é feita por `fusions: [{"remove":613},...]` antes da resolução, do modo que o FM Editor define; o wrapper final também bloqueia resultados de hooks não autorizados.

## Descrições das cartas (registro das descricoes da Beta.7)

Os cards de Gaia #37 e Darkfire #168 agora mostram textos curtos em inglês, no padrão de exemplo: título `<Effect>` em laranja, texto principal branco, destaque de quantidade em vermelho, moldura laranja para indicar monstro com efeito. São alterações visuais por `cards[].replace` no `mod.json`; elas não alteram o funcionamento da carta, seus atributos nem seus stats.

- **Gaia:** `While face-up, protects your other Dragons of Level 6 or lower from destruction by battle, Magic or monster effects.`
- **Darkfire:** `While face-up, your Sparks, Hinotama, Final Flame, Ookazi and Tremendous Fire deal 2x LP damage to your opponent.`

O Twin-Headed Thunder Dragon teve sua receita alterada, **não ganhou efeito próprio** nem recebeu um texto de efeito fictício. Outras cartas, incluindo Feral Imp (usada somente como referência estética), não foram alteradas.

## CPU e compatibilidade

O evento de fusão do jogo informa a IA e o jogador das novas regras. Durante o planejamento, continua a prioridade adicional para Gaia se protege Dragons e Darkfire se há mágicas de dano, e a preservação de Gaia e de mágicas de dano quando estrategicamente aplicável. Essas prioridades **não** aumentam o ATK/DEF real.

Nenhum arquivo ou regra do Ritual Duelists, Melhoria de Rituais, Efeitos de Rituais, drops ou decks foi modificado. O MOD é autocontido. Outro MOD que substitua o resultado inteiro de um evento de fusão ou as mesmas descrições pode criar conflito de ordem de carregamento; isso precisa de validação quando integrado.

## Verificações internas

- Corrigida a tentativa de hook em função PC não suportada; `nm -u` confirma que o MOD não referencia mais `MonsterEffects_Update`.

- Teste C de lógica de fusões, limites de ATK, identificação do dragon **mesmo quando a carta está temporariamente levantada**, Gaia, Darkfire e avaliação da IA: **PASS**.
- Teste do manifesto conforme regras do FM Editor: remove 168/613, três receitas específicas, mantém as demais fusões e preserva descrições: **PASS**.
- Hook de retorno de `Duel_CheckFusion`: impede resultados 168/613 se não vieram das duas receitas autorizadas, inclusive em outros MODs: **compilado**.
- Build SDK oficial v0.3.0 para `i386`: **compilada**; referências externas verificadas pelo `build_mod.py` com os exports do SDK.
- Build SDK oficial v0.3.0 para `x86_64-windows`: **compilada**, **sem exports x64 disponibilizados** no SDK para validação equivalente.
- Manifesto e presença dos arquivos: checados; teste de lógica não substitui o teste real de batalhas e leitura das descrições no Windows.
- Teste real em runtime Windows: **NÃO executado neste ambiente**.

## Checklist de teste no jogo

0. **Inicialização:** substitua integralmente a pasta da versão anterior em `mods/`, feche e abra o jogo; confira se o MOD aparece como **Active** (sem `refused to start`).

1. **Descrições aprovadas:** somente confira se permanecem iguais; não foram modificadas na Beta.4.
2. **Darkfire:** Yamatano + Sparks e Yamatano + Hinotama devem produzir Darkfire, ambos nos dois sentidos. Uma receita Vanilla antiga do Darkfire não deve produzir Darkfire.
3. **Twin-Headed:** refazer primeiro a fusão observada no Mountain Mage, **Gaia the Dragon Champion + Thunder**, e conferir que NÃO produz Twin-Headed. Depois testar Thunder Dragon + Thunder Dragon e Two-mouth Darkruler + parceiros Dragon/Thunder com ATK 1600, 2700, 2800 e outro tipo.
4. **Gaia:** repetir o caso reportado (**Gaia face-up e Yamadron aliado atacado**). Yamadron deve sobreviver, mas se estava em modo ataque os LP do controlador ainda perdem a diferença dos ATKs. Repetir sem Gaia e testar Raigeki, Dark Hole e efeitos destrutivos; observar 2D e 3D.
5. **Regressão:** retirar um Dragon protegido voluntariamente para fusão/ritual e confirmar que o custo funciona normalmente; testar mágicas de dano direto com/sem Darkfire e uma batalha comum; observar as decisões da CPU.


## v0.1.0-beta.14 — Encore, Battle Warrior, Mystical Sheep #1

- Musician King #495: permanent `Encore!!!`, a single real second attempt
  for failed coin/roulette skills. Multiple Kings do not stack. This optional
  API is shared as `fusion-auxiliary-balance:encore-v1` and read by the
  compatible Ritual and Time Wizard modules. On fusion only: cannot be
  destroyed by battle and cannot be switched to defense, but LP damage
  applies normally. The 3D/2D battle restore reuses Seiyaryu's battle guard.
- Battle Warrior #100: real recovered LP/2 is sent to opponent as Direct LP
  Damage after native healing; Darkfire contributes additional base-sized
  impacts under the central registry. On fusion only: one normal coin (and
  a single Encore reroll on failure) can exchange current ATK values with a
  higher-Level battle opponent until battle return. Cards tagged as immune
  and Blue-Eyes Ultimate Dragon resist the exchange. Coin UI comes from the
  optional ritual-card-effects-test:coin-flip-v1 provider.
- Mystical Sheep #1 #642: while face-up the opponent's new Equip cards are
  rejected by the game's original incompatible/discard pathway. Old Equip
  gains stay. AI makes one failed real Equip attempt, then suppresses future
  attempts while Sheep is visible, keeping knowledge during the duel.
  Fusion-only support doubles positive Equip ATK and DEF gains for allies,
  including old Equip gains. More fused Sheep copies never stack and the
  boost is reversed when the last disappears. Literal effect description:
  `Its mystical aura rejects enemy equipment.`
- Both desktop targets compile with SDK 0.3.1-preview.1; static/regression
  checks pass. User runtime testing is still required for controller timing,
  animation positions and interaction with other installed mods.

**Installation:** replace only the existing Balanceamento package with this
folder; replace Ritual Effects/Time Wizard with their own compatibility
packages if Encore is desired. Never install a duplicate ID alongside the
old version. Dual Play itself is not changed.

### Beta 18 (Mystical Sheep #1: self-Equip)
- Ao equipar o proprio Mystical Sheep #1 durante a animacao nativa, o jogo
  pode levantar o registro e desmarcar `OCCUPIED`. A elegibilidade do Fusion
  Effect de Equip agora reconhece essa copia especifica pela identidade de
  fusao e ponteiro `data` preservados, usando `aux_fusion_special(target,1)`.
- Equips em outros monstros e acumulacao maxima x2 continuam iguais.
- Validado por build x86 e x64 e testes estaticos; runtime ainda deve ser
  confirmado em Windows, especialmente Megamorph na propria Sheep.


### Beta.28 — títulos condicionados em campo

Corrige seleção de canal nativo do renderizador por `obj->field_67` (em vez
de procurar `channel->field_28==obj`) e reconhece todas as variantes Shift-JIS
dos sinais `<`/`>`. Em duelo, título de efeito indisponível fica branco;
fora do duelo permanece sempre na cor original. Sem alteração mecânica.

### Beta.30 — títulos por instância (2026-10-29)
Os estados coloridos de `<Fusion Effect>`/`<Metalmorph Effect>` agora
consultam exclusivamente o registro de carta identificado pelo cursor de
campo; a mão não herda a origem de outra cópia. O fallback de títulos é
independente por categoria em cartas como Fusionist, que têm dois títulos.
Build Deck/Library recebem sempre as cores originais. Nenhuma regra de
fusão, equipagem, IA ou efeitos foi alterada nesta beta.
