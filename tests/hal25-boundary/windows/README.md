# Automação WinAPI/PE32 — tentativa reproduzível

A autorização atual permite testes automatizados agora. Somente participação pessoal do usuário jogando fica para o final. Esta tentativa usa Wine10.0, uma camada de compatibilidade em Linux, **não um Windows nativo**.

O helper é nosso: compila controles PE32/PE64 e um driver Win64 que cria o processo filho, observa somente janelas desse PID, envia Escape/WM_CLOSE quando houver janela e registra saída/timeout. Não interage com janelas pessoais. Não prepara dados de jogo fictícios. Prefixo, usuário, display Xvfb e cópia do executável ficam numa pasta nova de teste.

```sh
python tests/hal25-boundary/windows/run.py \
  --wine /caminho/wine-10.0-amd64-wow64/bin/wine \
  --xvfb /caminho/Xvfb \
  --cc-root /caminho/llvm-mingw/bin \
  --game-dir /caminho/build-oficial-win32 \
  --work /tmp/hal25-windows-new
```

Resultado observado: prefix-init e controle PE64 PASS; controle PE32 falha antes de main (CreateProcess erro1359); jogo PE32 headless/visível falha na criação (erro731). Execuções diretas do PE32 mínimo e do jogo também terminam SIGSEGV na camada Wine. Assim os casos de jogo são AUTOMATION_UNAVAILABLE; UI/input/restart/duelo/save-load são NOT_REACHED, sem atribuir falha funcional aos MODs.

Wine32 sob QEMU foi tentado separadamente: sem WINELOADERNOEXEC não executa o loader; com ele há timeout antes da entrada do programa, repetindo mmap; outra base de guest também termina em timeout. São limites das tentativas neste ambiente, não prova de impossibilidade em outros backends. Wine64 console funcional não valida UI/gameplay i386.

Runtime obtido da release pública https://github.com/Kron4ek/Wine-Builds/releases/tag/10.0, arquivos `wine-10.0-amd64-wow64.tar.xz` e `wine-10.0-x86.tar.xz`, ambos conferidos com `sha256sums.txt` da mesma release. Xvfb foi extraído localmente do pacote oficial Debian `xvfb_21.1.16-1.3+deb13u4_amd64.deb`; nenhum sistema/SDK de terceiros foi alterado. Runtime, executável do jogo, SDL e toolchain não são publicados; fontes próprios e hashes permitem repetir a tentativa.

Além do backend PE32, falta uma imagem legítima do jogo neste workspace. O executável Linux oficial sob QEMU reporta isso explicitamente (exit1). Nenhuma ROM/imagem foi baixada ou fabricada. Para a próxima execução automática será necessário um backend PE32 funcional e esse recurso legítimo privado; não é exigido teste manual intermediário.

[Evidências e ambiente](../../../notes/hal-25-boundary/windows). Scripts não certificam savestates antigos nem convertem falha do controle mínimo em FAIL de gameplay.
