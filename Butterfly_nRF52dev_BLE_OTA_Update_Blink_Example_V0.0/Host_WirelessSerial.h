#ifndef WirelessSerial_h
#define WirelessSerial_h

#include "BLE_OTA.h"

// Configuration definitions
#define UART_NUMBER                    4                                                              // Total number of possible serial ports on the STM32
#define TX_BUFFER_SIZE                 384                                                            // Large enough to hold a whole OTA packet with plenty of margin
#define INBUF_SIZE                     384                                                            // Large enough to hold a whole OTA payload with plenty of margin
#define DATA_BLOCK_SIZE                32                                                             // ".iap" file data block size; 32 bytes works well for the NUS UART pass-through without HW flow control
#define RECEIVE_SERIAL_DATA            10                                                             // Command 10, receive serial data from the nRF52
#define INITIATE_OTA_UPDATE            12                                                             // Command 12, initiate OTA FW update
#define RECEIVE_OTA_DATA               13                                                             // Command 13, receive a numberd block of OTA data
#define SEND_SERIAL_DATA               20                                                             // Command 20, send serial data to the nRF52
#define UPDATE_SIZE                    20                                                             // Size (in float data points) of the data update message pushed to the ESP82XX WiFi bridge (if used)
#define BUTTERFLY_BLE_UART             1                                                              // Butterfly UART port connected to the nRF52 to constitute the UART/BLE bridge

static enum _WirelessSerial_state
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
static uint8_t                          UartRxBuffer[256];                                            // Augmented UART Rx buffer; necessary for large payload (>64 byte) smessages...
static uint8_t                          checksum[UART_NUMBER];
static uint8_t                          indRX[UART_NUMBER];
static uint8_t                          cmdWirelessSerial[UART_NUMBER];
static uint8_t                          CURRENTPORT = 0;
static uint8_t                          offset[UART_NUMBER];
static uint8_t                          dataSize[UART_NUMBER];


extern float                            data_update[UPDATE_SIZE];                                     // 1D array for data points to be sent over the UART bridge. DATA TREATED AS SP FLOATS
extern uint8_t                          WiFi_bridge_ready;                                            // Status variable to let Butterfly know the WiFi bridge is awake and ready to transmit data
extern uint8_t                          OTA_bytes[DATA_BLOCK_SIZE+2];
extern uint32_t                         OTA_Update_Size;
extern uint16_t                         OTA_Update_Blocks;
extern uint16_t                         block;

class WirelessSerial
{
  public:
                                       WirelessSerial();
	   static void                       serialCom();
	   static bool                       SerialTXfree(uint8_t port);
	   static void                       SerialOpen(uint8_t port, uint32_t baud);
	   static void                       SerialEnd(uint8_t port);
	   static uint8_t                    SerialRead(uint8_t port);
	   static uint8_t                    SerialAvailable(uint8_t port);
	   static void                       SerialWrite(uint8_t port,uint8_t c);
     static void                       SerialSendMWPcmd(uint8_t port, uint8_t err,uint8_t cmd);
     static void                       Push_Data_Update();
     static void                       OTA_Update_ACK_NAK(uint8_t ack_nak_byte);
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

#endif // WirelessSerial_h
