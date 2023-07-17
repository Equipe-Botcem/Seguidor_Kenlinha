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
	const char JOYSTICK = 'J';
	switch (command[0])
	{
	case SET:
		set_Consts(command);
		break;
	case STOP:
		stop("Controle");
		break;
	case RUN:
		run();
		break;
	case CALIBRACAO:
		if(command[1] != 'l')
			calibracao();
		break;
	case SEND_CONST:
		send_Consts();
		break;
	case TESTE_FRONTAL:
		teste_frontal();
		break;
	case TESTE_LATERAL:
		teste_lateral();
		break;	
	case JOYSTICK:
		joystick_control(command);
		break;
	default:
		Serial.println("Comando não reconhecido.");	
		break;
	}
}

void Seguidor_de_Linha::send_Consts(){

	float constsPID[3]; Controlador.get_const(constsPID);

	float Kp = constsPID[0], Ki = constsPID[1], Kd = constsPID[2];
	
	int vels[2]; Controlador.get_vel(vels);

	int vel_max = vels[0];
	int vel_min = vels[1];
	/*String Str_mapa = "";
	int mapa[40] = {0}; Controlador.get_mapa(mapa);
	for (int i =0; i < 40; i++){
		Str_mapa += (String)mapa[i] + "+";
	}*/
	
	Serial.print("T: Constantes: ");
	Serial.print(Kp); 			Serial.print("|");
	Serial.print(Ki); 			Serial.print("|");
	Serial.print(Kd); 			Serial.print("|");
	Serial.print(vel_min);  	Serial.print("|");
	Serial.print(vel_max);   	Serial.print("|");
	Serial.print(TMP_calib); 	Serial.print("|");
	Serial.print(Controlador.get_estado_mapa() ? "Seguindo" : "Nao seguindo"); 	Serial.print("|");
	Serial.print("Mapa"); 	Serial.print("|");
	
	Serial.println(
		"\nFrontal: " + (String)MAX_PRETO +
		" - Chegada: " + (String)MAX_PRETO_CHEGADA + 
		" - Mapa: " + (String)MAX_PRETO_MAPA); 
}
void Seguidor_de_Linha::set_Consts(String valores)
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
	Controlador.set_vel(dados[4], dados[3]);
	TMP_calib = dados[5];

	Controlador.set_estado_mapa(dados[6] == 1 ? true : false);
	Controlador.reset();
	delay(200);
	send_Consts();
}

void Seguidor_de_Linha::stop(String agente)
{
	set_direcao('B');
	modo = 'B';
	ler_sensores_fast = true;
	ler_sensores_sem_pausa = false;
	send_Consts();
	Serial.println("Tempo: " + (String)(millis() - start_time));
	Serial.println("Agente: " + agente);
	Controlador.reset();
	Estado_corrida = false;
}

void Seguidor_de_Linha::run()
{
	//delay(4te000);
	ler_sensores_fast =true;
	ler_sensores_sem_pausa = true;
	ADCSRA |= (1 << ADSC);
	delay(10);
	ler_sensores_sem_pausa = false;

	set_direcao('F');
	modo = 'N';
	tempo = 0;
	tmp_fora_linha = 0;
	estado_s_chegada = 0;
	estado_s_mapa = 0;
	curva_time = 0;
	start_time = 0;
	
	curva_time = 0;
	qnt_linhas = 2;
	
	Controlador.reset();
	Estado_corrida = true;
	
}


void Seguidor_de_Linha::teste_frontal(){
	double soma = 0;
	
	ler_sensores_sem_pausa = true;
	ADCSRA |= (1 << ADSC);
	delay(50);
	ler_sensores_sem_pausa = false;
	int n_sensores = sns_frontais.get_N_sns();
	for (int i = 0; i < n_sensores; i++)
	{
		soma += sns_frontais.sensores[i].get_ult_leitura();
		Serial.println((String)i + " - " + (String)sns_frontais.sensores[i].get_ult_leitura());
	}

	Serial.print("Resultado teste frontal: ");
	Serial.println(soma/n_sensores);
	Serial.println("Angulo: " + (String)sns_frontais.erro_analogico());
}
void Seguidor_de_Linha::teste_lateral(){
	ler_sensores_sem_pausa = true;
	ADCSRA |= (1 << ADSC);
	delay(50);
	ler_sensores_sem_pausa = false;
	Serial.println("Chegada: " + (String)sensor_chegada.get_ult_leitura());
	Serial.println("Mapa: " + (String)sensor_mapa.get_ult_leitura());
}
