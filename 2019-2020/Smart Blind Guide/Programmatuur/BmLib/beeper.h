// file:   beeper.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef BEEPER_H
#define BEEPER_H

#include "actuator.h"
#include "linkedlist.h"

class Beeper : public Actuator
{
public:
    Beeper();
    ~Beeper();

    void setPin( uint8_t pin);

    void setBeeper( uint16_t herz, uint16_t msec);

    uint8_t addTone( uint16_t herz, uint16_t msec);
    void insertTone( uint8_t pos, uint16_t herz, uint16_t msec);
    void deleteTone( uint8_t pos, uint8_t count = 1);
    void clear();

    void play();    // plays the tones

    void setOn();   // activates the beeper
    void setOff();

protected:

    void playtone( uint16_t herz, uint16_t msec);

    uint16_t                m_beep;
    uint16_t                m_dur;

    uint8_t                 m_pin;
    LinkedList<uint16_t>    m_hz;
    LinkedList<uint16_t>    m_tm;
};

#endif // Beeper_H
