#include "sensor.hpp"


sensor::sensor()
{
    ultima_leitura = -1;  penultima_leitura = -1; valor_max_lido = -1; valor_min_lido = 1024;
    pin_dado = -1;
}
int sensor::ler(){
    penultima_leitura = ultima_leitura;
    if(pin_dado == -1) return -1;
    ultima_leitura = 1023 - analogRead(pin_dado);
    if(ultima_leitura > valor_max_lido) valor_max_lido = ultima_leitura;
    if(ultima_leitura < valor_min_lido) valor_min_lido = ultima_leitura;
    return ultima_leitura;
}

int sensor::ler_fast(){
    ultima_leitura = 1023 - analogRead(pin_dado);
    return ultima_leitura;
}
float sensor::calc_limite(){
    return (valor_max_lido - valor_min_lido)/2 + valor_min_lido;
}
void sensor::reset(){
    ultima_leitura = -1;  penultima_leitura = -1; valor_max_lido = -1; valor_min_lido = 1024;
}
void sensor::set_pin(int pin){ pin_dado = pin;}
int sensor::get_max_leitura(){ return valor_max_lido;}
int sensor::get_min_leitura(){ return valor_min_lido;}
int sensor::get_ult_leitura(){ return ultima_leitura;}
int sensor::get_pen_leitura(){ return penultima_leitura;}
int sensor::get_pin(){ return pin_dado;}