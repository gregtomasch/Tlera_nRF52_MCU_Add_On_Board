## PJRC Teensy3.X Remote MCU Test Sketch

This is an Arduino test sketch for use with a *remote* Teensy3.X MCU development board equipped with a Peripheral role Tlera nRF52832 add-on board. It will sequentially blink the main LED at ~1Hz and send serial a message regarding the LED logic state over UART serial to the nRF52832. When paired with a Central role BLE NUS device, a continouos update of the LED logic state will be displayed.
