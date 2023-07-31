# Seguidor Ômega 2023 V2

## Índice
1. [Introdução](#introdução)
2. [Estrutura do Código](#estrutura-do-código)
3. [Diretivas de pré-processamento](#diretivas-de-pré-processamento)
4. [Funções](#funções)
5. [Refatoração](#refatoração)
6. [Problemas principais](#problemas-principais)
7. [Correções](#correções)
8. [Adições](#adições)
9. [Resultado](#resultado)

## Introdução

Este projeto é uma implementação de um robô seguidor de linha, que usa uma variedade de sensores e motores para seguir uma linha predefinida. Em desenvolvimento...

## Estrutura do Código

O projeto é composto de várias classes, cada uma representando um componente do robô:

1. `Motor`: Controla um único motor do robô.
2. `Sensor`: Lê e processa dados de um único sensor.
3. `SensoresFrontais`: Gerencia um conjunto de sensores frontais.
4. `ControladorPID`: Implementa um controlador PID para ajustar a direção do robô com base na leitura do sensor.
5. `SeguidorDeLinha`: A classe principal que usa as classes acima para fazer o robô seguir uma linha.

## Diretivas de pré-processamento

No código, você verá várias seções envolvidas por diretivas `#ifdef ... #endif`. Essas são diretivas de pré-processamento em C++, usadas para incluir ou excluir partes do código com base na definição de certas condições.

Por exemplo, se tivermos:

```cpp
#ifdef EXEMPLO
    // Este código será incluído
#endif
```

O código dentro do bloco `#ifdef ... #endif` será incluído na compilação, porque a condição `EXEMPLO` foi definida com a diretiva `#define`.

Isso é útil quando você quer ter partes do código que são incluídas apenas em certas condições, como durante o teste ou a depuração.

## Funções

Aqui está uma visão geral das funções em cada classe:

### Motor:

- `set_direcao()`: Define a direção do motor.
- `set_velocidade()`: Define a velocidade do motor.
- `get_direcao()`: Retorna a direção atual do motor.
- `get_velocidade()`: Retorna a velocidade atual do motor.

### Sensor:

- `ler()`: Lê os dados do sensor.
- `reset()`: Redefine os valores lidos do sensor.
- `get_max_leitura()`: Retorna o valor máximo lido pelo sensor.
- `get_min_leitura()`: Retorna o valor mínimo lido pelo sensor.

### SensoresFrontais:

- `set_pinos()`: Define os pinos do Arduino usados pelos sensores.
- `ler_sensor()`: Lê os dados de um sensor específico.

### ControladorPID:

- `corrigir_trajeto()`: Ajusta a direção do robô com base no erro atual.
- `set_const()`: Define as constantes do controlador PID.
- `reset()`: Redefine o controlador PID.

### SeguidorDeLinha:

- `seguir_linha()`: Faz o robô seguir a linha.
- `checar_chegada()`: Checa se o robô chegou ao destino.
- `calibracao()`: Calibra os sensores e motores do robô.
- `set_direcao()`: Define a direção do robô.
- `set_velocidade()`: Define a velocidade do robô.

Cada função tem um propósito específico e desempenha um papel importante na operação geral do robô. Para mais detalhes sobre cada função, consulte os comentários no código-fonte.

## Refatoração

- Foi criada uma classe própria para o controlador PID.
- O erro agora é calculado diretamente pela classe sensores_frontais.
- Agora cada sensor tem seu limite de luminosidade (voltando alguns passos >> 2022) -> Não utilizado, já que agora usa erro analógico.
- Retirada de várias funções e variáveis desnecessárias.
- Adição de códigos de teste que podem ser desativados facilmente.

## Problemas principais

- P1: Muitas oscilações em trajetos retilineos.
- P2: Código lento.
- P3-1: Perda de trajetória em curvas: por alta velocidade.
- P3-2: Perda de trajetória em curvas: o robô vai pro lado contrário ao que deveria.

## Correções

- C1: O tamanho do vetor dos erros foi reduzido.
- C2: Mudanças na metodologia de aplicação do PID.
- C3: Utilizar filtro na leitura de cada sensor.
- C4: Adicionada uma variável em sensores_frontais que guarda o valor absoluto do erro caso seja > 0.3, o objetivo e utilizar esse erro para servir de parâmetro quando o robô perder a linha. Foco em P3-2.

## Adições

- A1: Obtenção dos valores analógicos por meio de interrupção.
- A2: Adição do calculo do erro analógico (por consequência dos resultados, o erro digital foi retirado).
- A3: Adição do filtro de Kalman sobre o erro obtido de sensores_frontais.

## Resultado

- C2, A1 e A2 são os pontos mais importantes da correção de P1 e P2, estes fizeram o robô se tornar muito mais eficiente em retas e continuar fazendo curvas bem.
- C3, C4 e A3 melhoraram a trajetória e tentam resolver P3-2, que foi atenuado, no entanto algumas vezes ainda ocorre.
- Em testes...
