// file:   rfid.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef RFID_H
#define RFID_H

#include <QString>
#include "sensor.h"
#include "MFRC522.h"

class Rfid : public Sensor
{
public:
    Rfid();
    ~Rfid();

    void setPin( unsigned int pinRst);
    void setTimeout( int timeout); // timeout in millisec

    void reset();
    void read();
    QString tag();

private:

    MFRC522 m_mfrc;
    QString m_tag;
    int     m_timeout;
    int     m_rst;
};

#endif // RFID_H
