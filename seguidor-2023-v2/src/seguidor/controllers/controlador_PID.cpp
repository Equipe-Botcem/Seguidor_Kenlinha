#include <controlador_PID.hpp>

void controlador_PID::corrigir_trajeto(float erro,int vel_max, int vel_min, motor * m_dir, motor * m_esq)
{
	float PID = get_correcao(erro);
	if(abs(erro) >= 30) vel_min = -250;
	if(PID >= 0){
		float vel_corrigida = vel_max - PID;
		if(vel_corrigida <= vel_min) vel_corrigida = vel_min;
		
        (*m_dir).set_velocidade_fast((int)vel_corrigida);
        (*m_esq).set_velocidade_fast(vel_max);
	}
	else{
		float vel_corrigida = vel_max + PID;
		if(vel_corrigida <= vel_min) vel_corrigida = vel_min;

		(*m_dir).set_velocidade_fast(vel_max);
        (*m_esq).set_velocidade_fast((int)vel_corrigida);
	}
}

float controlador_PID::get_correcao(float erro){
    float _kp = Kp, _kd = Kd,_ki = Ki;
	if(seguir_mapa && mapa[secao_atual]){
		_kp = LKp;
		_ki = LKi;
		_kd = LKd;
	}

	/*if(abs(erro - erro_antigo) > 5){
		erro_D = erro - erro_antigo;
	}
	else{
		erro_D = 0;
	}*/
	erro_D = erro - erro_antigo;
	if(abs(erro) <= 1){
		erro_P = 0;
		if(abs(erro) <= 0.03) erro_I = 0;
		else if(millis() - tempo_acumulo >= 100) {
			erro_I += erro;
			tempo_acumulo = millis();
			if		(erro_I > 50) 	erro_I = 50;
			else if	(erro_I < -50) 	erro_I = -50;
		}
		
	}
	else{
		erro_P = erro;
		erro_I = 0;
	}
	erro_antigo = erro;
	return ((_kp * erro_P) + (_ki * erro_I) + (_kd * erro_D));
}

void controlador_PID::reset(){
    secao_atual = 0;
	erro_antigo = 0;
    erro_P = 0;
	erro_I = 0;
	erro_D = 0;
}

void controlador_PID::set_const(float _kp, float _ki, float _kd){
    Kp = _kp;   Ki = _ki;   Kd = _kd;
}
void controlador_PID::set_const_L(float _kp, float _ki, float _kd){
    LKp = _kp;   LKi = _ki;   LKd = _kd;
}
void controlador_PID::set_mapa(int * _mapa, int tam){
    for(int i = 0; i < T_mapa; i++){
        mapa[i] = 0;
    }
    for(int i = 0; i < tam; i++){
        mapa[i] = _mapa[i];
    }
}
void controlador_PID::set_estado_mapa(bool _bool){
    seguir_mapa = _bool;
}
void controlador_PID::set_secao(int secao){
    secao_atual = secao;
}
void controlador_PID::prox_secao(){
    secao_atual++;
}
void controlador_PID::get_const(float * consts){
    consts[0] = Kp;
    consts[1] = Ki;
    consts[2] = Kd;
}
void controlador_PID::get_const_L(float * consts){
    consts[0] = LKp;
    consts[1] = LKi;
    consts[2] = LKd;
}
void controlador_PID::get_mapa(int * _mapa){
    for(int i = 0; i < T_mapa; i++){
        _mapa[i] = mapa[i];
    }
}

bool controlador_PID::get_estado_mapa(){
    return seguir_mapa;
}
int controlador_PID::get_secao(){
    return secao_atual;
}
int controlador_PID::get_controle_secao(){
    return mapa[secao_atual];
}
float controlador_PID::get_erro_antigo(){
    return erro_antigo;
}