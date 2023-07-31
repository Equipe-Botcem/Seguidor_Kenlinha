#include "motor.hpp"

// Define a direção do motor com base no parâmetro fornecido
void motor::set_direcao(char dir){
    switch (dir)
    {
    case 'F': //Frente
        digitalWrite(pin_DIR1, sinal);
	    digitalWrite(pin_DIR2, !sinal);
        break;
    case 'T': //Trás
        digitalWrite(pin_DIR1, !sinal);
	    digitalWrite(pin_DIR2, sinal);
        break;
    case 'P': //Parar
        digitalWrite(pin_DIR1, LOW);
	    digitalWrite(pin_DIR2, LOW);
        break;
    case 'B': //Travar
        digitalWrite(pin_DIR1, HIGH);
	    digitalWrite(pin_DIR2, HIGH);
        break;
    default:
        break;
    }
    direcao = dir;
}

// Construtor: inicializa os valores dos pinos do motor e define a direção para 'Frente'
motor::motor()
{
    pin_PWM = 0; pin_DIR1 =-1; pin_DIR2 = -1;
    sinal = HIGH; set_direcao('F');
}

// Configura os pinos do motor
void motor::set_pins(int p_pwm, int p_dir1, int p_dir2)
{
    pin_PWM = p_pwm; pin_DIR1 = p_dir1; pin_DIR2 = p_dir2;
    sinal = HIGH; set_direcao('F');
}

// Define a velocidade do motor, ajustando a direção se necessário
void motor::set_velocidade(int vel){
    if(vel < -255) vel = -255;
    else if(vel > 255) vel = 255;

    sinal = vel >= 0 ? HIGH : LOW;
    set_direcao(direcao);
    analogWrite(pin_PWM, abs(vel));
    velocidade = vel;
}

// Similar à função set_velocidade, mas não realiza nenhuma verificação de limite de velocidade
void motor::set_velocidade_fast(int vel){
    sinal = vel >= 0 ? HIGH : LOW;
    set_direcao(direcao);
    analogWrite(pin_PWM, abs(vel));
    velocidade = vel;
}

// Retorna a direção atual do motor
char motor::get_direcao(){
    return direcao;
}

// Retorna a velocidade atual do motor
int motor::get_velocidade(){
    return velocidade;
}
