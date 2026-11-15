# HAL-20 — composição candidata e reprodução

[Relatório](../../notes/HAL20_CONSOLIDACAO_CANDIDATOS.md). Base oficial API24/i386. Os quatro pacotes próprios são cópias **integrais e idênticas** dos PRs9/22/27. Dual Play é privado/local. O guard é opt-in, experimental; não certifica gameplay Windows.

## Ambiente

Python/Pillow, SDK Windows oficial preview2, Clang/llvm-mingw, GCC/libs i386, objetos nativos ELF32 da base e QEMU se o host não executar i386:

```sh
export MEMORIES_AI_DIR=/caminho/base/mods/ai-hard-mode
export MEMORIES_DUAL_DIR=/caminho/privado/dual-api24
export MEMORIES_RITUALS_DIR="$PWD/mods/melhoria-dos-rituais"
export MEMORIES_FUSION_DIR="$PWD/mods/fusion-auxiliary-balance"
export MEMORIES_ENGINE_OBJ_DIR=/caminho/base-linux-i386/obj
export MEMORIES_SDK_EXPORTS=/caminho/base-linux-i386/sdk/exports.i386.txt
export MEMORIES_I386_CC=/caminho/gcc-i386
export MEMORIES_I386_RUNNER='/caminho/qemu-i386-static -L /caminho/sysroot'
export MEMORIES_SDK=/caminho/sdk-windows
export MEMORIES_MOD_CC=/caminho/llvm-mingw/bin/clang
```

Os arquivos e SDK são fixados em `INPUTS.json`; insumo divergente impede o teste, não é substituído automaticamente. Todos os objetos de MOD são ELF32 REL i386, o formato esperado pelo loader Windows; não são DLLs PE.

## Composição privada

```sh
python tests/hal20-consolidacao/stage.py --output /pasta/fora/do/repositorio/colecao \
  --ai "$MEMORIES_AI_DIR" --dual "$MEMORIES_DUAL_DIR"
python tools/pc/build_game32.py --target windows --release
```

O stager não baixa Dual, não o altera, não sobrescreve destinos e exige SHA exato. Cria sete IDs únicos (M/F/R/RD/H/D/DMC), settings de ativação/ordem e recibo privado. A única alteração de manifest é `restart:true` **na cópia local do AI**; todas as preferências estratégicas e demais manifests permanecem originais. `--profile owned-only` monta M/F/R/RD/DMC explicitamente. Para executar o build, aponte `MEMORIES_MODS_DIR`, `MEMORIES_SETTINGS` e `MEMORIES_USER_DIR` para as pastas privadas criadas; não publique a pasta que contém Dual. O build oficial usa uma allowlist de MODs: ele sozinho não monta a coleção customizada. Os objetos canônicos fixados devem prevalecer no staging; recompilações com outro caminho de debug não são automaticamente admitidas.

## Matrizes combinadas

```sh
python tests/hal20-consolidacao/run.py --composition --guard both --trace-returns --output /tmp/comb144
python tests/hal20-consolidacao/run.py --composition --guard 1 --fence --output /tmp/fence72
python tests/hal20-consolidacao/run.py --subsets --guard 1 --output /tmp/subsets63
python tests/hal20-consolidacao/run.py --providers --guard 1 --output /tmp/providers63
python tests/hal20-consolidacao/run.py --consolidation --guard 1 --output /tmp/boundaries20
python tests/hal20-consolidacao/run.py --manager --guard 1 --output /tmp/manager24
python tests/hal20-consolidacao/run.py --spoof --guard 1 --output /tmp/spoof4
```

`--manager` usa `restart:true` só no manifest AI temporário. `--spoof` acrescenta bytes de teste ao objeto Fusion **numa cópia descartável**, conserva ID/código executável e exige recusa antes do controller. Não altera artefatos de referência.

`consolidation.inc`: 1 reproduz a perda de Queen após restore/chunks/reset/event, mesmo numa fronteira quieta; 2 recusa load no retorno do planner e após consumo; 3 recusa em quarentena; 4 callback errado/duplicado, predecessor corrompido e wrapper desconhecido; 5 mudanças efetivas/preferências/ordem; 6–11 staged window, cancelamento, falha simulada de reinício, primeira carga AI tardia e reload. Não restaura cartas após commit. O callback de observação somente lê a pré-validação de load; não cria pending/planos/quotas.

O guard verifica callbacks sem os thunks **exclusivamente de instrumentação deste harness**. O build do jogo não contém esse adaptador. O observador que seria um hook extra não é registrado nos perfis com guard; o retorno instrumentado do planner observa a mesma fronteira.

## Regressões preservadas

```sh
python tests/hal20-consolidacao/run.py --output /tmp/main174
python tests/hal20-consolidacao/run.py --hardening --output /tmp/hardening72
python tests/hal20-consolidacao/run.py --boundary --trace-returns --output /tmp/boundary128
python tests/hal20-consolidacao/run.py --recovery-only --output /tmp/recovery64
python tests/hal20-consolidacao/run.py --reset-recovery-only --output /tmp/reset16
python tests/hal20-consolidacao/run.py --restrictions-only --output /tmp/position2
python tests/hal20-consolidacao/regressions.py --qualified-loader --output /tmp/canonical
python mods/fusion-auxiliary-balance/tests/run_preview2.py
python mods/melhoria-dos-rituais/tests/run_preview2.py
python tests/hal20-consolidacao/audit.py --output /tmp/audit.json
```

Fixtures de producers adversariais ficam com guard OFF: não autorizar automaticamente um produtor de teste como produção. Os cenários combinados usam objetos reais. `--reference26 --boundary --bcase 12 --bcase 13 --quick` com `MEMORIES_PR26_DIR` reproduz quatro controles e oito FAIL funcionais esperados do cursor/pilha antigo. A auditoria imutável original de RD continua falhando sobre a candidata autorizada; não foi removida. `--qualified-loader` altera **apenas uma cópia temporária** da expectativa8→9 hooks e é documentado desde PR26. `audit_pr27_reference.py` preserva o auditor antigo como referência; sua exigência de core intocado não é a política autorizada desta consolidação.

`boundary.inc` preserva disconnect/ACK/PROT_NONE, reset/OFF reentrante, gerações, vetos persistentes, recuperação explícita, três cartas combinadas, não rollback, cursor/pilha e rituais Field/Both/Hand/Vanilla. Produtor de proposta física combinada, interrupções reentrantes e alterações de identidades são instrumentação explícita, não gameplay normal.

## Limites

São reais o manager, loader, MODs, hooks e as unidades nativas indicadas no recibo. Catálogo/VM retail, render, conclusão de animação, avanços de fase e parte do motor são modelos. `Duel_InitScene` tem corpo de motor simulado, com hooks reais. O monitor próprio não garante segurança de qualquer materializador nativo depois da última validação. Recusa segura não é vitória de gameplay. Save/load aqui não inventa serialização de BSS nem compatibilidade com estados antigos.

[Automação PE32](windows/README.md): está autorizada agora; indisponibilidade deve ser demonstrada por controles e logs, nunca atribuída a uma proibição do usuário. Só os testes pessoais jogando ficam para o fim.
