#include "PID.h"

PID::PID(int kp, int ki, int kd)
{
    KP = kp; KI = ki; KD = kd;
}

PID::~PID()
{
}