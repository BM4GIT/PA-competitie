// file:   rfid.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include <QElapsedTimer>
#include "rfid.h"
#include "wiringPi.h"

Rfid::Rfid()
{
    m_mfrc.PCD_Init();
    m_timeout = 500;
}

Rfid::~Rfid()
{
}

void Rfid::setPin( unsigned int pinRst)
{
    m_rst = pinRst;
}

void Rfid::setTimeout( int timeout)
{
    m_timeout = timeout;
}

void Rfid::reset()
{
    if ( m_rst < 0 ) return;
    digitalWrite( m_rst, HIGH);
    delay( 200);
    digitalWrite( m_rst, LOW);
}

void Rfid::read()
{
    QElapsedTimer tm;
    tm.start();

    m_tag = "";
    while ( tm.elapsed() < m_timeout ) {
        if( m_mfrc.PICC_IsNewCardPresent() && m_mfrc.PICC_ReadCardSerial() ) {
            for( byte i = 0; i < m_mfrc.uid.size; ++i)
                if ( m_mfrc.uid.uidByte[i] < 0x10 )
                    m_tag += "0" + QString::number( m_mfrc.uid.uidByte[i], 16) + " ";
                else
                    m_tag += QString::number( m_mfrc.uid.uidByte[i], 16) + " ";
        }
    }
}

QString Rfid::tag()
{
    return m_tag;
}
