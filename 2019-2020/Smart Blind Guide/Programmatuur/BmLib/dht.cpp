// file:   dht.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "dht.h"

namespace DHT
{

const char* DHT11 = "DHT11";
const char* DHT22 = "DHT22";

Temperature::Temperature()
{
    m_pin = 0;
    m_temp = 0;
    m_hum = 0;
}

void Temperature::setSensor( String id, int pin)
{
    m_id = id;
    m_pin = pin;
    pinMode( pin, INPUT);
}


void Temperature::read()
{
	m_temp = m_hum = 0;
	int i = 0;
	while ( !readDHT() && (i < 3) ) {
		i++;
		delay( m_id == DHT11 ? 1000 : 2000); // time for dht to take a measurement
	}
	if ( i == 3 ) return;

    Sensor::read();
}

bool Temperature::readDHT()
{
    unsigned long startTime;

    // request data
    digitalWrite( m_pin, LOW);
    pinMode( m_pin, OUTPUT);
    delayMicroseconds( m_id == DHT11 ? 18000 : 800);
    pinMode( m_pin, INPUT);
    digitalWrite( m_pin, HIGH);

    // Read 83 edges:
    // - First a FALLING, RISING, and FALLING edge for the start bit
    // - Then 40 bits: RISING and then a FALLING edge per bit
    // To keep the code simple, accept any HIGH or LOW reading if it's max 85 usecs long

    uint16_t rawHumidity = 0;
    uint16_t rawTemperature = 0;
    uint16_t data = 0;

    for ( int8_t i = -3 ; i < 2 * 40; i++ ) {
      uint8_t age;
      startTime = micros();

      do {
        age = (unsigned long)(micros() - startTime);
        if ( age > 90 ) {
          // timeout
          return false;
        }
      }
      while ( digitalRead( m_pin) == (i & 1) ? HIGH : LOW );

      if ( i >= 0 && (i & 1) ) {
        // all 40 bits are there
        data <<= 1;

        // A zero max 30 usecs, a one at least 68 usecs.
        if ( age > 30 ) {
          data |= 1; // it is an 'one'
        }
      }

      switch ( i ) {
        case 31:
          rawHumidity = data;
          break;
        case 63:
          rawTemperature = data;
          data = 0;
          break;
      }
    }

    // Verify checksum

    if ( (uint8_t)(((uint8_t)rawHumidity) + (rawHumidity >> 8) + ((uint8_t)rawTemperature) + (rawTemperature >> 8)) != data ) {
      // checksum error
      return false;
    }

    // Store readings

    if ( m_id == DHT11 ) {
      m_hum = rawHumidity >> 8;
      m_temp = rawTemperature >> 8;
    }
    else {
      m_hum = rawHumidity * 0.1;

      if ( rawTemperature & 0x8000 ) {
        rawTemperature = -(int16_t)(rawTemperature & 0x7FFF);
      }
      m_temp = ((int16_t)rawTemperature) * 0.1;
    }
    m_temp += 273;

    return true;
}

float Temperature::humidity()
{
    return m_hum;
}

float Temperature::kelvin()
{
    return m_temp;
}

float Temperature::celcius()
{
    return m_temp - 273;
}

float Temperature::fahrenheit()
{
    return (m_temp - 273) * 9 / 5 + 32;
}

} // end namespace
