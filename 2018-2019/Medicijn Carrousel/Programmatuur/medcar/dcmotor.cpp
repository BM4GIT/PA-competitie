// file:   dcmotor.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "dcmotor.h"
#include "wiringPi.h"

DcMotor::DcMotor()
{
    m_pin = -1;
    wiringPiSetupGpio();
}

DcMotor::~DcMotor()
{
    setOff();
}

void DcMotor::setPin( unsigned int pin)
{
    m_pin = pin;
    pinMode( pin, OUTPUT);
}

void DcMotor::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    digitalWrite( m_pin, HIGH);
}

void DcMotor::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    digitalWrite( m_pin, LOW);
}
