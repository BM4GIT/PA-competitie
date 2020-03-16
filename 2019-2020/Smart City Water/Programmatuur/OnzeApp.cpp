// OnzeApp.cpp
// Januari 2020
#include <dcmotor.h>
#include <switch.h>

DcMotor pomp1;
DcMotor pomp2;
Switch vlotter1;
Switch vlotter2;
Switch ws1;
Switch ws2;

long tijd;

void setup()
{
	pomp1.setPin( 26, false);
	pomp2.setPin( 20, false);
	vlotter1.setPin( 19);
	vlotter2.setPin( 16);
	ws1.setPin( 6);
	ws2.setPin( 12);
}

void loop()
{
	ws1.read();
	if ( ws1.pressed() ) {
		pomp1.setOn();
		delay( 5000);
	}
	else
		pomp1.setOff();
}
