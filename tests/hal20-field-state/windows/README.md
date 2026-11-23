# Automação PE32 isolada

Esta etapa está autorizada agora. O bloqueio encontrado foi técnico, não uma exigência de testes manuais do usuário. Ver `notes/hal-20-field-state/windows/ENVIRONMENT.json` e os logs públicos (apenas texto).

`run.py` e `direct.py` preservam os probes Wine/controle mínimos do PR28; consultar `--help` para caminhos explícitos. `qemu_backend.py` reproduz a tentativa de roteamento ELF32 em cópia privada dos binários Wine, com timeout por CPU. `hello.c` e `driver.c` são controles próprios. Não fornecer imagem do jogo ao controle mínimo nem presumir que build PASS signifique runtime PASS.

QEMU11.1.2+Wine11 executou controle PE64, mas PE32 falhou antes do marcador de entrada. Wine10 x86/QEMU11 inicialmente falhou ao executar wineserver ELF32; wrappers explícitos numa cópia privada avançaram até uma illegal instruction no bootstrap (também CPU max). Root cause de instrução/módulo não confirmada. Nenhum controle PE32 chegou a main. O jogo atual e a composição real são NOT_REACHED; backend AUTOMATION_UNAVAILABLE.

O ambiente selecionado é Linux, sem VM Windows funcional ou imagem do jogo acessível. Varredura de recursos privados acessíveis encontrou ZIPs de MODs e buffers gráficos; não se presume acesso a imagem enviada em outro contexto. Usar apenas recurso legítimo em caminho/mount privado ou referência da tarefa quando disponível. Não publicar ROM, saves, binário do jogo, prefixes, core dump ou pacote Dual. Não solicitar gameplay pessoal intermediário.
