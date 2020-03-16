// file:   distance.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "hcsr04.h"

namespace HCSR04
{

/////////////
// AVERAGE //
/////////////

Average::Average()
{
    clear();
}

void Average::clear()
{
    m_cur = 0;
    m_complete = false;
}

void Average::add( float value)
{
    m_val[m_cur] = value;
    m_cur++;
    if ( m_cur == 20 ) {
        m_cur = 0;
        m_complete = true;
    }
}

float Average::avg()
{
    float sum = 0;
    int x = (m_complete ? 20 : m_cur + 1);
    for ( int i = 0; i < x; i++ )
        sum += m_val[i];
    return sum / x + 0.5;
}


//////////////
// DISTANCE //
//////////////

Distance::Distance()
{
    m_trigger = 0;
    m_echo = 0;
}

Distance::~Distance()
{
}

void Distance::setPin( unsigned int trigger, unsigned int echo)
{
    m_trigger = trigger;
    m_echo = echo;
    pinMode( m_trigger, OUTPUT);
    pinMode( m_echo, INPUT);
}

void Distance::read()
{
    digitalWrite( m_trigger, HIGH);
    delayMicroseconds( 10);
    digitalWrite( m_trigger, LOW);

    long now = micros();
    long timeout = 500000;

    while ( (digitalRead( m_echo) == LOW) && (micros() - now < timeout) );

    long startTime = micros();
    while ( (digitalRead( m_echo) == HIGH) && (micros() - now < timeout) );
    long endTime = micros();

    m_cm = (endTime - startTime) / 58.138;
    Sensor::read();
}


int Distance::cm()
{
    return m_cm;
}

int Distance::inch()
{
    return m_cm / 2.54;
}

} //end namespace
