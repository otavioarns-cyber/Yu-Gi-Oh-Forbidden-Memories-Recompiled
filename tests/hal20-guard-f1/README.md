# HAL20 — integração experimental guard PR20 + Fusion F1 PR22

Base `d649647c159c91d615c07d6960928a37e270b461`. Os quatro pacotes próprios e os cinco fontes do guard são copiados literalmente dos commits identificados em `notes/hal-20/guard-f1/INPUTS.json`. Não se recompilam/substituem objetos dos MODs no runner. Dual é dependência privada local; não acompanha a branch. Nenhum wrapper desconhecido é liberado.

```sh
export MEMORIES_AI_DIR=/caminho/ai-hard-mode-oficial
export MEMORIES_DUAL_DIR=/privado/dual-play-PR10-API24
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base/game32/obj
export MEMORIES_SDK_EXPORTS=/caminho/base/game32/sdk/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot'
python tests/hal20-guard-f1/run.py --output /tmp/hal20-matrix
python tests/hal20-guard-f1/run.py --queen --output /tmp/hal20-queen
```

`--smoke` executa seis casos de preparação; não substitui as matrizes finais. Usar diretório novo. Os runners retornam1 diante dos FAIL diagnósticos e2 se a coleta/build fica incompleta. As recusas do guard aparecem como **REFUSED**, com controller **NOT_RUN**, e não como gameplay aprovado. Os JSONs preservam PASS/FAIL e as observações brutas. Não trocar contabilidade, seleção, configurações ou guard para produzir PASS.

A matriz principal tem280 processos: todas as63 combinações não vazias dos seis MODs × guard OFF/ON nos controllers; providers nos32 subconjuntos com Fusion; tributos nos32 com Melhoria;14 provas de estado; ordens inseguras e posição dos wrappers. Os20 processos Queen são adicionais e disjuntos como coorte; usam cinco composições × guard OFF/ON × wrappers antes/depois do trio. A topologia ativa é registrada pelo registry real, por alvo e por ordem de execução; ranks não são tratados como prova.

O observer reaproveitado do #20 só lê após o planner; não cria pending, plano ou quota. O controller, snapshots, planners e materializadores disponíveis são reais. VM retail é stub de conclusão sem seleção escrita; catálogo, RAM inicial, render e resolução de efeito/placement→campo são modelos explícitos. A concessão de bônus usa callbacks reais sobre essa ponte. As buscas680/Monster535 e a exclusão344 estão nos perfis qualificados, não representam um duelo completo Windows.

As cópias temporárias preservam manifests e defaults (inclusive AI restart:false original). Enabled/ranks, opt-in novo e Drop Missing Cards ON são somente perfis isolados. Não são usados manifests experimentais restart:true permanentes. Não se altera min_field para fazer passar tributos só da mão; com default1 essa tentativa deve ser recusada. Field e Both usam três instâncias físicas e preservam cópias não escolhidas. O primeiro protótipo tinha stats sintéticos de Yamadron inferiores ao catálogo exigido; somente o catálogo da fixture foi corrigido para1600/1800.

Providers/F1 usam lease77 **semeada e identificada**, seguida de OFF efetivo por `Hal20_ForceApplied`. Essa ponte chama o `activate` oficial para isolar callbacks, contornando restart somente após admissão; não é uma janela de UI demonstrada. A prova separada de preferência OFF preserva restart e aplicação atual. Novo grant chama o bind real qualificado (fastcall privado do compilador), serial78. Nenhum código de produção recebe essa ponte.

A coorte Queen prepara campo/mão sintéticos, marca Cosmo Queen pelo hook público real de commit ritual e apresenta395 como Female legal, com vaga livre. Não escreve marca privada, pending, plano ou quota. A busca RD é real. O serviço externo de catálogo da fixture modela explicitamente395 no grupo Female, conforme o jogo. O controle Ritual+AI sem RD passa; com RD ocorre o conflito documentado. Isso não prova que um duelo Windows chega ao mesmo estado. Não implementar correção nem criar um provider adicional com base apenas no nome do MOD.

`manager_bridge.c` expõe callbacks somente para testes; o conteúdo de `mods.c` incluído é exatamente o #20. `window_bridge.c` permanece a ponte anterior, sem mudança de produção. `native_load` usa dez unidades i386 oficiais anteriores conferidas por hash. Só harness/observer e entrada experimental do duelo são compilados. SAVE/LOAD visita apenas buffers registrados, sem inventar persistência da BSS ou certificar estados antigos.

[Procedimento Windows](WINDOWS.md) e `package_windows.py` geram um ZIP **local** com o core PE32 compilado, os objetos próprios pinados, AI oficial e Drop Missing Cards. O ZIP não é publicado como release e não inclui Dual, ROM ou saves pessoais. O launcher verifica hashes e usa dados/logs separados; o perfil completo tem recusa esperada.

Resultados: `notes/hal-20/guard-f1/main/` e `queen/`. Os arquivos `RAW_LOGS.tar.gz` contêm **todos** os logs por nome. Alguns logs relevantes também estão expostos como texto; para materializar os demais nomes referenciados na matriz, extrair o arquivo no próprio diretório. `LOG_INDEX.json` contém hashes de cada log original. Os JSON/CSV são resultados completos, não apenas uma seleção verde. Build Windows e regressão Fusion estão na pasta de resultados geral. Nenhum arquivo privado dos consumidores é distribuído.
