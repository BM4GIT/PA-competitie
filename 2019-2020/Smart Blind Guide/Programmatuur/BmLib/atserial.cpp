#include "Arduino.h"
#include "atserial.h"

AtSerial atSerial;


AtSerial::AtSerial()
{
#ifdef SOFTSERIAL
    m_softser = NULL;
#endif
}

AtSerial::~AtSerial()
{
#ifdef SOFTSERIAL
    if ( m_softser ) delete m_softser;
#endif
}

#ifdef SOFTSERIAL
// ###   SoftwareSerial library   ###

void AtSerial::begin( const uint rxpin, const uint txpin)
{
    m_softser = new SoftwareSerial( rxpin, txpin);
    m_softser->begin( 115200);
#ifdef ATDEBUG
    Serial.print( "Using SoftwareSerial (rx="); Serial.print( rxpin); Serial.print( ", tx="); Serial.print( txpin); Serial.println( ")\n");
#endif
}

void AtSerial::clearBuffer()
{
    String dummy;
    while ( m_softser->available() )
        dummy = m_softser->readString();
}

bool AtSerial::send( uint32_t timeout, const String command, const String message)
{
    String cmd = command;
    String msg = message;
#ifdef ATDEBUG
    Serial.print( "COMMAND: "); Serial.println( command);
    if ( msg.length() ) {
        Serial.print( "MESSAGE: "); Serial.println( message);
    }
#endif
    m_softser->flush();
    delay( 100);
    m_softser->println( cmd.c_str());
    m_response = "";
    uint32_t tm = millis();
    while ( tm + timeout > (uint32_t) millis() ) {
        while ( m_softser->available() )
            m_response += m_softser->readString();
        if ( m_response.length() )
            break;
    }
    if ( !m_response.length() || (m_response.indexOf( "ERROR") >= 0) ) {
#ifdef ATDEBUG
        if ( !m_response.length() ) {
            Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
        }
        else
            Serial.println( "ERROR\n");
        Serial.println( "");
#endif
        return false;
    }
#ifdef ATDEBUG
    if ( m_response.length() ) {
        Serial.print( "RESPONSE: "); Serial.println( m_response);
    }
    Serial.println( "DONE\n");
#endif
    if ( msg.length() ) {
        m_response = "";
        m_softser->flush();
        delay( 100);
        m_softser->println( msg.c_str());
        String response = "";
        tm = millis();
        while ( tm + timeout > (uint32_t) millis() ) {
            while ( m_softser->available() )
                m_response += Serial1.read();
        }
        if ( !m_response.length() || (m_response.indexOf( String( "ERROR")) >= 0) ) {
#ifdef ATDEBUG
            if ( !m_response.length() ) {
                Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
            }
            else
                Serial.println( "ERROR\n");
            Serial.println( "");
#endif
            return false;
        }
#ifdef ATDEBUG
        if ( m_response.length() ) {
            Serial.print( "RESPONSE: "); Serial.println( m_response);
        }
        Serial.println( "DONE\n");
#endif
    }
    return true;
}

#else
#ifdef ARDUINO_AVR_MEGA2560
// ###   Arduino Mega   ###

void AtSerial::begin( const uint serialinterface)
{
    m_serial = serialinterface;
    switch( m_serial ) {
        case SERIAL1    : Serial1.begin( 115200); break;
        case SERIAL2    : Serial2.begin( 115200); break;
        case SERIAL3    : Serial3.begin( 115200); break;
        default         : Serial.begin( 115200);
    }
#ifdef ATDEBUG
    Serial.print( "Using Serial"); Serial.print( m_serial); Serial.println( " on Arduino Mega\n");
#endif
}

void AtSerial::clearBuffer()
{
    String dummy;
    switch( m_serial ) {
        case SERIAL1    : while ( Serial1.available() )
                              dummy = Serial1.readString();
                          break;
        case SERIAL2    : while ( Serial2.available() )
                              dummy = Serial2.readString();
                          break;
        case SERIAL3    : while ( Serial3.available() )
                              dummy = Serial3.readString();
                          break;
        default         : while ( Serial.available() )
                              dummy = Serial.readString();
    }
}

bool AtSerial::send( uint32_t timeout, const String command, const String message)
{
    String cmd = command;
    String msg = message;
#ifdef ATDEBUG
    Serial.print( "COMMAND: "); Serial.println( command);
    if ( msg.length() ) {
        Serial.print( "MESSAGE: "); Serial.println( message);
    }
#endif
    switch( m_serial ) {
        case SERIAL1    : Serial1.flush();
                          delay( 100);
                          Serial1.println( cmd.c_str());
                          break;
        case SERIAL2    : Serial2.flush();
                          delay( 100);
                          Serial2.println( cmd.c_str());
                          break;
        case SERIAL3    : Serial3.flush();
                          delay( 100);
                          Serial3.println( cmd.c_str());
                          break;
        default         : Serial.flush();
                          delay( 100);
                          Serial.println( cmd.c_str());
    }
    m_response = "";
    uint32_t tm = millis();
    while ( tm + timeout > (uint32_t) millis() ) {
        switch( m_serial ) {
            case SERIAL1    : while ( Serial1.available() )
                                  m_response += Serial1.readString();
                              break;
            case SERIAL2    : while ( Serial2.available() )
                                  m_response += Serial2.readString();
                              break;
            case SERIAL3    : while ( Serial3.available() )
                                  m_response += Serial3.readString();
                              break;
            default         : while ( Serial.available() )
                                  m_response += Serial.readString();
        }
        if ( m_response.length() )
            break;
    }
    if ( !m_response.length() || (m_response.indexOf( "ERROR") >= 0) ) {
#ifdef ATDEBUG
        if ( !m_response.length() ) {
            Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
        }
        else
            Serial.println( "ERROR\n");
        Serial.println( "");
#endif
        return false;
    }
#ifdef ATDEBUG
    if ( m_response.length() ) {
        Serial.print( "RESPONSE: "); Serial.println( m_response);
    }
    Serial.println( "DONE\n");
#endif
    if ( msg.length() ) {
        m_response = "";
        switch( m_serial ) {
            case SERIAL1    : Serial1.flush();
                              delay( 100);
                              Serial1.println( msg.c_str());
                              break;
            case SERIAL2    : Serial2.flush();
                              delay( 100);
                              Serial2.println( msg.c_str());
                              break;
            case SERIAL3    : Serial3.flush();
                              delay( 100);
                              Serial3.println( msg.c_str());
                              break;
            default         : Serial.flush();
                              delay( 100);
                              Serial.println( msg.c_str());
        }
        tm = millis();
        while ( tm + timeout > (uint32_t) millis() ) {
            switch( m_serial ) {
                case SERIAL1    : while ( Serial1.available() )
                                      m_response += Serial1.readString();
                                  break;
                case SERIAL2    : while ( Serial2.available() )
                                      m_response += Serial2.readString();
                                  break;
                case SERIAL3    : while ( Serial3.available() )
                                      m_response += Serial3.readString();
                                  break;
                default         : while ( Serial.available() )
                                      m_response += Serial.readString();
            }
        }
        if ( !m_response.length() || (m_response.indexOf( String( "ERROR")) >= 0) ) {
#ifdef ATDEBUG
            if ( !m_response.length() ) {
                Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
            }
            else
                Serial.println( "ERROR\n");
            Serial.println( "");
#endif
            return false;
        }
#ifdef ATDEBUG
        if ( m_response.length() ) {
            Serial.print( "RESPONSE: "); Serial.println( m_response);
        }
        Serial.println( "DONE\n");
#endif
    }
    return true;
}

#else
#ifdef ARDUINO_AVR_UNO
// ###   Arduino Uno   ###

void AtSerial::begin()
{
    Serial.begin( 115200);
#ifdef ATDEBUG
    Serial.println( "Using Serial on Arduino Uno\n");
#endif
}

void AtSerial::clearBuffer()
{
    String dummy;
    while ( Serial.available() )
        dummy = Serial.readString();
}

bool AtSerial::send( uint32_t timeout, const String command, const String message)
{
    String cmd = command;
    String msg = message;
#ifdef ATDEBUG
    Serial.print( "COMMAND: "); Serial.println( command);
    if ( msg.length() ) {
        Serial.print( "MESSAGE: "); Serial.println( message);
    }
#endif
    Serial.flush();
    delay( 100);
    Serial.println( cmd.c_str());
    m_response = "";
    uint32_t tm = millis();
    while ( tm + timeout > (uint32_t) millis() ) {
        while ( Serial.available() )
            m_response += Serial.readString();
        if ( m_response.length() )
            break;
    }
    if ( !m_response.length() || (m_response.indexOf( "ERROR") >= 0) ) {
#ifdef ATDEBUG
        if ( !m_response.length() ) {
            Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
        }
        else
            Serial.println( "ERROR\n");
        Serial.println( "");
#endif
        return false;
    }
#ifdef ATDEBUG
    if ( m_response.length() ) {
        Serial.print( "RESPONSE: "); Serial.println( m_response);
    }
    Serial.println( "DONE\n");
#endif
    if ( msg.length() ) {
        m_response = "";
        Serial.flush();
        delay( 100);
        Serial.println( msg.c_str());
        String response = "";
        tm = millis();
        while ( tm + timeout > (uint32_t) millis() ) {
            while ( Serial.available() )
                m_response += Serial.read();
        }
        if ( !m_response.length() || (m_response.indexOf( String( "ERROR")) >= 0) ) {
#ifdef ATDEBUG
            if ( !m_response.length() ) {
                Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
            }
            else
                Serial.println( "ERROR\n");
            Serial.println( "");
#endif
            return false;
        }
#ifdef ATDEBUG
        if ( m_response.length() ) {
            Serial.print( "RESPONSE: "); Serial.println( m_response);
        }
        Serial.println( "DONE\n");
#endif
    }
    return true;
}

#else
// ###   Raspberry   ###

void AtSerial::begin()
{
    Serial1.begin( 115200);
#ifdef ATDEBUG
    Serial.println( "Using Serial1 on Raspberry\n");
#endif
}

void AtSerial::clearBuffer()
{
    while ( Serial1.available() )
        Serial1.read();
}

bool AtSerial::send( uint32_t timeout, const String command, const String message)
{
    String cmd = command;
    String msg = message;
#ifdef ATDEBUG
    Serial.print( "COMMAND: "); Serial.println( command);
    if ( msg.length() ) {
        Serial.print( "MESSAGE: "); Serial.println( message);
    }
#endif
    Serial1.flush();
    delay( 100);
    Serial1.println( cmd.c_str());
    m_response = "";
    uint32_t tm = millis();
    while ( tm + timeout > (uint32_t) millis() ) {
        while ( Serial1.available() )
            m_response += (char) Serial1.read();
    }
    if ( !m_response.length() || (m_response.indexOf( String( "ERROR")) >= 0) ) {
#ifdef ATDEBUG
        if ( !m_response.length() ) {
            Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
        }
        else
            Serial.println( "ERROR\n");
        Serial.println( "");
#endif
        return false;
    }
#ifdef ATDEBUG
    if ( m_response.length() ) {
        Serial.print( "RESPONSE: "); Serial.println( m_response);
    }
    Serial.println( "DONE\n");
#endif
    if ( msg.length() ) {
        m_response = "";
        Serial1.flush();
        delay( 100);
        Serial1.println( msg.c_str());
        String response = "";
        tm = millis();
        while ( tm + timeout > (uint32_t) millis() ) {
            while ( Serial1.available() )
                m_response += (char) Serial1.read();
        }
        if ( !m_response.length() || (m_response.indexOf( String( "ERROR")) >= 0) ) {
#ifdef ATDEBUG
            if ( !m_response.length() ) {
                Serial.print( "TIMEOUT: "); Serial.println( (int) timeout);
            }
            else
                Serial.println( "ERROR\n");
            Serial.println( "");
#endif
            return false;
        }
#ifdef ATDEBUG
        if ( m_response.length() ) {
            Serial.print( "RESPONSE: "); Serial.println( m_response);
        }
        Serial.println( "DONE\n");
#endif
    }
    return true;
}

#endif
#endif
#endif

void AtSerial::setPin( const String pin)
{
    m_pin = pin;
}

void AtSerial::setProvider( const String apn, const String user, const String password)
{
    m_apn = apn;
    m_user = user;
    m_password = password;
}

void AtSerial::setNetwork( const String id, const String band)
{
    m_network = id;
    m_band = band;
}

void AtSerial::setMethod( const uint method)
{
    m_method = method;
}

void AtSerial::setServer( const String url, const String port)
{
    m_url = url;
    m_port = port;
}

String AtSerial::response()
{
    return m_response;
}

bool AtSerial::asleep()
{
    return m_asleep;
}
