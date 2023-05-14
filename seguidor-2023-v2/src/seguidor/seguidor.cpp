#include <seguidor.hpp>

//Configuracao inicial
Seguidor_de_Linha::Seguidor_de_Linha()
{
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
float Seguidor_de_Linha::seguir_linha(){
	float erro = sns_frontais.erro_digital();
	if(abs(erro) < 8){
		//checar_chegada(); // 1 if em media
	}
	else if(erro == 111111) erro = 0;
	//mapeamento
	checar_secao();
	ADCSRA |= (1 << ADSC);
	Controlador.corrigir_trajeto(erro,vel_max,vel_min,&motor_dir, &motor_esq);
	return erro;
}

float Seguidor_de_Linha::seguir_linha_final(){
	float erro = sns_frontais.erro_digital();
	if (abs(erro) > 3) erro = erro/abs(erro) * 3;
	float v_min = vel_max - 10;
	Controlador.corrigir_trajeto(erro,vel_max,vel_min,&motor_dir, &motor_esq);
	return erro;
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
	if(direcao == 'P') modo = 'P';
	else if(direcao == 'F') modo = 'N';
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
	if (sensor_chegada.get_ult_leitura() >= MAX_PRETO_CHEGADA)
	{
		bool ignorar_leitura = (millis() - encru_time) <= (300) ? true : false;
		if (ignorar_leitura)
		{
			qnt_linhas--;
			Serial.println("qnt_linha: " + (String)qnt_linhas);
			encru_time = millis();
			if (qnt_linhas == 0){
				modo = 'P';
				Estado_corrida = false;
			}
		}
	}
}

void Seguidor_de_Linha::checar_secao()
{
	bool ignorar_leitura = (millis() - secao_time) <= (300) ? true : false;
	if (sensor_mapa.get_ult_leitura() >= MAX_PRETO_MAPA && (ignorar_leitura))
	{
		Controlador.prox_secao();
		secao_time = millis();
		Serial.println("Secao" + (String)Controlador.get_secao());
	}
}

