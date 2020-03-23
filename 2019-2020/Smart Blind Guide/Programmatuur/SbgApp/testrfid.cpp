#include <pn532.h>
using namespace PN532;

Rfid r;

void setup()
{
	r.setTimeout( 100);
	if ( !r.init( 20, 4) ) {
		sprintln( "Error");
		exit( 1);
	}
}

void loop()
{
	r.read();
	if ( r.dataReady() )
		sprintln( r.tag());
}
