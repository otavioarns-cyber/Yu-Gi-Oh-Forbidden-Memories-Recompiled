# HAL25 — contrato real experimental, sem promoção automática

`R.patch` aplica somente `mod.c` e3 headers sobre R do PR #11. `RD.patch` faz o mesmo sobre RD do PR #13. São diffs de fontes para revisão; objetos/proveniência atuais estão nos respectivos diretórios `mods/`. Não aplicar sobre A/B do #24, outra versão ou uma instalação já modificada.

R fornece a regra real através do rendezvous público `ritual-duelists:mandatory-hand-selection-v1`. Nenhuma ponte privada de Queen está nesta implementação. Os dois projetos continuam opcionais/independentes; não renomear manifests nem misturar objetos de revisões.

`loader-expectation.patch` é somente de teste: reconhece9 hooks do RD em vez de8, por causa da fronteira local de cancelamento. O runner `--qualified-loader` aplica esse delta na cópia temporária; todas as outras assertions continuam iguais.

Leia o [contrato](../../notes/hal-25-contract/CONTRATO.md), [relatório](../../notes/HAL25_SELECTION_CONTRACT_REAL_POC.md) e [reprodução](../../tests/hal25-contract/README.md). A quarentena de identidade física e a fronteira abort/retry precisam de revisão; não promover sem autorização posterior. Testes manuais Windows ficam para a última fase, conforme orientação do usuário.
