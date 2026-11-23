# Time Wizard × RD, bônus e lifecycle

Continuação do PR32, não uma nova migração. Requer o build privado PR31 cujo objeto final tem SHA6470024d…d5d7, SDK24/i386, pyelftools, toolchain/runner ELF32 e as dependências privadas já descritas em `tests/hal9-support/README.md`. Não disponibiliza imagem do jogo ou Dual. `build.py` altera só a cópia do adapter disponível, reaproveita e verifica o link do núcleo exato e produz objeto/BUILD.json fora do repositório.

```sh
python tests/hal9-compat/build.py --reference-build "$HAL9_REFERENCE_BUILD" --sdk "$MEMORIES_SDK" --output "$HAL9_WORK/candidate"
export HAL9_BONUS_RECEIPT="$HAL9_WORK/candidate/BUILD.json"
python tests/hal20-field-state/run.py --inputs notes/hal-9/INPUTS.json --fence --timewizard-object "$HAL9_WORK/candidate/package" --tw-case 27 --tw-case 40 --tw-case 41 --tw-case 42 --tw-case 43 --tw-case 44 --tw-case 45 --tw-case 46 --tw-case 47 --trace-returns --output "$HAL9_WORK/final-qualification"
python tests/hal9-timewizard-contracts/regressions.py --package "$HAL9_WORK/candidate/package" --sdk "$MEMORIES_SDK" --output "$HAL9_WORK/native"
```

Apontar M/H/F para esta árvore e o overlay nativo para o binding COMMON do PR32. O recibo opt-in só muda o pin de imports numa **cópia privada do core usada pelo harness**. Nenhum arquivo `src` é alterado; o guard original continua recusando TW. Não instalar o objeto em um core stock nem trocar permanentemente o pin de produção. O recibo aceita apenas o baseline e o patch exatos desta experiência. Não é uma whitelist de gameplay.

Para controle PR32, remover `HAL9_BONUS_RECEIPT`, usar o pacote6470024d e casos24/27. A nova asserção da máscara identifica FAIL histórico no27 com Dual ON. Preservar esses FAIL. O controle adicional `snapshot-only` preserva a falha com duas cópias antes da correção da zona; não usar como candidata final.

|Caso|Cobertura adicional|
|---|---|
|27|Support→Monster real, snapshot/ledger/quota, mesma fixture do PR32|
|40|Reset quieto após resolução, nova fixture de duelo/controller real|
|41|Cancel do proxy real, Reset em voo e callback antigo|
|42|Reset em voo, 240 ticks de controllers sem fabricar conclusão|
|43|Reset em voo, nova fixture de duelo, nova tentativa e conclusão antiga|
|44|Pending real, BLOCKED, recuperação pública por sequence, novo veto limitado|
|45|Queen mandatory real com ameaças favoráveis a TW; recusa da alteração posterior|
|46|Duas cópias ID16, deck40 e41; Support→TW Monster, ledger3 e sem reroute para Time Magic|
|47|Quadrado com cópias distintas, tentativa de alcançar bônus humano; NOT_REACHED se o frame real não o concede|

Casos8/24–36/39 do PR32 são regressões. Caso34 mantém FAIL da hipótese de reset terminal. Não zerar estado nem fabricar ACK/plano/pending. `support.inc` acrescenta observação de snapshots e callbacks; só `recover()` público cancela a transação no caso44. Tabuleiro/catálogo, clock, renderer e transporte wheel são modelos declarados, não gameplay Windows. O wheel devolve o setor selecionado pelo RNG original; não escreve o resultado do jogo.

O45 demonstra Queen intacta pela **recusa segura**, não um duelo completado. O46 testa os wrappers reais de placement, mas o corpo nativo de animação Monster permanece modelado. O43 usa uma nova fixture de duelo e IDs de wheel monotônicos: não prova que todos os cancelamentos/reinícios da UI tenham essa propriedade. O42 não é um load completo: apenas testa o callback fora de seu fluxo normal documentado.

ABI1 permanece congelada. Não há implementação de handoff entre RD e TW. Ver a proposta no relatório antes de ampliar o contrato ou escolher prioridades.
