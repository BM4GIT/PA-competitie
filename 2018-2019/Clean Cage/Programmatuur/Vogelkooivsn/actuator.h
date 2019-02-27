// file:   actuator.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef ACTUATOR_H
#define ACTUATOR_H

#define __SOFTPWM__

#include <QObject>
#include <wiringPi.h>
#ifdef __SOFTPWM__
#include <softPwm.h>
#endif

class Actuator : public QObject
{
    Q_OBJECT

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
