#include "Arduino.h"
#include "motor.hpp"
#ifndef CTRLR_PID_H
#define CTRLR_PID_H
class controlador_PID
{
private:
    int vel_max = 255;
    int vel_min = -255;
    float Kp = 25;
    float Ki = 0.0170968;
    float Kd = 1642;

    unsigned long tmp_passado = 0;

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

    const static int T_mapa = 40;
    int secao_atual = 0;
    bool seguir_mapa = false;
    int mapa[T_mapa] = {0};
public:
    void corrigir_trajeto(float erro, motor * m_dir, motor * m_esq);
    void corrigir_trajeto_sem_mover(float erro, motor * m_dir, motor * m_esq);
    void prox_secao();
    void encontrar_linha(float erro, motor * m_dir, motor * m_esq, char lado);

    void set_vel(int v_max, int v_min);

    void set_const(float _kp, float _ki, float _kd);
    void set_const_L(float _kp, float _ki, float _kd);
    void set_mapa(int * _mapa, int tam);
    void set_estado_mapa(bool _bool);
    void set_secao(int secao);
    
    void get_const(float * consts);
    void get_const_L(float * consts);
    void get_mapa(int * _mapa);
    void get_vel(int * vetor);
    
    bool get_estado_mapa();
    int get_secao();
    int get_controle_secao();
    float get_erro_antigo();

    float get_correcao(float erro, bool att = true);

    void reset();
};
#endif
