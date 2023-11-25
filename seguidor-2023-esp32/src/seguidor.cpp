#include <seguidor.hpp>
#include <esp_system.h>
#include <driver/adc.h>
#include <driver/gpio.h>
#include "esp_timer.h"
#include <ESP_PWM/ESP_PWM.h>
#include <SimpleKalmanFilter/SimpleKalmanFilter.h>
#include "nvs_flash.h"
#include "nvs.h"
//Configuracao inicial
Seguidor_de_Linha::Seguidor_de_Linha()
{	

	sns_frontais.set_pinos(pinos);
	sensor_chegada.set_pin(pinos[8]);
	sensor_mapa.set_pin(pinos[9]);

	motor_dir.set_pins(LEDC_CHANNEL_0, LEDC_CHANNEL_1, pinos[10], pinos[11], 16, 15);  
	gpio_set_direction((gpio_num_t)pinos[12], GPIO_MODE_OUTPUT);    
    gpio_set_level((gpio_num_t)pinos[12], 1);          
	motor_esq.set_pins(LEDC_CHANNEL_2, LEDC_CHANNEL_3, pinos[13], pinos[14], 17, 18);

	//ventoinhas
	

	//gpio_set_direction((gpio_num_t)36, GPIO_MODE_OUTPUT);    
    //gpio_set_level((gpio_num_t)36, 1);

	//gpio_set_direction((gpio_num_t)38, GPIO_MODE_OUTPUT);    
    //gpio_set_level((gpio_num_t)38, 1);

	//ventoinhaD.set_pins(LEDC_CHANNEL_4, LEDC_CHANNEL_5,pinos_ventoinha[0], pinos_ventoinha[1]);
	//ventoinhaE.set_pins(LEDC_CHANNEL_6, LEDC_CHANNEL_7,pinos_ventoinha[3], pinos_ventoinha[4]);

	//Sensor lateral
    adc2_config_channel_atten(ADC2_CHANNEL_1,ADC_ATTEN_DB_0);
	
	
}
void Seguidor_de_Linha::init(){
	Controlador.loadMap();
}
void Seguidor_de_Linha::intr_enc_dir(){
	motor_dir.encoder();
}
void Seguidor_de_Linha::intr_enc_esq(){
	motor_esq.encoder();
}

void Seguidor_de_Linha::loop(){
	motor_dir.updateVel();
	motor_esq.updateVel();
	updateOutput();
}

void Seguidor_de_Linha::printEncoders(){
	output(to_string(Controlador.get_secao()));
	output(to_string(motor_dir.vel_real) + "m/s");
	output(to_string(motor_esq.vel_real) + "m/s");
}
void Seguidor_de_Linha::printError(){
	
	output(to_string(sns_frontais.erro_analogico()));
}


void Seguidor_de_Linha::set_BLE_CHAR(BLECharacteristic * Canal){
	esp_com = Canal;
}
//funcao para seguir a linha
float Seguidor_de_Linha::seguir_linha(){
	float erro = sns_frontais.erro_analogico();
	float erro_filtrado = Kalman.updateEstimate(erro);
	//float erro = sns_frontais.erro_analogico();

	if(erro == 111111){
		if(sensor_chegada.get_ult_leitura() >= MAX_PRETO_CHEGADA){
			//stop("Todos no branco");
		}
		//estado_s_chegada = 2;
		//estado_s_mapa = 2;
		erro_filtrado = 0;
		//output("\nEncruzilhada");
	}
	else{
		curva_time = esp_timer_get_time()/1000;
	}
	//if(abs(erro) < 4) checar_chegada();
	//set_ventoinha(9000 + 300*abs(erro));
	
	if(abs(erro) < 4 || (erro == 111111)) checar_chegada();
	checar_secao();
	
	/*if(abs(erro) > 2){
		set_ventoinha(15);
	}
	else{ set_ventoinha(5);
	}*/

	if(abs(erro) == 30){
		Controlador.corrigir_trajeto(erro,&motor_dir, &motor_esq);
		//output("Saiu da linha");
		//output(to_string(motor_dir.get_velocidade()) + " | " + to_string(motor_esq.get_velocidade()));
	}
	else Controlador.corrigir_trajeto(erro_filtrado,&motor_dir, &motor_esq);

	/*if((motor_dir.getPosicao() > 340) && (motor_esq.getPosicao() > 340)){
		stop("1 Metro");
		motor_dir.printencoder();
		motor_esq.printencoder();
	}*/
	
	return erro;
}

float Seguidor_de_Linha::seguir_linha_final(){
	float erro = Kalman.updateEstimate(sns_frontais.erro_analogico());
	Controlador.corrigir_trajeto_sem_mover(erro,&motor_dir, &motor_esq);
	return erro;
}
int Seguidor_de_Linha::getpin(int pin){
	return pinos[pin];
}
char Seguidor_de_Linha::get_modo(){
	return modo;
}

//Controle dos motores
void Seguidor_de_Linha::set_direcao(char direcao = 'F'){
    switch (direcao)
    {
	case 'D': //Direita
		motor_dir.set_direcao('T');
		motor_esq.set_direcao('F');
        break;
    case 'E': //Esquerda
		motor_dir.set_direcao('F');
		motor_esq.set_direcao('T');
        break;
    default:
		motor_dir.set_direcao(direcao);
		motor_esq.set_direcao(direcao);
        break;
    }
	direcao_atual = direcao;
}
void Seguidor_de_Linha::set_ventoinha(int vel){

	if (vel_ventoinha == vel) return;
	vel_ventoinha = vel;
	ESP_PWM::enableChannel(13, 4, 50, 13, 1);
    ESP_PWM::enableChannel(14, 5, 50, 13, 1);
	long tmp = esp_timer_get_time();
    
    ESP_PWM::setDuty(14, 8191 - 8*76);
    ESP_PWM::setDuty(13, 8191 - 8*76);
    while ((esp_timer_get_time() - tmp)/1000 < 3000);

    ESP_PWM::setDuty(14, 8191 - 8*(76 - vel));
    ESP_PWM::setDuty(13, 8191 - 8*(76 - vel));

	printf("\n%i\n", vel);
	
	//ventoinhaD.set_velocidade(-vel);
	//ventoinhaE.set_velocidade(-vel);
}
void Seguidor_de_Linha::set_velocidade(int vel_dir, int vel_esq){
	motor_dir.set_velocidade(vel_dir);
	motor_esq.set_velocidade(vel_esq);
}

void Seguidor_de_Linha::set_velocidade_fast(int vel_dir, int vel_esq){
	motor_dir.set_velocidade_fast(vel_dir);
	motor_esq.set_velocidade_fast(vel_esq);
}
//Condicao de parada

void Seguidor_de_Linha::checar_chegada()
{
	/*int saida = 0;
	adc2_get_raw(ADC2_CHANNEL_1, ADC_WIDTH_BIT_12, &saida);
	sensor_chegada.ler(saida);*/
	int leitura = sensor_chegada.get_ult_leitura();
	bool cond_tmp = (esp_timer_get_time()/1000 - ult_leitura_chg > 50);
	//output(to_string(leitura));
	if ((leitura > MAX_PRETO_CHEGADA) && cond_tmp)
	{
		
		if(estado_s_chegada == 0){
			ult_leitura_chg = esp_timer_get_time()/1000;
			if(marcacoes_chegada == 0) start_time = esp_timer_get_time()/1000;

			if(enc_chegada > 1) enc_chegada--;
			else if(enc_chegada < 1) return;
			else if(((esp_timer_get_time()/1000 - start_time) >= 50)){
				enc_chegada--;
				long tempo_para_n_perder_volta = esp_timer_get_time()/1000;
				while ((esp_timer_get_time()/1000 - tempo_para_n_perder_volta) <= 500)
				{
					seguir_linha();
				}
				
				stop("Sensor Chegada");
				
			}
			marcacoes_chegada++;

			
			/*output("Linhas de Chegada faltando: " + to_string(enc_chegada) +
							"\nMarcacoes lidas: " + to_string(marcacoes_chegada)+
							"\nLeitura: " + to_string(leitura));*/
			estado_s_chegada = 1;
		}
		else if(estado_s_chegada == 2 && cond_tmp) estado_s_chegada = 1;
		
	}
	else if(estado_s_chegada == 1 && cond_tmp) {
		estado_s_chegada = 0;
		//output("\nEstado 0");
	}
}

//Mapeamento
void Seguidor_de_Linha::checar_secao()
{
	int saida = 0;
	adc2_get_raw(ADC2_CHANNEL_2, ADC_WIDTH_BIT_12, &saida);
	sensor_mapa.ler(saida);
	if (sensor_mapa.get_ult_leitura() >= MAX_PRETO_MAPA)
	{
		if(estado_s_mapa == 0){
			if(Controlador.get_estado_secao() == false) Controlador.prox_secao();
			estado_s_mapa = 1;
		}
		else if(estado_s_mapa == 2) estado_s_mapa = 1;
		
	}
	else if(estado_s_mapa == 1) estado_s_mapa = 0;
}

