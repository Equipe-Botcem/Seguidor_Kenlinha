#include <controlador_PID.hpp>

void controlador_PID::corrigir_trajeto(float erro, motor * m_dir, motor * m_esq)
{
	float PID = get_correcao(erro);
	int v_max = vel_max;
	//if(abs(erro) < 1) v_max = (1 - abs(erro)/2) * vel_max;
	if(PID >= 0){
		float vel_corrigida = v_max - PID;
		if(vel_corrigida < vel_min) vel_corrigida = vel_min;
		
        (*m_dir).set_velocidade_fast((int)vel_corrigida);
        (*m_esq).set_velocidade_fast(v_max);
	}
	else{
		float vel_corrigida = v_max + PID;
		if(vel_corrigida < vel_min) vel_corrigida = vel_min;

		(*m_dir).set_velocidade_fast(v_max);
        (*m_esq).set_velocidade_fast((int)vel_corrigida);
	}
}

float controlador_PID::get_correcao(float erro){
    float _kp = Kp, _kd = Kd,_ki = Ki;
	/*if(seguir_mapa && mapa[secao_atual]){
		_kp = LKp;
		_ki = LKi;
		_kd = LKd;
	}*/
	
	if(abs(erro) <= 1){
		

		//KI 1 grau
		
		if(abs(erro) <= 0.005 || erro*erro_I < 0) {
			erro_I = 0;
			erro_antigo_I = 0;
		}
		else if(millis() - tempo_ult_atualizacao >= 2) {
			float variacao_erro = abs(erro) - abs(erro_antigo_I);
			if((variacao_erro >= 0) || erro*erro_antigo_I < 0){
				erro_I += variacao_erro*(erro >= 0 ? 500:-500);
				tempo_ult_atualizacao = millis();
			}
			else if(variacao_erro <= (-0.015*abs(erro))){
				erro_I -= variacao_erro*(erro >= 0 ? 500:-500);
				tempo_ult_atualizacao = millis();
			}
			erro_antigo_I = erro;
		}
		
		
	}
	else{
		
		erro_P = erro * abs(erro)/3;
		erro_antigo_I = erro;
		erro_I = 0;
		erro_D = erro - erro_antigo;
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

void controlador_PID::set_vel(int v_max, int v_min){
    vel_max = v_max;
	vel_min = v_min;
}

void controlador_PID::get_vel(int * vetor){
    vetor[0] = vel_max;
	vetor[1] = vel_min;
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

/*KI 2 grau
		if(abs(erro) <= 0.1 || erro*erro_I < 0) {
			erro_I = 0;
			erro_antigo_I = 0;
		}
		else if(millis() - tempo_ult_atualizacao >= 2) {
			float variacao_erro = abs(erro) - abs(erro_antigo_I);
			if((variacao_erro >= 0) || erro*erro_antigo_I < 0){
				erro_I += variacao_erro*(erro >= 0 ? 500:-500);
				tempo_ult_atualizacao = millis();
			}
			else if(variacao_erro <= (-0.001)){
				erro_I -= variacao_erro*(erro >= 0 ? 500:-500);
				tempo_ult_atualizacao = millis();
			}
			erro_antigo_I = erro;
		}
		erro_P = 0;*/