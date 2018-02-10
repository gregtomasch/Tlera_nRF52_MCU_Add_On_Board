/* Draonfly BLE data monitor and OTA update utility example code
   by: Greg Tomasch
   date: May 25, 2017
   license: Beerware - Use this code however you'd like. If you 
   find it useful you can buy me a beer some time.
*/

#include "Host_BLEserial.h"
#include "Config.h"

/*****************************************************/
/***************** Global Variables ******************/
/*****************************************************/
uint8_t                          serial_byte;

/******************** OTA Specific ********************/
// OTA firmware update infrastructure variable; functional for STM32L4
uint8_t                          buffer[256];                                                                       // Intermediate byte array to hold OTA_DATA_BLOCK_SIZE-bytes of the FW image read from the FW image ".iap" file
uint8_t                          numbytes;                                                                          // Actual number of bytes read into "buffer[]" byte array during file read
uint8_t                          OTA_bytes[258];                                                                    // Array for holding the uint16_t (2-byte) block number and the data payload up to 256-bytes
uint32_t                         OTA_Update_Size;                                                                   // FW image size in bytes rounded up to the nearest integral multiple of the 2048-byte page size
uint16_t                         OTA_Update_Blocks;                                                                 // Integral number of OTA_DATA_BLOCK_SIZE-byte blocks containing the new FW image
uint16_t                         block;                                                                             // Current OTA_DATA_BLOCK_SIZE-byte FW image data block being handled
uint8_t                          ack_nak_byte;                                                                      // Flow control byte; Prnding = 2, OTA update request ACK = 4, data block ACK = 1, data block resend = 3, NAK = 0

void setup()
{
  BLEserial::SerialOpen(TEENSY_BLE_UART, 115200);                                                                   // Initialize the BLE UART port
  delay(1000);

  // Set up LED and BLE connection status pins
  pinMode(BLE_CONNECT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.begin(115200);                                                                                             // Open the USB serial port
  delay(100);
}

void loop()
{
  while(NRF52_UART.available())
  {
    serial_byte = NRF52_UART.read();
    Serial.write(serial_byte);
  }
}

