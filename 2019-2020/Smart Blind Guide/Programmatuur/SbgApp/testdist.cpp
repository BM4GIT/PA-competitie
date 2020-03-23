#include "vl53l0x.h"

VL53L0X lox;

void setup()
{
	pinMode( 10, OUTPUT);
	digitalWrite( 10, LOW);
	lox.setTimeout( 500);
	sprint( "Dist Sensor address: 0x");
	sprintln( lox.getAddress(), HEX);
	if ( !lox.init() ) {
		sprintln( "Failed to start up");
		exit( 1);
	}
	lox.setSignalRateLimit( 0.1);
	lox.setVcselPulsePeriod( VL53L0X::VcselPeriodPreRange, 18);
	lox.setVcselPulsePeriod( VL53L0X::VcselPeriodFinalRange, 14);
	lox.setMeasurementTimingBudget( 20000);
}

void loop()
{
	float d = lox.readRangeSingleMillimeters();
	if ( lox.timeoutOccurred() )
		Serial.println( "TIMEOUT");
	else {
		Serial.print( d, 2);
		Serial.println( " mm");
	}
}
