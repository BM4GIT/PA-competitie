#include "Arduino.h"
#include "goutechiotga6.h"
#include "atserial.h"

namespace GouTechIotGA6
{

#ifdef SOFTSERIAL
void Gprs::beginAtSerial( const uint rxpin, const uint txpin)
{
    atSerial.begin( rxpin, txpin);
}
#else
#ifdef ARDUINO_AVR_MEGA2560
void Gprs::beginAtSerial( const uint serialinterface)
{
    atSerial.begin( serialinterface);
}
#else
void Gprs::beginAtSerial()
{
    atSerial.begin();
}
#endif
#endif

void Gprs::setPin( String pin)
{
    atSerial.setPin( pin);
}

void Gprs::setProvider( String apn, String user, String password)
{
    atSerial.setProvider( apn, user, password);
}

void Gprs::setNetwork( const String id, const String band)
{
    atSerial.setNetwork( id, band);
}

void Gprs::setMethod( const uint method)
{
    atSerial.setMethod( method);
}

void Gprs::setServer( String url, String port)
{
    atSerial.setServer( url, port);
}

bool Gprs::startup()
{
    if ( !atSerial.send( 3000, "AT") ) return false;
    if ( !atSerial.send( 10000, "AT+CFUN=1") ) return false;
    // It is required to ask the CPIN? status before the command CPIN is issued.
    if ( !atSerial.send( 100, "AT+CPIN?") ) return false;
    String cmd = "AT+CPIN=\"";
    cmd += atSerial.m_pin;
    cmd += "\"";
    if ( !atSerial.send( 200, cmd) ) return false;
    if ( !atSerial.send( 1000, "AT+CPAS") ) return false;
    if ( !atSerial.send( 1000, "AT+CREG?") ) return false;
    if ( !atSerial.send( 1000, "AT+CGACT?") ) return false;
    if ( !atSerial.send( 6000, "AT+CGATT=1") ) return false;
    cmd = "AT+CSTT=\"";
    cmd += atSerial.m_apn;
    cmd += "\",\"";
    cmd += atSerial.m_user;
    cmd += "\",\"";
    cmd += atSerial.m_password;
    cmd += "\"";
    if ( !atSerial.send( 100, cmd) ) return false;
    if ( !atSerial.send( 10000, "AT+CIICR") ) return false;
    if ( !atSerial.send( 2000, "AT+CIFSR") ) return false;
//    atSerial.m_ip = "";
//    for ( int i = 0; i < atSerial.m_response.length(); i++ )
//        if ( (atSerial.m_response[i] >= '0' && atSerial.m_response[i] <= '9') || (atSerial.m_response[i] == '.') )
//            atSerial.m_ip += atSerial.m_response[i];
	return true;
}

uint32_t Gprs::calculatePsm( uint32_t &idletime, uint32_t &psmtime)
{
    uint8_t t3324, t3412;
    uint32_t b3324, b3412;

    // IDLE TIME
    if ( idletime < 120 )   { b3324 = 2, t3324 = iotTimer( 0, 2, idletime, 0, 62); }
    else
    if ( idletime < 2160 )  { b3324 = 60, t3324 = iotTimer( 1, 60, idletime, 120, 1860); }
    else
                            { b3324 = 360, t3324 = iotTimer( 2, 360, idletime, 2160, 11160); }
    // SLEEP TIME
    uint32_t psm = idletime + psmtime;
    if ( psm < 90 )	     { b3412 = 2, t3412 = iotTimer( 3, 2, psm, 0, 62); }
    else
    if ( psm < 960 )     { b3412 = 30, t3412 = iotTimer( 4, 30, psm, 90, 930); }
    else
    if ( psm < 2400 )    { b3412 = 60, t3412 = iotTimer( 5, 60, psm, 960, 1860); }
    else
    if ( psm < 21600 )   { b3412 = 600, t3412 = iotTimer( 0, 600, psm, 2400, 18600); }
    else
    if ( psm < 144000 )  { b3412 = 3600, t3412 = iotTimer( 1, 3600, psm, 21600, 111600); }
    else
    if ( psm < 1152000 ) { b3412 = 36000, t3412 = iotTimer( 2, 36000, psm, 144000, 1116000); }
    else
                         { b3412 = 1152000, t3412 = iotTimer( 6, 1152000, psm, 1152000, 35712000); }

    psm = (t3412 & 0b00011111) * b3412;
    idletime = (t3324 & 0b00011111) * b3324;
    psmtime  =  psm - idletime;
    return psm;
}

bool Gprs::beginPsm( const uint32_t idletime, const uint32_t psmtime)
{
    uint8_t t3324, t3412;

    // IDLE TIME
    if ( idletime < 120 )   { t3324 = iotTimer( 0, 2, idletime, 0, 62); }
    else
    if ( idletime < 2160 )  { t3324 = iotTimer( 1, 60, idletime, 120, 1860); }
    else
                            { t3324 = iotTimer( 2, 360, idletime, 2160, 11160); }
    // SLEEP TIME
    uint32_t psm = idletime + psmtime;
    if ( psm < 90 )	     { t3412 = iotTimer( 3, 2, psm, 0, 62); }
    else
    if ( psm < 960 )     { t3412 = iotTimer( 4, 30, psm, 90, 930); }
    else
    if ( psm < 2400 )    { t3412 = iotTimer( 5, 60, psm, 960, 1860); }
    else
    if ( psm < 21600 )   { t3412 = iotTimer( 0, 600, psm, 2400, 18600); }
    else
    if ( psm < 144000 )  { t3412 = iotTimer( 1, 3600, psm, 21600, 111600); }
    else
    if ( psm < 1152000 ) { t3412 = iotTimer( 2, 36000, psm, 144000, 1116000); }
    else
                         { t3412 = iotTimer( 6, 1152000, psm, 1152000, 35712000); }

    String s3324( t3324, BIN);
    String s3412( t3412, BIN);
    while ( s3324.length() < 8 ) s3324 = String(0) + s3324;
    while ( s3412.length() < 8 ) s3412 = String(0) + s3412;

/* FROM WAVESHARESIM7030
    if ( !atSerial.send( 500, "AT+CPSMSTATUS=1") ) return false;
//    if ( !atSerial.send( 500, "AT+IPR=115200") ) return false;
//    if ( !atSerial.send( 500, "AT+CEREG=4") ) return false;
//    if ( !atSerial.send( 500, "AT+CEREG?") ) return false;
    if ( !atSerial.send( 2000, "AT+CPSMS=1,,,\"01011111\",\"00000001\"") ) return false;
    return true;
*/
    return false;
}

bool Gprs::endPsm()
{
/* FROM WAVESHARESIM7030
//    if ( !atSerial.send( 2000, "AT+CEREG?") ) return false;
//    if ( !atSerial.send( 2000, "AT+CEREG=0") ) return false;
    if ( !atSerial.send( 2000, "AT+CPSMS=0") ) return false;
    return true;
*/
    return false;
}

bool Gprs::sleep( const hirestime wakeuptime, callback onwakeup)
{
    // YET TO IMPLEMENT ONWAKEUP
	return false;
}

bool Gprs::sleep( const int wakeuppin, callback onwakeup)
{
    // YET TO IMPLEMENT ONWAKEUP
    return false;
}

bool Gprs::wakeup()
{
    // YET TO IMPLEMENT ONWAKEUP
    return false;
}

bool Gprs::connect()
{
    String cmd = "AT+CIPSTART=\"TCP\",\""; // TCP: overrule m_method
    cmd += atSerial.m_url;
    cmd += "\",";
    cmd += atSerial.m_port;
    if ( !atSerial.send( 5000, cmd) ) return false;
    return true;
}

bool Gprs::disconnect()
{
    if ( !atSerial.send( 1000, "AT+CIPCLOSE") ) return false;
    if ( !atSerial.send( 1000, "AT+CIPSHUT") ) return false;
    return true;
}

bool Gprs::post( const String message)
{
    m_msg.add( message);
    if ( !atSerial.asleep() ) {
        while ( m_msg.size() ) {
            String msg = m_msg.at( 0);
            String cmd = "AT+CIPSEND=";
            cmd += String( msg.length());
            if ( !atSerial.send( 10000, cmd, msg) )
                break;
            m_msg.removeAt( 0);
            atSerial.clearBuffer();
        }
        atSerial.clearBuffer();
    }
    return (m_msg.size() > 0 ? false : true);
}

String Gprs::imei()
{
	if ( !atSerial.send( 500, "AT+CSQ") ) return false; // ??? needed ???
	if ( !atSerial.send( 500, "AT+GSN") ) return false;
	// TODO retrieve imei from response
	// String resp = atSerial.reponse();
    // ...
    return atSerial.response();
}

String Gprs::response()
{
    String resp = atSerial.response();
    resp = resp.substring( 0, resp.length() - 4);
    return resp;
}

uint Gprs::pending()
{
    return m_msg.size();
}

void Gprs::clearPending()
{
    m_msg.clear();
}

uint8_t Gprs::iotTimer( const uint8_t id, const uint32_t base, const uint32_t msec, const uint32_t min, const uint32_t max)
{
    uint8_t  tmr = (id << 5);
    uint32_t ms = msec;
    if ( ms < min ) ms = min;
    if ( ms > max ) ms = max;
    ms /= base;
    return (tmr | ms);
}

} // end namespace
