# Bugs conhecidos / investigacao ativa

## 1. Millennium Shield ocasionalmente vai para o lado adversario
Relato de teste: ao realizar o Ritual do Millennium Shield, em uma circunstancia nao reproduzida com exatidao, o proprio Millennium Shield apareceu no campo do adversario e ficou totalmente funcional. Em outro duelo, o mesmo Ritual foi invocado normalmente no campo do jogador. Tratar como bug circunstancial de ownership/side durante a resolucao do Ritual; ainda precisa de reproducao deterministica.

## 2. Soft lock apos Black Hole do Magician of Black Chaos
Relato de teste: ao usar o efeito de buraco negro do Magician of Black Chaos, os Millennium Shields sobreviveram conforme programado, mas o duelo entrou em soft lock. Musica e animacoes 3D continuaram; a interface/cartas da mao deixaram de progredir. Investigar transicao/limpeza de estado apos destruicao em massa com sobreviventes protegidos.

## 3. Confirmacao em jogo das otimizacoes v0.74
A v0.74 reduziu trabalho redundante em miniaturas, moeda, settings e matching de tributos em testes locais. Ainda e necessario confirmar em jogo que nao ha regressao de HD/texturas e que o FPS/estabilidade melhoraram ou permaneceram equivalentes.

Prioridade tecnica: manter o codigo compacto e eficiente sem quebrar efeitos ou mecanicas existentes.
