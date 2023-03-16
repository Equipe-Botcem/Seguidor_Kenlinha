#include "Arduino.h"
// Defines
#define NUM_SENSORS 6

// Constantes
int nomes_sensores[NUM_SENSORS] = {A0, A2, A3, A4, A5, A6};

int pin_PWM_motor_dir = 6;			 //   PWMB_PONTEH
int pin_direcional_motor_dir_1 = 10; //   BIN2_PONTEH
int pin_direcional_motor_dir_2 = 9;	 //   BIN1_PONTEH

int pin_PWM_motor_esq = 5;			//   PWMA_PONTEH
int pin_direcional_motor_esq_1 = 7; //   AIN2_PONTEH
int pin_direcional_motor_esq_2 = 8; //   AIN1_PONTEH

double Kp = 0.050;
double Ki = 0;
double Kd = 0;
double K = 0.400;

double vel_max = 180;
double vel_min = 15;
double vel_base = 20;

double P = 0;
// double I = 0;
// double D = 0;

double vel_motor_direito = 0;
double vel_motor_esquerdo = 0;

//variaveis para curvas
double c_Kp = 0.080;
double c_Ki = 0;
double c_Kd = 0;
double c_K = 0.250;
double c_vel_max = 90;
double c_vel_min = 5;
double c_vel_base = 20;

char modo = 'N'; // N: Normal | C: Curva

unsigned int TMP_calib = 150;
double STOP_VALUE_LATERAL = 300;
double STOP_VALUE_CURVA = 300;
double STOP_VALUE_FRONTAL = 300;

double erro_antigo = 0;

bool stop_condition = false;
unsigned long start_time = 0;
int qnt_linhas = 2;

unsigned long second = 1000;

double valLeitura_min[6] = {1023, 1023, 1023, 1023, 1023, 1023};
double valLeitura_max[6] = {0, 0, 0, 0, 0, 0};
double val_leitura_max_lateral =0;
double val_leitura_max_curva =0;

double valor_med_max = 0;
double valor_med_min = 1023;

unsigned long cagaco = 0;
int ultimo_sensor_na_linha = 0;
bool saiu_da_linha = false;
double valoresLidos[6];
//-------------------//
bool Estado_corrida = false;

bool Curva_foi_pro_preto = false;

unsigned long tmp_saida_linha = 0;
void pin_config()
{
	// Input
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	pinMode(A5, INPUT);
	pinMode(A6, INPUT);
	pinMode(A7, INPUT);

	//Output - Sensor de curvas
	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);
	pinMode(3, OUTPUT);
	digitalWrite(3, LOW);

	// Output - motor dir
	pinMode(pin_PWM_motor_dir, OUTPUT);
	pinMode(pin_direcional_motor_dir_1, OUTPUT);
	pinMode(pin_direcional_motor_dir_2, OUTPUT);
	digitalWrite(pin_direcional_motor_dir_1, HIGH); // direcional
	digitalWrite(pin_direcional_motor_dir_2, LOW);	// direcional

	// Output - motor esq
	pinMode(pin_PWM_motor_esq, OUTPUT);
	pinMode(pin_direcional_motor_esq_1, OUTPUT);
	pinMode(pin_direcional_motor_esq_2, OUTPUT);
	digitalWrite(pin_direcional_motor_esq_1, HIGH); // direcional
	digitalWrite(pin_direcional_motor_esq_2, LOW);	// direcional
}

bool timer_cagaco(){
	// if(millis() - cagaco >= 83*second) return true;
	return false;
}

bool ignorar_leitura()
{
	if ((millis() - start_time) <= (0.3 * second))
		return true;
	return false;
}

void sensor_stop_condition(int value)
{
	if (value >= STOP_VALUE_LATERAL)
	{
		//Serial.print("Lateral: ");
		//Serial.println(value);
		if (!ignorar_leitura())
		{
			qnt_linhas--;
			Serial.print("qnt_linha: ");
			Serial.println(qnt_linhas);
			start_time = millis();
			if (qnt_linhas == 0)
				stop_condition = true;
		}
	}
}

void sensor_curva_condition(int value)
{
	if(value < STOP_VALUE_CURVA) Curva_foi_pro_preto = true;
	else if(Curva_foi_pro_preto){
		if(modo == 'N') {
			modo = 'C';
			analogWrite(pin_PWM_motor_dir, 0);
			analogWrite(pin_PWM_motor_esq, 0);
			delay(10);
		}
		else modo = 'N';
		Curva_foi_pro_preto = false;
		
	}
}

void sensor_encruzilhada_condition()//double soma)
{
	bool encruzilhada = true;
	for (unsigned int i = 0; i < NUM_SENSORS; i++){
		if (valoresLidos[i] <= (STOP_VALUE_FRONTAL)){
			encruzilhada = false;
		}
	}

	if(encruzilhada){
		start_time = millis();
	}
	/*if (soma >= (STOP_VALUE_FRONTAL) * (NUM_SENSORS))
	{
		//Serial.println("encruzilhada");
		start_time = millis();
	}*/
}

double calibracao_frontal(double valor_descalibrado, double valLeitura_max, double valLeitura_min)
{
	double c = 0;

	c = (valor_descalibrado - valLeitura_min) * (1023 / (valLeitura_max - valLeitura_min));

	if(c>1023) c=1023;
	if(c<0)	c=0;
	/*Serial.println("-------------------------------------");
	Serial.print("valor descalibrado = ");
	Serial.println(valor_descalibrado);
	Serial.print("valor_max = ");
	Serial.println(valLeitura_max);
	Serial.print("valor min = ");
	Serial.println(valLeitura_min);
	*/

	return c;
}

/**
 * @brief
 *
 * @return double
 */

void ler_sensores_frontais(){
	for (unsigned int i = 0; i < NUM_SENSORS; i++)
	{
		valoresLidos[i] = 1023 - analogRead(nomes_sensores[i]);
	}
}
bool sensor_check_black(){
	bool all_black = true;
	for(int i = 0; i < 6; i++){
		if(valoresLidos[i] > STOP_VALUE_FRONTAL){
			if(valoresLidos[i] > valoresLidos[ultimo_sensor_na_linha] && (!saiu_da_linha)) ultimo_sensor_na_linha = i;
			all_black = false;
		}
	}
	return all_black;
}
void identifica_perda_caminho(){
	
	if(!sensor_check_black()) return;
	if(tmp_saida_linha == 0) tmp_saida_linha = millis();
	if((millis() - tmp_saida_linha) < TMP_calib) return;

	int leitura_linha = 0;
	//int tmp_passado = millis();
	int cont = 0;
	//&& ((millis() - tmp_passado) < 200)
	while (leitura_linha < STOP_VALUE_FRONTAL ){
		//Serial.println("Aqui 3");
		stop_condition = true;
		saiu_da_linha = true;

		if(ultimo_sensor_na_linha < 3){
			digitalWrite(pin_direcional_motor_dir_1, LOW); // direcional
			digitalWrite(pin_direcional_motor_dir_2, HIGH);	// direcional
			
			digitalWrite(pin_direcional_motor_esq_1, HIGH); // direcional
			digitalWrite(pin_direcional_motor_esq_2, LOW);	// direcional
			leitura_linha = 1023 - analogRead(A4);
		}
		else{
			digitalWrite(pin_direcional_motor_dir_1, HIGH); // direcional
			digitalWrite(pin_direcional_motor_dir_2, LOW);	// direcional
			
			digitalWrite(pin_direcional_motor_esq_1, LOW); // direcional
			digitalWrite(pin_direcional_motor_esq_2, HIGH);	// direcional
			leitura_linha = 1023 - analogRead(A3);

		}
		
		analogWrite(pin_PWM_motor_dir, 180); //120 // 180
		analogWrite(pin_PWM_motor_esq, 180); //120 // 180
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
	
	analogWrite(pin_PWM_motor_dir, 0);
	analogWrite(pin_PWM_motor_esq, 0);
	digitalWrite(pin_direcional_motor_dir_1, HIGH); // direcional
	digitalWrite(pin_direcional_motor_dir_2, LOW);	// direcional
			
	digitalWrite(pin_direcional_motor_esq_1, HIGH); // direcional
	digitalWrite(pin_direcional_motor_esq_2, LOW);	// direcional
	/*Serial.println(leitura_linha);
	Serial.println(STOP_VALUE_FRONTAL);
	delay(3000);*/
	
	stop_condition = false;
	saiu_da_linha = false;
	tmp_saida_linha = 0;
	if(ultimo_sensor_na_linha >= 3) ultimo_sensor_na_linha = 0;
	else ultimo_sensor_na_linha = 5;
}
void iguala_curva_reta(){
	c_Kp = Kp;
	c_Ki = 0;
	c_Kd = 0;
	c_K = K;
	//c_vel_max = vel_max;
	//c_vel_min = vel_min;
	//c_vel_base = vel_base;
}
double calc_erro()
{

	//double soma = 0;
	double erro_calc = 0;

	ler_sensores_frontais();
	iguala_curva_reta();

	identifica_perda_caminho();
	ler_sensores_frontais();
	sensor_encruzilhada_condition();
	//sensor_curva_condition(1023 - analogRead(A7));
	sensor_stop_condition(1023 - analogRead(A1));

	double pesos[6] = {-8, -3, -1, 1, 3, 8};
	if(modo != 'N') {
		pesos[0] = -10;
		pesos[5] = 10;
	}
	int valor_calibrado;

	for (unsigned int i = 0; i < NUM_SENSORS; i++)
	{
		valor_calibrado = calibracao_frontal(valoresLidos[i], valLeitura_max[i], valLeitura_min[i]);
		erro_calc += valor_calibrado * pesos[i];
	}
	return erro_calc;
}

/**
 * @brief Calcula a resposta angular para a velocidades dos motores
 *
 * @param erro erro da leitura dos sensores
 * @return Kp * erro + Ki * a soma de todos os erros
 */

double PID_calc(double erro)
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

int motor_check(double vel_motor)
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

/**
 * @brief Quanto maior o erro menor o retorno
 *
 * @param erro erro da leitura dos sensores
 * @return velocidade maxima - velocidade base - K * erro
 */
double Vel_direcional(double erro)
{	
	double value = (vel_max - vel_base) - K * abs(erro); 
	if(modo == 'C') value = (c_vel_max - c_vel_base) - c_K * abs(erro); 
	if (value <= 0) return 0;
	return value;
}

double motor_controler_esq(double erro)
{
	if(modo == 'N') vel_motor_esquerdo = vel_base + Vel_direcional(erro) - PID_calc(erro);
	else vel_motor_esquerdo = c_vel_base + Vel_direcional(erro) - PID_calc(erro);
	return motor_check(vel_motor_esquerdo);
}

double motor_controler_dir(double erro)
{
	if(modo == 'N') vel_motor_direito = vel_base + Vel_direcional(erro) + PID_calc(erro);
	else vel_motor_direito = c_vel_base + Vel_direcional(erro) + PID_calc(erro);
	return motor_check(vel_motor_direito);
}

void motor_controler(double erro)
{
	analogWrite(pin_PWM_motor_dir, motor_controler_dir(erro));
	analogWrite(pin_PWM_motor_esq, motor_controler_esq(erro));
}

/**
 * @brief Resposavel por controlar o robô durante a calibração 
 * 
 */
void motor_calibration(int dir){

	// Se dir for 1 joga pra tras 
	if (dir == 1){

		// Configura o motor pra tras 
		// Sentido do motor direito 
		digitalWrite(pin_direcional_motor_dir_1, LOW); // direcional
		digitalWrite(pin_direcional_motor_dir_2, HIGH);	// direcional

		// Sentido do motor esquerdo 
		digitalWrite(pin_direcional_motor_esq_1, LOW); // direcional
		digitalWrite(pin_direcional_motor_esq_2, HIGH);	// direcional

		analogWrite(pin_PWM_motor_dir, 150);
		analogWrite(pin_PWM_motor_esq, 150);

		delay(150);

		analogWrite(pin_PWM_motor_dir, 0);
		analogWrite(pin_PWM_motor_esq, 0);

		// Reconfigura pra frente 
		// Sentido do motor direito 
		digitalWrite(pin_direcional_motor_dir_1, HIGH); // direcional
		digitalWrite(pin_direcional_motor_dir_2, LOW);	// direcional

		// Sentido do motor esquerdo 
		digitalWrite(pin_direcional_motor_esq_1, HIGH); // direcional
		digitalWrite(pin_direcional_motor_esq_2, LOW);	// direcional
	
	}else if(dir == 2){
		int leitura_linha = 1023 - analogRead(A1);
		while (leitura_linha < 2 * STOP_VALUE_FRONTAL)
		{
			analogWrite(pin_PWM_motor_dir, 90);
			analogWrite(pin_PWM_motor_esq, 90);
			leitura_linha = 1023 - analogRead(A1);
		}
		analogWrite(pin_PWM_motor_dir, 0);
		analogWrite(pin_PWM_motor_esq, 0);
		
	}
	else if(dir == 3){
		
		int leitura_linha = 1023 - analogRead(A7);
		while (leitura_linha < 2 * STOP_VALUE_FRONTAL)
		{
			analogWrite(pin_PWM_motor_dir, 90);
			analogWrite(pin_PWM_motor_esq, 90);
			leitura_linha = 1023 - analogRead(A7);
		}
		analogWrite(pin_PWM_motor_dir, 0);
		analogWrite(pin_PWM_motor_esq, 0);
	}

}