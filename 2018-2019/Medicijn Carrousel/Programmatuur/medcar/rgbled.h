// file:   rgbled.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef RGBLED_H
#define RGBLED_H

#include "actuator.h"

class RgbLed : public Actuator
{
public:
    RgbLed();
    ~RgbLed();

    enum tColor { Red, Green, Blue, Magenta, Yellow, Cyan, White };

    void setPin( unsigned int pinRed,
                 unsigned int pinGreen,
                 unsigned int pinBlue);

    void setColor( tColor color);
    void setOn();
    void setOff();

private:
    int m_pinred;
    int m_pingreen;
    int m_pinblue;
    tColor m_color;
};

#endif // RGBLED_H
