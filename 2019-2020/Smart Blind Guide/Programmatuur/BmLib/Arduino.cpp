// Arduino.cpp

#include <chrono>
#include <termio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <cstring>
#include "Arduino.h"

namespace unistd {
	//All functions of unistd.h must be called like this: unistd::the_function()
    #include <unistd.h>
};

hirestime currentTime()
{
    return chrono::high_resolution_clock::now();
}

ulong diffMillis( hirestime stop, hirestime start)
{
    return chrono::duration_cast<chrono::milliseconds>( stop - start).count();
}

ulong diffMicros( hirestime stop, hirestime start)
{
    return chrono::duration_cast<chrono::microseconds>( stop - start).count();
}

//////////////////
// CLASS STRING //
//////////////////

String::String()
{
}

String::String( const String &str)
{
	S = str.S;
}

String::String( const char chr)
{
	S = chr;
}

String::String( const char* str)
{
	S = str;
}

String::String( const string &str)
{
	S = str;
}

String::String( const int val, const NUMSYSTEM type)
{
	uint i = val;
	uint mask = 0xF;
	uint shift = 4;
	switch ( type ) {

		case BIN :	S = "";
					do {
						S.insert( 0, 1, ((i & 1) ? '1' : '0'));
						i = i >> 1;
					} while ( i );
					break;

		case OCT :	mask = 0x7;
					shift = 3;
		case HEX :	{
					uint8_t c;
					S = "";
					do {
						c = i & mask;
						if ( c < 10 ) c += '0';
						else c += ('A' - 10);
						S.insert( 0, 1, c);
						i = i >> shift;
					} while ( i );
					break;
					}

		default :	{
					char s[20];
					sprintf( s, "%d", i); S = s;
					break;
					}
											   
	}
}

String::String( const double f, const int decimals)
{
	char s[20];
	if ( decimals >= 0 ) {
		char fmt[10];
		sprintf( fmt, "%%0.%df", decimals);
		sprintf( s, fmt, f);
	}
	else
		sprintf( s, "%f", f);
	S = s;
}

String::~String()
{
}

char String::operator[] ( const uint index)
{
	return S[index];
}

String String::operator= ( const String str)
{
	S = str.S;
	return String( S);
}

String String::operator+ ( const String str)
{
	return S + str.S;
}

String String::operator+= ( const String str)
{
	S += str.S;
	return String( S);
}

bool String::operator== ( const String str)
{
	return S == str.S;
}

bool String::operator> ( const String str)
{
	return S > str.S;
}

bool String::operator>= ( const String str)
{
	return S >= str.S;
}

bool String::operator< ( const String str)
{
	return S < str.S;
}

bool String::operator<= ( const String str)
{
	return S <= str.S;
}

bool String::operator!= ( const String str)
{
	return S != str.S;
}

char String::charAt( const uint index)
{
	return S.at( index);
}

int String::compareTo( const String str)
{
	for ( uint i = 0; i < S.length() && i < str.S.length(); i++ )
		if ( S.at( i) != str.S.at( i) ) {
			if ( S.at( i) < str.S.at( i) ) return -i;
			return i;
		}
	return 0;
}

String String::concat( const String str1, const String str2)
{
	return str1.S + str2.S;
}

const char* String::c_str()
{
	return S.c_str();
}

bool String::endsWith( const String str)
{
	uint len = str.S.length();
	uint pos = S.length() - len;
	if ( pos > S.length() ) return false;
	return S.substr( pos, len) == str.S;
}

bool String::equals( const String str)
{
	return S == str.S;
}

bool String::equalsIgnoreCase( const String str)
{
	String s1 = str;
	String s2( S);
	s1.toLowerCase();
	s2.toLowerCase();
	return s1.S == s2.S;
}

void String::getBytes( byte buf[], const uint len)
{
	uint l = len;
	if ( S.length() < l )
		l = S.length();
	S.copy( (char*) buf, l);
}

int String::indexOf( const String str)
{
//    return S.find_first_of( str.S); // seems not to work with \n\r characters
    int i, j;
    for ( i = 0; i < (int) S.length() - (int) str.S.length() + 1; i++ ) {
	for ( j = 0; j < (int) str.S.length(); j++ )
	    if ( S[i+j] != str.S[j] ) break;
	if ( j == (int) str.S.length() )
	    return i;
    }
    return -1;
}

int String::indexOf( const char* str)
{
    return indexOf( String( str));
}

int String::lastIndexOf( const String str, const uint from)
{
//    return S.find_last_of( str.S); // seems not to work with \n\r characters

    int i;
    uint j;
    for ( i = S.length() - str.S.length(); i >= (int) from; i-- ) {
	for ( j = 0; j < str.S.length(); j++ )
	    if ( S[i+j] != str.S[j] ) break;
	if ( j == str.S.length() )
	    return i;
    }
    return -1;
}

int String::lastIndexOf( const char* str, const uint from)
{
    return lastIndexOf( String( str), from);
}

int String::length()
{
//    return S.length(); // seems not to work with \n\r characters
   return strlen( S.c_str());
}

void String::remove( const uint index, const uint count)
{
	S.erase( index, count);
}

String String::replace( const String str1, const String str2)
{
	string s = S;
	int len = str1.S.length();
	int pos = s.find( str1.S);
	while ( pos >= 0 ) {
		s.replace( pos, len, str2.S);
		pos = s.find( str1.S);
	}
	return s;
}

void String::reserve( const int size)
{
	S.reserve( size);
}

void String::setCharAt( const uint index, const char chr)
{
	S.replace( index, 1, &chr);
}

bool String::startsWith( const String str)
{
	return S.find_first_of( str.S) == 0;
}

String String::substring( const uint from, const uint to)
{
	if ( to >= 0 )
		return S.substr( from, to - from);
	else
		return S.substr( from, S.length() - from);
}

void String::toCharArray( char buf[], const uint len)
{
	uint l = len;
	if ( S.length() < l )
		l = S.length();
	S.copy( buf, l);
}

int String::toInt()
{
	return stoi( S);
}

double String::toFloat()
{
	return stod( S);
}

void String::toLowerCase()
{
	for ( uint i = 0; i < S.length(); i++ )
		if ( S[i] >= 'A' && S[i] <= 'Z' ) {
			char c = S[i] - 'A' + 'a';
			S.replace( i, 1, &c);
		}
}

void String::toUpperCase()
{
	for ( uint i = 0; i < S.length(); i++ )
		if ( S[i] >= 'a' && S[i] <= 'z' ) {
			char c = S[i] - 'a' + 'A';
			S.replace( i, 1, &c);
		}
}

void String::trim()
{
	uint from, to;
	for ( from = 0; from < S.length(); from++ )
		if ( S[from] != ' ' ) break;
	for ( to = S.length() - 1; to >= 0; to-- )
		if ( S[from] != ' ' ) break;
	S = S.substr( from, to - from + 1);
}


String String::toHex( const uint8_t *data, const uint size)
{
	String str;
	uint32_t szPos;
	for ( szPos=0; szPos < size; szPos++ )
	{
		str += String( data[szPos] < 10 ? "0x0" : "0x");
		str += String( data[szPos], HEX);
		if ( (size > 1) && (szPos != size - 1) )
			str += " ";
	}
	return str;
}

String String::toHexChar( const uint8_t *data, const uint size)
{
	String str = toHex( data, size);
	uint32_t szPos;

	str += " ";
	for ( szPos=0; szPos < size; szPos++ )
	{
		if ( data[szPos] <= 0x1F )
			str += ".";
		else
			str += String( (char) data[szPos]);
	}
	return str;
}

String String::fillout( const String str, const char c, const uint size)
{
	String s = str;
	for ( uint i = s.length(); i < size; i++ )
		s += c;
	return s;
}
//////////////////////
// END CLASS STRING //
//////////////////////

hirestime g_start;

struct pwmgoStruct {
	void (*pwmworker)();
	thread pwmthread;
	int  pin;
	int  high;
	int  low;
	bool go;
};

#define pwmgoMax 5 // number of pwmXLoop routines
pwmgoStruct pwmgo[pwmgoMax];

void pinMode( int pin, int mode)
{
    int fsel;
    switch ( mode ) {
	case OUTPUT :		fsel = BCM2835_GPIO_FSEL_OUTP; break;
	case INPUT :		fsel = BCM2835_GPIO_FSEL_INPT; break;
	case INPUT_PULLUP :	fsel = BCM2835_GPIO_FSEL_INPT;
				bcm2835_gpio_set_pud( pin, BCM2835_GPIO_PUD_UP);
				break;
	case INPUT_PULLDOWN :	fsel = BCM2835_GPIO_FSEL_INPT;
				bcm2835_gpio_set_pud( pin, BCM2835_GPIO_PUD_DOWN);
				break;
    }
    bcm2835_gpio_fsel( pin, fsel);
}

void analogWrite( int pin, int level)
{
    if ( level < 0 ) level = 0;
    if ( level > 1023 ) level = 1023;
    int high = level;
    int low = 1023 - level;

    // if pin already in use
    for ( int i = 0; i < pwmgoMax; i++ )
	if ( pwmgo[i].pin == pin ) {
	    if ( !level ) {
		// stop thread
		pwmgo[i].go = false;
		pwmgo[i].pwmthread.join();
		pwmgo[i].high = -1;
		pwmgo[i].low = -1;
		pwmgo[i].pin = -1;
	    }
	    else {
		// renew thread
		pwmgo[i].high = high;
		pwmgo[i].low = low;
	    }
	    return;
	}

    // find a free thread
    for ( int i = 0; i < pwmgoMax; i++ )
	if ( pwmgo[i].pin < 0 ) {
	    // start thread
	    pwmgo[i].pin = pin;
	    pwmgo[i].high = high;
	    pwmgo[i].low = low;
	    pwmgo[i].go = true;
	    pwmgo[i].pwmthread = thread( pwmgo[i].pwmworker);
	    break;
	}
}

long millis()
{
    hirestime now = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>( now - g_start).count();
}

long micros()
{
    hirestime now = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>( now - g_start).count();
}

void pwm0loop()
{
    printf( "Started 'pwm0loop'.\n");
    while ( pwmgo[0].go ) {
	digitalWrite( pwmgo[0].pin, HIGH);
	delayMicroseconds( pwmgo[0].high);
	digitalWrite( pwmgo[0].pin, LOW);
	delayMicroseconds( pwmgo[0].low);
    }
    printf( "Stopped 'pwm0loop'.\n");
}

void pwm1loop()
{
    printf( "Started 'pwm1loop'.\n");
    while ( pwmgo[1].go ) {
	digitalWrite( pwmgo[1].pin, HIGH);
	delayMicroseconds( pwmgo[1].high);
	digitalWrite( pwmgo[1].pin, LOW);
	delayMicroseconds( pwmgo[1].low);
    }
    printf( "Stopped 'pwm1loop'.\n");
}

void pwm2loop()
{
    printf( "Started 'pwm2loop'.\n");
    while ( pwmgo[2].go ) {
	digitalWrite( pwmgo[2].pin, HIGH);
	delayMicroseconds( pwmgo[2].high);
	digitalWrite( pwmgo[2].pin, LOW);
	delayMicroseconds( pwmgo[2].low);
    }
    printf( "Stopped 'pwm2loop'.\n");
}

void pwm3loop()
{
    printf( "Started 'pwm3loop'.\n");
    while ( pwmgo[3].go ) {
	digitalWrite( pwmgo[3].pin, HIGH);
	delayMicroseconds( pwmgo[3].high);
	digitalWrite( pwmgo[3].pin, LOW);
	delayMicroseconds( pwmgo[3].low);
    }
    printf( "Stopped 'pwm3loop'.\n");
}

void pwm4loop()
{
    printf( "Started 'pwm4loop'.\n");
    while ( pwmgo[4].go ) {
	digitalWrite( pwmgo[4].pin, HIGH);
	delayMicroseconds( pwmgo[4].high);
	digitalWrite( pwmgo[4].pin, LOW);
	delayMicroseconds( pwmgo[4].low);
    }
    printf( "Stopped 'pwm4loop'.\n");
}

extern void setup();	// must be declared in the '<application>.cpp' file
extern void loop();	// must be declared in the '<application>.cpp' file
			// the <application>.cpp' file must start with: #include "Arduino.h"

int main( int argc, char **argv)
{
    if ( !bcm2835_init() ) {
	printf( "Can not initialise bcm2835 library.\nProgram will be aborted.\n");
	bcm2835_close();
	return 1;
    }

    pwmgo[0].pwmworker = pwm0loop;
    pwmgo[1].pwmworker = pwm1loop;
    pwmgo[2].pwmworker = pwm2loop;
    pwmgo[3].pwmworker = pwm3loop;
    pwmgo[4].pwmworker = pwm4loop;
    for ( int i = 0; i < pwmgoMax; i++ ) {
	pwmgo[i].pin = -1;
	pwmgo[i].high = -1;
	pwmgo[i].low = -1;
	pwmgo[i].go = false;
    }

    g_start = chrono::high_resolution_clock::now();

    printf( "Starting 'setup'\n\n");
    setup();
    printf( "Starting 'loop'\n\n");
    while ( 1 ) loop();

    bcm2835_close();
}

//////////////////
// CLASS SERIAL //
//////////////////

SerialRPi::SerialRPi( bool isconsole)
{
    m_console = isconsole;
    m_enabled = false;
    m_timeout = 1000;
}

uint SerialRPi::available()
{
    uint avail = 0;
    if ( m_console ) {
	struct termios original;
	tcgetattr( STDIN_FILENO, &original);
	struct termios term;
	memcpy( &term, &original, sizeof( term));
	term.c_lflag &= ~ICANON;
	tcsetattr( STDIN_FILENO, TCSANOW, &term);
	ioctl( STDIN_FILENO, FIONREAD, &avail);
	tcsetattr( STDIN_FILENO, TCSANOW, &original);
    }
    else
    if ( ioctl( m_uart, FIONREAD, &avail) < 0)
	return 0;
    return avail;
}

uint SerialRPi::availableForWrite()
{
    return 0;
}

void SerialRPi::begin( const ulong baud, const uint config)
{
    if ( !m_console ) {

	// old style: "/dev/ttyAMA0"
	if ( (m_uart = open( "/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1 )
	    return;

	printf( "UART settings:\n");
	printf( "  Baud rate = %d\n  ", (uint) baud);

	ulong speed;
	switch ( baud ) {
	    case 50:	speed = B50; break;
	    case 75:	speed = B75; break;
	    case 110:	speed = B110; break;
	    case 134:	speed = B134; break;
	    case 150:	speed = B150; break;
	    case 200:	speed = B200; break;
	    case 300:	speed = B300; break;
	    case 600:	speed = B600; break;
	    case 1200:	speed = B1200; break;
	    case 1800:	speed = B1800; break;
	    case 2400:	speed = B2400; break;
	    case 9600:	speed = B9600; break;
	    case 19200:	speed = B19200; break;
	    case 38400:	speed = B38400; break;
	    case 57600:	speed = B57600; break;
	    case 115200:speed = B115200; break;
	    case 230400:speed = B230400; break;
	    default:    speed = baud; // perhaps a none regular baud rate
	}

	uint parity   = config >> 3;		// 0=N, 1=E, 2=O
	uint stopbits = ((config >> 2) & 1);	// 0=1, 1=2
	uint databits = (config & 3) + 5;	// 5 ~ 8
	uint datasize = (config & 3) << 4;	// 5=0, 6=16, 7=32, 8=48

	struct termios options;
	tcgetattr( m_uart, &options);

	cfmakeraw( &options);
	cfsetispeed( &options, speed);
	cfsetospeed( &options, speed);

	options.c_cflag |= (CLOCAL | CREAD);

	if ( parity ) {
	    options.c_cflag |= PARENB;
	    if ( parity == 2 ) {
		options.c_cflag |= PARODD;
		printf( "Parity = Odd (Flags = 0x%02x), ", PARENB | PARODD);
	    }
	    else {
		options.c_cflag &= ~PARODD;
		printf( "Parity = Even (Flags = 0x%02x), ", PARENB);
	    }
	}
	else {
	    options.c_cflag &= ~PARENB;
	    printf( "No parity (Flags = 0x00), ");
	}

	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= stopbits;
	printf( "Stopbits = %d (Flags = 0x%02x), ", stopbits + 1, stopbits);

	options.c_cflag &= ~CSIZE;
	options.c_cflag |= datasize;
	printf( "Databits = %d (Flags = 0x%02x)\n\n", databits, datasize);

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	tcflush( m_uart, TCIFLUSH);
	tcsetattr( m_uart, TCSANOW, &options);

	uint status;
	ioctl( m_uart, TIOCMGET, &status);
	status |= TIOCM_DTR;
	status |= TIOCM_RTS;
	ioctl( m_uart, TIOCMSET, &status);

	delay( 10);
    }
    m_enabled = true;
}

void SerialRPi::end()
{
    m_enabled = false;
    if ( m_console )
	unistd::close( m_uart);
}

bool SerialRPi::find( const String search)
{
    return findUntil( search);
}

bool SerialRPi::findUntil( const String search, const String stop)
{
    String stp= stop;
    String str;
    char chr = 0;
    hirestime ht = currentTime();
    while ( chr != '\n' ) {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return false;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	    str += chr;
	    if ( str.indexOf( search) ) break;
	    if ( stp.length() && (str.indexOf( stp) >= 0) ) break;
	}
    }
    return (chr != '\n');
}

void SerialRPi::flush()
{
    char chr;
    while( available() )
	if ( m_console )
	    chr = getchar();
	else
	    unistd::read( m_uart, &chr, 1);
}

double SerialRPi::parseFloat()
{
    String str;
    char chr = '0';
    hirestime ht = currentTime();
    while ( (chr < '0' || chr > '9') && (chr != '-' && chr != '.') ) {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    while ( (chr >= '0' && chr <= '9') || (chr == '-' || chr == '.') ) {
	str += chr;
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    return str.toFloat();
}

long SerialRPi::parseInt()
{
    String str;
    char chr = '0';
    hirestime ht = currentTime();
    while ( (chr < '0' || chr > '9') && (chr != '-') ) {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    while ( (chr >= '0' && chr <= '9') || (chr == '-') ) {
	str += chr;
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    return str.toInt();
}

int8_t SerialRPi::peek()
{
    if ( !available() )
	return -1;
    char c = 0;
    if ( !m_console ) {
	FILE * f = fdopen( m_uart,"r+");
	c = getc( f);
	ungetc( c, f);
    }
    return c;
}

uint SerialRPi::print( const int i, const NUMSYSTEM type)
{
    return print( String( i, type));
}

uint SerialRPi::print( const double f, const int decimals)
{
    return print( String( f, decimals));
}

uint SerialRPi::print( const String s)
{
    String str = s;
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, str.c_str(), str.length());
}

uint SerialRPi::println( const int i, const NUMSYSTEM type)
{
    return print( String( i, type) + "\r\n");
}

uint SerialRPi::println( const double f, const int decimals)
{
    return print( String( f, decimals) + "\r\n");
}

uint SerialRPi::println( const String s)
{
    return print( String( s) + "\r\n");
}

int8_t SerialRPi::read()
{
    if ( !available() )
	return 0;
    char chr;
    if ( m_console )
	chr = getchar();
    else
	if ( !unistd::read( m_uart, &chr, 1) )
	    return 0;
    return chr;
}

byte SerialRPi::readB( const char stop, byte data[], const byte len, bool dostop)
{
    memset( data, 0, len);
    char chr;
    byte cnt = 0;
    hirestime ht = currentTime();
    do {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    break;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	    if ( dostop && (chr == stop) )
		break;
	    data[cnt] = chr;
	    cnt++;
	}
    } while ( cnt < len );
    return cnt;
}

byte SerialRPi::readBytes( byte data[], const byte len)
{
    return readB( 0, data, len, false);
}

byte SerialRPi::readBytesUntil( const char stop, byte data[], const byte len)
{
    return readB( stop, data, len, true);
}

String SerialRPi::readS( const char stop, bool dostop)
{
    String str;
    char chr;
    hirestime ht = currentTime();
    do {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    break;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	    if ( dostop && (chr == stop) )
		break;
	    str += chr;
	}
    }  while ( true );
    return str;
}

String SerialRPi::readString()
{
    if ( m_console ) {
	char inp[255];
	scanf( "%s", inp);
	return String( inp);
    }
    else
	return readS( 0, false);
}

String SerialRPi::readStringUntil( const char stop)
{
    return readS( stop, true);
}

void SerialRPi::setTimeout( ulong millis)
{
    m_timeout = millis;
}

uint SerialRPi::write( const byte data)
{
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, &data, 1);
}

uint SerialRPi::write( const String str)
{
    String s = str;
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, s.c_str(), s.length());
}

uint SerialRPi::write( const byte buf[], const uint len)
{
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, &buf, len);
}

SerialRPi Serial( true);
SerialRPi Serial1( false);

//////////////////////
// END CLASS SERIAL //
//////////////////////


/*
*  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
*  http://www.libelium.com
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Version 2.4 (For Raspberry Pi 2)
*  Author: Sergio Martinez, Ruben Martin
*/


struct bcm2835_peripheral gpio = {GPIO_BASE2};
struct bcm2835_peripheral bsc_rev1 = {IOBASE + 0X205000};
struct bcm2835_peripheral bsc_rev2 = {IOBASE + 0X804000};
struct bcm2835_peripheral bsc0;
volatile uint32_t *bcm2835_bsc01;

void *spi0 = MAP_FAILED;
static  uint8_t *spi0Mem = NULL;

pthread_t idThread2;
pthread_t idThread3;
pthread_t idThread4;
pthread_t idThread5;
pthread_t idThread6;
pthread_t idThread7;
pthread_t idThread8;
pthread_t idThread9;
pthread_t idThread10;
pthread_t idThread11;
pthread_t idThread12;
pthread_t idThread13;

timeval start_program, end_point;

static int REV = 0;


/*******************************
 *                             *
 * WirePi Class implementation *
 * --------------------------- *
 *******************************/

/******************
 * Public methods *
 ******************/

//Constructor
WirePi::WirePi(){
	REV = getBoardRev();
	if(map_peripheral(&gpio) == -1) {
		printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
	}
	
	memfd = -1;
	i2c_byte_wait_us = 0;
	
	// Open the master /dev/memory device
    if ((memfd = open("/dev/mem", O_RDWR | O_SYNC) ) < 0) 
    {
	fprintf(stderr, "bcm2835_init: Unable to open /dev/mem: %s\n",
		strerror(errno)) ;
	exit(1);
    }
	
	bcm2835_bsc01 = mapmem("bsc1", BLOCK_SIZE, memfd, BCM2835_BSC1_BASE2);
    if (bcm2835_bsc01 == MAP_FAILED) exit(1);
	
    // start timer
    gettimeofday(&start_program, NULL);
    
}

//Initiate the Wire library and join the I2C bus.
void WirePi::begin(){

	volatile uint32_t* paddr = bcm2835_bsc01 + BCM2835_BSC_DIV/4;

    // Set the I2C/BSC1 pins to the Alt 0 function to enable I2C access on them
    ch_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_ALT0); // SDA
    ch_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_ALT0); // SCL

    // Read the clock divider register
    uint16_t cdiv = ch_peri_read(paddr);
    // Calculate time for transmitting one byte
    // 1000000 = micros seconds in a second
    // 9 = Clocks per byte : 8 bits + ACK
    i2c_byte_wait_us = ((float)cdiv / BCM2835_CORE_CLK_HZ) * 1000000 * 9;
}

//Begin a transmission to the I2C slave device with the given address
void WirePi::beginTransmission(unsigned char address){
	// Set I2C Device Address
	volatile uint32_t* paddr = bcm2835_bsc01 + BCM2835_BSC_A/4;
	ch_peri_write(paddr, address);
}

//Writes data to the I2C.
void WirePi::write(char data){
	
	char i2cdata[1];
	i2cdata[0] = data;
	
	write(i2cdata,1);
	
}

//Writes data to the I2C.
uint8_t WirePi::write(const char * buf, uint32_t len){
	
	volatile uint32_t* dlen    = bcm2835_bsc01 + BCM2835_BSC_DLEN/4;
    volatile uint32_t* fifo    = bcm2835_bsc01 + BCM2835_BSC_FIFO/4;
    volatile uint32_t* status  = bcm2835_bsc01 + BCM2835_BSC_S/4;
    volatile uint32_t* control = bcm2835_bsc01 + BCM2835_BSC_C/4;

    uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    ch_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1 , BCM2835_BSC_C_CLEAR_1 );
    // Clear Status
	ch_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
	// Set Data Length
    ch_peri_write_nb(dlen, len);
    // pre populate FIFO with max buffer
    while( remaining && ( i < BCM2835_BSC_FIFO_SIZE ) )
    {
        ch_peri_write_nb(fifo, buf[i]);
        i++;
        remaining--;
    }
    
    // Enable device and start transfer
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST);
    
    // Transfer is over when BCM2835_BSC_S_DONE
    while(!(ch_peri_read_nb(status) & BCM2835_BSC_S_DONE ))
    {
        while ( remaining && (ch_peri_read_nb(status) & BCM2835_BSC_S_TXD ))
    	{
        	// Write to FIFO, no barrier
        	ch_peri_write_nb(fifo, buf[i]);
        	i++;
        	remaining--;
    	}
    }

    // Received a NACK
    if (ch_peri_read(status) & BCM2835_BSC_S_ERR)
    {
		reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

    // Received Clock Stretch Timeout
    else if (ch_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
		reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

    // Not all data is sent
    else if (remaining)
    {
		reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    ch_peri_set_bits(control, BCM2835_BSC_S_DONE , BCM2835_BSC_S_DONE);

    return reason;
}


void WirePi::endTransmission(){
	// Set all the I2C/BSC1 pins back to input
    ch_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_INPT); // SDA
    ch_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_INPT); // SCL
}

//Used by the master to request bytes from a slave device
void WirePi::requestFrom(unsigned char address,int quantity){
	// Set I2C Device Address
	volatile uint32_t* paddr = bcm2835_bsc01 + BCM2835_BSC_A/4;
	ch_peri_write(paddr, address);
	
	i2c_bytes_to_read = quantity;
}

//Reads a byte that was transmitted from a slave device to a master after a call to WirePi::requestFrom()
unsigned char WirePi::read(){
	char buf;
	i2c_bytes_to_read=1;
	read(&buf);
	return (unsigned char)buf;
}

uint8_t WirePi::read(char* buf){
    volatile uint32_t* dlen    = bcm2835_bsc01 + BCM2835_BSC_DLEN/4;
    volatile uint32_t* fifo    = bcm2835_bsc01 + BCM2835_BSC_FIFO/4;
    volatile uint32_t* status  = bcm2835_bsc01 + BCM2835_BSC_S/4;
    volatile uint32_t* control = bcm2835_bsc01 + BCM2835_BSC_C/4;

    uint32_t remaining = i2c_bytes_to_read;
    uint32_t i = 0;
    uint8_t reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    ch_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1 , BCM2835_BSC_C_CLEAR_1 );
    // Clear Status
	ch_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
	// Set Data Length
    ch_peri_write_nb(dlen, i2c_bytes_to_read);
    // Start read
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST | BCM2835_BSC_C_READ);
    
    // wait for transfer to complete
    while (!(ch_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        // we must empty the FIFO as it is populated and not use any delay
        while (ch_peri_read_nb(status) & BCM2835_BSC_S_RXD)
    	{
    		// Read from FIFO, no barrier
    		buf[i] = ch_peri_read_nb(fifo);
        	i++;
        	remaining--;
    	}
    }
    
    // transfer has finished - grab any remaining stuff in FIFO
    while (remaining && (ch_peri_read_nb(status) & BCM2835_BSC_S_RXD))
    {
        // Read from FIFO, no barrier
        buf[i] = ch_peri_read_nb(fifo);
        i++;
        remaining--;
    }
    
    // Received a NACK
    if (ch_peri_read(status) & BCM2835_BSC_S_ERR)
    {
		reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

    // Received Clock Stretch Timeout
    else if (ch_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
		reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

    // Not all data is received
    else if (remaining)
    {
		reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    ch_peri_set_bits(control, BCM2835_BSC_S_DONE , BCM2835_BSC_S_DONE);

    return reason;
}


// Read an number of bytes from I2C sending a repeated start after writing
// the required register. Only works if your device supports this mode
uint8_t WirePi::read_rs(char* regaddr, char* buf, uint32_t len){   
    volatile uint32_t* dlen    = bcm2835_bsc01 + BCM2835_BSC_DLEN/4;
    volatile uint32_t* fifo    = bcm2835_bsc01 + BCM2835_BSC_FIFO/4;
    volatile uint32_t* status  = bcm2835_bsc01 + BCM2835_BSC_S/4;
    volatile uint32_t* control = bcm2835_bsc01 + BCM2835_BSC_C/4;
    
	uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason = BCM2835_I2C_REASON_OK;
    
    // Clear FIFO
    ch_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1 , BCM2835_BSC_C_CLEAR_1 );
    // Clear Status
	ch_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
	// Set Data Length
    ch_peri_write_nb(dlen, 1);
    // Enable device and start transfer
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN);
    ch_peri_write_nb(fifo, regaddr[0]);
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST);
    
    // poll for transfer has started
    while ( !( ch_peri_read_nb(status) & BCM2835_BSC_S_TA ) )
    {
        // Linux may cause us to miss entire transfer stage
        if(ch_peri_read(status) & BCM2835_BSC_S_DONE)
            break;
    }
    
    // Send a repeated start with read bit set in address
    ch_peri_write_nb(dlen, len);
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST  | BCM2835_BSC_C_READ );
    
    // Wait for write to complete and first byte back.	
    delayMicroseconds( (uint64_t) i2c_byte_wait_us * 3);
    
    // wait for transfer to complete
    while (!(ch_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        // we must empty the FIFO as it is populated and not use any delay
        while (remaining && ch_peri_read_nb(status) & BCM2835_BSC_S_RXD)
    	{
    		// Read from FIFO, no barrier
    		buf[i] = ch_peri_read_nb(fifo);
        	i++;
        	remaining--;
    	}
    }
    
    // transfer has finished - grab any remaining stuff in FIFO
    while (remaining && (ch_peri_read_nb(status) & BCM2835_BSC_S_RXD))
    {
        // Read from FIFO, no barrier
        buf[i] = ch_peri_read_nb(fifo);
        i++;
        remaining--;
    }
    
    // Received a NACK
    if (ch_peri_read(status) & BCM2835_BSC_S_ERR)
    {
		reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

    // Received Clock Stretch Timeout
    else if (ch_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
		reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

    // Not all data is sent
    else if (remaining)
    {
		reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    ch_peri_set_bits(control, BCM2835_BSC_S_DONE , BCM2835_BSC_S_DONE);

    return reason;
}


/*******************
 * Private methods *
 *******************/

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int WirePi::map_peripheral(struct bcm2835_peripheral *p)
{
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }

   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,  // File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p      // Address in physical map that we want this memory block to expose
   );

   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }

   p->addr = (volatile unsigned int *)p->map;

   return 0;
}

void WirePi::unmap_peripheral(struct bcm2835_peripheral *p) {

    munmap(p->map, BLOCK_SIZE);
    unistd::close(p->mem_fd);
}

void WirePi::wait_i2c_done() {
        //Wait till done, let's use a timeout just in case
        int timeout = 50;
        while((!((BSC0_S) & BSC_S_DONE)) && --timeout) {
            unistd::usleep(1000);
        }
        if(timeout == 0)
            printf("wait_i2c_done() timeout. Something went wrong.\n");
}





/*******************************
 *                             *
 * SPIPi Class implementation *
 * --------------------------- *
 *******************************/

/******************
 * Public methods *
 ******************/

 SPIPi::SPIPi(){
	 
	REV = getBoardRev();

    uint8_t *mapaddr;

    if ((spi0Mem = (uint8_t*)malloc(BLOCK_SIZE + (PAGESIZE - 1))) == NULL){
        fprintf(stderr, "bcm2835_init: spi0Mem malloc failed: %s\n", strerror(errno));
        exit(1);
    }
    
    mapaddr = spi0Mem;
    if (((uint32_t)mapaddr % PAGESIZE) != 0)
        mapaddr += PAGESIZE - ((uint32_t)mapaddr % PAGESIZE) ;
    
    spi0 = (uint32_t *)mmap(mapaddr, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, gpio.mem_fd, BCM2835_SPI0_BASE2) ;
    
    if ((int32_t)spi0 < 0){
        fprintf(stderr, "bcm2835_init: mmap failed (spi0): %s\n", strerror(errno)) ;
        exit(1);
    }
 }

void SPIPi::begin(){
    // Set the SPI0 pins to the Alt 0 function to enable SPI0 access on them
    ch_gpio_fsel(7, BCM2835_GPIO_FSEL_ALT0); // CE1
    ch_gpio_fsel(8, BCM2835_GPIO_FSEL_ALT0); // CE0
    ch_gpio_fsel(9, BCM2835_GPIO_FSEL_ALT0); // MISO
    ch_gpio_fsel(10, BCM2835_GPIO_FSEL_ALT0); // MOSI
    ch_gpio_fsel(11, BCM2835_GPIO_FSEL_ALT0); // CLK
    
    // Set the SPI CS register to the some sensible defaults
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    ch_peri_write(paddr, 0); // All 0s
    
    // Clear TX and RX fifos
    ch_peri_write_nb(paddr, BCM2835_SPI0_CS_CLEAR);
}

void SPIPi::end(){  
    // Set all the SPI0 pins back to input
    ch_gpio_fsel(7, BCM2835_GPIO_FSEL_INPT); // CE1
    ch_gpio_fsel(8, BCM2835_GPIO_FSEL_INPT); // CE0
    ch_gpio_fsel(9, BCM2835_GPIO_FSEL_INPT); // MISO
    ch_gpio_fsel(10, BCM2835_GPIO_FSEL_INPT); // MOSI
    ch_gpio_fsel(11, BCM2835_GPIO_FSEL_INPT); // CLK
}

void SPIPi::setBitOrder(uint8_t order){
    // BCM2835_SPI_BIT_ORDER_MSBFIRST is the only one suported by SPI0
}

// defaults to 0, which means a divider of 65536.
// The divisor must be a power of 2. Odd numbers
// rounded down. The maximum SPI clock rate is
// of the APB clock
void SPIPi::setClockDivider(uint16_t divider){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CLK/4;
    ch_peri_write(paddr, divider);
}

void SPIPi::setDataMode(uint8_t mode){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    // Mask in the CPO and CPHA bits of CS
    ch_peri_set_bits(paddr, mode << 2, BCM2835_SPI0_CS_CPOL | BCM2835_SPI0_CS_CPHA);
}

// Writes (and reads) a single byte to SPI
uint8_t SPIPi::transfer(uint8_t value){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    volatile uint32_t* fifo = (volatile uint32_t*)spi0 + BCM2835_SPI0_FIFO/4;

    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

    while (!(ch_peri_read(paddr) & BCM2835_SPI0_CS_TXD))
    delayMicroseconds( (uint64_t) 10);

    ch_peri_write_nb(fifo, value);

    while (!(ch_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE))
    delayMicroseconds( (uint64_t) 10);

    uint32_t ret = ch_peri_read_nb(fifo);

    ch_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);

    return ret;
}

// Writes (and reads) a number of bytes to SPI
void SPIPi::transfernb(char* tbuf, char* rbuf, uint32_t len){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    volatile uint32_t* fifo = (volatile uint32_t*)spi0 + BCM2835_SPI0_FIFO/4;

    // This is Polled transfer as per section 10.6.1
    // BUG ALERT: what happens if we get interupted in this section, and someone else
    // accesses a different peripheral? 

    // Clear TX and RX fifos
    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

    // Set TA = 1
    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

    uint32_t i;
    for (i = 0; i < len; i++)
    {
    // Maybe wait for TXD
    while (!(ch_peri_read(paddr) & BCM2835_SPI0_CS_TXD))
        delayMicroseconds( (uint64_t) 10);

    // Write to FIFO, no barrier
    ch_peri_write_nb(fifo, tbuf[i]);

    // Wait for RXD
    while (!(ch_peri_read(paddr) & BCM2835_SPI0_CS_RXD))
        delayMicroseconds( (uint64_t) 10);

    // then read the data byte
    rbuf[i] = ch_peri_read_nb(fifo);
    }
    // Wait for DONE to be set
    while (!(ch_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE))
    delayMicroseconds( (uint64_t) 10);

    // Set TA = 0, and also set the barrier
    ch_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);
}

void SPIPi::chipSelect(uint8_t cs){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    // Mask in the CS bits of CS
    ch_peri_set_bits(paddr, cs, BCM2835_SPI0_CS_CS);
}

void SPIPi::setChipSelectPolarity(uint8_t cs, uint8_t active){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    uint8_t shift = 21 + cs;
    // Mask in the appropriate CSPOLn bit
    ch_peri_set_bits(paddr, active << shift, 1 << shift);
}


// safe read from peripheral
uint32_t ch_peri_read(volatile uint32_t* paddr){
    uint32_t ret = *paddr;
    ret = *paddr;
    return ret;
    
}

// read from peripheral without the read barrier
uint32_t ch_peri_read_nb(volatile uint32_t* paddr){
    return *paddr;
}

// safe write to peripheral
void ch_peri_write(volatile uint32_t* paddr, uint32_t value){
    *paddr = value;
    *paddr = value;
}

// write to peripheral without the write barrier
void ch_peri_write_nb(volatile uint32_t* paddr, uint32_t value){
    *paddr = value;
}

// Set/clear only the bits in value covered by the mask
void ch_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask){
    uint32_t v = ch_peri_read(paddr);
    v = (v & ~mask) | (value & mask);
    ch_peri_write(paddr, v);
}


int analogRead (int pin){

	int value;
	char selected_channel[1];
	char read_values[2];

	if (pin == 0) {
		selected_channel[0] = 0xDC;
	} else if (pin == 1){
		selected_channel[0] = 0x9C;
	} else if (pin == 2){ 
		selected_channel[0] = 0xCC ;
	} else if (pin == 3){ 
		selected_channel[0] = 0x8C;
	} else if (pin == 4){ 
		selected_channel[0] = 0xAC;
	} else if (pin == 5){ 
		selected_channel[0] = 0xEC;
	} else if (pin == 6){ 
		selected_channel[0] = 0xBC;
	} else if (pin == 7){ 
		selected_channel[0] = 0xFC;
	}
	
	Wire.begin();
	Wire.beginTransmission(8); 
	Wire.read_rs(selected_channel, read_values, 2);
	Wire.read_rs(selected_channel, read_values, 2);

	value = int(read_values[0])*16 + int(read_values[1]>>4);
	value = value * 1023 / 4095;  //mapping the value between 0 and 1023
	return value;
}

void attachInterrupt(int p,void (*f)(), uint m){
	int GPIOPin = raspberryPinNumber(p);
	pthread_t *threadId = getThreadIdFromPin(p);
	struct ThreadArg *threadArgs = (ThreadArg *)malloc(sizeof(ThreadArg));
	threadArgs->func = f;
	threadArgs->pin = GPIOPin;
	
	//Export pin for interrupt
	FILE *fp = fopen("/sys/class/gpio/export","w");
	if (fp == NULL){
		fprintf(stderr,"Unable to export pin %d for interrupt\n",p);
		exit(1);
	}else{
		fprintf(fp,"%d",GPIOPin); 
	}
	fclose(fp);
	
	//The system to create the file /sys/class/gpio/gpio<GPIO number>
	//So we wait a bit
	delay(1L);
	
	char * interruptFile = NULL;
	asprintf(&interruptFile, "/sys/class/gpio/gpio%d/edge",GPIOPin);
	
	//Set detection condition
	fp = fopen(interruptFile,"w");
	if (fp == NULL){
		fprintf(stderr,"Unable to set detection type on pin %d\n",p);
		exit(1);
	}else{
		switch(m){
			case RISING: fprintf(fp,"rising");break;
			case FALLING: fprintf(fp,"falling");break;
			default: fprintf(fp,"both");break;
		}
		
	}
	fclose(fp);
	
	if(*threadId == 0){
		//Create a thread passing the pin and function
		pthread_create (threadId, NULL, threadFunction, (void *)threadArgs);
	}else{
		//First cancel the existing thread for that pin
		pthread_cancel(*threadId);
		//Create a thread passing the pin, function and mode
		pthread_create (threadId, NULL, threadFunction, (void *)threadArgs);
	}
	
}

void detachInterrupt(int p){
	int GPIOPin = raspberryPinNumber(p);
	
	FILE *fp = fopen("/sys/class/gpio/unexport","w");
	if (fp == NULL){
		fprintf(stderr,"Unable to unexport pin %d for interrupt\n",p);
		exit(1);
	}else{
		fprintf(fp,"%d",GPIOPin); 
	}
	fclose(fp);
	
	pthread_t *threadId = getThreadIdFromPin(p);
	pthread_cancel(*threadId);
}

/* Some helper functions */

int getBoardRev(){
	
	FILE *cpu_info;
	char line [120];
	char *c,finalChar;
	
	if (REV != 0) return REV;
	
	if ((cpu_info = fopen("/proc/cpuinfo","r"))==NULL){
		fprintf(stderr,"Unable to open /proc/cpuinfo. Cannot determine board reivision.\n");
		exit(1);
	}
	
	while (fgets (line,120,cpu_info) != NULL){
		if(strncmp(line,"Revision",8) == 0) break;
	}
	
	fclose(cpu_info);
	
	if (line == NULL){
		fprintf (stderr, "Unable to determine board revision from /proc/cpuinfo.\n") ;
		exit(1);
	}
	
	for (c = line ; *c ; ++c)
    if (isdigit (*c))
      break ;

	if (!isdigit (*c)){
		fprintf (stderr, "Unable to determine board revision from /proc/cpuinfo\n") ;
		fprintf (stderr, "  (Info not found in: %s\n", line) ;
		exit(1);
	}
	
	finalChar = c [strlen (c) - 2] ;
	
	if ((finalChar == '2') || (finalChar == '3')){
		bsc0 = bsc_rev1;
		return 1;
	}else{
		bsc0 = bsc_rev2;
		return 2;
	}
}

uint32_t* mapmem(const char *msg, size_t size, int fd, off_t off)
{
    uint32_t *map = (uint32_t *)mmap(NULL, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, off);
    if (MAP_FAILED == map)
	fprintf(stderr, "bcm2835_init: %s mmap failed: %s\n", msg, strerror(errno));
    return map;
}

int raspberryPinNumber(int bcmPin /*int arduinoPin*/){
	/*
	switch(arduinoPin){
		case 1: return 14; break;
		case 2: return 18; break;
		case 3: return 23; break;
		case 4: return 24; break;
		case 5: return 25; break;
		case 6: return  4; break;
		case 7: return 17; break;
		case 8: if(REV == 1){return 21;}else{return 27;} break;
		case 9: return 22; break;
		case 10: return 8; break;
		case 11: return 10; break;
		case 12: return 9; break;
		case 13: return 11; break;
	}*/
	return bcmPin;
}

void ch_gpio_fsel(uint8_t pin, uint8_t mode){
    // Function selects are 10 pins per 32 bit word, 3 bits per pin
    volatile uint32_t* paddr = (volatile uint32_t*)gpio.map + BCM2835_GPFSEL0/4 + (pin/10);
    uint8_t   shift = (pin % 10) * 3;
    uint32_t  mask = BCM2835_GPIO_FSEL_MASK << shift;
    uint32_t  value = mode << shift;
    ch_peri_set_bits(paddr, value, mask);
}

pthread_t *getThreadIdFromPin(int pin){
	switch(pin){
		case 2: return &idThread2; break;
		case 3: return &idThread3; break;
		case 4: return &idThread4; break;
		case 5: return &idThread5; break;
		case 6: return &idThread6; break;
		case 7: return &idThread7; break;
		case 8: return &idThread8; break;
		case 9: return &idThread9; break;
		case 10: return &idThread10; break;
		case 11: return &idThread11; break;
		case 12: return &idThread12; break;
		case 13: return &idThread13; break;
	}
    return 0; // should never come here
}

/* This is the function that will be running in a thread if
 * attachInterrupt() is called */
void * threadFunction(void *args){
	ThreadArg *arguments = (ThreadArg *)args;
	int pin = arguments->pin;
	
	int GPIO_FN_MAXLEN = 32;
	int RDBUF_LEN = 5;
	
	char fn[GPIO_FN_MAXLEN];
	int fd,ret;
	struct pollfd pfd;
	char rdbuf [RDBUF_LEN];
	
	memset(rdbuf, 0x00, RDBUF_LEN);
	memset(fn,0x00,GPIO_FN_MAXLEN);
	
	snprintf(fn, GPIO_FN_MAXLEN-1, "/sys/class/gpio/gpio%d/value",pin);
	fd=open(fn, O_RDONLY);
	if(fd<0){
		perror(fn);
		exit(1);
	}
	pfd.fd=fd;
	pfd.events=POLLPRI;
	
	ret=unistd::read(fd,rdbuf,RDBUF_LEN-1);
	if(ret<0){
		perror("Error reading interrupt file\n");
		exit(1);
	}
	
	while(1){
		memset(rdbuf, 0x00, RDBUF_LEN);
		unistd::lseek(fd, 0, SEEK_SET);
		ret=poll(&pfd, 1, -1);
		if(ret<0){
			perror("Error waiting for interrupt\n");
			unistd::close(fd);
			exit(1);
		}
		if(ret==0){
			printf("Timeout\n");
			continue;
		}
		ret=unistd::read(fd,rdbuf,RDBUF_LEN-1);
		if(ret<0){
			perror("Error reading interrupt file\n");
			exit(1);
		}
		//Interrupt. We call user function.
		arguments->func();
	}
}

//SerialPi Serial1 = SerialPi();
WirePi Wire = WirePi();
SPIPi SPI = SPIPi();
