#include "seguidor.h"
void Seguidor_de_Linha::joystick_control(String cmd){
    if(Estado_corrida) return;
    modo = 'J';

	String dados_texto[10];
	double dados[10];
	bool novo_valor = false;
	int cont = 0;
	for(unsigned int i = 0; i < cmd.length(); i++){
		if(isDigit(cmd[i]) || (cmd[i] == '.') || (cmd[i] == '-') || (cmd[i] == '+')){
			dados_texto[cont] += cmd[i];
			novo_valor = true;
		}
		else if(novo_valor){
			novo_valor = false;
			dados[cont] = dados_texto[cont].toDouble();
			cont++;
		}
	}
	double x = dados[0], y = dados[1], raio = dados[2];

    if(x == 0 && (y == 0)){
        set_direcao('B');
        set_velocidade(0,0);
        return;
    }
    
    int vel_max_joystick = 200;
    double fat_drc = y/raio;
    double fat_rot = x/raio;

    int vel_drc = -fat_drc * vel_max_joystick;
    int vel_rot = abs(fat_rot * vel_max_joystick);
    if(fat_rot > 0.51){
        set_direcao('D');
        set_velocidade(vel_rot, vel_rot);
        
    }
	else if(fat_rot < -0.51){
        set_direcao('E');
        set_velocidade(vel_rot, vel_rot);
        
    }
    else{
		set_direcao('F');
        set_velocidade(vel_drc, vel_drc);
        
    }
    delay(50);

}