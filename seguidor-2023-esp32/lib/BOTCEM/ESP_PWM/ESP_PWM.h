#ifndef BOTCEM_ESP_PWM_H
#define BOTCEM_ESP_PWM_H
/*Para usar essa classe declare a variavel pinChannels[16] antes de
usar outras funcoes, dessa forma:

int ESP_PWM::pinChannels[16];

O uso dessa classe e simples:
- use a funcao enableChannel passando como parametros o pino para a
saida pwm e o canal pwm do esp32 (geralmente de 0-15 ou 0-7)
- mude a intensidade (duty cycle) com a funcao setDuty passando
como parametros o pino e o duty (por padrao de 0 a 8191)

Exemplo de uso:

ESP_PWM::enableChannel(10, 0); //Canal 0 habilitado no pino 10
ESP_PWM::setDuty(10, 4095); 

Tambem e possivel alterar a frequencia, resolucao do duty, etc
Para isso e so passar os parametros correspondentes para enableChannel

Obs: Lembre-se de, quando usar mais de um canal, se atentar a parametros
que sao passados tambem para o timer usado, ja que se dois canais usarem
o mesmo timer, estarao compartilhando alguns recursos deste timer
*/
class ESP_PWM
{
private:
    /* data */
public:
    ESP_PWM(/* args */);
    ~ESP_PWM();
    static int pinChannels[16];
    static void enableChannel(int pin, int channel, int freq = 4000, int resolution = 13, int timer = 0, int clock = 0, int speed_mode = 0);
    static void setDuty(int pin, int duty, int speed_mode = 0);
};



#endif