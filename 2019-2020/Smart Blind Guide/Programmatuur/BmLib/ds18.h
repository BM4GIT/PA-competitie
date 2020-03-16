// file:   dst.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef DS18_H
#define DS18_H

// Max sample rate Dallas w1 sensor is 1.3 Hz (duty cycle 750 ms)

#include "sensor.h"
#include <vector>

namespace DS18
{

typedef vector<String> StringList;

class Temperature : public Sensor
{
public:

	Temperature();
	~Temperature() {};

    static StringList getIdList();
    void setSensor( String id, int pin = 4);

    void read();

    float kelvin();
    float celcius();
    float fahrenheit();

private:

    String  m_id;
    int     m_pin;
    float   m_temp;
};

} // end namespace

#endif
