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
	if(esp_com != NULL && (millis >= 200) && (output_buffer !="")){
		
		esp_com->indicate();
		esp_com->setValue(output_buffer);
		output_buffer = "";
		tmp_ult_cmd = esp_timer_get_time();
	}
}
void Seguidor_de_Linha::output(string text, bool new_line){
	
	if(new_line) text+= "\n";
	text = ";;;" + text;
	printf("%s", text.c_str());
	output_buffer += text;
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
	const char TESTE_PWM = 'W';
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
	case TESTE_PWM:
		for(unsigned int i = 2; i < command.length(); i++){
			if(command[i] == '|'){
				set_velocidade(stoi(command.substr(2, i)),stoi(command.substr(2, i)));
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
	
	int vels[2]; Controlador.get_vel(vels);

	int vel_max = vels[0];
	int vel_min = vels[1];
	float TOL = sns_frontais.get_tol();
	/*string Str_mapa = "";
	int mapa[40] = {0}; Controlador.get_mapa(mapa);
	for (int i =0; i < 40; i++){
		Str_mapa += (string)mapa[i] + "+";
	}*/
	output("T: Constantes: " + to_string_csd(Kp) + "|"+ to_string_csd(Ki)+"|"+to_string_csd(Kd)+"|"+to_string(vel_min)+"|"+to_string(vel_max)+"|"+
			to_string(TMP_calib)+"|"+to_string_csd(K)+"|"+to_string_csd(TOL)+"|" + (Controlador.get_estado_mapa() ? "Seguindo" : "Nao seguindo")+
			"|"+"Mapa"+ "|");
	
	output(
		("\nFrontal: " + to_string(MAX_PRETO) +
		" - Chegada: " + to_string(MAX_PRETO_CHEGADA) + 
		" - Mapa: " + to_string(MAX_PRETO_MAPA)).c_str()); 
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
	Controlador.reset();
	vTaskDelay(200 / portTICK_PERIOD_MS);
	send_Consts();
}

void Seguidor_de_Linha::stop(string agente)
{
	set_direcao('B');
	modo = 'B';
	ler_sensores_fast = true;
	ler_sensores_sem_pausa = false;
	send_Consts();
	//output(("Tempo: " + to_string(esp_timer_get_time()/1000 - start_time)).c_str());
	output(("Stop - Agente: " + agente).c_str());
	//output(("Secao: " + to_string(Controlador.get_secao())).c_str());
	Controlador.reset();
	Estado_corrida = false;

	set_ventoinha(0);
}

void Seguidor_de_Linha::run()
{
	//delay(4te000);
	output("Chamado");
	
	vTaskDelay(200 / portTICK_PERIOD_MS);
	ler_sensores_fast =true;
	ler_sensores_sem_pausa = true;
	ler_sensores_sem_pausa = false;
	troca_max_min = false;

	set_direcao('F');
	modo = 'N';
	tempo = 0;
	tmp_fora_linha = 0;
	estado_s_chegada = 0;
	estado_s_mapa = 0;
	curva_time = 0;
	start_time = 0;
	marcacoes_chegada = 0;
	marcacoes_secao = 0;
	curva_time = 0;
	qnt_linhas = 2;//10
	enc_chegada=14;
	Controlador.reset();
	sns_frontais.semireset();
	Estado_corrida = true;
	
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
