#include "seguidor.h"
double Seguidor_de_Linha::calc_erro_digital()
{
	double erro = 0;

	int s0 = sns_frontais.leituras[0] > MAX_PRETO ? 3:0;
	int s1 = sns_frontais.leituras[1] > MAX_PRETO ? 11:0;
	int s2 = sns_frontais.leituras[2] > MAX_PRETO ? 23:0;
	int s3 = sns_frontais.leituras[3] > MAX_PRETO ? 31:0;
	int s4 = sns_frontais.leituras[4] > MAX_PRETO ? 43:0;
	int s5 = sns_frontais.leituras[5] > MAX_PRETO ? 59:0;
	double pesos[171] = {	
		55, //0
		0, //1
		0, //2
		-11.39, //3
		0, //4
		0, //5
		0, //6
		0, //7
		0, //8
		0, //9
		0, //10
		0, //11
		0, //12
		0, //13
		-7.95, //14
		0, //15
		0, //16
		0, //17
		0, //18
		0, //19
		0, //20
		0, //21
		0, //22
		0, //23
		0, //24
		0, //25
		0, //26
		0, //27
		0, //28
		0, //29
		0, //30
		0, //31
		0, //32
		0, //33
		-2.88, //34
		0, //35
		0, //36
		-5.12, //37
		0, //38
		0, //39
		0, //40
		0, //41
		0, //42
		0, //43
		0, //44
		-7.95, //45
		0, //46
		0, //47
		0, //48
		0, //49
		0, //50
		0, //51
		0, //52
		0, //53
		0, //54
		0, //55
		0, //56
		-7.95, //57
		0, //58
		11.39, //59
		0, //60
		0, //61
		0, //62
		0, //63
		0, //64
		-0.74, //65
		0, //66
		0, //67
		-10, //68
		0, //69
		0, //70
		0, //71
		0, //72
		-7.95, //73
		2.88, //74
		0, //75
		0, //76
		-2.88, //77
		0, //78
		0, //79
		-5.12, //80
		0, //81
		0, //82
		0, //83
		0, //84
		2.88, //85
		0, //86
		0, //87
		0, //88
		0, //89
		0, //90
		0, //91
		0, //92
		-2.88, //93
		0, //94
		0, //95
		-5.12, //96
		0.74, //97
		0, //98
		0, //99
		0.74, //100
		0, //101
		7.95, //102
		0, //103
		0, //104
		7.95, //105
		0, //106
		0, //107
		0, //108
		0, //109
		0, //110
		-9, //111
		0, //112
		7.95, //113
		0, //114
		0, //115
		0, //116
		0, //117
		0, //118
		0, //119
		0, //120
		0, //121
		0, //122
		0, //123
		-0.74, //124
		7.95, //125
		0, //126
		-10, //127
		0, //128
		0, //129
		0, //130
		0, //131
		0, //132
		5.12, //133
		0, //134
		0, //135
		5.12, //136
		0, //137
		0, //138
		-5.12, //139
		0, //140
		0, //141
		0, //142
		0, //143
		5.12, //144
		0, //145
		0, //146
		5.12, //147
		0, //148
		0, //149
		0, //150
		0, //151
		0, //152
		0, //153
		0, //154
		0, //155
		10, //156
		0, //157
		0, //158
		10, //159
		0, //160
		0, //161
		0, //162
		0, //163
		0, //164
		0, //165
		0, //166
		9, //167
		0, //168
		0, //169
		0, //170
	};
	int pos = s0 + s1 + s2 + s3 + s4 + s5;
	erro = pesos[pos];

	if(pos == 0 && erro_antigo != 0) erro *= abs(erro_antigo)/erro_antigo;
	return erro;
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
	/*if(cont_KI == 2){
		erro_I += erro;
		cont_KI = 0;
	}
	cont_KI++;*/
	erro_I += erro;
	if(abs(erro) == 0) erro_I = 0;
	if		(erro_I > 50) 	erro_I = 50;
	else if	(erro_I < -50) 	erro_I = -50;

	//Derivativo
	erro_D = erro - erro_antigo;
	erro_antigo = erro;

	//double value = ((Kp * erro_P) + (Ki * erro_I) + (Kd * erro_D))* 100;

	//if(abs(value) <= 1) erro_I =0;
	return ((_kp * erro_P) + (_ki * erro_I) + (_kd * erro_D))* 100;
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