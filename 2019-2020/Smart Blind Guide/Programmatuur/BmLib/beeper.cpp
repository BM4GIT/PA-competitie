// file:   beeper.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "beeper.h"

Beeper::Beeper()
{
    m_pin = -1;
}

Beeper::~Beeper()
{
    setOff();
}

void Beeper::setPin( uint8_t pin)
{
    m_pin = pin;
    pinMode( pin, OUTPUT);
}

void Beeper::setBeeper( uint16_t herz, uint16_t msec)
{
    m_beep = herz;
    m_dur = msec;
}

uint8_t Beeper::addTone( uint16_t herz, uint16_t msec)
{
    uint8_t ix = m_hz.add( herz);
    m_tm.add( msec);
    return ix;
}

void Beeper::insertTone( uint8_t pos, uint16_t herz, uint16_t msec)
{
    m_hz.insert( pos, herz);
    m_tm.insert( pos, msec);
}

void Beeper::deleteTone( uint8_t pos, uint8_t count)
{
    for ( int i = pos; i < pos + count; i++ ) {
        m_hz.removeAt( i);
        m_tm.removeAt( i);
    }
}

void Beeper::clear()
{
    m_hz.removeAll();
    m_tm.removeAll();
}

void Beeper::play()
{
    if ( m_pin < 0 ) return;
    for (int i = 0; i < m_hz.size(); i++ )
        playtone( m_hz[i], m_tm[i]);
}

void Beeper::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    playtone( m_beep, m_dur);
}

void Beeper::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
}

void Beeper::playtone( uint16_t herz, uint16_t msec)
{
    uint32_t t = 1000000 / herz;
    uint32_t tm = millis() + msec;
    while ( (uint32_t) millis() < tm ) {
        digitalWrite( m_pin, HIGH);
        delayMicroseconds( t);
        digitalWrite( m_pin, LOW);
        delayMicroseconds( t);
    }
}
