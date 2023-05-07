
#include "Arduino.h"
#include <motor.h>
#include <sensor.h>
#include <sensores_frontais.h>
#ifndef SDL_H
#define SDL_H
class Seguidor_de_Linha
{
    private:
        
        sensores_frontais sns_frontais;
        sensor sensor_chegada;
        sensor sensor_mapa;    
        
        //Const Linha
        double LKp = 0.120;
        double LKi = 0.010;
        double LKd = 0.200;

        //constantes para o PID
        double Kp = 0.220;
        double Ki = 0.130;
        double Kd = 0.500;
        double vel_max = 255;
        double vel_min = -50;
        int cont_KI = 0;

        //constantes para fisica do PID
        double tempos_medios[55] = {0};
        unsigned long tempo_ult_update = 0;
        
        
        //controlador P-K
        double P = 0;
        double K = 0.050;
        double vel_base =  100;
        double vel_motor_direito = 0;
        double vel_motor_esquerdo = 0;

        //controlador PID
        double erro_P = 0;
        double erro_I = 0;
        double erro_D = 0;
        //double erro_antigo = 0;

        //Parametros de calibracao
        
        double MAX_PRETO_CHEGADA = 100;
        double MAX_PRETO_MAPA = 100;
        double MAX_PRETO = 100;

        char direcao_atual = 'F';
        unsigned long start_time = 0;
        int qnt_linhas = 2;
        int ultimo_sensor_na_linha = 0;
        int vel_calib = 130;
        //-------------------//

        unsigned long tmp_saida_linha = 0;
                        
        int pinos[14] = {A5,A4,A3,A2,A1,A0,
                        A6, A7, 6, 10, 9, 5, 7, 8};

        unsigned long secao_time = 0;
        
        


    public:
        unsigned long TMP_calib = 15;
        //Mapeamento
        bool seguir_mapa = false;
        int mapa[40] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,0 ,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,1};
        //int mapa[40] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};

        double erro_antigo = 0;
        int secao_mapa = 0;
        unsigned long tempo = TMP_calib;
        motor motor_dir;
        motor motor_esq;
        char modo = 'N'; // N: Normal | C: Curva | J: Joystick | P: Parado
        bool Estado_corrida = false;

        Seguidor_de_Linha();
        void conf_pinos();
        void set_direcao(char direcao);
        void set_velocidade(int vel_dir, int vel_esq);
        void set_velocidade_fast(int vel_dir, int vel_esq);
        double seguir_linha();
        double seguir_linha_final();
        bool ignorar_leitura();
        bool ignorar_leitura_mapa();

        //Checks
        void checar_chegada();
        void checar_secao(); //mapeamento
        void checar_encruzilhada();
        void checar_perda_caminho();
        bool checar_fora_linha();

        //controlador P
        void motor_controler(double erro);
        double calibracao_frontal(double valor_descalibrado, double valLeitura_max, double valLeitura_min);
        double calc_erro();
        double P_calc(double erro);
        double Vel_direcional(double erro);
        double motor_controler_esq(double erro);
        double motor_controler_dir(double erro);
        int motor_check(double vel_motor);

        //controlador PID
        double calc_erro_digital();
        double PID_calc(double erro);
        void set_velocidade_pid(double erro, double v_min);
        void reset_pid();

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
        void motor_calibration(int dir);
        
        //joystick
        void joystick_control(String cmd);

 };
 #endif
