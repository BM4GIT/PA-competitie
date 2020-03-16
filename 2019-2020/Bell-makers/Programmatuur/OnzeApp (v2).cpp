// OnzeApp.cpp
// Januari 2020
#include <hcsr04.h>
#include <rotary.h>
#include <sound.h>

using namespace HCSR04;

Distance dist;
Sound snd;
Motion rot;
Average avg;

void setup()
{
	dist.setPin(5,6);
	mot.setPin(20,21);

	snd.init( snd.Local);
	snd.add( "/home/pi/Effect.wav");
	snd.play();
}

void loop()
{
	rot.read();
	dist.read();
    if ( dist.dataReady() )
		avg.add(dist.cm());
	if ( rot.moving() && (avg.avg() < 100) ) {
		sprintln( dist.cm());
		snd.play();
	}
}
