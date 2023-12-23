#include <string>
#include <stdio.h>
#ifndef SENSOR_H
#define SENSOR_H
class sensor
{
private:
    const int resolucao = 4095;
    int pin_dado;
    int ultima_leitura;
    int valor_max_lido;
    int valor_min_lido;
    float dead_percent;
public:
    sensor();

    int calc_limite(int fator);
    int ler(int dado);
    int ler_fast( int dado);
    void reset();

    int get_max_leitura();
    int get_min_leitura();
    int get_ult_leitura();
    int get_media_movel();
    int get_pin();
    float get_ult_leitura_percent();

    void set_pin(int pin);
};
#endif


