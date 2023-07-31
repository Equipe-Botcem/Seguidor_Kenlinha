// Construtor
SimpleKalmanFilter::SimpleKalmanFilter(float mea_e, float est_e, float q)
{
  _err_measure = mea_e;        // Erro de medição
  _err_estimate = est_e;       // Erro de estimativa
  _q = q;                      // Ruído do processo
}

// Método para atualizar a estimativa com base em uma nova medição
float SimpleKalmanFilter::updateEstimate(float mea)
{
  // Cálculo do ganho de Kalman
  _kalman_gain = _err_estimate/(_err_estimate + _err_measure);
  
  // Atualização da estimativa atual com base na medição e na última estimativa
  _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
  
  // Atualização do erro de estimativa
  _err_estimate =  (1.0f - _kalman_gain) * _err_estimate + fabsf(_last_estimate - _current_estimate) * _q;
  
  // Atualização da última estimativa
  _last_estimate = _current_estimate;

  return _current_estimate;
}

// Métodos para ajustar os parâmetros do filtro
void SimpleKalmanFilter::setMeasurementError(float mea_e)
{
  _err_measure = mea_e;
}

void SimpleKalmanFilter::setEstimateError(float est_e)
{
  _err_estimate = est_e;
}

void SimpleKalmanFilter::setProcessNoise(float q)
{
  _q = q;
}

// Métodos para recuperar valores do filtro
float SimpleKalmanFilter::getKalmanGain() 
{
  return _kalman_gain;
}

float SimpleKalmanFilter::getEstimateError() 
{
  return _err_estimate;
}
