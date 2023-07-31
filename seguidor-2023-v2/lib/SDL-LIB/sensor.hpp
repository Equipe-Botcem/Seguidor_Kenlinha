#include "Arduino.h"
#ifndef SENSOR_H
#define SENSOR_H
class sensor
{
private:
    // Pino de dados do sensor
    int pin_dado;
    // Última leitura do sensor
    int ultima_leitura;
    // Valor máximo lido pelo sensor
    int valor_max_lido;
    // Valor mínimo lido pelo sensor
    int valor_min_lido;
    // Percentagem morta (dead zone)
    float dead_percent;
public:
    // Construtor da classe sensor
    sensor();

    // Calcula o limite do sensor, dado um fator
    int calc_limite(float fator);
    // Lê o valor do sensor (leitura completa)
    int ler();
    // Lê o valor do sensor (leitura rápida)
    int ler_fast();
    // Reseta o sensor
    void reset();

    // Retorna o valor máximo lido pelo sensor
    int get_max_leitura();
    // Retorna o valor mínimo lido pelo sensor
    int get_min_leitura();
    // Retorna a última leitura do sensor
    int get_ult_leitura();
    // Retorna a média móvel das leituras do sensor
    int get_media_movel();
    // Retorna o pino de dados do sensor
    int get_pin();
    // Retorna a última leitura do sensor em percentagem
    float get_ult_leitura_percent();

    // Define o pino de dados do sensor
    void set_pin(int pin);
};
#endif
