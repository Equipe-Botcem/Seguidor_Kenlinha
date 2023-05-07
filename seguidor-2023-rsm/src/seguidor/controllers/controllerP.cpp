#include "seguidor.h"
double Seguidor_de_Linha::calc_erro()
{

	//double soma = 0;
	double erro_calc = 0;

	sns_frontais.ler_todos();

	checar_perda_caminho();
	sns_frontais.ler_todos();
	checar_encruzilhada();
	//sensor_curva_condition(1023 - analogRead(A7));
	checar_chegada();

	double pesos[6] = {-8, -3, -1, 1, 3, 8};
	int valor_calibrado;
	
	for (unsigned int i = 0; i < 6; i++)
	{
		valor_calibrado = calibracao_frontal(sns_frontais.leituras[i], sns_frontais.max_leituras[i], sns_frontais.min_leituras[i]);
		erro_calc += valor_calibrado * pesos[i];

	}
	return erro_calc;
}
double Seguidor_de_Linha::calibracao_frontal(double valor_descalibrado, double valLeitura_max, double valLeitura_min)
{
	double c = 0;

	c = (valor_descalibrado - valLeitura_min) * (1023 / (valLeitura_max - valLeitura_min));

	if(c>1023) c=1023;
	if(c<0)	c=0;

	return c;
}
double Seguidor_de_Linha::P_calc(double erro)
{
	double value = 0;
	P = erro;
	// I = I + erro;
	//  D = erro_antigo - erro;
	//  erro_antigo = erro;

	value = (Kp * P);
	// double valor = Kp * P + Ki * I + Kd * D;
	return value;
}
double Seguidor_de_Linha::Vel_direcional(double erro)
{	
	double value = (vel_max - vel_base) - K * abs(erro); 
	if (value <= 0) return 0;
	return value;
}

double Seguidor_de_Linha::motor_controler_esq(double erro)
{
	vel_motor_esquerdo = vel_base + Vel_direcional(erro) - P_calc(erro);
	return motor_check(vel_motor_esquerdo);
}

double Seguidor_de_Linha::motor_controler_dir(double erro)
{
	vel_motor_direito = vel_base + Vel_direcional(erro) + P_calc(erro);
	return motor_check(vel_motor_direito);
}

void Seguidor_de_Linha::motor_controler(double erro)
{
	set_velocidade(motor_controler_dir(erro), motor_controler_esq(erro));
}

int Seguidor_de_Linha::motor_check(double vel_motor)
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