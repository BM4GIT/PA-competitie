// file:   actuator.h
// author: D.E.Veloper
// date:   2020-01-01


#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "device.h"

class Actuator : public Device
{
public:
    Actuator();
    ~Actuator();

    virtual void setOn();
    virtual void setOff();
    bool isOn();

protected:
    bool m_ison;
};

#endif // ACTUATOR_H
