#include "sensor.hpp"


sensor::sensor()
{
    ultima_leitura = -1; valor_max_lido = -1; valor_min_lido = 1024;
    pin_dado = -1; dead_percent = 0;
}
int sensor::ler(){
    ultima_leitura = 1023 - analogRead(pin_dado);
    if(ultima_leitura > valor_max_lido) valor_max_lido = ultima_leitura;
    if(ultima_leitura < valor_min_lido) valor_min_lido = ultima_leitura;
    return ultima_leitura;
}

int sensor::ler_fast(){
    ultima_leitura = 1023 - analogRead(pin_dado);
    return ultima_leitura;
}
int sensor::calc_limite(float fator){
    if(fator == 0) fator = 1;
    return (valor_max_lido - valor_min_lido)/fator + valor_min_lido;
}
void sensor::reset(){
    ultima_leitura = -1; valor_max_lido = -1; valor_min_lido = 1024;
}
void sensor::set_pin(int pin){ pin_dado = pin;}
int sensor::get_max_leitura(){ return valor_max_lido;}
int sensor::get_min_leitura(){ return valor_min_lido;}
int sensor::get_ult_leitura(){ return ultima_leitura;}
float sensor::get_ult_leitura_percent(){

    
    float percent = 0;
    if(valor_max_lido != valor_min_lido) percent = (ultima_leitura -valor_min_lido)/float(valor_max_lido-valor_min_lido) - dead_percent;
    if(percent < 0) return 0;
    if(percent > 1 - dead_percent - 0.02) return 1 - dead_percent;
    return percent;
}
int sensor::get_pin(){ return pin_dado;}