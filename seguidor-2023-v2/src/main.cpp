// Este código implementa o main loop de um seguidor de linha.
// Algumas partes do código foram comentadas para testes. 

#include "seguidor.hpp"

// As definições a seguir podem ser usadas para ativar diferentes partes do código para teste. 
// Elas estão comentadas por padrão.
//#define TESTE_SENSOR 1;
//#define TESTE_CTRL 1;
//#define TESTE_VEL 1;
//#define SEM_BLUETOOTH 1;

#ifdef TESTE_SENSOR
// Variáveis para testes
unsigned long cont = 0;
unsigned long cont2 = 0;
unsigned long tmp_mili;
#endif

// Contador para os sensores
int cont_sns = 0;

// Definições para o prescaler do ADC
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// Criação do objeto principal do seguidor de linha
Seguidor_de_Linha CEMLinha = Seguidor_de_Linha();

// Função setup, executada uma vez quando o programa inicia
void setup() {
	
	// Configuração do prescaler do ADC
	ADCSRA &= ~PS_128; 
	ADCSRA |= PS_16; 

	// Configuração da interrupção do ADC
	ADCSRA |= (1 << ADIE);
	sei();

	// Configuração inicial do ADC para ler o primeiro sensor
	ADMUX = (1 << 6) | ((CEMLinha.getpin(cont_sns) - 14) & 0x07);
	ADCSRA |= (0 << ADSC);

	// Inicialização da comunicação serial
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

// Interrupção do ADC
ISR(ADC_vect) {
	// Lê o valor do ADC e processa o resultado
	if(cont_sns == 6){
		CEMLinha.sensor_chegada.ler();
	}
	else if(cont_sns == 7){
		CEMLinha.sensor_mapa.ler();
	}
	else{
		CEMLinha.sns_frontais.ler_sensor(cont_sns, CEMLinha.ler_sensores_fast);
	}
	
	// Prepara para a próxima leitura do ADC
	cont_sns++;
	if(cont_sns > 7){
		cont_sns = 0;
		ADMUX = (1 << 6) | ((CEMLinha.getpin(cont_sns) - 14) & 0x07);
		if(CEMLinha.ler_sensores_sem_pausa == false) ADCSRA |= (0 << ADSC);
		else ADCSRA |= (1 << ADSC);
	}
	else{
		ADMUX = (1 << 6) | ((CEMLinha.getpin(cont_sns) - 14) & 0x07);
		ADCSRA |= (1 << ADSC);
	}
	#ifdef TESTE_SENSOR
	cont++;
	#endif
}

#ifdef TESTE_CTRL
// Para teste do controle
controlador_PID control_teste;
SimpleKalmanFilter kf = SimpleKalmanFilter(0.008,0.01,0.008);

#endif

// Função loop, executada repetidamente
void loop()
{
	#ifndef TESTE_SENSOR
	if (CEMLinha.Estado_corrida == true){
		CEMLinha.seguir_linha();
	}
	else if(CEMLinha.get_modo() != 'B' && (CEMLinha.get_modo() != 'J')){
		CEMLinha.stop("Loop");
	}
	#endif
	#ifdef TESTE_CTRL
	volatile float erro_anl = CEMLinha.sns_frontais.erro_analogico();
	volatile float corr = control_teste.get_correcao(erro_anl);
	#endif

	// Teste 
	#ifdef TESTE_SENSOR
	if(millis() - tmp_mili > 500){
		tmp_mili = millis();
		Serial.println();
		#ifndef TESTE_CTRL
		Serial.println(CEMLinha.sns_frontais.erro_analogico());
		Serial.println(CEMLinha.sensor_chegada.get_ult_leitura());
		#endif
		#ifdef TESTE_CTRL
		float filtro = kf.updateEstimate(erro_anl);
		Serial.print(corr, 6);
		Serial.print(" ");
		Serial.println(filtro, 6);
		#endif
	}
	cont2++;
	#endif
}

// Evento serial, chamado quando há dados disponíveis no Serial
void serialEvent(){
	if(Serial.available()) 
	{	
		CEMLinha.ControlCMD(Serial.readStringUntil(';'));
	}
}
