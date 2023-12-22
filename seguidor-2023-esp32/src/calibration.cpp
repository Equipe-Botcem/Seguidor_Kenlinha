#include "seguidor.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_timer.h"
int Seguidor_de_Linha::sensor_calib(int pos){

	int vel_calib =3000; 
	// pos = 0: sensores frontais em qualquer lugar na frente da linha branca
	// pos = 1: sensores laterais
	if (pos == 0){
		sns_frontais.reset();
		vTaskDelay(50 / portTICK_PERIOD_MS);

		set_direcao('T');
		set_velocidade(vel_calib,vel_calib);
		long cont_tolerancia = esp_timer_get_time()/1000;
		while (!sns_frontais.comp_max_value(400) && (esp_timer_get_time()/1000 - cont_tolerancia < 1500)){
			vTaskDelay(1 / portTICK_PERIOD_MS);
		}
		
		set_direcao('B');
		set_velocidade(0,0);
		if((esp_timer_get_time()/1000 - cont_tolerancia) >= 1500){
			stop("Timeout: Calibração Sensores frontais");
			return 0;
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);

		for( int j = 0; j < sns_frontais.get_N_sns(); j++){
			sns_frontais.limites[j] = sns_frontais.sensores[j].calc_limite(2);
		}
		
		set_direcao('F');
		printf("FIM");
	
	}else if(pos == 1){
		sensor_chegada.reset();
		//sensor_mapa.reset();

		vTaskDelay(50 / portTICK_PERIOD_MS);
		set_direcao('F');
		set_velocidade(vel_calib,vel_calib);
		long cont_tolerancia = esp_timer_get_time()/1000;
		while(sensor_chegada.get_max_leitura() < 1500 && (esp_timer_get_time()/1000 - cont_tolerancia < 1500)){
			vTaskDelay(1 / portTICK_PERIOD_MS);
		}
		if((esp_timer_get_time()/1000 - cont_tolerancia) >= 1500){
			stop("Timeout: Calibração Sensor chegada");
			return 0;
		}
		/*while(sensor_mapa.get_max_leitura() < 2 * sensor_mapa.get_min_leitura()){
			vTaskDelay(1 / portTICK_PERIOD_MS);
		}*/
		set_direcao('B');
		set_velocidade(0,0);
		
		vTaskDelay(50 / portTICK_PERIOD_MS);

		MAX_PRETO_CHEGADA = sensor_chegada.calc_limite(2.2);
		MAX_PRETO_MAPA = sensor_mapa.calc_limite(2.2);

	}
	return 1;
}
void Seguidor_de_Linha::calibracao()
{
	int vel_calib =3000; 
	//iniciando interrupcao sem pausa
	ler_sensores_sem_pausa = true;
	ler_sensores_fast = false;

	troca_max_min = true;
	output("Calibrando sensores frontais");
	if(sensor_calib(0) == 0) return;

	output("Calibrando sensor lateral");
	if(sensor_calib(1) == 0) return;

	
	output(("Chegada: " + to_string(MAX_PRETO_CHEGADA) + 
		" | Mapa: " + to_string(MAX_PRETO_MAPA)).c_str());

	set_direcao(lado_pista);
	set_velocidade(vel_calib,vel_calib);
	int Ref_sensor = (lado_pista == 'E' ?  0 : sns_frontais.get_N_sns() -1);
	long cont_tolerancia = esp_timer_get_time()/1000;
	while (sns_frontais.sensores[Ref_sensor].get_ult_leitura() < sns_frontais.limites[Ref_sensor] &&
	(esp_timer_get_time()/1000 - cont_tolerancia < 1500)){vTaskDelay(1 / portTICK_PERIOD_MS);}
	set_direcao('F');
	long tmp = esp_timer_get_time()/1000;
	ler_sensores_fast =true;
	while (esp_timer_get_time()/1000 - tmp < 150){ seguir_linha_final();}
	
	set_direcao('B');
	set_velocidade(0,0);
	set_direcao('F');
	troca_max_min=false;
	ler_sensores_fast =false;
	ler_sensores_sem_pausa = false;

}