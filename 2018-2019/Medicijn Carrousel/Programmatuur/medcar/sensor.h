// file:   sensor.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef SENSOR_H
#define SENSOR_H

#include "wiringPi.h"

class Sensor
{
public:
    Sensor();
    ~Sensor();

    void setPin( unsigned int pin);

    bool readPin();

private:

    int m_pin;
};

#endif // SENSOR_H
