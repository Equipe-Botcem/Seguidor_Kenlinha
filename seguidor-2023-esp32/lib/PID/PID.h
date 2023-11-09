#ifndef BOTCEM_PID_H
#define BOTCEM_PID_H
class PID
{
private:
    long tmp_last_att = 0;

    float last_response;
    int last_erro;
    int integral = 0;
    
public:
    float KP, KI, KD;

    float update(int error);
    float getLastResponse();
    void reset();
    PID(int kp, int ki, int kd);
    ~PID();
};


#endif
