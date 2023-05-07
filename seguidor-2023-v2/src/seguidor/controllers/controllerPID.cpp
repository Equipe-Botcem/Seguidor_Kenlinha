#include "seguidor.hpp"
double Seguidor_de_Linha::calc_erro_digital()
{
	double erro = 0;

	int s0 = sns_frontais.leituras[0] > MAX_PRETO ? 3:0;
	int s1 = sns_frontais.leituras[1] > MAX_PRETO ? 11:0;
	int s2 = sns_frontais.leituras[2] > MAX_PRETO ? 23:0;
	int s3 = sns_frontais.leituras[3] > MAX_PRETO ? 31:0;
	int s4 = sns_frontais.leituras[4] > MAX_PRETO ? 43:0;
	int s5 = sns_frontais.leituras[5] > MAX_PRETO ? 59:0;
	int pos = s0 + s1 + s2 + s3 + s4 + s5;
	erro = pesos[pos];

	if(pos == 0 && erro_antigo != 0) erro *= abs(erro_antigo)/erro_antigo;

	//convergencia para retas
	if(erro_antigo_nao_nulo*erro < 0){
		cont_alternancia++;
		if(cont_alternancia > 2){
			cont_alternancia = 0;
			fator_convergencia *=2;
			erro_alternado = erro;
		}
	}
	else{
		cont_alternancia = 0;
	}

	if(abs(erro) > abs(erro_alternado)){
		fator_convergencia = 1;
		cont_alternancia = 0;
	}
	if(erro != 0) erro_antigo_nao_nulo = erro;

	return erro/fator_convergencia;
}

double Seguidor_de_Linha::PID_calc(double erro)
{
	double _kp = Kp, _kd = Kd,_ki = Ki;
	if(seguir_mapa){
		_kp = LKp;
		_ki = LKi;
		_kd = LKd;

	}
	//Proporcional
	erro_P = erro;

	//Integral
	erro_I += erro;
	if(abs(erro) == 0) erro_I = 0;
	if		(erro_I > 50) 	erro_I = 50;
	else if	(erro_I < -50) 	erro_I = -50;

	//Derivativo
	erro_D = erro - erro_antigo;
	erro_antigo = erro;

	double value = ((_kp * erro_P) + (_ki * erro_I / taxa_de_amostra) + (_kd * erro_D))* 100;

	//pid fisico
	/*if(fim_correcao <= millis() || (erro_sendo_corrigindo * erro < 0) || (abs(erro) > abs(erro_sendo_corrigindo))){

		double t_rotacao = erro * tempo_correcao_grau_1v * abs(value);
		fim_correcao =  t_rotacao + millis();
		meio_correcao = t_rotacao/2 + millis();
		erro_sendo_corrigindo = erro;

		correcao_atual = value;
	}
	else if(meio_correcao <= millis()){
		value = 0;
	}
	else value = correcao_atual;*/
	return value;
}

void Seguidor_de_Linha::set_velocidade_pid(double erro, double v_min)
{
	double PID = PID_calc(erro);
	if(abs(erro) >= 55) v_min = -250;
	//Serial.println(erro);
	if(PID >= 0){
		double vel_corrigida = vel_max - PID;
		if(vel_corrigida <= v_min) vel_corrigida = v_min;
		
		set_velocidade_fast((int)vel_corrigida, vel_max);
	}
	else{
		double vel_corrigida = vel_max + PID;
		if(vel_corrigida <= v_min) vel_corrigida = v_min;

		set_velocidade_fast(vel_max, (int)vel_corrigida);
	}
}

void Seguidor_de_Linha::reset_pid(){
	erro_antigo = 0;
	erro_I = 0;
	erro_P = 0;
	erro_D = 0;
}