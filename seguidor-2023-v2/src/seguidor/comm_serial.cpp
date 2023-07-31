#include "seguidor.hpp"

// Função para lidar com comandos recebidos
void Seguidor_de_Linha::ControlCMD(String command)
{
    Serial.println(command); // Imprime o comando recebido

    // Define constantes para cada tipo de comando
    const char SET = 'S';
    const char STOP = 'P';
    const char RUN = 'R';
    const char CALIBRACAO = 'C';
    const char SEND_CONST = 'T';
    const char TESTE_LATERAL = 'L';
    const char TESTE_FRONTAL = 'F';
    const char JOYSTICK = 'J';

    // Escolhe a ação apropriada para cada comando
    switch (command[0])
    {
    case SET:
        set_Consts(command); // Define constantes
        break;
    case STOP:
        stop("Controle"); // Para o robô
        break;
    case RUN:
        run(); // Inicia o robô
        break;
    case CALIBRACAO:
        if(command[1] != 'l')
            calibracao(); // Calibra o robô
        break;
    case SEND_CONST:
        send_Consts(); // Envia constantes
        break;
    case TESTE_FRONTAL:
        teste_frontal(); // Realiza o teste frontal
        break;
    case TESTE_LATERAL:
        teste_lateral(); // Realiza o teste lateral
        break;    
    case JOYSTICK:
        joystick_control(command); // Controla o robô com joystick
        break;
    default:
        Serial.println("Comando não reconhecido."); // Imprime mensagem de erro para comando não reconhecido   
        break;
    }
}

// Função para enviar constantes
void Seguidor_de_Linha::send_Consts(){

    // Busca constantes e valores atuais
    float constsPID[4]; Controlador.get_const(constsPID);

    float Kp = constsPID[0], Ki = constsPID[1], Kd = constsPID[2], K = constsPID[3];
    
    int vels[2]; Controlador.get_vel(vels);

    int vel_max = vels[0];
    int vel_min = vels[1];
    float TOL = sns_frontais.get_tol();

    // Imprime as constantes e valores atuais
    Serial.print("T: Constantes: ");
    Serial.print(Kp);           Serial.print("|");
    Serial.print(Ki);           Serial.print("|");
    Serial.print(Kd);           Serial.print("|");
    Serial.print(vel_min);      Serial.print("|");
    Serial.print(vel_max);      Serial.print("|");
    Serial.print(TMP_calib);    Serial.print("|");
    Serial.print(K);            Serial.print("|");
    Serial.print(TOL);          Serial.print("|");
    Serial.print(Controlador.get_estado_mapa() ? "Seguindo" : "Nao seguindo");   Serial.print("|");
    Serial.print("Mapa");   Serial.print("|");
    
    Serial.println(
        "\nFrontal: " + (String)MAX_PRETO +
        " - Chegada: " + (String)MAX_PRETO_CHEGADA + 
        " - Mapa: " + (String)MAX_PRETO_MAPA); 
}

// Função para definir constantes
void Seguidor_de_Linha::set_Consts(String valores)
{   
    // Processa a string de valores e converte cada valor em um número
    String dados_texto[10];
    float dados[10];
    bool novo_valor = false;
    int cont = 0;
    for(unsigned int i = 0; i < valores.length(); i++){
        if(isDigit(valores[i]) || (valores[i] == '.') || (valores[i] == '-') || (valores[i] == '+')){
            dados_texto[cont] += valores[i];
            novo_valor = true;
        }
        else if(novo_valor){
            novo_valor = false;
            dados[cont] = dados_texto[cont].toDouble();
            cont++;
        }
    }
    dados[cont] = dados_texto[cont].toDouble();
    
    // Define constantes e valores
    Controlador.set_const(dados[0], dados[1], dados[2], dados[6]); //KP,KI,KD
    Controlador.set_vel(dados[4], dados[3]);
    TMP_calib = dados[5];
    sns_frontais.set_tol(dados[7]);

    Controlador.set_estado_mapa(dados[8] == 1 ? true : false);
    Controlador.reset();
    delay(200);
    send_Consts();
}

// Função para parar o robô
void Seguidor_de_Linha::stop(String agente)
{
    set_direcao('B');
    modo = 'B';
    ler_sensores_fast = true;
    ler_sensores_sem_pausa = false;
    send_Consts();
    Serial.println("Tempo: " + (String)(millis() - start_time));
    Serial.println("Agente: " + agente);
    Serial.println("Secao: " + (String)Controlador.get_secao());
    Controlador.reset();
    Estado_corrida = false;
}

// Função para iniciar o robô
void Seguidor_de_Linha::run()
{
    // Prepara o robô para iniciar
    ler_sensores_fast =true;
    ler_sensores_sem_pausa = true;
    ADCSRA |= (1 << ADSC);
    delay(10);
    ler_sensores_sem_pausa = false;

    // Define direção e modo
    set_direcao('F');
    modo = 'N';
    tempo = 0;
    tmp_fora_linha = 0;
    estado_s_chegada = 0;
    estado_s_mapa = 0;
    curva_time = 0;
    start_time = millis();
    
    curva_time = 0;
    qnt_linhas = 2;//10
    
    Controlador.reset();
    Estado_corrida = true;
    
}

// Função para teste frontal
void Seguidor_de_Linha::teste_frontal(){
    // Realiza o teste frontal
    double soma = 0;
    
    ler_sensores_fast = false;
    ler_sensores_sem_pausa = true;
    ADCSRA |= (1 << ADSC);
    delay(50);
    ler_sensores_sem_pausa = false;
    ler_sensores_fast = true;
    int n_sensores = sns_frontais.get_N_sns();
    for (int i = 0; i < n_sensores; i++)
    {
        soma += sns_frontais.sensores[i].get_ult_leitura();
        Serial.println((String)i + " - " + (String)sns_frontais.sensores[i].get_ult_leitura() +
        " - Min: " + (String)sns_frontais.sensores[i].get_min_leitura() + 
        " - Max: " + (String)sns_frontais.sensores[i].get_max_leitura() + 
            + " - " + (String)(int)(sns_frontais.sensores[i].get_ult_leitura_percent()*100) + "%");
    }

    // Imprime o resultado do teste frontal
    Serial.print("Resultado teste frontal: ");
    Serial.println(soma/n_sensores);
    Serial.println("Angulo: " + (String)sns_frontais.erro_analogico());
}

// Função para teste lateral
void Seguidor_de_Linha::teste_lateral(){
    // Realiza o teste lateral
    ler_sensores_sem_pausa = true;
    ADCSRA |= (1 << ADSC);
    delay(50);
    ler_sensores_sem_pausa = false;
    Serial.println("Chegada: " + (String)sensor_chegada.get_ult_leitura());
    Serial.println("Mapa: " + (String)sensor_mapa.get_ult_leitura());
}
