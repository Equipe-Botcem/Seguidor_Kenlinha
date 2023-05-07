#include <sensor.h>
#ifndef SENSORES_FRONTAIS_H
#define SENSORES_FRONTAIS_H
class sensores_frontais
{
private:
    
public:
    sensor sensor0;
    sensor sensor1;
    sensor sensor2;
    sensor sensor3;
    sensor sensor4;
    sensor sensor5;
    int leituras[6];
    int max_leituras[6];
    int min_leituras[6];
    void set_pinos(int pins[]);
    void ler_todos();
    void ler_todos_fast();
    bool leituras_max_dif(double fator);
    double get_media();
    
};
#endif

