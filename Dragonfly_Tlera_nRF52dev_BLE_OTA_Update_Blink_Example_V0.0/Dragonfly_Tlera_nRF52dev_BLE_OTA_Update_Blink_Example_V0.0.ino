/*
 BLE/UART Bridge and OTA FW update example by: Greg Tomasch
 date: January 30, 2018
 license: Beerware - Use this code however you'd like. If you 
 find it useful you can buy me a beer some time.
 
 Demonstrate basic infrastructure to support BLE OTA firmware updates 
 using the Tlera nRF52 add-on board and a Central Role NUS UART BLE device
 */
 
#include "Host_WirelessSerial.h"                                                                           // Serial protocol to support the BLE/UART bridge
#include "BLE_OTA.h"                                                                                       // OTA FW update utility functon(s)

#define myLed1          25                                                                                 // Red LED
#define myLed2          13                                                                                 // Blue LED
#define myLed3          26                                                                                 // Green LED
#define DRAGONFLY_3V3   39
#define DRAGONFLY_GND   40


/*****************************************************/
/***************** Global Variables ******************/
/*****************************************************/

float         data_update[UPDATE_SIZE];                                                                    // Serial data update array for general use
int           blueLed           = myLed3;                                                                  // Blue LED indicator; shows activity when the Dragonfly is configuring the nRF52
uint32_t      OTA_Update_Size   = 0;                                                                       // FW image size in bytes rounded up to the nearest integral multiple of the 2048-byte page size
uint16_t      OTA_Update_Blocks = 0;                                                                       // Integral number of DATA_BLOCK_SIZE-byte blocks containing the new FW image
uint16_t      block;                                                                                       // Current DATA_BLOCK_SIZE-byte block of the new FW image being transferred
uint8_t       OTA_bytes[DATA_BLOCK_SIZE+2];                                                                // Array holding the DATA_BLOCK_SIZE-byte FW block data payload and 2-byte block number

void setup()
{
  delay(5000);                                                                                             // Delay a few seconds before powering up the nRF52 add-on board to eusure disconnect from the Central
  pinMode(DRAGONFLY_GND, OUTPUT);
  digitalWrite(DRAGONFLY_GND, LOW);
  pinMode(DRAGONFLY_3V3, OUTPUT);
  digitalWrite(DRAGONFLY_3V3, HIGH);
  delay(1000);
  Serial.begin(115200);                                                                                    // Open the Dragonfly USB Serial Monitor
  delay(1000);

  // Set RGB LED pinModes
  pinMode(myLed1, OUTPUT);
  digitalWrite(myLed1, HIGH);                                                                              // Start with LED's off, (active LOW)
  pinMode(myLed2, OUTPUT);
  digitalWrite(myLed2, HIGH);
  pinMode(myLed3, OUTPUT);
  digitalWrite(myLed3, HIGH);
  delay(1000);

  // Open BLE serial port
  WirelessSerial::SerialOpen(BLE_UART, 115200);                                                            // Serial1 is for the BLE link
  delay(1000);
  digitalWrite(myLed3, HIGH);                                                                              // Turn off blue LED, (active LOW)
}

void loop()
{  
  // Check for incoming OTA update request
  WirelessSerial::serialCom();                                                                             // Must be called regularly to process incoming OTA firmware update requests

  // User-specific code begins here...
  digitalWrite(myLed1, !digitalRead(myLed1));                                                              // Toggle green led on
  Serial1.println("Red LED ON");                                                                           // Push "Green LED ON" message over the BLE/UART bridge
  delay(100);                                                                                              // Wait 1 second
  digitalWrite(myLed1, !digitalRead(myLed1));                                                              // Toggle green led off
  delay(1000);
  digitalWrite(myLed2, !digitalRead(myLed2));
  Serial1.println("Blue LED ON");
  delay(100);
  digitalWrite(myLed2, !digitalRead(myLed2));
  delay(1000);
  digitalWrite(myLed3, !digitalRead(myLed3));
  Serial1.println("Green LED ON");
  delay(100);
  digitalWrite(myLed3, !digitalRead(myLed3));
  delay(1000);
}


