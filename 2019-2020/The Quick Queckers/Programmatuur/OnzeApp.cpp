// OnzeApp.cpp
// Januari 2020

#include <rgbled.h>
#include <led.h>
#include <switch.h>
#include <cstdlib> 
#include <ctime> 
#include <iostream>

RgbLed rgb;
Led led1, led2, led3, led4, led5, led6, led7, led8;
Switch sw1, sw2, sw3, sw4, sw5, sw6, sw7, sw8;

void setup()
{
	rgb.setPin( 21, 20, 16, false);
   
	led1.setPin( 19, false);
	led2.setPin( 18, false);
	led3.setPin( 22, false);
	led4.setPin( 23, false);
	led5.setPin( 24, false);
	led6.setPin( 26, false);
	led7.setPin( 29, false);
	led8.setPin( 31, false);
	
	sw1.setPin( 33);
	sw2.setPin( 35);
	sw3.setPin( 36);
	sw4.setPin( 37);
	sw5.setPin( 38);
	sw6.setPin( 40);
	sw7.setPin( 8);
	sw8.setPin( 10);

	srand((unsigned)time(0));
}

int kleur()
{
	int i = (rand()%4)+1;
	return i;
}

void loop()
{
	// een random kleur kiezen
	int ledkleur = kleur();

	
	// rgb ledjes aanzetten met de goede kleur
	if ( ledkleur == 1 ) rgb.setColor( rgb.Red);
	if ( ledkleur == 2 ) rgb.setColor( rgb.Green);
	if ( ledkleur == 3 ) rgb.setColor( rgb.Blue);
	if ( ledkleur == 4 ) rgb.setColor( rgb.Yellow);
	rgb.setOn();
	
	// wachten op een knop
}
