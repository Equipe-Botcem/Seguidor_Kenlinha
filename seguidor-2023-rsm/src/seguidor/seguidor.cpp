#include <seguidor.h>

//Configuracao inicial
Seguidor_de_Linha::Seguidor_de_Linha()
{
    conf_pinos();
}
void Seguidor_de_Linha::conf_pinos(){
	pinMode(12, OUTPUT);
	digitalWrite(12, HIGH);
    for(int i = 0; i < 8; i++){
        pinMode(pinos[i], INPUT);
    }
    for(int i = 8; i < 14; i++){
        pinMode(pinos[i], OUTPUT);
    }
	motor_dir.set_pins(pinos[8] , pinos[9] , pinos[10]);
	motor_esq.set_pins(pinos[11], pinos[12], pinos[13]);
	sns_frontais.set_pinos(pinos);
	sensor_chegada.set_pin(pinos[6]);
	sensor_mapa.set_pin(pinos[7]);
	set_direcao('F');
}
//funcao para seguir a linha
double Seguidor_de_Linha::seguir_linha(){
	sns_frontais.ler_todos_fast(); // o fast tira 18 ifs em media - sem ifs
	//checar_perda_caminho(); // 14 ifs em media
	//sns_frontais.ler_todos_fast();
	checar_encruzilhada(); // 1 for e 4 ifs em media
	if(abs(erro_antigo) < 10)
		checar_chegada(); // 1 if em media
	
	double erro = calc_erro_digital(); // em edicao aprox 17 ifs
	double v_min = vel_min;

	//mapeamento
	
	if(seguir_mapa){
		checar_secao();
		if((mapa[secao_mapa] == 1) && (abs(erro) < 30)) {
			v_min = 220;
		}
	}
	/*else{
		if(abs(erro) > 30) mapa[secao_mapa] = 1;
	}*/

	set_velocidade_pid(erro, v_min); // 7 ifs em media e switch de 4 casos

	
	return erro;
}

double Seguidor_de_Linha::seguir_linha_final(){
	sns_frontais.ler_todos_fast(); // o fast tira 18 ifs em media - sem ifs
	double erro = calc_erro_digital(); // em edicao aprox 17 ifs
	if (abs(erro) > 3) erro = erro/abs(erro) * 3;
	double v_min = vel_max - 10;
	set_velocidade_pid(erro, v_min); // 7 ifs em media e switch de 4 casos
	return erro;
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

void Seguidor_de_Linha::set_velocidade(int vel_dir, int vel_esq){
	motor_dir.set_velocidade(vel_dir);
	motor_esq.set_velocidade(vel_esq);
}

void Seguidor_de_Linha::set_velocidade_fast(int vel_dir, int vel_esq){
	motor_dir.set_velocidade_fast(vel_dir);
	motor_esq.set_velocidade_fast(vel_esq);
}

//Condicao de parada
bool Seguidor_de_Linha::ignorar_leitura()
{
	if ((millis() - start_time) <= (300))
		return true;
	return false;
}
bool Seguidor_de_Linha::ignorar_leitura_mapa()
{
	if ((millis() - secao_time) <= (300))
		return true;
	return false;
}

void Seguidor_de_Linha::checar_chegada()
{
	if (sensor_chegada.ler_fast() >= MAX_PRETO_CHEGADA)
	{
		if (!ignorar_leitura())
		{
			qnt_linhas--;
			Serial.println("qnt_linha: " + (String)qnt_linhas);
			start_time = millis();
			if (qnt_linhas == 0){
				modo = 'P';
				Estado_corrida = false;
			}
		}
	}
}

void Seguidor_de_Linha::checar_secao()
{
	if (sensor_mapa.ler_fast() >= MAX_PRETO_MAPA && (!ignorar_leitura_mapa()))
	{
		secao_mapa++;
		secao_time = millis();
		Serial.println("Secao" + (String)secao_mapa);
	}
}

//Check de encruzilhada

void Seguidor_de_Linha::checar_encruzilhada()
{
	bool encruzilhada = true;
	for (unsigned int i = 0; i < 6; i++){
		if (sns_frontais.leituras[i] <= (MAX_PRETO)){
			encruzilhada = false;
			break;
		}
	}

	if(encruzilhada){
		start_time = millis();
	}
}

bool Seguidor_de_Linha::checar_fora_linha(){
	bool all_black = true;
	for(int i = 0; i < 6; i++){
		if(sns_frontais.leituras[i] > MAX_PRETO){
			if(sns_frontais.leituras[i] > sns_frontais.leituras[ultimo_sensor_na_linha]) ultimo_sensor_na_linha = i;
			all_black = false;
		}
	}
	return all_black;
}

//Metodo para retornar a linha apos perdida
void Seguidor_de_Linha::checar_perda_caminho(){
	
	if(!checar_fora_linha()){
		tmp_saida_linha = 0; //Adicionado
		return;
	}
	if(tmp_saida_linha == 0) tmp_saida_linha = millis();
	if((millis() - tmp_saida_linha) < TMP_calib) return;

	set_direcao('B');
	set_velocidade(0,0);
	if(ultimo_sensor_na_linha >= 3) {
		set_direcao('D');
		ultimo_sensor_na_linha = 0;
	}
	else {
		set_direcao('E');
		ultimo_sensor_na_linha = 5;
	}
	set_velocidade(120,120);

	//unsigned int tmp_girando = millis();
	while (sns_frontais.sensor3.ler() < MAX_PRETO); //&& (millis() - tmp_girando < 3000));
	set_direcao('B');
	set_velocidade(0,0);
	set_direcao('F');
	tmp_saida_linha = 0;

	sns_frontais.ler_todos_fast(); // trocado do seguir_linha
}

