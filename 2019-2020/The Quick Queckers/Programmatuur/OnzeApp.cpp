// OnzeApp.cpp
// Januari 2020

#include <rgbled.h>
#include <switch.h>
#include <realtime.h>

RgbLed rgb;
Switch sw1, sw2, sw3, sw4;

RealTime tijd;
int wacht;
int ledkleur;

void setup()
{
	rgb.setPin( 21, 20, 16, false);
	rgb.setOn();
	
	sw1.setPin( 5);
	sw2.setPin( 6);
	sw3.setPin( 13);
	sw4.setPin( 19);

	tijd.read();
	wacht = 5;

	ledkleur = random( 1, 4);
}

void loop()
{
	// rgb ledjes aanzetten met de goede kleur
	if ( ledkleur == 1 ) {
		rgb.setColor( rgb.Red);
		// wachten op de juiste knop
		sw1.read();
		if ( sw1.pressed() ) {
			wacht = wacht - 1;
			ledkleur = random( 1, 4);
		}
	}
	else
	// rgb ledjes aanzetten met de goede kleur
	if ( ledkleur == 2 ) {
		rgb.setColor( rgb.Green);
		// wachten op de juiste knop
		sw2.read();
		if ( sw2.pressed() ) {
			wacht = wacht - 1;
			ledkleur = random( 1, 4);
		}
	}
	else
	// rgb ledjes aanzetten met de goede kleur
	if ( ledkleur == 3 ) {
		rgb.setColor( rgb.Blue);
		// wachten op de juiste knop
		sw3.read();
		if ( sw3.pressed() ) {
			wacht = wacht - 1;
			ledkleur = random( 1, 4);
		}
	}
	else
	// rgb ledjes aanzetten met de goede kleur
	if ( ledkleur == 4 ) {
		rgb.setColor( rgb.Yellow);
		// wachten op de juiste knop
		sw4.read();
		if ( sw4.pressed() ) {
			wacht = wacht - 1;
			ledkleur = random( 1, 4);
		}
	}

	// de tijd is verstreken
	if ( RealTime::now() > tijd ) {
		ledkleur = random( 1, 4);
		wacht = wacht + 1;
        tijd.addSeconds( wacht);
	}

	// niet minder dan 2 seconden wachten
	if ( wacht < 2 ) wacht = 2;
	// niet langer dan 10 seconden wachten
	if ( wacht > 10 ) wacht = 10;
}
