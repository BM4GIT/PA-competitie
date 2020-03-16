// OnzeApp.cpp
// Januari 2020
#include <hcsr04.h>
#include <hcsr501.h>
#include <sound.h>

using namespace HCSR04;
using namespace HCSR501;

Distance dist;
Sound snd;
Motion mot;
Average avg;

void setup()
{
	dist.setPin(5,6);
	mot.setPin(20);

	snd.init( snd.Local);
	snd.add( "/home/pi/Effect.wav");
	snd.play();
}

void loop()
{
	mot.read();
	dist.read();
    if ( dist.dataReady() )
		avg.add(dist.cm());
	if ( mot.detected() && (avg.avg() < 100) ) {
		sprintln( dist.cm());
		snd.play();
	}
}
