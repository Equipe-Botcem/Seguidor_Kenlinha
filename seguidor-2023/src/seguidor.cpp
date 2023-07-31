#include <seguidor.h>

// Construtor da classe Seguidor_de_Linha
Seguidor_de_Linha::Seguidor_de_Linha(/* args */)
{
    // Configura os pinos quando uma nova instância da classe é criada
    conf_pinos();
}

// Função para configurar os pinos do Arduino
void Seguidor_de_Linha::conf_pinos(){
    // Configura os pinos 0 a 7 como entradas
    for(int i =0; i < 8; i++){
        pinMode(pinos[i], INPUT);
    }
    // Configura os pinos 8 a 13 como saídas
    for(int i =8; i < 14; i++){
        pinMode(pinos[i], OUTPUT);
    }

    // Define a direção inicial do robô como para frente ('F')
    set_direcao('F');
    // Configura os pinos dos motores e sensores
    motor_dir.set_pins(pinos[8] , pinos[9] , pinos[10]);
    motor_esq.set_pins(pinos[11], pinos[12], pinos[13]);
    sns_frontais.set_pinos(pinos);
    sensor_chegada.set_pin(pinos[6]);
}

// Função para definir a direção dos motores
void Seguidor_de_Linha::set_direcao(char direcao = 'F'){
    switch (direcao)
    {
    case 'D': // Direita
        motor_dir.set_direcao('T');
        motor_esq.set_direcao('F');
        break;
    case 'E': // Esquerda
        motor_dir.set_direcao('F');
        motor_esq.set_direcao('T');
        break;
    default:
        motor_dir.set_direcao(direcao);
        motor_esq.set_direcao(direcao);
        break;
    }
}

// Função para definir a velocidade dos motores
void Seguidor_de_Linha::set_velocidade(int vel_dir, int vel_esq){
    motor_dir.set_velocidade(vel_dir);
    motor_esq.set_velocidade(vel_esq);
}

// Função para ignorar a leitura dos sensores se o robô tiver sido iniciado há menos de 0.3 segundos
bool Seguidor_de_Linha::ignorar_leitura()
{
    if ((millis() - start_time) <= (0.3 * second))
        return true;
    return false;
}

// Função para verificar a condição de parada do robô
void Seguidor_de_Linha::sensor_stop_condition()
{
    int value = sensor_chegada.ler();
    if (value >= STOP_VALUE_LATERAL)
    {
        if (!ignorar_leitura())
        {
            qnt_linhas--;
            Serial.print("qnt_linha: ");
            Serial.println(qnt_linhas);
            start_time = millis();
            if (qnt_linhas == 0){
                stop_condition = true;
                modo = 'P';
            }
        }
    }
}

// Função para verificar a condição de curva
void Seguidor_de_Linha::sensor_curva_condition(int value)
{
    if(value < STOP_VALUE_CURVA) Curva_foi_pro_preto = true;
    else if(Curva_foi_pro_preto){
        if(modo == 'N') {
            modo = 'C';
            set_velocidade(0,0);
            delay(10);
        }
        else modo = 'N';
        Curva_foi_pro_preto = false;
        
    }
}

// Função para verificar a condição de encruzilhada
void Seguidor_de_Linha::sensor_encruzilhada_condition()//double soma)
{
    bool encruzilhada = true;
    for (unsigned int i = 0; i < 6; i++){
        if (sns_frontais.leituras[i] <= (STOP_VALUE_FRONTAL)){
            encruzilhada = false;
        }
    }

    if(encruzilhada){
        start_time = millis();
    }
}

// Função para verificar se o robô está fora da linha
bool Seguidor_de_Linha::checar_fora_linha(){
    bool all_black = true;
    for(int i = 0; i < 6; i++){
        if(sns_frontais.leituras[i] > STOP_VALUE_FRONTAL){
            if(sns_frontais.leituras[i] > sns_frontais.leituras[ultimo_sensor_na_linha] && (!saiu_da_linha)) ultimo_sensor_na_linha = i;
            all_black = false;
        }
    }
    return all_black;
}

// Função para identificar quando o robô perdeu o caminho
void Seguidor_de_Linha::identifica_perda_caminho(){
    
    if(!checar_fora_linha()) return;
    if(tmp_saida_linha == 0) tmp_saida_linha = millis();
    if((millis() - tmp_saida_linha) < TMP_calib) return;

    int leitura_linha = 0;
    int cont = 0;
    while (leitura_linha < STOP_VALUE_FRONTAL ){
        stop_condition = true;
        saiu_da_linha = true;

        if(ultimo_sensor_na_linha >= 3){
            set_direcao('D');
            leitura_linha = 1023 - analogRead(A4);
        }
        else{
            set_direcao('E');
            leitura_linha = 1023 - analogRead(A3);

        }
        
        set_velocidade(120,120);
        cont++;
        if(cont == 30 && Serial.available()){
            String cmd = Serial.readStringUntil(';');
            if(cmd[0] == 'P') {
                Estado_corrida = false;
                break;
            }
            cont = 0;
        }
    }
    
    set_velocidade(0,0);
    set_direcao('B');
    set_direcao('F');
    
    stop_condition = false;
    saiu_da_linha = false;
    tmp_saida_linha = 0;
    if(ultimo_sensor_na_linha >= 3) ultimo_sensor_na_linha = 0;
    else ultimo_sensor_na_linha = 5;
}

// Função para calibrar os motores
void Seguidor_de_Linha::motor_calibration(int dir){

    // Se dir for 1 joga pra tras 
    if (dir == 1){

        // Configura o motor pra tras 
        set_direcao('T');

        set_velocidade(150,150);

        delay(150);

        set_velocidade(0,0);

        // Reconfigura pra frente 
        set_direcao('F');
    
    }else if(dir == 2){
        while (sensor_chegada.ler() < 2 * STOP_VALUE_FRONTAL)
        {
            set_velocidade(90,90);
        }
        set_direcao('B');
        set_velocidade(0,0);
        
        
        
    }
    else if(dir == 3){
        
        int leitura_linha = 1023 - analogRead(A7);
        while (leitura_linha < 2 * STOP_VALUE_FRONTAL)
        {
            set_velocidade(90,90);
            leitura_linha = 1023 - analogRead(A7);
        }
        set_velocidade(0,0);
    }

}

// Função para enviar constantes pela serial
void Seguidor_de_Linha::send_const(){
    String msg = "T: Constantes: ";
    Serial.print(msg);
    Serial.print(Kp*1000);     Serial.print("|");
    Serial.print(Ki*1000);     Serial.print("|");
    Serial.print(Kd*1000);     Serial.print("|");
    Serial.print(vel_base);    Serial.print("|");
    Serial.print(K*1000);      Serial.print("|");
    Serial.print(vel_min);     Serial.print("|");
    Serial.print(vel_max);     Serial.print("|");
    Serial.print(TMP_calib); 
}

// Função para setar valores gerais
void Seguidor_de_Linha::set_geral(String valores){
    double dados[10];
    int pos_del = 0;
    int cont = 0;
    for(int i = 2; i < valores.lastIndexOf(','); i++){
        pos_del = valores.indexOf(',', i);
        dados[cont] = valores.substring(i+2, pos_del).toDouble();
        i = pos_del; cont++;
    }
    Kp = dados[0] / 1000;
    Kd = dados[1] / 1000;
    Ki = dados[2] / 1000;
    vel_base = dados[3];
    K = dados[4] / 1000;
    vel_min = dados[5];
    vel_max = dados[6];
    TMP_calib = dados[7];
    //TMP_enc = dados[8];

}

// Função para lidar com comandos de set enviados pela serial
void Seguidor_de_Linha::set_handler()
{   
    String valores = command;
    double dados[10];
    int pos_del = 0;
    int cont = 0;
    for(int i = 2; i < valores.lastIndexOf(','); i++){
        pos_del = valores.indexOf(',', i);
        dados[cont] = valores.substring(i+2, pos_del).toDouble();
        i = pos_del; cont++;
    }
    Kp = dados[0] / 1000;
    Kd = dados[1] / 1000;
    Ki = dados[2] / 1000;
    vel_base = dados[3];
    K = dados[4] / 1000;
    vel_min = dados[5];
    vel_max = dados[6];
    TMP_calib = dados[7];

    erro_antigo = 0;
    erro_I = 0;
    erro_P = 0;
    erro_D = 0;
    // Serial.println(STOP_VALUE_LATERAL);
}

// Função para lidar com comandos de stop enviados pela serial
void Seguidor_de_Linha::stop_handler()
{
    set_direcao('P');
    modo = 'P';
    erro_antigo = 0;
    erro_I = 0;
    erro_P = 0;
    erro_D = 0;
    Estado_corrida = false;
}

// Função para lidar com comandos de run enviados pela serial
void Seguidor_de_Linha::run_handler()
{
    set_direcao('F');
    modo = 'N';
    erro_antigo = 0;
    erro_I = 0;
    erro_P = 0;
    erro_D = 0;
    qnt_linhas = 2;
    Estado_corrida = true;
    stop_condition = false;
}

// Função para encontrar o valor máximo de leitura do sensor
void Seguidor_de_Linha::find_max()
{

    for (unsigned j = 0; j <= 5000; j++)
    {
            sns_frontais.ler_todos();
    }
    double soma = 0;
    for (unsigned int i = 0; i < 6; i++)
    {
        soma += sns_frontais.max_leituras[i];
    }
    STOP_VALUE_FRONTAL = soma /( 6 * 2);
}

// Função para encontrar o valor mínimo de leitura do sensor
void Seguidor_de_Linha::find_min()
{
    for (unsigned j = 0; j <= 5000; j++)
    {
        sns_frontais.ler_todos();
    }
}

// Função para encontrar o valor máximo de leitura do sensor lateral
void Seguidor_de_Linha::find_max_lateral()
{
    for (unsigned int i = 0; i < 1000; i++)
    {
        sensor_chegada.ler();
    }
    STOP_VALUE_LATERAL = sensor_chegada.get_max_leitura()/3;

    //Serial.println("Valor da calibracao lateral: ");
    //Serial.println(STOP_VALUE_LATERAL);

    
}

// Função para encontrar o valor máximo de leitura do sensor de curva
void Seguidor_de_Linha::find_max_curvas()
{
    double valor_lido = 0;
    for (unsigned int i = 0; i < 1000; i++)
    {
        valor_lido = 1023 - analogRead(A7);

        if (val_leitura_max_curva < valor_lido)
                val_leitura_max_curva = valor_lido;
        delay(2);
    }
    STOP_VALUE_CURVA = val_leitura_max_lateral/3;

    //Serial.println("Valor da calibracao lateral: ");
    //Serial.println(STOP_VALUE_LATERAL);

    
}

// Função para lidar com comandos de calibração enviados pela serial
void Seguidor_de_Linha::calibracao_handler()
{
    Serial.println("start calibracao");

    // Calibra linha branca
    Serial.println("Linha Branca");
    find_max();

    //Serial.println("Branco Calibrado");

    // Vai pra trás pro preta 
    Serial.println("Linha Preta");
    motor_calibration(1);

    // Vai pra linha preta 
    find_min();

    //Serial.println("Preto Calibrado");
    
    Serial.println("end calibracao frontal");

    delay(1000);

    Serial.println("start calibracao lateral");

    // Vai pra frente pra calibrar o lateral 
    motor_calibration(2);

    find_max_lateral();
    
    Serial.println(STOP_VALUE_LATERAL);
    Serial.println("end calibracao lateral");
    
    tmp_saida_linha = 1;
    ultimo_sensor_na_linha = 2;
    int leitura_linha = 0;
    set_direcao('D');
    set_velocidade(100,100);
    while (leitura_linha < STOP_VALUE_FRONTAL ){
        leitura_linha = 1023 - analogRead(A0);
    }
    set_direcao('B');
    set_velocidade(0,0);

    /*delay(500);

    Serial.println("start calibracao sensor de curva");
    motor_calibration(3);
    find_max_curvas();

    Serial.println("end calibracao sensor de curva");*/
}

// Função para testar o sensor frontal
void Seguidor_de_Linha::teste_frontal(){
    for (unsigned j = 0; j <= 1000; j++)
    {
        sns_frontais.ler_todos();
    }
    double soma = 0;
    for (unsigned int i = 0; i < 6; i++)
    {
        soma += sns_frontais.leituras[i];
        Serial.println(sns_frontais.leituras[i]);
    }

    Serial.print("Resultado teste frontal: ");
    Serial.println(soma/6);
    Serial.println(STOP_VALUE_FRONTAL);
    
    double erro = calc_erro2();
    Serial.print("Erro sensores: ");
    Serial.println(erro);
    Serial.print("Erro integral: ");
    Serial.println(erro_I);
    Serial.print("Correcao PID: ");
    Serial.println(PID_calc2(erro));
}

// Função para testar o sensor lateral
void Seguidor_de_Linha::teste_lateral(){
    double valor_lido = 0;
    for (unsigned int i = 0; i < 1000; i++)
    {
        valor_lido += sensor_chegada.ler();
        delay(2);
    }
    Serial.print("Resultado teste lateral: ");
    Serial.println(valor_lido/1000);
}

// Função para testar o sensor de curva
void Seguidor_de_Linha::teste_curva(){
    double valor_lido = 0;
    for (unsigned int i = 0; i < 1000; i++)
    {
        valor_lido += 1023 - analogRead(A7);
        delay(2);
    }
    Serial.print("Resultado teste lateral: ");
    Serial.println(valor_lido/1000);
    Serial.print("Modo: ");
    Serial.println(modo);
}

// Função para lidar com mensagens recebidas pela serial
void Seguidor_de_Linha::msg_handler()
{

    const char SET = 'S';
    const char STOP = 'P';
    const char RUN = 'R';
    const char CALIBRACAO = 'C';
    const char SEND_CONST = 'T';
    const char TESTE_LATERAL = 'L';
    const char TESTE_FRONTAL = 'F';
    const char TESTE_CURVA = 'G';
    const char JOYSTICK = 'J';
    const char PRINT = 'Z';
    switch (command[0])
    {
    case SET:
        set_handler();
        break;
    case STOP:
        stop_handler();
        break;
    case RUN:
        run_handler();
        break;
    case CALIBRACAO:
        if(command[1] != 'l')
            calibracao_handler();
        break;
    case SEND_CONST:
        send_const();
        break;
    case TESTE_FRONTAL:
        teste_frontal();
        break;
    case TESTE_LATERAL:
        teste_lateral();
        break;  
    case TESTE_CURVA:
        teste_curva();
        break;
    case JOYSTICK:
        joystick_control(command);
        break;
    case PRINT:
        printar = true;
        break;
    default:
        Serial.println("Comando não reconhecido.");    
    }
    
    
}

// Função para lidar com a comunicação serial
void Seguidor_de_Linha::comunica_serial()
{
    if (Serial.available())
    {
        command = Serial.readStringUntil(';');
        Serial.println(command);
        msg_handler();
        command = "";
    }
}

// Função para controlar o robô com um joystick
void Seguidor_de_Linha::joystick_control(String cmd){
    if(Estado_corrida) return;
    modo = 'J';
    double dados[5]; int pos_del, cont = 0;
    for(int i = 2; i < cmd.lastIndexOf(','); i++){
        pos_del = cmd.indexOf(',', i);
        dados[cont] = cmd.substring(i, pos_del).toDouble();
        i = pos_del; cont++;
    }
    double x = dados[0], y = dados[1], raio = dados[2];

    if(x == 0 && (y == 0)){
        set_direcao('B');
        set_velocidade(0,0);
        return;
    }
    
    int vel_max_joystick = 200;
    double fat_drc = y/raio;
    double fat_rot = x/raio;

    int vel_drc = -fat_drc * vel_max_joystick;
    int vel_rot = abs(fat_rot * vel_max_joystick);
    if(fat_rot > 0.51){
        set_direcao('D');
        set_velocidade(vel_rot, vel_rot);
        
    }
    else if(fat_rot < -0.51){
        set_direcao('E');
        set_velocidade(vel_rot, vel_rot);
        
    }
    else{
        set_direcao('F');
        set_velocidade(vel_drc, vel_drc);
        
    }
    delay(50);

} 
