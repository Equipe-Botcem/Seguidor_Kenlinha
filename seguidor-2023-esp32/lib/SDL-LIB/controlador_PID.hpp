#include <string>
#include <stdio.h>
#include "motor.hpp"
#ifndef CTRLR_PID_H
#define CTRLR_PID_H
class controlador_PID
{
private:
    const int resolucao = 8191;
    float vel_max = 3;
    float vel_min = 0;

    float controle_2m[3] = {800,57,40000};
    float controle_3m[3] = {2359,57,120000};
    float controle_4m[3] = {4720,57,240000};

    float K = 0.01;//3;
    float Kp = 800;//800;
    float Ki = 57;//2.6899;
    float Kd = 80000;//40000;

    unsigned long start_time = 0;

    unsigned long tmp_passado = 0;
    unsigned long tmp_ignorar = 0;

    float erro_P = 0;

    float erro_I = 0;
    float erro_antigo_I = 0;
    unsigned long tempo_acumulo = 10;
    unsigned long tempo_ult_atualizacao = 0;
    

    float erro_D = 0;
    float erro_antigo = 0;
    
    float corr_old = 0;

    //Mapeamento
    float LKp = 15;
    float LKi = 13;
    float LKd = 50;

    unsigned long tmp_last_curva = 0;
    unsigned long tmp_last_reta=0;
    const static int T_mapa = 40;
    float maior_erro_curva = 0;

    int cont_local = 0;
    long tmp_curva = 0;
 
    bool estado = false;
    float erro_perda = 0;
    int secao_atual = 0;
    bool seguir_mapa = false;
    int mapa[T_mapa] = {90,68,180,30};

public:
    controlador_PID();

    void corrigir_trajeto(float erro, motor * m_dir, motor * m_esq);
    void corrigir_trajeto_sem_mover(float erro, motor * m_dir, motor * m_esq);
    void corrigir_trajeto_vmax(float erro, motor * m_dir, motor * m_esq, int v_max);
    void prox_secao();
    void encontrar_linha(float erro, motor * m_dir, motor * m_esq, char lado);
    void loadMap();
    void saveMap();

    void set_vel(float v_max, float v_min);

    void set_const(float _kp, float _ki, float _kd, float _k);
    void set_const_L(float _kp, float _ki, float _kd);
    void set_mapa(int * _mapa, int tam);
    void set_estado_mapa(bool _bool);
    void set_secao(int secao);
    
    void get_const(float * consts);
    void get_const_L(float * consts);
    void get_mapa(int * _mapa);
    void get_vel(float * vetor);
    
    float getIntegral();
    bool get_estado_mapa();
    float get_erro_perda();
    int get_secao();
    bool get_estado_secao();
    int get_controle_secao();
    float get_erro_antigo();

    float get_correcao(float erro, bool att = true);

    void reset();
};
#endif