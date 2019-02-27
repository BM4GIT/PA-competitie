// file:   led.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef LED_H
#define LED_H

#include "actuator.h"

class Led : public Actuator
{
public:
    Led();
    ~Led();

    void setPin( unsigned int pin);

    void setOn();
    void setOff();

protected:
    int m_pin;
};

#endif // LED_H
