#include "seguidor.hpp"
#include <string>
#include <ctype.h>
using namespace std;
void Seguidor_de_Linha::joystick_control(string cmd){
    if(Estado_corrida) return;
    modo = 'J';
	float dados[4];
	int cont = 0;
    int pos_ini = -1;
	for(unsigned int i = 0; i < cmd.length(); i++){
		if(isdigit(cmd[i]) || (cmd[i] == '.') || (cmd[i] == '-') || (cmd[i] == '+')){
			if(pos_ini == -1) pos_ini = i;
		}
		else if(pos_ini != -1){
			dados[cont] = stod(cmd.substr(pos_ini, i+1));
			cont++;
			pos_ini = -1;
		}
	}
	float x = dados[0], y = dados[1], raio = dados[2];
    if(x == 0 && (y == 0)){
        set_direcao('B');
        set_velocidade(0,0);
        modo = 'B';
        return;
    }
    
    int vel_max_joystick = 6000;
    float fat_drc = y/raio;
    float fat_rot = x/raio;

    int vel_drc = -fat_drc * vel_max_joystick + (-fat_drc > 0? 50:-50);
    int vel_rot = -fat_rot * vel_max_joystick + (-fat_rot > 0? 50:-50);


	set_direcao('F');
    if(abs(fat_rot) < 0.1){
        vel_drc +=(-fat_drc > 0? 50:-50);
        set_velocidade(vel_drc, vel_drc);
    }
    else if(abs(fat_drc) < 0.1){
        set_velocidade(vel_rot, -vel_rot);
    }
    else if(vel_rot > 0){
        set_velocidade(vel_drc + vel_rot, vel_drc);
    }
    else{
        set_velocidade(vel_drc, vel_drc -vel_rot);
    }
    
    
    //delay(50);
    //vTaskDelay(10/portTICK_PERIOD_MS);
}