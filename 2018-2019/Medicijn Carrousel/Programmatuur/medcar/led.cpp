// file:   led.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "led.h"

Led::Led()
{
    m_pin = -1;
}

Led::~Led()
{
    setOff();
}

void Led::setPin( unsigned int pin)
{
    m_pin = pin;
    pinMode( pin, OUTPUT);
}

void Led::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    digitalWrite( m_pin, HIGH);
}

void Led::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    digitalWrite( m_pin, LOW);
}
