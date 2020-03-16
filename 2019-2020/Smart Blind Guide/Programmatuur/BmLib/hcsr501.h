// file:   hcsr501.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef HCSR501_H
#define HCSR501_H

#include "sensor.h"

namespace HCSR501
{

class Motion : public Sensor
{
public:
    Motion();
    ~Motion();

    void setPin( unsigned int pin);

    void read();

    bool detected();

private:
    int  m_pin;
    bool m_detect;
};

} // end namespace

#endif // HCSR501_H
