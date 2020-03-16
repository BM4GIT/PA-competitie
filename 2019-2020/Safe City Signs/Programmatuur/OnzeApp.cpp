// OnzeApp.cpp
// Januari 2020

#include <usbcamera.h>

UsbCamera cam;
int i = 0;
int perc;
long timer;

void toonSnelheid( int snelheid)
{
	// opsplitsen in decimale getallen
	int t = snelheid / 10;
	int e = snelheid - t * 10;

	// opsplitsen in binaire waardes
	digitalWrite( 5, t & 1);
	digitalWrite( 6, t & 2);
	digitalWrite( 13, t & 4);
	digitalWrite( 19, t & 8);

	digitalWrite( 12, e & 1);
	digitalWrite( 16, e & 2);
	digitalWrite( 20, e & 4);
	digitalWrite( 21, e & 8);
}


void setup()
{
	cam.init( 640, 480); 
	cam.setAnalyze( 3, { 5, 5});
	cam.start();
	timer = millis();
}

void loop()
{
	cam.read();
	if ( cam.dataReady() ) {
		perc = cam.percentageChanged( ALT_COLOR);
		if ( timer + 15000 > millis() ) {
			if ( perc < 2 )
				toonSnelheid( 50);
			else
			if ( perc < 4 )
				toonSnelheid( 40);
			else
			if ( perc < 8 )
				toonSnelheid( 30);
            else
            if ( perc < 10 )
                toonSnelheid( 20);
            else
                toonSnelheid( 10);
			timer = millis();
		}

		imshow( "Test", cam.image());
		waitKey( 40);
	}
}
