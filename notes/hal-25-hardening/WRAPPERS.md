# HAL-25 — inventário de interposição por artefato e execução

[Inventário completo JSON](WRAPPERS.json), [CSV](WRAPPERS.csv) e [matriz de cenários](MATRIZ.csv). Foram vinculados 125 nós, incluindo instrumentação e bindings nativos, em 72 processos; nenhum registro de hook ficou sem correspondência no ELF. Essa correspondência é identificação, **não liberação automática de compatibilidade**.

Para cada registro, a instrumentação captura o ponteiro efetivamente passado a `Hooks_Add`, resolve o símbolo no objeto carregado, registra o hash do loader e associa SHA-256 do artefato, seção/offset, tamanho, hash do corpo e hash das relocations. Nome e ID servem apenas como rótulos. O inventário não publica bytes nem fontes privados do Dual. SHA-256 completo e função são as chaves de evidência; não se usa nome/ID como whitelist.

| Artefato | Loader code | Papéis nos ramos observados |
|---|---|---|
| R `3c472220…` | `1e478c68` | seleção obrigatória por provider real; wrapper de seleção; hand-controller delegante |
| RD `92664e4a…` | `98d3ee01` | planner, transação, materialização própria ou delegada, monitor do controller |
| Melhorias `c36d033f…` | `1fe34a1e` | hand-controller delega no lado CPU; recipes reais nos testes históricos |
| Fusion F1 corrigido `384e11ee…` | `afb75202` | hand-controller delega no lado CPU; código e assets preservados |
| AI oficial `9171b85b…` | `3e0648cb` | planner e materializador independentes; pode não delegar o swap |
| Dual privado `77cd32ff…` | `3028c68a` | gate/ledger e controller próprios, somente teste local |

O JSON contém 50 callbacks registrados de R, 9 RD, 7 Melhorias, 33 Fusion, 9 AI e 6 Dual, mais 10 bindings nativos e 1 observer de teste. As impressões de função são específicas aos objetos listados em INPUTS/BUILD; não qualificam recompilações futuras apenas pelo mesmo nome.

As prioridades são registradas por perfil: Melhorias=-50, Fusion=-30/+30, R=-20/+40, RD=10, AI=0/+50, Dual=20. O resultado efetivo é a sequência de `CALLBACK_EDGE` e `TRACE`, não uma ordem presumida pelos ranks. As arestas repetidas permanecem no JSON, inclusive chamadas aninhadas e retornos. Os ranks empatados/ativação tardia do manager pertencem às evidências históricas HAL-22/24; esta matriz varia as ordens relevantes de R/F/H e não afirma repetir toda a UI do guard.

`CALLBACK_EXIT` mede efeitos inclusivos; `CALLBACK_SELF` exclui os filhos críticos rastreados. Contadores não devem ser somados entre pai e filho para deduzir materializações. O tracer de retorno é opt-in, só para quatro alvos cdecl conhecidos: `AiScript_Run`, `func_8001BAF0`, `DuelScene_UpdateHandActions`, `DuelEffect_ApplyRitual`. Não interpreta ABIs arbitrárias nem todas as chamadas C internas. Nos demais hooks, registro e entradas podem estar identificados sem haver qualificação do retorno/ramo. Ausência de evento não prova pureza.

Nos ramos CPU exercitados, os hand-wrappers de Melhorias e Fusion delegam e não escrevem seleção/deck nem consomem cartas por conta própria. R modifica seleção no callback de planejamento e respeita a transação; RD materializa 24 ações próprias na matriz, enquanto o controller nativo realiza o consumo físico. AI pode modificar seleção e executar seu caminho sem chamar o predecessor. Dual pode zerar uma seleção vetada. Os dados inclusivos/exclusivos por função e os caminhos por caso permitem verificar essas afirmações sem atribuir todo efeito a quem apenas delegou.

Interposições adversariais próprias verificam escrita antes/depois do swap, escrita depois do retorno do controller e permuta física coerente entre cópias de mesmo ID. A nova fronteira evita o consumo obsoleto nesses pontos. Um wrapper que não delega deixa a ação NOT_REACHED. Um wrapper que consome por conta própria antes de devolver controle permanece fora da garantia; não existe aqui uma barreira global de commit. Alterações após o primeiro consumo ou nos ramos humanos de Surrender/Select e Fusion/Square não foram qualificadas por esta matriz. Gameplay/UI Windows ficam BLOCKED até a fase final.

## Guard #20 somente como referência

`guard_reference.py` compila o `manager.c` **inalterado** do commit `936c3513c9e6f8131b1eb4a5410d9c21c3c2c592` em executável isolado e exercita seu predicado de versão. O novo code RD=`98d3ee01` não está na qualificação antiga. Com Dual ON, a referência recusa antes de percorrer hooks; com Dual OFF, o predicado não se aplica. São duas REFUSED e duas NOT_APPLICABLE, não quatro liberações do conjunto. A prova usa stubs deliberadamente inalcançáveis para travessia; não é uma execução de duelo com o guard incorporado. Nenhum arquivo do guard/core foi promovido, nem se ampliou whitelist.
