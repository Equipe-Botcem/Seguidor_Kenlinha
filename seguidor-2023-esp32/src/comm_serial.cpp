#include "seguidor.hpp"
#include <string>
#include <ctype.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ESP_PWM/ESP_PWM.h>
using namespace std;

template <typename T>
std::string to_string_csd(const T a_value, const int n = 2)
{	
	string cnv = to_string(a_value);
	int ponto = -1;
	for(int i = 0; i < cnv.length(); i++){
		if(cnv[i] == '.'){
			ponto = i;
		}
		else if(ponto != -1){
			if(i - ponto > n){
				return cnv.substr(0, i);
			}
		}
	}
	return cnv;
}

void Seguidor_de_Linha::updateOutput(){
	unsigned long millis = (esp_timer_get_time()- tmp_ult_cmd)/1000;
	if((millis >= 200) && (output_buffer !="")){
		if(esp_com != NULL){
			esp_com->indicate();
			esp_com->setValue(output_buffer);
		}
		if(wifi_saida != NULL){
			(*wifi_saida) = output_buffer;
		}
		output_buffer = "";
		tmp_ult_cmd = esp_timer_get_time();
	}

}
void Seguidor_de_Linha::output(string text, bool new_line){
	
	if(new_line) text+= "\n";
	text = ";;;" + text;
	printf("%s", text.c_str());
	output_buffer += text;
	tmp_ult_cmd = esp_timer_get_time();
}
void Seguidor_de_Linha::ControlCMD(string command)
{
	if(command[0] != 'J') output(command.c_str());
	else{
		printf(command.c_str());
	}
	const char SET = 'S';
	const char STOP = 'P';
	const char RUN = 'R';
	const char CALIBRACAO = 'C';
	const char SEND_CONST = 'T';
	const char TESTE_LATERAL = 'L';
	const char TESTE_FRONTAL = 'F';
	const char JOYSTICK = 'J';
	const char TESTE_VCT = 'W';
	const char TESTE_PWM = 'D';
	switch (command[0])
	{
	case SET:
		set_Consts(command);
		break;
	case STOP:
		stop("Controle");
		break;
	case RUN:
		run();
		break;
	case CALIBRACAO:
		if(command[1] != 'l')
			calibracao();
		break;
	case SEND_CONST:
		send_Consts();
		break;
	case TESTE_FRONTAL:
		teste_frontal();
		break;
	case TESTE_LATERAL:
		teste_lateral();
		break;	
	case JOYSTICK:
		joystick_control(command);
		break;
	case TESTE_VCT:
		for(unsigned int i = 1; i < command.length(); i++){
			if(command[i] == ';'){
				motor_dir.ativar();
				motor_esq.ativar();
				setVel(stof(command.substr(1, i)),stof(command.substr(1, i)));
				if (motor_dir.getVelObj() == 0){
					motor_dir.desativar();
					motor_esq.desativar();
				}
				break;
			}
		}
		output(to_string(motor_dir.get_velocidade()));
		break;
	case TESTE_PWM:
		for(unsigned int i = 1; i < command.length(); i++){
			if(command[i] == ';'){
				motor_dir.ativar();
				motor_esq.ativar();
				motor_dir.velControl = false;
				motor_esq.velControl = false;
				set_velocidade(stoi(command.substr(1, i)),stoi(command.substr(1, i)));
				if (motor_dir.get_velocidade() == 0){
					motor_dir.desativar();
					motor_esq.desativar();
					motor_dir.velControl = true;
					motor_esq.velControl = true;
				}
				break;
			}
		}
		output(to_string(motor_dir.get_velocidade()));
		break;
	default:
		output("Comando não reconhecido.");	
		break;
	}
}

void Seguidor_de_Linha::send_Consts(){

	float constsPID[4]; Controlador.get_const(constsPID);

	float Kp = constsPID[0], Ki = constsPID[1], Kd = constsPID[2], K = constsPID[3];
	
	float vels[2]; Controlador.get_vel(vels);

	int vel_max = vels[0];
	int vel_min = vels[1];
	float TOL = sns_frontais.get_tol();

	output("T: Constantes: " + to_string_csd(Kp) + "|"+ to_string_csd(Ki)+"|"+to_string_csd(Kd)+"|"+to_string(vel_min)+"|"+to_string(vel_max)+"|"+
			to_string(TMP_calib)+"|"+to_string_csd(K)+"|"+to_string_csd(TOL)+"|" + (Controlador.get_estado_mapa() ? "Seguindo" : "Nao seguindo")+
			"|"+"Mapa"+ "|");
}
void Seguidor_de_Linha::set_Consts(string valores)
{	
	float dados[10];
	int cont = 0;
	int pos_ini = -1;
	int cont_barras = 0;
	for(unsigned int i = 0; i < valores.length(); i++){
		if(valores[i] == '|'){
			cont_barras++;
			if(cont_barras == 8){
				dados[8] = (valores[i+1] == 'N' ? 0:1);
			}
		}
		if(isdigit(valores[i]) || (valores[i] == '.') || (valores[i] == '-') || (valores[i] == '+')){
			if(pos_ini == -1) pos_ini = i;
		}
		else if(pos_ini != -1){
			dados[cont] = stod(valores.substr(pos_ini, i+1));
			cont++;
			pos_ini = -1;
		}
	}
	
	Controlador.set_const(dados[0], dados[1], dados[2], dados[6]); //KP,KI,KD
	Controlador.set_vel(dados[4], dados[3]);
	TMP_calib = dados[5];
	sns_frontais.set_tol(dados[7]);

	Controlador.set_estado_mapa(dados[8] == 1 ? true : false);
	vTaskDelay(300 / portTICK_PERIOD_MS);
	send_Consts();
}

void Seguidor_de_Linha::stop(string agente)
{
	set_direcao('B');
	modo = 'B';
	ler_sensores_fast = true;
	ler_sensores_sem_pausa = false;
	//send_Consts();
	output(("Stop - Agente: " + agente).c_str());
	//Controlador.reset();
	Estado_corrida = false;

	set_ventoinha(0);
	string mp = "";
	int mapa[40];
	Controlador.get_mapa(mapa);
	if(agente != "Loop") Controlador.saveMap();
	for(int i=0; i < 40; i++){
		
		mp += to_string(mapa[i]) + ", ";
	}
	output(mp);
	output(to_string_csd(motor_dir.getPosicao()));
	output(to_string_csd(motor_esq.getPosicao()));

	motor_dir.desativar();
	motor_esq.desativar();
}

void Seguidor_de_Linha::run()
{
	//vTaskDelay(5000 / portTICK_PERIOD_MS);
	output("Chamado");
	setVel(0,0);
	motor_dir.resetEncoder(); motor_dir.ativar();
	motor_esq.resetEncoder(); motor_esq.ativar();

	

	vTaskDelay(200 / portTICK_PERIOD_MS);
	ler_sensores_fast = true;
	ler_sensores_sem_pausa = true;
	ler_sensores_sem_pausa = false;
	troca_max_min = false;

	set_direcao('F');
	modo = 'N';
	tempo = 0;
	estado_s_chegada = 0;
	estado_s_mapa = 0;
	curva_time = 0;
	start_time = 0;
	marcacoes_chegada = 0;
	marcacoes_secao = 0;
	cont_local = 0;
	curva_time = 0;
	enc_chegada = TMP_calib; //12
	Controlador.reset();
	sns_frontais.semireset();
	Estado_corrida = true;
	//set_ventoinha(15);
}

/**/
void Seguidor_de_Linha::teste_frontal(){
	double soma = 0;
	
	ler_sensores_fast = false;
	ler_sensores_sem_pausa = true;
	ler_sensores_sem_pausa = false;
	ler_sensores_fast = true;
	int n_sensores = sns_frontais.get_N_sns();
	for (int i = 0; i < n_sensores; i++)
	{
		
		soma += sns_frontais.sensores[i].get_ult_leitura();
		output((to_string(i) + " - " + to_string(sns_frontais.sensores[i].get_ult_leitura()) +
		" - Min: " + to_string(sns_frontais.sensores[i].get_min_leitura()) + 
		" - Max: " + to_string(sns_frontais.sensores[i].get_max_leitura()) + 
			+ " - " + to_string((int)(sns_frontais.sensores[i].get_ult_leitura_percent()*100)) + "%").c_str());
	}

	output("Resultado teste frontal: ");
	output(to_string(soma/n_sensores));
	output(("Angulo: " + to_string(sns_frontais.erro_analogico())).c_str());
}
void Seguidor_de_Linha::teste_lateral(){
	ler_sensores_sem_pausa = true;
	ler_sensores_sem_pausa = false;
	output(("Chegada: " + to_string(sensor_chegada.get_ult_leitura())).c_str());
	output(("Mapa: " + to_string(sensor_mapa.get_ult_leitura())).c_str());
}
