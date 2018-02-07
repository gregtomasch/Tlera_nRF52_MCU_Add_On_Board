#include "Arduino.h"
#include "BLE_OTA.h"

OTA::OTA()
{
}

void OTA::OTA_Update(uint32_t update_size, uint16_t update_blocks)
{
  block = -1;
  uint16_t cycles = 0;
  uint32_t imageStart = FLASHEND - ((update_size + 2047) & ~2047);
  uint32_t ACK_Start;

  // Wait 2s beforte starting transfer, for any async processes to complete......
  delay(2000);

  // OTA sub-loop; blocking function so that there are no other data transactions on the WiFi/UART bridge...
  if(abs(FLASHEND - FLASHSTART) < ((update_size/2048)*2048 + 2048))
  {
    // Not enough flash memory for the new sketch; NAK the tranaction
    WirelessSerial::OTA_Update_ACK_NAK(0);
    delay(50);
    return;
  } else
  {
    // Enough flash memory; erase the block for the new sketch and ACK the transaction
    STM32.flashErase(imageStart, ((update_size + 2047) & ~2047));
    WirelessSerial::OTA_Update_ACK_NAK(4);
    delay(200);
    WirelessSerial::serialCom();                                                                                     // Handle the serial Rx to see if the first data block message has come in
    ACK_Start = millis();
    while(1)                                                                                                         // Data block "0" should be coming in
    {
      if(block == 0) { break; }                                                                                      // Data block "0" confirmed; bail
      if((millis() - ACK_Start) > 10000)
      {
        // It's been too long... Re-ACK the transaction to re-end the block
        WirelessSerial::OTA_Update_ACK_NAK(4);
        ACK_Start = millis();
        cycles++;
      }
      if(cycles > 2)
      {
        // Something's wrong, NAK out of this...
        WirelessSerial::OTA_Update_ACK_NAK(0);
        delay(50);
        return;
      }
      delay(200);
      WirelessSerial::serialCom();                                                                                   // Handle the serial Rx to see if the first data block message has come in
    }
  }
 cycles = 0;
  for(uint16_t i=0; i<update_blocks; i++)
  {
    // Keep checking serial bridge for the next block
    while(1)
    {
      if(block > i)
      {
        // If the block number is greater than the loop index, you missed a block; NAK the transaction
        WirelessSerial::OTA_Update_ACK_NAK(0);
        return;
      }
      if(block == i)                                                                                                 // Block number confirmed; bail
      {
        cycles = 0;
        break;
      }
      cycles++;
      if(cycles > 10)
      {
        // It's been too long... Tell the host to re-send this block
        WirelessSerial::OTA_Update_ACK_NAK(3);
        cycles = 0;
      }
      delay(50);
      WirelessSerial::serialCom();                                                                                   // Handle the serial Rx to see if the next data block message has come in
    }
    if(block == i)
    {
      // Looking good; write the block to flash, ACK the transaction
      STM32.flashProgram((imageStart + DATA_BLOCK_SIZE*i), OTA_bytes, DATA_BLOCK_SIZE);
      WirelessSerial::OTA_Update_ACK_NAK(1);
    } else
    {
      // The block count is off; NAK the transaction
      WirelessSerial::OTA_Update_ACK_NAK(0);
      return;
    }
  }

  // You've written the full number of blocks, reset to activate the new FW image!
  delay(2000);
  STM32.reset();
}

