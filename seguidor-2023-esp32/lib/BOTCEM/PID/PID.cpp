#include "PID.h"
#ifdef ESP32
#include "esp_timer.h"
#endif

PID::PID(int kp, int ki, int kd){
    KP = kp; KI = ki; KD = kd;
    reset();
    tmp_last_att = getTime();
}


float PID::update(float error){
    float dt = (getTime() - tmp_last_att)/1000000;
    if (dt == 0) dt = 0.0000001; // improvavel

    tmp_last_att = getTime();
    
    if(reset_I_onSwitch){
        if(integral > 0 && ((error -last_error) < 0)) integral = 0;
        else if(integral < 0 && ((error -last_error) > 0)) integral = 0;
    }
    integral += error * dt;

    int sinal = integral > 0 ? 1:-1;

    if (integral*sinal > integral_limit && integral_limit > 0) integral = sinal *integral_limit;
    
    last_response = KP*error + KI*integral + KD*(error-last_error)/dt;

    last_error = error;

    return last_response;
}
float PID::getLastResponse(){
    return last_response;
}
float PID::getLastError(){
    return last_error;
}
float PID::getIntegralValue(){
    return integral;
}


void PID::reset(){
    last_response = 0;
    last_error = 0;
    integral = 0;
}
long PID::getTime(){
    #ifdef ESP32
    return esp_timer_get_time();
    #endif
    #ifdef Arduino
    return micros();
    #endif
    return 0;
}