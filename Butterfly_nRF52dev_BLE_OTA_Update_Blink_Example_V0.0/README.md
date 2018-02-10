## Tlera Butterfly Remote MCU Test Sketch

This is an Arduino test sketch for use with a *remote* Tlera Butterfly MCU development board equipped with a Peripheral role Tlera nRF52832 add-on board. It will sequentially blink the red, green and blue LED's and send serial a message regarding the active LED color over UART serial to the nRF52832. When paired with a Central role BLE NUS device, a continuous update of the active LED color will be displayed.
