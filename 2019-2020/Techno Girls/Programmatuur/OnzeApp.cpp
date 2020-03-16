// OnzeApp.cpp
// Januari 2020

#include <dcmotor.h>
#include <switch.h>

DcMotor pomp1;
DcMotor pomp2;
DcMotor pomp3;
Switch plant1;
Switch plant2;
Switch plant3;

int  spd = 0;
bool fwd = true;

void setup()
{
	pomp1.setPin( 13, false);
	pomp2.setPin( 19, false);
	pomp3.setPin( 26, false);
	plant1.setPin( 21);
	plant2.setPin( 20);
	plant3.setPin( 16);

	pomp1.setOff();
	pomp2.setOff();
	pomp3.setOff();
}

void loop()
{
	// als plant 1 dorst heeft 3 sec water geven
	plant1.read();
	if ( plant1.pressed() ) {
		pomp1.setOn();
		delay( 3000);
		pomp1.setOff();
	}

	// als plant 2 dorst heeft 3 sec water geven
	plant2.read();
	if ( plant2.pressed() ) {
		pomp2.setOn();
		delay( 3000);
		pomp2.setOff();
	}

	// als plant 3 dorst heeft 3 sec water geven
	plant3.read();
	if ( plant3.pressed() ) {
		pomp3.setOn();
		delay( 3000);
		pomp3.setOff();
	}
}
