#include "seguidor.hpp"

unsigned long cont = 0;
unsigned long cont2 = 0;
int cont_sns = 0;
unsigned long tmp_mili;
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

sensores_frontais Seguidor_de_Linha::sns_frontais;
sensor Seguidor_de_Linha::sensor_chegada;
sensor Seguidor_de_Linha::sensor_mapa;
bool Seguidor_de_Linha::ler_sensores_sem_pausa = false;
Seguidor_de_Linha CEMLinha = Seguidor_de_Linha();
void setup() {

	ADCSRA &= ~PS_128; 
	ADCSRA |= PS_16; // 16 prescaler Nao recomendado
	//interrupcao
	ADCSRA |= (1 << ADIE);
	sei();

	Serial.begin(115200);
	Serial.println(ADCSRA);
	tmp_mili = millis();
	
	//setando para comecar no sensor 0
	ADMUX = (ADMUX & 0xF0) | (cont_sns);
	ADCSRA |= (0 << ADSC);
}

ISR(ADC_vect) {
  	// Read and process the ADC result
	if(cont_sns == 6){
		Seguidor_de_Linha::sensor_chegada.ler();
	}
	else if(cont_sns == 7){
		Seguidor_de_Linha::sensor_mapa.ler();
	}
	else{
		Seguidor_de_Linha::sns_frontais.ler_sensor(5 - cont_sns);
	}
	cont_sns++;
	if(cont_sns > 7){
		cont_sns = 0;
		ADMUX = (ADMUX & 0xF0) | (cont_sns);
		if(Seguidor_de_Linha::ler_sensores_sem_pausa == false) ADCSRA |= (0 << ADSC);
		else ADCSRA |= (1 << ADSC);
	}
	else{
		ADMUX = (ADMUX & 0xF0) | (cont_sns); // Switch to A<number>
		// Start the next ADC conversion
		ADCSRA |= (1 << ADSC);
		
	}
	cont ++;
}
void loop()
{
	
	
	if (CEMLinha.Estado_corrida == true)
	{
		CEMLinha.seguir_linha();
	}
	else
	{
		if(CEMLinha.tempo < CEMLinha.TMP_calib){
			CEMLinha.tempo++;
			CEMLinha.seguir_linha_final();
		}
		else if(CEMLinha.get_modo() != 'P'){
			CEMLinha.set_direcao('P');
			CEMLinha.set_velocidade_fast(0,0);
		}
		
	}
	if(millis() - tmp_mili > 1000){
		tmp_mili = millis();
		Serial.println((int)(cont/8));
		Serial.println(cont2);
		/*Serial.print((String)Seguidor_de_Linha::sns_frontais.sensores[0].get_pin() + " - "); Serial.println(Seguidor_de_Linha::sns_frontais.leituras[0]);
		Serial.print((String)Seguidor_de_Linha::sns_frontais.sensores[1].get_pin() + " - ");Serial.println(Seguidor_de_Linha::sns_frontais.leituras[1]);
		Serial.print((String)Seguidor_de_Linha::sns_frontais.sensores[2].get_pin() + " - ");Serial.println(Seguidor_de_Linha::sns_frontais.leituras[2]);
		Serial.print((String)Seguidor_de_Linha::sns_frontais.sensores[3].get_pin() + " - ");Serial.println(Seguidor_de_Linha::sns_frontais.leituras[3]);
		Serial.print((String)Seguidor_de_Linha::sns_frontais.sensores[4].get_pin() + " - ");Serial.println(Seguidor_de_Linha::sns_frontais.leituras[4]);
		Serial.print((String)Seguidor_de_Linha::sns_frontais.sensores[5].get_pin() + " - ");Serial.println(Seguidor_de_Linha::sns_frontais.leituras[5]);
		*/
		cont = 0;
		cont2 = 0;
		
	}
	cont2++;
}
void serialEvent(){
	CEMLinha.comunica_serial();
}