#include "sensores_frontais.hpp"

sensores_frontais::sensores_frontais(){
    reset();
}
float sensores_frontais::erro_analogico(){
    float erro = 0;
    float soma = 0;

    float valor = 0;
    float menor = 1, maior = 0;
    //Ideias: tirar sempre o menor percentual de todos os outros; diferenciar apenas quando a diferença deste para o menor for de +20%;
	
    int cont_maior_tol = 0;
    for(int i = 0; i < N_sns; i++){
        percent[i] = sensores[i].get_ult_leitura_percent();
        if(percent[i] > maior) maior = percent[i];
        if(percent[i] < menor) menor = percent[i];
        if(percent[i] > tolerancia) cont_maior_tol++;
    }
    
    int ultimo_branco = -1;
    
    if(cont_maior_tol >= (N_sns/2 + 1)) return 111111;
    if(maior - menor < tolerancia){
        if(abs(erro_antigo) != 30) tmp_ignorar = micros();
        erro_antigo = (erro_antigo_alto > 0 ? 1 : -1) * 30;
        return erro_antigo;
    }
    else{
        for(int i = 0; i < N_sns; i++){
            if(percent[i] - menor < tolerancia) percent[i] = 0;
            else if(ultimo_branco == -1 || (ultimo_branco == i - 1)) ultimo_branco = i;
            else {
                ultimo_branco = -2;
                tmp_ignorar = micros();
            }
        }
    }
    if((abs(erro_antigo) >= 30) || (ultimo_branco == -2)){
        if(erro_antigo > 0){
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
    //Serial.println("Diferença: " + (String)(maior - menor));
    /*if(soma < N_sns/2 && (maior - menor < tolerancia)){
        erro = 30 * (erro_antigo_alto > 0 ? 1:-1);
    }
    else if(soma > 2.2 && (maior - menor < tolerancia)) erro = 111111;
    else{
        erro /= soma;
        erro = atan(erro/127.5);
        erro *= 57.29578;
    }*/
    if(soma == 0) {
        erro_antigo = (erro_antigo_alto > 0 ? 1 : -1) * 30;
        return erro_antigo;
    }
    erro /= soma;
    erro = atan(erro/127.5);
    erro *= 57.29578;
    /*if(abs(erro) == 30){
        if(abs(erro_antigo) != 30){
            erro_perda = erro_antigo;
        }
    }
    else if(abs(erro_antigo) == 30 ){
        if((erro > 0 && (erro_perda < 0)) || (erro < 0 && (erro_perda > 0))){
            return erro_antigo;
        }
        else if(abs(erro - erro_perda) > 3){
            return erro_antigo;
        }
    }
    else if(abs(erro_antigo2) == 30 ){
        
    }*/
   
    if(erro != 111111) {
        erro_antigo2 = erro_antigo;
        erro_antigo = erro;
        if(abs(erro) > 1) erro_antigo_alto = erro; 
    }
	return erro;
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



void sensores_frontais::ler_sensor(int n, bool fast){
    if(n >= N_sns) return;
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
