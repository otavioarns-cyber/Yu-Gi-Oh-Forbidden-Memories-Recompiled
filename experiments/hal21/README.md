# Variantes alternativas — somente HAL-21 experimental

Aplicar **um** patch em cópia do pacote do PR #12, commit `907ed98cfb763a96f5aa1ad6369f809cceca5f1c`:

```sh
cd /copia/fusion-auxiliary-balance
patch -p1 --batch -i /checkout/experiments/hal21/revoke.patch
MEMORIES_MOD_CC=/caminho/clang python /caminho/sdk/tools/build_mod.py . --target i386
```

- `hide.patch`: respostas neutras OFF, leases retidas; ON faz sweep de instâncias e revalida elegibilidade. Tem dois riscos reproduzidos no harness; não recomendado para promoção nesta forma.
- `revoke.patch`: mesmas respostas neutras OFF; `applied(0)` encerra somente as leases, preservando contador serial. ON não recria grants anteriores. É a recomendação para revisão, sem autorização definitiva nesta entrega.

Ambos alteram experimentalmente **mod.c, fusionist_runtime.h, owner_f.h** do Fusion; nenhum desses arquivos é promovido para `mods/` nesta branch. Os patches preservam ABI, manifests, configurações, receitas e demais fontes/assets. Não alteram nenhum consumidor, core, AI, Dual Play, nem incorporam o PR #20.

Os objetos correspondentes estão fora da árvore instalável, em `artifacts/hal21/hide/` e `artifacts/hal21/revoke/`. São candidatos mutuamente exclusivos do ID original `fusion-auxiliary-balance`, não novos MODs independentes. Para teste local, usar o pacote completo e autorizado do PR #12 (mesmo manifest/assets), substituir SOMENTE o objeto na cópia descartável e registrar qual variante está instalada. Nunca instalar ambas ou sobrepor versões. Nenhuma variante foi escolhida como default.

Créditos/permissões originais em [NOTICES-HAL20.md](NOTICES-HAL20.md). Permissão de publicação dos fontes próprios e coin-lib foi registrada na HAL-20; esta entrega não amplia os termos nem publica assets novos. Hashes, limites e proposta: [relatório](../../notes/HAL21_FUSION_OFF_PROVIDERS_POC.md).
