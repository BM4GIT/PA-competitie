// file:   distance.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef HCSR04_H
#define HCSR04_H

#include "sensor.h"

namespace HCSR04
{

class Average
{
public:
    Average();

    void  add( float value);
    void  clear();
    float avg();

protected:
    float  m_val[20];
    int  m_cur;
    bool m_complete;
};

class Distance : public Sensor
{
public:
    Distance();
    ~Distance();

    void setPin( unsigned int trigger, unsigned int echo);

    void read();

    int cm();
    int inch();

private:
    int     m_trigger;
	int		m_echo;
    int     m_cm;
};

} // end namespace

#endif // DISTANCE_H
