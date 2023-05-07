# Seguidor Ômega 2022

Código usado na competição RCX - CPBR14

## Problemas iniciais:
- P1: Não retorna a pista após sair da linha
- P2: Perda da linha em curvas muito fechadas
- P3: Confundir linha lateral(chegada e curvas) com a pista

## Correções:
- C1: Adição da função "identifica_perda_caminho" para resolver P1 e P2, que consiste em parar sempre que saí da linha após um determinado tempo, depois gira para o lado em que a linha foi perdida até que encontre novamente. Essa correção ajudou a resolver P3 consequentemente.
- C2: Ajuste nos valores de erros identificados por cada sensor. Ajudou em P2 e P3.

## Problemas gerados pelas correções:
- PC1: Saída da pista muitas vezes durante o trajetoria, ou seja, mesmo retornando o robô acaba saindo muitas vezes da pista, o que diminui sua velocidade média na pista.

## Resultado:
- P1 e P2 foram corrigidos completamente.
- P3 não ocorre mais, no entanto não há garantias que seja sempre assim.
- PC1 pode ser atenuado por meio dos ajustes das constantes do controlador, no entanto não há uma solução definitiva.