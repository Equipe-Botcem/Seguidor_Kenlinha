# Seguidor Ômega 2023

Código refatorado em POO a partir da versão de 2022

## POO:
- Adição das classes: Seguidor_de_Linha, motor, sensor, sensores_frontais
- Cada classe gerencia a parte fisica do robo que dá seu nome. Por exemplo a classe **motor** gerencia um motor do robo.
- A classe **sensores_frontais** faz uso da classe **sensor** para gerenciar os sensores frontais do robo.
- A classe **Seguidor_de_Linha** integra as outras classes por composição, ou seja, um objeto **Seguidor_de_Linha** tem como objetos internos: 2 **motor**, 1 **sensor**, 1 **sensores_frontais**. 

## Adições
- A1: Controlador PID.