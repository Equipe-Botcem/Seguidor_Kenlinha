#include "seguidor.hpp"

int Seguidor_de_Linha::sensor_calib(int pos){

	//vel_calib = 160;
	// pos = 0: sensores frontais em qualquer lugar na frente da linha branca
	// pos = 1: sensores laterais
	if (pos == 0){
		sns_frontais.reset();
		delay(50);

		set_direcao('T');
		set_velocidade(vel_calib,vel_calib);
		long cont_tolerancia = millis();
		while (!sns_frontais.comp_max_value(3 * sns_frontais.get_min_media()) && (millis() - cont_tolerancia < 1500)){
			delay(1);
		}
		
		set_direcao('B');
		set_velocidade(0,0);
		if((millis() - cont_tolerancia) >= 1500){
			stop("Timeout: Calibração Sensores frontais");
			return 0;
		}

		delay(50);

		for( int j = 0; j < sns_frontais.get_N_sns(); j++){
			sns_frontais.limites[j] = sns_frontais.sensores[j].calc_limite(2);
		}
		MAX_PRETO = (sns_frontais.get_max_media() - sns_frontais.get_min_media())/2 + sns_frontais.get_min_media();
		set_direcao('F');
		Serial.println("FIM");
	
	}else if(pos == 1){
		sensor_chegada.reset();
		//sensor_mapa.reset();

		delay(50);
		set_direcao('F');
		set_velocidade(vel_calib,vel_calib);
		long cont_tolerancia = millis();
		while(sensor_chegada.get_max_leitura() < 2 * sensor_chegada.get_min_leitura() && (millis() - cont_tolerancia < 1500)){
			delay(1);
		}
		if((millis() - cont_tolerancia) >= 1500){
			stop("Timeout: Calibração Sensor chegada");
			return 0;
		}
		/*while(sensor_mapa.get_max_leitura() < 2 * sensor_mapa.get_min_leitura()){
			delay(1);
		}*/
		set_direcao('B');
		set_velocidade(0,0);
		
		delay(50);

		MAX_PRETO_CHEGADA = sensor_chegada.calc_limite(2);
		//MAX_PRETO_MAPA = sensor_mapa.calc_limite();

	}
	return 1;
}
void Seguidor_de_Linha::calibracao()
{
	//iniciando interrupcao sem pausa
	ler_sensores_sem_pausa = true;
	ler_sensores_fast = false;
	ADCSRA |= (1 << ADSC);

	Serial.println("Calibrando sensores frontais");
	if(sensor_calib(0) == 0) return;

	Serial.println("Calibrando sensor lateral");
	if(sensor_calib(1) == 0) return;

	Serial.println("Frontal: " + (String)MAX_PRETO +
		" | Chegada: " + (String)MAX_PRETO_CHEGADA + 
		" | Mapa: " + (String)MAX_PRETO_MAPA);

	set_direcao(lado_pista);
	set_velocidade(vel_calib,vel_calib);
	int Ref_sensor = (lado_pista == 'E' ?  0 : sns_frontais.get_N_sns() -1);
	long cont_tolerancia = millis();
	while (sns_frontais.sensores[Ref_sensor].get_ult_leitura() < sns_frontais.limites[Ref_sensor] &&
	(millis() - cont_tolerancia < 1500)){delay(1);}
	set_direcao('F');
	long tmp = millis();
	ler_sensores_fast =true;
	while (millis() - tmp < 150){ seguir_linha_final();}
	
	set_direcao('B');
	set_velocidade(0,0);
	set_direcao('F');
	ler_sensores_fast =false;
	ler_sensores_sem_pausa = false;

}