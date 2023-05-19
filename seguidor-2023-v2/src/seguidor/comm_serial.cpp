#include "seguidor.hpp"

void Seguidor_de_Linha::ControlCMD(String command)
{
	Serial.println(command);
	const char SET = 'S';
	const char STOP = 'P';
	const char RUN = 'R';
	const char CALIBRACAO = 'C';
	const char SEND_CONST = 'T';
	const char TESTE_LATERAL = 'L';
	const char TESTE_FRONTAL = 'F';
	const char TESTE_CURVA = 'G';
	const char ERROS = 'E';
	const char JOYSTICK = 'J';
	switch (command[0])
	{
	case SET:
		set_CONSTS(command);
		break;
	case STOP:
		stop();
		break;
	case RUN:
		run();
		break;
	case CALIBRACAO:
		if(command[1] != 'l')
			calibracao();
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
	case ERROS:
		send_erros();
		break;
	default:
		Serial.println("Comando não reconhecido.");	
		break;
	}
}

void Seguidor_de_Linha::send_const(){

	float constsPID[3]; Controlador.get_const(constsPID);

	float Kp = constsPID[0], Ki = constsPID[1], Kd = constsPID[2];

	String Str_mapa = "";
	int mapa[40] = {0}; Controlador.get_mapa(mapa);
	for (int i =0; i < 40; i++){
		Str_mapa += (String)mapa[i] + "+";
	}
	
	String msg = "T: Constantes: ";
	Serial.print(msg);
	Serial.print(Kp); 			Serial.print("|");
	Serial.print(Ki); 			Serial.print("|");
	Serial.print(Kd); 			Serial.print("|");
	Serial.print(0); 			Serial.print("|");
	Serial.print(0); 			Serial.print("|");
	Serial.print(vel_min);  	Serial.print("|");
	Serial.print(vel_max);   	Serial.print("|");
	Serial.print(TMP_calib); 	Serial.print("|");
	Serial.print(Controlador.get_estado_mapa() ? "Seguindo" : "Nao seguindo"); 	Serial.print("|");
	Serial.print(Str_mapa); 	Serial.print("|");
	
	Serial.println(
		"\nFrontal: " + (String)MAX_PRETO +
		" - Chegada: " + (String)MAX_PRETO_CHEGADA + 
		" - Mapa: " + (String)MAX_PRETO_MAPA); 
}
void Seguidor_de_Linha::send_erros(){
	Serial.println("E:" );//+ erros);
}

void Seguidor_de_Linha::set_CONSTS(String valores)
{	
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
	
	Controlador.set_const(dados[0], dados[1], dados[2]); //KP,KI,KD
	vel_min = dados[5];
	vel_max = dados[6];
	TMP_calib = dados[7];

	Controlador.set_estado_mapa(dados[8] == 1 ? true : false);
	Controlador.reset();
	delay(200);
	send_const();
}

void Seguidor_de_Linha::stop()
{
	set_direcao('B');
	modo = 'B';
	send_const();
	Controlador.reset();
	Estado_corrida = false;
}

void Seguidor_de_Linha::run()
{
	set_direcao('F');
	modo = 'N';
	tempo = 0;
	encru_time = 0;
	secao_time = 0;
	qnt_linhas = 2;
	Estado_corrida = true;
	Controlador.reset();
}


void Seguidor_de_Linha::teste_frontal(){
	double soma = 0;
	
	for(int i = 0; i < 6; i++){
        sns_frontais.leituras[i] = -1;
	}
	ler_sensores_sem_pausa = true;
	ADCSRA |= (1 << ADSC);
	delay(50);
	ler_sensores_sem_pausa = false;
	for (unsigned int i = 0; i < 6; i++)
	{
		soma += sns_frontais.leituras[i];
		Serial.println((String)i + " - " + (String)sns_frontais.leituras[i]);
	}

	Serial.print("Resultado teste frontal: ");
	Serial.println(soma/6);
	Serial.println("Max preto: " + (String)MAX_PRETO);
}
void Seguidor_de_Linha::teste_lateral(){
	Serial.println("Chegada: " + (String)sensor_chegada.ler());
	Serial.println("Mapa: " + (String)sensor_mapa.ler());
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

void Seguidor_de_Linha::comunica_serial()
{
	if (Serial.available())
	{
		ControlCMD(Serial.readStringUntil(';'));
	}
}