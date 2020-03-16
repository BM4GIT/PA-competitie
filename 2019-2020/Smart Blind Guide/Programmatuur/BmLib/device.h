// file:   device.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef DEVICE_H
#define DEVICE_H

#include "Arduino.h"
#include <list>
#include <thread>
#include <chrono>

using namespace std;

class Device;

struct TimerEvent
{
    void (*event)();
    long dtime;
    chrono::time_point<chrono::high_resolution_clock> ftime;
};

typedef list<TimerEvent*> TimerEvents;

class TimerThread
{
public:

    TimerThread();
    ~TimerThread();

    TimerEvent* startEvent( long msec, void (*event)());
    void stopEvent( TimerEvent* te);

protected:

};

class Device
{
public:

    Device();
    ~Device();

    // event handling
    void startTimerEvent( long msec, void (*event)());
    void stopTimerEvent();

protected:

    TimerEvent *m_ei;
};

extern TimerThread TT;

#endif
