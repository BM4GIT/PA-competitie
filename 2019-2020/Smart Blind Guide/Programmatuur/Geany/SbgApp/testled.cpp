#include <rgbled.h>
#include <switch.h>

Switch sw;
RgbLed rgb;

void setup()
{
	rgb.setPin( 13, 19, 26, false);
	sw.setPin( 5);
}

void loop()
{
	sw.read();
	if ( sw.pressed() ) {
		rgb.setOn();
		rgb.setColor(rgb.Red);
		delay(500);
		rgb.setColor(rgb.Green);
		delay(500);
		rgb.setColor(rgb.Blue);
		delay(750);
	    rgb.setOff();
	}
}
