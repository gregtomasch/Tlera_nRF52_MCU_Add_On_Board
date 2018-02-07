#include "Arduino.h"
#include "Host_BLEserial.h"

BLEserial::BLEserial()
{
}

/**
* @fn: serialCom()
*
* @brief: Main MultiWii Serial Protocol (MSP) function; manages port state and packet logistics
* @params:
* @returns:
*/
void BLEserial::serialCom()
{
  uint8_t c,n;  

  for(n=0; n < UART_NUMBER; n++)                                                                                        // General function to handle multiple active ports
  {
    CURRENTPORT = n;
    while (BLEserial::SerialAvailable(CURRENTPORT))
    {
      // Indicates the number of occupied bytes in TX buffer
      uint8_t bytesTXBuff = ((uint8_t)(serialHeadTX[CURRENTPORT] - serialTailTX[CURRENTPORT]))%TX_BUFFER_SIZE;

      // Ensure there is enough free TX buffer to go further (50 bytes margin)
      if (bytesTXBuff > TX_BUFFER_SIZE - 50 ) return;
      c = BLEserial::SerialRead(CURRENTPORT);
      if (c_state[CURRENTPORT] == IDLE)
      {
        c_state[CURRENTPORT] = (c=='$') ? HEADER_START : IDLE;
      }else if (c_state[CURRENTPORT] == HEADER_START)
      {
        c_state[CURRENTPORT] = (c=='M') ? HEADER_M : IDLE;
      } else if (c_state[CURRENTPORT] == HEADER_M)
      {
        c_state[CURRENTPORT] = (c=='<') ? HEADER_ARROW : IDLE;
      } else if (c_state[CURRENTPORT] == HEADER_ARROW)
      { 
        // Now we are expecting the payload size
        if (c > INBUF_SIZE)
        {
          c_state[CURRENTPORT] = IDLE;
          continue;
        }
        dataSize[CURRENTPORT] = c;
        offset[CURRENTPORT] = 0;
        checksum[CURRENTPORT] = 0;
        indRX[CURRENTPORT] = 0;
        checksum[CURRENTPORT] ^= c;

        // The command is to follow
        c_state[CURRENTPORT] = HEADER_SIZE;
      } else if (c_state[CURRENTPORT] == HEADER_SIZE)
      {
        cmdBLEserial[CURRENTPORT] = c;
        checksum[CURRENTPORT] ^= c;
        c_state[CURRENTPORT] = HEADER_CMD;
      } else if (c_state[CURRENTPORT] == HEADER_CMD && offset[CURRENTPORT] < dataSize[CURRENTPORT])
      {
        checksum[CURRENTPORT] ^= c;
        inBuf[offset[CURRENTPORT]++][CURRENTPORT] = c;
      } else if (c_state[CURRENTPORT] == HEADER_CMD && offset[CURRENTPORT] >= dataSize[CURRENTPORT])
      {
        // Compare calculated and transferred checksum
        if (checksum[CURRENTPORT] == c) 
        {
          // We got a valid packet, evaluate it
          BLEserial::evaluateCommand();
        }
        c_state[CURRENTPORT] = IDLE;
      }
    }
  }
}

/**
* @fn: evaluateCommand()
*
* @brief: Main (MSP) command decoder
* @params:
* @returns:
*/
void BLEserial::evaluateCommand()
{
  switch(cmdBLEserial[CURRENTPORT])
  {
   case RX_ACK_NAK_BYTE:                                                                                                // CMD 14
     ack_nak_byte = BLEserial::read8();
     break;
   default:
     break;
  }
}

/**
* @fn:  Initiate_OTA_Update()
*
* @brief: Sends the new image size and number of data blocks to the hust to initiate OTA FW update
* @params:
* @returns:
*/
void BLEserial::Initiate_OTA_Update()
{
  CURRENTPORT = TEENSY_BLE_UART;
  cmdBLEserial[TEENSY_BLE_UART] = 12;                                                                                  // Call CMD 12, "INITIATE_OTA_UPDATE" payload 6 bytes
  BLEserial::headSerialRequest(TEENSY_BLE_UART, 0, 6);
  BLEserial::serialize32(OTA_Update_Size);
  BLEserial::serialize16(OTA_Update_Blocks);
  BLEserial::tailSerialRequest(TEENSY_BLE_UART);
}

/**
* @fn:  Push_OTA_Data()
*
* @brief: Pushes the current values of OTA update array into a serial command; data variables treated as unsigned bytes
* @params:
* @returns:
*/
void BLEserial::Push_OTA_Data()
{
  CURRENTPORT = TEENSY_BLE_UART;
  cmdBLEserial[TEENSY_BLE_UART] = 13;                                                                                  // Call CMD 13, "RECEIVE_OTA_DATA" payload OTA_DATA_BLOCK_SIZE+2 bytes
  BLEserial::headSerialRequest(TEENSY_BLE_UART, 0, OTA_DATA_BLOCK_SIZE+2);
  for(uint8_t i=0; i<OTA_DATA_BLOCK_SIZE+2; i++)                                                                        // First two bytes of the array are the block number in uint16_t form
  {
    BLEserial::serialize8(OTA_bytes[i]);
  }
  BLEserial::tailSerialRequest(TEENSY_BLE_UART);
}

/**
* @fn: readfloat()
*
* @brief: Read 4bytes from the input buffer and converts to float
* @params:
* @returns:
*/
float BLEserial::readfloat()
{
  union
  {
    uint8_t float_data_byte[4];
    float float_data;
  } f;
  
  for( uint8_t i=0; i<4; i++)
  {
    f.float_data_byte[i] = BLEserial::read8();
  }
  return f.float_data;
}

/**
* @fn: read32()
*
* @brief: Read 4bytes from the input buffer
* @params:
* @returns:
*/
uint32_t BLEserial::read32()
{
  uint32_t t = BLEserial::read16();
  t+= (uint32_t)BLEserial::read16()<<16;
  return t;
}

/**
* @fn: read16()
*
* @brief: Read 2bytes from the input buffer
* @params:
* @returns:
*/
uint16_t BLEserial::read16()
{
  uint16_t t = BLEserial::read8();
  t+= (uint16_t)BLEserial::read8()<<8;
  return t;
}

/**
* @fn: read8()
*
* @brief: Read 1byte from the input buffer
* @params:
* @returns:
*/
uint8_t BLEserial::read8()
{
  return inBuf[indRX[CURRENTPORT]++][CURRENTPORT]&0xff;
}

/**
* @fn: headSerialRequest(uint8_t port, uint8_t err, uint8_t s)
*
* @brief: Construct serial request header
* @params: Port, error state and message size in bytes
* @returns:
*/
void BLEserial::headSerialRequest(uint8_t port, uint8_t err, uint8_t s)
{
  BLEserial::serialize8('$');
  BLEserial::serialize8('M');
  BLEserial::serialize8(err ? '!' : '<');
  
  // start calculating a new checksum
  checksum[port] = 0;
  BLEserial::serialize8(s);
  BLEserial::serialize8(cmdBLEserial[port]);
}

/**
* @fn: headSerialResponse(uint8_t err, uint8_t s)
*
* @brief: Construct serial reply header
* @params: Error state and reply size in bytes
* @returns:
*/
void BLEserial::headSerialResponse(uint8_t err, uint8_t s)
{
  BLEserial::serialize8('$');
  BLEserial::serialize8('M');
  BLEserial::serialize8(err ? '!' : '>');
  
  // start calculating a new checksum
  checksum[CURRENTPORT] = 0;
  BLEserial::serialize8(s);
  BLEserial::serialize8(cmdBLEserial[CURRENTPORT]);
}

/**
* @fn: headSerialReply(uint8_t s)
*
* @brief: Construct serial reply header with error state "0"
* @params: Reply size in bytes
* @returns:
*/
void BLEserial::headSerialReply(uint8_t s)
{
  BLEserial::headSerialResponse(0, s);
}

/**
* @fn: headSerialError(uint8_t s)
*
* @brief: Construct serial reply header with error state "1"
* @params: Reply size in bytes
* @returns:
*/
void inline BLEserial::headSerialError(uint8_t s)
{
  BLEserial::headSerialResponse(1, s);
}

/**
* @fn: tailSerialRequest()
*
* @brief: Construct and transmit checksum data for end of packet
* @params:
* @returns:
*/
void BLEserial::tailSerialRequest(uint8_t port)
{
  BLEserial::serialize8(checksum[port]);
  BLEserial::UartSendData(port);
}

/**
* @fn: tailSerialReply()
*
* @brief: Construct and transmit checksum data for end of packet
* @params:
* @returns:
*/
void BLEserial::tailSerialReply()
{
  BLEserial::serialize8(checksum[CURRENTPORT]);
  BLEserial::UartSendData(CURRENTPORT);
}

/**
* @fn: serializeNames(PGM_P s)
*
* @brief: "Serialize" ASCII into bytes for serial transmission
* @params:
* @returns:
*/
void BLEserial::serializeNames(PGM_P s)
{
  for (PGM_P c = s; pgm_read_byte(c); c++)
  {
    BLEserial::serialize8(pgm_read_byte(c));
  }
}

/**
* @fn: serializefloat(float a)
*
* @brief: "Serialize" a float into four bytes for serial transmission
* @params:
* @returns:
*/
void BLEserial::serializefloat(float a)
{
  union
  {
    float float_data;
    uint8_t float_data_byte[4];
  } f;
  f.float_data = a;
  for( uint8_t i=0; i<4; i++)
  {
    BLEserial::serialize8(f.float_data_byte[i]);
  }
}

/**
* @fn: serialize32(uint32_t a)
*
* @brief: "Serialize" a 32bit unsigned integer into four bytes for serial transmission
* @params:
* @returns:
*/
void BLEserial::serialize32(uint32_t a)
{
  BLEserial::serialize8((a    ) & 0xFF);
  BLEserial::serialize8((a>> 8) & 0xFF);
  BLEserial::serialize8((a>>16) & 0xFF);
  BLEserial::serialize8((a>>24) & 0xFF);
}

/**
* @fn: serialize16(int16_t a)
*
* @brief: "Serialize" a 16bit signed integer into four bytes for serial transmission
* @params:
* @returns:
*/
void BLEserial::serialize16(int16_t a)
{
  BLEserial::serialize8((a   ) & 0xFF);
  BLEserial::serialize8((a>>8) & 0xFF);
}

/**
* @fn: serialize16(uint8_t a)
*
* @brief: "Serialize" an 8bit unsigned integer into four bytes for serial transmission
* @params:
* @returns:
*/
void BLEserial::serialize8(uint8_t a)
{
  uint8_t t = serialHeadTX[CURRENTPORT];
  if (++t >= TX_BUFFER_SIZE) t = 0;
  serialBufferTX[t][CURRENTPORT] = a;
  checksum[CURRENTPORT] ^= a;
  serialHeadTX[CURRENTPORT] = t;
}

/**
* @fn: T_USB_Available(uint8_t port)
*
* @brief: Returns the number of bytes available to be read from port
* @params:
* @returns:
*/
unsigned char BLEserial::T_USB_Available(uint8_t port)
{
  int n ; 
  switch (port)
  {
    case 0: n= Serial.available(); break;
    case 1: n= Serial1.available(); break;
    case 2: n= Serial2.available(); break;
    case 3: n= Serial3.available(); break;
    default: n=0; break;
  }
  if (n > 255) n = 255;
  return n;
}

/**
* @fn: T_USB_Write(uint8_t port, uint8_t uc_data)
*
* @brief: Writes specified data to specified port
* @params: Data to be written, port to be written to
* @returns: Number of btes written
*/
unsigned char BLEserial::T_USB_Write(uint8_t port, uint8_t uc_data)
{
  int n ; 
  switch (port)
  {
    case 0: n= Serial.write(uc_data); break;
    case 1: n= Serial1.write(uc_data); break;
    case 2: n= Serial2.write(uc_data); break;
    case 3: n= Serial3.write(uc_data); break;
    default: n=0; break;
  }
  return n;
}


/**
* @fn: UartSendData(uint8_t port)
*
* @brief: Writes TX buffer to serial port
* @params: Port to be written to
* @returns:
*/
void BLEserial::UartSendData(uint8_t port)
{
  while(serialHeadTX[port] != serialTailTX[port])
  {
    if (++serialTailTX[port] >= TX_BUFFER_SIZE) serialTailTX[port] = 0;
    BLEserial::T_USB_Write (port,serialBufferTX[serialTailTX[port]][port]);
  }

  // Append <Lf> ('\n') to send remaining data over NUS BLE/UART bridge
  BLEserial::T_USB_Write (port,'\n');
}

/**
* @fn: SerialTXfree(uint8_t port)
*
* @brief: Determines if serial port is free (TX buffer empty)
* @params: Port to be written to
* @returns: Boolean regarding port availability
*/
bool BLEserial::SerialTXfree(uint8_t port)
{
  return (serialHeadTX[port] == serialTailTX[port]);
}

/**
* @fn: SerialOpen(uint8_t port, uint32_t baud)
*
* @brief: Opens serial port for use
* @params: Port to be initialized, baud rate
* @returns:
*/
void BLEserial::SerialOpen(uint8_t port, uint32_t baud)
{
  // Assumes only one UART port open at a time! 
  switch (port)
  {
    case 0:
      Serial.begin(baud);
      break;
    case 1:
      Serial1.begin(baud);
      break;
    case 2:
      Serial2.begin(baud);
      break;
    case 3:
      Serial3.begin(baud);
      break;
  }
}

/**
* @fn: SerialEnd(uint8_t port)
*
* @brief: Closes serial port
* @params: Port to be closed
* @returns:
*/
void BLEserial::SerialEnd(uint8_t port)
{
  switch (port)
  {
      case 0: Serial.end(); break;
      case 1: Serial1.end(); break;
      case 2: Serial2.end(); break;
      case 3: Serial3.end(); break;
  }
}

/**
* @fn: SerialRead(uint8_t port)
*
* @brief: Reads a byte from a serialport
* @params: Port to be read
* @returns:
*/
uint8_t BLEserial::SerialRead(uint8_t port)
{
  switch (port)
  {
    case 0: return Serial.read();
    case 1: return Serial1.read();
    case 2: return Serial2.read();
    case 3: return Serial3.read();
  }
  return 0;
}

uint8_t BLEserial::SerialAvailable(uint8_t port)
{
  return BLEserial::T_USB_Available(port);
}

void BLEserial::SerialSerialize(uint8_t port,uint8_t a)
{
  uint8_t t = serialHeadTX[port];
  if (++t >= TX_BUFFER_SIZE) t = 0;
  serialBufferTX[t][port] = a;
  serialHeadTX[port] = t;
}

void BLEserial::SerialWrite(uint8_t port,uint8_t c)
{
  CURRENTPORT=port;
  SerialSerialize(port,c);BLEserial::UartSendData(port);
}
