// file:   Sound.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef SOUND_H
#define SOUND_H

#include "actuator.h"
#include "linkedlist.h"

class Sound : public Actuator
{
public:
    Sound() {};
    ~Sound() {};

    uint8_t add( String sound, bool astext = false);
    void insert( uint8_t pos, String sound, bool astext = false);
    void remove( uint8_t pos, uint8_t count = 1);
    void clear();

    void play();                    // plays the list
    void playFile( String path);    // calls 'aplay' to play the file
    void playText( String text);    // calls 'espeak' to say the text

protected:

    LinkedList<String>  m_item; // wav-file or spoken text
    LinkedList<uint8_t> m_type; // 0 = wav-file, 1 = spoken text
};

#endif // Sound_H
