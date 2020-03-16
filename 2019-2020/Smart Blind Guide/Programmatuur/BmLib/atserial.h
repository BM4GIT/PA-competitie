#ifndef ATSERIAL_H
#define ATSERIAL_H

/*
// only for test compilation of ARDUINO_AVR_... on raspberry
//#define ARDUINO_AVR_UNO
//#define ARDUINO_AVR_MEGA2560
*/

//#define SOFTSERIAL
#define ATDEBUG

#include "Arduino.h"

#ifdef SOFTSERIAL
#include "SoftwareSerial.h"
/*
// only for test compilation of SOFTSERIAL on raspberry
class SoftwareSerial
{
public:
    SoftwareSerial( int rx, int tx);
    void begin( int baud);
    void print( String str);
    void println( String str);
    void flush();
    bool available();
    char read();
    String readString();
};
*/
#else

//#define SERIAL 0 // already defined

#ifdef ARDUINO_AVR_UNO
#undef ATDEBUG // ATDEBUG will conflict with Serial occupation by gprs module
#else

#define SERIAL1     1 // mega and raspberry

#ifdef ARDUINO_AVR_MEGA2560
#define SERIAL2     2
#define SERIAL3     3
/*
// only for test compilation of ARDUINO_AVR_MEGA2560 on raspberry
#define Serial2 Serial1
#define Serial3 Serial1
*/
#endif
#endif
#endif

class AtSerial
{
public:
    AtSerial();
    ~AtSerial();

#ifdef SOFTSERIAL
    void begin( const uint rxpin, const uint txpin);
#else
#ifdef ARDUINO_AVR_MEGA2560
    void begin( const uint serialinterface); // SERIAL, SERIAL1, SERIAL2 or SERIAL3
#else
    void begin();
#endif
#endif

    void setPin( const String pin);
    void setProvider( const String apn, const String user = "", const String password = "");
    void setNetwork( const String id, const String band = "");
    void setMethod( const uint method); // TCP or UDP
    void setServer( const String url, const String port);

    bool send( uint32_t timeout, const String command, const String message = "");
    String response();
    void clearBuffer();

    bool asleep();

    uint    m_method; // TCP or UDP
    String  m_pin;
    String  m_apn;
    String  m_user;
    String  m_password;
    String  m_network;
    String  m_band;
    String  m_socket;
    String  m_url;
    String  m_port;

protected:

#ifdef SOFTSERIAL
    SoftwareSerial  *m_softser;
#else
    uint            m_serial;
#endif

    bool    m_asleep; // in PSM mode?
    String  m_response;
};

extern AtSerial atSerial;

#endif
