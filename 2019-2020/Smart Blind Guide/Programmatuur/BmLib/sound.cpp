// file:   Sound.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "sound.h"

uint8_t Sound::add( String sound, bool astext)
{
    uint8_t ix = m_item.add( sound);
    m_type.add( astext);
    return ix;
}

void Sound::insert( uint8_t pos, String sound, bool astext)
{
    m_item.insert( pos, sound);
    m_type.insert( pos, astext);
}

void Sound::remove( uint8_t pos, uint8_t count)
{
    for ( int i = pos; i < pos + count; i++ ) {
        m_item.removeAt( i);
        m_type.removeAt( i);
    }
}

void Sound::clear()
{
    m_item.removeAll();
    m_type.removeAll();
}

void Sound::play()
{
    for (int i = 0; i < m_item.size(); i++ )
        if ( m_type[i] )
            playText( m_item[i]);
        else
            playFile( m_item[i]);
}

void Sound::playFile( String path)
{
	String play = "aplay -D bluealsa:HCI=hci0,DEV=D2:8D:DC:1C:48:37,PROFILE=a2dp ";
    play += path;
	system( play.c_str());
}

void Sound::playText( String text)
{
	String speak = "espeak \"[][][][] ";
    speak += text;
    speak += "\" -a20 -ven+f2 -k5 -s170 --stdout | aplay -D bluealsa:HCI=hci0,DEV=D2:8D:DC:1C:48:37,PROFILE=a2dp";
	system( speak.c_str());
}
