#ifndef JSON_H
#define JSON_H

#include "Arduino.h"

class JSON
{
public:
    JSON();

    void clear();

    void addToValues( const String name, const String value);
    void addToValues( const String name, long value);
    void addToValues( const String name, float value, uint8_t decimals);

    // next routines add a json object to another json object
    // both objects will be comma separated solely and are NOT formatted as a new object
    // with the '..ToValues' versions the combination is placed in the values stack
    // with the '..ToElements' version the combination is placed in the elements stack
    // with the '..ToArrays' version the combination is placed in the arrays stack
    // the added object is cleared afterwards
    void addToElements( const String json);
    void addToArrays( const String json);
    void addElementsToValues();
    void addArraysToValues();

    // next routines add a json object to another json object
    // both objects will be comma separated and ARE formatted as a new object
    // with the '..ToElements' versions the new object is an element and is placed in the elements stack
    // with the '..ToArrays' versions the new object is an array and is placed in the arrays stack
    // the added object is cleared afterwards
    void addValuesToElements();
    void addArraysToElements();
    void addValuesToArrays();
    void addElementsToArrays();

    // find the n'th occurrence of a value in an object
    String findValue( const String name, uint8_t occurence = 1);
 
    // next routines return the content of either arrays or elements
    String build();

protected:
    void addToValues( const String name, const String value, bool inquotes);

    String m_values;
    String m_elements;
    String m_arrays;

    uint8_t m_build;
};

class Encryption
{
public:
    Encryption();

	void setKey( const String key);

    String encrypt( const String msg);
    String decrypt( const String msg);

protected:
    String extend_key( const String key, int len);

    String m_key;
};

#endif
