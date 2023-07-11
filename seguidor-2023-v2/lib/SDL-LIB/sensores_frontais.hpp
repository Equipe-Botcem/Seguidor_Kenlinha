#include <sensor.hpp>
#ifndef SENSORES_FRONTAIS_H
#define SENSORES_FRONTAIS_H
class sensores_frontais
{
private:
	const static int N_sns = 4;
    float erro_antigo = 0;
	float erro_antigo_alto = 0;
public:

	sensor sensores[N_sns];
    int limites[N_sns] = {100,100,100,100};//,100,100};

    sensores_frontais();
	void set_pinos(int pins[]);
	void reset();

	float erro_analogico();
    
	int get_N_sns();
	float get_media();
    float get_max_media();
    float get_min_media();

    void ler_sensor(int n, bool fast);
    
    bool comp_max_value(int valor); 
};
#endif

