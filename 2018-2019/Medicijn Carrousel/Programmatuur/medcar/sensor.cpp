// file:   sensor.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "sensor.h"

Sensor::Sensor()
{
}

Sensor::~Sensor()
{
}

void Sensor::setPin( unsigned int pin)
{
    m_pin = pin;
    pinMode( pin, INPUT);
}

bool Sensor::readPin()
{
    return digitalRead( m_pin);
}
