
#include "Arduino.h"
#include <motor.hpp>
#include <sensor.hpp>
#include <sensores_frontais.hpp>
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

        //constantes para chute da fisica do PID
        double tempos_medios[55] = {0};
        unsigned long tempo_ult_update = 0;
        //constantes para fisica do PID
        double raio_roda = 1.6; //cm
        double tamanho_eixo_motor = 16; //cm
        double rpm_motor = 2000;
        unsigned long fim_correcao = 0; //milisec
        unsigned long meio_correcao = 0; //milisec
        unsigned long correcao_atual = 0; //milisec
        double vel_max_motor = rpm_motor/60 * raio_roda*2*3.1415/100;//m/s
        double m_grau = tamanho_eixo_motor/100 * 2 * 3.1415 /360;
        unsigned long tempo_correcao_grau_1v = (m_grau/(vel_max_motor/255) * 1000); //milisec
        double erro_sendo_corrigindo = 0;
        

        //controlador PID
        double erro_P = 0;
        double erro_I = 0;
        double erro_D = 0;
        
        double pesos[171] = {	
		55, //0
		0, //1
		0, //2
		-11.39, //3
		0, //4
		0, //5
		0, //6
		0, //7
		0, //8
		0, //9
		0, //10
		0, //11
		0, //12
		0, //13
		-7.95, //14
		0, //15
		0, //16
		0, //17
		0, //18
		0, //19
		0, //20
		0, //21
		0, //22
		0, //23
		0, //24
		0, //25
		0, //26
		0, //27
		0, //28
		0, //29
		0, //30
		0, //31
		0, //32
		0, //33
		-2.88, //34
		0, //35
		0, //36
		-5.12, //37
		0, //38
		0, //39
		0, //40
		0, //41
		0, //42
		0, //43
		0, //44
		-7.95, //45
		0, //46
		0, //47
		0, //48
		0, //49
		0, //50
		0, //51
		0, //52
		0, //53
		0, //54
		0, //55
		0, //56
		-7.95, //57
		0, //58
		11.39, //59
		0, //60
		0, //61
		0, //62
		0, //63
		0, //64
		-0.74, //65
		0, //66
		0, //67
		-10, //68
		0, //69
		0, //70
		0, //71
		0, //72
		-7.95, //73
		2.88, //74
		0, //75
		0, //76
		-2.88, //77
		0, //78
		0, //79
		-5.12, //80
		0, //81
		0, //82
		0, //83
		0, //84
		2.88, //85
		0, //86
		0, //87
		0, //88
		0, //89
		0, //90
		0, //91
		0, //92
		-2.88, //93
		0, //94
		0, //95
		-5.12, //96
		0.74, //97
		0, //98
		0, //99
		0.74, //100
		0, //101
		7.95, //102
		0, //103
		0, //104
		7.95, //105
		0, //106
		0, //107
		0, //108
		0, //109
		0, //110
		-9, //111
		0, //112
		7.95, //113
		0, //114
		0, //115
		0, //116
		0, //117
		0, //118
		0, //119
		0, //120
		0, //121
		0, //122
		0, //123
		-0.74, //124
		7.95, //125
		0, //126
		-10, //127
		0, //128
		0, //129
		0, //130
		0, //131
		0, //132
		5.12, //133
		0, //134
		0, //135
		5.12, //136
		0, //137
		0, //138
		-5.12, //139
		0, //140
		0, //141
		0, //142
		0, //143
		5.12, //144
		0, //145
		0, //146
		5.12, //147
		0, //148
		0, //149
		0, //150
		0, //151
		0, //152
		0, //153
		0, //154
		0, //155
		10, //156
		0, //157
		0, //158
		10, //159
		0, //160
		0, //161
		0, //162
		0, //163
		0, //164
		0, //165
		0, //166
		9, //167
		0, //168
		0, //169
		0, //170
	};


        //constantes para aumentar convergencia em retas
        double erro_antigo_nao_nulo = 0;
        double erro_alternado = 0;
        double fator_convergencia = 1;
        int cont_alternancia = 0;

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

        long taxa_de_amostra = 0;
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
