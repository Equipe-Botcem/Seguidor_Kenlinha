#include <controlador_PID.hpp>

// Corrigir a trajetória do robô de acordo com o erro calculado
void controlador_PID::corrigir_trajeto(float erro, motor * m_dir, motor * m_esq)
{
    // Atualiza o maior erro registrado durante uma curva
    if(abs(erro) > abs(maior_erro_curva)) maior_erro_curva = erro;

    // Controla a progressão entre seções da pista
    if((abs(erro) > 5) && (estado == false)) prox_secao(); // Se o erro for maior que 5 e ainda não estiver na próxima seção, avance para a próxima seção
    else if((abs(erro) < 0.5)  && (estado == true) && (abs(maior_erro_curva) >= 3)) prox_secao(); // Se o erro for menor que 0.5, já estiver na próxima seção e o maior erro na curva for maior ou igual a 3, avance para a próxima seção

    // Atualiza o valor de erro_perda dependendo do erro atual e anterior
    if(abs(erro_antigo) != 30 && (abs(erro) == 30)){
        erro_perda = erro_antigo;
    }
    else if(abs(erro_antigo) == 30 && (abs(erro) != 30)){
        erro_perda = 0;
    }

    // Obtém a correção a ser feita com base no erro
    float PID = get_correcao(erro);

    // Ajusta a velocidade máxima dependendo do tempo que passou desde a última seção reta
    int v_max = vel_max;

    // Aplica a correção às velocidades dos motores
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

// Retorna o estado da seção atual
bool controlador_PID::get_estado_secao(){
    return estado;
}

// Retorna o valor de K
int controlador_PID::get_K(){
    return K;
}

// Corrige a trajetória do robô sem mover o robô
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

// Calcula a correção a ser feita com base no erro
float controlador_PID::get_correcao(float erro, bool att){
    float _kp = Kp, _kd = Kd,_ki = Ki;

    // Calcula a diferença de tempo desde a última chamada
    float d_tempo = (micros() - tmp_passado) /1000.00;
    if(d_tempo == 0) d_tempo = 1;
    tmp_passado = micros();

    // Atualiza o erro integral
    erro_I += erro - erro_antigo;
    if((erro_I <= 0 && (erro - erro_antigo >= 0)) || (erro_I >= 0 && (erro - erro_antigo <= 0)))// || (abs(erro) < 0.1))
		erro_I = 0;

    // Calcula a correção usando o controlador PID
	float correcao = _kp * erro + _ki * erro_I * d_tempo + _kd * (erro - erro_antigo)/d_tempo;
	if(att) erro_antigo = erro;
	
    // Limita a correção para um valor máximo de 510
	if(abs(correcao) > 510) correcao = 510 * (correcao > 0? 1:-1);
    
	return correcao;
}

// Reinicia o controlador PID
void controlador_PID::reset(){
    tmp_passado = micros();
    secao_atual = 0;
	erro_antigo = 0;
    erro_P = 0;
	erro_I = 0;
	erro_D = 0;
    estado = false;
}

// Define as velocidades máxima e mínima
void controlador_PID::set_vel(int v_max, int v_min){
    vel_max = v_max;
	vel_min = v_min;
}

// Obtém as velocidades máxima e mínima
void controlador_PID::get_vel(int * vetor){
    vetor[0] = vel_max;
	vetor[1] = vel_min;
}

// Define as constantes do controlador PID
void controlador_PID::set_const(float _kp, float _ki, float _kd, float _k){
    Kp = _kp;   Ki = _ki;   Kd = _kd;   K= _k;
}

// Define as constantes do controlador PID para a trajetória lateral
void controlador_PID::set_const_L(float _kp, float _ki, float _kd){
    LKp = _kp;   LKi = _ki;   LKd = _kd;
}

// Define o mapa da pista
void controlador_PID::set_mapa(int * _mapa, int tam){
    for(int i = 0; i < T_mapa; i++){
        mapa[i] = 0;
    }
    for(int i = 0; i < tam; i++){
        mapa[i] = _mapa[i];
    }
}

// Define se o robô deve seguir o mapa ou não
void controlador_PID::set_estado_mapa(bool _bool){
    seguir_mapa = _bool;
}

// Define a seção atual
void controlador_PID::set_secao(int secao){
    secao_atual = secao;
}

// Avança para a próxima seção
void controlador_PID::prox_secao(){
    secao_atual++;
    
    if(!estado){
        tmp_last_curva = micros();
        maior_erro_curva = erro_antigo;
    }
    else {
        tmp_last_reta = micros();
        maior_erro_curva = 0;
    }
    estado = !estado;
}

// Obtém as constantes do controlador PID
void controlador_PID::get_const(float * consts){
    consts[0] = Kp;
    consts[1] = Ki;
    consts[2] = Kd;
    consts[3] = K;
}

// Obtém as constantes do controlador PID para a trajetória lateral
void controlador_PID::get_const_L(float * consts){
    consts[0] = LKp;
    consts[1] = LKi;
    consts[2] = LKd;
}

// Obtém o mapa da pista
void controlador_PID::get_mapa(int * _mapa){
    for(int i = 0; i < T_mapa; i++){
        _mapa[i] = mapa[i];
    }
}

// Retorna se o robô está seguindo o mapa ou não
bool controlador_PID::get_estado_mapa(){
    return seguir_mapa;
}

// Retorna a seção atual
int controlador_PID::get_secao(){
    return secao_atual;
}

// Retorna o controle da seção atual
int controlador_PID::get_controle_secao(){
    return mapa[secao_atual];
}

// Retorna o último erro registrado
float controlador_PID::get_erro_antigo(){
    return erro_antigo;
}

// Encontra a linha com base no erro
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
