#include "Arduino.h"
#include "motor.hpp"
#ifndef CTRLR_PID_H
#define CTRLR_PID_H
class controlador_PID
{
private:
    float Kp = 22;
    float Ki = 13;
    float Kd = 50;

    float erro_P = 0;
    float erro_I = 0;
    float erro_D = 0;

    float erro_antigo = 0;
    unsigned long tempo_acumulo = 0;

    //Mapeamento
    float LKp = 15;
    float LKi = 13;
    float LKd = 50;

    const static int T_mapa = 40;
    int secao_atual = 0;

    bool seguir_mapa = false;
    int mapa[T_mapa] = {0};
public:
    void corrigir_trajeto(float erro,int vel_max, int vel_min, motor * m_dir, motor * m_esq);
    void prox_secao();

    void set_const(float _kp, float _ki, float _kd);
    void set_const_L(float _kp, float _ki, float _kd);
    void set_mapa(int * _mapa, int tam);
    void set_estado_mapa(bool _bool);
    void set_secao(int secao);
    
    void get_const(float * consts);
    void get_const_L(float * consts);
    void get_mapa(int * _mapa);
    
    bool get_estado_mapa();
    int get_secao();
    int get_controle_secao();
    float get_erro_antigo();

    float get_correcao(float erro);

    void reset();
};
#endif
