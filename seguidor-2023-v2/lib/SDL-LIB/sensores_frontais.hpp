#include <sensor.hpp>
#ifndef SENSORES_FRONTAIS_H
#define SENSORES_FRONTAIS_H
class sensores_frontais
{
private:
	// Número de sensores frontais
	const static int N_sns = 6;
	// Tolerância para o cálculo do erro
	float tolerancia = 0.3;
	// Erro anterior
    float erro_antigo = 0;
	// Segundo erro anterior
	float erro_antigo2 = 0;
	// Erro de perda
	float erro_perda = 0;
	// Erro antigo alto
	float erro_antigo_alto = 0;
	// Tempo para ignorar
	unsigned long tmp_ignorar = 0;
	// Objeto centralizado
	char obj_centralizado = 'C';
	// Percentagem de leitura de cada sensor
	float percent[N_sns] = {0,0,0,0}; //,0,0};

public:
	// Array dos sensores frontais
	sensor sensores[N_sns];
	// Limites de cada sensor
    int limites[N_sns] = {100,100,100,100}; //,100,100};

    // Construtor da classe sensores_frontais
    sensores_frontais();
	// Define os pinos dos sensores
	void set_pinos(int pins[]);
	// Define a tolerância
	void set_tol(float tol);
	// Reseta os sensores
	void reset();

	// Calcula o erro analógico
	float erro_analogico();
    
	// Retorna a tolerância
	float get_tol();
	// Retorna o número de sensores
	int get_N_sns();
	// Retorna o erro anterior
	float get_erro_antigo();
	// Retorna a média das leituras dos sensores
	float get_media();
	// Retorna a média máxima das leituras dos sensores
    float get_max_media();
	// Retorna a média mínima das leituras dos sensores
    float get_min_media();

	// Lê o sensor no índice fornecido
    void ler_sensor(int n, bool fast);
    
	// Compara o valor máximo lido com um valor fornecido
    bool comp_max_value(int valor); 
};
#endif
