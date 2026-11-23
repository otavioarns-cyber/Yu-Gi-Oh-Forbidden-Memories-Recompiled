# HAL-9 — execução do objeto Time Wizard e contratos locais

Candidata experimental sobre PR30, **não uma reconstrução integral do core**. O objeto privado usa três imports adicionais, somente no core desta branch, com `experimental.hal9_contracts=1`. O SDK/host é API24/i386; o core legado continua declarando `mod.api=4`. Não instalar esse objeto em jogo oficial sem a ponte nem habilitá-lo no guard por nome.

Pré-requisitos legítimos: pacote original com os 38 hashes de `notes/hal-9/TIMEWIZARD_INPUTS.json`, SDK oficial preview2/API24, Clang do SDK, binutils, Python/pyelftools; para testes integrados, Dual privado com SHA do PR10, objetos nativos do motor e sysroot/runner ELF32. Não baixar ou publicar imagem/saves/Dual. Definir os `MEMORIES_*` já usados no README de `tests/hal20-field-state`; apontar AI/M/F para os arquivos desta branch, não para versões anteriores.

```sh
python tests/hal9-timewizard-contracts/build.py --package "$HAL9_PACKAGE" --sdk "$MEMORIES_SDK" --output "$HAL9_WORK/candidate"
```

O script valida os inputs, preserva o pacote e compila em cópia. Paths de debug normalizados permitem reprodução em outro diretório; dois builds deram o SHA `6470024d45bd099b7800bd5f84ccdcef1a895a8a2447ecef058af11ada97d5d7`. Import binding exige esse hash completo. Uma versão de compilador que produza outro hash exige auditoria explícita; não regenerar os pins como forma de contornar recusa.

```sh
python tests/hal20-field-state/run.py --inputs notes/hal-9/INPUTS.json --fence --timewizard-object "$HAL9_WORK/candidate/package" --tw-case 0 --tw-case 1 --tw-case 2 --tw-case 3 --tw-case 4 --tw-case 5 --tw-case 6 --tw-case 7 --tw-case 8 --tw-case 9 --tw-case 10 --tw-case 11 --tw-case 12 --tw-case 13 --output "$HAL9_WORK/runtime"
python tests/hal9-timewizard-contracts/regressions.py --package "$HAL9_WORK/candidate/package" --sdk "$MEMORIES_SDK" --output "$HAL9_WORK/native"
python tests/hal9-timewizard-contracts/audit.py --original "$HAL9_PACKAGE" --candidate "$HAL9_WORK/candidate/package" --build "$HAL9_WORK/candidate" --output "$HAL9_WORK/AUDIT.json"
```

O controle original usa `--tw-reference --timewizard-object "$HAL9_PACKAGE"`: termina com FAIL históricos esperados, não deve ser transformado em teste positivo. Casos0–10 exercitam fronteiras;11 executa Queen/consumo físico;12 tenta Time Magic na coleção;13 tenta com RD frio OFF. Os16 últimos perfis podem terminar NOT_REACHED para Time Magic sem forçar seleção. PASS de consulta não significa ação concluída.

A fixture acrescenta wrappers de trace e desembrulha seus ponteiros em `Hooks_Inspect`, tornando visíveis os callbacks reais qualificados. O traço preserva owner, code hash, símbolo, tamanho, ordem, delegação e contagens. VM, catálogo, renderer/disco são fronteiras modeladas; o loader, manager, hooks, providers, planners e controllers indicados executam código real. Para CPU sem FXSR foi usado `qemu-i386-static -cpu pentium2,-fxsr`; isso é teste ELF32 do probe, não suporte Windows para esse CPU.

`regressions.py` conserva testes recebidos: open_wheel/card_use reais no objeto final, transporte wheel/renderer modelado; três consultas de host ficam neutras **somente nesses testes freestanding**. Os testes de manager acima usam as consultas reais. Compara5120 seleções,672 decisões e18 asserts de evolução; não confundir essas unidades com gameplay UI.

`official_regressions.py` executa testes oficiais sem editar asserts, roteia ELF32 ao runner e adiciona `-lpthread` ao link do sysroot Debian11 (separado antes de glibc2.34). `policy_tests.py` e `load_prewrite_test.py` da HAL26 são reutilizados sem edits.

A composição admite apenas os hashes anteriores qualificados. Com Time Wizard, o guard **deve recusar**. Não copiar a candidata para os MODs canônicos nem executar o builder antigo do pacote como se preservasse esta correção: ele recompõe novamente o núcleo sem estes patches.

`collect.py` publica metadados/logs próprios; compacta somente repetições exatas de trace após três ocorrências, com contagem e SHA dos logs integrais preservados. Não copia objetos, executáveis, fontes/sons/imagens privados.
