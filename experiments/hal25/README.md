# HAL25 — variantes mutuamente exclusivas, somente PoC

O pacote `mods/ritual-duelists` é o snapshot original do PR #13. Nenhuma correção foi instalada nele. Aplique **um** dos patches a uma cópia separada desse pacote, nunca A sobre B ou vice-versa. O ID/versão/manifest original são preservados e não permitem instalar as duas variantes juntas.

- `alternative-a.patch`: preserva uma seleção externa fisicamente válida antes da própria troca. Resolve somente a ordem externa, não demonstra autoridade do efeito nem cancelamento genérico.
- `alternative-b.patch` + `selection-rules-v1.h`: consumidor de interface opcional **proposta**, anterior à materialização. Ritual Effects PR #11 não implementa essa interface; B sem provider não corrige Queen Lock.
- `../../artifacts/hal25/`: objetos recompilados do SDK24 i386, para reproduzir testes. Não são uma distribuição instalável aprovada. Seus hashes/proveniência estão em `../../notes/hal-25/BUILD.json`.

A ponte de `tests/hal25/cases.inc` usa um helper privado do objeto exato de R somente para provar o ponto de validação e executar a regra genuína. Não copiar essa técnica para nenhum MOD de produção. O contrato real deve ser projetado, aprovado e oferecido pelo owner; não há autorização nesta entrega para modificar Ritual Effects.

O [relatório](../../notes/HAL25_QUEEN_LOCK_RD_POC.md) contém as falhas remanescentes, incluindo cancelamento bruto pós-planner. Não promover patches, objetos ou contrato à integração sem nova autorização.
