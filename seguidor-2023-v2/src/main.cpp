#include "seguidor.hpp"
Seguidor_de_Linha CEMLinha = Seguidor_de_Linha();
long cont = 0;
long cont2 = 0;
unsigned long tmp_mili = 0;

const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
 
void setup() {
	ADCSRA &= ~PS_128; 
	ADCSRA |= PS_16; // 16 prescaler Nao recomendado
	Serial.begin(9600);
	Serial.println(ADCSRA);
	tmp_mili = millis();
}
void loop()
{
	if (CEMLinha.Estado_corrida == true)
	{
		CEMLinha.modo = 'N';
		/*double erro = */CEMLinha.seguir_linha();
	}
	else
	{
		if(CEMLinha.tempo < CEMLinha.TMP_calib){
			CEMLinha.tempo++;
			CEMLinha.seguir_linha_final();
		}
		else{
			if(CEMLinha.modo != 'J'){
				CEMLinha.set_direcao('P');
				CEMLinha.set_velocidade_fast(0,0);
			}
		}
		
	}
	if(millis() - tmp_mili > 1000){
		tmp_mili = millis();
		CEMLinha.taxa_de_amostra = cont;
		cont = 0;
		Serial.println(CEMLinha.taxa_de_amostra);
	}
	cont++;
}
void serialEvent(){
	CEMLinha.comunica_serial();
}