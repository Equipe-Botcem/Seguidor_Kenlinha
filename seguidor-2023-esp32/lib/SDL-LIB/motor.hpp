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

    int vel_espera;
    unsigned long tmp_espera = 0;
public:
    motor();
    void init_motor_pwm();
    void set_pins(ledc_channel_t p1, ledc_channel_t p2, int gp1, int gp2);
    void set_direcao(char dir);
    void set_velocidade(int vel);
    void set_velocidade_fast(int vel);

    char get_direcao();
    int get_velocidade();
    
};
#endif
