#include "seguidor.hpp"

// Função para calibrar o sensor. Se pos = 0, calibra os sensores frontais. Se pos = 1, calibra os sensores laterais.
int Seguidor_de_Linha::sensor_calib(int pos){

    // Define a velocidade de calibração
	vel_calib = 100;

    // Calibração dos sensores frontais
	if (pos == 0){
		sns_frontais.reset(); // Reseta os sensores frontais
		delay(50);

		set_direcao('T'); // Define a direção do robô para 'T'
		set_velocidade(vel_calib,vel_calib); // Define a velocidade do robô para a velocidade de calibração

        // Aguarda até que os sensores frontais estejam corretamente calibrados
		long cont_tolerancia = millis();
		while (!sns_frontais.comp_max_value(8 * sns_frontais.get_min_media()) && (millis() - cont_tolerancia < 1500)){
			delay(1);
		}
		
		set_direcao('B'); // Define a direção do robô para 'B'
		set_velocidade(0,0); // Para o robô

        // Se a calibração dos sensores frontais falhou, para o robô e retorna 0
		if((millis() - cont_tolerancia) >= 1500){
			stop("Timeout: Calibração Sensores frontais");
			return 0;
		}

		delay(50);

        // Define os limites de leitura dos sensores frontais
		for( int j = 0; j < sns_frontais.get_N_sns(); j++){
			sns_frontais.limites[j] = sns_frontais.sensores[j].calc_limite(2);
		}

        // Define o valor de preto máximo para os sensores frontais
		MAX_PRETO = (sns_frontais.get_max_media() - sns_frontais.get_min_media())/2 + sns_frontais.get_min_media();

		set_direcao('F'); // Define a direção do robô para 'F'
		Serial.println("FIM"); // Imprime "FIM"
	
    // Calibração dos sensores laterais
	}else if(pos == 1){
		sensor_chegada.reset(); // Reseta o sensor de chegada
		//sensor_mapa.reset(); // Reseta o sensor de mapa

		delay(50);
		set_direcao('F'); // Define a direção do robô para 'F'
		set_velocidade(vel_calib,vel_calib); // Define a velocidade do robô para a velocidade de calibração

        // Aguarda até que os sensores laterais estejam corretamente calibrados
		long cont_tolerancia = millis();
		while(sensor_chegada.get_max_leitura() < 1.5 * sensor_chegada.get_min_leitura() && (millis() - cont_tolerancia < 1500)){
			delay(1);
		}

        // Se a calibração do sensor de chegada falhou, para o robô e retorna 0
		if((millis() - cont_tolerancia) >= 1500){
			stop("Timeout: Calibração Sensor chegada");
			return 0;
		}

        // Aguarda até que o sensor de mapa esteja corretamente calibrado
		while(sensor_mapa.get_max_leitura() < 2 * sensor_mapa.get_min_leitura()){
			delay(1);
		}

		set_direcao('B'); // Define a direção do robô para 'B'
		set_velocidade(0,0); // Para o robô
		
		delay(50);

        // Define o valor de preto máximo para o sensor de chegada e o sensor de mapa
		MAX_PRETO_CHEGADA = sensor_chegada.calc_limite(1.5);
		MAX_PRETO_MAPA = sensor_mapa.calc_limite(1.5);

	}
	return 1; // Retorna 1 se a calibração foi bem-sucedida
}

// Função para realizar a calibração do robô
void Seguidor_de_Linha::calibracao()
{
    // Inicia a interrupção sem pausa
	ler_sensores_sem_pausa = true;
	ler_sensores_fast = false;
	ADCSRA |= (1 << ADSC);

	Serial.println("Calibrando sensores frontais");
	if(sensor_calib(0) == 0) return; // Se a calibração dos sensores frontais falhou, retorna

	Serial.println("Calibrando sensor lateral");
	if(sensor_calib(1) == 0) return; // Se a calibração dos sensores laterais falhou, retorna

    // Imprime os valores de preto máximo para os sensores frontais, o sensor de chegada e o sensor de mapa
	Serial.println("Frontal: " + (String)MAX_PRETO +
		" | Chegada: " + (String)MAX_PRETO_CHEGADA + 
		" | Mapa: " + (String)MAX_PRETO_MAPA);

	set_direcao(lado_pista); // Define a direção do robô para o lado da pista
	set_velocidade(vel_calib,vel_calib); // Define a velocidade do robô para a velocidade de calibração

    // Define o sensor de referência baseado no lado da pista
	int Ref_sensor = (lado_pista == 'E' ?  0 : sns_frontais.get_N_sns() -1);

    // Aguarda até que o sensor de referência esteja corretamente posicionado
	long cont_tolerancia = millis();
	while (sns_frontais.sensores[Ref_sensor].get_ult_leitura() < sns_frontais.limites[Ref_sensor] &&
	(millis() - cont_tolerancia < 1500)){delay(1);}
	set_direcao('F'); // Define a direção do robô para 'F'
	long tmp = millis();
	ler_sensores_fast =true;
	while (millis() - tmp < 150){ seguir_linha_final();}
	
	set_direcao('B'); // Define a direção do robô para 'B'
	set_velocidade(0,0); // Para o robô
	set_direcao('F'); // Define a direção do robô para 'F'
	ler_sensores_fast =false; // Desativa a leitura rápida dos sensores
	ler_sensores_sem_pausa = false; // Desativa a leitura dos sensores sem pausa
}
