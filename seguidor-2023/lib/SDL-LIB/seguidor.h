#include <motor.h>
#include <sensor.h>
#include <sensores_frontais.h>
#ifndef SDL_H
#define SDL_H
class Seguidor_de_Linha
{
    private:
        motor motor_dir;
        motor motor_esq;
        sensores_frontais sns_frontais;
        sensor sensor_chegada;    
        
        bool printar = true;

        double Kp = 0.035;
        double Ki = 0.020;
        double Kd = 0.020;
        double K = 0.050;

        double vel_max = 100;
        double vel_min = 15;
        double vel_base = 200;

        double P = 0; //Retirar
        double erro_P = 0;
        double erro_I = 0;
        double erro_D = 0;
        double erro_antigo = 0;

        double vel_motor_direito = 0;
        double vel_motor_esquerdo = 0;

        unsigned int TMP_calib = 50;
        double STOP_VALUE_LATERAL = 100;
        double STOP_VALUE_CURVA = 300;
        double STOP_VALUE_FRONTAL = 100;

        

        
        unsigned long start_time = 0;
        int qnt_linhas = 2;

        unsigned long second = 1000;

        double val_leitura_max_lateral =0;
        double val_leitura_max_curva =0;

        double valor_med_max = 0;
        double valor_med_min = 1023;

        int ultimo_sensor_na_linha = 0;
        bool saiu_da_linha = false;
        //-------------------//
        

        bool Curva_foi_pro_preto = false;

        unsigned long tmp_saida_linha = 0;
                        
        int pinos[14] = {A6,A5,A4,A3,A2,A0,
                        A1, A7, 6, 10, 9, 5, 7, 8};

        String command = "";


    public:
        char modo = 'N'; // N: Normal | C: Curva | J: Joystick | P: Parado
        bool Estado_corrida = false;
        bool stop_condition = false;

        Seguidor_de_Linha(/* args */);
        //~Seguidor_de_Linha();
        void conf_pinos();
        void set_direcao(char direcao);
        void set_velocidade(int vel_dir, int vel_esq);
        void set_velocidade(double erro);
        void sensor_stop_condition();
        void sensor_curva_condition(int value);
        void sensor_encruzilhada_condition();
        void identifica_perda_caminho();
        void motor_controler(double erro);

        void motor_calibration(int dir);
        
        double calibracao_frontal(double valor_descalibrado, double valLeitura_max, double valLeitura_min);
        double calc_erro();
        double calc_erro2();
        double PID_calc(double erro);
        double PID_calc2(double erro);
        double Vel_direcional(double erro);
        double motor_controler_esq(double erro);
        double motor_controler_dir(double erro);

        bool ignorar_leitura();
        bool checar_fora_linha();

        int motor_check(double vel_motor);

        //handler
        void send_const();
        void set_handler();
        void set_geral(String valores);
        void stop_handler();
        void run_handler();
        void find_max();
        void find_min();
        void find_max_lateral();
        void find_max_curvas();
        void calibracao_handler();
        void teste_frontal();
        void teste_lateral();
        void teste_curva();
        void msg_handler();
        void comunica_serial();

        void joystick_control(String cmd);

 };
 #endif
