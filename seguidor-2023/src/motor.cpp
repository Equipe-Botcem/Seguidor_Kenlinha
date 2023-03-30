#include "motor.h"

// Construtor
motor::motor()
{
    // Inicialização dos atributos
    pin_PWM = 0;
    pin_DIR1 = -1;
    pin_DIR2 = -1;
    invertido = false;
    set_direcao('F');
}

// Configura os pinos do motor
void motor::set_pins(int p_pwm, int p_dir1, int p_dir2)
{
    // Armazena os pinos
    pin_PWM = p_pwm;
    pin_DIR1 = p_dir1;
    pin_DIR2 = p_dir2;

    // Configura a direção para frente
    invertido = false;
    set_direcao('F');
}

// Define a direção do motor
void motor::set_direcao(char dir)
{
    // Seleciona o sinal a ser enviado
    int sinal = HIGH;
    if (invertido) {
        sinal = LOW;
    }

    // Configura a direção do motor
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
    case 'B': //Freio
        digitalWrite(pin_DIR1, HIGH);
        digitalWrite(pin_DIR2, HIGH);
        break;
    default:
        break;
    }

    // Armazena a direção
    direcao = dir;
}

// Define a velocidade do motor
void motor::set_velocidade(int vel)
{
    // Limita o valor da velocidade
    if (vel < -255) {
        vel = -255;
    } else if (vel > 255) {
        vel = 255;
    }

    // Define a direção do motor
    invertido = vel < 0 ? true : false;
    set_direcao(direcao);

    // Define a velocidade do motor
    analogWrite(pin_PWM, abs(vel));
    velocidade = vel;
}

// Retorna a direção do motor
char motor::get_direcao()
{
    return direcao;
}

// Retorna a velocidade do motor
int motor::get_velocidade()
{
    return velocidade;
}
