# Bring BLE Connectivity to Your Favorite MCU Development Board With Tlera nRF52 Add-on Boards

## Background
Microcontroller (MCU) development boards are widely used for many Internet of Things (IoT) applications. They are compact, powerful and highly versatile with a wide range of I/O capabilities. The PJRC Teensy3.X family of products are highly capable and very popular. These were some of the first MCU development boards that brought transformative performance to the IoT Maker community in small form factor and at a reasonable price. Tlera Corporation offers a [family of MCU development boards](https://www.tindie.com/stores/TleraCorp/) that were initially inspired by the Teensy3.2 but are based upon [ST Microelectronics' STM32L4 microcontroller.](http://www.st.com/content/ccc/resource/technical/document/reference_manual/02/35/09/0c/4f/f7/40/03/DM00083560.pdf/files/DM00083560.pdf/jcr:content/translations/en.DM00083560.pdf)![alt text](https://user-images.githubusercontent.com/5760946/35936194-677fc7dc-0bf7-11e8-8aba-164e4539fc1b.png)![alt text](https://user-images.githubusercontent.com/5760946/35936276-9dd7b4b6-0bf7-11e8-9f23-42b35ff0d7db.png)The STM32L4 is an 80MHz ARM Cortex M4 microcontroller with an embedded floating point unit and a very rich I/O portfolio. Like the Teensy3.X family, Tlera's MCU development boards are supported by a [mature Arduino core](https://github.com/GrumpyOldPizza/arduino-STM32L4) for easy application development. The differentiating advantage of the Tlera STM32L4-based MCU boards is low power sleep capability. State-preserving sleep modes can be easily implemented using the Arduino IDE and typically result in ~1.6uA current consumption. The STM32L4 can be woken up by either an external interrupt or periodically by using the internal RTC.

The combination of ample numerical processing speed, rich I/O capability, very low power sleep states and Arduino IDE support make the STM32L4 an excellent choice for developing battery-powered remote sensing devices. However, one critical limitation has persisted: **No integrated wireless connectivity.** In order to address this need, Tlera Corp. now offers an nRF5282 BLE board to bring wireless connectivity to your favorite MCU boards:![alt text](https://user-images.githubusercontent.com/5760946/35936401-033b6c76-0bf8-11e8-8e20-05b05b8464fb.png) This add-on product was specifically designed to plug directly into the Tlera ["Dragonfly"](https://www.tindie.com/products/TleraCorp/dragonfly-stm32l47696-development-board/) and ["Butterfly"](https://www.tindie.com/products/TleraCorp/butterfly-stm32l433-development-board/) MCU development boards and is also pinout compatible with the Teensy3.X family of products.
