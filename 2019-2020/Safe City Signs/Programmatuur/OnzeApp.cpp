// OnzeApp.cpp
// Januari 2020

#include <usbcamera.h>

UsbCamera cam;
int i = 0;

void toonSnelheid( int snelheid)
{
	// opsplitsen in decimale getallen
	int t = snelheid / 10;
	int e = snelheid - t * 10;

	// opsplitsen in binaire waardes
	digitalWrite( 11, t & 1);
	digitalWrite( 12, t & 2);
	digitalWrite( 13, t & 4);
	digitalWrite( 14, t & 8);
}


void setup()
{
	cam.init( 640, 480); 
	cam.setAnalyze( 3, {20,20});
	cam.start();
}

void loop()
{
	cam.read();
	Mat img = cam.image();
	sprintln( cam.percentageChanged( ALT_COLOR));
	imshow( "Test", img);
	waitKey( 40);
}
