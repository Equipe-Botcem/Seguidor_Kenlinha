#ifndef BOTCEM_PID_H
#define BOTCEM_PID_H
/*Para usar a classe defina no inicio do codigo qual tipo de
microcontrolador esta usando, como abaixo:

#define ESP32 1
ou
#define Arduino 1

Agora basta criar um objeto PID e passar como parametros as
constantes do controle, dessa forma:

PID nome_do_objeto(KP, KI, KD);

O uso do objeto se baseia na funcao update que recebe o erro como
parametro e retorna a correcao PID para ele. Exemplo:

float correcao = controle.update(10);
*/
class PID
{
private:
    long tmp_last_att = 0;
    long getTime(); 
    
    float last_response;
    float last_error = 0;
    float integral = 0;
    
public:

    /*Se for igual a true, zera a integral toda vez que o erro muda de sentido*/
    bool reset_I_onSwitch = false;

    /*Limitador pro valor da integral*/
    float integral_limit = 0;
    float KP, KI, KD;

    /*Funcao que realiza o calculo e retorna o valor*/
    float update(float error);
    float getLastResponse();
    float getLastError();
    float getIntegralValue();

    /*Redefine os valores de resposta, erro anterior e a integral para 0*/
    void reset();
    PID(int kp, int ki, int kd);
};


#endif
