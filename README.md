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
- Abrir o diretório **seguidor-<version>** por meio da extensão PlatformioIO no Visual Studio Code
- Fazer o upload a partir do PlatformioIO para o Arduino Nano

Obs.: Caso o upload não funcione, desconecte o módulo bluetooth do Seguidor e tente fazer o upload novamente.

### Iniciando o seguidor
Após instalar o app em um dispositivo, já é possível controlar o seguidor! 😃

Para configurar o seguidor é necessário seguir os seguintes passos:
- Transmitir o código para o seguidor conforme os passos anteriores
- Ligar o seguidor e colocá-lo na pista de forma que todos os seus sensores frontais fiquem sobre a linha que deve ser seguida.
- Fazer o pareamento bluetooth com “HC-05’ ou "MMD-7585" (senha de pareamento: 1234 ou 0000)
- Abrir o app e pressionar “Conectar”
- Selecionar o dispositivo pareado na lista
- Pressionar “Calibração”
- Aguardar até que no robô termine a calibração
- Colocar o seguidor no início do percurso com a frente no sentido que ele deve seguir
- Pronto! Agora é só tocar no botão “RUN” que o seguidor vai iniciar
