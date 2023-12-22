#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_adc/adc_continuous.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#include "SimpleKalmanFilter/SimpleKalmanFilter.h"
#include <NimBLEDevice.h>

#include <string>
#include <stdio.h>

#include <motor.hpp>
#include <sensor.hpp>
#include <sensores_frontais.hpp>
#include <controlador_PID.hpp>


using namespace std;
#ifndef SDL_H
#define SDL_H

class Seguidor_de_Linha
{
    
    private:
        controlador_PID Controlador;
        motor motor_dir;
        motor motor_esq;

        motor ventoinhaD;
        motor ventoinhaE;

        SimpleKalmanFilter Kalman = SimpleKalmanFilter(0.008,0.008,0.015);

        BLECharacteristic * esp_com = NULL;
        string * wifi_saida = NULL;
        unsigned long start_time = 0;
        char modo = 'N';

        //Parametros de calibracao
        float MAX_PRETO_CHEGADA = 100;
        float MAX_PRETO_MAPA = 100;

        unsigned long ult_leitura_chg = 0;

        int estado_s_chegada = 0;
        int estado_s_mapa = 0;
        //-------------------//
        //unsigned long tmp_fora_linha = 0;
        int marcacoes_secao = 0;
        int marcacoes_chegada = 0;
        int enc_chegada = 10;
        
        unsigned long curva_time = 0;
        char direcao_atual = 'F';
        char lado_pista = 'D';

        //int pinos[15] = {ADC_CHANNEL_2,ADC_CHANNEL_3,ADC_CHANNEL_4,ADC_CHANNEL_5,ADC_CHANNEL_6,ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9, ADC_CHANNEL_0, ADC_CHANNEL_1,
        //                    0,45,48,21,47};

        //10 13 17 26 28 29 27 14 , 24 , 15 lampedrao adcnum
        int pinos[15] = {ADC_CHANNEL_0,ADC_CHANNEL_3,ADC_CHANNEL_7,ADC_CHANNEL_6,ADC_CHANNEL_8,ADC_CHANNEL_9, ADC_CHANNEL_7, ADC_CHANNEL_4, ADC_CHANNEL_4, ADC_CHANNEL_5,
                            45,0,48,21,47};
        int pinos_ventoinha[5] = {2,42,46,40,39};
        
        string output_buffer;

        int vel_ventoinha = 0;
        
        int cont_local = 0;
        
    public:
        bool troca_max_min = false;
        unsigned long tmp_ult_cmd = 0;
        sensores_frontais sns_frontais;
        sensor sensor_chegada;
        sensor sensor_mapa;
        bool ler_sensores_sem_pausa = false;
        bool ler_sensores_fast = false;

        unsigned long TMP_calib = 12;
        unsigned long tempo = TMP_calib;
        bool Estado_corrida = false;
        

        Seguidor_de_Linha();

        void loop();
        void init();
        void intr_enc_dir();
        void intr_enc_esq();
        void printEncoders();
        void updateOutput();
        void set_BLE_CHAR(BLECharacteristic * Canal);
        void set_WIFIBuffer(string * buffer);
        char get_modo();
        int getpin(int pin);

        char getDir();
        float getVelDir();
        float getVelEsq();

        float seguir_linha();
        float seguir_linha_final();

        void printError();

        //Checks
        void checar_chegada();
        void checar_secao();

        //comunicacao
        void ControlCMD(string command);
        void run();
        void stop(string agente = "desconhecido");
        
        void send_Consts();
        void set_Consts(string valores);

        void teste_frontal();
        void teste_lateral();
        
        void output(string text, bool new_line =true);
        //calibracao
        void calibracao();
        int sensor_calib(int pos);
        
        //joystick
        void joystick_control(string cmd);

        void set_direcao(char direcao);
        void set_velocidade(int vel_dir, int vel_esq);
        void setVel(float vel_dir, float vel_esq);
        void set_velocidade_fast(int vel_dir, int vel_esq);
        void set_ventoinha(int vel);

 };
 #endif
