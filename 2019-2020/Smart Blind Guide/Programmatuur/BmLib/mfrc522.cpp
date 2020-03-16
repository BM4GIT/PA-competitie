// Based on the Arduino library for the MRFC522 rfid card reader
// Based on code by Dr.Leong ( WWW.B2CQSHOP.COM )
// Created by Miguel Balboa (circuitito.com), Jan, 2012.
// Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
// Extended by Tom Clement with functionality to write to sector 0 of UID changeable Mifare cards.
// Repuposed to fit the Raspberry Pi
// Adjusted to fit the architecture of the RPi-Cpp repository.

#include "Arduino.h"
#include "mfrc522.h"
#include "bcm2835.h"
#include <linux/types.h>
#include <stdint.h>
#include <cstring>
#include <stdio.h>
#include <string>


#define RSTPIN RPI_V2_GPIO_P1_22

using namespace std;

namespace MFRC522
{

Rfid::Rfid()
{
  if ( !bcm2835_init() )
    Serial.print( "Failed to initialize. This tool needs root access, use sudo.\n");
  bcm2835_gpio_fsel( RSTPIN, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write( RSTPIN, LOW);
  setSPIConfig();
}

void Rfid::setSPIConfig()
{
  bcm2835_spi_begin();
  bcm2835_spi_setBitOrder( BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
  bcm2835_spi_setDataMode( BCM2835_SPI_MODE0);                   // The default
  bcm2835_spi_setClockDivider( BCM2835_SPI_CLOCK_DIVIDER_64);    // ~ 4 MHz
  bcm2835_spi_chipSelect( BCM2835_SPI_CS0);                      // The default
  bcm2835_spi_setChipSelectPolarity( BCM2835_SPI_CS0, LOW);      // the default
}

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

void Rfid::PCD_WriteRegister(	byte reg, byte value)
{
  char data[2];
  data[0] = reg & 0x7E;
  data[1] = value;
  bcm2835_spi_transfern( data, 2);
}

void Rfid::PCD_WriteRegister(	byte reg,byte count, byte *values)
{
  for ( byte index = 0; index < count; index++ ) {
  	PCD_WriteRegister( reg, values[index]);
	}
}

byte Rfid::PCD_ReadRegister(	byte reg)
{
  char data[2];
  data[0] = 0x80 | ((reg) & 0x7E);
  bcm2835_spi_transfern( data,2);
  return (byte) data[1];
}

void Rfid::PCD_ReadRegister(	byte reg, byte count, byte *values, byte rxAlign)
{
  if ( count == 0 ) return;

  byte address = 0x80 | (reg & 0x7E);		// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
  byte index = 0;							// Index in values array.
  count--;								// One read is performed outside of the loop
  bcm2835_spi_transfer( address);

  while ( index < count ) {
    if ( index == 0 && rxAlign ) {		// Only update bit positions rxAlign..7 in values[0]
      // Create bit mask for bit positions rxAlign..7
      byte mask = 0;
      for (byte i = rxAlign; i <= 7; i++)
        mask |= (1 << i);

      // Read value and tell that we want to read the same address again.
      byte value = bcm2835_spi_transfer( address);

      // Apply mask to both current value of values[0] and the new data in value.
      values[0] = (values[index] & ~mask) | (value & mask);
    }
    else { // Normal case
      values[index] = bcm2835_spi_transfer( address);
    }
    index++;
  }
  values[index] = bcm2835_spi_transfer( 0);			// Read the final byte. Send 0 to stop reading.
}

void Rfid::PCD_SetRegisterBitMask(	byte reg, byte mask)
{ 
  byte tmp;
  tmp = PCD_ReadRegister( reg);
  PCD_WriteRegister( reg, tmp | mask);			// set bit mask
}

void Rfid::PCD_ClearRegisterBitMask(	byte reg, byte mask)
{
  byte tmp;
  tmp = PCD_ReadRegister( reg);
  PCD_WriteRegister( reg, tmp & (~mask));		// clear bit mask
}

byte Rfid::PCD_CalculateCRC(	byte *data, byte length, byte *result)
{
  PCD_WriteRegister( CommandReg, PCD_Idle);		// Stop any active command.
  PCD_WriteRegister( DivIrqReg, 0x04);				// Clear the CRCIRq interrupt request bit
  PCD_SetRegisterBitMask( FIFOLevelReg, 0x80);		// FlushBuffer = 1, FIFO initialization
  PCD_WriteRegister( FIFODataReg, length, data);	// Write data to the FIFO
  PCD_WriteRegister( CommandReg, PCD_CalcCRC);		// Start the calculation
	
  // Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73�s.
  word i = 5000;
  byte n;
  while (1) {
    n = PCD_ReadRegister( DivIrqReg);	// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
    if ( n & 0x04 ) {						// CRCIRq bit set - calculation done
      break;
    }
    if ( --i == 0 ) {						// The emergency break. We will eventually terminate on this one after 89ms. Communication with the MFRC522 might be down.
      return STATUS_TIMEOUT;
    }
  }
  PCD_WriteRegister( CommandReg, PCD_Idle);		// Stop calculating CRC for new content in the FIFO.
	
  // Transfer the result from the registers to the result buffer
  result[0] = PCD_ReadRegister( CRCResultRegL);
  result[1] = PCD_ReadRegister( CRCResultRegH);
  return STATUS_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
// Functions for manipulating the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

void Rfid::PCD_Init()
{
  if ( bcm2835_gpio_lev( RSTPIN) == LOW ) {	//The MFRC522 chip is in power down mode.
    bcm2835_gpio_write( RSTPIN, HIGH);		// Exit power down mode. This triggers a hard reset.
    // Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74�s. Let us be generous: 50ms.
    delay( 50);
  }
  else  // Perform a soft reset
    PCD_Reset();
	
  // When communicating with a PICC we need a timeout if something goes wrong.
  // f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
  // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
  PCD_WriteRegister( TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
  PCD_WriteRegister( TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25�s.
  PCD_WriteRegister( TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
  PCD_WriteRegister( TReloadRegL, 0xE8);
	
  PCD_WriteRegister( TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
  PCD_WriteRegister( ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
  PCD_AntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
}

void Rfid::PCD_Reset()
{
  PCD_WriteRegister( CommandReg, PCD_SoftReset);	// Issue the SoftReset command.
  // The datasheet does not mention how long the SoftRest command takes to complete.
  // But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg) 
  // Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74�s. Let us be generous: 50ms.
  delay( 50);
  // Wait for the PowerDown bit in CommandReg to be cleared
  while ( PCD_ReadRegister( CommandReg) & (1 << 4) ) {
    // PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
  }
}

void Rfid::PCD_AntennaOn()
{
  byte value = PCD_ReadRegister( TxControlReg);
  if ( (value & 0x03) != 0x03 ) {
    PCD_WriteRegister( TxControlReg, value | 0x03);
  }
}

void Rfid::PCD_AntennaOff()
{
  PCD_ClearRegisterBitMask( TxControlReg, 0x03);
}

byte Rfid::PCD_GetAntennaGain()
{
  return PCD_ReadRegister( RFCfgReg) & (0x07 << 4);
}

void Rfid::PCD_SetAntennaGain(byte mask)
{
  if ( PCD_GetAntennaGain() != mask ) {						// only bother if there is a change
    PCD_ClearRegisterBitMask( RFCfgReg, (0x07<<4));		// clear needed to allow 000 pattern
    PCD_SetRegisterBitMask( RFCfgReg, mask & (0x07<<4));	// only set RxGain[2:0] bits
  }
}

bool Rfid::PCD_PerformSelfTest()
{
  // This follows directly the steps outlined in 16.1.1
  // 1. Perform a soft reset.
  PCD_Reset();
	
  // 2. Clear the internal buffer by writing 25 bytes of 00h
  byte ZEROES[25] = {0x00};
  PCD_SetRegisterBitMask( FIFOLevelReg, 0x80); // flush the FIFO buffer
  PCD_WriteRegister( FIFODataReg, 25, ZEROES); // write 25 bytes of 00h to FIFO
  PCD_WriteRegister( CommandReg, PCD_Mem); // transfer to internal buffer
	
  // 3. Enable self-test
  PCD_WriteRegister( AutoTestReg, 0x09);
	
  // 4. Write 00h to FIFO buffer
  PCD_WriteRegister( FIFODataReg, 0x00);
	
  // 5. Start self-test by issuing the CalcCRC command
  PCD_WriteRegister( CommandReg, PCD_CalcCRC);
	
  // 6. Wait for self-test to complete
  word i;
  byte n;
  for ( i = 0; i < 0xFF; i++ )
  {
    n = PCD_ReadRegister( DivIrqReg);	// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
    if ( n & 0x04 ) {						// CRCIRq bit set - calculation done
      break;
    }
  }
  PCD_WriteRegister( CommandReg, PCD_Idle);		// Stop calculating CRC for new content in the FIFO.
	
  // 7. Read out resulting 64 bytes from the FIFO buffer.
  byte result[64];
  PCD_ReadRegister( FIFODataReg, 64, result, 0);
	
  // Auto self-test done
  // Reset AutoTestReg register to be 0 again. Required for normal operation.
  PCD_WriteRegister(AutoTestReg, 0x00);
	
  // Determine firmware version (see section 9.3.4.8 in spec)
  byte version = PCD_ReadRegister(VersionReg);
	
  // Pick the appropriate reference values
  const byte *reference;
  switch ( version ) {
    case 0x91:	// Version 1.0
      reference = MFRC522_firmware_referenceV1_0;
      break;
    case 0x92:	// Version 2.0
      reference = MFRC522_firmware_referenceV2_0;
      break;
    default:	// Unknown version
      return false;
  }
	
  // Verify that the results match up to our expectations
  for ( i = 0; i < 64; i++ )
    if ( result[i] != reference[i] )
      return false;

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with PICCs
/////////////////////////////////////////////////////////////////////////////////////

byte Rfid::PCD_TransceiveData(	byte *sendData, byte sendLen,
                                  byte *backData, byte *backLen,
                                  byte *validBits, byte rxAlign, bool checkCRC)
{
  byte waitIRq = 0x30;		// RxIRq and IdleIRq
  return PCD_CommunicateWithPICC( PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
}

byte Rfid::PCD_CommunicateWithPICC(	byte command, byte waitIRq,
                                        byte *sendData, byte sendLen,
                                        byte *backData, byte *backLen,
                                        byte *validBits, byte rxAlign, bool checkCRC)
{
  byte n, _validBits;
  unsigned int i;
	
  // Prepare values for BitFramingReg
  byte txLastBits = validBits ? *validBits : 0;
  byte bitFraming = (rxAlign << 4) + txLastBits;		// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
	
  PCD_WriteRegister( CommandReg, PCD_Idle);			// Stop any active command.
  PCD_WriteRegister( ComIrqReg, 0x7F);					// Clear all seven interrupt request bits
  PCD_SetRegisterBitMask( FIFOLevelReg, 0x80);			// FlushBuffer = 1, FIFO initialization
  PCD_WriteRegister( FIFODataReg, sendLen, sendData);	// Write sendData to the FIFO
  PCD_WriteRegister( BitFramingReg, bitFraming);		// Bit adjustments
  PCD_WriteRegister( CommandReg, command);				// Execute the command
  if ( command == PCD_Transceive )
    PCD_SetRegisterBitMask( BitFramingReg, 0x80);	// StartSend=1, transmission of data starts

  // Wait for the command to complete.
  // In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
  // Each iteration of the do-while-loop takes 17.86�s.
  i = 2000;
  while ( 1 ) {
    n = PCD_ReadRegister( ComIrqReg);	// ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
    if ( n & waitIRq )				// One of the interrupts that signal success has been set.
      break;
    if ( n & 0x01 )						// Timer interrupt - nothing received in 25ms
      return STATUS_TIMEOUT;
    if ( --i == 0 )						// The emergency break. If all other condions fail we will eventually terminate on this one after 35.7ms. Communication with the MFRC522 might be down.
      return STATUS_TIMEOUT;
  }

  // Stop now if any errors except collisions were detected.
  byte errorRegValue = PCD_ReadRegister( ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
  if ( errorRegValue & 0x13 ) // BufferOvfl ParityErr ProtocolErr
    return STATUS_ERROR;

  // If the caller wants data back, get it from the MFRC522.
  if ( backData && backLen ) {
    n = PCD_ReadRegister( FIFOLevelReg);			// Number of bytes in the FIFO
    if ( n > *backLen )
      return STATUS_NO_ROOM;
    *backLen = n;											// Number of bytes returned
    PCD_ReadRegister( FIFODataReg, n, backData, rxAlign);	// Get received data from FIFO
    _validBits = PCD_ReadRegister( ControlReg) & 0x07;		// RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
    if (validBits)
      *validBits = _validBits;
  }
	
  // Tell about collisions
  if ( errorRegValue & 0x08 )		// CollErr
    return STATUS_COLLISION;
	
  // Perform CRC_A validation if requested.
  if ( backData && backLen && checkCRC ) {
    // In this case a MIFARE Classic NAK is not OK.
    if ( *backLen == 1 && _validBits == 4 )
      return STATUS_MIFARE_NACK;
    // We need at least the CRC_A value and all 8 bits of the last byte must be received.
    if ( *backLen < 2 || _validBits != 0 )
      return STATUS_CRC_WRONG;

    // Verify CRC_A - do our own calculation and store the control in controlBuffer.
    byte controlBuffer[2];
    n = PCD_CalculateCRC( &backData[0], *backLen - 2, &controlBuffer[0]);
    if ( n != STATUS_OK )
      return n;
    if ( (backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1]) )
      return STATUS_CRC_WRONG;
  }
	
  return STATUS_OK;
}

byte Rfid::PICC_RequestA( byte *bufferATQA, byte *bufferSize)
{
  return PICC_REQA_or_WUPA( PICC_CMD_REQA, bufferATQA, bufferSize);
}

byte Rfid::PICC_WakeupA(	byte *bufferATQA, byte *bufferSize)
{
  return PICC_REQA_or_WUPA( PICC_CMD_WUPA, bufferATQA, bufferSize);
}

byte Rfid::PICC_REQA_or_WUPA(	byte command, byte *bufferATQA, byte *bufferSize)
{
  byte validBits;
  byte status;
  if ( bufferATQA == NULL || *bufferSize < 2 )	// The ATQA response is 2 bytes long.
    return STATUS_NO_ROOM;

  PCD_ClearRegisterBitMask( CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.
  validBits = 7;									// For REQA and WUPA we need the short frame format - transmit only 7 bits of the last (and only) byte. TxLastBits = BitFramingReg[2..0]
  status = PCD_TransceiveData( &command, 1, bufferATQA, bufferSize, &validBits);
  if ( status != STATUS_OK )
    return status;
  if ( *bufferSize != 2 || validBits != 0 )	// ATQA must be exactly 16 bits.
    return STATUS_ERROR;

  return STATUS_OK;
}

byte Rfid::PICC_Select( Uid *uid, byte validBits)
{
  bool uidComplete;
  bool selectDone;
  bool useCascadeTag;
  byte cascadeLevel = 1;
  byte result;
  byte count;
  byte index;
  byte uidIndex;					// The first index in uid->uidByte[] that is used in the current Cascade Level.
  signed char currentLevelKnownBits;		// The number of known UID bits in the current Cascade Level.
  byte buffer[9];					// The SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A
  byte bufferUsed;				// The number of bytes used in the buffer, ie the number of bytes to transfer to the FIFO.
  byte rxAlign;					// Used in BitFramingReg. Defines the bit position for the first bit received.
  byte txLastBits;				// Used in BitFramingReg. The number of valid bits in the last transmitted byte. 
  byte *responseBuffer;
  byte responseLength;
	
  // Description of buffer structure:
  //		Byte 0: SEL 				Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3
  //		Byte 1: NVB					Number of Valid Bits (in complete command, not just the UID): High nibble: complete bytes, Low nibble: Extra bits. 
  //		Byte 2: UID-data or CT		See explanation below. CT means Cascade Tag.
  //		Byte 3: UID-data
  //		Byte 4: UID-data
  //		Byte 5: UID-data
  //		Byte 6: BCC					Block Check Character - XOR of bytes 2-5
  //		Byte 7: CRC_A
  //		Byte 8: CRC_A
  // The BCC and CRC_A is only transmitted if we know all the UID bits of the current Cascade Level.
  //
  // Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
  //		UID size	Cascade level	Byte2	Byte3	Byte4	Byte5
  //		========	=============	=====	=====	=====	=====
  //		 4 bytes		1			uid0	uid1	uid2	uid3
  //		 7 bytes		1			CT		uid0	uid1	uid2
  //						2			uid3	uid4	uid5	uid6
  //		10 bytes		1			CT		uid0	uid1	uid2
  //						2			CT		uid3	uid4	uid5
  //						3			uid6	uid7	uid8	uid9
	
  // Sanity checks
  if ( validBits > 80 )
    return STATUS_INVALID;
	
  // Prepare MFRC522
  PCD_ClearRegisterBitMask( CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.
	
  // Repeat Cascade Level loop until we have a complete UID.
  uidComplete = false;
  while ( !uidComplete ) {
    // Set the Cascade Level in the SEL byte, find out if we need to use the Cascade Tag in byte 2.
    switch ( cascadeLevel ) {
      case 1:
        buffer[0] = PICC_CMD_SEL_CL1;
        uidIndex = 0;
        useCascadeTag = validBits && uid->size > 4;	// When we know that the UID has more than 4 bytes
        break;
        
      case 2:
        buffer[0] = PICC_CMD_SEL_CL2;
        uidIndex = 3;
        useCascadeTag = validBits && uid->size > 7;	// When we know that the UID has more than 7 bytes
        break;
        
      case 3:
        buffer[0] = PICC_CMD_SEL_CL3;
        uidIndex = 6;
        useCascadeTag = false;						// Never used in CL3.
        break;
        
      default:
        return STATUS_INTERNAL_ERROR;
        break;
    }
		
    // How many UID bits are known in this Cascade Level?
    currentLevelKnownBits = validBits - (8 * uidIndex);
    if ( currentLevelKnownBits < 0 )
      currentLevelKnownBits = 0;

    // Copy the known bits from uid->uidByte[] to buffer[]
    index = 2; // destination index in buffer[]
    if ( useCascadeTag)
      buffer[index++] = PICC_CMD_CT;

    byte bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0); // The number of bytes needed to represent the known bits for this level.
    if ( bytesToCopy ) {
      byte maxBytes = useCascadeTag ? 3 : 4; // Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag
      if ( bytesToCopy > maxBytes )
        bytesToCopy = maxBytes;
      for ( count = 0; count < bytesToCopy; count++ )
        buffer[index++] = uid->uidByte[uidIndex + count];
    }

    // Now that the data has been copied we need to include the 8 bits in CT in currentLevelKnownBits
    if ( useCascadeTag ) {
      currentLevelKnownBits += 8;
    }
		
    // Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
    selectDone = false;
    while ( !selectDone ) {
      // Find out how many bits and bytes to send and receive.
      if (currentLevelKnownBits >= 32) { // All UID bits in this Cascade Level are known. This is a SELECT.
        buffer[1] = 0x70; // NVB - Number of Valid Bits: Seven whole bytes
        // Calculate BCC - Block Check Character
        buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
        // Calculate CRC_A
        result = PCD_CalculateCRC( buffer, 7, &buffer[7]);
        if ( result != STATUS_OK )
          return result;

        txLastBits = 0; // 0 => All 8 bits are valid.
        bufferUsed = 9;
        // Store response in the last 3 bytes of buffer (BCC and CRC_A - not needed after tx)
        responseBuffer = &buffer[6];
        responseLength = 3;
      }
      else { // This is an ANTICOLLISION.
        txLastBits = currentLevelKnownBits % 8;
        count = currentLevelKnownBits / 8;	// Number of whole bytes in the UID part.
        index = 2 + count;					// Number of whole bytes: SEL + NVB + UIDs
        buffer[1] = (index << 4) + txLastBits;	// NVB - Number of Valid Bits
        bufferUsed = index + (txLastBits ? 1 : 0);
        // Store response in the unused part of buffer
        responseBuffer = &buffer[index];
        responseLength = sizeof(buffer) - index;
      }
			
      // Set bit adjustments
      rxAlign = txLastBits;											// Having a seperate variable is overkill. But it makes the next line easier to read.
      PCD_WriteRegister( BitFramingReg, (rxAlign << 4) + txLastBits);	// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
			
      // Transmit the buffer and receive the response.
      result = PCD_TransceiveData( buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign);
      if ( result == STATUS_COLLISION ) { // More than one PICC in the field => collision.

        result = PCD_ReadRegister( CollReg); // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]
        if ( result & 0x20 ) // CollPosNotValid
          return STATUS_COLLISION; // Without a valid collision position we cannot continue
 
        byte collisionPos = result & 0x1F; // Values 0-31, 0 means bit 32.
        if ( collisionPos == 0 )
          collisionPos = 32;
        if ( collisionPos <= currentLevelKnownBits ) // No progress - should not happen 
          return STATUS_INTERNAL_ERROR;

        // Choose the PICC with the bit set.
        currentLevelKnownBits = collisionPos;
        count			= (currentLevelKnownBits - 1) % 8; // The bit to modify
        index			= 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First byte is index 0.
        buffer[index]	|= (1 << count);
      }
      else
      if ( result != STATUS_OK )
        return result;
      else { // STATUS_OK
        if ( currentLevelKnownBits >= 32 ) { // This was a SELECT.
          selectDone = true; // No more anticollision 
          // We continue below outside the while.
        }
        else { // This was an ANTICOLLISION.
          // We now have all 32 bits of the UID in this Cascade Level
          currentLevelKnownBits = 32;
          // Run loop again to do the SELECT.
        }
      }
    } // End of while (!selectDone)
		
    // We do not check the CBB - it was constructed by us above.
		
    // Copy the found UID bytes from buffer[] to uid->uidByte[]
    index = (buffer[2] == PICC_CMD_CT ? 3 : 2); // source index in buffer[]
    bytesToCopy = (buffer[2] == PICC_CMD_CT ? 3 : 4);
    for ( count = 0; count < bytesToCopy; count++ )
      uid->uidByte[uidIndex + count] = buffer[index++];
		
    // Check response SAK (Select Acknowledge)
    if ( responseLength != 3 || txLastBits != 0 ) // SAK must be exactly 24 bits (1 byte + CRC_A).
      return STATUS_ERROR;

    // Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those bytes are not needed anymore.
    result = PCD_CalculateCRC( responseBuffer, 1, &buffer[2]);
    if ( result != STATUS_OK )
      return result;

    if ( (buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2]) )
      return STATUS_CRC_WRONG;

    if ( responseBuffer[0] & 0x04 ) // Cascade bit set - UID not complete yes
      cascadeLevel++;
    else {
      uidComplete = true;
      uid->sak = responseBuffer[0];
    }
  } // End of while (!uidComplete)
	
  // Set correct uid->size
  uid->size = 3 * cascadeLevel + 1;
	
  return STATUS_OK;
}

byte Rfid::PICC_HaltA()
{
  byte result;
  byte buffer[4];
	
  // Build command buffer
  buffer[0] = PICC_CMD_HLTA;
  buffer[1] = 0;

  // Calculate CRC_A
  result = PCD_CalculateCRC( buffer, 2, &buffer[2]);
  if ( result != STATUS_OK )
    return result;
	
  // Send the command.
  // The standard says:
  //		If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
  //		HLTA command, this response shall be interpreted as 'not acknowledge'.
  // We interpret that this way: Only STATUS_TIMEOUT is an success.
  result = PCD_TransceiveData( buffer, sizeof(buffer), NULL, 0);
  if ( result == STATUS_TIMEOUT ) {
    return STATUS_OK;
  }
  if ( result == STATUS_OK ) { // That is ironically NOT ok in this case ;-)
    return STATUS_ERROR;
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with MIFARE PICCs
/////////////////////////////////////////////////////////////////////////////////////

byte Rfid::PCD_Authenticate( byte command, byte blockAddr, MIFARE_Key *key, Uid *uid)
{
  byte waitIRq = 0x10;		// IdleIRq
	
  // Build command buffer
  byte sendData[12];
  sendData[0] = command;
  sendData[1] = blockAddr;
  for ( byte i = 0; i < MF_KEY_SIZE; i++ )	// 6 key bytes
    sendData[2+i] = key->keyByte[i];
  for ( byte i = 0; i < 4; i++ )				// The first 4 bytes of the UID
    sendData[8+i] = uid->uidByte[i];
	
  // Start the authentication.
  return PCD_CommunicateWithPICC( PCD_MFAuthent, waitIRq, &sendData[0], sizeof(sendData));
}

void Rfid::PCD_StopCrypto1()
{
  // Clear MFCrypto1On bit
  PCD_ClearRegisterBitMask( Status2Reg, 0x08); // Status2Reg[7..0] bits are: TempSensClear I2CForceHS reserved reserved MFCrypto1On ModemState[2:0]
}

byte Rfid::MIFARE_Read(	byte blockAddr, byte *buffer, byte *bufferSize)
{
  byte result;
	
  // Sanity check
  if ( buffer == NULL || *bufferSize < 18 )
    return STATUS_NO_ROOM;
	
  buffer[0] = PICC_CMD_MF_READ;
  buffer[1] = blockAddr;
  // Calculate CRC_A
  result = PCD_CalculateCRC( buffer, 2, &buffer[2]);
  if ( result != STATUS_OK ) {
    return result;
  }
	
  // Transmit the buffer and receive the response, validate CRC_A.
  return PCD_TransceiveData( buffer, 4, buffer, bufferSize, NULL, 0, true);
}

byte Rfid::MIFARE_Write(	byte blockAddr, byte *buffer, byte bufferSize)
{
  byte result;
	
  // Sanity check
  if ( buffer == NULL || bufferSize < 16 )
    return STATUS_INVALID;
	
  // Mifare Classic protocol requires two communications to perform a write.
  // Step 1: Tell the PICC we want to write to block blockAddr.
  byte cmdBuffer[2];
  cmdBuffer[0] = PICC_CMD_MF_WRITE;
  cmdBuffer[1] = blockAddr;
  result = PCD_MIFARE_Transceive( cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
  if ( result != STATUS_OK )
    return result;
	
  // Step 2: Transfer the data
  result = PCD_MIFARE_Transceive( buffer, bufferSize); // Adds CRC_A and checks that the response is MF_ACK.
  if ( result != STATUS_OK )
    return result;
	
  return STATUS_OK;
}
/*
byte Rfid::MIFARE_Ultralight_Write( byte page, byte *buffer, byte bufferSize)
{
  byte result;
	
  // Sanity check
  if ( buffer == NULL || bufferSize < 4 )
    return STATUS_INVALID;
	
  // Build commmand buffer
  byte cmdBuffer[6];
  cmdBuffer[0] = PICC_CMD_UL_WRITE;
  cmdBuffer[1] = page;
  memcpy( &cmdBuffer[2], buffer, 4);
	
  // Perform the write
  result = PCD_MIFARE_Transceive( cmdBuffer, 6); // Adds CRC_A and checks that the response is MF_ACK.
  if ( result != STATUS_OK )
    return result;
  return STATUS_OK;
}

byte Rfid::MIFARE_Decrement(	byte blockAddr, long delta)
{
  return MIFARE_TwoStepHelper( PICC_CMD_MF_DECREMENT, blockAddr, delta);
}

byte Rfid::MIFARE_Increment(	byte blockAddr, long delta)
{
  return MIFARE_TwoStepHelper( PICC_CMD_MF_INCREMENT, blockAddr, delta);
}

byte Rfid::MIFARE_Restore(	byte blockAddr)
{
  // The datasheet describes Restore as a two step operation, but does not explain what data to transfer in step 2.
  // Doing only a single step does not work, so I chose to transfer 0L in step two.
  return MIFARE_TwoStepHelper( PICC_CMD_MF_RESTORE, blockAddr, 0L);
}

byte Rfid::MIFARE_TwoStepHelper(	byte command, byte blockAddr, long data)
{
  byte result;
  byte cmdBuffer[2]; // We only need room for 2 bytes.
	
  // Step 1: Tell the PICC the command and block address
  cmdBuffer[0] = command;
  cmdBuffer[1] = blockAddr;
  result = PCD_MIFARE_Transceive(	cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
  if ( result != STATUS_OK )
    return result;
	
  // Step 2: Transfer the data
  result = PCD_MIFARE_Transceive(	(byte*) &data, 4, true); // Adds CRC_A and accept timeout as success.
  if ( result != STATUS_OK )
    return result;

  return STATUS_OK;
}

byte Rfid::MIFARE_Transfer( byte blockAddr)
{
  byte result;
  byte cmdBuffer[2]; // We only need room for 2 bytes.
	
  // Tell the PICC we want to transfer the result into block blockAddr.
  cmdBuffer[0] = PICC_CMD_MF_TRANSFER;
  cmdBuffer[1] = blockAddr;
  result = PCD_MIFARE_Transceive(	cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
  if ( result != STATUS_OK )
    return result;

  return STATUS_OK;
}

byte Rfid::MIFARE_GetValue( byte blockAddr, long *value)
{
  byte status;
  byte buffer[18];
  byte size = sizeof( buffer);
	
  // Read the block
  status = MIFARE_Read( blockAddr, buffer, &size);
  if ( status == STATUS_OK ) {
    // Extract the value
    *value = (long(buffer[3]) << 24) | (long(buffer[2]) << 16) | (long(buffer[1]) << 8) | long(buffer[0]);
  }
  return status;
}

byte Rfid::MIFARE_SetValue( byte blockAddr, long value)
{
  byte buffer[18];
	
  // Translate the long into 4 bytes; repeated 2x in value block
  buffer[0] = buffer[ 8] = (value & 0xFF);
  buffer[1] = buffer[ 9] = (value & 0xFF00) >> 8;
  buffer[2] = buffer[10] = (value & 0xFF0000) >> 16;
  buffer[3] = buffer[11] = (value & 0xFF000000) >> 24;
  // Inverse 4 bytes also found in value block
  buffer[4] = ~buffer[0];
  buffer[5] = ~buffer[1];
  buffer[6] = ~buffer[2];
  buffer[7] = ~buffer[3];
  // Address 2x with inverse address 2x
  buffer[12] = buffer[14] = blockAddr;
  buffer[13] = buffer[15] = ~blockAddr;
	
  // Write the whole data block
  return MIFARE_Write( blockAddr, buffer, 16);
}
*/
/////////////////////////////////////////////////////////////////////////////////////
// Support functions
/////////////////////////////////////////////////////////////////////////////////////

byte Rfid::PCD_MIFARE_Transceive( byte *sendData, byte sendLen, bool acceptTimeout)
{
  byte result;
  byte cmdBuffer[18]; // We need room for 16 bytes data and 2 bytes CRC_A.
	
  // Sanity check
  if ( sendData == NULL || sendLen > 16 )
    return STATUS_INVALID;

  // Copy sendData[] to cmdBuffer[] and add CRC_A
  memcpy( cmdBuffer, sendData, sendLen);
  result = PCD_CalculateCRC( cmdBuffer, sendLen, &cmdBuffer[sendLen]);
  if ( result != STATUS_OK)
    return result;

  sendLen += 2;
	
  // Transceive the data, store the reply in cmdBuffer[]
  byte waitIRq = 0x30;		// RxIRq and IdleIRq
  byte cmdBufferSize = sizeof( cmdBuffer);
  byte validBits = 0;
  result = PCD_CommunicateWithPICC( PCD_Transceive, waitIRq, cmdBuffer, sendLen, cmdBuffer, &cmdBufferSize, &validBits);
  if ( acceptTimeout && result == STATUS_TIMEOUT )
    return STATUS_OK;
  if (result != STATUS_OK)
    return result;

  // The PICC must reply with a 4 bit ACK
  if ( cmdBufferSize != 1 || validBits != 4 )
    return STATUS_ERROR;
  if (cmdBuffer[0] != MF_ACK)
    return STATUS_MIFARE_NACK;
  return STATUS_OK;
}
/*
const string Rfid::GetStatusCodeName( byte code) {
  switch ( code ) {
    case STATUS_OK:				      return "Success.";
    case STATUS_ERROR:			    return "Error in communication.";
    case STATUS_COLLISION:		  return "Collission detected.";
    case STATUS_TIMEOUT:		    return "Timeout in communication.";
    case STATUS_NO_ROOM:		    return "A buffer is not big enough.";
    case STATUS_INTERNAL_ERROR:	return "Internal error in the code. Should not happen.";
    case STATUS_INVALID:		    return "Invalid argument.";
    case STATUS_CRC_WRONG:		  return "The CRC_A does not match.";
    case STATUS_MIFARE_NACK:	  return "A MIFARE PICC responded with NAK.";
    default:					          return "Unknown error";
  }
}

byte Rfid::PICC_GetType( byte sak)
{
  if ( sak & 0x04 ) // UID not complete
    return PICC_TYPE_NOT_COMPLETE;

  switch ( sak ) {
    case 0x09:	return PICC_TYPE_MIFARE_MINI;
    case 0x08:	return PICC_TYPE_MIFARE_1K;
    case 0x18:	return PICC_TYPE_MIFARE_4K;
    case 0x00:	return PICC_TYPE_MIFARE_UL;
    case 0x10:
    case 0x11:	return PICC_TYPE_MIFARE_PLUS;
    case 0x01:	return PICC_TYPE_TNP3XXX;
    default:	  break;
  }
  if ( sak & 0x20 )
    return PICC_TYPE_ISO_14443_4;
  if ( sak & 0x40 )
    return PICC_TYPE_ISO_18092;

  return PICC_TYPE_UNKNOWN;
}

const string Rfid::PICC_GetTypeName(byte piccType)
{
  switch (piccType) {
    case PICC_TYPE_ISO_14443_4:		return "PICC compliant with ISO/IEC 14443-4";
    case PICC_TYPE_ISO_18092:		  return "PICC compliant with ISO/IEC 18092 (NFC)";
    case PICC_TYPE_MIFARE_MINI:		return "MIFARE Mini, 320 bytes";
    case PICC_TYPE_MIFARE_1K:		  return "MIFARE 1KB";
    case PICC_TYPE_MIFARE_4K:		  return "MIFARE 4KB";
    case PICC_TYPE_MIFARE_UL:		  return "MIFARE Ultralight or Ultralight C";
    case PICC_TYPE_MIFARE_PLUS:		return "MIFARE Plus";
    case PICC_TYPE_TNP3XXX:			  return "MIFARE TNP3XXX";
    case PICC_TYPE_NOT_COMPLETE:	return "SAK indicates UID is not complete.";
    case PICC_TYPE_UNKNOWN:
    default:						          return "Unknown type";
  }
}

void Rfid::PICC_DumpToSerial( Uid *uid)
{
  MIFARE_Key key;

  // UID
  Serial.print( "Card UID:");
  Serial.println( String::toHex( uid->uidByte, uid->size));
	
  // PICC type
  byte piccType = PICC_GetType( uid->sak);
  Serial.print( "PICC type: ");
  Serial.println( PICC_GetTypeName( piccType));

  // Dump contents
  switch (piccType) {
    case PICC_TYPE_MIFARE_MINI:
    case PICC_TYPE_MIFARE_1K:
    case PICC_TYPE_MIFARE_4K:
      // All keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
      for (byte i = 0; i < 6; i++)
        key.keyByte[i] = 0xFF;
      PICC_DumpMifareClassicToSerial( uid, piccType, &key);
      break;
    case PICC_TYPE_MIFARE_UL:
      PICC_DumpMifareUltralightToSerial();
      break;
    case PICC_TYPE_ISO_14443_4:
    case PICC_TYPE_ISO_18092:
    case PICC_TYPE_MIFARE_PLUS:
    case PICC_TYPE_TNP3XXX:
      Serial.println( "Dumping memory contents not implemented for this PICC type.");
      break;
    case PICC_TYPE_UNKNOWN:
    case PICC_TYPE_NOT_COMPLETE:
    default:
      break;
  }

  Serial.println();
  PICC_HaltA(); // Already done if it was a MIFARE Classic PICC.
}

void Rfid::PICC_DumpMifareClassicToSerial(	Uid *uid, byte piccType, MIFARE_Key *key)
{
  byte no_of_sectors = 0;
  switch (piccType) {
    case PICC_TYPE_MIFARE_MINI:
      // Has 5 sectors * 4 blocks/sector * 16 bytes/block = 320 bytes.
      no_of_sectors = 5;
      break;
    case PICC_TYPE_MIFARE_1K:
      // Has 16 sectors * 4 blocks/sector * 16 bytes/block = 1024 bytes.
      no_of_sectors = 16;
      break;
    case PICC_TYPE_MIFARE_4K:
      // Has (32 sectors * 4 blocks/sector + 8 sectors * 16 blocks/sector) * 16 bytes/block = 4096 bytes.
      no_of_sectors = 40;
      break;
    default: // Should not happen. Ignore.
      break;
  }
	
  // Dump sectors, highest address first.
  if ( no_of_sectors ) {
    Serial.println( "Sector Block   0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15  AccessBits");
    for ( char i = no_of_sectors - 1; i >= 0; i-- )
      PICC_DumpMifareClassicSectorToSerial( uid, key, i);
  }
  PICC_HaltA(); // Halt the PICC before stopping the encrypted session.
  PCD_StopCrypto1();
}

void Rfid::PICC_DumpMifareClassicSectorToSerial( Uid *uid, MIFARE_Key *key, byte sector)
{
  byte status;
  byte firstBlock;		// Address of lowest address to dump actually last block dumped)
  byte no_of_blocks;		// Number of blocks in sector
  bool isSectorTrailer;	// Set to true while handling the "last" (ie highest address) in the sector.
	
  // The access bits are stored in a peculiar fashion.
  // There are four groups:
  //		g[3]	Access bits for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
  //		g[2]	Access bits for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
  //		g[1]	Access bits for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
  //		g[0]	Access bits for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
  // Each group has access bits [C1 C2 C3]. In this code C1 is MSB and C3 is LSB.
  // The four CX bits are stored together in a nible cx and an inverted nible cx_.
  byte c1, c2, c3;		// Nibbles
  byte c1_, c2_, c3_;		// Inverted nibbles
  bool invertedError;		// True if one of the inverted nibbles did not match
  byte g[4];				// Access bits for each of the four groups.
  byte group;				// 0-3 - active group for access bits
  bool firstInGroup;		// True for the first block dumped in the group
	
  // Determine position and size of sector.
  if ( sector < 32 ) { // Sectors 0..31 has 4 blocks each
    no_of_blocks = 4;
    firstBlock = sector * no_of_blocks;
  }
  else
  if ( sector < 40 ) { // Sectors 32-39 has 16 blocks each
    no_of_blocks = 16;
    firstBlock = 128 + (sector - 32) * no_of_blocks;
  }
  else  // Illegal input, no MIFARE Classic PICC has more than 40 sectors.
    return;
		
  // Dump blocks, highest address first.
  byte byteCount;
  byte buffer[18];
  byte blockAddr;
  isSectorTrailer = true;
  for ( char blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset-- ) {
    blockAddr = firstBlock + blockOffset;
    // Sector number - only on first line
    if ( isSectorTrailer ) {
      if( sector < 10 )
        Serial.print( "   "); // Pad with spaces
      else
        Serial.print( "  "); // Pad with spaces
      Serial.print( sector, HEX);
      Serial.print( "   ");
    }
    else
      Serial.print( "       ");

    // Block number
    if( blockAddr < 10 )
      Serial.print( "   "); // Pad with spaces
    else {
      if( blockAddr < 100 )
        Serial.print( "  "); // Pad with spaces
      else
        Serial.print( " "); // Pad with spaces
    }
    Serial.print( blockAddr, HEX);
    Serial.print( "  ");
    // Establish encrypted communications before reading the first block
    if ( isSectorTrailer ) {
      status = PCD_Authenticate( PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, uid);
      if ( status != STATUS_OK ) {
        Serial.print( "PCD_Authenticate() failed: ");
        Serial.println( GetStatusCodeName(status));
        return;
      }
    }
    // Read block
    byteCount = sizeof( buffer);
    status = MIFARE_Read( blockAddr, buffer, &byteCount);
    if ( status != STATUS_OK ) {
      Serial.print( GetStatusCodeName(status));
      continue;
    }
    // Dump data
    for ( byte index = 0; index < 16; index++ ) {
      if(buffer[index] < 0x10)
        Serial.print( " 0");
      else
        Serial.print( " ");
      Serial.print( "0x");
      Serial.print( buffer[index], HEX);
      if ( (index % 4) == 3 ) {
        Serial.print( " ");
      }
    }
    // Parse sector trailer data
    if ( isSectorTrailer ) {
      c1  = buffer[7] >> 4;
      c2  = buffer[8] & 0xF;
      c3  = buffer[8] >> 4;
      c1_ = buffer[6] & 0xF;
      c2_ = buffer[6] >> 4;
      c3_ = buffer[7] & 0xF;
      invertedError = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
      g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
      g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
      g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
      g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
      isSectorTrailer = false;
    }
		
    // Which access group is this block in?
    if ( no_of_blocks == 4)  {
      group = blockOffset;
      firstInGroup = true;
    }
    else {
      group = blockOffset / 5;
      firstInGroup = (group == 3) || (group != (blockOffset + 1) / 5);
    }
		
    if ( firstInGroup ) {
      // Print access bits
      Serial.print( " [ ");      
      Serial.print( (g[group] >> 2) & 1, HEX); Serial.print(" ");
      Serial.print( (g[group] >> 1) & 1, HEX); Serial.print(" ");
      Serial.print( (g[group] >> 0) & 1, HEX);
      Serial.print( " ] ");
      
      if ( invertedError )
        Serial.print( " Inverted access bits did not match! ");
    }
		
    if ( group != 3 && (g[group] == 1 || g[group] == 6)) { // Not a sector trailer, a value block
      long value = (long(buffer[3])<<24) | (long(buffer[2])<<16) | (long(buffer[1])<<8) | long(buffer[0]);
      Serial.print( " Value = 0x"); Serial.print( value, HEX);
      Serial.print( " Adr = 0x"); Serial.print( buffer[12], HEX);
    }
    Serial.println();
  }
	
  return;
}

void Rfid::PICC_DumpMifareUltralightToSerial()
{
  byte status;
  byte byteCount;
  byte buffer[18];
  byte i;
	
  Serial.print( "Page  0  1  2  3");
  // Try the mpages of the original Ultralight. Ultralight C has more pages.
  for ( byte page = 0; page < 16; page +=4 ) { // Read returns data for 4 pages at a time.
    // Read pages
    byteCount = sizeof( buffer);
    status = MIFARE_Read( page, buffer, &byteCount);
    if ( status != STATUS_OK ) {
      Serial.print( "MIFARE_Read() failed: ");
      Serial.println( GetStatusCodeName(status));
      break;
    }
    // Dump data
    for ( byte offset = 0; offset < 4; offset++ ) {
      i = page + offset;
      if( i < 10 )
        Serial.print( "  "); // Pad with spaces
      else
        Serial.print( " "); // Pad with spaces
      Serial.print( i, HEX);
      Serial.print( "  ");
      for ( byte index = 0; index < 4; index++ ) {
        i = 4 * offset + index;
        if( buffer[i] < 0x10 )
          Serial.print( " 0");
        else
          Serial.print( " ");
        Serial.print( "0x");
        Serial.print( buffer[i], HEX);
      }
      Serial.println();
    }
  }
}

void Rfid::MIFARE_SetAccessBits(	byte *accessBitBuffer,	// Pointer to byte 6, 7 and 8 in the sector trailer. Bytes [0..2] will be set.
					byte g0,				// Access bits [C1 C2 C3] for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
					byte g1,				// Access bits C1 C2 C3] for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
					byte g2,				// Access bits C1 C2 C3] for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
					byte g3					// Access bits C1 C2 C3] for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
					)
{
  byte c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
  byte c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
  byte c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);
	
  accessBitBuffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
  accessBitBuffer[1] =          c1 << 4 | (~c3 & 0xF);
  accessBitBuffer[2] =          c3 << 4 | c2;
}


bool Rfid::MIFARE_OpenUidBackdoor( bool logErrors)
{
  // Magic sequence:
  // > 50 00 57 CD (HALT + CRC)
  // > 40 (7 bits only)
  // < A (4 bits only)
  // > 43
  // < A (4 bits only)
  // Then you can write to sector 0 without authenticating
	
  PICC_HaltA(); // 50 00 57 CD
	
  byte cmd = 0x40;
  byte validBits = 7; // Our command is only 7 bits.
                      // After receiving card response,
                      // this will contain amount of valid response bits.
  byte response[32];
  byte received;
  byte status = PCD_TransceiveData( &cmd, (byte)1, response, &received, &validBits, (byte)0, false); // 40
  if( status != STATUS_OK ) {
    if( logErrors ) {
      Serial.print( "Card did not respond to 0x40 after HALT command. Are you sure it is a UID changeable one?");
      Serial.print( "Error name: ");
      Serial.println( GetStatusCodeName( status));
    }
    return false;
  }
  if ( received != 1 || response[0] != 0x0A ) {
    if ( logErrors ) {
      
      Serial.print( "Got bad response on backdoor 0x40 command: ");
      Serial.print( "0x");
      Serial.print( response[0], HEX);
      Serial.print( " (");
      Serial.print( validBits, HEX);
      Serial.println( " valid bits)");
    }
    return false;
  }
	
  cmd = 0x43;
  validBits = 8;
  status = PCD_TransceiveData( &cmd, (byte)1, response, &received, &validBits, (byte)0, false); // 43
  if( status != STATUS_OK ) {
    if( logErrors ) {
      Serial.print( "Error in communication at command 0x43, after successfully executing 0x40");
      Serial.print( "Error name: ");
      Serial.println( GetStatusCodeName( status));
    }
    return false;
  }
  if ( received != 1 || response[0] != 0x0A ) {
    if ( logErrors ) {
      Serial.print( "Got bad response on backdoor 0x43 command: ");
      Serial.print( response[0], HEX);
      Serial.print( " (");
      Serial.print( validBits, HEX);
      Serial.println( " valid bits)");
    }
    return false;
  }
	
  // You can now write to sector 0 without authenticating!
  return true;
}

bool Rfid::MIFARE_SetUid( byte *newUid, byte uidSize, bool logErrors) {
	
  // UID + BCC byte can not be larger than 16 together
  if ( !newUid || !uidSize || uidSize > 15 ) {
    if ( logErrors )
      Serial.println( "New UID buffer empty, size 0, or size > 15 given");
    return false;
  }
	
  // Authenticate for reading
  MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  byte status = PCD_Authenticate( Rfid::PICC_CMD_MF_AUTH_KEY_A, (byte)1, &key, &uid);
  if ( status != STATUS_OK ) {
		
    if ( status == STATUS_TIMEOUT ) {
      // We get a read timeout if no card is selected yet, so let's select one
			
      // Wake the card up again if sleeping
      //			  byte atqa_answer[2];
      //			  byte atqa_size = 2;
      //			  PICC_WakeupA(atqa_answer, &atqa_size);
			
      if (!PICC_IsNewCardPresent() || !PICC_ReadCardSerial()) {
        Serial.println( "No card was previously selected, and none are available. Failed to set UID.");
        return false;
      }
			
      status = PCD_Authenticate( Rfid::PICC_CMD_MF_AUTH_KEY_A, (byte)1, &key, &uid);
      if (status != STATUS_OK) {
        // We tried, time to give up
        if ( logErrors ) {
         Serial.print( "Failed to authenticate to card for reading, could not set UID: ");
         Serial.println( GetStatusCodeName(status));
        }
        return false;
      }
    }
    else {
      if (logErrors) {
        Serial.print( "PCD_Authenticate() failed: ");
        Serial.println( GetStatusCodeName(status));
      }
      return false;
    }
  }
	
  // Read block 0
  byte block0_buffer[18];
  byte byteCount = sizeof( block0_buffer);
  status = MIFARE_Read( (byte) 0, block0_buffer, &byteCount);
  if ( status != STATUS_OK ) {
    if ( logErrors ) {
      Serial.print( "MIFARE_Read() failed: ");
      Serial.println( GetStatusCodeName(status));
      Serial.println( "Are you sure your KEY A for sector 0 is 0xFFFFFFFFFFFF?");
    }
    return false;
  }
	
  // Write new UID to the data we just read, and calculate BCC byte
  byte bcc = 0;
  for ( int i = 0; i < uidSize; i++ ) {
    block0_buffer[i] = newUid[i];
    bcc ^= newUid[i];
  }
	
  // Write BCC byte to buffer
  block0_buffer[uidSize] = bcc;
	
  // Stop encrypted traffic so we can send raw bytes
  PCD_StopCrypto1();
	
  // Activate UID backdoor
  if ( !MIFARE_OpenUidBackdoor(logErrors) ) {
    if ( logErrors )
      Serial.println( "Activating the UID backdoor failed.");
    return false;
  }
	
  // Write modified block 0 back to card
  status = MIFARE_Write( (byte) 0, block0_buffer, (byte) 16);
  if ( status != STATUS_OK ) {
    if ( logErrors ) {
      Serial.print( "MIFARE_Write() failed: ");
      Serial.println( GetStatusCodeName(status));
    }
    return false;
  }
	
  // Wake the card up again
  byte atqa_answer[2];
  byte atqa_size = 2;
  PICC_WakeupA( atqa_answer, &atqa_size);
	
  return true;
}

bool Rfid::MIFARE_UnbrickUidSector( bool logErrors)
{
  MIFARE_OpenUidBackdoor( logErrors);
	
  byte block0_buffer[] = {0x01, 0x02, 0x03, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
  // Write modified block 0 back to card
  byte status = MIFARE_Write( (byte)0, block0_buffer, (byte)16);
  if ( status != STATUS_OK ) {
    if (logErrors) {
      Serial.print( "MIFARE_Write() failed: ");
      Serial.println( GetStatusCodeName(status));
    }
    return false;
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Convenience functions - does not add extra functionality
/////////////////////////////////////////////////////////////////////////////////////

*/

bool Rfid::PICC_IsNewCardPresent()
{
  byte bufferATQA[2];
  byte bufferSize = sizeof( bufferATQA);
  byte result = PICC_RequestA( bufferATQA, &bufferSize);
  if ( result == STATUS_OK || result == STATUS_COLLISION )
    return true;
  return false;
}

bool Rfid::PICC_ReadCardSerial()
{
  byte result = PICC_Select( &uid);
  if ( result == STATUS_OK )
    return true;
  return false;
}

} // end namespace
