#include "Arduino.h"
#ifndef MOTOR_H
#define MOTOR_H
class motor
{
private:
    char direcao;
    bool invertido;
    int pin_PWM, pin_DIR1, pin_DIR2;
    int velocidade;
public:
    motor();
    void set_pins(int p_pwm, int p_dir1, int p_dir2);
    void set_direcao(char dir);
    void set_velocidade(int vel);

    char get_direcao();
    int get_velocidade();
    
};
#endif
