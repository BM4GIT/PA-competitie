// file:   dst.cpp
// author: D.E.Veloper
// date:   2020-01-01

#include "ds18.h"
#include <dirent.h>
#include <fstream>

namespace DS18
{

Temperature::Temperature()
{
    m_pin = 4;
    m_temp = 0;
}

StringList Temperature::getIdList()
{
	StringList idlist;
	DIR *dir;
	struct dirent *ent;

	if ( (dir = opendir( "/sys/bus/w1/devices")) != NULL ) {
		while ( (ent = readdir( dir)) != NULL )
			idlist.push_back( ent->d_name);
		closedir( dir);
	}
    for( int ix = idlist.size() - 1; ix >= 0; ix-- )
        if ( idlist[ix].substring( 0, 3) != "28-" )
            idlist.erase( idlist.begin() + ix);
    return idlist;
}

void Temperature::setSensor( String id, int pin)
{
    m_id = id;
    m_pin = pin;
    if ( m_id == "" ) {
        // set to first dallas id
        StringList idlist = getIdList();
        if ( idlist.size() > 2)
            m_id = idlist.at( 2);
    }
}

void Temperature::read()
{
	m_temp = 0;
    if ( m_id == "" )
        return;

    String fn = String( ) + m_id + String( "/w1_slave");
	ifstream in;
	in.open( fn.c_str());
	if ( in.is_open() ) {

        String value;
        char val = '!';
        while ( !in.eof() && val != 0  ) {
            in >> val;
            value += val;
        }
        int ix = value.lastIndexOf( "t=");
        if ( ix < 0 ) {
            m_temp = 0;
            return;
        }
        else
            m_temp = value.substring( ix - 2).toFloat() / 1000 + 273;
    }
    else {
        m_temp = 0;
        return;
    }

    Sensor::read();
}

float Temperature::kelvin()
{
    return m_temp;
}

float Temperature::celcius()
{
    return m_temp - 273;
}

float Temperature::fahrenheit()
{
    return (m_temp - 273) * 9 / 5 + 32;
}

} // end namespace
