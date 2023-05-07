#include "Arduino.h"
#ifndef SENSOR_H
#define SENSOR_H
class sensor
{
private:
    int pin_dado;
    int ultima_leitura;
    int penultima_leitura;
    int valor_max_lido;
    int valor_min_lido;
public:
    void set_pin(int pin);
    int ler();
    int ler_fast();
    int get_max_leitura();
    int get_min_leitura();
    int get_pen_leitura();
    int get_ult_leitura();
    void reset();
    sensor();
};
#endif


