// file:   buzzer.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef BUZZER_H
#define BUZZER_H

#include "actuator.h"

class Buzzer : public Actuator
{
public:
    Buzzer();
    ~Buzzer();

    void setPin( unsigned int pin);

    void setOn();
    void setOff();

protected:
    int m_pin;
};

#endif // BUZZER_H
