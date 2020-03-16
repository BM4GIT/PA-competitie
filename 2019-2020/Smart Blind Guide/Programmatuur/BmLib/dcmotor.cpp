// file:   dcmotor.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "dcmotor.h"

DcMotor::DcMotor()
{
    m_pin = -1;
    m_pwm = true;
}

DcMotor::~DcMotor()
{
    setOff();
}

void DcMotor::setPin(unsigned int pin, bool pwm)
{
    m_pin = pin;
    m_pwm = pwm;
    m_speed = FULLSPEED;
    pinMode( pin, OUTPUT);
}

void DcMotor::setSpeed( int speed)
{
    if ( speed < 0 ) speed = 0;
    if ( speed > 100 )
        m_speed = FULLSPEED;
    else
        m_speed = speed * 1023 / 100;
    if ( m_ison )
        setOn();
}

void DcMotor::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    printf( "DcMotor on: %d\n", m_speed);
    if ( m_pwm )
        analogWrite( m_pin, m_speed);
    else
        digitalWrite( m_pin, m_speed ? HIGH : LOW);
}

void DcMotor::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    printf( "DcMotor off\n");
    if ( m_pwm )
        analogWrite( m_pin, 0);
    else
        digitalWrite( m_pin, LOW);
}
