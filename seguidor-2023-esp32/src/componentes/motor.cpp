#include "motor.hpp"
#include "esp_timer.h"
#include "ESP_PWM/ESP_PWM.h"

void motor::init_motor_pwm(){
    ESP_PWM::enableChannel(gpio1, pwm1);
    ESP_PWM::enableChannel(gpio2, pwm2);
    /*ledc_timer_config_t pwm_timer;
    pwm_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    pwm_timer.timer_num = LEDC_TIMER_0;
    pwm_timer.duty_resolution = LEDC_TIMER_13_BIT;
    pwm_timer.freq_hz = 4000;
    pwm_timer.clk_cfg = LEDC_AUTO_CLK;

    ledc_timer_config(&pwm_timer);

    ledc_channel_config_t pwm_ch1;
    pwm_ch1.speed_mode = LEDC_LOW_SPEED_MODE;
    pwm_ch1.channel = pwm1;
    pwm_ch1.timer_sel = LEDC_TIMER_0;
    pwm_ch1.intr_type = LEDC_INTR_DISABLE;
    pwm_ch1.gpio_num = gpio1;
    pwm_ch1.duty = 0;
    pwm_ch1.hpoint = 0;

    ledc_channel_config_t pwm_ch2;
    pwm_ch2.speed_mode = LEDC_LOW_SPEED_MODE;
    pwm_ch2.channel = pwm2;
    pwm_ch2.timer_sel = LEDC_TIMER_0;
    pwm_ch2.intr_type = LEDC_INTR_DISABLE;
    pwm_ch2.gpio_num = gpio2;
    pwm_ch2.duty = 0;
    pwm_ch2.hpoint = 0;

    ledc_channel_config(&pwm_ch1);
    ledc_channel_config(&pwm_ch2);*/
}

void motor::set_direcao(char dir){
    direcao = dir;
    if (dir == 'B' || dir == 'P'){
        set_velocidade(0);
    }
}
motor::motor()
{
    gpio1 = -1;gpio2 = -1;
    sinal = 1; set_direcao('F');
}
void motor::set_pins(ledc_channel_t p1, ledc_channel_t p2, int gp1, int gp2)
{
    pwm1 = p1; pwm2 = p2; gpio1 = gp1; gpio2 = gp2;
    sinal = 1; set_direcao('F');
    init_motor_pwm();
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

