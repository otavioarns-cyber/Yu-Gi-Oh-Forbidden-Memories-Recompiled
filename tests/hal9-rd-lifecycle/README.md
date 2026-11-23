# HAL-9 — fronteira de novo duelo do owner RD

Continuação experimental do PR36, commit `c8a5605852707589d3d166ad153a88fdd0017b30`.
Nenhum arquivo canônico é substituído. Não distribuir os objetos privados de RD/TW/Dual.

## Entradas e reprodução

- RD companion PR34: objeto `d044cfef6019c8a97c869bade40dfe91ae081fda797e62e6f2f53fbe2a7b2a97` e fontes correspondentes.
- TW03cc do PR36 e seu BUILD.json, sem recompilar ou modificar o adapter.
- SDK24, toolchain i386 e overlay nativa documentados nos PRs33–36.
- Dual local `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`, somente leitura.

```sh
python tests/hal9-rd-lifecycle/build.py --reference <RD-PR34> --sdk <SDK24> --output <build-novo>
export HAL9_RD_LIFECYCLE_BUILD=<build-novo>
export HAL9_RECOVERY_RECEIPT=<TW-PR36>/BUILD.json
python tests/hal20-field-state/run.py --inputs notes/hal-9/INPUTS.json --fence --trace-returns --timewizard-object <TW-PR36>/package --tw-case 301 --output <logs>
```

O builder copia os fontes autorizados, inclui o fonte próprio em `experiments/hal9-rd-lifecycle`, troca somente os três pontos de registro dos callbacks RD e registra Main/Init. `owner.patch` é o delta incremental do `mod.c`, não um patch para o objeto. A compilação integral de16 fontes usa o SDK oficial, alvo i386. Paths de debug são normalizados; repetir em outro diretório deve conservar o objeto. O formato de MOD Windows é ELF32 relocatable, não PE32 executável. Executar os testes aqui com ELF32/QEMU não valida Windows.

## Matriz nova

Cada modo roda HARD/Dual OFF/ON × duas solicitações de ordem R (dependências podem normalizá-las). Registrar ordem efetiva e hashes; nomes não autorizam wrappers.

|Modo|Fronteira exercitada|
|---|---|
|300|OBSERVE cria BLOCKED real; novo duelo invalida a ação antiga, mas o conflito funcional novo continua recusado|
|301|HANDOFF confirmado e cursor alterado antes da coleta|
|302|QUARANTINE por cursor tardio, TW ativo|
|303|QUARANTINE por referência física tardia, RD sem TW|
|304|Pending ritual real ainda não consumido; observador pausa o retorno do planner|
|305–306|Ação consumida e reapresentação de ticket/callback antigo|
|307|Tabela descoberta substituída antes do ACK; composição recusada|
|308|ACK de fechamento ausente; composição recusada|
|309|Queen mandatória nos dois duelos, KEEP_RD explícito|
|310|RD sem TW: divergências reais de snapshot até BLOCKED|
|311|TW sem RD|
|312|Main solicitado durante callback authorize real; recusa reentrante e tentativa posterior em novo duelo|
|313|Planejamento durante InitScene; QUARANTINE, não recuperação|
|314|Desconexão cooperativa reconhecida; nova conexão após novo duelo|
|315|Exaustão de geração/serial; contadores não voltam|
|316|Exaustão de connection/interrupt/sequence|
|317|Exaustão de nonce/client epoch do companion; limpeza local não é recuperação completa|
|318|Exaustão do epoch privado de duelo|
|319|Novo duelo seguido de bônus Support→Monster com duas cópias físicas TW|
|321|BLOCKED OBSERVE; mudar para ALLOW de laboratório continua bloqueado no mesmo duelo; novo duelo permite transação nova|

`300/304/310` incluem InitScene isolado com flags semelhantes às de entrada, mudança de cena, Settings e shuffle do jogador: nenhum desses eventos invalida o owner. Todos os modos executam os cases1/2 de saída do Main real e exigem que fim sozinho não recupere RD/TW. O flag de fim é entrada explícita da fixture. A segunda entrada executa Main real/callbacks reais, com delta Mods_Reset=0.

O corpo de InitScene com IO/renderer é modelado. A fixture despacha os callbacks reais de Populate representando a fase de entrada oficial, cujo corpo nativo de leitura de deck continua modelado. Isso é necessário também para o lifecycle próprio de Ritual Effects/Queen, e não consiste em escrita manual em seus estados. Novo tabuleiro é uma fixture nova, nunca rollback ou restauração de cartas antigas por ID.

Os faults são declarados nos logs. Não se injetam pending, planos ou quotas. O teste314 remove o fault de ACK ausente somente após uma desconexão real reconhecida: a próxima conexão tem callbacks normais. Manter o fault instalado deve continuar recusando, como308. Exaustão é instrumentação de metadados de alocadores, não fluxo normal de UI.

## Regressões e evidência

Reexecutar os modos listados nos grupos primary/boundaries/regressions/projections/off-projection/end-boundary do PR36, com a candidata RD. Caso122 deve ser executado sem ambos os overlays RD para testar companion ausente. A coleção sem TW e a política B podem ser verificadas com os procedimentos herdados. Não reinterpretar Mods_Reset como cancelamento: modo34 preserva seus8 FAIL históricos.

`verify.py --work <work>` verifica os oráculos e os tokens/resultados dos logs. `collect.py --work <work>` publica somente evidência textual e hashes. O grupo owner original contém o controle314 de fault persistente; a entrega usa a repetição focal314 do grupo extra, sem apagar o controle. Não considerar REFUSED/NOT_REACHED como jogada concluída.

Manter OBSERVE padrão, KEEP/ALLOW só na configuração de teste, guard sem TW, política B intacta e A desligada. Não executar objetos privados stock sem a bridge API24/i386 já qualificada.
