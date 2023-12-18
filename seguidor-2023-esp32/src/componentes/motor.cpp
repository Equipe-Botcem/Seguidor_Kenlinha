#include "motor.hpp"
#include "esp_timer.h"
#include "ESP_PWM/ESP_PWM.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

using namespace std;

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
void motor::ativar(){
    ativo = true;
    set_direcao('F');
    tmp_controle = esp_timer_get_time();
    erro_I = 0;
    erro_antigo = 0;
}
void motor::desativar(){
    ativo = false;
    set_direcao('P');
}
motor::motor()
{
    tmp_last_att = esp_timer_get_time();
    gpio1 = -1;gpio2 = -1;
    posicao = 0;
    //set_direcao('P');
}
void motor::set_pins(ledc_channel_t p1, ledc_channel_t p2, int gp1, int gp2, int e1, int e2)
{
    pwm1 = p1; pwm2 = p2; gpio1 = gp1; gpio2 = gp2; enc1 = e1; enc2 = e2;

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
float motor::getVelObj(){
    return vel_objetivo;
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
void motor::teste(string *teste){

    * teste  = (
        to_string(vel_real) + " -> " + to_string(vel_objetivo) + " m/s\n"+
        "Integral: " + to_string(erro_I)
    );
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
    
    if(!ativo || !velControl) return;
    float erro = vel_objetivo - vel_real;
    
    float _kp = 3000, _kd = 500000,_ki = 40;
    //if (abs(erro) > 1) _kp += 3000;// 1000 a 3000
    float d_tempo = (esp_timer_get_time() - tmp_controle) /1000.00;
    if(d_tempo == 0) d_tempo = 0.000001;
    tmp_controle = esp_timer_get_time();

    //erro_I += erro - erro_antigo;
    erro_I += erro * d_tempo;
    if(abs(erro_I)> 2000) erro_I = 2000* erro_I/abs(erro_I);
    float correcao = _kp * erro + _ki * erro_I + _kd * (erro - erro_antigo)/d_tempo;
    correcao += 0;
	erro_antigo = erro;
	
    if(correcao*vel_objetivo < 0){
        correcao = 0;
        /*if (abs(vel_real) > 2.5) correcao = vel_objetivo/abs(vel_objetivo);

        else if(abs(correcao) > (8191 - abs(vel_real)*3276)){
            correcao = -vel_objetivo/abs(vel_objetivo)*(8191 - abs(vel_real)*3276);
        }*/
        
    }
    set_velocidade(correcao);
}
void motor::setVel(float vel){
    vel_objetivo = vel;
}
int motor::getPosicao(){
    return posicao;
}

void motor::resetEncoder(){
    posicao = 0;
    ult_pos = 0;
    tmp_last_att = esp_timer_get_time();
    vel_real = 0;
}

void motor::printencoder(){
    printf("%i\n", posicao);
}

