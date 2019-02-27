
// file:   led.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef LED_H
#define LED_H

#include "actuator.h"

#define BRIGHT 100
#define NOBLINK 0

class Led : public Actuator
{
    Q_OBJECT

public:
    Led();
    ~Led();

    void setPin( unsigned int pin);

    void setBrightness( int on = BRIGHT, int off = 0); // percentage
    void setBlink( int on = NOBLINK, int off = NOBLINK ); // millisec
    void setOn();
    void setOff();

private slots:
    void blink();

private:
    int  m_pin;
    int  m_id;
    int  m_max;
    int  m_min;
    int  m_ton;
    int  m_toff;
    int  m_blink;
    bool m_isblinking;
};

#endif // LED_H
