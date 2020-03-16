#include "json.h"

//##########
//## JSON ##
//##########

#define JSON_VALUES   0
#define JSON_ELEMENTS 1
#define JSON_ARRAYS   2

JSON::JSON()
{
    m_build = JSON_VALUES;
}

void JSON::addToValues( const String name, const String value, bool inquotes)
{
    if ( m_values.length() ) m_values += ",";
    m_values += "\"";
    m_values += name;
    m_values += (inquotes ? "\":\"" : "\":");
    m_values += value;
    if ( inquotes ) m_values += "\"";
}

void JSON::addToValues( const String name, const String value)
{
    addToValues( name, value, true);
}

void JSON::addToValues( const String name, long value)
{
    String val( (int) value);
    addToValues( name, val, false);
}

void JSON::addToValues( const String name, float value, uint8_t decimals)
{
    String val( value, decimals);
    addToValues( name, val, false);
}

void JSON::addToElements( const String json)
{
    if ( m_elements.length() ) m_elements += ",";
    m_elements += json;
    m_build = JSON_ELEMENTS;
}

void JSON::addToArrays( const String json)
{
    if ( m_arrays.length() ) m_arrays += ",";
    m_arrays += json;
    m_build = JSON_ARRAYS;
}

void JSON::addElementsToValues()
{
    if ( m_values.length() ) m_values += ",";
    m_values += m_elements;
    m_elements = "";
}

void JSON::addArraysToValues()
{
    if ( m_values.length() ) m_values += ",";
    m_values += m_arrays;
    m_arrays = "";
}

void JSON::addValuesToElements()
{
    if ( m_elements.length() ) m_elements += ",";
    m_elements += "{";
    m_elements += m_values;
    m_elements += "}";
    m_values = "";
    m_build = JSON_ELEMENTS;
}

void JSON::addArraysToElements()
{
    if ( m_elements.length() ) m_elements += ",";
    m_elements += "{";
    m_elements += m_arrays;
    m_elements += "}";
    m_arrays = "";
    m_build = JSON_ELEMENTS;
}

void JSON::addValuesToArrays()
{
    if ( m_arrays.length() ) m_arrays += ",";
    m_arrays += "[";
    m_arrays += m_values;
    m_arrays += "]";
    m_values = "";
    m_build = JSON_ARRAYS;
}

void JSON::addElementsToArrays()
{
    if ( m_arrays.length() ) m_arrays += ",";
    m_arrays += "[";
    m_arrays += m_elements;
    m_arrays += "]";
    m_elements = "";
    m_build = JSON_ARRAYS;
}

String JSON::findValue( const String name, uint8_t occurence)
{
    if ( m_build == JSON_VALUES ) return "";

    String json = (m_build == JSON_ARRAYS ? m_arrays : m_elements);
    int i = 0, len = json.length();
    bool start = false, quotes = false;
    String nm, val;

    while ( i < len ) {
        if ( start && (json[i] == '\"') ) {
            quotes = true;
            start = false;
        }
        else
        if ( quotes ) {
            if ( json[i] == '\"' ) {
                if ( nm == name ) occurence--;
                if ( !occurence ) break; // THE TARGET IS FOUND
                quotes = false;
                nm = "";
            }
            else
                nm += json[i];
        }
        else
        if ( json[i] == ',' || json[i] == '{' || json[i] == '[' ) {
            start = true;
        }

        i++;
    }
    if ( i == len ) return ""; // THE TARGET IS NOT FOUND
    while ( i < len && json[i] != ':' ) i++;
    i++;
    while ( i < len && json[i] != ',' && json[i] != '}' && json[i] != ']' ) {
        val += json[i];
        i++;
    }
    val.trim();
    if ( val.length() && (val[0] == '\"') )
        val = val.substring( 1, val.length() - 1); // assume a closing quote too
    return val;
}

String JSON::build()
{
    if ( m_build == JSON_VALUES ) return "";
    m_values = "";
    String json = (m_build == JSON_ARRAYS ? m_arrays : m_elements);
    m_elements = "";
    m_arrays = "";
    return json;
}

void JSON::clear()
{
    m_values = "";
    m_elements = "";
    m_arrays = "";
    m_build = JSON_VALUES;
}

//###############
//## ECRYPTION ##
//###############

Encryption::Encryption()
{
}

void Encryption::setKey( const String key)
{
    m_key = key;
}

String Encryption::encrypt( const String msg)
{
	String m = msg;
	String key = extend_key( m_key, m.length());
	String str;
	
    for ( int i = 0; i < m.length(); i++ ) {
        unsigned char cm = m[i];
        unsigned char ck = key[i];
        cm ^= ck;
		// replace NULL character by 128 to avoid end of string
		// when sent over LoRa it becomes 0 automatically
        if ( !cm ) cm = 128; 
        str += String( char( cm));
    }

	Serial.print( "M: "); Serial.println( m);
	Serial.print( "E: "); Serial.println( str); Serial.println( "");
	return str;
}

String Encryption::decrypt( const String msg)
{
	String m = msg;
	String key = extend_key( m_key, m.length());
	String str;

    for ( int i = 0; i < m.length(); i++ ) {
        unsigned char cm = m[i];
        unsigned char ck = key[i];
		// turn character 128 back into NULL character (see encryption)
        if ( cm == 128 ) cm = 0;
        cm ^= ck;
        str += String( char( cm));
    }

	Serial.print( "E: "); Serial.println( m);
	Serial.print( "M: "); Serial.println( str); Serial.println( "");
	return str;
}

String Encryption::extend_key( const String key, int len)
{
	String k = key;
    String extkey;
    int keylen = k.length();
	int i, j;
    for( i = 0, j = 0; i < len; i++, j++ ) { 
        if( j == keylen ) j = 0;
        extkey += String( k[j]);
    }
    return extkey;
}
