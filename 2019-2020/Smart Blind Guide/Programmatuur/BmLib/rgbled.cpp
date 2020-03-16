// file:   rgbled.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "rgbled.h"

RgbLed::RgbLed()
{
	m_invert = true;
	m_red.setInversion();
	m_green.setInversion();
	m_blue.setInversion();
}

RgbLed::~RgbLed()
{
    setOff();
}

void RgbLed::setPin( unsigned int pinRed,
                     unsigned int pinGreen,
                     unsigned int pinBlue,
                     bool pwm)
{
	m_red.setPin( pinRed, pwm);
	m_green.setPin( pinGreen, pwm);
	m_blue.setPin( pinBlue, pwm);
}

void RgbLed::setInversion( bool invert)
{
	m_red.setInversion( invert);
	m_green.setInversion( invert);
	m_blue.setInversion( invert);
}

void RgbLed::setColor( tColor color)
{
	switch ( color ) {
		case Red :      setColor( BRIGHT, 0, 0); break;
		case Green :    setColor( 0, BRIGHT, 0); break;
		case Blue :     setColor( 0, 0, BRIGHT); break;
		case Magenta :  setColor( BRIGHT, 0, BRIGHT); break;
		case Yellow :   setColor( BRIGHT, BRIGHT, 0); break;
		case Cyan :     setColor( 0, BRIGHT, BRIGHT); break;
		case White :    setColor( BRIGHT, BRIGHT, BRIGHT); break;
		default :       setColor( 0, 0, 0);
	}
}

void RgbLed::setColor( int red, int green, int blue)
{
	if ( red < 0 ) red = 0;
	if ( red > BRIGHT ) red = BRIGHT;
	if ( green < 0  ) green = 0;
	if ( green > BRIGHT ) green = BRIGHT;
	if ( blue < 0 ) blue = 0;
	if ( blue > BRIGHT ) blue = BRIGHT;
	m_red.setBrightness( red);
	m_green.setBrightness( green);
	m_blue.setBrightness( blue);
}

void RgbLed::setOn()
{
	Actuator::setOn();
	m_red.setOn();
	m_green.setOn();
	m_blue.setOn();
}

void RgbLed::setOff()
{
	Actuator::setOff();
	m_red.setOff();
	m_green.setOff();
	m_blue.setOff();
}


Led* RgbLed::redLed()
{
	return &m_red;
}

Led* RgbLed::greenLed()
{
	return &m_green;
}

Led* RgbLed::blueLed()
{
	return &m_blue;
}
