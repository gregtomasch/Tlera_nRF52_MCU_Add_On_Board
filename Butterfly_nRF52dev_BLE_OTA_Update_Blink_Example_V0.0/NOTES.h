/*
 1/30/17 V0.0
 This example shows how to use a Peripheral role nRF52 add-on board with the Butterfly MCU development board to send data updates to a Nordic UART Service (NUS) Central Role device
 https://www.tindie.com/products/onehorse/nrf52-add-on-for-butterfly-and-teensy/
 https://www.tindie.com/products/TleraCorp/butterfly-stm32l433-development-board/

 1) The Pesky Products nRF52 add-on board is programmed as a Peripheral role Nordic UART Service (NUS) BLE/UART pass-through, compatible with NUS Central role devices
 2) "WirelessSerial::serialCom()" is called each iteration of the loop to check for incoming OTA FW update requests
 3) The nRF52 BLE add-on board is connected to the Butterfly UART 1 port. Data updates to the paired BLE Central device are done using "Serial1.print()" or "Serial1.write()"
 4) When notification of an OTA FW update request arrives, "OTA::OTA_Update()" is called and control is diverted to the OTA FW update handler
 5) The OTA FW update messages contain 32-byte payloads; this is the largest practicable size for BLE NUS UART without HW flow control
 6) Each data block message has to be ACK'd and the ACK message has to be received by the OTA FW update utility before the next data block message is sent
 7) The Butterfly board will NAK the transaction if the FW image exceeds available flash space or if a data black is skipped; all NAK conditions result
    in resumption of normal steady-state operaton of the Butterfly board
*/
