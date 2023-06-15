#include <sensor.hpp>
#ifndef SENSORES_FRONTAIS_H
#define SENSORES_FRONTAIS_H
class sensores_frontais
{
private:
    /*float casos[59] = {	
		55, //0
		-11.39, //1
		0, //2
		-7.95, //3
		0, //4
		0, //5
		-2.88, //6
		-5.12, //7
		0, //8
		0, //9
		0, //10
		-7.95, //11
		0, //12
		0, //13
		-0.74, //14
		-10, //15
		0, //16
		0, //17
		0, //18
		-7.95, //19
		0, //20
		0, //21
		-2.88, //22
		-5.12, //23
		2.88, //24
		2.88, //25
		2.88, //26
		11.39, //27
		0.74, //28
		0.74, //29
		-7.95, //30
		-9, //31
		0, //32
		-2.88, //33
		-5.12, //34
		0, //35
		0, //36
		0, //37
		0, //38
		0, //39
		0, //40
		-0.74, //41
		-10, //42
		7.95, //43
		7.95, //44
		7.95, //45
		0, //46
		7.95, //47
		0, //48
		0, //49
		-5.12, //50
		5.12, //51
		5.12, //52
		5.12, //53
		5.12, //54
		10, //55
		10, //56
		9, //57
		0 //58
	};
    */
	//
	const static int N_sns = 6;
    float erro_antigo = 0;
	float erro_antigo_alto = 0;
public:

	sensor sensores[N_sns];
    int limites[N_sns] = {100,100,100,100,100,100};

    sensores_frontais();
	void set_pinos(int pins[]);
	void reset();

    float erro_digital();
	float erro_analogico();
    
	int get_N_sns();
	float get_media();
    float get_max_media();
    float get_min_media();

    void ler_sensor(int n, bool fast);
    
    bool comp_max_value(int valor); 
};
#endif

