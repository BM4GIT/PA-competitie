// file:   switch.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "switch.h"

Switch::Switch()
{
    m_pressed = false;
    m_normalopen = true;
}

Switch::~Switch()
{
}

void Switch::setPin( unsigned int pin)
{
    m_pin = pin;
    pinMode( pin, INPUT_PULLDOWN);
}

void Switch::setNormalOpen( bool normalopen)
{
    m_normalopen = normalopen;
}

void Switch::triggerEvent( void (*callback)(), Event  event)
{
    // note that switches trigger both events a few times
    uint edge;
    switch ( event ) {
        case Push :     edge = (m_normalopen ? RISING : FALLING); break;
        case Release :  edge = (m_normalopen ? FALLING : RISING); break;
        case Both :     edge = CHANGE; break;
        default :       return;
    }
    attachInterrupt( m_pin, callback, edge);
}

void Switch::read()
{
    m_pressed = (digitalRead( m_pin) ? true : false);
    if ( !m_normalopen )
        m_pressed = !m_pressed;
    Sensor::read();
}

bool Switch::pressed()
{
    return m_pressed;
}

bool Switch::released()
{
    return !m_pressed;
}
