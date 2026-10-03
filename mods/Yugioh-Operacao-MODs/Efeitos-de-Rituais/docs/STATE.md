# Estado atual — auditoria de 2026-10-03

Versão: `0.74.0-test-performance`, mantida sem alterações funcionais.
Fonte efetivamente recebida: `zzz Efeitos Rituais.zip`, pasta `zzz Efeitos Rituais/`.
O texto de transferência menciona outro nome de ZIP. Os hashes de LEIA-ME.txt,
mod.c, mod.json e ritual-card-effects.o coincidem com os quatro hashes já
registrados no commit inicial; o manifesto identifica a mesma v0.74.
Não se afirma identidade byte a byte entre dois arquivos ZIP diferentes.

`dev/` contém os 112 arquivos originais; `versions/v0.74-test-performance/`
contém os mesmos 112 arquivos e um manifesto SHA-256 completo.
O objeto .o recebido é preservado nos dois lugares para reprodução do pacote
que o usuário testou. Builds de verificação usam saída separada.

A v0.73 é a base funcional declarada pelo pacote e pelo usuário; nenhum
snapshot dessa versão foi inventado. A v0.74 ainda tem bugs abertos.
As suítes antigas não vieram neste ZIP: seus resultados são relatos históricos,
não execuções reproduzidas nesta migração. Ver VALIDATION-v0.74.md.

O MOD MelhoriaDosRituais é uma dependência de integração para materiais da mão:
este MOD publica as receitas exatas, enquanto aquele cria proxies e consome
a mão. Preservar três registros distintos e pelo menos um tributo no campo.
Não integrar nem modificar o outro MOD durante esta migração.

Próxima prioridade: rastrear a resolução Black Hole e sua continuação com
sobreviventes, preservando a proteção de Millennium Shield. Depois investigar
lado/ownership na invocação. Não há correção desses bugs nesta migração.
