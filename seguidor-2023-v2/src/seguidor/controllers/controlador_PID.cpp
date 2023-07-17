#include <controlador_PID.hpp>

void controlador_PID::corrigir_trajeto(float erro, motor * m_dir, motor * m_esq)
{
    
    
    if(abs(erro) > 31 && abs(erro_antigo) < abs(erro)){
        float PID = get_correcao(erro, false);
        if(PID >= 0){
            float vel_corrigida = -255 + PID;
		    if(vel_corrigida > vel_max) vel_corrigida = vel_max;
            (*m_dir).set_velocidade_fast(-255);
            (*m_esq).set_velocidade_fast(vel_corrigida);
        }
        else{
            float vel_corrigida = -255 - PID;
		    if(vel_corrigida > vel_max) vel_corrigida = vel_max;
            (*m_dir).set_velocidade_fast(vel_corrigida);
            (*m_esq).set_velocidade_fast(-255);
        }
    }
	else{
        float PID = get_correcao(erro);
        int v_max = vel_max;
        if(abs(erro) > 2 && (abs(erro) < 9)) v_max-= abs(erro) * 10;
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
}
void controlador_PID::corrigir_trajeto_sem_mover(float erro, motor * m_dir, motor * m_esq){
    float PID = get_correcao(erro);
    if(PID >= 0){
        int vel_corrigida = 120 - PID;
        if(vel_corrigida < vel_min) vel_corrigida = vel_min;
        (*m_dir).set_velocidade_fast(vel_corrigida);
        (*m_esq).set_velocidade_fast(120);
    }
    else{
        int vel_corrigida = 120 + PID;
        if(vel_corrigida < vel_min) vel_corrigida = vel_min;
        (*m_dir).set_velocidade_fast(120);
        (*m_esq).set_velocidade_fast(vel_corrigida);
    }
}
float controlador_PID::get_correcao(float erro, bool att){
    float _kp = Kp, _kd = Kd,_ki = Ki;
    
	/*if(seguir_mapa && mapa[secao_atual]){
		_kp = LKp;
		_ki = LKi;
		_kd = LKd;
	}*/
    /*if(abs(erro) < 0.04){
        erro_I = 0;
        erro_antigo = 0;
        return 0;
    }*/
    float d_tempo = (micros() - tmp_passado) /1000.00;
    tmp_passado = micros();

	float correcao = _kp * erro + _ki * erro_I * d_tempo + _kd * (erro - erro_antigo)/d_tempo;
	if(att) erro_antigo = erro;
	erro_I += erro;
	if((erro_I <= 0 && erro >= 0) || (erro_I >= 0 && erro <= 0))// || (abs(erro) < 0.1))
		erro_I = 0;

	if(abs(correcao) > 510) correcao = 510 * (correcao > 0? 1:-1);
    
	return correcao;
	
}

void controlador_PID::reset(){
    tmp_passado = micros();
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

void controlador_PID::encontrar_linha(float erro, motor * m_dir, motor * m_esq, char lado = 'D'){

    if(abs(erro) > 10) erro *= (lado == 'E' ? 1:-1);
    float PID = get_correcao(erro);
	if(PID < 0){
		float vel_corrigida = PID;
        (*m_dir).set_velocidade(vel_corrigida);
        (*m_esq).set_velocidade(0);
	}
	else{
		float vel_corrigida = -PID;
		(*m_dir).set_velocidade(0);
        (*m_esq).set_velocidade(vel_corrigida);
	}
}