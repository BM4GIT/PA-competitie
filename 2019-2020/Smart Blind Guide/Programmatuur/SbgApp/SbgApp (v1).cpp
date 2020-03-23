#include <sqlclient.h>
#include <sound.h>
#include <beeper.h>
#include <pn532.h>
#include <rgbled.h> 
using namespace PN532;
#include <switch.h>

Switch sw;
SqlClient sql;
StringList sl;
Sound spk;
Beeper snd;
Rfid rfid;
RgbLed led;

String htg;
String vtg;
String qry;

void setup()
{
	// led setup
	led.setPin ( 13,19,26,false);
	led.setOn ();

	// rfid setup
	pinMode( 10, OUTPUT);
	digitalWrite( 10, LOW);
	rfid.setTimeout( 1000);
	if ( !rfid.init( 20, 4) ) {
		// rfid is fout
		led.setColor( led.Red);
		exit( 1);
	}
	rfid.setAuthentication( DEFAULT_KEY);

	// led op blauw
	led.setColor( led.Blue);

	// piep geluid
	snd.setPin( 6);
	snd.addTone( 220, 500); 
	snd.addTone( 440, 500); 
	snd.addTone( 880, 750);
	snd.play();

	// is de knop ingedrukt?
	sw.setPin( 5);
	while ( sw.released() ) {
		sw.read();
	}

	// is er contact met de database?
	while ( !sql.connect ( "10.0.0.1", "SBG", "sbg_client", "welkom") ) {
		led.setColor( led.Red);
		spk.playText( "[]No connection with the database.");
		spk.playText( "[]Please wait a moment.");
		led.setColor( led.Blue);
		led.blueLed()->setBlink( 500, 500);
		delay (5000);
		led.blueLed()->setBlink();
	}

	//klaar
	spk.playText( "[][]Connected to the database[] []the System is ready to be used.");
	led.setColor( led.Green);
}

void loop()
{
	// is er een chip gescand?
	rfid.read();
	if ( rfid.dataReady() ) {
		// ja led blauw
		led.setColor( led.Blue);
		
		// vorige tag wordt huidige tag
		vtg = htg;
		htg = rfid.tag();

		// maak de query
		qry = sql.filloutQuery( "SELECT infotekst FROM infospot WHERE vorigetag = \"@@1\" AND huidigetag = \"@@2\" ORDER BY volgorde", vtg, htg);

		// query de database
		if ( sql.query( qry) ) {
			sl = sql.nextRow();
			// spreek uit
			while ( sl.size() )  {
				spk.playText( sl.at( 0));
				sl = sql.nextRow();
			}
		}

		// led groen
		led.setColor( led.Green);
	}


}
