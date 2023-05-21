
#include "Arduino.h"
#include <motor.hpp>
#include <sensor.hpp>
#include <sensores_frontais.hpp>
#include <controlador_PID.hpp>
#ifndef SDL_H
#define SDL_H
class Seguidor_de_Linha
{
    private:
        controlador_PID Controlador;
        motor motor_dir;
        motor motor_esq;

        char modo = 'N';
        float vel_max = 255;
        float vel_min = -250;

        //Parametros de calibracao
        float MAX_PRETO_CHEGADA = 100;
        float MAX_PRETO_MAPA = 100;
        float MAX_PRETO = 100;
        int vel_calib = 130;
        //-------------------//
        int qnt_linhas = 2;
        unsigned long encru_time = 0;
        unsigned long secao_time = 0;
        char direcao_atual = 'F';

        int pinos[14] = {A6,A5,A3,A4,A2,A0,
                        A1, A7, 6, 10, 9, 5, 7, 8};
                        
    public:
        
        static sensores_frontais sns_frontais;
        static sensor sensor_chegada;
        static sensor sensor_mapa;
        static bool ler_sensores_sem_pausa;

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
        void stop();
        void comunica_serial();
        void send_const();
        void send_erros();
        void set_CONSTS(String valores);
        void teste_frontal();
        void teste_lateral();
        void teste_curva();
        
        //calibracao
        void calibracao();
        void sensor_calib(int pos);
        
        //joystick
        void joystick_control(String cmd);

        void set_direcao(char direcao);
        void set_velocidade(int vel_dir, int vel_esq);
        void set_velocidade_fast(int vel_dir, int vel_esq);

 };
 #endif
