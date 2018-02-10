#include "Arduino.h"
#include "Wire.h"
#include "Host_WirelessSerial.h"

WirelessSerial::WirelessSerial()
{
}

/**
* @fn: serialCom()
*
* @brief: Main MultiWii Serial Protocol (MSP) function; manages port state and packet logistics
* @params:
* @returns:
*/
void WirelessSerial::serialCom()
{
  uint8_t c,n;  

  for(n=0; n < UART_NUMBER; n++)                                                                                          // General function to handle multiple active ports. Limited to UART1 - UART3 for the IU Butterfly-based board
  {
    CURRENTPORT = n;
    while (WirelessSerial::SerialAvailable(CURRENTPORT))
    {
      // Indicates the number of occupied bytes in TX buffer
      uint8_t bytesTXBuff = ((uint8_t)(serialHeadTX[CURRENTPORT] - serialTailTX[CURRENTPORT]))%TX_BUFFER_SIZE;

      // Ensure there is enough free TX buffer to go further (50 bytes margin)
      if (bytesTXBuff > TX_BUFFER_SIZE - 50 ) return;
      c = WirelessSerial::SerialRead(CURRENTPORT);
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
        cmdWirelessSerial[CURRENTPORT] = c;
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
          WirelessSerial::evaluateCommand();
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
void WirelessSerial::evaluateCommand()
{
  switch(cmdWirelessSerial[CURRENTPORT])
  {
   case RECEIVE_SERIAL_DATA:                                                                                              // CMD 10
     for(uint8_t i=0; i < (UPDATE_SIZE-1); i++)
     {
       data_update[i] = WirelessSerial::readfloat();
     }
     break;
   case INITIATE_OTA_UPDATE:                                                                                              // CMD 12
     OTA_Update_Size = WirelessSerial::read32();
     OTA_Update_Blocks = WirelessSerial::read16();
     OTA::OTA_Update(OTA_Update_Size, OTA_Update_Blocks);
     break;
   case RECEIVE_OTA_DATA:                                                                                                 // CMD 13
     block = WirelessSerial::read16();
     for(uint8_t i=0; i<DATA_BLOCK_SIZE; i++)
     {
       OTA_bytes[i] = WirelessSerial::read8();
     }
     break;
   case SEND_SERIAL_DATA:                                                                                                 // CMD 20
     cmdWirelessSerial[BUTTERFLY_BLE_UART] = 10;
     WirelessSerial::headSerialRequest(BUTTERFLY_BLE_UART, 0, 4*(UPDATE_SIZE));
     for(uint8_t i=0; i<(UPDATE_SIZE-1); i++)
     {
       WirelessSerial::serializefloat(data_update[i]);
     }
     WirelessSerial::tailSerialRequest(BUTTERFLY_BLE_UART);
     break;
   default:
     break;
  }
}

/**
* @fn: SerialSendMWPcmd(uint8_t port, uint8_t err,uint8_t cmd)
*
* @brief: Sends a data request command to the remote host
* @params: port, command
* @returns:
*/
void WirelessSerial::SerialSendMWPcmd(uint8_t port, uint8_t err,uint8_t cmd)
{
  // Buffer previous command value for this port
  uint8_t cmd_prev = cmdWirelessSerial[port];
  
  cmdWirelessSerial[port] = cmd;
  WirelessSerial::headSerialRequest(port, err, 0);
  WirelessSerial::tailSerialRequest(port);

  // Restore previous command value for this port
  cmdWirelessSerial[port] = cmd_prev;
}

/**
* @fn: Push_Data_Update()
*
* @brief: Pushes the current values of output variables into an update message; data variables treated as SP floats
* @params:
* @returns:
*/
void WirelessSerial::Push_Data_Update()
{
  CURRENTPORT = BUTTERFLY_BLE_UART;
  cmdWirelessSerial[BUTTERFLY_BLE_UART] = 10;
  WirelessSerial::headSerialRequest(BUTTERFLY_BLE_UART, 0, 4*(UPDATE_SIZE));
  for(uint8_t i=0; i < (UPDATE_SIZE); i++)
  {
    WirelessSerial::serializefloat(data_update[i]);
  }
  WirelessSerial::tailSerialRequest(BUTTERFLY_BLE_UART);
}

/**
* @fn:  OTA_Update_ACK_NAK(uint8_t ack_nak_byte)
*
* @brief: Sends an ACK (1) or a NAK (0) back to the host
* @params:
* @returns: ACK/NAK byte
*/
void WirelessSerial::OTA_Update_ACK_NAK(uint8_t ack_nak_byte)
{
  CURRENTPORT = BUTTERFLY_BLE_UART;
  cmdWirelessSerial[BUTTERFLY_BLE_UART] = 14;
  WirelessSerial::headSerialRequest(BUTTERFLY_BLE_UART, 0, 1);
  WirelessSerial::serialize8(ack_nak_byte);
  WirelessSerial::tailSerialRequest(BUTTERFLY_BLE_UART);
}

/**
* @fn: readfloat()
*
* @brief: Read 4bytes from the input buffer and converts to float
* @params:
* @returns:
*/
float WirelessSerial::readfloat()
{
  union
  {
    uint8_t float_data_byte[4];
    float float_data;
  } f;
  
  for( uint8_t i=0; i<4; i++)
  {
    f.float_data_byte[i] = WirelessSerial::read8();
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
uint32_t WirelessSerial::read32()
{
  uint32_t t = WirelessSerial::read16();
  t+= (uint32_t)WirelessSerial::read16()<<16;
  return t;
}

/**
* @fn: read16()
*
* @brief: Read 2bytes from the input buffer
* @params:
* @returns:
*/
uint16_t WirelessSerial::read16()
{
  uint16_t t = WirelessSerial::read8();
  t+= (uint16_t)WirelessSerial::read8()<<8;
  return t;
}

/**
* @fn: read8()
*
* @brief: Read 1byte from the input buffer
* @params:
* @returns:
*/
uint8_t WirelessSerial::read8()
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
void WirelessSerial::headSerialRequest(uint8_t port, uint8_t err, uint8_t s)
{
  WirelessSerial::serialize8('$');
  WirelessSerial::serialize8('M');
  WirelessSerial::serialize8(err ? '!' : '<');
  
  // start calculating a new checksum
  checksum[port] = 0;
  WirelessSerial::serialize8(s);
  WirelessSerial::serialize8(cmdWirelessSerial[port]);
}

/**
* @fn: headSerialResponse(uint8_t err, uint8_t s)
*
* @brief: Construct serial reply header
* @params: Error state and reply size in bytes
* @returns:
*/
void WirelessSerial::headSerialResponse(uint8_t err, uint8_t s)
{
  WirelessSerial::serialize8('$');
  WirelessSerial::serialize8('M');
  WirelessSerial::serialize8(err ? '!' : '>');
  
  // start calculating a new checksum
  checksum[CURRENTPORT] = 0;
  WirelessSerial::serialize8(s);
  WirelessSerial::serialize8(cmdWirelessSerial[CURRENTPORT]);
}

/**
* @fn: headSerialReply(uint8_t s)
*
* @brief: Construct serial reply header with error state "0"
* @params: Reply size in bytes
* @returns:
*/
void WirelessSerial::headSerialReply(uint8_t s)
{
  WirelessSerial::headSerialResponse(0, s);
}

/**
* @fn: headSerialError(uint8_t s)
*
* @brief: Construct serial reply header with error state "1"
* @params: Reply size in bytes
* @returns:
*/
void inline WirelessSerial::headSerialError(uint8_t s)
{
  WirelessSerial::headSerialResponse(1, s);
}

/**
* @fn: tailSerialRequest()
*
* @brief: Construct and transmit checksum data for end of packet
* @params:
* @returns:
*/
void WirelessSerial::tailSerialRequest(uint8_t port)
{
  WirelessSerial::serialize8(checksum[port]);
  WirelessSerial::UartSendData(port);
}

/**
* @fn: tailSerialReply()
*
* @brief: Construct and transmit checksum data for end of packet
* @params:
* @returns:
*/
void WirelessSerial::tailSerialReply()
{
  WirelessSerial::serialize8(checksum[CURRENTPORT]);
  WirelessSerial::UartSendData(CURRENTPORT);
}

/**
* @fn: serializeNames(PGM_P s)
*
* @brief: "Serialize" ASCII into bytes for serial transmission
* @params:
* @returns:
*/
void WirelessSerial::serializeNames(PGM_P s)
{
  for (PGM_P c = s; pgm_read_byte(c); c++)
  {
    WirelessSerial::serialize8(pgm_read_byte(c));
  }
}

/**
* @fn: serializefloat(float a)
*
* @brief: "Serialize" a float into four bytes for serial transmission
* @params:
* @returns:
*/
void WirelessSerial::serializefloat(float a)
{
  union
  {
    float float_data;
    uint8_t float_data_byte[4];
  } f;
  f.float_data = a;
  for( uint8_t i=0; i<4; i++)
  {
    WirelessSerial::serialize8(f.float_data_byte[i]);
  }
}

/**
* @fn: serialize32(uint32_t a)
*
* @brief: "Serialize" a 32bit unsigned integer into four bytes for serial transmission
* @params:
* @returns:
*/
void WirelessSerial::serialize32(uint32_t a)
{
  WirelessSerial::serialize8((a    ) & 0xFF);
  WirelessSerial::serialize8((a>> 8) & 0xFF);
  WirelessSerial::serialize8((a>>16) & 0xFF);
  WirelessSerial::serialize8((a>>24) & 0xFF);
}

/**
* @fn: serialize16(int16_t a)
*
* @brief: "Serialize" a 16bit signed integer into four bytes for serial transmission
* @params:
* @returns:
*/
void WirelessSerial::serialize16(int16_t a)
{
  WirelessSerial::serialize8((a   ) & 0xFF);
  WirelessSerial::serialize8((a>>8) & 0xFF);
}

/**
* @fn: serialize16(uint8_t a)
*
* @brief: "Serialize" an 8bit unsigned integer into four bytes for serial transmission
* @params:
* @returns:
*/
void WirelessSerial::serialize8(uint8_t a)
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
unsigned char WirelessSerial::T_USB_Available(uint8_t port)
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
unsigned char WirelessSerial::T_USB_Write(uint8_t port, uint8_t uc_data)
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
void WirelessSerial::UartSendData(uint8_t port)
{
  while(serialHeadTX[port] != serialTailTX[port])
  {
    if (++serialTailTX[port] >= TX_BUFFER_SIZE) serialTailTX[port] = 0;
    WirelessSerial::T_USB_Write (port,serialBufferTX[serialTailTX[port]][port]);
  }

  // Append <Lf> ('\n') to send remaining data over NUS BLE/UART bridge
  WirelessSerial::T_USB_Write (port,'\n');
}

/**
* @fn: SerialTXfree(uint8_t port)
*
* @brief: Determines if serial port is free (TX buffer empty)
* @params: Port to be written to
* @returns: Boolean regarding port availability
*/
bool WirelessSerial::SerialTXfree(uint8_t port)
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
void WirelessSerial::SerialOpen(uint8_t port, uint32_t baud)
{
  // Assumes only one UART port open at a time! 
  switch (port)
  {
    case 0:
      Serial.begin(baud);
      break;
    case 1:
      Serial1.begin(baud, UartRxBuffer, 256);
      break;
    case 2:
      Serial2.begin(baud, UartRxBuffer, 256);
      break;
    case 3:
      Serial3.begin(baud, UartRxBuffer, 256);
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
void WirelessSerial::SerialEnd(uint8_t port)
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
uint8_t WirelessSerial::SerialRead(uint8_t port)
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

uint8_t WirelessSerial::SerialAvailable(uint8_t port)
{
  return WirelessSerial::T_USB_Available(port);
}

void WirelessSerial::SerialSerialize(uint8_t port,uint8_t a)
{
  uint8_t t = serialHeadTX[port];
  if (++t >= TX_BUFFER_SIZE) t = 0;
  serialBufferTX[t][port] = a;
  serialHeadTX[port] = t;
}

void WirelessSerial::SerialWrite(uint8_t port,uint8_t c)
{
  CURRENTPORT=port;
  SerialSerialize(port,c);WirelessSerial::UartSendData(port);
}
