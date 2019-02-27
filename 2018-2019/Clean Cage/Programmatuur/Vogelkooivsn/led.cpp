// file:   led.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "led.h"
#include <QTimer>

Led::Led()
{
    m_pin = -1;
    m_id = -1;
    m_blink = false;
    m_isblinking = false;
}

Led::~Led()
{
    m_max = BRIGHT;
    m_min = 0;
    m_ton = NOBLINK;
    m_toff = NOBLINK;
    setOff();
}

void Led::setPin( unsigned int pin)
{
    m_pin = pin;
#ifdef __SOFTPWM__
    softPwmCreate( pin, 0, 100);
#else
    pinMode( pin, OUTPUT);
#endif
}

void Led::setBrightness( int on, int off)
{
    if ( on < 0 ) on = 0;
    if ( on > BRIGHT ) on = BRIGHT;
    if ( off < 0 ) off = 0;
    if ( off > BRIGHT ) off = BRIGHT;
    m_max = on;
    m_min = off;
    if ( m_ison )
        setOn();
}

void Led::setBlink( int on, int off)
{
    if ( on < NOBLINK ) on = NOBLINK;
    if ( off < NOBLINK ) off = NOBLINK;
    m_ton = on;
    m_toff = off;
    if ( m_ison )
        setOn();
}

void Led::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
#ifdef __SOFTPWM__
    softPwmWrite( m_pin, m_max);
#else
    digitalWrite( m_pin, m_max / 100);
#endif
    if ( m_ton != NOBLINK && m_toff != NOBLINK ) {
        m_isblinking = true;
        m_blink = true;
        QTimer::singleShot( m_ton, this, SLOT(blink()));
    }
}

void Led::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    m_isblinking = false;
#ifdef __SOFTPWM__
    softPwmWrite( m_pin, 0);
#else
    digitalWrite( m_pin, LOW);
#endif
    m_blink = false;
}

void Led::blink()
{
    if ( !m_isblinking ) return;

    m_blink = !m_blink;
#ifdef __SOFTPWM__
    softPwmWrite( m_pin, m_blink ? m_max : m_min);
#else
    digitalWrite( m_pin, m_blink ? m_max / 100 : m_min / 100);
#endif
    QTimer::singleShot( m_blink ? m_ton : m_toff, this, SLOT(blink()));
}
