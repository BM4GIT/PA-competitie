#ifndef Gprs_H
#define Gprs_H

#include "Arduino.h"
#include "linkedlist.h"

#define TCP         0
#define UDP         1

typedef void (*callback)();

namespace WaveshareSim7020
{

class Gprs
{
public:

#ifdef SOFTSERIAL
    void beginAtSerial( const uint rxpin, const uint txpin);
#else
#ifdef ARDUINO_AVR_MEGA2560
    void beginAtSerial( const uint serialinterface); // SERIAL, SERIAL1, SERIAL2 or SERIAL3
#else
    void beginAtSerial();
#endif
#endif

    void setPin( const String pin);
    void setProvider( const String apn, const String user = "", const String password = "");
    void setNetwork( const String id, const String band = "");
    void setMethod( const uint method); // TCP or UDP
    void setServer( const String url, const String port);

    // call 'set...' routines before 'startup'
    bool startup();

    // Requesting Power Saving Mode (PSM):
    // Before going into PSM the network will enter idle time.
    //   |-----------|--------------------------|
    //   ^ idle time ^         psm time         ^
    //   |   T3324   |                          |
    //   |         T3412 = idle+psm             |
    // During PSM user requests are handled but user notifications
    // from the network are blocked. PSM can be canceled by the user.

    uint32_t calculatePsm( uint32_t &idleTime, uint32_t &psmTime);
    bool beginPsm( const uint32_t idleTime, const uint32_t psmTime);
    bool endPsm();

    // PSM only affects the network activity, but does not bring the module itself to sleep
    // This is a separate activity. When asleep, a module can be waked up again by an interrupt.
    // The interrupt may come from an external gpio pin or from an internal timer.

    bool sleep( const hirestime wakeuptime, callback onwakeup = NULL);
    bool sleep( const int wakeuppin, callback onwakeup = NULL);
    bool wakeup();

    String imei();

    bool connect();                 // connect before one or multiple posts
    bool post( const String msg);   // posts msg directly when active, but pends msg when asleep
    String response();              // gets the response to a post (in fifo order after wakeup)
    bool disconnect();              // disconnect when posting is done

    uint pending(); // returns the number of pending messages
    void clearPending();

private:

    uint8_t iotTimer( uint8_t id, uint32_t base, uint32_t msec, uint32_t min, uint32_t max);

    LinkedList<String>  m_msg;
};

} // end namespace

#endif
