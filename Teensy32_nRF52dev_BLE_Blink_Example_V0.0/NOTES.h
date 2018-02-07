/*
 1/30/18 V0.0
 This example shows how to use a Peripheral role nRF52 add-on board with the Teensy3.X MCU development board to send data updates to a Nordic UART Service (NUS) Central Role device
 https://www.tindie.com/products/onehorse/nrf52-add-on-for-butterfly-and-teensy/

 1) The Pesky Products nRF52 add-on board is programmed as a Peripheral role Nordic UART Service (NUS) BLE/UART pass-through, compatible with NUS Central role devices
 2) Although there is not currently a known method for updating the Teensy3.X via UART port, the infrastructure required to do UART-based firmware updates was left intact for future development
 3) "WirelessSerial::serialCom()" is called each iteration of the loop to check for incoming OTA FW update requests (not functional for Teensy3.X)
 4) The nRF52 BLE unit is connected to the Teensy3.X UART 1 port. Data updates to the paired BLE Central device are done using "Serial1.print()" or "Serial1.write()"
 5) When notification of an OTA FW update request arrives, "OTA::OTA_Update()" is called and control is diverted to the OTA FW update handler (To be developed for Teensy3.X)
 6) The OTA FW update messages contain 32-byte payloads; this is the largest practicable size for BLE NUS UART without HW flow control
 7) Each data block message has to be ACK'd and the ACK message has to be received by the OTA FW update utility before the next data block message is sent
 8) The Teensy3.X board should NAK the transaction if the FW image exceeds available flash space or if a data black is skipped; all NAK conditions should result
    in resumption of normal steady-state operaton of the board
*/
