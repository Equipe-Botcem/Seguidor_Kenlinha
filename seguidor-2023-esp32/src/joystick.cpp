#include "seguidor.hpp"
#include <string>
#include <ctype.h>
using namespace std;
void Seguidor_de_Linha::joystick_control(string cmd){
    if(Estado_corrida) return;
    if(modo != 'J'){
        motor_dir.ativar();
        motor_esq.ativar();
    }
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
    if(abs(x) <= 5 && (abs(y) <= 5)){
        set_direcao('B');
        setVel(0,0);
        motor_dir.desativar();
        motor_esq.desativar();
        modo = 'B';
        return;
    }
    
    int vel_max_joystick = 2;
    float fat_drc = y/raio;
    float fat_rot = x/raio;

    float vel_drc = -fat_drc * vel_max_joystick + (-fat_drc > 0? 0.5:-0.5);
    float vel_rot = -fat_rot * vel_max_joystick + (-fat_rot > 0? 0.5:-0.5);


	set_direcao('F');
    if(abs(fat_rot) < 0.1){
        vel_drc +=(-fat_drc > 0? 0.5:-0.5);
        setVel(vel_drc, vel_drc);
    }
    else if(abs(fat_drc) < 0.1){
        setVel(vel_rot, -vel_rot);
    }
    else if(vel_rot > 0){
        setVel(vel_drc + vel_rot, vel_drc);
    }
    else{
        setVel(vel_drc, vel_drc -vel_rot);
    }
    
    
    //delay(50);
    //vTaskDelay(10/portTICK_PERIOD_MS);
}