#include "motor.hpp"
#include "esp_timer.h"
#include "ESP_PWM/ESP_PWM.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void motor::init_motor_pwm(){
    ESP_PWM::enableChannel(gpio1, pwm1);
    ESP_PWM::enableChannel(gpio2, pwm2);
}

void motor::set_direcao(char dir){
    direcao = dir;
    if (dir == 'B' || dir == 'P'){
        set_velocidade(0);
    }
}
motor::motor()
{
    tmp_last_att = esp_timer_get_time();
    gpio1 = -1;gpio2 = -1;
    sinal = 1; set_direcao('F');
    posicao = 0;
}
void motor::set_pins(ledc_channel_t p1, ledc_channel_t p2, int gp1, int gp2, int e1, int e2)
{
    pwm1 = p1; pwm2 = p2; gpio1 = gp1; gpio2 = gp2; enc1 = e1; enc2 = e2;
    sinal = 1; set_direcao('F');

    init_motor_pwm();
    if (enc1 <= 0 && enc2 <= 0) return;
     gpio_config_t en1 = {
        .pin_bit_mask = (1ULL<<enc1),
        .mode = GPIO_MODE_INPUT, // Pode ser GPIO_MODE_INPUT para entrada
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE
        
    };
    gpio_config_t en2 = {
        .pin_bit_mask = (1ULL<<enc2),
        .mode = GPIO_MODE_INPUT, // Pode ser GPIO_MODE_INPUT para entrada
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
        
    };
    gpio_config(&en1);
    gpio_config(&en2);


}
void motor::set_velocidade(int vel){
    if(vel < -8191) vel = -8191;
    else if(vel > 8191) vel = 8191;

   
    int ref = (vel >= 0 ? 1 : -1);
    if(direcao == 'T') ref *= -1;
    else if(direcao == 'P') ref = 0;
    else if(direcao == 'B') ref = 2;
    

    
    if (esp_timer_get_time()/1000 - tmp_espera < 30 && ((vel > 0 && velocidade < 0) || (vel<0 && velocidade>0))){
        tmp_espera = esp_timer_get_time()/1000;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm1, 8191);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm2, 8191);
    }
    if (esp_timer_get_time()/1000 - tmp_espera > 30){
        switch (ref)
        {
        case 1:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm1, abs(vel));
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm2, 0);
            velocidade = vel;
            break;
        case -1:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm1, 0);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm2, abs(vel));
            velocidade = vel;
            break;
        case 0:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm1, 0);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm2, 0);
            velocidade = 0;
            break;
        default:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm1, 8191);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm2, 8191);
            velocidade = 0;
            break;
        }
    }
    ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm2);
    
}

void motor::set_velocidade_fast(int vel){
    
}

char motor::get_direcao(){
    return direcao;
}
int motor::get_velocidade(){
    return velocidade;
}

void motor::encoder(){
    if (enc1 <= 0 && enc2 <= 0) return;
    if (gpio_get_level((gpio_num_t)enc1) == 0) return;
    if (gpio_get_level((gpio_num_t)enc2)) posicao++;
    else posicao--;
}

void motor::updateVel(){
    if(tmp_controle==0) tmp_controle=esp_timer_get_time();
    if(abs(posicao - ult_pos) >= 1 && (esp_timer_get_time() - tmp_last_att >=500)){
        vel_real = 1000000*(float)(posicao-ult_pos)/345/(esp_timer_get_time() - tmp_last_att);
        ult_pos = posicao;
        tmp_last_att = esp_timer_get_time();
    }
    else if(esp_timer_get_time() - tmp_last_att >=200000){
        vel_real = 0;
        //tmp_last_att = esp_timer_get_time();
    }

    /*float erro = vel_objetivo - vel_real;
    float _kp = 140000, _kd = 200000,_ki = 200;
    
    float d_tempo = (esp_timer_get_time() - tmp_controle) /1000.00;
    if(d_tempo == 0) d_tempo = 1;
    tmp_controle = esp_timer_get_time();

    //erro_I += erro - erro_antigo;
    erro_I += erro * d_tempo;
    if(abs(erro_I)> 4000) erro_I = 4000* erro_I/abs(erro_I);
    float correcao = _kp * erro + _ki * erro_I + _kd * (erro - erro_antigo)/d_tempo;
	erro_antigo = erro;
	
	
    set_direcao('F');
    if(correcao*vel_objetivo < 0) correcao = vel_objetivo/abs(vel_objetivo);
    if (correcao<0) printf("%.2f\n", correcao);
    set_velocidade(correcao);*/
}

int motor::getPosicao(){
    return posicao;
}

void motor::resetEncoder(){
    posicao = 0;
}

void motor::printencoder(){
    printf("%i\n", posicao);
}

