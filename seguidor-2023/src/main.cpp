#include "seguidor.h"

double erro = 0;

Seguidor_de_Linha Omega;

void setup()
{
	// Inicializa a comunicação serial com o baud rate de 9600
	Serial.begin(9600);
}

void loop()
{
	// Verifica se o modo de operação do seguidor é diferente de joystick (controle manual)
	if(Omega.modo != 'J'){

		// Verifica se o seguidor está em estado de corrida
		if (Omega.Estado_corrida)
		{
			// Verifica se o seguidor não chegou ao fim do percurso
			if (!Omega.stop_condition)
			{
				erro = 0;
				
				// Controlador P (comentado por estar sendo utilizado o controlador PID)
				//erro = Omega.calc_erro();
				//Omega.motor_controler(erro);

				// Controlador PID
				erro = Omega.calc_erro2();
				Omega.set_velocidade(erro);
			}
			else
			{
				// Caso o seguidor tenha chegado ao fim do percurso, espera 100ms e muda a direção do motor para trás (B)
				delay(100);
				Omega.set_direcao('B');
			}
		}
	}
}

// Função que lida com a comunicação serial
void serialEvent(){
	Omega.comunica_serial();
}
