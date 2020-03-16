// file:   rgbled.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef RGBLED_H
#define RGBLED_H

#include "led.h"

class RgbLed : public Actuator
{
public:
    RgbLed();
    ~RgbLed();

    enum tColor { Red, Green, Blue, Magenta, Yellow, Cyan, White };

    void setPin( unsigned int pinRed,
                 unsigned int pinGreen,
                 unsigned int pinBlue,
                 bool pwm = true);
    void setInversion( bool invert = false);

    void setColor( tColor color);
    void setColor( int red, int green, int  blue);
    void setOn();
    void setOff();

    Led* redLed();
    Led* greenLed();
    Led* blueLed();

private:
    Led m_red;
    Led m_green;
    Led m_blue;
    bool m_invert;
};

#endif // RGBLED_H
