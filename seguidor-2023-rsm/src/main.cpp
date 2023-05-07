#include "seguidor.h"
Seguidor_de_Linha CEMLinha = Seguidor_de_Linha();
long cont = 0;

const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
 
void setup() {
 	
 
	ADCSRA &= ~PS_128; 
 
	// PS_16, PS_32, PS_64 or PS_128
	//ADCSRA |= PS_128; // 128 prescaler
    //ADCSRA |= PS_64; // 64 prescaler
	//ADCSRA |= PS_32; // 32 prescaler
	ADCSRA |= PS_16; // 16 prescaler Nao recomendado
	Serial.begin(9600);
	Serial.println(ADCSRA);
	//tempo = millis();
	//mudança
}
void loop()
{
	
	
	if (CEMLinha.Estado_corrida == true)
	{
		double erro = CEMLinha.seguir_linha();
		/*if(cont == 150){
			Serial.print("erro ");
			Serial.println(erro);
			Serial.print("erro antigo ");
			Serial.println(CEMLinha.erro_antigo);
			Serial.print("Secao ");
			Serial.println(CEMLinha.secao_mapa);
			Serial.print("Dir: ");
			Serial.println(CEMLinha.motor_dir.get_velocidade());
			Serial.print("Esq: ");
			Serial.println(CEMLinha.motor_esq.get_velocidade());
			cont =0;
		}
		cont++;*/
	}
	else
	{
		if(CEMLinha.tempo < CEMLinha.TMP_calib){
			/*int vel_dir = - (CEMLinha.motor_dir.get_velocidade() / 2);
			int vel_esq = - (CEMLinha.motor_esq.get_velocidade() / 2);*/
			//CEMLinha.set_velocidade_fast(vel_dir, vel_esq);
			//delay(20);
			CEMLinha.tempo++;
			CEMLinha.seguir_linha_final();
			
			
		}
		else{
			CEMLinha.set_direcao('P');
			CEMLinha.set_velocidade_fast(0,0);
		}
		
	}
}
void serialEvent(){
	CEMLinha.comunica_serial();
}