#include <sqlclient.h>
SqlClient sql;


void setup()
{
	if ( sql.connect( "10.0.0.1", "SBG", "sbg_client", "welkom") ) {
		sprintln( "contact");
		if ( sql.query ( "SELECT infotekst FROM infospot WHERE vorigetag = \"0x64 0xF5 0xE5 0x20\" AND huidigetag = \"0x64 0xF5 0xE5 0x20\" ORDER BY volgorde ;") ) {
			StringList h;
			h = sql.columnHeaders();
			for ( uint c = 0; c < h.size(); c++ )
				sprint( h[c] + ", ");
			sprintln( "");
			h = sql.nextRow();
			while ( h.size() > 0 ) {
				for ( uint i = 0; i < h.size(); i++ )
					sprintln( h[i]);
				h = sql.nextRow();
			}
		}
	}
}

void loop()
{
}
