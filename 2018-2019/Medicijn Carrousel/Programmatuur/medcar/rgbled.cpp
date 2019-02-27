// file:   rgbled.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "rgbled.h"
#include "wiringPi.h"

RgbLed::RgbLed()
{
    m_pinred = -1;
    m_pingreen = -1;
    m_pinblue = -1;
    m_color = White;
}

RgbLed::~RgbLed()
{
    setOff();
}

void RgbLed::setPin( unsigned int pinRed,
                     unsigned int pinGreen,
                     unsigned int pinBlue)
{
    m_pinred = pinRed;
    m_pingreen = pinGreen;
    m_pinblue = pinBlue;

    if ( m_pinred < 0 || m_pingreen < 0 || m_pinblue < 0 )
        return;

    pinMode( pinRed, OUTPUT);
    pinMode( pinGreen, OUTPUT);
    pinMode( pinBlue, OUTPUT);
    setOff();
}

void RgbLed::setColor( tColor color)
{
    m_color = color;
    if ( m_ison )
        setOn();
}

void RgbLed::setOn()
{
    if ( m_pinred < 0 || m_pingreen < 0 || m_pinblue < 0 )
        return;
    Actuator::setOn();

    switch ( m_color ) {
        case Red :      digitalWrite( m_pinred, HIGH);
                        digitalWrite( m_pingreen, LOW);
                        digitalWrite( m_pinblue, LOW);
                        break;

        case Green :    digitalWrite( m_pinred, LOW);
                        digitalWrite( m_pingreen, HIGH);
                        digitalWrite( m_pinblue, LOW);
                        break;

        case Blue :     digitalWrite( m_pinred, LOW);
                        digitalWrite( m_pingreen, LOW);
                        digitalWrite( m_pinblue, HIGH);
                        break;

        case Magenta :  digitalWrite( m_pinred, HIGH);
                        digitalWrite( m_pingreen, LOW);
                        digitalWrite( m_pinblue, HIGH);
                        break;

        case Yellow :   digitalWrite( m_pinred, HIGH);
                        digitalWrite( m_pingreen, HIGH);
                        digitalWrite( m_pinblue, LOW);
                        break;

        case Cyan :     digitalWrite( m_pinred, LOW);
                        digitalWrite( m_pingreen, HIGH);
                        digitalWrite( m_pinblue, HIGH);
                        break;

        case White :    digitalWrite( m_pinred, HIGH);
                        digitalWrite( m_pingreen, HIGH);
                        digitalWrite( m_pinblue, HIGH);
                        break;

        default :       setOff();

    }
}

void RgbLed::setOff()
{
    Actuator::setOff();
    if ( m_pinred < 0 || m_pingreen < 0 || m_pinblue < 0 )
        return;

    digitalWrite( m_pinred, LOW);
    digitalWrite( m_pingreen, LOW);
    digitalWrite( m_pinblue, LOW);
}
