// file:   temperature.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <wiringPi.h>
#include "temperature.h"

Temperature::Temperature()
{
}

Temperature::~Temperature()
{
}

void Temperature::setId( QString id)
{
    m_id = id;
}

void Temperature::read()
{
    if ( m_id.isEmpty() ) {
        QDir dir( "/sys/bus/w1/devices");
        QStringList lst = dir.entryList( QDir::Dirs);
        int ix;
        for( ix = 0; ix < lst.count(); ix++ )
            if ( lst.at( ix).left( 3) == "28-" )
                break;
        m_id = (ix < lst.count() ? lst.at( ix) : "");
    }
    if ( m_id.isEmpty() ) {
        m_value = -1000;
        return;
    }
    QFile sensor( QString( "/sys/bus/w1/devices/") + m_id + QString( "/w1_slave"));
    if ( sensor.open( QFile::ReadOnly) ) {
        QTextStream in( &sensor);
        QString value, val = "!";
        while ( !in.atEnd() && !val.isEmpty() ) {
            in >> val;
            value += val;
        }
        int ix = value.lastIndexOf( "t=");
        if ( ix < 0 )
            m_value = -1000;
        else
            m_value = value.right( value.length() - ix - 2).toFloat() / 1000;
    }
    else
        m_value = -1000;
}

float Temperature::celcius()
{
    return m_value;
}

float Temperature::fahrenheit()
{
    return m_value * 9 / 5 + 32;
}

QString Temperature::celciusStr()
{
    QString val;
    val.setNum( m_value, 'g', 4);
    if ( val.indexOf( '.') < 0 )
        val += ".0";
    return val;
}

QString Temperature::fahrenheitStr()
{
    QString val;
    val.setNum( m_value * 9 / 5 + 32, 'g', 4);
    if ( val.indexOf( '.') < 0 )
        val += ".0";
    return val;
}
