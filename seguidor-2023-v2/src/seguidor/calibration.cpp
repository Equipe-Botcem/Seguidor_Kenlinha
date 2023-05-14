#include "seguidor.hpp"

void Seguidor_de_Linha::sensor_calib(int pos){

	//vel_calib = 160;
	// pos = 0: sensores frontais em qualquer lugar na frente da linha branca
	// pos = 1: sensores laterais
	if (pos == 0){
		sns_frontais.reset();
		for (unsigned j = 0; j <= 10000; j++);

		set_direcao('T');
		set_velocidade(vel_calib,vel_calib);
		while (!sns_frontais.comp_max_value(2 * sns_frontais.get_min_media()));

		set_direcao('B');
		set_velocidade(0,0);

		for (unsigned j = 0; j <= 10000; j++);

		for( int j = 0; j < sns_frontais.get_N_sns(); j++){
			sns_frontais.limites[j] = sns_frontais.sensores[j].calc_limite();
		}
		MAX_PRETO = (sns_frontais.get_max_media() - sns_frontais.get_min_media())/2 + sns_frontais.get_min_media();
		set_direcao('F');
		Serial.println("FIM");
	
	}else if(pos == 1){
		sensor_chegada.reset();
		sensor_mapa.reset();
		set_direcao('F');
		set_velocidade(vel_calib,vel_calib);
		while(sensor_chegada.get_max_leitura() < 2 * sensor_chegada.get_min_leitura());
		while(sensor_mapa.get_max_leitura() < 2 * sensor_mapa.get_min_leitura());
		set_direcao('B');
		set_velocidade(0,0);
		
		for (unsigned int i = 0; i < 10000; i++);

		MAX_PRETO_CHEGADA = sensor_chegada.calc_limite();
		MAX_PRETO_MAPA = sensor_mapa.calc_limite();

	}
}
void Seguidor_de_Linha::calibracao()
{
	//iniciando interrupcao sem pausa
	ler_sensores_sem_pausa = true;
	ADCSRA |= (1 << ADSC);

	Serial.println("Calibrando sensores frontais");
	sensor_calib(0);

	Serial.println("Calibrando sensor lateral");
	sensor_calib(1);

	Serial.println("Frontal: " + (String)MAX_PRETO +
		" | Chegada: " + (String)MAX_PRETO_CHEGADA + 
		" | Mapa: " + (String)MAX_PRETO_MAPA);
	
	set_direcao('E');
	set_velocidade(vel_calib,vel_calib);

	while (sns_frontais.leituras[2] < sns_frontais.limites[2]){}
	set_direcao('B');
	set_velocidade(0,0);
	set_direcao('F');
	ler_sensores_sem_pausa = false;

}