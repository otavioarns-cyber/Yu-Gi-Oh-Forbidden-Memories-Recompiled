# Auditoria e migração da v0.74

## Escopo e referências iniciais

Somente `otavioarns-cyber/Yu-Gi-Oh-Forbidden-Memories-Recompiled`, branch
`mod-ritual-effects`, diretório `mods/Yugioh-Operacao-MODs/Efeitos-de-Rituais/`.

| Branch | SHA inicial |
| --- | --- |
| master | 4f58718bb89751fe8070b6093a659cb55908aa9d |
| mod-field-effects | cf60db86d37685d66c5a093e8650859bb247d706 |
| mod-ritual-effects | 4e933b5ab59752135f205681d896b4dd30f39b2b |

## Conteúdo e diferenças

112 arquivos, 1.441.081 bytes descompactados: 2 C, 10 headers, 1 JSON,
1 objeto compilado, 53 PNGs e 45 TXT (42 textos de cartas, 2 coin.txt,
1 LEIA-ME). Inventário por caminho, tamanho e SHA-256 em INVENTORY-v0.74.json.

Antes: dev/mod.c era um comentário de 254 bytes; dev/mod.json tinha 380 bytes
sem settings/textos; dev/LEIA-ME.txt era uma versão resumida de 2.035 bytes.
Esses três foram substituídos pelos arquivos reais do ZIP. Foram adicionados
109 arquivos ausentes em dev. O snapshot tinha somente LEIA-ME resumido e
quatro hashes: foram adicionados 111 arquivos, restaurado LEIA-ME e ampliado
source-package.sha256 para todos os 112 arquivos. Nenhum arquivo recebido
foi descartado. O histórico Git mantém os documentos anteriores.

Decisão explícita sobre binários: preservar o .o distribuído, as 53 imagens e
os auxiliares necessários à instalação, tanto em dev quanto no snapshot.
Não inserir objetos temporários de compilação nem arquivos do jogo/ROM.

## Estrutura resultante

- dev/: 15 arquivos na raiz (2 C, 10 H, manifesto, LEIA-ME e .o),
  assets/ com 55 arquivos, text/ com 42 arquivos.
- versions/v0.74-test-performance/: mesmos 112 arquivos + source-package.sha256.
- docs/: documentação técnica anterior, estado corrigido, inventário completo,
  relatório desta migração, arquitetura e instruções originais de continuidade.

## Validação e limites

A igualdade byte a byte com o ZIP deve ser verificada para todos os arquivos
nos dois destinos antes do commit. O commit usa como pai o SHA inicial do MOD,
não realiza merge e não escreve em nenhuma outra branch. A comparação de
SHAs remotos após publicação deve confirmar o isolamento. O SHA do próprio
commit e a verificação posterior são informados no relatório da conversa
(um commit não pode incluir o seu próprio hash).

Nenhum bug funcional é declarado resolvido aqui. Build e hashes não substituem
teste no jogo. Referência oficial: release v0.2.0, alvo
0185f58df7928cc6338ceb221d169e2b42feca6d, consultada somente para leitura.
