// file:   led.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef LED_H
#define LED_H

#include "actuator.h"

#define BRIGHT 1023
#define NOBLINK 0

class Led : public Actuator
{
public:
    Led();
    ~Led();

    void setPin( unsigned int pin, bool pwm = false);
    void setInversion( bool invert = true);

    void setBrightness( int on = BRIGHT, int off = 0); // percentage
    void setBlink( int on = NOBLINK, int off = NOBLINK ); // millisec
    void setOn();
    void setOff();

private:
    int  m_pin;
    bool m_pwm;
    int  m_max;
    int  m_min;
    int  m_ton;
    int  m_toff;
    bool m_isblinking;
    bool m_invert;
};

#endif // LED_H
