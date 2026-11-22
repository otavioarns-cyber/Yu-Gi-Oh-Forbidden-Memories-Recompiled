# Cliente TW experimental — reprodução privada

Base PR34/7855862525222ae6dbecfa02ae607352a41cc675. Não instalar em core stock nem admitir no guard. SDK24/i386 e bridge privada qualificada dos PR31–34 são necessários. Núcleo e Dual não são distribuídos.

1. Construir a cópia do adapter PR33 (SHA46f63050…9093) com `build.py --reference-build <PR33-build> --sdk <SDK24> --output <diretório novo>`. Os seis link units privados continuam fixos e o relink de referência deve ser byte-idêntico antes de aceitar o candidato.
2. Usar o RD companion endurecido do PR34 (SHAd044cfef…a97) e seu BUILD.json; não alterar ABI1 nem os MODs canônicos.
3. Configurar o ambiente i386/QEMU dos PR33/34; exportar `HAL9_CLIENT_RECEIPT=<build>/BUILD.json`, `HAL9_HANDOFF_BUILD=<RD-build>`. `run.py` aceita exclusivamente o objeto e fontes do recibo numa cópia privada do pin para testes; não modifica o pin de produção ou o guard.
4. Executar `python tests/hal20-field-state/run.py --inputs notes/hal-9/INPUTS.json --fence --trace-returns --timewizard-object <build>/package --tw-case N --output <logs>`. Cada caso gera oito perfis H/D OFF/ON × ranks R before/after, que não necessariamente representam oito ordens efetivas diferentes.
5. Ausência: retirar `HAL9_HANDOFF_BUILD`, usar RD canônico e caso122. ABI: compilar `tests/hal9-handoff/abi.c -m32 -Imods/ritual-duelists` e executar com o runner i386.
6. Após os quatro grupos, executar `verify_logs.py --logs <logs-root>`, depois `collect.py --work <work-root>`. Logs originais locais ficam preservados; publicação comprime apenas linhas exatamente repetidas e registra seus hashes/contagens.

Políticas privadas de teste:101/103 KEEP_RD;102/104–119/121–125 ALLOW_TW;100 e regressões anteriores OBSERVE. Setting não consta do manifest nem é promovido.122 usa companion ausente.124 altera preferência pelo manager mas mantém TW aplicado enquanto reinício está pendente.123/125 OFF efetivo são falhas de timing instrumentadas; o harness para na primeira transação invalidada, antes de outro planejamento.

Casos100–104: OBSERVE, KEEP, ALLOW e Queen.105–111/114–117: tabela ABI/callback, geração, invalidação, ACK, reentrância, desconexão, nonce, identidade, cancelamento e oferta inválida.112: cursor alterado após ACK.113: handoff + bônus real com duas TW.118: callbacks antigos após reset.119: ação anterior vetada, novo planejamento com novo token/ACK.120: Monster real → bônus Time Magic (RD frio OFF).121: fault de tipo lógico depois do handoff do bônus; gate Dual real deve recusar Support já gasto.123/125: OFF efetivo TW/RD durante negociação.124: OFF salvo com reinício pendente.

O driver antigo50–84 permanece separado e não negocia nos novos casos. As regressões preservam o FAIL histórico34 da roleta; recusas não são jogadas concluídas. HOLD/combos/rituais parcialmente consumidos continuam fora do contrato e não são liberados por esta cliente.
