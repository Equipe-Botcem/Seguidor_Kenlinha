# Seguidor Ômega 2023 V2

Em desenvolvimento...

## Refatoração:
- Foi criada uma classe própria para o controlador PID
- O erro agora é calculado diretamente pela classe sensores_frontais
- ~~Agora cada sensor tem seu limite de luminosidade~~ (voltando alguns passos >> [2022](https://github.com/Equipe-Botcem/Seguidor_Omega/tree/main/seguidor-2022)) -> Não utilizado, já que agora usa erro analógico
- Retirada de várias funções e variáveis desnecessárias
- Adição de códigos de teste que podem ser desativados facilmente.

## Problemas principais:
- P1: Muitas oscilações em trajetos retilineos.
- P2: Código lento
- P3-1: Perda de trajetória em curvas: por alta velocidade.
- P3-2: Perda de trajetória em curvas: o robô vai pro lado contrário ao que deveria.

## Correções:
- ~~C1: O tamanho do vetor dos erros foi reduzido.~~
- C2: Mudanças na metodologia de aplicação do PID
- C3: Utilizar filtro na leitura de cada sensor
- C4: Adicionada uma variável em sensores_frontais que guarda o valor absoluto do erro caso seja > 0.3, o objetivo e utilizar esse erro para servir de parâmetro quando o robô perder a linha. Foco em P3-2


## Adições:
- A1: Obtenção dos valores analógicos por meio de interrupção.
- A2: Adição do calculo do erro analógico (por consequência dos resultados, o erro digital foi retirado).
- A3: Adição do filtro de Kalman sobre o erro obtido de sensores_frontais.

## Resultado:
- C2, A1 e A2 são os pontos mais importantes da correção de P1 e P2, estes fizeram o robô se tornar muito mais eficiente em retas e continuar fazendo curvas bem.
- C3, C4 e A3 melhoraram a trajetória e tentam resolver P3-2, que foi atenuado, no entanto algumas vezes ainda ocorre.

- Em testes...
