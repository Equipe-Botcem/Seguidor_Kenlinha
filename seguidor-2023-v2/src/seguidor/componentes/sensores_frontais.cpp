#include "sensores_frontais.hpp"

sensores_frontais::sensores_frontais(){
    reset();
}

float sensores_frontais::erro_digital(){
    float erro = 0;

	int s0 = leituras[0] > limites[0] ? 1:0;
	int s1 = leituras[1] > limites[1] ? 2:0;
	int s2 = leituras[2] > limites[2] ? 4:0;
	int s3 = leituras[3] > limites[3] ? 8:0;
	int s4 = leituras[4] > limites[4] ? 16:0;
	int s5 = leituras[5] > limites[5] ? 27:0;
    int pos = s0 + s1 + s2 + s3 + s4 + s5;
	
	erro = casos[pos];
	
	if(pos == 0 && erro_antigo != 0) erro *= abs(erro_antigo)/erro_antigo;
	else if(pos == 58) erro = 111111;

    erro_antigo = erro;
	return erro;
}
int sensores_frontais::get_N_sns(){
	return N_sns;
}

void sensores_frontais::ler_sensor(int n){
    leituras[n] = sensores[n].ler();
}


void sensores_frontais::reset(){
    for(int i = 0; i < N_sns; i++){
        leituras[i] = -1;
        max_leituras[i] = -1;
        min_leituras[i] = 1023;
        sensores[i].reset();
    }
}

void sensores_frontais::set_pinos(int pins[])
{
    for (int i =0; i < N_sns; i++){
      sensores[i].set_pin(pins[i]);
    }
}
void sensores_frontais::ler_todos()
{
    for(int i =0; i < N_sns; i++){
        leituras[i] = sensores[i].ler();
        max_leituras[i] = sensores[i].get_max_leitura();
        min_leituras[i] = sensores[i].get_min_leitura();
    }
}

bool sensores_frontais::comp_max_value(int valor)
{
    for(int i =0; i < N_sns; i++){
        if(sensores[i].get_max_leitura() < valor) return false;
    }
    return true;
}

void sensores_frontais::ler_todos_fast()
{
    for(int i =0; i < N_sns; i++){
        leituras[i] = sensores[i].ler();
    }
}
bool sensores_frontais::leituras_max_dif(double fator)
{
    int maior = 0;
    int menor = 1024;
    for (int i = 0; i < N_sns; i++)
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
    for (int i = 0; i < N_sns; i++)
        soma += leituras[i];
    return soma / N_sns;
}
double sensores_frontais::get_max_media()
{
    double soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += max_leituras[i];
    return soma / N_sns;
}
double sensores_frontais::get_min_media()
{
    double soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += min_leituras[i];
    return soma / N_sns;
}
