# Seguidor Ômega 2023 V2

O Seguidor Ômega é um projeto de robô seguidor de linha que utiliza um Arduino Nano para controlar seus movimentos. Este documento serve como um guia para entender a estrutura do código, suas funções e como o robô funciona.

## Índice

- [Repositório](#repositório)
- [Guia Básico](#guia-básico)
    - [Requisitos](#requisitos)
    - [Como Transmitir o Código](#como-transmitir-o-código)
    - [Iniciando o Seguidor](#iniciando-o-seguidor)
- [Estrutura do Código](#estrutura-do-código)
- [Diretivas de pré-processamento `#ifdef`](#diretivas-de-pré-processamento-ifdef)
- [Funções](#funções)
- [Refatoração](#refatoração)
- [Problemas principais](#problemas-principais)
- [Correções](#correções)
- [Adições](#adições)
- [Resultado](#resultado)

## Repositório

Este repositório contém todos os arquivos necessários para construir e controlar o Seguidor Ômega. Especificamente, ele contém:

- **Código em C++ para Arduino Nano** - Este é o código que controla o comportamento do robô. Temos várias versões disponíveis para diferentes anos: [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2022) | [2023](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023) | [2023-rsm](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023-rsm) | [2023-v2](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023-v2)
- **Código fonte e App do controle do seguidor desenvolvido por meio da plataforma [Kodular](https://creator.kodular.io/)** - Este é o código para o aplicativo móvel que pode ser usado para controlar o robô: [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/app/2022)
- **Código fonte e App do controle do seguidor desenvolvido em Flutter** - Este é outra versão do aplicativo móvel, desenvolvida usando Flutter: [2023](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/app/2023)

## Guia Básico

### Requisitos

Para começar a trabalhar com o Seguidor Ômega, você precisa do seguinte:

- Visual Studio Code - Este é o editor de código que usaremos. Certifique-se de instalar também a extensão PlatformioIO para facilitar a programação do Arduino.
- Dispositivo Android - Este será usado para controlar o robô. Certifique-se de ter o app do Seguidor de Linha instalado nele.

### Como Transmitir o Código

Aqui estão os passos para transmitir o código para o robô Seguidor Ômega:

1. Faça o download do diretório no formato .zip pelo GitHub.
2. Extraia os arquivos para uma pasta no seu computador.
3. Abra o diretório **seguidor-[version]** no Visual Studio Code usando a extensão PlatformioIO.
4. Com o projeto aberto, use a opção de upload do PlatformioIO para transmitir o código para o Arduino Nano.

> **Nota:** Se o upload não funcionar, tente desconectar o módulo Bluetooth do Seguidor e tente fazer o upload novamente. Isso pode resolver problemas de comunicação entre o computador e o Arduino.

### Iniciando o Seguidor

Após instalar o app em seu dispositivo Android, você está pronto para começar a controlar o seguidor! Aqui estão os passos para configurá-lo e iniciar a operação:

1. Transmita o código para o seguidor conforme os passos mencionados acima.
2. Ligue o seguidor e posicione-o na pista, certificando-se de que os sensores frontais estejam paralelos à linha que o robô deve seguir.
3. Faça o pareamento Bluetooth do seu dispositivo Android com o Seguidor. Você pode encontrar o Seguidor como "HC-05’ ou "MMD-7585". A senha para o pareamento é geralmente 1234 ou 0000.
4. Abra o app no seu dispositivo e pressione "Conectar".
5. Na lista que aparece, selecione o dispositivo pareado.
6. Pressione "Calibração" para calibrar os sensores e motores do robô.
7. Aguarde até que a calibração seja concluída.
8. Posicione o seguidor no início do percurso, certificando-se de que ele esteja voltado para a direção correta.
9. Agora é só tocar no botão "RUN" que o seguidor vai iniciar!

## Estrutura do Código

O código do Seguidor Ômega é estruturado em várias classes, cada uma representando um componente específico do robô:

1. `Motor`: Esta classe controla um único motor do robô. Ela tem funções para definir e obter a direção e a velocidade do motor.
2. `Sensor`: Esta classe lê e processa os dados de um único sensor. Ela tem funções para ler os dados do sensor, redefinir os valores lidos, e obter os valores máximo e mínimo lidos pelo sensor.
3. `SensoresFrontais`: Esta classe gerencia um conjunto de sensores frontais. Ela tem funções para definir os pinos do Arduino usados pelos sensores e para ler os dados de um sensor específico.
4. `ControladorPID`: Esta classe implementa um controlador PID (Proporcional-Integral-Derivativo) para ajustar a direção do robô com base na leitura do sensor. Ela tem funções para corrigir a trajetória do robô, definir as constantes do controlador PID e redefinir o controlador PID.
5. `SeguidorDeLinha`: Esta é a classe principal que usa todas as classes acima para fazer o robô seguir uma linha. Ela tem funções para fazer o robô seguir a linha, checar se ele chegou ao destino, calibrar os sensores e motores, e definir a direção e a velocidade do robô.

## Diretivas de pré-processamento `#ifdef`

As diretivas de pré-processamento são uma característica de C e C++ que permite a inclusão condicional de partes do código durante a compilação. Elas são úteis para controlar quais partes do código são compiladas em diferentes situações.

Por exemplo, o código do Seguidor Ômega utiliza a diretiva `#ifdef` para incluir ou excluir partes do código. Quando a condição `EXEMPLO` é definida com a diretiva `#define`, o código dentro do bloco `#ifdef ... #endif` será incluído na compilação:

```cpp
#define EXEMPLO

#ifdef EXEMPLO
    // Este código será incluído
#endif
```
Esta abordagem é útil para incluir partes do código apenas sob certas condições, como durante testes ou depuração. O código de teste ou depuração pode ser colocado dentro de um bloco `#ifdef`, e então a condição pode ser definida ou não definida para incluir ou excluir esse código durante a compilação.


## Funções

Aqui está uma visão geral das funções em cada classe:

### Motor:

- `set_direcao()`: Define a direção do motor. A direção pode ser para a frente ou para trás.
- `set_velocidade()`: Define a velocidade do motor. A velocidade é um valor entre 0 (parado) e 255 (velocidade máxima).
- `get_direcao()`: Retorna a direção atual do motor.
- `get_velocidade()`: Retorna a velocidade atual do motor.

### Sensor:

- `ler()`: Lê os dados do sensor. O sensor pode detectar a presença de uma linha preta em uma superfície branca.
- `reset()`: Redefine os valores lidos pelo sensor. Isso é útil para preparar o sensor para uma nova leitura.
- `get_max_leitura()`: Retorna o valor máximo lido pelo sensor. Isso pode ser útil para detectar se o sensor passou por uma linha preta.
- `get_min_leitura()`: Retorna o valor mínimo lido pelo sensor. Isso pode ser útil para detectar se o sensor está sobre uma superfície branca.

### SensoresFrontais:

- `set_pinos()`: Define os pinos do Arduino que estão conectados aos sensores. Cada sensor está conectado a um pino específico.
- `ler_sensor()`: Lê os dados de um sensor específico. Isso é útil para determinar a posição da linha em relação ao robô.

### ControladorPID:

- `corrigir_trajeto()`: Ajusta a direção do robô com base no erro atual. O erro é a diferença entre a posição atual da linha e a posição desejada.
- `set_const()`: Define as constantes do controlador PID. Estas constantes determinam quão rápido e quão fortemente o robô reage ao erro.
- `reset()`: Redefine o controlador PID. Isso é útil para preparar o controlador para um novo percurso.

### SeguidorDeLinha:

- `seguir_linha()`: Faz o robô seguir a linha. Esta função usa o controlador PID para ajustar a direção do robô de forma que siga a linha.
- `checar_chegada()`: Checa se o robô chegou ao destino. O destino é o final da linha.
- `calibracao()`: Calibra os sensores e motores do robô. Isso é útil para adaptar o robô a diferentes condições de luz e superfície.
- `set_direcao()`: Define a direção geral do robô. A direção pode ser para a frente ou para trás.
- `set_velocidade()`: Define a velocidade geral do robô. A velocidade é um valor entre 0 (parado) e 255 (velocidade máxima).

Para mais detalhes sobre cada função, consulte os comentários no código-fonte.

## Refatoração

A refatoração é o processo de modificar o código para torná-lo mais eficiente, legível ou manutenível, sem alterar sua funcionalidade. Durante o desenvolvimento do Seguidor Ômega, várias alterações foram feitas para melhorar o código:

- Foi criada uma classe própria para o controlador PID. Isso torna o código mais organizado e fácil de entender.
- O erro agora é calculado diretamente pela classe `SensoresFrontais`. Isso simplifica o código e reduz a chance de erros.
- Agora cada sensor tem seu próprio limite de luminosidade. Isso permite que cada sensor seja mais preciso, mesmo em condições de luz variáveis. No entanto, essa funcionalidade foi descontinuada na versão de 2022 e substituída pelo uso de erro analógico.
- Foram removidas várias funções e variáveis desnecessárias. Isso torna o código mais enxuto e fácil de entender.
- Foram adicionados códigos de teste que podem ser facilmente ativados ou desativados. Isso facilita a depuração e o teste do código.

## Problemas principais

Durante o desenvolvimento do Seguidor Ômega, encontramos vários problemas que precisavam ser resolvidos:

- P1: O robô tinha muitas oscilações em trajetos retos. Isso fazia com que ele se movesse de forma ineficiente e às vezes saísse da linha.
- P2: O código era lento. Isso limitava a velocidade máxima do robô e atrasava sua resposta aos desvios da linha.
- P3-1: O robô perdia a trajetória em curvas devido à alta velocidade. Isso fazia com que ele saísse da linha em curvas fechadas.
- P3-2: Em algumas curvas, o robô ia para o lado contrário ao que deveria. Isso fazia com que ele saísse da linha e às vezes se movesse na direção errada.

## Correções

Para resolver os problemas mencionados acima, implementamos várias correções:

- C1: Reduzimos o tamanho do vetor de erros. Isso acelera o cálculo do erro e melhora a velocidade do código.
- C2: Mudamos a metodologia de aplicação do PID. Isso melhora a forma como o robô ajusta sua direção, reduzindo as oscilações e melhorando a eficiência do movimento.
- C3: Começamos a usar um filtro na leitura de cada sensor. Isso reduz o ruído nas leituras do sensor e torna o robô mais preciso.
- C4: Adicionamos uma variável na classe `SensoresFrontais` que guarda o valor absoluto do erro caso seja maior que 0.3. O objetivo é usar esse erro como parâmetro quando o robô perder a linha. Isso é especialmente útil para resolver o problema P3-2.

## Adições

Além das correções mencionadas acima, também adicionamos algumas novas funcionalidades ao Seguidor Ômega:

- A1: Adicionamos a capacidade de obter valores analógicos por meio de interrupção. Isso permite que o robô leia os sensores mais rapidamente e responda mais prontamente às mudanças na linha.
- A2: Adicionamos o cálculo do erro analógico. Isso permite que o robô seja mais preciso na detecção da posição da linha. Como resultado, removemos o cálculo do erro digital.
- A3: Adicionamos um filtro de Kalman sobre o erro obtido pelos sensores frontais. Isso reduz o ruído no cálculo do erro e torna o robô mais preciso.

## Resultado

As correções e adições mencionadas acima levaram a várias melhorias no desempenho do Seguidor Ômega:

- C2, A1 e A2 foram as alterações mais importantes para resolver os problemas P1 e P2. Elas tornaram o robô muito mais eficiente em trajetos retos e permitiram que ele fizesse curvas com mais precisão.
- C3, C4 e A3 melhoraram a trajetória do robô e tentam resolver o problema P3-2. Embora este problema tenha sido atenuado, ele ainda ocorre ocasionalmente.
**Atualmente, o robô está em fase de testes para avaliar seu desempenho e identificar possíveis melhorias.**
