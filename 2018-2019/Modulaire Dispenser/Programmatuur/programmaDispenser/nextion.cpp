#include "nextion.h"
#include "wiringSerial.h"
#include "wiringPi.h"

Nextion::Nextion()
{
    m_fd = 0;
}

Nextion::~Nextion()
{
    if ( m_fd )
        serialClose( m_fd);
}

bool Nextion::connect()
{
    m_fd = serialOpen( "/dev/ttyS0", 9600);
    return (m_fd > 0);
}

QVariant Nextion::receive()
{
    QVariant data;
    if ( serialDataAvail( m_fd) ) {
        char c = (char) serialGetchar( m_fd);
        if ( c == '#' ) {
            int val = 0;
            for ( int i = 0; i < 4; i++ ) {
                // 4 bytes in little endian order
                int v = serialGetchar( m_fd);
                val += v << (8 * i);
            }
            data = val;
        }
        else {
            QString str;
            while ( c != '~' ) {
                str += c;
                c = (char) serialGetchar( m_fd);
            }
            data = str;
        }
    }

    return data;
}

void Nextion::sendPage( QVariant page)
{
    send( "page " + page.toString());
}

void Nextion::sendNumber( QString field, QVariant data)
{
    send( field + ".val=" + data.toString());
}

void Nextion::sendText( QString field, QVariant data)
{
    send( field + ".txt=" + data.toString());
}

void Nextion::send( QString data)
{
    QByteArray ba = data.toLatin1();
    ba.append( 3, 0xFF);
    for ( int i = 0; i < ba.count(); i++ )
        serialPutchar( m_fd, ba.at(i));
}
