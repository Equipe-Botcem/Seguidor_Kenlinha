#include "seguidor.hpp"

#define TESTE_SENSOR 1;
#define TESTE_CTRL 1;
//#define TESTE_VEL 1;


#ifdef TESTE_SENSOR
//vars para testes
unsigned long cont = 0;
unsigned long cont2 = 0;
unsigned long tmp_mili;
#endif

int cont_sns = 0;
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

Seguidor_de_Linha CEMLinha = Seguidor_de_Linha();

void setup() {
	
	ADCSRA &= ~PS_128; 
	ADCSRA |= PS_16; // setando prescaler

	//interrupcao
	ADCSRA |= (1 << ADIE);
	sei();

	//setando para comecar no sensor 0
	ADMUX = (1 << 6) | ((CEMLinha.getpin(cont_sns) - 14) & 0x07);
	ADCSRA |= (0 << ADSC);

	Serial.begin(9600);
	Serial.println("CEMLinha PRONTO!");

	#ifdef TESTE_SENSOR
		tmp_mili = millis();

		#ifndef TESTE_VEL
		CEMLinha.ler_sensores_sem_pausa = true;
		ADCSRA |= (1 << ADSC);
		#endif
	#endif
	#ifdef TESTE_CTRL
	//delay(4000);
	#endif
}

ISR(ADC_vect) {
  	// Read and process the ADC result
	if(cont_sns == 6){
		CEMLinha.sensor_chegada.ler();
	}
	else if(cont_sns == 7){
		CEMLinha.sensor_mapa.ler();
	}
	else{
		CEMLinha.sns_frontais.ler_sensor(cont_sns);
	}
	
	cont_sns++;
	if(cont_sns > 7){
		cont_sns = 0;
		ADMUX = (1 << 6) | ((CEMLinha.getpin(cont_sns) - 14) & 0x07);
		if(CEMLinha.ler_sensores_sem_pausa == false) ADCSRA |= (0 << ADSC);
		else ADCSRA |= (1 << ADSC);
	}
	else{
		ADMUX = (1 << 6) | ((CEMLinha.getpin(cont_sns) - 14) & 0x07); // Switch to A<number>
		// Start the next ADC conversion
		ADCSRA |= (1 << ADSC);
		
	}
	#ifdef TESTE_SENSOR
	cont++;
	#endif
}
#ifdef TESTE_CTRL
controlador_PID control_teste;
#endif

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

	#ifdef TESTE_CTRL
	volatile float erro_anl = CEMLinha.sns_frontais.erro_analogico();
	volatile float corr = control_teste.get_correcao(erro_anl);
	#endif

	// Teste 
	#ifdef TESTE_SENSOR
	if(millis() - tmp_mili > 50){
		
		tmp_mili = millis();
		//Serial.println((int)(cont/8));
		//Serial.println(cont2);
		cont = 0;
		cont2 = 0;
		#ifndef TESTE_CTRL
		Serial.println(CEMLinha.sns_frontais.sensores[0].get_ult_leitura());
		Serial.println(CEMLinha.sns_frontais.sensores[1].get_ult_leitura());
		Serial.println(CEMLinha.sns_frontais.sensores[2].get_ult_leitura());
		Serial.println(CEMLinha.sns_frontais.sensores[3].get_ult_leitura());
		Serial.println(CEMLinha.sns_frontais.sensores[4].get_ult_leitura());
		Serial.println(CEMLinha.sns_frontais.sensores[5].get_ult_leitura());
		#endif
		#ifdef TESTE_CTRL
		Serial.println(corr);
		Serial.println(erro_anl, 4);
		#endif
	}
	cont2++;
	#endif
}
void serialEvent(){
	if(Serial.available()) CEMLinha.ControlCMD(Serial.readStringUntil(';'));
}