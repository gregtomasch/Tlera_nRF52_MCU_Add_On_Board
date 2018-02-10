## PJRC Teensy3.X Gateway MCU Test Sketch

This is an Arduino test sketch for use with a *gateway* Teensy3.X MCU development board equipped with a Central role Tlera nRF52832 add-on board. Upload the sketch to the gateway Teensy MCU and open a USB serial monitor. If there is a BLE NUS Peripheral role device in range it will pair. You will see messaging about the BLE NUS connection status on the USB serial monitor during the pairing process. Once successfully paired, Any serial data sent from the the *remote* device will be displayed on the USB serial monitor.

The Teensy3.X family of products does not currently support direct writing of data to the bootable portion of the MCU's flash memory. If this capability were to be developed, the Teensy3.X family of products could have their firmware updated over-the-air as well. This sketch has the serial protocol included to facilitate future Teensy3.X OTA update development.
