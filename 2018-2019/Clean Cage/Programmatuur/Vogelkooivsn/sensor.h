// file:   sensor.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef SENSOR_H
#define SENSOR_H

#define __SOFTPWM__

#include <QObject>
#include <wiringPi.h>
#ifdef __SOFTPWM__
#include <softPwm.h>
#endif

class Sensor : public QObject
{
    Q_OBJECT

public:
    Sensor();
    ~Sensor();

    virtual void read();
    bool dataReady();

private:

    bool m_dataready;
};

#endif // SENSOR_H
