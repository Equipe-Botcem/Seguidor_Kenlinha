// Inclui a biblioteca do Arduino.
#include "Arduino.h"

// Garante que a biblioteca não seja incluída mais de uma vez.
#ifndef MOTOR_H
#define MOTOR_H

// Define a classe motor.
class motor
{
private:
    // Variáveis privadas.
    char direcao; // Indica a direção atual do motor.
    int sinal; // Indica se a velocidade é positiva ou negativa.
    int pin_PWM, pin_DIR1, pin_DIR2; // Pinagem para a ligação com o Arduino.
    int velocidade; // Armazena a velocidade atual do motor.

public:
    // Construtor.
    motor();
    
    // Funções de configuração.
    void set_pins(int p_pwm, int p_dir1, int p_dir2); // Define a pinagem.
    void set_direcao(char dir); // Define a direção do motor.
    void set_velocidade(int vel); // Define a velocidade do motor.
    void set_velocidade_fast(int vel); // Define rapidamente a velocidade do motor.

    // Funções de consulta.
    char get_direcao(); // Retorna a direção atual do motor.
    int get_velocidade(); // Retorna a velocidade atual do motor.
};

// Fecha a definição de prevenção de dupla inclusão.
#endif
