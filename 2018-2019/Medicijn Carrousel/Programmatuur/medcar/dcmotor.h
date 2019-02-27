// file:   dcmotor.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef DCMOTOR_H
#define DCMOTOR_H

#include "actuator.h"

class DcMotor : public Actuator
{
public:
    DcMotor();
    ~DcMotor();

    void setPin( unsigned int pin);

    void setOn();
    void setOff();

protected:
    int m_pin;
};

#endif // DCMOTOR_H
