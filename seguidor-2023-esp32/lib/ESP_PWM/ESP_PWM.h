#ifndef ESP_PWM_H
#define ESP_PWM_H
class ESP_PWM
{
private:
    /* data */
public:
    ESP_PWM(/* args */);
    ~ESP_PWM();
    static int pinChannels[16];
    static void enableChannel(int pin, int channel, int freq = 4000, int resolution = 13, int timer = 0, int clock = 0, int speed_mode = 0);
    static void setDuty(int pin, int duty, int speed_mode = 0);
};



#endif