// file:   dht.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef DHT_H
#define DHT_H

// Max sample rate DHT11 is 1 Hz   (duty cicle 1000 ms)
// Max sample rate DHT22 is 0.5 Hz (duty cicle 2000 ms)

#include "sensor.h"

namespace DHT
{

extern const char* DHT11;
extern const char* DHT22;

class Temperature : public Sensor
{
public:

    Temperature();
    ~Temperature() {};

    void setSensor( String id, int pin = 4);
    void read();

    float kelvin();
    float celcius();
    float fahrenheit();

    float humidity();

private:

    bool readDHT();

    String  m_id;
    int     m_pin;
    float   m_temp;
    float   m_hum;
};

} //end namespace

#endif
