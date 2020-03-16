// Arduino.h

#ifndef ARDUINO_H
#define ARDUINO_H

#include <thread>
#include <cstring>
#include <string>
#include <chrono>
#include "bcm2835.h"

using namespace std;

typedef chrono::time_point<chrono::high_resolution_clock>	hirestime;
extern hirestime currentTime();
extern ulong diffMillis( hirestime stop, hirestime start);
extern ulong diffMicros( hirestime stop, hirestime start);


#define OUTPUT			0
#define INPUT			1
#define INPUT_PULLUP	2
#define INPUT_PULLDOWN	3

//#define LOW			0
//#define HIGH			1
#define RISING			2
#define FALLING			3
#define CHANGE			4

#define sprint			Serial.print
#define sprintln		Serial.println
#define sflush			Serial.flush

typedef uint8_t byte;

enum NUMSYSTEM { DEC, HEX, OCT, BIN, BYTE };

class String
{
public:

	String();
	String( const String &str);
	String( const char chr);
	String( const char* str);
	String( const string &str);
	String( const int val, const NUMSYSTEM type = DEC);
	String( const double f, const int decimals = 2);
	~String();

	char operator[] ( const uint index);
	String operator= ( const String str);
	String operator+ ( const String str);
	String operator+= ( const String str);
	bool operator== ( const String str);
	bool operator> ( const String str);
	bool operator>= ( const String str);
	bool operator< ( const String str);
	bool operator<= ( const String str);
	bool operator!= ( const String str);

	char charAt( const uint index);
	int compareTo( const String str);
	String concat( const String str1, const String str2);
	const char* c_str();
	bool endsWith( const String str);
	bool equals( const String str);
	bool equalsIgnoreCase( const String str);
	void getBytes( byte buf[], const uint len);
	int indexOf(  const String str);
	int indexOf( const char* str);
	int lastIndexOf( const String str, const uint from = 0);
	int lastIndexOf( const char* str, const uint from = 0);
	int length();
	void remove( const uint index, const uint count = 1);
	String replace(  const String str1, const String str2);
	void reserve( const int size);
	void setCharAt( const uint index, const char chr);
	bool startsWith( const String str);
	String substring( const uint from, const uint to = -1);
	void toCharArray( char buf[], const uint len);
	int toInt();
	double toFloat();
	void toLowerCase();
	void toUpperCase();
	void trim();

	static String toHex( const uint8_t *data, const uint size);
	static String toHexChar( const uint8_t *data, const uint size);
	static String fillout( const String str, const char c, const uint size);

protected:

	string S;
};

// GENERAL ARDUINO FUNCTIONS

void pinMode( int pin, int mode);
#define digitalWrite( pin, state)	bcm2835_gpio_write( pin, state)
#define digitalRead( pin)			bcm2835_gpio_lev( pin)
void analogWrite( int pin, int level);

long millis();
long micros();

// ARDUINO SERIAL INTERFACE

#define SERIAL_5N1	0
#define SERIAL_6N1	1
#define SERIAL_7N1	2
#define SERIAL_8N1	3
#define SERIAL_5N2	4
#define SERIAL_6N2	5
#define SERIAL_7N2	6
#define SERIAL_8N2	7
#define SERIAL_5E1	8
#define SERIAL_6E1	9
#define SERIAL_7E1	10
#define SERIAL_8E1	11
#define SERIAL_5E2	12
#define SERIAL_6E2	13
#define SERIAL_7E2	14
#define SERIAL_8E2	15
#define SERIAL_5O1	16
#define SERIAL_6O1	17
#define SERIAL_7O1	18
#define SERIAL_8O1	19
#define SERIAL_5O2	20
#define SERIAL_6O2	21
#define SERIAL_7O2	22
#define SERIAL_8O2	23

// ARDUINO SERIAL MONITOR

class SerialRPi
{
public:

	SerialRPi( bool isconsole);

	uint available();
	uint availableForWrite();

	void begin( const ulong baud, const uint config = SERIAL_8N1);
	void end(); 

	bool find( const String search);
	bool findUntil( const String search, const String stop = "");

	void flush();

	double parseFloat();
	long   parseInt();

	int8_t peek();

	uint print( const int i, const NUMSYSTEM type = DEC);
	uint print( const double f, const int decimals = 2);
	uint print( const String s = "");
	uint println( const int i, const NUMSYSTEM type = DEC);
	uint println( const double f, const int decimals = 2);
	uint println( const String s = "");

	int8_t read();
	byte   readBytes( byte data[], const byte len);
	byte   readBytesUntil( const char stop, byte data[], const byte len);
	String readString();
	String readStringUntil( const char stop);

	void setTimeout( ulong millis);

	uint write( const byte data);
	uint write( const String str);
	uint write( const byte buf[], const uint len);

private:

	byte readB( const char stop, byte data[], const byte len, bool dostop);
	String readS( const char stop, bool dostop);

	bool   m_console;
	bool   m_enabled;
	ulong  m_timeout;
	int    m_uart;
};

extern SerialRPi Serial;
extern SerialRPi Serial1;


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

#include <sys/time.h>
#include <sys/mman.h>
#include <poll.h>
#include <bcm2835.h>
#include <stdarg.h> //Include forva_start, va_arg and va_end strings functions

#define IOBASE   0x3f000000


#define GPIO_BASE2 (IOBASE + 0x200000)
#define BCM2835_SPI0_BASE2 (IOBASE + 0x204000)

#define BCM2835_BSC1_BASE2		(IOBASE + 0x804000)

// Defines for I2C
// GPIO register offsets from BCM2835_BSC*_BASE.
// Offsets into the BSC Peripheral block in bytes per 3.1 BSC Register Map
#define BCM2835_BSC_C 							0x0000 ///< BSC Master Control
#define BCM2835_BSC_S 							0x0004 ///< BSC Master Status
#define BCM2835_BSC_DLEN						0x0008 ///< BSC Master Data Length
#define BCM2835_BSC_A 							0x000c ///< BSC Master Slave Address
#define BCM2835_BSC_FIFO						0x0010 ///< BSC Master Data FIFO
#define BCM2835_BSC_DIV							0x0014 ///< BSC Master Clock Divider
#define BCM2835_BSC_DEL							0x0018 ///< BSC Master Data Delay
#define BCM2835_BSC_CLKT						0x001c ///< BSC Master Clock Stretch Timeout

// Register masks for BSC_C
#define BCM2835_BSC_C_I2CEN 					0x00008000 ///< I2C Enable, 0 = disabled, 1 = enabled
#define BCM2835_BSC_C_INTR 						0x00000400 ///< Interrupt on RX
#define BCM2835_BSC_C_INTT 						0x00000200 ///< Interrupt on TX
#define BCM2835_BSC_C_INTD 						0x00000100 ///< Interrupt on DONE
#define BCM2835_BSC_C_ST 						0x00000080 ///< Start transfer, 1 = Start a new transfer
#define BCM2835_BSC_C_CLEAR_1 					0x00000020 ///< Clear FIFO Clear
#define BCM2835_BSC_C_CLEAR_2 					0x00000010 ///< Clear FIFO Clear
#define BCM2835_BSC_C_READ 						0x00000001 ///<	Read transfer

// Register masks for BSC_S
#define BCM2835_BSC_S_CLKT 						0x00000200 ///< Clock stretch timeout
#define BCM2835_BSC_S_ERR 						0x00000100 ///< ACK error
#define BCM2835_BSC_S_RXF 						0x00000080 ///< RXF FIFO full, 0 = FIFO is not full, 1 = FIFO is full
#define BCM2835_BSC_S_TXE 						0x00000040 ///< TXE FIFO full, 0 = FIFO is not full, 1 = FIFO is full
#define BCM2835_BSC_S_RXD 						0x00000020 ///< RXD FIFO contains data
#define BCM2835_BSC_S_TXD 						0x00000010 ///< TXD FIFO can accept data
#define BCM2835_BSC_S_RXR 						0x00000008 ///< RXR FIFO needs reading (full)
#define BCM2835_BSC_S_TXW 						0x00000004 ///< TXW FIFO needs writing (full)
#define BCM2835_BSC_S_DONE 						0x00000002 ///< Transfer DONE
#define BCM2835_BSC_S_TA 						0x00000001 ///< Transfer Active

#define BCM2835_BSC_FIFO_SIZE   				16 ///< BSC FIFO size
#define BCM2835_CORE_CLK_HZ				250000000	///< 250 MHz

#define BSC0_C        *(bsc0.addr + 0x00)
#define BSC0_S        *(bsc0.addr + 0x01)
#define BSC0_DLEN    *(bsc0.addr + 0x02)
#define BSC0_A        *(bsc0.addr + 0x03)
#define BSC0_FIFO    *(bsc0.addr + 0x04)

#define BSC_C_I2CEN    (1 << 15)
#define BSC_C_INTR    (1 << 10)
#define BSC_C_INTT    (1 << 9)
#define BSC_C_INTD    (1 << 8)
#define BSC_C_ST    (1 << 7)
#define BSC_C_CLEAR    (1 << 4)
#define BSC_C_READ    1

#define START_READ    BSC_C_I2CEN|BSC_C_ST|BSC_C_CLEAR|BSC_C_READ
#define START_WRITE    BSC_C_I2CEN|BSC_C_ST

#define BSC_S_CLKT    (1 << 9)
#define BSC_S_ERR    (1 << 8)
#define BSC_S_RXF    (1 << 7)
#define BSC_S_TXE    (1 << 6)
#define BSC_S_RXD    (1 << 5)
#define BSC_S_TXD    (1 << 4)
#define BSC_S_RXR    (1 << 3)
#define BSC_S_TXW    (1 << 2)
#define BSC_S_DONE    (1 << 1)
#define BSC_S_TA    1

#define CLEAR_STATUS    BSC_S_CLKT|BSC_S_ERR|BSC_S_DONE

#define GPFSEL0    *(gpio.addr + 0)
#define GPFSEL1    *(gpio.addr + 1)
#define GPFSEL2    *(gpio.addr + 2)
#define GPFSEL3    *(gpio.addr + 3)
#define GPFSEL4    *(gpio.addr + 4)
#define GPFSEL5    *(gpio.addr + 5)
// Reserved @ word offset 6
#define GPSET0    *(gpio.addr + 7)
#define GPSET1    *(gpio.addr + 8)
// Reserved @ word offset 9
#define GPCLR0    *(gpio.addr + 10)
#define GPCLR1    *(gpio.addr + 11)
// Reserved @ word offset 12
#define GPLEV0    *(gpio.addr + 13)
#define GPLEV1    *(gpio.addr + 14)

#define BIT_4 (1 << 4)
#define BIT_6 (1 << 6)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_14 (1 << 14)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_21 (1 << 21)
#define BIT_27 (1 << 27)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)




#define PAGESIZE 4096
#define BLOCK_SIZE 4096


/// Defines for SPI
/// GPIO register offsets from BCM2835_SPI0_BASE. 
/// Offsets into the SPI Peripheral block in bytes per 10.5 SPI Register Map
#define BCM2835_SPI0_CS                      0x0000 ///< SPI Master Control and Status
#define BCM2835_SPI0_FIFO                    0x0004 ///< SPI Master TX and RX FIFOs
#define BCM2835_SPI0_CLK                     0x0008 ///< SPI Master Clock Divider
#define BCM2835_SPI0_DLEN                    0x000c ///< SPI Master Data Length
#define BCM2835_SPI0_LTOH                    0x0010 ///< SPI LOSSI mode TOH
#define BCM2835_SPI0_DC                      0x0014 ///< SPI DMA DREQ Controls

// Register masks for SPI0_CS
#define BCM2835_SPI0_CS_LEN_LONG             0x02000000 ///< Enable Long data word in Lossi mode if DMA_LEN is set
#define BCM2835_SPI0_CS_DMA_LEN              0x01000000 ///< Enable DMA mode in Lossi mode
#define BCM2835_SPI0_CS_CSPOL2               0x00800000 ///< Chip Select 2 Polarity
#define BCM2835_SPI0_CS_CSPOL1               0x00400000 ///< Chip Select 1 Polarity
#define BCM2835_SPI0_CS_CSPOL0               0x00200000 ///< Chip Select 0 Polarity
#define BCM2835_SPI0_CS_RXF                  0x00100000 ///< RXF - RX FIFO Full
#define BCM2835_SPI0_CS_RXR                  0x00080000 ///< RXR RX FIFO needs Reading ( full)
#define BCM2835_SPI0_CS_TXD                  0x00040000 ///< TXD TX FIFO can accept Data
#define BCM2835_SPI0_CS_RXD                  0x00020000 ///< RXD RX FIFO contains Data
#define BCM2835_SPI0_CS_DONE                 0x00010000 ///< Done transfer Done
#define BCM2835_SPI0_CS_TE_EN                0x00008000 ///< Unused
#define BCM2835_SPI0_CS_LMONO                0x00004000 ///< Unused
#define BCM2835_SPI0_CS_LEN                  0x00002000 ///< LEN LoSSI enable
#define BCM2835_SPI0_CS_REN                  0x00001000 ///< REN Read Enable
#define BCM2835_SPI0_CS_ADCS                 0x00000800 ///< ADCS Automatically Deassert Chip Select
#define BCM2835_SPI0_CS_INTR                 0x00000400 ///< INTR Interrupt on RXR
#define BCM2835_SPI0_CS_INTD                 0x00000200 ///< INTD Interrupt on Done
#define BCM2835_SPI0_CS_DMAEN                0x00000100 ///< DMAEN DMA Enable
#define BCM2835_SPI0_CS_TA                   0x00000080 ///< Transfer Active
#define BCM2835_SPI0_CS_CSPOL                0x00000040 ///< Chip Select Polarity
#define BCM2835_SPI0_CS_CLEAR                0x00000030 ///< Clear FIFO Clear RX and TX
#define BCM2835_SPI0_CS_CLEAR_RX             0x00000020 ///< Clear FIFO Clear RX 
#define BCM2835_SPI0_CS_CLEAR_TX             0x00000010 ///< Clear FIFO Clear TX 
#define BCM2835_SPI0_CS_CPOL                 0x00000008 ///< Clock Polarity
#define BCM2835_SPI0_CS_CPHA                 0x00000004 ///< Clock Phase
#define BCM2835_SPI0_CS_CS                   0x00000003 ///< Chip Select

#define BCM2835_GPFSEL0                      0x0000 ///< GPIO Function Select 0

#define BCM2835_GPEDS0                       0x0040 ///< GPIO Pin Event Detect Status 0
#define BCM2835_GPREN0                       0x004c ///< GPIO Pin Rising Edge Detect Enable 0

#define BCM2835_GPHEN0                       0x0064 ///< GPIO Pin High Detect Enable 0
#define BCM2835_GPLEN0                       0x0070 ///< GPIO Pin Low Detect Enable 0

#define LSBFIRST  0  ///< LSB First
#define MSBFIRST  1   ///< MSB First

#define SPI_MODE0 0  ///< CPOL = 0, CPHA = 0
#define SPI_MODE1 1  ///< CPOL = 0, CPHA = 1
#define SPI_MODE2 2  ///< CPOL = 1, CPHA = 0
#define SPI_MODE3 3

#define SPI_CLOCK_DIV65536 0       ///< 65536 = 256us = 4kHz
#define SPI_CLOCK_DIV32768 32768   ///< 32768 = 126us = 8kHz
#define SPI_CLOCK_DIV16384 16384   ///< 16384 = 64us = 15.625kHz
#define SPI_CLOCK_DIV8192 8192    ///< 8192 = 32us = 31.25kHz
#define SPI_CLOCK_DIV4096 4096    ///< 4096 = 16us = 62.5kHz
#define SPI_CLOCK_DIV2048 2048    ///< 2048 = 8us = 125kHz
#define SPI_CLOCK_DIV1024 1024    ///< 1024 = 4us = 250kHz
#define SPI_CLOCK_DIV512 512     ///< 512 = 2us = 500kHz
#define SPI_CLOCK_DIV256 256     ///< 256 = 1us = 1MHz
#define SPI_CLOCK_DIV128 128     ///< 128 = 500ns = = 2MHz
#define SPI_CLOCK_DIV64 64      ///< 64 = 250ns = 4MHz
#define SPI_CLOCK_DIV32 32      ///< 32 = 125ns = 8MHz
#define SPI_CLOCK_DIV16 16      ///< 16 = 50ns = 20MHz
#define SPI_CLOCK_DIV8 8       ///< 8 = 25ns = 40MHz
#define SPI_CLOCK_DIV4 4       ///< 4 = 12.5ns 80MHz
#define SPI_CLOCK_DIV2 2       ///< 2 = 6.25ns = 160MHz
#define SPI_CLOCK_DIV1 1       ///< 0 = 256us = 4kHz

namespace unistd {
	//All functions of unistd.h must be called like this: unistd::the_function()
    #include <unistd.h>
};


typedef bool boolean;
typedef unsigned char byte;

struct bcm2835_peripheral{
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

struct ThreadArg{
    void (*func)();
    int pin;
};


// WirePi Class

class WirePi{
	private:
		int memfd;
		int i2c_byte_wait_us;
		int i2c_bytes_to_read;
		void dump_bsc_status();
		int map_peripheral(struct bcm2835_peripheral *p);
		void unmap_peripheral(struct bcm2835_peripheral *p);
		void wait_i2c_done();
	public:
		WirePi();
		void begin();
		void beginTransmission(unsigned char address);
		void write(char data);
		uint8_t write(const char * buf, uint32_t len);
		void endTransmission();
		void requestFrom(unsigned char address,int quantity);
		unsigned char read();
		uint8_t read(char* buf);
		uint8_t read_rs(char* regaddr, char* buf, uint32_t len);
};

class SPIPi{
	public:
		SPIPi();
  		void begin();
    	void end();
    	void setBitOrder(uint8_t order);
 		void setClockDivider(uint16_t divider);
		void setDataMode(uint8_t mode);
 		void chipSelect(uint8_t cs);
		void setChipSelectPolarity(uint8_t cs, uint8_t active);
 		uint8_t transfer(uint8_t value);
 		void transfernb(char* tbuf, char* rbuf, uint32_t len);
};

// Some useful arduino functions
int analogRead (int pin);
void attachInterrupt(int p,void (*f)(), uint m);
void detachInterrupt(int p);

// Helper functions
int getBoardRev();
uint32_t *mapmem(const char *msg, size_t size, int fd, off_t off);
void setBoardRev(int rev);
int raspberryPinNumber(int arduinoPin);
pthread_t *getThreadIdFromPin(int pin);
uint32_t ch_peri_read(volatile uint32_t* paddr);
uint32_t ch_peri_read_nb(volatile uint32_t* paddr);
void ch_peri_write(volatile uint32_t* paddr, uint32_t value);
void ch_peri_write_nb(volatile uint32_t* paddr, uint32_t value);
void ch_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask);
void ch_gpio_fsel(uint8_t pin, uint8_t mode);
void * threadFunction(void *args);

extern WirePi Wire;
extern SPIPi SPI;


#endif // ARDUINO_H