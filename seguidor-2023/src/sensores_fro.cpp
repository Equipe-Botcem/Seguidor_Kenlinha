#include "sensores_frontais.h"

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

void sensores_frontais::ler_todos_fast()
{
    sensor * sensores[] = {&sensor0, &sensor1, &sensor2, &sensor3, &sensor4, &sensor5};

    for(int i =0; i < 6; i++){
        leituras[i] = (*sensores[i]).ler();
    }
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