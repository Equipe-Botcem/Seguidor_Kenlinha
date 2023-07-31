#include "seguidor.hpp"

// Método para controlar o robô usando um joystick
void Seguidor_de_Linha::joystick_control(String cmd){
    if(Estado_corrida) return; // Se o robô estiver em uma corrida, retorna
    modo = 'J'; // Define o modo como 'Joystick'

    // Inicializa variáveis
    String dados_texto[4]; // Array de strings para armazenar os valores do comando
    float dados[4]; // Array de floats para armazenar os valores convertidos do comando
    bool novo_valor = false; // Variável booleana para verificar se um novo valor foi encontrado
    int cont_caracter = 0; // Contador de caracteres
    int cont = 0; // Contador de valores

    // Loop para percorrer a string do comando
    for(unsigned int i = 0; i < cmd.length(); i++){
        // Se o caracter atual for um dígito, um ponto, um sinal de menos ou um sinal de mais
        if(isDigit(cmd[i]) || (cmd[i] == '.') || (cmd[i] == '-') || (cmd[i] == '+')){
            if(cont_caracter < 4){ // Se ainda não foram encontrados 4 caracteres
                cont_caracter++; // Incrementa o contador de caracteres
                dados_texto[cont] += cmd[i]; // Adiciona o caracter atual ao valor atual
                novo_valor = true; // Define que um novo valor foi encontrado
            }
        }
        // Se um novo valor foi encontrado e o caracter atual não é um dígito, um ponto, um sinal de menos ou um sinal de mais
        else if(novo_valor){
            cont_caracter = 0; // Reinicia o contador de caracteres
            novo_valor = false; // Define que um novo valor não foi encontrado
            dados[cont] = dados_texto[cont].toDouble(); // Converte o valor atual para float e o adiciona ao array 'dados'
            cont++; // Incrementa o contador de valores
        }
    }
    // Converte o último valor para float e o adiciona ao array 'dados'
    dados[cont] = dados_texto[cont].toDouble();

    // Define os valores do comando
    float x = dados[0], y = dados[1], raio = dados[2];
    
    // Se o joystick não está sendo movido
    if(x == 0 && (y == 0)){
        set_direcao('B'); // Define a direção do robô como 'Parar'
        set_velocidade(0,0); // Define a velocidade do robô como 0
        modo = 'B'; // Define o modo como 'Parar'
        return; // Retorna
    }
    
    // Define a velocidade máxima do joystick
    int vel_max_joystick = 100;
    // Calcula os fatores de direção e rotação
    float fat_drc = y/raio;
    float fat_rot = x/raio;

    // Calcula as velocidades de direção e rotação
    int vel_drc = -fat_drc * vel_max_joystick + (-fat_drc > 0? 50:-50);
    int vel_rot = -fat_rot * vel_max_joystick + (-fat_rot > 0? 50:-50);

    // Define a direção do robô como 'Frente'
    set_direcao('F');
    // Se o fator de rotação é pequeno
    if(abs(fat_rot) < 0.1){
        vel_drc +=(-fat_drc > 0? 50:-50); // Ajusta a velocidade de direção
        set_velocidade(vel_drc, vel_drc); // Define a velocidade do robô
    }
    // Se o fator de direção é pequeno
    else if(abs(fat_drc) < 0.1){
        set_velocidade(vel_rot, -vel_rot); // Define a velocidade do robô
    }
    // Se a velocidade de rotação é positiva
    else if(vel_rot > 0){
        set_velocidade(vel_drc + vel_rot, vel_drc); // Define a velocidade do robô
    }
    // Se a velocidade de rotação é negativa
    else{
        set_velocidade(vel_drc, vel_drc - vel_rot); // Define a velocidade do robô
    }

    // delay(50); // Pausa a execução do programa por 50 milissegundos (comentado no código original)
}
