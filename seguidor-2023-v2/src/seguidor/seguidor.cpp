#include <seguidor.hpp>

// Método construtor que configura os pinos e define a direção inicial
Seguidor_de_Linha::Seguidor_de_Linha()
{
    pinMode(12, OUTPUT); // Pino 12 configurado como saída
    digitalWrite(12, HIGH); // Define a saída digital para o pino 12 como ALTA
    for(int i = 0; i < 8; i++){
        pinMode(pinos[i], INPUT); // Define os pinos dos sensores como entrada
    }
    for(int i = 8; i < 14; i++){
        pinMode(pinos[i], OUTPUT); // Define os pinos dos motores como saída
    }
    sns_frontais.set_pinos(pinos); // Configura os pinos dos sensores frontais
    sensor_chegada.set_pin(pinos[6]); // Configura o pino do sensor de chegada
    sensor_mapa.set_pin(pinos[7]); // Configura o pino do sensor de mapa
    motor_dir.set_pins(pinos[8] , pinos[9] , pinos[10]); // Configura os pinos do motor direito
    motor_esq.set_pins(pinos[11], pinos[12], pinos[13]); // Configura os pinos do motor esquerdo
    pinMode(11, INPUT); // Configura o pino 11 como entrada
    digitalWrite(11, LOW); // Define a entrada digital para o pino 11 como BAIXA
    set_direcao('F'); // Define a direção inicial como 'Frente'
}

// Função para fazer o robô seguir a linha
float Seguidor_de_Linha::seguir_linha(){
    float erro = sns_frontais.erro_analogico(); // Calcula o erro entre a posição do robô e a linha
    float erro_filtrado = Kalman.updateEstimate(erro); // Usa o filtro de Kalman para estimar o erro real
    if(erro == 111111){ // Se o erro for 111111, o robô está em uma encruzilhada
        estado_s_chegada = 2;
        estado_s_mapa = 2;
        erro_filtrado = 0; // Reseta o erro filtrado
    }
    else if(abs(erro_filtrado) < 10){ } // Se o erro for menor que 10, o robô está quase na linha
    else{ 
        curva_time = millis(); // Atualiza a variável 'curva_time' com o tempo atual
    }
    if(TMP_calib != 0) checar_chegada(); // Verifica se o robô está na linha de chegada
    if(millis() - start_time >= Controlador.get_K() && (Controlador.get_K() != 0)){ // Verifica se o tempo limite foi atingido
        stop("Tempo Limite"); // Para o robô
    }
    ADCSRA |= (1 << ADSC); // Inicia a conversão ADC
    if(abs(erro) == 30) Controlador.corrigir_trajeto(erro,&motor_dir, &motor_esq); // Corrige a trajetória do robô
    else Controlador.corrigir_trajeto(erro_filtrado,&motor_dir, &motor_esq); // Corrige a trajetória do robô usando o erro filtrado
    return erro; // Retorna o erro
}

// Função para fazer o robô seguir a linha até a linha de chegada
float Seguidor_de_Linha::seguir_linha_final(){
    float erro = Kalman.updateEstimate(sns_frontais.erro_analogico()); // Usa o filtro de Kalman para estimar o erro
    ADCSRA |= (1 << ADSC); // Inicia a conversão ADC
    Controlador.corrigir_trajeto_sem_mover(erro,&motor_dir, &motor_esq); // Corrige a trajetória do robô sem mover o robô
    return erro; // Retorna o erro
}

// Função para obter o pino
int Seguidor_de_Linha::getpin(int pin){
    return pinos[pin]; // Retorna o pino
}

// Função para obter o modo
char Seguidor_de_Linha::get_modo(){
    return modo; // Retorna o modo
}

// Função para definir a direção do robô
void Seguidor_de_Linha::set_direcao(char direcao = 'F'){
    switch (direcao) // Define a direção baseada no valor de 'direcao'
    {
	case 'D': // Direita
		motor_dir.set_direcao('T');
		motor_esq.set_direcao('F');
        break;
    case 'E': // Esquerda
		motor_dir.set_direcao('F');
		motor_esq.set_direcao('T');
        break;
    default: // Frente
		motor_dir.set_direcao(direcao);
		motor_esq.set_direcao(direcao);
        break;
    }
    direcao_atual = direcao; // Atualiza a direção atual
}

// Função para definir a velocidade dos motores
void Seguidor_de_Linha::set_velocidade(int vel_dir, int vel_esq){
    motor_dir.set_velocidade(vel_dir); // Define a velocidade do motor direito
    motor_esq.set_velocidade(vel_esq); // Define a velocidade do motor esquerdo
}

// Função para definir a velocidade dos motores rapidamente
void Seguidor_de_Linha::set_velocidade_fast(int vel_dir, int vel_esq){
    motor_dir.set_velocidade_fast(vel_dir); // Define a velocidade rápida do motor direito
    motor_esq.set_velocidade_fast(vel_esq); // Define a velocidade rápida do motor esquerdo
}

// Função para verificar a linha de chegada
void Seguidor_de_Linha::checar_chegada()
{
    if (sensor_chegada.get_ult_leitura() >= MAX_PRETO_CHEGADA) // Se o robô está na linha de chegada
    {
        if(estado_s_chegada == 0){
            if(qnt_linhas > 1 || ((millis() - start_time) >= TMP_calib)) qnt_linhas--;
            else{
                estado_s_chegada = 1;
                return;
            }
            estado_s_chegada = 1; // Atualiza o estado do sensor de chegada
        }
        else if(estado_s_chegada == 2) estado_s_chegada = 1;
    }
    else if(estado_s_chegada == 1) estado_s_chegada = 0;
}

// Função para verificar a seção
void Seguidor_de_Linha::checar_secao()
{
    if (sensor_mapa.get_ult_leitura() >= MAX_PRETO_MAPA) // Se o robô está na seção do mapa
    {
        if(estado_s_mapa == 0){
            if(Controlador.get_estado_secao() == false) Controlador.prox_secao(); // Vai para a próxima seção
            estado_s_mapa = 1; // Atualiza o estado do sensor de mapa
        }
        else if(estado_s_mapa == 2) estado_s_mapa = 1;
    }
    else if(estado_s_mapa == 1) estado_s_mapa = 0; // Reseta o estado do sensor de mapa
}
