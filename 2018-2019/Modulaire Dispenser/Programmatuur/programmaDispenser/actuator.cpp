// file:   actuator.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "actuator.h"


Actuator::Actuator()
{
    m_ison = false;
    wiringPiSetupGpio();
}

Actuator::~Actuator()
{
}

void Actuator::setOn()
{
    m_ison = true;
}

void Actuator::setOff()
{
    m_ison = false;
}

bool Actuator::isOn()
{
    return m_ison;
}
