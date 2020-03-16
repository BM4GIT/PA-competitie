// file:   distance.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "hcsr501.h"

namespace HCSR501
{

Motion::Motion()
{
    m_pin = 0;
}

Motion::~Motion()
{
}

void Motion::setPin( unsigned int pin)
{
    m_pin = pin;
    pinMode( m_pin, INPUT);
}

void Motion::read()
{
    m_detect = (digitalRead( m_pin) == HIGH);
    Sensor::read();
}


bool Motion::detected()
{
    return m_detect;
}

} // end namespace
