# Seguidor Ômega 2023

Código usado na competição RSM Challenge 2023

## Problemas iniciais:
- P1: Confundir linha lateral(chegada e curvas) com a pista
- P2: Erro não consegue chegar/manter em 0 nas retas

## Correções:
- C1: O erro do robô foi remodelado para funcionar caso a caso. Foco em P1 e P2.
- C2: Os erros agora são os ângulos em relação a pista. Foco P2.
- C3: O controlador PID, identificar o caso de erro em que o robô perdeu a linha, permite a rotação pra trás dos motores. Ou seja, o resultado é uma rotação mais rápida e com raio menor.

## Adições:
- A1: Códigos para seguir um mapeamento. Foco em P2.
- A2: Códigos incompletos para fazer o mapeamento de pistas

## Problemas gerados pelas adições:
- PA1: O código de seguir o mapeamento acaba fazendo o robo sair pra longe da pista, o que acaba resultando ou em parar porque o sensor de chegada lê a pista ou em ficar girando sem achar a pista.

## Resultado:
- P1 continua sem resolução, já que se os sensores frontais encontrarem uma linha lateral é bem provável que o robô perca a trajetória.
- P2 foi atenuado, porém ainda é um problema grave que deve ser corrigido.
- A1 e A2 devem ser aprimorados para que PA1 deixe de ocorrer.
