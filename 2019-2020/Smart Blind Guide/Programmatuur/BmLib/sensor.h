// file:   sensor.h
// author: D.E.Veloper
// date:   2020-01-01


#ifndef SENSOR_H
#define SENSOR_H

#include "device.h"

class Sensor : public Device
{
public:
    Sensor();
    ~Sensor();

    virtual void read();
    bool dataReady();

private:

    bool m_dataready;
};

#endif // SENSOR_H
