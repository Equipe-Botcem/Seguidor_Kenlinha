#include "motor.h"

void motor::set_direcao(char dir){
    switch (dir)
    {
    case 'F': //Frente
        digitalWrite(pin_DIR1, sinal);
	    digitalWrite(pin_DIR2, !sinal);
        break;
    case 'T': //Tras
        digitalWrite(pin_DIR1, !sinal);
	    digitalWrite(pin_DIR2, sinal);
        break;
    case 'P': //Parar
        digitalWrite(pin_DIR1, LOW);
	    digitalWrite(pin_DIR2, LOW);
        break;
    case 'B':
        digitalWrite(pin_DIR1, HIGH);
	    digitalWrite(pin_DIR2, HIGH);
        break;
    default:
        break;
    }
    direcao = dir;
}
motor::motor()
{
    pin_PWM = 0; pin_DIR1 =-1; pin_DIR2 = -1;
    sinal = HIGH; set_direcao('F');
}
void motor::set_pins(int p_pwm, int p_dir1, int p_dir2)
{
    pin_PWM = p_pwm; pin_DIR1 = p_dir1; pin_DIR2 = p_dir2;
    sinal = HIGH; set_direcao('F');
}
void motor::set_velocidade(int vel){
    if(vel < -255) vel = -255;
    else if(vel > 255) vel = 255;

    sinal = vel >= 0 ? HIGH : LOW;
    set_direcao(direcao);
    analogWrite(pin_PWM, abs(vel));
    velocidade = vel;
}

void motor::set_velocidade_fast(int vel){
    sinal = vel >= 0 ? HIGH : LOW;
    set_direcao(direcao);
    analogWrite(pin_PWM, abs(vel));
    velocidade = vel;
}

char motor::get_direcao(){
    return direcao;
}
int motor::get_velocidade(){
    return velocidade;
}

