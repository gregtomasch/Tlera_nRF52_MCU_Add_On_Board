## Tlera Dragonfly Gateway MCU Test Sketch

This is an Arduino test sketch for use with a *gateway* Tlera Dragonfly MCU development board equipped with a Central role Tlera nRF52832 add-on board. Upload the sketch to the gateway Dragonfly MCU and open a USB serial monitor. If there is a BLE NUS Peripheral role device in range it will pair. You will see messaging about the BLE NUS connection status on the USB serial monitor during the pairing process. Once successfully paired, Any serial data sent from the the *remote* device will be displayed on the USB serial monitor.

If the *remote* device uses an STM32L4 MCU and is running firmware that supports OTA firmware updates, this sketch will also manage these updates over the BLE NUS link. Simply copy the new firmware image to the Dragonfly's virtual flash drive and send a "1" from the PC's USB serial monitor. Detailed instructions are in "STM32L4_BLE_Bridge_and_OTA_Wiki.pdf" loacted in the main folder of this repository.
