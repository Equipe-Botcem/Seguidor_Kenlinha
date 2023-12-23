#include "sensores_frontais.hpp"
#include "esp_timer.h"
#include "math.h"

sensores_frontais::sensores_frontais(){
    reset();
}
float sensores_frontais::erro_analogico(){
    float erro = 0;
    float soma = 0;
    float valor = 0;
    float menor = 1, maior = 0;
    int ultimo_branco = -1;
    
    //Ideias: tirar sempre o menor percentual de todos os outros; diferenciar apenas quando a diferença deste para o menor for de +20%;
	
    for(int i = 0; i < N_sns; i++){
        percent[i] = sensores[i].get_ult_leitura_percent();
        if(percent[i] > maior) maior = percent[i];
        if(percent[i] < menor) menor = percent[i];
    }
    
    
    if      (menor > tolerancia) return 111111;
    else if (maior - menor < tolerancia){

        if(abs(erro_antigo) != 30) tmp_ignorar = esp_timer_get_time();
        erro_antigo = (erro_antigo_alto > 0 ? 1 : -1) * 30;
        return erro_antigo;
    }
    else{
        for(int i = 0; i < N_sns; i++){
            if(percent[i] - menor < tolerancia) percent[i] = 0;
            else if(ultimo_branco == -1 || (ultimo_branco == i - 1)) ultimo_branco = i;
            else {
                ultimo_branco = -2;
                tmp_ignorar = esp_timer_get_time();
            }
        }
    }
    if(abs(erro_antigo) >= 30){
        if(erro_antigo < 0){
            for(int i = 0; i < N_sns/2; i++) {
                percent[i] = 0;
            }
        }
        else{
            for(int i = N_sns/2; i < N_sns; i++) {
                percent[i] = 0;
            }
        }
    }
    else if(ultimo_branco == -2){
        if(erro_antigo < 0){
            for(int i = 0; i < N_sns/2; i++) {
                percent[i] = 0;
            }
        }
        else{
            for(int i = N_sns/2; i < N_sns; i++) {
                percent[i] = 0;
            }
        }
    }
    for(int i = 0; i < N_sns/2; i++){
        valor = percent[i];
        erro += - valor * ((N_sns/2-1-i)*8 + 4.6); //distancia dos sensores ao centro da pcb vermelha
        soma += valor;

        valor = percent[i+N_sns/2];
        erro += valor * ((i)*8 + 4.6);
        soma += valor;
    }
    if (soma > 3.5) return 111111;
    if(soma < N_sns/2*tolerancia) {
        erro_antigo = (erro_antigo_alto > 0 ? 1 : -1) * 30;
        return erro_antigo;
    }
    erro /= soma;
    erro = atan2(erro, 60);//127.5);
    erro *= 57.29578;
    erro *= -1;

    erro_antigo = erro;
    if(abs(erro) > 1) erro_antigo_alto = erro; 
	return erro ;
}

int sensores_frontais::get_N_sns(){
	return N_sns;
}
float sensores_frontais::get_tol(){
    return tolerancia;
}
float sensores_frontais::get_erro_antigo(){
	return erro_antigo;
}



void sensores_frontais::ler_sensor(int n, int dado, bool fast){
    if(dado < 0) printf("Dado invalido\n");
    if(n >= N_sns +2) return;
    if(!fast)sensores[ordem_canal_sensores[n]].ler(dado);
    else sensores[ordem_canal_sensores[n]].ler_fast(dado);
    
}
void sensores_frontais::reset(){
    for(int i = 0; i < N_sns; i++){
        sensores[i].reset();
    }
    erro_antigo = 0;
    erro_antigo_alto =0;
    erro_perda = 0;
    
}

void sensores_frontais::semireset(){
    erro_antigo = 0;
    erro_antigo_alto =0;
    erro_perda = 0;
    
}
void sensores_frontais::set_pinos(int pins[])
{
    for (int i =0; i < N_sns; i++){
      sensores[i].set_pin(pins[i]);
    }
}
void sensores_frontais::set_tol(float tol){
    tolerancia = tol;
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
