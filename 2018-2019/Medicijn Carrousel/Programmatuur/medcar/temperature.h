// file:   temperature.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "sensor.h"

class Temperature : public Sensor
{
public:
    Temperature();
    ~Temperature();

    void setId( QString id);

    void read();

    float celcius();
    float fahrenheit();
    QString celciusStr();
    QString fahrenheitStr();

private:

    QString m_id;
    float   m_value;
};

#endif // TEMPERATURE_H
