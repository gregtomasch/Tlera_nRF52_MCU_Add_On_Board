/*
 BLE/UART Bridge and OTA FW update example for Teensy3.X by: Greg Tomasch
 date: January 30, 2018
 license: Beerware - Use this code however you'd like. If you 
 find it useful you can buy me a beer some time.
 
 Demonstrate basic infrastructure to support BLE data updates 
 from a Teensy3.X to a Central Role NUS UART BLE device
 */
 
#include "Host_WirelessSerial.h"                                                                           // Serial protocol to support the BLE/UART bridge

#define myLed1 13                                                                                          // Indicator LED

/*****************************************************/
/***************** Global Variables ******************/
/*****************************************************/

float         data_update[UPDATE_SIZE];                                                                    // Serial data update array for general use

void setup()
{
  Serial.begin(115200);                                                                                    // Open the Butterfly USB Serial Monitor
  delay(1000);

  // Set LED pinMode
  pinMode(myLed1, OUTPUT);
  digitalWrite(myLed1, LOW);                                                                               // Start with LED off
  delay(1000);
  
  // ***************** Initiate the BLE/UART bridge ******************
  NRF52_UART.begin(115200);                                                                                // Serial1 is for the BLE link
  delay(100);
  digitalWrite(myLed1, LOW);                                                                               // Turn off LED
}

void loop()
{  
  // *************** Check for incoming data requests ****************
  WirelessSerial::serialCom();                                                                             // Must be called regularly to process incoming serial requests

  // User-specific code begins here...
  digitalWrite(myLed1, !digitalRead(myLed1));                                                              // Toggle LED on
  NRF52_UART.println("LED ON");                                                                            // Push "LED ON" message over the BLE/UART bridge
  delay(100);                                                                                              // Wait 0.1 second
  digitalWrite(myLed1, !digitalRead(myLed1));                                                              // Toggle LED off
  NRF52_UART.println("LED OFF");
  delay(1000);
}


