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
	sns_frontais.set_pinos(pinos);
	sensor_chegada.set_pin(pinos[6]);
	sensor_mapa.set_pin(pinos[7]);
	motor_dir.set_pins(pinos[8] , pinos[9] , pinos[10]);
	motor_esq.set_pins(pinos[11], pinos[12], pinos[13]);
	
	set_direcao('F');
}
//funcao para seguir a linha
float Seguidor_de_Linha::seguir_linha(){
	float erro = Kalman.updateEstimate(sns_frontais.erro_analogico());
	//float erro = sns_frontais.erro_analogico();

	if(erro == 111111){
		estado_s_chegada = 2;
		estado_s_mapa = 2;
		erro = 0;
	}
	else if(abs(erro) < 3){
		checar_chegada(); 
		//checar_secao();
	}
	else{
		//if(Controlador.get_controle_secao() == 1) Controlador.prox_secao();
		curva_time = millis();
	}
	checar_chegada(); 
	ADCSRA |= (1 << ADSC);
	Controlador.corrigir_trajeto(erro,&motor_dir, &motor_esq);
	return erro;
}

float Seguidor_de_Linha::seguir_linha_final(){
	float erro = sns_frontais.erro_analogico();
	ADCSRA |= (1 << ADSC);
	Controlador.corrigir_trajeto(erro,&motor_dir, &motor_esq);
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
	if (sensor_chegada.get_ult_leitura() >= MAX_PRETO_CHEGADA && (millis() - curva_time > 10))
	{
		if(estado_s_chegada == 0){
			qnt_linhas--;
			Serial.println("qnt_linha: " + (String)qnt_linhas);
			if (qnt_linhas == 0){
				Estado_corrida = false;
			}
			estado_s_chegada = 1;
		}
		else if(estado_s_chegada == 2) estado_s_chegada = 1;
		
	}
	else if(estado_s_chegada == 1) estado_s_chegada = 0;
}

//Mapeamento
void Seguidor_de_Linha::checar_secao()
{
	if (sensor_mapa.get_ult_leitura() >= MAX_PRETO_MAPA && (millis() - curva_time > 10))
	{
		if(estado_s_mapa == 0){
			Controlador.prox_secao();
			Serial.println("Secao" + (String)Controlador.get_secao());
			estado_s_mapa = 1;
		}
		else if(estado_s_mapa == 2) estado_s_mapa = 1;
		
	}
	else if(estado_s_mapa == 1) estado_s_mapa = 0;
}

