#include "ESP_PWM.h"
#include <driver/ledc.h>

void ESP_PWM::enableChannel(int pin, int channel, int freq, int resolution, int timer, int clock, int speed_mode){
    ledc_timer_config_t pwm_timer;
    pwm_timer.speed_mode = (ledc_mode_t)speed_mode;
    pwm_timer.timer_num = (ledc_timer_t)timer;
    pwm_timer.duty_resolution = (ledc_timer_bit_t)resolution;
    pwm_timer.freq_hz = freq;
    pwm_timer.clk_cfg = (ledc_clk_cfg_t)clock;

    ledc_timer_config(&pwm_timer);

    ledc_channel_config_t ledc_channel;
    ledc_channel.speed_mode = (ledc_mode_t)speed_mode;
    ledc_channel.channel = (ledc_channel_t)channel;
    ledc_channel.timer_sel = (ledc_timer_t)timer;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.gpio_num = pin;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;

    pinChannels[channel] = pin;

    ledc_channel_config(&ledc_channel);
}

void ESP_PWM::setDuty(int pin, int duty, int speed_mode){
    if (pin < 0) return;
    for (int i = 0; i < 16; i ++){
        if(pinChannels[i] == pin){
            ledc_set_duty((ledc_mode_t)speed_mode, (ledc_channel_t)i, duty);
            ledc_update_duty((ledc_mode_t)speed_mode, (ledc_channel_t)i);
            break;
        }
    }
}

ESP_PWM::ESP_PWM(/* args */)
{
    
}

ESP_PWM::~ESP_PWM()
{
}