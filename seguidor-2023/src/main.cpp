#include "seguidor.h"
double erro = 0;
Seguidor_de_Linha Omega;
void setup()
{
	Serial.begin(9600);
}
void loop()
{
	
	if(Omega.modo != 'J'){
		if (Omega.Estado_corrida)
		{
			if (!Omega.stop_condition)
			{
				erro = 0;
				//Controlador P
				//erro = Omega.calc_erro();
				//Omega.motor_controler(erro);

				//Controlador PID
				erro = Omega.calc_erro2();
				Omega.set_velocidade(erro);
			}
			else
			{
				delay(100);
				Omega.set_direcao('P');
			}
		}
	}
}
void serialEvent(){
	Omega.comunica_serial();
}