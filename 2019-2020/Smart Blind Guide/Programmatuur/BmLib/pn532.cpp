// Based on Adafruit-PN532 library for their product
// https://www.adafruit.com/products/364//
// Copyright (c) 2012, Adafruit Industries. All rights reserved.
// Adafruit invests time and resources providing this open source code,
// please support Adafruit and open-source hardware by purchasing
// products from Adafruit!
//
// Adjusted to fit the architecture of the RPi-Cpp repository.
// Although the Adafruit PN532 class supports both I2C and SPI,
// here the class supports I2C only.

#include "pn532.h"
#include <cstring>

namespace PN532 {

//#define PN532_DEBUG

const String NOBUTTON = "No button";
const String EXPIRED = "Validation expired";

const RFIDKEY DEFAULT_KEY = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


////////////////////////////////////////////////////////////////////////


Rfid::Rfid()
{
  _irq = 0;
  _reset = 0;
}

bool Rfid::init( uint pinRst, uint pinIrq)
{
    begin( pinIrq, pinRst);
    return SAMConfig();
}

void Rfid::setTimeout( int timeout)
{
    m_timeout = timeout;
}

void Rfid::setAuthentication( const RFIDKEY key)
{
    for ( uint i = 0; i < sizeof( RFIDKEY); i++ )
        m_key[i] = key[i];
}

void Rfid::reset()
{
    if ( _reset < 0 ) return;
    digitalWrite( _reset, HIGH);
    delay( 50);
    digitalWrite( _reset, LOW);
}

void Rfid::read()
{
    m_readok = false;
    m_tag = NOBUTTON;
    uint8_t len = sizeof( m_uid) - 1;
    if ( readPassiveTargetID( PN532_MIFARE_ISO14443A, m_uid, &len, m_timeout) ) {
        m_tag = String::toHex( m_uid, sizeof( m_uid));
        goto card_read_ok;
    }
    return;
card_read_ok:
    m_stamp = millis();
    m_readok = true;
    Sensor::read();
}

void Rfid::done() // dummy to keep in line with MFRC522 lib
{
}

String Rfid::tag( long expireAfter)
{
    return m_tag;
}

bool Rfid::readValue( int index, String& value)
{
   if ( index < 0 || index > 63 ) return false;
    if ( !mifareclassic_AuthenticateBlock( m_uid, sizeof( m_uid), index, 1, m_key) )
        return false;
    byte buffer[18];
    int i;
    if ( !mifareclassic_ReadDataBlock( index, buffer) )
        return false;
    for ( i = 15; i >= 0 ; i-- )
        if ( buffer[i] != ' ' ) break;
    buffer[i+1] = 0;
    value = (char*) buffer;
    return true;
}

bool Rfid::writeValue( int index, String value)
{
    if ( index < 0 || index > 63 ) return false;
    if ( !mifareclassic_AuthenticateBlock( m_uid, sizeof( m_uid), index, 1, m_key) )
        return false;
    byte buffer[16];
    int i;
    for ( i = 0; i < value.length() && i < 16; i++ )
        buffer[i] = value[i];
    for ( ; i < 16; i++ ) buffer[i] = ' ';
    if ( !mifareclassic_WriteDataBlock( index, buffer) )
        return false;
    return true;
}


////////////////////////////////////////////////////////////////////////


byte pn532ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
byte pn532response_firmwarevers[] = {0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03};

#define PN532_PACKBUFFSIZ 64
byte pn532_packetbuffer[PN532_PACKBUFFSIZ];

bool Rfid::isready()
{
  uint8_t x = digitalRead( _irq);
  return x == 0;
}

bool Rfid::waitready( uint32_t timeout)
{
  // timeout logic reworked from original code
  // now using Arduino style 'millis()'
  uint32_t timer = millis();
  while ( timer + timeout > (uint32_t) millis() )
    if ( isready() )
      return true;
//  Serial.print( "Timeout. Irq on GPIO_"); Serial.print( _irq); Serial.print( " not received.\n");
  return false;
}

void Rfid::readdata( uint8_t* buff, uint8_t n)
{
  // Wait for chip to say its ready!
  if ( !waitready( 100) )
    return;

  uint8_t buf[n+2];
  bcm2835_i2c_setSlaveAddress( (uint8_t) PN532_I2C_ADDRESS);
  uint8_t ret = bcm2835_i2c_read( (char*) buf, n+2);
#ifdef PN532_DEBUG
  switch ( ret ) {
    case BCM2835_I2C_REASON_ERROR_NACK : Serial.println( "- Read error NACK: Not acknowledged"); break;
    case BCM2835_I2C_REASON_ERROR_CLKT : Serial.println( "- Read error CLKT: Clock stretch timeout"); break;
    case BCM2835_I2C_REASON_ERROR_DATA : Serial.println( "- Read error DATA: Not all data transferred"); break;
  }
#endif
  // Discard the leading 0x01 and trailing 0x00
  memcpy( buff, &buf[1], n);
}

void Rfid::writecommand(uint8_t* cmd, uint8_t cmdlen)
{
  delay(2);     // wake up board

  uint8_t checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;

  cmdlen++;

  uint8_t buf[cmdlen+7];
  buf[0] = PN532_PREAMBLE;
  buf[1] = PN532_PREAMBLE;
  buf[2] = PN532_STARTCODE2;
  buf[3] = cmdlen;
  buf[4] = ~cmdlen + 1;
  buf[5] = PN532_HOSTTOPN532;

  checksum += PN532_HOSTTOPN532;

  uint8_t i;
  for ( i=0; i < cmdlen - 1; i++ ) {
    buf[6+i] = cmd[i];
    checksum += cmd[i];
  }
  buf[6+i] = (uint8_t) ~checksum;
  buf[7+i] = (uint8_t) PN532_POSTAMBLE;

  bcm2835_i2c_setSlaveAddress( (uint8_t) PN532_I2C_ADDRESS);
  for ( int x = 0; x < 3; x++ ) {
    uint8_t ret = bcm2835_i2c_write( (char*) buf, 8+i);
#ifdef PN532_DEBUG
    switch ( ret ) {
      case BCM2835_I2C_REASON_ERROR_NACK : Serial.println( "- Write error NACK: Not acknowledged"); break;
      case BCM2835_I2C_REASON_ERROR_CLKT : Serial.println( "- Write error CLKT: Clock stretch timeout"); break;
      case BCM2835_I2C_REASON_ERROR_DATA : Serial.println( "- Write error DATA: Not all data transferred"); break;
    }
#endif
  }
}

void Rfid::begin( uint8_t irq, uint8_t reset)
{
  _irq = irq;
  _reset = reset;
  pinMode( _irq, INPUT);
  pinMode( _reset, OUTPUT);

  // I2C initialization.
  bcm2835_i2c_set_baudrate( 100000);
  bcm2835_i2c_begin();
  bcm2835_i2c_setSlaveAddress( (uint8_t) PN532_I2C_ADDRESS);

  // Reset the PN532
  digitalWrite( _reset, HIGH);
  digitalWrite( _reset, LOW);
  delay( 50);
  digitalWrite( _reset, HIGH);

  // Small delay required before taking other actions after reset.
  // See timing diagram on page 209 of the datasheet, section 12.23.
}

uint32_t Rfid::getFirmwareVersion()
{
  uint32_t response;
  pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

  if ( !sendCommandCheckAck(pn532_packetbuffer, 1) )
    return 0;

  // read data packet
  readdata( pn532_packetbuffer, 12);

  // check some basic stuff
  if ( strncmp((char *)pn532_packetbuffer, (char *)pn532response_firmwarevers, 6) != 0 ) {
    Serial.println( "Error: Firmware doesn't match.");
    return 0;
  }

  int offset = 7;  // Skip a response byte when using I2C to ignore extra data.
  response = pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];

  return response;
}

// default timeout of one second
bool Rfid::sendCommandCheckAck( uint8_t *cmd, uint8_t cmdlen, uint16_t timeout)
{
  // write the command
  writecommand( cmd, cmdlen);

  // read acknowledgement
  if ( !readack() )
    return false;

  return true;
}

bool Rfid::SAMConfig()
{
  pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
  pn532_packetbuffer[1] = 0x01; // normal mode;
  pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
  pn532_packetbuffer[3] = 0x01; // use IRQ pin!

  if ( !sendCommandCheckAck( pn532_packetbuffer, 4) ) {
    Serial.println( "SAMConfig-sendCommandCheckAck FAILED");
    return false;
  }

  // read data packet
  readdata( pn532_packetbuffer, 8);
  if ( pn532_packetbuffer[6] != 0x15 )
    Serial.println( "SAMConfig-readdata FAILED");

  return (pn532_packetbuffer[6] == 0x15);
}

bool Rfid::setPassiveActivationRetries( uint8_t maxRetries)
{
  pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
  pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
  pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
  pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
  pn532_packetbuffer[4] = maxRetries;

  if ( !sendCommandCheckAck( pn532_packetbuffer, 5) )
    return 0;  // no ACK

  return 1;
}

bool Rfid::readPassiveTargetID(uint8_t cardbaudrate, uint8_t * uid, uint8_t * uidLength, uint32_t timeout) 
{
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
  pn532_packetbuffer[2] = cardbaudrate;

  if ( !sendCommandCheckAck( pn532_packetbuffer, 3, timeout) )
    return 0;  // no cards read

  // read data packet
  readdata( pn532_packetbuffer, 20);
  // check some basic stuff

  // ISO14443A card response should be in the following format:

  //  byte            Description
  //  -------------   ------------------------------------------
  //  b0..6           Frame header and preamble
  //  b7              Tags Found
  //  b8              Tag Number (only one used in this example)
  //  b9..10          SENS_RES
  //  b11             SEL_RES
  //  b12             NFCID Length
  //  b13..NFCIDLen   NFCID

  //Serial.print( "Found "); Serial.print( pn532_packetbuffer[7], DEC); Serial.println( " tags");

  if ( pn532_packetbuffer[7] != 1 )
    return 0;

  uint16_t sens_res = pn532_packetbuffer[9];
  sens_res <<= 8;
  sens_res |= pn532_packetbuffer[10];

  // Card appears to be Mifare Classic 
  *uidLength = pn532_packetbuffer[12];
  for (uint8_t i=0; i < pn532_packetbuffer[12]; i++)
    uid[i] = pn532_packetbuffer[13+i];

  return 1;
}

// HI LEVEL MIFARE CLASSIC FUNCTION

bool Rfid::mifareclassic_IsFirstBlock( uint32_t uiBlock)
{
  // Test if we are in the small or big sectors
  if ( uiBlock < 128 )
    return ((uiBlock) % 4 == 0);
  else
    return ((uiBlock) % 16 == 0);
}

bool Rfid::mifareclassic_IsTrailerBlock( uint32_t uiBlock)
{
  // Test if we are in the small or big sectors
  if ( uiBlock < 128 )
    return ((uiBlock + 1) % 4 == 0);
  else
    return ((uiBlock + 1) % 16 == 0);
}

uint8_t Rfid::mifareclassic_AuthenticateBlock( uint8_t * uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t * keyData)
{
  uint8_t i;

  // Hang on to the key and uid data
  memcpy ( _key, keyData, 6);
  memcpy ( _uid, uid, uidLen);
  _uidLen = uidLen;

  // Prepare the authentication command //
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;   // Data Exchange Header 
  pn532_packetbuffer[1] = 1;                              // Max card numbers 
  pn532_packetbuffer[2] = keyNumber ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A;
  pn532_packetbuffer[3] = blockNumber;                    // Block Number (1K = 0..63, 4K = 0..255 
  memcpy( pn532_packetbuffer+4, _key, 6);
  for ( i = 0; i < _uidLen; i++ )
    pn532_packetbuffer[10+i] = _uid[i];                // 4 byte card ID 

  if ( !sendCommandCheckAck( pn532_packetbuffer, 10+_uidLen) )
    return 0;

  // Read the response packet
  readdata(pn532_packetbuffer, 12);

  // check if the response is valid and we are authenticated???
  // for an auth success it should be bytes 5-7: 0xD5 0x41 0x00
  // Mifare auth error is technically byte 7: 0x14 but anything other and 0x00 is not good
  if ( pn532_packetbuffer[7] != 0x00 )
    return 0;

  return 1;
}

uint8_t Rfid::mifareclassic_ReadDataBlock( uint8_t blockNumber, uint8_t * data)
{
  // Prepare the command 
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                      // Card number 
  pn532_packetbuffer[2] = MIFARE_CMD_READ;        // Mifare Read command = 0x30 
  pn532_packetbuffer[3] = blockNumber;            // Block Number (0..63 for 1K, 0..255 for 4K) 

  // Send the command 
  if ( !sendCommandCheckAck( pn532_packetbuffer, 4) )
    return 0;

  // Read the response packet
  readdata( pn532_packetbuffer, 26);

  // If byte 8 isn't 0x00 we probably have an error 
  if ( pn532_packetbuffer[7] != 0x00 )
    return 0;

  // Copy the 16 data bytes to the output buffer        
  // Block content starts at byte 9 of a valid response 
  memcpy( data, pn532_packetbuffer+8, 16);

  return 1;
}

uint8_t Rfid::mifareclassic_WriteDataBlock( uint8_t blockNumber, uint8_t * data)
{
  // Prepare the first command 
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                      // Card number 
  pn532_packetbuffer[2] = MIFARE_CMD_WRITE;       // Mifare Write command = 0xA0 
  pn532_packetbuffer[3] = blockNumber;            // Block Number (0..63 for 1K, 0..255 for 4K) 
  memcpy( pn532_packetbuffer+4, data, 16);        // Data Payload 

  // Send the command 
  if ( !sendCommandCheckAck( pn532_packetbuffer, 20) )
    return 0;

  // Read the response packet 
  readdata( pn532_packetbuffer, 26);

  return 1;
}

uint8_t Rfid::mifareclassic_FormatNDEF()
{
  uint8_t sectorbuffer1[16] = {0x14, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer2[16] = {0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer3[16] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77, 0x88, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Note 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 must be used for key A
  // for the MAD sector in NDEF records (sector 0)

  // Write block 1 and 2 to the card
  if ( !(mifareclassic_WriteDataBlock( 1, sectorbuffer1)) )
    return 0;
  if ( !(mifareclassic_WriteDataBlock( 2, sectorbuffer2)) )
    return 0;
  // Write key A and access rights card
  if ( !(mifareclassic_WriteDataBlock( 3, sectorbuffer3)) )
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}

uint8_t Rfid::mifareclassic_WriteNDEFURI( uint8_t sectorNumber, uint8_t uriIdentifier, const char * url)
{
  // Figure out how long the string is
  uint8_t len = strlen(url);

  // Make sure we're within a 1K limit for the sector number
  if ( (sectorNumber < 1) || (sectorNumber > 15) )
    return 0;

  // Make sure the URI payload is between 1 and 38 chars
  if ( (len < 1) || (len > 38) )
    return 0;

  // Note 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7 must be used for key A
  // in NDEF records

  // Setup the sector buffer (w/pre-formatted TLV wrapper and NDEF message)
  uint8_t l5 = len + 5;
  uint8_t l1 = len + 1;
  uint8_t sectorbuffer1[16] = {0x00, 0x00, 0x03, l5, 0xD1, 0x01, l1, 0x55, uriIdentifier, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer3[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07, 0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  if ( len <= 6 ) {
    // Unlikely we'll get a url this short, but why not ...
    memcpy( sectorbuffer1+9, url, len);
    sectorbuffer1[len+9] = 0xFE;
  }
  else
  if ( len == 7 ) {
    // 0xFE needs to be wrapped around to next block
    memcpy( sectorbuffer1+9, url, len);
    sectorbuffer2[0] = 0xFE;
  }
  else
  if ( (len > 7) && (len <= 22) ) {
    // Url fits in two blocks
    memcpy( sectorbuffer1+9, url, 7);
    memcpy( sectorbuffer2, url+7, len-7);
    sectorbuffer2[len-7] = 0xFE;
  }
  else
  if ( len == 23 ) {
    // 0xFE needs to be wrapped around to final block
    memcpy( sectorbuffer1+9, url, 7);
    memcpy( sectorbuffer2, url+7, len-7);
    sectorbuffer3[0] = 0xFE;
  }
  else {
    // Url fits in three blocks
    memcpy( sectorbuffer1+9, url, 7);
    memcpy( sectorbuffer2, url+7, 16);
    memcpy( sectorbuffer3, url+23, len-24);
    sectorbuffer3[len-22] = 0xFE;
  }

  // Now write all three blocks back to the card
  if ( !(mifareclassic_WriteDataBlock( sectorNumber*4, sectorbuffer1)) )
    return 0;
  if ( !(mifareclassic_WriteDataBlock( (sectorNumber*4)+1, sectorbuffer2)) )
    return 0;
  if ( !(mifareclassic_WriteDataBlock( (sectorNumber*4)+2, sectorbuffer3)) )
    return 0;
  if ( !(mifareclassic_WriteDataBlock( (sectorNumber*4)+3, sectorbuffer4)) )
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}
/*
uint8_t Rfid::mifareultralight_ReadPage( uint8_t page, uint8_t * buffer)
{
  if ( page >= 64 )
    return 0;

  // Prepare the command 
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                   // Card number 
  pn532_packetbuffer[2] = MIFARE_CMD_READ;     // Mifare Read command = 0x30 
  pn532_packetbuffer[3] = page;                // Page Number (0..63 in most cases) 

  // Send the command 
  if ( !sendCommandCheckAck(pn532_packetbuffer, 4) )
    return 0;

  // Read the response packet 
  delay( 10);
  readdata( pn532_packetbuffer, 26);

  // If byte 8 isn't 0x00 we probably have an error 
  if ( pn532_packetbuffer[7] == 0x00 ) {
    // Copy the 4 data bytes to the output buffer         
    // Block content starts at byte 9 of a valid response 
    // Note that the command actually reads 16 byte or 4  
    // pages at a time ... we simply discard the last 12  
    // bytes                                              
    memcpy( buffer, pn532_packetbuffer+8, 4);
  }
  else
    return 0;

  // Return OK signal
  return 1;
}

uint8_t Rfid::mifareultralight_WritePage (uint8_t page, uint8_t * data)
{
  if ( page >= 64 )
    return 0;

  // Prepare the first command 
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                      // Card number 
  pn532_packetbuffer[2] = MIFARE_ULTRALIGHT_CMD_WRITE;       // Mifare Ultralight Write command = 0xA2 
  pn532_packetbuffer[3] = page;                   // Page Number (0..63 for most cases) 
  memcpy( pn532_packetbuffer+4, data, 4);         // Data Payload 

  // Send the command 
  if ( !sendCommandCheckAck( pn532_packetbuffer, 8) )
    return 0;

  // Read the response packet 
  delay( 10);
  readdata( pn532_packetbuffer, 26);

  // Return OK Signal
  return 1;
}


uint8_t Rfid::ntag2xx_ReadPage (uint8_t page, uint8_t * buffer)
{
  // TAG Type       PAGES   USER START    USER STOP
  // --------       -----   ----------    ---------
  // NTAG 203       42      4             39
  // NTAG 213       45      4             39
  // NTAG 215       135     4             129
  // NTAG 216       231     4             225

  if ( page >= 231 )
    return 0;

  // Prepare the command 
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                   // Card number 
  pn532_packetbuffer[2] = MIFARE_CMD_READ;     // Mifare Read command = 0x30 
  pn532_packetbuffer[3] = page;                // Page Number (0..63 in most cases) 

  // Send the command 
  if ( !sendCommandCheckAck( pn532_packetbuffer, 4) )
    return 0;

  // Read the response packet
  delay( 10);
  readdata( pn532_packetbuffer, 26);

  Serial.print( String::toHexChar( pn532_packetbuffer, 26));


  // If byte 8 isn't 0x00 we probably have an error 
  if ( pn532_packetbuffer[7] == 0x00 )
  {
    // Copy the 4 data bytes to the output buffer         
    // Block content starts at byte 9 of a valid response 
    // Note that the command actually reads 16 byte or 4  
    // pages at a time ... we simply discard the last 12  
    // bytes                                              
    memcpy( buffer, pn532_packetbuffer+8, 4);
  }
  else
    return 0;

  // Return OK signal
  return 1;
}

uint8_t Rfid::ntag2xx_WritePage (uint8_t page, uint8_t * data)
{
  // TAG Type       PAGES   USER START    USER STOP
  // --------       -----   ----------    ---------
  // NTAG 203       42      4             39
  // NTAG 213       45      4             39
  // NTAG 215       135     4             129
  // NTAG 216       231     4             225

  if ( (page < 4) || (page > 225) )
    return 0;

  // Prepare the first command 
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                              // Card number 
  pn532_packetbuffer[2] = MIFARE_ULTRALIGHT_CMD_WRITE;    // Mifare Ultralight Write command = 0xA2 
  pn532_packetbuffer[3] = page;                           // Page Number (0..63 for most cases) 
  memcpy (pn532_packetbuffer+4, data, 4);                 // Data Payload 

  // Send the command 
  if ( !sendCommandCheckAck( pn532_packetbuffer, 8) )
    return 0;

  // Read the response packet 
  delay( 10);
  readdata( pn532_packetbuffer, 26);

  // Return OK Signal
  return 1;
}

uint8_t Rfid::ntag2xx_WriteNDEFURI (uint8_t uriIdentifier, char * url, uint8_t dataLen)
{
  uint8_t pageBuffer[4] = { 0, 0, 0, 0 };

  // Remove NDEF record overhead from the URI data (pageHeader below)
  uint8_t wrapperSize = 12;

  // Figure out how long the string is
  uint8_t len = strlen( url);

  // Make sure the URI payload will fit in dataLen (include 0xFE trailer)
  if ( (len < 1) || (len+1 > (dataLen-wrapperSize)) )
    return 0;

  // Setup the record header
  // See NFCForum-TS-Type-2-Tag_1.1.pdf for details
  uint8_t l5 = len + 5;
  uint8_t l1 = len + 1;
  uint8_t pageHeader[12] = {
    // NDEF Lock Control TLV (must be first and always present) 
    0x01,         // Tag Field (0x01 = Lock Control TLV) 
    0x03,         // Payload Length (always 3) 
    0xA0,         // The position inside the tag of the lock bytes (upper 4 = page address, lower 4 = byte offset) 
    0x10,         // Size in bits of the lock area 
    0x44,         // Size in bytes of a page and the number of bytes each lock bit can lock (4 bit + 4 bits) 
    // NDEF Message TLV - URI Record 
    0x03,         // Tag Field (0x03 = NDEF Message) 
    l5,        // Payload Length (not including 0xFE trailer) 
    0xD1,         // NDEF Record Header (TNF=0x1:Well known record + SR + ME + MB) 
    0x01,         // Type Length for the record type indicator 
    l1,        // Payload len 
    0x55,         // Record Type Indicator (0x55 or 'U' = URI Record) 
    uriIdentifier // URI Prefix (ex. 0x01 = "http://www.") 
  };

  // Write 12 byte header (three pages of data starting at page 4)
  memcpy( pageBuffer, pageHeader, 4);
  if ( !(ntag2xx_WritePage( 4, pageBuffer)) )
    return 0;
  memcpy( pageBuffer, pageHeader+4, 4);
  if ( !(ntag2xx_WritePage( 5, pageBuffer)) )
    return 0;
  memcpy( pageBuffer, pageHeader+8, 4);
  if ( !(ntag2xx_WritePage( 6, pageBuffer)) )
    return 0;

  // Write URI (starting at page 7)
  uint8_t currentPage = 7;
  char * urlcopy = url;
  while ( len ) {
    if ( len < 4 ) {
      memset( pageBuffer, 0, 4);
      memcpy( pageBuffer, urlcopy, len);
      pageBuffer[len] = 0xFE; // NDEF record footer
      if ( !(ntag2xx_WritePage( currentPage, pageBuffer)) )
        return 0;
      // DONE!
      return 1;
    }
    else if ( len == 4 ) {
      memcpy( pageBuffer, urlcopy, len);
      if ( !(ntag2xx_WritePage( currentPage, pageBuffer)) )
        return 0;
      memset( pageBuffer, 0, 4);
      pageBuffer[0] = 0xFE; // NDEF record footer
      currentPage++;
      if ( !(ntag2xx_WritePage( currentPage, pageBuffer)) )
        return 0;
      // DONE!
      return 1;
    }
    else {
      // More than one page of data left
      memcpy( pageBuffer, urlcopy, 4);
      if ( !(ntag2xx_WritePage( currentPage, pageBuffer)) )
        return 0;
      currentPage++;
      urlcopy += 4;
      len -= 4;
    }
  }

  // Seems that everything was OK (?!)
  return 1;
}
*/

bool Rfid::readack()
{
  uint8_t ackbuff[6];
  readdata( ackbuff, 6);

  return (strncmp( (char*) ackbuff, (char*) pn532ack, 6) == 0);
}

/*
uint8_t Rfid::AsTarget()
{
  pn532_packetbuffer[0] = 0x8C;
  uint8_t target[] = {
    0x8C, // INIT AS TARGET
    0x00, // MODE -> BITFIELD
    0x08, 0x00, //SENS_RES - MIFARE PARAMS
    0xdc, 0x44, 0x20, //NFCID1T
    0x60, //SEL_RES
    0x01,0xfe, //NFCID2T MUST START WITH 01fe - FELICA PARAMS - POL_RES
    0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,//PAD
    0xff,0xff, //SYSTEM CODE
    0xaa,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x01,0x00, //NFCID3t MAX 47 BYTES ATR_RES
    0x0d,0x52,0x46,0x49,0x44,0x49,0x4f,0x74,0x20,0x50,0x4e,0x35,0x33,0x32 //HISTORICAL BYTES
  };
  if ( !sendCommandCheckAck( target, sizeof( target)) )
    return false;

  // read data packet
  readdata( pn532_packetbuffer, 8);

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x15);
}

uint8_t Rfid::getDataTarget(uint8_t* cmd, uint8_t *cmdlen)
{
  uint8_t length;
  pn532_packetbuffer[0] = 0x86;

  if ( !sendCommandCheckAck(pn532_packetbuffer, 1, 1000) )
    return false;

  // read data packet
  delay( 10);
  readdata( pn532_packetbuffer, 64);
  length = pn532_packetbuffer[3]-3;

  //if (length > *responseLength) {// Bug, should avoid it in the reading target data
  //  length = *responseLength; // silent truncation...
  //}

  for ( int i = 0; i < length; ++i )
      cmd[i] = pn532_packetbuffer[8+i];
  *cmdlen = length;
  return true;
}

uint8_t Rfid::setDataTarget(uint8_t* cmd, uint8_t cmdlen)
{
  uint8_t length;
  //cmd1[0] = 0x8E; Must!

  if ( !sendCommandCheckAck(cmd, cmdlen) )
    return false;

  // read data packet
  readdata( pn532_packetbuffer, 8);
  length = pn532_packetbuffer[3]-3;
  for ( int i = 0; i < length; ++i )
    cmd[i] = pn532_packetbuffer[8+i];
  //cmdl = 0
  cmdlen = length;

  int offset = 6;
  return  (pn532_packetbuffer[offset] == 0x15);
}
*/

} // end of namespace
