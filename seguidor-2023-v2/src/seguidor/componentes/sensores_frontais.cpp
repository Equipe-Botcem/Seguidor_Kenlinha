#include "sensores_frontais.hpp"

// Construtor: chama a função de reset para inicializar os sensores
sensores_frontais::sensores_frontais(){
    reset();
}

// Calcula o erro analógico com base nas leituras dos sensores
float sensores_frontais::erro_analogico(){
    float erro = 0;
    float soma = 0;

    float valor = 0;
    float menor = 1, maior = 0;

    int cont_maior_50 = 0;

    // Encontra os valores mínimo e máximo entre as leituras dos sensores
    for(int i = 0; i < N_sns; i++){
        percent[i] = sensores[i].get_ult_leitura_percent();
        if(percent[i] > maior) maior = percent[i];
        if(percent[i] < menor) menor = percent[i];
        if(percent[i] > 0.5) cont_maior_50++;
    }
    
    int ultimo_branco = -1;
    
    // Se a maioria dos sensores lê acima de 50%, retorna um erro muito grande
    if(cont_maior_50 >= 4) return 111111;

    // Se a diferença entre a leitura máxima e mínima é menor que a tolerância,
    // todos os sensores estão lendo o mesmo valor, então retorna um valor de erro de 30 ou -30 dependendo do erro anterior
    if(maior - menor < tolerancia){
        if(abs(erro_antigo) != 30) tmp_ignorar = micros();
        erro_antigo = (erro_antigo_alto > 0 ? 1 : -1) * 30;
        return erro_antigo;
    }

    // Se a diferença entre a leitura máxima e mínima é maior que a tolerância, calcula o erro baseado nas leituras dos sensores
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

    // Ignora as leituras de um lado dos sensores se o erro anterior era 30 ou -30, ou se mais de um sensor está lendo branco
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

    // Calcula o erro como a média ponderada das leituras dos sensores
    for(int i = 0; i < N_sns/2; i++){
        valor = percent[i];
        erro += - valor * ((N_sns/2-1-i)*8 + 4.6); //distancia dos sensores ao centro da pcb vermelha
        soma += valor;

        valor = percent[i+N_sns/2];
        erro += valor * ((i)*8 + 4.6);
        soma += valor;
    }

    // Se a soma das leituras for 0, retorna um erro de 30 ou -30 dependendo do último erro alto
    if(soma == 0) {
        erro_antigo = (erro_antigo_alto > 0 ? 1 : -1) * 30;
        return erro_antigo;
    }

    // Calcula o erro como a tangente inversa do erro ponderado pela soma das leituras, convertendo o resultado para graus
    erro /= soma;
    erro = atan(erro/127.5);
    erro *= 57.29578;

    // Atualiza as variáveis de erro_antigo e erro_antigo_alto
    if(erro != 111111) {
        erro_antigo2 = erro_antigo;
        erro_antigo = erro;
        if(abs(erro) > 1) erro_antigo_alto = erro; 
    }

    // Retorna o erro
	return erro;
}

// Retorna o número de sensores
int sensores_frontais::get_N_sns(){
	return N_sns;
}

// Retorna a tolerância
float sensores_frontais::get_tol(){
    return tolerancia;
}

// Retorna o erro anterior
float sensores_frontais::get_erro_antigo(){
	return erro_antigo;
}

// Lê o valor do sensor especificado
void sensores_frontais::ler_sensor(int n, bool fast){
    if(n >= N_sns) return;
    if(!fast)sensores[n].ler();
    else sensores[n].ler_fast();
    
}

// Reseta todos os sensores
void sensores_frontais::reset(){
    for(int i = 0; i < N_sns; i++){
        sensores[i].reset();
    }
}

// Define os pinos para os sensores
void sensores_frontais::set_pinos(int pins[])
{
    for (int i =0; i < N_sns; i++){
      sensores[i].set_pin(pins[i]);
    }
}

// Define a tolerância para as leituras dos sensores
void sensores_frontais::set_tol(float tol){
    tolerancia = tol;
}

// Verifica se todas as leituras máximas dos sensores são menores que um valor especificado
bool sensores_frontais::comp_max_value(int valor)
{
    for(int i =0; i < N_sns; i++){
        if(sensores[i].get_max_leitura() < valor) return false;
    }
    return true;
}

// Retorna a média das leituras atuais dos sensores
float sensores_frontais::get_media()
{
    float soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += sensores[i].get_ult_leitura();
    return soma / N_sns;
}

// Retorna a média das leituras máximas dos sensores
float sensores_frontais::get_max_media()
{
    float soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += sensores[i].get_max_leitura();
    return soma / N_sns;
}

// Retorna a média das leituras mínimas dos sensores
float sensores_frontais::get_min_media()
{
    float soma = 0;
    for (int i = 0; i < N_sns; i++)
        soma += sensores[i].get_min_leitura();
    return soma / N_sns;
}
