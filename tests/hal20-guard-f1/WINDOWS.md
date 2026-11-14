# HAL-20 — roteiro Windows i386/API24, experimental

**A composição completa está RECUSADA pelo guard #20. Não desligue o guard para tratar a coleção como aprovada.** O pacote local de teste não é release; não inclui ROM, imagem do jogo, saves pessoais, Dual Play ou Time Wizard. O core compila para Windows; gameplay real ainda não foi executado.

## Preparação

1. Obter esta branch e conferir o relatório `notes/HAL20_INTEGRACAO_GUARD_F1.md`. Usar instalação/pasta de dados exclusiva para testes, sem substituir a instalação habitual. Começar duelo novo; não transportar savestates antigos.
2. Construir o core experimental desta branch, com as dependências oficiais descritas em `notes/pc-build.md`:

   ```text
   python tools/pc/build.py --target windows --release --build tmp/pc/win32
   ```

   O executável antigo da integração não contém o guard. Compilação não comprova runtime. O builder escreve cópias recompiladas em seu diretório de saída; o empacotador desta tarefa recoloca os objetos qualificados e preservados dos PRs.
3. Gerar o pacote LOCAL, apontando `MEMORIES_AI_DIR` para o AI oficial qualificado do PR8 (objeto SHA9171b85b...; hash completo em INPUTS.json):

   ```text
   python tests/hal20-guard-f1/package_windows.py --build tmp/pc/win32 --output tmp/hal20-package --report tmp/hal20-package-report.json
   ```

   Não publicar o ZIP como versão final. O script não copia Dual nem arquivos de jogo. Extrair o ZIP local numa pasta nova. Usar sua imagem legítima apenas localmente, selecionando-a no jogo; não colocá-la no Git ou em arquivos de diagnóstico destinados a compartilhamento.
4. Perfis que exigem Dual precisam da cópia privada API24 do PR10, obtida legalmente. Colocar seu objeto e manifest em `mods/dual-play/` apenas na instalação LOCAL. SHA objeto **77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378**, manifest **c3244ab680764a0d930c12929c5adf8ae3128e4d6b981f5e06ff3eb624839563**. O ZIP original API5 não é substituto. Uma recompilação local com SHA diferente exige conferir proveniência/código carregado e qualificação; não editar o preflight cegamente. Sem a dependência qualificada, esses perfis ficam bloqueados.
5. Abrir PowerShell na pasta extraída e executar, por exemplo:

   ```powershell
   .\tools\Run-Profile.ps1 -Profile fusion
   .\tools\Run-Profile.ps1 -Profile own-mrf
   .\tools\Run-Profile.ps1 -Profile minimal-h-rd-dual
   ```

   O launcher confere os hashes do pacote e, quando necessário, os do Dual privado. Cria `test-data/<perfil>/` e logs isolados; não sobrescreve dados pessoais. A primeira execução copia o perfil; posteriores preservam suas escolhas salvas. Para repetir um perfil limpo, conservar/renomear a pasta de testes antiga antes de criar outra instalação. Nunca instalar duas versões de `fusion-auxiliary-balance`.

## Perfis e ordem de execução

| Perfil | Componentes ativos, além de Drop Missing Cards1.0 | Resultado esperado antes de gameplay |
|---|---|---|
| `melhoria` | Melhoria/Surrender | Carrega isolado; verificar rituais e Select. |
| `ritual` | Ritual Effects | Carrega isolado; verificar seus efeitos. |
| `fusion` | Fusion F1 PR22 | Carrega isolado; preservar mecânicas ON e restart. |
| `ritual-duelists` | Ritual Duelists | Carrega isolado; manter Droplist externo separado. |
| `own-mrf` | Melhoria + Ritual + Fusion | Sem par Dual/materializador protegido; investigar tributos/providers. Não é certificação do ecossistema. |
| `h-dual` | AI + Dual privado | Ordem AI→Dual; precisa de Dual qualificado. |
| `rd-dual` | RD + Dual privado | Ordem RD→Dual; precisa de Dual qualificado. |
| `minimal-h-rd-dual` | AI + RD + Dual privado | Ordem inicial AI→RD→Dual, cadeia swap Dual→RD→AI→motor. |
| `full-refused` | Os seis MODs | **Recusa esperada**: wrappers M/R/F não qualificados; nenhum duelo autorizado. Testar somente a mensagem/UX. |

Todos os perfis têm o opt-in `experimental.hal2224_order_guard=1`. Não alteram manifests/settings estratégicos. Drop Missing Cards fica ON; equilíbrio final dos drops só pode ser medido com tabelas reais. A chave do guard não cria checkbox. AI preserva seu manifest original, sem tornar restart:true permanente. Alterações recusadas/adiadas e o estado efetivo devem ser anotados separadamente.

## Bateria manual, com evidência por caso

1. **Inicialização/refusas:** registrar versão/API, buildid, perfil, ordem mostrada e mensagens. Em `full-refused`, confirmar erro antes do controller; corrigir/desativar perfil somente para diagnóstico e relançar quando solicitado. Não forçar entrada removendo wrappers/guard.
2. **Rituais com Melhoria:** usar a receita aprovada, três instâncias físicas, três no campo e um campo+dois na mão, com cópia idêntica não escolhida. Conferir consumo e preservação dos demais. Com default min_field=1, só mão deve ser recusado; não mudar o default para obter PASS. Repetir com Ritual Effects presente/ausente e recipes Vanilla/modificadas. Registrar invocação, animação e resultado reais.
3. **Fusion/efeitos ON:** Fusionist, Egotist, Metalmorph, resultado/origem por instância, mesma ID em cópias diferentes, movimento de slot/controlador, remoção/transformação, títulos e valores. Conferir efeitos já resolvidos e nenhuma reaplicação indevida.
4. **F1/lifecycle:** criar uma grant legítima, pedir OFF e confirmar que `restart:true` mantém a aplicação atual até relançar; cancelar/confirmar reinício pela UI; começar novo duelo e conceder nova grant após ON. Não se presume que a UI entregue OFF efetivo no meio da ação. Os contratos de callback OFF foram testados no harness, com essa limitação. Se houver um caminho real de remoção em memória, registrar precisamente, sem provocar a janela por debugger.
5. **Contabilidade mínima protegida:** alcançar naturalmente o planner de Rex buscando Support680; anotar carta lógica proposta e carta física consumida. Resolver sua primeira ação completamente. O bônus deve ser Monster (535 no cenário controlado de referência), não segunda Support344. Não escrever mão/deck/quotas para fabricar o resultado. Catálogo/estado real podem produzir outra jogada: registrar NOT_REACHED e como montar um estado legítimo equivalente.
6. **AI OFF/ON × Dual OFF/ON, com/sem RD:** repetir os cenários alcançáveis, reiniciar após mudanças que o exijam, conferir cadeia efetiva após primeira carga e OFF/ON. Um rank salvo não prova a cadeia executada. Reset, novo duelo, cancelamentos e seleção de instâncias devem ser observados no fluxo completo.
7. **Queen Lock — bloqueio novo a reproduzir:** com Ritual Effects e RD, invocar Cosmo Queen legitimamente no lado adversário; no turno do oponente, manter uma Female legal (395 no cenário de referência) na mão física, uma vaga livre e a busca ritual disponível. Verificar se RD joga680 ou restaura535 em vez da Female. A fixture reproduz isso, inclusive sem AI/Dual; não há prova de duelo Windows. Não usar a composição completa recusada para contornar o guard. Essa reprodução exige um perfil de diagnóstico RD+R sem Dual explicitamente preparado e deve parar ao confirmar a falha; não é perfil aprovado para jogar.
8. **HAL23:** mudanças no manager somente pelos menus e fronteiras realmente disponíveis. Não injetar uma chamada entre planner e swap. Se pending persistir, guardar seleção anterior/nova, ação física, sequência exata de UI e logs; não limpar pending manualmente.
9. **SAVE/LOAD:** primeiro no mesmo perfil/build, com dados de teste próprios e sem trocar objeto/ordem. Repetir reset e novo duelo. Perfil diferente/estado antigo deve respeitar as recusas do core; não ignorar a assinatura.
10. **Controles/render/áudio:** Select Sim/Não e fluxo de derrota, modo nativo de dois humanos separado de Dual Monster+Support, imagens/sons, textos, HD/3D e resoluções. Ainda não testados aqui.

## Coleta e limitações da observabilidade

O launcher ativa `MEMORIES_TRACE_MODS`, `MEMORIES_TRACE_DUEL_EFFECTS`, `MEMORIES_TRACE_STATE` e grava `logs/<perfil>-<hora>.log`. Guardar também o MANIFEST, buildid, perfil/template e settings efetivamente usados, Windows/arquitetura e passo a passo com cartas/slots/lados. Para crash, conservar o relatório e PDB do build. Revisar qualquer ZIP de replay antes de compartilhar: pode conter estado derivado do jogo ou dados pessoais; não anexar ROM/Dual/Time Wizard/saves pessoais ao repositório.

Os logs oficiais Windows não equivalem aos thunks de contagem da fixture. Para qualificar todos os caminhos de um wrapper, será necessária observabilidade não intrusiva adicional (por exemplo, debugger com PDB e contagem dos callbacks), sem registrar outro wrapper crítico que o guard deva recusar. Registrar essa limitação ao reportar apenas gameplay visual.

Time Wizard, seu núcleo/ponte Dual, cache de Encore já iniciado, adaptador AI0.1.1 e Droplist externo permanecem etapas separadas. Nenhum workaround foi instalado. **Não promover coleção completa ou guard à integração por causa de um build, pacote ou um teste visual isolado.**
