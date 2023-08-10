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
	
	pinMode(11, INPUT);
	digitalWrite(11, LOW);
	set_direcao('F');
}
//funcao para seguir a linha
float Seguidor_de_Linha::seguir_linha(){
	float erro = sns_frontais.erro_analogico();
	float erro_filtrado = Kalman.updateEstimate(erro);

	if(erro == 111111){
		//Serial.println("encruzilhada");
		estado_s_chegada = 2;
		estado_s_mapa = 2;
		erro_filtrado = 0;
	}
	else{
		curva_time = millis();
	}
	
	checar_chegada();
	checar_secao();
	ADCSRA |= (1 << ADSC);
	if(abs(erro) == 30) Controlador.corrigir_trajeto(erro,&motor_dir, &motor_esq);
	else Controlador.corrigir_trajeto(erro_filtrado,&motor_dir, &motor_esq);
	return erro;
}

float Seguidor_de_Linha::seguir_linha_final(){
	float erro = Kalman.updateEstimate(sns_frontais.erro_analogico());
	ADCSRA |= (1 << ADSC);
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
		if(estado_s_chegada == 0){
			if(marcacoes_chegada == 0) start_time = millis();

			if(qnt_linhas > 1) qnt_linhas--;
			else if(((millis() - start_time) >= TMP_calib)) stop("Sensor Chegada");
			marcacoes_chegada++;

			Serial.println("Linhas de Chegada faltando: " + (String)qnt_linhas +
							"\nMarcacoes lidas: " + String(marcacoes_chegada));
			estado_s_chegada = 1;
		}
		else if(estado_s_chegada == 2) estado_s_chegada = 1;
		
	}
	else if(estado_s_chegada == 1) estado_s_chegada = 0;
}

//Mapeamento
void Seguidor_de_Linha::checar_secao()
{
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

