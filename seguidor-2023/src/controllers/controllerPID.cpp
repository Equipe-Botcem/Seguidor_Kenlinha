#include "seguidor.h"
double Seguidor_de_Linha::calc_erro2()
{

	//double soma = 0;
	double erro_calc = 0;

	sns_frontais.ler_todos();
	identifica_perda_caminho();
	sns_frontais.ler_todos();
	sensor_encruzilhada_condition();
	sensor_stop_condition();

	
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