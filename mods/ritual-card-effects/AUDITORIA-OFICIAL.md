# Auditoria oficial de compatibilidade — Efeitos de Rituais 0.78.39

Inspeção em 2026-10-11, usando apenas leitura dos repositórios oficiais. O usuário confirmou **v0.3.0 Windows** como alvo e não soube informar a arquitetura. O pacote Windows publicado nessa release foi verificado como **i386/32 bits**; esse é o alvo adotado, correspondente também ao objeto recebido. Esta auditoria prepara o SDK desse alvo sem instalar o jogo, alterar o fork ou portar o MOD para x64. As referências oficiais locais continuam com arquivos rastreados limpos.

## Versões verificadas

| Referência | Revisão | API do host | Situação observada |
| --- | --- | --- | --- |
| Base citada pelo MOD / fork do usuário | `v0.2.1-preview.1`, `0d78202729967a5bbdbc886aa128e48e82bef7ac` | 10 | Commit de 2026-10-05; fork local está exatamente nessa revisão. |
| Tag oficial mais recente encontrada | `v0.3.0`, `663626b1d38355dd11f5532894a2b94169e7f349` | 12 | Tag/commit de 2026-10-11, confirmados por Git. |
| `master` oficial observado | `8337e161e43c809da91e964f0a6df1bbbba268b9` | 12 | Após a tag, só alterações do README: código igual ao da tag. |

A API mínima do MOD é **4**, independente da API 10 fornecida pela base original e da API 12 do executável v0.3.0 confirmado agora. A biblioteca recebida é `ritual-card-effects.o`, **ELF32 relocável i386**, carregada pelo próprio jogo; esse formato também é o correto no Windows de 32 bits. O ID `ritual-card-effects-test`, a chave `library` e as opções existentes devem ser preservados. Atualizar os headers usados na compilação não exige elevar a API mínima quando o MOD continua usando somente os serviços já disponíveis.

Os tags e o código foram consultados com a autenticação Git HTTPS já fornecida pelo ambiente. A API REST de releases do GitHub retornou HTTP 403 no proxy, mas as páginas HTTPS oficiais da release e dos assets funcionaram. As release notes, arquivos publicados e seus hashes foram então verificados por essas páginas. O arquivo `yfm-redecomp-v0.3.0-windows.zip` tem **7.829.947 bytes** e SHA-256 **`40f1fedd363c0db193f8d67dff48c49ebc375f5aa903279fd70d7c622ccb63a6`**, idêntico ao publicado pelo GitHub. O executável desse ZIP tem PE machine `0x14c` (**i386**). Nenhum asset `windows-x64` apareceu na lista observada; não se presume um pacote x64 publicado.

Proveniência e evidências foram salvas em `/workspace/mod-development/ritual-effects/sdk-v030`: ZIP original, `official-assets.json`, `official-assets.html`, release notes em HTML/texto e `SDK-PROVENANCE.json`. O SDK extraído tem **810 arquivos** e **3.964 nomes** na lista de exports; `exports.i386.txt` e `exports.txt` são idênticos. O executável/SDL/PDB de referência ficam em `runtime-reference`, sem terem sido executados.

Referências locais: `/workspace/reference/yfm-v021` (tag da base) e `/workspace/reference/yfm-official` (master observado). Referências públicas: [v0.2.1-preview.1](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/v0.2.1-preview.1), [v0.3.0](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tree/v0.3.0).

## Funcionamento atual do MOD

- Instala hooks públicos `host->hook`, guarda `original` em variáveis estáticas e chama a cadeia anterior. A ordem `after` inclui `melhoria-dos-rituais`, `ai-hard-mode` e `time-wizard-time-magic`, sem dependência `requires` obrigatória.
- Usa `MEMORIES_EVENT_DAMAGE` e oferece a interface opcional de leitura `ritual-card-effects-test:recipe_v1`, por `host->provide`. O catálogo verifica se o próprio MOD está ativo; isso respeita o contrato oficial, pois os ponteiros compartilhados permanecem carregados mesmo com o produtor desativado.
- As receitas são privadas: `ritual_recipe_data.h` define os requisitos e `ritual_recipe_compat.h` publica trios válidos em `gDuel_awRitualData`, com restauração e validação de identidades físicas. Apesar do comentário antigo no início de `mod.c`, o manifest recebido **não tem uma lista `rituals`**.
- Mantém categorias privadas 254 (Fiend que não seja Chakra nem cópia dele) e 255 (Skull Monster), alternativas de nomes e exclusões próprias. Essa semântica não existe integralmente nas condições oficiais atuais.
- O MOD só seleciona materiais do Campo. Um MOD opcional pode apresentar candidatos da Mão/Deck ao oracle público; este MOD valida, mas não busca nem consome essas origens.
- Usa `overlay_signature` para o cache oficial da sobreposição e packets/ordering tables nativas para parte das apresentações. Ambos já são recursos disponíveis na base e podem ser otimizados sem mudar a versão-alvo.

## Classificação das mudanças oficiais

| Categoria | Mudança comprovada | Benefício / decisão |
| --- | --- | --- |
| A — adoção recomendada no MOD atual | Continuar usando hooks encadeados, `provide`, cache `overlay_signature` e estruturas oficiais da base. | Melhorar os custos locais sem substituir efeitos ou aumentar a API mínima. Verificar equivalência funcional e preservar a identidade da instalação. |
| A — já disponível no executável-alvo | [VSync: `f75bcc22`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/f75bcc22fc7da20dc0c84a0fca1d2fae651282bf): o swap passa a cadenciar o VBlank, sem acrescentar trabalho de cada frame ao período. | Corrige perda de fluidez com widescreen, CRT e modelos em PCs antigos. O usuário já confirmou v0.3.0, que inclui a correção; ela não precisa ser copiada para este MOD. |
| A — já disponível no executável-alvo | [OpenGL: `352bf11b`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/352bf11b2d2a1cf97033ca263c11b7a922ae97da): resolve só regiões lidas de MSAA; shader deixa xBR de fora quando desligado. | Reduz custo de GPU em alta resolução, especialmente hardware antigo. v0.3.0 inclui a mudança. Medições descritas no commit pertencem ao projeto oficial; não são medições deste MOD nem garantia de ganho no PC do usuário. |
| B — adoção futura | [Rituais: `94b53ec4`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/94b53ec4078a0981c86d8cff822b5c40f6eb47cf), `Tables_RitualRule`, `Duel_RitualActive/Begin/Spend/End`, de 1 a 5 tributos do Campo/Mão/ambos. | Pode reduzir seleção/consumo paralelo em uma migração futura. Não cobre Deck, categorias privadas, exclusão do Chakra ou todas as alternativas do MOD. Substituir agora arriscaria alterar comportamento aprovado e integração com MelhoriaDosRituais. |
| B — adoção futura | [API 12: `017cee6a`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/017cee6a): `sound_add`, `sound_play`, `sound_free`. | API pública para sons próprios, com volume de SFX e ciclo de vida administrado. O MOD já usa substituições oficiais `audio.sfx`; a nova API não justifica migração para manter exatamente os sons aprovados. Exige API 12 e registro fora de Init. |
| B — adoção futura | [Cache de PNGs de sprites: `5cf944a4`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/5cf944a4b1e677acd8785704a3ffee85ae0d6533): PNG compartilhado por arquivo, pré-aquecido fora do render, geração própria e palette correta. | Bom padrão de referência para recursos visuais próprios. Está no executável e não pode ser retroportado sem alterar a recompilação; manter a otimização atual local ao MOD. |
| B — adoção futura | FM Editor passa a gravar `min_api` conforme as funções do manifest ([`7ef42b8b`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/7ef42b8ba3865e0379cddfaa069f098a8337567d)). | Útil em futuros manifests que adotem features novas; não elevar `min_api` do MOD atual apenas por usar um SDK mais novo. |
| C — não aplicável nesta etapa | Painéis móveis/importação Android, rotação, crash reports Android e backend traduzido macOS. | Sem benefício direto para a otimização Windows i386 solicitada. As otimizações do backend traduzido não comprovam ganho no backend Windows da base. |
| C — não aplicável nesta etapa | Abas UI/Map/Packs/Values, apresentação de 3D Monsters e edição de novos duelistas. | Recursos oficiais disponíveis para futuros projetos, mas sem necessidade de modificar estes rituais ou outro MOD agora. |
| D — exige atenção em port futuro | [SDK x64: `750080ad`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/750080ad), [loader por alvo: `2d918011`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/2d918011). | O código oficial pode compilar um jogo Windows x64 que precisa de `ritual-card-effects.x86_64-windows.o` ELF64 com ABI Windows x64. O pacote Windows **publicado e verificado** nesta etapa é 32 bits. Não ampliar o escopo para x64 nem distribuir ELF32 como se fosse x64. |
| D — exige atenção | Pointers armazenados no jogo continuam de 4 bytes (`G32`) mesmo em executável x64; prelude64 e casts são verificados no SDK atual. | Migrar requer revisão dos tipos/casts e compilação com ferramentas do alvo. Não basta recompilar com `gcc -m64`, produzir COFF/DLL ou trocar a extensão do objeto. |
| D — exige atenção | `Tables_Ritual` retorna -1 e `Tables_RitualRequirements` retorna 0 para receitas estendidas; consumidores precisam consultar `Tables_RitualRule` primeiro. | O MOD recebeu seu formato privado de 3 materiais; adaptar o provedor/oracle para os novos formatos requer trabalho separado e testes combinados. |
| D — exige atenção | Novos `card_effects` de magia/trap e correções de resolução/limpeza de batalha ([`f8b18375`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/f8b18375), [`686e2084`](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/commit/686e2084)). | Imunidades/proteções do MOD têm hooks de remoção e combate; migrar exige casos Ultimate, Sengenjin, Javelin, Tri-Horned, efeitos replacement e AI Hard Mode. Compilar não comprova esses comportamentos. |

As [release notes v0.3.0](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/releases/tag/v0.3.0) confirmam a queda de FPS com VSync/xBR corrigida, efeitos de monstro/magias/traps, melhorias de 3D Monsters e reestruturação do FM Editor. Elas afirmam manutenção de saves normais/settings e mods v0.1.2/v0.2.0, mas dizem que save states de versões anteriores não carregam. Isso descreve garantias do projeto oficial para suas referências; não comprova testes do MOD do usuário com v0.3.0.

## FM Editor: fonte oficial disponível

O FM Editor é parte do **mesmo repositório oficial**, em `tools/pc/fm_editor`; não foi localizado um repositório separado que precisasse ser presumido. A guia oficial é [tools/pc/fm_editor/README.md](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/v0.3.0/tools/pc/fm_editor/README.md). A versão observada suporta API 12 (`compat.py`).

Mudanças úteis verificadas no código: Rituals permite condições por carta, tipo, grupo, ATK/DEF, nível e DEF > ATK, além de 1–5 materiais Campo/Mão/ambos; Mod info mostra requisitos de API e stamp de `min_api`; Conflicts inspeciona sobrescritas e ordem; recovery periódico fica fora do thread de UI e undo armazena passos menores. Esses últimos recursos melhoram a experiência do editor, não os FPS do jogo. A exportação deve continuar sendo um diff em `mod.json`; o editor não compila os hooks C ou converte automaticamente a API privada `recipe_v1`.

Fontes particularmente relevantes: `rituals_tab.py`, `manifest.py`, `compat.py`, `overlaps.py`, `history.py`, `recovery.py`. A documentação oficial reconhece limites da validação: [notes/fm-editor-verification.md](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/v0.3.0/notes/fm-editor-verification.md) relata testes de gameplay Linux e editor empacotado Linux; eles não demonstram o runtime Windows deste MOD.

## Compilação correta para o alvo confirmado Windows v0.3.0

O SDK usado agora foi extraído do pacote Windows oficial com checksum publicado verificado. A lista de imports vem do **próprio SDK da release**, não de uma lista deduzida por nomes do fonte nem de exports inventados. Comando deste ambiente:

```sh
python3 /workspace/mod-development/ritual-effects/sdk-v030/sdk/tools/build_mod.py \
  '/workspace/mod-development/ritual-effects/optimized/zzz Efeitos Rituais' \
  --target i386
```

O script encontra automaticamente `sdk/include`, seus headers libc e `sdk/exports.i386.txt`, gera ELF32 e valida os nomes indefinidos contra essa lista real. No Windows, com o SDK distribuído junto ao jogo v0.3.0:

```powershell
python sdk/tools/build_mod.py "mods/zzz Efeitos Rituais" --target i386
```

A compilação aceita gcc existente ou clang + lld. Não exige llvm-objcopy para i386. Não se afirma que compilação ou resolução de imports demonstram gameplay Windows; nenhuma execução desse executável foi realizada nesta preparação.

## Referência histórica: compilação da base original

O SDK da base é o de `v0.2.1-preview.1`. O SDK já gerado em `/workspace/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tmp/pc/game32/sdk` corresponde ao mesmo commit e contém `exports.txt` para validar imports. Pode-se usar o compilador gcc existente ou clang + lld, respeitando os flags do script oficial.

```sh
python3 /workspace/reference/yfm-v021/tools/pc/build_mod.py \
  '/workspace/mod-development/ritual-effects/optimized/zzz Efeitos Rituais' \
  --game /workspace/Yu-Gi-Oh-Forbidden-Memories-Recompiled/tmp/pc/game32
```

Esse script da base **não tem `--target`**: gera i386 ELF32, válido para ambos os jogos de 32 bits. No Windows, usando o SDK distribuído com o mesmo jogo:

```powershell
python sdk/tools/build_mod.py "mods/zzz Efeitos Rituais"
```

O script faz freestanding, sem headers do sistema; realinha stack, mantém x87 e usa thunks indiretos oficiais. Não substituir por compilação C genérica com flags Windows/COFF. O fato de o objeto ter formato ELF não significa que só funcione em Linux.

Para documentar o formato de alvos adicionais do SDK atual, os comandos correspondentes seriam:

```sh
python3 tools/pc/build_mod.py 'MOD' --target i386
python3 tools/pc/build_mod.py 'MOD' --target x86_64-windows
```

O alvo x64 exige clang, lld, llvm-objcopy e headers compatíveis, gera ELF64 relocável com calling convention Windows (`--target=x86_64-w64-windows-gnu-elf`) e seção `.memories.abi`. Ver [notes/modding.md — Code mods](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/blob/v0.3.0/notes/modding.md#code-mods).

## Recomendação desta etapa

Compilar e validar imports para **v0.3.0 Windows i386**, alvo confirmado pelo usuário e pelo pacote publicado, preservando ID, configurações, três identidades físicas por ritual, categorias/exclusões privadas, efeitos e contratos públicos. Melhorar hot paths e uso do cache já existentes sem reescrever receitas ou migrar para x64. O alinhamento do SDK à versão informada está autorizado pelo trabalho pedido; uma mudança futura de regras/fontes de tributos ou port x64 deve ser avaliada separadamente conforme a diretriz. Nenhuma compatibilidade in-game Windows foi comprovada por esta inspeção/preparação.
