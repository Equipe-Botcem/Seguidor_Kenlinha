#include "seguidor.h"

void Seguidor_de_Linha::motor_calibration(int dir){

	vel_calib = 160;
	// dir = 0: sensores frontais em qualquer lugar na frente da linha branca
	// dir = 1: sensores frontais comecando na linha branca
	// dir = 2: sensor lateral
	if (dir == 0){

		for (unsigned j = 0; j <= 100; j++) sns_frontais.ler_todos();

		set_direcao('T');
		set_velocidade(vel_calib,vel_calib);
		while (!sns_frontais.comp_max_value(2 * sns_frontais.get_min_media())){
			sns_frontais.ler_todos(); 
		}

		set_direcao('B');
		set_velocidade(0,0);

		for (unsigned int i = 0; i < 6; i++)
		{
			Serial.println(sns_frontais.leituras[i]);
		}

		for (unsigned j = 0; j <= 100; j++) sns_frontais.ler_todos();

		MAX_PRETO = (sns_frontais.get_max_media() - sns_frontais.get_min_media())/2 + sns_frontais.get_min_media();
		set_direcao('F');

		Serial.println("FIM");
	
	}else if (dir == 1){

		for (unsigned j = 0; j <= 1000; j++) sns_frontais.ler_todos();

		set_direcao('T');
		set_velocidade(150,150);
		delay(150);
		set_direcao('B');
		set_velocidade(0,0);

		for (unsigned j = 0; j <= 1000; j++) sns_frontais.ler_todos();

		MAX_PRETO = sns_frontais.get_max_media()/2;
		set_direcao('F');
	
	}else if(dir == 2){
		set_velocidade(vel_calib,vel_calib);
		while(sensor_chegada.get_max_leitura() < 2 * sensor_chegada.get_min_leitura())
		{
			sensor_chegada.ler();
		}
		
		set_direcao('B');
		set_velocidade(0,0);
		
		for (unsigned int i = 0; i < 200; i++){
			sensor_chegada.ler();
		}

		MAX_PRETO_CHEGADA = (sensor_chegada.get_max_leitura() - sensor_chegada.get_min_leitura())/2 + sensor_chegada.get_min_leitura();
		MAX_PRETO_MAPA = (sensor_mapa.get_max_leitura() - sensor_mapa.get_min_leitura())/2 + sensor_mapa.get_min_leitura();

	}
}
void Seguidor_de_Linha::calibracao()
{
	sns_frontais.reset();
	sensor_chegada.reset();
	sensor_mapa.reset();
	Serial.println("Calibrando sensores frontais");
	motor_calibration(0);

	Serial.println("Calibrando sensor lateral");
	motor_calibration(2);

	Serial.println("Frontal: " + (String)MAX_PRETO +
		" | Chegada: " + (String)MAX_PRETO_CHEGADA + 
		" | Mapa: " + (String)MAX_PRETO_MAPA);
	
	set_direcao('E');
	set_velocidade(vel_calib,vel_calib);

	while (sns_frontais.sensor4.ler() < MAX_PRETO && sns_frontais.sensor3.ler() < MAX_PRETO){}
	set_direcao('B');
	set_velocidade(0,0);
	set_direcao('F');

	/*Serial.println("_Sensor 0: " + (String)sns_frontais.sensor0.ler());
	Serial.println("_Sensor 1: " + (String)sns_frontais.sensor1.ler());
	Serial.println("_Sensor 2: " + (String)sns_frontais.sensor2.ler());
	Serial.println("_Sensor 3: " + (String)sns_frontais.sensor3.ler());
	Serial.println("_Sensor 4: " + (String)sns_frontais.sensor4.ler());
	Serial.println("_Sensor 5: " + (String)sns_frontais.sensor5.ler());*/
	// + "Frontal: " + (String)MAX_PRETO + " | Lateral: " + (String)MAX_PRETO_LATERAL);

}