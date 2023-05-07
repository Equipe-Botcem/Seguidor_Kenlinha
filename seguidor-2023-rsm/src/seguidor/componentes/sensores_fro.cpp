#include "sensores_frontais.h"

void sensores_frontais::reset(){
    sensor0.reset();
    sensor1.reset();
    sensor2.reset();
    sensor3.reset();
    sensor4.reset();
    sensor5.reset();

    leituras[0] = -1;
    leituras[1] = -1;
    leituras[2] = -1;
    leituras[3] = -1;
    leituras[4] = -1;
    leituras[5] = -1;

    max_leituras[0] = -1;
    max_leituras[1] = -1;
    max_leituras[2] = -1;
    max_leituras[3] = -1;
    max_leituras[4] = -1;
    max_leituras[5] = -1;
    
    min_leituras[0] = 1023;
    min_leituras[1] = 1023;
    min_leituras[2] = 1023;
    min_leituras[3] = 1023;
    min_leituras[4] = 1023;
    min_leituras[5] = 1023;
    
    
}

void sensores_frontais::set_pinos(int pins[])
{
    sensor0.set_pin(pins[0]);
    sensor1.set_pin(pins[1]);
    sensor2.set_pin(pins[2]);
    sensor3.set_pin(pins[3]);
    sensor4.set_pin(pins[4]);
    sensor5.set_pin(pins[5]);
}
void sensores_frontais::ler_todos()
{
    sensor * sensores[] = {&sensor0, &sensor1, &sensor2, &sensor3, &sensor4, &sensor5};

    for(int i =0; i < 6; i++){
        leituras[i] = (*sensores[i]).ler();
        max_leituras[i] = (*sensores[i]).get_max_leitura();
        min_leituras[i] = (*sensores[i]).get_min_leitura();
    }
}

bool sensores_frontais::comp_max_value(int valor)
{
    sensor * sensores[] = {&sensor0, &sensor1, &sensor2, &sensor3, &sensor4, &sensor5};

    for(int i =0; i < 6; i++){
        if((*sensores[i]).get_max_leitura() < valor) return false;
    }
    return true;
}

void sensores_frontais::ler_todos_fast()
{
    leituras[0] = sensor0.ler_fast();
    leituras[1] = sensor1.ler_fast();
    leituras[2] = sensor2.ler_fast();
    leituras[3] = sensor3.ler_fast();
    leituras[4] = sensor4.ler_fast();
    leituras[5] = sensor5.ler_fast();
    /*sensor * sensores[] = {&sensor0, &sensor1, &sensor2, &sensor3, &sensor4, &sensor5};

    for(int i =0; i < 6; i++){
        leituras[i] = (*sensores[i]).ler_fast();
    }*/
}
bool sensores_frontais::leituras_max_dif(double fator)
{
    int maior = 0;
    int menor = 1024;
    for (int i = 0; i < 6; i++)
    {
        if (maior < leituras[i])
            maior = leituras[i];
        if (menor > leituras[i])
            menor = leituras[i];
    }
    double percentual = (maior - menor) / maior;
    if (percentual > fator)
        return true;
    return false;
}
double sensores_frontais::get_media()
{
    double soma = 0;
    for (int i = 0; i < 6; i++)
        soma += leituras[i];
    return soma / 6;
}
double sensores_frontais::get_max_media()
{
    double soma = 0;
    for (int i = 0; i < 6; i++)
        soma += max_leituras[i];
    return soma / 6;
}
double sensores_frontais::get_min_media()
{
    double soma = 0;
    for (int i = 0; i < 6; i++)
        soma += min_leituras[i];
    return soma / 6;
}
