// file:   device.h
// author: D.E.Veloper
// date:   2020-01-01

#include "device.h"

///////////////////////////////////
// Global timer thread mechanism //
///////////////////////////////////

TimerEvents g_te;
thread      g_tt;
bool        g_tb = false;

void threadWorker()
{
    while ( g_tb ) {
        auto ct = chrono::high_resolution_clock::now();
        TimerEvents::iterator i;
        for ( i = g_te.begin(); i != g_te.end(); ++i ) {
            TimerEvent *te = *i;
            long dt = chrono::duration_cast<chrono::milliseconds>( ct - te->ftime).count();
            if ( dt > te->dtime ) {
                te->event();
                te->ftime = ct;
            }
        }
    }
}

TimerThread TT;


///////////////////////
// TimerThread class //
///////////////////////

TimerThread::TimerThread()
{
}

TimerThread::~TimerThread()
{
}

TimerEvent* TimerThread::startEvent( long msec, void (*event)())
{
    // start timer thread if not running yet
    if ( !g_te.size() ) {
        g_tb = true;
        g_tt = thread( threadWorker);
    }

    // add event to the timer event handling
    TimerEvent *te = new TimerEvent;
    te->event = event;
    te->dtime = msec;
    te->ftime = chrono::high_resolution_clock::now();
    g_te.push_back( te);
    return te;
}

void TimerThread::stopEvent( TimerEvent *te)
{
    // remove event from the timer event handling
    g_te.remove( te);
    delete te;

    // stop timer thread if no more timer events
    if ( !g_te.size() ) {
        g_tb = false;
        g_tt.join();
    }
}


//////////////////
// Device class //
//////////////////

Device::Device()
{
}

Device::~Device()
{
    stopTimerEvent();
}

void Device::startTimerEvent( long msec, void (*event)())
{
    if ( m_ei != nullptr )
        stopTimerEvent();
    m_ei = TT.startEvent( msec, event);
}

void Device::stopTimerEvent()
{
    if ( m_ei != nullptr )
        TT.stopEvent( m_ei);
    m_ei = nullptr;
}
