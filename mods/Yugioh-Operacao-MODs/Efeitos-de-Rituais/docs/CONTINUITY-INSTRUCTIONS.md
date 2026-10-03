# CONTINUAÇÃO DO PROJETO — MOD EFEITOS DE RITUAIS

Você está assumindo a execução de um projeto que **já está em desenvolvimento** em outra conversa do ChatGPT.

## REGRA PRINCIPAL

**NÃO REINICIE O PROJETO. NÃO REFAÇA O MOD DO ZERO.**

Tudo que já foi desenvolvido, decidido, implementado e testado anteriormente continua válido.

Seu trabalho agora é **continuar o desenvolvimento existente**, usando o GitHub como local principal para código, versões e documentação técnica.

Estou anexando a este Work o ZIP da versão mais recente que estávamos testando:

`MOD-Efeitos-de-Cartas-Rituais-v0.74-test-Otimizacao(1).zip`

Esse ZIP deve ser tratado como a **fonte autoritativa da versão atual v0.74** para a migração.

Também fornecerei:

- link do meu fork pessoal;
- link do repositório/release oficial v0.2.0;
- acesso ao meu GitHub conectado.

---

# 1. REPOSITÓRIO EM QUE VOCÊ DEVE TRABALHAR

O MOD deve existir SOMENTE no meu fork pessoal:

`otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled`

O repositório oficial de Unchiga pode ser consultado como referência, mas **NUNCA deve ser alterado**.

---

# 2. BRANCH EXCLUSIVA

Já foi criada uma branch para este projeto:

`mod-ritual-effects`

Use exclusivamente essa branch para o MOD Efeitos de Rituais.

### É PROIBIDO alterar:

- `master`;
- `mod-field-effects`;
- qualquer outra branch de outro MOD;
- o repositório oficial de Unchiga.

A branch `mod-field-effects` pertence a outro projeto, relacionado a **Field Effects e Tipos Secundários**.

Não altere, mova, apague, reorganize ou reutilize arquivos pertencentes a esse projeto.

---

# 3. DIRETÓRIO EXCLUSIVO DO MOD

O MOD Efeitos de Rituais deve permanecer isolado em:

`mods/Yugioh-Operacao-MODs/Efeitos-de-Rituais/`

A organização pretendida é:

`dev/`

Versão atualmente em desenvolvimento.

`versions/`

Snapshots das versões de teste importantes.

`docs/`

Documentação técnica, arquitetura, regras das mecânicas, decisões tomadas, bugs conhecidos e resultados de testes.

Uma versão funcional anterior **não deve desaparecer simplesmente porque uma versão nova começou a ser desenvolvida**.

Conforme avançarmos, preservar versões relevantes como:

`v0.74`
`v0.75`
`v0.76`
etc.

Não renumere artificialmente versões antigas apenas para começar em v0.1.

---

# 4. ESTADO ATUAL DA MIGRAÇÃO

Uma primeira tentativa de migração já foi realizada pelo ChatGPT anterior.

A branch:

`mod-ritual-effects`

já foi criada.

Também foi criada a estrutura inicial dentro de:

`mods/Yugioh-Operacao-MODs/Efeitos-de-Rituais/`

Foi criado um commit inicial de migração:

`4e933b5ab59752135f205681d896b4dd30f39b2b`

Porém, houve uma limitação na ferramenta utilizada naquela conversa e **nem todos os arquivos do ZIP puderam ser enviados de forma confiável ao GitHub**, principalmente determinados arquivos binários/compilados.

Portanto:

**NÃO presuma que a migração atual está completa.**

Antes de modificar o código, faça uma auditoria.

Compare:

1. conteúdo real do ZIP v0.74 anexado;
2. conteúdo atualmente existente na branch `mod-ritual-effects`;
3. estrutura `dev/`;
4. estrutura `versions/`;
5. documentação já criada.

Identifique exatamente o que está presente e o que está faltando.

Depois complete a migração.

Não substitua arquivos reais por arquivos inventados.

Não reconstrua código por memória se o arquivo correspondente existir no ZIP.

---

# 5. VERSÃO ATUAL

A versão atualmente em desenvolvimento/teste é:

**v0.74 — versão de teste de otimização**

Ela deriva do estado funcional anterior do MOD e foi preparada após a v0.73.

Não trate versões antigas do histórico como se fossem a versão atual.

O ZIP v0.74 anexado é a referência primária para determinar o conteúdo exato da versão.

Antes de fazer qualquer correção nova, preserve no GitHub uma cópia fiel dessa versão.

---

# 6. OBJETIVO DO MOD

Este projeto modifica e expande o funcionamento dos **Rituais de Yu-Gi-Oh! Forbidden Memories** na versão recompilada/decompilada do jogo.

O projeto já passou muito além de uma simples prova de conceito.

Não reduza o MOD novamente ao estágio inicial de "fazer Ritual funcionar".

Existem mecânicas, efeitos específicos, regras, proteções e decisões técnicas que já foram implementadas e testadas.

Preserve o comportamento existente sempre que possível.

---

# 7. REGRA DE MATERIAIS DE RITUAL

Uma decisão fundamental já estabelecida:

O Ritual continua exigindo **exatamente 3 monstros/materiais no total**.

A expansão permite combinar monstros do campo e da mão.

Combinações válidas incluem, por exemplo:

- 1 monstro no campo + 2 monstros na mão;
- 2 monstros no campo + 1 monstro na mão.

O objetivo não é transformar o Ritual em uma invocação de apenas um material.

Essa regra já foi decidida e não deve ser reinterpretada.

---

# 8. EFEITOS ESPECIAIS DE RITUAIS

O MOD já possui trabalho relacionado a efeitos próprios dos monstros Ritual.

Um dos principais casos já desenvolvido e testado é:

**Magician of Black Chaos**

Existe um efeito relacionado a **Black Hole**.

Esse sistema já estava funcional o suficiente para testes avançados e não deve ser removido/reimplementado do zero sem uma razão técnica comprovada.

A v0.73 foi considerada uma base funcional importante para esse comportamento, e a v0.74 é uma versão posterior focada em otimização/testes.

Use o código real do ZIP para entender a implementação exata.

---

# 9. BUGS ATUALMENTE EM INVESTIGAÇÃO

Existem pelo menos dois bugs importantes que estavam sendo investigados antes da migração.

## BUG 1 — Millennium Shield no campo adversário

Em uma situação específica, ao realizar o Ritual para invocar **Millennium Shield**, o monstro apareceu no campo do adversário em vez do campo do jogador.

O monstro funcionava normalmente no campo adversário.

Foi um comportamento circunstancial/intermitente: em outros duelos o mesmo Ritual colocou Millennium Shield corretamente no campo do jogador.

Ainda não temos uma reprodução 100% determinística.

Portanto, não assuma imediatamente a causa.

Investigue especialmente propriedade do card, side/player ownership, slots, referências ao grid e estado residual utilizado durante a resolução do Ritual.

## BUG 2 — Soft lock após Black Hole do Magician of Black Chaos

Foi realizado um teste usando o efeito Black Hole do **Magician of Black Chaos** enquanto Millennium Shield estava presente.

Millennium Shield sobreviveu, o que estava **de acordo com a programação esperada**.

Porém, depois disso ocorreu um soft lock.

Sintomas:

- música continuava;
- animações 3D continuavam;
- o jogo não crashava;
- porém o duelo não prosseguia normalmente;
- interação/estado do duelo ficava travado.

Esse bug é uma prioridade técnica.

Investigue a máquina de estados/resolução após o efeito, principalmente quando existem monstros que sobrevivem ao processo de destruição.

Não "conserte" simplesmente removendo a sobrevivência/proteção do Millennium Shield se ela estiver funcionando conforme projetado.

Precisamos corrigir a transição/estado que causa o soft lock preservando a mecânica.

---

# 10. PRIORIDADE DE ENGENHARIA

Daqui para frente, existe uma diretriz importante:

**manter o código o mais compacto e eficiente possível, MAS nunca sacrificar o funcionamento correto das mecânicas e efeitos apenas para reduzir código.**

Ordem de prioridade:

1. funcionamento correto;
2. estabilidade;
3. preservação das mecânicas já aprovadas;
4. compatibilidade;
5. código compacto/eficiente;
6. facilidade de manutenção.

Não faça grandes refactors apenas por estética enquanto bugs funcionais estiverem sendo investigados.

---

# 11. DECOMP / SDK v0.2.0

A versão/release v0.2.0 do projeto oficial pode e deve ser pesquisada como referência técnica.

Ela pode ajudar a entender:

- estruturas;
- lifecycle das cartas;
- grid;
- ownership;
- resolução dos duelos;
- controllers;
- funções internas;
- mudanças da recompilação.

Mas existe uma distinção importante:

**nosso MOD continua sendo um MOD separado.**

Não transforme nosso projeto em um fork modificado indiscriminadamente do código principal.

Preserve a separação em:

`mods/Yugioh-Operacao-MODs/Efeitos-de-Rituais/`

---

# 12. ARQUIVOS E CÓDIGO

Durante a auditoria do ZIP:

- identifique o código-fonte real;
- identifique `mod.json`;
- identifique assets;
- identifique arquivos auxiliares;
- identifique objetos/binários compilados;
- diferencie arquivos fonte de artefatos de build;
- preserve somente aquilo que faça sentido preservar no histórico do MOD.

Não invente arquivos que não existiam.

Se considerar que algum artefato compilado não deveria ser versionado, **não o apague silenciosamente**.

Primeiro registre o que encontrou e justifique tecnicamente a decisão.

---

# 13. DOCUMENTAÇÃO

O GitHub deve permitir que outro desenvolvedor continue o projeto sem depender exclusivamente desta conversa.

Mantenha documentação suficiente para registrar:

- versão atual;
- arquitetura do MOD;
- arquivos importantes;
- mecânicas implementadas;
- regras dos Rituais;
- efeitos especiais;
- decisões técnicas;
- bugs conhecidos;
- testes realizados;
- resultados dos testes;
- alterações de cada versão;
- próximos passos.

Não substitua documentação técnica objetiva por textos genéricos.

---

# 14. HISTÓRICO DE VERSÕES

Queremos acompanhar quantas versões forem necessárias durante o desenvolvimento.

Quando começarmos uma nova versão:

- preserve a versão anterior relevante em `versions/`;
- trabalhe na nova em `dev/`;
- documente o motivo da mudança;
- registre o resultado dos testes;
- faça commits claros.

Se uma versão apresentar regressão, devemos conseguir voltar rapidamente à última versão funcional.

---

# 15. PROCEDIMENTO IMEDIATO

Antes de começar uma nova correção de código, execute nesta ordem:

### Etapa A — Segurança

Verifique os commits/SHAs atuais de:

- `master`;
- `mod-field-effects`;
- `mod-ritual-effects`.

Registre os valores.

### Etapa B — Auditoria

Inspecione completamente o ZIP v0.74 anexado.

Compare-o com o conteúdo da branch `mod-ritual-effects`.

### Etapa C — Completar migração

Envie para a pasta exclusiva todos os arquivos necessários que estejam faltando.

Garanta que `dev/` represente fielmente a versão atualmente em desenvolvimento.

Preserve uma cópia apropriada da v0.74 em `versions/`.

### Etapa D — Documentação

Atualize os documentos com o estado real encontrado.

Não mantenha afirmações antigas se a auditoria do código mostrar algo diferente.

### Etapa E — Commit

Crie um commit exclusivamente na:

`mod-ritual-effects`

com mensagem clara indicando a conclusão da migração/auditoria da v0.74.

### Etapa F — Verificação de isolamento

Depois do commit, confira novamente os SHAs de:

- `master`;
- `mod-field-effects`.

Eles devem continuar exatamente iguais aos valores registrados na Etapa A.

Não faça merge para `master`.

Não abra PR para o repositório oficial.

Não faça push em branches de outros projetos.

---

# 16. DEPOIS DA MIGRAÇÃO

Somente depois de confirmar que a v0.74 está preservada corretamente no GitHub, continue o desenvolvimento.

Não reinicie a investigação.

O próximo trabalho deve partir do estado atual do código e dos bugs conhecidos.

A prioridade inicial é compreender e corrigir os bugs existentes, especialmente o soft lock relacionado ao Black Hole, sem quebrar as mecânicas já funcionais.

Quando uma correção exigir uma nova versão de teste, crie a próxima versão lógica do nosso histórico, em vez de sobrescrever silenciosamente a v0.74 preservada.

---

# 17. MEU PAPEL COMO TESTER

Eu não tenho experiência avançada em programação.

Você deve assumir a maior parte possível do trabalho técnico.

Quando precisar que eu faça um teste local:

- explique exatamente o que devo baixar;
- onde colocar;
- como compilar/executar, se necessário;
- quais cartas usar;
- qual situação reproduzir;
- o que observar;
- quais prints/logs preciso devolver.

Não peça que eu diagnostique código C por conta própria se você puder realizar essa análise.

Posso atuar como tester dentro do jogo e fornecer screenshots, vídeos, resultados e diagnósticos solicitados.

---

# 18. REGRA CONTRA PERDA DE PROGRESSO

Antes de qualquer alteração potencialmente arriscada:

- preserve o estado funcional anterior;
- faça commits pequenos e identificáveis;
- evite misturar correção de bugs com refactors grandes;
- mantenha rollback fácil.

Se uma hipótese falhar, não destrua a versão anterior para testar outra.

---

# 19. RELATÓRIO QUE QUERO APÓS A MIGRAÇÃO

Quando terminar a auditoria e migração, informe claramente:

1. branch utilizada;
2. SHA inicial da `master`;
3. SHA inicial da `mod-field-effects`;
4. SHA inicial da `mod-ritual-effects`;
5. conteúdo encontrado no ZIP v0.74;
6. quais arquivos já estavam no GitHub;
7. quais arquivos estavam faltando;
8. quais arquivos foram adicionados;
9. estrutura final de `dev/`;
10. estrutura final de `versions/`;
11. documentação criada/atualizada;
12. novo commit da migração;
13. SHA final da `master`;
14. SHA final da `mod-field-effects`;
15. confirmação de que ambas permaneceram intocadas;
16. confirmação de que o repositório oficial de Unchiga não foi alterado;
17. bugs que continuam abertos;
18. próximo passo técnico recomendado para continuar exatamente do ponto em que o desenvolvimento parou.

**Não considere a tarefa concluída apenas porque a estrutura de pastas existe. Confirme que o conteúdo real da v0.74 foi preservado.**

Depois disso, continue normalmente como responsável técnico pelo desenvolvimento do MOD Efeitos de Rituais.