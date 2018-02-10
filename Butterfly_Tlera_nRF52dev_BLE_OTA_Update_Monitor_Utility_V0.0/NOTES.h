/*
 1/30/18 V0.0
 This sketch is used on a Butterfly STM32L4 MCU development board and in conjunction with an nRF52 add-on board to serve as a BLE/UART bridge for Peripheral role Nordic UART Service (NUS) devices
 https://www.tindie.com/products/TleraCorp/butterfly-stm32l433-development-board/ - Butterfly STM32L4 development board
 https://github.com/GrumpyOldPizza/arduino-STM32L4 - STM32L4 Arduino core
 https://www.tindie.com/products/onehorse/nrf52-add-on-for-butterfly-and-teensy/ - nRF52 BLE add-on board
 
 The add-on board combined with the MCU board serves as a Central Role BLE data monitor and OTA FW update utility:
 
 1) The Pesky Products nRF52 add-on board is programmed as a Central role Nordic UART Service (NUS) BLE/UART pass-through, compatible any other NUS Peripheral role device
 2) If it is desired to use the Butterfly + Central role nRF52We add-on for OTA FW updates, the Butterfly needs to be equipped with an SPI Micro SD card reader:
    https://www.tindie.com/products/TleraCorp/micro-sd-card-reader-for-butterfly/. You can then use the Windows drag-and-drop file manager features for easy introduction of the FW image ".iap" file
 3) The Butterfly's file manager does not support file name wildcarding; the FW image needs to be named "new_sketch.iap"
 4) The transfer is designed to be robust to common problems:
    a) When the nRF52 central pairs with the target nRF52 Butterfly pin 2 is pulled low indicating the BLE/UART bridge is ready. In not, data will not transfer until it is
    b) If the FW image exceeds available flash space on the target STM32L4, the transaction is NAK'd
    c) The serial protocol is fairly robust to errors; preamble, command, data payload and XOR checksum must all be valid for the data block to be accepted as valid
    d) If the transaction stalls, the target will request re-send of the current block
    e) If the block count is off, the transaction will be NAK'd
 5) NAK'ing of an update will not corrupt the current FW image on the target and will return both the target STM32L4 and this utility back to steady-state operation
 6) If the OTA update is NAK'd this utility will return to the main loop and the update can be tried again...
 7) When not performing the OTA FW update function, any UART dat areceived from the peripheral role device is passed through to the Butterfly's USB serial port...
 8) nRF52 pin assignments/parameters, LED pin assignments and UART bridge parameters are defined in "Config.h"
*/
