# HAL-9 — recuperação cooperativa / hardening privado

Base PR35, commit `e5c97876009e0e57127166039821426a617729db`. Os experimentos e controles anteriores ficam preservados. Este diretório não distribui core, objeto TW ou Dual privado.

## Entradas e build

- TW privado PR35: SHA `d70a25110d5bff51a90cd2634ec7c9182536ad0c94e209b9f4dadbd6803361a7`, pacote e adapter.o do build anterior.
- RD companion PR34: SHA `d044cfef6019c8a97c869bade40dfe91ae081fda797e62e6f2f53fbe2a7b2a97` e BUILD.json; ABI1/companion não mudam.
- Dual local SHA `77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378`, sem redistribuição.
- SDK24/i386 e seis unidades legadas PR31 já fixadas no builder. O relink de referência deve reproduzir o objeto PR35 antes de aceitar a nova candidata.

```sh
python tests/hal9-recovery/build.py --reference-build <build-PR35> --sdk <SDK24> --output <novo-build>
export HAL9_RECOVERY_RECEIPT=<novo-build>/BUILD.json
export HAL9_HANDOFF_BUILD=<build-RD-companion>
python tests/hal20-field-state/run.py --inputs notes/hal-9/INPUTS.json --fence --trace-returns --timewizard-object <novo-build>/package --tw-case 200 --output <logs>
```

Usar o ambiente i386/QEMU e a overlay nativa dos PR33/35; não executar um objeto stock sem a bridge privada qualificada. Cada caso roda H/D OFF/ON × duas solicitações de rank R (dependências podem normalizar a ordem efetiva). A permissão de hash existe somente na overlay privada do harness, nunca no pin/guard de produção.

Grupos finais: lifecycle200–212; fim/início214; projeções220–227,230 e231; regressões e matrizes PR35 preservadas. Ausência de companion: retirar HAL9_HANDOFF_BUILD e caso122. ABI: `tests/hal9-handoff/abi.c` i386. Após os grupos, executar `verify_logs.py --logs <logs-root>` e `collect.py --work <work-root>`.

## Casos e limites

200 ACK ausente e desconexão sem recuperação automática;201 nonce adulterado;202 cursor depois do ACK/monitor RD;203 desconexão já reconhecida pelo produtor;204 RD OFF efetivo;205 TW OFF efetivo;206 OBSERVE/RD BLOCKED;207 preferência OFF pendente + monitor;208 ação consumida em apresentação;209 Queen KEEP;210 cancelamento pré-confirmação;211 InitScene reentrante (instrumentação);212 tabela descoberta trocada antes do ACK;214 fim/início pelos cases1/2 do Main_RunDuel real.

A segunda fixture é um novo tabuleiro, não restauração do anterior. Não há escrita em pending/BSS RD. O `--wrap=Mods_Reset` somente conta/delega chamadas reais; a segunda inicialização deve ter delta0. InitScene tem corpo de IO/renderer modelado, com callbacks de MODs e Main_RunDuel realmente executados. O flag de término do caso214 é entrada explícita de fixture; não certifica o fluxo UI Windows.

220 retorno antecipado do predecessor de swap;221 outro writer de tipo lógico;222 writer independente de stats;223 writer de tipo após snapshot;224 retorno sem produzir snapshot;225 reentrância;226 troca lógica com mesmo ID e byte opaco diferente;227 escrita em outra carta;230 seleção alterada após delegação;231 OFF efetivo forçado após o swap (continuação física NOT_REACHED). Esses probes trocam apenas slots de predecessor no adapter próprio, em teste. Não são wrappers admitidos. Coleta física não executada neles é NOT_REACHED; conflito realmente latched pelo adapter é REFUSED. PASS da restauração nunca é PASS de gameplay.

OBSERVE continua padrão. KEEP_RD somente101/103/209, ALLOW_TW apenas os casos explícitos de protocolo/lifecycle. Não existe setting público novo. Usar `HfStatus` como licença, alterar nonce/epoch para fazer replay, chamar recover automaticamente ou restaurar cartas por ID invalida o teste.

Os oito FAIL do reset34 da roleta continuam separados. Logs comprimem só linhas exatamente repetidas; hashes originais ficam no manifesto. Fontes exatas das versões de harness e controles próprios são preservadas quando diferem da entrega atual.
