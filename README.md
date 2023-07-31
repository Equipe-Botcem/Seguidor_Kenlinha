# Seguidor Ômega
Neste repositório estão presentes todos os códigos utilizados no desenvolvimento do Seguidor de Linha Ômega da equipe BOTCEM em 2022 e 2023. Especificamente contém:
 - Código em C++ para Arduino Nano >> [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2022) | [2023](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023) | [2023-rsm](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023-rsm) | [2023-v2](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023-v2)
- Código fonte e App do controle do seguidor desenvolvido por meio da plataforma [Kodular](https://creator.kodular.io/) >> [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/app/2022)
- Código fonte e App do controle do seguidor desenvolvido em Flutter >> [2023](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/app/2023)

## Guia básico

### Requisitos
- Visual Studio Code com a extensão PlatformioIO
- Dispositivo Android com o app do Seguidor de Linha instalado

### Como transmitir o código
Para transmitir o código para o seguidor basta seguir o passo-a-passo:
- Fazer o download do diretório no formato .zip pelo GitHub
- Extrair os arquivos para uma pasta
- Abrir o diretório **seguidor-[version]** por meio da extensão PlatformioIO no Visual Studio Code
- Fazer o upload a partir do PlatformioIO para o Arduino Nano

Obs.: Caso o upload não funcione, desconecte o módulo bluetooth do Seguidor e tente fazer o upload novamente.

### Iniciando o seguidor
Após instalar o app em um dispositivo, já é possível controlar o seguidor! 😃

Para configurar o seguidor é necessário seguir os seguintes passos:
- Transmitir o código para o seguidor conforme os passos anteriores
- Ligar o seguidor e colocá-lo na pista de forma que os seus sensores frontais fiquem paralelos e após a linha que deve ser seguida.
- Fazer o pareamento bluetooth com “HC-05’ ou "MMD-7585" (senha de pareamento: 1234 ou 0000)
- Abrir o app e pressionar “Conectar”
- Selecionar o dispositivo pareado na lista
- Pressionar “Calibração”
- Aguardar até que no robô termine a calibração
- Colocar o seguidor no início do percurso com a frente no sentido que ele deve seguir
- Pronto! Agora é só tocar no botão “RUN” que o seguidor vai iniciar


# Seguidor Ômega

Neste repositório estão presentes todos os códigos utilizados no desenvolvimento do Seguidor de Linha Ômega da equipe BOTCEM em 2022 e 2023. 

## Índice

- [Repositório](#repositório)
- [Guia Básico](#guia-básico)
    - [Requisitos](#requisitos)
    - [Como Transmitir o Código](#como-transmitir-o-código)
    - [Iniciando o Seguidor](#iniciando-o-seguidor)
- [Análise Detalhada do Código](#análise-detalhada-do-código)
- [Compilação e Execução](#compilação-e-execução)
- [Conclusão](#conclusão)

## Repositório

Especificamente contém:

- **Código em C++ para Arduino Nano** >> [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2022) | [2023](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023) | [2023-rsm](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023-rsm) | [2023-v2](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2023-v2)
- **Código fonte e App do controle do seguidor desenvolvido por meio da plataforma [Kodular](https://creator.kodular.io/)** >> [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/app/2022)
- **Código fonte e App do controle do seguidor desenvolvido em Flutter** >> [2023](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/app/2023)

## Guia Básico

### Requisitos

- Visual Studio Code com a extensão PlatformioIO
- Dispositivo Android com o app do Seguidor de Linha instalado

### Como Transmitir o Código

Para transmitir o código para o seguidor basta seguir o passo-a-passo:
- Fazer o download do diretório no formato .zip pelo GitHub
- Extrair os arquivos para uma pasta
- Abrir o diretório **seguidor-[version]** por meio da extensão PlatformioIO no Visual Studio Code
- Fazer o upload a partir do PlatformioIO para o Arduino Nano

> **Obs.:** Caso o upload não funcione, desconecte o módulo bluetooth do Seguidor e tente fazer o upload novamente.

### Iniciando o Seguidor

Após instalar o app em um dispositivo, já é possível controlar o seguidor! 😃

Para configurar o seguidor é necessário seguir os seguintes passos:
- Transmitir o código para o seguidor conforme os passos anteriores
- Ligar o seguidor e colocá-lo na pista de forma que os seus sensores frontais fiquem paralelos e após a linha que deve ser seguida.
- Fazer o pareamento bluetooth com “HC-05’ ou "MMD-7585" (senha de pareamento: 1234 ou 0000)
- Abrir o app e pressionar “Conectar”
- Selecionar o dispositivo pareado na lista
- Pressionar “Calibração”
- Aguardar até que no robô termine a calibração
- Colocar o seguidor no início do percurso com a frente no sentido que ele deve seguir
- Pronto! Agora é só tocar no botão “RUN” que o seguidor vai iniciar

## Análise Detalhada do Código

O código do Seguidor Ômega é dividido em vários arquivos `.cpp` e `.hpp`, cada um responsável por uma parte específica do controle do robô. A seguir está uma análise detalhada de cada um desses arquivos:

### main.cpp

Este é o ponto de entrada do programa. O arquivo `main.cpp` inicializa o robô, configura os sensores e executa o loop principal de controle do robô. Ele também contém funções de interrupção para lidar com eventos do ADC (Analog-to-Digital Converter) e eventos de entrada serial.

### seguidor.cpp

Este arquivo contém a implementação da classe `Seguidor_de_Linha`, que é responsável pela lógica de controle principal do robô. Ela contém funções para seguir a linha (`seguir_linha()` e `seguir_linha_final()`), controlar a direção e velocidade dos motores (`set_direcao()`, `set_velocidade()` e `set_velocidade_fast()`), e verificar a chegada e a seção atual do robô (`checar_chegada()` e `checar_secao()`).

### joystick.cpp

Este arquivo contém a implementação da função `joystick_control`, que permite controlar o robô manualmente com um joystick. A função lê comandos do joystick e altera a direção e a velocidade dos motores do robô de acordo.

### comm_serial.cpp

Este arquivo contém várias funções que permitem a comunicação do robô com um computador ou outro dispositivo via porta serial. Ele contém funções para interpretar e executar comandos (`ControlCMD()`), enviar constantes de controle (`send_Consts()`), configurar constantes de controle (`set_Consts()`), parar e iniciar o robô (`stop()` e `run()`) e testar os sensores (`teste_frontal()` e `teste_lateral()`).

### calibration.cpp

Este arquivo contém funções para calibrar os sensores do robô. A função `sensor_calib()` calibra os sensores individuais e a função `calibracao()` executa uma calibração completa dos sensores frontais e laterais.

### controlador_PID.cpp

Este arquivo implementa um controlador PID (Proporcional-Integral-Derivativo) para corrigir a trajetória do robô com base no erro entre a posição atual do robô e a linha que ele está tentando seguir. A função `corrigir_trajeto()` usa o controlador PID para ajustar a velocidade dos motores e corrigir a trajetória do robô. A função `get_correcao()` é usada para calcular a correção PID com base no erro atual.

### motor.cpp

Este arquivo contém a implementação da classe `motor`, que é usada para controlar os motores do robô. Ela contém funções para configurar os pinos de controle do motor (`set_pins()`), definir a direção do motor (`set_direcao()`), e definir a velocidade do motor (`set_velocidade()` e `set_velocidade_fast()`).

### sensor.cpp

Este arquivo contém a implementação da classe `sensor`, que é usada para ler e processar dados dos sensores do robô. Ela contém funções para ler valores dos sensores (`ler()` e `ler_fast()`), calcular um valor limite para o sensor (`calc_limite()`), e redefinir os sensores (`reset()`).

### sensores_frontais.cpp

Este arquivo contém a implementação da classe `sensores_frontais`, que é usada para manipular e processar dados de vários sensores frontais do robô. Ela contém funções para ler valores dos sensores (`ler_sensor()`), calcular o erro entre a posição atual do robô e a linha (`erro_analogico()`), e redefinir os sensores (`reset()`).

## Compilação e Execução

Para compilar e executar o código, você precisa de um ambiente de desenvolvimento Arduino. Você pode abrir o arquivo `main.cpp` no ambiente de desenvolvimento Arduino e compilar e fazer o upload do código para o seu robô Arduino.

## Conclusão

Este projeto é um exemplo de como implementar um robô seguidor de linha usando um Arduino. Ele demonstra como usar sensores para detectar uma linha, um controlador PID para seguir a linha, um joystick para controle manual e comunicação serial para interação com o usuário. O código é bem estruturado e modular, tornando-o um bom ponto de partida para desenvolver seu próprio robô seguidor de linha.
