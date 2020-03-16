// file:   led.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "led.h"
#include "linkedlist.h"

struct BlinkInfo {
    int     pin;
    bool    pwm;
    int     max;
    int     min;
    int     ton;
    int     toff;
    int     time;
    bool    blink;
    bool    invert;
};
LinkedList<BlinkInfo*> g_blink;

void onBlinkEvent()
{
    for ( int i = 0; i < g_blink.size(); i++ ) {
        BlinkInfo *blk = g_blink[i];
        long tm = millis();
        if ( blk->blink && (blk->time + blk->ton < tm) ) {
            blk->time = tm;
            if ( blk->pwm )
                analogWrite( blk->pin, blk->invert ? BRIGHT - blk->min : blk->min);
            else
                digitalWrite( blk->pin, blk->invert ? HIGH : LOW);
            blk->blink = false;
        }
        if ( !blk->blink && (blk->time + blk->toff < tm) ) {
            blk->time = tm;
            if ( blk->pwm )
                analogWrite( blk->pin, blk->invert ? BRIGHT - blk->max : blk->max);
            else
                digitalWrite( blk->pin, blk->invert ? LOW : HIGH);
            blk->blink = true;
        }
    }
}

Led::Led()
{
    m_pin = -1;
    m_pwm = false;
    m_max = BRIGHT;
    m_min = 0;
    m_ton = NOBLINK;
    m_toff = NOBLINK;
    m_isblinking = false;
    m_invert = false;
}

Led::~Led()
{
    setOff();
}

void Led::setPin( unsigned int pin, bool pwm)
{
    m_pin = pin;
    m_pwm = pwm;
    pinMode( pin, OUTPUT);
}

void Led::setInversion( bool invert)
{
    m_invert = invert;
}

void Led::setBrightness( int on, int off)
{
    if ( on < 0 ) on = 0;
    if ( on >= 100 )
        m_max = BRIGHT;
    else
        m_max = on * 1023 / 100;
    if ( off < 0 ) off = 0;
    if ( off >= 100 )
        m_min = BRIGHT;
    else
        m_min = off * 1023 / 100;
    if ( m_ison )
        setOn();
}

void Led::setBlink( int on, int off)
{
    if ( on < NOBLINK ) on = NOBLINK;
    if ( off < NOBLINK ) off = NOBLINK;
    m_ton = on;
    m_toff = off;
    if ( m_ison )
        setOn();
}

void Led::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    if ( m_pwm )
        analogWrite( m_pin, m_invert ? BRIGHT - m_max : m_max);
    else
        digitalWrite( m_pin, m_invert ? (m_max ? LOW : HIGH) : (m_max ? HIGH : LOW));
    if ( m_ton != NOBLINK && m_toff != NOBLINK ) {
        m_isblinking = true;
        BlinkInfo* bi = new BlinkInfo;
        bi->pin = m_pin;
        bi->pwm = m_pwm;
        bi->max = m_max;
        bi->min = m_min;
        bi->ton = m_ton;
        bi->toff = m_toff;
        bi->time = 0;
        bi->blink = true;
        bi->invert = m_invert;
        g_blink.add( bi);
        startTimerEvent( 1, onBlinkEvent);
    }
}

void Led::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    m_isblinking = false;
    stopTimerEvent();

    list<BlinkInfo*>::iterator i;
    for ( int i = 0; i < g_blink.size(); i++ ) {
        BlinkInfo *blk = g_blink[i];
        if ( blk->pin == m_pin ) {
            g_blink.removeAt( i);
            delete blk;
            break;
        }
    }
    if ( m_pwm )
        analogWrite( m_pin, m_invert ? BRIGHT : 0);
    else
        digitalWrite( m_pin, m_invert ? HIGH : LOW);
}
