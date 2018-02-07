#ifndef BLEserial_h
#define BLEserial_h

#include "Config.h"

#define TX_BUFFER_SIZE                 512                                                            // 512 is a good size for general use
#define INBUF_SIZE                     512
#define RX_ACK_NAK_BYTE                14                                                             // Command 14, receive command to handle ACK/NAK byte from the STM32 target MCU

static enum _BLEserial_state
                                      {
                                        IDLE,
                                        HEADER_START,
                                        HEADER_M,
                                        HEADER_ARROW,
                                        HEADER_SIZE,
                                        HEADER_CMD,
                                      } c_state[UART_NUMBER];
static volatile uint8_t                 serialHeadTX[UART_NUMBER],serialTailTX[UART_NUMBER];
static uint8_t                          serialBufferTX[TX_BUFFER_SIZE][UART_NUMBER];
static uint8_t                          inBuf[INBUF_SIZE][UART_NUMBER];
static uint8_t                          checksum[UART_NUMBER];
static uint8_t                          indRX[UART_NUMBER];
static uint8_t                          cmdBLEserial[UART_NUMBER];
static uint8_t                          CURRENTPORT = 0;
static uint8_t                          offset[UART_NUMBER];
static uint8_t                          dataSize[UART_NUMBER];

extern uint8_t                          OTA_bytes[258];
extern uint8_t                          UartRxBuffer[1024];
extern uint32_t                         OTA_Update_Size;
extern uint16_t                         OTA_Update_Blocks;
extern uint16_t                         block;
extern uint8_t                          ack_nak_byte;

class BLEserial
{
  public:
                                       BLEserial();
	   static void                       serialCom();
	   static bool                       SerialTXfree(uint8_t port);
	   static void                       SerialOpen(uint8_t port, uint32_t baud);
	   static void                       SerialEnd(uint8_t port);
	   static uint8_t                    SerialRead(uint8_t port);
	   static uint8_t                    SerialAvailable(uint8_t port);
	   static void                       SerialWrite(uint8_t port,uint8_t c);
     static void                       Initiate_OTA_Update();
     static void                       Push_OTA_Data();
  private:
     static float                      readfloat();
     static uint32_t                   read32();
     static uint16_t                   read16();
     static uint8_t                    read8();
     static void                       headSerialRequest(uint8_t port, uint8_t err, uint8_t s);
     static void                       headSerialResponse(uint8_t err, uint8_t s);
     static void                       headSerialReply(uint8_t s);
     static void inline                headSerialError(uint8_t s);
     static void                       evaluateCommand();
     static void                       serializefloat(float a);
	   static void                       serialize32(uint32_t a);
	   static void                       serialize16(int16_t a);
	   static void                       serialize8(uint8_t a);
     static void                       serializeNames(PGM_P s);
	   static unsigned char              T_USB_Available(uint8_t port);
	   static unsigned char              T_USB_Write(uint8_t port, uint8_t uc_data);
	   static void                       UartSendData(uint8_t port);
	   static void                       SerialSerialize(uint8_t port,uint8_t a);
     static void                       tailSerialRequest(uint8_t port);
     static void                       tailSerialReply();
};

#endif // BLEserial_h
