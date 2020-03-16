// file:   switch.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef SWITCH_H
#define SWITCH_H

#include "sensor.h"

class Switch : public Sensor
{
public:

    enum Event { Push, Release, Both };

    Switch();
    ~Switch();

    void setPin( unsigned int pin);
    void setNormalOpen( bool normalopen);
    void triggerEvent( void (*callback)(), Event  event);

    void read();

    bool pressed();
    bool released();

private:

    int		m_pin;
    bool	m_normalopen;
    bool	m_pressed;
};

#endif // SWITCH_H
