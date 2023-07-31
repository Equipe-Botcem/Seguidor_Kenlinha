// Inclui as bibliotecas necessárias.
#include "Arduino.h"
#include <motor.hpp>
#include <sensor.hpp>
#include <sensores_frontais.hpp>
#include <controlador_PID.hpp>
#include "SimpleKalmanFilter.h"

// Verifica se a biblioteca já não foi definida.
#ifndef SDL_H
#define SDL_H

// Define a classe Seguidor_de_Linha.
class Seguidor_de_Linha
{
    // Define variáveis e objetos privados.
    private:
        controlador_PID Controlador; // Objeto para controle PID.
        motor motor_dir; // Objeto para controle do motor direito.
        motor motor_esq; // Objeto para controle do motor esquerdo.

        SimpleKalmanFilter Kalman = SimpleKalmanFilter(0.008,0.008,0.015); // Objeto para o filtro de Kalman.

        unsigned long start_time = 0; // Variável para armazenar o tempo de início.
        char modo = 'N'; // Variável para armazenar o modo de operação.

        // Parâmetros de calibração.
        float MAX_PRETO_CHEGADA = 100; // Valor máximo do sensor de chegada para preto.
        float MAX_PRETO_MAPA = 100; // Valor máximo do sensor de mapa para preto.
        float MAX_PRETO = 100; // Valor máximo do sensor para preto.
        int vel_calib = 130; // Velocidade de calibração.

        int estado_s_chegada = 0; // Estado do sensor de chegada.
        int estado_s_mapa = 0; // Estado do sensor de mapa.

        // Outras variáveis privadas...
        unsigned long tmp_fora_linha = 0;
        int qnt_linhas = 2;
        unsigned long curva_time = 0;
        char direcao_atual = 'F';
        char lado_pista = 'D';
        int pinos[14] = {A0,A1,A2,A3,A4,A5,A6, A7, 6, 10, 9, 5, 7, 8};

    // Define métodos e variáveis públicos.
    public:
        sensores_frontais sns_frontais; // Objeto para controle dos sensores frontais.
        sensor sensor_chegada; // Objeto para controle do sensor de chegada.
        sensor sensor_mapa; // Objeto para controle do sensor de mapa.
        bool ler_sensores_sem_pausa = false; // Variável para indicar se os sensores devem ser lidos sem pausa.
        bool ler_sensores_fast = false; // Variável para indicar se os sensores devem ser lidos rapidamente.

        unsigned long TMP_calib = 30000; // Tempo para calibração.
        unsigned long tempo = TMP_calib; // Variável para armazenar o tempo atual.
        bool Estado_corrida = false; // Variável para indicar o estado da corrida.

        Seguidor_de_Linha(); // Construtor da classe.

        char get_modo(); // Método para obter o modo de operação.
        int getpin(int pin); // Método para obter o número do pino de acordo com o índice fornecido.
        float seguir_linha(); // Método para seguir a linha.
        float seguir_linha_final(); // Método para seguir a linha até o final.

        // Métodos para verificação.
        void checar_chegada(); // Método para verificar a chegada.
        void checar_secao(); // Método para verificar a seção.

        // Métodos para comunicação.
        void ControlCMD(String command); // Método para controle de comando.
        void run(); // Método para executar o robô.
        void stop(String agente = "desconhecido"); // Método para parar o robô.
        
        void send_Consts(); // Método para enviar constantes.
        void set_Consts(String valores); // Método para definir constantes.

        // Métodos de teste.
        void teste_frontal(); // Método para teste frontal.
        void teste_lateral(); // Método para teste lateral.
        
        // Métodos de calibração.
        void calibracao(); // Método para calibração.
        int sensor_calib(int pos); // Método para calibração do sensor.
        
        // Método para controle do joystick.
        void joystick_control(String cmd);

        // Métodos para controle de direção e velocidade.
        void set_direcao(char direcao); // Método para definir a direção.
        void set_velocidade(int vel_dir, int vel_esq); // Método para definir a velocidade.
        void set_velocidade_fast(int vel_dir, int vel_esq); // Método para definir a velocidade rapidamente.
};

// Encerra a verificação se a biblioteca já foi definida.
#endif
