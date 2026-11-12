# Execução e evidências — HAL-20 / base oficial

Esta evidência acompanha o [relatório](../../HAL20_BASE_OFICIAL_V031_PREVIEW2.md) e os [resultados estruturados](RESULTADOS.json). Nenhum dos launchers temporários descritos abaixo foi aplicado ao código-fonte oficial. Os logs brutos permanecem no workspace; seus tamanhos/hashes são registrados no JSON. Os trechos abaixo são sanitizados e aparam a saída de compilação repetitiva, não as falhas.

## Comandos e ambiente

`$CHECKOUT` é a cópia isolada da integração; `$CHECKOUT_ANTIGO` é o checkout original preservado; `$TRABALHO` é o diretório de logs/helpers fora do checkout. O código oficial foi obtido por Git, com `git switch -c integration/hal-20-v0.3.1-preview.2 origin/master` seguido de `git merge --ff-only v0.3.1-preview.2`. O objeto da tag foi verificado como `aceac3782902746f35ddb0148809450933327cb3`; `git diff --exit-code` contra ele passou antes da documentação.

Foram reutilizadas somente dependências externas retidas (sysroot Linux e SDL), por links sob tmp/pc da cópia nova, nunca objetos de MODs ou do jogo antigo. CMake/Ninja estavam no venv preparado. O wrapper GCC previamente disponível injeta sysroot/startfiles somente para -m32, encaminhando o resto a /usr/bin/gcc; não altera fontes. Não foi reexecutado setup.sh sobre o checkout antigo. O builder Windows baixou seus próprios arquivos com pins oficiais.

```sh
export PATH="/workspace/.setup-venv/bin:/usr/bin:/bin"
CMAKE_BUILD_PARALLEL_LEVEL=4 MEMORIES_SKIP_WINDOWS=1 python3 tools/pc/build.py
CMAKE_BUILD_PARALLEL_LEVEL=4 python3 tools/pc/build.py --target windows

cmake -S . -B tmp/pc/tests -DBUILD_TESTING=ON \
  -DCMAKE_C_COMPILER="$CHECKOUT_ANTIGO/tmp/onboarding/bin/gcc" \
  -DCMAKE_PREFIX_PATH="$CHECKOUT_ANTIGO/tmp/onboarding/host-deps/usr" \
  -DCMAKE_LIBRARY_PATH="$CHECKOUT_ANTIGO/tmp/onboarding/host-deps/usr/lib/x86_64-linux-gnu" \
  -DCMAKE_BUILD_RPATH="$CHECKOUT_ANTIGO/tmp/onboarding/host-deps/usr/lib/x86_64-linux-gnu"
cmake --build tmp/pc/tests -j4
export PATH="$CHECKOUT/tmp/pc/llvm-mingw/bin:$CHECKOUT_ANTIGO/tmp/onboarding/bin:$PATH"
ctest --test-dir tmp/pc/tests --output-on-failure --timeout 90 -j4

cmake -S . -B tmp/pc/tests-windows -G Ninja -DBUILD_TESTING=ON \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER="$CHECKOUT/tmp/pc/llvm-mingw/bin/i686-w64-mingw32-clang" \
  -DPNG_LIBRARY="$CHECKOUT/tmp/pc/win32-deps/lib/libpng16.a" \
  -DPNG_PNG_INCLUDE_DIR="$CHECKOUT/tmp/pc/win32-deps/include" \
  -DZLIB_LIBRARY="$CHECKOUT/tmp/pc/win32-deps/lib/libzs.a" \
  -DZLIB_INCLUDE_DIR="$CHECKOUT/tmp/pc/win32-deps/include"
cmake --build tmp/pc/tests-windows -j4

python3 tools/pc/check_layouts.py --target i386
python3 tools/pc/check_layouts.py --target x86_64-windows
python3 tmp/pc/game32/sdk/tools/build_mod.py tmp/pc/game32/sdk/examples/mods/gameplay-rules \
  --target i386 --game tmp/pc/game32/sdk --out "$TRABALHO/sdk-example-i386.o"
python3 tmp/pc/win32/sdk/tools/build_mod.py tmp/pc/win32/sdk/examples/mods/gameplay-rules \
  --target i386 --game tmp/pc/win32/sdk --out "$TRABALHO/sdk-example-windows"
cmake --build tmp/pc/tests --target pc_audit
python3 tools/pc/test_vt_check.py
python3 tools/pc/test_model_gate.py
```

O `--out` do builder de MODs é diretório: no exemplo Linux, apesar do sufixo .o usado no nome do diretório temporário, a saída real é sdk-example-i386.o/rules.o. Não foi alterado o exemplo oficial.

## Sanitizadores: seleção e comando

CMake foi configurado em tmp/pc/tests-sanitizers com `-DMEMORIES_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug`, GCC do sistema e o mesmo prefixo/lib/RPATH das dependências host acima. Foram construídos os alvos memories_game_files_test, memories_pc_core_test, memories_fusion_test, memories_fusion_rules_test, memories_ai_hard_mode_test, memories_tables_test, memories_mods_manager_test, memories_state_io_test, memories_state_remap_test e memories_scratch_sweep_test. O último é fixture exigida automaticamente pelo CTest.

```sh
ASAN_OPTIONS=detect_leaks=0 ctest --test-dir tmp/pc/tests-sanitizers \
  -R '^pc_(game_files_.*|core|fusion|fusion_rules|ai_hard_mode|tables|mods_manager|state_io|state_remap)$' \
  --output-on-failure --timeout 60 -j2
```

ASan/UBSan ativos nos alvos configurados pelo CMake oficial; leak detection desligada. Nenhuma alegação de cobertura de leaks ou instrumentação integral dos targets.

## Adaptações de execução externas ao repositório

O host rejeita executáveis ELF32, inclusive harnesses simples. QEMU veio de https://deb.debian.org/debian/pool/main/q/qemu/qemu-user-static_5.2+dfsg-11+deb11u3_amd64.deb, SHA-256 `0b75df79ae2ddfbc4daab0223a0fc2e79c9be7b1e35c6d030879d4bd21684bca`, confrontado com o índice Debian retido. Apenas qemu-i386-static foi extraído no workspace, sem instalação/root.

O launcher temporário envolve subprocess.Popen somente para comandos cujo executável começa com ELFCLASS32. Acrescenta `qemu-i386-static -L "$CHECKOUT/tmp/pc/linux-sysroot"` antes do comando. Quando roda um teste Python oficial via runpy, aplica esse mesmo encaminhamento aos seus filhos ELF32. Nenhuma assertion é removida e nenhum resultado é transformado em sucesso. Os comandos originais dos 179 casos estão no JSON.

Para o harness lifecycle, a execução complementar usou um wrapper `gcc` temporário em primeiro lugar no PATH que apenas acrescenta `-pthread` ao comando encaminhado ao wrapper de sysroot. O teste oficial falhou sem esse ajuste, como preservado na rodada original. O mesmo launcher ELF32 executou a regressão LIBGS e o verificador oficial tools/pc/check_mod_exports.py.

### Código do launcher de emulação usado (fora da árvore versionada)

```python
"""Test-only launcher outside the repository: run ELF32 children under QEMU.

No production/test sources, assertions or results are changed. Native ELF64,
compilers, scripts and every non-ELF32 command are launched normally.
"""
import json
import os
from pathlib import Path
import runpy
import shutil
import subprocess
import sys

qemu = '$TRABALHO/tools/qemu-i386-static'
sysroot = '$CHECKOUT/tmp/pc/linux-sysroot'
original_init = subprocess.Popen.__init__

def emulated_init(self, args, *positional, **kwargs):
    if isinstance(args, (list, tuple)) and args and not kwargs.get('shell'):
        executable = os.fspath(args[0])
        candidate = executable if '/' in executable else shutil.which(executable)
        try:
            with open(candidate, 'rb') as stream:
                is_elf32 = stream.read(5) == b'\x7fELF\x01'
        except (OSError, TypeError):
            is_elf32 = False
        if is_elf32:
            args = [qemu, '-L', sysroot, *args]
    original_init(self, args, *positional, **kwargs)

subprocess.Popen.__init__ = emulated_init
command = sys.argv[1:]
if command[0] == '--python':
    sys.argv = command[1:]
    sys.path.insert(0, str(Path(sys.argv[0]).resolve().parent))
    runpy.run_path(sys.argv[0], run_name='__main__')
else:
    raise SystemExit(subprocess.run(command).returncode)

```

## Trechos dos logs e resultados

### build_linux — exit 0

Complete Linux i386 game: 628 units, SDK and bundled mods.

```text
$CHECKOUT/tmp/pc/sdl-m32-portable/libSDL3.a
tmp/pc/game32/mods: 3d-monsters, ai-hard-mode, drop-missing-cards (data), hand-camera, yamyi-mods
tmp/pc/game32/languages: de, en-eu, es, fr, it
tmp/pc/game32/memories-pc: 628 game units, 773 pinned data symbols, 12 outside_resident_image stubs, 18 sdk_asm stubs
```

### build_windows — exit 0

Complete Windows i386 cross-build: 628 units, SDK and bundled mods.

```text
-- Installing: $CHECKOUT/tmp/pc/win32-deps/include/freetype2/freetype/ftbzip2.h
-- Installing: $CHECKOUT/tmp/pc/win32-deps/include/freetype2/freetype/otsvg.h
-- Installing: $CHECKOUT/tmp/pc/win32-deps/include/freetype2/freetype/config/ftconfig.h
-- Installing: $CHECKOUT/tmp/pc/win32-deps/include/freetype2/freetype/config/ftoption.h
-- Installing: $CHECKOUT/tmp/pc/win32-deps/lib/pkgconfig/freetype2.pc
-- Installing: $CHECKOUT/tmp/pc/win32-deps/lib/libfreetype.a
-- Installing: $CHECKOUT/tmp/pc/win32-deps/lib/cmake/freetype/freetype-config.cmake
-- Installing: $CHECKOUT/tmp/pc/win32-deps/lib/cmake/freetype/freetype-config-release.cmake
-- Installing: $CHECKOUT/tmp/pc/win32-deps/lib/cmake/freetype/freetype-config-version.cmake
fetch https://github.com/mstorsjo/llvm-mingw/releases/download/20260922/llvm-mingw-20260922-ucrt-ubuntu-22.04-x86_64.tar.xz
fetch https://github.com/madler/zlib/releases/download/v1.3.2/zlib-1.3.2.tar.gz
fetch https://github.com/pnggroup/libpng/archive/refs/tags/v1.6.58.tar.gz
fetch https://github.com/freetype/freetype/archive/refs/tags/VER-2-14-3.tar.gz
fetch https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-devel-3.4.16-mingw.tar.gz
i686-w64-mingw32 deps: $CHECKOUT/tmp/pc/win32-deps
tmp/pc/win32/mods: 3d-monsters, ai-hard-mode, drop-missing-cards (data), hand-camera, yamyi-mods
tmp/pc/win32/languages: de, en-eu, es, fr, it
tmp/pc/win32/memories-pc.exe: 628 game units, 773 pinned data symbols, 12 outside_resident_image stubs, 18 sdk_asm stubs
```

### cmake_configure_linux — exit 0

Native CMake suite configured with sysroot compiler wrapper.

```text
-- The C compiler identification is GNU 14.2.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: $CHECKOUT_ANTIGO/tmp/onboarding/bin/gcc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Found ZLIB: $CHECKOUT_ANTIGO/tmp/onboarding/host-deps/usr/lib/x86_64-linux-gnu/libz.a (found version "1.2.11")
-- Found PNG: $CHECKOUT_ANTIGO/tmp/onboarding/host-deps/usr/lib/x86_64-linux-gnu/libpng.so (found version "1.6.37")
-- Found Python3: $VENV/bin/python3.12 (found version "3.12.14") found components: Interpreter
-- Configuring done (1.5s)
-- Generating done (0.3s)
-- Build files have been written to: $CHECKOUT/tmp/pc/tests
```

### cmake_build_linux — exit 0

All CMake test targets compiled.

```text
Building C object CMakeFiles/memories_state_requests_test.dir/tests/pc/state_requests_test.c.o
Built target memories_log_test
Building C object CMakeFiles/memories_state_requests_test.dir/src/pc/guest/state_requests.c.o
Built target memories_busts_make_test
Linking C executable memories_scratch_rules_test
Building C object CMakeFiles/memories_image_loader_test.dir/tests/pc/image_loader_test.c.o
Built target memories_scratch_rules_test
Building C object CMakeFiles/memories_image_loader_test.dir/src/pc/guest/image_loader.c.o
Building C object CMakeFiles/memories_function_map_test.dir/tests/pc/function_map_test.c.o
Linking C executable memories_state_requests_test
Linking C executable memories_image_loader_test
Linking C executable memories_function_map_test
Building C object CMakeFiles/memories_ot_chain_test.dir/tests/pc/ot_chain_test.c.o
Built target memories_state_requests_test
Built target memories_image_loader_test
Built target memories_function_map_test
Linking C executable memories_ot_chain_test
Built target memories_ot_chain_test
```

### cmake_configure_windows — exit 0

Windows CMake cross-build configuration.

```text
-- The C compiler identification is Clang 23.1.2
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: $CHECKOUT/tmp/pc/llvm-mingw/bin/i686-w64-mingw32-clang - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
-- Found ZLIB: $CHECKOUT/tmp/pc/win32-deps/lib/libzs.a (found version "1.3.2")
-- Found PNG: $CHECKOUT/tmp/pc/win32-deps/lib/libpng16.a (found version "1.6.58")
-- Found Python3: $VENV/bin/python3.12 (found version "3.12.14") found components: Interpreter
-- Configuring done (0.6s)
-- Generating done (0.1s)
-- Build files have been written to: $CHECKOUT/tmp/pc/tests-windows
```

### cmake_build_windows — exit 0

Windows CMake suite cross-compiled; not executed.

```text
[474/492] Linking C executable memories_audio_replace_test.exe
[475/492] Linking C executable memories_controls_config_test.exe
[476/492] Linking C executable memories_controls_runtime_test.exe
[477/492] Linking C executable memories_controls_window_test.exe
[478/492] Linking C executable memories_texture_pack_test.exe
[479/492] Linking C executable memories_card_layout_art_test.exe
[480/492] Linking C executable memories_mod_import_test.exe
[481/492] Linking C executable memories_card_art_test.exe
[482/492] Linking C executable memories_card_plate_test.exe
[483/492] Linking C executable memories_busts_make_test.exe
[484/492] Linking C executable memories_release_check_test.exe
[485/492] Linking C executable memories_log_test.exe
[486/492] Linking C executable memories_scratch_sweep_test.exe
[487/492] Linking C executable memories_scratch_rules_test.exe
[488/492] Linking C executable memories_function_map_test.exe
[489/492] Linking C executable memories_state_requests_test.exe
[490/492] Linking C executable memories_image_loader_test.exe
[491/492] Linking C executable memories_ot_chain_test.exe
```

### ctest_original — exit 8

164 passed, 13 failed, 2 skipped; details per case.

```text
	 16 - pc_rank_art (Skipped)
	 94 - pc_font_art (Skipped)

The following tests FAILED:
	 11 - pc_yamyi_mods (Failed)
	 12 - pc_trap_effects (Failed)
	 13 - pc_card_effects_start (Failed)
	 14 - pc_deck_draft (Failed)
	 15 - pc_rank (Failed)
	 29 - pc_ai_hard_mode_adapter (Failed)
	 53 - pc_mips_varargs (Failed)
	 54 - pc_mips_stack (Failed)
	 95 - pc_card_drops (Failed)
	 96 - pc_starchip_prize (Failed)
	107 - pc_results_names (Failed)
	135 - pc_mods_lifecycle (Failed)
	136 - pc_object_loader (Failed)
Errors while running CTest
```

### ctest_emulated — exit 1

12 supplemental cases passed; lifecycle still lacked -pthread.

```text
pc_yamyi_mods 0
pc_trap_effects 0
pc_card_effects_start 0
pc_deck_draft 0
pc_rank 0
pc_ai_hard_mode_adapter 0
pc_mips_varargs 0
pc_mips_stack 0
pc_card_drops 0
pc_starchip_prize 0
pc_results_names 0
pc_mods_lifecycle 1
pc_object_loader 0
```

### lifecycle_with_pthread — exit 0

Unmodified official harness: supplementary QEMU run, wrapper adds -pthread.

```text
memories-pc: cannot create /home/agent/.local/share/YFM Re-Decomp; using ./saves
memories-pc: mod reject: refused to start
memories-pc: mod no-api: MemoriesModInit did not set mod->api
memories-pc: cannot create /home/agent/.local/share/YFM Re-Decomp; using ./saves
memories-pc: mods: overlay loaded its code (overlay.o)
mods overlay: 2444 of 4000 passes drawn in part, 1556 with nothing changed
mods overlay: sustained partial redraws recycle the image cache
mods overlay: a sink takes the calls in order; pictures the host made keep their hash
mods overlay: pictures read ahead kept to their budget and let go with their mod
hooks: passed
mods lifecycle: linux passed
mods overlay: linux passed
mod save-state compatibility: linux passed
```

### asan_ubsan_selected — exit 0

19 selected CTest records passed; detect_leaks=0.

```text
12/19 Test   #5: pc_game_files_moved ..............   Passed    0.00 sec
      Start  19: pc_state_remap
13/19 Test   #8: pc_game_files_picker-failure .....   Passed    0.01 sec
      Start   7: pc_game_files_override
14/19 Test  #19: pc_state_remap ...................   Passed    0.00 sec
      Start   4: pc_game_files_remembered
15/19 Test   #4: pc_game_files_remembered .........   Passed    0.00 sec
      Start  10: pc_game_files_write-failure
16/19 Test   #7: pc_game_files_override ...........   Passed    0.01 sec
      Start  32: pc_core
17/19 Test  #32: pc_core ..........................   Passed    0.00 sec
18/19 Test  #10: pc_game_files_write-failure ......   Passed    0.01 sec
      Start 129: pc_scratch_sweep
19/19 Test #129: pc_scratch_sweep .................   Passed    0.00 sec

100% tests passed out of 19

Total Test time (real) =   0.16 sec
```

### layouts_i386 — exit 0

698 headers compared, 72 not standalone, 0 record differences.

```text
check_layouts: i386: 698 headers compared, 72 do not compile alone, 0 structures differ
```

### layouts_x64 — exit 0

759 headers compared, 11 not standalone, 0 record differences.

```text
check_layouts: x86_64-windows: 759 headers compared, 11 do not compile alone, 0 structures differ
```

### exports_linux — exit 0

Official verifier under QEMU, 4089 game exports checked against link.

```text
$CHECKOUT/tmp/pc/game32/memories-pc: 4089 mod exports, 4089 checked against the link
```

### sdk_linux_example — exit 0

Official gameplay-rules example compiled using generated Linux SDK.

```text
$TRABALHO/sdk-example-i386.o/rules.o: 1 source (tmp/pc/mod-build/gameplay-rules-d52464486740c525)
build_mod: built i386
```

### sdk_windows_example — exit 0

Official gameplay-rules example compiled using generated Windows SDK.

```text
$TRABALHO/sdk-example-windows/rules.o: 1 source (tmp/pc/mod-build/gameplay-rules-5242b8ef7ae3fed1)
build_mod: built i386
```

### pc_audit — exit 0

Port audit inventory generated, not proof all blockers are solved.

```text
Inventory native-port blockers and SDK references
PC audit: 1165 source files, 219 referenced resident SDK symbols, 61 ASM targets
Review candidates: {'address_to_32bit_cast': 193, 'asm_or_symbol_alias': 26, 'gte_macro': 237, 'ram_literal_or_integer_constant': 80, 'scratchpad_literal': 127}
Report: $CHECKOUT/tmp/pc/tests/port-audit.json
Built target pc_audit
```

### vt_check — exit 0

19 tests passed.

```text
...................
----------------------------------------------------------------------
Ran 19 tests in 0.047s

OK
```

### model_gate — exit 0

22 tests passed.

```text
......................
----------------------------------------------------------------------
Ran 22 tests in 0.128s

OK
```

### libgs_priority — exit 0

Official LIBGS harness passed with sysroot and QEMU.

```text
LIBGS priorities: valid links, endpoint clamping and adjacent memory passed
```

### Primeiro erro de link do lifecycle sem -pthread

```text
/usr/bin/ld: $CHECKOUT/tmp/pc/mods-lifecycle/cc6hc54y.o: in function `ahead_thread':
mods.c:(.text+0x53d7): undefined reference to `pthread_sigmask'
/usr/bin/ld: $CHECKOUT/tmp/pc/mods-lifecycle/cc6hc54y.o: in function `host_prefetch_image':
mods.c:(.text+0x58a4): undefined reference to `pthread_sigmask'
/usr/bin/ld: mods.c:(.text+0x58bb): undefined reference to `pthread_create'
/usr/bin/ld: mods.c:(.text+0x58df): undefined reference to `pthread_sigmask'
/usr/bin/ld: mods.c:(.text+0x58f8): undefined reference to `pthread_detach'
collect2: error: ld returned 1 exit status
Traceback (most recent call last):
  File "$TRABALHO/emular.py", line 36, in <module>
```

### Observações sobre as tentativas corrigidas

A primeira chamada de check_layouts usou target windows (inválido); depois foram usados i386 e x86_64-windows. A primeira seleção ASan omitiu o build da fixture scratch_sweep e registrou Not Run; após compilar a fixture, os 19 registros passaram. São ajustes da orquestração, não patches de fonte. As falhas originais de execução ELF32 e pthread continuam separadas dos resultados complementares.
