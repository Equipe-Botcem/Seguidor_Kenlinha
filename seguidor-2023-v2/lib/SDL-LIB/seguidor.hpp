
#include "Arduino.h"
#include <motor.hpp>
#include <sensor.hpp>
#include <sensores_frontais.hpp>
#include <controlador_PID.hpp>
#include "SimpleKalmanFilter.h"
#ifndef SDL_H
#define SDL_H
class Seguidor_de_Linha
{
    private:
        controlador_PID Controlador;
        motor motor_dir;
        motor motor_esq;

        SimpleKalmanFilter Kalman = SimpleKalmanFilter(0.008,0.008,0.015);

        unsigned long start_time = 0;
        char modo = 'N';

        //Parametros de calibracao
        float MAX_PRETO_CHEGADA = 100;
        float MAX_PRETO_MAPA = 100;
        float MAX_PRETO = 100;
        int vel_calib = 130;

        int estado_s_chegada = 0;
        int estado_s_mapa = 0;
        //-------------------//
        unsigned long tmp_fora_linha = 0;
        int qnt_linhas = 2;
        unsigned long curva_time = 0;
        char direcao_atual = 'F';
        char lado_pista = 'D';

        int pinos[14] = {A1,A2,A3,A4,A0,A5, //Sensores frontais
                        A6, A7, 6, 9, 10, 5, 8, 7};
                        
    public:
        
        sensores_frontais sns_frontais;
        sensor sensor_chegada;
        sensor sensor_mapa;
        bool ler_sensores_sem_pausa = false;
        bool ler_sensores_fast = false;

        unsigned long TMP_calib = 15;
        unsigned long tempo = TMP_calib;
        bool Estado_corrida = false;

        Seguidor_de_Linha();

        char get_modo();
        int getpin(int pin);
        float seguir_linha();
        float seguir_linha_final();

        //Checks
        void checar_chegada();
        void checar_secao();

        //comunicacao
        void ControlCMD(String command);
        void run();
        void stop(String agente = "desconhecido");
        
        void send_Consts();
        void set_Consts(String valores);

        void teste_frontal();
        void teste_lateral();
        
        //calibracao
        void calibracao();
        int sensor_calib(int pos);
        
        //joystick
        void joystick_control(String cmd);

        void set_direcao(char direcao);
        void set_velocidade(int vel_dir, int vel_esq);
        void set_velocidade_fast(int vel_dir, int vel_esq);

 };
 #endif
