// file:   stepper.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "stepper.h"

int g_coils[8] = { 0b01000, 0b01100, 0b00100, 0b00110, 0b00010, 0b00011, 0b00001, 0b01001};

Stepper::Stepper()
{
    m_dt = DT_A4988;
    m_pin1 = -1;
    m_pin2 = -1;
    m_pin3 = -1;
    m_pin4 = -1;
    m_stepspr = 200; // 1.8 degree stepper
    m_forward = true;
    m_step = 0;
}

Stepper::~Stepper()
{
    setOff();
}

void Stepper::init( int dt, unsigned int stepsPerRotation)
{
    m_dt = dt;
    m_stepspr = stepsPerRotation;
}

void Stepper::stepUniPolar()
{
    if ( m_forward ) {
        m_step++;
        if ( m_step > 7 ) m_step = 0;
    }
    else {
        m_step--;
        if ( m_step < 0 ) m_step = 7;
    }
    digitalWrite( m_pin1, g_coils[m_step] & 1);
    digitalWrite( m_pin2, g_coils[m_step] & 2);
    digitalWrite( m_pin3, g_coils[m_step] & 4);
    digitalWrite( m_pin4, g_coils[m_step] & 8);
}

void Stepper::stepBiPolar()
{
    digitalWrite( m_pin3, LOW);
    digitalWrite( m_pin2, m_forward ? HIGH : LOW);
    digitalWrite( m_pin1, HIGH);
    delayMicroseconds( 10);
    digitalWrite( m_pin1, LOW);
    delayMicroseconds( 10);
}

void Stepper::setPin( unsigned int pin1,
                      unsigned int pin2,
                      unsigned int pin3,
                      unsigned int pin4)
{
    m_pin1 = pin1;
    m_pin2 = pin2;
    m_pin3 = pin3;
    m_pin4 = pin4;

    pinMode( pin1, OUTPUT);
    pinMode( pin2, OUTPUT);
    pinMode( pin3, OUTPUT);
    pinMode( pin4, OUTPUT);
}

void Stepper::setPin( unsigned int pinStep,
                      unsigned int pinDir,
                      unsigned int pinEnab)
{
    setPin( pinStep, pinDir, pinEnab, -1);
}

void Stepper::setForward( bool forward)
{
    m_forward = forward;
}

void Stepper::setReverse()
{
    m_forward = false;
}

void Stepper::setHold( bool hold)
{
    m_hold = hold;
}

void Stepper::setRelease()
{
    m_hold = false;
}

void Stepper::turn( unsigned int steps, unsigned int speed)
{
    if ( speed > 100 ) return;

    if ( speed )
    {
        Actuator::setOn();

        long dly = (100 - speed) * 100 + 1000;
        while ( steps ) {
            if ( m_dt == DT_UNIPOLAR )
                stepUniPolar();
            else
                stepBiPolar();
            steps--;
            delayMicroseconds( dly);
        }
    }
    setOff();
}

void Stepper::rotate( unsigned int angle, unsigned int speed)
{
    int steps = m_stepspr * angle / 360;
    turn( steps, speed);
}

void Stepper::move( unsigned int time, unsigned int speed)
{
    if ( speed > 100 ) return;

    if ( speed ) {
        Actuator::setOn();

        long dly = (100 - speed) * 10 + 1;
        int tm = millis() + time;
        while ( millis() < tm ) {
            if ( m_dt == DT_UNIPOLAR )
                stepUniPolar();
            else
                stepBiPolar();
            delay( dly);
        }
    }
    setOff();
}

void Stepper::setOff()
{
    Actuator::setOff();
    digitalWrite( m_pin1, 0);
    digitalWrite( m_pin2, 0);
    if ( (m_dt == DT_UNIPOLAR) || m_hold)
        digitalWrite( m_pin3, 0);
    else {
        // wait 100 usec to set motor in position
        delayMicroseconds( 100);
        // then release torque
        digitalWrite( m_pin3, 1);
    }
    digitalWrite( m_pin4, 0);
}
