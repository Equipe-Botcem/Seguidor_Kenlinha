#include "sensores_frontais.hpp"

sensores_frontais::sensores_frontais(){
    reset();
}
float sensores_frontais::erro_analogico(){
    float erro = 0;
    float soma = 0;

    float valor = 0;
	for(int i = 0; i < N_sns/2; i++){
        valor = sensores[i].get_ult_leitura_percent();
        erro += - valor * ((N_sns/2-1-i)*8 + 4.6); //distancia dos sensores ao centro da pcb vermelha
        soma += valor;

        valor = sensores[i+N_sns/2].get_ult_leitura_percent();
        erro += valor * ((i)*8 + 4.6);
        soma += valor;
    }
    if(soma == 0) erro = 30 * (erro_antigo_alto > 0 ? 1:-1);
    else if(soma > N_sns - 0.5) erro = 111111;
    else /*if(soma != 0)*/{
        erro /= soma;
        erro = atan(erro/125.4);
        erro *= 57.29578;
    }
    erro_antigo = erro;
    if(abs(erro) > 1) erro_antigo_alto = erro; 
	return erro;
}

int sensores_frontais::get_N_sns(){
	return N_sns;
}

void sensores_frontais::ler_sensor(int n, bool fast){
    if(!fast)sensores[n].ler();
    else sensores[n].ler_fast();
    
}
void sensores_frontais::reset(){
    for(int i = 0; i < N_sns; i++){
        sensores[i].reset();
    }
}
void sensores_frontais::set_pinos(int pins[])
{
    for (int i =0; i < N_sns; i++){
      sensores[i].set_pin(pins[i]);
    }
}
bool sensores_frontais::comp_max_value(int valor)
{
    for(int i =0; i < N_sns; i++){
        if(sensores[i].get_max_leitura() < valor) return false;
    }
    return true;
}

float sensores_frontais::get_media()
{
    float soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += sensores[i].get_ult_leitura();
    return soma / N_sns;
}
float sensores_frontais::get_max_media()
{
    float soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += sensores[i].get_max_leitura();
    return soma / N_sns;
}
float sensores_frontais::get_min_media()
{
    float soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += sensores[i].get_min_leitura();
    return soma / N_sns;
}
