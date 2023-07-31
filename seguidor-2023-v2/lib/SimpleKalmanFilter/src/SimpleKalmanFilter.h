/*
 * SimpleKalmanFilter - a Kalman Filter implementation for single variable models.
 * Created by Denys Sene, January, 1, 2017.
 * Released under MIT License - see LICENSE file for details.
 */

// Proteção contra inclusão múltipla. Se a biblioteca já foi incluída, não a inclui novamente.
#ifndef SimpleKalmanFilter_h
#define SimpleKalmanFilter_h

// Definição da classe SimpleKalmanFilter.
class SimpleKalmanFilter
{
public:
  // Construtor que recebe três parâmetros: erro de medição, erro de estimativa e ruído do processo.
  SimpleKalmanFilter(float mea_e, float est_e, float q);
  
  // Método para atualizar a estimativa com base em uma nova medição.
  float updateEstimate(float mea);

  // Métodos para ajustar os parâmetros do filtro.
  void setMeasurementError(float mea_e);
  void setEstimateError(float est_e);
  void setProcessNoise(float q);

  // Métodos para recuperar valores do filtro.
  float getKalmanGain();
  float getEstimateError();

private:
  // Variáveis privadas para armazenar o estado do filtro.
  float _err_measure;        // Erro de medição.
  float _err_estimate;       // Erro de estimativa.
  float _q;                  // Ruído do processo.
  float _current_estimate;   // Estimativa atual.
  float _last_estimate;      // Última estimativa.
  float _kalman_gain;        // Ganho de Kalman.
};

// Fim da proteção contra inclusão múltipla.
#endif

