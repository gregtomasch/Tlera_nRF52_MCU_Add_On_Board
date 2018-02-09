# Bring BLE Connectivity to Your Favorite MCU Development Board With Tlera nRF52832 Add-on Boards

## Background
Microcontroller (MCU) development boards are widely used for many Internet of Things (IoT) applications. They are compact, powerful and highly versatile with a wide range of I/O capabilities. The PJRC Teensy3.X family of products are highly capable and very popular. These were some of the first MCU development boards that brought transformative performance to the IoT Maker community in small form factor and at a reasonable price. Tlera Corporation offers a [family of MCU development boards](https://www.tindie.com/stores/TleraCorp/) that were initially inspired by the Teensy3.2 but are based upon [ST Microelectronics' STM32L4 microcontroller.](http://www.st.com/content/ccc/resource/technical/document/reference_manual/02/35/09/0c/4f/f7/40/03/DM00083560.pdf/files/DM00083560.pdf/jcr:content/translations/en.DM00083560.pdf)![alt text](https://user-images.githubusercontent.com/5760946/35936194-677fc7dc-0bf7-11e8-8aba-164e4539fc1b.png)![alt text](https://user-images.githubusercontent.com/5760946/35936276-9dd7b4b6-0bf7-11e8-9f23-42b35ff0d7db.png)The STM32L4 is an 80MHz ARM Cortex M4 microcontroller with an embedded floating point unit and a very rich I/O portfolio. Like the Teensy3.X family, Tlera's MCU development boards are supported by a [mature Arduino core](https://github.com/GrumpyOldPizza/arduino-STM32L4) for easy application development. The differentiating advantage of the Tlera STM32L4-based MCU boards is low power sleep capability. State-preserving sleep modes can be easily implemented using the Arduino IDE and typically result in ~1.6uA current consumption. The STM32L4 can be woken up by either an external interrupt or periodically by using the internal RTC.

The combination of ample numerical processing speed, rich I/O capability, very low power sleep states and Arduino IDE support make the STM32L4 an excellent choice for developing battery-powered remote sensing devices. **However, one critical limitation has persisted: No integrated wireless connectivity.** In order to address this need, Tlera Corp. now offers an nRF5282 BLE board to bring wireless connectivity to your favorite MCU boards:![alt text](https://user-images.githubusercontent.com/5760946/35936401-033b6c76-0bf8-11e8-8e20-05b05b8464fb.png) This add-on product was specifically designed to plug directly into the Tlera ["Dragonfly"](https://www.tindie.com/products/TleraCorp/dragonfly-stm32l47696-development-board/) and ["Butterfly"](https://www.tindie.com/products/TleraCorp/butterfly-stm32l433-development-board/) MCU development boards and is also pinout compatible with the Teensy3.X family of products. The nRF5282 add-on board shares 3.3V power and ground connections with the MCU board. The UART serial Tx and Rx lines are connected in null modem configuration. An indicator pin is also available to connect with a digital input on the MCU board. When the nRF5282 pairs with another suitable BLE device, the "IND" node of the add-on board is pulled low. This feature is helpful to notify the MCU that there is a BLE connection before attempting to send data to the paired device.

Physical mounting of the add-on board to the MCU development board is straightforward. Dragonfly/Butterfly:![alt text](https://user-images.githubusercontent.com/5760946/35993753-2254dba8-0cc3-11e8-9c2f-3617f0f20ac7.JPG) Teensy 3.1/3.2:![alt text](https://user-images.githubusercontent.com/5760946/36031384-71d8afc8-0d5f-11e8-8e29-d08cf735cbd2.JPG) Teensy 3.5/3.6:![alt text](https://user-images.githubusercontent.com/5760946/36032009-73147578-0d61-11e8-855f-a6f0dac4e965.JPG)Machine pin headers are convenient to mate the add-on board with the MCU development board. In all cases (Dragonfly, Butterfly, Teensy 3.1/3.2 and Teensy 3.5/3.6) the location of the add-on board with respect to the MCU's USB connector (on the right) is identical. The BLE antenna consists of simple 1.2" wire segment

## How the Connectivity Works and Use Case Options
### Streaming Data From Remote MCU Devices
The add-on boards can be flashed to act as either a Peripheral or Central role BLE [Nordic UART Service](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk52.v0.9.0%2Fgroup__ble__sdk__srv__nus.html) (NUS) device. The NUS characteristic turns out to be extremely useful for enabling BLE wireless connectivity; it acts as a simple BLE-UART bridge. If the nRF52832 receives data from a paired BLE NUS device, it simply relays that data to the MCU over a UART port. Conversely, data written from the MCU to the nRF52832 over the UART port is sent immediately to the paired BLE NUS device. So one could think of the NUS as a virtual "BLE Serial extension cable" connecting two UART ports... 

There are a number of [NUS BLE Central role applications](https://learn.adafruit.com/bluefruit-le-connect-for-ios/ios-setup) for smart devices such as cell phones and tablet computers. It is simple to use the nRF5282 add-on in Peripheral role to stream serial data from an MCU to a Central role smart device. However, it has proven to be much harder to establish BLE connectivity between a remote MCU device and a PC. This problem can be easily solved by using a second MCU development board and a Central role nRF5282 add-on:![alt text](https://user-images.githubusercontent.com/5760946/36044991-2dbfa902-0d89-11e8-9547-ddb436c5b8a5.png)

   * A serial bridge is made between the *remote* MCU device and a *Peripheral role* nRF5282 add-on
   * A second serial bridge is made between a *second* MCU device and a *Central role* nRF5282 add-on. We'll call this second MCU the
     *gateway*
   * The Central and Peripheral role add-ons pair and can exchange data between the *gateway* and *remote* MCU's
   * Internal to the *gateway MCU's* programming, data exchanged to/from the BLE/UART bridge is passed from/to to gateway MCU's USB serial port and is available to any kind of serial terminal application on the PC

### Wireless Firmware Updates to Remote MCU Devices
Having a BLE serial link between a remote MCU device and a PC opens up another exciting possibility: Over-The-Air (OTA) firmware updates for the remote MCU device. The BLE serial connection described above is bi-directional so there is no reason in principle why new firmware information can't be sent to a remote MCU. The Tlera MCU development boards support byte-by-byte writing of a new firmware image from an Arduino sketch directly to the STM32L4's native flash memory. Once the new firmware image is completely written, a soft reset activates it. Using a Tlera Dragonfly board as the gateway MCU makes this process even easier. It is equipped with a 128Mbit QSPI flash chip that is mountable as a virtual disk drive. New MCU firmware can be built with the Arduino IDE, drag-and-drop copied to the Dragonfly's QSPI flash memory and transmitted to the remote MCU over the BLE NUS connection. 

The example Arduino sketches in this repository include the necessary infrastructure to perform OTA firmware updates. Data is read from the Firmware image file on the gateway MCU and transferred over the BLE NUS link to the remote device in 32byte blocks. A variation of the [MultiWii Serial Protocol (MSP)](http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol) is used to packetize and decode the firmware image data. MSP has reasonably good packet integrity to protect against data corruption. Block-to-block handshaking, byte counting and limited error recovery have been implemented to ensure that the complete image is received. Transfer times of 2-10min for a complete firmware image are typical.

OTA firmware update is fully functional for STL32L4-based devices but not for Teensy3.X products. At the time of writing this wiki, there is no known method for transferring firmware image data from an Arduino sketch to the bootable portion of the Teensy MCU's flash memory. If this capability were to be developed OTA firmware updates would be possible for the Teensy3.X products as well.

## Step-by-Step Guide
### Set up Your Remote MCU
  * Select your MCU board (Dragonfly, Butterfly or Teensy3.X
  * Get a Tlera nRF5282 add-on board flashed for *Peripheral role*. (The add-on boards can be purchased with either Peripheral or Central role firmware)
  * Connect the add-on board to the MCU board as ahown in the photos above
  * Make sure that you have the proper version of Arduino and the correct core/libraries installed for either the [Tlera STM32L4](https://github.com/GrumpyOldPizza/arduino-STM32L4) or [Teensy3.X](https://www.pjrc.com/teensy/td_download.html)MCU board you have selected
  * 

## Software Overview
It is recommended to read the "STM32L4_BLE_Bridge_and_OTA_Wiki.pdf" document in this repository for instructions on how to use the Tlera nRF52832 add-on boards and software examples. Practical considerations of using both the Arduino MCU sketches and nRF52832 firmware are discussed there. This section will focus more on using the example sketches as an infrastructure to develop new BLE-connected Arduino applications and how to use the nRF52832 firmware source code in the Nordic SDK to make user-specific modifications.

### Arduino Sketches
This repository contains example Arduino sketches for using the Tlera nRF52832 add-on boards on both "remote" and "gateway" MCU development boards. The sketches support "Dragonfly", "Butterfly" and Teensy3.X MCU products. The names should make it obvious which sketches support which boards. All sketches have been successfully tested with their respective products. The Teensy3.X sketches have been tested with the Teensy 3.2 and 3.6 boards. All sketches containing "Blink_Example" in their name are intended to run on the remote MCU. All sketches with "Monitor_Utility" in their name are intended to run on the host MCU.

The most basic form of remote MCU sketches all work on the same principle: Open the UART port connected to the nRF52832 add-on board and read/write/print operations addressed to that port will be handled over the BLE/NUS connection. This is all that is required for the remote MCU device to send results to a tablet computer, smart phone or PC. It should be noted that BLE NUS data rates are fairly limited. As a practical matter, more than about 1kb/s may result in data loss. If OTA firmware update capability is desired, it is necessary to include a serial protocol and an OTA update handler. These are implemnted in "Host_WirelessSerial.cpp" and "BLE_OTA.cpp" respectively. The serial protocol is included in all examples but the OTA update handler is only in the STM42L4-related examples. The "WirelessSerial::serialCom()" MSP handler is called every cycle of the main loop to process incoming MSP messages. If a valid MSP message to initiate OTA firmware update is received, the "OTA::OTA_Update(OTA_Update_Size, OTA_Update_Blocks)" sub-loop handler is called to complete the update.

Gateway MCU sketches are equally simple in concept: Check the UART port connected to the nRF52832 add-on board for incoming data each cycle of the main loop. As data is available, read each incoming byte and write it to the MCU's USB serial port. Conversely, the USB serial port can also be checked for incoming data which is then written to the BLE UART port. The STM32L4-related examples monitor the USB serial port for a specific incoming byte in order to call "flashFW()", the OTA firmware update handler. The Teensy3.X-related examples do not monitor data incoming on the USB serial port but it would be a simple matter to do so if desired.

### nRF52832 Firmware
Both Central and Peripheral role firmware for the nRF52832 were developed using v12.2.0 of the [Nordic nRF5 SDK.](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk52.v0.9.2%2Findex.html)
