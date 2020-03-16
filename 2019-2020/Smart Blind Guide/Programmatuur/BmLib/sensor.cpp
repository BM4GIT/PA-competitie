// file:   sensor.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "sensor.h"

Sensor::Sensor()
{
    m_dataready = false;
}

Sensor::~Sensor()
{
}

void Sensor::read()
{
    m_dataready = true;
}

bool Sensor::dataReady()
{
    if ( m_dataready ) {
        m_dataready = false;
        return true;
    }
    return false;
}
