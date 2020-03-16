#include <linkedlist.h>
#include <sound.h>
#include <beeper.h>

Beeper bzr;
Sound snd;
LinkedList<String> strl;

void setup()
{
	bzr.setPin (6);
	bzr.addTone(220,500); 
	bzr.addTone(440,500); 
	bzr.addTone(880,750); 
	strl.add( "[]Hello this is Espeak");
	strl.add( "[]I am a talking raspberry");
	strl.add( "[]Do you like it?");

	for ( uint s = 0; s < 3; s++ )
		snd.playText( strl[s]);
}

void loop()
{
	bzr.play();    
	delay(3000);
}
