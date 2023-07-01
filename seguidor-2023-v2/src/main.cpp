#include "seguidor.hpp"

//#define TESTE_SENSOR 1;
//#define TESTE_CTRL 1;
//#define TESTE_VEL 1;
//#define SEM_BLUETOOTH 1;


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

	#ifdef SEM_BLUETOOTH
	CEMLinha.ler_sensores_sem_pausa = true;
	ADCSRA |= (1 << ADSC);
	while (CEMLinha.sensor_chegada.get_ult_leitura() < 500) delay(1);
	delay(1000);
	CEMLinha.calibracao();
	pinMode(13, OUTPUT);
	
	CEMLinha.ler_sensores_sem_pausa = true;
	ADCSRA |= (1 << ADSC);
	CEMLinha.ler_sensores_fast = true;
	while (abs(CEMLinha.sns_frontais.erro_analogico()) > 1)
	{
		while(abs(CEMLinha.sns_frontais.erro_analogico()) > 1){
			digitalWrite(13,LOW);
			delay(100);
			digitalWrite(13, HIGH);
		}
		digitalWrite(13,HIGH);
		delay(3000);
	}
	CEMLinha.ler_sensores_fast = false;
	CEMLinha.ler_sensores_sem_pausa = false;
	ADCSRA |= (0 << ADSC);
	CEMLinha.run();
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
		CEMLinha.sns_frontais.ler_sensor(cont_sns, CEMLinha.ler_sensores_fast);
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
SimpleKalmanFilter kf = SimpleKalmanFilter(0.008,0.01,0.008);
#endif

void loop()
{
	if (CEMLinha.Estado_corrida == true)
	{
		CEMLinha.seguir_linha();
	}
	else
	{
		
		/*if(CEMLinha.tempo < CEMLinha.TMP_calib){
			CEMLinha.tempo++;
			CEMLinha.seguir_linha_final();
		}
		else */if(CEMLinha.get_modo() != 'B' && (CEMLinha.get_modo() != 'J')){
			CEMLinha.stop();
		}
		
	}

	#ifdef TESTE_CTRL
	volatile float erro_anl = CEMLinha.sns_frontais.erro_analogico();
	volatile float corr = control_teste.get_correcao(erro_anl);
	#endif

	// Teste 
	#ifdef TESTE_SENSOR
	if(millis() - tmp_mili > 10){
		
		tmp_mili = millis();
		//Serial.println((int)(cont/8));
		//Serial.println(cont2);
		cont = 0;
		cont2 = 0;
		#ifndef TESTE_CTRL
		Serial.println();
		Serial.print(CEMLinha.sns_frontais.sensores[0].get_ult_leitura()); Serial.print(" - ");  Serial.println(CEMLinha.sns_frontais.sensores[0].get_ult_leitura_percent());
		Serial.print(CEMLinha.sns_frontais.sensores[1].get_ult_leitura()); Serial.print(" - ");  Serial.println(CEMLinha.sns_frontais.sensores[1].get_ult_leitura_percent());
		Serial.print(CEMLinha.sns_frontais.sensores[2].get_ult_leitura()); Serial.print(" - ");  Serial.println(CEMLinha.sns_frontais.sensores[2].get_ult_leitura_percent());
		Serial.print(CEMLinha.sns_frontais.sensores[3].get_ult_leitura()); Serial.print(" - ");  Serial.println(CEMLinha.sns_frontais.sensores[3].get_ult_leitura_percent());
		Serial.print(CEMLinha.sns_frontais.sensores[4].get_ult_leitura()); Serial.print(" - ");  Serial.println(CEMLinha.sns_frontais.sensores[4].get_ult_leitura_percent());
		Serial.print(CEMLinha.sns_frontais.sensores[5].get_ult_leitura()); Serial.print(" - ");  Serial.println(CEMLinha.sns_frontais.sensores[5].get_ult_leitura_percent());
		Serial.println(CEMLinha.sns_frontais.erro_analogico());
		Serial.println(CEMLinha.sensor_chegada.get_ult_leitura());
		#endif
		#ifdef TESTE_CTRL
		//Serial.println(corr);
		float filtro = kf.updateEstimate(erro_anl);
		Serial.print(corr, 6);
		Serial.print(" ");
		Serial.println(filtro, 6);
		#endif
	}
	cont2++;
	#endif

}
void serialEvent(){
	if(Serial.available()) 
	{	
		CEMLinha.ControlCMD(Serial.readStringUntil(';'));
	}
}