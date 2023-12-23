#include "seguidor.hpp"
void Seguidor_de_Linha::joystick_control(String cmd){
    if(Estado_corrida) return;
    modo = 'J';

	String dados_texto[4];
	float dados[4];
	bool novo_valor = false;
    int cont_caracter = 0;
	int cont = 0;
	for(unsigned int i = 0; i < cmd.length(); i++){
		if(isDigit(cmd[i]) || (cmd[i] == '.') || (cmd[i] == '-') || (cmd[i] == '+')){
            if(cont_caracter < 4){
                cont_caracter++;
                dados_texto[cont] += cmd[i];
                novo_valor = true;
            }
		}
		else if(novo_valor){
            cont_caracter = 0;
			novo_valor = false;
			dados[cont] = dados_texto[cont].toDouble();
			cont++;
		}
	}
    dados[cont] = dados_texto[cont].toDouble();
	float x = dados[0], y = dados[1], raio = dados[2];
    if(x == 0 && (y == 0)){
        set_direcao('B');
        set_velocidade(0,0);
        modo = 'B';
        return;
    }
    
    int vel_max_joystick = 140;
    int vel_base = 70;
    float fat_drc = y/raio;
    float fat_rot = x/raio;

    int vel_drc = -fat_drc * (vel_max_joystick - vel_base) + (-fat_drc > 0? vel_base:-vel_base);
    int vel_rot = -fat_rot * (vel_max_joystick - vel_base) + (-fat_rot > 0? vel_base:-vel_base);


	set_direcao('F');
    if(abs(fat_rot) < 0.15){
        set_velocidade(vel_drc, vel_drc);
    }
    else if(abs(fat_drc) < 0.15){
        set_velocidade(vel_rot, -vel_rot);
    }
    else if(vel_rot > 0){
        set_velocidade(vel_drc + vel_rot, vel_drc);
    }
    else{
        set_velocidade(vel_drc, vel_drc -vel_rot);
    }
    
    
    //delay(50);

}