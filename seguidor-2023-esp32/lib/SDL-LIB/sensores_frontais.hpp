#include <sensor.hpp>
#ifndef SENSORES_FRONTAIS_H
#define SENSORES_FRONTAIS_H
class sensores_frontais
{
private:
	
	const static int N_sns = 8;
	float tolerancia = 0.1;
    float erro_antigo = 0;
	float erro_antigo2 = 0;
	float erro_perda = 0;
	float erro_antigo_alto = 0;
	unsigned long tmp_ignorar = 0;
	char obj_centralizado = 'C';
	float percent[N_sns] = {0,0,0,0};//,0,0};

	int ordem_canal_sensores[10] = {8,9,5,0,1,2,3,4,6,7};

public:

	sensor sensores[N_sns];
    int limites[N_sns] = {100,100,100,100,100,100,100,100};

    sensores_frontais();
	void set_pinos(int pins[]);
	void set_tol(float tol);
	void reset();
	void semireset();

	float erro_analogico();
    
	float get_tol();
	int get_N_sns();
	float get_erro_antigo();
	float get_media();
    float get_max_media();
    float get_min_media();

    void ler_sensor(int n, int dado, bool fast);
    
    bool comp_max_value(int valor); 
};
#endif

