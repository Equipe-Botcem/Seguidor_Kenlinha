#include <string>
#include <stdio.h>
#include <stdbool.h>
#include <driver/ledc.h>
#include <esp_system.h>
#include <driver/gpio.h>
#include <unistd.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifndef MOTOR_H
#define MOTOR_H
class motor
{
private:
    char direcao;
    int sinal; //pode ser invertido por velocidade negativa
    ledc_channel_t pwm1, pwm2;
    int gpio1, gpio2;
    int velocidade;
    int posicao, enc1, enc2;

    int vel_espera;

    bool ativo = false;

    //controle
    bool velControl = true;
    float vel_objetivo = 0;
    int tmp_last_att = 0;
    int ult_pos = 0;
    unsigned long tmp_espera = 0;
    float erro_I =0;
    float erro_antigo = 0;
    long tmp_controle = 0;
    
public:
    float vel_real = 0;
    motor();
    void init_motor_pwm();
    void set_pins(ledc_channel_t p1, ledc_channel_t p2, int gp1, int gp2, int e1 = -1, int e2 = -1);
    void set_direcao(char dir);
    void set_velocidade(int vel);
    void set_velocidade_fast(int vel);

    void updateVel();
    void setVel(float vel);
    char get_direcao();
    int get_velocidade();

    void encoder();
    int getPosicao();
    void resetEncoder();
    void printencoder();

    void ativar();
    void desativar();

    
};
#endif
