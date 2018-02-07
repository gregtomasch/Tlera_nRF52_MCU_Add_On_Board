/* Draonfly BLE data monitor and OTA update utility example code
   by: Greg Tomasch
   date: January 30, 2018
   license: Beerware - Use this code however you'd like. If you 
   find it useful you can buy me a beer some time.
*/

#include <FS.h>
#include "OTA_Host_BLEserial.h"
#include "Config.h"

/*****************************************************/
/***************** Global Variables ******************/
/*****************************************************/
uint8_t                          buffer[256];                                                                       // Intermediate byte array to hold OTA_DATA_BLOCK_SIZE-bytes of the FW image read from the FW image ".iap" file
uint8_t                          numbytes;                                                                          // Actual number of bytes read into "buffer[]" byte array during file read
uint8_t                          OTA_bytes[258];                                                                    // Array for holding the uint16_t (2-byte) block number and the data payload up to 256-bytes
uint8_t                          UartRxBuffer[1024];                                                                // Augmented UART Rx buffer; necessary for large payload messages...
uint32_t                         OTA_Update_Size;                                                                   // FW image size in bytes rounded up to the nearest integral multiple of the 2048-byte page size
uint16_t                         OTA_Update_Blocks;                                                                 // Integral number of OTA_DATA_BLOCK_SIZE-byte blocks containing the new FW image
uint16_t                         block;                                                                             // Current OTA_DATA_BLOCK_SIZE-byte FW image data block being handled
uint8_t                          ack_nak_byte;                                                                      // Flow control byte; Prnding = 2, OTA update request ACK = 4, data block ACK = 1, data block resend = 3, NAK = 0
uint8_t                          serial_byte;
File                             file;

/*****************************************************/
/************** Function Declarations ****************/
/*****************************************************/
void flashFW();

void setup()
{
  pinMode(DRAGONFLY_GND, OUTPUT);
  digitalWrite(DRAGONFLY_GND, LOW);
  pinMode(DRAGONFLY_3V3, OUTPUT);
  digitalWrite(DRAGONFLY_3V3, HIGH);
  delay(1000);
  BLEserial::SerialOpen(STM32L4_BLE_UART, 115200);                                                                  // Initialize the BLE UART port
  delay(1000);

  // Set up LED and BLE connection status pins
  pinMode(BLE_CONNECT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.begin(115200);                                                                                             // Open the USB serial port
  delay(100);
  Serial.println("Send a '1' to initiate OTA FW update");
}

void loop()
{
  while(NRF52_UART.available())
  {
    serial_byte = NRF52_UART.read();
    Serial.write(serial_byte);
  }
  if(Serial.read() == 49)                                                                                           // ASCII "1"
  {
    flashFW();
  }
}

void flashFW()
{
  uint8_t  cancel_xfer = 0;
  uint16_t numbytes;
  uint32_t totnum = 0;
  union
  {
    uint16_t short_unsigned;
    uint8_t  short_unsigned_byte[2];
  };

  DOSFS.begin();
  file = DOSFS.open("/new_sketch.iap", "r");                                                                        // DOSFS does not support file name wildcarding; FW file needs to be named "new_sketch.iap"
  if(file)
  {
    Serial.println("File Open!");
  } else
  {
    Serial.println("File Open Failed!");                                                                            // "new_sketch.iap" does not exist; close DOSFS and return to the main loop...
    DOSFS.end();
    delay(1000);
    Serial.println("Send a '1' to initiate OTA FW update");
    return;
  }
  
  // Determine the firmware image file size and number of blocks
  OTA_Update_Size = file.size();
  OTA_Update_Blocks = (OTA_Update_Size/OTA_DATA_BLOCK_SIZE) + 1;
  Serial.print("FW image size: "); Serial.print(OTA_Update_Size); Serial.println(" bytes");
  Serial.print("FW transfer blocks: "); Serial.println(OTA_Update_Blocks);
  Serial.println("Checking bridge status...");

  // Check the BLE/UART bridge status (Inverted logic). Comment out this "while loop" if no BLE connection status pin is used
  while(1)
  {
    if(!digitalRead(BLE_CONNECT_PIN))
    {
      Serial.println("Bridge ready...");
      break;
    }
    delay(50);
  }
  
  ack_nak_byte = 2;
  Serial.println("Sending OTA update request...");
  BLEserial::Initiate_OTA_Update();
  delay(50);
  
  // Wait for the remote board to respond; ACK/NAK byte set to 2 (Neither ACK nor NAK)
  BLEserial::serialCom();
  while(1)
  {
    if(ack_nak_byte == 4)
    {
      Serial.println("Transfer request and FW image size ACK'd");
      digitalWrite(LED_PIN, HIGH);
      break;
    }
    if(ack_nak_byte == 0)
    {
      cancel_xfer = 1;
      break;
    }
    BLEserial::serialCom();
    delay(50);
  }

  if (cancel_xfer)
  {
    Serial.println("Transaction NAK'd! The FW image is too large...");
  } else
  {
    Serial.println("Writing OTA FW data to remote board...");
    for(uint16_t i=0; i<OTA_Update_Blocks; i++)
    {
      // Read the next block from the FW image file
      numbytes = file.read(buffer, OTA_DATA_BLOCK_SIZE);                                                            // OTA_DATA_BLOCK_SIZE bytes per block
      Serial.print("First two bytes: "); Serial.print("0x"); Serial.print(buffer[0], HEX); 
      Serial.print(" "); Serial.print("0x"); Serial.println(buffer[1], HEX);
      Serial.print("Block Number: "); Serial.print(i); Serial.print("/"); Serial.println(OTA_Update_Blocks);
      Serial.print("Number of bytes = "); Serial.println(numbytes);                                                 // Print number of bytes read

      // Insert block number into firtst two bytes of the OTA_bytes array
      short_unsigned = i;
      OTA_bytes[0] = short_unsigned_byte[0];
      OTA_bytes[1] = short_unsigned_byte[1];

      // Build the OTA_bytes array
      for(uint16_t j=2; j<(numbytes+2); j++)
      {
        OTA_bytes[j] = buffer[j-2];                                                                                 // Write data from file read buffer to OTA_bytes array
      }
      ack_nak_byte = 2;

      // Check the BLE/UART bridge status (Inverted logic). Comment out this "while loop" and call "BLEserial::Push_OTA_Data()" if no BLE connection status pin is used
      while(1)
      {
        if(!digitalRead(BLE_CONNECT_PIN))
        {
          Serial.println("Bridge ready, pushing data...");
          BLEserial::Push_OTA_Data();
          break;
        }
        delay(50);
      }

      // Wait for the remote IU board to respond; ACK/NAK byte set to 2 (Neither ACK, NAK or resend)
      delay(50);
      while(1)
      {
        BLEserial::serialCom();
        if(ack_nak_byte == 1)
        {
          cancel_xfer = 0;
          Serial.print("Block Number: "); Serial.print(i); Serial.print(" "); Serial.println("ACK'd");
          break;
        }
        if(ack_nak_byte == 3)
        {
          // ack_nak_byte == 3 means re-send current block
          BLEserial::Push_OTA_Data();
        }
        if(ack_nak_byte == 0)
        {
          cancel_xfer = 1;
          break;
        }
        delay(50);
      }
      if (cancel_xfer)
      {
        Serial.println("Transaction NAK'd! Block count error...");
        break;
      }
      totnum += numbytes;
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      Serial.print("bytes: "); Serial.println(totnum);
      if(i >= (OTA_Update_Blocks-1)) { break; }
    }
  }
  Serial.print("Total bytes transferred: "); Serial.println(totnum);
  totnum = 0;
  DOSFS.end();
  Serial.println("Send a '1' to initiate OTA FW update");
}
