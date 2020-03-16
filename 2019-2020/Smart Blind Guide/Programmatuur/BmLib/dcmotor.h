// file:   dcmotor.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef DCMOTOR_H
#define DCMOTOR_H

#include "actuator.h"

#define FULLSPEED 1023

class DcMotor : public Actuator
{
public:
    DcMotor();
    ~DcMotor();

    void setPin( unsigned int pin, bool pwm = true);

    void setSpeed( int speed);
    void setOn();
    void setOff();

protected:
    int  m_pin;
    int  m_speed;
    bool m_pwm;
};

#endif // DCMOTOR_H
