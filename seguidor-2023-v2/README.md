# Seguidor Ômega 2023 V2

Em desenvolvimento...

## Refatoração:
- Foi criada uma classe própria para o controlador PID
- O erro agora é calculado diretamente pela classe sensores_frontais
- Agora cada sensor tem seu limite de luminosidade (voltando alguns passos >> [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2022))
- Retirada de várias funções e variáveis desnecessárias

## Problemas principais:
- P1: Muitas oscilações em trajetos retilineos.
- P2: Código lento

## Correções:
- C1: O tamanho do vetor dos erros foi reduzido.

## Adições:
- A1: Obtenção dos valores analógicos por meio de interrupção.

## Resultado:
- Em testes...
