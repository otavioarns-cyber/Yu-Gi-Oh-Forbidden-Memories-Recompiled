# Automação Windows / PE32

Automatizar agora é permitido. Os scripts usam um prefixo e display isolados, controles PE32/PE64 próprios, driver WinAPI próprio (criação, janela por PID, input/fechamento) e o executável legítimo compilado da base. Wine não equivale a Windows nativo.

```sh
python tests/hal20-consolidacao/windows/run.py \
 --wine /caminho/wine-11.0-amd64-wow64/bin/wine --xvfb /caminho/Xvfb \
 --cc-root /caminho/llvm-mingw/bin --game-dir /caminho/build-win32 \
 --work /tmp/windows-isolado-novo
python tests/hal20-consolidacao/windows/direct.py \
 --wine /caminho/wine-11.0-amd64-wow64/bin/wine --xvfb /caminho/Xvfb \
 --work /tmp/windows-isolado-novo
```

Wine11.0 WOW64 obtido de https://github.com/Kron4ek/Wine-Builds/releases/tag/11.0. Arquivo `wine-11.0-amd64-wow64.tar.xz`, SHA256 `39574efa1132c3ca0d5c77dd2eddbe4a49cca0d6cc2c290ff4924493a1c40314`, conferido com `sha256sums.txt` da release. Não foi instalado no sistema nem alterou prefixos anteriores. QEMU x86_64 5.2.0 + Wine10 foi tentado antes desta variante: PE64 iniciou; PE32 continuou sem entrada de programa. As tentativas anteriores Wine10, Wine32/QEMU e ELF32 nativo permanecem no PR27.

Nesta entrega: prefix-init e PE64 PASS; `CreateProcess` de PE32 e jogo termina erro731/driver91. A chamada direta com cache gravável passa PE64, mas PE32/jogo saem1 com ShellExecuteEx/Internal error. Assim o backend é AUTOMATION_UNAVAILABLE antes de main. Interface, controles, reinício real, duelos e load integral ficam NOT_REACHED, não FAIL dos MODs. O preview estático Linux também encontrou headers png/freetype/fontconfig ausentes no compilador host; não substitui backend de jogo.

Não há imagem legítima fornecida para o jogo no workspace. Não baixar nem produzir imagem fictícia. Quando existir backend funcional e recurso legítimo privado, repetir startup, UI/restart, duelos, efeitos, fronteiras e load de forma automatizada; a autorização já existe. Somente a bateria pessoal jogando é reservada à última fase.

Fontes próprios, logs e hashes são publicáveis. Runtime Wine/SDL, executável do jogo, prefixos e imagem privada não acompanham o PR. Ver [evidências](../../../notes/hal-20-consolidacao/windows).
