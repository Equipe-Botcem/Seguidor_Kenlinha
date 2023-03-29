#include <seguidor.h>


Seguidor_de_Linha::Seguidor_de_Linha(/* args */)
{
    conf_pinos();
}
void Seguidor_de_Linha::conf_pinos(){
    for(int i =0; i < 8; i++){
        pinMode(pinos[i], INPUT);
    }
    for(int i =8; i < 14; i++){
        pinMode(pinos[i], OUTPUT);
    }

    set_direcao('F');
	motor_dir.set_pins(pinos[8] , pinos[9] , pinos[10]);
	motor_esq.set_pins(pinos[11], pinos[12], pinos[13]);
	sns_frontais.set_pinos(pinos);
	sensor_chegada.set_pin(pinos[6]);
}
void Seguidor_de_Linha::set_direcao(char direcao = 'F'){
    switch (direcao)
    {
	case 'D': //Direita
		motor_dir.set_direcao('T');
		motor_esq.set_direcao('F');
        break;
    case 'E': //Esquerda
		motor_dir.set_direcao('F');
		motor_esq.set_direcao('T');
        break;
    default:
		motor_dir.set_direcao(direcao);
		motor_esq.set_direcao(direcao);
        break;
    }
}

void Seguidor_de_Linha::set_velocidade(int vel_dir, int vel_esq){
	motor_dir.set_velocidade(vel_dir);
	motor_esq.set_velocidade(vel_esq);
}

bool Seguidor_de_Linha::ignorar_leitura()
{
	if ((millis() - start_time) <= (0.3 * second))
		return true;
	return false;
}

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

double Seguidor_de_Linha::calibracao_frontal(double valor_descalibrado, double valLeitura_max, double valLeitura_min)
{
	double c = 0;

	c = (valor_descalibrado - valLeitura_min) * (1023 / (valLeitura_max - valLeitura_min));

	if(c>1023) c=1023;
	if(c<0)	c=0;

	return c;
}
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
void Seguidor_de_Linha::iguala_curva_reta(){
	c_Kp = Kp;
	c_Ki = 0;
	c_Kd = 0;
	c_K = K;
	//c_vel_max = vel_max;
	//c_vel_min = vel_min;
	//c_vel_base = vel_base;
}
double Seguidor_de_Linha::calc_erro()
{

	//double soma = 0;
	double erro_calc = 0;

	sns_frontais.ler_todos();
	iguala_curva_reta();

	identifica_perda_caminho();
	sns_frontais.ler_todos();
	sensor_encruzilhada_condition();
	//sensor_curva_condition(1023 - analogRead(A7));
	sensor_stop_condition();

	double pesos[6] = {-8, -3, -1, 1, 3, 8};
	if(modo != 'N') {
		pesos[0] = -10;
		pesos[5] = 10;
	}
	int valor_calibrado;
	
	for (unsigned int i = 0; i < 6; i++)
	{
		valor_calibrado = calibracao_frontal(sns_frontais.leituras[i], sns_frontais.max_leituras[i], sns_frontais.min_leituras[i]);
		erro_calc += valor_calibrado * pesos[i];

	}
	return erro_calc;
}

double Seguidor_de_Linha::calc_erro2()
{

	//double soma = 0;
	double erro_calc = 0;

	sns_frontais.ler_todos();
	iguala_curva_reta();

	identifica_perda_caminho();
	sns_frontais.ler_todos();
	sensor_encruzilhada_condition();
	//sensor_curva_condition(1023 - analogRead(A7));
	//sensor_stop_condition();

	
	if(sns_frontais.leituras[2] > STOP_VALUE_FRONTAL && (sns_frontais.leituras[3] > STOP_VALUE_FRONTAL)){
		erro_calc = 0;
	}
	else if(sns_frontais.leituras[0] > STOP_VALUE_FRONTAL && (sns_frontais.leituras[1] > STOP_VALUE_FRONTAL)){
		erro_calc = -8;
	}
	else if(sns_frontais.leituras[0] > STOP_VALUE_FRONTAL){
		erro_calc = -16;
	}
	else if(sns_frontais.leituras[5] > STOP_VALUE_FRONTAL && (sns_frontais.leituras[4] > STOP_VALUE_FRONTAL)){
		erro_calc = 8;
	}
	else if(sns_frontais.leituras[5] > STOP_VALUE_FRONTAL){
		erro_calc = 16;
	}
	else if(sns_frontais.leituras[1] > STOP_VALUE_FRONTAL){
		erro_calc = -4;
	}
	else if(sns_frontais.leituras[4] > STOP_VALUE_FRONTAL){
		erro_calc = 4;
	}
	else if(sns_frontais.leituras[2] > STOP_VALUE_FRONTAL){
		erro_calc = -2;
	}
	else if(sns_frontais.leituras[3] > STOP_VALUE_FRONTAL){
		erro_calc = 2;
	}
	return erro_calc;
}

double Seguidor_de_Linha::PID_calc(double erro)
{
	double value = 0;
	P = erro;
	// I = I + erro;
	//  D = erro_antigo - erro;
	//  erro_antigo = erro;

	if(modo == 'N') value = (Kp * P);
	else value = (c_Kp * P);
	// double valor = Kp * P + Ki * I + Kd * D;
	return value;
}

double Seguidor_de_Linha::PID_calc2(double erro)
{
	double value = 0;
	if(abs(erro) <= 1) erro_I = 0;
	erro_P = erro;
	erro_I += erro;
	erro_D = erro_antigo - erro;

	erro_antigo = erro;

	if(erro_I > 255) erro_I = 255;
	else if(erro_I < -255) erro_I = -255;
	
	value = (Kp * erro_P) + (Ki * erro_I) + (Kd * erro_D);
	return value * 100;
}
void Seguidor_de_Linha::set_velocidade(double erro)
{
	double PID = PID_calc2(erro);
	//Serial.println(erro);
	if(PID >= 0){
		int vel_corrigida = vel_max - PID;
		if(vel_corrigida <= vel_min) vel_corrigida = vel_min;

		set_velocidade(vel_corrigida, vel_max);
	}
	else{
		int vel_corrigida = vel_max + PID;
		if(vel_corrigida <= vel_min) vel_corrigida = vel_min;

		set_velocidade(vel_max, vel_corrigida);
	}
}
double Seguidor_de_Linha::Vel_direcional(double erro)
{	
	double value = (vel_max - vel_base) - K * abs(erro); 
	if (value <= 0) return 0;
	return value;
}

double Seguidor_de_Linha::motor_controler_esq(double erro)
{
	vel_motor_esquerdo = vel_base + Vel_direcional(erro) - PID_calc(erro);
	return motor_check(vel_motor_esquerdo);
}

double Seguidor_de_Linha::motor_controler_dir(double erro)
{
	vel_motor_direito = vel_base + Vel_direcional(erro) + PID_calc(erro);
	return motor_check(vel_motor_direito);
}

void Seguidor_de_Linha::motor_controler(double erro)
{
	set_velocidade(motor_controler_dir(erro), motor_controler_esq(erro));
}

int Seguidor_de_Linha::motor_check(double vel_motor)
{
	if (vel_motor > 255)
	{
		vel_motor = 255;
	}
	else if (vel_motor < vel_min)
	{
		vel_motor = vel_min;
	}
	return vel_motor;
} 
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

void Seguidor_de_Linha::send_const(){
	String msg = "T: Constantes: ";
	Serial.print(msg);
	Serial.print(Kp*1000); 		Serial.print("|");
	Serial.print(Ki*1000); 		Serial.print("|");
	Serial.print(Kd*1000); 		Serial.print("|");
	Serial.print(vel_base); 	Serial.print("|");
	Serial.print(K*1000); 		Serial.print("|");
	Serial.print(vel_min);  	Serial.print("|");
	Serial.print(vel_max);   	Serial.print("|");
	Serial.print(TMP_calib); 
}

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
// reescrever para tirar sobrecarga de tarefas
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

void Seguidor_de_Linha::find_min()
{
	for (unsigned j = 0; j <= 5000; j++)
	{
		sns_frontais.ler_todos();
	}
}

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