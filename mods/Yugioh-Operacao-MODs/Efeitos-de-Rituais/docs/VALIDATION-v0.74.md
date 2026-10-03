# Resultados de validacao da v0.74

Fonte: LEIA-ME incluido no pacote v0.74 fornecido pelo usuario.

- Build concluido com SDK v0.2.0 e importacoes verificadas pelo build oficial.
- 24.098 verificacoes aprovadas com MelhoriaDosRituais v1.0 Final inalterado.
- 24.074 verificacoes aprovadas com MelhoriaDosRituais v1.1 inalterado.
- 200 conjuntos de candidatos produziram a mesma selecao da v0.73; no pior caso medido, chamadas ao predicado cairam de 1.159 para ate 30.
- 780 comparacoes renderizadas da moeda sem diferenca de pixels, cobrindo 52 quadros, escalas 1/2/4 e cinco alturas.
- Chamadas de desenho da moeda: escala 1 = 1.052.965 -> 1.052.965; escala 2 = 2.737.969 -> 1.825.315; escala 4 = 5.475.598 -> 1.825.315.
- 1.000 repeticoes de cada caminho de miniaturas inalteradas nao fizeram novos uploads; alterar um retangulo causou somente seu reenvio.

Esses testes sao locais/simulados e nao substituem validacao dentro do jogo com os demais MODs e HD ativos.

## Auditoria atual

Os números acima foram transcritos do LEIA-ME original; as suítes não estão
no ZIP e não foram reexecutadas nesta migração. A conferência atual compara
os 112 arquivos recebidos com dev e snapshot, incluindo binário e imagens.

Build reexecutado em 2026-10-03 com o build_mod.py do SDK oficial v0.2.0
fornecido no ZIP Windows: sucesso, 2 fontes C, importações aceitas pela lista
exports.txt do SDK. Saída em diretório separado; objeto original preservado.
Não foi executado duelo nesta auditoria nem comprovada identidade binária
entre compiladores. Os 224 hashes dos dois destinos conferem com o pacote.
