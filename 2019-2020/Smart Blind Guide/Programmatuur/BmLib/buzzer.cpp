// file:   buzzer.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "buzzer.h"

Buzzer::Buzzer()
{
    m_pin = -1;
}

Buzzer::~Buzzer()
{
    setOff();
}

void Buzzer::setPin( unsigned int pin)
{
    m_pin = pin;
    pinMode( pin, OUTPUT);
}

void Buzzer::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    digitalWrite( m_pin, HIGH);
}

void Buzzer::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    digitalWrite( m_pin, LOW);
}
