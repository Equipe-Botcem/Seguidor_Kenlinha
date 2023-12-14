#include <controlador_PID.hpp>
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string>

using namespace std;

controlador_PID::controlador_PID(){
    return;
}
void controlador_PID::loadMap(){
    
    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);

    // Read data, key - "data", value - "read_data"
    size_t required_size = 0;
    nvs_get_str(my_handle, "mapa", NULL, &required_size);

    if (required_size == 0){
        printf("Nao ha mapa salvo\n");
        nvs_close(my_handle);
        return;
    }
    char *map = (char*)malloc(required_size);
    nvs_get_str(my_handle, "mapa", map, &required_size);
    
    int sec = 0;
    string num = "";
    for (int i =0; i < (int)required_size; i++){
        if(map[i] == ','){
            mapa[sec] = stoi(num);
            num = "";
            sec++;
        }
        else num += map[i];

        if(sec >= 40) break;
    }

    for (int i =sec; i < 40; i++){
        mapa[i] = 0;
    }

    
    nvs_close(my_handle);
    free(map);
    return;
    
}
void controlador_PID::saveMap(){
    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);

    string m = "";
    for (int i =0; i < 40; i++){
        m += to_string(mapa[i]) + ",";
    }
    nvs_set_str(my_handle, "mapa", m.c_str());
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

void controlador_PID::corrigir_trajeto(float erro, motor * m_dir, motor * m_esq)
{
/*
    if (start_time == 0) start_time = esp_timer_get_time()/1000;
    if(abs(erro) > abs(maior_erro_curva)) maior_erro_curva = erro;
    if((abs(erro) > 3) && (estado == false)) prox_secao();
    else if((abs(erro) < 0.5)  && (estado == true) && (abs(maior_erro_curva) >= 3)) prox_secao();
*/

    if(abs(erro_antigo) != 30 && (abs(erro) == 30)){
        erro_perda = erro_antigo;
    }
    else if(abs(erro_antigo) == 30 && (abs(erro) != 30)){
        erro_perda = 0;
    }

    int v_max = vel_max;
    int v_min = vel_min;
    /*if (esp_timer_get_time()/1000 - start_time < 1000) {
        v_max = 5000;
        v_min = -5000;
    }*/

    //mapeamento
    if(seguir_mapa){
        if (mapa[cont_local] + mapa[cont_local +1] <= m_dir->getPosicao()){
            cont_local += 2;
            m_dir->resetEncoder();
        }
        else if (mapa[cont_local] - 34 <= m_dir->getPosicao() && (mapa[cont_local] != 0)){
            v_max = 2;
            //v_min = -3000;
        }
        
    }
    else{
        if(abs(m_dir->getPosicao() - m_esq->getPosicao()) >= 20 && (cont_local % 2 == 0)
            && (abs(m_dir->vel_real - m_esq->vel_real) > 0.2)){
            mapa[cont_local] = m_dir->getPosicao();
            m_dir->resetEncoder();
            m_esq->resetEncoder();
            cont_local++;
            tmp_curva = esp_timer_get_time();
        }
        else if(abs(m_dir->vel_real - m_esq->vel_real) <= 0.2 && (cont_local % 2 != 0)
                && (m_dir->getPosicao() > 40)){
            mapa[cont_local] = m_dir->getPosicao();
            m_dir->resetEncoder();
            m_esq->resetEncoder();
            cont_local++;
        }
    }
    /*printf("%i %i\n", v_max,v_min);
    printf("%i %i\n", cont_local,mapa[cont_local]);
    printf("%i\n", m_dir->getPosicao());*/

    float PID = get_correcao(erro)/8191 * v_max;
    //float PID = 0.288*erro;
    
    if(PID >= 0){
        float vel_corrigida = v_max - PID;
        if(vel_corrigida < v_min) vel_corrigida = v_min;
        
        (*m_dir).setVel(vel_corrigida);
        (*m_esq).setVel(v_max);
    }
    else{
        float vel_corrigida = v_max + PID;
        if(vel_corrigida < v_min) vel_corrigida = v_min;

        (*m_dir).setVel(v_max);
        (*m_esq).setVel(vel_corrigida);
    }
    /*
    float PID = get_correcao(erro)/8191 * v_max;
    
    
    if(PID >= 0){
        float vel_corrigida = v_max - PID;
        if(vel_corrigida < v_min) vel_corrigida = v_min;
        
        (*m_dir).set_velocidade((int)vel_corrigida);
        (*m_esq).set_velocidade(v_max);
    }
    else{
        float vel_corrigida = v_max + PID;
        if(vel_corrigida < v_min) vel_corrigida = v_min;

        (*m_dir).set_velocidade(v_max);
        (*m_esq).set_velocidade((int)vel_corrigida);
    }*/
    
}
bool controlador_PID::get_estado_secao(){
    return estado;
}
void controlador_PID::corrigir_trajeto_sem_mover(float erro, motor * m_dir, motor * m_esq){
    float PID = get_correcao(erro);
    if(PID >= 0){
        int vel_corrigida = 3840 - PID;
        if(vel_corrigida < vel_min) vel_corrigida = vel_min;
        (*m_dir).set_velocidade(vel_corrigida);
        (*m_esq).set_velocidade(3840);
    }
    else{
        int vel_corrigida = 3840 + PID;
        if(vel_corrigida < vel_min) vel_corrigida = vel_min;
        (*m_dir).set_velocidade(3840);
        (*m_esq).set_velocidade(vel_corrigida);
    }
}
float controlador_PID::get_correcao(float erro, bool att){
    float _kp = Kp, _kd = Kd,_ki = Ki;
    
    float d_tempo = (esp_timer_get_time() - tmp_passado) /1000.00;
    if(d_tempo == 0) d_tempo = 1;
    tmp_passado = esp_timer_get_time();

    //erro_I += erro - erro_antigo;
    erro_I += erro * d_tempo;
    if((erro_I <= 0 && (erro - erro_antigo >= 0)) || (erro_I >= 0 && (erro - erro_antigo <= 0)))// || (abs(erro) < 0.1))
		erro_I = 0;

	//float correcao = _kp * erro + _ki * erro_I * d_tempo + _kd * (erro - erro_antigo)/d_tempo;
    float correcao = _kp * erro + _ki * erro_I + _kd * (erro - erro_antigo)/d_tempo;
	if(att) erro_antigo = erro;
	
	if(abs(correcao) > 16382) correcao = 16382 * (correcao > 0? 1:-1);
    
	return correcao;
	
}

void controlador_PID::reset(){
    start_time = 0;
    tmp_passado = esp_timer_get_time();
    secao_atual = 0;
    cont_local = 0;
	erro_antigo = 0;
    erro_P = 0;
	erro_I = 0;
	erro_D = 0;
    estado = false;

    
    if(!seguir_mapa){
        set_mapa(NULL, 0);
    }
}

void controlador_PID::set_vel(int v_max, int v_min){
    vel_max = v_max;
	vel_min = v_min;
}

void controlador_PID::get_vel(int * vetor){
    vetor[0] = vel_max;
	vetor[1] = vel_min;
}

void controlador_PID::set_const(float _kp, float _ki, float _kd, float _k){
    Kp = _kp;   Ki = _ki;   Kd = _kd;   K= _k;
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
    
    if(!estado){
        tmp_last_curva = esp_timer_get_time();
        maior_erro_curva = erro_antigo;
    }
    else {
        tmp_last_reta = esp_timer_get_time();
        maior_erro_curva = 0;
    }
    estado = !estado;
}
void controlador_PID::get_const(float * consts){
    consts[0] = Kp;
    consts[1] = Ki;
    consts[2] = Kd;
    consts[3] = K;
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
    return cont_local;
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