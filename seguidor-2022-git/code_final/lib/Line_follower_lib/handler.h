#include "motor.h"

String command = "";



const char SET = 'S';
const char STOP = 'P';
const char RUN = 'R';
const char CALIBRACAO = 'C';
const char SEND_CONST = 'T';
const char TESTE_LATERAL = 'L';
const char TESTE_FRONTAL = 'F';
const char TESTE_CURVA = 'J';




// reescrever para tirar sobrecarga de tarefas
void set_handler()
{
	String VB = "", KP_str = "", KI_str = "", KD_str = "", K_str = "", vel_min_str = "", vel_max_str = "", TMP_calib_txt = "";
	int pos = command.indexOf(',', 2);
	for (int i = 4; i < pos; i++)
		KP_str += command[i];

	int pos2 = command.indexOf(',', pos + 1);
	for (int i = pos + 3; i < pos2; i++)
		KI_str += command[i];

	pos = command.indexOf(',', pos2 + 1);
	for (int i = pos2 + 3; i < pos; i++)
		KD_str += command[i];

	pos2 = command.indexOf(',', pos + 1);
	for (int i = pos + 3; i < pos2; i++)
		VB += command[i];

	pos = command.indexOf(',', pos2 + 1);
	for (int i = pos2 + 3; i < pos; i++)
		K_str += command[i];

	pos2 = command.indexOf(',', pos + 1);
	for (int i = pos + 3; i < pos2; i++)
		vel_min_str += command[i];
	
	pos = command.indexOf(',', pos2 + 1);
	for (int i = pos2 + 3; i < pos; i++)
		vel_max_str += command[i];

	pos2 = command.indexOf(',', pos + 1);
	for (int i = pos + 3; i < pos2; i++)
		TMP_calib_txt += command[i];

	Kp = KP_str.toDouble() / 1000;
	Kd = KD_str.toDouble() / 1000;
	Ki = KI_str.toDouble() / 1000;
	vel_base = VB.toInt();
	K = K_str.toDouble() / 1000;
	vel_min = vel_min_str.toInt();
	vel_max = vel_max_str.toInt();
	TMP_calib = TMP_calib_txt.toInt();
	qnt_linhas = 2;
	stop_condition = false;

	// Serial.println(STOP_VALUE_LATERAL);
}

void stop_handler()
{
	digitalWrite(pin_direcional_motor_dir_1, LOW); // direcional
	digitalWrite(pin_direcional_motor_dir_2, LOW); // direcional

	digitalWrite(pin_direcional_motor_esq_1, LOW); // direcional
	digitalWrite(pin_direcional_motor_esq_2, LOW); // direcional

	Estado_corrida = false;
}

void run_handler()
{
	digitalWrite(pin_direcional_motor_dir_1, HIGH); // direcional
	digitalWrite(pin_direcional_motor_dir_2, LOW);	// direcional

	digitalWrite(pin_direcional_motor_esq_1, HIGH); // direcional
	digitalWrite(pin_direcional_motor_esq_2, LOW);	// direcional

	Estado_corrida = true;
}

void find_max()
{
	double valoresLidos[6] = {0, 0, 0, 0, 0, 0};

	for (unsigned j = 0; j <= 5000; j++)
	{
		// delay(100);
		for (unsigned i = 0; i < 6; i++)
		{
			valoresLidos[i] = 1023 - analogRead(nomes_sensores[i]);

			if (valLeitura_max[i] < valoresLidos[i])
				valLeitura_max[i] = valoresLidos[i];
		}
	}
	double soma = 0;
	for (unsigned int i = 0; i < 6; i++)
	{
		soma += valLeitura_max[i];
	}
	STOP_VALUE_FRONTAL = soma /( NUM_SENSORS * 2);
}

void find_min()
{
	double valoresLidos[6] = {0, 0, 0, 0, 0, 0};

	for (unsigned j = 0; j <= 5000; j++)
	{
		// delay(100);
		for (unsigned i = 0; i < 6; i++)
		{
			valoresLidos[i] = 1023 - analogRead(nomes_sensores[i]);

			if (valLeitura_min[i] > valoresLidos[i])
				valLeitura_min[i] = valoresLidos[i];
		}
	}
}

void find_max_lateral()
{
	double valor_lido = 0;
	for (unsigned int i = 0; i < 1000; i++)
	{
		valor_lido = 1023 - analogRead(A1);

		if (val_leitura_max_lateral < valor_lido)
				val_leitura_max_lateral = valor_lido;
		delay(2);
	}
	STOP_VALUE_LATERAL = val_leitura_max_lateral/3;

	//Serial.println("Valor da calibracao lateral: ");
	//Serial.println(STOP_VALUE_LATERAL);

	
}
void find_max_curvas()
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
void calibracao_handler()
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
	
	/*delay(500);

	Serial.println("start calibracao sensor de curva");
	motor_calibration(3);
	find_max_curvas();

	Serial.println("end calibracao sensor de curva");*/
}
void teste_frontal(){
	double valoresLidos[6] = {0, 0, 0, 0, 0, 0};

	for (unsigned j = 0; j <= 1000; j++)
	{
		// delay(100);
		for (unsigned i = 0; i < 6; i++)
		{
			valoresLidos[i] += 1023 - analogRead(nomes_sensores[i]);
		}
	}
	double soma = 0;
	for (unsigned int i = 0; i < 6; i++)
	{
		soma += valoresLidos[i]/1000;
	}
	Serial.print("Resultado teste frontal: ");
	Serial.println(soma/6);
}
void teste_lateral(){
	double valor_lido = 0;
	for (unsigned int i = 0; i < 1000; i++)
	{
		valor_lido += 1023 - analogRead(A1);
		delay(2);
	}
	Serial.print("Resultado teste lateral: ");
	Serial.println(valor_lido/1000);
}
void teste_curva(){
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
void send_const(){
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

void msg_handler()
{
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
	default:
		Serial.println("Comando não reconhecido.");	
	}
	
	
}

void comunica_serial()
{
	if (Serial.available())
	{
		command = Serial.readStringUntil(';');
		Serial.println(command);
		msg_handler();
		command = "";
	}
}


